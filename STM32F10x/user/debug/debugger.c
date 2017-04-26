#include "debugger.h"
#include "debug.h"
#include "usart/usart.h"
#include "buffer/buffer.h"
#include "transfer/transfer.h"
#include "stm32f10x_usart.h"

static tagEUSART debugger = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE];
static tagRingCache* cache = NULL;
static u8 recv_state = 0;

// print out through debug com
void OnDebugData(u8* data, u32 len)
{
    if (USART_COM_INVALID != debugger)
    {
        USARTSendData(debugger, data, len);
    }
}

void DebuggerIRQHandler(void)
{
    u8 r;
    if (0 != USARTRecvData(debugger, &r))
    {
        console("%c", r);
//         WriteChar(cache, &r);
//         if (r == 0x0D)
//         {
//             recv_state = 1;
//         }
//         else if (r == 0x0A)
//         {
//             recv_state = 0;
//             transmit(debugger);
//         }
    }
}

void InitDebugger(tagEUSART tag, tagEUSART target)
{
    u16 irq;
    irq = USART_IT_RXNE;
    cache = InitRingCache(buffer, MAX_CACHE_SIZE);
    ASSERT(NULL == cache, "OOM, failed to init cache");
    Register(tag, target, cache, OnDebugData);
    debugger = tag;
    SetDebugOutHandler(OnDebugData);
    InitUSARTCTRL(tag, 1, 0, &irq, 1, DebuggerIRQHandler);
}
