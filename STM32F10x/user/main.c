#include "led/led.h"
#include "gpio/gpio.h"
#include "debug/debugger.h"
#include "usart/usart.h"
#include "at/aithinker_a6.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

int main(void)
{
    u8 i = 0;
    u16 irq[3];
    //SystemInit();
    irq[0] = USART_IT_IDLE;
    irq[1] = USART_IT_RXNE;
    //irq[2] = USART_IT_TC;
    // init debugger
    debugger_init(USART_COM_3);
    // init led
    led_init(GPIO_A, GPIO_Pin_2);
    // init a6
    a6_init(USART_COM_4, irq, 2);
    
    while (i < 50)
    {
        wait(1000);
    }
    while (1)
    {
        super_debug();
        //wait(0);
    }
    
    return 0;
}
