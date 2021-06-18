#pragma once
#include <types.h>
extern bool ISRInit();
void IRQSetMask(u8 IRQline);
 
void IRQClearMask(u8 IRQline);