#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "communication_config.h"
#include "binary_download.h"
#include "errno.h"


static void download_error(target_instance_t *targetP, int err, int socket) {
	targetP->download_state = DOWNLOAD_ERROR;
	targetP->download_error = err;
	if(err != SOCKET_ERROR){
		lwip_close(socket);
	}
	vTaskDelete(NULL);
}

int startDownload(target_instance_t *targetP) {

	int socket;
    socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0 ) {
        printf("cannot create socket: %d\r\n", errno);
        download_error(targetP, SOCKET_ERROR, socket);
    }

	struct sockaddr_in clientAddress;
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(80);
    clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    int result = lwip_connect(socket, (struct sockaddr *) &clientAddress, sizeof (clientAddress));
    if (result < 0) {
	    printf("cannot connect to server, err: %d\r\n", result);
        lwip_close(socket);
        download_error(targetP,CONNECTION_ERROR, socket);
    }

    char message[200];
    sprintf(message, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n", targetP->firmware_url, SERVER_IP);

    if( lwip_send(socket , message , strlen(message) , 0) < 0) {
        printf("Send failed\r\n");
        download_error(targetP,REQUEST_ERROR, socket);
    }

    int server_response_length = 300;
    char server_reply[server_response_length + 1];
    server_reply[server_response_length] = 0;
    long payload_len = 0;
    long total_received_len = 0;

    int received_len = lwip_recv(socket, server_reply , sizeof server_reply - 1, 0);
    if( received_len < 0 ){
        printf("recv failed\r\n");
        download_error(targetP,RECEIVE_ERROR, socket);
    }

    if(!lwm2m_strncmp(server_reply, "HTTP/1.1 200 OK", 15) == 0){
    	long response_code;
    	char *asd;
    	asd = server_reply + 9;
    	if(xatoi(&asd, &response_code) != 1) {
    		printf("Error reading Server response code\r\n");
    		download_error(targetP,UNKNOWN_ERROR, socket);
    	}
    	printf("Server Response status:%d\r\n", response_code);
    	download_error(targetP, response_code, socket);
    }
    printf("Server responded with 200!\r\n");
    for(int i = 15 ; i < received_len ; i++) {
		if(lwm2m_strncmp(server_reply + i, "Content-Length: ", 15) == 0){
			char *asd;
			asd = server_reply + i + 15;
			if (xatoi(&asd, &payload_len) != 1) {
				printf("Error reading Content-Length\r\n");
				download_error(targetP, UNKNOWN_ERROR, socket);
			}
			break;
		}
    }

    while(1){
        int received_len = lwip_recv(socket, server_reply , sizeof server_reply - 1, 0);

        if( received_len < 0 ){
            printf("recv failed\r\n");
            download_error(targetP, RECEIVE_ERROR, socket);
        }


        targetP->download_progress = 100 * total_received_len / payload_len;

        total_received_len += received_len;
        server_reply[received_len] = '\0';
  
  		if(get_usb_ready()) {
  			usb_write(server_reply, targetP->binary_filename, received_len);
  		} else {
  			download_error(targetP, USB_ERROR, socket);
  		}

        if( total_received_len >= payload_len ){
            break;
        }   
    }

    targetP->download_state = DOWNLOAD_COMPLETED;
    targetP->download_progress = 100;
    targetP->download_error = NO_ERROR;	
    lwip_close(socket);
    vTaskDelete();

}