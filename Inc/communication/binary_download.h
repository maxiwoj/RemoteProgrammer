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

#ifndef _BINARY_DOWNLOAD_H
#define _BINARY_DOWNLOAD_H

#include "object_target.h"

// Downlaod States
#define NO_DOWNLOAD_DATA 0
#define DOWNLOAD_IN_PROGRESS 1
#define DOWNLOAD_ERROR 2
#define DOWNLOAD_COMPLETED 3

// Download Errors
#define NO_ERROR 0
#define UNKNOWN_ERROR 1
#define SOCKET_ERROR 2
#define CONNECTION_ERROR 3
#define REQUEST_ERROR 4
#define RECEIVE_ERROR 5
#define USB_ERROR 6
#define ERROR_PARSING_URL 7
#define HOST_UNKNOWN_ERROR 8
#define CONTENT_LENGTH_NOT_SPECIFIED_ERROR 9

void startDownload(void *object_target);
#endif