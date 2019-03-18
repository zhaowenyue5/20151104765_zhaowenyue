#include <reg52.h>
#include <intrins.h>	 //			 ����ͷ�ļ�
#include <stdio.h>
#include <string.H>
#define uint            unsigned int
#define uchar           unsigned char
#define ulong           unsigned long	 //�궨��
#define LCD_DATA        P0				 //����P0��ΪLCD_DATA
uchar SpO2[16]=       "SpO2:           ";
sbit LCD_RS =P2^5;
sbit LCD_RW =P2^6;
sbit LCD_E  =P2^7;						 //����LCD��������

sbit Xintiao =P3^2 ;					 //�����������˶���
sbit speaker =P2^4;						 //���������Ŷ���

void delay5ms(void);   //��� 0us
void LCD_WriteData(uchar LCD_1602_DATA);	 /********LCD1602����д��***********/
void LCD_WriteCom(uchar LCD_1602_COM);		 /********LCD1602����д��***********/
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data); /*1602�ַ���ʾ��������������Ϊ�ַ���ʾ�׵�ַ����ʾ�ַ����ȣ�����ʾ���ַ�*/
void InitLcd();//Һ����ʼ������

void Tim_Init();

uchar dxflag1=0,dxflag2=0,dxflag3=0;

uchar Xintiao_Change=0;	   //
uint  Xintiao_Jishu;
uchar stop;
uchar View_Data[3];
uchar View_L[3];
uchar View_H[3];
uchar Xintiao_H=100;	//��������
uchar Xintiao_L=40;		//��������

uint xintiaoflag;
uchar sendflag=0;

uchar Key_Change;
uchar Key_Value;		//������ֵ
uchar View_Con;			//���õ�λ��0����������1�������ޣ�2�������ޣ�
uchar View_Change;


//////////////����ΪѪ�����¶Ȳɼ�����///////////////////
sbit IIC_SCL    =P1^3;     //IIC��SCL
sbit IIC_SDA    =P1^4;     //IIC��SDA
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
//////////////����ΪѪ�����¶Ȳɼ�����///////////////////

//////////////����Ϊ���ŷ��ͳ���/////////////////////////
#define FOSC_110592M
unsigned int  rec_data_len_uart=0;    //���Buffer_Uart0��������
unsigned char idata Buffer_Uart0_Rec[25]={0};		 //Uart0�жϽ�������
/*��ʼ�����򣨱���ʹ�ã������޷��շ������γ��򽫻�ʹ�ö�ʱ��1*/
void SerialInti()//��ʼ�����򣨱���ʹ�ã������޷��շ���
{
	TMOD=0x20;//��ʱ��1����ģʽ2:8λ�Զ����ض�ʱ��

#ifdef FOSC_12M		   //��������ݾ����С���ò�ͬ����ֵ��ʼ������
	TH1=0xf3;//װ���ֵ��������2400
	TL1=0xf3;	
#else 	
	TH1=0xfd;//װ���ֵ��������9600
	TL1=0xfd;
#endif //end of SOC_12M
	
	TR1=1;//�򿪶�ʱ��
	SM0=0;//���ô���ͨѶ����ģʽ����10Ϊһ�����ͣ������ʿɱ䣬�ɶ�ʱ��1������ʿ��ƣ�
	SM1=1;//(ͬ��)�ڴ�ģʽ�£���ʱ�����һ�ξͷ���һ��λ������
	REN=1;//���н�������λ��Ҫ������sm0sm1�ٿ���������
	EA=1;//�����ж�
	ES=1;//�����п��ж�	
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

/*����ͨѶ�жϣ��շ���ɽ�������ж�*/
void Serial_interrupt() interrupt 4 
{
	unsigned char temp_rec_data_uart0;	
	temp_rec_data_uart0 = SBUF;//��ȡ��������		
	RI=0;//�����ж��ź����㣬��ʾ����������			
	Buffer_Uart0_Rec[rec_data_len_uart]=temp_rec_data_uart0;	//��������
	rec_data_len_uart++;
	if(rec_data_len_uart>24)
	{
		rec_data_len_uart=0; 	//��ͷ��ʼ��������
	}	
}
void Uart1Send(uchar c)
{
	SBUF=c;
	while(!TI);//�ȴ���������źţ�TI=1������
	TI=0;	
}
//���п���������char�����飬������ֹ��/0��ֹͣ
void Uart1Sends(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(!TI);//�ȴ���������źţ�TI=1������
		TI=0;
		str++;
	}
}
//��ʱ���������1s�ӣ�������ʱ��Ļ���׼...
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
	ET0=0;			  //�ر�T0�ж�������
	ET1=0;			  //�ر�T1�ж�������
	SerialInti();
	Uart1Sends("AT+CSCS=\"UCS2\"\r\n");
    DelaySec(1);//��ʱ��Լ3��
    Uart1Sends("AT+CMGF=1\r\n");
    DelaySec(1);//��ʱ3��												
	Uart1Sends("AT+CSCA?\r\n");											
    DelaySec(1);//��ʱ3��
	Uart1Sends("AT+CSMP=17,167,0,25\r\n");
    DelaySec(1);//��ʱ3��
    Uart1Sends("AT+CMGS=\"00310038003800340037003100330038003000300033\"\r\n");//�˴��޸�Ϊ�Է��ĵ绰�ţ��绰����UNICON���룬��ʹ���������׵�
																				//���ϰ��е� ����תUNICON���빤������ȡ���ر�ע�⣺�Ǹ����
																				//��õ�unicon ������пո�ʵ����Ҫȥ��������ģ��ᱨ��
    DelaySec(1);//��ʱ3��
	if(sendflag==1)
	{
    Uart1Sends("6CE8610FFF1A88406C275F025E38FF01");//�޸Ķ�������,�������ݿ�Ϊ���ġ�Ӣ�ĺ����֣������Ƕ�Ӧ��unicon ���롣
	}
	else if(sendflag==2)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC79AD8FF01");//�޸Ķ�������,�������ݿ�Ϊ���ġ�Ӣ�ĺ����֣������Ƕ�Ӧ��unicon ���롣
	}
	else if(sendflag==3)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC74F4EFF01");//�޸Ķ�������,�������ݿ�Ϊ���ġ�Ӣ�ĺ����֣������Ƕ�Ӧ��unicon ���롣
	}
    Uart1Send(0x1a);
    DelaySec(10);//��ʱ20��
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


