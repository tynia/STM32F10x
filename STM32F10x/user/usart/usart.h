#ifndef _STM32_USART_H_
#define _STM32_USART_H_

#include "stm32f10x.h"
#include "irq_usart_handler.h"

enum
{
    USART_COM_INVALID = -1,
    USART_COM_1 = 0,
    USART_COM_2 = 1,
    USART_COM_3 = 2,
    USART_COM_4 = 3,
    USART_COM_5 = 4,
    USART_COM_R0 = 5, // USART REMAP BEGIN
    USART_COM_R1 = 5,
    USART_COM_R2 = 6,
    USART_COM_R3 = 7,
    USART_COM_RU43 = 8,
    USART_COM_COUNT
};

void usart_init(u8 idx, u16* irp, u8 priority, u8 sub_priority, IRQ_CALLBACK_FUNC func);

void usart_send_data(u8 idx, u8* data, u32 len);

u8 usart_recv_data(u8 idx, u8* c);

u8 usart_is_ok(u8 idx, u16 irq);

#endif
