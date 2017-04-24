#ifndef _STM32_EXCHANGER_H_
#define _STM32_EXCHANGER_H_

#include "stm32f10x.h"

typedef void(*EXCHANGE_CALLBACK)(u8* data, u32 len);

void exchanger_init(u8 idx, u16* irq, u8 len, EXCHANGE_CALLBACK func);

EXCHANGE_CALLBACK set_data_exchange_handler(EXCHANGE_CALLBACK func);

#endif
