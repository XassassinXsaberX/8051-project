#include <AT89X52.h>
#define SP P3_4

void delayms(unsigned int);
code char table[] = {0xf9,0xe5,0xfb,0xee};  // �s��ӭ����۹��������ɰѼ�
char high,low;                              // �ݷ|�n�ΨӨM�wTH0�BTL0


int main()
{
	unsigned int i;
	TMOD = 0X01;        // �O�O�ɾ�0�u�@�� mode 1
	EA = 1;             // enable all interrupt
	ET0 = 1;            // enable timer0 interrupt
	while(1)
	{
		for(i=0;i<20;i++)
		{
			high = 0;       // �PTH0�۹����Ѽƪ���m
			low = 1;        // �PTL0�۹����Ѽƪ���m
			TR0 = 1;        // �Ұʭp�ɾ�0
			delayms(25);    // delay 25ms(���ɭY�o��timer0 interrupt�N�|����SP���q��A�Ǧ��o�X���w�W�v���n��)
			high = 2;       // �PTH0�۹����Ѽƪ���m
			low = 3;        // �PTL0�۹����Ѽƪ���m
			delayms(25);    // delay 25ms(���ɭY�o��timer0 interrupt�N�|����SP���q��A�Ǧ��o�X���w�W�v���n��)
		}
		TR0 = 0;          // �Ȱ��O�ɾ� (���|�A�o��timer0 interrupt�A�ҥH���|����SP���q��A�G���n�����|�A�o�X�n��)
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
	TH0 = table[high];  // �]�w�O�ɾ���������4bit
	TL0 = table[low];   // �]�w�O�ɾ������C��4bit
	SP = !SP;           // �NSP���q�����
}