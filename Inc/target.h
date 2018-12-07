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

#ifndef __TARGET_H
#define __TARGET_H

#include "fatfs.h"

// Flash States
#define NO_FLASH_DATA 0
#define FLASH_IN_PROGRESS 1
#define FLASH_ERROR 2
#define FLASH_COMPLETED 3

// Flash Error status
// 0x0 -- 0x1000 reserved for target
#define USB_FS_ERROR 0x1001

// TODO: define real functions with real arguments
typedef struct TARGET_OPS_s {
  int (*flash_target)(void *priv, FIL *file, int *progress);
  void (*reset_target)(void *priv);

  void (*free_priv)(void *priv);
} TARGET_OPS_t;

typedef struct TARGET_s {
  TARGET_OPS_t *ops;
  void *priv;
  char *name;
  struct TARGET_s *next;
} TARGET_t;

void register_target(void *priv, TARGET_OPS_t *ops, char *name);
void reset_target_task(void *object);
void flash_target_task(void *object);

// Head of targets list
extern TARGET_t target_list;

#endif //__TARGET_H