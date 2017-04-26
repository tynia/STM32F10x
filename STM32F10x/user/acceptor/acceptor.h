#ifndef _STM32_EXCHANGER_H_
#define _STM32_EXCHANGER_H_

#include "stm32f10x.h"
#include "usart/usart.h"

void InitAcceptor(tagEUSART tag, u16* irq, u8 len, tagEUSART target);

#endif
