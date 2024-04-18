#include "pwm.h"


void tim3_pwm_init(u16 arr , u32 fre)
{
	uint16_t PrescalerValue = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	
	
	/*GPIOB Configuration: TIM3 channel1*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  

	/* Compute the prescaler value */	//分频系数  71 = (72MHz / 2M) -1
	PrescalerValue = (uint16_t) (SystemCoreClock / fre) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = arr;	//重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计算
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//pwm配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);		//打开TIM3的通道3
	TIM_CtrlPWMOutputs(TIM3,ENABLE);		//控制pwm输出
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	

}


void GPIO_Shift(void)			//GPIO  PB0
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}


float Change_Number(float A,float B)							//对数据进行转换，转换为占空比
{
	float result;
	
	result = A/3.3*B;
	
	return result;
	
}
