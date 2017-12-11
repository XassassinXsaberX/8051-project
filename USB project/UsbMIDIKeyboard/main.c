#include <AT89X52.h>
#include <stdlib.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

code char headTable[][74] = {
"**********************************************************\r\n", 
"******             ���ӿ���USB��L \n", 
"******             AT89S52 CPU \n", 
"******               �إߤ���G",__DATE__,"\n", 
"******               �إ߮ɶ��G",__TIME__,"\r\n", 
"******               �@�̡Gphisoner\n", 
"******               �w��X�ݧڪ�github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               �Ы�K0-K15���O�i�����\n", 
"******  K0:��Ctrl��  K1:��Shift��  K2:��Alt��  K3:��GUI\n",
"******  K4:�Ʀr�p��L1  K5:�Ʀr�p��L2  K6:�Ʀr�p��L3\n", 
"******  K7:�Ʀr�p��L4  K8:�Ʀr�p��L5  K9:�Ʀr�p��L6\n",
"******  K10:�Ʀr�p��L7  K11:�Ʀr�p��L8  K12:�Ʀr�p��L9\n",
"******  K13:�Ʀr�p��L0  K14:Caps Lock  K15:Num Lock\n",  
"**********************************************************\n", 
}; 


void SendNoteOnMsg(void) 
{ 
	//4 byte���w�İ�
	unsigned char Buf[4];
	
	//Note On message�Ĥ@��byte�T�w��0x09�A�ĤG��byte��0x9n (n���q�D��)
	//�ĤT��byte��0xKK�]K�������^�A�ĥ|��byte��0xVV�]V���O�ס^�C
	
	Buf[0]=0x09; //Note On message��packet header 
	Buf[1]=0x90; //�b�q�D0�W�o�eNote On message 
	Buf[3]=0x7F; //���q�]�m���̤j 
	
	
	if((KEY>>4) & 0x01) 
	{ 
		Buf[2]=55;       //C�ժ�5 (���ﭵ����G��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
 	} 

	else if((KEY>>5) & 0X01) 
	{ 
		Buf[2]=57;       //C�ժ�6 (���ﭵ����A��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	} 
	
	else if((KEY>>6) & 0X01)
	{
		Buf[2]=60;       //C�ժ�1 (���ﭵ����C���A�Y����C)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C�C
	} 
	
	else if((KEY>>7) & 0X01) 
	{ 
		Buf[2]=62;       //C�ժ�2 (���ﭵ����D��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	} 
	
	else if((KEY>>8) & 0X01) 
	{ 
		Buf[2]=64;       //C�ժ�3 (���ﭵ����E��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	} 
	
	else if((KEY>>9) & 0X01) 
	{
		Buf[2]=67;       //C�ժ�5 (���ﭵ����G��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	} 
	
	else if((KEY>>10) & 0X01) 
	{ 
		Buf[2]=69;       //C�ժ�6 (���ﭵ����A��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	} 
	
	else if((KEY>>11) & 0X01) 
	{ 
		Buf[2]=72;       //C�ժ�1 (���ﭵ����C��)
		//�z�L IN endpoint 2 ��^4 byte�� MIDI event packet�C
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C
	}
	else
	{
		//�p�G������u�_�A�h������������
		Buf[3]=0x00; //���q�]�m��0 
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //�]�mendpoint 2 ���лx�C	
	}
	
	
} 


void main()
{
	
	unsigned int id;
	volatile unsigned interruptSource;
	int i;
	InitUART();			 //��l��serial port�]�w
	InitKeyboard();	     //��l����L�]�w
	

	for(i=0;i<sizeof(headTable)/74;i++)
	{
		Prints(headTable[i]);
	}
	id = D12ReadID();
	Prints("chip ID is ");
	PrintShortIntHex(id);
	UARTPutChar('\n');
	
	UsbDisconnect();
	UsbConnect();

	ConfigValue = 0;	  //�]�w���}�l��configuration value
   	
	
	while(1)
	{	
		KeyCanChange = 1;	      //��\�����Q���U
		if(pressKey != NOPRESS)	  //�p�G������Q���U��
		{
			if(ConfigValue)			   //�p�G�w���\�]�wconfiguration
			{
				if(Ep2InIsBusy == 0)   //�p�G����endpoint 2 IN �S���ǰe��Ƶ�host
					SendNoteOnMsg();   //�o�ɤ~��z�Lendpoint 2 IN �ǰeNote On message��host 
				while(pressKey != NOPRESS);	  //��������P�}�~�|���Xloop
				SendNoteOnMsg();              //�p�G�����P�}��n�e�X����Note On message�A�i��host�w�g�S��������U�h�F
			}
		}
		
		
		if(D12_INT == 0)   //�p�GPDIUSBD12�����o��interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//�g�J "Ū��interrupt register command"
			interruptSource = D12ReadByte();		    //Ū��interrupt register
			
			if((interruptSource>>7) & 0x01)			    //�p�G�o��USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //�p�G�o��USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)			    //�p�GUSB��endpoint 2 IN �ǰe��data packet��host��
				UsbEp2In();
			if((interruptSource>>4) & 0x01)				//�p�GUSB��endpoint 2 OUT ������Ӧ�host��data packet
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)				//�p�GUSB��endpoint 1 IN �ǰe��data packet��host��
				UsbEp1In();
			if((interruptSource>>2) & 0x01)				//�p�GUSB��endpoint 1 OUT ������Ӧ�host��data packet
				UsbEp1Out();
			if((interruptSource>>1) & 0x01)				//�p�GUSB��endpoint 0 IN �ǰe��data packet��host��
				UsbEp0In();
			if((interruptSource>>0) & 0x01)				//�p�GUSB��endpoint 0 OUT ������Ӧ�host��data packet
				UsbEp0Out();
			
				
		}
		
	}
	
}





