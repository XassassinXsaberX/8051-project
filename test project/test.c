#include <AT89x52.h>

/*external interrupt 外部中斷實驗*/

void delayms(unsigned int time);

int main()
{
	EA = 1;
	EX0 = 1;
	while(1)
	{
		P2 = ~0xaa;
	}
}

void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

void int0(void) interrupt 0
{
	unsigned int i;
	for(i=0;i<5;i++)
	{
		P2 = 0x00;
		delayms(500);
		P2 = 0xff;
		delayms(500);
	}
	
}