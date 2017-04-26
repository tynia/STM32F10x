#include "util.h"
#include <stdio.h>
#include <string.h>

enum {
    LED_TWINKLE_DELAY = 1000, /* ms: 1000 x 1000 times system time cycle */
};

void wait(u32 ms) // ms
{
    u32 i = 0;
    if (ms == 0) {
        ms = LED_TWINKLE_DELAY * 1000;
    }
    else
    {
        ms = ms * 1000;
    }

    for (; i < ms; ++i);
}

u8  equel(u8* src, u8* dst, u32 len)
{
    u32 i = 0;
    if (NULL == src || NULL == dst)
    {
        return 0;
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

u32 digitLength(u8* str)
{
    u32 i = 0;
    u8* ptr = str;
    if (NULL == str)
    {
        return 0;
    }
    
    while (*ptr != '\0')
    {
        ++ptr;
        ++i;
    }

    return i;
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
    u32 i = 0;
    if (NULL == buffer)
    {
        return;
    }
    
    for (; i < len; ++i)
    {
        *(buffer + i) = 0;
    }
}

s8   findString(u8* data, u32 len, u8* mark)
{
    u8* ptr = NULL;
    if (NULL == data)
    {
        return -1;
    }
    ptr = (u8*)strstr((const char*)data, (const char*)mark);
    if (NULL != ptr)
    {
        return 1;
    }
    return 0;
}
