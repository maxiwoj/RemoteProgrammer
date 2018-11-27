#ifndef __STM32L4_H
#define __STM32L4_H

#define STM32L4_PAGE_SIZE   0x800
/* Flash Program ad Erase Controller Register Map */
#define STM32L4_FPEC_BASE			0x40022000
#define STM32L4_FLASH_ACR			(STM32L4_FPEC_BASE+0x00)
#define STM32L4_FLASH_KEYR			(STM32L4_FPEC_BASE+0x08)
#define STM32L4_FLASH_OPTKEYR		(STM32L4_FPEC_BASE+0x0c)
#define STM32L4_FLASH_SR			(STM32L4_FPEC_BASE+0x10)
#define STM32L4_FLASH_CR			(STM32L4_FPEC_BASE+0x14)
#define STM32L4_FLASH_OPTR			(STM32L4_FPEC_BASE+0x20)

#define STM32L4_FLASH_CR_PG			(1 << 0)
#define STM32L4_FLASH_CR_PER		(1 << 1)
#define STM32L4_FLASH_CR_MER1		(1 << 2)
#define STM32L4_FLASH_CR_PAGE_SHIFT	3
#define STM32L4_FLASH_CR_BKER		(1 << 11)
#define STM32L4_FLASH_CR_MER2		(1 << 15)
#define STM32L4_FLASH_CR_STRT		(1 << 16)
#define STM32L4_FLASH_CR_OPTSTRT	(1 << 17)
#define STM32L4_FLASH_CR_FSTPG	 	(1 << 18)
#define STM32L4_FLASH_CR_EOPIE		(1 << 24)
#define STM32L4_FLASH_CR_ERRIE		(1 << 25)
#define STM32L4_FLASH_CR_OBL_LAUNCH	(1 << 27)
#define STM32L4_FLASH_CR_OPTLOCK	(1 << 30)
#define STM32L4_FLASH_CR_LOCK		(1 << 31)

#define STM32L4_FLASH_SR_EOP		(1 << 0)
#define STM32L4_FLASH_SR_OPERR		(1 << 1)
#define STM32L4_FLASH_SR_PROGERR	(1 << 3)
#define STM32L4_FLASH_SR_WRPERR		(1 << 4)
#define STM32L4_FLASH_SR_PGAERR		(1 << 5)
#define STM32L4_FLASH_SR_SIZERR		(1 << 6)
#define STM32L4_FLASH_SR_PGSERR		(1 << 7)
#define STM32L4_FLASH_SR_MSERR		(1 << 8)
#define STM32L4_FLASH_SR_FASTERR	(1 << 9)
#define STM32L4_FLASH_SR_RDERR		(1 << 14)
#define STM32L4_FLASH_SR_OPTVERR	(1 << 15)
#define STM32L4_FLASH_SR_ERROR_MASK	0xC3FA
#define STM32L4_FLASH_SR_BSY		(1 << 16)

#define STM32L4_KEY1 0x45670123
#define STM32L4_KEY2 0xCDEF89AB

#define STM32L4_OPTKEY1 0x08192A3B
#define STM32L4_OPTKEY2 0x4C5D6E7F

#define STM32L4_SR_ERROR_MASK	0xF2
#define STM32L4_SR_EOP		0x01

#define STM32L4_OR_DUALBANK		(1 << 21)

#define STM32L4_DBGMCU_IDCODE	0xE0042000
#define STM32L4_FLASH_SIZE_REG 0x1FFF75E0

#define STM32L4_SIZE_OF_ONE_WRITE    0x1000
#define STM32L4_ERASE_TIME_IN_WRITES 10

/* 
 * flash errors returned to flash_target_task
 * 0x0 - 0x1000   - reserved for target (we can use this pool here)
 * bit 11 and 10 == 0 - error while flash erasing. [9..0] represent FLASH_SR[9..0]
 * bit 10 (0x400) - error while flash writing. [9..0] represent FLASH_SR[9..0]
 */
#define STM32F4_ERASE_ERROR_BIT            0x000
#define STM32L4_FLASH_ERROR_BIT            0x400
#define STM32L4_ERASE_NEVER_END            0x800
#define STM32L4_ERROR_ON_FLASH_WRITE_SETUP 0x801

typedef struct CORTEXM_s CORTEXM_t;

typedef struct STM32L4_PRIV_s {
  CORTEXM_t *cortex;
} STM32L4_PRIV_t;

int stm32l4_probe(CORTEXM_t *cortexm);

#endif //__STM32L4_H