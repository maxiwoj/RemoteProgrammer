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

  int start_time = xTaskGetTickCount();

	result = usb_open_file(target_object->binary_filename, &file, FA_READ);
	if (result != 0) {
		target_object->flash_error = USB_FS_ERROR;
		target_object->flash_state = FLASH_ERROR;
		JTAG_BUSY = 0;
		USB_BUSY = 0;
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
	
  printf("Flash request ended in %d ms\n", xTaskGetTickCount() - start_time);

	result = usb_close_file(&file);
	JTAG_BUSY = 0;
	USB_BUSY = 0;
	if (result != 0) {
		target_object->flash_error = USB_FS_ERROR;
		vTaskDelete(NULL);
	}
	vTaskDelete(NULL);
}

void reset_target_task(void *object) {
  int start_time = xTaskGetTickCount();
	target_instance_t * target_object = (target_instance_t *) object;
	target_object->target->ops->reset_target(target_object->target->priv);
  printf("Reset request ended in %d ms\n", xTaskGetTickCount() - start_time);
	JTAG_BUSY = 0;
	vTaskDelete(NULL);	
}