////////����Ϊ���ŷ��ͳ���///////////////////////////
void main()	  //������
{
    uint temp,temp1,temp2,xy;

	InitLcd();
	Tim_Init();
  //  DelaySec(5);//��ʱԼ15��,�˴���ʱ����Ϊ����ģ�����㹻��ʱ��ע�ᵽ���磬
	lcd_1602_word(0x80,16,"Heart Rate:     ");	  //��ʼ����ʾ
	lcd_1602_word(0xc0,16,"SpO2:           ");
	TR0=1;
	TR1=1;				  //�򿪶�ʱ��
//	while(1)
//	{
//	 sendflag=1;
//	 sendduxin();
//	}

	while(1)			  //����ѭ��
	{
		if(Key_Change)	  //�а������²��Ѿ��ó���ֵ
		{
			Key_Change=0;	  //������ʹ�ܱ������㣬�ȴ��´ΰ�������
			View_Change=1;
			switch(Key_Value)				//�жϼ�ֵ
			{
				case 1:						//���ü�����
				{
					View_Con++;			//���õ�λ��
					if(View_Con==3)		//�����úú󽫴˱�������
					View_Con=0;
					break;				//��������ͬ
				}
				case 2:						//�Ӽ�����
				{
					if(View_Con==2)		//�ж�����������
					{
						if(Xintiao_H<150)	//������ֵС��150
						Xintiao_H++;		//����+
					}
					if(View_Con==1)		//�������������
					{
						if(Xintiao_L<Xintiao_H-1)//����ֵС������-1������ֵ���ܳ������ޣ�
						Xintiao_L++;		//����ֵ��
					}
					break;
				}
				case 3:						//��������
				{
					if(View_Con==2)		//��������
					{
						if(Xintiao_H>Xintiao_L+1)//�������ݴ�������+1��ͬ������ֵ����С�����ޣ�
						Xintiao_H--;		//�������ݼ�
					}
					if(View_Con==1)		//��������
					{
						if(Xintiao_L>30)	//�������ݴ���30ʱ
						Xintiao_L--;		//�������ݼ�
					}
					break;
				} 
			}
		}
		if(View_Change)//����ʾ����
		{
			View_Change=0;//��������
			if(stop==0)			  //��������ʱ
			{
				if(View_Data[0]==0x30) //���λΪ0ʱ����ʾ
				View_Data[0]=' ';
			}
			else					  //���ʲ���������������5000��Ҳ���������ź�ʱ�䳬��5s������ʾ����
			{
				View_Data[0]=' ';
				View_Data[1]=' ';
				View_Data[2]=' ';
			}
			switch(View_Con)
			{
				case 0: //������ʾ
				{
				
					wr_max30100_one_data(0xae,0x06,0x0a);       // 0X06��ַB3λTEMP_EN��1
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
				   lcd_1602_word(0x80,16,"Heart Rate:     ");//��ʾһ������
					
			//		lcd_1602_word(0xc0,16,"SpO2:           ");//��ʾ�ڶ�������
				   lcd_1602_word(0x8d,3,View_Data);			 //�ڶ�����ʾ����
				   lcd_1602_word(0xc0,16,SpO2);			   //��ʾ�ڶ�������	
				    if(sendflag>0) 
					{
					sendduxin();
					}
					else{SpO2[15]=' ';}
					break;
				}
				case 1: //��������ʱ��ʾ
				{
					lcd_1602_word(0x80,16,"Heart Rate:     ");//��һ����ʾ����
					lcd_1602_word(0x8d,3,View_Data);
					
					View_L[0]=Xintiao_L/100+0x30;		//���������ݲ���
					View_L[1]=Xintiao_L%100/10+0x30;
					View_L[2]=Xintiao_L%10+0x30;
					
					if(View_L[0]==0x30)					//���λΪ0ʱ������ʾ
					View_L[0]=' ';
	
					lcd_1602_word(0xC0,16,"Warning L :     ");//�ڶ�����ʾ��������
					lcd_1602_word(0xCd,3,View_L);
					break;
				}
				case 2: //��������ʱ��ʾ��ͬ�ϣ�
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

void Time1() interrupt 3		//��ʱ��1������
{
	static uchar Key_Con,Xintiao_Con;
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //���¸���ֵ
	switch(Key_Con)   //�ް�������ʱ��ֵΪ0
	{
		case 0:		   //ÿ10msɨ��˴�
		{
			if((P1&0x07)!=0x07)//ɨ�谴���Ƿ��а���
			{
				Key_Con++;		  //�а��´�ֵ��1��ֵΪ1
			}
			break;
		}
		case 1:					  //10ms����ν����жϺ�ɨ��˴���Key_ConΪ1��
		{
			if((P1&0x07)!=0x07)//�ڶ��ν����ж�ʱ��������Ȼ�ǰ��£��𵽰�����ʱȥ�������ã�
			{
				Key_Con++;		  //������1��ֵΪ2
				switch(P1&0x07)  //�ж����ĸ���������
				{
					case 0x06:Key_Value=1;break;	 //�жϺð����󽫼�ֵ��ֵ������Key_Value
					case 0x05:Key_Value=2;break;
					case 0x03:Key_Value=3;break;
				}
			}
			else								 //���10msʱû�м�⵽�������£�����ʱ����̣�
			{
				Key_Con=0;						 //�������㣬���¼�ⰴ��
			}
			break;
		}
		case 2:									 //20ms���ⰴ��
		{
			if((P1&0x07)==0x07)				 //��ⰴ���Ƿ��ǰ���״̬
			{
				Key_Change=1;					 //�а�������ʹ�ܱ��������˱���Ϊ1ʱ�Żᴦ���ֵ���ݣ�
				Key_Con=0;						//�������㣬�ȴ��´��а�������
			}
			break;
		}
	}
	switch (Xintiao_Con)//�˴������水���ļ������
	{
		case 0:			 //Ĭ��Xintiao_Con��Ϊ0��
		{
			if(!Xintiao)//ÿ10ms������Ķ�ʱ�������һ�������Ƿ����ź�
			{
				Xintiao_Con++;//������źţ�������һ������ͻ���������
			}
			break;
		}
		case 1:
		{
			if(!Xintiao)	   //ÿ��10ms���һ���ź��Ƿ񻹴���
			{
				Xintiao_Con++;//���ھͼ�һ
			}
			else
			{
				Xintiao_Con=0;//����������ˣ����ʱ��̣ܶ�˵����⵽�Ĳ��������źţ��������������ţ����������㣬�����˴μ��
			} 
			break;
		}
		case 2:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//���ھͼ�һ
			}
			else
			{
				Xintiao_Con=0;//����������ˣ����ʱ��̣ܶ�˵����⵽�Ĳ��������źţ��������������ţ����������㣬�����˴μ��
			} 
			break;
		}
		case 3:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//���ھͼ�һ
			}
			else
			{
				Xintiao_Con=0;//����������ˣ����ʱ��̣ܶ�˵����⵽�Ĳ��������źţ��������������ţ����������㣬�����˴μ��
			} 
			break;
		}
		case 4:
		{
			if(Xintiao)//����30msһֱ���źţ��ж��˴��������źţ�ִ�����³���
			{
				if(Xintiao_Change==1)//���ʼ�ԭ��Ϊ�������������ʱ��������ʣ�����Xintiao_Change��һ������ʱΪ0�ģ������������else���ڶ���������
				{
					if(60000/Xintiao_Jishu>200)
					{
						View_Data[0]='-';	
						View_Data[1]='-';	 
						View_Data[2]='-';
						speaker=1;			//����	
					}
					else
					{
						View_Data[0]=(60000/Xintiao_Jishu)/100+0x30;		  //����������������ʾ��������ʱ����10msΪ��λ�����������м���������100�Σ�Ҳ����100*10ms=1000ms=1s
						View_Data[1]=(60000/Xintiao_Jishu)%100/10+0x30;	  //��ô�������һ���ӣ�60s�����������ǣ�60*1000/��100*10ms��=60��	  ����60��һ����60s��1000��һ����1000ms��100�Ǽ���ֵ��10��һ�μ�����Ӧ ��ʱ����10ms
						View_Data[2]=(60000/Xintiao_Jishu)%10+0x30;		  //�������������/100�õ������İ�λ��%100��ȡ��ģ����ǳ���100���������ٳ���10�͵õ�ʮλ�ˣ��Դ�����
								  //��ֺ�ĵ�������+0x30��Ŀ���ǵõ���Ӧ���ֵ�Һ����ʾ�룬����0��Ӧ��Һ����ʾ����0x30��1��0x30+1���Դ�����
						if(((60000/Xintiao_Jishu)>=Xintiao_H)||((60000/Xintiao_Jishu)<=Xintiao_L))//���ʲ��ڷ�Χ�ڱ���
						{
						speaker=0;			//��������
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
						speaker=1;			//����
						 dxflag3=dxflag2=0;
						}
					}
					xintiaoflag=60000/Xintiao_Jishu;
					View_Change=1;	   //��������ʺ�������ʾ
					Xintiao_Jishu=0;	   //������������
					Xintiao_Change=0;   //��������ʺ�ñ������㣬׼���´μ������
					stop=0;			   //��������ʺ�stop��
				}
				else//��һ������ʱXintiao_ChangeΪ0
				{
					Xintiao_Jishu=0;	//�����ʱ�������㣬��ʼ��ʱ
					Xintiao_Change=1;//Xintiao_Change��1��׼���ڶ��μ�⵽����ʱ��������
				}
				Xintiao_Con=0;	//���㣬׼�������һ������
				break;
			}
		}
	}
}
/**��ʱ��T0��������**/
void Time0() interrupt 1
{
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //���¸���ֵ
	Xintiao_Jishu++;  //����������
	if(Xintiao_Jishu==5000)//������������5000
	{
		Xintiao_Jishu=0;		//��������
		View_Change=1;		//��ʾλ��1
		Xintiao_Change=0;	//���㣬׼���ٴμ��
		stop=1;	   //������������5000��˵�����ʲ���������û�в����stop��1
		speaker=1;  //�رշ�����
	}
}
/**��ʱ����ʼ������**/
void Tim_Init()
{
	EA=1;			  //���ж��ܿ���
	ET0=1;			  //��T0�ж�������
	ET1=1;			  //��T1�ж�������
	TMOD=0x11;		  //�趨��ʱ��״̬
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //����ֵ
	
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //����ֵ
}
/**��ָ����ַ��ʾָ��������ָ���ַ�**/
/**Adress_Com��ʾ��ַ��Num_Adat��ʾ�ַ�������Adress_Data��ʾ�ַ�������**/ 
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data)
{
	uchar a=0;
	uchar Data_Word;
	LCD_WriteCom(Adress_Com); //ѡ�е�ַ
	for(a=0;a<Num_Adat;a++)   //forѭ��������ʾ�ַ�����
	{
		Data_Word=*Adress_Data;	  //��ȡ�ַ�������
		LCD_WriteData(Data_Word);  //��ʾ�ַ���
		Adress_Data++;			  //��ʾ��ַ��һ
	}
}

/***************1602����*******************/
void LCD_WriteData(uchar LCD_1602_DATA)	 /********LCD1602����д��***********/
{
	delay5ms();  //����ǰ������ʱ����֤�ź��ȶ�
	LCD_E=0;
	LCD_RS=1;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_DATA;
	LCD_E=0;
	LCD_RS=0;
}

/********LCD1602����д��***********/
void LCD_WriteCom(uchar LCD_1602_COM)
{
	delay5ms();//����ǰ������ʱ����֤�ź��ȶ�
	LCD_E=0;
	LCD_RS=0;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_COM;
	LCD_E=0;
	LCD_RS=0;
}


void InitLcd()		   //��ʼ��Һ������
{
	delay5ms();
	delay5ms();
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x06); //��ʾ����ƶ�λ��
	LCD_WriteCom(0x0c); //��ʾ�����������
	LCD_WriteCom(0x01); //��ʾ����
	delay5ms();
	delay5ms();
}

void delay5ms(void)   //5ms��ʱ����
{
	unsigned char a,b;
	for(b=185;b>0;b--)
	for(a=12;a>0;a--);
}
