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
"******  K0:左Ctrl鍵  K1:左Shift鍵  K2:左Alt鍵  K3:左GUI\n",
"******  K4:數字小鍵盤1  K5:數字小鍵盤2  K6:數字小鍵盤3\n", 
"******  K7:數字小鍵盤4  K8:數字小鍵盤5  K9:數字小鍵盤6\n",
"******  K10:數字小鍵盤7  K11:數字小鍵盤8  K12:數字小鍵盤9\n",
"******  K13:數字小鍵盤0  K14:Caps Lock  K15:Num Lock\n",  
"**********************************************************\n", 
}; 


void KeyBoardReset(unsigned char *Buf)
{
	char i;
	for(i=2;i<8;i++)
		Buf[i] = 0xff;
}

void SendReport(void)
{
	unsigned char Buf[8] = {0,0,0,0,0,0,0,0};
	unsigned char i=2;
	Ep1InIsBusy = 1;
	
	//Buf[0]有8個bit
	//bit 0:為左Ctrl鍵、bit 1:為左Shift鍵、bit 2:為左Alt鍵、bit 3:為左GUI(即Window鍵)
	//bit 4:為右Ctrl鍵、bit 5:為右Shift鍵、bit 6:為右Alt鍵、bit 7:為右GUI(即Window鍵)
	if(KEY & 0x01)			  //如果鍵盤0被按下，則代表按下左Ctrl鍵
		Buf[0] |= 1<<0;	      
	if((KEY>>1) & 0x01)       //如果鍵盤1被按下，則代表按下左Shift鍵
		Buf[0] |= 1<<1;
	if((KEY>>2) & 0X01)       //如果鍵盤2被按下，則代表按下左Alt鍵
		Buf[0] |= 1<<2;
	if((KEY>>3) & 0X01)       //如果鍵盤3被按下，則代表按下左GUI(即Window鍵
		Buf[0] |= 1<<3;	 


	if((KEY>>4) & 0X01)       //如果鍵盤4被按下，則代表按下數字小鍵盤1 
	{
		Buf[i++] = 0x59;
	}
	if((KEY>>5) & 0X01)       //如果鍵盤5被按下，則代表按下數字小鍵盤2
	{
		Buf[i++] = 0x5a;
	}
	if((KEY>>6) & 0X01)       //如果鍵盤6被按下，則代表按下數字小鍵盤3
	{
		Buf[i++] = 0x5b;
	}
	if((KEY>>7) & 0X01)       //如果鍵盤7被按下，則代表按下數字小鍵盤4
	{
		Buf[i++] = 0x5c;
	}
	if((KEY>>8) & 0X01)       //如果鍵盤8被按下，則代表按下數字小鍵盤5
	{
		Buf[i++] = 0x5d;
	}
	if((KEY>>9) & 0X01)       //如果鍵盤9被按下，則代表按下數字小鍵盤6
	{
		Buf[i++] = 0x5e;
	}
	if((KEY>>10) & 0X01)      //如果鍵盤10被按下，則代表按下數字小鍵盤7
	{	
		if(i >= 8)
			KeyBoardReset(Buf);	
		else
			Buf[i++] = 0x5f;
	}
	if((KEY>>11) & 0X01)      //如果鍵盤11被按下，則代表按下數字小鍵盤8
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x60;			
	}
	if((KEY>>12) & 0X01)      //如果鍵盤12被按下，則代表按下數字小鍵盤9
	{	
		if(i >= 8)
			KeyBoardReset(Buf);	
		else
			Buf[i++] = 0x61;
	}
	if((KEY>>13) & 0X01)      //如果鍵盤13被按下，則代表按下數字小鍵盤0
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x62;			
	}
	if((KEY>>14) & 0X01)      //如果鍵盤14被按下，則代表按下Caps Lock
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x39;			
	}
   	if((KEY>>15) & 0X01)      //如果鍵盤15被按下，則代表按下Num Lock
	{
		if(i >= 8)
			KeyBoardReset(Buf);
		else	
			Buf[i++] = 0x53;			
	}

	
	D12WriteEndpointBuffer(3,8,Buf);
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
				if(Ep1InIsBusy == 0)   //如果此時endpoint 1 IN 沒有傳送資料給host
					SendReport();	   //這時才能透過endpoint 1 IN 傳送report給host 
			}
				
			while(pressKey != NOPRESS);	  //等到按鍵鬆開才會跳出loop
			SendReport();                 //如果按鍵鬆開後要送出ㄧ個report，告知host已經沒有按鍵按下去了

		}
		
		
		if(D12_INT == 0)   //如果PDIUSBD12晶片發生interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//寫入 "讀取interrupt register command"
			interruptSource = D12ReadByte();		    //讀取interrupt register
			D12ReadByte();
			if((interruptSource>>7) & 0x01)			    //如果發生USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //如果發生USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)			    //如果USB的endpoint 2 IN 傳送完資料
				UsbEp2In();
			if((interruptSource>>4) & 0x01)				//如果USB的endpoint 2 OUT 有收到資料
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)				//如果USB的endpoint 1 IN 傳送完資料
				UsbEp1In();
			if((interruptSource>>2) & 0x01)				//如果USB的endpoint 1 OUT 有收到資料
				UsbEp1Out();
			if((interruptSource>>1) & 0x01)				//如果USB的endpoint 0 IN 傳送完資料
				UsbEp0In();
			if((interruptSource>>0) & 0x01)				//如果USB的endpoint 0 OUT 有收到資料
				UsbEp0Out();
			
				
		}
		
	}
	
}





