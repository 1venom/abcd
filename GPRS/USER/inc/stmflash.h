#ifndef   __FLASH_H__
#define   __FLASH_H__

#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#define FLASH_PAGE_SIZE    ((uint16_t)0x400)

int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) ;
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite);
void Jump2App(int nAddr);

#endif

