#include "rcc.h"

void rcc_set_clock(u32 mask, u32 APB, u8 enable)
{
    RCC_APB2PeriphClockCmd(mask, enable);
    if (IS_RCC_APB1_PERIPH(APB))
    {
        RCC_APB1PeriphClockCmd(APB, enable);
    }
    else if (IS_RCC_APB2_PERIPH(APB))
    {
        RCC_APB2PeriphClockCmd(APB, enable);
    }
    else
    {
        // ERROR !!
    }
}
