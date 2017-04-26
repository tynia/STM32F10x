#include "rcc.h"
#include "stm32f10x_rcc.h"

void InitAPBCLKCTRL(u32 APB, u8 state)
{
    if (IS_RCC_APB1_PERIPH(APB))
    {
        RCC_APB1PeriphClockCmd(APB, state);
    }
    else if (IS_RCC_APB2_PERIPH(APB))
    {
        RCC_APB2PeriphClockCmd(APB, state);
    }
    else
    {
        // ERROR !!
    }
}
