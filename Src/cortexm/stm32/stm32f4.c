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
 *   -- Add function stm32f4_erase_flash()
 *   -- Add function stm32f4_erase_all_flash()
 *   -- Add function stm32f4_program()
 */

#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "cortexm/cortexm.h"
#include "cortexm/stm32/stm32f4.h"
#include "fatfs.h"
#include "target.h"

/* This routine is uses word access.  Only usable on target voltage >2.7V */
static uint16_t stm32f4_flash_write_stub[] = {
// _start:
  0x480a,	// ldr r0, [pc, #40] // _flashbase
  0x490b,	// ldr r1, [pc, #44] // _addr
  0x467a, // mov r2, pc
  0x3230, // adds r2, #48
  0x4b0a, // ldr r3, [pc, #36] // _size
  0x4d07, // ldr r5, [pc, #28] // _cr
// _next:
  0xb153, // cbz r3, _done
  0x6105, // str r5, [r0, #16]
  0x6814, // ldr r4, [r2]
  0x600c, // str r4, [r1]
// _wait:
  0x89c4, // ldrb r4, [r0, #14]
  0x2601, // movs r6, #1
  0x4234, // tst r4, r6
  0xd1fb, // bne _wait

  0x3b04, // subs r3, #4
  0x3104, // adds r1, #4
  0x3204, // adds r2, #4
  0xe7f3, // b _next
// _done:
  0xbe00, // bkpt
  0x0000,
// .org 0x28
//_cr:
  0x0201, 0x0000, //.word 0x00000201 (Value to write to FLASH_CR) */
// _flashbase:
  0x3c00, 0x4002, // .word 0x40023c00 (FPEC_BASE)
// _addr: // to fill
  0x0000, 0x0000,
// _size: // to fill
  0x0000, 0x0000,
// _data:
// 	...
};

static void stm32f4_flash_unlock(STM32F4_PRIV_t *priv)
{
  printf("Flash unlock\n");
  if (priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_CR) & STM32F4_FLASH_CR_LOCK) {
    /* Enable FPEC controller access */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_KEYR, STM32F4_KEY1);
    priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_KEYR, STM32F4_KEY2);
  }
}

static int stm32f4_erase_all_flash(STM32F4_PRIV_t *priv, int *progress, int progress_end)
{
  uint16_t sr;
  int time = 0;
  int progress_step = progress_end/10;

  /* Flash mass erase start instruction */
  priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_CR, STM32F4_FLASH_CR_MER);
  priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_CR, STM32F4_FLASH_CR_STRT | STM32F4_FLASH_CR_MER | STM32F4_FLASH_CR_EOPIE);

  /* Read FLASH_SR to poll for BSY bit */
  while(priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_SR) & STM32F4_FLASH_SR_BSY) {
    if(priv->cortex->ops->check_error(priv->cortex->priv)) {
      // TODO: handle error
      printf("Error while waiting for erase end\n");
      return STM32F4_ERASE_NEVER_END;
    }
    osDelay(10);
    time++;
    // after each 100 loops add one to progress, we asume that whole erase will take 1000 loops
    if(time > 100) {
      time = 0;
      if(*progress < progress_end - progress_step) {
        *progress += progress_step;
        printf("Flash progress %d\n", *progress);
      }
    }
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_SR);
  if ((sr & STM32F4_SR_ERROR_MASK) || !(sr & STM32F4_SR_EOP)) {
    // TODO: handle error
    printf("Error after erase 0x%x\n", sr);
    return sr | STM32F4_ERASE_ERROR_BIT;
  }

  // End of erase, update progress
  *progress = progress_end;
  return 0;
}

// This function return how many sectors is needed in bank 1 of flash
// to save len bytes. If len is bigger then 7 sectors, return bigger number than number of sectors
static uint8_t stm32f4_sectors_in_bank1(int len)
{
  uint8_t sector = 0;
  while(len > 0) {
    /*
     * Secotr sizes from:
     * RM0090 -- STM32F4xx reference manual, chapter 3.3
     */
    switch (sector) {
      case 0:
      case 1:
      case 2:
      case 3:
        len -= 0x4000;
        break;
      case 4:
        len -= 0x10000;
        break;
      case 5:
      case 6:
      case 7:
        len -= 0x20000;
        break;
      case 8:
        return 255;
    }
    sector++;
  }
  return sector;
}

