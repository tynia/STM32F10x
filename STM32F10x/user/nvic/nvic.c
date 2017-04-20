#include "nvic.h"
#include "misc.h"

void NVICInit(u32 irq, u8 Priority, u8 SubPriority)
{
    NVIC_InitTypeDef nvicInitStruct;
    nvicInitStruct.NVIC_IRQChannel = irq;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = Priority;
    nvicInitStruct.NVIC_IRQChannelSubPriority = SubPriority;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInitStruct);
}
