#include <AT89X52.h>
#include "PDIUSBD12.h"
#include "UART.h"
#include "key.h"
#include "UsbCore.h"

/*
serial port實驗(傳資料到電腦中)
設定baud rate = 1200  (1200bps)
PC使用RS232的serial port來接收資料
*/

code char headTable[][74] = {
"**********************************************************\r\n", 
"******             江承翰的USB鼠標 \r\n", 
"******             AT89S52 CPU \r\n", 
"******               建立日期：",__DATE__,"\r\n", 
"******               建立時間：",__TIME__,"\r\n", 
"******               作者：phisoner\n", 
"******               歡迎訪問我的github\n", 
"******        https://github.com/XassassinXsaberX\n",  
"******               請按K0-K15分別進行測試\n", 
"******  K4、K5:游標右移  K6、K7:遊標左移  K9、K10:遊標下移\n", 
"******  K13、K14:遊標上移  K8、K11:滾輪下滾  K12、K15:滾輪上滾\n", 
"**********************************************************\n", 
}; 

code char led_word[] = {0x3f,0x06,0x5b,0x4f,
                        0x66,0x6d,0x7d,0x07,
						0x7f,0x6f,0x77,0x7c,
						0x39,0x5e,0x79,0x71};


void SendReport(void)
{
	unsigned char Buf[4] = {0,0,0,0};
	Ep1InIsBusy = 1;
	if(KEY & 0x01)			  //如果鍵盤0被按下，則代表按下滑鼠左鍵
		Buf[0] |= 1<<7;	      
	if((KEY>>1) & 0x01)       //如果鍵盤1被按下，則代表按下滑鼠右鍵
		Buf[0] |= 1<<6;
	if((KEY>>2) & 0X01)       //如果鍵盤2被按下，則代表按下滑鼠中鍵
		Buf[0] |= 1<<5;

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
	char led;
	volatile unsigned interruptSource;
	int i;
	InitUART();
	InitKeyboard();
	

	for(i=0;i<16;i++)
	{
		Prints(headTable[i]);
	}
	id = D12ReadID();
	Prints("chip ID is ");
	PrintShortIntHex(id);
	UARTPutChar('\n');
	
	UsbDisconnect();
	UsbConnect();

	ConfigValue = 0;
   	
	
	while(1)
	{	
		P2 = 0x00;
		KeyCanChange = 1;	      //准許按鍵能被按下
		if(pressKey != NOPRESS)	  //如果有按鍵被按下時
		{
			//檢查KEY變數，來看看是哪個按鍵被按下
			for(i=0;i<16;i++)
				if( (KEY>>i) & 0x01 )
					led = led_word[i];
			//id = D12ReadID();
			//Prints("chip ID 為 ");
			//PrintShortIntHex(id);
			//UARTPutChar('\n');
			//Prints("you press ");
			//PrintHex(led);
			//UARTPutChar('\n');
			//P2 = led;
			if(ConfigValue)
			{
				//PrintShortIntHex(ConfigValue);
				if(Ep1InIsBusy == 0)
					SendReport();
			}
				
			//while(pressKey != NOPRESS);

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
			if((interruptSource>>5) & 0x01)			    //
				UsbEp2In();
			if((interruptSource>>4) & 0x01)
				UsbEp2Out();
			if((interruptSource>>3) & 0x01)
				UsbEp1In();
			if((interruptSource>>2) & 0x01)
				UsbEp1Out();
			if((interruptSource>>1) & 0x01)
				UsbEp0In();
			if((interruptSource>>0) & 0x01)
				UsbEp0Out();
			
				
		}
		
	}
	
}






