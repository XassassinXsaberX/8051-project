#include <AT89X52.h>

/*
serial port實驗(傳資料到電腦中)
設定baud rate = 1200  (1200bps)
PC使用RS232的serial port來接收資料
*/

char keypad(void);                //掃描4x4鍵盤，看是否有按鍵被按下
void send_char(char);             //從serial port送出字元
void send_string(char*);          //從serial port送出字串  
volatile bit Sending;             //用來表示目前serial port是否有在送資料，若Sending=1代表正在送資料，Sending=0則否
code char HexTable[] = {'0','1','2','3','4','5','6','7',
                        '8','9','a','b','c','d','e','f'};

int main()
{
	
	char key;
	//接下來要規劃baud rate
	TMOD = 0x20;     //令timer1 工作於模式2(auto reload)
	TH1 = 230;       //設定timer1
	TL1 = 230;
	TR1 = 1;         //啟動timer1
	
	//接下來要設定serial port的模式
	SCON = 0X70;     //設定serial port為模式1
	RI = 0;          //設定Receive Interrupt flag = 0
	TI = 0;          //設定Transmit Interrupt flag = 0
	EA = 1;          //enable all interrupt
	ES = 1;          //enable ther serial port interrupt
	
	while(1)
	{	
		P2 = 0xff;
		//如果有按鈕被按下
		while(1)
		{
			key = keypad();
			if(key != 16)
				break;
		}
		P2 = ~key;   //led燈顯示一下東西吧
		//傳送以下資料
		send_string("Sending data:");
		send_char(HexTable[key]);
		//如果有按鈕被放開
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
	Sending = 1;     //代表目前正在送資料了
	SBUF = c;        //待會serial port會將SBUF存放的資料送出去
	while(Sending);  //等待資料送完
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
		RI = 0;        //只能手動清除Receive Interrupt flag
	else
	{
		TI = 0;        //只能手動清除Transmit Interrupt flag
		Sending = 0;   //將該變數設為0，代表傳送結束
	}
}


