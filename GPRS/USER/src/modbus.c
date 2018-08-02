#include "stm32f10x.h"
#include "modbus.h"
#include "sysrun.h"
#include "uart.h"
#include "io.h"
#include "User.h"
#include <string.h>
#include <stdlib.h>
#include "stmflash.h"
#include "stdio.h"


extern  mqttparameter MqttParameter;
extern unsigned char mycpuid[32];

unsigned char ucUart1Buff[500];		           // ����1���ݻ�����
unsigned char ucUart1SendBuff[500];		      // ����1���ݻ�����
unsigned int  ucUart1ReceLen;					     // ����1���ճ���
unsigned int ucUart1SendLen;			        // ����1���ͳ���
unsigned int ucUart1SendPosi;			       // ����1����λ��
unsigned char ucUart1ReceTimeOut;		    // ����1��ʱ

 unsigned char ucUart2Buff[800];		        // ����2���ݻ�����
 unsigned char ucUart2SendBuff[500];		   // ����2���ݻ�����
 unsigned char ucUart2replyBuff[100];
 unsigned char ucUart2DataBuff[500];

 unsigned  int ucUart2ReceLen;					    // ����2���ճ���
 unsigned char ucUart2SendLen;			       // ����2���ͳ���
 unsigned char ucUart2SendPosi;			      // ����2����λ��
 unsigned char ucUart2ReceTimeOut;		   // ����2��ʱ

volatile unsigned char ucUart3Buff[300];		     //  ����3���ݻ�����
volatile unsigned char ucUart3SendBuff[250];    //����3�������ݻ�����
volatile unsigned int  ucUart3ReceLen;				 //����3���ճ���
volatile unsigned char ucUart3SendLen;			  // ����3���ͳ���
volatile unsigned char ucUart3SendPosi;			 // ����3����λ��
volatile unsigned char ucUart3ReceTimeOut;  // ����3��ʱ
unsigned char ReadFlagUart3=0;
unsigned char ReadFlagUart1=0;

extern volatile uint64_t Systick_1ms;


void StartUart1SendData()
{
	int i;
	for(i=0;i<1000;i++)
	__nop(); 
	ucUart1SendPosi = 1;					
	ucUart1ReceTimeOut = 0;
    USART1->DR=ucUart1SendBuff[0];
}
void Uart1SendString(const char *p)
{
	unsigned char i;
	
	ucUart1SendLen = strlen(p);
	for(i=0;i<ucUart1SendLen;i++)
	{
		ucUart1SendBuff[i] = *p++;
	}		
	StartUart1SendData();
}
void Uart1SendData(const char *p,int length)
{
	int i;
	ucUart1SendLen = length;
	for(i=0;i<ucUart1SendLen;i++)
	{
		ucUart1SendBuff[i] = *p++;
	}	
	StartUart1SendData();		
}

//����1���ݴ���ID��ӡ
void Uart1DataProcess()
{	
	static unsigned char ReadID = 0;

    u8 temp[16]={0xfa,0x02,0x0c};   
    u32 temp0,temp1,temp2;

	if((uc10ms)&&(ucUart1ReceTimeOut > 0))              
	{
		if(--ucUart1ReceTimeOut == 0)
		{	
 			ucUart1ReceLen = 0;
		}
	}
	
 	if((ucUart1ReceTimeOut<=1)&&ReadFlagUart1)
 	{
	    if((ucUart1Buff[0] == 'A')&&(ucUart1Buff[1] == 'T')&&(ucUart1Buff[2] == '+')&&(ucUart1Buff[3] == 'M')&&(ucUart1Buff[4] == 'A')&&(ucUart1Buff[5] == 'C'))
        {
            memset((char*)ucUart1Buff,0,sizeof(ucUart1Buff));
            ReadID = 1;
        }		
    }	


	if(ReadID ==1) 
	{			
        ReadID=0;ReadFlagUart1=0;

        temp0=*(__IO u32*)(0x1FFFF7E8);    //��ƷΨһ��ݱ�ʶ�Ĵ�����96λ��
        temp1=*(__IO u32*)(0x1FFFF7EC);
        temp2=*(__IO u32*)(0x1FFFF7F0);
        temp[3] = (u8)(temp0 & 0x000000FF);
        temp[4] = (u8)((temp0 & 0x0000FF00)>>8);
        temp[5] = (u8)((temp0 & 0x00FF0000)>>16);
        temp[6] = (u8)((temp0 & 0xFF000000)>>24);
        temp[7] = (u8)(temp1 & 0x000000FF);
        temp[8] = (u8)((temp1 & 0x0000FF00)>>8);
        temp[9] = (u8)((temp1 & 0x00FF0000)>>16);
        temp[10] = (u8)((temp1 & 0xFF000000)>>24);
        temp[11] = (u8)(temp2 & 0x000000FF);
        temp[12] = (u8)((temp2 & 0x0000FF00)>>8);
        temp[13] = (u8)((temp2 & 0x00FF0000)>>16);
        temp[14] = (u8)((temp2 & 0xFF000000)>>24);
        
        temp[15] = temp[3]+temp[4]+temp[5]+temp[6]+temp[7]+temp[8]+temp[9]+temp[10]+temp[11]+temp[12]+temp[13]+temp[14];


		Uart1SendString((char*)temp);
	}
	
}


mqttparameter MqttParameter;





void StartUart2SendData()
{
	int i;
	for(i=0;i<1000;i++)
	__nop(); 
	ucUart2SendPosi = 1;					
	ucUart2ReceTimeOut = 0;
  USART2->DR=ucUart2SendBuff[0];
}


 




void Uart2SendString(const char *p)
{

	unsigned char i;
	
	ucUart2SendLen = strlen(p);
	for(i=0;i<ucUart2SendLen;i++)
	{
		ucUart2SendBuff[i] = *p++;
	}		
	StartUart2SendData();
}



void StartUart3SendData()
{
	int i;
	for(i=0;i<1000;i++)
	__nop(); 
	ucUart3SendPosi = 1;					
	ucUart3ReceTimeOut = 0;
  USART3->DR=ucUart3SendBuff[0];
}


 
void Uart3SendData(const char *p,int length)
{
	unsigned char i;
	
	ucUart3SendLen = length;
	for(i=0;i<ucUart3SendLen;i++)
	{
		ucUart3SendBuff[i] = *p++;
	}	
	StartUart3SendData();		
}



void Uart3SendString(const char *p)
{
	unsigned char i;
	
	ucUart3SendLen = strlen(p);
	for(i=0;i<ucUart3SendLen;i++)
	{
		ucUart3SendBuff[i] = *p++;
	}		
	StartUart3SendData();
}




