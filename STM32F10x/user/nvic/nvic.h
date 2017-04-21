#ifndef _STM32_NVIC_H_
#define _STM32_NVIC_H_

#include "stm32f10x.h"

void nvic_init(u32 irq, u8 priority, u8 sub_priority);

#endif
