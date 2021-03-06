#include <reg52.h>
#include <intrins.h>	 //			 包含头文件
#include <stdio.h>
#include <string.H>
#define uint            unsigned int
#define uchar           unsigned char
#define ulong           unsigned long	 //宏定义
#define LCD_DATA        P0				 //定义P0口为LCD_DATA
uchar SpO2[16]=       "SpO2:           ";
sbit LCD_RS =P2^5;
sbit LCD_RW =P2^6;
sbit LCD_E  =P2^7;						 //定义LCD控制引脚

sbit Xintiao =P3^2 ;					 //脉搏检测输入端定义
sbit speaker =P2^4;						 //蜂鸣器引脚定义

void delay5ms(void);   //误差 0us
void LCD_WriteData(uchar LCD_1602_DATA);	 /********LCD1602数据写入***********/
void LCD_WriteCom(uchar LCD_1602_COM);		 /********LCD1602命令写入***********/
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data); /*1602字符显示函数，变量依次为字符显示首地址，显示字符长度，所显示的字符*/
void InitLcd();//液晶初始化函数

void Tim_Init();

uchar dxflag1=0,dxflag2=0,dxflag3=0;

uchar Xintiao_Change=0;	   //
uint  Xintiao_Jishu;
uchar stop;
uchar View_Data[3];
uchar View_L[3];
uchar View_H[3];
uchar Xintiao_H=100;	//脉搏上限
uchar Xintiao_L=40;		//脉搏下限

uint xintiaoflag;
uchar sendflag=0;

uchar Key_Change;
uchar Key_Value;		//按键键值
uchar View_Con;			//设置的位（0正常工作，1设置上限，2设置下限）
uchar View_Change;


//////////////以下为血氧及温度采集程序///////////////////
sbit IIC_SCL    =P1^3;     //IIC的SCL
sbit IIC_SDA    =P1^4;     //IIC的SDA
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
//////////////以上为血氧及温度采集程序///////////////////

//////////////以下为短信发送程序/////////////////////////
#define FOSC_110592M
unsigned int  rec_data_len_uart=0;    //标记Buffer_Uart0接收数组
unsigned char idata Buffer_Uart0_Rec[25]={0};		 //Uart0中断接收数组
/*初始化程序（必须使用，否则无法收发），次程序将会使用定时器1*/
void SerialInti()//初始化程序（必须使用，否则无法收发）
{
	TMOD=0x20;//定时器1操作模式2:8位自动重载定时器

#ifdef FOSC_12M		   //在这里根据晶振大小设置不同的数值初始化串口
	TH1=0xf3;//装入初值，波特率2400
	TL1=0xf3;	
#else 	
	TH1=0xfd;//装入初值，波特率9600
	TL1=0xfd;
#endif //end of SOC_12M
	
	TR1=1;//打开定时器
	SM0=0;//设置串行通讯工作模式，（10为一部发送，波特率可变，由定时器1的溢出率控制）
	SM1=1;//(同上)在此模式下，定时器溢出一次就发送一个位的数据
	REN=1;//串行接收允许位（要先设置sm0sm1再开串行允许）
	EA=1;//开总中断
	ES=1;//开串行口中断	
}
unsigned char hand(unsigned char *data_source,unsigned char *ptr)
{
	if(strstr(data_source,ptr)!=NULL)
		return 1;
	else
		return 0;
}
void clear_rec_data()
{
	uchar i,temp_len;
	temp_len=strlen(Buffer_Uart0_Rec);
	if(temp_len>25)
	{
		temp_len=25;
	}
	for(i=0;i<temp_len;i++)
	{
		Buffer_Uart0_Rec[i]='\0';	
	}
	rec_data_len_uart=0;
}

