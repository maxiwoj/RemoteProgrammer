#include "FreeRTOS.h"
#include "task.h"
#include <sys/errno.h>
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "liblwm2m.h"
#include "communication_config.h"
#include "binary_download.h"
#include "errno.h"
#include "usb_host.h"
#include "yuarel.h"
#include "term_io.h"

static void resolveAddress(target_instance_t *targetP, char *url_str, struct yuarel *url, int *socket,
                           struct sockaddr_in *clientAddressv4);
static void connectToServer(target_instance_t *targetP, char *url_str, int socket, struct sockaddr_in *clientAddressv4);
static void makeRequest(target_instance_t *targetP, struct yuarel *url, char *url_str, int socket);
static int receive(target_instance_t *targetP, char *url_str, int socket, char *server_reply, int max_received_len);
static void checkResponseCode(target_instance_t *targetP, char *url_str, int socket, char *server_reply);
static uint8_t createFile(target_instance_t *targetP, char *url_str, int socket, FIL *file);




static void download_error(target_instance_t *targetP, int err, int socket, char *url_str) {
    targetP->download_state = DOWNLOAD_ERROR;
    targetP->download_error = (int16_t) err;
    if (err != SOCKET_ERROR && err != ERROR_PARSING_URL) {
        lwip_close(socket);
    }
    lwm2m_free(url_str);
    USB_BUSY = 0;
    vTaskDelete(NULL);
}

void startDownload(void *object_target) {
    target_instance_t *targetP = (target_instance_t *) object_target;
    struct yuarel url;
    char *url_str = lwm2m_strdup(targetP->firmware_url);
    FIL file;
    uint8_t file_initialised = 0;

    int socket;
    struct sockaddr_in clientAddressv4;
    resolveAddress(targetP, url_str, &url, &socket, &clientAddressv4);
    connectToServer(targetP, url_str, socket, &clientAddressv4);

    makeRequest(targetP, &url, url_str, socket);

    int server_response_length = 300;
    char server_reply[server_response_length + 1];
    server_reply[server_response_length] = '\0';
    long payload_len = 0;
    long total_received_len = 0;


    int received_len = receive(targetP, url_str, socket, server_reply, server_response_length);
    checkResponseCode(targetP, url_str, socket, server_reply);

    while (1) {
        if (payload_len == 0) {
            char *content_length_str = strstr(server_reply, "Content-Length: ");
            if (content_length_str != NULL) {
                if (strstr(content_length_str, "\r\n")) {
                    char *asd = content_length_str + 16;
                    if (xatoi(&asd, &payload_len) != 1) {
                        printf("Error reading Content-Length\r\n");
                        download_error(targetP, UNKNOWN_ERROR, socket, url_str);
                    }
                } else {
                    // Copy the bytes from Content-Length to the beginning and read more, because it may be the case,
                    // that not all the value of the content length had been received
                    int i;
                    for (i = 0; content_length_str - server_reply + i < received_len; i++) {
                        server_reply[i] = content_length_str[i];
                    }
                    received_len = receive(targetP, url_str, socket, server_reply + i, server_response_length - i) + i;
                    continue;
                }
            } else {
                if (strstr(server_reply, "\r\n\r\n")) {
                    download_error(targetP, CONTENT_LENGTH_NOT_SPECIFIED_ERROR, socket, url_str);
                }

                int i;
                for (i = 0; i < 20; i++) {
                    server_reply[i] = server_reply[received_len - 20 + i];
                }
                received_len = receive(targetP, url_str, socket, server_reply + i, server_response_length - i) + i;
                continue;
            }
        }

        if (payload_len > 0) {
            char *payload = strstr(server_reply, "\r\n\r\n");
            if (payload != NULL) {
                unsigned long read_length = payload - server_reply + 4; //+ 4 because of \r\n\r\n sequence
                if (read_length < received_len) {
                    file_initialised = createFile(targetP, url_str, socket, &file);
                    int result = usb_write(&file, payload + 4, received_len - read_length);
                    if (result != 0) {
                        usb_close_file(&file);
                        download_error(targetP, USB_ERROR, socket, url_str);
                    }
                    total_received_len += received_len - read_length;
                    targetP->download_progress = (uint8_t) (100 * total_received_len / payload_len);
                }
                break;
            } else {
                int i;
                for (i = 0; i < 5; i++) {
                    server_reply[i] = server_reply[received_len - 5 + i];
                }
                received_len = receive(targetP, url_str, socket, server_reply + i, server_response_length - i) + i;
            }
        }
    }

    if (!file_initialised) {
        file_initialised = createFile(targetP, url_str, socket, &file);
    }

    while (1) {
        received_len = lwip_recv(socket, server_reply, (size_t) server_response_length, 0);

        if (received_len < 0) {
            printf("recv failed\r\n");
            usb_close_file(&file);
            download_error(targetP, RECEIVE_ERROR, socket, url_str);
        }
        total_received_len += received_len;
        targetP->download_progress = (uint8_t) (100 * total_received_len / payload_len);
        server_reply[received_len] = '\0';

        long bytes_to_write;
        if (total_received_len > payload_len) {
            bytes_to_write = received_len - (total_received_len - payload_len);
        } else {
            bytes_to_write = received_len;
        }
        int result = usb_write(&file, server_reply, (size_t) bytes_to_write);
        if (result != 0) {
            usb_close_file(&file);
            download_error(targetP, USB_ERROR, socket, url_str);
        }

        if (total_received_len >= payload_len) {
            break;
        }
    }

    usb_close_file(&file);
    targetP->download_state = DOWNLOAD_COMPLETED;
    targetP->download_progress = 100;
    targetP->download_error = NO_ERROR;
    lwip_close(socket);
    lwm2m_free(url_str);
    USB_BUSY = 0;
    vTaskDelete(NULL);
}

