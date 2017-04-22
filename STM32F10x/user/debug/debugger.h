#ifndef _STM32_DEBUGGER_H_
#define _STM32_DEBUGGER_H_

#include "stm32f10x.h"

typedef void(*SUPER_CMD_CALLBACK)(u8* super_cmd, u32 len);

void debugger_init(u8 idx, u16* irq);
void set_debugger_acceptor(u8 idx);
void set_super_cmd_handler(SUPER_CMD_CALLBACK func);

#endif
