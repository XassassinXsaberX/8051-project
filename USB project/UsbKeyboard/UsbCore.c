#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"
#include <INTRINS.h>

idata unsigned char Buffer[16]; //Ū�� endpoint 0�Ϊ��w�İ�

unsigned char *sendPtr;   //���V���U�ӭn�ǰe�����
unsigned int SendLength;  //�N���٭n�e�h��byte�����
bit SendZeroPacket;       //�Y��1�N��n�edata byte�ƥج�0��data packet�A�Y��0�h�N���ΰe
bit Ep1InIsBusy;
unsigned char ConfigValue;



code unsigned char DeviceDescriptor[0x12]=  //device descriptor��18byte 
{
	//bLength�r�q�Cdevice descriptor�����׬�18(0x12)byte 
	0x12,
	
	//bDescriptorType�r�q�Cdevice descriptor���s����0x01 
	0x01,
	
	//bcdUSB�r�q�C�o�̳]�m������USB1.1�A�Y0x0110�C 
	//�ѩ�Olittle endian���c�A�ҥH�Cbyte�b�e�A�Y0x10�B0x01�C 
	0x10,
	0x01,

	//bDeviceClass�r�q�C�ڭ̤��bdevice descriptor���w�qdevice class�A
	//�Ӧbinterface descriptor���w�qdevice class�A�ҥH�Ӧr�q���Ȭ�0�C
	0x00,
	
	//bDeviceSubClass�r�q�CbDeviceClass�r�q��0�ɡA�Ӧr�q�]��0�C
	0x00,
	
	//bDeviceProtocol�r�q�CbDeviceClass�r�q��0�ɡA�Ӧr�q�]��0�C
	0x00,
	
	//bMaxPacketSize0�r�q�CPDIUSBD12��endpoint 0�j�p��16byte�C
	0x10,
	
	//idVender�r�q�CVendor ID�A�ڭ̳o�̨�0x8888�A�Ȩѹ���ΡC
	//��ڲ��~�����H�K�ϥ�Vendor ID�A������USB��|�ӽ�Vendor ID�C
	//�`�Nlittle endian�Ҧ��A�Cbyte�b�e�C
	0x88,
	0x88,
	
	//idProduct�r�q�CProduct ID�A�ѩ�O�ĤG�ӹ���A�ڭ̳o�̨�0x0002�C
	//�`�Nlittle endian�Ҧ��A�Cbyte���Ӧb�e�C
	0x02,
	0x00,
	
	//bcdDevice�r�q�C�ڭ̳o��USB�ƹ���L��}�l���A�N�s��1.0���a�A�Y0x0100�C
	//little endian�Ҧ��A�Cbyte�b���C
	0x00,
	0x01,
	
	//iManufacturer�r�q�Cvendor string��index�A���F��K�O�ЩM�޲z�A
	//string index�N�q1�}�l�a�C
	0x01,
	
	//iProduct�r�q�Cproduce sting��index�C���ΤF1�A�o�̴N��2�a�C
	//�`�Nstring index���n�ϥάۦP���ȡC
	0x02,
	
	//iSerialNumber�r�q�Cdevice's serial number string��index�C
	//�o�̨�3�N�i�H�F�C
	0x03,
	
	///bNumConfigurations�r�q�C�ӳ]�ƩҨ㦳��configuration�ơC
	//�ڭ̥u�ݭn�@��configuration�N��F�A�]���ӭȳ]�m��1�C
	0x01
};

