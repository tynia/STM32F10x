#include "nvic.h"
#include "misc.h"

void nvic_init(u32 irq, u8 priority, u8 sub_priority)
{
    NVIC_InitTypeDef nvic_init_struct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_init_struct.NVIC_IRQChannel = irq;
    nvic_init_struct.NVIC_IRQChannelPreemptionPriority = priority;
    nvic_init_struct.NVIC_IRQChannelSubPriority = sub_priority;
    nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_struct);
}
