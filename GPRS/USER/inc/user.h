#ifndef __USER_H
#define __USER_H
//类型定义
typedef   signed          char int8_t;	  //有符号8位
typedef   signed short     int int16_t;	  //有符号16位
typedef   signed           int int32_t;	  //有符号32位
typedef   signed       __int64 int64_t;	  //有符号64位
typedef unsigned          char uint8_t;	  //无符号8位
typedef unsigned short     int uint16_t;  //无符号16位
typedef unsigned           int uint32_t;  //无符号32位
typedef unsigned       __int64 uint64_t;  //无符号64位

#include "io.h"
#include "modbus.h"
#include "sysrun.h"
#include "uart.h"
#include "stm32f10x.h"


extern uint8_t ReadFlag,WriteFlag;
extern unsigned char GPRS;
void MQTT_Init(void);
int json_parsing(char* root);
void Hex_char(u8 *p,int length);
void Char_Hex(u8*p,int length);
u8 Find(char *a);
unsigned int Second_AT_Command(unsigned char *b,char *a,int count);
void GPRS_Config(unsigned char mode);
void Even_Processing(void);
void User(void)	;
#endif
