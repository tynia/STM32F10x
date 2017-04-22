#ifndef _STM32_DEBUGGER_H_
#define _STM32_DEBUGGER_H_

#include "stm32f10x.h"

void debugger_init(u8 idx, u16* irq);
void set_debugger_acceptor(u8 idx);
#endif
