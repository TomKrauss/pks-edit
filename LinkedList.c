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
 * (c) Pahlen & Krauﬂ
 */

#include <malloc.h>
#include "lineoperations.h"

/*--------------------------------------------------------------------------
 * ll_kill()
 * destroy a linked list. Pass a pointer to the linkedList of the linked list and
 * an additional destruction function for one element in the list.
 */
void ll_kill(void **pointerLinkedList,int (*destroy)(void *elem)) {
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
 * delete an element in a linked list.Return 1 if the element was successfully deleted.
 */
int ll_delete(void **pointerLinkedList, void *element) {
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

/*--------------------------------------------------------------------------
 * ll_top()
 * put an element to the top of the linked list
 */
int ll_top(void **pointerLinkedList, void *elem)
{	LINKED_LIST *lp,*lpprev, **head;

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
 * insert an element to a linked list
 */
void *ll_insert(void **pointerLinkedList,long size) {
	LINKED_LIST *lp;

	if ((lp = malloc((size_t)size)) == 0)
		return 0;

	blfill(lp, (size_t)size,0);

	lp->next = *pointerLinkedList;
	*pointerLinkedList = lp;
	return lp;
}

/*--------------------------------------------------------------------------
 * ll_find()
 * 
 * Find an element in a linked list, with a given name.
 */
void *ll_find(void *linkedList, char *name)
{
	LINKED_LIST *lp;

	for (lp = linkedList; lp != 0; lp = lp->next) {
		if (strcmp(lp->name,name) == 0)
			return lp;
	}
	return (void*) 0;
}

/*--------------------------------------------------------------------------
 * ll_count()
 * Count the elements in a linked list.
 */
int ll_count(void *linkedList)
{
	int		nCnt;
	LINKED_LIST *lp;

	for (lp = linkedList, nCnt = 0; lp != 0; lp = lp->next) {
		nCnt++;
	}
	return nCnt;
}

