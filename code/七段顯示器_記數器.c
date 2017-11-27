#include <AT89X52.h>

/*
�O�ƾ�����
���U4x4���s�K�|�O��1
*/

void delayms(unsigned int time);  // delay 1ms
char keypad(void);                // ���y4x4��L�A�ݬO�_������Q���U
void led_scan(unsigned long);     // ��ܤK�ӤC�q��ܾ�
code char led_word[] = {0x3f,0x06,0x5b,0x4f,0x66,
                        0x6d,0x7d,0x07,0x7f,0x6f}; //led 0~9���r�νX


int main()
{
	unsigned long count = 0;
	char key;
	EA = 1;      // enable all interrupt 
	EX0 = 1;     // enable external interupt
	while(1)
	{	
		// �Ĥ@�Ӱj��O�n�����O�_4x4��L�������s�Q���U
		while(1)
		{
			led_scan(count); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�ƭ�count
			key = keypad();
			if(key != 16) // �Y�����s�Q���U��
				break;
		}
		count++;
		// �ĤG�Ӱj��O�n�����O�_4x4��L�������s�Q��}
		while(1)
		{
			led_scan(count); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�ƭ�count
			key = keypad();
			if(key == 16) // �Y�����s�Q��}��
				break;
		}
		if(count == 100000000) //�K�ӤC�q��ܾ��̦h�u����ܨ�99999999�A�ҥH�Y�W�L���ܭn�k0
			count = 0;
		
		
	}
}
	
void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

char keypad(void)
{
	char key = 16;
	
	P0 = 0xef;
	if(P0_0 == 0) key = 0;
	else if(P0_1 == 0) key = 1;
	else if(P0_2 == 0) key = 2;
	else if(P0_3 == 0) key = 3;
	
	P0 = 0xdf;
	if(P0_0 == 0) key = 4;
	else if(P0_1 == 0) key = 5;
	else if(P0_2 == 0) key = 6;
	else if(P0_3 == 0) key = 7;
	
	P0 = 0xbf;
	if(P0_0 == 0) key = 8;
	else if(P0_1 == 0) key = 9;
	else if(P0_2 == 0) key = 10;
	else if(P0_3 == 0) key = 11;
	
	P0 = 0x7f;
	if(P0_0 == 0) key = 12;
	else if(P0_1 == 0) key = 13;
	else if(P0_2 == 0) key = 14;
	else if(P0_3 == 0) key = 15;
	
	return key;
}

void led_scan(unsigned long num)
{
	int i;
	unsigned char table[8];  // �s��num���C�@��Ʀr
	
	bit valid_bit = 0;
	
	
	// �Ynum=2491�A�htable[] = {0,0,0,0,2,4,9,1}
	for(i=7;i>=0;i--)
	{
		table[i] = num%10;
		num /= 10;
	}
	
	for(i=0;i<8;i++)
	{
		P1 = i;
		P2 = led_word[table[i]];
		
		if(table[i] == 0 && valid_bit == 0 && i != 7)  // �`�N�Ynum == 0�ɡA�̫�@��led�N���n����
			P2 = 0x00;  //�ӤC�q��ܾ��n����		
		else if(table[i] != 0)
			valid_bit = 1;
		
		delayms(2);
	}
}

// �w�qexternal interrupt 
void int0(void) interrupt 0
{
	unsigned int i,j,k;
	
	for(j=0;j<5;j++)
	{
		//8�ӤC�q��ܾ���ܼƦr0���0.4��
		for(k=0;k<25;k++)
			for(i=0;i<8;i++)
			{
				P1 = i;
				P2 = led_word[0];
				delayms(2);
			}
		
		//8�ӤC�q��ܾ�����0.4��
		for(k=0;k<25;k++)
			for(i=0;i<8;i++)
			{
				P1 = i;
				P2 = 0x00;
				delayms(2);
			}
	}
	
	//P2 = led_word[0];
	//delayms(1000);
		
}
