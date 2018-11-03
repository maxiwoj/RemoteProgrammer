#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "cortexm/cortexm.h"
#include "cortexm/stm32/stm32f4.h"
#include "fatfs.h"

/* This routine is uses word access.  Only usable on target voltage >2.7V */
uint16_t stm32f4_flash_write_stub[] = {
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

void stm32f4_flash_unlock(STM32F4_PRIV_t *priv)
{
  if (priv->cortex->ops->read_word(priv->cortex->priv, FLASH_CR) & FLASH_CR_LOCK) {
    /* Enable FPEC controller access */
    priv->cortex->ops->write_word(priv->cortex->priv, FLASH_KEYR, KEY1);
    priv->cortex->ops->write_word(priv->cortex->priv, FLASH_KEYR, KEY2);
  }
}

int stm32f4_erase_all_flash(STM32F4_PRIV_t *priv)
{
  uint16_t sr;

  /* Flash mass erase start instruction */
  priv->cortex->ops->write_word(priv->cortex->priv, FLASH_CR, FLASH_CR_MER);
  priv->cortex->ops->write_word(priv->cortex->priv, FLASH_CR, FLASH_CR_STRT | FLASH_CR_MER);

  /* Read FLASH_SR to poll for BSY bit */
  while(priv->cortex->ops->read_word(priv->cortex->priv, FLASH_SR) & FLASH_SR_BSY) {
    if(priv->cortex->ops->check_error(priv->cortex->priv)) {
      // TODO: handle error
      return 1;
    }
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, FLASH_SR);
  if ((sr & SR_ERROR_MASK) || !(sr & SR_EOP)) {
    // TODO: handle error
    return 1;
  }
  return 0;
}

int stm32f4_flash_write(STM32F4_PRIV_t *priv, uint32_t dest, const uint32_t *src, int len)
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
    return 1;
  }

  /* Execute the stub */
  priv->cortex->ops->halt_resume(priv->cortex->priv);
  while(!priv->cortex->ops->halt_wait(priv->cortex->priv)) {
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, FLASH_SR);
  if (sr & SR_ERROR_MASK) {
    // TODO: handle error
    return 1;
  }

  return 0;
}

int stm32f4_program(STM32F4_PRIV_t *priv, FIL *file)
{
  UINT br;
  uint8_t unaligned;
  uint32_t addr = 0x8000000; // start of flash memory
  uint32_t data[SIZE_OF_ONE_WRITE/sizeof(uint32_t)];

  stm32f4_flash_unlock(priv);
  if(stm32f4_erase_all_flash(priv)) {
    return 1;
  }

  do {
    f_read(file, data, SIZE_OF_ONE_WRITE, &br);
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
      if(stm32f4_flash_write(priv, addr, data, br)) {
        return 1;
      }
      // Unaligned read is always smaller then SIZE_OF_ONE_WRITE.
      // This is EOF so we have done here.
      break;
    }
    if(stm32f4_flash_write(priv, addr, data, br)) {
      return 1;
    }
    addr += br;
  } while(br < SIZE_OF_ONE_WRITE);

  return 0;
}

int stm32f4_probe(CORTEXM_t *cortexm)
{
  uint32_t idcode;

  idcode = cortexm->ops->read_word(cortexm->priv, DBGMCU_IDCODE);

  switch (idcode) {
    case 0x411: /* Documented to be 0x413! This is what I read... */
    case 0x413:
    case 0x423: /* F401 */
    case 0x419: /* 427/437 */
      return 1;
  }

  return 0;
}