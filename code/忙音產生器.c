#include <AT89X52.h>
#define SP P3_4

void delayms(unsigned int);

int main()
{
	TMOD = 0X01;           // �O�O�ɾ�0�u�@�� mode 1
	EA = 1;                // enable all interrupt
	ET0 = 1;               // enable timer0 interrupt
	while(1)
	{
		TR0 = 1;           // �Ұʭp�ɾ�0 
		delayms(500);      // delay 500ms (���ɴ��n���|�o�X�n��) 
		TR0 = 0;           // �����O�ɾ�0 (���A�O��) 
		delayms(500);      // delay 500ms (���ɴ��n�����|�o�X�n��) 
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
	TH0 = 0XFB;    // �]�w�O�ɾ���������4bit 
	TL0 = 0X1E;    // �]�w�O�ɾ������C��4bit
	SP = !SP;      // �NSP���q�����
}
