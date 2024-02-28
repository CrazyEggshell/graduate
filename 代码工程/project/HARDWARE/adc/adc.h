#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"



extern char Flag;


void Adc_Init(void);								//ADC初始化
void TIM2_Int_Init(u16 arr,u16 psc);//定时器2初始化
void TIMER(u8 NewState);						//控制定时器中断
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
 
#endif 
