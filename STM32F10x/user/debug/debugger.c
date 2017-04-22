#include "debugger.h"
#include "debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "led/led.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

static u8 buffer[MAX_CACHE_SIZE];
static ring_cache* cache;
static u8 debugger = USART_INVALID;
static u8 acceptor = USART_INVALID;
SUPER_CMD_CALLBACK super_cb;

static u8 super[256] = { 0 };
void process(ring_cache* cache)
{
    u8* ptr = super;
    u32 len = 0;
    ASSERT(NULL != cache, "invalid cache");
    while (read_cache_char(cache, ptr) > 0)
    {
        ++ptr;
        ++len;
    }

    super_cb(super, len);
}

//////////////////////////////////////////////////////////////////////
void super_debug(void)
{
    u8 r;
    if (0 != usart_recv_data(debugger, &r))
    {
        write_cache_char(cache, &r);

        if (0 != usart_is_ok(debugger, USART_IT_IDLE))
        {
            // receive command from computer
            //debug("received msg, processing");
            //led_twinkle(LED_A, GPIO_Pin_2, 2, 1000);
            if (NULL != super_cb)
            {
                process(cache);
            }
            //debug("receive msg processed over");
        }
    }

//     if (0 != usart_is_ok(debugger, USART_IT_TC))
//     {
//         led_twinkle(LED_A, GPIO_Pin_2, 3, 1000);
//     }
}

void debug_handler(u8* data, u32 len)
{
    if (USART_INVALID != debugger)
    {
        usart_send_data(debugger, data, len);
        //led_twinkle(LED_A, GPIO_Pin_2, 1, 1000);
    }
}

///////////////////////////////////////////////////////
void debugger_init(u8 idx)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");

    usart_init(idx, NULL, 3, 3, NULL);
#ifdef _DEBUG
    ring_cache_init("DEBUGGER", cache, buffer, MAX_CACHE_SIZE);
#else
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
#endif
    debugger = idx;
    set_debug_handler(debug_handler);
}

void set_debugger_acceptor(u8 idx)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");
    if (idx >= 0 && idx < USART_COM_COUNT)
    {
        acceptor = idx;
    }
}

void set_super_cmd_handler(SUPER_CMD_CALLBACK func)
{
    if (NULL != func)
    {
        super_cb = func;
    }
}
