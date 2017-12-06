#include <AT89X52.h>
#include <stdlib.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

bit function = 0; //當function = 0時，代表使用鍵盤功能、當function = 1時，代表使用滑鼠功能

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
"******  K2切換鍵盤/滑鼠\n",
"******\n",
"******  鍵盤如下\n",
"******  K0:左Ctrl鍵  K1:左Shift鍵  K2:切換鍵盤滑鼠  K3:左GUI\n",
"******  K4:數字小鍵盤1  K5:數字小鍵盤2  K6:數字小鍵盤3\n", 
"******  K7:數字小鍵盤4  K8:數字小鍵盤5  K9:數字小鍵盤6\n",
"******  K10:數字小鍵盤7  K11:數字小鍵盤8  K12:數字小鍵盤9\n",
"******  K13:數字小鍵盤0  K14:Caps Lock  K15:Num Lock\n", 
"******\n",
"******  滑鼠如下\n",
"******  K0:滑鼠右鍵  K1:滑鼠中鍵  K3:滑鼠左鍵\n",
"******  K4、K5:游標右移  K6、K7:遊標左移  K9、K10:遊標下移\n", 
"******  K13、K14:遊標上移  K8、K11:滾輪下滾  K12、K15:滾輪上滾\n",  
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
	unsigned char Buf[9] = {0,0,0,0,0,0,0,0,0};
	unsigned char i=3;
	Ep1InIsBusy = 1;
	
	if(function == 0) //如果選擇鍵盤功能
	{
		Buf[0] = 1;   //鍵盤的report ID為1		

		//Buf[1]有8個bit
		//bit 0:為左Ctrl鍵、bit 1:為左Shift鍵、bit 2:為左Alt鍵、bit 3:為左GUI(即Window鍵)
		//bit 4:為右Ctrl鍵、bit 5:為右Shift鍵、bit 6:為右Alt鍵、bit 7:為右GUI(即Window鍵)
		if(KEY & 0x01)			  //如果鍵盤0被按下，則代表按下左Ctrl鍵
			Buf[1] |= 1<<0;	      
		if((KEY>>1) & 0x01)       //如果鍵盤1被按下，則代表按下左Shift鍵
			Buf[1] |= 1<<1;
		if((KEY>>2) & 0X01)       //如果鍵盤2被按下，則代表按下左Alt鍵
			Buf[1] |= 1<<2;
		if((KEY>>3) & 0X01)       //如果鍵盤3被按下，則代表按下左GUI(即Window鍵
			Buf[1] |= 1<<3;	 
	
	
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

		D12WriteEndpointBuffer(3,9,Buf);
	}
	else  //如果選擇滑鼠功能時
	{
		Buf[0] = 2;               //滑鼠的report ID為2

		if(KEY & 0x01)			  //如果鍵盤0被按下，則代表按下滑鼠右鍵
			Buf[1] |= 1<<1;	      
		if((KEY>>1) & 0x01)       //如果鍵盤1被按下，則代表按下滑鼠中鍵
			Buf[1] |= 1<<2;
		if((KEY>>2) & 0X01)       //如果鍵盤2被按下，則代表按下滑鼠左鍵
			Buf[1] |= 1<<0;
		if((KEY>>3) & 0X01)       //如果鍵盤3被按下，則代表按下滑鼠左鍵
			Buf[1] |= 1<<0;	 
	
	
		if((KEY>>4) & 0X01)       //如果鍵盤4被按下，則代表滑鼠右移 (即X軸為正值)
			Buf[2] = 1;
		else if((KEY>>5) & 0X01)  //如果鍵盤5被按下，則代表滑鼠右移 (即X軸為正值)
			Buf[2] = 1;
		else if((KEY>>6) & 0X01)  //如果鍵盤6被按下，則代表滑鼠左移 (即X軸為負值)
			Buf[2] = -1;
		else if((KEY>>7) & 0X01)  //如果鍵盤7被按下，則代表滑鼠左移 (即X軸為負值)
			Buf[2] = -1;
	
		if((KEY>>9) & 0X01)        //如果鍵盤9被按下，則代表滑鼠上移 (即Y軸為正值)
			Buf[3] = 1;
		else if((KEY>>10) & 0X01)  //如果鍵盤10被按下，則代表滑鼠上移 (即Y軸為正值)
			Buf[3] = 1;
		else if((KEY>>13) & 0X01)  //如果鍵盤13被按下，則代表滑鼠下移 (即Y軸為負值)
			Buf[3] = -1;
		else if((KEY>>14) & 0X01)  //如果鍵盤14被按下，則代表滑鼠下移 (即Y軸為負值)
			Buf[3] = -1;
	
		if((KEY>>12) & 0X01)       //如果鍵盤12被按下，則代表滾輪上滾 (即滾輪值為正值)
			Buf[4] = 1;
		else if((KEY>>15) & 0X01)  //如果鍵盤15被按下，則代表滾輪上滾 (即滾輪值為正值)
			Buf[4] = 1;
		else if((KEY>>8) & 0X01)   //如果鍵盤8被按下，則代表滾輪下滾 (即滾輪值為負值)
			Buf[4] = -1;
		else if((KEY>>11) & 0X01)  //如果鍵盤11被按下，則代表滾輪下滾 (即滾輪值為負值)
			Buf[4] = -1;
		
		D12WriteEndpointBuffer(3,5,Buf);
	
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
   	
	Prints("目前是");
	if(function == 0)
		//Prints("keyboard function\n");
		Prints("鍵盤 function\n");
	else
		Prints("滑鼠 function\n");

	while(1)
	{	
		KeyCanChange = 1;	      //准許按鍵能被按下
		if(pressKey == KEY2)		   //如果按鍵2被按下時，代表切換功能(鍵盤/滑鼠)
		{
			function = !function;
			while(pressKey != NOPRESS);//等到按鍵鬆開時才跳出loop
			Prints("選擇");
			if(function == 0)
				Prints("鍵盤 function\n");
			else
				Prints("滑鼠 function\n");
		}
		else if(pressKey != NOPRESS)   //如果有按鍵被按下時
		{ 	
			if(function == 0)		   //如果使用鍵盤功能時
			{
				if(ConfigValue)			      //如果已成功設定configuration
				{
					if(Ep1InIsBusy == 0)      //如果此時endpoint 1 IN 沒有傳送資料給host
						SendReport();	      //這時才能透過endpoint 1 IN 傳送report給host 
				}
					
				while(pressKey != NOPRESS);	  //等到按鍵鬆開才會跳出loop
				SendReport();                 //如果按鍵鬆開後要送出ㄧ個report，告知host已經沒有按鍵按下去了		
			}
			else					   //如果使用滑鼠功能時
			{
				if(ConfigValue)			      //如果已成功設定configuration
				{
					if(Ep1InIsBusy == 0)      //如果此時endpoint 1 IN 沒有傳送資料給host
						SendReport();	      //這時才能透過endpoint 1 IN 傳送report給host 
				}	
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


