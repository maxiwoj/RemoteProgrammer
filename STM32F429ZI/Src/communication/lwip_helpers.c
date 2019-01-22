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
