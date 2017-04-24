#ifndef _STM32_DEBUGGER_H_
#define _STM32_DEBUGGER_H_

#include "stm32f10x.h"

typedef void(*DATA_RECEIVED_CALLBACK)(u8* data, u32 len);

void debugger_init(u8 idx);

DATA_RECEIVED_CALLBACK set_data_received_handler(DATA_RECEIVED_CALLBACK func);

#endif
