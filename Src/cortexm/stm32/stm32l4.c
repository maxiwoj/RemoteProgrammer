#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "cortexm/cortexm.h"
#include "cortexm/stm32/stm32l4.h"
#include "fatfs.h"
#include "target.h"

/* This routine is uses word access. */
static uint16_t stm32l4_flash_write_stub[] = {
// 00000000 <_start>:
  0xf8df, 0x002c,     // ldr.w	r0, [pc, #44]	; 30 <_flashbase>
  0x490b,             // ldr	r1, [pc, #44]	; (34 <_addr>)
  0xa20d,             // add	r2, pc, #52	; (adr r2, 3c <_data>)
  0x4b0b,             // ldr	r3, [pc, #44]	; (38 <_size>)
  0x4d08,             // ldr	r5, [pc, #32]	; (2c <_cr>)

// 0000000c <_next>:
  0xb16b,             // cbz	r3, 2a <_done>
  0x6145,             // str	r5, [r0, #20]
  0x6814,             // ldr	r4, [r2, #0]
  0x600c,             // str	r4, [r1, #0]

//00000014 <_wait>:
  0x6904,             // ldr	r4, [r0, #16]
  0xf416, 0x3f80,     // tst.w	r6, #65536	; 0x10000
  0xd1fb,             // bne.n	14 <_wait>
  0xf016, 0x0ffa,     // tst.w	r6, #250	; 0xfa
  0xd103,             // bne.n	2a <_done>
  0x3b04,             // subs	r3, #4
  0x3104,             // adds	r1, #4
  0x3204,             // adds	r2, #4
  0xe7f0,             // b.n	c <_next>

//0000002a <_done>:
  0xbe00,             // bkpt	0x0000

//0000002c <_cr>:
  0x0001, 0x0000,     //.word	0x00000001

//00000030 <_flashbase>:
  0x2000, 0x4002,     //.word	0x40022000

//00000034 <_addr>:
  0x0000, 0x0000,     //.word	0x00000000

//00000038 <_size>:
  0x0000, 0x0000      //.word	0x00000000

//  0000003c <_data>:
};

static void stm32l4_flash_unlock(STM32L4_PRIV_t *priv)
{
  printf("Flash unlock\n");
  if (priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_CR) & STM32L4_FLASH_CR_LOCK) {
    /* Enable FPEC controller access */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_KEYR, STM32L4_KEY1);
    priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_KEYR, STM32L4_KEY2);
  }
  // Clear flash error
  priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_SR, STM32L4_FLASH_SR_ERROR_MASK);
}

static int stm32l4_erase_all_flash(STM32L4_PRIV_t *priv, int *progress, int progress_end)
{
  uint16_t sr;
  int time = 0;
  int progress_step = progress_end/10;

  /* Flash mass erase start instruction */
  priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_CR, STM32L4_FLASH_CR_MER1 | STM32L4_FLASH_CR_MER2);
  priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_CR, STM32L4_FLASH_CR_STRT | STM32L4_FLASH_CR_MER1 | STM32L4_FLASH_CR_MER2 | STM32L4_FLASH_CR_EOPIE);

  /* Read FLASH_SR to poll for BSY bit */
  while(priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_SR) & STM32L4_FLASH_SR_BSY) {
    if(priv->cortex->ops->check_error(priv->cortex->priv)) {
      // TODO: handle error
      printf("Error while waiting for erase end\n");
      return STM32L4_ERASE_NEVER_END;
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
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_SR);
  if ((sr & STM32L4_SR_ERROR_MASK) || !(sr & STM32L4_SR_EOP)) {
    // TODO: handle error
    printf("Error after erase 0x%x\n", sr);
    return sr | STM32L4_ERASE_ERROR_BIT;
  }

  // End of erase, update progress
  *progress = progress_end;
  return 0;
}

// This function return how many sectors is needed to save len bytes.
// If len is bigger then 64 sectors, return bigger number than number of sectors
static uint16_t stm32l4_last_sector_to_erase(int len)
{
  uint16_t sectors = len/STM32L4_PAGE_SIZE;
  return sectors <= 63 ? sectors : 1024;
}

static int stm32l4_erase_flash(STM32L4_PRIV_t *priv, int len, int *progress, int progress_end)
{
  uint16_t sr;
  uint32_t cr;
  uint16_t last_sector_to_flash = stm32l4_last_sector_to_erase(len);
  int progress_step = (progress_end + 511)/512;

  printf("Erase Flash\n");
  // TODO: For simplicity only when writing less than 64 sectors (2KB each) optimal sector erase
  //       algorithm is used. First 64 sectors are always present which is independent
  //       of size of flash memory.
  if(last_sector_to_flash > 63) {
    return stm32l4_erase_all_flash(priv, progress, progress_end);
  }

  for(uint8_t sector = 0; sector <= last_sector_to_flash; sector++) {
    cr = STM32L4_FLASH_CR_EOPIE | STM32L4_FLASH_CR_ERRIE | STM32L4_FLASH_CR_PER;
    cr |= sector << STM32L4_FLASH_CR_PAGE_SHIFT;
    /* Flash page erase instruction */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_CR, cr);
    /* write address to FMA */
    priv->cortex->ops->write_word(priv->cortex->priv, STM32L4_FLASH_CR, cr | STM32L4_FLASH_CR_STRT);
    /* Read FLASH_SR to poll for BSY bit */
    while(priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_SR) & STM32L4_FLASH_SR_BSY) {
      if(priv->cortex->ops->check_error(priv->cortex->priv)) {
        // TODO: handle error
        printf("Error while waiting for erase end\n");
        return STM32L4_ERASE_NEVER_END;
      }
    }
    *progress += progress_step;
    printf("Flash progress %d\n", *progress);
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_SR);
  if ((sr & STM32L4_SR_ERROR_MASK) || !(sr & STM32L4_SR_EOP)) {
    // TODO: handle error
    printf("Error after erase 0x%x\n", sr);
    return sr | STM32L4_ERASE_ERROR_BIT;
  }

  return 0;
}

