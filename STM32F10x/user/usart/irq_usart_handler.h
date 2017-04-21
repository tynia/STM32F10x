#ifndef _STM32_IRQ_USART_HANDLER_H_
#define _STM32_IRQ_USART_HANDLER_H_

#include "stm32f10x.h"

typedef void (*IRQ_CALLBACK_FUNC)(void);

void set_irq_handler(u8 idx, IRQ_CALLBACK_FUNC func);

#endif
