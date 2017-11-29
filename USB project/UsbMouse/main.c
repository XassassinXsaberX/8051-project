#include <AT89X52.h>
#include <stdlib.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

code char headTable[][74] = {
"**********************************************************\r\n", 
"******             江承翰的USB滑鼠 \n", 
"******             AT89S52 CPU \n", 
"******               建立日期：",__DATE__,"\n", 
"******               建立時間：",__TIME__,"\r\n", 
"******               作者：phisoner\n", 
"******               歡迎訪問我的github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               請按K0-K15分別進行測試\n", 
"******  K0:滑鼠右鍵  K1:滑鼠中鍵  K2、K3:滑鼠左鍵\n",
"******  K4、K5:游標右移  K6、K7:遊標左移  K9、K10:遊標下移\n", 
"******  K13、K14:遊標上移  K8、K11:滾輪下滾  K12、K15:滾輪上滾\n", 
"**********************************************************\n", 
}; 


void SendReport(void)
{
	unsigned char Buf[4] = {0,0,0,0};
	Ep1InIsBusy = 1;
	

	if(KEY & 0x01)			  //如果鍵盤0被按下，則代表按下滑鼠右鍵
		Buf[0] |= 1<<1;	      
	if((KEY>>1) & 0x01)       //如果鍵盤1被按下，則代表按下滑鼠中鍵
		Buf[0] |= 1<<2;
	if((KEY>>2) & 0X01)       //如果鍵盤2被按下，則代表按下滑鼠左鍵
		Buf[0] |= 1<<0;
	if((KEY>>3) & 0X01)       //如果鍵盤3被按下，則代表按下滑鼠左鍵
		Buf[0] |= 1<<0;	 


	if((KEY>>4) & 0X01)       //如果鍵盤4被按下，則代表滑鼠右移 (即X軸為正值)
		Buf[1] = 1;
	else if((KEY>>5) & 0X01)  //如果鍵盤5被按下，則代表滑鼠右移 (即X軸為正值)
		Buf[1] = 1;
	else if((KEY>>6) & 0X01)  //如果鍵盤6被按下，則代表滑鼠左移 (即X軸為負值)
		Buf[1] = -1;
	else if((KEY>>7) & 0X01)  //如果鍵盤7被按下，則代表滑鼠左移 (即X軸為負值)
		Buf[1] = -1;

	if((KEY>>9) & 0X01)        //如果鍵盤9被按下，則代表滑鼠上移 (即Y軸為正值)
		Buf[2] = 1;
	else if((KEY>>10) & 0X01)  //如果鍵盤10被按下，則代表滑鼠上移 (即Y軸為正值)
		Buf[2] = 1;
	else if((KEY>>13) & 0X01)  //如果鍵盤13被按下，則代表滑鼠下移 (即Y軸為負值)
		Buf[2] = -1;
	else if((KEY>>14) & 0X01)  //如果鍵盤14被按下，則代表滑鼠下移 (即Y軸為負值)
		Buf[2] = -1;

	if((KEY>>12) & 0X01)       //如果鍵盤12被按下，則代表滾輪上滾 (即滾輪值為正值)
		Buf[3] = 1;
	else if((KEY>>15) & 0X01)  //如果鍵盤15被按下，則代表滾輪上滾 (即滾輪值為正值)
		Buf[3] = 1;
	else if((KEY>>8) & 0X01)   //如果鍵盤8被按下，則代表滾輪下滾 (即滾輪值為負值)
		Buf[3] = -1;
	else if((KEY>>11) & 0X01)  //如果鍵盤11被按下，則代表滾輪下滾 (即滾輪值為負值)
		Buf[3] = -1;
	
	D12WriteEndpointBuffer(3,4,Buf);
}


void main()
{
	
	unsigned int id;
	volatile unsigned interruptSource;
	int i;
	InitUART();			   //初始化serial port設定
	InitKeyboard();	       //初始化4x4鍵盤設定
	
   	//透過serial port印出晶片ID
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

	ConfigValue = 0;			  //設定ㄧ開始的configuration value
   	
	
	while(1)
	{	
		P2 = 0x00;
		KeyCanChange = 1;	      //准許按鍵能被按下
		if(pressKey != NOPRESS)	  //如果有按鍵被按下時
		{
			if(ConfigValue)	      //如果已成功設定configuration
			{
				if(Ep1InIsBusy == 0)	   //如果此時endpoint 1 IN 沒有傳送資料給host
					SendReport();		   //這時才能透過endpoint 1 IN 傳送report給host
			}
				
			//while(pressKey != NOPRESS);

		}
		
		
		if(D12_INT == 0)   //如果PDIUSBD12晶片發生interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//寫入 "讀取interrupt register command"
			interruptSource = D12ReadByte();		    //讀取interrupt register

			if((interruptSource>>7) & 0x01)			    //如果發生USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //如果發生USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)			    //如果USB的endpoint 2 IN 傳送完資料 
				UsbEp2In();
			if((interruptSource>>4) & 0x01)			    //如果USB的endpoint 2 OUT 有收到資料 
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)		        //如果USB的endpoint 1 IN 傳送完資料
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






