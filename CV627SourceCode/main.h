#ifndef _MAIN_H_
#define _MAIN_H_
#include "PinDefine.h"
#include "stm32f10x.h"
#include "status.h"


void InitDevice(void);


//------------------------------------------------------------------------------
// Soft Delay Define
//------------------------------------------------------------------------------
#define TIME_UNIT_100US			0x01
#define TIME_UNIT_1MS			0x02
#define TIME_UNIT_200MS			0x03

void Delay100uS(unsigned int iTimes);
void Timer4_Init(unsigned char cMode);
void Timer4_Wait(unsigned short sTime);
void Timer4_Stop(void);



#endif
