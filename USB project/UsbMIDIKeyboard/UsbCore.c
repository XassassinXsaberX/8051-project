#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"
#include <INTRINS.h>

idata unsigned char Buffer[16]; //讀取 endpoint 0用的緩衝區

unsigned char *sendPtr;   //指向接下來要傳送的資料
unsigned int SendLength;  //代表還要送多少byte的資料
bit SendZeroPacket;       //若為1代表要送data byte數目為0的data packet，若為0則代表不用送
bit Ep1InIsBusy;
bit Ep2InIsBusy;
unsigned char ConfigValue;



code unsigned char DeviceDescriptor[0x12]=  //device descriptor為18byte 
{
	//bLength字段。device descriptor的長度為18(0x12)byte 
	0x12,
	
	//bDescriptorType字段。device descriptor的編號為0x01 
	0x01,
	
	//bcdUSB字段。這裡設置版本為USB1.1，即0x0110。 
	//由於是little endian結構，所以低byte在前，即0x10、0x01。 
	0x10,
	0x01,

	//bDeviceClass字段。我們不在device descriptor中定義device class，
	//而在interface descriptor中定義device class，所以該字段的值為0。
	0x00,
	
	//bDeviceSubClass字段。bDeviceClass字段為0時，該字段也為0。
	0x00,
	
	//bDeviceProtocol字段。bDeviceClass字段為0時，該字段也為0。
	0x00,
	
	//bMaxPacketSize0字段。PDIUSBD12的endpoint 0大小的16byte。
	0x10,
	
	//idVender字段。Vendor ID，我們這裡取0x8888，僅供實驗用。
	//實際產品不能隨便使用Vendor ID，必須跟USB協會申請Vendor ID。
	//注意little endian模式，低byte在前。
	0x88,
	0x88,
	
	//idProduct字段。Product ID，由於是第八個實驗，我們這裡取0x0008。
	//注意little endian模式，低byte應該在前。
	0x08,
	0x00,
	
	//bcdDevice字段。我們這個USB MIDI鍵盤剛開始做，就叫它1.0版吧，即0x0100。
	//little endian模式，低字節在先。
	0x00,
	0x01,
	
	//iManufacturer字段。vendor string的index，為了方便記憶和管理，
	//string index就從1開始吧。
	0x01,
	
	//iProduct字段。produce sting的index。剛剛用了1，這裡就取2吧。
	//注意string index不要使用相同的值。
	0x02,
	
	//iSerialNumber字段。device's serial number string的index。
	//這裡取3就可以了。
	0x03,
	
	///bNumConfigurations字段。該設備所具有的configuration數。
	//我們只需要一種configuration就行了，因此該值設置為1。
	0x01
};


