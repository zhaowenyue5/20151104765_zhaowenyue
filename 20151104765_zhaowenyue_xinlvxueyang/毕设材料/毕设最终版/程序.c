#include <reg52.h>
#include <intrins.h>	 //			 °üº¬Í·ÎÄ¼ş
#include <stdio.h>
#include <string.H>
#define uint            unsigned int
#define uchar           unsigned char
#define ulong           unsigned long	 //ºê¶¨Òå
#define LCD_DATA        P0				 //¶¨ÒåP0¿ÚÎªLCD_DATA
uchar SpO2[16]=       "SpO2:           ";
sbit LCD_RS =P2^5;
sbit LCD_RW =P2^6;
sbit LCD_E  =P2^7;						 //¶¨ÒåLCD¿ØÖÆÒı½Å

sbit Xintiao =P3^2 ;					 //Âö²«¼ì²âÊäÈë¶Ë¶¨Òå
sbit speaker =P2^4;						 //·äÃùÆ÷Òı½Å¶¨Òå

void delay5ms(void);   //Îó²î 0us
void LCD_WriteData(uchar LCD_1602_DATA);	 /********LCD1602Êı¾İĞ´Èë***********/
void LCD_WriteCom(uchar LCD_1602_COM);		 /********LCD1602ÃüÁîĞ´Èë***********/
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data); /*1602×Ö·ûÏÔÊ¾º¯Êı£¬±äÁ¿ÒÀ´ÎÎª×Ö·ûÏÔÊ¾Ê×µØÖ·£¬ÏÔÊ¾×Ö·û³¤¶È£¬ËùÏÔÊ¾µÄ×Ö·û*/
void InitLcd();//Òº¾§³õÊ¼»¯º¯Êı

void Tim_Init();

uchar dxflag1=0,dxflag2=0,dxflag3=0;

uchar Xintiao_Change=0;	   //
uint  Xintiao_Jishu;
uchar stop;
uchar View_Data[3];
uchar View_L[3];
uchar View_H[3];
uchar Xintiao_H=100;	//Âö²«ÉÏÏŞ
uchar Xintiao_L=40;		//Âö²«ÏÂÏŞ

uint xintiaoflag;
uchar sendflag=0;

uchar Key_Change;
uchar Key_Value;		//°´¼ü¼üÖµ
uchar View_Con;			//ÉèÖÃµÄÎ»£¨0Õı³£¹¤×÷£¬1ÉèÖÃÉÏÏŞ£¬2ÉèÖÃÏÂÏŞ£©
uchar View_Change;


//////////////ÒÔÏÂÎªÑªÑõ¼°ÎÂ¶È²É¼¯³ÌĞò///////////////////
sbit IIC_SCL    =P1^3;     //IICµÄSCL
sbit IIC_SDA    =P1^4;     //IICµÄSDA
bit  IIC_ACK;              //IICµÄACK
int  rda;                  //IIC¶Á³ö
//-------------------------------------------------------------------------------------//
//º¯Êı:    delayms()
//¹¦ÄÜ:    ÑÓÊ±³ÌĞò
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
//º¯Êı:    void iic_start();
//¹¦ÄÜ:    I2C×ÜÏß¿ªÊ¼
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
//º¯Êı:    void iic_stop();
//¹¦ÄÜ:    I2C×ÜÏß½áÊø
//Ğè¶¨Òå£º
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
//º¯Êı:    void iic_sendbyte(unsigned char c);
//¹¦ÄÜ:    ·¢ËÍ 8_BIT Êı¾İ
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
//º¯Êı:    int iic_rcvbyte_nack();
//¹¦ÄÜ:    ½ÓÊÕ 8_BIT Êı¾İ  ×îºóackĞÅºÅ
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
//¸ø³öNACKĞÅºÅ
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
//º¯Êı:    int iic_rcvbyte_ack();
//¹¦ÄÜ:    ½ÓÊÕ 8_BIT Êı¾İ ×îºóackĞÅºÅ
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
//¸ø³öACKĞÅºÅ
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
//º¯Êı:      wr_max30100_one_data()
//¹¦ÄÜ:      Ğ´Ò»Î»max30100Êı¾İ
//address:   Ğ¾Æ¬´ÓµØÖ·
//saddress:  Ğ´¼Ä´æÆ÷µØÖ·
//w_data:    ´ıĞ´Êı¾İ
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
//º¯Êı:      rd_max30100_one_data()
//¹¦ÄÜ:      ¶ÁÒ»Î»max30100Êı¾İ
//address:   Ğ¾Æ¬´ÓµØÖ·
//saddress:  ¶Á¼Ä´æÆ÷µØÖ·
//rda:       ¶Á³öµÄÊı¾İ
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
//////////////ÒÔÉÏÎªÑªÑõ¼°ÎÂ¶È²É¼¯³ÌĞò///////////////////

