/*
 * BRACKETS.H
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: definition of brackets structure
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauß
 */

# ifndef 	BRACKETS_H

#define	BRT_MATCH		0
#define	BRT_INDENT	1

#define	UA_SHOWMATCH	1
#define	UA_ABBREV		2
#define	UA_BRINDENT	3
#define	UA_UCMACRO	4

#define	UA_MPLEN		8

struct uclist {
	struct uclist* next;
	int		ctx;				/* -> current document descriptor */
	char* pat;			/* pattern for scanning */
	int		len;				/* len of pattern (or -1 for RE) */
	int  	action;			/* type of action to perform */
	void* p;				/* pointer to actions Parameter */
};

typedef struct uclist UCLIST;

struct uclist* uc_find(int, char*, int, int);

typedef struct tagBRACKET_RULE BRACKET;

/*--------------------------------------------------------------------------
 * uc_shiftRange() 
 * shift text range
 */
extern int uc_shiftRange(int scope, int dir);

/*--------------------------------------------------------------------------
 * uc_showMatchingBracket(s)
 */
extern int uc_showMatchingBracket(LINE* lp, int col);

/*--------------------------------------------------------------------------
 * sm_defineBracketIndentation()
 * Defines the indentation rules for auto-indent regarding brackets - is depending
 * on programming language / document type,
 */
extern int sm_defineBracketIndentation(char* leftBracketsCharacterClass, char* rightBracketsCharacterClass,
	int indentationValue,
	int up1, int down1, int up2, int down2,
	int documentCtx);

/*--------------------------------------------------------------------------
 * sm_setup()
 * Add the bracket matching definition to our internal matchlist (uclist), to prepare for bracket matching
 */
extern int sm_setup(void);

extern void uc_initializeUnderCursorActions(BRACKET** bp, PASTELIST** pp[], UCLIST** up);


#define	BRACKETS_H
# endif

