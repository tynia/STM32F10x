#ifndef _STM32_IRQ_USART_HANDLER_H_
#define _STM32_IRQ_USART_HANDLER_H_

#include "usart.h"

typedef void (*IRQ_CALLBACK_FUNC)(void);

enum
{
    USART_COM_1 = 0,
    USART_COM_2 = 1,
    USART_COM_3 = 2,
    USART_COM_4 = 3,
    USART_COM_5 = 4,
    USART_COM_COUNT
};

void Set_IRQHandler(u8 idx, IRQ_CALLBACK_FUNC func);

#endif
