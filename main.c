#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "debug/debug.h"

#define USART UART4

void NVIC_ConfigInit()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
}

#define LED_ON  (GPIO_ResetBits(GPIOA, GPIO_Pin_2))
#define LED_OFF (GPIO_SetBits(GPIOA, GPIO_Pin_2))

void GPIO_ConfigInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    LED_OFF;
}

void USART_ConfigInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    // RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART, &USART_InitStructure);
    USART_Cmd(USART, ENABLE);
}

void RCC_ConfigInit()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
}

void wait(u32 ms) // ms
{
    u32 i = 0;
    if (ms == 0) {
        ms = 1000 * 1000;
    }
    else
    {
        ms = ms * 1000;
    }

    for (; i < ms; ++i);
}

void debug_out_handler(u8* data, u32 len)
{
    u32 i = 0;
    u8* ptr = data;
    for (; i < len; ++i)
    {
        USART_SendData(USART, *ptr++);
        while (USART_GetFlagStatus(USART, USART_FLAG_TXE) == RESET);
    }
    LED_ON;
    wait(1000);
    LED_OFF;
}

void UART4_IRQHandler(void)
{
    u8 r;
    if (USART_GetITStatus(USART, USART_IT_RXNE) != RESET)
    {
        r = USART_ReceiveData(USART);
        console("%c", r);
        while (USART_GetFlagStatus(USART, USART_FLAG_TXE) == RESET);
    }
}

int main(void)
{
    RCC_ConfigInit();
    GPIO_ConfigInit();
    USART_ConfigInit();
    NVIC_ConfigInit();
    
    set_debug_handler(debug_out_handler);

    while (1)
    {
        wait(1000);
        console("ABC\r\n");
    }
    return 0;
}