//USB report descriptor���w�q
code unsigned char ReportDescriptor[]=
{
	//�C��}�l���Ĥ@��byte����item��prefix�Aprefix���榡���G
	//D7~D4�GbTag�CD3~D2�GbType�FD1~D0�GbSize�C�H�U���O��C��item�����C
	
	//�o�O�@�� global item (bType��1)�A�Nusage page��ܬ����q�ୱGeneric Desktop Page(0x01)
	//�᭱��@��byte�ƾ� (bSize��1)�A�᭱��byte�ƥشN�������F�A
	//�ۤv�ھ�bSize�ӧP�_�C
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	
	//�o�O�@�� local item (bType��2)�A�������U�Ӫ����X�γ~�Ω���L
	0x09, 0x06, // USAGE (Keyboard)
	
	//�o�O�@�� main item (bType��0)�A�}���X�A�᭱�򪺼ƾ�0x01���
	//��collection�O�@��application collection�C�����ʽ�b�e����usage page�Musage�w�q��
	//���q�ୱ�Ϊ���L�C
	0xa1, 0x01, // COLLECTION (Application)
	
		//�o�O�@�� global item�A��� usage page����L�]Keyboard/Keypad(0x07)�^
		0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
		
		//�o�O�@�� local item�A����usage���̤p�Ȭ�0xe0�C��ڤW�O��L��Ctrl��C
		//���骺usage value�i�bHID usage table���d�ݡC
		0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
		
		//�o�O�@�� local item�A����usage���̤j�Ȭ�0xe7�C��ڤW�O��L�kGUI��C
		0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
		
		//�o�O�@�� global item�A������^���ƾڪ��޿�ȡ]�N�O�ڭ̪�^��data field���ȡ^
		//�̤p��0�C�]���ڭ̳o�̥�Bit�Ӫ�ܤ@��data field�A�]���̤p��0�A�̤j��1�C
		0x15, 0x00, // LOGICAL_MINIMUM (0)
		
		//�o�O�@�� glabal item�A�����޿�ȳ̤j��1�C
		0x25, 0x01, // LOGICAL_MAXIMUM (1)
		
		//�o�O�@�� global item�A����data field���ƶq���K�ӡC
		0x95, 0x08, // REPORT_COUNT (8)
		
		//�o�O�@�� global item�A�����C��data field�����׬�1��bit�C
		0x75, 0x01, // REPORT_SIZE (1)
		
		//�o�O�@�� main item�A������8�Ӫ��׬�1bit��data field�]�ƶq�M���ץѫe�������golbal item�ҩw�q�^�ΨӰ�����J�A
		//�ݩʬ��GData,Var,Abs�CData��ܳo�Ǽƾڥi�H�ܰ�
		//Var��ܳo�Ǽƾڰ�O�W�ߪ��A�C�Ӱ��ܤ@�ӷN��C
		//Abs��ܵ���ȡC
		//�o�˩w�q�����G�N�O�A��Y�Ӱ쪺�Ȭ�1�ɡA�N��ܹ���������U�C
		//bit0�N������Usage�̤p��0xe0�Abit7������Usage�̤j��0xe7�C
		0x81, 0x02, // INPUT (Data,Var,Abs)
		
		//�o�O�@�� global item�A����data field�ƶq��1��
		0x95, 0x01, // REPORT_COUNT (1)
		
		//�o�O�@�� global item�A�����C��data field�����׬�8bit�C
		0x75, 0x08, // REPORT_SIZE (8)
		
		//�o�O�@�� main item�A��J�ΡA�ѫe�����main item�i���A���׬�8bit�A
		//�ƶq��1�ӡC�����ݩʬ�constant�]�Y��^���ƾڤ@���O0�^�C
		//��byte�O�O�dbyte�]�O�d��OEM�ϥΡ^�C
		0x81, 0x03, // INPUT (Cnst,Var,Abs)
		
		//�o�O�@�� global item�C�w�qbit field�ƶq��6�ӡC
		0x95, 0x06, // REPORT_COUNT (6)
		
		//�o�O�@�� global item�C�w�q�C��bit field���׬�8bit�C
		//���o�̳o��item���n�]�O�i�H���A�]���b�e���w�g���@�өw�q
		//���׬�8bit��global item�F�C
		0x75, 0x08, // REPORT_SIZE (8)
		
		//�o�O�@�� global item�A�w�q�޿�̤p�Ȭ�0�C
		//�P�W�A�o�̳o��global item�]�O�i�H���n���A�]���e���w�g���@��
		//�w�q�޿�̤p�Ȭ�0��global item�F�C
		0x15, 0x00, // LOGICAL_MINIMUM (0)
		
		//�o�O�@�� global item�A�w�q�޿�̤j�Ȭ�255�C
		0x25, 0xFF, // LOGICAL_MAXIMUM (255)
		
		//�o�O�@�� global item�A���usage page����L�C
		//�e���w�g��ܹLusage page����L�F�A�ҥH��item���n�]�i�H�C
		0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
		
		//�o�O�@�� local item�A�w�qusage�̤p�Ȭ�0�]0��ܨS������U�^
		0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
		
		//�o�O�@�� local item�A�w�qusage�̤j�Ȭ�0x65
		0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
		
		//�o�O�@�� main item�C�������o����8bit��data field�O��J�Ϊ��A
		//�ݩʬ��GData,Ary,Abs�CData�����ƾڬO�i�H�ܪ�
		//Ary�����o�Ǽƾڰ�O�@��array�A�Y�C��8bit���i�H��ܬY����ȡA
		//�p�G���U����Ӧh�]�Ҧp�W�L�o�̩w�q�����שΪ���L�����L�k
		//���y�X���䱡�p�ɡ^�A�h�o�Ǽƾڪ�^��1�]�G�i��^�A��ܫ���L�ġC
		//Abs��ܳo�ǭȬO����ȡC
		0x81, 0x00, // INPUT (Data,Ary,Abs)
		
		/*
		//�H�U��output report���y�z
		//�޿�̤p�ȫe���w�g���w�q��0�F�A�o�̥i�H�ٲ��C 
		//�o�O�@�� global item�A�����޿�ȳ̤j��1�C
		0x25, 0x01, // LOGICAL_MAXIMUM (1)
		
		//�o�O�@�� global item�A����data field�ƶq��5�ӡC 
		0x95, 0x05, // REPORT_COUNT (5)
		
		//�o�O�@�� global item�A����data field�����׬�1bit�C
		0x75, 0x01, // REPORT_SIZE (1)
		
		//�o�O�@�� global item�A�����ϥΪ�usage page�����ܿO�]LED�^
		0x05, 0x08, // USAGE_PAGE (LEDs)
		
		//�o�O�@�� local item�A����usage�̤p�Ȭ��Ʀr��L�O�C
		0x19, 0x01, // USAGE_MINIMUM (Num Lock)
		
		//�o�O�@�� local item�A����usage�̤j�Ȭ�Kana�O�C
		0x29, 0x05, // USAGE_MAXIMUM (Kana)
		
		//�o�O�@�� main item�C�w�q��X�ƾڡA�Y�e���w�q��5��LED�C
		0x91, 0x02, // OUTPUT (Data,Var,Abs)
		
		//�o�O�@�� global item�C�w�qbit field�ƶq��1�ӡC
		0x95, 0x01, // REPORT_COUNT (1)
		
		//�o�O�@�� global item�C�w�qbit field���׬�3bit�C
		0x75, 0x03, // REPORT_SIZE (3)
		
		//�o�O�@�� main item�A�w�q��Xconstant�A�e���ΤF5bit�A�ҥH�o�̻ݭn
		//3��bit�Ӵꦨ�@��byte�C
		0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
		*/
	
	//�U���o�� main item�Ψ������e����collection�CbSize��0�A�ҥH�᭱�S�ƾڡC
	0xc0 // END_COLLECTION
};
//�q�L�W����report descriptor���w�q�A�ڭ̪��D��^��input report�㦳8 byte�C
//�Ĥ@��byte��8��bit�ΨӪ�ܯS����O�_���U�]�ҦpShift�BAlt����^�C
//�ĤG��byte���O�d�ȡA�Ȭ�constant 0�C�ĤT��ĤK��byte�O�@�Ӵ��q����Ȫ�array 
//��S������U�ɡA����6��byte�ȳ���0�C��u���@�Ӵ��q����U�ɡA
//�o����byte�����Ĥ@��byte�ȧY���ӫ��䪺��ȡ]���骺��ȽЬ�HID��usage table���ɡ^
//���h�Ӵ��q��P�ɫ��U�ɡA�h�P�ɪ�^�o���䪺��ȡC
//�p�G���U����Ӧh�A�h�o����byte����0xFF�]�����^0x00�A�o�˷|���@�~�t�λ{���Ҧ��䳣�w�g����^�C
//�ܩ���Ȧbarray�������ᶶ�ǬO�L�ҿת�
//�@�~�t�η|�t�d�ˬd�O�_���s����U�C�ڭ����Ӧbinterrupt endpoint 1�����ӤW�����榡��^��ڪ���L�ƾڡC
//
//�t�~�Areport���٩w�q�F�@��byte��out report
//�O�Ψӱ���LED���p���C�u�ϥΤF�C7 bit�A��1 bit�O�O�d��0�C
//��Ybit���Ȭ�1�ɡA�h��ܹ�����LED�n�I�G�C�@�~�t�η|�t�d�P�B�U��
//��L������LED�A�Ҧp�A�������L�A�@�����Ʀr��L�O�G�ɡA�t�@��
//�]�|��۫G�C��L�������ݭn�P�_�U��LED���Ӧ�ɫG�A���u�O���ݥD��
//�o�ereport�����A�M��ھ�report�Ȩ��I�G������LED�C�ڭ̦bendpoint 1 OUT interrupt
//��Ū�X�o1 byte��outpur report�A�M��復���ϡ]�]���ǲߪO�W��LED�O�C�q����
//�G�^�A�����o�e��LED�W�C�o��main��Ƥ������I�GLED���N�X�N���ݭn�F�C
///////////////////////////report descriptor����////////////////////////////


