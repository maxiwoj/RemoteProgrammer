#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "target.h"
#include "fatfs.h"
#include "object_target.h"

TARGET_t target_list = {0, 0, 0};

// TODO: Add wakaama registration here or wakaama will travel target_list leater
void register_target(void *priv, TARGET_OPS_t *ops, char *name){
  TARGET_t *new_target = pvPortMalloc(sizeof(TARGET_t));
  new_target->priv = priv;
  new_target->ops = ops;
  new_target->name = name;
  new_target->next = target_list.next;
  target_list.next = new_target;
  printf("Registered target \"%s\"\n", name);
}

void flash_target_task(void *object) {
	target_instance_t * target_object = (target_instance_t *) object;

	FIL file;
	int result;

	result = usb_open_file(target_object->binary_filename, &file, FA_READ);
	if (result != 0) {
		target_object->flash_error = USB_FS_ERROR;
		target_object->flash_state = FLASH_ERROR;
		vTaskDelete(NULL);
	}

	result = target_object->target->ops->flash_target(target_object->target->priv, &file, &target_object->flash_progress);
	if (result != 0) {
		target_object->flash_error = result;
		target_object->flash_state = FLASH_ERROR;
	} else {
		target_object->flash_progress = 100;
		target_object->flash_state = FLASH_COMPLETED;
	}
	
	result = usb_close_file(&file);
	if (result != 0) {
		target_object->flash_error = USB_FS_ERROR;
		vTaskDelete(NULL);
	}
	vTaskDelete(NULL);
}

void reset_target_task(void *object) {
	target_instance_t * target_object = (target_instance_t *) object;
	target_object->target->ops->reset_target(target_object->target->priv);
	vTaskDelete(NULL);	
}