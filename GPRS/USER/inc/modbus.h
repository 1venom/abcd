#ifndef __MODBUS_H
#define __MODBUS_H
#include "mqtt.h"
extern volatile unsigned char ucUart1ReceError;
extern volatile unsigned char ucUart2ReceError;
extern volatile unsigned char ucUart3ReceError;




extern unsigned char ucUart1Buff[500];		            // 串口2数据缓冲区
extern unsigned char ucUart1SendBuff[500];
extern unsigned int ucUart1ReceLen;					// 串口1接收长度
extern unsigned int ucUart1SendLen;			        // 串口1发送长度
extern unsigned int ucUart1SendPosi;			        // 串口1发送位置
extern unsigned char ucUart1ReceTimeOut;		        // 串口1超时

extern  unsigned char ucUart2Buff[800];		            // 串口2数据缓冲区
extern  unsigned char ucUart2SendBuff[500];

extern  unsigned char ucUart2replyBuff[100];
extern  unsigned char ucUart2DataBuff[500];

extern  unsigned int ucUart2ReceLen;					// 串口2接收长度
extern  unsigned char ucUart2SendLen;			        // 串口2发送长度
extern  unsigned char ucUart2SendPosi;			        // 串口2发送位置
extern  unsigned char ucUart2ReceTimeOut;		        // 串口2超时


extern volatile unsigned char ucUart3Buff[300];		            // 串口3数据缓冲区
extern volatile unsigned char ucUart3SendBuff[250];
extern volatile unsigned int  ucUart3ReceLen;					// 串口3接收长度
extern volatile unsigned char ucUart3SendLen;			        // 串口3发送长度
extern volatile unsigned char ucUart3SendPosi;			        // 串口3发送位置
extern volatile unsigned char ucUart3ReceTimeOut;		        // 串口3超时

extern mqttparameter MqttParameter;
extern unsigned char ReadFlagUart3;

extern unsigned char ReadFlagUart1;

unsigned short CreatCRC(volatile unsigned char *buff, volatile unsigned short len);
void StartUart2SendData(void);
void Uart1SendData(const char *p,int length);
void Uart1SendString(const char *p);
//串口1通信主函数
void CheckUart1Data(void);	//DB9通信口，
//串口2通信主函数
void CheckUart2Data(void);	//4P	远程监控
//串口3通信主函数
void CheckUart3Data(void);	//DB9通信口
//模块通信主函数
void ModuleCommunicaiton(void);
void Uart1DataProcess(void);                 //串口1数据处理
void ComErrCheck(void);
//void Uart1SendString(void);
void Uart2SendString(const char *p);
//void Uart2SendPString(const char *p,int16_t data);
void Uart2SendData(void);
void Uart3SendData(const char *p,int length);
#endif
