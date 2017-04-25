#include "buffer/buffer.h"
#include "util/util.h"
#include "debug/debug.h"

void ring_cache_init(ring_cache* cache, u8* buffer, u32 size)
{
    cache->ptr = buffer;
    cache->head = buffer;
    cache->tail = buffer;
    cache->capacity = size;
}

s8 zero_cache(ring_cache* cache)
{
    u8* ptr = 0;
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return -1;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    ptr = cache->ptr;
    for (; ptr < cache->ptr + cache->capacity; ++ptr)
    {
        *ptr = 0;
    }

    return 0;
}

s8 cache_empty(ring_cache* cache)
{
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head == cache->tail)
    {
        return 1;
    }

    return 0;
}

s8 cache_full(ring_cache* cache)
{
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        return 1;
    }

    return 0;
}

s32 write_cache(ring_cache* cache, u8* data, u16 len)
{
    u32 rest = 0;
    u16 wlen = 0;
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == data)
    {
        console("invalid data, NULL pointer in");
        return 0;
    }

    if (cache->head + 1 == cache->tail)
    {
        console("cache is full");
        return 0;
    }
    
    if (cache->head == cache->tail)
    {
        rest = cache->capacity;
    }
    else if (cache->head < cache->tail)
    {
        rest = cache->tail - cache->head;
    }
    else
    {
        rest = cache->capacity - (cache->head - cache->tail);
    }

    if (len > rest)
    {
        console("warning: the length to write [%d] is greater than rest buffer size[%d]", len, rest);
    }

    console("the rest size of cache is %d", rest);

    while (cache->head < cache->ptr + cache->capacity && wlen < len)
    {

    }
    // head is in front of the tail
    if (cache->tail <= cache->head)
    {
        if (cache->head < cache->ptr + cache->capacity)
        {
            while (cache->head < cache->ptr + cache->capacity && wlen < len)
            {
                *(cache->head) = *data;
                ++cache->head;
                ++data;
                ++wlen;
            }
        }
        else
        {
            cache->head = cache->ptr;
        }
    }

    while (cache->head < cache->tail)
    {
        *(cache->head) = *data;
        ++cache->head;
        ++data;
        ++wlen;
    }

    return wlen;
}

s8 write_cache_char(ring_cache* cache, u8* c)
{
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        console("cache is full");
        return 0;
    }

    if (NULL == c)
    {
        console("invalid char buffer");
        return -1;
    }

    *(cache->head) = *c;
    ++cache->head;

    if (cache->head >= cache->ptr + cache->capacity)
    {
        cache->head = cache->ptr;
    }

    return 1;
}

u8 write_cache_char_not_safe(ring_cache* cache, u8* c)
{
    ASSERT(NULL != cache, "cache is uninitialized");
    ASSERT(NULL != cache->ptr, "invalid cache, cache ptr is NULL");
    ASSERT(NULL != cache->head, "invalid cache, head pointer is NULL");
    ASSERT(NULL != cache->tail, "invalid cache, tail pointer is NULL");
    ASSERT(NULL != c, "invalid char buffer, char pointer is NULL");

    if (cache->head + 1 == cache->tail)
    {
        console("cache is full");
        return 0;
    }

    *(cache->head) = *c;
    ++cache->head;

    if (cache->head >= cache->ptr + cache->capacity)
    {
        cache->head = cache->ptr;
    }

    return 1;
}

s8 read_cache_char(ring_cache* cache, u8* c)
{
    if (NULL == cache)
    {
        console("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        console("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == c)
    {
        console("char buffer is not initialized");
        return -1;
    }

    if (cache->head == cache->tail)
    {
        console("cache is empty");
        return 0;
    }

    if (NULL != cache->tail)
    {
        *c = *(cache->tail);
        ++cache->tail;
    }

    if (cache->tail >= cache->ptr + cache->capacity)
    {
        cache->tail = cache->ptr;
    }

    return 1;
}

u8 read_cache_char_not_safe(ring_cache* cache, u8* c)
{
    ASSERT(NULL != cache, "cache is uninitialized");
    ASSERT(NULL != cache->ptr, "invalid cache, cache ptr is NULL");
    ASSERT(NULL != cache->head, "invalid cache, head pointer is NULL");
    ASSERT(NULL != cache->tail, "invalid cache, tail pointer is NULL");
    ASSERT(NULL != c, "invalid char buffer, char pointer is NULL");

    if (cache->head == cache->tail)
    {
        console("cache is empty");
        return 0;
    }

    *c = *(cache->tail);
    ++cache->tail;

    if (cache->tail >= cache->ptr + cache->capacity)
    {
        cache->tail = cache->ptr;
    }

    return 1;
}

s8 cache_find_string(ring_cache* cache, u8* dst)
{
    u32 i = 0;
    u8* ptr = cache->tail;

    if (NULL == cache)
    {
        console("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == dst)
    {
        return 0;
    }

    if (cache->head == cache->tail)
    {
        console("cache is empty");
        return -1;
    }

    for (; ptr != cache->head; ++ptr)
    {
        if (ptr >= cache->ptr + cache->capacity)
        {
            ptr = cache->ptr;
        }

        if (*(dst + i) == '\0')
        {
            return 1;
        }

        if (*(dst + i) == *(ptr))
        {
            ++i;
        }
        else
        {
            i = 0;
        }
    }

    return 0;
}

void markr(ring_cache* cache)
{
    cache->tail = cache->head;
}

u32 fetch_len(ring_cache* cache)
{
    u32 i = 0;
    u8* ptr = NULL;
    if (NULL == cache)
    {
        return 0;
    }

    ptr = cache->tail;
    while (ptr != cache->head)
    {
        ++i;
        ++ptr;
        if (ptr > cache->ptr + cache->capacity)
        {
            ptr = cache->ptr;
        }
    }

    return i;
}
