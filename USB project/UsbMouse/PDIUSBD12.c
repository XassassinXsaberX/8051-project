#include "PDIUSBD12.h"
#include "UsbCore.h"
#include "UART.h"

void D12WriteCommand(unsigned char command)	 //��PSIUSBD12������J"�g�J���O"�A�üg�J1 byte��command���
{
	//�i�Ѧ�PDIUSBD12����datasheet�����ɧǹ�
	D12_A0 = 1;		     //select command instruction
	D12_WR = 0;
	D12_DATA = command;
	D12_WR = 1;
	DelayXus(10);		 //�������q�ɶ��ϸ�ƽT��g�J
}


unsigned char D12ReadByte(void)	//��PSIUSBD12������J"Ū��1 byte ���"�A�æ^��Ū���쪺1 byte���
{
	//�i�Ѧ�PDIUSBD12����datasheet�����ɧǹ�
	unsigned char tmp;
	D12_A0 = 0;			 //select the data phase
	D12_RD = 0;
	tmp = D12_DATA;
	D12_RD = 1;
	return tmp;
}

void D12WriteByte(unsigned char d)	//��PSIUSBD12������J"�g�J1 byte ���"
{
	//�i�Ѧ�PDIUSBD12����datasheet�����ɧǹ�
	D12_A0 = 0;		 	//select the data phase
	D12_WR = 0;
	D12_DATA = d;
	D12_WR = 1;
	DelayXus(10);		//�������q�ɶ��ϸ�ƽT��g�J
}

unsigned int D12ReadID(void)  //�^�ǸӴ�����id
{
	unsigned int tmp;
	D12WriteCommand(Read_ID); //��PSIUSBD12������J"Ū�����O" �A���O��Read_ID(�Y��0xfd�A�N��Ū������id)
	tmp = D12ReadByte();	  //id���C1byte
	tmp |= D12ReadByte()<<8;  //id����1byte
	return tmp;

}

void D12SelectEndpoint(unsigned char endpoint)	 //select endpoint command
{
	D12WriteCommand(endpoint);  //select endpoint command
}

unsigned char D12ReadEndpointBuffer(unsigned char endpoint, unsigned char Len, unsigned char *Buf)   //Ū�����wendpoint��buffer
{
	//��J���ѼƤ�
	//endpoint=0�N�� endpoint 0 OUT �B endpoint=1�N�� endpoint 0 IN
	//endpoint=2�N�� endpoint 1 OUT �B endpoint=3�N�� endpoint 1 IN
	//endpoint=4�N�� endpoint 2 OUT �B endpoint=5�N�� endpoint 2 IN
	//Len�N��Q�nŪ���h��byte�����
	//Buf�h�N��Nendpoint buffer����Ʀs�b���}�C��

	unsigned char real_len;        
	int i;    
	
	D12SelectEndpoint(endpoint);       //�����n���endpoint
	D12WriteCommand(D12_READ_BUFFER);  //�g�J "Ū��endpoint buffer" ���R�O
	//���U�Ӷ}�lŪ�����
	
	D12ReadByte();                     //�Ĥ@��Ū���쪺byte�S���N�q
	real_len = D12ReadByte();	       //�ĤG��Ū���쪺byte�N��endpoint buffer���A��ڤW�s���h��byte�����
	if(real_len > Len)
		real_len = Len;	               
	//�ڭ̹�ڤW�uŪ��real_len��byte�����

	//�z�Lserial port�L�Xdebug��T
	#ifdef DEBUG1
	Prints("Ū��OUT endpoint ");
	PrintLongInt(endpoint/2);
	Prints(" ��buffer�A�@ ");
	PrintLongInt(real_len);
	Prints(" byte\n");
	#endif

	for(i=0;i<real_len;i++)		       //�ĤT�ӥH��Ū���쪺byte�~�O�u����data
	{
		Buf[i] = D12ReadByte();		   //Ū��byte

		#ifdef DEBUG1
			PrintHex(Buf[i]);		   //�z�Lserial port�L�X�o��byte��16�i���ܪk
			UARTPutChar(' ');
			if((i+1)%16 == 0)
				UARTPutChar('\n');
		#endif
	}
	#ifdef DEBUG1
	if((real_len)%16!=0)
		UARTPutChar('\n');
	#endif
	return real_len;    //�̫�return��ڤWŪ����X��byte�����	


}