//////////////ÒÔÏÂÎª¶ÌĞÅ·¢ËÍ³ÌĞò/////////////////////////
#define FOSC_110592M
unsigned int  rec_data_len_uart=0;    //±ê¼ÇBuffer_Uart0½ÓÊÕÊı×é
unsigned char idata Buffer_Uart0_Rec[25]={0};		 //Uart0ÖĞ¶Ï½ÓÊÕÊı×é
/*³õÊ¼»¯³ÌĞò£¨±ØĞëÊ¹ÓÃ£¬·ñÔòÎŞ·¨ÊÕ·¢£©£¬´Î³ÌĞò½«»áÊ¹ÓÃ¶¨Ê±Æ÷1*/
void SerialInti()//³õÊ¼»¯³ÌĞò£¨±ØĞëÊ¹ÓÃ£¬·ñÔòÎŞ·¨ÊÕ·¢£©
{
	TMOD=0x20;//¶¨Ê±Æ÷1²Ù×÷Ä£Ê½2:8Î»×Ô¶¯ÖØÔØ¶¨Ê±Æ÷

#ifdef FOSC_12M		   //ÔÚÕâÀï¸ù¾İ¾§Õñ´óĞ¡ÉèÖÃ²»Í¬µÄÊıÖµ³õÊ¼»¯´®¿Ú
	TH1=0xf3;//×°Èë³õÖµ£¬²¨ÌØÂÊ2400
	TL1=0xf3;	
#else 	
	TH1=0xfd;//×°Èë³õÖµ£¬²¨ÌØÂÊ9600
	TL1=0xfd;
#endif //end of SOC_12M
	
	TR1=1;//´ò¿ª¶¨Ê±Æ÷
	SM0=0;//ÉèÖÃ´®ĞĞÍ¨Ñ¶¹¤×÷Ä£Ê½£¬£¨10ÎªÒ»²¿·¢ËÍ£¬²¨ÌØÂÊ¿É±ä£¬ÓÉ¶¨Ê±Æ÷1µÄÒç³öÂÊ¿ØÖÆ£©
	SM1=1;//(Í¬ÉÏ)ÔÚ´ËÄ£Ê½ÏÂ£¬¶¨Ê±Æ÷Òç³öÒ»´Î¾Í·¢ËÍÒ»¸öÎ»µÄÊı¾İ
	REN=1;//´®ĞĞ½ÓÊÕÔÊĞíÎ»£¨ÒªÏÈÉèÖÃsm0sm1ÔÙ¿ª´®ĞĞÔÊĞí£©
	EA=1;//¿ª×ÜÖĞ¶Ï
	ES=1;//¿ª´®ĞĞ¿ÚÖĞ¶Ï	
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

/*´®ĞĞÍ¨Ñ¶ÖĞ¶Ï£¬ÊÕ·¢Íê³É½«½øÈë¸ÃÖĞ¶Ï*/
void Serial_interrupt() interrupt 4 
{
	unsigned char temp_rec_data_uart0;	
	temp_rec_data_uart0 = SBUF;//¶ÁÈ¡½ÓÊÕÊı¾İ		
	RI=0;//½ÓÊÕÖĞ¶ÏĞÅºÅÇåÁã£¬±íÊ¾½«¼ÌĞø½ÓÊÕ			
	Buffer_Uart0_Rec[rec_data_len_uart]=temp_rec_data_uart0;	//½ÓÊÕÊı¾İ
	rec_data_len_uart++;
	if(rec_data_len_uart>24)
	{
		rec_data_len_uart=0; 	//´ÓÍ·¿ªÊ¼½ÓÊÕÊı¾İ
	}	
}
void Uart1Send(uchar c)
{
	SBUF=c;
	while(!TI);//µÈ´ı·¢ËÍÍê³ÉĞÅºÅ£¨TI=1£©³öÏÖ
	TI=0;	
}
//´®ĞĞ¿ÚÁ¬Ğø·¢ËÍcharĞÍÊı×é£¬Óöµ½ÖÕÖ¹ºÅ/0½«Í£Ö¹
void Uart1Sends(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(!TI);//µÈ´ı·¢ËÍÍê³ÉĞÅºÅ£¨TI=1£©³öÏÖ
		TI=0;
		str++;
	}
}
//ÑÓÊ±º¯Êı´ó¸ÅÊÇ1sÖÓ£¬²»¹ıÑÓÊ±´óµÄ»°²»×¼...
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
	ET0=0;			  //¹Ø±ÕT0ÖĞ¶ÏÔÊĞí¿ª¹Ø
	ET1=0;			  //¹Ø±ÕT1ÖĞ¶ÏÔÊĞí¿ª¹Ø
	SerialInti();
	Uart1Sends("AT+CSCS=\"UCS2\"\r\n");
    DelaySec(1);//ÑÓÊ±´óÔ¼3Ãë
    Uart1Sends("AT+CMGF=1\r\n");
    DelaySec(1);//ÑÓÊ±3Ãë												
	Uart1Sends("AT+CSCA?\r\n");											
    DelaySec(1);//ÑÓÊ±3Ãë
	Uart1Sends("AT+CSMP=17,167,0,25\r\n");
    DelaySec(1);//ÑÓÊ±3Ãë
    Uart1Sends("AT+CMGS=\"00310038003800340037003100330038003000300033\"\r\n");//´Ë´¦ĞŞ¸ÄÎª¶Ô·½µÄµç»°ºÅ£¬µç»°»°µÄUNICON±àÂë£¬¿ÉÊ¹ÓÃÎÒÃÇÅäÌ×µÄ
																				//×ÊÁÏ°üÖĞµÄ ÖĞÎÄ×ªUNICON±àÂë¹¤¾ßÀ´»ñÈ¡¡£ÌØ±ğ×¢Òâ£ºÄÇ¸öÈí¼ş
																				//»ñµÃµÄunicon ±àÂë´ıÓĞ¿Õ¸ñ£¬Êµ¼ÊÖĞÒªÈ¥µô£¬·ñÔòÄ£¿é»á±¨´í
    DelaySec(1);//ÑÓÊ±3Ãë
	if(sendflag==1)
	{
    Uart1Sends("6CE8610FFF1A88406C275F025E38FF01");//ĞŞ¸Ä¶ÌĞÅÄÚÈİ,¶ÌĞÅÄÚÈİ¿ÉÎªÖĞÎÄ¡¢Ó¢ÎÄºÍÊı×Ö£¬µ«¶¼ÊÇ¶ÔÓ¦µÄunicon ±àÂë¡£
	}
	else if(sendflag==2)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC79AD8FF01");//ĞŞ¸Ä¶ÌĞÅÄÚÈİ,¶ÌĞÅÄÚÈİ¿ÉÎªÖĞÎÄ¡¢Ó¢ÎÄºÍÊı×Ö£¬µ«¶¼ÊÇ¶ÔÓ¦µÄunicon ±àÂë¡£
	}
	else if(sendflag==3)
	{
    Uart1Sends("6CE8610FFF1A5FC38DF38FC74F4EFF01");//ĞŞ¸Ä¶ÌĞÅÄÚÈİ,¶ÌĞÅÄÚÈİ¿ÉÎªÖĞÎÄ¡¢Ó¢ÎÄºÍÊı×Ö£¬µ«¶¼ÊÇ¶ÔÓ¦µÄunicon ±àÂë¡£
	}
    Uart1Send(0x1a);
    DelaySec(10);//ÑÓÊ±20Ãë
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


