#ifndef __CLIENT_CONFIG_H
#define __CLIENT_CONFIG_H


#define DEVICE_NAME "Remote Programmer"

#define SERVER_URI "coap://192.168.1.13:5683"
#define LWM2M_PORT 5683 
#define MAX_PACKET_SIZE 1024

#define WAKAAMA_SHORT_ID 100  // 0 is forbidden
#define WAKAAMA_BINDING  'U'
#define WAKAAMA_COMPANY 'AGH'
#define WAKAAMA_MODEL_NUMBER DEVICE_NAME

#endif