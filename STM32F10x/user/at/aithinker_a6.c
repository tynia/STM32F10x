#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "buffer/buffer.h"

enum
{
    AT_CMD_SERVICE = 1000,
    AT_CMD_ROAM
};

static tagCommand AT_TABLE_INNER[] = {
    { AT_CMD_SERVICE,  "", "+CIEV:", "", AT_ERROR_UNKNOWN },
    { AT_CMD_ROAM,     "", "+CIEV:", "", AT_ERROR_UNKNOWN },
};

static tagUSART* curUSART;
static u8 buffer[MAX_CACHE_SIZE];
static ringcache* cache;

void GPRSInit(tagUSART* USARTx)
{
    ASSERT(NULL != USARTx, "USART have not been initialized");

    USARTInit(USARTx);
#ifdef _DEBUG
    InitRingCache("GPRS", cache, buffer, MAX_CACHE_SIZE);
#else
    InitRingCache(cache, buffer, MAX_CACHE_SIZE);
#endif

    Power(USARTx, 1);

    Use(USARTx);
}

void Use(tagUSART* USARTx)
{
    curUSART = USARTx;
}

u8 IsCommandOK(u16 cmd)
{
    u32 count = 0;
    u32 len = str_len(AT_TABLE[cmd].success);
    ASSERT(cache->head == cache->tail, "not the same position");
    while (RESET != USART_GetITStatus(curUSART->USARTx, USART_IT_RXNE))
    {
        WriteCacheChar(cache, USART_ReceiveData(curUSART->USARTx));
        ++count;
        if (RESET != USART_GetITStatus(curUSART->USARTx, USART_IT_IDLE))
        {
            if (0 == str_cmp(AT_TABLE[cmd].success, cache->head, len))
            {
                return 1;
            }
        }
    }

    return 0;
}

u8 Dial(u8* target, u32 port)
{

}

u8 _begin()
{
    u8* cmd = AT_TABLE[AT_CMD_CIPSEND].cmd;
    u32 len = str_len(cmd);
    u8 i = 0;
    for (; i < len; ++i)
    {
        USART_SendData(curUSART->USARTx, *cmd++);
        while (USART_GetFlagStatus(curUSART->USARTx, USART_FLAG_TXE) == RESET);
    }
    while (0 != IsCommandOK(AT_CMD_CIPSEND));
}

u8 _end()//"+CIPRCV:" 
{
    u8 cmd = 0x1A;
    u8 i = 0;
    USART_SendData(curUSART->USARTx, cmd);
    while (USART_GetFlagStatus(curUSART->USARTx, USART_FLAG_TXE) == RESET);

    while (0 != IsCommandOK(AT_CMD_DATA));
}

u8 SendData(u8* data, u32 len)
{
    _begin();
    u8 i = 0;
    for (; i < len; ++i)
    {
        USART_SendData(curUSART->USARTx, *data++);
        while (USART_GetFlagStatus(curUSART->USARTx, USART_FLAG_TXE) == RESET);
    }
    _end();
}

u8 _mail();

u8 Close();

void USART3_IRQHandler(void)
{
    u8 r;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        r = USART_ReceiveData(USART3);
        if (r == '>')
        {
            cur = 0;
            u32 i = 0;
            for (; i < 1024; ++i)
            {
                RX_BUF[i] = 0;
            }
        }
        else
        {
            RX_BUF[cur++] = r;
            if (RESET != USART_GetITStatus(curUSART->USARTx, USART_IT_IDLE))
            {
                printf("you receive: %s\n", RX_BUF);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
void USART3_IRQHandler(void)
{
    u8 r;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        r = USART_ReceiveData(USART3);
        if (r == '>')
        {
            cur = 0;
            u32 i = 0;
            for (; i < 1024; ++i)
            {
                RX_BUF[i] = 0;
            }
        }
        else
        {
            RX_BUF[cur++] = r;
            if (RESET != USART_GetITStatus(curUSART->USARTx, USART_IT_IDLE))
            {
                printf("you receive: %s\n", RX_BUF);
            }
        }
    }
}