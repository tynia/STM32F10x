#ifndef _STM32_RING_BUFFER_H_
#define _STM32_RING_BUFFER_H_

#include "stm32f10x.h"

#define MAX_CACHE_SIZE 2048

typedef struct
{
#ifdef _DEBUG
    u8* id;
#endif
    u8* ptr;
    u8* head;
    u8* tail;
    u32    capacity;
} ringcache;

#ifdef _DEBUG
u8 InitRingCache(u8* id, ringcache* cache, u8* buffer, u32 size);
#else
u8 InitRingCache(ringcache* cache, u8* buffer, u32 size);
#endif
u8 ZeroCache(ringcache* cache);
u8 IsEmpty(ringcache* cache);
u8 IsFull(ringcache* cache);
u32 WriteCache(ringcache* cache, u8* data, u32 len);
u8 WriteCacheChar(ringcache* cache, u8* c);
u8 WriteCacheCharNotSafe(ringcache* cache, u8* c);
u8 ReadCacheChar(ringcache* cache, u8* c);
u8 ReadCacheCharNotSafe(ringcache* cache, u8* c);

u32 FindString(ringcache* cache, u8* dst);

#endif
