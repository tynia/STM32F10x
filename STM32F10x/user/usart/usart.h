#ifndef _STM32_USART_H_
#define _STM32_USART_H_

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

typedef enum _tagEUSART
{
    USART_COM_1 = 0,
    USART_COM_2 = 1,
    USART_COM_3 = 2,
    USART_COM_4 = 3,
    USART_COM_5 = 4,
    USART_COM_R0 = 5, // USART REDIRECT
    USART_COM_R1 = 5,
    USART_COM_R2 = 6,
    USART_COM_R3 = 7,
    USART_COM_RU43 = 8,
    USART_COM_COUNT
} tagEUSART;

typedef struct _tagUSART
{
    u8  EGPIO;
    u8  Chn;
    u8  ChnPreemptionPriority;
    u8  ChnSubPriority;
    u8  ChnState;
    u16 Tx;
    u16 Rx;
    u16 Vcc;
    USART_TypeDef* USARTx;
    GPIO_TypeDef*  GPIOx;
} tagUSART;

void USARTInit(tagUSART* USARTx);

void Power(tagUSART* USARTx, u8 on);
#endif
