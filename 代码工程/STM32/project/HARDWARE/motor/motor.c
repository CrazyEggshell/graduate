#include "motor.h"
void MOTOR_Init(void)
{
 GPIO_InitTypeDef  MOTOR_in;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
 MOTOR_in.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;				
 MOTOR_in.GPIO_Mode = GPIO_Mode_Out_PP; 		
 MOTOR_in.GPIO_Speed = GPIO_Speed_50MHz;	
 GPIO_Init(GPIOA, &MOTOR_in);	 
 GPIO_ResetBits(GPIOA,GPIO_Pin_5);
 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}
