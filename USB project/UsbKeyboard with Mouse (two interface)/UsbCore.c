#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"
#include <INTRINS.h>

idata unsigned char Buffer[16]; //讀取 endpoint 0用的緩衝區

unsigned char *sendPtr;   //指向接下來要傳送的資料
unsigned int SendLength;  //代表還要送多少byte的資料
bit SendZeroPacket;       //若為1代表要送data byte數目為0的data packet，若為0則代表不用送
bit Ep1InIsBusy;
unsigned char ConfigValue;



code unsigned char DeviceDescriptor[0x12]=  //device descriptor為18byte 
{
	//bLength字段。device descriptor的長度為18(0x12)byte 
	0x12,
	
	//bDescriptorType字段。device descriptor的編號為0x01 
	0x01,
	
	//bcdUSB字段。這裡設置版本為USB1.1，即0x0110。 
	//由於是little endian結構，所以低byte在前，即0x10、0x01。 
	0x10,
	0x01,

	//bDeviceClass字段。我們不在device descriptor中定義device class，
	//而在interface descriptor中定義device class，所以該字段的值為0。
	0x00,
	
	//bDeviceSubClass字段。bDeviceClass字段為0時，該字段也為0。
	0x00,
	
	//bDeviceProtocol字段。bDeviceClass字段為0時，該字段也為0。
	0x00,
	
	//bMaxPacketSize0字段。PDIUSBD12的endpoint 0大小的16byte。
	0x10,
	
	//idVender字段。Vendor ID，我們這裡取0x8888，僅供實驗用。
	//實際產品不能隨便使用Vendor ID，必須跟USB協會申請Vendor ID。
	//注意little endian模式，低byte在前。
	0x88,
	0x88,
	
	//idProduct字段。Product ID，由於是第四個實驗，我們這裡取0x0004。
	//注意little endian模式，低byte應該在前。
	0x04,
	0x00,
	
	//bcdDevice字段。我們這個USB滑鼠鍵盤剛開始做，就叫它1.0版吧，即0x0100。
	//little endian模式，低byte在先。
	0x00,
	0x01,
	
	//iManufacturer字段。vendor string的index，為了方便記憶和管理，
	//string index就從1開始吧。
	0x01,
	
	//iProduct字段。produce sting的index。剛剛用了1，這裡就取2吧。
	//注意string index不要使用相同的值。
	0x02,
	
	//iSerialNumber字段。device's serial number string的index。
	//這裡取3就可以了。
	0x03,
	
	///bNumConfigurations字段。該設備所具有的configuration數。
	//我們只需要一種configuration就行了，因此該值設置為1。
	0x01
};

