#include "irq_usart_handler.h"

static IRQ_CALLBACK_FUNC irq[USART_COM_COUNT];

void Set_IRQHandler(u8 idx, IRQ_CALLBACK_FUNC func)
{
    irq[idx] = func;
}

void USART1_IRQHandler(void)
{
    if (NULL != irq[USART_COM_1])
    {
        irq[USART_COM_COUNT]();
    }
}

void USART2_IRQHandler(void)
{
    if (NULL != irq[USART_COM_2])
    {
        irq[USART_COM_COUNT]();
    }
}

void USART3_IRQHandler(void)
{
    if (NULL != irq[USART_COM_3])
    {
        irq[USART_COM_COUNT]();
    }
}

void USART4_IRQHandler(void)
{
    if (NULL != irq[USART_COM_4])
    {
        irq[USART_COM_COUNT]();
    }
}

void USART5_IRQHandler(void)
{
    if (NULL != irq[USART_COM_5])
    {
        irq[USART_COM_COUNT]();
    }
}