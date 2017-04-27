#include "irq_usart_handler.h"
#include "debug/debug.h"

enum
{
    USART_IRQ_1 = 0,
    USART_IRQ_2 = 1,
    USART_IRQ_3 = 2,
    USART_IRQ_4 = 3,
    USART_IRQ_5 = 4,
    USART_IRQ_R1,
    USART_IRQ_R2,
    USART_IRQ_PR3,
    USART_IRQ_FR3,
    MAX_USART_IRQ_COUNT,
};

static u8 IRQHandlerMap[] = {
    USART_IRQ_1,
    USART_IRQ_2,
    USART_IRQ_3,
    USART_IRQ_4,
    USART_IRQ_5,
    USART_IRQ_1,
    USART_IRQ_2,
    USART_IRQ_3,
    USART_IRQ_3,
};

#define MAX_IRQ_HANDLER_COUNT 5

static IRQ_CALLBACK_FUNC irq[MAX_IRQ_HANDLER_COUNT] = { 0 };

void SetIRQHandler(u8 tag, IRQ_CALLBACK_FUNC func)
{
    ASSERT((tag < MAX_USART_IRQ_COUNT), "invalid USART IRQ tag");
    irq[IRQHandlerMap[tag]] = func;
}

void USART1_IRQHandler(void)
{
    if (0 != irq[USART_IRQ_1])
    {
        irq[USART_IRQ_1]();
    }
}

void USART2_IRQHandler(void)
{
    if (0 != irq[USART_IRQ_2])
    {
        irq[USART_IRQ_2]();
    }
}

void USART3_IRQHandler(void)
{
    if (0 != irq[USART_IRQ_3])
    {
        irq[USART_IRQ_3]();
    }
}

void UART4_IRQHandler(void)
{
    if (0 != irq[USART_IRQ_4])
    {
        irq[USART_IRQ_4]();
    }
}

void UART5_IRQHandler(void)
{
    if (0 != irq[USART_IRQ_5])
    {
        irq[USART_IRQ_5]();
    }
}
