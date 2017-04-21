#include "usart/usart.h"
#include "gpio/gpio.h"
#include "buffer/buffer.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


typedef struct _tagUSARTGroup
{
    u8  GPIOTx;
    u8  GPIORx;
    u16 IRQChannel;
    u16 Tx;
    u16 Rx;
    u16 Vcc;
    u32 ReMap;
    u32 Rcc;
    u32 SelfRcc;
    USART_TypeDef* USARTx;
} tagUSARTGroup;


static tagUSARTGroup USARTGroup[USART_COM_COUNT] = {
    { GPIO_A, GPIO_A, USART1_IRQn, GPIO_Pin_9,  GPIO_Pin_10,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1,  USART1 },
    { GPIO_A, GPIO_A, USART2_IRQn, GPIO_Pin_2,  GPIO_Pin_3,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2, USART2 },
    { GPIO_B, GPIO_B, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
    { GPIO_C, GPIO_C, UART4_IRQn,  GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART4, UART4  },
    { GPIO_C, GPIO_D, UART5_IRQn,  GPIO_Pin_12, GPIO_Pin_2,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART5, UART5  },
    // REMAP
    { GPIO_B, GPIO_B, USART1_IRQn, GPIO_Pin_6,  GPIO_Pin_7,
      0, GPIO_Remap_USART1, RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1,        USART1 },
    { GPIO_D, GPIO_D, USART2_IRQn, GPIO_Pin_5,  GPIO_Pin_6,
      0, GPIO_Remap_USART2, RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2,        USART2 },
    { GPIO_D, GPIO_D, USART3_IRQn, GPIO_Pin_8,  GPIO_Pin_9,
      0, GPIO_FullRemap_USART3, RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3,    USART3 },
    { GPIO_C, GPIO_C, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11,
      0, GPIO_PartialRemap_USART3, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 }
};

USART_TypeDef* GetUSARTGroup(tagEUSART EUSART)
{
    ASSERT((EUSART >= 0 && EUSART < USART_COM_COUNT), "invalid usart index");
    return USARTGroup[EUSART].USARTx;
}

void USARTInit(tagEUSART EUSART, u16* irq, u8 PrePriority, u8 SubPriority, IRQ_CALLBACK_FUNC func)
{
    ASSERT((EUSART >= 0 && EUSART < USART_COM_COUNT), "invalid usart index");
    ASSERT((NULL != irq), "invalid IRQ for usart");

    USART_InitTypeDef USART_InitStruct;
    // CLOCK
    RCC_ClockSet(USARTGroup[EUSART].SelfRcc, ENABLE);
    RCC_ClockSet(USARTGroup[EUSART].Rcc, ENABLE);
    if (USART_COM_R0 <= EUSART)
    {
        GPIO_PinRemapConfig(USARTGroup[EUSART].ReMap, ENABLE);
    }

    // GPIO
    // TX
    GPIOInit(USARTGroup[EUSART].GPIOTx, USARTGroup[EUSART].Tx, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    // RX
    GPIOInit(USARTGroup[EUSART].GPIORx, USARTGroup[EUSART].Rx, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    // USART
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTGroup[EUSART].USARTx, &USART_InitStruct);
    USART_Cmd(USARTGroup[EUSART].USARTx, ENABLE);

    // NVIC
    NVICInit(USARTGroup[EUSART].IRQChannel, PrePriority, SubPriority);

    if (NULL == irq)
    {
        USART_ITConfig(USARTGroup[EUSART].USARTx, USART_IT_RXNE, ENABLE);
        USART_ITConfig(USARTGroup[EUSART].USARTx, USART_IT_IDLE, ENABLE);
    }
    else
    {
        for (; *irq; ++irq)
        {
            USART_ITConfig(USARTGroup[EUSART].USARTx, *irq, ENABLE);
        }
    }

    if (NULL != func)
    {
        Set_IRQHandler(EUSART, func);
    }
}

void USARTSendData(tagEUSART EUSART, u8* data, u32 len)
{
    u32 i = 0;
    for (; i < len; ++i)
    {
        USART_SendData(USARTGroup[EUSART].USARTx, *data++);
        while (USART_GetFlagStatus(USARTGroup[EUSART].USARTx, USART_FLAG_TXE) == RESET);
    }
}


u8 RecvData(tagEUSART EUSART, u8* c)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        *c = USART_ReceiveData(USART3);
        return 1;
    }
    return 0;
}

u8 IsOK(tagEUSART EUSART, u16 irq)
{
    if (RESET != USART_GetITStatus(USARTGroup[EUSART].USARTx, irq))
    {
        return 1;
    }
    return 0;
}

void Power(tagEUSART EUSART, u8 on)
{
    GPIOSetBits(USARTGroup[EUSART].GPIORx, USARTGroup[EUSART].Vcc, on);
}

void USART_ITChange(tagEUSART EUSART, u16 irq)
{
    //TODO:
}