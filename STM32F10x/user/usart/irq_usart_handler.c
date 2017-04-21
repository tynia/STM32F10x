#include "irq_usart_handler.h"

enum
{
    USART_IRQ_1 = 0,
    USART_IRQ_2 = 1,
    USART_IRQ_3 = 2,
    USART_IRQ_4 = 3,
    USART_IRQ_5 = 4,
    USART_IRQ_COUNT
};

static IRQ_CALLBACK_FUNC irq[USART_IRQ_COUNT];

void set_irq_handler(u8 idx, IRQ_CALLBACK_FUNC func)
{
    ASSERT((idx >= USART_IRQ_1 && idx < USART_IRQ_COUNT), "invalid usart tag index");
    irq[idx] = func;
}

void USART1_IRQHandler(void)
{
    if (NULL != irq[USART_IRQ_1])
    {
        irq[USART_IRQ_1]();
    }
}

void USART2_IRQHandler(void)
{
    if (NULL != irq[USART_IRQ_2])
    {
        irq[USART_IRQ_2]();
    }
}

void USART3_IRQHandler(void)
{
    if (NULL != irq[USART_IRQ_3])
    {
        irq[USART_IRQ_3]();
    }
}

void USART4_IRQHandler(void)
{
    if (NULL != irq[USART_IRQ_4])
    {
        irq[USART_IRQ_5]();
    }
}

void USART5_IRQHandler(void)
{
    if (NULL != irq[USART_IRQ_5])
    {
        irq[USART_IRQ_5]();
    }
}