#include "usart/usart.h"
#include "gpio/gpio.h"
#include "buffer/buffer.h"
#include "nvic/nvic.h"
#include "rcc/rcc.h"
#include "debug/debug.h"
#include "led/led.h"
#include "util/util.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tag_usart_group
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
} tag_usart_group;


static tag_usart_group USARTGroup[USART_COM_COUNT] = {
    { GPIO_A, GPIO_A, USART1_IRQn, GPIO_Pin_9,  GPIO_Pin_10,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1, USART1 },
    { GPIO_A, GPIO_A, USART2_IRQn, GPIO_Pin_2,  GPIO_Pin_3,
      0, 0, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2, USART2 },
    { GPIO_B, GPIO_B, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
    { GPIO_C, GPIO_C, UART4_IRQn,  GPIO_Pin_10, GPIO_Pin_11,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART4,  UART4  },
    { GPIO_C, GPIO_D, UART5_IRQn,  GPIO_Pin_12, GPIO_Pin_2,
      0, 0, RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_UART5,  UART5  },
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

void usart_init(u8 idx, u16* irq, u8 priority, u8 sub_priority, IRQ_CALLBACK_FUNC func)
{
    USART_InitTypeDef usart_init_struct;
    
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");
    ASSERT((0 != irq), "invalid IRQ for usart");

    // CLOCK
    rcc_set_clock(USARTGroup[idx].SelfRcc, ENABLE);
    rcc_set_clock(USARTGroup[idx].Rcc, ENABLE);
    if (USART_COM_R0 <= idx)
    {
        GPIO_PinRemapConfig(USARTGroup[idx].ReMap, ENABLE);
    }

    // GPIO
    // TX
    gpio_init(USARTGroup[idx].GPIOTx, USARTGroup[idx].Tx, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    // RX
    gpio_init(USARTGroup[idx].GPIORx, USARTGroup[idx].Rx, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    // USART
    usart_init_struct.USART_BaudRate = 115200;
    usart_init_struct.USART_WordLength = USART_WordLength_8b;
    usart_init_struct.USART_StopBits = USART_StopBits_1;
    usart_init_struct.USART_Parity = USART_Parity_No;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTGroup[idx].USARTx, &usart_init_struct);
    USART_Cmd(USARTGroup[idx].USARTx, ENABLE);

    // NVIC
    nvic_init(USARTGroup[idx].IRQChannel, priority, sub_priority);

    if (NULL == irq)
    {
        USART_ITConfig(USARTGroup[idx].USARTx, USART_IT_RXNE, ENABLE);
        USART_ITConfig(USARTGroup[idx].USARTx, USART_IT_IDLE, ENABLE);
    }
    else
    {
        for (; *irq; ++irq)
        {
            USART_ITConfig(USARTGroup[idx].USARTx, *irq, ENABLE);
        }
    }

    if (NULL != func)
    {
        set_irq_handler(idx, func);
    }
}

void usart_send_data(u8 idx, u8* data, u32 len)
{
    u32 i = 0;
    for (; i < len; ++i)
    {
        USART_SendData(USARTGroup[idx].USARTx, *data++);
        while (USART_GetFlagStatus(USARTGroup[idx].USARTx, USART_FLAG_TXE) == RESET);
    }

    wait(1000);
    //led_twinkle(LED_A, GPIO_Pin_2, 5, 500);
}

u8 usart_recv_data(u8 idx, u8* c)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        *c = USART_ReceiveData(USART3);
        return 1;
    }
    return 0;
}

u8 usart_is_ok(u8 idx, u16 irq)
{
    if (RESET != USART_GetITStatus(USARTGroup[idx].USARTx, irq))
    {
        return 1;
    }
    return 0;
}

void set_power(u8 idx, u8 on)
{
    gpio_set_bits(USARTGroup[idx].GPIORx, USARTGroup[idx].Vcc, on);
}

void usart_change_irq(u8 idx, u16 irq)
{
    //TODO:
}
