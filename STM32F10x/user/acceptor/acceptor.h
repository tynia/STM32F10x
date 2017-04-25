#ifndef _STM32_EXCHANGER_H_
#define _STM32_EXCHANGER_H_

#include "stm32f10x.h"

void acceptor_init(u8 idx, u16* irq, u8 len, u8 acceptor_target);

#endif
