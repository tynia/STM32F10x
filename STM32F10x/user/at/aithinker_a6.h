#ifndef _STM32_AITHINKER_A6_H_
#define _STM32_AITHINKER_A6_H_

#include "stm32f10x.h"
#include "usart/usart.h"

void A6Init(tagEUSART tag, u16* IRQs, u8 len, tagEUSART target);
u8   WaitOK(void);
s8   Dial(u8* target, u16 port);
s8   SendData(u8 RAM, u8* data, u32 len);
s8   Close(void);

#endif
