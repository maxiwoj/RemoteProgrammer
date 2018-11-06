#ifndef __TARGET_H
#define __TARGET_H

#include "fatfs.h"

// TODO: define real functions with real arguments
typedef struct TARGET_OPS_s {
  int (*flash_target)(void *priv, FIL *file);

  void (*free_priv)(void *priv);
} TARGET_OPS_t;

typedef struct TARGET_s {
  TARGET_OPS_t *ops;
  void *priv;
  struct TARGET_s *next;
} TARGET_t;

void register_target(void *priv, TARGET_OPS_t *ops);

// Head of targets list
extern TARGET_t target_list;

#endif //__TARGET_H