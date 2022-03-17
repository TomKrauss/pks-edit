/*
 * JumpLabels.c
 *
 * managing jump labels
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.					
 * 
 * author: Tom
 * created: 18.05.1991
 */

#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"

#define L_FREE			0
#define L_DEFINED		1
#define L_WAITING		2
#define L_RESOLVED		3

typedef struct label {
	int			type;
	char	*	name;
	COM_GOTO*	recp;
} LABEL;

typedef struct gotos {
	LABEL *		lp;
	COM_GOTO *	recp;
} GOTOS;

#define MAXLABEL	32

static LABEL _labels[MAXLABEL];
static LABEL _waitlist[DIM(_labels)];
static GOTOS _gotos[MAXLABEL];
static int   _ngotos;

void 	yyerror(char *s, ...);
unsigned char  *yystralloc(unsigned char *s);

/*---------------------------------*/
/* bytecode_findLabelNamed()					*/
/*---------------------------------*/
static LABEL *bytecode_findLabelNamed(LABEL *lp, char *name)
{	int i;

	for (i = 0; i < MAXLABEL; i++, lp++)
		if (lp->name && strcmp(lp->name,name) == 0) 
			return lp;

	return 0;
}

/*---------------------------------*/
/* bytecode_insertLabelType()					*/
/*---------------------------------*/
static LABEL *bytecode_insertLabelType(LABEL *lp, char *name, int type)
{	int i;
	LABEL *lptmp;

	if ((lptmp = bytecode_findLabelNamed(lp,name)) != 0) {
		lp = lptmp;
		goto redef;
	}

	for (i = 0; lp->name; i++, lp++) {
		if (i >= MAXLABEL) {
			yyerror("too many labels");
			return 0;
		}
	}

	lp->name = yystralloc(name);
	lp->recp = 0;

redef:
	lp->type = type;
	return lp;
}

/*---------------------------------*/
/* bytecode_destroyLabel()					*/
/*---------------------------------*/
static void bytecode_destroyLabel(LABEL *lp)
{
	if (lp->name)
		free(lp->name);
	lp->name = 0;
	lp->recp = 0;
	lp->type = L_FREE;
}

/*---------------------------------*/
/* bytecode_closeLabels()				*/
/*---------------------------------*/
static int bytecode_closeLabels(LABEL *lp)
{	int ret = 1,i;

	for (i = 0; i < MAXLABEL; i++, lp++)
		if (lp->name) {
			if (lp->type == L_WAITING) {
				yyerror("Unresolved label %s (state waiting to be resolved)",lp->name);
				ret = 0;
			}
			bytecode_destroyLabel(lp);
		}
	return ret;
}

/*---------------------------------*/
/* bytecode_createBranchLabel()					*/
/*---------------------------------*/
int bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name)
{
	LABEL *	lp;
	int		i;
	char* recp = pBuffer->bb_current;

	if ((lp = bytecode_findLabelNamed(_labels,name)) != 0) {
		yyerror("Label %s redefined",name);
	} else	{
		if ((lp = bytecode_insertLabelType(_labels,name,L_DEFINED)) == 0) {
			return 0;
		}
	}
	lp->recp = (COM_GOTO *)recp;

	/*
	 * unresolved goto to this label ?
	 */
	if (lp->type != L_RESOLVED && (lp = bytecode_findLabelNamed(_waitlist,name)) != 0) {
		/*
		 * this was a forward goto not yet resolved
		 * here is the label, so insert rel. offset for branch
		 */
		lp->recp->offset = 
			(int)(recp - (unsigned char *)lp->recp);
		lp->type = L_RESOLVED;
		for (i = 0; i < _ngotos; i++) {
			if (_gotos[i].lp == lp) {
				_gotos[i].recp->offset = (int)(recp - (unsigned char*)_gotos[i].recp);
				// mark goto as being resolved.
				_gotos[i].lp = 0;
			}
		}
	}
	return 1;
}