/*串行通讯中断，收发完成将进入该中断*/
void Serial_interrupt() interrupt 4 
{
	unsigned char temp_rec_data_uart0;	
	temp_rec_data_uart0 = SBUF;//读取接收数据		
	RI=0;//接收中断信号清零，表示将继续接收			
	Buffer_Uart0_Rec[rec_data_len_uart]=temp_rec_data_uart0;	//接收数据
	rec_data_len_uart++;
	if(rec_data_len_uart>24)
	{
		rec_data_len_uart=0; 	//从头开始接收数据
	}	
}
void Uart1Send(uchar c)
{
	SBUF=c;
	while(!TI);//等待发送完成信号（TI=1）出现
	TI=0;	
}
//串行口连续发送char型数组，遇到终止号/0将停止
void Uart1Sends(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(!TI);//等待发送完成信号（TI=1）出现
		TI=0;
		str++;
	}
}
//延时函数大概是1s钟，不过延时大的话不准...
void DelaySec(int sec)
{
	uint i , j= 0;
	for(i=0; i<sec; i++)
	{
		for(j=0; j<40000; j++)
		{	
		}
	}
}

void sendduxin()
{
  	if(sendflag>0)
	{
  	EA=0;
	ET0=0;			  //关闭T0中断允许开关
	ET1=0;			  //关闭T1中断允许开关
	SerialInti();
	Uart1Sends("AT+CSCS=\"UCS2\"\r\n");
    DelaySec(1);//延时大约3秒
    Uart1Sends("AT+CMGF=1\r\n");
    DelaySec(1);//延时3秒												
	Uart1Sends("AT+CSCA?\r\n");											
    DelaySec(1);//延时3秒
	Uart1Sends("AT+CSMP=17,167,0,25\r\n");
    DelaySec(1);//延时3秒
    Uart1Sends("AT+CMGS=\"00310038003800340037003100330038003000300033\"\r\n");//此处修改为对方的电话号，电话话的UNICON编码，可使用我们配套的
																				//资料包中的 中文转UNICON编码工具来获取。特别注意：那个软件
																				//获得的unicon 编码待有空格，实际中要去掉，否则模块会报错
    DelaySec(1);//延时3秒
	if(sendflag==1)
	{
    Uart1Sends("6CE8610FFF1A88406C275F025E38FF01");//修改短信内容,短信内容可为中文、英文和数字，但都是对应的unicon 编码。
	}
	else if(sendflag==2)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC79AD8FF01");//修改短信内容,短信内容可为中文、英文和数字，但都是对应的unicon 编码。
	}
	else if(sendflag==3)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC74F4EFF01");//修改短信内容,短信内容可为中文、英文和数字，但都是对应的unicon 编码。
	}
    Uart1Send(0x1a);
    DelaySec(10);//延时20秒
	EA=0;
	ES=0;
	sendflag=0;
	SpO2[15]=' ';
	Tim_Init();
	TR0=1;
	TR1=1;
	dxflag1=dxflag2=dxflag3=0;
	}
}