//USB配置描述符集合的定義
//configuration descriptor總長度為9+9+9+9+7+6+6+9+9+7+5+7+5 byte
code unsigned char ConfigurationDescriptor[9+9+9+9+7+6+6+9+9+7+5+7+5]=
{
 	/***************configuration descriptor***********************/
	//bLength字段。configuration descriptor的長度為9byte。
	0x09,
	
	//bDescriptorType字段。configuration descriptor編號為0x02。
	0x02,
	
	//wTotalLength字段。配置描述符集合的總長度，
 	//包括configuration descriptor本身、interface descriptor、class descriptor、endpoint descriptor等。
	sizeof(ConfigurationDescriptor)&0xFF,      //低byte
	(sizeof(ConfigurationDescriptor)>>8)&0xFF, //高byte 
	
	//bNumInterfaces字段。該configuration包含的interface數，有兩個interface。
	0x02,
	
	//bConfiguration字段。該configuration的值為1。
	0x01,
	
	//iConfigurationz字段，該configuration的string index。這裡沒有，為0。
	0x00,
	
	//bmAttributes字段，該設備的屬性。由於我們的板子是bus供電的，
 	//並且我們不想實現remote wakeup的功能，所以該字段的值為0x80。
	0x80,
	
	//bMaxPower字段，該設備需要的最大電流量。由於我們的板子
 	//需要的電流不到100mA，因此我們這裡設置為100mA。由於每單位
 	//電流為2mA，所以這裡設置為50(0x32)。
	0x32,
	
	/**********************************************************************/
	/*****************audio control interface descriptor*******************/ 
	//bLength字段。interface descriptor的長度為9 byte。
	0x09, 
	
	//bDescriptorType字段。interface descriptor的編號為0x04。
	0x04, 
	
	//bInterfaceNumber字段。該interface的編號，第一個interface，編號為0。
	0x00, 
	
	//bAlternateSetting字段。該interface的備用編號，為0。
	0x00, 
	
	//bNumEndpoints字段。non-zero endpoint的數目。該interface沒有endpoint
	0x00,
	
	//bInterfaceClass字段。該interface所使用的class。audio interface class的代碼為0x01。
	0x01,
	
	//bInterfaceSubClass字段。該interface所使用的subclass。audio control interface的subclass代碼為0x01。
	0x01, 
	
	//bInterfaceProtocol字段。沒有使用協議。
	0x00, 
	
	//iConfiguration字段。該interface的string descriptor index。這裡沒有，為0。
	0x00, 
	
	
	/*************class-specific AC (Audio Control) interface descriptor**********/ 
	//bLength字段，該descriptor的長度。為9 byte。
	0x09, 
	
	//bDescriptorType字段，descriptor的類型。編號為0x24，(Class-Specific) CS_INTERFACE_DESCRIPTOR。
	0x24, 
	
	//bDescriptorSubtype字段，descriptor subtype。編號為0x01，HEADER。(header subtype)
	0x01, 
	
	//bcdADC字段，協議版本。這裡為1.0版。
	0x00, 
	0x01, 
	
	//wTotalLength字段，class-specific descriptor的總長度。這裡為9 byte。
	0x09, 
	0x00, 
	
	//bInCollection字段，stream interface的數量。這裡僅有一個。
	0x01, 
	
	//baInterfaceNr字段，屬於此interface的stream interface編號。MIDIStreaming interface 屬於此audio control interface
	0x01,  //即bInterfaceNumber = 1 的interface屬於此audio control interface
	/***************************************************************************/

	/***************************************************************************/	
	/*****************MS (MIDIStreaming) interface descriptor*******************/ 
	//bLength字段。interface descriptor的長度為9 byte。
	0x09, 
	
	//bDescriptorType字段。interface descriptor的編號為0x04。
	0x04, 
	
	//bInterfaceNumber字段。該interface的編號，第二個interface，編號為1。
	0x01, 
	
	//bAlternateSetting字段。該interface的備用編號，為0。
	0x00, 
	
	//bNumEndpoints字段。non-zero endpoint的數目。MIDIStreaming interface使用一對bulk OUT/IN endpoint
	0x02, 
	
	//bInterfaceClass字段。該interface所使用的class。audio interface class的代碼為0x01。
	0x01, 
	
	//bInterfaceSubClass字段。該interface所使用的subclass。MIDIStreaming interface的subclass代碼為0x03。
	0x03, 
	
	//bInterfaceProtocol字段。沒有使用協議。
	0x00, 
	
	//iConfiguration字段。該interface的string descriptor index。這裡沒有，為0。
	0x00, 
	
	/**********class-specific MS (MIDIStreaming) interface descriptor*****/
	/*******header descriptor********/ 
	//bLength字段。該descriptor的長度，7 byte。
	0x07,
	
	//bDescriptorType字段。該descriptor的類型，為(Class-Specific) CS_INTERFACE。
	0x24, 
	
	//bDescriptorSubtype字段。descriptor subtype，為MS_HEADER (header subtype) 
	0x01, 
	
	//bcdMSC字段。該MIDIStreaming Class所使用的協議版本，為1.0 
	0x00, 
	0x01, 
	
	//wTotalLengthz字段。整個class-specific MIDIStreaming interface descriptor總長度
	0x25, 
	0x00, 
	
	/**********embedded IN Jack descriptor********/ 
	//bLength字段。該descriptor的長度，為6 byte。
	0x06, 
	
	//bDescriptorType字段。該descriptor的類型，為(Class-Specific) CS_INTERFACE。
	0x24, 
	
	//bDescriptorSubtype字段。descriptor subtype，為MIDI_IN_JACK 
	0x02, 
	
	//bJackType字段。該Jack的類型，為embedded
	0x01, 
	
	//bJackID字段。該Jack的唯一ID，這裡取值1 
	0x01, 
	
	//iJack字段。該Jack的string descriptor index，這裡沒有，為0 
	0x00,
	
	/**********external IN Jack descriptor********/ 
	//bLength字段。該descriptor的長度，為6 byte。
	0x06, 
	
	//bDescriptorType字段。該descriptor的類型，為(Class-Specific) CS_INTERFACE。
	0x24, 
	
	//bDescriptorSubtype字段。descriptor subtype類，為MIDI_IN_JACK 
	0x02, 
	
	//bJackType字段。該Jack的類型，為external
	0x02, 
	
	//bJackID字段。該Jack的唯一ID，這裡取值2 
	0x02, 
	
	//iJack字段。該Jack的string descriptor index，這裡沒有，為0 
	0x00, 
	
	/**********embedded OUT Jack descriptor********/ 
	//bLength字段。該descriptor的長度，為 9byte。
	0x09, 
	
	//bDescriptorType字段。該descriptor的類型，為(Class-Specific) CS_INTERFACE。
	0x24, 
	
	//bDescriptorSubtype字段。descriptor subtype，為MIDI_OUT_JACK 
	0x03, 
	
	//bJackType字段。該Jack的類型，為embedded
	0x01, 
	
	//bJackID字段。該Jack的唯一ID，這裡取值3 
	0x03,
	
	//bNrInputPins字段。該OUT Jack的input pin數。這裡僅有一個。
	0x01, 
	
	//baSourceID字段。連接到該Jack input pin的IN Jack的ID，選擇為external IN Jack 2 
	0x02,  //指的bJackID為0x02的Jack
	
	//BaSourcePin字段。external IN Jack連接在該Jack的input pin 1 上
	0x01, 
	
	//iJack字段。該Jack的string descriptor index，這裡沒有，為0 
	0x00, 
	
	/**********external OUT Jack descriptor********/ 
	//bLength字段。該descriptor的長度，為9 byte。
	0x09, 
	
	//bDescriptorType字段。該descriptor的類型，為(Class-Specific) CS_INTERFACE。
	0x24, 
	
	//bDescriptorSubtype字段。descriptor subtype，為MIDI_OUT_JACK 
	0x03, 
	
	//bJackType字段。該Jack的類型，為external
	0x02, 
	
	//bJackID字段。該Jack的唯一ID，這裡取值4 
	0x04, 
	
	//bNrInputPins字段。該OUT Jack的input pin數。這裡僅有一個。
	0x01, 
	
	//baSourceID字段。連接到該Jack input pin的IN Jack的ID，選擇為embedded IN Jack 1
	0x01,  //指的bJackID為0x01的Jack
	
	//BaSourcePin字段。embedded IN Jack連接在該Jack的input pin 1 上 
	0x01,
	
	//iJack字段。該Jack的string descriptor index，這裡沒有，為0 
	0x00, 
	/****************************************************************************************/

	/****************************************************************************************/	
	/*************standard MS(MIDIStreaming) bulk data IN endpoint descriptor****************/ 
	//bLength字段。endpoint descriptor長度為7 byte。
	0x07, 
	
	//bDescriptorType字段。endpoint descriptor編號為0x05。
	0x05, 
	
	//bEndpointAddress字段。endpoint的地址。我們使用D12的IN endpoint 2。
	//D7位表示數據方向，IN endpoint D7為1。所以IN endpoint 2的地址為0x82。
	0x82, 
	
	//bmAttributes字段。D1~D0為endpoint傳輸類型選擇。
	//該endpoint為bulk endpoint。bulk endpoint的編號為2。其它bit保留為0。
	0x02, 
	
	//wMaxPacketSize字段。該endpoint的最大packet長度。endpoint 2的最大packet長度為64 byte。
	//注意低byte在先。
	0x40, 
	0x00, 
	
	//bInterval字段。endpoint polling的時間，此處無意義。
	0x00, 
	
	/**********class-specific MS(MIDIStreaming) bulk data endpoint descriptor********/ 
	//bLength字段，該descriptor的長度。為5 byte。
	0x05, 
	
	//bDescriptorType字段，該descriptor的類型，為class-specific endpoint descriptor ((Class-Specific) CS_ENDPOINT)
	0x25, 
	
	//bDescriptorSubType字段，該descriptor的subtype，為(MIDIStreaming) MS_GENERAL 
	0x01, 
	
	//bNumEmbMIDIJack字段，該endpoint的embedded MIDI OUT Jack的數量，這裡只有1個
	0x01, 
	
	//baAssocJackID字段，該endpoint的embedded MIDI OUT Jack的ID號。
	//我們在前面定義了一個embedded MIDI OUT Jack，ID號為3。
	0x03,  //即bJackID = 3的Jack
	
	/*************standard MS(MIDIStreaming) bulk data OUT endpoint descriptor****************/ 
	//bLength字段。endpoint descriptor長度為7 byte。
	0x07, 
	
	//bDescriptorType字段。endpoint descriptor編號為0x05。
	0x05, 
	
	//bEndpointAddress字段。endpoint的地址。我們使用D12的OUT endpoint 2。
	//D7位表示數據方向，OUT endpoint D7為0。所以OUT endpoint 2的地址為0x02。
	0x02, 
	
	//bmAttributes字段。D1~D0為endpoint傳輸類型選擇。
	//該endpoint為bulk endpoint。bulk endpoint的編號為2。其它bit保留為0。
	0x02, 
	
	//wMaxPacketSize字段。該endpoint的最大packet長度。endpoint 2的最大packet長度為64 byte。
	//注意低byte在先。
	0x40, 
	0x00, 
	
	//bInterval字段。endpoint polling的時間，此處無意義。
	0x00, 
	
	/**********class-specific MS(MIDIStreaming) bulk data endpoint descriptor********/ 
	//bLength字段，該descriptor的長度。為5 byte。
	0x05, 
	
	//bDescriptorType字段，該descriptor的類型，為class-specific endpoint descriptor(CS_ENDPOINT)
	0x25, 
	
	//bDescriptorSubType字段，該descriptor的subtype，為MS_GENERAL 
	0x01, 
	
	//bNumEmbMIDIJack字段，該endpoint的embedded MIDI IN Jack的數量，這裡只有1個
	0x01, 
	
	//baAssocJackID字段，該endpoint的embedded MIDI IN Jack的ID號。
	//我們在前面定義了一個embedded MIDI IN Jack，ID號為1。
	0x01  //即bJackID = 1的Jack
	
};
////////////////////////配置描述符集合完畢//////////////////////////

