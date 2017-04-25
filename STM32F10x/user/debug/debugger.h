#ifndef _STM32_DEBUGGER_H_
#define _STM32_DEBUGGER_H_

#include "stm32f10x.h"

void debugger_init(u8 idx, u8 debug_target);

void debug_in();
#endif
