#include <AT89X52.h>


void InitUART(void);
void UARTPutChar(unsigned char c);		   //透過serial port傳送ㄧ個char字元
void Prints(unsigned char*s);			   //透過serial port傳送字串(即多個字元)
void PrintLongInt(unsigned long num);      //透過serial port印出整數數字
void PrintShortIntHex(unsigned int num);   //將2byte的unsigned int整數以16進位方式透過serial port傳出
void PrintHex(unsigned char c);		       //將1byte的unsigned char字元以16進位方式透過serial port傳出