//USB�t�m�y�z�Ŷ��X���w�q
//configuration descriptor�`���׬�9+9+9+7 byte
code unsigned char ConfigurationDescriptor[9+9+9+7]=
{
 /***************configuration descriptor***********************/
	//bLength�r�q�Cconfiguration descriptor�����׬�9byte�C
	0x09,
	
	//bDescriptorType�r�q�Cconfiguration descriptor�s����0x02�C
	0x02,
	
	//wTotalLength�r�q�C�t�m�y�z�Ŷ��X���`���סA
 	//�]�Aconfiguration descriptor�����Binterface descriptor�Bclass descriptor�Bendpoint descriptor���C
	sizeof(ConfigurationDescriptor)&0xFF,      //�Cbyte
	(sizeof(ConfigurationDescriptor)>>8)&0xFF, //��byte 
	
	//bNumInterfaces�r�q�C��configuration�]�t��interface�ơA�u���@��interface�C
	0x01,
	
	//bConfiguration�r�q�C��configuration���Ȭ�1�C
	0x01,
	
	//iConfigurationz�r�q�A��configuration��string index�C�o�̨S���A��0�C
	0x00,
	
	//bmAttributes�r�q�A�ӳ]�ƪ��ݩʡC�ѩ�ڭ̪��O�l�Obus�ѹq���A
 	//�åB�ڭ̤��Q��{remote wakeup���\��A�ҥH�Ӧr�q���Ȭ�0x80�C
	0x80,
	
	//bMaxPower�r�q�A�ӳ]�ƻݭn���̤j�q�y�q�C�ѩ�ڭ̪��O�l
 	//�ݭn���q�y����100mA�A�]���ڭ̳o�̳]�m��100mA�C�ѩ�C���
 	//�q�y��2mA�A�ҥH�o�̳]�m��50(0x32)�C
	0x32,
	
	/*******************interface descriptor*********************/
	//bLength�r�q�Cinterface descriptor�����׬�9byte�C
	0x09,
	
	//bDescriptorType�r�q�Cinterface descriptor���s����0x04�C
	0x04,
	
	//bInterfaceNumber�r�q�C��interface���s���A�Ĥ@��interface�A�s����0�C
	0x00,
	
	//bAlternateSetting�r�q�C��interface���ƥνs���A��0�C
	0x00,
	
	//bNumEndpoints�r�q�Cnon-zero endpoint���ƥءC��USB��L�ݭn�G��interrupt endpoint
	//(�@��IN�B�@��OUT)�A�]���ӭȬ�2 (���u�ϥ�IN endpoint�A�ҥH�u���@��endpoint)�C
	0x01,
	
	//bInterfaceClass�r�q�C��interface�ҨϥΪ�class�CUSB��L�OHID class�A
	//HID class���s�X��0x03�C
	0x03,
	
	//bInterfaceSubClass�r�q�C��interface�ҨϥΪ�subclass�C�bHID1.1��ĳ���A
	//�u�W�w�F�@��subclass�G���BIOS�޾ɱҰʪ�subclass�C
	//USB��L�B�ƹ��ݩ��subclass�Asubclass�N�X��0x01�C
	0x01,
	
	//bInterfaceProtocol�r�q�C�p�Gsubclass������޾ɱҰʪ�subclass�A
	//�h��ĳ�i��ܷƹ��M��L�C��L�N�X��0x01�A�ƹ��N�X��0x02�C
	0x01,
	
	//iConfiguration�r�q�C��interface��string index�C�o�̨S���A��0�C
	0x00,
	
	/******************HID descriptor************************/
	//bLength�r�q�C��HID descriptor�U�u���@�ӤU��descriptor�C�ҥH���׬�9 byte�C
	0x09,
	
	//bDescriptorType�r�q�CHID descriptor���s����0x21�C
	0x21,
	
	//bcdHID�r�q�C����ĳ�ϥΪ�HID1.1��ĳ�C�`�N�Cbyte�b���C
	0x10,
	0x01,
	
	//bCountyCode�r�q�C�]�ƾA�Ϊ���a�N�X�A�o�̿�ܬ�����A�N�X0x21�C
	0x21,
	
	//bNumDescriptors�r�q�C�U�Ŵy�z�Ū��ƥءC�ڭ̥u���@��report descriptor�C
	0x01,
	
	//bDescriptorType�r�q�C�U�Ŵy�z�Ū������A��report descriptor�A�s����0x22�C
	0x22,
	
	//bDescriptorLength�r�q�C�U�Ŵy�z�Ū����סC�U��descroptor��report descriptor�C
	sizeof(ReportDescriptor)&0xFF,
	(sizeof(ReportDescriptor)>>8)&0xFF,
	
	/**********************IN endpoint descriptor***********************/
	//bLength�r�q�Cendpoint descriptor���׬�7byte�C
	0x07,
	
	//bDescriptorType�r�q�Cendpoint descriptor�s����0x05�C
	0x05,
	
	//bEndpointAddress�r�q�Cendpoint���a�}�C�ڭ̨ϥ�D12��IN endpoint 1�C
	//D7���ܼƾڤ�V�AIN endpoint D7��1�C�ҥHIN endpoint 1 ���a�}��0x81�C
	0x81,
	
	//bmAttributes�r�q�CD1~D0��endpoint�ǿ�������ܡC
	//��endpoint��interrupt endpoint�Cinterrupt endpoint���s����3�C�䥦bit�O�d��0�C
	0x03,
	
	//wMaxPacketSize�r�q�C��endpoint���̤jpacket���Cendpoint 1 ���̤jpacket����16byte�C
	//�`�N�Cbyte�b���C
	0x10,
	0x00,
	
	//bInterval�r�q�Cendpoint polling���ɶ��A�ڭ̳]�m��10��frame�ɶ��A�Y10ms�C
	0x0A

	/**********************OUT endpoint descriptor***********************/
	/*
	//bLength�r�q�Cendpoint descriptor���׬�7 byte�C
	0x07,
	
	//bDescriptorType�r�q�Cendpoint descriptor�s����0x05�C
	0x05,
	
	//bEndpointAddress�r�q�Cendpoint���a�}�C�ڭ̨ϥ�D12��OUT endpoint 1�C
	//D7���ܼƾڤ�V�AOUT endpoint D7��0�C�ҥHOUT endpoint 1 ���a�}��0x01�C
	0x01,
	
	//bmAttributes�r�q�CD1~D0��endpoint�ǿ�������ܡC
	//��endpoint��interrupt endpoint�Cinterrupt endpoint���s����3�C�䥦bit�O�d��0�C
	0x03,
	
	//wMaxPacketSize�r�q�C��endpoint���̤jpacket���Cendpoint 1 ���̤jpacket����16 byte�C
	//�`�N�Cbyte�b���C
	0x10,
	0x00,
	
	//bInterval�r�q�Cendpoint polling���ɶ��A�ڭ̳]�m��10��frame�ɶ��A�Y10ms�C
	0x0A
	*/
};
////////////////////////�t�m�y�z�Ŷ��X����//////////////////////////

