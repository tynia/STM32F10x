#ifndef _STM32_AITHINKER_A6_H_
#define _STM32_AITHINKER_A6_H_

#include "stm32f10x.h"

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

void a6_init(u8 idx, u16* irq);
s8 wait_cmd_ok(u32 delay);
s8 dial(u8* target, u32 port);
s8 send_data(u8* data, u32 len);
s8 close(void);

void send_cmd(u8* cmd);
#endif
