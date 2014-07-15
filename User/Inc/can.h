#ifndef __CAN_H_
#define __CAN_H_

#include "stm32f10x.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
static void CAN_GPIO_Config(void);
static void CAN_Mode_Config(void);
static void CAN_Filter_Config(void);
TestStatus CAN_Polling(void);
void USER_CAN_Init(void);
void USER_CAN_Test(void);

#endif

