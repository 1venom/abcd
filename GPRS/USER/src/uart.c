#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "misc.h"
#include "User.h"

/*************************************************************************************/
//�������� UART1Init();
//����˵����RS232ͨ�ſ�
//���룺
//���أ�
/*************************************************************************************/
void UART1Init(void)   
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE );	
  GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	
	/* Configure USART1 Rx (PA10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOB, &GPIO_InitStructure );
	
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &USART_InitStructure );

	/* Configrate interrupts for modbus protocol. */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	//ʹ�ܷ��ͺ��ж�
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		 	  			
	USART_Cmd(USART1, ENABLE);						  			
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	  	
}
/*************************************************************************************/


void UART2Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//USART_ClockInitTypeDef  USART_ClockInitStructure;
	/* Enable USART2 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 , ENABLE );	
	
	/* Enable the USART2 Pins Software Remapping */
// 	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	
	/* Configure USART2 Rx (PA3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	/* Configure USART2 Tx (PA2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART2, &USART_InitStructure );

	/* Configrate interrupts for modbus protocol. */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ�ܷ��ͺ��ж�
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	 	  			
	USART_Cmd(USART2, ENABLE);						  			
//	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	  	
}

void UART3Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//USART_ClockInitTypeDef  USART_ClockInitStructure;
	/* Enable USART3 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE );	
	
	/* Enable the USART3 Pins Software Remapping */
 //	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	
	/* Configure USART3 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOB, &GPIO_InitStructure );
	
	/* Configure USART3 Tx (PB10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART3, &USART_InitStructure );

	/* Configrate interrupts for modbus protocol. */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ�ܷ��ͺ��ж�
	USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	 	  			
	USART_Cmd(USART3, ENABLE);						  			
//	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	  	
}


