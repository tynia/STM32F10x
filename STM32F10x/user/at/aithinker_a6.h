#ifndef _STM32_AITHINKER_A6_H_
#define _STM32_AITHINKER_A6_H_

#include "usart/usart.h"

enum AT_COMMAND
{
    AT_INIT_OK = 0,
    AT_CMD_CCID,
    AT_CMD_CREG,
    AT_CMD_CGATT,
    AT_CMD_CGACT,
    AT_CMD_CSQ,
    AT_CMD_CIPSTART,
    AT_CMD_CIPSEND,
    AT_CMD_DATA,
    AT_CMD_CIPCLOSE,
};

#define AT_ERROR_UNKNOWN 0

typedef struct _tagCommand
{
    u8 id;
    u8 cmd[30];
    u8 success[15];
    u8 failed[15];
    u8 result;
} tagCommand;

static tagCommand AT_TABLE[] = {
    { AT_CMD_CCID,     "AT+CCID",      "+SCID:",  "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CREG,     "AT+CREG=1",    "+CREG:",  "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CGATT,    "AT+CGATT=1",   "OK",      "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CGACT,    "AT+CGACT=1,1", "OK",      "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CSQ,      "AT+CSQ",       "+CSQ:",   "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CIPSTART, "AT+CIPSTART",  "OK",      "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CIPSEND,  "AT+CIPSEND",   ">",       "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_DATA,     ""              "+CIVE:",  "+CME", AT_ERROR_UNKNOWN },
    { AT_CMD_CIPCLOSE, "AT+CIPCLOSE",  "OK",      "+CME", AT_ERROR_UNKNOWN }
};

void GPRSInit(tagUSART* USARTx);
void Use(tagUSART* USARTx);

u8 IsCommandOK(u16 cmd);
u8 Dial(u8* target, u32 port);
u8 _begin();
u8 _end();
u8 _recv();
u8 SendData(u8* data, u32 len);
u8 Close();

#endif
