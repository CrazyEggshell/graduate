#ifndef __LIGHT_ADC_H
#define __LIGHT_ADC_H

//����ͷ�ļ�
#include "stm32f10x.h"

//�궨��

//��������
typedef struct
{
	unsigned short ohm;//��������ֵ
	
	unsigned short lux;//����
	
}PhotoRes_TypeDef;

//��������
uint16_t GetLux(float PhotoResistor);

#endif
