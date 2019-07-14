/*
 * HttpHeaders.h
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#ifndef SRC_VALUEPAIR_H_
#define SRC_VALUEPAIR_H_
#include "Array.h"

/**
 *
 */
typedef struct {
	char* name;
	char* value;
} ValuePair;

/**
 * Initialized a header set
 * @param h header set to initialize
 */
void ValuePair_init(Array* h);
/**
 * Clears the data within a header set.
 * Does not free the memory associated with the set, you must free() it
 * afterwards if you need to free a dynamically allocated header set.
 * @param h header set to clear
 */
void ValuePair_clear(Array* h);
/**
 * Adds a header to a set.
 * @param h header set to add a header to
 * @param name name of the header to be added
 * @param value value of the header
 * @return index of the header
 */
unsigned short ValuePair_add(Array* h, const char* name, const char* value);
/**
 * Counts the amount of a given header.
 * @param h header set to count from
 * @param name name of the header to count
 * @return header count
 */
unsigned short ValuePair_count(Array* h, const char* name);
/**
 * Checks if the header set has at least one of a given header.
 * This is preferable to HttpHeaderSet_count() if you do not need to know the amount of headers,
 * as it is faster.
 * @param h header set to search from
 * @param name name of the header to search for
 * @return true if at least one is present, false if otherwise
 */
bool ValuePair_has(Array* h, const char* name);
int ValuePair_nextIdx(Array* h, const char* name, unsigned short idxStart);
int ValuePair_idx(Array* h, const char* name);
const char* ValuePair_get(Array* h, const char* name);
const char* ValuePair_name(Array* h, int idx);
const char* ValuePair_value(Array* h, int idx);
const char* ValuePair_next(Array* h, const char* name, unsigned short *idxStart);
const ValuePair* ValuePair_header(Array* h, int idx);

size_t HttpHeader_length(Array* h);
char* HttpHeader_toString(Array* h, char* out);

size_t Post_length(Array* h);
char* Post_toString(Array* h, char* out);

#endif /* SRC_VALUEPAIR_H_ */
