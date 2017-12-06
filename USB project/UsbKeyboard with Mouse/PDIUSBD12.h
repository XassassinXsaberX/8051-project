#include <AT89X52.h>
#define D12_DATA   P2
#define D12_INT    P3_2
#define D12_A0     P3_3
#define D12_WR     P3_6
#define D12_RD     P3_7

//D12��Ū��ID command
#define Read_ID  0xFD

//D12��Set Mode command
#define D12_SET_MODE  0xF3

//D12��Ū��interrupt register command
#define READ_INTERRUPT_REGISTER  0xF4

//D12��Ū��endpoint buffer command 
#define D12_READ_BUFFER 0xF0

//D12���g�Jendpoint buffer command 
#define D12_WRITE_BUFFER 0xF0

//D12���M��receive endpoint buffer command 
#define D12_CLEAR_BUFFER    0xF2

//D12���ϯ�transmit endpoint buffer command 
#define D12_VALIDATE_BUFFER 0xFA

//D12�������]�m�] command 
#define D12_ACKNOWLEDGE_SETUP 0xF1

//D12��set address/enable command 
#define D12_SET_ADDRESS_ENABLE 0xD0

//D12��set endpoint enable command 
#define D12_SET_ENDPOINT_ENABLE 0xD8

//��ƫŧi
void D12WriteCommand(unsigned char);           //��PSIUSBD12������J"�g�J���O"�A�üg�J1 byte��command���
unsigned char D12ReadByte(void);	           //��PSIUSBD12������J"Ū��1 byte ���"�A�æ^��Ū���쪺1 byte���
void D12WriteByte(unsigned char);	           //��PSIUSBD12������J"�g�J1 byte ���"
void DelayXms(unsigned int t);                 //���� t ms
unsigned int D12ReadID(void);		           //�^�ǸӴ�����id
unsigned char D12ReadEndpointBuffer(unsigned char, unsigned char, unsigned char*);      //Ū�����wendpoint��buffer
unsigned char D12WriteEndpointBuffer(unsigned char, unsigned char, unsigned char*);     //�N��Ƽg�J���wendpoint��buffer
void D12ClearBuffer(unsigned char);                                                     //�M�ůS�wendpoint��buffer
void D12ValidateBuffer(unsigned char);                                                  //��IN endpoint buffer����
unsigned char D12ReadEndpointLastStatus(unsigned char);	                                //Ū�����wendpoint���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
void D12AcknowledgeSetup(void);			                                                //acknowledge setup(�ت��O��Clear Buffer command�BValidate Buffer command��ͮ�) 
//void D12SetAddress(unsigned char Addr);
//void D12SetEndpointEnable(unsigned char Enable);

//#define DEBUG1