//USB report descriptor的定義
code unsigned char KeyboardReportDescriptor[]=
{

	//每行開始的第一個byte為該item的prefix，prefix的格式為：
	//D7~D4：bTag。D3~D2：bType；D1~D0：bSize。以下分別對每個item註釋。
	
	//這是一個 global item (bType為1)，將usage page選擇為普通桌面Generic Desktop Page(0x01)
	//後面跟一個byte數據 (bSize為1)，後面的byte數目就不註釋了，
	//自己根據bSize來判斷。
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	
	//這是一個 local item (bType為2)，說明接下來的集合用途用於鍵盤
	0x09, 0x06, // USAGE (Keyboard)
	
	//這是一個 main item (bType為0)，開集合，後面跟的數據0x01表示
	//該collection是一個application collection。它的性質在前面由usage page和usage定義為
	//普通桌面用的鍵盤。
	0xa1, 0x01, // COLLECTION (Application)

		//report ID，這裡定義鍵盤report的ID為1 (report ID 0是保留的)
		0x85,0x01,	// Report ID 1
	
		//這是一個 global item，選擇 usage page為鍵盤（Keyboard/Keypad(0x07)）
		0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
		
		//這是一個 local item，說明usage的最小值為0xe0。實際上是鍵盤左Ctrl鍵。
		//具體的usage value可在HID usage table中查看。
		0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
		
		//這是一個 local item，說明usage的最大值為0xe7。實際上是鍵盤右GUI鍵。
		0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
		
		//這是一個 global item，說明返回的數據的邏輯值（就是我們返回的data field的值）
		//最小為0。因為我們這裡用Bit來表示一個data field，因此最小為0，最大為1。
		0x15, 0x00, // LOGICAL_MINIMUM (0)
		
		//這是一個 glabal item，說明邏輯值最大為1。
		0x25, 0x01, // LOGICAL_MAXIMUM (1)
		
		//這是一個 global item，說明data field的數量為八個。
		0x95, 0x08, // REPORT_COUNT (8)
		
		//這是一個 global item，說明每個data field的長度為1個bit。
		0x75, 0x01, // REPORT_SIZE (1)
		
		//這是一個 main item，說明有8個長度為1bit的data field（數量和長度由前面的兩個golbal item所定義）用來做為輸入，
		//屬性為：Data,Var,Abs。Data表示這些數據可以變動
		//Var表示這些數據域是獨立的，每個域表示一個意思。
		//Abs表示絕對值。
		//這樣定義的結果就是，當某個域的值為1時，就表示對應的鍵按下。
		//bit0就對應著Usage最小值0xe0，bit7對應著Usage最大值0xe7。
		0x81, 0x02, // INPUT (Data,Var,Abs)
		
		//這是一個 global item，說明data field數量為1個
		0x95, 0x01, // REPORT_COUNT (1)
		
		//這是一個 global item，說明每個data field的長度為8bit。
		0x75, 0x08, // REPORT_SIZE (8)
		
		//這是一個 main item，輸入用，由前面兩個main item可知，長度為8bit，
		//數量為1個。它的屬性為constant（即返回的數據一直是0）。
		//該byte是保留byte（保留給OEM使用）。
		0x81, 0x03, // INPUT (Cnst,Var,Abs)
		
		//這是一個 global item。定義bit field數量為6個。
		0x95, 0x06, // REPORT_COUNT (6)
		
		//這是一個 global item。定義每個bit field長度為8bit。
		//其實這裡這個item不要也是可以的，因為在前面已經有一個定義
		//長度為8bit的global item了。
		0x75, 0x08, // REPORT_SIZE (8)
		
		//這是一個 global item，定義邏輯最小值為0。
		//同上，這裡這個global item也是可以不要的，因為前面已經有一個
		//定義邏輯最小值為0的global item了。
		0x15, 0x00, // LOGICAL_MINIMUM (0)
		
		//這是一個 global item，定義邏輯最大值為255。
		0x25, 0xFF, // LOGICAL_MAXIMUM (255)
		
		//這是一個 global item，選擇usage page為鍵盤。
		//前面已經選擇過usage page為鍵盤了，所以該item不要也可以。
		0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
		
		//這是一個 local item，定義usage最小值為0（0表示沒有鍵按下）
		0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
		
		//這是一個 local item，定義usage最大值為0x65
		0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
		
		//這是一個 main item。它說明這六個8bit的data field是輸入用的，
		//屬性為：Data,Ary,Abs。Data說明數據是可以變的
		//Ary說明這些數據域是一個array，即每個8bit都可以表示某個鍵值，
		//如果按下的鍵太多（例如超過這裡定義的長度或者鍵盤本身無法
		//掃描出按鍵情況時），則這些數據返回全1（二進制），表示按鍵無效。
		//Abs表示這些值是絕對值。
		0x81, 0x00, // INPUT (Data,Ary,Abs)
		
		
		//以下為output report的描述
		//邏輯最小值前面已經有定義為0了，這裡可以省略。
		//這是一個 global item，說明邏輯值最大為1。
		0x25, 0x01, // LOGICAL_MAXIMUM (1)
		
		//這是一個 global item，說明data field數量為5個。 
		0x95, 0x05, // REPORT_COUNT (5)
		
		//這是一個 global item，說明data field的長度為1bit。
		0x75, 0x01, // REPORT_SIZE (1)
		
		//這是一個 global item，說明使用的usage page為指示燈（LED）
		0x05, 0x08, // USAGE_PAGE (LEDs)
		
		//這是一個 local item，說明usage最小值為數字鍵盤燈。
		0x19, 0x01, // USAGE_MINIMUM (Num Lock)
		
		//這是一個 local item，說明usage最大值為Kana燈。
		0x29, 0x05, // USAGE_MAXIMUM (Kana)
		
		//這是一個 main item。定義輸出數據，即前面定義的5個LED。
		0x91, 0x02, // OUTPUT (Data,Var,Abs)
		
		//這是一個 global item。定義bit field數量為1個。
		0x95, 0x01, // REPORT_COUNT (1)
		
		//這是一個 global item。定義bit field長度為3bit。
		0x75, 0x03, // REPORT_SIZE (3)
		
		//這是一個 main item，定義輸出constant，前面用了5bit，所以這裡需要
		//3個bit來湊成一個byte。
		0x91, 0x01, // OUTPUT (Cnst,Var,Abs)
		
	
	//下面這個 main item用來關閉前面的collection。bSize為0，所以後面沒數據。
	0xc0, // END_COLLECTION
	

};

