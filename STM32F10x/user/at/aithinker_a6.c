#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "buffer/buffer.h"
#include "stm32f10x_usart.h"

static u8 buffer[MAX_CACHE_SIZE];
static ringcache* cache;
static tagEUSART GPRS;
static tagEUSART OUTPUT;
static tagCommand* curCommand = &AT_TABLE[AT_CMD_NONE];

//////////////////////////////////////////////////////////////////////
void GPRS_IRQHandler(void)
{
    u8 r;
    if (curCommand->state != AT_STATE_IDLE)
    {
        if (0 != RecvData(GPRS, &r))
        {
            WriteCacheChar(cache, r);
        }
    }

    if (0 != IsOK(GPRS, USART_IT_IDLE))
    {
        curCommand->state = AT_STATE_RECV;
    }
}

void GPRSInit(tagEUSART EUSART, u16* irq)
{
    ASSERT((EUSART >= 0 && EUSART < USART_COM_COUNT), "invalid usart index");

    USARTInit(EUSART, irq, 3, 3, GPRS_IRQHandler);
#ifdef _DEBUG
    InitRingCache("GPRS", cache, buffer, MAX_CACHE_SIZE);
#else
    InitRingCache(cache, buffer, MAX_CACHE_SIZE);
#endif
    GPRS = EUSART;
}

static tagCommand AT_TABLE[] = {
    { AT_CMD_NONE,         AT_STATE_IDLE, NULL,               "OK",       NULL   },
    { AT_CMD_CCID,         AT_STATE_IDLE, "AT+CCID\r\n",      "+SCID:",   "+CME" },
    { AT_CMD_CREG,         AT_STATE_IDLE, "AT+CREG=1\r\n",    "+CREG:",   "+CME" },
    { AT_CMD_CGATT,        AT_STATE_IDLE, "AT+CGATT=1\r\n",   "OK",       "+CME" },
    { AT_CMD_CGACT,        AT_STATE_IDLE, "AT+CGACT=1,1\r\n", "OK",       "+CME" },
    { AT_CMD_CSQ,          AT_STATE_IDLE, "AT+CSQ\r\n",       "+CSQ:",    "+CME" },
    { AT_CMD_CIPSTART,     AT_STATE_IDLE, "AT+CIPSTART=",     "OK",       "+CME" },
    { AT_CMD_CIPSEND,      AT_STATE_IDLE, "AT+CIPSEND\r\n",   ">",        "+CME" },
    { AT_CMD_CIPDATA,      AT_STATE_IDLE, NULL,               NULL,       "+CME" },
    { AT_CMD_CIPDATA_DONE, AT_STATE_IDLE, NULL,               "+CIPRCV:", "+CME" },
    { AT_CMD_CIPCLOSE,     AT_STATE_IDLE, "AT+CIPCLOSE\r\n",  "OK",       "+CME" }
};

u8 addr[] = { "\"TCP\",\"255.255.255.255\",00000" };
u8 ips = 7;
u8 ps = 24;

u32 WaitCommandOK(u32 delay) // ms
{
    if (delay == 0)
    {
        delay = 60 * 1000 * 1000;
    }

    // wait
    {
        u32 cost = 0;
        u32 period = 1000000;
        while (AT_STATE_RECV > curCommand->state)
        {
            if (cost < delay)
            {
                wait(period);
                cost += period;
            }
            else
            {
                debug("wait time out[cmd: %s]", curCommand->cmd);
                return -1;
            }
        }
    }

    if (NULL != curCommand->ok)
    {
        if (curCommand->id == AT_CMD_CIPDATA_DONE)
        {
            curCommand->cmd = NULL;
        }
        u32 w = FindString(cache, curCommand->ok);
        if (w > 0)
        {
            // find the expect response
            curCommand->state = AT_STATE_IDLE;
            return w;
        }
    }

    return 0;
}

u8 Dial(u8* target, u32 port)
{
    curCommand = &AT_TABLE[AT_CMD_CIPSTART];
    _send(0);
}

u8 _begin()
{
    curCommand = &AT_TABLE[AT_CMD_CIPSEND];
    u32 w = _send(0);
    if (w != 0)
    {
        skipr(cache, w);
    }
}

u32 _send(u32 len)
{
    if (0 == len)
    {
        len = str_len(curCommand->cmd);
    }
    USARTSendData(GPRS, curCommand->cmd, len);
    u32 w = WaitCommandOK(0);
    if (-1 == w)
    {
        debug("wait response time out, cmd: %s", curCommand->cmd);
    }

    return w;
}

u8 _end()
{
    u8 cmd[2];
    cmd[0] = 0x1A;
    cmd[1] = '\0';
    curCommand = &AT_TABLE[AT_CMD_CIPDATA_DONE];
    curCommand->cmd = cmd;
    u32 w = _send(1);
    if (w != 0)
    {
        //skipr(cache, w);
        //TODO: pass it to OUTPUT
        // 
    }
}

u8 SendData(u8* data, u32 len)
{
    // send at command
    _begin();
    // data to send
    curCommand->cmd = data;
    _send(len);
    // send done char
    _end();
}

u8 Close()
{
    curCommand = &AT_TABLE[AT_CMD_CIPDATA];
    u32 len = str_len(curCommand->cmd);
    USARTSendData(GPRS, curCommand->cmd, len);
    u32 w = WaitCommandOK(0);
    if (w != 0)
    {
        skipr(cache, w);
    }
}

u8 SendCommand(u8 cmd)
{
    curCommand = &AT_TABLE[cmd];
    _send(0);
}