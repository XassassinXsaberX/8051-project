#include <AT89X52.h>
#define RS P1_0
#define RW P1_1
#define E P1_2
#define DB P2

void delayms(unsigned int);  // delay 約1ms
void init(void);             // 在使用LCD前，要先進行初始化
void wr_instruction(char);   // 寫入指令到LCD中
void wr_data(char);          // 寫入資料到LCD中
void position(char,char);    // set DD RAM address
void display(char*);         // 在LCD中顯示該字串
void wrcprogram(void);       // 把自訂的圖形碼送到LCD中

code char pattern[] = {0x00,0x00,0x00,0x1c,  
                       0x1f,0x00,0x00,0x00,  // 機頭的圖形碼
                       0x10,0x0c,0x06,0x1f,
                       0x1f,0x06,0x0c,0x10,  // 機身的圖形碼
                       0x18,0x18,0x18,0x1f,
                       0x1f,0x00,0x00,0x00}; // 機尾的圖形碼


int main()
{
	char s[] = "hello hank";
	char s2[] = {0x0a,0x09,0x08,0x00};
	init();         // 初始化LCD狀態 
	wrcprogram();   // 自訂特殊字元 
	
	// 要寫入資料到 DD RAM上時 
	position(1,1);  // 要先set DD RAM address (也就是說先決定等等要將資料寫在DD RAM的哪個位址) 
	display(s);     // 接下來寫入資料時，就是將該資料寫入至DD RAM的該位址 (要寫入的位址剛剛上一行已設定了) 
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
	//以下進行功能設定
	//設定LCD模組為兩行顯示，8位元傳輸介面，5X7字型
	wr_instruction(0x38);
	wr_instruction(0x38);
	wr_instruction(0x38);
	wr_instruction(0x38);
	
	wr_instruction(0x08); //令顯示器OFF
	wr_instruction(0x01); //清除顯示
	wr_instruction(0x06); //令LCD每收到1byte資料後，AC值自動+1
	wr_instruction(0x0C); //令顯示器ON，但游標不顯示
}

void wr_instruction(char instruction)
{
	// 將指令寫入LCD中
	RS = 0;
	RW = 0;
	
	// 將LCD enable後才能寫入資指令
	E = 1;
	DB = instruction; // 寫入指令
	
	// 將LCD disable 防避免LCD中的資料被不經意的更動
	E = 0;
	
	delayms(8); // 每執行一次寫入動作後都必須等待Busy Flag = 0，或直接延遲一段時間
	            // 才能在執行下個寫入動作
}

void wr_data(char d)
{
	// 將資料寫入LCD中
	RS = 1;
	RW = 0;
	
	// 將LCD enable後才能寫入資指令
	E = 1;
	DB = d; // 寫入資料
	
	// 將LCD disable 防避免LCD中的資料被不經意的更動
	E = 0;
	
	delayms(1); // 每執行一次寫入動作後都必須等待Busy Flag = 0，或直接延遲一段時間
	            // 才能在執行下個寫入動作
}

void position(char row,char col)
{
	unsigned char instruction;
	// set DD RAM address
	// 也就是說會決定等等要將資料寫在DD RAM的哪個位址上
	// Address Counter會存放此位址 
	instruction = 0x80 + (0x40*(row-1) + (col-1)); 
	  
	wr_instruction(instruction);
	// 現在Address Counter(AC)存放此位址 
}

void display(char*s)
{
	unsigned int i;
	for(i=0;;i++)
	{
		if(s[i] == '\0')
			break;
		wr_data(s[i]);  // 將資料寫在Address Counter(AC)所指定的位址上 
	}
}

void wrcprogram(void)
{
	char i;
	// 要先set CG RAM address (也就是說先決定等等要將資料寫在CG RAM的哪個位址)
	wr_instruction(0x40);
	// 接下來寫入data時，才會將data寫到CG RAM的位址上 (要寫入的位址剛剛已在上面的指定中設定了) 
	for(i=0;i<24;i++)
		wr_data(pattern[i]);
}
	
