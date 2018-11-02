#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "adiv5/adiv5.h"
#include "cortexm/cortexm.h"

int probe_cortexm(ADIv5_AP_t *ap)
{
  /* Should probe here to make sure it's Cortex-M3 */

  // probe stm32

  return 1;
}

uint32_t cortexm_pc_read(CORTEXM_PRIV_t *priv)
{
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DCRSR, 0x0F);
  return priv->ap->ops->mem_read_word(priv->ap->priv, CORTEXM_DCRDR);
}

void cortexm_pc_write(CORTEXM_PRIV_t *priv, uint32_t val)
{
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DCRDR, val);
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DCRSR, CORTEXM_DCRSR_REGWnR | 0x0F);
}

void cortexm_halt_request(CORTEXM_PRIV_t *priv)
{
  //allow_timeout == false
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_C_HALT | CORTEXM_DHCSR_C_DEBUGEN);
}

uint32_t cortexm_halt_wait(CORTEXM_PRIV_t *priv)
{
  uint32_t dfsr;

  if(!(priv->ap->ops->mem_read_word(priv->ap->priv, CORTEXM_DHCSR) & CORTEXM_DHCSR_S_HALT)) {
    return 0;
  }

  //allow_timeout == false

  // TODO: We need that???
  /* We've halted.  Let's find out why. */
  dfsr = priv->ap->ops->mem_read_word(priv->ap->priv, CORTEXM_DFSR);
	/* write back to reset */
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DFSR, dfsr);

  if ((dfsr & CORTEXM_DFSR_VCATCH) ) {//&& cortexm_fault_unwind(target)) {
    return SIGSEGV;
  }

  return SIGTRAP;
}

void cortexm_halt_resume(CORTEXM_PRIV_t *priv)
{
  uint32_t dhcsr;

  dhcsr = CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_C_DEBUGEN;
  priv->ap->ops->mem_write_word(priv->ap->priv, CORTEXM_DHCSR, dhcsr);
  //allow_timeout == true
}
