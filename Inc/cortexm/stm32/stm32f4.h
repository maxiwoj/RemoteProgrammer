#ifndef __STM32F4_H
#define __STM32F4_H

/* Flash Program ad Erase Controller Register Map */
#define STM32F4_FPEC_BASE	0x40023C00
#define STM32F4_FLASH_ACR	(STM32F4_FPEC_BASE+0x00)
#define STM32F4_FLASH_KEYR	(STM32F4_FPEC_BASE+0x04)
#define STM32F4_FLASH_OPTKEYR	(STM32F4_FPEC_BASE+0x08)
#define STM32F4_FLASH_SR	(STM32F4_FPEC_BASE+0x0C)
#define STM32F4_FLASH_CR	(STM32F4_FPEC_BASE+0x10)
#define STM32F4_FLASH_OPTCR	(STM32F4_FPEC_BASE+0x14)

#define STM32F4_FLASH_CR_PG		(1 << 0)
#define STM32F4_FLASH_CR_SER		(1 << 1)
#define STM32F4_FLASH_CR_MER		(1 << 2)
#define STM32F4_FLASH_CR_PSIZE8		(0 << 8)
#define STM32F4_FLASH_CR_PSIZE16	(1 << 8)
#define STM32F4_FLASH_CR_PSIZE32	(2 << 8)
#define STM32F4_FLASH_CR_PSIZE64	(3 << 8)
#define STM32F4_FLASH_CR_STRT		(1 << 16)
#define STM32F4_FLASH_CR_EOPIE		(1 << 24)
#define STM32F4_FLASH_CR_ERRIE		(1 << 25)
#define STM32F4_FLASH_CR_STRT		(1 << 16)
#define STM32F4_FLASH_CR_LOCK		(1 << 31)

#define STM32F4_FLASH_SR_BSY		(1 << 16)

#define STM32F4_FLASH_OPTCR_OPTLOCK	(1 << 0)
#define STM32F4_FLASH_OPTCR_OPTSTRT	(1 << 1)
#define STM32F4_FLASH_OPTCR_RESERVED	0xf0000013

#define STM32F4_KEY1 0x45670123
#define STM32F4_KEY2 0xCDEF89AB

#define STM32F4_OPTKEY1 0x08192A3B
#define STM32F4_OPTKEY2 0x4C5D6E7F

#define STM32F4_SR_ERROR_MASK	0xF2
#define STM32F4_SR_EOP		0x01

#define STM32F4_DBGMCU_IDCODE	0xE0042000

#define STM32F4_SIZE_OF_ONE_WRITE    0x1000
#define STM32F4_ERASE_TIME_IN_WRITES 10

/* 
 * flash errors returned to flash_target_task
 * 0x0 - 0x1000   - reserved for target (we can use this pool here)
 * bit 9 (0x200)  - error while flash erasing. [8..0] represent FLASH_SR[8..0]
 * bit 10 (0x400) - error while flash writing. [8..0] represent FLASH_SR[8..0]
 */
#define STM32F4_ERASE_ERROR_BIT            0x200
#define STM32F4_FLASH_ERROR_BIT            0x400
#define STM32F4_ERASE_NEVER_END            0x800
#define STM32F4_ERROR_ON_FLASH_WRITE_SETUP 0x801

typedef struct CORTEXM_s CORTEXM_t;

typedef struct STM32F4_PRIV_s {
  CORTEXM_t *cortex;
} STM32F4_PRIV_t;

int stm32f4_probe(CORTEXM_t *cortexm);

#endif //__STM32F4_H