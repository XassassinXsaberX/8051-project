#include <AT89X52.h>
//�w�q���
#define NOPRESS 0
#define KEY0 1
#define KEY1 2
#define KEY2 3
#define KEY3 4
#define KEY4 5
#define KEY5 6
#define KEY6 7
#define KEY7 8
#define KEY8 9
#define KEY9 10
#define KEY10 11
#define KEY11 12
#define KEY12 13
#define KEY13 14
#define KEY14 15
#define KEY15 16

void InitKeyboard(void);	         //��l����L�]�w
void keypad(void);                   //���y4x4��L�A�ݬO�_������Q���U
extern unsigned int KEY;			 //2 byte���ܼơA�C�@��bit�N�������L�Q���U(�Y�Q���U�ɸ�bit��1�A�_�h��0)
extern char pressKey;			     //pressKey�ܼƥN��ثe���
extern volatile bit KeyCanChange;    //���ܼƥΨӨM�w�O�_�n���y��L




