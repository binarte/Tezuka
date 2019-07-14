/*
 * Utils.h
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_
#include <stddef.h>

typedef unsigned char bool;
#define true 1
#define false 0

char* copyString(const char *str);

int bufferArrayChunk(void **buffer, unsigned short *bufferSize, size_t objectSize,
		unsigned short count, unsigned short chunk);

#define bufferArray(SRC,TYPE,CHUNK)\
		bufferArrayChunk((void*)&SRC##s, &SRC##Buffer,sizeof(TYPE),SRC##Count,CHUNK);

#endif /* SRC_UTILS_H_ */
