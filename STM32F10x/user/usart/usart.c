#include "usart/usart.h"
#include "gpio/gpio.h"
#include "misc.h"
#include "stm32f10x_rcc.h"

typedef struct _tagUSARTGroup
{
    u8  GPIOTx;
    u8  GPIORx;
    u8  Chn;
    u8  ChnPreemptionPriority;
    u8  ChnSubPriority;
    u8  ChnState;
    u16 IRQ;
    u16 Tx;
    u16 Rx;
    u16 Vcc;
    u32 ReMap;
    u32 Rcc;
    u32 SelfRcc;
    USART_TypeDef* USARTx;
//     GPIO_TypeDef*  GPIOTx;
//     GPIO_TypeDef*  GPIORx;
} tagUSARTGroup;

// static tagUSARTGroup USARTGroup[USART_COM_COUNT] = {
//     { USART1_IRQn, 0, 0, 0, 0, GPIO_Pin_9,  GPIO_Pin_10, 0, 0, USART1, GPIOA, GPIOA },
//     { USART2_IRQn, 0, 0, 0, 0, GPIO_Pin_2,  GPIO_Pin_3,  0, 0, USART2, GPIOA, GPIOA },
//     { USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11, 0, 0, USART3, GPIOB, GPIOB },
//     { UART4_IRQn,  0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11, 0, 0, UART4,  GPIOC, GPIOC },
//     { UART5_IRQn,  0, 0, 0, 0, GPIO_Pin_12, GPIO_Pin_2,  0, 0, UART5,  GPIOC, GPIOD },
//     // REDIRECT
//     { USART1_IRQn, 0, 0, 0, 0, GPIO_Pin_6,  GPIO_Pin_7,  0, GPIO_Remap_USART1,        USART1, GPIOB, GPIOB },
//     { USART2_IRQn, 0, 0, 0, 0, GPIO_Pin_5,  GPIO_Pin_6,  0, GPIO_Remap_USART2,        USART2, GPIOD, GPIOD },
//     { USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_8,  GPIO_Pin_9,  0, GPIO_FullRemap_USART3,    USART3, GPIOD, GPIOD },
//     { USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11, 0, GPIO_PartialRemap_USART3, USART3, GPIOC, GPIOC },
// };

static tagUSARTGroup USARTGroup[USART_COM_COUNT] = {
    { GPIO_A, GPIO_A, USART1_IRQn, 0, 0, 0, 0, GPIO_Pin_9,  GPIO_Pin_10,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1,  USART1 },
    { GPIO_A, GPIO_A, USART2_IRQn, 0, 0, 0, 0, GPIO_Pin_2,  GPIO_Pin_3,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2, USART2 },
    { GPIO_B, GPIO_B, USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
    { GPIO_C, GPIO_C, UART4_IRQn,  0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART4, UART4  },
    { GPIO_C, GPIO_D, UART5_IRQn,  0, 0, 0, 0, GPIO_Pin_12, GPIO_Pin_2,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART5, UART5  },
    // REMAP
    { GPIO_B, GPIO_B, USART1_IRQn, 0, 0, 0, 0, GPIO_Pin_6,  GPIO_Pin_7,
      0, GPIO_Remap_USART1, RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1,        USART1 },
    { GPIO_D, GPIO_D, USART2_IRQn, 0, 0, 0, 0, GPIO_Pin_5,  GPIO_Pin_6,
      0, GPIO_Remap_USART2, RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2,        USART2 },
    { GPIO_D, GPIO_D, USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_8,  GPIO_Pin_9,
      0, GPIO_FullRemap_USART3, RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3,    USART3 },
    { GPIO_C, GPIO_C, USART3_IRQn, 0, 0, 0, 0, GPIO_Pin_10, GPIO_Pin_11,
      0, GPIO_PartialRemap_USART3, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
};

void USARTInit(tagEUSART EUSART, u8 PrePriority, u8 SubPriority)
{
    ASSERT((EUSART >= 0 && EUSART < USART_COM_COUNT), "invalid usart index");

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
    GPIOInit(USARTGroup[EUSART].GPIOTx, USARTGroup[EUSART].Tx, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStruct.GPIO_Pin = USARTGroup[EUSART].Tx;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_Init(USARTGroup[EUSART].GPIOTx, &GPIO_InitStruct);
//     GPIO_InitStruct.GPIO_Pin = USARTGroup[EUSART].Rx;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_Init(USARTGroup[EUSART].GPIORx, &GPIO_InitStruct);

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
    NVICInit(USARTGroup[EUSART].Chn, PrePriority, SubPriority);
}

void Power(tagUSART* USARTx, u8 on)
{
    if (0 != on)
    {
        GPIO_SetBits(USARTx->GPIOx, USARTx->Vcc);
    }
    else
    {
        GPIO_ResetBits(USARTx->GPIOx, USARTx->Vcc);
    }
}

void USART_ITChange(tagEUSART EUSART, u16 irq)
{
    //TODO:
}