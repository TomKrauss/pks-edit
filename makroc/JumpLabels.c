/*
 * JumpLabels.c
 *
 * managing jump labels
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 18.05.1991
 *									
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"edfuncs.h"
#include "pkscc.h"

# if !defined(_Cdecl)
#define	_Cdecl	/**/
#define	_FAR		/**/
# endif

#define L_FREE			0
#define L_DEFINED		1
#define L_WAITING		2
#define L_RESOLVED		3

typedef struct label {
	int			type;
	char	*		name;
	COM_GOTO 	*	recp;
} LABEL;

typedef struct gotos {
	LABEL *		lp;
	COM_GOTO *	recp;
} GOTOS;

#define MAXLABEL	20

static LABEL _labels[MAXLABEL];
static LABEL _waitlist[DIM(_labels)];
static GOTOS _gotos[MAXLABEL];
static int   _ngotos;

void 	yyerror(char *s, ...);
void	free(void *p);
char 	*yystralloc(char *s);
unsigned char *AddComSeq(unsigned char *sp, unsigned char *spend,
				     unsigned char typ, long par);

/*---------------------------------*/
/* FindLabel()					*/
/*---------------------------------*/
static LABEL *FindLabel(LABEL *lp, char *name)
{	int i;

	for (i = 0; i < MAXLABEL; i++, lp++)
		if (lp->name && strcmp(lp->name,name) == 0) 
			return lp;

	return 0;
}

/*---------------------------------*/
/* InsLabel()					*/
/*---------------------------------*/
static LABEL *InsLabel(LABEL *lp, char *name, int type)
{	int i;
	LABEL *lptmp;

	if ((lptmp = FindLabel(lp,name)) != 0) {
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

redef:
	lp->type = type;
	return lp;
}

/*---------------------------------*/
/* DelLabel()					*/
/*---------------------------------*/
static void DelLabel(LABEL *lp)
{
	if (lp->name)
		free(lp->name);
	lp->name = 0;
	lp->recp = 0;
	lp->type = L_FREE;
}

/*---------------------------------*/
/* CloseLabels()				*/
/*---------------------------------*/
static int CloseLabels(LABEL *lp)
{	int ret = 1,i;

	for (i = 0; i < MAXLABEL; i++, lp++)
		if (lp->name) {
			if (lp->type == L_WAITING) {
				yyerror("undefined label %s",lp->name);
				ret = 0;
			}
			DelLabel(lp);
		}
	return ret;
}

/*---------------------------------*/
/* MakeLabel()					*/
/*---------------------------------*/
int MakeLabel(char *name, char *recp)
{
	LABEL *	lp;
	int		i;

	if ((lp = FindLabel(_labels,name)) != 0) {
		yyerror("Label %s redefined",name);
	} else	{
		if ((lp = InsLabel(_labels,name,L_DEFINED)) == 0) {
			return 0;
		}
	}
	lp->recp = (COM_GOTO *)recp;

	/*
	 * unresolved goto to this label ?
	 */
	if ((lp = FindLabel(_waitlist,name)) != 0) {
		/*
		 * this was a forward goto not yet resolved
		 * here is the label, so insert rel. offset for branch
		 */
		lp->recp->offset = 
			(int)(recp - (unsigned char *)lp->recp);
		lp->type = L_RESOLVED;
		for (i = 0; i < _ngotos; i++) {
			if (_gotos[i].lp == lp) {
				_gotos[i].recp->offset = lp->recp->offset;
			}
		}
	}
	return 1;
}

/*---------------------------------*/
/* GotoLabel()					*/
/*---------------------------------*/
char *GotoLabel(char *name, char *sp, char *spend, int bratyp)
{	LABEL    *lp;
	COM_GOTO *cp = (COM_GOTO *)sp;

	if ((sp = AddComSeq(sp,spend,C_GOTO,bratyp)) == 0)
		return 0;

	if ((lp = FindLabel(_labels,name)) == 0) {
		/*
		 * branch forward - label not yet defined - push on waitstack
		 */
		if ((lp = InsLabel(_waitlist,name,L_WAITING)) == 0)
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
/* CloseLabelList()				*/
/*---------------------------------*/
int CloseLabelList(void)
{
	int		ret;

	CloseLabels(_labels);
	ret = CloseLabels(_waitlist);
	_ngotos = 0;
	return ret;
}

/*---------------------------------*/
/* LabelAutoName()				*/
/*---------------------------------*/
char *LabelAutoName(char *prefix,int num)
{	static char buf[20];

	sprintf(buf,"%s__%d",prefix,num);
	return buf;
}

/*---------------------------------*/
/* MakeAutoLabel()				*/
/*---------------------------------*/
static int _autonum,_currautolabel;
int MakeAutoLabel(COM_GOTO *cp)
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
	lastfree->name = yystralloc(LabelAutoName("L",_autonum++));
	lastfree->recp = cp;
	lastfree->type = L_DEFINED;
	return 1;
}

/*---------------------------------*/
/* StartAutoLabel()				*/
/*---------------------------------*/
static int _Cdecl lpcmp(const void _FAR *p1, const void _FAR *p2)
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

void StartAutoLabel(void)
{
	_currautolabel = 0;
	_autonum = 0;
	qsort(_labels,MAXLABEL,sizeof _labels[0], lpcmp);
}

/*---------------------------------*/
/* NextAutoLabel()				*/
/*---------------------------------*/
void NextAutoLabel(char **name, COM_GOTO **cp)
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
/* FindAutoLabel()				*/
/*---------------------------------*/
char *FindAutoLabel(COM_GOTO *cp)
{	LABEL *lp;
	int 	 i;

	for (lp = _labels, i = 0; i < MAXLABEL; i++,lp++)
		if (lp->recp == cp)
			return lp->name;

	return "label";
}

/*---------------------------------*/
/* CloseAutoLabels()			*/
/*---------------------------------*/
void CloseAutoLabels(void)
{
	CloseLabels(_labels);
}

/*---------------------------------*/
/* KillLabel()					*/
/*---------------------------------*/
void KillLabel(char *name)
{	LABEL *lp;

	if ((lp = FindLabel(_labels,name)) != 0)
		DelLabel(lp);
	if ((lp = FindLabel(_waitlist,name)) != 0)
		DelLabel(lp);
}

