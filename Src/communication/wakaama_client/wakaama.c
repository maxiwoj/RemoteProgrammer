#include "wakaama.h"


static void print_state(lwm2m_context_t * lwm2mH)
{
    if(lwm2mH->state == STATE_READY)
        return;
    fprintf(stderr, "State: ");
    switch(lwm2mH->state)
    {
    case STATE_INITIAL:
        fprintf(stderr, "STATE_INITIAL");
        break;
    case STATE_BOOTSTRAP_REQUIRED:
        fprintf(stderr, "STATE_BOOTSTRAP_REQUIRED");
        break;
    case STATE_BOOTSTRAPPING:
        fprintf(stderr, "STATE_BOOTSTRAPPING");
        break;
    case STATE_REGISTER_REQUIRED:
        fprintf(stderr, "STATE_REGISTER_REQUIRED");
        break;
    case STATE_REGISTERING:
        fprintf(stderr, "STATE_REGISTERING");
        break;
    case STATE_READY:
        fprintf(stderr, "STATE_READY");
        break;
    default:
        fprintf(stderr, "Unknown !");
        break;
    }
fprintf(stderr, "\r\n");
}

void taskWakaama(void *socket) {

    printf("wakaama task has started!\r\n");
    int result;
    while(1){

        result = 1;

        memset(&data, 0, sizeof (client_data_t));
        data.sock = (int) socket;
        data.addressFamily = AF_INET;

        if(objArray[0] == NULL) {
          objArray[0] = get_security_object();          
        }
        if (NULL == objArray[0]) {
            printf("Failed to create security object\r\n");
            result = -1;
            continue;
        }
        data.securityObjP = objArray[0];
        printf("SecurityObject Created\n");

        if(objArray[1] == NULL) {
          objArray[1] = get_server_object(WAKAAMA_SHORT_ID, WAKAAMA_BINDING, 300, false);
        }
        
        if (NULL == objArray[1]) {
            printf("Failed to create server object\r\n");
            result = -2;
            continue;
        }
        printf("ServerObject Created\n\r");

        if(objArray[2] == NULL) {
          printf("Getting object device\n\r");
          objArray[2] = get_object_device();
        }
        if (NULL == objArray[2]) {
            printf("Failed to create Device object\r\n");
            result = -3;
            continue;
        }
        printf("DeviceObject Created\n");

        if (objArray[3] == NULL) {
          objArray[3] = get_target_object();
        }
        if (objArray[3] == NULL) {
            printf("Failed to create Target object\r\n");
            result = -4;
            continue;
        }
        printf("TargetObject Created\n");

        // init context
        lwm2mContext = lwm2m_init(&data);
        if (NULL == lwm2mContext) {
            printf("lwm2m_init() failed\r\n");
            result = -8;
        }
        /*
        * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
        * the number of objects we will be passing through and the objects array
        */
        result = lwm2m_configure(lwm2mContext, DEVICE_NAME, NULL, NULL, OBJ_COUNT, objArray);
        if (result != COAP_NO_ERROR) {
            printf("lwm2m_configure() failed: 0x%X\r\n", result);
            result = -9;
        }

        printf("LWM2M Client has started\n");
        print_state(lwm2mContext);
        while (q_reset == 0) {
            struct timeval tv;
            fd_set readfds;

            tv.tv_sec = 60;
            tv.tv_usec = 0;

            FD_ZERO(&readfds);
            FD_SET(data.sock, &readfds);

            /*
             * This function does two things:
             *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
             *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
             *    (eg. retransmission) and the time before the next operation
             */
            result = lwm2m_step(lwm2mContext, &(tv.tv_sec));
            // FOR TESTS
            print_state(lwm2mContext);
            // FOR TESTS
            if (result != 0) {
                printf("lwm2m_step() failed: 0x%X\r\n", result);
                lwm2mContext->state = STATE_INITIAL;
            }
            /*
             * This part wait for an event on the socket until "tv" timed out (set
             * with the precedent function)
             */
            result = lwip_select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

            if (result < 0) {
                if (errno != EINTR) {
                    printf("Error in select(): %d %s\r\n", errno, strerror(errno));
                }
            } else if (result > 0) {
                uint8_t buffer[MAX_PACKET_SIZE];
                int numBytes = 0;

                /*
                 * If an event happens on the socket
                 */
                if (FD_ISSET(data.sock, &readfds)) {
                    struct sockaddr_storage addr;
                    socklen_t addrLen= 0;

                    addrLen = sizeof (addr);

                    /*
                     * We retrieve the data received
                     */
                    numBytes = lwip_recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &addr, &addrLen);

                    if (0 > numBytes) {
                        fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
                    } else if (0 < numBytes) {
                        connection_t * connP;

                        connP = connection_find(data.connList, &addr, addrLen);
                        if (connP != NULL) {
                            /*
                             * Let liblwm2m respond to the query depending on the context
                             */
                            lwm2m_handle_packet(lwm2mContext, buffer, numBytes, connP);
                        } else {
                            /*
                             * This packet comes from an unknown peer
                             */
                            fprintf(stderr, "received bytes ignored!\r\n");
                        }
                    }
                }
            }
        }



        /*
         * Finally when the loop is left, we unregister our client from it
         */
        lwm2m_close(lwm2mContext); // handles deregistration


        if(q_reset){
            lwip_close(data.sock);
            connection_free(data.connList);
            free_security_object(objArray[0]);
            clean_server_object(objArray[1]);
            free_object_device(objArray[2]);
            free_target_object(objArray[3]);
            fprintf(stdout, "\n\t RESET\r\n");
            q_reset = 0;
            NVIC_SystemReset(); // we assume we never end wakaama unless we want to restart
        }
    }
    vTaskDelete(NULL);
}

void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData) {
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    printf("Connecting to %s\r\n", uri);

    // parse uri in the form "coaps://[host]:[port]"
    if (0 == strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[') {
        host++;
        if (*(port - 1) == ']') {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        printf("Connection creation failed.\r\n");
    }
    else {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}

void lwm2m_close_connection(void * sessionH,
        void * userData) {
    client_data_t * app_data;
    connection_t * targetP;

    app_data = (client_data_t *) userData;
    targetP = (connection_t *) sessionH;

    if (targetP == app_data->connList) {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    } else {
        connection_t * parentP;

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP) {
            parentP = parentP->next;
        }
        if (parentP != NULL) {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
}