/************************Language ID���w�q********************/
code unsigned char LanguageId[4]=
{
	0x04,     //��descriptor������
	0x03,     //descriptor type��string descriptor
	//0x0409�������^�y��ID
	0x09,
	0x04
};
////////////////////////Language ID����//////////////////////////////////

//�i�ѦҦ�������unicode�ഫ
//https://www.chineseconverter.com/zh-tw/convert/unicode 

//�r�ꬰ"�w����[�ڪ�github:XassassinXsaberX" 
//8bit��little endian�榡 
code unsigned char ManufacturerStringDescriptor[60]={
	60,         //��descriptor�����׬�60 byte
	0x03,       //string descriptor�������s�X��0x03
	0x61,0xb6,  //�w
	0xce,0x8f,  //�� 
	0xc3,0x53,  //�� 
	0xc0,0x89,  //�[ 
	0x11,0x62,  //�� 
	0x84,0x76,  //�� 
	0x67,0x00,  //g
	0x69,0x00,  //i
	0x74,0x00,  //t
	0x68,0x00,  //h
	0x75,0x00,  //u
	0x62,0x00,  //b
	0x3a,0x00,  //:
	0x58,0x00,  //X
	0x61,0x00,  //a
	0x73,0x00,  //s
	0x73,0x00,  //s
	0x61,0x00,  //a
	0x73,0x00,  //s
	0x73,0x00,  //s
	0x69,0x00,  //i
	0x6e,0x00,  //n
	0x58,0x00,  //X
	0x73,0x00,  //s
	0x61,0x00,  //a
	0x62,0x00,  //b
	0x65,0x00,  //e
	0x72,0x00,  //r
	0x58,0x00   //X
};
/////////////////////////vendor string����/////////////////////////////