code unsigned char MouseReportDescriptor[]=
{
	//這是一個global item(bType為1)，選擇Usage page為普通桌面Generic Desktop Page(0x01)
	//後面跟 1 byte的數據（bSize為1），後面的byte數就不註釋了，
	//自己根據bSize來判斷。
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	
	//這是一個local item(bType為2)，說明接下來的application collection用途用於鼠標
	0x09, 0x02, // USAGE (Mouse)
	
	//這是一個main item(bType為0)，開集合，後面跟的數據0x01表示
	//該collection是一個application collection。它的性質在前面由Usage page和Usage定義為
	//普通桌面用的滑鼠。
	0xa1, 0x01, // COLLECTION (Application)

		//report ID，這裡定義鍵盤report的ID為1 (report ID 0是保留的)
		0x85,0x01,  // Report ID 1
	
		//這是一個local item。說明Usage為pointer collection
		0x09, 0x01, // USAGE (Pointer)
		
		//這是一個main item，開集合，後面跟的數據0x00表示該collection是一個
		//physical collection，Usage由前面的local item定義為pointer collection。
		0xa1, 0x00, // COLLECTION (Physical)
		
			//這是一個global item，選擇Usage page為按鍵（Button Page(0x09)）
			0x05, 0x09, // USAGE_PAGE (Button)
			
			//這是一個local item，說明Usage的最小值為1。實際上是滑鼠左鍵。
			0x19, 0x01, // USAGE_MINIMUM (Button 1)
			
			//這是一個local item，說明Usage的最大值為3。實際上是滑鼠中鍵。
			0x29, 0x03, // USAGE_MAXIMUM (Button 3)
			
			//這是一個global item，說明返回的數據的邏輯值（就是我們返回的數據域的值啦）
			//最小為0。因為我們這裡用Bit來表示一個數據域，因此最小為0，最大為1。
			0x15, 0x00, // LOGICAL_MINIMUM (0)
			
			//這是一個global item，說明邏輯值最大為1。
			0x25, 0x01, // LOGICAL_MAXIMUM (1)
			
			//這是一個global item，說明數據域的數量為三個。
			0x95, 0x03, // REPORT_COUNT (3)
			
			//這是一個global item，說明每個數據域的長度為1個bit。
			0x75, 0x01, // REPORT_SIZE (1)
			
			//這是一個main item，說明有3個長度為1bit的數據域（數量和長度
			//由前面的兩個global item所定義）用來做為輸入，
			//屬性為：Data,Var,Abs。Data表示這些數據可以變動，Var表示
			//這些數據域是獨立的，每個域表示一個意思。Abs表示絕對值。
			//這樣定義的結果就是，第一個數據域bit0表示按鍵1（左鍵）是否按下，
			//第二個數據域bit1表示按鍵2（右鍵）是否按下，第三個數據域bit2表示
			//按鍵3（中鍵）是否按下。
			0x81, 0x02, // INPUT (Data,Var,Abs)
			
			//這是一個global item，說明數據域數量為1個
			0x95, 0x01, // REPORT_COUNT (1)
			
			//這是一個global item，說明每個數據域的長度為5bit。
			0x75, 0x05, // REPORT_SIZE (5)
			
			//這是一個main item，輸入用，由前面兩個global item可知，長度為5bit，
			//數量為1個。它的屬性為constant（即返回的數據一直是0）。
			//這個只是為了湊齊一個byte（前面用了3個bit）而填充的一些數據
			//而已，所以它是沒有實際用途的。
			0x81, 0x03, // INPUT (Cnst,Var,Abs)
			
			//這是一個global item，選擇Usage page為普通桌面Generic Desktop Page(0x01)
			0x05, 0x01, // USAGE_PAGE (Generic Desktop)
			
			//這是一個local item，說明Usage為X軸
			0x09, 0x30, // USAGE (X)
			
			//這是一個local item，說明Usage為Y軸
			0x09, 0x31, // USAGE (Y)
			
			//這是一個local item，說明Usage為滾輪
			0x09, 0x38, // USAGE (Wheel)
			
			//下面兩個為global item，說明返回的邏輯最小和最大值。
			//因為鼠標指針移動時，通常是用相對值來表示的，
			//相對值的意思就是，當游標移動時，只發送移動量。
			//往右移動時，X值為正；往下移動時，Y值為正。
			//對於滾輪，當滾輪往上滾時，值為正。
			0x15, 0x81, // LOGICAL_MINIMUM (-127)
			0x25, 0x7f, // LOGICAL_MAXIMUM (127)
			
			//這是一個global item，說明數據域的長度為8bit。
			0x75, 0x08, // REPORT_SIZE (8)
			
			//這是一個global item，說明數據域的個數為3個。
			0x95, 0x03, // REPORT_COUNT (3)
			
			//這是一個main item。它說明這三個8bit的數據域是輸入用的，
			//屬性為：Data,Var,Rel。Data說明數據是可以變的，Var說明
			//這些數據域是獨立的，即第一個8bit表示X軸，第二個8bit表示
			//Y軸，第三個8bit表示滾輪。Rel表示這些值是相對值。
			0x81, 0x06, // INPUT (Data,Var,Rel)
			
		//下面這兩個main item用來關閉前面的collection用。
		//我們開了兩個collection，所以要關兩次。bSize為0，所以後面沒數據。
		0xc0, // END_COLLECTION
	0xc0 // END_COLLECTION	
};
///////////////////////////report descriptor完畢////////////////////////////


