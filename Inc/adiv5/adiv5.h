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
 *   -- ADIv5_* structures are changed to be in par with changes in adiv5.c
 *   -- Remove static inline functions and functions definitions. Only available
 *      is present by ADIv5_* structures.
 */ 

#ifndef __ADIV5_H
#define __ADIV5_H

#include "stm32f4xx_it.h"
/* ADIv5 DP Register addresses */
#define ADIV5_DP_IDCODE		0x0
#define ADIV5_DP_ABORT		0x0
#define ADIV5_DP_CTRLSTAT	0x4
#define ADIV5_DP_SELECT		0x8
#define ADIV5_DP_RDBUFF		0xC

/* AP Abort Register (ABORT) */
/* Bits 31:5 - Reserved */
#define ADIV5_DP_ABORT_ORUNERRCLR	(1 << 4)
#define ADIV5_DP_ABORT_WDERRCLR		(1 << 3)
#define ADIV5_DP_ABORT_STKERRCLR	(1 << 2)
#define ADIV5_DP_ABORT_STKCMPCLR	(1 << 1)
/* Bits 5:1 - SW-DP only, reserved in JTAG-DP */
#define ADIV5_DP_ABORT_DAPABORT		(1 << 0)

/* Control/Status Register (CTRLSTAT) */
#define ADIV5_DP_CTRLSTAT_CSYSPWRUPACK	(1u << 31)
#define ADIV5_DP_CTRLSTAT_CSYSPWRUPREQ	(1u << 30)
#define ADIV5_DP_CTRLSTAT_CDBGPWRUPACK	(1u << 29)
#define ADIV5_DP_CTRLSTAT_CDBGPWRUPREQ	(1u << 28)
#define ADIV5_DP_CTRLSTAT_CDBGRSTACK	(1u << 27)
#define ADIV5_DP_CTRLSTAT_CDBGRSTREQ	(1u << 26)
/* Bits 25:24 - Reserved */
/* Bits 23:12 - TRNCNT */
#define ADIV5_DP_CTRLSTAT_TRNCNT
/* Bits 11:8 - MASKLANE */
#define ADIV5_DP_CTRLSTAT_MASKLANE
/* Bits 7:6 - Reserved in JTAG-DP */
#define ADIV5_DP_CTRLSTAT_WDATAERR	(1u << 7)
#define ADIV5_DP_CTRLSTAT_READOK	(1u << 6)
#define ADIV5_DP_CTRLSTAT_STICKYERR	(1u << 5)
#define ADIV5_DP_CTRLSTAT_STICKYCMP	(1u << 4)
#define ADIV5_DP_CTRLSTAT_TRNMODE_MASK	(3u << 2)
#define ADIV5_DP_CTRLSTAT_STICKYORUN	(1u << 1)
#define ADIV5_DP_CTRLSTAT_ORUNDETECT	(1u << 0)


/* ADIv5 MEM-AP Registers */
#define ADIV5_AP_CSW		0x00
#define ADIV5_AP_TAR		0x04
/* 0x08 - Reserved */
#define ADIV5_AP_DRW		0x0C
#define ADIV5_AP_DB(x)		(0x10 + (4*(x)))
/* 0x20:0xF0 - Reserved */
#define ADIV5_AP_CFG		0xF4
#define ADIV5_AP_BASE		0xF8
#define ADIV5_AP_IDR		0xFC

/* AP Control and Status Word (CSW) */
#define ADIV5_AP_CSW_DBGSWENABLE	(1u << 31)
/* Bits 30:24 - Prot, Implementation defined, for Cortex-M3: */
#define ADIV5_AP_CSW_MASTERTYPE_DEBUG	(1u << 29)
#define ADIV5_AP_CSW_HPROT1		(1u << 25)
#define ADIV5_AP_CSW_SPIDEN		(1u << 23)
/* Bits 22:12 - Reserved */
/* Bits 11:8 - Mode, must be zero */
#define ADIV5_AP_CSW_TRINPROG		(1u << 7)
#define ADIV5_AP_CSW_DEVICEEN		(1u << 6)
#define ADIV5_AP_CSW_ADDRINC_NONE	(0u << 4)
#define ADIV5_AP_CSW_ADDRINC_SINGLE	(1u << 4)
#define ADIV5_AP_CSW_ADDRINC_PACKED	(2u << 4)
#define ADIV5_AP_CSW_ADDRINC_MASK	(3u << 4)
/* Bit 3 - Reserved */
#define ADIV5_AP_CSW_SIZE_BYTE		(0u << 0)
#define ADIV5_AP_CSW_SIZE_HALFWORD	(1u << 0)
#define ADIV5_AP_CSW_SIZE_WORD		(2u << 0)
#define ADIV5_AP_CSW_SIZE_MASK	(7u << 0)

/* Constants to make RnW and APnDP parameters more clear in code */
#define ADIV5_LOW_WRITE		0
#define ADIV5_LOW_READ		1
#define ADIV5_LOW_DP		0
#define ADIV5_LOW_AP	1


typedef struct ADIv5_DP_s ADIv5_DP_t;

/* low level functions to access ADIv5_DP */
typedef struct ADIv5_DP_OPS_s {

  void (*dp_write)(void *priv, uint8_t addr, uint32_t value);
  uint32_t (*dp_read)(void *priv, uint8_t addr);

  uint32_t (*error)(void *priv);

  uint32_t (*low_access)(void *priv, uint8_t APnDP, uint8_t RnW,
                         uint8_t addr, uint32_t value);

  void (*priv_free)(ADIv5_DP_t *this);
} ADIv5_DP_OPS_t;

typedef struct ADIv5_DP_s {
  uint16_t ap_count;
  ADIv5_DP_OPS_t *ops;
  void *priv;
} ADIv5_DP_t;


/* Structure describing AP */

typedef struct ADIv5_AP_s ADIv5_AP_t;

typedef struct ADIv5_AP_PRIV_s {
  ADIv5_DP_t *dp;

  uint8_t apsel;

  uint32_t idr;
  uint32_t cfg;
  uint32_t base;
  uint32_t csw;

} ADIv5_AP_PRIV_t;

typedef struct ADIv5_AP_OPS_s {
  uint32_t (*mem_read_word)(ADIv5_AP_PRIV_t *ap, uint32_t addr);
  void (*mem_write_word)(ADIv5_AP_PRIV_t *ap, uint32_t addr, uint32_t value);
  uint32_t (*mem_read_words)(ADIv5_AP_PRIV_t *ap, uint32_t *dest, uint32_t src, uint32_t len);
  uint32_t (*mem_write_words)(ADIv5_AP_PRIV_t *ap, uint32_t dest, const uint32_t *src, uint32_t len);
  uint32_t (*error_check)(ADIv5_AP_PRIV_t *ap);

  void (*priv_free)(ADIv5_AP_t *this);
} ADIv5_AP_OPS_t;

typedef struct ADIv5_AP_s {
  ADIv5_AP_OPS_t *ops;
  ADIv5_AP_PRIV_t *priv;
} ADIv5_AP_t;

uint16_t adiv5_init(ADIv5_DP_t *dp_low_level);

#endif //__ADIV5_H
