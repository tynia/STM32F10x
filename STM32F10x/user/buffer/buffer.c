#include "buffer.h"
#include "debug/debug.h"

#define MAX_CACHE_SLOT 8
static u8 RING_CACHE_STATE = 0;
static tagRingCache* CachePool[MAX_CACHE_SLOT] = { 0 };

void ZeroCache(tagRingCache* cache)
{
    u32 i = 0;
    u8* ptr = cache->ptr;
    if (NULL == cache)
    {
        return;
    }

    while (i < cache->capacity)
    {
        *ptr = '\0';
        ++i;
    }
}

tagRingCache* InitRingCache(u8* buffer, u32 size)
{
    u8 i = 0;
    tagRingCache* cache = NULL;
    ASSERT(NULL != buffer, "invalid buffer assigned to cache");

    for (; i < MAX_CACHE_SLOT; ++i)
    {
        if (!(RING_CACHE_STATE & (1 << i)))
        {
            cache = CachePool[i];
            cache->ptr = buffer;
            cache->capacity = size;
            cache->head = cache->ptr;
            cache->tail = cache->ptr;
            ZeroCache(cache);

            return cache;
        }
    }

    return cache;
}

s32 Write(tagRingCache* cache, u8* data, u32 len)
{
    u32 wlen = 0;
    u32 rest = 0;
    u8* ptr = NULL;
    if (NULL == cache)
    {
        return -1;
    }

    if (NULL == data)
    {
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        return 0;
    }

    if (cache->head >= cache->tail)
    {
        rest = cache->capacity - (cache->tail - cache->head);
    }
    else
    {
        rest = cache->tail - cache->head;
    }

    if (len > rest)
    {
        len = rest;
    }

    ptr = data;
    while (cache->head + 1 != cache->tail)
    {
        *(cache->head) = *ptr;
        ++cache->head;
        ++ptr;
        ++wlen;

        if (cache->head > cache->ptr + cache->capacity)
        {
            cache->head = cache->ptr;
        }

        if (wlen >= len)
        {
            break;
        }
    }

    return (s32)wlen;
}

s32 Read(tagRingCache* cache, u8* buffer, u32 size, u32 toRead)
{
    u32 rlen = 0;
    u8* ptr = buffer;
    if (NULL == cache)
    {
        return -1;
    }

    if (NULL == buffer)
    {
        return -1;
    }

    if (size < toRead)
    {
        toRead = size;
    }

    while (cache->tail != cache->head)
    {
        *ptr = *(cache->tail);
        ++cache->tail;
        ++ptr;
        ++rlen;

        if (cache->tail > cache->ptr + cache->capacity)
        {
            cache->tail = cache->ptr;
        }

        if (rlen >= toRead)
        {
            break;
        }
    }

    return rlen;
}
s8  WriteChar(tagRingCache* cache, u8 c)
{
    if (NULL == cache)
    {
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        return 0;
    }

    *(cache->head) = c;
    ++cache->head;

    if (cache->head > cache->ptr + cache->capacity)
    {
        cache->head = cache->ptr;
    }

    return 1;
}

s8  ReadChar(tagRingCache* cache, u8* c)
{
    if (NULL == cache)
    {
        return -1;
    }

    if (NULL == c)
    {
        return -1;
    }

    if (cache->head == cache->tail)
    {
        return 0;
    }

    *c = *(cache->tail);
    ++cache->tail;

    if (cache->tail > cache->ptr + cache->capacity)
    {
        cache->tail = cache->ptr;
    }

    return 1;
}

s32 ReadPacket(tagRingCache* cache, u8* buffer, u32 size)
{
    u8 state;
    u32 rlen;
    if (NULL == cache)
    {
        return -1;
    }

    if (NULL == buffer)
    {
        return -1;
    }

    if (cache->head == cache->tail)
    {
        return 0;
    }

    while (cache->tail != cache->head)
    {
        u8 c = *(cache->tail);
        *buffer = c;
        ++cache->tail;
        ++buffer;
        ++rlen;

        if (cache->tail > cache->ptr + cache->capacity)
        {
            cache->tail = cache->ptr;
        }

        if (c == 0x0D)
        {
            state |= 0x40;
        }
        if (c == 0x0A)
        {
            if (state & 0x40)
            {
                state = 0;
                return rlen;
            }
        }
    }

    return rlen;
}

void Markr(tagRingCache* cache)
{
    if (NULL == cache)
    {
        return;
    }

    cache->tail = cache->head;
}
