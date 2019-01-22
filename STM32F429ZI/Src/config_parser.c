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

#include "config_parser.h"
#include "default_config.h"
#include "fatfs.h"
#include "cmsis_os.h"
#include "communication/term_io.h"
#include "communication/wakaama_client/objects/object_target.h"
#include "communication/binary_download.h"

#define IF_MATCH(key, name) if(strncmp((key), (name), sizeof((name)) - 1) == 0)

config_t configuration = {
  .device_name = NULL,
  .server_uri = NULL,
  .local_port = -1,
  .wakaama_short_id = -1,
  .wakaama_company = NULL,
  .wakaama_model_number = NULL,
  .config_uri = NULL
};

static void copy_value(char **dst, const char *value, int value_len) {
  if(*dst != NULL) {
    vPortFree(*dst);
  }
  *dst = pvPortMalloc(value_len + 1);
  strcpy(*dst, value);
}

static void fill_configuration_with_default_value() {
  if(configuration.device_name == NULL) {
    copy_value(&(configuration.device_name), DEVICE_NAME, sizeof(DEVICE_NAME));
  }
  if(configuration.server_uri == NULL) {
    copy_value(&(configuration.server_uri), SERVER_URI, sizeof(SERVER_URI));
  }
  if(configuration.local_port == -1){
    configuration.local_port = LOCAL_PORT;
  }
  if(configuration.wakaama_short_id == -1){
    configuration.wakaama_short_id = WAKAAMA_SHORT_ID;
  }
  if(configuration.wakaama_company == NULL) {
    copy_value(&(configuration.wakaama_company), WAKAAMA_COMPANY, sizeof(WAKAAMA_COMPANY));
  }
  if(configuration.wakaama_model_number == NULL) {
    copy_value(&(configuration.wakaama_model_number), WAKAAMA_MODEL_NUMBER, sizeof(WAKAAMA_MODEL_NUMBER));
  }
}

int parse_config() {
  FIL config_file;
  const char *config_name = CONFIG_FILE_NAME;
  int config_to_parse = 1;
  char *buf = pvPortMalloc(MAX_LINE_LENGTH);
  char *key;
  char *value;
  int value_len;
  target_instance_t targetP;

  buf[MAX_LINE_LENGTH - 1] = 'x';

  if(configuration.device_name != NULL) {
    vPortFree(configuration.device_name);
    configuration.device_name = NULL;
  }
  if(configuration.server_uri != NULL) {
    vPortFree(configuration.server_uri);
    configuration.server_uri = NULL;
  }
  configuration.local_port = -1;
  configuration.wakaama_short_id = -1;
  if(configuration.wakaama_company != NULL) {
    vPortFree(configuration.wakaama_company);
    configuration.wakaama_company = NULL;
  }
  if(configuration.wakaama_model_number != NULL) {
    vPortFree(configuration.wakaama_model_number);
    configuration.wakaama_model_number = NULL;
  }
  if(configuration.config_uri != NULL) {
    vPortFree(configuration.config_uri);
    configuration.config_uri = NULL;
  }

  while(config_to_parse) {
    config_to_parse = 0;

    if(usb_open_file(config_name, &config_file, FA_READ) != 0) {
      vPortFree(buf);
      printf("%s: usb_open_file error\n", __FUNCTION__);
      fill_configuration_with_default_value();
      return -1;
    }

    do {
      if(f_gets(buf, MAX_LINE_LENGTH, &config_file) == NULL) {
        usb_close_file(&config_file);
        printf("%s: f_gets error\n", __FUNCTION__);
        break;
      }

      // Line exceded max line length, skip it
      if(buf[MAX_LINE_LENGTH - 1] != 'x') {
        buf[MAX_LINE_LENGTH - 1] = 'x';
        printf("%s: line to long, skip\n", __FUNCTION__);
        continue;
      }

      // Skip comment line
      if(*buf == COMMENT_SIGN) {
        continue;
      }

      key = buf;
      while(*key == ' ') {
        key++;
      }

      value = key;
      while(*value != '=' && *value != '\0') {
        value++;
      }
      // Skip line with wrong value
      if(*value == '\0') {
        continue;
      }
      // Skip '='
      value++;

      value_len = strlen(value);
      while(value[value_len - 1] == '\r' || value[value_len - 1] == '\n') {
        value[value_len - 1] = '\0';
        value_len--;
      }

      IF_MATCH(key, FN_DEVICE_NAME) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_DEVICE_NAME, value);
        copy_value(&(configuration.device_name), value, value_len);
      }
      IF_MATCH(key, FN_LWM2M_SERVER_URI) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_LWM2M_SERVER_URI, value);
        copy_value(&(configuration.server_uri), value, value_len);
      }
      IF_MATCH(key, FN_LOCAL_PORT) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_LOCAL_PORT, value);
        if(xatoi(&value, &(configuration.local_port)) != -1) {
          configuration.local_port = -1;
        }
      }
      IF_MATCH(key, FN_WAKAAMA_SHORT_ID) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_WAKAAMA_SHORT_ID, value);
        if(xatoi(&value, &(configuration.wakaama_short_id)) != -1) {
          configuration.wakaama_short_id = -1;
        }
      }
      IF_MATCH(key, FN_WAKAAMA_COMPANY) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_WAKAAMA_COMPANY, value);
        copy_value(&(configuration.wakaama_company), value, value_len);
      }
      IF_MATCH(key, FN_WAKAAMA_MODEL_NUMBER) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_WAKAAMA_MODEL_NUMBER, value);
        copy_value(&(configuration.wakaama_model_number), value, value_len);
      }
      IF_MATCH(key, FN_CONFIG_URI) {
        printf("%s: Set %s to \"%s\" \n", __FUNCTION__, FN_CONFIG_URI, value);
        copy_value(&(configuration.config_uri), value, value_len);
      }
    } while(!f_eof(&config_file));

    usb_close_file(&config_file);

    if(configuration.config_uri != NULL) {
      targetP.firmware_url = configuration.config_uri;
      targetP.binary_filename = DOWNLOADED_CONFIG_FILE_NAME;
      targetP.download_state = DOWNLOAD_IN_PROGRESS;
      xTaskCreate(startDownload, NULL, 2000, (void*) &targetP, 2, NULL);
      while(targetP.download_state == DOWNLOAD_IN_PROGRESS) {
        osDelay(10);
      }
      if(targetP.download_state == DOWNLOAD_COMPLETED) {
        config_name = DOWNLOADED_CONFIG_FILE_NAME;
        config_to_parse = 1;
        vPortFree(configuration.config_uri);
        configuration.config_uri = NULL;
      }
    }
  }

  fill_configuration_with_default_value();

  vPortFree(buf);
  return 0;
}
