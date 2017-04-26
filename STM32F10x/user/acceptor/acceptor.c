#include "stm32f10x_usart.h"
#include "acceptor.h"
#include "debug/debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "util/util.h"
#include "transfer/transfer.h"

static tagEUSART acceptor = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE] = { 0 };
static u8 recv_state = 0;
static tagRingCache* cache = NULL;

void OnAcceptorData(u8* data, u32 len)
{
    if (USART_COM_INVALID != acceptor)
    {
        USARTSendData(acceptor, data, len);
    }
}

void AcceptorIRQHandler(void)
{
    u8 r;
    if (0 != USARTRecvData(acceptor, &r))
    {
        WriteChar(cache, r);
        if (r == 0x0D)
        {
            recv_state = 1;
        }
        else if (r == 0x0A)
        {
            recv_state = 0;
            transmit(acceptor);
        }
    }
}

void InitAcceptor(tagEUSART tag, u16* irq, u8 len, tagEUSART target)
{
    ASSERT(tag < MAX_USART_COM_COUNT, "invalid USART tag");
    ASSERT(tag < MAX_USART_COM_COUNT, "invalid USART target tag");
    cache = InitRingCache(buffer, MAX_CACHE_SIZE);
    ASSERT(NULL == cache, "OOM, failed to init cache");
    Register(tag, target, cache, OnAcceptorData);
    acceptor = tag;
    InitUSARTCTRL(tag, 1, 0, irq, len, AcceptorIRQHandler);
}
