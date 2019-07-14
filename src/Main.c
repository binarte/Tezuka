/*
 * Main.c
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#include "Array.h"
#include <stdio.h>
#include "ValuePair.h"

int main(int argc, char **argv) {
	Array ar;
	ValuePair_init(&ar);

	ValuePair_add(&ar, "Foo", "Bar");
	ValuePair_add(&ar, "Bar", "Baz");


	char h[2048];

	printf("%s===\n", HttpHeader_toString(&ar, h));
	printf("%i %i\n", strlen(h), HttpHeader_length(&ar));

	ValuePair_clear(&ar);
	return 0;
}
