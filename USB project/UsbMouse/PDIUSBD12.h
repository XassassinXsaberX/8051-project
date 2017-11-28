#include <AT89X52.h>
#define D12_DATA   P2
#define D12_INT    P3_2
#define D12_A0     P3_3
#define D12_WR     P3_6
#define D12_RD     P3_7

//D12的讀取ID command
#define Read_ID  0xFD

//D12的Set Mode command
#define D12_SET_MODE  0xF3

//D12的讀取interrupt register command
#define READ_INTERRUPT_REGISTER  0xF4

//D12的讀取endpoint buffer command 
#define D12_READ_BUFFER 0xF0

//D12的寫入endpoint buffer command 
#define D12_WRITE_BUFFER 0xF0

//D12的清除receive endpoint buffer command 
#define D12_CLEAR_BUFFER    0xF2

//D12的使能transmit endpoint buffer command 
#define D12_VALIDATE_BUFFER 0xFA

//D12的應答設置包 command 
#define D12_ACKNOWLEDGE_SETUP 0xF1

//D12的set address/enable command 
#define D12_SET_ADDRESS_ENABLE 0xD0

//D12的set endpoint enable command 
#define D12_SET_ENDPOINT_ENABLE 0xD8

//函數宣告
void D12WriteCommand(unsigned char);           //對PSIUSBD12晶片輸入"寫入指令"，並寫入1 byte的command資料
unsigned char D12ReadByte(void);	           //對PSIUSBD12晶片輸入"讀取1 byte 資料"，並回傳讀取到的1 byte資料
void D12WriteByte(unsigned char);	           //對PSIUSBD12晶片輸入"寫入1 byte 資料"
void DelayXms(unsigned int t);                 //延遲 t ms
unsigned int D12ReadID(void);		           //回傳該晶片的id
unsigned char D12ReadEndpointBuffer(unsigned char, unsigned char, unsigned char*);      //讀取指定endpoint的buffer
unsigned char D12WriteEndpointBuffer(unsigned char, unsigned char, unsigned char*);     //將資料寫入指定endpoint的buffer
void D12ClearBuffer(unsigned char);                                                     //清空特定endpoint的buffer
void D12ValidateBuffer(unsigned char);                                                  //使IN endpoint buffer有效
unsigned char D12ReadEndpointLastStatus(unsigned char);	                                //讀取指定endpoint中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
void D12AcknowledgeSetup(void);			                                                //acknowledge setup(目的是讓Clear Buffer command、Validate Buffer command能生效) 
void D12SetAddress(unsigned char Addr);
//void D12SetEndpointEnable(unsigned char Enable);

//#define DEBUG1


