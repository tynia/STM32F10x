#ifndef _STM32_TRANSFER_H_
#define _STM32_TRANSFER_H_

#include "stm32f10x.h"
#include "usart/usart.h"
#include "buffer/buffer.h"

typedef void(*ON_DATA_RECEIVED_CALLBACK)(u8* data, u32 len);

void Register(tagEUSART tag, tagEUSART target, tagRingCache* cache, ON_DATA_RECEIVED_CALLBACK cb);

u8 transmit(tagEUSART id);

u8 empty(void);

void transfer(void);

#endif
