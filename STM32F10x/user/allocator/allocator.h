#ifndef _STM32_RTX_ALLOCATOR_
#define _STM32_RTX_ALLOCATOR_

#include "stm32f10x.h"
#include "RTL.h"

void InitMemoryMgr(void);
void* rtxMalloc(size_t size);
void  rtxFree(void* ptr);

#endif
