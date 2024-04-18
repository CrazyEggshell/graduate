#include "beep.h"
void BEEP_Init(void)
{
 GPIO_InitTypeDef  BEEP_in;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
 BEEP_in.GPIO_Pin = GPIO_Pin_8;				
 BEEP_in.GPIO_Mode = GPIO_Mode_Out_PP; 		
 BEEP_in.GPIO_Speed = GPIO_Speed_50MHz;	
 GPIO_Init(GPIOB, &BEEP_in);	 
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);
}
