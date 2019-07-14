/*
 * Utils.c
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#include "Utils.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char* copyString(const char *str) {
	size_t len = strlen(str) + 1;
	char *out = malloc(sizeof(char) * len);
	memcpy(out, str, len);
	return out;
}

int bufferArrayChunk(void **buffer, unsigned short *bufferSize, size_t objectSize,
		unsigned short count, unsigned short chunk) {
	if (*bufferSize > count) {
		return 0;
	}
	unsigned int newCapacity = ((count / chunk) + 1) * chunk;
	size_t newBufferSize = newCapacity * objectSize;
	char *newBuffer = malloc(newBufferSize);
	memset(newBuffer, '\0', newBufferSize);
	if (buffer != NULL) {
		memcpy(newBuffer, buffer, objectSize * count);
	}
	*buffer = newBuffer;
	*bufferSize = newBufferSize;

	return newCapacity;
}
