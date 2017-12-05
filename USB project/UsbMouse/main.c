#include <AT89X52.h>
#include <stdlib.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

code char headTable[][74] = {
"**********************************************************\r\n", 
"******             ���ӿ���USB�ƹ� \n", 
"******             AT89S52 CPU \n", 
"******               �إߤ���G",__DATE__,"\n", 
"******               �إ߮ɶ��G",__TIME__,"\r\n", 
"******               �@�̡Gphisoner\n", 
"******               �w��X�ݧڪ�github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               �Ы�K0-K15���O�i�����\n", 
"******  K0:�ƹ��k��  K1:�ƹ�����  K2�BK3:�ƹ�����\n",
"******  K4�BK5:��Хk��  K6�BK7:�C�Х���  K9�BK10:�C�ФU��\n", 
"******  K13�BK14:�C�ФW��  K8�BK11:�u���U�u  K12�BK15:�u���W�u\n", 
"**********************************************************\n", 
}; 


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
	volatile unsigned interruptSource;
	int i;
	InitUART();			   //��l��serial port�]�w
	InitKeyboard();	       //��l��4x4��L�]�w
	
   	//�z�Lserial port�L�X����ID
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

	ConfigValue = 0;			  //�]�w���}�l��configuration value
   	
	
	while(1)
	{	
		P2 = 0x00;
		KeyCanChange = 1;	      //��\�����Q���U
		if(pressKey != NOPRESS)	  //�p�G������Q���U��
		{
			if(ConfigValue)	      //�p�G�w���\�]�wconfiguration
			{
				if(Ep1InIsBusy == 0)	   //�p�G����endpoint 1 IN �S���ǰe��Ƶ�host
					SendReport();		   //�o�ɤ~��z�Lendpoint 1 IN �ǰereport��host
			}
				
			//while(pressKey != NOPRESS);

		}
		
		
		if(D12_INT == 0)   //�p�GPDIUSBD12�����o��interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//�g�J "Ū��interrupt register command"
			interruptSource = D12ReadByte();		    //Ū��interrupt register���Ĥ@��byte
			
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






