/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * Copyright (C) 2018  Tomasz Michalec <tomasz.michalec1996@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * What has been modified by Tomasz Michalec:
 *   -- Fucntions use new API defined in this project
 *   -- Use FreeRTOS memory management
 *   -- Now adiv5_jtag use ADIv5_DP_t structure and expose its functions through
 *      ADIv5_DP_OPS_t.
 */

#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "jtag/jtag_scan.h"
#include "jtag/jtag_low_level.h"
#include "adiv5/adiv5.h"
#include "adiv5/adiv5_jtag.h"

#define JTAGDP_ACK_OK	0x02
#define JTAGDP_ACK_WAIT	0x01

/* 35-bit registers that control the ADIv5 DP */
#define IR_ABORT	0x8
#define IR_DPACC	0xA
#define IR_APACC	0xB

static uint32_t adiv5_jtag_low_access(void *priv, uint8_t APnDP, uint8_t RnW,
              uint8_t addr, uint32_t value)
{
  uint64_t request, response;
  uint8_t ack;

  request = ((uint64_t)value << 3) | ((addr >> 1) & 0x06) | (RnW?1:0);

  jtag_select_dev(((ADIv5_DP_JTAG_t *)priv)->jtag_dev_num);
  jtag_dev_write_ir(APnDP?IR_APACC:IR_DPACC);

	int tries = 1000;
	do {
    jtag_dev_shift_dr((uint_jtag_transfer_t*)&request, (uint_jtag_transfer_t*)&response, 35);
    ack = response & 0x07;
	} while(--tries && (ack == JTAGDP_ACK_WAIT));

  /*if (dp->allow_timeout && (ack == JTAGDP_ACK_WAIT))
    return 0;*/

	if(ack != JTAGDP_ACK_OK) {
    /* Fatal error if invalid ACK response */
    // TODO: do something useful
    printf("ERROR: ack != JTAGDP_ACK_OK\n");
	}

	return (uint32_t)(response >> 3);
}

static void adiv5_jtag_write(void *priv, uint8_t addr, uint32_t value)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_WRITE, addr, value);
}

static uint32_t adiv5_jtag_read(void *priv, uint8_t addr)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ, addr, 0);
	return adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ,
					ADIV5_DP_RDBUFF, 0);
}

static uint32_t adiv5_jtag_error(void *priv)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ,
				ADIV5_DP_CTRLSTAT, 0);
	return adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_WRITE,
				ADIV5_DP_CTRLSTAT, 0xF0000032) & 0x32;
}

static void adiv5_jtag_priv_free(ADIv5_DP_t *this)
{
  if(this->ap_count) {
    printf("ERROR: ap_count = %d while freeing adiv5_jtag!\n", this->ap_count);
  }
  vPortFree(this->priv);
  vPortFree(this);
}

static ADIv5_DP_OPS_t adiv5_jtag_dp_ops = {
  adiv5_jtag_write,
  adiv5_jtag_read,
  adiv5_jtag_error,
  adiv5_jtag_low_access,
  adiv5_jtag_priv_free
};

int adiv5_jtag_handler(int dev_num)
{
  uint16_t dev_count;
  ADIv5_DP_t *adiv5_jtag_low_level = pvPortMalloc(sizeof(ADIv5_DP_t));

  adiv5_jtag_low_level->priv = pvPortMalloc(sizeof(ADIv5_DP_JTAG_t));
  ((ADIv5_DP_JTAG_t*) adiv5_jtag_low_level->priv)->jtag_dev_num = dev_num;
  adiv5_jtag_low_level->ops = &adiv5_jtag_dp_ops;
  adiv5_jtag_low_level->ap_count = 0;

  printf("ADIv5 init start\n");
  dev_count = adiv5_init(adiv5_jtag_low_level);

  if(!dev_count) {
    // non of AP has been probed successful, free low level adiv5
    adiv5_jtag_priv_free(adiv5_jtag_low_level);
  }

  return dev_count;
}

