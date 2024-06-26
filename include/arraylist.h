/*
 * arraylist.h
 *
 * Project: PKS Edit for Windows
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

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdio.h>

#define INITIAL_CAPACITY 3

typedef struct tagARRAY_LIST ARRAY_LIST;
typedef struct tagARRAY_ITERATOR ARRAY_ITERATOR;

struct tagARRAY_ITERATOR {
	/* Pointer to first element in the list */
	void** i_buffer;
	/* Pointer to last element in the list */
	void** i_bufferEnd;
};


/*---------------------------
 * Create an array list with an initial capacity.
 * Passing -1 will create an array list with a default capacity.
 */
extern ARRAY_LIST* arraylist_create(size_t capacity);

/*---------------------------
 * Destroy an array list freeing up all memory.
 */
extern void arraylist_destroy(ARRAY_LIST* pList);

/*
 * Clone an array list.
 */
extern ARRAY_LIST* arraylist_clone(ARRAY_LIST* pSource);

/*
 * Destroy an array list containing string malloced before. The elements
 * of the array are freed as well.
 */
extern void arraylist_destroyStringList(ARRAY_LIST* pList);

/*
 * Clone an array list containing strings. The strings are also cloned.
 */
extern ARRAY_LIST* arraylist_cloneStringList(ARRAY_LIST* pList);

/*---------------------------
 * Returns the size of an array list - i.e. the number of elements contained.
 */
extern size_t arraylist_size(ARRAY_LIST* pList);

/*---------------------------
 * Insert one element into the array list (at a given index).
 */
extern void arraylist_insertAt(ARRAY_LIST* pList, void* pElement, int nIndex);

/*---------------------------
 * Adds one element to the array list (at the end).
 */
extern void arraylist_add(ARRAY_LIST* pList, void* pElement);

/*
 * Returns the index of an element in the list. The element is compared by identity.
 * If it cannot be found, -1 is returned.
 */
extern int arraylist_indexOf(const ARRAY_LIST* pList, const void* pElement);

/*
 * Returns the index of an element in the list. The object is located by using the passed comparison function,
 * which must return 0 for equivalence. If it cannot be found, -1 is returned.
 */
extern int arraylist_indexOfComparing(const ARRAY_LIST* pList, const void* pElement, int (*compare)(const void* p1, const void* p2));

/*
 * Returns the element at index. If the index is out of range NULL is returned.
 */
extern void* arraylist_get(ARRAY_LIST* pList, int nIndex);

/*
 * Remove one element from the array list - the array lists capacity is not shrinked.
 * Return 1, if the element was successfully removed or 0, if it was not removed.
 * If it is contained multiple times in the list, remove only the 1st occurrence.
 */
extern int arraylist_remove(ARRAY_LIST* pList, void* pElement);

/*
 * Assigns an element at index. If the index is out of range nothing happens.
 */
extern void arraylist_set(ARRAY_LIST* pList, int nIndex, void* pElement);

/*
 * Remove one element from the array list at a given offset into the array list
 - the array lists capacity is not shrinked.
 * Return 1, if the element was successfully removed or 0, if it was not removed.
 * If it is contained multiple times in the list, remove only the 1st occurrence.
 */
extern int arraylist_removeAt(ARRAY_LIST* pList, int idx);

/**
 * Returns an "iterator", which allows fast iteration over the list elements.
 */
extern ARRAY_ITERATOR arraylist_iterator(ARRAY_LIST* pList);

/*
 * Sort the contents of an array list.
 */
extern void arraylist_sort(ARRAY_LIST* pList, int (*compare)(const void* p1, const void* p2));

#endif