#include "uart.h"
#include "math.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>


extern char flag;

/*
	·打开相应引脚的时钟
	·打开相关串口的时钟
	
	·配置相关引脚
	·配置串口通信模式
	
	·配置串口的中断（配置嵌套向量中断管理器NVIC）
	·打开串口的中断
	
	·使能串口

	·编写串口中断服务函数
	·把printf函数进行重定向（重新定义数据的输出方向）
*/

//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	if(flag == 1)
	{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);

		/* 等待发送完毕	数据发完标志位为1 */	
		while ( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);			
	}
	if(flag == 2)
	{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART2, (uint8_t) ch);

		/* 等待发送完毕	数据发完标志位为1 */	
		while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
	}
	
	
	return (ch);
}

//配置串口1  (PA9--TXD--发送引脚，  PA10--RXD--接收引脚）
void UART1_GPIO_Config(void)
{
	//定义变量
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	//打开相应引脚的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//配置相关引脚
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置串口通信模式
	USART_InitStructure.USART_BaudRate = 115200;	//波特率（通信速率）
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//（字长，串口接收一次数据的长度，即触发一次中断接收一次数据的长度）
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//没有检验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//开启接收 发送 模式
	
	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//这里设置优先级分组为组2，即抢占优先级等级有2^2个（4个），响应优先级等级有2^2个（4个）
	
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//只开启接收中断，因为需要能够及时接收数据

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}

//定义变量用于接收数据
char Rxdata;
char RxCounter;
char RxBuffer[256];


//编写串口中断服务函数
//需要注意的地方：该函数名称是固定的不可更改的，函数不得调用，不得声明，不得带返回值，不得传递参数。
void USART1_IRQHandler(void)
{
	//判断是否触发了串口1的接收中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//清除接收中断标志，方便下次判断状态发生变化
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		/* Read one byte from the receive data register */	//接收一个字节数据
		Rxdata = USART_ReceiveData(USART1);
			
		RxBuffer[RxCounter++] = Rxdata;
	}	
	
	/* Write one byte to the transmit data register */
	
	USART_SendData(USART1, Rxdata);
	
	//USART_SendData(USART1, RxBuffer[RxCounter++]);
	

	
}


//配置串口2  (PA2--TXD--发送引脚，  PA3--RXD--接收引脚）
void UART2_GPIO_Config(void)
{
	//定义变量
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	
	//打开相应引脚的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
	
	//配置相关引脚
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置串口通信模式
	USART_InitStructure.USART_BaudRate = 115200;	//波特率（通信速率）
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//（字长，串口接收一次数据的长度，即触发一次中断接收一次数据的长度）
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//没有检验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx;	//开启接收 发送 模式
	
	/* Configure USART1 */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART1 */
	USART_Cmd(USART2, ENABLE);
}

