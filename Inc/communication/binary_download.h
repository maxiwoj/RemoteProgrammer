#ifndef _BINARY_DOWNLOAD_H
#define _BINARY_DOWNLOAD_H

// Downlaod States
#define NO_DOWNLOAD_DATA 0
#define DOWNLOAD_IN_PROGRESS 1
#define DOWNLOAD_ERROR 2
#define DOWNLOAD_COMPLETED 3

// Download Errors
#define NO_ERROR 0
#define UNKNOWN_ERROR -1
#define SOCKET_ERROR -2
#define CONNECTION_ERROR -3
#define REQUEST_ERROR -4
#define RECEIVE_ERROR -5
#define USB_ERROR -6




int startDownload(char *path, char *filename);
#endif