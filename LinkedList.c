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

/*
 * Typical data structure for a linked list. 
 */
typedef struct linkedList {
	struct linkedList *next;
	char name[4];
} LINKED_LIST;

/*--------------------------------------------------------------------------
 * ll_kill()
 * destroy a linked list
 */
void ll_kill(void *head,int (*destroy)(void *elem))
{	LINKED_LIST *lp,*lpnext = NULL;

	for (lp = *(LINKED_LIST**)head; lp != 0; lp = lpnext) {
		lpnext = lp->next;
		if (!destroy || (*destroy)(lp) == 1) {
			free(lp);
		}
	}
	*(LINKED_LIST**)head = 0;
}

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list
 */
int ll_delete(void *Head, void *Elem)
{	LINKED_LIST *lp,*lpprev,**head;

	head = Head;
	for (lp = *head, lpprev = 0; lp != 0; lpprev = lp, lp = lp->next) {
		if (lp == Elem) {
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
int ll_top(LINKED_LIST **start, void *elem)
{	LINKED_LIST *lp,*lpprev, **head;

	head = start;
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
void *ll_insert(LINKED_LIST *head,long size) {
	LINKED_LIST *lp;

	if ((lp = malloc((size_t)size)) == 0)
		return 0;

	blfill(lp, (size_t)size,0);

	lp->next = *(LINKED_LIST **)head;
	*(LINKED_LIST **)head = lp;
	return lp;
}

/*--------------------------------------------------------------------------
 * ll_find()
 * 
 * Find an element in a linked list, with a given name.
 */
void *ll_find(LINKED_LIST *head, char *name)
{
	LINKED_LIST *lp;

	for (lp = head; lp != 0; lp = lp->next) {
		if (strcmp(lp->name,name) == 0)
			return lp;
	}
	return (void*) 0;
}

/*--------------------------------------------------------------------------
 * ll_count()
 */
int ll_count(LINKED_LIST *head)
{
	int		nCnt;
	LINKED_LIST *lp;

	for (lp = head, nCnt = 0; lp != 0; lp = lp->next) {
		nCnt++;
	}
	return nCnt;
}

