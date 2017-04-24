#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "buffer/buffer.h"
#include "led/led.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "debug/debugger.h"
#include "debug/debug.h"

enum
{
    AT_STATE_IDLE,
    AT_STATE_SEND,
    AT_STATE_RECV,
    AT_STATE_TIMEOUT
};

enum
{
    AT_CMD_NONE = 0,
    AT_CMD_CCID,
    AT_CMD_CREG,
    AT_CMD_CGATT,
    AT_CMD_CGACT,
    AT_CMD_CSQ,
    AT_CMD_CIPSTART,
    AT_CMD_CIPSEND,
    AT_CMD_CIPDATA,
    AT_CMD_CIPDATA_DONE,
    AT_CMD_CIPCLOSE,
    AT_CMD_CUSTOM,
};

#define AT_CHAR_CR "\r\n"
#define AT_CHAR_CR_SIZE 2
#define CUSTOM_CMD_SIZE 100 + AT_CHAR_CR_SIZE

typedef struct _tagATCommand
{
    u8 id;
    u8 state;
    u8* cmd;
    u8* ok;
    u8* err;
} tagATCommand;

static tagATCommand AT_TABLE[] = {
    { AT_CMD_NONE,         AT_STATE_IDLE, 0,                  "OK",       0      },
    { AT_CMD_CCID,         AT_STATE_IDLE, "AT+CCID\r\n",      "+SCID:",   "+CME" },
    { AT_CMD_CREG,         AT_STATE_IDLE, "AT+CREG=1\r\n",    "+CREG:",   "+CME" },
    { AT_CMD_CGATT,        AT_STATE_IDLE, "AT+CGATT=1\r\n",   "OK",       "+CME" },
    { AT_CMD_CGACT,        AT_STATE_IDLE, "AT+CGACT=1,1\r\n", "OK",       "+CME" },
    { AT_CMD_CSQ,          AT_STATE_IDLE, "AT+CSQ\r\n",       "+CSQ:",    "+CME" },
    { AT_CMD_CIPSTART,     AT_STATE_IDLE, "AT+CIPSTART=",     "OK",       "+CME" },
    { AT_CMD_CIPSEND,      AT_STATE_IDLE, "AT+CIPSEND\r\n",   ">",        "+CME" },
    { AT_CMD_CIPDATA,      AT_STATE_IDLE, 0,                  0,          "+CME" },
    { AT_CMD_CIPDATA_DONE, AT_STATE_IDLE, 0,                  "+CIPRCV:", "+CME" },
    { AT_CMD_CIPCLOSE,     AT_STATE_IDLE, "AT+CIPCLOSE\r\n",  "OK",       "+CME" },
    { AT_CMD_CUSTOM,       AT_STATE_IDLE, 0,                  0,          "+CME" } 
};

static tagATCommand* curCommand = &AT_TABLE[AT_CMD_NONE];
static u8 buffer[MAX_CACHE_SIZE];
static ring_cache rcache;
static ring_cache* cache = &rcache;
static u8 a6 = USART_INVALID;

s8 _send(u8* cmd, u32 len);

//////////////////////////////////////////////////////////////////////
void a6_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(a6, &r))
    {
        if (curCommand->state != AT_STATE_SEND)
        {
            debug("%c", (u8)(r));
        }
        else
        {
            write_cache_char(cache, &r);
        }
    }

    if (0 != usart_is_ok(a6, USART_IT_IDLE))
    {
        if (curCommand->state == AT_STATE_SEND)
        {
            curCommand->state = AT_STATE_RECV;
        }

        led_twinkle(LED_A, GPIO_Pin_2, 2, 1000);
    }

    if (0 != usart_is_ok(a6, USART_IT_TC))
    {
        led_twinkle(LED_A, GPIO_Pin_2, 3, 1000);
    }
}

