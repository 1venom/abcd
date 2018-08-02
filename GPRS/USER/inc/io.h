#ifndef __IO_H
#define __IO_H

#include "user.h"






#define PA4_H        (GPIOA->BSRR = GPIO_Pin_4)
#define PA4_L        (GPIOA->BRR  = GPIO_Pin_4) 
#define PA5_H        (GPIOA->BSRR = GPIO_Pin_5) 
#define PA5_L        (GPIOA->BRR  = GPIO_Pin_5) 
#define PA11_H       (GPIOA->BSRR = GPIO_Pin_11)
#define PA11_L       (GPIOA->BRR  = GPIO_Pin_11) 
#define PA12_H       (GPIOA->BSRR = GPIO_Pin_12)
#define PA12_L       (GPIOA->BRR  = GPIO_Pin_12) 

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define GPIOA_ODR_Addr    0x4001080C 
#define GPIOB_ODR_Addr    0x40010C0C
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n) 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n) 
#define LED2G PAout(4)
#define RF_LED PAout(12)
#define Relay_LED PAout(11)
#define RelayOUT PBout(5)



void I0Init(void);

#endif
