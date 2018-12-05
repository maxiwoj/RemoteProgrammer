#ifndef __WAKAAMA_H
#define __WAKAAMA_H

#include "connection.h"
#include "objects.h"
#include "default_config.h"

typedef struct
{
    lwm2m_object_t * securityObjP;
    int sock;
    connection_t * connList;
    int addressFamily;
} client_data_t;

//WAKAAMA
static lwm2m_context_t *lwm2mContext = NULL;
static lwm2m_object_t *objArray[OBJ_COUNT];
static client_data_t data;
static int q_reset = 0;

static void print_state(lwm2m_context_t * lwm2mH);
void taskWakaama(void *socket);
void * lwm2m_connect_server(uint16_t secObjInstID,
        void * userData);
void lwm2m_close_connection(void * sessionH,
        void * userData);
#endif