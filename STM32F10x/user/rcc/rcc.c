#include "rcc.h"

void rcc_set_clock(u32 mask, u8 enable)
{
    if (IS_RCC_APB2_PERIPH(mask))
    {
        RCC_APB2PeriphClockCmd(mask, enable);
    }
    else if (IS_RCC_APB1_PERIPH(mask))
    {
        RCC_APB1PeriphClockCmd(mask, enable);
    }
    else
    {
        // ERROR !!
    }
}
