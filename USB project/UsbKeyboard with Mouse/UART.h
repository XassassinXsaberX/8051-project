#include <AT89X52.h>


void InitUART(void);
void UARTPutChar(unsigned char c);		   //�z�Lserial port�ǰe����char�r��
void Prints(unsigned char*s);			   //�z�Lserial port�ǰe�r��(�Y�h�Ӧr��)
void PrintLongInt(unsigned long num);      //�z�Lserial port�L�X��ƼƦr
void PrintShortIntHex(unsigned int num);   //�N2byte��unsigned int��ƥH16�i��覡�z�Lserial port�ǥX
void PrintHex(unsigned char c);		       //�N1byte��unsigned char�r���H16�i��覡�z�Lserial port�ǥX