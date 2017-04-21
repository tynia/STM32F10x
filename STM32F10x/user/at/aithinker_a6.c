#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "buffer/buffer.h"
#include "stm32f10x_usart.h"

enum
{
    AT_STATE_IDLE,
    AT_STATE_SEND,
    AT_STATE_RECV,
    AT_STATE_TIMEOUT
};

typedef struct _tag_command
{
    u8 id;
    u8 state;
    u8* cmd;
    u8* ok;
    u8* err;
} tag_command;

static u8 buffer[MAX_CACHE_SIZE];
static ring_cache* cache;
static u8 A6 = USART_COM_INVALID;
static tag_command* curCommand = &AT_TABLE[AT_CMD_NONE];

//////////////////////////////////////////////////////////////////////
void a6_irq_handler(void)
{
    u8 r;
    if (curCommand->state != AT_STATE_IDLE)
    {
        if (0 != usart_recv_data(A6, &r))
        {
            write_cache_char(cache, r);
        }
    }

    if (0 != usart_is_ok(A6, USART_IT_IDLE))
    {
        curCommand->state = AT_STATE_RECV;
    }
}

void a6_init(u8 idx, u16* irq)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");

    usart_init(idx, irq, 3, 3, a6_irq_handler);
#ifdef _DEBUG
    ring_cache_init("GPRS", cache, buffer, MAX_CACHE_SIZE);
#else
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
#endif
    A6 = idx;
}

static tag_command AT_TABLE[] = {
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

u32 wait_cmd_ok(u32 delay) // ms
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
                traceout("wait time out[cmd: %s]", curCommand->cmd);
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
        u32 w = cache_find_string(cache, curCommand->ok);
        if (w > 0)
        {
            // find the expect response
            curCommand->state = AT_STATE_IDLE;
            return w;
        }
    }

    return 0;
}

u8 dial(u8* target, u32 port)
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
    usart_send_data(A6, curCommand->cmd, len);
    curCommand->state = AT_STATE_SEND; // begin to send
    u32 w = wait_cmd_ok(0);
    if (-1 == w)
    {
        traceout("wait response time out, cmd: %s", curCommand->cmd);
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

u8 send_data(u8* data, u32 len)
{
    // send at command
    _begin();
    // data to send
    curCommand->cmd = data;
    _send(len);
    // send done char
    _end();
}

u8 close()
{
    curCommand = &AT_TABLE[AT_CMD_CIPDATA];
    u32 len = str_len(curCommand->cmd);
    usart_send_data(A6, curCommand->cmd, len);
    u32 w = wait_cmd_ok(0);
    if (w != 0)
    {
        skipr(cache, w);
    }
}

u8 send_cmd(u8 cmd)
{
    curCommand = &AT_TABLE[cmd];
    _send(0);
}