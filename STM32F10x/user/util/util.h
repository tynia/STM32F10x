#ifndef _STM32_UTIL_H_
#define _STM32_UTIL_H_

#include "stm32f10x.h"

enum
{
    LED_TWINKLE_DELAY = 1000000, /*100 x 10000 times system time cycle*/
};

void wait(u32 clock_cycle);
u8  str_cmp(u8* src, u8* dest, u32 len);
u32 str_len(u8* str);
void uitoa(u32 i, u8* a);


#endif
