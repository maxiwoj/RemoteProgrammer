#ifndef __JTAG_SCAN_H
#define __JTAG_SCAN_H

#include "stm32f4xx_it.h"
#include "jtag/jtag_low_level.h"

#define JTAG_MAX_DEVS	5
#define JTAG_MAX_IR_LEN	16

#define BYPASS_IR (~0)

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