#include "stm32f10x_usart.h"
#include "acceptor.h"
#include "debug/debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "util/util.h"
#include "transfer/transfer.h"

static tagEUSART acceptor = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE] = { 0 };
static tagRingCache* cache = NULL;
static u16 toRead = 0;
#define CHECK(x, y)   (x | (1 << y))
#define ISCHECK(x, y) (x & (1 << y))
static u8 syncState = 0;
// byte   1 1 1 1 1 1 1 1
// bit    7 6 5 4 3 2 1 0
//        |     |     |
//    recv len  |     |
//          recv 0xEF |
//                recv 0xEA
#define RECV_SYNC_1 1
#define RECV_SYNC_2 4
#define RECV_LENGTH 7

void OnAcceptorData(u8* data, u32 len)
{
    if (USART_COM_INVALID != acceptor)
    {
        // parse data frame
        // handler
        // make reply data frame
        USARTSendData(acceptor, data, len);
    }
}

void AcceptorIRQHandler(void)
{
    u8 r = 0;
    if (0 != USARTRecvData(acceptor, &r))
    {
        if (0 != ISCHECK(syncState, RECV_SYNC_2))
        {
            if (0 == ISCHECK(syncState, RECV_LENGTH))
            {
                CHECK(syncState, RECV_LENGTH);
                toRead = r - 2;
            }
            WriteChar(cache, r);
            --toRead;
            if (toRead == 0)
            {
                syncState = 0;
                transmit(acceptor);
            }
        }
        else if (0 != ISCHECK(syncState, RECV_SYNC_1))
        {
            if (r == 0xEF)
            {
                CHECK(syncState, 4);
                WriteChar(cache, 0xEA);
                WriteChar(cache, 0xEF);
            }
            else
            {
                syncState = 0;
            }
        }
        else
        {
            if (r == 0xEA)
            {
                CHECK(syncState, RECV_SYNC_1);
            }
        }
    }
}

void InitAcceptor(tagEUSART tag, u16* irq, u8 len, tagEUSART target)
{
    ASSERT(tag < MAX_USART_COM_COUNT, "invalid USART tag");
    ASSERT(tag < MAX_USART_COM_COUNT, "invalid USART target tag");
    cache = InitRingCache(buffer, MAX_CACHE_SIZE);
    ASSERT(NULL != cache, "OOM, failed to init cache");
    Register(tag, target, cache, OnAcceptorData);
    acceptor = tag;
    InitUSARTCTRL(tag, 1, 0, irq, len, AcceptorIRQHandler);
}
