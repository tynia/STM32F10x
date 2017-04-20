#include "buffer/buffer.h"
#include "util/util.h"

#ifdef _DEBUG
u8 InitRingCache(u8* id, ringcache* cache, u8* buffer, u32 size)
#else
u8 InitRingCache(ringcache* cache, u8* buffer, u32 size)
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

u8 ZeroCache(ringcache* cache)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return -1;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    u8* ptr = cache->ptr;
    for (; ptr < cache->ptr + cache->capacity; ++ptr)
    {
        *ptr = 0;
    }

    return 0;
}

u8 IsEmpty(ringcache* cache)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return 0;
    }

    if (cache->head == cache->tail)
    {
        return 1;
    }

    return 0;
}

u8 IsFull(ringcache* cache)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return 0;
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        return 1;
    }

    return 0;
}

u32 WriteCache(ringcache* cache, u8* data, u32 len)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == data)
    {
        debug("invalid data, NULL pointer in");
        return 0;
    }

    if (cache->head + 1 == cache->tail)
    {
        debug("cache is full");
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
        debug("warning: the length to write [%d] is greater than rest buffer size[%d]", len, rest);
    }

    debug("the rest size of cache is %d", rest);
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

u8 WriteCacheChar(ringcache* cache, u8 c)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (cache->head + 1 == cache->tail)
    {
        debug("cache is full");
        return 0;
    }

    *(cache->head) = c;
    ++cache->head;

    if (cache->head >= cache->ptr + cache->capacity)
    {
        cache->head = cache->ptr;
    }

    return 1;
}

u8 WriteCacheCharNotSafe(ringcache* cache, u8 c)
{
    ASSERT(NULL != cache, "cache is uninitialized");
    ASSERT(NULL != cache->ptr, "invalid cache, cache ptr is NULL");
    ASSERT(NULL != cache->head, "invalid cache, head pointer is NULL");
    ASSERT(NULL != cache->tail, "invalid cache, tail pointer is NULL");
    ASSERT(NULL != c, "invalid char buffer, char pointer is NULL");

    *(cache->head) = c;
    ++cache->head;

    if (cache->head >= cache->ptr + cache->capacity)
    {
        cache->head = cache->ptr;
    }

    return 1;
}

u8 ReadCacheChar(ringcache* cache, u8* c)
{
    if (NULL == cache)
    {
        debug("cache is uninitialized");
        return -1; // error for none
    }

    if (NULL == cache->ptr || NULL == cache->head || NULL == cache->tail)
    {
        debug("invalid cache, one or more ring cache member is NULL");
        return -1;
    }

    if (NULL == c)
    {
        debug("char is not initialized");
        return -1;
    }

    if (cache->head == cache->tail)
    {
        debug("cache is empty");
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

u8 ReadCacheCharNotSafe(ringcache* cache, u8* c)
{
    ASSERT(NULL != cache, "cache is uninitialized");
    ASSERT(NULL != cache->ptr, "invalid cache, cache ptr is NULL");
    ASSERT(NULL != cache->head, "invalid cache, head pointer is NULL");
    ASSERT(NULL != cache->tail, "invalid cache, tail pointer is NULL");
    ASSERT(NULL != c, "invalid char buffer, char pointer is NULL");

    *c = *(cache->tail);
    ++cache->tail;

    if (cache->tail >= cache->ptr + cache->capacity)
    {
        cache->tail = cache->ptr;
    }

    return 1;
}