static int stm32l4_flash_write(STM32L4_PRIV_t *priv, uint32_t dest, const uint32_t *src, int len)
{
  uint32_t start_of_ram = 0x20000000;
  uint32_t stub_len = 0x3c;
  uint16_t sr;

  /* Fill stm32l4_flash_write_stub with address and size */
	*(uint32_t *)&(stm32l4_flash_write_stub[26]) = dest;
  *(uint32_t *)&(stm32l4_flash_write_stub[28]) = len;

  /* Write stub and data to target ram and set PC */
  priv->cortex->ops->write_words(priv->cortex->priv, start_of_ram, (void*)stm32l4_flash_write_stub, stub_len);
  priv->cortex->ops->write_words(priv->cortex->priv, start_of_ram + stub_len, src, len);
  priv->cortex->ops->pc_write(priv->cortex->priv, start_of_ram);
  if(priv->cortex->ops->check_error(priv->cortex->priv)) {
    // TODO: handle error
    printf("ERROR: Filed to setup write operation\n");
    return STM32L4_ERROR_ON_FLASH_WRITE_SETUP;
  }

  /* Execute the stub */
  priv->cortex->ops->halt_resume(priv->cortex->priv);
  while(!priv->cortex->ops->halt_wait(priv->cortex->priv)) {
    // Don't be greedy about CPU, allow another task
    osDelay(10);
  }

  /* Check for error */
  sr = priv->cortex->ops->read_word(priv->cortex->priv, STM32L4_FLASH_SR);
  if (sr & STM32L4_SR_ERROR_MASK) {
    // TODO: handle error
    printf("ERROR: writing ended with error 0x%x\n", sr);
    return sr | STM32L4_FLASH_ERROR_BIT;
  }

  return 0;
}

static int stm32l4_program(void *priv_void, FIL *file, int *progress)
{
  UINT br;
  uint8_t unaligned;
  uint32_t addr = 0x8000000; // start of flash memory
  uint32_t *data = pvPortMalloc(STM32L4_SIZE_OF_ONE_WRITE);
  STM32L4_PRIV_t *priv = priv_void;
  uint16_t result;
  uint32_t file_len = f_size(file);

  // these variables are only needed to show progress
  int number_of_writes = (file_len + STM32L4_SIZE_OF_ONE_WRITE - 1)/STM32L4_SIZE_OF_ONE_WRITE;
  float progress_as_float = 100 * STM32L4_ERASE_TIME_IN_WRITES/(number_of_writes + STM32L4_ERASE_TIME_IN_WRITES);
  float progress_on_one_write;

  printf("Start flashing STM32L4x\n");

  priv->cortex->ops->halt_request(priv->cortex->priv);
  stm32l4_flash_unlock(priv);
  result = stm32l4_erase_flash(priv, file_len, progress, progress_as_float);
  if(result) {
    vPortFree(data);
    return result;
  }

  progress_as_float = *progress;
  progress_on_one_write = (100 - *progress)/number_of_writes;

  do {
    f_read(file, data, STM32L4_SIZE_OF_ONE_WRITE, &br);
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
      result = stm32l4_flash_write(priv, addr, data, br);
      if(result) {
        vPortFree(data);
        return result;
      }
      // Unaligned read is always smaller then SIZE_OF_ONE_WRITE.
      // This is EOF so we have done here.
      break;
    }
    result = stm32l4_flash_write(priv, addr, data, br);
    if(result) {
      vPortFree(data);
      return 1;
    }
    addr += br;

    progress_as_float += progress_on_one_write;
    *progress = (int)progress_as_float;
    printf("Flash progress %d\n", *progress);

    // EOF is when readed less bytes than requested
  } while(br == STM32L4_SIZE_OF_ONE_WRITE);

  vPortFree(data);

  printf("Device flashed\nReset device\n");
  priv->cortex->ops->restart(priv->cortex->priv);

  *progress = 100;
  return 0;
}

static void stm32l4_restart(void *priv)
{
  ((STM32L4_PRIV_t*)priv)->cortex->ops->restart(((STM32L4_PRIV_t*)priv)->cortex->priv);
}

static void stm32l4_free_priv(void *priv){
  ((STM32L4_PRIV_t*)priv)->cortex->ops->free(((STM32L4_PRIV_t*)priv)->cortex);
  vPortFree(priv);
}

static TARGET_OPS_t stm32l4_ops = {
  .flash_target = stm32l4_program,
  .reset_target = stm32l4_restart,

  .free_priv = stm32l4_free_priv
};

static char stm32l4_name[] = "STM32L4x";

int stm32l4_probe(CORTEXM_t *cortexm)
{
  uint32_t idcode;
  STM32L4_PRIV_t *priv;

  idcode = cortexm->ops->read_word(cortexm->priv, STM32L4_DBGMCU_IDCODE);
  printf("STM32L4 probed id code: 0x%lx\n", idcode);

  switch (idcode & 0xFFF) {
    case 0x461: /* L496/RM0351 */
    case 0x415: /* L471/RM0392, L475/RM0395, L476/RM0351 */
    case 0x462: /* L45x L46x / RM0394  */
    case 0x435: /* L43x L44x / RM0394  */
      priv = pvPortMalloc(sizeof(STM32L4_PRIV_t));
      priv->cortex = cortexm;
      register_target(priv, &stm32l4_ops, stm32l4_name);
      return 1;
  }

  return 0;
}
