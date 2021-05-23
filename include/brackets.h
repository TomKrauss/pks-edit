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

struct tagBRACKET_RULE {
	struct tagBRACKET_RULE *next;
	int	ctx;			/* document descriptor context id */
	char* lefthand;		/* lefthand bracket character class (group of characters enclosed in [] as [{(<]) or single word to mach */
	char* righthand;	/* righthand bracket character class - see above */
	char d1,d2;			/* add val for lefthand, ... */
	char ci1[2];		/* automatic bracket indents (look up, down) indent 1-based of previous line and current line */
	char ci2[2];		/* automatic bracket indents cl2 outdent 1-based of previous line and current line */
};

typedef struct tagBRACKET_RULE BRACKET;

# ifdef	UC_H
extern void key_globs(BRACKET **bp, PASTELIST **pp[], UCLIST **up);
# endif

#define	BRACKETS_H
# endif

