/*
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
 *   -- Underling API changed to jtag_low_level module implemented in this project (RemoteProgrammer)
 *   -- Delete *post/prescan fields from jtag_dev_t. Now it jtag_dev_write_ir
 *      and jtag_dev_shift_dr calculate it in runtime.
 *   -- jtag_scan is splited into functions and limited to minimal functional part.
 */
#include "main.h"
#include "stm32f4xx_it.h"
#include "jtag/jtag_low_level.h"
#include "jtag/jtag_scan.h"
#include "adiv5/adiv5_jtag.h"

static jtag_dev_t devs[JTAG_MAX_DEVS];
static int num_of_devs;
static int current_dev;

static struct jtag_dev_descr_s {
  uint32_t idcode;
  uint32_t idmask;
  char *descr;
  int (*handler)(int dev_num);
} dev_descr[] = {
  {.idcode = 0x0BA00477, .idmask = 0x0FFF0FFF,
    .descr = "ARM Limited: ADIv5 JTAG-DP port.",
    .handler = adiv5_jtag_handler},
    /* Just for fun, unsupported */
  {.idcode = 0x3F0F0F0F, .idmask = 0xFFFFFFFF,
    .descr = "ST Microelectronics: STR730"},
  {.idcode = 0x06410041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, Medium density."},
  {.idcode = 0x06412041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, Low density."},
  {.idcode = 0x06414041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, High density."},
  {.idcode = 0x06416041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32L."},
  {.idcode = 0x06418041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, Connectivity Line."},
  {.idcode = 0x06420041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, Value Line."},
  {.idcode = 0x06428041, .idmask = 0x0FFFFFFF,
    .descr = "ST Microelectronics: STM32, Value Line, High density."},
  {.idcode = 0x06411041, .idmask = 0xFFFFFFFF,
    .descr = "ST Microelectronics: STM32F2xx."},
  {.idcode = 0x06413041 , .idmask = 0xFFFFFFFF,
    .descr = "ST Microelectronics: STM32F4xx."},
  {.idcode = 0x0BB11477 , .idmask = 0xFFFFFFFF,
    .descr = "NPX: LPC11C24."},
  {.idcode = 0x8940303F, .idmask = 0xFFFFFFFF, .descr = "ATMEL: ATMega16."},
  {.idcode = 0x0792603F, .idmask = 0xFFFFFFFF, .descr = "ATMEL: AT91SAM9261."},
  {.idcode = 0x20270013, .idmask = 0xFFFFFFFF, .descr = "Intel: i80386ex."},
  {.idcode = 0, .idmask = 0, .descr = "Unknown"},
};

static int get_ir_len_in_chain(jtag_dev_t *devs_table, int max_number_of_devs)
{
  int ir_len, num_of_devs = 0;

  jtag_go_to_idle();
  jtag_from_idle_to_shift_ir();
  // after reset, devices in chain should have 0b00...01 in IR

  if(!jtag_tdi(GPIO_PIN_SET, GPIO_PIN_RESET)) {
    // No device detected
    return num_of_devs;
  }
  do {
    num_of_devs++;
    if(num_of_devs > max_number_of_devs) {
      return -1;
    }
    ir_len = 1;
    // read zeros that are from current device ir
    while(!jtag_tdi(GPIO_PIN_SET, GPIO_PIN_RESET)) {
      ir_len++;
    }
    // we have one, this is begining of next device IR or end of chain
    devs_table->ir_len = ir_len;
    devs_table++;
  } while(!jtag_tdi(GPIO_PIN_SET, GPIO_PIN_RESET));

  //TODO: We pushed ones to IR, so all devices are in BYPASS mode. 
  //      We can check if we have num_of_devs * one bit register filed with 0.
  jtag_tms(GPIO_PIN_SET);    // from shift to exit
  jtag_from_exit_to_idle();

  return num_of_devs;
}

static int get_idcodes(jtag_dev_t *devs_table, int num_of_devs)
{
  int i;

  jtag_go_to_idle();
  jtag_from_idle_to_shift_dr();
  // after reset, devices in chain should have selected IDCODE DR
  // devices without IDCODE should have selected BYPASS DR
  
  for(i = 0; i < num_of_devs; i++) {
    if(!jtag_tdi(GPIO_PIN_SET, GPIO_PIN_RESET)){
      // 0 means BYPASS DR, skip this device
      devs_table->idcode = 0;
    } else {
      // IDCODE allways start with one and has 32 bits
      devs_table->idcode = jtag_tdin(31, (~0), GPIO_PIN_RESET);
      devs_table->idcode <<= 1;
      devs_table->idcode += 1;    // one that was read at begin of for-loop
    }
    printf("dev %d: 0x%lx\n", i, devs_table->idcode);
    devs_table++;
  }
  
  return 0;
}

static int init_tagrets_in_chain(jtag_dev_t *devs_table, int num_of_devs)
{
  int i, j;

  for(i = 0; i < num_of_devs; i++) {
    for(j = 0; dev_descr[j].idcode; j++) {
      if((devs_table->idcode & dev_descr[j].idmask) == dev_descr[j].idcode) {
        devs_table->descr = dev_descr[j].descr;
        if(dev_descr[j].handler) {
          dev_descr[j].handler(i);
        }
      }
    }
    devs_table++;
  }
  return 0;
}

/* Scan JTAG chain for devices, store IR length and IDCODE (if present).
 * Reset TAP state machine.
 * Select Shift-IR state.
 * Each device is assumed to shift out IR at 0x01. (this may not always be true)
 * Shift in ones until we read two consecutive ones, then we have shifted out the
 * 	IRs of all devices.
 *
 * After this process all the IRs are loaded with the BYPASS command.
 * Select Shift-DR state.
 * Shift in ones and count zeros shifted out. Should be one for each device.
 * Check this against device count obtained by IR scan above.
 *
 * Reset the TAP state machine again. This should load all IRs with IDCODE.
 * For each device, shift out one bit. If this is zero IDCODE isn't present,
 *	continue to next device. If this is one shift out the remaining 31 bits
 *	of the IDCODE register.
 */

void jtag_scan()
{
  // TODO: Preform JTAG reset sequence (50 clk with tms 1 ore something like that)
  //       Run throught the SWD to JTAG sequence for the case where an attached
  //       SWJ-DP is in SW-DP mode. 
  num_of_devs = get_ir_len_in_chain(devs, JTAG_MAX_DEVS);
  get_idcodes(devs, num_of_devs);
  init_tagrets_in_chain(devs, num_of_devs);
}


/*
 * Debug JTAG connection.
 * This function forever in loop shifts out 32 bits throught register and print
 * it out. TDI is set on 1. After 32 bits it is changed to 0, and so on.
 */
void jtag_test()
{
  uint32_t tdo;
  GPIO_PinState tdi = GPIO_PIN_RESET;

  jtag_go_to_idle();
  jtag_from_idle_to_shift_dr();
  while(1){
    tdo = 0;
    tdi = tdi == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET;
    uint32_t add = 1 << 31;
    for (int i = 0; i < 32; i++) {
      tdo >>= 1;
      tdo += jtag_tdi(tdi, GPIO_PIN_RESET) ? add : 0;
    }
    printf("0x%lx\n", tdo);
  }
  jtag_tdi(tdo, GPIO_PIN_RESET);
}

void jtag_select_dev(int dev)
{
  current_dev = dev;
}

void jtag_dev_write_ir(uint32_t ir)
{
  int i;

  jtag_from_idle_to_shift_ir();
  for(i = 0; i < current_dev; i++) {
    jtag_tdin(devs[i].ir_len, BYPASS_IR, GPIO_PIN_RESET);
  }
  jtag_tdin(devs[current_dev].ir_len, ir, ((current_dev == num_of_devs - 1) ? GPIO_PIN_SET : GPIO_PIN_RESET));
  for(i = current_dev + 1; i < num_of_devs; i++) {
    jtag_tdin(devs[i].ir_len, BYPASS_IR, ((i == num_of_devs - 1) ? GPIO_PIN_SET : GPIO_PIN_RESET));
  }
  jtag_from_exit_to_idle();
}

void jtag_dev_shift_dr(uint_jtag_transfer_t *din, uint_jtag_transfer_t *dout, int n)
{
  int i, read_size;

  jtag_from_idle_to_shift_dr();
  for(i = 0; i < current_dev; i++) {
    jtag_tdi(GPIO_PIN_SET, GPIO_PIN_RESET);
  }

  // TODO: check if we do correct operation for tables
  while(n){
    if(n > sizeof(uint_jtag_transfer_t)) {
      read_size = sizeof(uint_jtag_transfer_t);
    } else {
      read_size = n;
    }
    n -= read_size;
    *dout = jtag_tdin(read_size, *din, (((current_dev == num_of_devs - 1) && !n) ? GPIO_PIN_SET : GPIO_PIN_RESET));
    dout++;
    din++;
  }

  for(i = current_dev + 1; i < num_of_devs; i++) {
    jtag_tdi(GPIO_PIN_SET, ((i == num_of_devs - 1) ? GPIO_PIN_SET : GPIO_PIN_RESET));
  }
  jtag_from_exit_to_idle();
}
