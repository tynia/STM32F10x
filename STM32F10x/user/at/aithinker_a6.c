#include "aithinker_a6.h"
#include "usart/usart.h"
#include "util/util.h"
#include "transfer/transfer.h"
#include "buffer/buffer.h"
#include "debug/debug.h"
#include "stm32f10x_usart.h"

enum
{
    AT_STATE_IDLE,
    AT_STATE_BUSY
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
    u8* cmd;
    u8* ok;
    u8* err;
} tagATCommand;

static tagATCommand AT_TABLE[] = {
    { AT_CMD_NONE,         NULL,               "OK",       NULL },
    { AT_CMD_CCID,         "AT+CCID\r\n",      "+SCID:",   "+CME" },
    { AT_CMD_CREG,         "AT+CREG=1\r\n",    "+CREG:",   "+CME" },
    { AT_CMD_CGATT,        "AT+CGATT=1\r\n",   "OK",       "+CME" },
    { AT_CMD_CGACT,        "AT+CGACT=1,1\r\n", "OK",       "+CME" },
    { AT_CMD_CSQ,          "AT+CSQ\r\n",       "+CSQ:",    "+CME" },
    { AT_CMD_CIPSTART,     "AT+CIPSTART=",     "OK",       "+CME" },
    { AT_CMD_CIPSEND,      "AT+CIPSEND\r\n",   ">",        "+CME" },
    { AT_CMD_CIPDATA,      NULL,               NULL,       "+CME" },
    { AT_CMD_CIPDATA_DONE, NULL,               "+CIPRCV:", "+CME" },
    { AT_CMD_CIPCLOSE,     "AT+CIPCLOSE\r\n",  "OK",       "+CME" },
    { AT_CMD_CUSTOM,       NULL,               NULL,       "+CME" }
};

static tagEUSART a6  = USART_COM_INVALID;
static u8 recv_state = 0;
static u8 a6_state = AT_STATE_IDLE;
static u16 irq = USART_IT_RXNE;
static tagRingCache* cache = NULL;
static tagATCommand* curCommand = &AT_TABLE[AT_CMD_NONE];
static u8 buffer[MAX_CACHE_SIZE] = { 0 };

//////////////////////////////////////////////////////////////////////
void A6_IRQ_Handler(void)
{
    u8 r = 0;
    if (0 != USARTRecvData(a6, &r))
    {
        WriteChar(cache, r);
        if (r == 0x0D)
        {
            recv_state = 1;
        }
        else if (r == 0x0A)
        {
            recv_state = 0;
            a6_state = AT_STATE_BUSY;
            transmit(a6);
        }
    }
}
// send data to target

#ifdef _DEBUG
s8 SendDataEnd();
void OnGPRSCommandData(u8* data, u32 len)
{
    u8* ptr = data;
    u8* cmd = NULL;
    ASSERT(NULL != data, "invalid data received from debugger");
    if ('$' != *ptr)
    {
        console("command should start with $, e.g. $[0-9]$");
        return;
    }
    else if (0x1A == *ptr)
    {
        SendDataEnd();
        return;
    }

    ++ptr;
    if ('0' > *ptr || '9' < *ptr)
    {
        console("command should start with $, and assigned the number stands for cmd, regex: $[0-9]$");
        return;
    }

    if ('9' == *ptr && '$' == *(ptr + 1))
    {
        SendData(1, ptr + 2, len - 3);
        return;
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
    else if ('3' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CSQ\r\n";
    }
    else if ('4' == *ptr && '$' == *(ptr + 1))
    {
        cmd = "AT+CIPSTART=\"TCP\",\"123.58.34.241\",2000\r\n";
    }
    else if ('5' == *ptr && '$' == *(ptr + 1))
    {
        cmd = data + 3;
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
    else
    {
        console("unknown command: %s", data);
    }
    //console("send [%s]", cmd);
    if (SendData(0, cmd, digitLength(cmd)) < 0)
    {
        console("command send failed, cmd: %s", cmd);
    }
}
#else
void OnGPRSCommandData(u8* data, u32 len)
{}
#endif

void IRQEnable()
{
    InitUSARTIRQ(a6, &irq, 1, ENABLE);
}

void IRQDisable()
{
    InitUSARTIRQ(a6, &irq, 1, DISABLE);
}

////////////////////////////////////////////////////////////////////////
void A6Init(tagEUSART tag, u16* irq, u8 len, tagEUSART target)
{
    ASSERT((tag < MAX_USART_COM_COUNT), "invalid USART tag");
    cache = InitRingCache(buffer, MAX_CACHE_SIZE);
    ASSERT(NULL != cache, "OOM, failed to init cache");
    Register(tag, target, cache, OnGPRSCommandData);
    a6 = tag;
    InitUSARTCTRL(tag, 1, 0, irq, len, A6_IRQ_Handler);
}

u8 WaitOK(void)
{
    u8  data[80] = { 0 };
    u8  rx_state = 0;
    u8  c = 0;
    u8  flag = 0;
    u32 timeout = 60 * 1000;
    u8  ko = 0;
    while (timeout > 0)
    {
        if (USARTRecvData(a6, &c) != 0)
        {
            data[rx_state & 0x7F] = c;
            ++rx_state;
            if (flag & 0x80)
            {
                if (c == 0x0A)
                {
                    flag = 0;
                    data[rx_state & 0x7F] = '\0';
                    ko = findString(data, rx_state & 0x7F, "+CREG:");
                    if (ko > 0)
                    {
                        InitUSARTIRQ(a6, &irq, 1, ENABLE);
                        return 1;
                    }
                    else
                    {
                        zero(data, 80);
                    }
                }
            }
            else
            {
                if (c == 0x0D)
                {
                    flag |= 0x80;
                }
            }
        }
        else
        {
            wait(100);
            timeout -= 100;
        }
    }

    //console("wait a6 ok time out");
    return 0;
}

s8 Dial(u8* target, u16 port)
{
    u8 cmd[50] = { 0 };
    u32 len = 0;
    curCommand = &AT_TABLE[AT_CMD_CIPSTART];
    snprintf((char*)cmd, 50, "%s\"TCP\",\"%s\",%u\r\n", curCommand->cmd, target, port);
    len = digitLength(cmd);
    return SendData(0, cmd, len);
}

s8   SendDataLR()
{
    u8* cmd = "\r\n";
    return SendData(0, cmd, 2);
}

s8 SendDataBegin()
{
    u8* cmd = NULL;
    u32 len = 0;
    curCommand = &AT_TABLE[AT_CMD_CIPSEND];
    cmd = curCommand->cmd;
    len = digitLength(cmd);
    SendData(0, cmd, len);
    return SendDataLR();
}

s8 SendDataEnd()
{
    u8 cmd[] = { 0x1A };
    return SendData(0, cmd, 1);
}

s8   SendData(u8 RAW, u8* data, u32 len)
{
    if (NULL == data)
    {
        return -1;
    }

    if (RAW)
    {
        SendDataBegin();
        wait(5000);
        USARTSendData(a6, data, len);
        SendDataEnd();
    }
    else
    {
        USARTSendData(a6, data, len);
    }
    
    return 0;
}

s8   Close(void)
{
    u8* cmd = NULL;
    u32 len = 0;
    curCommand = &AT_TABLE[AT_CMD_CIPCLOSE];
    cmd = curCommand->cmd;
    len = digitLength(cmd);
    return SendData(0, cmd, len);
}
