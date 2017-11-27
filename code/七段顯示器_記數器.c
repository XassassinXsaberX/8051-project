#include <AT89X52.h>

/*
記數器實驗
按下4x4按鈕便會記數1
*/

void delayms(unsigned int time);  // delay 1ms
char keypad(void);                // 掃描4x4鍵盤，看是否有按鍵被按下
void led_scan(unsigned long);     // 顯示八個七段顯示器
code char led_word[] = {0x3f,0x06,0x5b,0x4f,0x66,
                        0x6d,0x7d,0x07,0x7f,0x6f}; //led 0~9的字形碼


int main()
{
	unsigned long count = 0;
	char key;
	EA = 1;      // enable all interrupt 
	EX0 = 1;     // enable external interupt
	while(1)
	{	
		// 第一個迴圈是要偵測是否4x4鍵盤中有按鈕被按下
		while(1)
		{
			led_scan(count); // 八個七段顯示器要在迴圈中不斷顯示目前的記數值count
			key = keypad();
			if(key != 16) // 若有按鈕被按下時
				break;
		}
		count++;
		// 第二個迴圈是要偵測是否4x4鍵盤中有按鈕被放開
		while(1)
		{
			led_scan(count); // 八個七段顯示器要在迴圈中不斷顯示目前的記數值count
			key = keypad();
			if(key == 16) // 若有按鈕被放開時
				break;
		}
		if(count == 100000000) //八個七段顯示器最多只能顯示到99999999，所以若超過的話要歸0
			count = 0;
		
		
	}
}
	
void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
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

void led_scan(unsigned long num)
{
	int i;
	unsigned char table[8];  // 存放num的每一位數字
	
	bit valid_bit = 0;
	
	
	// 若num=2491，則table[] = {0,0,0,0,2,4,9,1}
	for(i=7;i>=0;i--)
	{
		table[i] = num%10;
		num /= 10;
	}
	
	for(i=0;i<8;i++)
	{
		P1 = i;
		P2 = led_word[table[i]];
		
		if(table[i] == 0 && valid_bit == 0 && i != 7)  // 注意若num == 0時，最後一個led就不要熄滅
			P2 = 0x00;  //該七段顯示器要熄滅		
		else if(table[i] != 0)
			valid_bit = 1;
		
		delayms(2);
	}
}

// 定義external interrupt 
void int0(void) interrupt 0
{
	unsigned int i,j,k;
	
	for(j=0;j<5;j++)
	{
		//8個七段顯示器顯示數字0顯示0.4秒
		for(k=0;k<25;k++)
			for(i=0;i<8;i++)
			{
				P1 = i;
				P2 = led_word[0];
				delayms(2);
			}
		
		//8個七段顯示器熄滅0.4秒
		for(k=0;k<25;k++)
			for(i=0;i<8;i++)
			{
				P1 = i;
				P2 = 0x00;
				delayms(2);
			}
	}
	
	//P2 = led_word[0];
	//delayms(1000);
		
}
