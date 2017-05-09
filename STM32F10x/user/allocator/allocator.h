#ifndef _STM32_RTX_ALLOCATOR_
#define _STM32_RTX_ALLOCATOR_

#include "stm32f10x.h"
#include "RTL.h"

void* delegateMalloc(size_t size);
void  delegateFree(void* ptr);

#endif
