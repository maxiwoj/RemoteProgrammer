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

  .text
  .syntax unified
  .cpu cortex-m4
  .thumb


#define STM32_FLASH_CR_OFFSET	0x14	/* offset of CR register in FLASH struct */
#define STM32_FLASH_SR_OFFSET	0x10	/* offset of SR register in FLASH struct */

_start:
  ldr r0, _flashbase
  ldr r1, _addr
  adr r2, _data
  ldr r3, _size
  ldr r5, _cr
_next:
  cbz r3, _done
  str r5, [r0, #STM32_FLASH_CR_OFFSET]
  ldr r4, [r2]
  str r4, [r1]
_wait:
  ldr  r4, [r0, #STM32_FLASH_SR_OFFSET]
  tst  r6, #0x10000   /* BSY (bit16) == 1 => operation in progress */
  bne _wait
  tst r6, #0xfa       /* PGSERR | PGPERR | PGAERR | WRPERR | PROGERR*/
  bne _done           /* fail... */

  subs r3, #4
  adds r1, #4
  adds r2, #4
  b _next
_done:
  bkpt

_cr:
  .word 0x00000001 /*(Value to write to FLASH_CR) PG*/
_flashbase:
  .word 0x40022000 /* (FPEC_BASE) */
_addr:
  .word 0x0
_size:
  .word 0x0
_data:
  .word 0x0
