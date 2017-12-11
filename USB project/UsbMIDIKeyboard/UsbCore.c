#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"
#include <INTRINS.h>

idata unsigned char Buffer[16]; //Ū�� endpoint 0�Ϊ��w�İ�

unsigned char *sendPtr;   //���V���U�ӭn�ǰe�����
unsigned int SendLength;  //�N���٭n�e�h��byte�����
bit SendZeroPacket;       //�Y��1�N��n�edata byte�ƥج�0��data packet�A�Y��0�h�N���ΰe
bit Ep1InIsBusy;
bit Ep2InIsBusy;
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
	
	//idProduct�r�q�CProduct ID�A�ѩ�O�ĤK�ӹ���A�ڭ̳o�̨�0x0008�C
	//�`�Nlittle endian�Ҧ��A�Cbyte���Ӧb�e�C
	0x08,
	0x00,
	
	//bcdDevice�r�q�C�ڭ̳o��USB MIDI��L��}�l���A�N�s��1.0���a�A�Y0x0100�C
	//little endian�Ҧ��A�C�r�`�b���C
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


//USB�t�m�y�z�Ŷ��X���w�q
//configuration descriptor�`���׬�9+9+9+9+7+6+6+9+9+7+5+7+5 byte
code unsigned char ConfigurationDescriptor[9+9+9+9+7+6+6+9+9+7+5+7+5]=
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
	
	//bNumInterfaces�r�q�C��configuration�]�t��interface�ơA�����interface�C
	0x02,
	
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
	
	/**********************************************************************/
	/*****************audio control interface descriptor*******************/ 
	//bLength�r�q�Cinterface descriptor�����׬�9 byte�C
	0x09, 
	
	//bDescriptorType�r�q�Cinterface descriptor���s����0x04�C
	0x04, 
	
	//bInterfaceNumber�r�q�C��interface���s���A�Ĥ@��interface�A�s����0�C
	0x00, 
	
	//bAlternateSetting�r�q�C��interface���ƥνs���A��0�C
	0x00, 
	
	//bNumEndpoints�r�q�Cnon-zero endpoint���ƥءC��interface�S��endpoint
	0x00,
	
	//bInterfaceClass�r�q�C��interface�ҨϥΪ�class�Caudio interface class���N�X��0x01�C
	0x01,
	
	//bInterfaceSubClass�r�q�C��interface�ҨϥΪ�subclass�Caudio control interface��subclass�N�X��0x01�C
	0x01, 
	
	//bInterfaceProtocol�r�q�C�S���ϥΨ�ĳ�C
	0x00, 
	
	//iConfiguration�r�q�C��interface��string descriptor index�C�o�̨S���A��0�C
	0x00, 
	
	
	/*************class-specific AC (Audio Control) interface descriptor**********/ 
	//bLength�r�q�A��descriptor�����סC��9 byte�C
	0x09, 
	
	//bDescriptorType�r�q�Adescriptor�������C�s����0x24�A(Class-Specific) CS_INTERFACE_DESCRIPTOR�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Adescriptor subtype�C�s����0x01�AHEADER�C(header subtype)
	0x01, 
	
	//bcdADC�r�q�A��ĳ�����C�o�̬�1.0���C
	0x00, 
	0x01, 
	
	//wTotalLength�r�q�Aclass-specific descriptor���`���סC�o�̬�9 byte�C
	0x09, 
	0x00, 
	
	//bInCollection�r�q�Astream interface���ƶq�C�o�̶Ȧ��@�ӡC
	0x01, 
	
	//baInterfaceNr�r�q�A�ݩ�interface��stream interface�s���CMIDIStreaming interface �ݩ�audio control interface
	0x01,  //�YbInterfaceNumber = 1 ��interface�ݩ�audio control interface
	/***************************************************************************/

	/***************************************************************************/	
	/*****************MS (MIDIStreaming) interface descriptor*******************/ 
	//bLength�r�q�Cinterface descriptor�����׬�9 byte�C
	0x09, 
	
	//bDescriptorType�r�q�Cinterface descriptor���s����0x04�C
	0x04, 
	
	//bInterfaceNumber�r�q�C��interface���s���A�ĤG��interface�A�s����1�C
	0x01, 
	
	//bAlternateSetting�r�q�C��interface���ƥνs���A��0�C
	0x00, 
	
	//bNumEndpoints�r�q�Cnon-zero endpoint���ƥءCMIDIStreaming interface�ϥΤ@��bulk OUT/IN endpoint
	0x02, 
	
	//bInterfaceClass�r�q�C��interface�ҨϥΪ�class�Caudio interface class���N�X��0x01�C
	0x01, 
	
	//bInterfaceSubClass�r�q�C��interface�ҨϥΪ�subclass�CMIDIStreaming interface��subclass�N�X��0x03�C
	0x03, 
	
	//bInterfaceProtocol�r�q�C�S���ϥΨ�ĳ�C
	0x00, 
	
	//iConfiguration�r�q�C��interface��string descriptor index�C�o�̨S���A��0�C
	0x00, 
	
	/**********class-specific MS (MIDIStreaming) interface descriptor*****/
	/*******header descriptor********/ 
	//bLength�r�q�C��descriptor�����סA7 byte�C
	0x07,
	
	//bDescriptorType�r�q�C��descriptor�������A��(Class-Specific) CS_INTERFACE�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Cdescriptor subtype�A��MS_HEADER (header subtype) 
	0x01, 
	
	//bcdMSC�r�q�C��MIDIStreaming Class�ҨϥΪ���ĳ�����A��1.0 
	0x00, 
	0x01, 
	
	//wTotalLengthz�r�q�C���class-specific MIDIStreaming interface descriptor�`����
	0x25, 
	0x00, 
	
	/**********embedded IN Jack descriptor********/ 
	//bLength�r�q�C��descriptor�����סA��6 byte�C
	0x06, 
	
	//bDescriptorType�r�q�C��descriptor�������A��(Class-Specific) CS_INTERFACE�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Cdescriptor subtype�A��MIDI_IN_JACK 
	0x02, 
	
	//bJackType�r�q�C��Jack�������A��embedded
	0x01, 
	
	//bJackID�r�q�C��Jack���ߤ@ID�A�o�̨���1 
	0x01, 
	
	//iJack�r�q�C��Jack��string descriptor index�A�o�̨S���A��0 
	0x00,
	
	/**********external IN Jack descriptor********/ 
	//bLength�r�q�C��descriptor�����סA��6 byte�C
	0x06, 
	
	//bDescriptorType�r�q�C��descriptor�������A��(Class-Specific) CS_INTERFACE�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Cdescriptor subtype���A��MIDI_IN_JACK 
	0x02, 
	
	//bJackType�r�q�C��Jack�������A��external
	0x02, 
	
	//bJackID�r�q�C��Jack���ߤ@ID�A�o�̨���2 
	0x02, 
	
	//iJack�r�q�C��Jack��string descriptor index�A�o�̨S���A��0 
	0x00, 
	
	/**********embedded OUT Jack descriptor********/ 
	//bLength�r�q�C��descriptor�����סA�� 9byte�C
	0x09, 
	
	//bDescriptorType�r�q�C��descriptor�������A��(Class-Specific) CS_INTERFACE�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Cdescriptor subtype�A��MIDI_OUT_JACK 
	0x03, 
	
	//bJackType�r�q�C��Jack�������A��embedded
	0x01, 
	
	//bJackID�r�q�C��Jack���ߤ@ID�A�o�̨���3 
	0x03,
	
	//bNrInputPins�r�q�C��OUT Jack��input pin�ơC�o�̶Ȧ��@�ӡC
	0x01, 
	
	//baSourceID�r�q�C�s�����Jack input pin��IN Jack��ID�A��ܬ�external IN Jack 2 
	0x02,  //����bJackID��0x02��Jack
	
	//BaSourcePin�r�q�Cexternal IN Jack�s���b��Jack��input pin 1 �W
	0x01, 
	
	//iJack�r�q�C��Jack��string descriptor index�A�o�̨S���A��0 
	0x00, 
	
	/**********external OUT Jack descriptor********/ 
	//bLength�r�q�C��descriptor�����סA��9 byte�C
	0x09, 
	
	//bDescriptorType�r�q�C��descriptor�������A��(Class-Specific) CS_INTERFACE�C
	0x24, 
	
	//bDescriptorSubtype�r�q�Cdescriptor subtype�A��MIDI_OUT_JACK 
	0x03, 
	
	//bJackType�r�q�C��Jack�������A��external
	0x02, 
	
	//bJackID�r�q�C��Jack���ߤ@ID�A�o�̨���4 
	0x04, 
	
	//bNrInputPins�r�q�C��OUT Jack��input pin�ơC�o�̶Ȧ��@�ӡC
	0x01, 
	
	//baSourceID�r�q�C�s�����Jack input pin��IN Jack��ID�A��ܬ�embedded IN Jack 1
	0x01,  //����bJackID��0x01��Jack
	
	//BaSourcePin�r�q�Cembedded IN Jack�s���b��Jack��input pin 1 �W 
	0x01,
	
	//iJack�r�q�C��Jack��string descriptor index�A�o�̨S���A��0 
	0x00, 
	/****************************************************************************************/

	/****************************************************************************************/	
	/*************standard MS(MIDIStreaming) bulk data IN endpoint descriptor****************/ 
	//bLength�r�q�Cendpoint descriptor���׬�7 byte�C
	0x07, 
	
	//bDescriptorType�r�q�Cendpoint descriptor�s����0x05�C
	0x05, 
	
	//bEndpointAddress�r�q�Cendpoint���a�}�C�ڭ̨ϥ�D12��IN endpoint 2�C
	//D7���ܼƾڤ�V�AIN endpoint D7��1�C�ҥHIN endpoint 2���a�}��0x82�C
	0x82, 
	
	//bmAttributes�r�q�CD1~D0��endpoint�ǿ�������ܡC
	//��endpoint��bulk endpoint�Cbulk endpoint���s����2�C�䥦bit�O�d��0�C
	0x02, 
	
	//wMaxPacketSize�r�q�C��endpoint���̤jpacket���סCendpoint 2���̤jpacket���׬�64 byte�C
	//�`�N�Cbyte�b���C
	0x40, 
	0x00, 
	
	//bInterval�r�q�Cendpoint polling���ɶ��A���B�L�N�q�C
	0x00, 
	
	/**********class-specific MS(MIDIStreaming) bulk data endpoint descriptor********/ 
	//bLength�r�q�A��descriptor�����סC��5 byte�C
	0x05, 
	
	//bDescriptorType�r�q�A��descriptor�������A��class-specific endpoint descriptor ((Class-Specific) CS_ENDPOINT)
	0x25, 
	
	//bDescriptorSubType�r�q�A��descriptor��subtype�A��(MIDIStreaming) MS_GENERAL 
	0x01, 
	
	//bNumEmbMIDIJack�r�q�A��endpoint��embedded MIDI OUT Jack���ƶq�A�o�̥u��1��
	0x01, 
	
	//baAssocJackID�r�q�A��endpoint��embedded MIDI OUT Jack��ID���C
	//�ڭ̦b�e���w�q�F�@��embedded MIDI OUT Jack�AID����3�C
	0x03,  //�YbJackID = 3��Jack
	
	/*************standard MS(MIDIStreaming) bulk data OUT endpoint descriptor****************/ 
	//bLength�r�q�Cendpoint descriptor���׬�7 byte�C
	0x07, 
	
	//bDescriptorType�r�q�Cendpoint descriptor�s����0x05�C
	0x05, 
	
	//bEndpointAddress�r�q�Cendpoint���a�}�C�ڭ̨ϥ�D12��OUT endpoint 2�C
	//D7���ܼƾڤ�V�AOUT endpoint D7��0�C�ҥHOUT endpoint 2���a�}��0x02�C
	0x02, 
	
	//bmAttributes�r�q�CD1~D0��endpoint�ǿ�������ܡC
	//��endpoint��bulk endpoint�Cbulk endpoint���s����2�C�䥦bit�O�d��0�C
	0x02, 
	
	//wMaxPacketSize�r�q�C��endpoint���̤jpacket���סCendpoint 2���̤jpacket���׬�64 byte�C
	//�`�N�Cbyte�b���C
	0x40, 
	0x00, 
	
	//bInterval�r�q�Cendpoint polling���ɶ��A���B�L�N�q�C
	0x00, 
	
	/**********class-specific MS(MIDIStreaming) bulk data endpoint descriptor********/ 
	//bLength�r�q�A��descriptor�����סC��5 byte�C
	0x05, 
	
	//bDescriptorType�r�q�A��descriptor�������A��class-specific endpoint descriptor(CS_ENDPOINT)
	0x25, 
	
	//bDescriptorSubType�r�q�A��descriptor��subtype�A��MS_GENERAL 
	0x01, 
	
	//bNumEmbMIDIJack�r�q�A��endpoint��embedded MIDI IN Jack���ƶq�A�o�̥u��1��
	0x01, 
	
	//baAssocJackID�r�q�A��endpoint��embedded MIDI IN Jack��ID���C
	//�ڭ̦b�e���w�q�F�@��embedded MIDI IN Jack�AID����1�C
	0x01  //�YbJackID = 1��Jack
	
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

