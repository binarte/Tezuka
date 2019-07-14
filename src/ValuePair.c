/*
 * HttpHeaders.c
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#include "ValuePair.h"

#include "Utils.h"
#include <stdlib.h>
#include <strings.h>
#include <stddef.h>
#include <limits.h>

void ValuePair_init(Array *h) {
	Array_init(h, ValuePair);
}
void ValuePair_clear(Array *h) {
	if (h->data != NULL) {
		for (size_t i = 0; i < h->count; i++) {
			ValuePair *header = Array_itemPtr(h,ValuePair,i);
			free(header->name);
			free(header->value);
		}
		Array_clear(h);
	}
}

unsigned short ValuePair_add(Array *h, const char *name, const char *value) {
	unsigned short out = h->count;
	ValuePair *header = Array_pushVoid(h, NULL);
	header->name = copyString(name);
	header->value = copyString(value);
	return out;
}

unsigned short ValuePair_count(Array *h, const char *name) {
	unsigned short out = 0;
	for (unsigned short i = 0; i < h->count; i++) {
		ValuePair *header = Array_itemVoid(h, i);
		if (strcasecmp(name, header->name) == 0) {
			out++;
		}
	}
	return out;
}

bool ValuePair_has(Array *h, const char *name) {
	unsigned short out = 0;
	for (unsigned short i = 0; i < h->count; i++) {
		ValuePair *header = Array_itemVoid(h, i);
		if (strcasecmp(name, header->name) == 0) {
			return true;
		}
	}
	return false;
}

int ValuePair_idx(Array *h, const char *name) {
	for (unsigned short i = 0; i < h->count; i++) {
		ValuePair *header = Array_itemVoid(h, i);
		if (strcasecmp(name, header->name) == 0) {
			return i;
		}
	}
	return -1;
}

const char* ValuePair_get(Array *h, const char *name) {
	int idx = ValuePair_idx(h, name);
	if (idx < 0) {
		return NULL;
	}
	return Array_item(h,ValuePair,idx).value;
}

int ValuePair_nextIdx(Array *h, const char *name, unsigned short idxStart) {
	unsigned short i;
	for (i = idxStart; i < h->count; i++) {
		ValuePair *header = Array_itemVoid(h, i);
		if (strcasecmp(header->name, name) == 0) {
			return i;
		}
	}
	return -1;
}

const char* ValuePair_next(Array *h, const char *name,
		unsigned short *idxStart) {
	unsigned short i;
	for (i = *idxStart; i < h->count; i++) {
		ValuePair *header = Array_itemVoid(h, i);
		if (strcasecmp(header->name, name) == 0) {
			*idxStart = i + 1;
			return header->value;
		}
	}
	*idxStart = USHRT_MAX;
	return NULL;
}

size_t HttpHeader_length(Array* h){
	size_t out = 0;
	unsigned short i;
	for(i = 0; i < h->count; i++){
		ValuePair* header = Array_itemPtr(h,ValuePair,i);
		out += strlen(header->name) + strlen(header->value) + 3;
	}
	return out+1;
}
char* HttpHeader_toString(Array* h, char* out){
	char*c = out;
	unsigned short i;
	for(i = 0; i < h->count; i++){
		ValuePair* header = Array_itemPtr(h,ValuePair,i);
		sprintf(c,"%s: %s\n", header->name, header->value);
		c += sizeof(char) * strlen(c);
	}
	sprintf(c,"\n");
	return out;
}


