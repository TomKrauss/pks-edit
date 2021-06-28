/*
 * ArrayList.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: implementation of a variable size array list.
 *
 * 										created: 08.06.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arraylist.h>

#define INITIAL_CAPACITY 3


typedef void* buf_t[1];

struct tagARRAY_LIST {
	size_t li_size;
	size_t li_capacity;
	buf_t* li_buffer;
};

/*---------------------------
 * Create an array list with an initial capacity.
 * Passing 0 will create an array list with a default capacity.
 */
ARRAY_LIST* arraylist_create(size_t capacity) {
	if (capacity <= 0) {
		capacity = INITIAL_CAPACITY;
	}
	ARRAY_LIST* pResult = calloc(1, sizeof * pResult);
	pResult->li_buffer = calloc(capacity, sizeof (void*));
	pResult->li_capacity = capacity;
	return pResult;
}

/*---------------------------
 * Destroy an array list freeing up all memory.
 */
void arraylist_destroy(ARRAY_LIST* pList) {
	free(pList->li_buffer);
	free(pList);
}

/*---------------------------
 * Returns the size of an array list - i.e. the number of elements contained.
 */
size_t arraylist_size(ARRAY_LIST* pList) {
	return pList->li_size;
}

/**
 * Resize an array list so nNewCapacity elements will fit into it.
 */
static arraylist_resize(ARRAY_LIST* pList, size_t nNewCapacity) {
	pList->li_buffer = realloc(pList->li_buffer, nNewCapacity * sizeof (void*));
	pList->li_capacity = nNewCapacity;
}

/*---------------------------
 * Adds one element to the array list (at the end). 
 */
void arraylist_add(ARRAY_LIST* pList, void* pElement) {
	if (pList->li_size >= pList->li_capacity) {
		size_t nNewCapacity = pList->li_capacity;
		if (nNewCapacity > 200) {
			if (nNewCapacity > 2000) {
				nNewCapacity += 2000;
			} else {
				nNewCapacity += 200;
			}
		} else {
			nNewCapacity *= 2;
		}
		arraylist_resize(pList, nNewCapacity);
	}
	buf_t* pBuffer = pList->li_buffer;
	(*pBuffer)[pList->li_size++] = pElement;
}

/*
 * Remove one element from the array list - the array lists capacity is not shrinked.
 * Return 1, if the element was successfully removed or 0, if it was not removed.
 * If it is contained multiple times in the list, remove only the 1st occurrence.
 */
int arraylist_remove(ARRAY_LIST* pList, void* pElement) {
	int idx = arraylist_indexOf(pList, pElement);
	if (idx < 0) {
		return 0;
	}
	buf_t* pBuffer = pList->li_buffer;
	if (idx < pList->li_size - 1) {
		memmove((*pBuffer) + idx, (*pBuffer) + idx + 1, (pList->li_size - idx - 1) * sizeof(void*));
	}
	pList->li_size--;
	return 1;
}

/*
 * Returns the element at index. If the index is out of range NULL is returned.
 */
void* arraylist_get(ARRAY_LIST* pList, int nIndex) {
	buf_t* pBuffer = pList->li_buffer;
	if (nIndex >= 0 && nIndex < pList->li_size) {
		return (*pBuffer)[nIndex];
	}
	return NULL;
}

/*
 * Returns the index of an element in the list. If it cannot be found, -1 is returned.
 */
int arraylist_indexOf(ARRAY_LIST* pList, void* pElement) {
	buf_t* pBuffer = pList->li_buffer;
	for (int i = 0; i < pList->li_size; i++) {
		if (pElement == (*pBuffer)[i]) {
			return i;
		}
	}
	return -1;
}

/**
 * Returns an "iterator", which allows fast iteration over the list elements. 
 */
ARRAY_ITERATOR arraylist_iterator(ARRAY_LIST* pList) {
	ARRAY_ITERATOR iterator;
	iterator.i_buffer = *pList->li_buffer;
	iterator.i_bufferEnd = iterator.i_buffer + pList->li_size;
	return iterator;
}