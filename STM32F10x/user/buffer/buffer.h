#ifndef _STM32_RING_BUFFER_H_
#define _STM32_RING_BUFFER_H_

#include "stm32f10x.h"

#define MAX_CACHE_SIZE 256

typedef struct
{
#ifdef _DEBUG
    u8* id;
#endif
    u8* ptr;
    u8* head;
    u8* tail;
    u32 capacity;
} ring_cache;

#ifdef _DEBUG
u8 ring_cache_init(u8* id, ring_cache* cache, u8* buffer, u32 size);
#else
u8 ring_cache_init(ring_cache* cache, u8* buffer, u32 size);
#endif
s8 zero_cache(ring_cache* cache);
s8 cache_is_empty(ring_cache* cache);
s8 cache_is_full(ring_cache* cache);
s32 write_cache(ring_cache* cache, u8* data, u16 len);
s8 write_cache_char(ring_cache* cache, u8* c);
u8 write_cache_char_not_safe(ring_cache* cache, u8* c);
s8 read_cache_char(ring_cache* cache, u8* c);
u8 read_cache_char_not_safe(ring_cache* cache, u8* c);
void mark_read(ring_cache* cache);

s8 cache_find_string(ring_cache* cache, u8* dst);

#endif
