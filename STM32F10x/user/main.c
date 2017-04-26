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

int main(void)
{
    u16 irq[3];
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
//     }
    // init led
    InitLED(AGPIO, GPIO_Pin_2);

    while (1)
    {
        transfer();
        wait(5000);
        console("abc\r\n");
    }
    
    return 0;
}
