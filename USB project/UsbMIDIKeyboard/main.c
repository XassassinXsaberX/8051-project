#include <AT89X52.h>
#include <stdlib.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

code char headTable[][74] = {
"**********************************************************\r\n", 
"******             江承翰的USB鍵盤 \n", 
"******             AT89S52 CPU \n", 
"******               建立日期：",__DATE__,"\n", 
"******               建立時間：",__TIME__,"\r\n", 
"******               作者：phisoner\n", 
"******               歡迎訪問我的github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               請按K0-K15分別進行測試\n", 
"******  \n",
"******  K4:MIDI音高55  K5:MIDI音高57  K6:MIDI音高60\n", 
"******  K7:MIDI音高62  K8:MIDI音高64  K9:MIDI音高67\n",
"******  K10:MIDI音高69  K11:MIDI音高72 \n",
"******  \n",  
"**********************************************************\n", 
}; 


void SendNoteOnMsg(void) 
{ 
	//4 byte的緩衝區
	unsigned char Buf[4];
	
	//Note On message第一個byte固定為0x09，第二個byte為0x9n (n為通道號)
	//第三個byte為0xKK（K為音高），第四個byte為0xVV（V為力度）。
	
	Buf[0]=0x09; //Note On message的packet header 
	Buf[1]=0x90; //在通道0上發送Note On message 
	Buf[3]=0x7F; //音量設置為最大 
	
	
	if((KEY>>4) & 0x01) 
	{ 
		Buf[2]=55;       //C調的5 (絕對音高為G音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
 	} 

	else if((KEY>>5) & 0X01) 
	{ 
		Buf[2]=57;       //C調的6 (絕對音高為A音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	} 
	
	else if((KEY>>6) & 0X01)
	{
		Buf[2]=60;       //C調的1 (絕對音高為C音，即中央C)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。。
	} 
	
	else if((KEY>>7) & 0X01) 
	{ 
		Buf[2]=62;       //C調的2 (絕對音高為D音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	} 
	
	else if((KEY>>8) & 0X01) 
	{ 
		Buf[2]=64;       //C調的3 (絕對音高為E音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	} 
	
	else if((KEY>>9) & 0X01) 
	{
		Buf[2]=67;       //C調的5 (絕對音高為G音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	} 
	
	else if((KEY>>10) & 0X01) 
	{ 
		Buf[2]=69;       //C調的6 (絕對音高為A音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	} 
	
	else if((KEY>>11) & 0X01) 
	{ 
		Buf[2]=72;       //C調的1 (絕對音高為C音)
		//透過 IN endpoint 2 返回4 byte的 MIDI event packet。
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。
	}
	else
	{
		//如果有按鍵彈起，則關閉對應的音
		Buf[3]=0x00; //音量設置為0 
		D12WriteEndpointBuffer(5,4,Buf); 
		Ep2InIsBusy=1;   //設置endpoint 2 忙標誌。	
	}
	
	
} 


void main()
{
	
	unsigned int id;
	volatile unsigned interruptSource;
	int i;
	InitUART();			 //初始化serial port設定
	InitKeyboard();	     //初始化鍵盤設定
	

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

	ConfigValue = 0;	  //設定ㄧ開始的configuration value
   	
	
	while(1)
	{	
		KeyCanChange = 1;	      //准許按鍵能被按下
		if(pressKey != NOPRESS)	  //如果有按鍵被按下時
		{
			if(ConfigValue)			   //如果已成功設定configuration
			{
				if(Ep2InIsBusy == 0)   //如果此時endpoint 2 IN 沒有傳送資料給host
					SendNoteOnMsg();   //這時才能透過endpoint 2 IN 傳送Note On message給host 
				while(pressKey != NOPRESS);	  //等到按鍵鬆開才會跳出loop
				SendNoteOnMsg();              //如果按鍵鬆開後要送出ㄧ個Note On message，告知host已經沒有按鍵按下去了
			}
		}
		
		
		if(D12_INT == 0)   //如果PDIUSBD12晶片發生interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//寫入 "讀取interrupt register command"
			interruptSource = D12ReadByte();		    //讀取interrupt register
			
			if((interruptSource>>7) & 0x01)			    //如果發生USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //如果發生USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)			    //如果USB的endpoint 2 IN 傳送完data packet給host後
				UsbEp2In();
			if((interruptSource>>4) & 0x01)				//如果USB的endpoint 2 OUT 有收到來自host的data packet
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)				//如果USB的endpoint 1 IN 傳送完data packet給host後
				UsbEp1In();
			if((interruptSource>>2) & 0x01)				//如果USB的endpoint 1 OUT 有收到來自host的data packet
				UsbEp1Out();
			if((interruptSource>>1) & 0x01)				//如果USB的endpoint 0 IN 傳送完data packet給host後
				UsbEp0In();
			if((interruptSource>>0) & 0x01)				//如果USB的endpoint 0 OUT 有收到來自host的data packet
				UsbEp0Out();
			
				
		}
		
	}
	
}