////////以上为短信发送程序///////////////////////////
void main()	  //主函数
{
    uint temp,temp1,temp2,xy;

	InitLcd();
	Tim_Init();
  //  DelaySec(5);//延时约15秒,此处延时，是为了让模块有足够的时间注册到网络，
	lcd_1602_word(0x80,16,"Heart Rate:     ");	  //初始化显示
	lcd_1602_word(0xc0,16,"SpO2:           ");
	TR0=1;
	TR1=1;				  //打开定时器
//	while(1)
//	{
//	 sendflag=1;
//	 sendduxin();
//	}

	while(1)			  //进入循环
	{
		if(Key_Change)	  //有按键按下并已经得出键值
		{
			Key_Change=0;	  //将按键使能变量清零，等待下次按键按下
			View_Change=1;
			switch(Key_Value)				//判断键值
			{
				case 1:						//设置键按下
				{
					View_Con++;			//设置的位加
					if(View_Con==3)		//都设置好后将此变量清零
					View_Con=0;
					break;				//跳出，下同
				}
				case 2:						//加键按下
				{
					if(View_Con==2)		//判断是设置上限
					{
						if(Xintiao_H<150)	//上限数值小于150
						Xintiao_H++;		//上限+
					}
					if(View_Con==1)		//如果是设置下限
					{
						if(Xintiao_L<Xintiao_H-1)//下限值小于上限-1（下限值不能超过上限）
						Xintiao_L++;		//下限值加
					}
					break;
				}
				case 3:						//减键按下
				{
					if(View_Con==2)		//设置上限
					{
						if(Xintiao_H>Xintiao_L+1)//上限数据大于下限+1（同样上限值不能小于下限）
						Xintiao_H--;		//上限数据减
					}
					if(View_Con==1)		//设置下限
					{
						if(Xintiao_L>30)	//下限数据大于30时
						Xintiao_L--;		//下限数据减
					}
					break;
				} 
			}
		}
		if(View_Change)//显示变量
		{
			View_Change=0;//变量清零
			if(stop==0)			  //心率正常时
			{
				if(View_Data[0]==0x30) //最高位为0时不显示
				View_Data[0]=' ';
			}
			else					  //心率不正常（计数超过5000，也就是两次信号时间超过5s）不显示数据
			{
				View_Data[0]=' ';
				View_Data[1]=' ';
				View_Data[2]=' ';
			}
			switch(View_Con)
			{
				case 0: //正常显示
				{
				
					wr_max30100_one_data(0xae,0x06,0x0a);       // 0X06地址B3位TEMP_EN置1
					delayms(20);                               
					rd_max30100_one_data(0xae,0x16);           
					temp1=rda;
					rd_max30100_one_data(0xae,0x17);           
					temp2=rda;
					temp=temp1*100+temp2;
					if(((xintiaoflag)>10)&&((xintiaoflag)<200))
					{
					xintiaoflag=0;
					if((temp>=2000)&&(temp<3600))  { xy=940; }
					else if(temp<2000) 	 {xy=940-((2000-temp)/10); }
					else if(temp>3600)	 {xy=940-((2000-temp)/8);}
					
					SpO2[5]=xy/100+0x30;
					SpO2[6]=xy%100/10+0x30;
					SpO2[7]='.';
					SpO2[8]=xy%10+0x30;
					SpO2[9]='%';

					if(xy<900) 
					{
					dxflag1++;
					if(dxflag1>2)
					{
					sendflag=1;
					SpO2[15]='1';
					}
					else {dxflag1=0;}
					}
					}
					else
					{
					SpO2[5]=' ';
					SpO2[6]=' ';
					SpO2[7]=' ';
					SpO2[8]=' ';
					SpO2[9]=' ';
					dxflag1=0;
					}
				   lcd_1602_word(0x80,16,"Heart Rate:     ");//显示一行数据
					
			//		lcd_1602_word(0xc0,16,"SpO2:           ");//显示第二行数据
				   lcd_1602_word(0x8d,3,View_Data);			 //第二行显示心率
				   lcd_1602_word(0xc0,16,SpO2);			   //显示第二行数据	
				    if(sendflag>0) 
					{
					sendduxin();
					}
					else{SpO2[15]=' ';}
					break;
				}
				case 1: //设置下限时显示
				{
					lcd_1602_word(0x80,16,"Heart Rate:     ");//第一行显示心率
					lcd_1602_word(0x8d,3,View_Data);
					
					View_L[0]=Xintiao_L/100+0x30;		//将下限数据拆字
					View_L[1]=Xintiao_L%100/10+0x30;
					View_L[2]=Xintiao_L%10+0x30;
					
					if(View_L[0]==0x30)					//最高位为0时，不显示
					View_L[0]=' ';
	
					lcd_1602_word(0xC0,16,"Warning L :     ");//第二行显示下限数据
					lcd_1602_word(0xCd,3,View_L);
					break;
				}
				case 2: //设置上限时显示（同上）
				{
					lcd_1602_word(0x80,16,"Heart Rate:     ");
					lcd_1602_word(0x8d,3,View_Data);
					
					View_H[0]=Xintiao_H/100+0x30;
					View_H[1]=Xintiao_H%100/10+0x30;
					View_H[2]=Xintiao_H%10+0x30;
					
					if(View_H[0]==0x30)
					View_H[0]=' ';
					
					lcd_1602_word(0xC0,16,"Warning H :     ");
					lcd_1602_word(0xCd,3,View_H);
					break;
				}
			}
		}
	}
}

