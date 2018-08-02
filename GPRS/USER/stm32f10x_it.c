/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "modbus.h"
#include "io.h"
extern volatile uint64_t Systick_1ms;
extern unsigned char ReadFlagUart3;

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
	
	Systick_1ms++;
}


void USART1_IRQHandler(void)
{	

    if(USART1->SR&(1<<5))
	{ 
		USART1->SR &= (~(1<<5));
		ucUart1ReceTimeOut = 3;
		ucUart1Buff[ucUart1ReceLen++] = USART1->DR;	
    ReadFlagUart1 = 1;
    }

	if(USART1->SR&(1<<6))
	{
		USART1->SR &= (~(1<<6));
		if(ucUart1SendPosi < ucUart1SendLen)
		{	
			USART1->DR = ucUart1SendBuff[ucUart1SendPosi++];
		}
		else
		{		
			ucUart1ReceLen = 0;
			ucUart1ReceTimeOut = 0;
		}
	}
}
//
void USART2_IRQHandler(void)
{	
    if(USART2->SR&(1<<5))                     //接收中断
		{ 
		USART2->SR &= (~(1<<5));
		ucUart2ReceTimeOut = 3;	
		if(ucUart2ReceLen>=1400)  ucUart2ReceLen =1400;		
		ucUart2Buff[ucUart2ReceLen++] = USART2->DR;		
		ReadFlag = 1;			
    }
   
	if(USART2->SR&(1<<6))                     //发送中断
	{
		USART2->SR &= (~(1<<6));
		if(ucUart2SendPosi < ucUart2SendLen)
		{	
			USART2->DR = ucUart2SendBuff[ucUart2SendPosi++];
		}
		else
		{	
			ReadFlag = 0;
			WriteFlag = 1;
			ucUart2ReceLen = 0;
			ucUart2ReceTimeOut = 0;
		}
	
	}
		
}

void USART3_IRQHandler(void)
{	
  if(USART3->SR&(1<<5))
	{ 
		USART3->SR &= (~(1<<5));
		ucUart3ReceTimeOut = 3;
		ucUart3Buff[ucUart3ReceLen++] = USART3->DR;	
		ReadFlagUart3 = 1;
   }

	if(USART3->SR&(1<<6))
	{
		USART3->SR &= (~(1<<6));
		if(ucUart3SendPosi < ucUart3SendLen)
		{	
			USART3->DR=ucUart3SendBuff[ucUart3SendPosi++];
			
		}
		else
		{		
			ucUart3ReceLen = 0;
			ucUart3ReceTimeOut = 0;
		}
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