static int stm32f4_erase_flash(STM32F4_PRIV_t *priv, int len, int *progress, int progress_end)
{
  uint16_t sr;
  uint32_t cr;
  uint8_t  last_sector_to_flash = stm32f4_sectors_in_bank1(len);
  int progress_step = (progress_end + 31)/32;

  printf("Erase Flash\n");
  // TODO: For simplicity only when writing less than 512KB optimal sector erase
  //       algorithm is used. First 7 sectors are the same size which is independent
  //       of single and dual bank mode and size of flash memory.
  if(last_sector_to_flash > 7) {
    return stm32f4_erase_all_flash(priv, progress, progress_end);
  }

  for(uint8_t sector = 0; sector <= last_sector_to_flash; sector++) {
    cr = STM32F4_FLASH_CR_EOPIE | STM32F4_FLASH_CR_ERRIE | STM32F4_FLASH_CR_SER;
    cr |= sector << STM32F4_FLASH_CR_SNB_SHIFT;
    /* Flash page erase instruction */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_CR, cr);
    /* write address to FMA */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32F4_FLASH_CR, cr | STM32F4_FLASH_CR_STRT);
    /* Read FLASH_SR to poll for BSY bit */
    while(priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_SR) & STM32F4_FLASH_SR_BSY) {
      if(priv->cortex->ops->check_error(priv->cortex->priv)) {
        // TODO: handle error
        printf("Error while waiting for erase end\n");
        return STM32F4_ERASE_NEVER_END;
      }
    }
    *progress += progress_step;
    printf("Flash progress %d\n", *progress);
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_SR);
  if ((sr & STM32F4_SR_ERROR_MASK) || !(sr & STM32F4_SR_EOP)) {
    // TODO: handle error
    printf("Error after erase 0x%x\n", sr);
    return sr | STM32F4_ERASE_ERROR_BIT;
  }

  return 0;
}

static int stm32f4_flash_write(STM32F4_PRIV_t *priv, uint32_t dest, const uint32_t *src, int len)
{
  uint32_t start_of_ram = 0x20000000;
  uint32_t stub_len = 0x38;
  uint16_t sr;

  /* Fill stm32f4_flash_write_stub with address and size */
	*(uint32_t *)&(stm32f4_flash_write_stub[24]) = dest;
  *(uint32_t *)&(stm32f4_flash_write_stub[26]) = len;

  /* Write stub and data to target ram and set PC */
  priv->cortex->ops->write_words(priv->cortex->priv, start_of_ram, (void*)stm32f4_flash_write_stub, stub_len);
  priv->cortex->ops->write_words(priv->cortex->priv, start_of_ram + stub_len, src, len);
  priv->cortex->ops->pc_write(priv->cortex->priv, start_of_ram);
  if(priv->cortex->ops->check_error(priv->cortex->priv)) {
    // TODO: handle error
    printf("ERROR: Filed to setup write operation\n");
    return STM32F4_ERROR_ON_FLASH_WRITE_SETUP;
  }

  /* Execute the stub */
  priv->cortex->ops->halt_resume(priv->cortex->priv);
  while(!priv->cortex->ops->halt_wait(priv->cortex->priv)) {
    // Don't be greedy about CPU, allow another task
    osDelay(10);
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32F4_FLASH_SR);
  if (sr & STM32F4_SR_ERROR_MASK) {
    // TODO: handle error
    printf("ERROR: writing ended with error 0x%x\n", sr);
    return sr | STM32F4_FLASH_ERROR_BIT;
  }

  return 0;
}