/*---------------------------------*/
/* bytecode_emitGotoLabelInstruction()					*/
/*---------------------------------*/
char *bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int branchType)
{	LABEL    *lp;
	unsigned char* sp = pBuffer->bb_current;
	COM_GOTO *cp = (COM_GOTO *)sp;

	if ((sp = bytecode_emitInstruction(pBuffer, C_GOTO, (GENERIC_DATA) { branchType })) == 0)
		return 0;

	if ((lp = bytecode_findLabelNamed(_labels,name)) == 0) {
		/*
		 * branch forward - label not yet defined - push on waitstack
		 */
		if ((lp = bytecode_insertLabelType(_waitlist,name,L_WAITING)) == 0)
			return sp;
		if (lp->recp) {
			if (_ngotos >= MAXLABEL) {
				yyerror("too many jumps");
			} else {
				_gotos[_ngotos].lp = lp;
				_gotos[_ngotos].recp = cp;
				_ngotos++;
			}
		} else {
			lp->recp = cp;
		}
	} else {
		/*
		 * branch back(!) to label - label already known
		 */
		cp->offset = (int)((unsigned char *)lp->recp-
					    (unsigned char *)cp);
	}
	return sp;
}

/*---------------------------------*/
/* bytecode_closeOpenLabels()				*/
/*---------------------------------*/
int bytecode_closeOpenLabels(void)
{
	int		ret;

	bytecode_closeLabels(_labels);
	ret = bytecode_closeLabels(_waitlist);
	_ngotos = 0;
	return ret;
}

/*---------------------------------*/
/* bytecode_generateAutoLabelName()				*/
/*---------------------------------*/
char *bytecode_generateAutoLabelName(char *prefix,int num)
{	static char buf[20];

	sprintf(buf,"%s__%d",prefix,num);
	return buf;
}

/*---------------------------------*/
/* bytecode_makeAutoLabel()				*/
/*---------------------------------*/
static int _autonum,_currautolabel;
int bytecode_makeAutoLabel(COM_GOTO *cp)
{	int i;
	LABEL *lp,*lastfree = 0;

	for (i = 0, lp = _labels; i < MAXLABEL; i++, lp++) {
		if (lp->name) {
			/*
			 * branch point already inserted
			 */
			if (lp->recp == cp) {
				return 1;
			}
		} else if (!lastfree) {
			lastfree = lp;
		}
	}

	if (!lastfree)
		return 0;

	/*
	 * be careful: yystralloc requires a setjmp in main!
	 */
	lastfree->name = yystralloc(bytecode_generateAutoLabelName("L",_autonum++));
	lastfree->recp = cp;
	lastfree->type = L_DEFINED;
	return 1;
}

/*---------------------------------*/
/* bytecode_initializeAutoLabels()				*/
/*---------------------------------*/
static int bytecode_compareLabelsByName(const void *p1, const void *p2)
{
	LABEL *lp1,*lp2;

	lp1 = (LABEL*)p1;
	lp2 = (LABEL*)p2;
	if (lp1->name == 0) {
		if (lp2->name == 0)
			return 0;
		return 1;
	}
	if (lp2->name == 0)
		return -1;

	return (int)(lp1->recp - lp2->recp);
}

void bytecode_initializeAutoLabels(void)
{
	_currautolabel = 0;
	_autonum = 0;
	qsort(_labels,MAXLABEL,sizeof _labels[0], bytecode_compareLabelsByName);
}

/*---------------------------------*/
/* bytecode_startNextAutoLabel()				*/
/*---------------------------------*/
void bytecode_startNextAutoLabel(char **name, COM_GOTO **cp)
{	LABEL *lp;

	*cp = 0;
	if (_currautolabel >= MAXLABEL)
		return;
	lp  = &_labels[_currautolabel];
	if ((*name = lp->name) != 0) {
		*cp = lp->recp;
		_currautolabel++;
	}
}

/*---------------------------------*/
/* bytecode_findAutoLabelForInstruction()				*/
/*---------------------------------*/
char *bytecode_findAutoLabelForInstruction(COM_GOTO *cp)
{	LABEL *lp;
	int 	 i;

	for (lp = _labels, i = 0; i < MAXLABEL; i++,lp++)
		if (lp->recp == cp)
			return lp->name;

	return "label";
}

/*---------------------------------*/
/* bytecode_closeAutoLabels()			*/
/*---------------------------------*/
void bytecode_closeAutoLabels(void)
{
	bytecode_closeLabels(_labels);
}

/*---------------------------------*/
/* bytecode_destroyLabelNamed()					*/
/*---------------------------------*/
void bytecode_destroyLabelNamed(char *name)
{	LABEL *lp;

	if ((lp = bytecode_findLabelNamed(_labels,name)) != 0)
		bytecode_destroyLabel(lp);
	if ((lp = bytecode_findLabelNamed(_waitlist,name)) != 0)
		bytecode_destroyLabel(lp);
}

