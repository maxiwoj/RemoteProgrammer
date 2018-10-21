#ifndef __ADIV5_JTAG_H
#define __ADIV5_JTAG_H

#include "adiv5/adiv5.h"

typedef struct ADIv5_DP_JTAG_s {
  int jtag_dev_num;
} ADIv5_DP_JTAG_t;

int adiv5_jtag_handler(int dev_num);

#endif //__ADIV5_JTAG_H