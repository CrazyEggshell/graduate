/*******************************************************************
	SHT30
	此模块用于测量温湿度
	温度测量范围：-40 ~ 125
	湿度测量范围：  0 ~ 100
********************************************************************/

#include "sht30.h"
#include "delay.h"
#include "beep.h"

#define write 0
#define read  1
#define SCL PBout(6)
#define SDA_OUT PBout(7)
#define SDA_IN PBin(7)
#define IIC_INPUT_MODE_SET()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define IIC_OUTPUT_MODE_SET() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x30000000;}
 
float humiture[4];
u8 Refresh_SHT30_Data=0;
u8 humiture_buff1[20];
u8 humiture_buff2[20];
 
u8 Temperature_L=15;
u8 Humidity_L=50;
u8 Temperature_H=30;
u8 Humidity_H=80;

u16 tem1,hum1;

 void SHT30_Init(void)
{
  
  RCC->APB2ENR|=1<<3;
 
  GPIOB->CRL&=0X00FFFFFF;	
  GPIOB->CRL|=0X33000000;

  GPIOB->ODR|=3<<6; 
  //printf("SHT30_Init OK!\n");
}	

 /*主机发送ACK*/
void IIC_ACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  delay_us(2); 
  SDA_OUT=0;
  delay_us(2);     
  SCL=1;
  delay_us(2);                  
  SCL=0;                     
  delay_us(1);    
}

/*主机不发送ACK*/
void IIC_NACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  delay_us(2); 
  SDA_OUT=1;
  delay_us(2);      
  SCL=1;
  delay_us(2);                   
  SCL=0;                     
  delay_us(1);    
}

 /*主机等待从机的ACK响应*/
u8 IIC_wait_ACK(void)
{
    u8 t = 200;
    IIC_OUTPUT_MODE_SET();
    SDA_OUT=1;
    delay_us(1);
    SCL=0;
    delay_us(1); 
    IIC_INPUT_MODE_SET();
    delay_us(1); 
    while(SDA_IN)
    {
			t--;
			delay_us(1); 
			if(t==0)
			{
				SCL=0;
				return 1;
			}
			delay_us(1); 
    }
    delay_us(1);      
    SCL=1;
    delay_us(1);
    SCL=0;             
    delay_us(1);    
    return 0;	
}

/*******************************************************************
	启动IIC总线
********************************************************************/
void IIC_Start(void)
{
  IIC_OUTPUT_MODE_SET();
  SDA_OUT=1;
  SCL=1;
  delay_us(4);	
  SDA_OUT=0;
  delay_us(4); 
  SCL=0;
}
 
/*******************************************************************
	结束IIC总线
********************************************************************/
void IIC_Stop(void)
{
	IIC_OUTPUT_MODE_SET();
	SCL=0;
	SDA_OUT=0;  
	delay_us(4);	
	SCL=1;
	delay_us(4);
	SDA_OUT=1;
	delay_us(4);
}
 
/*******************************************************************
	字节数据发送函数 
********************************************************************/
void  IIC_SendByte(u8 byte)
{
	u8  BitCnt;
	IIC_OUTPUT_MODE_SET();
	SCL=0;
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{
		if(byte&0x80) SDA_OUT=1;
		else SDA_OUT=0; 
		byte<<=1;
		delay_us(2); 
		SCL=1;
		delay_us(2);
		SCL=0;
		delay_us(2);
	}
}

/*******************************************************************
	字节数据接收函数 
********************************************************************/
u8 IIC_RcvByte(void)
{
  u8 retc;
  u8 BitCnt;
  retc=0; 
  IIC_INPUT_MODE_SET();
  delay_us(1);                    
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {  
	SCL=0;
	delay_us(2);               
	SCL=1;               
	retc=retc<<1;
	if(SDA_IN) retc |=1;
	delay_us(1);
  }
  SCL=0;    
  return(retc);
}


/*******************************************************************
	主机读取从机的温湿度数据 
********************************************************************/
void SHT30_read_result(u8 addr)
{
	u16 tem,hum;
	u16 buff[6];
	float Temperature=0;
	float Humidity=0;
	
	IIC_Start();
	IIC_SendByte(addr<<1 | write);
	IIC_wait_ACK();
	IIC_SendByte(0x2C);
	IIC_wait_ACK();
	IIC_SendByte(0x06);
	IIC_wait_ACK();
	IIC_Stop();
	delay_ms(50);
	IIC_Start();
	IIC_SendByte(addr<<1 | read);
	if(IIC_wait_ACK()==0)
	{
		buff[0]=IIC_RcvByte();
		IIC_ACK();
		buff[1]=IIC_RcvByte();
		IIC_ACK();
		buff[2]=IIC_RcvByte();
		IIC_ACK();
		buff[3]=IIC_RcvByte();
		IIC_ACK();
		buff[4]=IIC_RcvByte();
		IIC_ACK();
		buff[5]=IIC_RcvByte();
		IIC_NACK();
		IIC_Stop();
	}
	
	tem = ((buff[0]<<8) | buff[1]);			//温度
	hum = ((buff[3]<<8) | buff[4]);			//湿度

	Temperature= (175.0*(float)tem/65535.0-45.0) ;// T = -45 + 175 * tem / (2^16-1)
	Humidity= (100.0*(float)hum/65535.0);// RH = hum*100 / (2^16-1)
	
	tem1 = Temperature;
	hum1 = Humidity;
	
	if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))   //将温湿度的数据写在同一行
	{
		humiture[0]=Temperature;
		humiture[2]=Humidity;
		sprintf((char *)humiture_buff1,"%6.2f*C %6.2f%%",Temperature,Humidity);
	}

	printf("温度  湿度 : %s\n",humiture_buff1);
	
	hum=0;
	tem=0;
}
