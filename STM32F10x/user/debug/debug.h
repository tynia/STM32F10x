#ifndef _STM32_DEBUG_H_
#define _STM32_DEBUG_H_

#include "stm32f10x.h"

void Debug(u8 idx, u16* irq);

#ifdef _DEBUG
void panic();
#endif

void debug(u8* fmt, ...);
void trace(u8* file, u32 line, u8* func, u8* fmt, ...);

#define traceout(fmt, ...)                                          \
do                                                                  \
{                                                                   \
    trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);    \
} while (0)

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
