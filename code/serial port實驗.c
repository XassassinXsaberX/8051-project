#include <AT89X52.h>

/*
serial port����(�Ǹ�ƨ�q����)
�]�wbaud rate = 1200  (1200bps)
PC�ϥ�RS232��serial port�ӱ������
*/

char keypad(void);                //���y4x4��L�A�ݬO�_������Q���U
void send_char(char);             //�qserial port�e�X�r��
void send_string(char*);          //�qserial port�e�X�r��  
volatile bit Sending;             //�ΨӪ�ܥثeserial port�O�_���b�e��ơA�YSending=1�N���b�e��ơASending=0�h�_
code char HexTable[] = {'0','1','2','3','4','5','6','7',
                        '8','9','a','b','c','d','e','f'};

int main()
{
	
	char key;
	//���U�ӭn�W��baud rate
	TMOD = 0x20;     //�Otimer1 �u�@��Ҧ�2(auto reload)
	TH1 = 230;       //�]�wtimer1
	TL1 = 230;
	TR1 = 1;         //�Ұ�timer1
	
	//���U�ӭn�]�wserial port���Ҧ�
	SCON = 0X70;     //�]�wserial port���Ҧ�1
	RI = 0;          //�]�wReceive Interrupt flag = 0
	TI = 0;          //�]�wTransmit Interrupt flag = 0
	EA = 1;          //enable all interrupt
	ES = 1;          //enable ther serial port interrupt
	
	while(1)
	{	
		P2 = 0xff;
		//�p�G�����s�Q���U
		while(1)
		{
			key = keypad();
			if(key != 16)
				break;
		}
		P2 = ~key;   //led�O��ܤ@�U�F��a
		//�ǰe�H�U���
		send_string("Sending data:");
		send_char(HexTable[key]);
		//�p�G�����s�Q��}
		while(1)
		{
			key = keypad();
			if(key == 16)
				break;
		}	
	}
}

char keypad(void)
{
	char key = 16;
	
	P0 = 0xef;
	if(P0_0 == 0) key = 0;
	else if(P0_1 == 0) key = 1;
	else if(P0_2 == 0) key = 2;
	else if(P0_3 == 0) key = 3;
	
	P0 = 0xdf;
	if(P0_0 == 0) key = 4;
	else if(P0_1 == 0) key = 5;
	else if(P0_2 == 0) key = 6;
	else if(P0_3 == 0) key = 7;
	
	P0 = 0xbf;
	if(P0_0 == 0) key = 8;
	else if(P0_1 == 0) key = 9;
	else if(P0_2 == 0) key = 10;
	else if(P0_3 == 0) key = 11;
	
	P0 = 0x7f;
	if(P0_0 == 0) key = 12;
	else if(P0_1 == 0) key = 13;
	else if(P0_2 == 0) key = 14;
	else if(P0_3 == 0) key = 15;
	
	return key;
}

void send_char(char c)
{
	Sending = 1;     //�N��ثe���b�e��ƤF
	SBUF = c;        //�ݷ|serial port�|�NSBUF�s�񪺸�ưe�X�h
	while(Sending);  //���ݸ�ưe��
}

void send_string(char*s)
{
	while(1)
	{
		if(*s == '\0')
				break;
		send_char(*s++);
		
	}
}

void serial_port_int(void) interrupt 4
{
	if(RI)
		RI = 0;        //�u���ʲM��Receive Interrupt flag
	else
	{
		TI = 0;        //�u���ʲM��Transmit Interrupt flag
		Sending = 0;   //�N���ܼƳ]��0�A�N��ǰe����
	}
}


