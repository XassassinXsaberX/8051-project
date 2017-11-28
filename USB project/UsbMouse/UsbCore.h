#ifndef __USBCORE_H__
#define __USBCORE_H__

#define GET_STATUS         0
#define CLEAR_FEATURE      1
#define SET_FEATURE        3
#define SET_ADDRESS        5
#define GET_DESCRIPTOR     6
#define SET_DESCRIPTOR     7
#define GET_CONFIGURATION  8
#define SET_CONFIGURATION  9
#define GET_INTERFACE      10
#define SET_INTERFACE      11
#define SYNCH_FRAME        12

#define DEVICE_DESCRIPTOR         1
#define CONFIGURATION_DESCRIPTOR  2
#define STRING_DESCRIPTOR         3
#define INTERFACE_DESCRIPTOR      4
#define ENDPOINT_DESCRIPTOR       5
#define REPORT_DESCRIPTOR         0x22

#define SET_IDLE 0x0A

void UsbDisconnect(void); //USB�_�}�s��
void UsbConnect(void);    //USB�s�� 
void UsbBusSuspend(void); //bus suspend , interrupt�B�z��� 
void UsbBusReset(void);   //bus reset , interrupt�B�z��� 
void UsbEp0Out(void);     //endpoint 0 OUT , interrupt�B�z��� 
void UsbEp0In(void);      //endpoint 0 IN , interrupt�B�z��� 
void UsbEp1Out(void);     //endpoint 1 OUT , interrupt�B�z��� 
void UsbEp1In(void);      //endpoint 1 IN , interrupt�B�z��� 
void UsbEp2Out(void);     //endpoint 2 OUT , interrupt�B�z��� 
void UsbEp2In(void);      //endpoint 2 IN , interrupt�B�z��� 

void DelayXms(unsigned int t); //delay t ms
void DelayXus(unsigned int t); //delay t us
void UsbEp0SendData(void);     //�NsendPtr���V����ưe�X

extern unsigned char ConfigValue;  //�ثe��confuguration value
extern bit Ep1InIsBusy;            //endpoint 1 �O�_���L 

#endif

//#define DEBUG0

