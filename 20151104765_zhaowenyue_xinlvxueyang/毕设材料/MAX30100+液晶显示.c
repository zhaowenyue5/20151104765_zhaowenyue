# include <reg52.h>
# include <stdio.h>
# include <intrins.h>
//定义IIC接口
sbit IIC_SCL    =P3^5;     //IIC的SCL
sbit IIC_SDA    =P3^7;     //IIC的SDA
bit  IIC_ACK;              //IIC的ACK
int  rda;                  //IIC读出
//-------------------------------------------------------------------------------------//
//函数:    delayms()
//功能:    延时程序
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
//函数:    void iic_start();
//功能:    I2C总线开始
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
//函数:    void iic_stop();
//功能:    I2C总线结束
//需定义：
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
//函数:    void iic_sendbyte(unsigned char c);
//功能:    发送 8_BIT 数据
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
//函数:    int iic_rcvbyte_nack();
//功能:    接收 8_BIT 数据  最后ack信号
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
//给出NACK信号
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
//函数:    int iic_rcvbyte_ack();
//功能:    接收 8_BIT 数据 最后ack信号
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
//给出ACK信号
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
//函数:      wr_max30100_one_data()
//功能:      写一位max30100数据
//address:   芯片从地址
//saddress:  写寄存器地址
//w_data:    待写数据
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
//函数:      rd_max30100_one_data()
//功能:      读一位max30100数据
//address:   芯片从地址
//saddress:  读寄存器地址
//rda:       读出的数据
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

///////////////////////////液晶显示程序///////////////////////////
/********IO引脚定义***********************************************************/
sbit LCD_RS=P1^0;//定义引脚
sbit LCD_RW=P1^1;
sbit LCD_E=P1^2;

/********宏定义***********************************************************/
#define LCD_Data P0
#define Busy    0x80 //用于检测LCD状态字中的Busy标识

/********数据定义*************************************************************/
unsigned char code uctech[] = {"XL:00  XY:00%  "};
unsigned char code net[] =      {"WD: 00   BJ:00  "};

/********函数声明*************************************************************/
void WriteDataLCD(unsigned char WDLCD);					//写数据
void WriteCommandLCD(unsigned char WCLCD,BuysC);		//写命令
unsigned char ReadDataLCD(void);						//读数据
unsigned char ReadStatusLCD(void);						//读状态
void LCDInit(void);										//初始化
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);			//相应坐标显示字节内容
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);	//相应坐标开始显示一串内容
void Delay5Ms(void);									//延时
void Delay400Ms(void);									//延时


/***********主函数开始********************************************************/	
void main(void)
{
	Delay400Ms(); 	//启动等待，等LCD讲入工作状态
	LCDInit(); 		//初始化
	Delay5Ms(); 	//延时片刻(可不要)

	DisplayListChar(0, 0, uctech);
	DisplayListChar(1, 5, net);
	ReadDataLCD();	//测试用句无意义
	while(1);
}

/***********写数据********************************************************/	
void WriteDataLCD(unsigned char WDLCD)
{
 	ReadStatusLCD(); //检测忙
 	LCD_Data = WDLCD;
 	LCD_RS = 1;
 	LCD_RW = 0;
 	LCD_E = 0; 		//若晶振速度太高可以在这后加小的延时
 	LCD_E = 0; 		//延时
 	LCD_E = 1;
}

/***********写指令********************************************************/	
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysC为0时忽略忙检测
{
 	if (BuysC) ReadStatusLCD(); //根据需要检测忙
 	LCD_Data = WCLCD;
 	LCD_RS = 0;
 	LCD_RW = 0; 
 	LCD_E = 0;
 	LCD_E = 0;
	LCD_E = 1; 
}

/***********读数据********************************************************/	
unsigned char ReadDataLCD(void)
{
 	LCD_RS = 1; 
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	return(LCD_Data);
}

/***********读状态*******************************************************/	
unsigned char ReadStatusLCD(void)
{
 	LCD_Data = 0xFF; 
 	LCD_RS = 0;
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	while (LCD_Data & Busy); //检测忙信号
 	return(LCD_Data);
}

/***********初始化********************************************************/	
void LCDInit(void)
{
 	LCD_Data = 0;
 	WriteCommandLCD(0x38,0); 	//三次模式设置，不检测忙信号
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 

 	WriteCommandLCD(0x38,1); 	//显示模式设置,开始要求每次检测忙信号
 	WriteCommandLCD(0x08,1); 	//关闭显示
 	WriteCommandLCD(0x01,1); 	//显示清屏
 	WriteCommandLCD(0x06,1); 	//显示光标移动设置
 	WriteCommandLCD(0x0C,1); 	//显示开及光标设置
}

/***********按指定位置显示一个字符*******************************************/	
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 	Y &= 0x1;
 	X &= 0xF; 				//限制X不能大于15，Y不能大于1
 	if (Y) X |= 0x40; 		//当要显示第二行时地址码+0x40;
 	X |= 0x80; 			//算出指令码
 	WriteCommandLCD(X, 0); //这里不检测忙信号，发送地址码
 	WriteDataLCD(DData);
}

/***********按指定位置显示一串字符*****************************************/	
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
 	unsigned char ListLength;

 	ListLength = 0;
 	Y &= 0x1;
 	X &= 0xF; 				//限制X不能大于15，Y不能大于1
 	while (DData[ListLength]>=0x20){ //若到达字串尾则退出
   		if (X <= 0xF){ 		//X坐标应小于0xF
     		DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
     		ListLength++;
     		X++;
    	}
  	}
}

/***********短延时********************************************************/	
void Delay5Ms(void)
{
 	unsigned int TempCyc = 5552;
 	while(TempCyc--);
}

/***********长延时********************************************************/	
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
//函数:      主程序
//功能:      读max30100内部温度
//-------------------------------------------------------------------------------------//
main()
{
double temp,temp1,temp2;
//temp       测量温度
//temp1      30100整数部分温度
//temp2      30100小数部分温度
TMOD=0x21;		  
SCON=0x50;
TH1=0xFD;
TL1=0xFD;
TR1=1;			   
TI=1;   
//设置51的波特率为 9600 N 8 1
//51主频为11.0592MHz
while(1)
{
wr_max30100_one_data(0xae,0x06,0x0a);       // 0X06地址B3位TEMP_EN置1
delayms(20);                                // 等待温度转换完成，不等待，读出数据有误
rd_max30100_one_data(0xae,0x16);            // 读出温度信号
printf("temp1=%d\n",rda);                   // 串口显示
temp1=rda;
rd_max30100_one_data(0xae,0x17);            // 读出温度小数部分数据
printf("temp2=%d\n",rda);                   // 串口显示
temp2=rda;
temp=temp1+(temp2/100);
printf("temp=%.2f\n",temp);                 // 串口显示
rd_max30100_one_data(0xae,0xff);            // 读出芯片ID
printf("MAX30100 ID =%d\n",rda);            // 串口显示
delayms(100);    
}
}