//USB配置描述符集合的定義
//configuration descriptor總長度為9+9+9+7+7+9+9+7 byte
code unsigned char ConfigurationDescriptor[9+9+9+7+7+9+9+7]=
{
 /***************configuration descriptor***********************/
	//bLength字段。configuration descriptor的長度為9byte。
	0x09,
	
	//bDescriptorType字段。configuration descriptor編號為0x02。
	0x02,
	
	//wTotalLength字段。配置描述符集合的總長度，
 	//包括configuration descriptor本身、interface descriptor、class descriptor、endpoint descriptor等。
	sizeof(ConfigurationDescriptor)&0xFF,      //低byte
	(sizeof(ConfigurationDescriptor)>>8)&0xFF, //高byte 
	
	//bNumInterfaces字段。該configuration包含的interface數，有兩個interface。
	0x02,
	
	//bConfiguration字段。該configuration的值為1。
	0x01,
	
	//iConfigurationz字段，該configuration的string index。這裡沒有，為0。
	0x00,
	
	//bmAttributes字段，該設備的屬性。由於我們的板子是bus供電的，
 	//並且我們不想實現remote wakeup的功能，所以該字段的值為0x80。
	0x80,
	
	//bMaxPower字段，該設備需要的最大電流量。由於我們的板子
 	//需要的電流不到100mA，因此我們這裡設置為100mA。由於每單位
 	//電流為2mA，所以這裡設置為50(0x32)。
	0x32,
	
	/*******************第一個interface descriptor*********************/
	//bLength字段。interface descriptor的長度為9byte。
	0x09,
	
	//bDescriptorType字段。interface descriptor的編號為0x04。
	0x04,
	
	//bInterfaceNumber字段。該interface的編號，第一個interface，編號為0。
	0x00,
	
	//bAlternateSetting字段。該interface的備用編號，為0。
	0x00,
	
	//bNumEndpoints字段。non-zero endpoint的數目。該USB鍵盤需要二個interrupt endpoint
	//(一個IN、一個OUT)，因此該值為2。
	0x02,
	
	//bInterfaceClass字段。該interface所使用的class。USB鍵盤是HID class，
	//HID class的編碼為0x03。
	0x03,
	
	//bInterfaceSubClass字段。該interface所使用的subclass。在HID1.1協議中，
	//只規定了一種subclass：支持BIOS引導啟動的subclass。
	//USB鍵盤、滑鼠屬於該subclass，subclass代碼為0x01。
	0x01,
	
	//bInterfaceProtocol字段。如果subclass為支持引導啟動的subclass，
	//則協議可選擇滑鼠和鍵盤。鍵盤代碼為0x01，滑鼠代碼為0x02。
	0x01,
	
	//iConfiguration字段。該interface的string index。這裡沒有，為0。
	0x00,
	
	/******************HID descriptor************************/
	//bLength字段。本HID descriptor下只有一個下級descriptor。所以長度為9 byte。
	0x09,
	
	//bDescriptorType字段。HID descriptor的編號為0x21。
	0x21,
	
	//bcdHID字段。本協議使用的HID1.1協議。注意低byte在先。
	0x10,
	0x01,
	
	//bCountyCode字段。設備適用的國家代碼，這裡選擇為美國，代碼0x21。
	0x21,
	
	//bNumDescriptors字段。下級描述符的數目。我們只有一個report descriptor。
	0x01,
	
	//bDescriptorType字段。下級描述符的類型，為report descriptor，編號為0x22。
	0x22,
	
	//bDescriptorLength字段。下級描述符的長度。下級descroptor為report descriptor。
	sizeof(KeyboardReportDescriptor)&0xFF,
	(sizeof(KeyboardReportDescriptor)>>8)&0xFF,
	
	/**********************IN endpoint descriptor***********************/
	//bLength字段。endpoint descriptor長度為7byte。
	0x07,
	
	//bDescriptorType字段。endpoint descriptor編號為0x05。
	0x05,
	
	//bEndpointAddress字段。endpoint的地址。我們使用D12的IN endpoint 1。
	//D7位表示數據方向，IN endpoint D7為1。所以IN endpoint 1 的地址為0x81。
	0x81,
	
	//bmAttributes字段。D1~D0為endpoint傳輸類型選擇。
	//該endpoint為interrupt endpoint。interrupt endpoint的編號為3。其它bit保留為0。
	0x03,
	
	//wMaxPacketSize字段。該endpoint的最大packet長。endpoint 1 的最大packet長為16byte。
	//注意低byte在先。
	0x10,
	0x00,
	
	//bInterval字段。endpoint polling的時間，我們設置為10個frame時間，即10ms。
	0x0A,

	/**********************OUT endpoint descriptor***********************/
	
	//bLength字段。endpoint descriptor長度為7 byte。
	0x07,
	
	//bDescriptorType字段。endpoint descriptor編號為0x05。
	0x05,
	
	//bEndpointAddress字段。endpoint的地址。我們使用D12的OUT endpoint 1。
	//D7位表示數據方向，OUT endpoint D7為0。所以OUT endpoint 1 的地址為0x01。
	0x01,
	
	//bmAttributes字段。D1~D0為endpoint傳輸類型選擇。
	//該endpoint為interrupt endpoint。interrupt endpoint的編號為3。其它bit保留為0。
	0x03,
	
	//wMaxPacketSize字段。該endpoint的最大packet長。endpoint 1 的最大packet長為16 byte。
	//注意低byte在先。
	0x10,
	0x00,
	
	//bInterval字段。endpoint polling的時間，我們設置為10個frame時間，即10ms。
	0x0A,

	/*******************第二個interface descriptor*********************/
	//bLength字段。interface descriptor的長度為9byte。
	0x09,
	
	//bDescriptorType字段。interface descriptor的編號為0x04。
	0x04,
	
	//bInterfaceNumber字段。該interface的編號，第二個interface，編號為1。
	0x01,
	
	//bAlternateSetting字段。該interface的備用編號，為0。
	0x00,
	
	//bNumEndpoints字段。non-zero endpoint的數目。該USB滑鼠需要ㄧ個interrupt IN endpoint，因此該值為1。
	0x01,
	
	//bInterfaceClass字段。該interface所使用的class。USB鍵盤是HID class，
	//HID class的編碼為0x03。
	0x03,
	
	//bInterfaceSubClass字段。該interface所使用的subclass。在HID1.1協議中，
	//只規定了一種subclass：支持BIOS引導啟動的subclass。
	//USB鍵盤、滑鼠屬於該subclass，subclass代碼為0x01。
	0x01,
	
	//bInterfaceProtocol字段。如果subclass為支持引導啟動的subclass，
	//則協議可選擇滑鼠和鍵盤。鍵盤代碼為0x01，滑鼠代碼為0x02。
	0x02,
	
	//iConfiguration字段。該interface的string index。這裡沒有，為0。
	0x00,
	
	/******************HID descriptor************************/
	//bLength字段。本HID descriptor下只有一個下級descriptor。所以長度為9 byte。
	0x09,
	
	//bDescriptorType字段。HID descriptor的編號為0x21。
	0x21,
	
	//bcdHID字段。本協議使用的HID1.1協議。注意低byte在先。
	0x10,
	0x01,
	
	//bCountyCode字段。設備適用的國家代碼，這裡選擇為美國，代碼0x21。
	0x21,
	
	//bNumDescriptors字段。下級描述符的數目。我們只有一個report descriptor。
	0x01,
	
	//bDescriptorType字段。下級描述符的類型，為report descriptor，編號為0x22。
	0x22,
	
	//bDescriptorLength字段。下級描述符的長度。下級descroptor為report descriptor。
	sizeof(MouseReportDescriptor)&0xFF,
	(sizeof(MouseReportDescriptor)>>8)&0xFF,
	
	/**********************IN endpoint descriptor***********************/
	//bLength字段。endpoint descriptor長度為7byte。
	0x07,
	
	//bDescriptorType字段。endpoint descriptor編號為0x05。
	0x05,
	
	//bEndpointAddress字段。endpoint的地址。我們使用D12的IN endpoint 2。
	//D7位表示數據方向，IN endpoint D7為1。所以IN endpoint 2 的地址為0x81。
	0x82,
	
	//bmAttributes字段。D1~D0為endpoint傳輸類型選擇。
	//該endpoint為interrupt endpoint。interrupt endpoint的編號為3。其它bit保留為0。
	0x03,
	
	//wMaxPacketSize字段。該endpoint的最大packet長。endpoint 2 的最大packet長為64byte。
	//注意低byte在先。
	0x40,
	0x00,
	
	//bInterval字段。endpoint polling的時間，我們設置為10個frame時間，即10ms。
	0x0A,

};
////////////////////////配置描述符集合完畢//////////////////////////

