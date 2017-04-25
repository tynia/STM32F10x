#include "debugger.h"
#include "debug.h"
#include "stm32f10x_usart.h"
#include "usart/usart.h"
#include "buffer/buffer.h"

static u8 debugger = USART_COM_INVALID;
static u8 target   = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE];
static ring_cache  dummy;
static ring_cache* cache = &dummy;
static u8 recv_state = 0;

// print out through debug com
void debug_out_handler(u8* data, u32 len)
{
    if (USART_COM_INVALID != debugger)
    {
        usart_send_data(debugger, data, len);
    }
}

u8 testbuffer[100] = { 0 };
u8 pos = 0;
void debugger_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(debugger, &r))
    {
        testbuffer[pos++] = r;
        write_cache_char(cache, &r);
        if (r == 0x0D)
        {
            recv_state = 1;
        }
        else if (r == 0x0A)
        {
            recv_state = 0;
            //console("%s", testbuffer);
            transmit(debugger, target);
        }
    }
}

///////////////////////////////////////////////////////
void debugger_init(u8 idx, u8 debug_target)
{
    u16 irq;
    irq = USART_IT_RXNE;
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
    reg(idx, cache, debug_out_handler);
    debugger = idx;
    target = debug_target;
    set_debug_handler(debug_out_handler);
    usart_init(idx, 3, &irq, 1, debugger_irq_handler);
}

void debug_in()
{
   write_cache(cache, "$1$\r\n", 5);
   transmit(debugger, target);
}