//�r��"phisoner����L" 
//8bit little endian�榡
code unsigned char ProductStringDescriptor[24]={
	24,         //��descriptor�����׬�24 byte
	0x03,       //string descriptor�������s�X��0x03
	0x70,0x00,  //p
	0x68,0x00,  //h
	0x69,0x00,  //i
	0x73,0x00,  //s
	0x6f,0x00,  //o
	0x6e,0x00,  //n
	0x65,0x00,  //e
	0x72,0x00,  //r
	0x84,0x76,  //�� 
	0x75,0x93,  //�� 
	0xe4,0x76   //�L 
};
////////////////////////product string����////////////////////////////


//�r�ꬰ��2017-11-16����Unicode�s�X
//8 bit��little endian�榡
code unsigned char SerialNumberStringDescriptor[22]={
	22,         //��descriptor�����׬�22 byte
	0x03,       //string descriptor�������s�X��0x03
	0x32, 0x00, //2
	0x30, 0x00, //0
	0x31, 0x00, //1
	0x37, 0x00, //7
	0x2d, 0x00, //-
	0x31, 0x00, //1
	0x31, 0x00, //1
	0x2d, 0x00, //-
	0x31, 0x00, //1
	0x36, 0x00  //6
};
//////////////////////device's serial number string����/////////////////////////


void DelayXms(unsigned int t)	 //delay t ms
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<120;i++);
}

void DelayXus(unsigned int t)	 //delay t us
{
	for(;t>0;t--)
		_nop_();
}

