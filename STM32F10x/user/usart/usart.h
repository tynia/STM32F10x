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
    USART_COM_R0 = 5, // USART REMAP BEGIN
    USART_COM_R1 = 5,
    USART_COM_R2 = 6,
    USART_COM_R3 = 7,
    USART_COM_RU43 = 8,
    USART_COM_COUNT
} tagEUSART;

void USARTInit(tagEUSART EUSART, u16* irp, u8 PrePriority, u8 SubPriority, IRQ_CALLBACK_FUNC func);

void USARTSendData(tagEUSART EUSART, u8* data, u32 len);

u8 RecvData(tagEUSART EUSART, u8* c);

u8 IsOK(tagEUSART EUSART, u16 irq)

void Power(tagEUSART EUSART, u8 on);
#endif
