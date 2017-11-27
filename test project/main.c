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
"******              江承翰的USB鼠標 \r\n", 
"******                 AT89S52 CPU \r\n", 
"******               建立日期：",__DATE__,"  \r\n", 
"******               建立時間：",__TIME__,"  \n", 
"******               作者：phisoner          \n", 
"******               歡迎訪問我的github \n", 
"******        https://github.com/XassassinXsaberX \n",  
"******               請按K1-K16分別進行測試 \n", 
"******     K1:光標左移K2:光標右移K3:光標上移K4:光標下移  \n", 
"******     K5:滾輪下滾K6:滾輪上滾K7:鼠標左鍵K8:鼠標右鍵  \n", 
"**********************************************************\n", 
}; 

code char led_word[] = {0x3f,0x06,0x5b,0x4f,
                        0x66,0x6d,0x7d,0x07,
						0x7f,0x6f,0x77,0x7c,
						0x39,0x5e,0x79,0x71};



void main()
{
	
	unsigned int id;
	char led;
	unsigned interruptSource;
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
			id = D12ReadID();
			Prints("chip ID 為 ");
			PrintShortIntHex(id);
			//Prints(__DATE__);
			UARTPutChar('\n');
			Prints("you press ");
			PrintHex(led);
			UARTPutChar('\n');
			P2 = led;
			
			while(pressKey != NOPRESS);

		}
		if(D12_INT == 0)   //如果PDIUSBD12晶片發生interrupt
		{
			D12WriteCommand(READ_INTERRUPT_REGISTER);	//寫入 "讀取interrupt register command"
			interruptSource = D12ReadByte();		    //讀取interrupt register
			if((interruptSource>>7) & 0x01)			    //如果發生USB Bus suspend
				UsbBusSuspend();
			if((interruptSource>>6) & 0x01)		        //如果發生USB Bus reset
				UsbBusReset();
			if((interruptSource>>5) & 0x01)
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






