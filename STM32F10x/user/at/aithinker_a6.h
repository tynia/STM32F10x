#ifndef _STM32_AITHINKER_A6_H_
#define _STM32_AITHINKER_A6_H_

#include "usart/usart.h"

enum AT_COMMAND
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
};

enum
{
    AT_STATE_IDLE,
    AT_STATE_RECV,
    AT_STATE_TIMEOUT
};

typedef struct _tagCommand
{
    u8 id;
    u8 state;
    u8* cmd;
    u8* ok;
    u8* err;
    
} tagCommand;

void GPRSInit(tagEUSART EUSART, u16* irq);
u32 WaitCommandOK(u32 delay);
u8 Dial(u8* target, u32 port);
u8 SendData(u8* data, u32 len);
u8 Close();
u8 SendCommand(u8 cmd);

#endif
