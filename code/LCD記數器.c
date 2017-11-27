#include <AT89X52.h>
#define RS P1_0
#define RW P1_1
#define E P1_2
#define DB P2

void delayms(unsigned int);     // delay ��1ms
void init(void);                // �b�ϥ�LCD�e�A�n���i���l��
void wr_instruction(char);      // �g�J���O��LCD��
void wr_data(char);             // �g�J��ƨ�LCD��
void position(char,char);       // set DD RAM address
void display_string(char*);     // �bLCD����ܸӦr��
void display_num(unsigned int); // �bLCD����ܸӼƦr
void wrcprogram(void);          // ��ۭq���ϧνX�e��LCD��
char keypad(void);

code char pattern[] = {0x00,0x00,0x00,0x1c,  
                       0x1f,0x00,0x00,0x00,  // ���Y���ϧνX
                       0x10,0x0c,0x06,0x1f,
                       0x1f,0x06,0x0c,0x10,  // �������ϧνX
                       0x18,0x18,0x18,0x1f,
                       0x1f,0x00,0x00,0x00}; // �������ϧνX


int main()
{
	unsigned int count1=0,count2=0,count3=0,count4=0;
	char key;
	init();         // ��l��LCD���A 
	
	wrcprogram();   // �ۭq�S��r�� 
	position(1,1);    // �n��set DD RAM address (�]�N�O�����M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}) 
	display_string("1:   0");    // ���U�Ӽg�J��ƮɡA�N�O�N�Ӹ�Ƽg�J��DD RAM���Ӧ�} (�n�g�J����}���W�@��w�]�w�F) 
	position(1,11);   // �n��set DD RAM address (�]�N�O�����M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}) 
	display_string("2:   0");    // ���U�Ӽg�J��ƮɡA�N�O�N�Ӹ�Ƽg�J��DD RAM���Ӧ�} (�n�g�J����}���W�@��w�]�w�F) 
	position(2,1);    // �n��set DD RAM address (�]�N�O�����M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}) 
	display_string("3:   0");    // ���U�Ӽg�J��ƮɡA�N�O�N�Ӹ�Ƽg�J��DD RAM���Ӧ�} (�n�g�J����}���W�@��w�]�w�F) 
	position(2,11);   // �n��set DD RAM address (�]�N�O�����M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}) 
	display_string("4:   0");    // ���U�Ӽg�J��ƮɡA�N�O�N�Ӹ�Ƽg�J��DD RAM���Ӧ�} (�n�g�J����}���W�@��w�]�w�F) 
	
	while(1)
	{
		//����s���U��
		while(1)
		{
			key = keypad();
			if(key%4 == 0 && key != 16)
			{
				count1++;
				if(count1 == 10000)
					count1 = 0;
				position(1,3);
				display_num(count1);
				break;
			}
			else if(key%4 == 1)
			{
				count2++;
				if(count2 == 10000)
					count2 = 0;
				position(1,13);
				display_num(count2);
				break;
			}
			else if(key%4 == 2)
			{
				count3++;
				if(count3 == 10000)
					count3 = 0;
				position(2,3);
				display_num(count3);
				break;
			}
			else if(key%4 == 3)
			{
				count4++;
				if(count4 == 10000)
					count4 = 0;
				position(2,13);
				display_num(count4);
				break;
			}
		}
		//����s��}��
		while(1)
		{
			key = keypad();
			if(key == 16)
				break;
		}
		
		
	}
	
}


void delayms(unsigned int time)
{
	unsigned int i;
	for(;time>0;time--)
		for(i=0;i<120;i++);
}

void init(void)
{
	//�H�U�i��\��]�w
	//�]�wLCD�Ҳլ������ܡA8�줸�ǿ餶���A5X7�r��
	wr_instruction(0x38);
	wr_instruction(0x38);
	wr_instruction(0x38);
	wr_instruction(0x38);
	
	wr_instruction(0x08); //�O��ܾ�OFF
	wr_instruction(0x01); //�M�����
	wr_instruction(0x06); //�OLCD�C����1byte��ƫ�AAC�Ȧ۰�+1
	wr_instruction(0x0C); //�O��ܾ�ON�A����Ф����
}

void wr_instruction(char instruction)
{
	// �N���O�g�JLCD��
	RS = 0;
	RW = 0;
	
	// �NLCD enable��~��g�J����O
	E = 1;
	DB = instruction; // �g�J���O
	
	// �NLCD disable ���קKLCD������ƳQ���g�N�����
	E = 0;
	
	delayms(8); // �C����@���g�J�ʧ@�᳣��������Busy Flag = 0�A�Ϊ�������@�q�ɶ�
	            // �~��b����U�Ӽg�J�ʧ@
}

void wr_data(char d)
{
	// �N��Ƽg�JLCD��
	RS = 1;
	RW = 0;
	
	// �NLCD enable��~��g�J����O
	E = 1;
	DB = d; // �g�J���
	
	// �NLCD disable ���קKLCD������ƳQ���g�N�����
	E = 0;
	
	delayms(1); // �C����@���g�J�ʧ@�᳣��������Busy Flag = 0�A�Ϊ�������@�q�ɶ�
	            // �~��b����U�Ӽg�J�ʧ@
}

void position(char row,char col)  // �Ө禡���D�n�\��O�M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}�W
{
	unsigned char instruction;
	// set DD RAM address
	// �]�N�O���|�M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}�W
	// Address Counter�|�s�񦹦�} 
	instruction = 0x80 + (0x40*(row-1) + (col-1));
	
	wr_instruction(instruction);
	// �{�bAddress Counter(AC)�s�񦹦�} 
}

void display_string(char*s)
{
	unsigned int i;
	for(i=0;;i++)
	{
		if(s[i] == '\0')
			break;
		wr_data(s[i]);  // �N��Ƽg�bAddress Counter(AC)�ҫ��w����}�W 
	}
}

void display_num(unsigned int num)
{
	char table[5];
	char i;
	table[4] = '\0';
	for(i=3;i>=0;i--)
	{
		table[i] = num%10 + '0';
		num /= 10;
	}
	for(i=0;i<4;i++)
	{
		if(table[i] == '0' && i != 3)
			table[i] = ' ';
	}
	display_string(table);
}

void wrcprogram(void)
{
	char i;
	// �n��set CG RAM address (�]�N�O�����M�w�����n�N��Ƽg�bCG RAM�����Ӧ�})
	// Address Counter(AC)�|�O������} 
	wr_instruction(0x40);
	// ���U�Ӽg�Jdata�ɡA�~�|�Ndata�g��AC�ҫ��w��CG RAM��}�W (�n�g�J����}���w�b�W�������w���]�w�F) 
	for(i=0;i<24;i++)
		wr_data(pattern[i]);
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
	
