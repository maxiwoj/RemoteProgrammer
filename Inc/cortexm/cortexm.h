#ifndef __CORTEXM_H
#define __CORTEXM_H

#include "stm32f4xx_it.h"

/* target options recognised by the Cortex-M target */
#define	TOPT_FLAVOUR_V6M	(1<<0)	/* if not set, target is assumed to be v7m */
#define	TOPT_FLAVOUR_V7MF	(1<<1)	/* if set, floating-point enabled. */

/* Private peripheral bus base address */
#define CORTEXM_PPB_BASE	0xE0000000

#define CORTEXM_SCS_BASE	(CORTEXM_PPB_BASE + 0xE000)

#define CORTEXM_AIRCR		(CORTEXM_SCS_BASE + 0xD0C)
#define CORTEXM_CFSR		(CORTEXM_SCS_BASE + 0xD28)
#define CORTEXM_HFSR		(CORTEXM_SCS_BASE + 0xD2C)
#define CORTEXM_DFSR		(CORTEXM_SCS_BASE + 0xD30)
#define CORTEXM_CPACR		(CORTEXM_SCS_BASE + 0xD88)
#define CORTEXM_DHCSR		(CORTEXM_SCS_BASE + 0xDF0)
#define CORTEXM_DCRSR		(CORTEXM_SCS_BASE + 0xDF4)
#define CORTEXM_DCRDR		(CORTEXM_SCS_BASE + 0xDF8)
#define CORTEXM_DEMCR		(CORTEXM_SCS_BASE + 0xDFC)

#define CORTEXM_FPB_BASE	(CORTEXM_PPB_BASE + 0x2000)

/* ARM Literature uses FP_*, we use CORTEXM_FPB_* consistently */
#define CORTEXM_FPB_CTRL	(CORTEXM_FPB_BASE + 0x000)
#define CORTEXM_FPB_REMAP	(CORTEXM_FPB_BASE + 0x004)
#define CORTEXM_FPB_COMP(i)	(CORTEXM_FPB_BASE + 0x008 + (4*(i)))

#define CORTEXM_DWT_BASE	(CORTEXM_PPB_BASE + 0x1000)

#define CORTEXM_DWT_CTRL	(CORTEXM_DWT_BASE + 0x000)
#define CORTEXM_DWT_COMP(i)	(CORTEXM_DWT_BASE + 0x020 + (0x10*(i)))
#define CORTEXM_DWT_MASK(i)	(CORTEXM_DWT_BASE + 0x024 + (0x10*(i)))
#define CORTEXM_DWT_FUNC(i)	(CORTEXM_DWT_BASE + 0x028 + (0x10*(i)))

/* Application Interrupt and Reset Control Register (AIRCR) */
#define CORTEXM_AIRCR_VECTKEY		(0x05FA << 16)
/* Bits 31:16 - Read as VECTKETSTAT, 0xFA05 */
#define CORTEXM_AIRCR_ENDIANESS		(1 << 15)
/* Bits 15:11 - Unused, reserved */
#define CORTEXM_AIRCR_PRIGROUP		(7 << 8)
/* Bits 7:3 - Unused, reserved */
#define CORTEXM_AIRCR_SYSRESETREQ	(1 << 2)
#define CORTEXM_AIRCR_VECTCLRACTIVE	(1 << 1)
#define CORTEXM_AIRCR_VECTRESET		(1 << 0)

/* HardFault Status Register (HFSR) */
#define CORTEXM_HFSR_DEBUGEVT		(1 << 31)
#define CORTEXM_HFSR_FORCED		(1 << 30)
/* Bits 29:2 - Not specified */
#define CORTEXM_HFSR_VECTTBL		(1 << 1)
/* Bits 0 - Reserved */

/* Debug Fault Status Register (DFSR) */
/* Bits 31:5 - Reserved */
#define CORTEXM_DFSR_RESETALL		0x1F
#define CORTEXM_DFSR_EXTERNAL		(1 << 4)
#define CORTEXM_DFSR_VCATCH		(1 << 3)
#define CORTEXM_DFSR_DWTTRAP		(1 << 2)
#define CORTEXM_DFSR_BKPT		(1 << 1)
#define CORTEXM_DFSR_HALTED		(1 << 0)

/* Debug Halting Control and Status Register (DHCSR) */
/* This key must be written to bits 31:16 for write to take effect */
#define CORTEXM_DHCSR_DBGKEY		0xA05F0000
/* Bits 31:26 - Reserved */
#define CORTEXM_DHCSR_S_RESET_ST	(1 << 25)
#define CORTEXM_DHCSR_S_RETIRE_ST	(1 << 24)
/* Bits 23:20 - Reserved */
#define CORTEXM_DHCSR_S_LOCKUP		(1 << 19)
#define CORTEXM_DHCSR_S_SLEEP		(1 << 18)
#define CORTEXM_DHCSR_S_HALT		(1 << 17)
#define CORTEXM_DHCSR_S_REGRDY		(1 << 16)
/* Bits 15:6 - Reserved */
#define CORTEXM_DHCSR_C_SNAPSTALL	(1 << 5)	/* v7m only */
/* Bit 4 - Reserved */
#define CORTEXM_DHCSR_C_MASKINTS	(1 << 3)
#define CORTEXM_DHCSR_C_STEP		(1 << 2)
#define CORTEXM_DHCSR_C_HALT		(1 << 1)
#define CORTEXM_DHCSR_C_DEBUGEN		(1 << 0)

