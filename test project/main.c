#include <AT89X52.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

/*
serial port����(�Ǹ�ƨ�q����)
�]�wbaud rate = 1200  (1200bps)
PC�ϥ�RS232��serial port�ӱ������
*/

code char headTable[][74] = {
"**********************************************************\r\n", 
"******              ���ӿ���USB���� \r\n", 
"******                 AT89S52 CPU \r\n", 
"******               �إߤ���G",__DATE__,"  \r\n", 
"******               �إ߮ɶ��G",__TIME__,"  \n", 
"******               �@�̡Gphisoner          \n", 
"******               �w��X�ݧڪ�github \n", 
"******        https://github.com/XassassinXsaberX \n",  
"******               �Ы�K1-K16���O�i����� \n", 
"******     K1:���Х���K2:���Хk��K3:���ФW��K4:���ФU��  \n", 
"******     K5:�u���U�uK6:�u���W�uK7:���Х���K8:���Хk��  \n", 
"**********************************************************\n", 
}; 

code char led_word[] = {0x3f,0x06,0x5b,0x4f,
                        0x66,0x6d,0x7d,0x07,
						0x7f,0x6f,0x77,0x7c,
						0x39,0x5e,0x79,0x71};



void main()
{
	
	unsigned int id;
	char led;
	unsigned interruptSource;
	int i;
	InitUART();
	InitKeyboard();
	

	for(i=0;i<16;i++)
	{
		Prints(headTable[i]);
	}
	id = D12ReadID();
	Prints("chip ID is ");
	PrintShortIntHex(id);
	UARTPutChar('\n');
	
	UsbDisconnect();
	UsbConnect();

	while(1)
	{	
		P2 = 0x00;
		KeyCanChange = 1;	      //��\�����Q���U
		if(pressKey != NOPRESS)	  //�p�G������Q���U��
		{
			//�ˬdKEY�ܼơA�ӬݬݬO���ӫ���Q���U
			for(i=0;i<16;i++)
				if( (KEY>>i) & 0x01 )
					led = led_word[i];
			id = D12ReadID();
			Prints("chip ID �� ");
			PrintShortIntHex(id);
			//Prints(__DATE__);
			UARTPutChar('\n');
			Prints("you press ");
			PrintHex(led);
			UARTPutChar('\n');
			P2 = led;
			
			while(pressKey != NOPRESS);

		}
		if(D12_INT == 0)   //�p�GPDIUSBD12�����o��interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//�g�J "Ū��interrupt register command"
			interruptSource = D12ReadByte();		    //Ū��interrupt register
			if((interruptSource>>7) & 0x01)			    //�p�G�o��USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //�p�G�o��USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)
				UsbEp2In();
			if((interruptSource>>4) & 0x01)
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)
				UsbEp1In();
			if((interruptSource>>2) & 0x01)
				UsbEp1Out();
			if((interruptSource>>1) & 0x01)
				UsbEp0In();
			if((interruptSource>>0) & 0x01)
				UsbEp0Out();
			
				
		}
		
	}
}






