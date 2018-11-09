#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "jtag/jtag_scan.h"
#include "jtag/jtag_low_level.h"
#include "adiv5/adiv5.h"
#include "adiv5/adiv5_jtag.h"

#define JTAGDP_ACK_OK	0x02
#define JTAGDP_ACK_WAIT	0x01

/* 35-bit registers that control the ADIv5 DP */
#define IR_ABORT	0x8
#define IR_DPACC	0xA
#define IR_APACC	0xB

static uint32_t adiv5_jtag_low_access(void *priv, uint8_t APnDP, uint8_t RnW,
              uint8_t addr, uint32_t value)
{
  uint64_t request, response;
  uint8_t ack;

  request = ((uint64_t)value << 3) | ((addr >> 1) & 0x06) | (RnW?1:0);

  jtag_select_dev(((ADIv5_DP_JTAG_t *)priv)->jtag_dev_num);
  jtag_dev_write_ir(APnDP?IR_APACC:IR_DPACC);

	int tries = 1000;
	do {
    jtag_dev_shift_dr((uint_jtag_transfer_t*)&request, (uint_jtag_transfer_t*)&response, 35);
    ack = response & 0x07;
	} while(--tries && (ack == JTAGDP_ACK_WAIT));

  /*if (dp->allow_timeout && (ack == JTAGDP_ACK_WAIT))
    return 0;*/

	if(ack != JTAGDP_ACK_OK) {
    /* Fatal error if invalid ACK response */
    // TODO: do something useful
    printf("ERROR: ack != JTAGDP_ACK_OK\n");
	}

	return (uint32_t)(response >> 3);
}

static void adiv5_jtag_write(void *priv, uint8_t addr, uint32_t value)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_WRITE, addr, value);
}

static uint32_t adiv5_jtag_read(void *priv, uint8_t addr)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ, addr, 0);
	return adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ,
					ADIV5_DP_RDBUFF, 0);
}

static uint32_t adiv5_jtag_error(void *priv)
{
	adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_READ,
				ADIV5_DP_CTRLSTAT, 0);
	return adiv5_jtag_low_access(priv, ADIV5_LOW_DP, ADIV5_LOW_WRITE,
				ADIV5_DP_CTRLSTAT, 0xF0000032) & 0x32;
}

static void adiv5_jtag_priv_free(ADIv5_DP_t *this)
{
  if(this->ap_count) {
    printf("ERROR: ap_count = %d while freeing adiv5_jtag!\n", this->ap_count);
  }
  vPortFree(this->priv);
  vPortFree(this);
}

static ADIv5_DP_OPS_t adiv5_jtag_dp_ops = {
  adiv5_jtag_write,
  adiv5_jtag_read,
  adiv5_jtag_error,
  adiv5_jtag_low_access,
  adiv5_jtag_priv_free
};

int adiv5_jtag_handler(int dev_num)
{
  uint16_t dev_count;
  ADIv5_DP_t *adiv5_jtag_low_level = pvPortMalloc(sizeof(ADIv5_DP_t));

  adiv5_jtag_low_level->priv = pvPortMalloc(sizeof(ADIv5_DP_JTAG_t));
  ((ADIv5_DP_JTAG_t*) adiv5_jtag_low_level->priv)->jtag_dev_num = dev_num;
  adiv5_jtag_low_level->ops = &adiv5_jtag_dp_ops;
  adiv5_jtag_low_level->ap_count = 0;

  printf("ADIv5 init start\n");
  dev_count = adiv5_init(adiv5_jtag_low_level);

  if(!dev_count) {
    // non of AP has been probed successful, free low level adiv5
    adiv5_jtag_priv_free(adiv5_jtag_low_level);
  }

  return dev_count;
}

