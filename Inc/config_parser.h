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

#ifndef __CONFIG_PARSER_H
#define __CONFIG_PARSER_H

#define CONFIG_FILE_NAME "config"
#define DOWNLOADED_CONFIG_FILE_NAME "netconf"

#define MAX_LINE_LENGTH 1024
#define COMMENT_SIGN '#'

/*
 * Default configuration is in file config/communication_config.h. If any of the
 * fields is not present in config it will fallback to default value.
 */

#define FN_DEVICE_NAME "device_name"

#define FN_LWM2M_SERVER_URI "lwm2m_server_uri"
#define FN_LOCAL_PORT "local_port"

#define FN_WAKAAMA_SHORT_ID "wakaama_short_id"
#define FN_WAKAAMA_COMPANY "wakaama_company"
#define FN_WAKAAMA_MODEL_NUMBER "wakaama_model_number"

#define FN_CONFIG_URI "config_uri"

typedef struct {
  char *device_name;

  char *server_uri;
  long  local_port;

  long  wakaama_short_id;
  char *wakaama_company;
  char *wakaama_model_number;

  char *config_uri;
} config_t;

extern config_t configuration;

int parse_config();

#endif //__CONFIG_PARSER_H