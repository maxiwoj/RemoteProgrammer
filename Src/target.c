#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "target.h"

TARGET_t target_list = {0, 0, 0};

// TODO: Add wakaama registration here or wakaama will travel target_list leater
void register_target(void *priv, TARGET_OPS_t *ops){
  TARGET_t *new_target = pvPortMalloc(sizeof(TARGET_t));
  new_target->priv = priv;
  new_target->ops = ops;
  new_target->next = target_list.next;
  target_list.next = new_target;
}