/* Debug Core Register Selector Register (DCRSR) */
#define CORTEXM_DCRSR_REGWnR		  0x00010000
#define CORTEXM_DCRSR_REGSEL_MASK	0x0000001F
#define CORTEXM_DCRSR_REGSEL_XPSR	0x00000010
#define CORTEXM_DCRSR_REGSEL_MSP	0x00000011
#define CORTEXM_DCRSR_REGSEL_PSP	0x00000012

/* Debug Exception and Monitor Control Register (DEMCR) */
/* Bits 31:25 - Reserved */
#define CORTEXM_DEMCR_TRCENA		(1 << 24)
/* Bits 23:20 - Reserved */
#define CORTEXM_DEMCR_MON_REQ		(1 << 19)	/* v7m only */
#define CORTEXM_DEMCR_MON_STEP		(1 << 18)	/* v7m only */
#define CORTEXM_DEMCR_VC_MON_PEND	(1 << 17)	/* v7m only */
#define CORTEXM_DEMCR_VC_MON_EN		(1 << 16)	/* v7m only */
/* Bits 15:11 - Reserved */
#define CORTEXM_DEMCR_VC_HARDERR	(1 << 10)
#define CORTEXM_DEMCR_VC_INTERR		(1 << 9)	/* v7m only */
#define CORTEXM_DEMCR_VC_BUSERR		(1 << 8)	/* v7m only */
#define CORTEXM_DEMCR_VC_STATERR	(1 << 7)	/* v7m only */
#define CORTEXM_DEMCR_VC_CHKERR		(1 << 6)	/* v7m only */
#define CORTEXM_DEMCR_VC_NOCPERR	(1 << 5)	/* v7m only */
#define CORTEXM_DEMCR_VC_MMERR		(1 << 4)	/* v7m only */
/* Bits 3:1 - Reserved */
#define CORTEXM_DEMCR_VC_CORERESET	(1 << 0)

/* Flash Patch and Breakpoint Control Register (FP_CTRL) */
/* Bits 32:15 - Reserved */
/* Bits 14:12 - NUM_CODE2 */	/* v7m only */
/* Bits 11:8 - NUM_LIT */	/* v7m only */
/* Bits 7:4 - NUM_CODE1 */
/* Bits 3:2 - Unspecified */
#define CORTEXM_FPB_CTRL_KEY		(1 << 1)
#define CORTEXM_FPB_CTRL_ENABLE		(1 << 0)

/* Data Watchpoint and Trace Mask Register (DWT_MASKx) */
#define CORTEXM_DWT_MASK_BYTE		(0 << 0)
#define CORTEXM_DWT_MASK_HALFWORD	(1 << 0)
#define CORTEXM_DWT_MASK_WORD		(3 << 0)

/* Data Watchpoint and Trace Function Register (DWT_FUNCTIONx) */
#define CORTEXM_DWT_FUNC_MATCHED	(1 << 24)
#define CORTEXM_DWT_FUNC_DATAVSIZE_WORD	(2 << 10)	/* v7m only */
#define CORTEXM_DWT_FUNC_FUNC_READ	(5 << 0)
#define CORTEXM_DWT_FUNC_FUNC_WRITE	(6 << 0)
#define CORTEXM_DWT_FUNC_FUNC_ACCESS	(7 << 0)

/* Signals returned by cortexm_halt_wait() */
#define SIGINT 2
#define SIGTRAP 5
#define SIGSEGV 11


typedef struct ADIv5_AP_s ADIv5_AP_t;
typedef struct CORTEXM_s CORTEXM_t;

typedef struct CORTEXM_PRIV_s
{
  ADIv5_AP_t *ap;
} CORTEXM_PRIV_t;

typedef struct CORTEXM_OPS_s
{
  uint32_t (*read_word)(CORTEXM_PRIV_t *priv, uint32_t addr);
  void (*write_word)(CORTEXM_PRIV_t *priv, uint32_t addr, uint32_t value);
  uint32_t (*read_words)(CORTEXM_PRIV_t *priv, uint32_t *dest, uint32_t src, uint32_t len);
  uint32_t (*write_words)(CORTEXM_PRIV_t *priv, uint32_t dest, const uint32_t *src, uint32_t len);
  uint32_t (*pc_read)(CORTEXM_PRIV_t *priv);
  void (*pc_write)(CORTEXM_PRIV_t *priv, uint32_t val);
  void (*halt_request)(CORTEXM_PRIV_t *priv);
  uint32_t (*halt_wait)(CORTEXM_PRIV_t *priv);
  void (*halt_resume)(CORTEXM_PRIV_t *priv);
  uint32_t (*check_error)(CORTEXM_PRIV_t *priv);

  void (*free)(CORTEXM_t *cortexm);
} CORTEXM_OPS_t;


typedef struct CORTEXM_s
{
  CORTEXM_OPS_t *ops;
  CORTEXM_PRIV_t *priv;
} CORTEXM_t;

int probe_cortexm(ADIv5_AP_t *ap);

#endif //__CORTEXM_H