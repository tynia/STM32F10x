#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "usart/usart.h"
#include "gpio/gpio.h"
#include "debug/debug.h"
#include "debug/debugger.h"
#include "util/util.h"
#include "at/aithinker_a6.h"
#include "led/led.h"
#include "transfer/transfer.h"
#include "RTL.h"

u16 irq[] = { USART_IT_RXNE,};

__task void TaskDebugger()
{
#ifdef _DEBUG
    // init debugger
    InitDebugger(USART_COM_4, USART_COM_3);
#else
    InitAcceptor(USART_COM_4, irq, 1, USART_COM_3);
#endif
    while (1)
    {
        os_dly_wait(20);
    }
}

__task void TaskA6()
{
    A6Init(USART_COM_3, irq, 1, USART_COM_4);
    while (1)
    {
        os_dly_wait(20);
    }
}

__task void TaskIdle(void)
{
    while (1)
    {
        transfer();
        os_dly_wait(20);
    }
}

__task void TaskOn(void)
{
    OS_TID debugID = os_tsk_create(TaskDebugger, 3);
    OS_TID a6ID    = os_tsk_create(TaskA6, 3);
    OS_TID idle = os_tsk_create(TaskIdle, 4);
    os_tsk_delete_self();
}

int main(void)
{
    /*u16 irq[3];
    irq[0] = USART_IT_RXNE;
    
#ifdef _DEBUG
    // init debugger
    InitDebugger(USART_COM_4, USART_COM_3);
#else
    InitAcceptor(USART_COM_4, irq, 1, USART_COM_3);
#endif
    // init a6
    A6Init(USART_COM_3, irq, 1, USART_COM_4);
//     if (WaitOK() <= 0)
//     {
//         console("GPRS module initialize failed");
//         return -1;
//     } */
    // init led
    InitLED(LED_A, GPIO_Pin_2);

    os_sys_init(TaskOn);
    console("program start...");
//     while (1)
//     {
//         transfer();
//         os_dly_wait(20);
//         //wait(100);
//         //console("abc\r\n");
//     }
    
    return 0;
}
