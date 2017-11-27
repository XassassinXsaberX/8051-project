#include <AT89X52.h>

/*
記時器實驗
按下4x4按鈕便會開始記時
再按下4x4按鈕便會停止記時
在按下4x4按鈕便會歸0
記時單位從0.0秒開始，以0.1秒為單位，記時範圍為0.0秒~9999999.9秒
*/

void delayms(unsigned int);               // 粗略的delay 1ms
char keypad(void);                // 掃描4x4鍵盤，看是否有按鍵被按下
void led_scan(unsigned long);     // 顯示八個七段顯示器
code char led_word[] = {0x3f,0x06,0x5b,0x4f,0x66,
                        0x6d,0x7d,0x07,0x7f,0x6f}; //led 0~9的字形碼
unsigned long count_time = 0;     // 八個七段顯示器會顯示此時間
unsigned int count=0; 
											

int main()
{
	char key;
	EA = 1;           // enable all interrupt
	ET0 = 1;          // enable timer 0 interrupt
	TMOD = 0X01;      // 把timer0工作於mode 1 
	while(1)
	{	
		count_time = 0;
		
		// 首先第一個按鈕按下是開始記時
		// 第一個迴圈是要偵測是否4x4鍵盤中有按鈕被按下
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數時間 count_time
			key = keypad();
			if(key != 16)         // 若有按鈕被按下時
				break;
		}
		// 第二個迴圈是要偵測是否4x4鍵盤中有按鈕被放開
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數值count
			key = keypad();
			if(key == 16)    // 若有按鈕被放開時
				break;
		}
		TH0 = (65536 - 50000) / 256; // 設定記數器的高位元 (8bit)
		TL0 = (65536 - 50000) % 256; // 設定記數器的低位元 (8bit)
		TR0 = 1;                     // 啟動記數器
		
		
		
		
		// 再來第二次按下按鈕是讓時間暫停
		// 第三個迴圈是要偵測是否4x4鍵盤中有按鈕被按下
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數時間 count_time
			key = keypad();
			if(key != 16)         // 若有按鈕被按下時
				break;
		}
		TR0 = 0;           // 將記數器暫停
		// 第四個迴圈是要偵測是否4x4鍵盤中有按鈕被放開
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數值count
			key = keypad();
			if(key == 16)    // 若有按鈕被放開時
				break;
		}
		
		
		// 第三次按下按鈕是讓時間歸0
		// 第五個迴圈是要偵測是否4x4鍵盤中有按鈕被按下
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數時間 count_time
			key = keypad();
			if(key != 16)         // 若有按鈕被按下時
				break;
		}
		count_time = 0;
		// 第六個迴圈是要偵測是否4x4鍵盤中有按鈕被放開
		while(1)
		{
			led_scan(count_time); // 八個七段顯示器要在迴圈中不斷顯示目前的記數值count
			key = keypad();
			if(key == 16)    // 若有按鈕被放開時
				break;
		}
		
		
		
		
		if(count_time == 100000000) //八個七段顯示器最多只能顯示到99999999，所以若超過的話要歸0
			count_time = 0;
		
		
	}
}
	
void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

void timer0(void) interrupt 1
{
	//每50ms就會執行此interrupt service routine
	
	count++;
	if(count == 2)   // 若經過0.1秒
	{
		count = 0;
		count_time++;  // 給八個七段顯示器顯示的時間值
	}
	if(count_time == 100000000)  // 八個七段顯示器最多只能顯示到9999999.9
		count_time = 0;
	
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
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
	char table[8];
	bit valid_bit=0;   // 若valid_bit = 0，則在一般情況下，七段顯示器應該熄滅
	int i;
	for(i=7;i>=0;i--)
	{
		table[i] = num%10;
		num/=10;
	}
  
	for(i=0;i<8;i++)
	{
		P1 = i;
		if(table[i] != 0 || i == 6)
				valid_bit = 1;
		if(table[i] == 0 && valid_bit == 0 && i < 6)    // 七段顯示器要熄滅的情況
			P2 = 0x00;
		else                                            // 七段顯示器要顯示數字的情況
			P2 = led_word[table[i]];
		
		
		if(i == 6)
			P2_7 = 1;   // 從右數過來第二個數字要印出小數點
		delayms(2);
	}
}

