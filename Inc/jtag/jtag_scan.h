#ifndef __JTAG_SCAN_H
#define __JTAG_SCAN_H

#include "stm32f4xx_it.h"

#define JTAG_MAX_DEVS	5
#define JTAG_MAX_IR_LEN	16

#define BYPASS_IR (~0)

typedef struct jtag_dev_s {
	uint8_t ir_len;
	uint32_t idcode;
	char *descr;
	uint32_t current_ir;
} jtag_dev_t;


#endif //__JTAG_SCAN_H