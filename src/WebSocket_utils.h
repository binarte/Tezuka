/*
 * WebSocket_utils.h
 *
 *  Created on: 29 de jun de 2019
 *      Author: jack
 */

#ifndef SRC_WEBSOCKET_UTILS_H_
#define SRC_WEBSOCKET_UTILS_H_

#include <unistd.h>
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/sha.h>

int base64_encode(const unsigned char *data, char *encoded_data,
		size_t input_length);

long int getHeaderLong(const char *headers, const char *headerName);

int getHeader(const char *headers, const char *headerName, char *out);

char* addHeader(char *headers, const char *name, const char *value);

#endif /* SRC_WEBSOCKET_UTILS_H_ */
