#ifndef _STM32_RING_BUFFER_H_
#define _STM32_RING_BUFFER_H_

#include "stm32f10x.h"

#define MAX_CACHE_SIZE 256

typedef struct _tagRingCache
{
    u8* ptr;
    u8* head;
    u8* tail;
    u32 capacity;
} tagRingCache;

tagRingCache* InitRingCache(u8* buffer, u32 size);
s32 Write(tagRingCache* cache, u8* data, u32 len);
s32 Read (tagRingCache* cache, u8* buffer, u32 size, u32 toRead);
s8  WriteChar(tagRingCache* cache, u8 c);
s8  ReadChar(tagRingCache* cache, u8* c);
void Markr(tagRingCache* cache);
s32 ReadPacket(tagRingCache* cache, u8* buffer, u32 size);

#endif
