#ifndef _STM32_APB_CLOCK_CTRL_H_
#define _STM32_APB_CLOCK_CTRL_H_

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"

void InitAPB1CLKCTRL(u32 APB, FunctionalState state);

void InitAPB2CLKCTRL(u32 APB, FunctionalState state);

#endif