////////ÒÔÉÏÎª¶ÌĞÅ·¢ËÍ³ÌĞò///////////////////////////
void main()	  //Ö÷º¯Êı
{
    uint temp,temp1,temp2,xy;

	InitLcd();
	Tim_Init();
  //  DelaySec(5);//ÑÓÊ±Ô¼15Ãë,´Ë´¦ÑÓÊ±£¬ÊÇÎªÁËÈÃÄ£¿éÓĞ×ã¹»µÄÊ±¼ä×¢²áµ½ÍøÂç£¬
	lcd_1602_word(0x80,16,"Heart Rate:     ");	  //³õÊ¼»¯ÏÔÊ¾
	lcd_1602_word(0xc0,16,"SpO2:           ");
	TR0=1;
	TR1=1;				  //´ò¿ª¶¨Ê±Æ÷
//	while(1)
//	{
//	 sendflag=1;
//	 sendduxin();
//	}

	while(1)			  //½øÈëÑ­»·
	{
		if(Key_Change)	  //ÓĞ°´¼ü°´ÏÂ²¢ÒÑ¾­µÃ³ö¼üÖµ
		{
			Key_Change=0;	  //½«°´¼üÊ¹ÄÜ±äÁ¿ÇåÁã£¬µÈ´ıÏÂ´Î°´¼ü°´ÏÂ
			View_Change=1;
			switch(Key_Value)				//ÅĞ¶Ï¼üÖµ
			{
				case 1:						//ÉèÖÃ¼ü°´ÏÂ
				{
					View_Con++;			//ÉèÖÃµÄÎ»¼Ó
					if(View_Con==3)		//¶¼ÉèÖÃºÃºó½«´Ë±äÁ¿ÇåÁã
					View_Con=0;
					break;				//Ìø³ö£¬ÏÂÍ¬
				}
				case 2:						//¼Ó¼ü°´ÏÂ
				{
					if(View_Con==2)		//ÅĞ¶ÏÊÇÉèÖÃÉÏÏŞ
					{
						if(Xintiao_H<150)	//ÉÏÏŞÊıÖµĞ¡ÓÚ150
						Xintiao_H++;		//ÉÏÏŞ+
					}
					if(View_Con==1)		//Èç¹ûÊÇÉèÖÃÏÂÏŞ
					{
						if(Xintiao_L<Xintiao_H-1)//ÏÂÏŞÖµĞ¡ÓÚÉÏÏŞ-1£¨ÏÂÏŞÖµ²»ÄÜ³¬¹ıÉÏÏŞ£©
						Xintiao_L++;		//ÏÂÏŞÖµ¼Ó
					}
					break;
				}
				case 3:						//¼õ¼ü°´ÏÂ
				{
					if(View_Con==2)		//ÉèÖÃÉÏÏŞ
					{
						if(Xintiao_H>Xintiao_L+1)//ÉÏÏŞÊı¾İ´óÓÚÏÂÏŞ+1£¨Í¬ÑùÉÏÏŞÖµ²»ÄÜĞ¡ÓÚÏÂÏŞ£©
						Xintiao_H--;		//ÉÏÏŞÊı¾İ¼õ
					}
					if(View_Con==1)		//ÉèÖÃÏÂÏŞ
					{
						if(Xintiao_L>30)	//ÏÂÏŞÊı¾İ´óÓÚ30Ê±
						Xintiao_L--;		//ÏÂÏŞÊı¾İ¼õ
					}
					break;
				} 
			}
		}
		if(View_Change)//ª¼ÏÔÊ¾±äÁ¿
		{
			View_Change=0;//±äÁ¿ÇåÁã
			if(stop==0)			  //ĞÄÂÊÕı³£Ê±
			{
				if(View_Data[0]==0x30) //×î¸ßÎ»Îª0Ê±²»ÏÔÊ¾
				View_Data[0]=' ';
			}
			else					  //ĞÄÂÊ²»Õı³££¨¼ÆÊı³¬¹ı5000£¬Ò²¾ÍÊÇÁ½´ÎĞÅºÅÊ±¼ä³¬¹ı5s£©²»ÏÔÊ¾Êı¾İ
			{
				View_Data[0]=' ';
				View_Data[1]=' ';
				View_Data[2]=' ';
			}
			switch(View_Con)
			{
				case 0: //Õı³£ÏÔÊ¾
				{
				
					wr_max30100_one_data(0xae,0x06,0x0a);       // 0X06µØÖ·B3Î»TEMP_ENÖÃ1
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
				   lcd_1602_word(0x80,16,"Heart Rate:     ");//ÏÔÊ¾Ò»ĞĞÊı¾İ
					
			//		lcd_1602_word(0xc0,16,"SpO2:           ");//ÏÔÊ¾µÚ¶şĞĞÊı¾İ
				   lcd_1602_word(0x8d,3,View_Data);			 //µÚ¶şĞĞÏÔÊ¾ĞÄÂÊ
				   lcd_1602_word(0xc0,16,SpO2);			   //ÏÔÊ¾µÚ¶şĞĞÊı¾İ	
				    if(sendflag>0) 
					{
					sendduxin();
					}
					else{SpO2[15]=' ';}
					break;
				}
				case 1: //ÉèÖÃÏÂÏŞÊ±ÏÔÊ¾
				{
					lcd_1602_word(0x80,16,"Heart Rate:     ");//µÚÒ»ĞĞÏÔÊ¾ĞÄÂÊ
					lcd_1602_word(0x8d,3,View_Data);
					
					View_L[0]=Xintiao_L/100+0x30;		//½«ÏÂÏŞÊı¾İ²ğ×Ö
					View_L[1]=Xintiao_L%100/10+0x30;
					View_L[2]=Xintiao_L%10+0x30;
					
					if(View_L[0]==0x30)					//×î¸ßÎ»Îª0Ê±£¬²»ÏÔÊ¾
					View_L[0]=' ';
	
					lcd_1602_word(0xC0,16,"Warning L :     ");//µÚ¶şĞĞÏÔÊ¾ÏÂÏŞÊı¾İ
					lcd_1602_word(0xCd,3,View_L);
					break;
				}
				case 2: //ÉèÖÃÉÏÏŞÊ±ÏÔÊ¾£¨Í¬ÉÏ£©
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

void Time1() interrupt 3		//¶¨Ê±Æ÷1·şÎñº¯Êı
{
	static uchar Key_Con,Xintiao_Con;
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //ÖØĞÂ¸³³õÖµ
	switch(Key_Con)   //ÎŞ°´¼ü°´ÏÂÊ±´ËÖµÎª0
	{
		case 0:		   //Ã¿10msÉ¨Ãè´Ë´¦
		{
			if((P1&0x07)!=0x07)//É¨Ãè°´¼üÊÇ·ñÓĞ°´ÏÂ
			{
				Key_Con++;		  //ÓĞ°´ÏÂ´ËÖµ¼Ó1£¬ÖµÎª1
			}
			break;
		}
		case 1:					  //10msºó¶ş´Î½øÈëÖĞ¶ÏºóÉ¨Ãè´Ë´¦£¨Key_ConÎª1£©
		{
			if((P1&0x07)!=0x07)//µÚ¶ş´Î½øÈëÖĞ¶ÏÊ±£¬°´¼üÈÔÈ»ÊÇ°´ÏÂ£¨Æğµ½°´¼üÑÓÊ±È¥¶¶µÄ×÷ÓÃ£©
			{
				Key_Con++;		  //±äÁ¿¼Ó1£¬ÖµÎª2
				switch(P1&0x07)  //ÅĞ¶ÏÊÇÄÄ¸ö°´¼ü°´ÏÂ
				{
					case 0x06:Key_Value=1;break;	 //ÅĞ¶ÏºÃ°´¼üºó½«¼üÖµ¸³Öµ¸ø±äÁ¿Key_Value
					case 0x05:Key_Value=2;break;
					case 0x03:Key_Value=3;break;
				}
			}
			else								 //Èç¹û10msÊ±Ã»ÓĞ¼ì²âµ½°´¼ü°´ÏÂ£¨°´ÏÂÊ±¼ä¹ı¶Ì£©
			{
				Key_Con=0;						 //±äÁ¿ÇåÁã£¬ÖØĞÂ¼ì²â°´¼ü
			}
			break;
		}
		case 2:									 //20msºó¼ì²â°´¼ü
		{
			if((P1&0x07)==0x07)				 //¼ì²â°´¼üÊÇ·ñ»¹ÊÇ°´ÏÂ×´Ì¬
			{
				Key_Change=1;					 //ÓĞ°´¼ü°´ÏÂÊ¹ÄÜ±äÁ¿£¬£¨´Ë±äÁ¿Îª1Ê±²Å»á´¦Àí¼üÖµÊı¾İ£©
				Key_Con=0;						//±äÁ¿ÇåÁã£¬µÈ´ıÏÂ´ÎÓĞ°´¼ü°´ÏÂ
			}
			break;
		}
	}
	switch (Xintiao_Con)//´Ë´¦ÓëÉÏÃæ°´¼üµÄ¼ì²âÀàËÆ
	{
		case 0:			 //Ä¬ÈÏXintiao_ConÊÇÎª0µÄ
		{
			if(!Xintiao)//Ã¿10ms£¨ÉÏÃæµÄ¶¨Ê±Æ÷£©¼ì²âÒ»´ÎÂö²«ÊÇ·ñÓĞĞÅºÅ
			{
				Xintiao_Con++;//Èç¹ûÓĞĞÅºÅ£¬±äÁ¿¼ÓÒ»£¬³ÌĞò¾Í»áÍùÏÂ×ßÁË
			}
			break;
		}
		case 1:
		{
			if(!Xintiao)	   //Ã¿¹ı10ms¼ì²âÒ»ÏÂĞÅºÅÊÇ·ñ»¹´æÔÚ
			{
				Xintiao_Con++;//´æÔÚ¾Í¼ÓÒ»
			}
			else
			{
				Xintiao_Con=0;//Èç¹û²»´æÔÚÁË£¬¼ì²âÊ±¼äºÜ¶Ì£¬ËµÃ÷¼ì²âµ½µÄ²»ÊÇÂö²«ĞÅºÅ£¬¿ÉÄÜÊÇÆäËû¸ÉÈÅ£¬½«±äÁ¿ÇåÁã£¬Ìø³ö´Ë´Î¼ì²â
			} 
			break;
		}
		case 2:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//´æÔÚ¾Í¼ÓÒ»
			}
			else
			{
				Xintiao_Con=0;//Èç¹û²»´æÔÚÁË£¬¼ì²âÊ±¼äºÜ¶Ì£¬ËµÃ÷¼ì²âµ½µÄ²»ÊÇÂö²«ĞÅºÅ£¬¿ÉÄÜÊÇÆäËû¸ÉÈÅ£¬½«±äÁ¿ÇåÁã£¬Ìø³ö´Ë´Î¼ì²â
			} 
			break;
		}
		case 3:
		{
			if(!Xintiao)
			{
				Xintiao_Con++;//´æÔÚ¾Í¼ÓÒ»
			}
			else
			{
				Xintiao_Con=0;//Èç¹û²»´æÔÚÁË£¬¼ì²âÊ±¼äºÜ¶Ì£¬ËµÃ÷¼ì²âµ½µÄ²»ÊÇÂö²«ĞÅºÅ£¬¿ÉÄÜÊÇÆäËû¸ÉÈÅ£¬½«±äÁ¿ÇåÁã£¬Ìø³ö´Ë´Î¼ì²â
			} 
			break;
		}
		case 4:
		{
			if(Xintiao)//³¬¹ı30msÒ»Ö±ÓĞĞÅºÅ£¬ÅĞ¶¨´Ë´ÎÊÇÂö²«ĞÅºÅ£¬Ö´ĞĞÒÔÏÂ³ÌĞò
			{
				if(Xintiao_Change==1)//ĞÄÂÊ¼ÆÔ­ÀíÎª¼ì²âÁ½´ÎÂö³å¼ä¸ôÊ±¼ä¼ÆËãĞÄÂÊ£¬±äÁ¿Xintiao_ChangeµÚÒ»´ÎÂö³åÊ±Îª0µÄ£¬ËùÓĞ×ßÏÂÃæµÄelse£¬µÚ¶ş´Î×ßÕâÀï
				{
					if(60000/Xintiao_Jishu>200)
					{
						View_Data[0]='-';	
						View_Data[1]='-';	 
						View_Data[2]='-';
						speaker=1;			//²»Ïì	
					}
					else
					{
						View_Data[0]=(60000/Xintiao_Jishu)/100+0x30;		  //¼ÆËãĞÄÌø²¢²ğ×ÖÏÔÊ¾£ºĞÄÌø¼ÆÊ±ÊÇÒÔ10msÎªµ¥Î»£¬Á½´ÎĞÄÌøÖĞ¼ä¼ÆÊıÈç¹ûÊÇ100´Î£¬Ò²¾ÍÊÇ100*10ms=1000ms=1s
						View_Data[1]=(60000/Xintiao_Jishu)%100/10+0x30;	  //ÄÇÃ´¼ÆËã³öµÄÒ»·ÖÖÓ£¨60s£©ĞÄÌøÊı¾ÍÊÇ£º60*1000/£¨100*10ms£©=60´Î	  ÆäÖĞ60ÊÇÒ»·ÖÖÓ60s£¬1000ÊÇÒ»ÃëÓĞ1000ms£¬100ÊÇ¼ÆÊıÖµ£¬10ÊÇÒ»´Î¼ÆÊı¶ÔÓ¦ µÄÊ±¼äÊÇ10ms
						View_Data[2]=(60000/Xintiao_Jishu)%10+0x30;		  //¼ÆËã³öµÄĞÄÌøÊı/100µÃµ½ĞÄÌøµÄ°ÙÎ»£¬%100ÊÇÈ¡ÓàµÄ£¬¾ÍÊÇ³ıÒÔ100µÄÓàÊı£¬ÔÙ³ıÒÔ10¾ÍµÃµ½Ê®Î»ÁË£¬ÒÔ´ËÀàÍÆ
								  //ğ×ÖºóµÄµ¥¸öÊı¾İ+0x30µÄÄ¿µÄÊÇµÃµ½¶ÔÓ¦Êı×ÖµÄÒº¾§ÏÔÊ¾Âë£¬Êı×Ö0¶ÔÓ¦µÄÒº¾§ÏÔÊ¾ÂëÊÇ0x30£¬1ÊÇ0x30+1£¬ÒÔ´ËÀàÍÆ
						if(((60000/Xintiao_Jishu)>=Xintiao_H)||((60000/Xintiao_Jishu)<=Xintiao_L))//ĞÄÂÊ²»ÔÚ·¶Î§ÄÚ±¨¾¯
						{
						speaker=0;			//·äÃùÆ÷Ïì
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
						speaker=1;			//²»Ïì
						 dxflag3=dxflag2=0;
						}
					}
					xintiaoflag=60000/Xintiao_Jishu;
					View_Change=1;	   //¼ÆËã³öĞÄÂÊºóÆô¶¯ÏÔÊ¾
					Xintiao_Jishu=0;	   //ĞÄÌø¼ÆÊıÇåÁã
					Xintiao_Change=0;   //¼ÆËã³öĞÄÂÊºó¸Ã±äÁ¿ÇåÁã£¬×¼±¸ÏÂ´Î¼ì²âĞÄÂÊ
					stop=0;			   //¼ÆËã³öĞÄÂÊºóstopåã
				}
				else//µÚÒ»´ÎÂö³åÊ±Xintiao_ChangeÎª0
				{
					Xintiao_Jishu=0;	//Âö³å¼ÆÊ±±äÁ¿ÇåÁã£¬¿ªÊ¼¼ÆÊ±
					Xintiao_Change=1;//Xintiao_ChangeÖÃ1£¬×¼±¸µÚ¶ş´Î¼ì²âµ½Âö³åÊ±¼ÆËãĞÄÂÊ
				}
				Xintiao_Con=0;	//ÇåÁã£¬×¼±¸¼ì²âÏÂÒ»´ÎÂö³å
				break;
			}
		}
	}
}
/**¶¨Ê±Æ÷T0¹¤×÷º¯Êı**/
void Time0() interrupt 1
{
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //ÖØĞÂ¸³³õÖµ
	Xintiao_Jishu++;  //ĞÄÌø¼ÆÊı¼Ó
	if(Xintiao_Jishu==5000)//ĞÄÌø¼ÆÊı´óÓÚ5000
	{
		Xintiao_Jishu=0;		//Êı¾İÇåÁã
		View_Change=1;		//ÏÔÊ¾Î»ÖÃ1
		Xintiao_Change=0;	//ÖÃÁã£¬×¼±¸ÔÙ´Î¼ì²â
		stop=1;	   //ĞÄÌø¼ÆÊı³¬¹ı5000ºóËµÃ÷ĞÄÂÊ²»Õı³£»òÕßÃ»ÓĞ²â³ö£¬stopÖÃ1
		speaker=1;  //¹Ø±Õ·äÃùÆ÷
	}
}
/**¶¨Ê±Æ÷³õÊ¼»¯º¯Êı**/
void Tim_Init()
{
	EA=1;			  //´ò¿ªÖĞ¶Ï×Ü¿ª¹Ø
	ET0=1;			  //´ò¿ªT0ÖĞ¶ÏÔÊĞí¿ª¹Ø
	ET1=1;			  //´ò¿ªT1ÖĞ¶ÏÔÊĞí¿ª¹Ø
	TMOD=0x11;		  //Éè¶¨¶¨Ê±Æ÷×´Ì¬
	TH0=0xfc;		   //1ms
	TL0=0x18;		   //¸³³õÖµ
	
	TH1=0xd8;		   //10ms
	TL1=0xf0;		   //¸³³õÖµ
}
/**ÔÚÖ¸¶¨µØÖ·ÏÔÊ¾Ö¸¶¨ÊıÁ¿µÄÖ¸¶¨×Ö·û**/
/**Adress_ComÏÔÊ¾µØÖ·£¬Num_AdatÏÔÊ¾×Ö·ûÊıÁ¿£¬Adress_DataÏÔÊ¾×Ö·û´®ÄÚÈİ**/ 
void lcd_1602_word(uchar Adress_Com,uchar Num_Adat,uchar *Adress_Data)
{
	uchar a=0;
	uchar Data_Word;
	LCD_WriteCom(Adress_Com); //Ñ¡ÖĞµØÖ·
	for(a=0;a<Num_Adat;a++)   //forÑ­»·¾ö¶¨ÏÔÊ¾×Ö·û¸öÊı
	{
		Data_Word=*Adress_Data;	  //¶ÁÈ¡×Ö·û´®Êı¾İ
		LCD_WriteData(Data_Word);  //ÏÔÊ¾×Ö·û´®
		Adress_Data++;			  //ÏÔÊ¾µØÖ·¼ÓÒ»
	}
}

/***************1602º¯Êı*******************/
void LCD_WriteData(uchar LCD_1602_DATA)	 /********LCD1602Êı¾İĞ´Èë***********/
{
	delay5ms();  //²Ù×÷Ç°¶ÌÔİÑÓÊ±£¬±£Ö¤ĞÅºÅÎÈ¶¨
	LCD_E=0;
	LCD_RS=1;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_DATA;
	LCD_E=0;
	LCD_RS=0;
}

/********LCD1602ÃüÁîĞ´Èë***********/
void LCD_WriteCom(uchar LCD_1602_COM)
{
	delay5ms();//²Ù×÷Ç°¶ÌÔİÑÓÊ±£¬±£Ö¤ĞÅºÅÎÈ¶¨
	LCD_E=0;
	LCD_RS=0;
	LCD_RW=0;
	_nop_();
	LCD_E=1;
	LCD_DATA=LCD_1602_COM;
	LCD_E=0;
	LCD_RS=0;
}


void InitLcd()		   //³õÊ¼»¯Òº¾§º¯Êı
{
	delay5ms();
	delay5ms();
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x38); //display mode
	LCD_WriteCom(0x06); //ÏÔÊ¾¹â±êÒÆ¶¯Î»ÖÃ
	LCD_WriteCom(0x0c); //ÏÔÊ¾¿ª¼°¹â±êÉèÖÃ
	LCD_WriteCom(0x01); //ÏÔÊ¾ÇåÆÁ
	delay5ms();
	delay5ms();
}

void delay5ms(void)   //5msÑÓÊ±º¯Êı
{
	unsigned char a,b;
	for(b=185;b>0;b--)
	for(a=12;a>0;a--);
}
