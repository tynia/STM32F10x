#include "util.h"
#include <stdio.h>

void wait(u32 clock_cycle) // ms
{
    u32 i = 0;
    if (clock_cycle == 0) {
        clock_cycle = LED_TWINKLE_DELAY * 1000;
    }
    clock_cycle = clock_cycle * 1000;

    for (; i < clock_cycle; ++i);
}

u8  str_cmp(u8* src, u8* dst, u32 len)
{
    u32 i = 0;
    if (NULL == src || NULL == dst)
    {
        return -1;
    }

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
    u32 i = 0;
    if (NULL == str)
    {
        return 0;
    }
    
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
    u8 power = 1;
    u32 r = i;
    // the buffer size of a is no than 12
    if (NULL == a)
    {
        return;
    }
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

void zero(u8* buffer, u32 len)
{
    if (NULL == buffer)
    {
        return;
    }

    u32 i = 0;
    for (; i < len; ++i)
    {
        *(buffer + i) = 0;
    }
}
