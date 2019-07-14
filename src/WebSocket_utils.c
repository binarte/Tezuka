/*
 * WebSociet_utils.c
 *
 *  Created on: 29 de jun de 2019
 *      Author: jack
 */

#include "WebSocket_utils.h"
#include <limits.h>

static char _BK_encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };
//static char *_BK_decoding_table = NULL;
static int _BK_mod_table[] = { 0, 2, 1 };

int base64_encode(const unsigned char *data, char *encoded_data,
		size_t input_length) {

	size_t output_length;
	output_length = 4 * ((input_length + 2) / 3);

	if (encoded_data == NULL)
		return 1;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char) data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char) data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char) data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = _BK_encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = _BK_encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = _BK_encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = _BK_encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < _BK_mod_table[input_length % 3]; i++) {
		encoded_data[output_length - 1 - i] = '=';
	}
	encoded_data[output_length] = '\0';

	return 0;
}

long int getHeaderLong(const char *headers, const char *headerName) {
	char buffer[128];
	if (getHeader(headers,headerName,buffer) == 0){
		return strtol(buffer,NULL,10);
	}
	return LONG_MIN;
}

int getHeader(const char *headers, const char *headerName, char *out) {
	regex_t regex;
	int reti;
	char regstr[128];

	regmatch_t pmatch[2];

	sprintf(regstr, "%s\\s*:\\s*([a-zA-Z0-9/+=]+)", headerName);

	reti = regcomp(&regex, regstr, REG_ICASE | REG_EXTENDED);
	if (reti) {
		out[0] = '\0';
		fprintf(stderr, "Could not compile regex: %s\n", regstr);
		return -2;
	}

	reti = regexec(&regex, headers, 2, pmatch, 0);
	if (reti == REG_NOMATCH) {
		out[0] = '\0';
		return 1;
	} else if (reti) {
		regerror(reti, &regex, regstr, sizeof(regstr));
		out[0] = '\0';
		fprintf(stderr, "Regex match failed: %s\n", regstr);
		return -1;
	}

	memcpy(out, &headers[pmatch[1].rm_so], pmatch[1].rm_eo - pmatch[1].rm_so);
	out[pmatch[1].rm_eo - pmatch[1].rm_so] = '\0';
	return 0;
}

char* addHeader(char* headers, const char* name, const char* value){
	sprintf(headers,"%s: %s\n", name, value);
	return headers + strlen(headers);
}
