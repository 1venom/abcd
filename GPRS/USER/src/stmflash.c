#include "stmflash.h"
#include "stm32f10x.h"

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) {
    uint16_t i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;    
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
      FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
      i = i+2;
      iAddress = iAddress + 2;
      buf = buf + 2;
    }
    
    return iNumByteToWrite;
}

/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to write into flash
  * @retval if success return the number to write, -1 if error
  *  
  */
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite)
{
        /* Unlock the Flash Bank1 Program Erase controller */
    uint32_t secpos;   //扇区地址
    uint32_t iNumByteToWrite = iNbrToWrite;
		uint16_t secoff;    //偏移地址
		uint16_t secremain;  //剩余地址
		uint16_t i = 0; 
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;		
    uint8_t tmp[FLASH_PAGE_SIZE];
        
    FLASH_Unlock();
		secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;  
		secoff=iAddress & (FLASH_PAGE_SIZE -1);      
		secremain=FLASH_PAGE_SIZE-secoff;                  
    if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;
        while(1)
		{
            Flash_Read(secpos, tmp, FLASH_PAGE_SIZE);  
            for(i=0;i<secremain;i++)
			{      
				if(tmp[secoff+i]!=0XFF)break;       
			}
            if(i<secremain) 
			{  
                FLASHStatus = FLASH_ErasePage(secpos); 
                if(FLASHStatus != FLASH_COMPLETE)
                  return -1;
                for(i=0;i<secremain;i++) 
				{  
                        tmp[i+secoff]=buf[i];   
                }
                Flash_Write_Without_check(secpos ,tmp ,FLASH_PAGE_SIZE);
            } 
			else
			{
                Flash_Write_Without_check(iAddress,buf,secremain);
            }
            
            if(iNumByteToWrite==secremain) 
                break;
            else {
                secpos += FLASH_PAGE_SIZE;
                secoff = 0;
                buf += secremain;  
                iAddress += secremain;  
                iNumByteToWrite -= secremain;  
                if(iNumByteToWrite>FLASH_PAGE_SIZE) secremain=FLASH_PAGE_SIZE;
                else secremain = iNumByteToWrite;  
            }
            
         }
        
        FLASH_Lock();
        return iNbrToWrite; 
}


/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to read from flash
  * @retval if success return the number to write, without error
  *  
  */
int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) {
        int i = 0;
        while(i < iNbrToRead ) {
           *(buf + i) = *(__IO uint8_t*) iAddress++;
           i++;
        }
        return i;
}

// void Jump2App(int nAddr)
// {
// 	asm ("LDR SP, [r0, #0x0]");
// 	asm ("LDR r0, [r0, #0x4]");
// 	asm ("BLX r0");
// }

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	iapfun jump2app; 
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}
