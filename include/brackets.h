/*
 * BRACKETS.H
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: definition of brackets structure
 *
 * 										created      : 28.10.91
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

# ifndef 	BRACKETS_H

struct matchbox {
	struct matchbox *next;
	int	ctx;			/* context id */
	char *lefthand,	/* e.g. "begin" */
		*righthand;	/*    - "end" */
	char d1,d2;		/* add val for lefthand, ... */
	char ci1[2];		/* automatic bracket indents (look up, down) , cl1 */
	char ci2[2];		/* automatic bracket indents cl2 */
};

typedef struct matchbox BRACKET;

# ifdef	UC_H
extern void key_globs(BRACKET **bp, PASTELIST **pp[], UCLIST **up);
# endif

#define	BRACKETS_H
# endif

