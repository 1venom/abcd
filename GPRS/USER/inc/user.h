#ifndef __USER_H
#define __USER_H
//���Ͷ���
typedef   signed          char int8_t;	  //�з���8λ
typedef   signed short     int int16_t;	  //�з���16λ
typedef   signed           int int32_t;	  //�з���32λ
typedef   signed       __int64 int64_t;	  //�з���64λ
typedef unsigned          char uint8_t;	  //�޷���8λ
typedef unsigned short     int uint16_t;  //�޷���16λ
typedef unsigned           int uint32_t;  //�޷���32λ
typedef unsigned       __int64 uint64_t;  //�޷���64λ

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
