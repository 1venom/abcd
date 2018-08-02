#include "sysrun.h"
#include "io.h"
#include "uart.h"
#include "modbus.h"
#include "rc522.h"

volatile uint64_t Systick_1ms;
volatile uint64_t Systick_10ms;
volatile uint64_t Systick_100ms;
volatile uint64_t Systick_1s;
volatile uint64_t Systick_1m;
volatile uint64_t Systick_1h;

unsigned char uc10ms;
unsigned char uc100ms;
unsigned char uc1s;
unsigned char uc1m;
unsigned char uc1h;

//系统初始化
void SystemUserInit(void)
{
	unsigned int i;
	const uint32_t SystemFrequency = 72000000;
	while(i<5000)i++;
	PrvSetupHardware();							//时钟初始化
	if(SysTick_Config(SystemFrequency / 1000)) 	//系统定时器初始化
	{
		while (1);
	}											
	I0Init(); 									//IO初始化
  SysInitRun();
	UART1Init();								//串口1初始化	
	UART2Init();								//串口2初始化
	UART3Init();

}

//系统主运行
void SystemAction(void)
{
	Time_check();	       		//时间处理部分
	IWDG_ReloadCounter();  	    //系统文件
	Uart1DataProcess();         //串口1数据处理
}


//系统时钟配置
void PrvSetupHardware(void)
{

	RCC_DeInit();  	/* 初始化RCC */
	RCC_HSEConfig( RCC_HSE_ON );	 /* 使能外部高速时钟 */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET );   	/* 等待外部时钟稳定 */
  FLASH_SetLatency(FLASH_Latency_2);			/* 设置FLASH速度 */
  RCC_HCLKConfig( RCC_SYSCLK_Div1 );    /* HCLK = SYSCLK */
  RCC_PCLK2Config( RCC_HCLK_Div1 );  /* PCLK2 = HCLK */
	RCC_PCLK1Config( RCC_HCLK_Div2 );   /* PCLK1 = HCLK/2 */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_9 );    	/* PLLCLK = 16MHz/2 * 9 = 72 MHz. */
	RCC_PLLCmd( ENABLE );   /* 使能PLL */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );    /* Select PLL as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 );     /* Wait till PLL is used as system clock source. */

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
// 	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
// 						  | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
// 						  | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE );

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE);						  
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );    /* SPI2 Periph clock enable */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}


void IWDG_Configuration(void)
{

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	   /* 写入0x5555,用于允许狗狗寄存器写入功能 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);     	/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(320);      	/* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
	IWDG_ReloadCounter();     	/* 喂狗*/
	IWDG_Enable();     	/* 使能狗狗*/
}



void SysInitRun(void)
{
	IWDG_Configuration();						//配置开门狗
	IWDG_Enable();								//使能开门狗
}


/*************************************************************************************/
//函数名：	LedFlash(void)
//作用说明:  通信故障,同步闪烁，周期为500MS  
//输入:									 
//返回:
/*************************************************************************************/
void LedFlash()
{
	static unsigned char Led_Count50=0,Led_Count10=0;
	Led_Count50 += uc10ms;
	Led_Count10 += uc10ms;
	if(Led_Count50 >= 50) 
		Led_Count50 = 0;
	if(Led_Count10 >= 10)
		Led_Count10 = 0;
	
}

//时间处理
void Time_check(void)
{
	static unsigned char InitTime_Check=0;
	if(InitTime_Check==0)
	{
		InitTime_Check=1;
		Systick_1ms = 0;
		Systick_10ms  = Systick_1ms;
		Systick_100ms = Systick_1ms;
		Systick_1s 	  = Systick_1ms;
		Systick_1m    = Systick_1ms;
		Systick_1h    = Systick_1ms;
		return;
	}
	uc10ms = 0;
	uc100ms = 0;
	uc1s = 0;
	uc1m = 0;
	uc1h = 0;
	if(Systick_1ms>=(Systick_10ms+10)) //10毫秒标志位
	{
		Systick_10ms = Systick_1ms;
		uc10ms = 1;
		if(ucUart2ReceTimeOut > 0)
		{
			if(--ucUart2ReceTimeOut == 0)
			{	
				ucUart2ReceLen = 0;
			}
		}
		if(ucUart3ReceTimeOut > 0)
		{
			if(--ucUart3ReceTimeOut == 0)
			{	
				ucUart3ReceLen = 0;
			}
		}
        
		if(Relay_Delay_Cnt > 0)
		{
			if(--Relay_Delay_Cnt == 0)
			{	
				ucUart3ReceLen = 0;
			}
		}
	}
	if(Systick_1ms>=(Systick_100ms+100)) //100毫秒标志位
	{
		Systick_100ms = Systick_1ms;
		uc100ms = 1;
	}

	if(Systick_1ms>=(Systick_1s+1000)) //1秒标志位
	{
		Systick_1s = Systick_1ms;
		uc1s = 1;
	}

	if(Systick_1ms>=(Systick_1m+60*1000)) //1分钟标志位                         
	{
		Systick_1m = Systick_1ms;
		uc1m = 1;
	}

	if(Systick_1ms>=(Systick_1h+60*60*1000)) //1小时标志位
	{
		Systick_1h = Systick_1ms;
		uc1h = 1;
	}

	LedFlash(); 
}


