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

//ϵͳ��ʼ��
void SystemUserInit(void)
{
	unsigned int i;
	const uint32_t SystemFrequency = 72000000;
	while(i<5000)i++;
	PrvSetupHardware();							//ʱ�ӳ�ʼ��
	if(SysTick_Config(SystemFrequency / 1000)) 	//ϵͳ��ʱ����ʼ��
	{
		while (1);
	}											
	I0Init(); 									//IO��ʼ��
  SysInitRun();
	UART1Init();								//����1��ʼ��	
	UART2Init();								//����2��ʼ��
	UART3Init();

}

//ϵͳ������
void SystemAction(void)
{
	Time_check();	       		//ʱ�䴦����
	IWDG_ReloadCounter();  	    //ϵͳ�ļ�
	Uart1DataProcess();         //����1���ݴ���
}


//ϵͳʱ������
void PrvSetupHardware(void)
{

	RCC_DeInit();  	/* ��ʼ��RCC */
	RCC_HSEConfig( RCC_HSE_ON );	 /* ʹ���ⲿ����ʱ�� */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET );   	/* �ȴ��ⲿʱ���ȶ� */
  FLASH_SetLatency(FLASH_Latency_2);			/* ����FLASH�ٶ� */
  RCC_HCLKConfig( RCC_SYSCLK_Div1 );    /* HCLK = SYSCLK */
  RCC_PCLK2Config( RCC_HCLK_Div1 );  /* PCLK2 = HCLK */
	RCC_PCLK1Config( RCC_HCLK_Div2 );   /* PCLK1 = HCLK/2 */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_9 );    	/* PLLCLK = 16MHz/2 * 9 = 72 MHz. */
	RCC_PLLCmd( ENABLE );   /* ʹ��PLL */
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

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	   /* д��0x5555,�����������Ĵ���д�빦�� */
	IWDG_SetPrescaler(IWDG_Prescaler_256);     	/* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(320);      	/* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
	IWDG_ReloadCounter();     	/* ι��*/
	IWDG_Enable();     	/* ʹ�ܹ���*/
}



void SysInitRun(void)
{
	IWDG_Configuration();						//���ÿ��Ź�
	IWDG_Enable();								//ʹ�ܿ��Ź�
}


/*************************************************************************************/
//��������	LedFlash(void)
//����˵��:  ͨ�Ź���,ͬ����˸������Ϊ500MS  
//����:									 
//����:
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

//ʱ�䴦��
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
	if(Systick_1ms>=(Systick_10ms+10)) //10�����־λ
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
	if(Systick_1ms>=(Systick_100ms+100)) //100�����־λ
	{
		Systick_100ms = Systick_1ms;
		uc100ms = 1;
	}

	if(Systick_1ms>=(Systick_1s+1000)) //1���־λ
	{
		Systick_1s = Systick_1ms;
		uc1s = 1;
	}

	if(Systick_1ms>=(Systick_1m+60*1000)) //1���ӱ�־λ                         
	{
		Systick_1m = Systick_1ms;
		uc1m = 1;
	}

	if(Systick_1ms>=(Systick_1h+60*60*1000)) //1Сʱ��־λ
	{
		Systick_1h = Systick_1ms;
		uc1h = 1;
	}

	LedFlash(); 
}


