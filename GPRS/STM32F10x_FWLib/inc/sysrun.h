#ifndef __sysrun_H
#define __sysrun_H   
#include <stm32f10x.h>

extern volatile uint64_t Systick_1ms;
extern unsigned char uc10ms;
extern unsigned char uc100ms;
extern unsigned char uc1s;
extern unsigned char uc1m;
extern unsigned char uc1h;
void SystemUserInit(void);
void SystemAction(void);
void PrvSetupHardware(void);
void IWDG_Configuration(void);
void SysInitRun(void);
void Time_check(void);
#endif

