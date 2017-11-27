#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"

idata unsigned char Buffer[16]; //Ū�� endpoint 0�Ϊ��w�İ�

unsigned char *sendPtr;   //���V���U�ӭn�ǰe�����
unsigned int SendLength;  //�N���٭n�e�h��byte�����
bit SendZeroPacket;       //�Y��1�N��n�edata byte�ƥج�0��data packet�A�Y��0�h�N���ΰe

//�H�U��standard device request���U��field
unsigned char bmRequestType;
unsigned char bRequest;
unsigned int wValue;
unsigned int wIndex;
unsigned int wLength;

code unsigned char DeviceDescriptor[0x12]=  //device descriptor��18byte 
{
	//bLength�r�q�Cdevice descriptor�����׬�18(0x12)byte 
	0x12,
	
	//bDescriptorType�r�q�Cdevice descriptor���s����0x01 
	0x01,
	
	//bcdUSB�r�q�C�o�̳]�m������USB1.1�A�Y0x0110�C 
	//�ѩ�Olittle endian���c�A�ҥH�Cbyte�b�e�A�Y0x10�B0x01�C 
	0x00,
	0x02,

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
	
	//idProduct�r�q�CProduct ID�A�ѩ�O�Ĥ@�ӹ���A�ڭ̳o�̨�0x0001�C
	//�`�Nlittle endian�Ҧ��A�Cbyte���Ӧb�e�C
	0x01,
	0x00,
	
	//bcdDevice�r�q�C�ڭ̳o��USB�ƹ����Э�}�l���A�N�s��1.0���a�A�Y0x0100�C
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


void DelayXms(unsigned int t)
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<120;i++);
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
} 

void UsbEp0Out(void)     //endpoint 0 OUT , interrupt�B�z��� 
{
	char status;
	int i;

	#ifdef DEBUG0
		Prints("USB endpoint 0 OUT interrupt\n");
	#endif

	status = D12ReadEndpointLastStatus(0);	    //"Ū��endpoint 0 OUT�̫᣸��transaction�����A" ��command
	                                            //�өR�O�i�M��interrupt register���A�C�@��interrupt�����쪺interrupt flag
                                                //�NŪ���쪺 "�̫᣸��transaction�����A���" �s��status�ܼƤ�
	Prints("status=");          
	PrintHex(status);
	UARTPutChar('\n');
	if(status & 0x01)	                            //�p�G�̫᣸��transaction�O���\�ǰeor���\����
	{
		if((status>>5) & 0x01)	                    //�p�G�̫᣸��transaction��setup stage
		{
			//Prints("Setup Packet\n");
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
						if(((wValue>>8)&0xff) == 1)
						{
							#ifdef DEBUG0
								Prints("device descriptor\n");
							#endif	
							if(wLength > DeviceDescriptor[0])
								SendLength =  DeviceDescriptor[0];  //�M�w�n�e�X��byte
							else
								SendLength = wLength;
	
							sendPtr = DeviceDescriptor;			    //�M�w�n�e���Ǹ��
							
							if(SendLength%DeviceDescriptor[7] == 0)	//�M�w�̫�O�_�ݭn�edata byte�ƥج�0��data packet
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
			D12ReadEndpointBuffer(0, 16, Buffer);	//Ū�����wendpoint��buffer
			D12ClearBuffer(0);			            //�M��endpoint 0 OUT buffer
		}
	}
	
		


}


//�NsendPtr���V����ưe�X
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


void UsbEp0In(void)      //endpoint 0 IN , interrupt�B�z���
{
	unsigned char status;
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif

	
	status = D12ReadEndpointLastStatus(1); //Ū��endpoint 0 IN���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag

	Prints("status=");         
	PrintHex(status);
	UARTPutChar('\n');

	if(SendLength >= 0)
		UsbEp0SendData();
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt�B�z���
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt�B�z��� 
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
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


