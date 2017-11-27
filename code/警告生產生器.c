#include <AT89X52.h>
#define SP P3_4
code char table[] = {0xf8,0xa1,0xfa,0x6b};  // �s��ӭ����۹��������ɰѼ�
char high,low;                              // �ݷ|�n�ΨӨM�wTH0�BTL0
void delayms(unsigned int);

int main()
{
	TMOD = 0X01;           // �O�O�ɾ�0�u�@�� mode 1
	EA = 1;                // enable all interrupt
	ET0 = 1;               // enable timer0 interrupt
	while(1)
	{
		high = 0;             // �PTH0�۹����Ѽƪ���m
		low = 1;              // �PTL0�۹����Ѽƪ���m
		TH0 = table[high];    // �]�w�O�ɾ���������4bit
		TL0 = table[low];     // �]�w�O�ɾ������C��4bit
		TR0 = 1;              // �Ұʭp�ɾ�0
		delayms(730);         // delay 730ms(���ɭY�o��timer0 interrupt�N�|����SP���q��A�Ǧ��o�X���w�W�v���n��)
		high = 2;             // �PTH0�۹����Ѽƪ���m
		low = 3;              // �PTL0�۹����Ѽƪ���m
		TH0 = table[high];    // �]�w�O�ɾ���������4bit
		TL0 = table[low];     // �]�w�O�ɾ������C��4bit
		delayms(730);         // delay 730ms(���ɭY�o��timer0 interrupt�N�|����SP���q��A�Ǧ��o�X���w�W�v���n��)
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
	TH0 = table[high];    // �]�w�O�ɾ���������4bit 
	TL0 = table[low];     // �]�w�O�ɾ������C��4bit
	SP = !SP;             // �NSP���q�����
}
