/*
 * linkedlist.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * maintain linked lists: delete, insert, modify,....
 *
 * 										created: 1.7.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef	LINKEDLIST_H

/*
 * Typical data structure for a linked list.
 */
typedef struct linkedList {
	struct linkedList* next;
	char name[4];
} LINKED_LIST;


/**
 * Find the index of an element in a linked list or return -1 if not found.
 */
extern long ll_indexOf(LINKED_LIST* pHead, LINKED_LIST* lp);

/*--------------------------------------------------------------------------
 * ll_moveElementToFront()
 * put an element to the top of the linked list
 */
extern int ll_moveElementToFront(LINKED_LIST** pointerLinkedList, void* elem);

/*--------------------------------------------------------------------------
 * ll_insert()
 * insert an element with the given size into a linked list. The head of the list
 * will be updated to point to the new element and existing elements will be pushed back.
 */
extern void* ll_insert(void** pointerLinkedList, long size);

/**
 * Add an element to the end of a linked list. If the linked list does not yet exist,
 * the HEAD pointer is updated to point to the new element.
 */
extern void ll_add(void** pointerLinkedList, LINKED_LIST* pElement);

/*--------------------------------------------------------------------------
 * ll_destroy()
 * destroy a linked list. Pass a pointer to the head of the linked list and
 * an additional destruction function for one element in the list.
 */
extern void ll_destroy(void** pointerLinkedList, int (*destroy)(void* elem));

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list.Return 1 if the element was successfully deleted.
 */
extern int ll_delete(void** pointerLinkedList, void* element);

/**
 * Return an element from the linked list at the given index or NULL
 * if no element exists for the index.
 */
extern LINKED_LIST* ll_at(LINKED_LIST* head, int idx);

/*--------------------------------------------------------------------------
 * ll_size()
 * Count the elements in a linked list.
 */
extern long ll_size(LINKED_LIST* linkedList);

/*--------------------------------------------------------------------------
 * ll_find()
 *
 * Find an element in a linked list, with a given name.
 */
extern void* ll_find(void* linkedList, char* name);



#define	LINKEDLIST_H

#endif
