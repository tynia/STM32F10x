#include "usart/usart.h"
#include "gpio/gpio.h"
#include "buffer/buffer.h"
#include "nvic/nvic.h"
#include "rcc/rcc.h"
#include "debug/debug.h"
#include "util/util.h"
#include "led/led.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tagUSARTConfig
{
    u8  GPIOTx;
    u8  GPIORx;
    u8  IRQChannel;
    u16 TXD;
    u16 RXD;
    u32 REMAP;
    u32 APBGPIO;
    u32 APB;        // remap to USARTx
    USART_TypeDef* USARTx;
} tagUSARTConfig;

static tagUSARTConfig USARTGroup[MAX_USART_COM_COUNT] = {
    { AGPIO, AGPIO, USART1_IRQn, GPIO_Pin_9,  GPIO_Pin_10, 0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_USART1, USART1 },
    { AGPIO, AGPIO, USART2_IRQn, GPIO_Pin_2,  GPIO_Pin_3,  0, RCC_APB2Periph_GPIOA, RCC_APB1Periph_USART2, USART2 },
    { BGPIO, BGPIO, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11, 0, RCC_APB2Periph_GPIOB, RCC_APB1Periph_USART3, USART3 },
    { CGPIO, CGPIO, UART4_IRQn,  GPIO_Pin_10, GPIO_Pin_11, 0, RCC_APB2Periph_GPIOC, RCC_APB1Periph_UART4,  UART4  },
    { CGPIO, DGPIO, UART5_IRQn,  GPIO_Pin_12, GPIO_Pin_2,  0, RCC_APB2Periph_GPIOC, RCC_APB1Periph_UART5,  UART5  },
    // REMAP
    { BGPIO, BGPIO, USART1_IRQn, GPIO_Pin_6,  GPIO_Pin_7, GPIO_Remap_USART1,
      RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, RCC_APB2Periph_USART1, USART1 },
    { DGPIO, DGPIO, USART2_IRQn, GPIO_Pin_5,  GPIO_Pin_6, GPIO_Remap_USART2,
      RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART2, USART2 },
    { DGPIO, DGPIO, USART3_IRQn, GPIO_Pin_8,  GPIO_Pin_9, GPIO_FullRemap_USART3,
      RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 },
    { CGPIO, CGPIO, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11, GPIO_PartialRemap_USART3,
      RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, RCC_APB1Periph_USART3, USART3 }
};

void InitUSARTCTRL(tagEUSART tag, u8 Priority, u8 SubPriority, u16* irq, u8 len, IRQ_CALLBACK_FUNC func)
{
    USART_InitTypeDef USART_InitStructure;

    ASSERT((tag < MAX_USART_COM_COUNT), "invalid USART tag");

    // CLOCK
    InitAPB2CLKCTRL(USARTGroup[tag].APBGPIO, ENABLE);
    if (tag == USART_COM_1 || tag == USART_COM_R1)
    {
        InitAPB2CLKCTRL(USARTGroup[tag].APB, ENABLE);
    }
    else
    {
        InitAPB1CLKCTRL(USARTGroup[tag].APB, ENABLE);
    }
    
    if (tag > USART_COM_R0)
    {
        GPIO_PinRemapConfig(USARTGroup[tag].REMAP, ENABLE);
    }

    // GPIO
    // TXD
    InitGPIOCTRL(USARTGroup[tag].GPIOTx, USARTGroup[tag].TXD, (u8)GPIO_Mode_AF_PP, (u8)GPIO_Speed_50MHz);
    // RXD
    InitGPIOCTRL(USARTGroup[tag].GPIORx, USARTGroup[tag].RXD, (u8)GPIO_Mode_IN_FLOATING, (u8)GPIO_Speed_50MHz);

    // USART
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTGroup[tag].USARTx, &USART_InitStructure);
    USART_Cmd(USARTGroup[tag].USARTx, ENABLE);
    USART_ClearFlag(USARTGroup[tag].USARTx, USART_FLAG_TC);

    if (NULL != func)
    {
        SetIRQHandler(tag, func);
    }

    // NVIC
    InitNVICCTRL(NVIC_PriorityGroup_0, USARTGroup[tag].IRQChannel, Priority, SubPriority);
    if (NULL != irq)
    {
        u8 i = 0;
        while (i < len)
        {
            USART_ITConfig(USARTGroup[tag].USARTx, *(irq + i), ENABLE);
            ++i;
        }
    }
}

void USARTSendData(tagEUSART tag, u8* data, u32 len)
{
    u32 i = 0;
    u8* ptr = data;
    ASSERT((tag < MAX_USART_COM_COUNT), "invalid USART tag");
    for (; i < len; ++i)
    {
        USART_SendData(USARTGroup[tag].USARTx, *ptr++);
        while (USART_GetFlagStatus(USARTGroup[tag].USARTx, USART_FLAG_TXE) == RESET);
    }
    LEDTwinkle(LED_A, GPIO_Pin_2, 3, 1000);
}

u8 USARTRecvData(tagEUSART tag, u8* c)
{
    ASSERT((tag < MAX_USART_COM_COUNT), "invalid USART tag");
    ASSERT(NULL != c, "invalid char buffer");
    if (USART_GetITStatus(USARTGroup[tag].USARTx, USART_IT_RXNE) != RESET)
    {
        *c = USART_ReceiveData(USARTGroup[tag].USARTx);
        return 1;
    }
    return 0;
}

u8 USARTCheckStatus(tagEUSART tag, u16 irq)
{
    ASSERT((tag < MAX_USART_COM_COUNT), "invalid USART tag");
    if (RESET != USART_GetITStatus(USARTGroup[tag].USARTx, irq))
    {
        return 1;
    }
    return 0;
}

void InitUSARTIRQ(tagEUSART tag, u16* irq, u8 len, FunctionalState state)
{
    if (NULL != irq)
    {
        u8 i = 0;
        while (i < len)
        {
            USART_ITConfig(USARTGroup[tag].USARTx, *(irq + i), state);
            ++i;
        }
    }
}
