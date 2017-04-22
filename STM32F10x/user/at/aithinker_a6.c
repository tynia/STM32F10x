#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "buffer/buffer.h"
#include "led/led.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

enum
{
    AT_STATE_IDLE,
    AT_STATE_SEND,
    AT_STATE_RECV,
    AT_STATE_TIMEOUT
};

#define AT_CHAR_CR "\r\n"
#define AT_CHAR_CR_SIZE 2
#define CUSTOM_CMD_SIZE 100 + AT_CHAR_CR_SIZE

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
static u8 a6 = USART_COM_INVALID;
static tag_command* curCommand = &AT_TABLE[AT_CMD_NONE];

//////////////////////////////////////////////////////////////////////
void a6_irq_handler(void)
{
    u8 r;
    if (curCommand->state != AT_STATE_IDLE)
    {
        if (0 != usart_recv_data(a6, &r))
        {
            if (curCommand->state != AT_STATE_SEND)
            {
                debug("%c", (u8)(r));
            }
            else
            {
                write_cache_char(cache, r);
            }
        }

        if (0 != usart_is_ok(a6, USART_IT_IDLE))
        {
            if (curCommand->state != AT_STATE_SEND)
            {
                curCommand->state = AT_STATE_RECV;
            }
        }
    }

    if (0 != usart_is_ok(a6, USART_IT_TC))
    {
        wait(2000);
        led_twinkle(LED_A, GPIO_Pin_2, 3, 2000);
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
    a6 = idx;
    // pass it to debugger
    set_usart_acceptor(a6);
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
    { AT_CMD_CIPCLOSE,     AT_STATE_IDLE, "AT+CIPCLOSE\r\n",  "OK",       "+CME" },
    { AT_CMD_CUSTOM,       AT_STATE_IDLE, NULL,               NULL,       "+CME" } 
};

u8 addr[] = { "\"TCP\",\"255.255.255.255\",00000" };
u8 ips = 7;
u8 ps = 24;

s8 wait_cmd_ok(u32 delay) // ms
{
    if (delay == 0)
    {
        delay = 60000 * 1000;
    }

    // wait
    {
        u32 cost = 0;
        u32 period = 1000 * 1000;
        while (AT_STATE_RECV > curCommand->state)
        {
            if (cost < delay)
            {
                wait(period);
                cost += period;
            }
            else
            {
                trace("wait time out[at id: %d]", curCommand->id);
                return -1;
            }
        }
    }

    curCommand->state = AT_STATE_IDLE;

    if (NULL != curCommand->ok)
    {
        if (cache_find_string(cache, curCommand->ok)> 0)
        {
            // find the expect response
            return 1;
        }
    }

    return 0;
}

u8 dial(u8* target, u32 port)
{
    curCommand = &AT_TABLE[AT_CMD_CIPSTART];
    u8 cmd[50] = { 0 };
    vsprintf_s(cmd, "%s\"TCP\",\"%s\",%d", curCommand->cmd, target, port);
    _send(cmd, 0);
}

void _begin()
{
    curCommand = &AT_TABLE[AT_CMD_CIPSEND];
    u8* cmd = curCommand->cmd;
    s8 rc = _send(cmd, 0);
    if (rc < 0)
    {
        debug("%s timeout", curCommand->cmd);
        mark_read(cache);
    }
}

s8 _send(u8* cmd, u32 len)
{
    if (0 == len)
    {
        len = str_len(cmd);
    }
    usart_send_data(a6, cmd, len);
    curCommand->state = AT_STATE_SEND; // begin to send
    u8 rc = wait_cmd_ok(0);
    if ( rc < 0)
    {
        trace("wait response time out, cmd: %s, id: %d", cmd, curCommand->id);
        mark_read(cache);
    }
}

void _end()
{
    u8 cmd[2];
    cmd[0] = 0x1A;
    cmd[1] = '\0';
    curCommand = &AT_TABLE[AT_CMD_CIPDATA_DONE];
    if (_send(cmd, 1) < 0)
    {
        debug("command timeout");
        //mark_read(cache);
    }
    //TODO: handler response
}

u8 send_data(u8* data, u32 len)
{
    // send at command
    _begin();
    // data to send
    curCommand = &AT_TABLE[AT_CMD_CIPDATA];
    _send(data, len);
    // send done char
    _end();
}

u8 close()
{
    curCommand = &AT_TABLE[AT_CMD_CIPCLOSE];
    u8* cmd = curCommand->cmd;
    u32 len = str_len(cmd);
    _send(cmd, len);
}

u8 send_cmd(u8* cmd)
{
    curCommand = &AT_TABLE[AT_CMD_CUSTOM];
    curCommand->ok = NULL;
    u32 len = str_len(cmd);
    u8 r = 0;
    if (cmd[len -1] == '\n' && cmd[len - 2] == '\r')
    {
        r = _send(cmd, len);
    }
    else
    {
        ASSERT(len < CUSTOM_CMD_SIZE - 2, "cmd len is larger than %d", CUSTOM_CMD_SIZE);
        u8* data[CUSTOM_CMD_SIZE];
        vsprintf_s(data, "%s\r\n", cmd);
        r = _send(data, len + 2);
    }

    if (r < 0)
    {
        debug("command: %s timeout", cmd);
    }

    // clear custom command
    curCommand->state = AT_STATE_IDLE;

    return w;
}