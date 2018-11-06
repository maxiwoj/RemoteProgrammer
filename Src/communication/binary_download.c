#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "communication_config.h"
#include "binary_download.h"

int startDownload(char *path, char *filename) {
	int socket;
    socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0 ) {
        printf("cannot create socket\r\n");
        return SOCKET_ERROR;
    }

	struct sockaddr_in clientAddress;
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(80);
    clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    int result = lwip_connect(socket, (struct sockaddr *) &clientAddress, sizeof (clientAddress));
    if (result < 0) {
	    printf("cannot connect to server, err: %d\r\n", result);
        lwip_close(socket);
        return CONNECTION_ERROR;
    }

    char message[200];
    sprintf(message, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n", path, SERVER_IP);

    if( lwip_send(socket , message , strlen(message) , 0) < 0) {
        printf("Send failed\r\n");
        return REQUEST_ERROR;
    }

    int server_response_length = 300;
    char server_reply[server_response_length + 1];
    server_reply[server_response_length] = 0;
    long payload_len = 0;
    long total_received_len = 0;

    int received_len = lwip_recv(socket, server_reply , sizeof server_reply - 1, 0);
    if( received_len < 0 ){
        printf("recv failed\r\n");
        return RECEIVE_ERROR;
    }

    if(!lwm2m_strncmp(server_reply, "HTTP/1.1 200 OK", 15) == 0){
    	long response_code;
    	printf("Wrong server_reply\r\n");
    	if(xatoi(server_reply + 9, &response_code) != 1) {
    		printf("Error reading Server response code\r\n");
    		return UNKNOWN_ERROR;
    	}
    	return response_code;
    }
    printf("Server responded with 200!\r\n");
    for(int i = 15 ; i < received_len ; i++) {
		if(lwm2m_strncmp(server_reply + i, "Content-Length: ", 15) == 0){
			char *asd;
			asd = server_reply + i + 15;
			if (xatoi(&asd, &payload_len) != 1) {
				printf("Error reading Content-Length\r\n");
				return UNKNOWN_ERROR;
			}
			break;
		}
    }

    while(1){
        int received_len = lwip_recv(socket, server_reply , sizeof server_reply - 1, 0);

        if( received_len < 0 ){
            printf("recv failed\r\n");
            return RECEIVE_ERROR;
        }

        total_received_len += received_len;
        server_reply[received_len] = '\0';
  
  		if(get_usb_ready()) {
  			usb_write(server_reply, filename, received_len);
  		} else {
  			return USB_ERROR;
  		}

        if( total_received_len >= payload_len ){
            break;
        }   
    }

    return NO_ERROR;
}