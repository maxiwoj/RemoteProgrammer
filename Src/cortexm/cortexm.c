#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "adiv5/adiv5.h"
#include "cortexm/cortexm.h"
#include "cortexm/stm32/stm32f4.h"
#include "cortexm/stm32/stm32l4.h"

inline static uint32_t cortexm_read_word(CORTEXM_PRIV_t *priv, uint32_t addr)
{
  return priv->ap->ops->mem_read_word(priv->ap->priv, addr);
}

inline static void cortexm_write_word(CORTEXM_PRIV_t *priv, uint32_t addr, uint32_t value)
{
  priv->ap->ops->mem_write_word(priv->ap->priv, addr, value);
}

static uint32_t cortexm_read_words(CORTEXM_PRIV_t *priv, uint32_t *dest, uint32_t src, uint32_t len) {
  return priv->ap->ops->mem_read_words(priv->ap->priv, dest, src, len);
}

static uint32_t cortexm_write_words(CORTEXM_PRIV_t *priv, uint32_t dest, const uint32_t *src, uint32_t len) {
  return priv->ap->ops->mem_write_words(priv->ap->priv, dest, src, len);
}

static uint32_t cortexm_pc_read(CORTEXM_PRIV_t *priv)
{
  cortexm_write_word(priv, CORTEXM_DCRSR, 0x0F);
  return cortexm_read_word(priv, CORTEXM_DCRDR);
}

static void cortexm_pc_write(CORTEXM_PRIV_t *priv, uint32_t val)
{
  cortexm_write_word(priv, CORTEXM_DCRDR, val);
  cortexm_write_word(priv, CORTEXM_DCRSR, CORTEXM_DCRSR_REGWnR | 0x0F);
}

static void cortexm_halt_request(CORTEXM_PRIV_t *priv)
{
  //allow_timeout == false
  cortexm_write_word(priv, CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_C_HALT | CORTEXM_DHCSR_C_DEBUGEN);
}

static uint32_t cortexm_halt_wait(CORTEXM_PRIV_t *priv)
{
  uint32_t dfsr;

  if(!(cortexm_read_word(priv, CORTEXM_DHCSR) & CORTEXM_DHCSR_S_HALT)) {
    return 0;
  }

  //allow_timeout == false

  // TODO: We need that???
  /* We've halted.  Let's find out why. */
  dfsr = cortexm_read_word(priv, CORTEXM_DFSR);
  /* write back to reset */
  cortexm_write_word(priv, CORTEXM_DFSR, dfsr);

  if ((dfsr & CORTEXM_DFSR_VCATCH) ) {//&& cortexm_fault_unwind(target)) {
    return SIGSEGV;
  }

  return SIGTRAP;
}

static void cortexm_halt_resume(CORTEXM_PRIV_t *priv)
{
  uint32_t dhcsr;

  dhcsr = CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_C_DEBUGEN;
  cortexm_write_word(priv, CORTEXM_DHCSR, dhcsr);
  //allow_timeout == true
}

static void cortexm_reset(CORTEXM_PRIV_t *priv)
{
  /* Read DHCSR here to clear S_RESET_ST bit before reset */
  cortexm_read_word(priv, CORTEXM_DHCSR);

  /* Request system reset from NVIC: SRST doesn't work correctly */
  /* This could be VECTRESET: 0x05FA0001 (reset only core)
   *          or SYSRESETREQ: 0x05FA0004 (system reset)
   */
  cortexm_write_word(priv, CORTEXM_AIRCR, CORTEXM_AIRCR_VECTKEY | CORTEXM_AIRCR_SYSRESETREQ);

  /* Poll for release from reset */
  while(cortexm_read_word(priv, CORTEXM_DHCSR) & CORTEXM_DHCSR_S_RESET_ST);

  /* Reset DFSR flags */
  cortexm_write_word(priv, CORTEXM_DFSR, CORTEXM_DFSR_RESETALL);
}

static uint32_t cortexm_check_error(CORTEXM_PRIV_t *priv)
{
  return priv->ap->ops->error_check(priv->ap->priv);
}

static void free_cortexm(CORTEXM_t *cortexm)
{
  cortexm->priv->ap->ops->priv_free(cortexm->priv->ap);
  vPortFree(cortexm->priv);
  vPortFree(cortexm);
}

static CORTEXM_OPS_t cortexm_ops = {
  .read_word = cortexm_read_word,
  .write_word = cortexm_write_word,
  .read_words = cortexm_read_words,
  .write_words = cortexm_write_words,
  .pc_read = cortexm_pc_read,
  .pc_write = cortexm_pc_write,
  .halt_request = cortexm_halt_request,
  .halt_wait = cortexm_halt_wait,
  .halt_resume = cortexm_halt_resume,
  .check_error = cortexm_check_error,
  .restart = cortexm_reset,

  .free = free_cortexm
};

int probe_cortexm(ADIv5_AP_t *ap)
{
  /* Should probe here to make sure it's Cortex-M3 */

  CORTEXM_t *cortexm = pvPortMalloc(sizeof(CORTEXM_t));
  CORTEXM_PRIV_t *priv = pvPortMalloc(sizeof(CORTEXM_PRIV_t));

  cortexm->ops = &cortexm_ops;
  cortexm->priv = priv;

  priv->ap = ap;
  // probe stm32

  if(stm32f4_probe(cortexm)){
    return 1;
  }

  if(stm32l4_probe(cortexm)){
    return 1;
  }

  // Non of the targets successful probed. Cleanup
  vPortFree(cortexm->priv);
  vPortFree(cortexm);

  return 0;
}