/************************Language ID的定義********************/
code unsigned char LanguageId[4]=
{
	0x04,     //本descriptor的長度
	0x03,     //descriptor type為string descriptor
	//0x0409為美式英語的ID
	0x09,
	0x04
};
////////////////////////Language ID完畢//////////////////////////////////

//可參考此網站的unicode轉換
//https://www.chineseconverter.com/zh-tw/convert/unicode 

//字串為"歡迎參觀我的github:XassassinXsaberX" 
//8bit的little endian格式 
code unsigned char ManufacturerStringDescriptor[60]={
	60,         //該descriptor的長度為60 byte
	0x03,       //string descriptor的類型編碼為0x03
	0x61,0xb6,  //歡
	0xce,0x8f,  //迎 
	0xc3,0x53,  //參 
	0xc0,0x89,  //觀 
	0x11,0x62,  //我 
	0x84,0x76,  //的 
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
/////////////////////////vendor string結束/////////////////////////////

//字串"phisoner的MIDI鍵盤" 
//8bit little endian格式
code unsigned char ProductStringDescriptor[32]={
	32,         //該descriptor的長度為32 byte
	0x03,       //string descriptor的類型編碼為0x03
	0x70,0x00,  //p
	0x68,0x00,  //h
	0x69,0x00,  //i
	0x73,0x00,  //s
	0x6f,0x00,  //o
	0x6e,0x00,  //n
	0x65,0x00,  //e
	0x72,0x00,  //r
	0x84,0x76,  //的
	0x4d,0x00,	//M
	0x49,0x00,	//I
	0x44,0x00,	//D
	0x49,0x00, 	//I
	0x75,0x93,  //鍵 
	0xe4,0x76   //盤 
};
////////////////////////product string結束////////////////////////////


//字串為“2017-12-10”的Unicode編碼
//8 bit的little endian格式
code unsigned char SerialNumberStringDescriptor[22]={
	22,         //該descriptor的長度為22 byte
	0x03,       //string descriptor的類型編碼為0x03
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
//////////////////////device's serial number string結束/////////////////////////


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

void UsbDisconnect(void) //USB斷開連接 , interrupt處理函數
{
 	#ifdef DEBUG0
		Prints("USB disconnect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);		//寫入Set Mode command到USB晶片中 (該command需要再輸入2byte資料)
	D12WriteByte(0x06);		            //寫入第一個byte的資料到USB晶片中
	D12WriteByte(0x47);				    //寫入第二個byte的資料到USB晶片中
	DelayXms(1000);                     //delay 1ms (使主機確認設備已經斷開連結)
}

void UsbConnect(void)    //USB連接 , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB connect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);	  	//寫入Set Mode command到USB晶片中 (該command需要再輸入2byte資料)
	D12WriteByte(0x16);		            //寫入第一個byte的資料到USB晶片中
	D12WriteByte(0x47);				    //寫入第二個byte的資料到USB晶片中
}

void UsbBusSuspend(void) //bus suspend , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB Bus Suspend\n");
	#endif
}

void UsbBusReset(void)   //bus reset , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB Bus Reset\n");
	#endif
	Ep1InIsBusy = 0;
	Ep2InIsBusy = 0;
} 

