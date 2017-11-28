#include "PDIUSBD12.h"
#include "UART.h"

void D12WriteCommand(unsigned char command)	 //對PSIUSBD12晶片輸入"寫入指令"，並寫入1 byte的command資料
{
	//可參考PDIUSBD12晶片datasheet中的時序圖
	D12_A0 = 1;		     //select command instruction
	D12_WR = 0;
	D12_DATA = command;
	D12_WR = 1;
	//D12_DATA = 0xff;
}


unsigned char D12ReadByte(void)	//對PSIUSBD12晶片輸入"讀取1 byte 資料"，並回傳讀取到的1 byte資料
{
	//可參考PDIUSBD12晶片datasheet中的時序圖
	unsigned char tmp;
	//D12_DATA = 0xff;
	D12_A0 = 0;			 //select the data phase
	D12_RD = 0;
	tmp = D12_DATA;
	D12_RD = 1;
	return tmp;
}

void D12WriteByte(unsigned char d)	//對PSIUSBD12晶片輸入"寫入1 byte 資料"
{
	//可參考PDIUSBD12晶片datasheet中的時序圖
	D12_A0 = 0;		 	//select the data phase
	D12_WR = 0;
	D12_DATA = d;
	D12_WR = 1;
	//D12_DATA = 0xff;
}

unsigned int D12ReadID(void)  //回傳該晶片的id
{
	unsigned int tmp;
	D12WriteCommand(Read_ID); //對PSIUSBD12晶片輸入"讀取指令" ，指令為Read_ID(即為0xfd，代表讀取晶片id)
	tmp = D12ReadByte();	  //id的低1byte
	tmp |= D12ReadByte()<<8;  //id的高1byte
	return tmp;

}

void D12SelectEndpoint(unsigned char endpoint)	 //select endpoint command
{
	D12WriteCommand(endpoint);  //select endpoint command
}

unsigned char D12ReadEndpointBuffer(unsigned char endpoint, unsigned char Len, unsigned char *Buf)   //讀取指定endpoint的buffer
{
	//輸入的參數中
	//endpoint=0代表 endpoint 0 OUT 、 endpoint=1代表 endpoint 0 IN
	//endpoint=2代表 endpoint 1 OUT 、 endpoint=3代表 endpoint 1 IN
	//endpoint=4代表 endpoint 2 OUT 、 endpoint=5代表 endpoint 2 IN
	//Len代表想要讀取多少byte的資料
	//Buf則代表將endpoint buffer的資料存在此陣列中

	unsigned char real_len;        
	int i;    
	
	D12SelectEndpoint(endpoint);       //首先要選擇endpoint
	D12WriteCommand(D12_READ_BUFFER);  //寫入 "讀取endpoint buffer" 的命令
	//接下來開始讀取資料
	
	D12ReadByte();                     //第一個讀取到的byte沒有意義
	real_len = D12ReadByte();	       //第二個讀取到的byte代表endpoint buffer中，實際上存有多少byte的資料
	if(real_len > Len)
		real_len = Len;	               
	//我們實際上只讀取real_len個byte的資料

	//透過serial port印出debug資訊
	#ifdef DEBUG1
	Prints("讀取OUT endpoint ");
	PrintLongInt(endpoint/2);
	Prints(" 的buffer，共 ");
	PrintLongInt(real_len);
	Prints(" byte\n");
	#endif

	for(i=0;i<real_len;i++)		       //第三個以後讀取到的byte才是真正的data
	{
		Buf[i] = D12ReadByte();		   //讀取byte

		#ifdef DEBUG1
			PrintHex(Buf[i]);		   //透過serial port印出這個byte的16進位表示法
			UARTPutChar(' ');
			if((i+1)%16 == 0)
				UARTPutChar('\n');
		#endif
	}
	#ifdef DEBUG1
	if((real_len)%16!=0)
		UARTPutChar('\n');
	#endif
	return real_len;    //最後return實際上讀取到幾個byte的資料	


}



