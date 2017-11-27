#include <AT89X52.h>
#define RS P1_0
#define RW P1_1
#define E P1_2
#define DB P2

void delayms(unsigned int);  // delay ��1ms
void init(void);             // �b�ϥ�LCD�e�A�n���i���l��
void wr_instruction(char);   // �g�J���O��LCD��
void wr_data(char);          // �g�J��ƨ�LCD��
void position(char,char);    // set DD RAM address
void display(char*);         // �bLCD����ܸӦr��
void wrcprogram(void);       // ��ۭq���ϧνX�e��LCD��

code char pattern[] = {0x00,0x00,0x00,0x1c,  
                       0x1f,0x00,0x00,0x00,  // ���Y���ϧνX
                       0x10,0x0c,0x06,0x1f,
                       0x1f,0x06,0x0c,0x10,  // �������ϧνX
                       0x18,0x18,0x18,0x1f,
                       0x1f,0x00,0x00,0x00}; // �������ϧνX


int main()
{
	char s[] = "hello hank";
	char s2[] = {0x0a,0x09,0x08,0x00};
	init();         // ��l��LCD���A 
	wrcprogram();   // �ۭq�S��r�� 
	
	// �n�g�J��ƨ� DD RAM�W�� 
	position(1,1);  // �n��set DD RAM address (�]�N�O�����M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}) 
	display(s);     // ���U�Ӽg�J��ƮɡA�N�O�N�Ӹ�Ƽg�J��DD RAM���Ӧ�} (�n�g�J����}���W�@��w�]�w�F) 
	position(1,12);
	display(s2);
	position(2,1);
	display("C/C++");
	while(1);
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

void position(char row,char col)
{
	unsigned char instruction;
	// set DD RAM address
	// �]�N�O���|�M�w�����n�N��Ƽg�bDD RAM�����Ӧ�}�W
	// Address Counter�|�s�񦹦�} 
	instruction = 0x80 + (0x40*(row-1) + (col-1)); 
	  
	wr_instruction(instruction);
	// �{�bAddress Counter(AC)�s�񦹦�} 
}

void display(char*s)
{
	unsigned int i;
	for(i=0;;i++)
	{
		if(s[i] == '\0')
			break;
		wr_data(s[i]);  // �N��Ƽg�bAddress Counter(AC)�ҫ��w����}�W 
	}
}

void wrcprogram(void)
{
	char i;
	// �n��set CG RAM address (�]�N�O�����M�w�����n�N��Ƽg�bCG RAM�����Ӧ�})
	wr_instruction(0x40);
	// ���U�Ӽg�Jdata�ɡA�~�|�Ndata�g��CG RAM����}�W (�n�g�J����}���w�b�W�������w���]�w�F) 
	for(i=0;i<24;i++)
		wr_data(pattern[i]);
}
	