void UsbDisconnect(void) //USB�_�}�s�� , interrupt�B�z���
{
 	#ifdef DEBUG0
		Prints("USB disconnect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);		//�g�JSet Mode command��USB������ (��command�ݭn�A��J2byte���)
	D12WriteByte(0x06);		            //�g�J�Ĥ@��byte����ƨ�USB������
	D12WriteByte(0x47);				    //�g�J�ĤG��byte����ƨ�USB������
	DelayXms(1000);                     //delay 1ms (�ϥD���T�{�]�Ƥw�g�_�}�s��)
}

void UsbConnect(void)    //USB�s�� , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB connect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);	  	//�g�JSet Mode command��USB������ (��command�ݭn�A��J2byte���)
	D12WriteByte(0x16);		            //�g�J�Ĥ@��byte����ƨ�USB������
	D12WriteByte(0x47);				    //�g�J�ĤG��byte����ƨ�USB������
}

void UsbBusSuspend(void) //bus suspend , interrupt�B�z��� 
{
	#ifdef DEBUG0
		Prints("USB Bus Suspend\n");
	#endif
}

void UsbBusReset(void)   //bus reset , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB Bus Reset\n");
	#endif
	Ep1InIsBusy = 0;
} 

void UsbEp0Out(void)     //endpoint 0 OUT , interrupt�B�z��� 
{
	char status;
	int i;

	//�H�U��standard device request���U��field
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned int wValue;
	unsigned int wIndex;
	unsigned int wLength;

	#ifdef DEBUG0
		Prints("USB endpoint 0 OUT interrupt\n");	
	#endif

	status = D12ReadEndpointLastStatus(0);	    //"Ū��endpoint 0 OUT�̫᣸��transaction�����A" ��command
	                                            //�өR�O�i�M��interrupt register���A�C�@��interrupt�����쪺interrupt flag
                                                //�NŪ���쪺 "�̫᣸��transaction�����A���" �s��status�ܼƤ�
   	#ifndef	DEBUG0
		DelayXus(50);	                        //�Y��delay 50 us�N�|�X�{���~(�ӥB����delay�ӧ֩ΤӤ[)�A�ثe���L�k����...
	#endif		
									 
	if((status>>5) & 0x01)	                    //�p�G�̫᣸��transaction��setup stage
	{
		for(i=0;i<16;i++)
			Buffer[i] = 0;
		D12ReadEndpointBuffer(0, 16, Buffer);	//Ū�����wendpoint��buffer
		D12AcknowledgeSetup();	                //Acknowledge Setup
		D12ClearBuffer(0);			            //�M��endpoint 0 OUT��buffer
		
		
		//���U�ӨM�wUSB standard device request���U��field
		//����Buffer[0] Buffer[1] Buffer[2] Buffer[3] Buffer[4] Buffer[5] Buffer[6] Buffer[7]
		bmRequestType = Buffer[0];
		bRequest = Buffer[1];
		wValue = Buffer[3];	     	//��little endian�榡�A�ҥHwValue��Buffer[3] Buffer[2]
		wValue <<= 8;
		wValue |= Buffer[2]; 
		wIndex = Buffer[5];		    //��little endian�榡�A�ҥHwIndex��Buffer[5] Buffer[4]
		wIndex <<= 8;
		wIndex |= Buffer[4]; 
		wLength = Buffer[7];		//��little endian�榡�A�ҥHwLength��Buffer[7] Buffer[6]
		wLength <<= 8;
		wLength |= Buffer[6];
		
		if((bmRequestType>>7)&0x01)	 //IN request
		{
			if(((bmRequestType>>5)&0x03) == 0)       //standard request
			{
				
				#ifdef DEBUG0
					Prints("USB standard IN request : ");
				#endif
				if(bRequest == GET_CONFIGURATION)
				{
					#ifdef DEBUG0
						Prints("GET_CONFIGURATION\n");
					#endif
				}
				else if(bRequest == GET_DESCRIPTOR)
				{
					#ifdef DEBUG0
						Prints("GET_DESCRIPTOR---");
					#endif
					if(((wValue>>8)&0xff) == DEVICE_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("device descriptor\n");
						#endif	
						if(wLength > DeviceDescriptor[0])		//�M�w�n�e�X��byte
							SendLength =  DeviceDescriptor[0];  
						else
							SendLength = wLength;

						sendPtr = DeviceDescriptor;			    //�M�w�n�e���Ǹ��

						UsbEp0SendData();                       //�z�Lendpoint 0 IN�e�X���
					}
					else if(((wValue>>8)&0xff) == CONFIGURATION_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("configuration descriptor\n");
						#endif	
						if(wLength > sizeof(ConfigurationDescriptor) )		//�M�w�n�e�X��byte
							SendLength =  sizeof(ConfigurationDescriptor);  
						else
							SendLength = wLength;

						sendPtr = ConfigurationDescriptor;		         	//�M�w�n�e���Ǹ��

						UsbEp0SendData();		                            //�z�Lendpoint 0 IN�e�X���

					}
					else if(((wValue>>8)&0xff) == STRING_DESCRIPTOR)
					{
						
						#ifdef DEBUG0
							Prints("string descriptor\n");
						#endif	
						
						if((wValue & 0xff) == 0)
						{
							//Prints("Index 0\n");				   //�M�w�n�e�X��byte
							if(wLength > sizeof(LanguageId) )
								SendLength =  sizeof(LanguageId);  
							else
								SendLength = wLength;
							
							sendPtr = LanguageId;			       //�M�w�n�e���Ǹ��	
						}
						else if((wValue & 0xff) == 1)
						{
							//Prints("Index 1\n");
							if(wLength > sizeof(ManufacturerStringDescriptor) )		 //�M�w�n�e�X��byte
								SendLength =  sizeof(ManufacturerStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ManufacturerStringDescriptor;			         //�M�w�n�e���Ǹ��
						}
						else if((wValue & 0xff) == 2)
						{
							//Prints("Index 2\n");
							if(wLength > sizeof(ProductStringDescriptor) )		//�M�w�n�e�X��byte
								SendLength =  sizeof(ProductStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ProductStringDescriptor;		            //�M�w�n�e���Ǹ��
						
						}
						else if((wValue & 0xff) == 3)
						{
							//Prints("Index 3\n");
							if(wLength > sizeof(SerialNumberStringDescriptor) )		 //�M�w�n�e�X��byte
								SendLength =  sizeof(SerialNumberStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = SerialNumberStringDescriptor;			         //�M�w�n�e���Ǹ��
						}
						else
						{
							Prints("Index error\n");
						}								                          //�z�Lendpoint 0 IN�e�X���
						UsbEp0SendData();
					}
					else if(((wValue>>8)&0xff) == INTERFACE_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("interface descriptor\n");
						#endif	
					}
					else if(((wValue>>8)&0xff) == ENDPOINT_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("endpoint descriptor\n");
						#endif	
					}
					else if(((wValue>>8)&0xff) == REPORT_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("report descriptor\n");
						#endif	
						if(wLength > sizeof(ReportDescriptor) )
								SendLength =  sizeof(ReportDescriptor);  //�M�w�n�e�X��byte
						else
							SendLength = wLength;

						sendPtr = ReportDescriptor;			             //�M�w�n�e���Ǹ��
						UsbEp0SendData();
					}
					else
					{
						#ifdef DEBUG0
							Prints("unknown descriptor\n");
						#endif	
					}
				}
				else if(bRequest == GET_INTERFACE)
				{
					#ifdef DEBUG0
						Prints("GET_INTERFACE\n");
					#endif
				}
				else if(bRequest == GET_STATUS)
				{
					#ifdef DEBUG0
						Prints("GET_STATUS\n");
					#endif
				}
				else if(bRequest == SYNCH_FRAME)
				{
					#ifdef DEBUG0
						Prints("SYNCH_FRAME\n");
					#endif
				}
				else
				{
					#ifdef DEBUG0
						Prints("error:undefined standard IN request\n");
					#endif
				}
			}
			else if(((bmRequestType>>5)&0x03) == 1)  //class request
			{
				#ifdef DEBUG0
					Prints("USB class IN request : \n");
				#endif
			}
			else if(((bmRequestType>>5)&0x03) == 2)  //vendor request
			{
				#ifdef DEBUG0
					Prints("USB vendor IN request : \n");
				#endif
			}
			else				                     //reserved
			{
				#ifdef DEBUG0
					Prints("error:undefined IN request\n");
				#endif
			}
		}
		else  //OUT request
		{
			if(((bmRequestType>>5)&0x03) == 0)       //standard request
			{
				#ifdef DEBUG0
					Prints("USB standard OUT request : ");
				#endif
				if(bRequest == CLEAR_FEATURE)
				{
					#ifdef DEBUG0
						Prints("CLEAR_FEATURE\n");
					#endif	
				}
				else if(bRequest == SET_ADDRESS)
				{
					#ifdef DEBUG0
						Prints("SET_ADDRESS\n");
					#endif
					//�����즹standard request�ɡA�ت��O�n����USB�����]�mhost�^�ǵ��L��device address
					D12WriteCommand(D12_SET_ADDRESS_ENABLE);
					//��standard request�� wValue���C 7 bit�Ȭ�device address
					D12WriteByte(0x80 | (wValue & 0x7f));		//��7 bit�]��1�N��enable the function(function�bUSB�������N���O�]��)

					//�^�ǣ���data byte�ƥج�0��data packet�i��host�w���\���즹standard OUT request
					//�^�ǣ���data byte�ƥج�0��data packet�ɡA�N��control transfer����data stage�S��data (�]�i�H��data stage���s�b)
					SendLength = 0;
					UsbEp0SendData();
				}
				else if(bRequest == SET_CONFIGURATION)
				{
					#ifdef DEBUG0
						Prints("SET_CONFIGURATION\n");
					#endif	
				   
				    //���즹standard request�ɡA�ت��O�T�w��configuration�O�_���T�A�Y���T���ܴNenable all non-zero endpoint
					//�ҥH���U�ӭn�g�JSet Endpoint Enable command�A�ت��O�nenable��disable non-zero endpoint
					D12WriteCommand(D12_SET_ENDPOINT_ENABLE);
					ConfigValue = wValue & 0xff; //��standard request�� wValue���Cbyte�Ȭ�configuration 

					//�Y��configuration���ȫD0�ɡA�N�����T��configuration (�i�Ѧ�configuration descriptor����bConfiguration�A�ڭ̧�configuration value�]��1)
					//���ɤ~��enable non-zero endpoint�A�_�hdisable non-zero endpoint
					if(ConfigValue)             
						D12WriteByte(0x01);	  //enable non-zero endpoint
					else
						D12WriteByte(0x00);   //disable non-zero endpoint

					//�^�ǣ���data byte�ƥج�0��data packet�i��host�w���\���즹standard OUT request
					//�^�ǣ���data byte�ƥج�0��data packet�ɡA�N��control transfer����data stage�S��data (�]�i�H��data stage���s�b)
					SendLength = 0;
					UsbEp0SendData();
				}
				else if(bRequest == SET_DESCRIPTOR)
				{
					#ifdef DEBUG0
						Prints("SET_DESCRIPTOR\n");
					#endif	
				}
				else if(bRequest == SET_FEATURE)
				{
					#ifdef DEBUG0
						Prints("SET_FEATURE\n");
					#endif	
				}
				else if(bRequest == SET_INTERFACE)
				{
					#ifdef DEBUG0
						Prints("SET_INTERFACE\n");
					#endif	
				}
				else
				{
					#ifdef DEBUG0
						Prints("error:undefined OUT request\n");
					#endif
				}
			}
			else if(((bmRequestType>>5)&0x03) == 1)  //class request
			{
				#ifdef DEBUG0
					Prints("USB class OUT request : ");
				#endif
				if(bRequest == SET_IDLE)
				{	
					#ifdef DEBUG0
						Prints("SET_IDLE\n");
					#endif
					//�^�ǣ���data byte�ƥج�0��data packet�i��host�w���\���즹standard OUT request
					//�^�ǣ���data byte�ƥج�0��data packet�ɡA�N��control transfer����data stage�S��data (�]�i�H��data stage���s�b)
					SendLength = 0;
					UsbEp0SendData();
				}
				else
				{
					Prints("unknown request\n");
				}
			}
			else if(((bmRequestType>>5)&0x03) == 2)  //vendor request
			{
				#ifdef DEBUG0
					Prints("USB vendor OUT request : \n");
				#endif
			}
			else				                   //reserved
			{
				#ifdef DEBUG0
					Prints("error:undefined OUT request\n");
				#endif
			}	
		}
		
	}
	else
	{
		D12ReadEndpointBuffer(0, 16, Buffer);	//Ū�����wendpoint��buffer
		D12ClearBuffer(0);			            //�M��endpoint 0 OUT buffer
	}
	
		
	
		


}


//�NsendPtr���V����ưe�X
void UsbEp0SendData(void)
{
	//SendLength�ܼƥN��ثe�٦��X��byte����ƻݭn�ǿ�
	//DeviceDescriptor[7]�N��endpoint 0 IN ��buffer�e�q (��쬰byte)
	if(SendLength >= DeviceDescriptor[7])
		D12WriteEndpointBuffer(1,DeviceDescriptor[7],sendPtr);	
	else
		D12WriteEndpointBuffer(1,SendLength,sendPtr);

}


void UsbEp0In(void)      //endpoint 0 IN , interrupt�B�z���
{
	unsigned char status;
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif
	
	status = D12ReadEndpointLastStatus(1); //Ū��endpoint 0 IN���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag

	#ifndef DEBUG0
		DelayXus(50);                      //�Y��delay 50 us�N�|�X�{���~(�ӥB����delay�ӧ֩ΤӤ[)�A�ثe���L�k����...
	#endif

	if(SendLength >= DeviceDescriptor[7])    //SendLength�N��W���ǿ�ɡA�ٻݭn�ǰe����ƪ���
	{
		sendPtr += DeviceDescriptor[7];		 //�i�H��e�U���ո�ƤF
		SendLength -= DeviceDescriptor[7];   //SendLength�{�b�N��ثe�ٻݭn�ǰe����ƪ��סA�]���W�����\���ǰe��ơA�ҥH�ݭn�ǰe��ƪ������ܤ֤F
		UsbEp0SendData();					 //�z�Lendpoint 0 IN �Ӱe���
	}
	
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt�B�z��� 
{
	unsigned char status;
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
	status = D12ReadEndpointLastStatus(3);

	Ep1InIsBusy = 0;
} 

void UsbEp2Out(void)     //endpoint 2 OUT , interrupt�B�z��� 
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 OUT interrupt\n");
	#endif
}

void UsbEp2In(void)      //endpoint 2 IN , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 IN interrupt\n");
	#endif
} 