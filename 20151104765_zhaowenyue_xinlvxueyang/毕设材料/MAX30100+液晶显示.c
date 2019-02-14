# include <reg52.h>
# include <stdio.h>
# include <intrins.h>
//����IIC�ӿ�
sbit IIC_SCL    =P3^5;     //IIC��SCL
sbit IIC_SDA    =P3^7;     //IIC��SDA
bit  IIC_ACK;              //IIC��ACK
int  rda;                  //IIC����
//-------------------------------------------------------------------------------------//
//����:    delayms()
//����:    ��ʱ����
//-------------------------------------------------------------------------------------//
void delayms(unsigned int ms)
{
	unsigned char i=100,j;
	for(;ms;ms--)
	{
		while(--i)
		{
			j=10;
			while(--j);
		}
	}
}
//-------------------------------------------------------------------------------------//
//����:    void iic_start();
//����:    I2C���߿�ʼ
//-------------------------------------------------------------------------------------//
//   SCL   --- --- ___
//   SDA   --- ___ ___
void iic_start()
{  
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;         
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//����:    void iic_stop();
//����:    I2C���߽���
//�趨�壺
//-------------------------------------------------------------------------------------//
//   SCL   ___ --- ---
//   SDA   ___ ___ ---
void iic_stop()
{  
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//����:    void iic_sendbyte(unsigned char c);
//����:    ���� 8_BIT ����
//-------------------------------------------------------------------------------------//
void iic_sendbyte(unsigned char c)
{
unsigned char bitcnt;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
if((c<<bitcnt)&0x80)
 IIC_SDA=1;
else
 IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
}
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
if(IIC_SDA==0)
 IIC_ACK=0;
else
 IIC_ACK=1;
IIC_SCL=0;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//����:    int iic_rcvbyte_nack();
//����:    ���� 8_BIT ����  ���ack�ź�
//-------------------------------------------------------------------------------------//
int iic_rcvbyte_nack()
{
unsigned char retc;
unsigned char bitcnt;
retc=0;
IIC_SDA=1;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
retc=retc<<1;
if(IIC_SDA==1)
retc=retc+1;
_nop_();
_nop_();
}
//����NACK�ź�
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
return(retc);
}
//-------------------------------------------------------------------------------------//
//����:    int iic_rcvbyte_ack();
//����:    ���� 8_BIT ���� ���ack�ź�
//-------------------------------------------------------------------------------------//
int iic_rcvbyte_ack()
{
unsigned char retc;
unsigned char bitcnt;
retc=0;
IIC_SDA=1;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
retc=retc<<1;
if(IIC_SDA==1)
retc=retc+1;
_nop_();
_nop_();
}
//����ACK�ź�
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
return(retc);
}
//-------------------------------------------------------------------------------------//
//����:      wr_max30100_one_data()
//����:      дһλmax30100����
//address:   оƬ�ӵ�ַ
//saddress:  д�Ĵ�����ַ
//w_data:    ��д����
//-------------------------------------------------------------------------------------//
void wr_max30100_one_data(int address,int saddress,int w_data )
{
_nop_();
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
iic_sendbyte(saddress);
_nop_();
iic_sendbyte(w_data);
_nop_();
iic_stop();
_nop_();
}
//-------------------------------------------------------------------------------------//
//����:      rd_max30100_one_data()
//����:      ��һλmax30100����
//address:   оƬ�ӵ�ַ
//saddress:  ���Ĵ�����ַ
//rda:       ����������
//-------------------------------------------------------------------------------------//
void rd_max30100_one_data(int address,int saddress)
{
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
iic_sendbyte(saddress);
_nop_();
address=address+1;
_nop_();
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
rda=iic_rcvbyte_nack();
_nop_();
iic_stop();
}

///////////////////////////Һ����ʾ����///////////////////////////
/********IO���Ŷ���***********************************************************/
sbit LCD_RS=P1^0;//��������
sbit LCD_RW=P1^1;
sbit LCD_E=P1^2;

/********�궨��***********************************************************/
#define LCD_Data P0
#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ

/********���ݶ���*************************************************************/
unsigned char code uctech[] = {"XL:00  XY:00%  "};
unsigned char code net[] =      {"WD: 00   BJ:00  "};

/********��������*************************************************************/
void WriteDataLCD(unsigned char WDLCD);					//д����
void WriteCommandLCD(unsigned char WCLCD,BuysC);		//д����
unsigned char ReadDataLCD(void);						//������
unsigned char ReadStatusLCD(void);						//��״̬
void LCDInit(void);										//��ʼ��
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);			//��Ӧ������ʾ�ֽ�����
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);	//��Ӧ���꿪ʼ��ʾһ������
void Delay5Ms(void);									//��ʱ
void Delay400Ms(void);									//��ʱ


