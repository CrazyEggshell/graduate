#include "stm32f10x.h"
#include "sys.h"

#ifndef MOTOR_INIT_
#define MOTOR_INIT_



void tim3_pwm_init(u16 arr , u32 fre);								//pwm初始化
void GPIO_Shift(void);																//GPIO初始化
float Change_Number(float A,float B);		//对数据进行转换，转换为占空比						

#endif

