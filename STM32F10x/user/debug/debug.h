#ifndef _STM32_DEBUG_H_
#define _STM32_DEBUG_H_

#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>

typedef void (*DEBUG_CALLBACK)(u8* data, u32 len);
void set_debug_handler(DEBUG_CALLBACK callback);
#ifdef _DEBUG
void panic();
#endif

void debug(const char* fmt, ...);
void debugc(const char* fmt, ...);
void wrap_trace(const char* file, u32 line, const char* func, const char* fmt, ...);

#define trace(fmt, ...)                                                  \
do                                                                       \
{                                                                        \
    wrap_trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);    \
} while (0)

#ifdef _DEBUG
#define ASSERT(ok, fmt, ...)                                             \
do                                                                       \
{                                                                        \
    if (!(ok))                                                           \
    {                                                                    \
        wrap_trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);\
        panic();                                                         \
    }                                                                    \
} while (0)
#else
#define ASSERT(ok, fmt, ...)                                             \
do                                                                       \
{                                                                        \
} while (0)
#endif // _DEBUG

#endif
