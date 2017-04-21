#include "buffer/buffer.h"
#include "util/util.h"

#ifdef _DEBUG
u8 ring_cache_init(u8* id, ring_cache* cache, u8* buffer, u32 size)
#else
u8 ring_cache_init(ring_cache* cache, u8* buffer, u32 size)
#endif
{
#ifdef _DEBUG
    cache->id = id;
#endif
    cache->ptr = buffer;
    cache->head = buffer;
    cache->tail = buffer;
    cache->capacity = size;
}

u8 zero_cache(ring_cache* cache)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return -1;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    u8* ptr = cache->ptr;
    for (; ptr < cache->ptr + cache->capacity; ++ptr)
    {
        *ptr = 0;
    }

    return 0;
}

u8 cache_is_empty(ring_cache* cache)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return 0;
    }

    if (cache->head == cache->tail)
    {
        return 1;
    }

    return 0;
}

u8 cache_is_full(ring_cache* cache)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        return 1;
    }

    return 0;
}

u32 write_cache(ring_cache* cache, u8* data, u32 len)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == data)
    {
        traceout("invalid data, NULL pointer in");
        return 0;
    }

    if (cache->head + 1 == cache->tail)
    {
        traceout("cache is full");
        return 0;
    }

    u32 rest = 0;
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
        traceout("warning: the length to write [%d] is greater than rest buffer size[%d]", len, rest);
    }

    traceout("the rest size of cache is %d", rest);
    u32 wlen = 0;
    // head is in front of the tail
    if (cache->tail <= cache->head)
    {
        if (cache->head < cache->ptr + cache->capacity)
        {
            while (cache->head < cache->ptr + cache->capacity)
            {
                *(cache->head) = *data;
                ++cache->head;
                ++data;
                ++wlen;
            }
        }
        cache->head = cache->ptr;
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

u8 write_cache_char(ring_cache* cache, u8* c)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        traceout("cache is full");
        return 0;
    }

    if (NULL == c)
    {
        traceout("invalid char buffer");
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
        traceout("cache is full");
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

u8 read_cache_char(ring_cache* cache, u8* c)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        traceout("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == c)
    {
        traceout("char buffer is not initialized");
        return -1;
    }

    if (cache->head == cache->tail)
    {
        traceout("cache is empty");
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
        traceout("cache is empty");
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

u32 cache_find_string(ring_cache* cache, u8* dst)
{
    if (NULL == cache)
    {
        traceout("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == dst)
    {
        return 0;
    }

    if (cache->head == cache->tail)
    {
        traceout("cache is empty");
        return -1;
    }

    u32 i = 0;
    u32 at = 0; // digit the length of read
    u8* ptr = cache->tail;
    for (; ptr != cache->head; ++ptr)
    {
        if (ptr >= cache->ptr + cache->capacity)
        {
            ptr = cache->ptr;
        }

        if (*(dst + i) == '\0')
        {
            return at;
        }

        if (*(dst + i) == *(ptr))
        {
            ++i;
        }
        else
        {
            i = 0;
        }

        ++at;
    }

    return 0;
}

void skipr(ring_cache* cache, u32 w)
{
    if (cache->tail + w > cache->ptr + cache->capacity)
    {
        u32 sk = w - (cache->capacity - (cache->tail - cache->ptr));
        cache->tail = cache->ptr + sk;
    }
}