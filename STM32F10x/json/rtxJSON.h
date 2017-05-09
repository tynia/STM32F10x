#ifndef _STM32_RTX_JSON_
#define _STM32_RTX_JSON_

#include "stm32f10x.h"

typedef struct _tag_mem_head {
    u32 id;
    u8* addr;
};

void* delegateMalloc(u32 size);
void  delegateFree(void* ptr);

#endif
