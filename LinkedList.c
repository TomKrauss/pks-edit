/*
 * LinkedList.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: handle linked lists - these are used quite often
 * for different purpose in PKS Edit.
 *
 * created: 
 * last modified:
 * author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <malloc.h>
#include <string.h>
#include "lineoperations.h"

/*--------------------------------------------------------------------------
 * ll_destroy()
 * destroy a linked list. Pass a pointer to the linkedList of the linked list and
 * an additional destruction function for one element in the list. If the destroy callback
 * is passed it must return TRUE to cause the pointer itself being eliminated,
 */
void ll_destroy(LINKED_LIST **pointerLinkedList, int (*destroy)(void *elem)) {
	LINKED_LIST *lp,*lpnext = NULL;

	for (lp = *pointerLinkedList; lp != 0; lp = lpnext) {
		lpnext = lp->next;
		if (!destroy || (*destroy)(lp) == 1) {
			free(lp);
		}
	}
	*pointerLinkedList = NULL;
}

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list. Return 1 if the element was successfully deleted.
 */
int ll_delete(LINKED_LIST **pointerLinkedList, void *element) {
	LINKED_LIST *lp,*lpprev,**head;

	head = (LINKED_LIST**) pointerLinkedList;
	for (lp = *head, lpprev = 0; lp != 0; lpprev = lp, lp = lp->next) {
		if (lp == element) {
			if (lpprev) {
				lpprev->next = lp->next;
			} else {
				*head = lp->next;
			}
			free(lp);
			return 1;
		}
	}
	return 0;
}

/**
 * Find the index of an element in a linked list or return -1 if not found. 
 */
long ll_indexOf(LINKED_LIST* pHead, LINKED_LIST *lp) {
	LINKED_LIST* lc = pHead;
	long 	ln = 0;

	while (lc != lp) {
		if ((lc = lc->next) == 0) return -1L;
		ln++;
	}
	return ln;
}

/*--------------------------------------------------------------------------
 * ll_moveElementToFront()
 * put an element to the top of the linked list
 */
int ll_moveElementToFront(LINKED_LIST**pointerLinkedList, void *elem) {
	LINKED_LIST *lp,*lpprev, **head;

	head = (LINKED_LIST**) pointerLinkedList;
	for (lp = *head, lpprev = 0; lp != 0; lpprev = lp, lp = lp->next) {
		if (lp == elem) {
			if (lpprev == 0) 
				return 1;
			lpprev->next = lp->next;
			lp->next = *head;
			*head = lp;
			return 1;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * ll_insert()
 * insert an element with the given size into a linked list. The head of the list
 * will be updated to point to the new element and existing elements will be pushed back.
 */
LINKED_LIST *ll_insert(LINKED_LIST **pointerLinkedList,long size) {
	LINKED_LIST *lp;

	if ((lp = malloc((size_t)size)) == 0)
		return 0;

	memset(lp, 0, (size_t)size);

	lp->next = *pointerLinkedList;
	*pointerLinkedList = lp;
	return lp;
}

/**
 * Add an element to the end of a linked list. If the linked list does not yet exist,
 * the HEAD pointer is updated to point to the new element.
 */
void ll_add(LINKED_LIST** pointerLinkedList, LINKED_LIST* pElement) {
	LINKED_LIST* lp = *pointerLinkedList;

	pElement->next = NULL;
	if (!lp) {
		*pointerLinkedList = pElement;
	}
	else {
		while (lp->next) {
			lp = lp->next;
		}
		lp->next = pElement;
	}
}

/*--------------------------------------------------------------------------
 * ll_find()
 * 
 * Find an element in a linked list, with a given name.
 */
LINKED_LIST *ll_find(LINKED_LIST *linkedList, char *name)
{
	LINKED_LIST *lp;

	for (lp = linkedList; lp != 0; lp = lp->next) {
		if (strcmp(lp->name,name) == 0)
			return lp;
	}
	return (LINKED_LIST*) 0;
}

/**
 * Return an element from the linked list at the given index or NULL
 * if no element exists for the index.
 */
LINKED_LIST* ll_at(LINKED_LIST* head, int idx) {
	if (idx < 0) {
		return 0;
	}
	while (head != 0 && idx > 0) {
		head = head->next;
		idx--;
	}
	return head;
}

/*--------------------------------------------------------------------------
 * ll_size()
 * Count the elements in a linked list.
 */
long ll_size(LINKED_LIST*linkedList)
{
	long		nCnt;
	LINKED_LIST *lp;

	for (lp = linkedList, nCnt = 0; lp != 0; lp = lp->next) {
		nCnt++;
	}
	return nCnt;
}

