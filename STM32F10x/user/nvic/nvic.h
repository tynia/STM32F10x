#ifndef _STM32_NVIC_H_
#define _STM32_NVIC_H_

#include "stm32f10x.h"

void InitNVICCTRL(u32 PriorityGroup, u8 IRQChn, u8 Priority, u8 SubPriority);

#endif
