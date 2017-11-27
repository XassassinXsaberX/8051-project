#include <AT89X52.h>

/*
�O�ɾ�����
���U4x4���s�K�|�}�l�O��
�A���U4x4���s�K�|����O��
�b���U4x4���s�K�|�k0
�O�ɳ��q0.0��}�l�A�H0.1�����A�O�ɽd��0.0��~9999999.9��
*/

void delayms(unsigned int);               // �ʲ���delay 1ms
char keypad(void);                // ���y4x4��L�A�ݬO�_������Q���U
void led_scan(unsigned long);     // ��ܤK�ӤC�q��ܾ�
code char led_word[] = {0x3f,0x06,0x5b,0x4f,0x66,
                        0x6d,0x7d,0x07,0x7f,0x6f}; //led 0~9���r�νX
unsigned long count_time = 0;     // �K�ӤC�q��ܾ��|��ܦ��ɶ�
unsigned int count=0; 
											

int main()
{
	char key;
	EA = 1;           // enable all interrupt
	ET0 = 1;          // enable timer 0 interrupt
	TMOD = 0X01;      // ��timer0�u�@��mode 1 
	while(1)
	{	
		count_time = 0;
		
		// �����Ĥ@�ӫ��s���U�O�}�l�O��
		// �Ĥ@�Ӱj��O�n�����O�_4x4��L�������s�Q���U
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�Ʈɶ� count_time
			key = keypad();
			if(key != 16)         // �Y�����s�Q���U��
				break;
		}
		// �ĤG�Ӱj��O�n�����O�_4x4��L�������s�Q��}
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�ƭ�count
			key = keypad();
			if(key == 16)    // �Y�����s�Q��}��
				break;
		}
		TH0 = (65536 - 50000) / 256; // �]�w�O�ƾ������줸 (8bit)
		TL0 = (65536 - 50000) % 256; // �]�w�O�ƾ����C�줸 (8bit)
		TR0 = 1;                     // �ҰʰO�ƾ�
		
		
		
		
		// �A�ӲĤG�����U���s�O���ɶ��Ȱ�
		// �ĤT�Ӱj��O�n�����O�_4x4��L�������s�Q���U
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�Ʈɶ� count_time
			key = keypad();
			if(key != 16)         // �Y�����s�Q���U��
				break;
		}
		TR0 = 0;           // �N�O�ƾ��Ȱ�
		// �ĥ|�Ӱj��O�n�����O�_4x4��L�������s�Q��}
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�ƭ�count
			key = keypad();
			if(key == 16)    // �Y�����s�Q��}��
				break;
		}
		
		
		// �ĤT�����U���s�O���ɶ��k0
		// �Ĥ��Ӱj��O�n�����O�_4x4��L�������s�Q���U
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�Ʈɶ� count_time
			key = keypad();
			if(key != 16)         // �Y�����s�Q���U��
				break;
		}
		count_time = 0;
		// �Ĥ��Ӱj��O�n�����O�_4x4��L�������s�Q��}
		while(1)
		{
			led_scan(count_time); // �K�ӤC�q��ܾ��n�b�j�餤���_��ܥثe���O�ƭ�count
			key = keypad();
			if(key == 16)    // �Y�����s�Q��}��
				break;
		}
		
		
		
		
		if(count_time == 100000000) //�K�ӤC�q��ܾ��̦h�u����ܨ�99999999�A�ҥH�Y�W�L���ܭn�k0
			count_time = 0;
		
		
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
	//�C50ms�N�|���榹interrupt service routine
	
	count++;
	if(count == 2)   // �Y�g�L0.1��
	{
		count = 0;
		count_time++;  // ���K�ӤC�q��ܾ���ܪ��ɶ���
	}
	if(count_time == 100000000)  // �K�ӤC�q��ܾ��̦h�u����ܨ�9999999.9
		count_time = 0;
	
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
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
	char table[8];
	bit valid_bit=0;   // �Yvalid_bit = 0�A�h�b�@�뱡�p�U�A�C�q��ܾ����Ӻ���
	int i;
	for(i=7;i>=0;i--)
	{
		table[i] = num%10;
		num/=10;
	}
  
	for(i=0;i<8;i++)
	{
		P1 = i;
		if(table[i] != 0 || i == 6)
				valid_bit = 1;
		if(table[i] == 0 && valid_bit == 0 && i < 6)    // �C�q��ܾ��n���������p
			P2 = 0x00;
		else                                            // �C�q��ܾ��n��ܼƦr�����p
			P2 = led_word[table[i]];
		
		
		if(i == 6)
			P2_7 = 1;   // �q�k�ƹL�ӲĤG�ӼƦr�n�L�X�p���I
		delayms(2);
	}
}

