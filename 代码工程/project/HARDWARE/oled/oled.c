#include "stm32f10x.h"
#include "oled.h"
#include "delay.h"
#include "oledfont.h"

void OLED_GPIO_Init(void)
{
	GPIO_InitTypeDef I2C_GPIOStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	I2C_GPIOStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
	I2C_GPIOStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
	I2C_GPIOStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &I2C_GPIOStructure);
	
	I2C_SDA_SET();
	I2C_SCL_SET();
	
}

static void I2C_Start(void)
{
	I2C_SDA_SET();
	I2C_SCL_SET();
	delay_us(1);
	
	I2C_SDA_CLR();
	delay_us(1);
	
	I2C_SCL_CLR();
	delay_us(1);
}

static void I2C_Stop(void)
{
	I2C_SDA_CLR();
	delay_us(1);
	
	I2C_SCL_SET();
	delay_us(1);
	
	I2C_SDA_SET();
	delay_us(1);
}

static unsigned char I2C_Wait_Ack(void)
{
	unsigned char Ack;
	
	I2C_SCL_CLR();
	delay_us(1);
	
	I2C_SDA_SET();
	delay_us(1);
	
	I2C_SCL_SET();
	delay_us(1);
	
	if( I2C_SDA_Read() )
	{
		Ack = I2C_NO_Ack;
	}
	else
	{
		Ack = I2C_Ack;
	}
	
	I2C_SCL_CLR();
	delay_us(1);
	
	return Ack;
}

static void I2C_WriteByte(unsigned char I2C_Byte)
{
	unsigned char i = 0;
	
	for(i=0;i<8;i++)
	{
		I2C_SCL_CLR();
		delay_us(1);
		
		if(I2C_Byte & 0x80)
		{
			I2C_SDA_SET();
		}
		else
		{
			I2C_SDA_CLR();
		}
		
		I2C_Byte <<= 1;
		delay_us(1);
		
		I2C_SCL_SET();
		delay_us(1);
	}
	
	I2C_SCL_CLR();
	delay_us(1);
	
	I2C_Wait_Ack();
}

static void OLED_Write_Cmd(unsigned char cmd)
{
	I2C_Start();
	I2C_WriteByte(0x78);
	I2C_WriteByte(0x00);
	I2C_WriteByte(cmd);
	I2C_Stop();
}

static void OLED_Write_Data(unsigned char data)
{
	I2C_Start();
	I2C_WriteByte(0x78);
	I2C_WriteByte(0x40);
	I2C_WriteByte(data);
	I2C_Stop();
}

void OLED_WR_Byte(unsigned char data,unsigned char cmd)
{
	if(cmd)
	{
		OLED_Write_Data(data);
	}
	else
	{
		OLED_Write_Cmd(data);
	}
}

void OLED_Set_Pos(unsigned char x,unsigned char y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01,OLED_CMD);
	OLED_WR_Byte((x & 0xf0) >>4 | 0x10,OLED_CMD);
}

void OLED_ON(void)
{
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
}

void OLED_OFF(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x10,OLED_CMD);
}

void OLED_Clear(void)
{
	unsigned char i = 0,j = 0;
	
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD);
		OLED_WR_Byte(0x00,OLED_CMD);
		OLED_WR_Byte(0x10,OLED_CMD);
		
		for(j=0;j<128;j++)
		{
			OLED_WR_Byte(0,OLED_DATA);
		}
	}
}

void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char Char)
{
	unsigned char c,i;
	
	c = Char - ' ';
			
	if(x>128)
	{
		x = 0;
		y += 2;
	
	}
	
	OLED_Set_Pos(x,y);
	
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
	}
	
	OLED_Set_Pos(x,y+1);
	
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
}

void OLED_ShowStr(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char i = 0;
	
	while(ch[i] != '\0')
	{
		OLED_ShowChar(x,y,ch[i]);
		
		if(x>128)
		{
			x = 0;
			y += 2;
		}
		
		i++;
	}
}

unsigned int OLED_Pow(unsigned char m,unsigned char n)
{
	unsigned int result = 1;
	
	while(n--)
	{
		result *= m;
	}
	
	return result;
}

void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{
	unsigned char headflag = 0;
	unsigned char temp;
	unsigned char t;
	
	for(t=0;t<len;t++)
	{
		temp = (num/OLED_Pow(10,len-t-1))%10;
		
		if(headflag==0 && t<(len-1)) //???num = 0 ???
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');
				continue;
			}
			else
			{
				headflag=1;
			}
		}
		
		OLED_ShowChar(x+(size/2)*t,y,temp+'0');
	}
}

void OLED_ShowChinese(unsigned char x,unsigned char y,unsigned char NO)
{
	unsigned char i = 0,addr = 0;
	
	OLED_Set_Pos(x,y);
	
	for(i=0;i<16;i++)
	{
		OLED_WR_Byte(Hzk[2*NO][i],OLED_DATA);
		addr += 1;
	}
	
	OLED_Set_Pos(x,y+1);
	
	for(i=0;i<16;i++)
	{
		OLED_WR_Byte(Hzk[2*NO+1][i],OLED_DATA);
		addr += 1;
	}
}



void OLED_Init(void)
{
	OLED_GPIO_Init();	
 
	delay_ms(200);	

	OLED_WR_Byte(0xAE,OLED_CMD);	

	OLED_WR_Byte(0x00,OLED_CMD);	
	OLED_WR_Byte(0x10,OLED_CMD);	
	OLED_WR_Byte(0x40,OLED_CMD);	
	OLED_WR_Byte(0xB0,OLED_CMD);	

	OLED_WR_Byte(0x81,OLED_CMD); 	
	OLED_WR_Byte(0xFF,OLED_CMD);	

	OLED_WR_Byte(0xA1,OLED_CMD);	
	OLED_WR_Byte(0xA6,OLED_CMD);	

	OLED_WR_Byte(0xA8,OLED_CMD);	
	OLED_WR_Byte(0x3F,OLED_CMD);	
	
	OLED_WR_Byte(0xC8,OLED_CMD);	

	OLED_WR_Byte(0xD3,OLED_CMD);	
	OLED_WR_Byte(0x00,OLED_CMD);	
	
	OLED_WR_Byte(0xD5,OLED_CMD);	
	OLED_WR_Byte(0x80,OLED_CMD);	
	
	OLED_WR_Byte(0xD9,OLED_CMD);	
	OLED_WR_Byte(0xF1,OLED_CMD);	
	
	OLED_WR_Byte(0xDA,OLED_CMD);	
	OLED_WR_Byte(0x12,OLED_CMD);	
	
	OLED_WR_Byte(0xDB,OLED_CMD);	
	OLED_WR_Byte(0x40,OLED_CMD);	
	
	OLED_WR_Byte(0x8D,OLED_CMD);	
	OLED_WR_Byte(0x14,OLED_CMD);	
	
	OLED_WR_Byte(0xAF,OLED_CMD);	
	OLED_Clear();        
	OLED_Set_Pos(0,0); 	 
} 

