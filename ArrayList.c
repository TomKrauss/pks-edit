/*
 * ArrayList.c
 *
 * PROJEKT: PKS-EDIT for Windows
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
#include "alloc.h"
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
	if (capacity == 0) {
		capacity = INITIAL_CAPACITY;
	}
	ARRAY_LIST* pResult = calloc(1, sizeof * pResult);
	if (!pResult) {
		return NULL;
	}
	pResult->li_buffer = calloc(capacity, sizeof (void*));
	if (!pResult->li_buffer) {
		free(pResult);
		return NULL;
	}
	pResult->li_capacity = capacity;
	return pResult;
}

/*---------------------------
 * Destroy an array list freeing up all memory.
 */
void arraylist_destroy(ARRAY_LIST* pList) {
	if (pList) {
		free(pList->li_buffer);
		free(pList);
	}
}

/*
 * Destroy an array list containing string malloced before. The elements
 * of the array are freed as well.
 */
void arraylist_destroyStringList(ARRAY_LIST* pList) {
	if (!pList) {
		return;
	}
	ARRAY_ITERATOR iterator = arraylist_iterator(pList);
	void** p = iterator.i_buffer;
	while (p < iterator.i_bufferEnd) {
		char* pszString = *p++;
		free(pszString);
	}
	arraylist_destroy(pList);
}

/*
 * Clone an array list containing strings. The strings are also cloned.
 */
ARRAY_LIST* arraylist_cloneStringList(ARRAY_LIST* pList) {
	ARRAY_LIST* pNew = arraylist_create(pList->li_size);
	if (pNew == NULL) {
		return pNew;
	}
	buf_t* pBuffer = pList->li_buffer;
	for (int i = 0; i < pList->li_size; i++) {
		arraylist_add(pNew, _strdup((*pBuffer)[i]));
	}
	return pNew;
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
static void arraylist_resize(ARRAY_LIST* pList, size_t nNewCapacity) {
	pList->li_buffer = realloc(pList->li_buffer, nNewCapacity * sizeof (void*));
	pList->li_capacity = nNewCapacity;
}

static void arraylist_ensureFits(ARRAY_LIST* pList, size_t nNewsize) {
	if (nNewsize >= pList->li_capacity) {
		size_t nNewCapacity = pList->li_capacity;
		if (nNewCapacity > 200) {
			if (nNewCapacity > 2000) {
				nNewCapacity += 2000;
			}
			else {
				nNewCapacity += 200;
			}
		}
		else {
			nNewCapacity *= 2;
		}
		arraylist_resize(pList, nNewCapacity);
	}
}

/*---------------------------
 * Adds one element to the array list (at the end). 
 */
void arraylist_add(ARRAY_LIST* pList, void* pElement) {
	arraylist_ensureFits(pList, pList->li_size);
	buf_t* pBuffer = pList->li_buffer;
	(*pBuffer)[pList->li_size++] = pElement;
}

/*
 * Remove one element from the array list at a given offset into the array list
 * the array lists capacity is not shrinked.
 * Return 1, if the element was successfully removed or 0, if it was not removed.
 * If it is contained multiple times in the list, remove only the 1st occurrence.
 */
int arraylist_removeAt(ARRAY_LIST* pList, int idx) {
	buf_t* pBuffer = pList->li_buffer;
	if (idx < pList->li_size - 1) {
		memmove((*pBuffer) + idx, (*pBuffer) + idx + 1, (pList->li_size - idx - 1) * sizeof(void*));
	}
	pList->li_size--;
	return 1;
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
	return arraylist_removeAt(pList, idx);
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
 * Assigns an element at index. If the index is out of range the array is resized so the element can be added
 * at the position.
 */
void arraylist_set(ARRAY_LIST* pList, int nIndex, void *pElement) {
	if (nIndex < 0) {
		return;
	}
	buf_t* pBuffer = pList->li_buffer;
	arraylist_ensureFits(pList, nIndex);
	(*pBuffer)[nIndex] = pElement;
	if (nIndex >= pList->li_size) {
		pList->li_size = nIndex+1;
	}
}

/*
 * Returns the index of an element in the list. The object is located by using the passed comparison function,
 * which must return 0 for equivalence. If it cannot be found, -1 is returned.
 */
int arraylist_indexOfComparing(const ARRAY_LIST* pList, const void* pElement, int (*compare)(const void* p1, const void* p2)) {
	buf_t* pBuffer = pList->li_buffer;
	for (int i = 0; i < pList->li_size; i++) {
		if (compare(pElement, (*pBuffer)[i]) == 0) {
			return i;
		}
	}
	return -1;
}

static int arraylist_equalsObjects(const void* p1, const void* p2) {
	return p1 == p2 ? 0 : 1;
}

/*
 * Returns the index of an element in the list. The element is compared by identity. 
 * If it cannot be found, -1 is returned.
 */
int arraylist_indexOf(const ARRAY_LIST* pList, const void* pElement) {
	return arraylist_indexOfComparing(pList, pElement, arraylist_equalsObjects);
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

/*
 * Sort the contents of an array list. 
 */
void arraylist_sort(ARRAY_LIST* pList, int (*compare)(const void* p1, const void* p2)) {
	qsort(*pList->li_buffer, pList->li_size, sizeof(void*), compare);
}