#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "adc.h"
#include "light_adc.h"
#include "pwm.h"
#include "sht30.h"
#include "oled.h"
#include "uart.h"
#include "beep.h"
#include "motor.h"
#include "rain.h"

extern u16 tem1,hum1;

char flag = 1;
int tem2 = 50;							//为了传给esp32模块
int hum2 = 1;			

void tem_hum()		//温湿度检测并且对其他元器件进行控制
{
	SHT30_read_result(0x44);		//温湿度检测
	OLED_ShowNum(38,0,(int)tem1,2,16);
	OLED_ShowNum(100,0,(int)hum1,2,16);
	
	if(hum1 > 70)	//判断蜂鸣器是否报警
		BEEP = 1;
	else
		BEEP = 0;
	
	
	if(hum1 > 70)
	{
		MOTOR1 = 0;	
		MOTOR2 = 1;
		//TIM_SetCompare3(TIM3,((int)tem1 - 25)/40 * 100);		
	}
	else
	{
		MOTOR1 = 0;	
		MOTOR2 = 0;		
		//TIM_SetCompare3(TIM3,0);		
	}
//	if(hum1 > 70)								//温度湿度太高打开风扇											(并且控制快慢)
//		//TIM_SetCompare3(TIM3,((int)hum1 - 70)/30 * 100);
//	else
//		//TIM_SetCompare3(TIM3,0);
//	if(tem1 > 10)
//	{
//		MOTOR1 = 0;	
//		MOTOR2 = 1;
//		//TIM_SetCompare3(TIM3,((int)tem1 - 25)/40 * 100);		
//	}
//	else
//	{
//		MOTOR1 = 0;	
//		MOTOR2 = 0;		
//		//TIM_SetCompare3(TIM3,0);		
//	}

	
}

void light()			//光强监测
{
	float adcx;										//电压值
	float PhotoResistor;					//电阻值
	unsigned short LUX;										//光照强度
	//float	result;									//对3.3V的百分比
	
	adcx=Get_Adc_Average(ADC_Channel_1,10);
	adcx = (float)adcx * (3.3/4096);
	PhotoResistor = (adcx*100000/5)/(1-adcx/5);
	//OLED_ShowNum(50,2,(int)PhotoResistor,5,16);	//电阻	
	LUX = GetLux(PhotoResistor);									//光强

	OLED_ShowNum(36,2,LUX,4,16);	
	printf("光强: %d\n",LUX);

	
	//result = Change_Number(adcx,100);	

}

void Wifi()			//用于连接esp32
{
	flag = 2;
	
	printf("%d\n",tem2);
	printf("%d\n",hum2);	
	if(hum2 < 100)
	{
		hum2++;
	}
	else
	{
		hum2 = 1;
	}
	if(tem2 >0)
	{
		tem2--;;
	}
	else
	{
		tem2 = 50;
	}		
	
	flag = 1;
		
}

int main(void)
{
	
	delay_init();	    						//延时初始化
	SHT30_Init();									//温湿度模块初始化
	OLED_Init(); 								  //显示OLED初始化
	Adc_Init();										//ADC初始化
	//TIM2_Int_Init(5000,7199);   	//ADC 0.5s记一次
	//TIMER(1);											//使能ADC及其中断
	//tim3_pwm_init(100,20000);			//一周期为5ms
	UART1_GPIO_Config();					//uart1初始化
	UART2_GPIO_Config();					//uart2初始化
	BEEP_Init();									//蜂鸣器初始化
	//MOTOR_Init();									//风扇初始化
	RAIN_Init();									//雨滴传感器
	
	//TIM_SetCompare3(TIM3,0);
	OLED_Clear();									//清屏
	
	OLED_ShowChinese(0,0,8);			//温湿度
	OLED_ShowChinese(16,0,9);
	OLED_ShowChinese(62,0,12);
	OLED_ShowChinese(78,0,9);	
	
	OLED_ShowChinese(0,2,18);
	OLED_ShowChinese(16,2,19);	
	
	
	while(1)
	{
		tem_hum();				//温湿度检测并且对其他元器件进行控制
		
		light();					//光强监测

		if(RAIN == 0)			//检测是否下雨
		{
			printf("it is rain\n");
			OLED_ShowChinese(0,4,20);
			OLED_ShowChinese(16,4,21);
			OLED_ShowChinese(32,4,24);
			OLED_ShowChinese(48,4,25);
		}
		else
		{
			printf("it is no rain\n");
			OLED_ShowChinese(0,4,22);
			OLED_ShowChinese(16,4,23);
			OLED_ShowChinese(32,4,24);
			OLED_ShowChinese(48,4,25);
		}
		
		Wifi();		
		
		delay_ms(1000);
	}
}	
	
