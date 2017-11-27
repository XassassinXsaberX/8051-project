#include <AT89X52.h>
#define SP P3_4

void delayms(unsigned int);

int main()
{
	TMOD = 0X01;           // 令記時器0工作於 mode 1
	EA = 1;                // enable all interrupt
	ET0 = 1;               // enable timer0 interrupt
	while(1)
	{
		TR0 = 1;           // 啟動計時器0 
		delayms(500);      // delay 500ms (此時揚聲器會發出聲音) 
		TR0 = 0;           // 關閉記時器0 (不再記時) 
		delayms(500);      // delay 500ms (此時揚聲器不會發出聲音) 
	}
}

void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

void timer0(void) interrupt 1
{
	TH0 = 0XFB;    // 設定記時器中較高的4bit 
	TL0 = 0X1E;    // 設定記時器中較低的4bit
	SP = !SP;      // 將SP的電位反轉
}
