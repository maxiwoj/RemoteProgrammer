#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "adiv5/adiv5.h"
#include "jtag/jtag_scan.h"

static void adiv5_ap_priv_free(ADIv5_AP_t *this)
{
  this->priv->dp->ap_count--;
  if(!this->priv->dp->ap_count) {
    this->priv->dp->ops->priv_free(this->priv->dp);
  }
  vPortFree(this->priv);
  vPortFree(this);
}

inline static void adiv5_select_ap(ADIv5_AP_PRIV_t *ap, uint8_t addr)
{
  ap->dp->ops->dp_write(ap->dp->priv, ADIV5_DP_SELECT,
      ((uint32_t)ap->apsel << 24)|(addr & 0xF0));
}

inline static void adiv5_dp_write_ap(ADIv5_DP_t *dp, uint8_t addr, uint32_t value)
{
  dp->ops->low_access(dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, addr, value);
}

inline static uint32_t adiv5_dp_read_ap(ADIv5_DP_t *dp, uint8_t addr)
{
  uint32_t ret;

  dp->ops->low_access(dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, addr, 0);
  ret = dp->ops->low_access(dp->priv, ADIV5_LOW_DP, ADIV5_LOW_READ, ADIV5_DP_RDBUFF, 0);

  return ret;
}

static void adiv5_ap_write(ADIv5_AP_PRIV_t *ap, uint8_t addr, uint32_t value)
{
  adiv5_select_ap(ap, addr);
	
  adiv5_dp_write_ap(ap->dp, addr, value);
}

static uint32_t adiv5_ap_read(ADIv5_AP_PRIV_t *ap, uint8_t addr)
{
	uint32_t ret;
  adiv5_select_ap(ap, addr);
	ret = adiv5_dp_read_ap(ap->dp, addr);
	return ret;
}

ADIv5_AP_OPS_t adiv5_ap_ops = {
  adiv5_ap_priv_free
};

uint16_t adiv5_init(ADIv5_DP_t *dp_low_level)
{
  uint32_t ctrlstat;

  ctrlstat = dp_low_level->ops->dp_read(dp_low_level->priv, ADIV5_DP_CTRLSTAT);

  /* Write request for system and debug power up */
  dp_low_level->ops->dp_write(dp_low_level->priv, ADIV5_DP_CTRLSTAT,
       ctrlstat |= ADIV5_DP_CTRLSTAT_CSYSPWRUPREQ | ADIV5_DP_CTRLSTAT_CDBGPWRUPREQ);
  /* Wait for acknowledge */
  while(((ctrlstat = dp_low_level->ops->dp_read(dp_low_level->priv, ADIV5_DP_CTRLSTAT)) & 
                  (ADIV5_DP_CTRLSTAT_CSYSPWRUPACK | ADIV5_DP_CTRLSTAT_CDBGPWRUPACK)) !=
                  (ADIV5_DP_CTRLSTAT_CSYSPWRUPACK | ADIV5_DP_CTRLSTAT_CDBGPWRUPACK));

#if 0
  /* This AP reset logic is described in ADIv5, but fails to work
   * correctly on STM32.  CDBGRSTACK is never asserted, and we
   * just wait forever.
   */

  /* Write request for debug reset */
  adiv5_dp_write(dp, ADIV5_DP_CTRLSTAT,
    ctrlstat |= ADIV5_DP_CTRLSTAT_CDBGRSTREQ);
  /* Wait for acknowledge */
  while(!((ctrlstat = adiv5_dp_read(dp, ADIV5_DP_CTRLSTAT)) & ADIV5_DP_CTRLSTAT_CDBGRSTACK));

  /* Write request for debug reset release */
  adiv5_dp_write(dp, ADIV5_DP_CTRLSTAT,
  ctrlstat &= ~ADIV5_DP_CTRLSTAT_CDBGRSTREQ);
  /* Wait for acknowledge */
  while(adiv5_dp_read(dp, ADIV5_DP_CTRLSTAT) & ADIV5_DP_CTRLSTAT_CDBGRSTACK);
#endif

  /* Probe for APs on this DP */
  for(int i = 0; i < 256; i++) {
    ADIv5_AP_PRIV_t *ap_priv = pvPortMalloc(sizeof(ADIv5_AP_PRIV_t));
    ap_priv->dp = dp_low_level;
    ap_priv->apsel = i;
    ap_priv->idr = adiv5_ap_read(ap_priv, ADIV5_AP_IDR);
    if(!ap_priv->idr) {
      /* IDR Invalid - Should we not continue here? */
      vPortFree(ap_priv);
      break;
    }

    dp_low_level->ap_count++;

    ADIv5_AP_t *ap = pvPortMalloc(sizeof(ADIv5_AP_t));
    ap->ops = &adiv5_ap_ops;
    ap->priv = ap_priv;
    ap->priv->cfg = adiv5_ap_read(ap->priv, ADIV5_AP_CFG);
    ap->priv->base = adiv5_ap_read(ap->priv, ADIV5_AP_BASE);
    ap->priv->csw = adiv5_ap_read(ap->priv, ADIV5_AP_CSW) & ~(ADIV5_AP_CSW_SIZE_MASK | ADIV5_AP_CSW_ADDRINC_MASK);

    /* Currently only CortexM is supported */
    
  }

  return dp_low_level->ap_count;
}

