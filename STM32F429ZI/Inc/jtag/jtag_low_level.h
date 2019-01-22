/* Copyright 2018 Maksymilian Wojczuk and Tomasz Michalec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __JTAG_LOW_LEVEL_H
#define __JTAG_LOW_LEVEL_H

#include "stm32f4xx_it.h"

// if TCKWAIT == 0 than alternate method of delay is used.
// if TCKWAIT > 0 than it represent half of TCK cycle in micro seconds
#define TCKWAIT 0
#define SIZEOF_IN_BITS(x) (sizeof(x) * 8)

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