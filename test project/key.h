#include <AT89X52.h>
//定義鍵值
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

void InitKeyboard(void);	         //初始化鍵盤設定
void keypad(void);                   //掃描4x4鍵盤，看是否有按鍵被按下
extern unsigned int KEY;			 //2 byte的變數，每一個bit代表哪個鍵盤被按下(若被按下時該bit為1，否則為0)
extern char pressKey;			     //pressKey變數代表目前鍵值
extern volatile bit KeyCanChange;    //該變數用來決定是否要掃描鍵盤




