#ifndef __TARGET_H
#define __TARGET_H

#include "fatfs.h"

// Flash States
#define NO_FLASH_DATA 0
#define FLASH_IN_PROGRESS 1
#define FLASH_ERROR 2
#define FLASH_COMPLETED 3

#define USB_FS_ERROR 1

// TODO: define real functions with real arguments
typedef struct TARGET_OPS_s {
  int (*flash_target)(void *priv, FIL *file);
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