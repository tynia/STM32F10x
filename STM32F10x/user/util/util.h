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

#ifdef _DEBUG
void panic();
#endif

void debug(u8* fmt, ...);
void trace(u8* file, u32 line, u8* func, u8* fmt, ...);

#ifdef _DEBUG
#define ASSERT(ok, fmt, ...)                                        \
do                                                                  \
{                                                                   \
    if (!(ok))                                                      \
    {                                                               \
        trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);\
        panic();                                                    \
    }                                                               \
} while (0)
#else
#define ASSERT(ok, fmt, ...)                                        \
do                                                                  \
{                                                                   \
} while (0)
#endif // _DEBUG

#endif
