#include "main.h"
#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "assert.h"
#include "jtag/jtag_low_level.h"

void jtag_tclk_up()
{
  HAL_GPIO_WritePin(JTAG_TCLK_GPIO_Port, JTAG_TCLK_Pin, GPIO_PIN_SET);
  osDelay(TCKWAIT);
}

void jtag_tclk_down()
{
  HAL_GPIO_WritePin(JTAG_TCLK_GPIO_Port, JTAG_TCLK_Pin, GPIO_PIN_RESET);
  osDelay(TCKWAIT);
}

void jtag_tclk(void)
{
  jtag_tclk_up();
  jtag_tclk_down();
}

// set TMS to value, cycle TCK
void jtag_tms(GPIO_PinState state)
{
  HAL_GPIO_WritePin(JTAG_TMS_GPIO_Port, JTAG_TMS_Pin, state);
  jtag_tclk();
}

// cycle TCK, set TDI to value, sample TDO
GPIO_PinState jtag_tdi(GPIO_PinState tdi_val, GPIO_PinState tms_val)
{
  GPIO_PinState tdo;
  HAL_GPIO_WritePin(JTAG_TDI_GPIO_Port, JTAG_TDI_Pin, tdi_val);
  HAL_GPIO_WritePin(JTAG_TMS_GPIO_Port, JTAG_TMS_Pin, tms_val);
  jtag_tclk_up();
  tdo = HAL_GPIO_ReadPin(JTAG_TDO_GPIO_Port, JTAG_TDO_Pin) == GPIO_PIN_SET ? 1 : 0;
  jtag_tclk_down();
  return tdo;
}

// multi-bit version of tdi()
// shift bits to TDI from right most
// first bit from TDO is n-th bit in res 
uint_jtag_transfer_t jtag_tdin(uint8_t n, uint_jtag_transfer_t bits, GPIO_PinState last_tms)
{
  uint_jtag_transfer_t res = 0, mask;

  assert(n <= sizeof(uint_jtag_transfer_t));

  // mask == 0b00100..n..0
  for(mask = 1; n; mask <<= 1, n--) {
    if (jtag_tdi(((bits & mask) ? GPIO_PIN_SET : GPIO_PIN_RESET), ((n - 1) ? GPIO_PIN_RESET : last_tms))) {
      // if TDO == 1 then add 1 to res
      res |= mask;
    }
    // get next bit to send
    bits >>= 1;
  }

  return res;
}

void jtag_from_idle_to_shift_ir()
{
  // jtag_tms: 1, 1, 0, 0
  jtag_tms(1);
  jtag_tms(1);
  jtag_tms(0);
  jtag_tms(0);
}

void jtag_from_idle_to_shift_dr()
{
  // jtag_tms: 1, 0, 0
  jtag_tms(1);
  jtag_tms(0);
  jtag_tms(0);
}

void jtag_from_exit_to_idle()
{
  // jtag_tms 1, 0
  jtag_tms(1);
  jtag_tms(0);
}

void jtag_go_to_idle()
{
  // jtag_tms 1, 1, 1, 1, 1, 0
  for (int i = 0; i < 5; i++){
    jtag_tms(1);
  }
  jtag_tms(0);
}
