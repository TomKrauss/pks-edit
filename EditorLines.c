/*
 * EditorLines.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: linked list generation, .. and destruction
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <malloc.h>

struct llist {
	struct llist 	*next;
	char			name[4];
};

/*--------------------------------------------------------------------------
 * ll_kill()
 * destroy a linked list
 */
void ll_kill(void *head,int (*destroy)(void *elem))
{	struct llist *lp,*lpnext;

	for (lp = *(struct llist**)head; lp != 0; lp = lpnext) {
		lpnext = lp->next;
		if (!destroy || (*destroy)(lp) == 1) {
			free(lp);
		}
	}
	*(struct llist**)head = 0;
}

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list
 */
int ll_delete(void *Head, void *Elem)
{	struct llist *lp,*lpprev,**head;

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
int ll_top(void *Head, void *Elem)
{	struct llist *lp,*lpprev,**head;

	head = Head;
	for (lp = *head, lpprev = 0; lp != 0; lpprev = lp, lp = lp->next) {
		if (lp == Elem) {
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
void *ll_insert(void *head,long size)
{
	struct llist *lp;

	if ((lp = malloc((size_t)size)) == 0)
		return 0;

	blfill(lp,(size_t)size,0);

	lp->next = *(struct llist **)head;
	*(struct llist **)head = lp;
	return lp;
}

/*--------------------------------------------------------------------------
 * ll_find()
 */
void *ll_find(void *Head, char *name)
{
	struct llist *lp;

	for (lp = (struct llist*)Head; lp != 0; lp = lp->next) {
		if (strcmp(lp->name,name) == 0)
			return lp;
	}
	return (void*) 0;
}

/*--------------------------------------------------------------------------
 * ll_count()
 */
int ll_count(void *Head)
{
	int		nCnt;
	struct llist *lp;

	for (lp = (struct llist*)Head, nCnt = 0; lp != 0; lp = lp->next) {
		nCnt++;
	}
	return nCnt;
}

