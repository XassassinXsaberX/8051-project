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


void KeyBoardReset(unsigned char *Buf)
{
	char i;
	for(i=2;i<8;i++)
		Buf[i] = 0xff;
}

void SendReport(void)
{
	unsigned char Buf[8] = {0,0,0,0,0,0,0,0};
	unsigned char i=2;
	Ep1InIsBusy = 1;
	
	//Buf[0]��8��bit
	//bit 0:����Ctrl��Bbit 1:����Shift��Bbit 2:����Alt��Bbit 3:����GUI(�YWindow��)
	//bit 4:���kCtrl��Bbit 5:���kShift��Bbit 6:���kAlt��Bbit 7:���kGUI(�YWindow��)
	if(KEY & 0x01)			  //�p�G��L0�Q���U�A�h�N����U��Ctrl��
		Buf[0] |= 1<<0;	      
	if((KEY>>1) & 0x01)       //�p�G��L1�Q���U�A�h�N����U��Shift��
		Buf[0] |= 1<<1;
	if((KEY>>2) & 0X01)       //�p�G��L2�Q���U�A�h�N����U��Alt��
		Buf[0] |= 1<<2;
	if((KEY>>3) & 0X01)       //�p�G��L3�Q���U�A�h�N����U��GUI(�YWindow��
		Buf[0] |= 1<<3;	 


	if((KEY>>4) & 0X01)       //�p�G��L4�Q���U�A�h�N����U�Ʀr�p��L1 
	{
		Buf[i++] = 0x59;
	}
	if((KEY>>5) & 0X01)       //�p�G��L5�Q���U�A�h�N����U�Ʀr�p��L2
	{
		Buf[i++] = 0x5a;
	}
	if((KEY>>6) & 0X01)       //�p�G��L6�Q���U�A�h�N����U�Ʀr�p��L3
	{
		Buf[i++] = 0x5b;
	}
	if((KEY>>7) & 0X01)       //�p�G��L7�Q���U�A�h�N����U�Ʀr�p��L4
	{
		Buf[i++] = 0x5c;
	}
	if((KEY>>8) & 0X01)       //�p�G��L8�Q���U�A�h�N����U�Ʀr�p��L5
	{
		Buf[i++] = 0x5d;
	}
	if((KEY>>9) & 0X01)       //�p�G��L9�Q���U�A�h�N����U�Ʀr�p��L6
	{
		Buf[i++] = 0x5e;
	}
	if((KEY>>10) & 0X01)      //�p�G��L10�Q���U�A�h�N����U�Ʀr�p��L7
	{	
		if(i >= 8)
			KeyBoardReset(Buf);	
		else
			Buf[i++] = 0x5f;
	}
	if((KEY>>11) & 0X01)      //�p�G��L11�Q���U�A�h�N����U�Ʀr�p��L8
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x60;			
	}
	if((KEY>>12) & 0X01)      //�p�G��L12�Q���U�A�h�N����U�Ʀr�p��L9
	{	
		if(i >= 8)
			KeyBoardReset(Buf);	
		else
			Buf[i++] = 0x61;
	}
	if((KEY>>13) & 0X01)      //�p�G��L13�Q���U�A�h�N����U�Ʀr�p��L0
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x62;			
	}
	if((KEY>>14) & 0X01)      //�p�G��L14�Q���U�A�h�N����UCaps Lock
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x39;			
	}
   	if((KEY>>15) & 0X01)      //�p�G��L15�Q���U�A�h�N����UNum Lock
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x53;			
	}

	
	D12WriteEndpointBuffer(3,8,Buf);
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
				if(Ep1InIsBusy == 0)   //�p�G����endpoint 1 IN �S���ǰe��Ƶ�host
					SendReport();	   //�o�ɤ~��z�Lendpoint 1 IN �ǰereport��host 
			}
				
			while(pressKey != NOPRESS);	  //��������P�}�~�|���Xloop
			SendReport();                 //�p�G�����P�}��n�e�X����report�A�i��host�w�g�S��������U�h�F

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





