#include "UART.h"


volatile bit Sending;   //用來表示目前serial port是否有在送資料，若Sending=1代表正在送資料，Sending=0則否
code char HexTable[] = {'0','1','2','3','4','5','6','7',
                        '8','9','a','b','c','d','e','f'};

void InitUART(void)
{
	//先規劃baud rate = 2400
	TMOD &= 0x0f;    //我們接下來要設定Timer/Counter Mode Control register的高4bit
	TMOD |= 0x20;    //令timer1 工作於模式2(auto reload)
	TH1 = 0xf3;      //設定timer1
	TL1 = 0xf3;
	TR1 = 1;         //啟動timer1

	//接下來要設定serial port的模式
	SCON = 0X70;     //設定serial port為模式1
	RI = 0;          //設定Receive Interrupt flag = 0
	TI = 0;          //設定Transmit Interrupt flag = 0
	EA = 1;          //enable all interrupt
	ES = 1;          //enable ther serial port interrupt
}



void UARTPutChar(unsigned char c)	 //透過serial port傳送ㄧ個char字元
{
	Sending = 1;			         //表示要開始傳送資料了
	SBUF = c;			             //將字元c丟到serial port data buffer中，serial port會自動將SBUF中的資料送出
	while(Sending);				     //等待資料送出後，才跳出loop
}

void Prints(unsigned char*s)	     //透過serial port傳送字串(即多個字元)
{
	int i;
	for(i=0;;i++)
	{
		if(s[i] == '\0')
			return;
		UARTPutChar(s[i]);
	}
}

void PrintShortIntHex(unsigned int num) //將2byte的unsigned int整數以16進位方式透過serial port傳出
{
	int i;
	Prints("0x");
	for(i=3;i>=0;i--)
	{
		UARTPutChar(HexTable[(num>>(4*i))&0xf]);
	}
}

void PrintLongInt(unsigned long num)    //透過serial port印出整數數字
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
				len = i; //代表字串的長度
				break;
			}
			str[i] = '0'+ (num%10);
			num /= 10;
		}
        //接下來將字串倒過來
		for(i=0;i<len/2;i++)
		{
			tmp = str[i];
			str[i] = str[len - 1 - i];
			str[len - 1 - i] = tmp;
		}
		Prints(str);
	}

}

void PrintHex(unsigned char c)		    //將1byte的unsigned char字元以16進位方式透過serial port傳出
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
		RI = 0;       //只能手動清除Receive Interrupt flag
	}
	else
	{
		Sending = 0;  //只能手動清除Transmit Interrupt flag
		TI = 0;		  //將該變數設為0，代表傳送結束
	}				  
}
