#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "adiv5/adiv5.h"
#include "jtag/jtag_scan.h"
#include "cortexm/cortexm.h"

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

uint32_t ap_check_error(ADIv5_AP_PRIV_t *ap) {
  return ap->dp->ops->error(ap->dp->priv);
}

uint32_t ap_mem_read_words(ADIv5_AP_PRIV_t *ap, uint32_t *dest, uint32_t src, uint32_t len)
{
	uint32_t osrc = src;
  len >>= 2;

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_WORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, src);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);

  while(--len) {
    //*dest++ = ...
    *dest = ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);
    dest++;
    src += 4;

    /* Check for 10 bit address overflow */
    if ((src ^ osrc) & 0xfffffc00) {
      osrc = src;
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, src);
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);
    }
  }
  *dest = ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_DP, ADIV5_LOW_READ, ADIV5_DP_RDBUFF, 0);
  dest++;

  return 0;
}

uint32_t ap_mem_read_bytes(ADIv5_AP_PRIV_t *ap, uint8_t *dest, uint32_t src, uint32_t len)
{
  uint32_t tmp;
	uint32_t osrc = src;
  len >>= 2;

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_BYTE | ADIV5_AP_CSW_ADDRINC_SINGLE);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, src);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);

  while(--len) {
    //*dest++ = ...
    tmp = ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);
    *dest = (tmp >> ((src & 0x3) << 3) & 0xFF);
    dest++;
    src++;

    /* Check for 10 bit address overflow */
    if ((src ^ osrc) & 0xfffffc00) {
      osrc = src;
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, src);
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);
    }
  }
  tmp = ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_DP, ADIV5_LOW_READ, ADIV5_AP_DRW, 0);
  *dest = (tmp >> ((src & 0x3) << 3) & 0xFF);
  dest++;

  return 0;
}

uint32_t ap_mem_write_words(ADIv5_AP_PRIV_t *ap, uint32_t dest, const uint32_t *src, uint32_t len)
{
	uint32_t odest = dest;
  len >>= 2;

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_WORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, dest);

  while(len--) {
    ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_DRW, *src);
    src++;
    dest += 4;

    /* Check for 10 bit address overflow */
    if ((dest ^ odest) & 0xfffffc00) {
      odest = dest;
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, dest);
    }
  }

  return 0;
}

uint32_t ap_mem_write_bytes(ADIv5_AP_PRIV_t *ap, uint32_t dest, const uint8_t *src, uint32_t len)
{
  uint32_t tmp;
	uint32_t odest = dest;

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_BYTE | ADIV5_AP_CSW_ADDRINC_SINGLE);
  ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, dest);

  while(len--) {
    tmp = (uint32_t)*src << ((dest & 0x3) << 3);
    ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_DRW, tmp);
    src++;
    dest++;

    /* Check for 10 bit address overflow */
    if ((dest ^ odest) & 0xfffffc00) {
      odest = dest;
      ap->dp->ops->low_access(ap->dp->priv, ADIV5_LOW_AP, ADIV5_LOW_WRITE, ADIV5_AP_TAR, dest);
    }
  }

  return 0;
}

static uint32_t ap_mem_read_word(ADIv5_AP_PRIV_t *ap, uint32_t addr)
{
  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_WORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  adiv5_ap_write(ap, ADIV5_AP_TAR, addr);
  return adiv5_ap_read(ap, ADIV5_AP_DRW);
}

static void ap_mem_write_word(ADIv5_AP_PRIV_t *ap, uint32_t addr, uint32_t value)
{
  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_WORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  adiv5_ap_write(ap, ADIV5_AP_TAR, addr);
  adiv5_ap_write(ap, ADIV5_AP_DRW, value);
}

uint16_t ap_mem_read_halfword(ADIv5_AP_PRIV_t *ap, uint32_t addr)
{
  uint32_t v;

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_HALFWORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  adiv5_ap_write(ap, ADIV5_AP_TAR, addr);
  v = adiv5_ap_read(ap, ADIV5_AP_DRW);
  if (addr & 2) {
    return v >> 16;
  } else {
    return v & 0xFFFF;
  }
}

void ap_mem_write_halfword(ADIv5_AP_PRIV_t *ap, uint32_t addr, uint16_t value)
{
  uint32_t v = value;
  if (addr & 2) {
    v <<= 16;
  }

  adiv5_ap_write(ap, ADIV5_AP_CSW, ap->csw | ADIV5_AP_CSW_SIZE_HALFWORD | ADIV5_AP_CSW_ADDRINC_SINGLE);
  adiv5_ap_write(ap, ADIV5_AP_TAR, addr);
  adiv5_ap_write(ap, ADIV5_AP_DRW, v);
}


ADIv5_AP_OPS_t adiv5_ap_ops = {
  ap_mem_read_word,
  ap_mem_write_word,
  ap_mem_read_words,
  ap_mem_write_words,
  ap_check_error,

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
    probe_cortexm(ap);
  }

  return dp_low_level->ap_count;
}