/************************Language ID的定義********************/
code unsigned char LanguageId[4]=
{
	0x04,     //本descriptor的長度
	0x03,     //descriptor type為string descriptor
	//0x0409為美式英語的ID
	0x09,
	0x04
};
////////////////////////Language ID完畢//////////////////////////////////

//可參考此網站的unicode轉換
//https://www.chineseconverter.com/zh-tw/convert/unicode 

//字串為"歡迎參觀我的github:XassassinXsaberX" 
//8bit的little endian格式 
code unsigned char ManufacturerStringDescriptor[60]={
	60,         //該descriptor的長度為60 byte
	0x03,       //string descriptor的類型編碼為0x03
	0x61,0xb6,  //歡
	0xce,0x8f,  //迎 
	0xc3,0x53,  //參 
	0xc0,0x89,  //觀 
	0x11,0x62,  //我 
	0x84,0x76,  //的 
	0x67,0x00,  //g
	0x69,0x00,  //i
	0x74,0x00,  //t
	0x68,0x00,  //h
	0x75,0x00,  //u
	0x62,0x00,  //b
	0x3a,0x00,  //:
	0x58,0x00,  //X
	0x61,0x00,  //a
	0x73,0x00,  //s
	0x73,0x00,  //s
	0x61,0x00,  //a
	0x73,0x00,  //s
	0x73,0x00,  //s
	0x69,0x00,  //i
	0x6e,0x00,  //n
	0x58,0x00,  //X
	0x73,0x00,  //s
	0x61,0x00,  //a
	0x62,0x00,  //b
	0x65,0x00,  //e
	0x72,0x00,  //r
	0x58,0x00   //X
};
/////////////////////////vendor string結束/////////////////////////////

//字串"phisoner的複合式鍵盤滑鼠" 
//8bit little endian格式
code unsigned char ProductStringDescriptor[34]={
	34,         //該descriptor的長度為34 byte
	0x03,       //string descriptor的類型編碼為0x03
	0x70,0x00,  //p
	0x68,0x00,  //h
	0x69,0x00,  //i
	0x73,0x00,  //s
	0x6f,0x00,  //o
	0x6e,0x00,  //n
	0x65,0x00,  //e
	0x72,0x00,  //r
	0x84,0x76,  //的
	0x07,0x89,  //複
	0x08,0x54,  //合
	0x0f,0x5f, 	//式
	0x75,0x93,  //鍵 
	0xe4,0x76,  //盤
	0xd1,0x6e,	//滑
	0x20,0x9f	//鼠
};
////////////////////////product string結束////////////////////////////


//字串為“2017-12-06”的Unicode編碼
//8 bit的little endian格式
code unsigned char SerialNumberStringDescriptor[22]={
	22,         //該descriptor的長度為22 byte
	0x03,       //string descriptor的類型編碼為0x03
	0x32, 0x00, //2
	0x30, 0x00, //0
	0x31, 0x00, //1
	0x37, 0x00, //7
	0x2d, 0x00, //-
	0x31, 0x00, //1
	0x32, 0x00, //2
	0x2d, 0x00, //-
	0x30, 0x00, //0
	0x36, 0x00  //6
};
//////////////////////device's serial number string結束/////////////////////////


