#include "key.h"

unsigned int KEY;			   //2 byte���ܼơA�C�@��bit�N�������L�Q���U(�Y�Q���U�ɸ�bit��1�A�_�h��0)
char pressKey = NOPRESS;       //�̫�T�w�O������key	   (�������u���{�H)
char currentKey = NOPRESS;     //�ثe�O�����������key
char previousKey = NOPRESS;    //�W���O������key
volatile bit KeyCanChange;     //���ܼƥΨӨM�w�O�_�n���y��L

void InitKeyboard(void)		   //���y��L��l�{��(�Q��timer0 interrupt�A�C�j�@�q�ɶ�(50ms)�A��o��timer0 interrupt�ɡA�N���y������L)
{
	TMOD &= 0xf0;              //�ڭ̱��U�ӭn�]�wTimer/Counter Mode Control register���C4bit
	TMOD |= 0x01;              //timer0 �u�@�bmode0
	TH0 = (65536-500000)/256;  //�]�wtimer0 (�O��50ms)
	TL0	= (65536-500000)%256;  //�]�wtimer0
	EA = 1;				       //enable all inerrupt
	ET0 = 1;			       //enable timer0 interrupt
	TR0 = 1;                   //�Ұ�timre0
}

void keypad(void)		    //4x4��L���y�{��
{
	currentKey = NOPRESS;   //�Y���ܼƬ�NOPRESS�N��S������Q���U�A�Y����NOPRESS�N������Q���U
	KEY = 0;		        //KEY�ܼ��`�@��16��bit�A���O�N��O���@�ӫ��s�Q����

	P0 = 0xef;
	if(P0_0 == 0) 
	{
		currentKey	= KEY0;
		KEY |= 1<<0;	//�NKEY���� 0 bit�]��1(�N���0�ӫ��s�Q���U)
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY1;
		KEY |= 1<<1;    //�NKEY���� 1 bit�]��1(�N���1�ӫ��s�Q���U)
	}
	if(P0_2 == 0) 
	{
		currentKey	= KEY2;
		KEY |= 1<<2;	//�NKEY���� 2 bit�]��1(�N���2�ӫ��s�Q���U)
	}
	if(P0_3 == 0)
	{
		currentKey	= KEY3;
		KEY |= 1<<3;    //�NKEY���� 3 bit�]��1(�N���3�ӫ��s�Q���U)
	}
	
	P0 = 0xdf;
	if(P0_0 == 0)
	{
		currentKey	= KEY4;   
		KEY |= 1<<4;    //�NKEY���� 4 bit�]��1(�N���4�ӫ��s�Q���U)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY5;
		KEY |= 1<<5;    //�NKEY���� 5 bit�]��1(�N���5�ӫ��s�Q���U)
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY6;
		KEY |= 1<<6;	//�NKEY���� 6 bit�]��1(�N���6�ӫ��s�Q���U)	
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY7;
		KEY |= 1<<7;    //�NKEY���� 7 bit�]��1(�N���7�ӫ��s�Q���U)	
	}
	
	P0 = 0xbf;
	if(P0_0 == 0)
	{
		currentKey	= KEY8;
		KEY |= 1<<8;	//�NKEY���� 8 bit�]��1(�N���8�ӫ��s�Q���U)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY9;
		KEY |= 1<<9;	//�NKEY���� 9 bit�]��1(�N���9�ӫ��s�Q���U)	
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY10;
		KEY |= 1<<10;   //�NKEY���� 10 bit�]��1(�N���10�ӫ��s�Q���U)		
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY11;
		KEY |= 1<<11;	//�NKEY���� 11 bit�]��1(�N���11�ӫ��s�Q���U)	
	};
	
	P0 = 0x7f;
	if(P0_0 == 0)
	{
		currentKey	= KEY12;
		KEY |= 1<<12;	//�NKEY���� 12 bit�]��1(�N���12�ӫ��s�Q���U)	
	}
	if(P0_1 == 0) 
	{
		currentKey	= KEY13;
		KEY |= 1<<13;	//�NKEY���� 13 bit�]��1(�N���13�ӫ��s�Q���U)
	}
	if(P0_2 == 0)
	{
		currentKey	= KEY14;
		KEY |= 1<<14;	//�NKEY���� 14 bit�]��1(�N���14�ӫ��s�Q���U)	
	}
	if(P0_3 == 0) 
	{
		currentKey	= KEY15;
		KEY |= 1<<15;	//�NKEY���� 15 bit�]��1(�N���15�ӫ��s�Q���U)	
	}

	//�H�U���{���X�O���F������L������u��
	//�p�G�⦸���y�o�쪺��ȳ��ۦP�ɡA�~��T�w�ثe���u�����
	if(currentKey != previousKey)	   
		previousKey = currentKey;
	else
		 pressKey = currentKey;
}

void timer0_int(void) interrupt 1
{
	//�n���s�]�wtimer0
	TH0 = (65536-500000)/256; //�]�wtimer0 (�O��50ms)
	TL0	= (65536-500000)%256; //�]�wtimer0
	if(KeyCanChange)	      //�Y�i�H���y��L
		keypad();

}