/***********��������ʼ********************************************************/	
void main(void)
{
	Delay400Ms(); 	//�����ȴ�����LCD���빤��״̬
	LCDInit(); 		//��ʼ��
	Delay5Ms(); 	//��ʱƬ��(�ɲ�Ҫ)

	DisplayListChar(0, 0, uctech);
	DisplayListChar(1, 5, net);
	ReadDataLCD();	//�����þ�������
	while(1);
}

/***********д����********************************************************/	
void WriteDataLCD(unsigned char WDLCD)
{
 	ReadStatusLCD(); //���æ
 	LCD_Data = WDLCD;
 	LCD_RS = 1;
 	LCD_RW = 0;
 	LCD_E = 0; 		//�������ٶ�̫�߿���������С����ʱ
 	LCD_E = 0; 		//��ʱ
 	LCD_E = 1;
}

/***********дָ��********************************************************/	
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCΪ0ʱ����æ���
{
 	if (BuysC) ReadStatusLCD(); //������Ҫ���æ
 	LCD_Data = WCLCD;
 	LCD_RS = 0;
 	LCD_RW = 0; 
 	LCD_E = 0;
 	LCD_E = 0;
	LCD_E = 1; 
}

/***********������********************************************************/	
unsigned char ReadDataLCD(void)
{
 	LCD_RS = 1; 
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	return(LCD_Data);
}

/***********��״̬*******************************************************/	
unsigned char ReadStatusLCD(void)
{
 	LCD_Data = 0xFF; 
 	LCD_RS = 0;
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	while (LCD_Data & Busy); //���æ�ź�
 	return(LCD_Data);
}

/***********��ʼ��********************************************************/	
void LCDInit(void)
{
 	LCD_Data = 0;
 	WriteCommandLCD(0x38,0); 	//����ģʽ���ã������æ�ź�
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 

 	WriteCommandLCD(0x38,1); 	//��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
 	WriteCommandLCD(0x08,1); 	//�ر���ʾ
 	WriteCommandLCD(0x01,1); 	//��ʾ����
 	WriteCommandLCD(0x06,1); 	//��ʾ����ƶ�����
 	WriteCommandLCD(0x0C,1); 	//��ʾ�����������
}

/***********��ָ��λ����ʾһ���ַ�*******************************************/	
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 	Y &= 0x1;
 	X &= 0xF; 				//����X���ܴ���15��Y���ܴ���1
 	if (Y) X |= 0x40; 		//��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
 	X |= 0x80; 			//���ָ����
 	WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
 	WriteDataLCD(DData);
}

/***********��ָ��λ����ʾһ���ַ�*****************************************/	
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
 	unsigned char ListLength;

 	ListLength = 0;
 	Y &= 0x1;
 	X &= 0xF; 				//����X���ܴ���15��Y���ܴ���1
 	while (DData[ListLength]>=0x20){ //�������ִ�β���˳�
   		if (X <= 0xF){ 		//X����ӦС��0xF
     		DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
     		ListLength++;
     		X++;
    	}
  	}
}

/***********����ʱ********************************************************/	
void Delay5Ms(void)
{
 	unsigned int TempCyc = 5552;
 	while(TempCyc--);
}

/***********����ʱ********************************************************/	
void Delay400Ms(void)
{
 	unsigned char TempCycA = 5;
 	unsigned int TempCycB;
 	while(TempCycA--){
  		TempCycB=7269;
  		while(TempCycB--);
 	}
}





//-------------------------------------------------------------------------------------//
//����:      ������
//����:      ��max30100�ڲ��¶�
//-------------------------------------------------------------------------------------//
main()
{
double temp,temp1,temp2;
//temp       �����¶�
//temp1      30100���������¶�
//temp2      30100С�������¶�
TMOD=0x21;		  
SCON=0x50;
TH1=0xFD;
TL1=0xFD;
TR1=1;			   
TI=1;   
//����51�Ĳ�����Ϊ 9600 N 8 1
//51��ƵΪ11.0592MHz
while(1)
{
wr_max30100_one_data(0xae,0x06,0x0a);       // 0X06��ַB3λTEMP_EN��1
delayms(20);                                // �ȴ��¶�ת����ɣ����ȴ���������������
rd_max30100_one_data(0xae,0x16);            // �����¶��ź�
printf("temp1=%d\n",rda);                   // ������ʾ
temp1=rda;
rd_max30100_one_data(0xae,0x17);            // �����¶�С����������
printf("temp2=%d\n",rda);                   // ������ʾ
temp2=rda;
temp=temp1+(temp2/100);
printf("temp=%.2f\n",temp);                 // ������ʾ
rd_max30100_one_data(0xae,0xff);            // ����оƬID
printf("MAX30100 ID =%d\n",rda);            // ������ʾ
delayms(100);    
}
}