void DelayXms(unsigned int t)	 //delay t ms
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<120;i++);
}

void DelayXus(unsigned int t)	 //delay t us
{
	for(;t>0;t--)
		_nop_();
}

void UsbDisconnect(void) //USB斷開連接 , interrupt處理函數
{
 	#ifdef DEBUG0
		Prints("USB disconnect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);		//寫入Set Mode command到USB晶片中 (該command需要再輸入2byte資料)
	D12WriteByte(0x06);		            //寫入第一個byte的資料到USB晶片中
	D12WriteByte(0x47);				    //寫入第二個byte的資料到USB晶片中
	DelayXms(1000);                     //delay 1ms (使主機確認設備已經斷開連結)
}

void UsbConnect(void)    //USB連接 , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB connect\n");
	#endif
	D12WriteCommand(D12_SET_MODE);	  	//寫入Set Mode command到USB晶片中 (該command需要再輸入2byte資料)
	D12WriteByte(0x16);		            //寫入第一個byte的資料到USB晶片中
	D12WriteByte(0x47);				    //寫入第二個byte的資料到USB晶片中
}

void UsbBusSuspend(void) //bus suspend , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB Bus Suspend\n");
	#endif
}

void UsbBusReset(void)   //bus reset , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB Bus Reset\n");
	#endif
	Ep1InIsBusy = 0;
} 

