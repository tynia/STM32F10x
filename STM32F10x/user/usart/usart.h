#ifndef _STM32_USART_H_
#define _STM32_USART_H_

#include "stm32f10x.h"
#include "irq_usart_handler.h"

typedef enum _tagEUSART
{
    USART_COM_1 = 0,
    USART_COM_2 = 1,
    USART_COM_3 = 2,
    USART_COM_4 = 3,
    USART_COM_5 = 4,
    USART_COM_R0 = 4, // USART REMAP BEGIN
    USART_COM_R1 = 5,
    USART_COM_R2 = 6,
    USART_COM_R3 = 7,
    USART_COM_RU43 = 8,
    MAX_USART_COM_COUNT,
    USART_COM_INVALID = 0xEF,
} tagEUSART;

typedef void(*ON_DATA_IN_CALLBACK)(u8* data, u32 len);

void InitUSARTCTRL(tagEUSART tag, u8 Priority, u8 SubPriority, u16* irq, u8 len, IRQ_CALLBACK_FUNC func);

void USARTSendData(tagEUSART tag, u8* data, u32 len);

u8   USARTRecvData(tagEUSART tag, u8* c);

u8   USARTCheckStatus(tagEUSART tag, u16 irq);

void InitUSARTIRQ(tagEUSART tag, u16* irq, u8 len, FunctionalState state);

#endif
