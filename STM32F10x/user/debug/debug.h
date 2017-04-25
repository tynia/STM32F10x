#ifndef _STM32_DEBUG_H_
#define _STM32_DEBUG_H_

#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>

typedef void(*DEBUG_CALLBACK)(u8* data, u32 len);
DEBUG_CALLBACK set_debug_handler(DEBUG_CALLBACK callback);

void panic(void);
void console(const char* fmt, ...);

#define ASSERT(ok, fmt, ...)            \
do                                      \
{                                       \
    if (!(ok))                          \
    {                                   \
        console(fmt, ##__VA_ARGS__);    \
        panic();                        \
    }                                   \
} while (0)

#endif
