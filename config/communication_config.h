#ifndef __CLIENT_CONFIG_H
#define __CLIENT_CONFIG_H


#define DEVICE_NAME "Remote Programmer"

#define SERVER_URI "coap://192.168.2.1:5683"
#define LWM2M_SERVER_PORT 5683 
#define LOCAL_PORT 5683
#define MAX_PACKET_SIZE 1024

#define WAKAAMA_SHORT_ID 123  // 0 is forbidden
#define WAKAAMA_BINDING  "U" // we want lwm2m to work over UDP
#define WAKAAMA_COMPANY "AGH"
#define WAKAAMA_MODEL_NUMBER DEVICE_NAME

#endif