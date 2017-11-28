#include "key.h"

unsigned int KEY;			   //2 byte的變數，每一個bit代表哪個鍵盤被按下(若被按下時該bit為1，否則為0)
char pressKey = NOPRESS;       //最後確定是按哪個key	   (防止機械彈跳現象)
char currentKey = NOPRESS;     //目前是偵測到按哪個key
char previousKey = NOPRESS;    //上次是按哪個key
volatile bit KeyCanChange;     //該變數用來決定是否要掃描鍵盤

void InitKeyboard(void)		   //掃描鍵盤初始程式(利用timer0 interrupt，每隔一段時間(50ms)，當發生timer0 interrupt時，就掃描ㄧ次鍵盤)
{
	TMOD &= 0xf0;              //我們接下來要設定Timer/Counter Mode Control register的低4bit
	TMOD |= 0x01;              //timer0 工作在mode0
	TH0 = (65536-500000)/256;  //設定timer0 (記時50ms)
	TL0	= (65536-500000)%256;  //設定timer0
	EA = 1;				       //enable all inerrupt
	ET0 = 1;			       //enable timer0 interrupt
	TR0 = 1;                   //啟動timre0
}

void keypad(void)		    //4x4鍵盤掃描程式
{
	currentKey = NOPRESS;   //若該變數為NOPRESS代表沒有按鍵被按下，若不為NOPRESS代表有按鍵被按下
	KEY = 0;		        //KEY變數總共有16個bit，分別代表是哪一個按鈕被按住

	P0 = 0xef;
	if(P0_0 == 0) 
	{
		currentKey	= KEY0;
		KEY |= 1<<0;	//將KEY的第 0 bit設為1(代表第0個按鈕被按下)
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY1;
		KEY |= 1<<1;    //將KEY的第 1 bit設為1(代表第1個按鈕被按下)
	}
	if(P0_2 == 0) 
	{
		currentKey	= KEY2;
		KEY |= 1<<2;	//將KEY的第 2 bit設為1(代表第2個按鈕被按下)
	}
	if(P0_3 == 0)
	{
		currentKey	= KEY3;
		KEY |= 1<<3;    //將KEY的第 3 bit設為1(代表第3個按鈕被按下)
	}
	
	P0 = 0xdf;
	if(P0_0 == 0)
	{
		currentKey	= KEY4;   
		KEY |= 1<<4;    //將KEY的第 4 bit設為1(代表第4個按鈕被按下)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY5;
		KEY |= 1<<5;    //將KEY的第 5 bit設為1(代表第5個按鈕被按下)
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY6;
		KEY |= 1<<6;	//將KEY的第 6 bit設為1(代表第6個按鈕被按下)	
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY7;
		KEY |= 1<<7;    //將KEY的第 7 bit設為1(代表第7個按鈕被按下)	
	}
	
	P0 = 0xbf;
	if(P0_0 == 0)
	{
		currentKey	= KEY8;
		KEY |= 1<<8;	//將KEY的第 8 bit設為1(代表第8個按鈕被按下)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY9;
		KEY |= 1<<9;	//將KEY的第 9 bit設為1(代表第9個按鈕被按下)	
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY10;
		KEY |= 1<<10;   //將KEY的第 10 bit設為1(代表第10個按鈕被按下)		
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY11;
		KEY |= 1<<11;	//將KEY的第 11 bit設為1(代表第11個按鈕被按下)	
	};
	
	P0 = 0x7f;
	if(P0_0 == 0)
	{
		currentKey	= KEY12;
		KEY |= 1<<12;	//將KEY的第 12 bit設為1(代表第12個按鈕被按下)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY13;
		KEY |= 1<<13;	//將KEY的第 13 bit設為1(代表第13個按鈕被按下)
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY14;
		KEY |= 1<<14;	//將KEY的第 14 bit設為1(代表第14個按鈕被按下)	
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY15;
		KEY |= 1<<15;	//將KEY的第 15 bit設為1(代表第15個按鈕被按下)	
	}

	//以下的程式碼是為了防止鍵盤的機械彈跳
	//如果兩次掃描得到的鍵值都相同時，才能確定目前的真實鍵值
	if(currentKey != previousKey)	   
		previousKey = currentKey;
	else
		pressKey = currentKey;
}

void timer0_int(void) interrupt 1
{
	//要重新設定timer0
	TH0 = (65536-500000)/256; //設定timer0 (記時50ms)
	TL0	= (65536-500000)%256; //設定timer0
	if(KeyCanChange)	      //若可以掃描鍵盤
		keypad();

}