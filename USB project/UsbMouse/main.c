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
"******             ���ӿ���USB���� \n", 
"******             AT89S52 CPU \n", 
"******               �إߤ���G",__DATE__,"\r\n", 
"******               �إ߮ɶ��G",__TIME__,"\r\n", 
"******               �@�̡Gphisoner\n", 
"******               �w��X�ݧڪ�github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               �Ы�K0-K15���O�i�����\n", 
"******  K4�BK5:��Хk��  K6�BK7:�C�Х���  K9�BK10:�C�ФU��\n", 
"******  K13�BK14:�C�ФW��  K8�BK11:�u���U�u  K12�BK15:�u���W�u\n", 
"**********************************************************\n", 
}; 

code char led_word[] = {0x3f,0x06,0x5b,0x4f,
                        0x66,0x6d,0x7d,0x07,
						0x7f,0x6f,0x77,0x7c,
						0x39,0x5e,0x79,0x71};


void SendReport(void)
{
	unsigned char Buf[4] = {0,0,0,0};
	Ep1InIsBusy = 1;
	

	if(KEY & 0x01)			  //�p�G��L0�Q���U�A�h�N����U�ƹ��k��
		Buf[0] |= 1<<1;	      
	if((KEY>>1) & 0x01)       //�p�G��L1�Q���U�A�h�N����U�ƹ�����
		Buf[0] |= 1<<2;
	if((KEY>>2) & 0X01)       //�p�G��L2�Q���U�A�h�N����U�ƹ�����
		Buf[0] |= 1<<0;
	if((KEY>>3) & 0X01)       //�p�G��L3�Q���U�A�h�N����U�ƹ�����
		Buf[0] |= 1<<0;	 


	if((KEY>>4) & 0X01)       //�p�G��L4�Q���U�A�h�N��ƹ��k�� (�YX�b������)
		Buf[1] = 1;
	else if((KEY>>5) & 0X01)  //�p�G��L5�Q���U�A�h�N��ƹ��k�� (�YX�b������)
		Buf[1] = 1;
	else if((KEY>>6) & 0X01)  //�p�G��L6�Q���U�A�h�N��ƹ����� (�YX�b���t��)
		Buf[1] = -1;
	else if((KEY>>7) & 0X01)  //�p�G��L7�Q���U�A�h�N��ƹ����� (�YX�b���t��)
		Buf[1] = -1;

	if((KEY>>9) & 0X01)        //�p�G��L9�Q���U�A�h�N��ƹ��W�� (�YY�b������)
		Buf[2] = 1;
	else if((KEY>>10) & 0X01)  //�p�G��L10�Q���U�A�h�N��ƹ��W�� (�YY�b������)
		Buf[2] = 1;
	else if((KEY>>13) & 0X01)  //�p�G��L13�Q���U�A�h�N��ƹ��U�� (�YY�b���t��)
		Buf[2] = -1;
	else if((KEY>>14) & 0X01)  //�p�G��L14�Q���U�A�h�N��ƹ��U�� (�YY�b���t��)
		Buf[2] = -1;

	if((KEY>>12) & 0X01)       //�p�G��L12�Q���U�A�h�N��u���W�u (�Y�u���Ȭ�����)
		Buf[3] = 1;
	else if((KEY>>15) & 0X01)  //�p�G��L15�Q���U�A�h�N��u���W�u (�Y�u���Ȭ�����)
		Buf[3] = 1;
	else if((KEY>>8) & 0X01)   //�p�G��L8�Q���U�A�h�N��u���U�u (�Y�u���Ȭ��t��)
		Buf[3] = -1;
	else if((KEY>>11) & 0X01)  //�p�G��L11�Q���U�A�h�N��u���U�u (�Y�u���Ȭ��t��)
		Buf[3] = -1;
	
	D12WriteEndpointBuffer(3,4,Buf);
}


void main()
{
	
	unsigned int id;
	char led;
	volatile unsigned interruptSource;
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

	ConfigValue = 0;
   	
	
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
			//id = D12ReadID();
			//Prints("chip ID �� ");
			//PrintShortIntHex(id);
			//UARTPutChar('\n');
			//Prints("you press ");
			//PrintHex(led);
			//UARTPutChar('\n');
			//P2 = led;
			if(ConfigValue)
			{
				//PrintShortIntHex(ConfigValue);
				if(Ep1InIsBusy == 0)
					SendReport();
			}
				
			//while(pressKey != NOPRESS);

		}
		
		
		if(D12_INT == 0)   //�p�GPDIUSBD12�����o��interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//�g�J "Ū��interrupt register command"
			interruptSource = D12ReadByte();		    //Ū��interrupt register
			D12ReadByte();
			if((interruptSource>>7) & 0x01)			    //�p�G�o��USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //�p�G�o��USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)			    //
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






