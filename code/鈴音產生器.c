#include <AT89X52.h>
#define SP P3_4

void delayms(unsigned int);
code char table[] = {0xf9,0xe5,0xfb,0xee};  // 存放個音階相對應的延時參數
char high,low;                              // 待會要用來決定TH0、TL0


int main()
{
	unsigned int i;
	TMOD = 0X01;        // 令記時器0工作於 mode 1
	EA = 1;             // enable all interrupt
	ET0 = 1;            // enable timer0 interrupt
	while(1)
	{
		for(i=0;i<20;i++)
		{
			high = 0;       // 與TH0相對應參數的位置
			low = 1;        // 與TL0相對應參數的位置
			TR0 = 1;        // 啟動計時器0
			delayms(25);    // delay 25ms(此時若發生timer0 interrupt就會改變SP的電位，藉此發出指定頻率的聲音)
			high = 2;       // 與TH0相對應參數的位置
			low = 3;        // 與TL0相對應參數的位置
			delayms(25);    // delay 25ms(此時若發生timer0 interrupt就會改變SP的電位，藉此發出指定頻率的聲音)
		}
		TR0 = 0;          // 暫停記時器 (不會再發生timer0 interrupt，所以不會改變SP的電位，故揚聲器不會再發出聲音)
		delayms(2000);    // delay 2s
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
	TH0 = table[high];  // 設定記時器中較高的4bit
	TL0 = table[low];   // 設定記時器中較低的4bit
	SP = !SP;           // 將SP的電位反轉
}