//將資料寫入特定endpoint的buffer中，當晶片收到token packet為IN request時，再把指定IN endpoint buffer中的資料送出
unsigned char D12WriteEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)	 
{
	//輸入的參數中
	//Endp=0代表 endpoint 0 OUT 、 Endp=1代表 endpoint 0 IN
	//Endp=2代表 endpoint 1 OUT 、 Endp=3代表 endpoint 1 IN
	//Endp=4代表 endpoint 2 OUT 、 Endp=5代表 endpoint 2 IN
	//Len代表想要寫入多少byte的資料
	//Buf則代表將此陣列的資料寫入到buffer中

	int i;
	D12SelectEndpoint(Endp);            //選擇endpoint	
	D12WriteCommand(D12_WRITE_BUFFER);  //寫入 "寫入資料endpoint buffer" 的命令
	//接下來要寫入資料
	D12WriteByte(0);                    //第一個byte要寫入0
	D12WriteByte(Len);                  //第二個byte要寫入ㄧ個數字，代表接下來要送多少byte

	#ifdef DEBUG1
		Prints("寫入資料到IN endpoint ");
		PrintLongInt(Endp/2);
	   	Prints(" 的buffer，共 ");
		PrintLongInt(Len);
		Prints(" byte\n");
	#endif


	//接下來寫入到buffer的資料才是真正要傳送出去的資料
	for(i=0;i<Len;i++)
	{
		D12WriteByte(Buf[i]);

		#ifdef DEBUG1
			PrintHex(Buf[i]);		   //透過serial port印出這個byte的16進位表示法
			UARTPutChar(' ');
			if((i+1)%16 == 0)
				UARTPutChar('\n');
		#endif
	}
	#ifdef DEBUG1
		if(Len%16!=0)
			UARTPutChar('\n');
	#endif

	D12ValidateBuffer(Endp);           //使該IN endpoint buffer有效
	//D12_DATA = 0xff;				   //data port切換到輸入狀態
	return Len;						   //回傳總共寫入幾個byte到buffer中

}

void D12ClearBuffer(unsigned char endpoint)	//清空特定OUT endpoint的buffer
{
	D12SelectEndpoint(endpoint);	        //選擇特定endpoint
	D12WriteCommand(D12_CLEAR_BUFFER);	    //寫入clear buffer command
}

void D12ValidateBuffer(unsigned char endpoint) //使IN endpoint buffer有效
{
	D12SelectEndpoint(endpoint);	           //選擇特定endpoint
	D12WriteCommand(D12_VALIDATE_BUFFER);      //寫入validate buffer command
}

unsigned char D12ReadEndpointLastStatus(unsigned char Endp)	//讀取指定endpoint中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
{
	D12WriteCommand(0x40 + Endp);	       //寫入"Read Endpoint Last Transaction Status command"
	return D12ReadByte();	               //回傳ㄧ個byte，其內容為特定endpoint中最後ㄧ次transaction的狀態
}

void D12AcknowledgeSetup(void)	 //acknowledge setup(目的是讓Clear Buffer command、Validate Buffer command能生效) 
{
	//只會在control transfer的setup stage中使用到
	//當收到SETUP packet時
	//endpoint 0 IN 的Validate Buffer command
	//和endpoint 0 OUT 的Clear Buffer command會失效
	//所以要使用Acknowledge Setup command
	//才能讓Clear Buffer command、Validate Buffer command生效
	
	D12SelectEndpoint(1);                    //選擇endpoint 0 IN
	D12WriteCommand(D12_ACKNOWLEDGE_SETUP);	 //送出Acknowledge Setup command
	D12SelectEndpoint(0);                    //選擇endpoint 0 OUT
	D12WriteCommand(D12_ACKNOWLEDGE_SETUP);  //送出Acknowledge Setup command
	
}

//void D12SetAddress(unsigned char Addr)
//{}

//void D12SetEndpointEnable(unsigned char Enable)
//{}
