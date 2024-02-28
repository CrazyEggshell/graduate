#include "rain.h"
void RAIN_Init(void)
{
 GPIO_InitTypeDef  RAIN_in;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
 RAIN_in.GPIO_Pin = GPIO_Pin_4;				
 RAIN_in.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		
 GPIO_Init(GPIOB, &RAIN_in);	 
}
