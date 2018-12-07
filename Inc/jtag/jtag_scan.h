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

#ifndef __JTAG_SCAN_H
#define __JTAG_SCAN_H

#include "stm32f4xx_it.h"
#include "jtag/jtag_low_level.h"

#define JTAG_MAX_DEVS	5
#define JTAG_MAX_IR_LEN	16

#define BYPASS_IR UINT32_MAX

typedef struct jtag_dev_s {
	uint8_t ir_len;
	uint32_t idcode;
	char *descr;
	uint32_t current_ir;
} jtag_dev_t;


void jtag_scan();
void jtag_test();
void jtag_select_dev(int dev);
void jtag_dev_write_ir(uint32_t ir);
void jtag_dev_shift_dr(uint_jtag_transfer_t *din, uint_jtag_transfer_t *dout, int n);

#endif //__JTAG_SCAN_H