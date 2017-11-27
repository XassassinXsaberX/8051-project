#include <AT89X52.h>
#define SP P3_4
code char table[] = {0xf8,0xa1,0xfa,0x6b};  // 存放個音階相對應的延時參數
char high,low;                              // 待會要用來決定TH0、TL0
void delayms(unsigned int);

int main()
{
	TMOD = 0X01;           // 令記時器0工作於 mode 1
	EA = 1;                // enable all interrupt
	ET0 = 1;               // enable timer0 interrupt
	while(1)
	{
		high = 0;             // 與TH0相對應參數的位置
		low = 1;              // 與TL0相對應參數的位置
		TH0 = table[high];    // 設定記時器中較高的4bit
		TL0 = table[low];     // 設定記時器中較低的4bit
		TR0 = 1;              // 啟動計時器0
		delayms(730);         // delay 730ms(此時若發生timer0 interrupt就會改變SP的電位，藉此發出指定頻率的聲音)
		high = 2;             // 與TH0相對應參數的位置
		low = 3;              // 與TL0相對應參數的位置
		TH0 = table[high];    // 設定記時器中較高的4bit
		TL0 = table[low];     // 設定記時器中較低的4bit
		delayms(730);         // delay 730ms(此時若發生timer0 interrupt就會改變SP的電位，藉此發出指定頻率的聲音)
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
	TH0 = table[high];    // 設定記時器中較高的4bit 
	TL0 = table[low];     // 設定記時器中較低的4bit
	SP = !SP;             // 將SP的電位反轉
}