void super_cmd_handler(u8* data, u32 len)
{
    u8* ptr = data;
    u8* cmd = NULL;
    ASSERT(NULL != data, "invalid data received from debugger");
    if ('$' != *ptr)
    {
        debug("not an valid super command, should start with \"$\", cmd: %s", data);
        return;
    }

    ++ptr;
    if ('0' > *ptr || '9' < *ptr)
    {
        debug("not an valid super command, must be 0-9, cmd: %s", data);
        return;
    }

    if ('9' == *ptr && '$' == *(ptr + 1))
    {
        send_data(ptr + 3, len - 3);
    }

    if ('0' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT\r\n";
    }
    else if ('1' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CCID\r\n";
    }
    else if ('2' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CREG?\r\n";
    }
    else if('3' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CSQ\r\n";
    }
    else if ('4' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CIPSTART=\"TCP\",\"123.58.34.241\",2000\r\n";
    }
    else if ('5' == *ptr && '$' == *(ptr + 1))
    {
        cmd = data;
    }
    else if ('6' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CIPCLOSE\r\n";
    }
    else if ('7' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CIPSTATUS\r\n";
    }
    else if ('8' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CIFSR\r\n";
    }
    debug("send [%s]", cmd);
    if (_send(cmd, 0) < 0)
    {
        debug("command send time out, cmd: %s", cmd);
    }
}
////////////////////////////////////////////////////////////////////////

void a6_init(u8 idx, u16* irq, u8 len)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");

    usart_init(idx, 3, 3, irq, len, a6_irq_handler);
#ifdef _DEBUG
    ring_cache_init("GPRS", cache, buffer, MAX_CACHE_SIZE);
#else
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
#endif
    a6 = idx;
    // pass it to debugger
    set_debugger_acceptor(a6);
    set_super_cmd_handler(super_cmd_handler);
}

u8 a6_is_ready()
{
    u8 r = 0;
    curCommand = &AT_TABLE[AT_CMD_CREG];
    curCommand->state = AT_STATE_SEND; // begin to send
    r = wait_cmd_ok(0);
    return r > 1 ? 1 : 0;
}

s8 wait_cmd_ok(u32 delay) // ms
{
    if (delay == 0)
    {
        delay = 60 * 1000;
    }

    // wait
    {
        u32 cost = 0;
        u32 period = 1000;
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

    if (0 != curCommand->ok)
    {
        if (cache_find_string(cache, curCommand->ok)> 0)
        {
            // find the expect response
            return 1;
        }
    }

    return 0;
}

s8 _send(u8* cmd, u32 len)
{
    s8 rc = 0;
    if (NULL == cmd)
    {
        trace("try to send invalid data");
        return -1;
    }

    if (0 == len)
    {
        len = str_len(cmd);
    }
    curCommand = &AT_TABLE[AT_CMD_CIPDATA];
    usart_send_data(a6, cmd, len);
    curCommand->state = AT_STATE_SEND; // begin to send
    rc = wait_cmd_ok(0);
    if (rc < 0)
    {
        trace("wait response time out, cmd: %s, id: %d", cmd, curCommand->id);
        mark_read(cache);
        return rc;
    }

    return 0;
}

s8 dial(u8* target, u32 port)
{
    u8 cmd[50] = { 0 };
    curCommand = &AT_TABLE[AT_CMD_CIPSTART];
    snprintf((char*)cmd, 50, "%s\"TCP\",\"%s\",%u\r\n", curCommand->cmd, target, port);
    return _send(cmd, 0);
}

s8 _begin()
{
    u8* cmd = NULL;
    curCommand = &AT_TABLE[AT_CMD_CIPSEND];
    cmd = curCommand->cmd;
    return _send(cmd, 0);
}

s8 _end()
{
    u8 cmd[2];
    cmd[0] = 0x1A;
    cmd[1] = '\0';
    curCommand = &AT_TABLE[AT_CMD_CIPDATA_DONE];
    if (_send(cmd, 1) < 0)
    {
        trace("command timeout");
        return -1;
    }
    //TODO: handler response
    return 0;
}

s8 send_data(u8* data, u32 len)
{
    s8 r = 0;
    // send at command
    r = _begin();
    if (r < 0)
    {
        trace("command time out");
        return r;
    }
    // data to send
    r = _send(data, len);
    if (r < 0)
    {
        trace("send user data time out, data: %s", data);
        return r;
    }
    // send done char
    r = _end();
    if (r < 0)
    {
        trace("send command 0x1A response timeout");
        return r;
    }

    return 0;
}

s8 close()
{
    u8* cmd = NULL;
    u32 len = 0;
    curCommand = &AT_TABLE[AT_CMD_CIPCLOSE];
    cmd = curCommand->cmd;
    len = str_len(cmd);
    return _send(cmd, len);
}

void send_cmd(u8* cmd)
{
    u32 len = str_len(cmd);
    s8 r = 0;
    ASSERT(NULL != cmd, "invalid cmd in");
    curCommand = &AT_TABLE[AT_CMD_CUSTOM];
    curCommand->ok = 0;
    r = _send(cmd, len);
    if (r < 0)
    {
        trace("command: %s timeout", cmd);
    }
}
