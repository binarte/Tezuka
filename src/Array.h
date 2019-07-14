/*
 * BufferedArray.h
 *
 *  Created on: 5 de jul de 2019
 *      Author: jack
 */

#ifndef SRC_ARRAY_H_
#define SRC_ARRAY_H_

#include "Utils.h"
#include <stddef.h>

typedef struct {
	void* data;
	unsigned short count;
	unsigned short capacity;
	unsigned short chunkSize;
	size_t itemSize;
} Array;

/**
 * Fine-tuned initialization of an array
 * @param a          pointer to Array to be initialized
 * @param itemSize   size of individual items in the array
 * @param chunkSize  amount of items to add to capacity when the array is full
 */
void Array_initCustom(Array* a, size_t itemSize, unsigned short chunkSize);
/**
 * Initializes an array
 * @param AR   pointer to Array to be initialized
 * @param TYPE the type of item on the array
 */
#define Array_init(AR,TYPE)\
		Array_initCustom(AR, sizeof(TYPE), 64);
/**
 * Clears an array
 * @param a          pointer to Array to be cleared
 */
unsigned short Array_clear(Array* a);

/**
 * Pushes an item to the end of the array and returns it as a void pointer
 * @param a       pointer to Array to push an item to
 * @param out_idx if not null, the index of the item is stored here
 * @return        pointer to the new item
 */
void* Array_pushVoid(Array* a, unsigned short* out_idx);
/**
 * Pushes an item to the end of the array and returns it as a pointer with the specified type
 * @param AR   pointer to Array to push an item to
 * @param TYPE the type of item on the array
 * @return     pointer to the new item
 */
#define Array_pushPtr(AR,TYPE)\
		((TYPE*)Array_pushVoid(AR, NULL))
/**
 * Pushes an item to the end of the array and returns it's contents
 * @param AR   pointer to Array to push an item to
 * @param TYPE the type of item on the array
 * @return     content of the new item
 */
#define Array_push(AR,TYPE)\
		(*Array_pushPtr(AR,TYPE))

/**
 * Gets an item as a void pointer
 * @param a   pointer to Array to get item from
 * @param idx index of the item to get
 * @return    pointer to the item, NULL if out of bounds
 */
void* Array_itemVoid(Array* a, unsigned short idx);
/**
 * Gets an item as a pointer with the specified type
 * @param a   pointer to Array to get item from
 * @param idx index of the item to get
 * @return    pointer to the item, NULL if out of bounds
 */
#define Array_itemPtr(AR,TYPE,IDX)\
		((TYPE*)Array_itemVoid(AR, IDX))
/**
 * Gets the content of an item with the specified type.
 * WARNING: use this only when you are sure the item exists, otherwise this will most likely cause
 * a segmentation fault
 * @param a   pointer to Array to get item from
 * @param idx index of the item to get
 * @return    content of the item
 */
#define Array_item(AR,TYPE,IDX)\
		(*Array_itemPtr(AR,TYPE,IDX))

/**
 * Gets the last item as a void pointer
 * @param a   pointer to Array to get item from
 * @return    pointer to the item, NULL if out of bounds
 */
void* Array_lastVoid(Array* a);
/**
 * Gets the last item as a pointer with the specified type
 * @param a   pointer to Array to get item from
 * @return    pointer to the item, NULL if out of bounds
 */
#define Array_lastPtr(AR,TYPE)\
		((TYPE*)Array_lastVoid(AR))
/**
 * Gets the content of the last item with the specified type.
 * WARNING: use this only when you are sure the item exists, otherwise this will most likely cause
 * a segmentation fault
 * @param a   pointer to Array to get item from
 * @return    content of the item
 */
#define Array_last(AR,TYPE)\
		(*Array_lastPtr(AR,TYPE))


unsigned short Array_pull(Array* a);
/**
 * Attempt to reduce capacity to decrease memory usage by the array
 */
unsigned short Array_reduce(Array* a);


#endif /* SRC_ARRAY_H_ */