static uint8_t createFile(target_instance_t *targetP, char *url_str, int socket, FIL *file) {
    if (get_usb_ready()) {
        int result = usb_open_file(targetP->binary_filename, file, FA_WRITE | FA_CREATE_ALWAYS);
        if (result != 0) {
            download_error(targetP, USB_ERROR, socket, url_str);
        }
    }
    return 1;
}

static void checkResponseCode(target_instance_t *targetP, char *url_str, int socket, char *server_reply) {
    if (!lwm2m_strncmp(server_reply, "HTTP/1.1 200 OK", 15) == 0 &&
        !lwm2m_strncmp(server_reply, "HTTP/1.0 200 OK", 15) == 0) {
        long response_code;
        char *asd;
        asd = server_reply + 9;
        if (xatoi(&asd, &response_code) != 1) {
            printf("Error reading Server response code\r\n");
            download_error(targetP, UNKNOWN_ERROR, socket, url_str);
        }
        printf("Server Response status:%ld\r\n", response_code);
        download_error(targetP, (int) response_code, socket, url_str);
    }
    printf("Server responded with 200!\r\n");
}

static int receive(target_instance_t *targetP, char *url_str, int socket, char *server_reply, int max_received_len) {
    int received_len = lwip_recv(socket, server_reply, (size_t) max_received_len, 0);
    if (received_len < 0) {
        printf("recv failed\r\n");
        download_error(targetP, RECEIVE_ERROR, socket, url_str);
    }
    server_reply[received_len] = '\0';
    return received_len;
}

static void makeRequest(target_instance_t *targetP, struct yuarel *url, char *url_str, int socket) {
    char message[200];
    sprintf(message, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n",
            (*url).path, (*url).host);

    if (lwip_send(socket, message, strlen(message), 0) < 0) {
        printf("Send failed\r\n");
        download_error(targetP, REQUEST_ERROR, socket, url_str);
    }
}

static void connectToServer(target_instance_t *targetP, char *url_str, int socket, struct sockaddr_in *clientAddressv4) {
    int result = lwip_connect(socket, (struct sockaddr *) clientAddressv4, sizeof(struct sockaddr_in));
    if (result < 0) {
        printf("cannot connect to server, err: %d\r\n", result);
        lwip_close(socket);
        download_error(targetP, CONNECTION_ERROR, socket, url_str);
    }
}

static void resolveAddress(target_instance_t *targetP, char *url_str, struct yuarel *url, int *socket,
                           struct sockaddr_in *clientAddressv4) {
    if (yuarel_parse(url, url_str) != 0) {
        printf("Error parsing URL\r\n");
        download_error(targetP, ERROR_PARSING_URL, 0, url_str);
    }
    (*socket) = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if ((*socket) < 0) {
        printf("cannot create socket: %d\r\n", errno);
        download_error(targetP, SOCKET_ERROR, (*socket), url_str);
    }

    u32_t addr;
    if (netconn_gethostbyname((*url).host, (ip_addr_t *) &addr) != ERR_OK) {
        printf("cannot resolve hostname\r\n");
        download_error(targetP, HOST_UNKNOWN_ERROR, (*socket), url_str);
    }
    (*clientAddressv4).sin_family = AF_INET;
    (*clientAddressv4).sin_port = htons(80);
    (*clientAddressv4).sin_addr.s_addr = addr;
}