#include <AT89X52.h>
#include <INTRINS.h>
#define CLK P1_3
#define DOUT P1_4
#define CS P1_6

void delayms(unsigned int);
void delay(void);
unsigned char readADC0831(void);
void led_scan(unsigned int);
code char led_word[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
                        0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};


int main()
{
	unsigned int voltage;
	char i;
	while(1)
	{
		voltage = readADC0831();    // Ū���q����
		voltage = voltage*100/51;   // �N�q���ȴ���
		for(i=0;i<100;i++)
			led_scan(voltage);
	}
}

void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

void delay(void) // delay 1us
{
	_nop_();
	_nop_();
}

unsigned char readADC0831(void)
{
	char i;
	unsigned char voltage=0;
	
	DOUT = 1; // �@�}�lDOUT�]��1
	
	CS = 0;   // ADC0831�}�l��A/D�ഫ
	
	CLK = 0;  
	delay();
	CLK = 1;  // �Ĥ@�Ӯɧǯߪi�����t
	delay();  // ���ɯߪi��high level
	CLK = 0;  // �Ĥ@�Ӯɧǯߪi���t�t
	delay();  // ���ɯߪi��low level
	CLK = 1;  // �ĤG�Ӯɧǯߪi�����t
	delay();  // ���ɯߪi��high level
	CLK = 0;  // �ĤG�Ӯɧǯߪi���t�t
	delay();  // ���ɯߪi��low level
	
	// ���U�ӥi�H�}�lŪ���ƾ�
	for(i=0;i<8;i++)
	{
		voltage <<= 1;   // �Nvoltage�V�����@bit
		// �ڭ̥i�H�}�lŪ��DOUT����Xbit��
		if(DOUT == 1)
			voltage |= 1;  // �N�̥k��Ĥ@��bit�]��1
		else
			voltage &= ~1; // �N�̥k��Ĥ@��bit�]��0
		CLK = 1;         
		delay();         // ���ɯߪi��high level
		CLK = 0;
		delay();         // ���ɯߪi��low level
	}
	CS = 1;             // ADC0831����A/D�ഫ
	return voltage;     // �Ǧ^�@��byte�����
}

void led_scan(unsigned int voltage)
{
	char table[8];
	char i;
	bit valid_bit=0;
	for(i=7;i>=0;i--)
	{
		table[i] = voltage%10;
		voltage /= 10;
	}
	
	for(i=0;i<8;i++)
	{
		P1 = i;
		if(i == 5)
			P2 = led_word[table[i]+10];
		else
			P2 = led_word[table[i]];
		
		if(valid_bit == 0 && table[i] == 0 && i<5)
			P2 = 0x00;
		else if(table[i] != 0)
			valid_bit = 1;
		delayms(2);
	}
}