void UsbEp0Out(void)     //endpoint 0 OUT , interrupt處理函數 
{
	char status;
	int i;

	//以下為standard device request的各個field
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned int wValue;
	unsigned int wIndex;
	unsigned int wLength;

	#ifdef DEBUG0
		Prints("USB endpoint 0 OUT interrupt\n");	
	#endif

	status = D12ReadEndpointLastStatus(0);	    //"讀取endpoint 0 OUT最後ㄧ次transaction的狀態" 的command
	                                            //該命令可清除interrupt register中，每一個interrupt對應到的interrupt flag
                                                //將讀取到的 "最後ㄧ次transaction的狀態資料" 存到status變數中	
									 
	if((status>>5) & 0x01)	                    //如果最後ㄧ次transaction為setup stage
	{
		for(i=0;i<16;i++)
			Buffer[i] = 0;
		D12ReadEndpointBuffer(0, 16, Buffer);	//讀取指定endpoint的buffer
		D12AcknowledgeSetup();	                //Acknowledge Setup
		D12ClearBuffer(0);			            //清空endpoint 0 OUT的buffer
		
		
		//接下來決定USB standard device request的各個field
		//收到Buffer[0] Buffer[1] Buffer[2] Buffer[3] Buffer[4] Buffer[5] Buffer[6] Buffer[7]
		bmRequestType = Buffer[0];
		bRequest = Buffer[1];
		wValue = Buffer[3];	     	//採little endian格式，所以wValue為Buffer[3] Buffer[2]
		wValue <<= 8;
		wValue |= Buffer[2]; 
		wIndex = Buffer[5];		    //採little endian格式，所以wIndex為Buffer[5] Buffer[4]
		wIndex <<= 8;
		wIndex |= Buffer[4]; 
		wLength = Buffer[7];		//採little endian格式，所以wLength為Buffer[7] Buffer[6]
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
						if(wLength > DeviceDescriptor[0])		//決定要送幾個byte
							SendLength =  DeviceDescriptor[0];  
						else
							SendLength = wLength;

						sendPtr = DeviceDescriptor;			    //決定要送哪些資料

						UsbEp0SendData();                       //透過endpoint 0 IN送出資料
					}
					else if(((wValue>>8)&0xff) == CONFIGURATION_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("configuration descriptor\n");
						#endif	
						if(wLength > sizeof(ConfigurationDescriptor) )		//決定要送幾個byte
							SendLength =  sizeof(ConfigurationDescriptor);  
						else
							SendLength = wLength;

						sendPtr = ConfigurationDescriptor;		         	//決定要送哪些資料

						UsbEp0SendData();		                            //透過endpoint 0 IN送出資料

					}
					else if(((wValue>>8)&0xff) == STRING_DESCRIPTOR)
					{
						
						#ifdef DEBUG0
							Prints("string descriptor\n");
						#endif	
						
						if((wValue & 0xff) == 0)
						{
							//Prints("Index 0\n");				   //決定要送幾個byte
							if(wLength > sizeof(LanguageId) )
								SendLength =  sizeof(LanguageId);  
							else
								SendLength = wLength;
							
							sendPtr = LanguageId;			       //決定要送哪些資料	
						}
						else if((wValue & 0xff) == 1)
						{
							//Prints("Index 1\n");
							if(wLength > sizeof(ManufacturerStringDescriptor) )		 //決定要送幾個byte
								SendLength =  sizeof(ManufacturerStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ManufacturerStringDescriptor;			         //決定要送哪些資料
						}
						else if((wValue & 0xff) == 2)
						{
							//Prints("Index 2\n");
							if(wLength > sizeof(ProductStringDescriptor) )		//決定要送幾個byte
								SendLength =  sizeof(ProductStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ProductStringDescriptor;		            //決定要送哪些資料
						
						}
						else if((wValue & 0xff) == 3)
						{
							//Prints("Index 3\n");
							if(wLength > sizeof(SerialNumberStringDescriptor) )		 //決定要送幾個byte
								SendLength =  sizeof(SerialNumberStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = SerialNumberStringDescriptor;			         //決定要送哪些資料
						}
						else
						{
							Prints("Index error\n");
						}								                          //透過endpoint 0 IN送出資料
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
					//當接收到此standard request時，目的是要為此USB晶片設置host回傳給他的device address
					D12WriteCommand(D12_SET_ADDRESS_ENABLE);
					//此standard request中 wValue的低 7 bit值為device address
					D12WriteByte(0x80 | (wValue & 0x7f));		//第7 bit設為1代表enable the function(function在USB中指的就像是設備)

					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
					SendLength = 0;
					UsbEp0SendData();
				}
				else if(bRequest == SET_CONFIGURATION)
				{
					#ifdef DEBUG0
						Prints("SET_CONFIGURATION\n");
					#endif	
				   
				    //當收到此standard request時，目的是確定此configuration是否正確，若正確的話就enable all non-zero endpoint
					//所以接下來要寫入Set Endpoint Enable command，目的是要enable或disable non-zero endpoint
					D12WriteCommand(D12_SET_ENDPOINT_ENABLE);
					ConfigValue = wValue & 0xff; //此standard request中 wValue的低byte值為configuration 

					//若此configuration的值非0時，代表為正確的configuration (可參考configuration descriptor中的bConfiguration，我們把configuration value設為1)
					//此時才能enable non-zero endpoint，否則disable non-zero endpoint
					if(ConfigValue)             
						D12WriteByte(0x01);	  //enable non-zero endpoint
					else
						D12WriteByte(0x00);   //disable non-zero endpoint

					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
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
					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
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
		D12ReadEndpointBuffer(0, 16, Buffer);	//讀取指定endpoint的buffer
		D12ClearBuffer(0);			            //清空endpoint 0 OUT buffer
	}

}


//將sendPtr指向的資料送出
void UsbEp0SendData(void)
{
	//SendLength變數代表目前還有幾個byte的資料需要傳輸
	//DeviceDescriptor[7]代表endpoint 0 IN 的buffer容量 (單位為byte)
	if(SendLength >= DeviceDescriptor[7])
		D12WriteEndpointBuffer(1,DeviceDescriptor[7],sendPtr);	
	else
		D12WriteEndpointBuffer(1,SendLength,sendPtr);

}


void UsbEp0In(void)      //endpoint 0 IN , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif
	
	D12ReadEndpointLastStatus(1);            //讀取endpoint 0 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag

	if(SendLength >= DeviceDescriptor[7])    //SendLength代表上次傳輸時，還需要傳送的資料長度
	{
		sendPtr += DeviceDescriptor[7];		 //可以串送下ㄧ組資料了
		SendLength -= DeviceDescriptor[7];   //SendLength現在代表目前還需要傳送的資料長度，因為上次成功的傳送資料，所以需要傳送資料的長度變少了
		UsbEp0SendData();					 //透過endpoint 0 IN 來送資料
	}
	
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt處理函數
{
	
	unsigned char Buf=0;
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif

	D12ReadEndpointLastStatus(2);            //讀取endpoint 1 OUT中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
	D12ClearBuffer(2);						 //清空endpoint 1 OUT的buffer
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
	D12ReadEndpointLastStatus(3);	          //讀取endpoint 1 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
	Ep1InIsBusy = 0;						  //清除endpoint 1 忙標誌
} 

void UsbEp2Out(void)     //endpoint 2 OUT , interrupt處理函數 
{
	unsigned char Buf[4];
	unsigned i,len;
	#ifdef DEBUG0
		Prints("USB endpoint 2 OUT interrupt\n");
	#endif

	D12ReadEndpointLastStatus(4);	          //讀取endpoint 2 OUT中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
	len = D12ReadEndpointBuffer(4,4,Buf);     //讀取endpoint 2 OUT的buffer，並將內容存到Buf陣列中
	for(i=0;i<4;i++)
	{
		PrintHex(Buf[i]);
		Prints(" ");
	}
	Prints("\n");
	D12ClearBuffer(4);					      //清空endpoint 2 OUT的buffer

}

void UsbEp2In(void)      //endpoint 2 IN , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 IN interrupt\n");
	#endif
	D12ReadEndpointLastStatus(5);	          //讀取endpoint 2 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
	Ep2InIsBusy = 0;						  //清除endpoint 2 忙標誌
} 