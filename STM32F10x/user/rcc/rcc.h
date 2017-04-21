#ifndef _STM32_RCC_H_
#define _STM32_RCC_H_

#include "stm32f10x.h"

void rcc_set_clock(u32 mask, u8 enable);
#endif
