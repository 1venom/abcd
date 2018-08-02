#include "stm32f10x.h"
#include "io.h"
#include "sysrun.h"
#include "User.h"
#include <string.h>



//输入输出初始化
void I0Init(void) 
{
	    
	GPIO_InitTypeDef  GPIO_InitStructure; 
	

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	PA4_H;PA5_H;PA11_H;PA12_H;
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

 
//void Power_RF_ON(int GPRS_Stat,int RF_Stat,int Relay_Stat)
//{
//	RF_LED = 0;
//	RF_Stat = 1;
//	Relay_LED = 0;
//	RelayOUT = 1;
//	Relay_Stat = 1;
//}

//void Power_RF_OFF(int GPRS_Stat,int RF_Stat,int Relay_Stat)
//{
//	RF_LED = 1;
////	RelayReturn = 1;
//	RF_Stat = 0;
//	if((Relay_Stat==1) && (GPRS_Stat==0))							//????????????
//	{
////		Relay_Delay_Cnt = 1500;
//	}
//}

//void Power_WF_ON(int online,int RF_Stat,int Relay_Stat)
//{
//	GPRS_Stat = 1;
//	Relay_LED = 0;
//	RelayOUT = 1;
//	Relay_Stat = 1;
//}

//void Power_WF_OFF(int RF_Stat,int Relay_Stat)
//{
//	if(RF_Stat==0)
//	{
//		Relay_LED = 1;
//		RelayOUT = 0;
//		Relay_Stat = 0;
//	}
//}