static int stm32f4_program(void *priv_void, FIL *file, int *progress)
{
  UINT br;
  uint8_t unaligned;
  uint32_t addr = 0x8000000; // start of flash memory
  uint32_t *data = pvPortMalloc(STM32F4_SIZE_OF_ONE_WRITE);
  STM32F4_PRIV_t *priv = priv_void;
  uint16_t result;
  uint32_t file_len = f_size(file);

  // these variables are only needed to show progress
  int number_of_writes = (file_len + STM32F4_SIZE_OF_ONE_WRITE - 1)/STM32F4_SIZE_OF_ONE_WRITE;
  float progress_as_float = 100 * STM32F4_ERASE_TIME_IN_WRITES/(number_of_writes + STM32F4_ERASE_TIME_IN_WRITES);
  float progress_on_one_write;

  printf("Start flashing STM32F4x\n");

  priv->cortex->ops->halt_request(priv->cortex->priv);
  stm32f4_flash_unlock(priv);
  result = stm32f4_erase_flash(priv, file_len, progress, progress_as_float);
  if(result) {
    vPortFree(data);
    return result;
  }

  progress_as_float = *progress;
  progress_on_one_write = (100 - *progress)/number_of_writes;

  do {
    f_read(file, data, STM32F4_SIZE_OF_ONE_WRITE, &br);
    printf("flash 0x%x bytes on 0x%lx\n", br, addr);
    unaligned = br & 0x3;
    if (unaligned) {
      // If number of readed bytes % sizeof(uint32_t) != 0, last readed bytes are unaligned.
      // Fill all unreaded bytes in last 4 bytes as 0xFF. This way flash is not damaged
      // by writing additional (1..3) bytes.
      br >>= 2;
      data[br] |= ((~0) >> (unaligned << 0x3));
      // add modified bytes to bytes that will be written
      br++;
      br <<= 2;
      result = stm32f4_flash_write(priv, addr, data, br);
      if(result) {
        vPortFree(data);
        return result;
      }
      // Unaligned read is always smaller then SIZE_OF_ONE_WRITE.
      // This is EOF so we have done here.
      break;
    }
    result = stm32f4_flash_write(priv, addr, data, br);
    if(result) {
      vPortFree(data);
      return 1;
    }
    addr += br;

    progress_as_float += progress_on_one_write;
    *progress = (int)progress_as_float;
    printf("Flash progress %d\n", *progress);

    // EOF is when readed less bytes than requested
  } while(br == STM32F4_SIZE_OF_ONE_WRITE);

  vPortFree(data);

  printf("Device flashed\nReset device\n");
  priv->cortex->ops->restart(priv->cortex->priv);

  *progress = 100;
  return 0;
}

static void stm32f4_restart(void *priv)
{
  ((STM32F4_PRIV_t*)priv)->cortex->ops->restart(((STM32F4_PRIV_t*)priv)->cortex->priv);
}

static void stm32f4_free_priv(void *priv){
  ((STM32F4_PRIV_t*)priv)->cortex->ops->free(((STM32F4_PRIV_t*)priv)->cortex);
  vPortFree(priv);
}

static TARGET_OPS_t stm32f4_ops = {
  .flash_target = stm32f4_program,
  .reset_target = stm32f4_restart,

  .free_priv = stm32f4_free_priv
};

static char stm32f4_name[] = "STM32F4x";

int stm32f4_probe(CORTEXM_t *cortexm)
{
  uint32_t idcode;
  STM32F4_PRIV_t *priv;

  idcode = cortexm->ops->read_word(cortexm->priv, STM32F4_DBGMCU_IDCODE);
  printf("STM32F4 probed id code: 0x%lx\n", idcode);

  switch (idcode & 0xFFF) {
    case 0x411: /* Documented to be 0x413! This is what I read... */
    case 0x413:
    case 0x423: /* F401 */
    case 0x419: /* 427/437 */
      priv = pvPortMalloc(sizeof(STM32F4_PRIV_t));
      priv->cortex = cortexm;
      register_target(priv, &stm32f4_ops, stm32f4_name);
      return 1;
  }

  return 0;
}
