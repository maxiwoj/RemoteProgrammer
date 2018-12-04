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

#define FN_SERVER_URI "server_uri"
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