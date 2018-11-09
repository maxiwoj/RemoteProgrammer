#ifndef __JTAG_LOW_LEVEL_H
#define __JTAG_LOW_LEVEL_H

#include "stm32f4xx_it.h"

#define TCKWAIT 20

// maximum size of transfer in jtag_tdin function
typedef uint32_t uint_jtag_transfer_t;


void jtag_tclk(void);

// set TMS to value, cycle TCK
void jtag_tms(GPIO_PinState state);

// cycle TCK, set TDI to value, sample TDO
int jtag_tdi(GPIO_PinState tdi_val, GPIO_PinState tms_val);


// multi-bit version of tdi()
// shift bits to TDI from right most
// first bit from TDO is n-th bit in res 
uint_jtag_transfer_t jtag_tdin(uint8_t n, uint_jtag_transfer_t bits, GPIO_PinState last_tms);

void jtag_from_idle_to_shift_ir();

void jtag_from_idle_to_shift_dr();

void jtag_from_exit_to_idle();

void jtag_go_to_idle();

#endif //__JTAG_LOW_LEVEL_H