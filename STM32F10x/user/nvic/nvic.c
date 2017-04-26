#include "nvic.h"
#include "misc.h"

void InitNVICCTRL(u32 PriorityGroup, u8 IRQChn, u8 Priority, u8 SubPriority)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(PriorityGroup);
    NVIC_InitStructure.NVIC_IRQChannel = IRQChn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