//�N��Ƽg�J�S�wendpoint��buffer���A��������token packet��IN request�ɡA�A����wIN endpoint buffer������ưe�X
unsigned char D12WriteEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)	 
{
	//��J���ѼƤ�
	//Endp=0�N�� endpoint 0 OUT �B Endp=1�N�� endpoint 0 IN
	//Endp=2�N�� endpoint 1 OUT �B Endp=3�N�� endpoint 1 IN
	//Endp=4�N�� endpoint 2 OUT �B Endp=5�N�� endpoint 2 IN
	//Len�N��Q�n�g�J�h��byte�����
	//Buf�h�N��N���}�C����Ƽg�J��buffer��

	int i;
	D12SelectEndpoint(Endp);            //���endpoint	
	D12WriteCommand(D12_WRITE_BUFFER);  //�g�J "�g�J���endpoint buffer" ���R�O
	//���U�ӭn�g�J���
	D12WriteByte(0);                    //�Ĥ@��byte�n�g�J0
	D12WriteByte(Len);                  //�ĤG��byte�n�g�J���ӼƦr�A�N���U�ӭn�e�h��byte

	#ifdef DEBUG1
		Prints("�g�J��ƨ�IN endpoint ");
		PrintLongInt(Endp/2);
	   	Prints(" ��buffer�A�@ ");
		PrintLongInt(Len);
		Prints(" byte\n");
	#endif


	//���U�Ӽg�J��buffer����Ƥ~�O�u���n�ǰe�X�h�����
	for(i=0;i<Len;i++)
	{
		D12WriteByte(Buf[i]);

		#ifdef DEBUG1
			PrintHex(Buf[i]);		   //�z�Lserial port�L�X�o��byte��16�i���ܪk
			UARTPutChar(' ');
			if((i+1)%16 == 0)
				UARTPutChar('\n');
		#endif
	}
	#ifdef DEBUG1
		if(Len%16!=0)
			UARTPutChar('\n');
	#endif

	D12ValidateBuffer(Endp);           //�ϸ�IN endpoint buffer����
	//D12_DATA = 0xff;				   //data port�������J���A
	return Len;						   //�^���`�@�g�J�X��byte��buffer��

}

void D12ClearBuffer(unsigned char endpoint)	//�M�ůS�wOUT endpoint��buffer
{
	D12SelectEndpoint(endpoint);	        //��ܯS�wendpoint
	D12WriteCommand(D12_CLEAR_BUFFER);	    //�g�Jclear buffer command
}

void D12ValidateBuffer(unsigned char endpoint) //��IN endpoint buffer����
{
	D12SelectEndpoint(endpoint);	           //��ܯS�wendpoint
	D12WriteCommand(D12_VALIDATE_BUFFER);      //�g�Jvalidate buffer command
}

unsigned char D12ReadEndpointLastStatus(unsigned char Endp)	//Ū�����wendpoint���̫᣸��transaction�����A�A�òM��interrupt register�����Ҧ�interrupt flag
{
	D12WriteCommand(0x40 + Endp);	       //�g�J"Read Endpoint Last Transaction Status command"
	return D12ReadByte();	               //�^�ǣ���byte�A�䤺�e���S�wendpoint���̫᣸��transaction�����A
}

void D12AcknowledgeSetup(void)	 //acknowledge setup(�ت��O��Clear Buffer command�BValidate Buffer command��ͮ�) 
{
	//�u�|�bcontrol transfer��setup stage���ϥΨ�
	//����SETUP packet��
	//endpoint 0 IN ��Validate Buffer command
	//�Mendpoint 0 OUT ��Clear Buffer command�|����
	//�ҥH�n�ϥ�Acknowledge Setup command
	//�~����Clear Buffer command�BValidate Buffer command�ͮ�
	
	D12SelectEndpoint(1);                    //���endpoint 0 IN
	D12WriteCommand(D12_ACKNOWLEDGE_SETUP);	 //�e�XAcknowledge Setup command
	D12SelectEndpoint(0);                    //���endpoint 0 OUT
	D12WriteCommand(D12_ACKNOWLEDGE_SETUP);  //�e�XAcknowledge Setup command
	
}

//void D12SetAddress(unsigned char Addr)
//{}

//void D12SetEndpointEnable(unsigned char Enable)
//{}
