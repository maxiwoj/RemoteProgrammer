#include <liblwm2m.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lwip_helpers.h"
#include "sockets.h"
#include "mem.h"

// TODO: 
int createUDPSocket(int port, int addressFamily){
    /* create a UDP socket */
    int socket;
    socket = lwip_socket(addressFamily, SOCK_DGRAM, 0);
    if (socket < 0 ) {
        xprintf("cannot create socket\r\n");
        return -1;
    }
    if(fcntl(socket, F_SETFL, O_NONBLOCK) == -1){
        printf("cannot set socket O_NONBLOCK\r\n");
        return -1;
    }
    struct sockaddr_in clientAddress;
    clientAddress.sin_family = addressFamily;
    clientAddress.sin_port = htons(port);
    clientAddress.sin_addr.s_addr = INADDR_ANY;

    /* bind */
    if (lwip_bind(socket, (struct sockaddr *) &clientAddress, sizeof (clientAddress)) < 0) {
        printf("cannot bind socket\r\n");
        lwip_close(socket);
        return -1;
    }
    return socket;
}
