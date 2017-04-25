#ifndef _STM32_TRANSFER_H_
#define _STM32_TRANSFER_H_

#include "stm32f10x.h"
#include "buffer/buffer.h"

typedef void(*ON_DATA_RECEIVED_CALLBACK)(u8* data, u32 len);

void reg(u8 idx, ring_cache* cache, ON_DATA_RECEIVED_CALLBACK cb);

u8 transmit(u8 from, u8 to);

void transfer(void);

#endif
