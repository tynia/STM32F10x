#ifndef _STM32_DEBUGGER_H_
#define _STM32_DEBUGGER_H_

#include "stm32f10x.h"
#include "usart/usart.h"

void InitDebugger(tagEUSART tag, tagEUSART target);

#endif