void UsbEp0Out(void)     //endpoint 0 OUT , interrupt處理函數 
{
	char status;
	int i;

	//以下為standard device request的各個field
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned int wValue;
	unsigned int wIndex;
	unsigned int wLength;

	#ifdef DEBUG0
		Prints("USB endpoint 0 OUT interrupt\n");	
	#endif

	status = D12ReadEndpointLastStatus(0);	    //"讀取endpoint 0 OUT最後ㄧ次transaction的狀態" 的command
	                                            //該命令可清除interrupt register中，每一個interrupt對應到的interrupt flag
                                                //將讀取到的 "最後ㄧ次transaction的狀態資料" 存到status變數中	
									 
	if((status>>5) & 0x01)	                    //如果最後ㄧ次transaction為setup stage
	{
		for(i=0;i<16;i++)
			Buffer[i] = 0;
		D12ReadEndpointBuffer(0, 16, Buffer);	//讀取指定endpoint的buffer
		D12AcknowledgeSetup();	                //Acknowledge Setup
		D12ClearBuffer(0);			            //清空endpoint 0 OUT的buffer
		
		
		//接下來決定USB standard device request的各個field
		//收到Buffer[0] Buffer[1] Buffer[2] Buffer[3] Buffer[4] Buffer[5] Buffer[6] Buffer[7]
		bmRequestType = Buffer[0];
		bRequest = Buffer[1];
		wValue = Buffer[3];	     	//採little endian格式，所以wValue為Buffer[3] Buffer[2]
		wValue <<= 8;
		wValue |= Buffer[2]; 
		wIndex = Buffer[5];		    //採little endian格式，所以wIndex為Buffer[5] Buffer[4]
		wIndex <<= 8;
		wIndex |= Buffer[4]; 
		wLength = Buffer[7];		//採little endian格式，所以wLength為Buffer[7] Buffer[6]
		wLength <<= 8;
		wLength |= Buffer[6];
		
		if((bmRequestType>>7)&0x01)	 //IN request
		{
			if(((bmRequestType>>5)&0x03) == 0)       //standard request
			{
				
				#ifdef DEBUG0
					Prints("USB standard IN request : ");
				#endif
				if(bRequest == GET_CONFIGURATION)
				{
					#ifdef DEBUG0
						Prints("GET_CONFIGURATION\n");
					#endif
				}
				else if(bRequest == GET_DESCRIPTOR)
				{
					#ifdef DEBUG0
						Prints("GET_DESCRIPTOR---");
					#endif
					if(((wValue>>8)&0xff) == DEVICE_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("device descriptor\n");
						#endif	
						if(wLength > DeviceDescriptor[0])		//決定要送幾個byte
							SendLength =  DeviceDescriptor[0];  
						else
							SendLength = wLength;

						sendPtr = DeviceDescriptor;			    //決定要送哪些資料

						UsbEp0SendData();                       //透過endpoint 0 IN送出資料
					}
					else if(((wValue>>8)&0xff) == CONFIGURATION_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("configuration descriptor\n");
						#endif	
						if(wLength > sizeof(ConfigurationDescriptor) )		//決定要送幾個byte
							SendLength =  sizeof(ConfigurationDescriptor);  
						else
							SendLength = wLength;

						sendPtr = ConfigurationDescriptor;		         	//決定要送哪些資料

						UsbEp0SendData();		                            //透過endpoint 0 IN送出資料

					}
					else if(((wValue>>8)&0xff) == STRING_DESCRIPTOR)
					{
						
						#ifdef DEBUG0
							Prints("string descriptor\n");
						#endif	
						
						if((wValue & 0xff) == 0)
						{
							//Prints("Index 0\n");				   //決定要送幾個byte
							if(wLength > sizeof(LanguageId) )
								SendLength =  sizeof(LanguageId);  
							else
								SendLength = wLength;
							
							sendPtr = LanguageId;			       //決定要送哪些資料	
						}
						else if((wValue & 0xff) == 1)
						{
							//Prints("Index 1\n");
							if(wLength > sizeof(ManufacturerStringDescriptor) )		 //決定要送幾個byte
								SendLength =  sizeof(ManufacturerStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ManufacturerStringDescriptor;			         //決定要送哪些資料
						}
						else if((wValue & 0xff) == 2)
						{
							//Prints("Index 2\n");
							if(wLength > sizeof(ProductStringDescriptor) )		//決定要送幾個byte
								SendLength =  sizeof(ProductStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = ProductStringDescriptor;		            //決定要送哪些資料
						
						}
						else if((wValue & 0xff) == 3)
						{
							//Prints("Index 3\n");
							if(wLength > sizeof(SerialNumberStringDescriptor) )		 //決定要送幾個byte
								SendLength =  sizeof(SerialNumberStringDescriptor);  
							else
								SendLength = wLength;
	
							sendPtr = SerialNumberStringDescriptor;			         //決定要送哪些資料
						}
						else
						{
							Prints("Index error\n");
						}								                          //透過endpoint 0 IN送出資料
						UsbEp0SendData();
					}
					else if(((wValue>>8)&0xff) == INTERFACE_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("interface descriptor\n");
						#endif	
					}
					else if(((wValue>>8)&0xff) == ENDPOINT_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("endpoint descriptor\n");
						#endif	
					}
					else if(((wValue>>8)&0xff) == REPORT_DESCRIPTOR)
					{
						#ifdef DEBUG0
							Prints("report descriptor\n");
						#endif	
						//GET_DESCRIPTOR(report descriptor)時，wIndex中保存的是request的interface編號
						//此處有兩個interface，interface 0 用來時現鍵盤功能、interface 1 用來時現鍵盤功能
						if(wIndex == 0)	     //鍵盤interface
						{
							if(wLength > sizeof(KeyboardReportDescriptor) )
									SendLength =  sizeof(KeyboardReportDescriptor);  //決定要送幾個byte
							else
								SendLength = wLength;
	
							sendPtr = KeyboardReportDescriptor;			             //決定要送哪些資料	
						}
						else if(wIndex == 1) //滑鼠interface
						{
							if(wLength > sizeof(MouseReportDescriptor) )
									SendLength =  sizeof(MouseReportDescriptor);     //決定要送幾個byte
							else
								SendLength = wLength;
	
							sendPtr = MouseReportDescriptor;			             //決定要送哪些資料	
						}
						else                  //其他為定義的interface，device回傳data byte數目為0的data packet給host
						{
							SendLength = 0;
						}
						UsbEp0SendData();
					}
					else
					{
						#ifdef DEBUG0
							Prints("unknown descriptor\n");
						#endif	
					}
				}
				else if(bRequest == GET_INTERFACE)
				{
					#ifdef DEBUG0
						Prints("GET_INTERFACE\n");
					#endif
				}
				else if(bRequest == GET_STATUS)
				{
					#ifdef DEBUG0
						Prints("GET_STATUS\n");
					#endif
				}
				else if(bRequest == SYNCH_FRAME)
				{
					#ifdef DEBUG0
						Prints("SYNCH_FRAME\n");
					#endif
				}
				else
				{
					#ifdef DEBUG0
						Prints("error:undefined standard IN request\n");
					#endif
				}
			}
			else if(((bmRequestType>>5)&0x03) == 1)  //class request
			{
				#ifdef DEBUG0
					Prints("USB class IN request : \n");
				#endif
			}
			else if(((bmRequestType>>5)&0x03) == 2)  //vendor request
			{
				#ifdef DEBUG0
					Prints("USB vendor IN request : \n");
				#endif
			}
			else				                     //reserved
			{
				#ifdef DEBUG0
					Prints("error:undefined IN request\n");
				#endif
			}
		}
		else  //OUT request
		{
			if(((bmRequestType>>5)&0x03) == 0)       //standard request
			{
				#ifdef DEBUG0
					Prints("USB standard OUT request : ");
				#endif
				if(bRequest == CLEAR_FEATURE)
				{
					#ifdef DEBUG0
						Prints("CLEAR_FEATURE\n");
					#endif	
				}
				else if(bRequest == SET_ADDRESS)
				{
					#ifdef DEBUG0
						Prints("SET_ADDRESS\n");
					#endif
					//當接收到此standard request時，目的是要為此USB晶片設置host回傳給他的device address
					D12WriteCommand(D12_SET_ADDRESS_ENABLE);
					//此standard request中 wValue的低 7 bit值為device address
					D12WriteByte(0x80 | (wValue & 0x7f));		//第7 bit設為1代表enable the function(function在USB中指的就像是設備)

					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
					SendLength = 0;
					UsbEp0SendData();
				}
				else if(bRequest == SET_CONFIGURATION)
				{
					#ifdef DEBUG0
						Prints("SET_CONFIGURATION\n");
					#endif	
				   
				    //當收到此standard request時，目的是確定此configuration是否正確，若正確的話就enable all non-zero endpoint
					//所以接下來要寫入Set Endpoint Enable command，目的是要enable或disable non-zero endpoint
					D12WriteCommand(D12_SET_ENDPOINT_ENABLE);
					ConfigValue = wValue & 0xff; //此standard request中 wValue的低byte值為configuration 

					//若此configuration的值非0時，代表為正確的configuration (可參考configuration descriptor中的bConfiguration，我們把configuration value設為1)
					//此時才能enable non-zero endpoint，否則disable non-zero endpoint
					if(ConfigValue)             
						D12WriteByte(0x01);	  //enable non-zero endpoint
					else
						D12WriteByte(0x00);   //disable non-zero endpoint

					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
					SendLength = 0;
					UsbEp0SendData();
				}
				else if(bRequest == SET_DESCRIPTOR)
				{
					#ifdef DEBUG0
						Prints("SET_DESCRIPTOR\n");
					#endif	
				}
				else if(bRequest == SET_FEATURE)
				{
					#ifdef DEBUG0
						Prints("SET_FEATURE\n");
					#endif	
				}
				else if(bRequest == SET_INTERFACE)
				{
					#ifdef DEBUG0
						Prints("SET_INTERFACE\n");
					#endif	
				}
				else
				{
					#ifdef DEBUG0
						Prints("error:undefined OUT request\n");
					#endif
				}
			}
			else if(((bmRequestType>>5)&0x03) == 1)  //class request
			{
				#ifdef DEBUG0
					Prints("USB class OUT request : ");
				#endif
				if(bRequest == SET_IDLE)
				{	
					#ifdef DEBUG0
						Prints("SET_IDLE\n");
					#endif
					//回傳ㄧ個data byte數目為0的data packet告知host已成功收到此standard OUT request
					//回傳ㄧ個data byte數目為0的data packet時，代表control transfer中的data stage沒有data (也可以說data stage不存在)
					SendLength = 0;
					UsbEp0SendData();
				}
				else
				{
					Prints("unknown request\n");
				}
			}
			else if(((bmRequestType>>5)&0x03) == 2)  //vendor request
			{
				#ifdef DEBUG0
					Prints("USB vendor OUT request : \n");
				#endif
			}
			else				                   //reserved
			{
				#ifdef DEBUG0
					Prints("error:undefined OUT request\n");
				#endif
			}	
		}
		
	}
	else
	{
		D12ReadEndpointBuffer(0, 16, Buffer);	//讀取指定endpoint的buffer
		D12ClearBuffer(0);			            //清空endpoint 0 OUT buffer
	}

}


//將sendPtr指向的資料送出
void UsbEp0SendData(void)
{
	//SendLength變數代表目前還有幾個byte的資料需要傳輸
	//DeviceDescriptor[7]代表endpoint 0 IN 的buffer容量 (單位為byte)
	if(SendLength >= DeviceDescriptor[7])
		D12WriteEndpointBuffer(1,DeviceDescriptor[7],sendPtr);	
	else
		D12WriteEndpointBuffer(1,SendLength,sendPtr);

}


void UsbEp0In(void)      //endpoint 0 IN , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 0 IN interrupt\n");
	#endif
	
	D12ReadEndpointLastStatus(1);            //讀取endpoint 0 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag

	if(SendLength >= DeviceDescriptor[7])    //SendLength代表上次傳輸時，還需要傳送的資料長度
	{
		sendPtr += DeviceDescriptor[7];		 //可以串送下ㄧ組資料了
		SendLength -= DeviceDescriptor[7];   //SendLength現在代表目前還需要傳送的資料長度，因為上次成功的傳送資料，所以需要傳送資料的長度變少了
		UsbEp0SendData();					 //透過endpoint 0 IN 來送資料
	}
	
} 

void UsbEp1Out(void)     //endpoint 1 OUT , interrupt處理函數
{
	
	unsigned char Buf[2];
	#ifdef DEBUG0
		Prints("USB endpoint 1 OUT interrupt\n");
	#endif

	D12ReadEndpointLastStatus(2);            //讀取endpoint 1 OUT中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag
	D12ClearBuffer(2);						 //清空endpoint 1 OUT的buffer
	switch(D12ReadEndpointBuffer(2,2,Buf))
	{
		case 0:
			Prints("endpoint 1 OUT 收到0 byte的資料\n");
			PrintHex(Buf[1]);
			Prints("\n");
			break;
		default:
			Prints("endpoint 1 OUT 收到");
			PrintHex(Buf[1]);
			Prints("\n");

			if(Buf[1] & 0x01)
				Prints("Num Lock燈亮\n");
			else
				Prints("Num Lock燈暗\n");
			if(Buf[1] & 0x02)
				Prints("Caps Lock燈亮\n");
			else
				Prints("Caps Lock燈暗\n");
			break;
	}
	
} 

void UsbEp1In(void)      //endpoint 1 IN , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB endpoint 1 IN interrupt\n");
	#endif
	D12ReadEndpointLastStatus(3);	          //讀取endpoint 1 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag

	Ep1InIsBusy = 0;
} 

void UsbEp2Out(void)     //endpoint 2 OUT , interrupt處理函數 
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 OUT interrupt\n");
	#endif
}

void UsbEp2In(void)      //endpoint 2 IN , interrupt處理函數
{
	#ifdef DEBUG0
		Prints("USB endpoint 2 IN interrupt\n");
	#endif

	D12ReadEndpointLastStatus(5);	          //讀取endpoint 2 IN中最後ㄧ次transaction的狀態，並清除interrupt register中的所有interrupt flag

	Ep1InIsBusy = 0;
} 