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

void UsbDisconnect(void); //USB耞秨硈钡
void UsbConnect(void);    //USB硈钡 
void UsbBusSuspend(void); //bus suspend , interrupt矪瞶ㄧ计 
void UsbBusReset(void);   //bus reset , interrupt矪瞶ㄧ计 
void UsbEp0Out(void);     //endpoint 0 OUT , interrupt矪瞶ㄧ计 
void UsbEp0In(void);      //endpoint 0 IN , interrupt矪瞶ㄧ计 
void UsbEp1Out(void);     //endpoint 1 OUT , interrupt矪瞶ㄧ计 
void UsbEp1In(void);      //endpoint 1 IN , interrupt矪瞶ㄧ计 
void UsbEp2Out(void);     //endpoint 2 OUT , interrupt矪瞶ㄧ计 
void UsbEp2In(void);      //endpoint 2 IN , interrupt矪瞶ㄧ计 

void DelayXms(unsigned int t); //delay t ms
void DelayXus(unsigned int t); //delay t us
void UsbEp0SendData(void);     //盢sendPtr戈癳

extern unsigned char ConfigValue;  //ヘ玡confuguration value
extern bit Ep1InIsBusy;            //endpoint 1 琌Γ窵
extern bit Ep2InIsBusy;            //endpoint 2 琌Γ窵 

#endif

//#define DEBUG0												

