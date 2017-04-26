#ifndef _STM32_UTIL_H_
#define _STM32_UTIL_H_

#include "stm32f10x.h"

void wait(u32 ms);
u8   equel(u8* src, u8* dest, u32 len);
u32  digitLength(u8* str);
void uitoa(u32 i, u8* a);
void zero(u8* buffer, u32 len);
s8   findString(u8* data, u32 len, u8* mark);

#endif