//�r��"phisoner��MIDI��L" 
//8bit little endian�榡
code unsigned char ProductStringDescriptor[32]={
	32,         //��descriptor�����׬�32 byte
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
	0x4d,0x00,	//M
	0x49,0x00,	//I
	0x44,0x00,	//D
	0x49,0x00, 	//I
	0x75,0x93,  //�� 
	0xe4,0x76   //�L 
};
////////////////////////product string����////////////////////////////


//�r�ꬰ��2017-12-10����Unicode�s�X
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
	0x32, 0x00, //2
	0x2d, 0x00, //-
	0x31, 0x00, //1
	0x30, 0x00  //0
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
	Ep2InIsBusy = 0;
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
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif
	
	D12ReadEndpointLastStatus(1);            //Ū��endpoint 0 IN���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag

	if(SendLength >= DeviceDescriptor[7])    //SendLength�N��W���ǿ�ɡA�ٻݭn�ǰe����ƪ���
	{
		sendPtr += DeviceDescriptor[7];		 //�i�H��e�U���ո�ƤF
		SendLength -= DeviceDescriptor[7];   //SendLength�{�b�N��ثe�ٻݭn�ǰe����ƪ��סA�]���W�����\���ǰe��ơA�ҥH�ݭn�ǰe��ƪ������ܤ֤F
		UsbEp0SendData();					 //�z�Lendpoint 0 IN �Ӱe���
	}
	
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt�B�z���
{
	
	unsigned char Buf=0;
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif

	D12ReadEndpointLastStatus(2);            //Ū��endpoint 1 OUT���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
	D12ClearBuffer(2);						 //�M��endpoint 1 OUT��buffer
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt�B�z��� 
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
	D12ReadEndpointLastStatus(3);	          //Ū��endpoint 1 IN���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
	Ep1InIsBusy = 0;						  //�M��endpoint 1 ���лx
} 

void UsbEp2Out(void)     //endpoint 2 OUT , interrupt�B�z��� 
{
	unsigned char Buf[4];
	unsigned i,len;
	#ifdef DEBUG0
		Prints("USB endpoint 2 OUT interrupt\n");
	#endif

	D12ReadEndpointLastStatus(4);	          //Ū��endpoint 2 OUT���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
	len = D12ReadEndpointBuffer(4,4,Buf);     //Ū��endpoint 2 OUT��buffer�A�ñN���e�s��Buf�}�C��
	for(i=0;i<4;i++)
	{
		PrintHex(Buf[i]);
		Prints(" ");
	}
	Prints("\n");
	D12ClearBuffer(4);					      //�M��endpoint 2 OUT��buffer

}

void UsbEp2In(void)      //endpoint 2 IN , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 IN interrupt\n");
	#endif
	D12ReadEndpointLastStatus(5);	          //Ū��endpoint 2 IN���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
	Ep2InIsBusy = 0;						  //�M��endpoint 2 ���лx
} 