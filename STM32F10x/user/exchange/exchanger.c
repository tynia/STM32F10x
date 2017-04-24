#include "exchanger.h"
#include "debug/debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "led/led.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

static u8 buffer[MAX_CACHE_SIZE];
static ring_cache rcache;
static ring_cache* cache = &rcache;
static u8 exchanger = USART_INVALID;
EXCHANGE_CALLBACK exchange_cb = NULL;
u8 ro = 0; // recv over
static u8 super[256] = { 0 };
void exchange_data_handler()
{
    u8* ptr = super;
    u32 len = 0;
    ASSERT(NULL != cache, "invalid cache");
    while (read_cache_char(cache, ptr) > 0)
    {
        ++ptr;
        ++len;
    }

    debug_cb(super, len);
}

//////////////////////////////////////////////////////////////////////
void exchange_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(exchanger, &r))
    {
        if (r == 0x0D)
        {
            ro = 1;
        }
        else if (r == 0x0A)
        {
            ro = 0;
            led_twinkle(LED_A, GPIO_Pin_2, 2, 1000);
            // signal data received
        }
        else
        {
            write_cache_char(cache, &r);
        }
    }
}

void exchange_data_handler(u8* data, u32 len)
{
    if (USART_INVALID != exchanger)
    {
        usart_send_data(exchanger, data, len);
    }
}

///////////////////////////////////////////////////////
void exchanger_init(u8 idx, u16* irq, u8 len, EXCHANGE_CALLBACK func)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");
    usart_init(idx, 3, 3, irq, len, exchange_irq_handler);
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
    exchanger = idx;
}

EXCHANGE_CALLBACK set_data_exchange_handler(EXCHANGE_CALLBACK func)
{
    EXCHANGE_CALLBACK old = exchange_cb;
    exchange_cb = func;
    return old;
}
