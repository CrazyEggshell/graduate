#ifndef __LIGHT_ADC_H
#define __LIGHT_ADC_H

//包含头文件
#include "stm32f10x.h"

//宏定义

//变量声明
typedef struct
{
	unsigned short ohm;//光敏电阻值
	
	unsigned short lux;//流明
	
}PhotoRes_TypeDef;

//函数声明
uint16_t GetLux(float PhotoResistor);

#endif
