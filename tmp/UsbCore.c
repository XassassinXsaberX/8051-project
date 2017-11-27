#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"

idata unsigned char Buffer[16]; //讀取 endpoint 0用的緩衝區

unsigned char *sendPtr;   //指向接下來要傳送的資料
unsigned int SendLength;  //代表還要送多少byte的資料
bit SendZeroPacket;       //若為1代表要送data byte數目為0的data packet，若為0則代表不用送

//以下為standard device request的各個field
unsigned char bmRequestType;
unsigned char bRequest;
unsigned int wValue;
unsigned int wIndex;
unsigned int wLength;

code unsigned char DeviceDescriptor[0x12]=  //device descriptor為18byte 
{
	//bLength字段。device descriptor的長度為18(0x12)byte 
	0x12,
	
	//bDescriptorType字段。device descriptor的編號為0x01 
	0x01,
	
	//bcdUSB字段。這裡設置版本為USB1.1，即0x0110。 
	//由於是little endian結構，所以低byte在前，即0x10、0x01。 
	0x00,
	0x02,

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
	
	//idProduct字段。Product ID，由於是第一個實驗，我們這裡取0x0001。
	//注意little endian模式，低byte應該在前。
	0x01,
	0x00,
	
	//bcdDevice字段。我們這個USB滑鼠鼠標剛開始做，就叫它1.0版吧，即0x0100。
	//little endian模式，低byte在先。
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


void DelayXms(unsigned int t)
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<120;i++);
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
} 

void UsbEp0Out(void)     //endpoint 0 OUT , interrupt處理函數 
{
	char status;
	int i;

	#ifdef DEBUG0
		Prints("USB endpoint 0 OUT interrupt\n");
	#endif

	status = D12ReadEndpointLastStatus(0);	    //"讀取endpoint 0 OUT最後ㄧ次transaction的狀態" 的command
	                                            //該命令可清除interrupt register中，每一個interrupt對應到的interrupt flag
                                                //將讀取到的 "最後ㄧ次transaction的狀態資料" 存到status變數中
	Prints("status=");          
	PrintHex(status);
	UARTPutChar('\n');
	if(status & 0x01)	                            //如果最後ㄧ次transaction是成功傳送or成功接收
	{
		if((status>>5) & 0x01)	                    //如果最後ㄧ次transaction為setup stage
		{
			//Prints("Setup Packet\n");
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
						if(((wValue>>8)&0xff) == 1)
						{
							#ifdef DEBUG0
								Prints("device descriptor\n");
							#endif	
							if(wLength > DeviceDescriptor[0])
								SendLength =  DeviceDescriptor[0];  //決定要送幾個byte
							else
								SendLength = wLength;
	
							sendPtr = DeviceDescriptor;			    //決定要送哪些資料
							
							if(SendLength%DeviceDescriptor[7] == 0)	//決定最後是否需要送data byte數目為0的data packet
								SendZeroPacket = 1;
							else
								SendZeroPacket = 0;

							UsbEp0SendData();
						}
						else if(((wValue>>8)&0xff) == 2)
						{
							#ifdef DEBUG0
								Prints("configuration descriptor\n");
							#endif	
						}
						else if(((wValue>>8)&0xff) == 3)
						{
							#ifdef DEBUG0
								Prints("string descriptor\n");
							#endif	
						}
						else if(((wValue>>8)&0xff) == 4)
						{
							#ifdef DEBUG0
								Prints("interface descriptor\n");
							#endif	
						}
						else if(((wValue>>8)&0xff) == 5)
						{
							#ifdef DEBUG0
								Prints("endpoint descriptor\n");
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
				else				                   //reserved
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
					}
					else if(bRequest == SET_CONFIGURATION)
					{
						#ifdef DEBUG0
							Prints("SET_CONFIGURATION\n");
						#endif	
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
						Prints("USB class OUT request : \n");
					#endif
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
			//Prints("non SETUP packet\n");
			D12ReadEndpointBuffer(0, 16, Buffer);	//讀取指定endpoint的buffer
			D12ClearBuffer(0);			            //清空endpoint 0 OUT buffer
		}
	}
	
		


}


//將sendPtr指向的資料送出
void UsbEp0SendData(void)
{
	if(SendLength > DeviceDescriptor[7])
	{
		D12WriteEndpointBuffer(1,DeviceDescriptor[7],sendPtr);
		sendPtr += DeviceDescriptor[7];
		SendLength -= DeviceDescriptor[7];
	}
	else
	{
		if(SendLength != 0)
		{
			D12WriteEndpointBuffer(1,SendLength,sendPtr);
			SendLength = 0;	
		}
		
		else
		{
			if(SendZeroPacket)
			{
				Prints("SendZeroPacket\n");
				D12WriteEndpointBuffer(1,0,sendPtr);
				SendZeroPacket = 0;	
			}
				
		}
	}
}


void UsbEp0In(void)      //endpoint 0 IN , interrupt處理函數
{
	unsigned char status;
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif

	
	status = D12ReadEndpointLastStatus(1); //讀取endpoint 0 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag

	Prints("status=");         
	PrintHex(status);
	UARTPutChar('\n');

	if(SendLength >= 0)
		UsbEp0SendData();
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
} 

void UsbEp2Out(void)     //endpoint 2 OUT , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 OUT interrupt\n");
	#endif
}

void UsbEp2In(void)      //endpoint 2 IN , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 IN interrupt\n");
	#endif
} 


