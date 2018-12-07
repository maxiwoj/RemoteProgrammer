/* Copyright 2018 Maksymilian Wojczuk and Tomasz Michalec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "main.h"
#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "assert.h"
#include "jtag/jtag_low_level.h"

uint32_t getUs(void) {
  static uint32_t usTicks = 0;
  if (usTicks == 0) {
    usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
  }
  register uint32_t ms, cycle_cnt;
  do {
    ms = HAL_GetTick();
    cycle_cnt = SysTick->VAL;
  } while (ms != HAL_GetTick());
  return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delayUs(uint16_t micros)
{
  uint32_t start = getUs();

  while (getUs()-start < (uint32_t) micros) {
    asm("nop");
  }
}

#if TCKWAIT == 0
// current implementation of JTAG low level routines allow us to get ~1MHz if there
// is no additional delay.
static inline void short_delay(){
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
}
#endif

void jtag_tclk_up()
{
#if TCKWAIT == 0
  short_delay();
#else
  delayUs(TCKWAIT);
#endif
  HAL_GPIO_WritePin(JTAG_TCLK_GPIO_Port, JTAG_TCLK_Pin, GPIO_PIN_SET);
}

void jtag_tclk_down()
{
#if TCKWAIT == 0
  short_delay();
#else
  delayUs(TCKWAIT);
#endif
  HAL_GPIO_WritePin(JTAG_TCLK_GPIO_Port, JTAG_TCLK_Pin, GPIO_PIN_RESET);
}

void jtag_tclk(void)
{
  jtag_tclk_up();
  jtag_tclk_down();
}

// set TMS to value, cycle TCK
void jtag_tms(GPIO_PinState state)
{
  jtag_tclk_down();
  HAL_GPIO_WritePin(JTAG_TMS_GPIO_Port, JTAG_TMS_Pin, state);
  jtag_tclk_up();
}

// cycle TCK, set TDI to value, sample TDO
int jtag_tdi(GPIO_PinState tdi_val, GPIO_PinState tms_val)
{
  int tdo;
  jtag_tclk_down();
  HAL_GPIO_WritePin(JTAG_TDI_GPIO_Port, JTAG_TDI_Pin, tdi_val);
  HAL_GPIO_WritePin(JTAG_TMS_GPIO_Port, JTAG_TMS_Pin, tms_val);
  jtag_tclk_up();
  tdo = HAL_GPIO_ReadPin(JTAG_TDO_GPIO_Port, JTAG_TDO_Pin) == GPIO_PIN_SET ? 1 : 0;
  return tdo;
}

// multi-bit version of tdi()
// shift bits to TDI from right most
// first bit from TDO is n-th bit in res 
uint_jtag_transfer_t jtag_tdin(uint8_t n, uint_jtag_transfer_t bits, GPIO_PinState last_tms)
{
  uint_jtag_transfer_t res = 0, mask;

  assert(n <= SIZEOF_IN_BITS(uint_jtag_transfer_t));

  // mask == 0b00100..n..0
  for(mask = 1; n; mask <<= 1, n--) {
    if (jtag_tdi(((bits & mask) ? GPIO_PIN_SET : GPIO_PIN_RESET), ((n - 1) ? GPIO_PIN_RESET : last_tms))) {
      // if TDO == 1 then add 1 to res
      res |= mask;
    }
  }

  return res;
}

void jtag_from_idle_to_shift_ir()
{
  // jtag_tms: 1, 1, 0, 0
  jtag_tms(0);
  jtag_tms(1);
  jtag_tms(1);
  jtag_tms(0);
  jtag_tms(0);
}

void jtag_from_idle_to_shift_dr()
{
  // jtag_tms: 1, 0, 0
  jtag_tms(0);
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