void Time1() interrupt 3		//定时器1服务函数
{
	static uchar Key_Con,Xintiao_Con;
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //重新赋初值
	switch(Key_Con)   //无按键按下时此值为0
	{
		case 0:		   //每10ms扫描此处
		{
			if((P1&0x07)!=0x07)//扫描按键是否有按下
			{
				Key_Con++;		  //有按下此值加1，值为1
			}
			break;
		}
		case 1:					  //10ms后二次进入中断后扫描此处（Key_Con为1）
		{
			if((P1&0x07)!=0x07)//第二次进入中断时，按键仍然是按下（起到按键延时去抖的作用）
			{
				Key_Con++;		  //变量加1，值为2
				switch(P1&0x07)  //判断是哪个按键按下
				{
					case 0x06:Key_Value=1;break;	 //判断好按键后将键值赋值给变量Key_Value
					case 0x05:Key_Value=2;break;
					case 0x03:Key_Value=3;break;
				}
			}
			else								 //如果10ms时没有检测到按键按下（按下时间过短）
			{
				Key_Con=0;						 //变量清零，重新检测按键
			}
			break;
		}
		case 2:									 //20ms后检测按键
		{
			if((P1&0x07)==0x07)				 //检测按键是否还是按下状态
			{
				Key_Change=1;					 //有按键按下使能变量，（此变量为1时才会处理键值数据）
				Key_Con=0;						//变量清零，等待下次有按键按下
			}
			break;
		}
	}
	switch (Xintiao_Con)//此处与上面按键的检测类似
	{
		case 0:			 //默认Xintiao_Con是为0的
		{
			if(!Xintiao)//每10ms（上面的定时器）检测一次脉搏是否有信号
			{
				Xintiao_Con++;//如果有信号，变量加一，程序就会往下走了
			}
			break;
		}
		case 1:
		{
			if(!Xintiao)	   //每过10ms检测一下信号是否还存在
			{
				Xintiao_Con++;//存在就加一
			}
			else
			{
				Xintiao_Con=0;//如果不存在了，检测时间很短，说明检测到的不是脉搏信号，可能是其他干扰，将变量清零，跳出此次检测
			} 
			break;
		}
		case 2:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//存在就加一
			}
			else
			{
				Xintiao_Con=0;//如果不存在了，检测时间很短，说明检测到的不是脉搏信号，可能是其他干扰，将变量清零，跳出此次检测
			} 
			break;
		}
		case 3:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//存在就加一
			}
			else
			{
				Xintiao_Con=0;//如果不存在了，检测时间很短，说明检测到的不是脉搏信号，可能是其他干扰，将变量清零，跳出此次检测
			} 
			break;
		}
		case 4:
		{
			if(Xintiao)//超过30ms一直有信号，判定此次是脉搏信号，执行以下程序
			{
				if(Xintiao_Change==1)//心率计原理为检测两次脉冲间隔时间计算心率，变量Xintiao_Change第一次脉冲时为0的，所有走下面的else，第二次走这里
				{
					if(60000/Xintiao_Jishu>200)
					{
						View_Data[0]='-';	
						View_Data[1]='-';	 
						View_Data[2]='-';
						speaker=1;			//不响	
					}
					else
					{
						View_Data[0]=(60000/Xintiao_Jishu)/100+0x30;		  //计算心跳并拆字显示：心跳计时是以10ms为单位，两次心跳中间计数如果是100次，也就是100*10ms=1000ms=1s
						View_Data[1]=(60000/Xintiao_Jishu)%100/10+0x30;	  //那么计算出的一分钟（60s）心跳数就是：60*1000/（100*10ms）=60次	  其中60是一分钟60s，1000是一秒有1000ms，100是计数值，10是一次计数对应 的时间是10ms
						View_Data[2]=(60000/Xintiao_Jishu)%10+0x30;		  //计算出的心跳数/100得到心跳的百位，%100是取余的，就是除以100的余数，再除以10就得到十位了，以此类推
								  //鹱趾蟮牡ジ鍪�+0x30的目的是得到对应数字的液晶显示码，数字0对应的液晶显示码是0x30，1是0x30+1，以此类推
						if(((60000/Xintiao_Jishu)>=Xintiao_H)||((60000/Xintiao_Jishu)<=Xintiao_L))//心率不在范围内报警
						{
						speaker=0;			//蜂鸣器响
						if(((60000/Xintiao_Jishu)>=Xintiao_H)&&((60000/Xintiao_Jishu)>=Xintiao_H)<195)  
						{
						dxflag2++;
						if(dxflag2>2)
						{
						sendflag=2;
						SpO2[15]='2';
						}
						}
						else  {dxflag2=0;}
					    if(((60000/Xintiao_Jishu)<=Xintiao_L))  
						{
						dxflag3++;
						if(dxflag3>2)
						{
						sendflag=3;
						SpO2[15]='3';
						}
						 }
						 else{dxflag3=0;}
						}
						else
						{
						speaker=1;			//不响
						 dxflag3=dxflag2=0;
						}
					}
					xintiaoflag=60000/Xintiao_Jishu;
					View_Change=1;	   //计算出心率后启动显示
					Xintiao_Jishu=0;	   //心跳计数清零
					Xintiao_Change=0;   //计算出心率后该变量清零，准备下次检测心率
					stop=0;			   //计算出心率后stop邈
				}
				else//第一次脉冲时Xintiao_Change为0
				{
					Xintiao_Jishu=0;	//脉冲计时变量清零，开始计时
					Xintiao_Change=1;//Xintiao_Change置1，准备第二次检测到脉冲时计算心率
				}
				Xintiao_Con=0;	//清零，准备检测下一次脉冲
				break;
			}
		}
	}
}
/**定时器T0工作函数**/
void Time0() interrupt 1
{
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //重新赋初值
	Xintiao_Jishu++;  //心跳计数加
	if(Xintiao_Jishu==5000)//心跳计数大于5000
	{
		Xintiao_Jishu=0;		//数据清零
		View_Change=1;		//显示位置1
		Xintiao_Change=0;	//置零，准备再次检测
		stop=1;	   //心跳计数超过5000后说明心率不正常或者没有测出，stop置1
		speaker=1;  //关闭蜂鸣器
	}
}
/**定时器初始化函数**/
void Tim_Init()
{
	EA=1;			  //打开中断总开关
	ET0=1;			  //打开T0中断允许开关
	ET1=1;			  //打开T1中断允许开关
	TMOD=0x11;		  //设定定时器状态
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //赋初值
	
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //赋初值
}
/**在指定地址显示指定数量的指定字符**/
/**Adress_Com显示地址，Num_Adat显示字符数量，Adress_Data显示字符串内容**/ 
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data)
{
	uchar a=0;
	uchar Data_Word;
	LCD_WriteCom(Adress_Com); //选中地址
	for(a=0;a<Num_Adat;a++)   //for循环决定显示字符个数
	{
		Data_Word=*Adress_Data;	  //读取字符串数据
		LCD_WriteData(Data_Word);  //显示字符串
		Adress_Data++;			  //显示地址加一
	}
}

/***************1602函数*******************/
void LCD_WriteData(uchar LCD_1602_DATA)	 /********LCD1602数据写入***********/
{
	delay5ms();  //操作前短暂延时，保证信号稳定
	LCD_E=0;
	LCD_RS=1;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_DATA;
	LCD_E=0;
	LCD_RS=0;
}

/********LCD1602命令写入***********/
void LCD_WriteCom(uchar LCD_1602_COM)
{
	delay5ms();//操作前短暂延时，保证信号稳定
	LCD_E=0;
	LCD_RS=0;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_COM;
	LCD_E=0;
	LCD_RS=0;
}


void InitLcd()		   //初始化液晶函数
{
	delay5ms();
	delay5ms();
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x06); //显示光标移动位置
	LCD_WriteCom(0x0c); //显示开及光标设置
	LCD_WriteCom(0x01); //显示清屏
	delay5ms();
	delay5ms();
}

void delay5ms(void)   //5ms延时函数
{
	unsigned char a,b;
	for(b=185;b>0;b--)
	for(a=12;a>0;a--);
}
