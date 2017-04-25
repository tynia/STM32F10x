#ifndef _STM32_RING_BUFFER_H_
#define _STM32_RING_BUFFER_H_

#include "stm32f10x.h"

#define MAX_CACHE_SIZE 256

typedef struct
{
    u8* ptr;
    u8* head;
    u8* tail;
    u32 capacity;
} ring_cache;

void ring_cache_init(ring_cache* cache, u8* buffer, u32 size);
s8 zero_cache(ring_cache* cache);
s8 cache_empty(ring_cache* cache);
s8 cache_full(ring_cache* cache);
s32 write_cache(ring_cache* cache, u8* data, u16 len);
s8 write_cache_char(ring_cache* cache, u8* c);
u8 write_cache_char_not_safe(ring_cache* cache, u8* c);
s8 read_cache_char(ring_cache* cache, u8* c);
u8 read_cache_char_not_safe(ring_cache* cache, u8* c);
void markr(ring_cache* cache);
u32 fetch_len(ring_cache* cache);
s8 cache_find_string(ring_cache* cache, u8* dst);

#endif
