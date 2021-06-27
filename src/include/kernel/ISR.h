#ifndef __ISR_HEADER__
#define __ISR_HEADER__
#include <types.h>
extern bool ISRInit();
void IRQSetMask(u8 IRQline);
 
void IRQClearMask(u8 IRQline);
#endif