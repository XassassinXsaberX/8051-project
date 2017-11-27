#include "UART.h"


volatile bit Sending;   //�ΨӪ�ܥثeserial port�O�_���b�e��ơA�YSending=1�N���b�e��ơASending=0�h�_
code char HexTable[] = {'0','1','2','3','4','5','6','7',
                        '8','9','a','b','c','d','e','f'};

void InitUART(void)
{
	//���W��baud rate = 2400
	TMOD &= 0x0f;    //�ڭ̱��U�ӭn�]�wTimer/Counter Mode Control register����4bit
	TMOD |= 0x20;    //�Otimer1 �u�@��Ҧ�2(auto reload)
	TH1 = 0xf3;      //�]�wtimer1
	TL1 = 0xf3;
	TR1 = 1;         //�Ұ�timer1

	//���U�ӭn�]�wserial port���Ҧ�
	SCON = 0X70;     //�]�wserial port���Ҧ�1
	RI = 0;          //�]�wReceive Interrupt flag = 0
	TI = 0;          //�]�wTransmit Interrupt flag = 0
	EA = 1;          //enable all interrupt
	ES = 1;          //enable ther serial port interrupt
}



void UARTPutChar(unsigned char c)	 //�z�Lserial port�ǰe����char�r��
{
	Sending = 1;			         //��ܭn�}�l�ǰe��ƤF
	SBUF = c;			             //�N�r��c���serial port data buffer���Aserial port�|�۰ʱNSBUF������ưe�X
	while(Sending);				     //���ݸ�ưe�X��A�~���Xloop
}

void Prints(unsigned char*s)	     //�z�Lserial port�ǰe�r��(�Y�h�Ӧr��)
{
	int i;
	for(i=0;;i++)
	{
		if(s[i] == '\0')
			return;
		UARTPutChar(s[i]);
	}
}

void PrintShortIntHex(unsigned int num) //�N2byte��unsigned int��ƥH16�i��覡�z�Lserial port�ǥX
{
	int i;
	Prints("0x");
	for(i=3;i>=0;i--)
	{
		UARTPutChar(HexTable[(num>>(4*i))&0xf]);
	}
}

void PrintLongInt(unsigned long num)    //�z�Lserial port�L�X��ƼƦr
{
	int i,len;
	char str[10],tmp;
	if(num == 0)
		Prints("0");
	else
	{
		for(i=0;i<10;i++)
		{
			if(num == 0)
			{
				str[i] = '\0';
				len = i; //�N��r�ꪺ����
				break;
			}
			str[i] = '0'+ (num%10);
			num /= 10;
		}
        //���U�ӱN�r��˹L��
		for(i=0;i<len/2;i++)
		{
			tmp = str[i];
			str[i] = str[len - 1 - i];
			str[len - 1 - i] = tmp;
		}
		Prints(str);
	}

}

void PrintHex(unsigned char c)		    //�N1byte��unsigned char�r���H16�i��覡�z�Lserial port�ǥX
{
	int i;
	Prints("0x");
	for(i=1;i>=0;i--)
	{
		UARTPutChar(HexTable[(c>>(4*i))&0xf]);
	}
}

void UART_ISR(void) interrupt 4	 //serial port interrupt
{
	if(RI)
	{
		RI = 0;       //�u���ʲM��Receive Interrupt flag
	}
	else
	{
		Sending = 0;  //�u���ʲM��Transmit Interrupt flag
		TI = 0;		  //�N���ܼƳ]��0�A�N��ǰe����
	}				  
}
