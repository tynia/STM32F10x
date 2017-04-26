#ifndef _STM32_APB_CLOCK_CTRL_H_
#define _STM32_APB_CLOCK_CTRL_H_

#include "stm32f10x.h"

#define DISABLE 0
#define ENABLE !DISABLE

void InitAPBCLKCTRL(u32 APB, u8 state);

#endif
