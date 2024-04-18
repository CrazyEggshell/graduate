#ifndef _OLED_H
#define _OLED_H

#define	I2C_SCL_SET()  GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define I2C_SCL_CLR()  GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define I2C_SDA_SET()  GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define I2C_SDA_CLR()  GPIO_ResetBits(GPIOB, GPIO_Pin_11)
#define I2C_SDA_Read() GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11)

#define I2C_Ack    0
#define I2C_NO_Ack 1
#define OLED_CMD   0
#define OLED_DATA  1

void OLED_GPIO_Init(void);
static void I2C_Start(void);
static void I2C_Stop(void);
static unsigned char I2C_Wait_Ack(void);
static void I2C_WriteByte(unsigned char I2C_Byte);
static void OLED_Write_Cmd(unsigned char cmd);
static void OLED_Write_Data(unsigned char data);
void OLED_WR_Byte(unsigned char data,unsigned char cmd);
void OLED_Set_Pos(unsigned char x,unsigned char y);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Clear(void);
void OLED_Init(void);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char Char);
void OLED_ShowStr(unsigned char x,unsigned char y,unsigned char ch[]);
unsigned int OLED_Pow(unsigned char m,unsigned char n);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size);
void OLED_ShowChinese(unsigned char x,unsigned char y,unsigned char NO);

#endif

