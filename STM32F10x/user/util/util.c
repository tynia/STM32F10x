#include "util.h"
#include <vadefs.h>

void wait(u32 clock_cycle)
{
    u32 i = 0;
    if (clock_cycle == 0) {
        clock_cycle = LED_TWINKLE_DELAY;
    }

    for (; i < clock_cycle; ++i);
}

u8  str_cmp(u8* src, u8* dst, u32 len)
{
    ASSERT(NULL != src, "invalid source string");
    ASSERT(NULL != dst, "invalid destination string");

    u32 i = 0;
    while (i < len)
    {
        if (*(src + i) == 0 || *(dst + i) == 0 || *(src + i) != *(dst + i))
        {
            return 1; // not equal
        }
        ++i;
    }

    return 0;
}

u32 str_len(u8* str)
{
    ASSERT(NULL != str, "invalid string");

    u32 i = 0;
    while (*str)
    {
        ++i;
    }

    return i;
}

u8 mod(u32 i)
{
    u8 m = i % 10;
    return '0' + m;
}

void uitoa(u32 i, u8* a)
{
    // the buffer size of a is no than 12
    ASSERT(NULL != a, "NULL buffer for itoa");
    u8 power = 1;
    u32 r = i;
    while (r > 10)
    {
        power *= 10;
        r = r / 10;
    }

    while (power > 0)
    {
        *a++ = '0' + i / power;
        i %= power;
        power /= 10;
    }
    *a = '\0';
}