#include <AT89X52.h>
#define SP P3_4
char keypad(void);

code char table[] = {0xf4,0x2a,0xf4,0xd7,
                     0xf6,0x09,0xf7,0x1f,
                     0xf8,0x18,0xf8,0x8c,
                     0xf9,0x5c,0xfa,0x15,
                     0xfa,0x67,0xfb,0x04,
                     0xfb,0x90,0xfc,0x0c,
                     0xfc,0x42,0xfc,0xac,
                     0xfd,0x09,0xfd,0x36};
char high,low;
										 
										 
int main()
{
	char key;
	EA = 1;   // enable all interrupt
	ET0 = 1;  // enable timer 0 interrupt
	while(1)
	{
		//首先偵測按鈕是否被按下
		while(1)
		{
			key = keypad();
			if(key != 16)
				break;
		}
		high = key*2;
		low = high+1;
		TH0 = table[high];
		TL0 = table[low];
		TR0 = 1;              // start timer 0
		
		//接下來偵測按鈕是否放開
		while(1)
		{
			key = keypad();
			if(key == 16)
				break;
		}
		TR0 = 0;               // stop timer 0
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

void timer0_int(void) interrupt 1
{
	TH0 = table[high];
	TL0 = table[low];
	SP = !SP;
}