#include "usart/usart.h"
#include "gpio/gpio.h"
#include "buffer/buffer.h"
#include "nvic/nvic.h"
#include "rcc/rcc.h"
#include "debug/debug.h"
#include "util/util.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tagUSART
{
    u8  GPIOTx;
    u8  GPIORx;
    u16 IRQChannel;
    u16 Tx;
    u16 Rx;
    u16 vcc;
    u32 ReMap;
    u32 rcc;
    u32 to;        // remap to USARTx
    USART_TypeDef* USARTx;
} tagUSART;


static tagUSART USARTGroup[USART_COM_COUNT] = {
    { GPIO_A, GPIO_A, USART1_IRQn, GPIO_Pin_9,  GPIO_Pin_10,
      0, 0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_USART1, USART1 },
    { GPIO_A, GPIO_A, USART2_IRQn, GPIO_Pin_2,  GPIO_Pin_3,
      0, 0, RCC_APB2Periph_GPIOA, RCC_APB1Periph_USART2, USART2 },
    { GPIO_B, GPIO_B, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOB, RCC_APB1Periph_USART3, USART3 },
    { GPIO_C, GPIO_C, UART4_IRQn,  GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOC, RCC_APB1Periph_UART4,  UART4  },
    { GPIO_C, GPIO_D, UART5_IRQn,  GPIO_Pin_12, GPIO_Pin_2,
      0, 0, RCC_APB2Periph_GPIOC, RCC_APB1Periph_UART5,  UART5  },
    // REMAP
    { GPIO_B, GPIO_B, USART1_IRQn, GPIO_Pin_6,  GPIO_Pin_7,
      0, GPIO_Remap_USART1,        RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1, USART1 },
    { GPIO_D, GPIO_D, USART2_IRQn, GPIO_Pin_5,  GPIO_Pin_6,
      0, GPIO_Remap_USART2,        RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2, USART2 },
    { GPIO_D, GPIO_D, USART3_IRQn, GPIO_Pin_8,  GPIO_Pin_9,
      0, GPIO_FullRemap_USART3,    RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
    { GPIO_C, GPIO_C, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11,
      0, GPIO_PartialRemap_USART3, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 }
};

void usart_init(tagEUSART idx, u8 priority, u8 sub_priority, u16* irq, u8 len, IRQ_CALLBACK_FUNC func)
{
    USART_InitTypeDef USART_InitStruct;

    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid USART index");
    ASSERT((0 != irq), "invalid IRQ for usart");

    // CLOCK
    rcc_set_clock(USARTGroup[idx].rcc, ENABLE);
    if (USART_COM_R0 <= idx)
    {
        rcc_set_clock(USARTGroup[idx].to, ENABLE);
        GPIO_PinRemapConfig(USARTGroup[idx].ReMap, ENABLE);
    }

    // GPIO
    // TX
    gpio_init(USARTGroup[idx].GPIOTx, USARTGroup[idx].Tx, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    // RX
    gpio_init(USARTGroup[idx].GPIORx, USARTGroup[idx].Rx, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    // USART
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTGroup[idx].USARTx, &USART_InitStruct);
    USART_Cmd(USARTGroup[idx].USARTx, ENABLE);

    if (NULL != irq)
    {
        u8 i = 0;
        while (i < len)
        {
            USART_ITConfig(USARTGroup[idx].USARTx, *(irq+i), ENABLE);
            ++i;
        }
        // NVIC
        nvic_init(USARTGroup[idx].IRQChannel, priority, sub_priority);
    }

    if (NULL != func)
    {
        set_irq_handler(idx, func);
    }
}

void usart_send_data(tagEUSART idx, u8* data, u32 len)
{
    u32 i = 0;
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid USART index");
    for (; i < len; ++i)
    {
        USART_SendData(USARTGroup[idx].USARTx, *data++);
        while (USART_GetFlagStatus(USARTGroup[idx].USARTx, USART_FLAG_TXE) == RESET);
    }
}

u8 usart_recv_data(tagEUSART idx, u8* c)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid USART index");
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        *c = USART_ReceiveData(USART3);
        return 1;
    }
    return 0;
}

u8 usart_is_ok(tagEUSART idx, u16 irq)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid USART index");
    if (RESET != USART_GetITStatus(USARTGroup[idx].USARTx, irq))
    {
        return 1;
    }
    return 0;
}
