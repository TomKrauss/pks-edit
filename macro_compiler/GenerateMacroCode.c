/*
 * GenerateMacroCode.c
 *
 * generate code for macros
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 13.03.1991
 *									
 */

#include <stdio.h>
#include <string.h>

#include "pkscc.h"
#include "edfuncs.h"
#include "scanner.h"

extern void 	yyerror(char *s, ...);
extern int 	mac_insert(char *name, char *comment, char *macdata, int size);
extern int 	macro_isParameterStringType(unsigned char typ);
extern void 	freeval(TYPEDVAL *v);
static MACRO	**		_macrotab;
extern unsigned char *	_recp;
extern unsigned char *	_recpend;

#if defined(STAND_ALONE)
int 	_nmacros;
/*--------------------------------------------------------------------------
 * mac_insert()
 */
int mac_insert(char *name, char *comment, void *data, int size)
{
	MACRO  *mp;
	int    i;

	protokoll("Defining Macro \"%s\" (%ld bytes)",name,(long)size);

	for (i = 0; i < _nmacros && _macrotab; i++) {
		if ((mp = mac_byindex(i)) == 0 ||
			strcmp(mp->name,name) == 0) {
			_free(mp);
			_macrotab[i] = 0;
			break;
		}
	}

	if (!_macrotab) {
		_macrotab = _calloc(64 * sizeof _macrotab[0]);
		_nmacros = 64;
	} else if (i >= _nmacros) {
		_nmacros += 64;
		_macrotab = _realloc(_macrotab,_nmacros * sizeof _macrotab[0]);
	}

	_macrotab[i] = mp = mac_new(name,comment,data,size);

	return i;
}

# endif

/*
 * AddComSeq()
 */
static COM_FORM *_lastformstart;
unsigned char *AddComSeq(unsigned char *sp, unsigned char *spend,
				     unsigned char typ, intptr_t par)
{	long s;
	unsigned char *spret;

	if (sp == 0) {
		yyerror(/*STR*/"adding to zero buffer?");
		return 0;
	}

	/*
	 * pack longs
	 */
	if (typ == C_LONG1PAR || typ == C_INT1PAR) {
		if (par >= 0 && par <= 255)
			typ = C_CHAR1PAR;
	}

	s = macro_getParameterSize(typ,(char *)par);
	if ((spret = sp+s) > spend) {
		yyerror(/*STR*/"buffer overflow");
		return 0;
	}

	switch(typ) {
		case C_STOP:
		case C_0FUNC:
		case C_CHAR1PAR: 
		case C_FURET:
			((COM_CHAR1 *)sp)->val = (unsigned char)par;
			break;
		case C_FORMSTART:
			_lastformstart = (COM_FORM *)sp;
			_lastformstart->options = (unsigned char)par;
			_lastformstart->nfields = 0;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)par;
			break;
		case C_INT1PAR:
			((COM_INT1 *)sp)->val = (int)par;
			break;
		case C_TEST:
			((COM_TEST *)sp)->testop = (unsigned char)par;
			((COM_TEST *)sp)->size = sizeof(COM_TEST);
			break;
		case C_GOTO:
			((COM_GOTO *)sp)->bratyp = (unsigned char)par;
			((COM_GOTO *)sp)->offset = sizeof(COM_GOTO);
			break;
		case C_LONG1PAR:
			((COM_LONG1 *)sp)->val = par;
			break;
		case C_STRINGVAR:
		case C_LONGVAR:
		case C_MACRO:
			strcpy(((COM_MAC*)sp)->name,(char *)par);
			break;
		case C_STRING1PAR:
			if (par)
				strcpy(((COM_STRING1*)sp)->s,(char *)par);
			else
				((COM_STRING1*)sp)->s[0] = 0;
			break;
	}

	if (C_ISCMD(typ)) {
		_lastformstart = 0;
	}

	if (typ != C_FORMSTART) {
		if (C_IS1PAR(typ)) {
			if (_lastformstart)
				_lastformstart->nfields++;
		} else {
			_lastformstart = 0;
		}
	}

	((COM_1FUNC *)sp)->typ = typ;
	return spret;
}

/*--------------------------------------------------------------------------
 * PushTypedAssign()
 */
static void PushTypedAssign(int comType, char *name, int typ, intptr_t val)
{
	unsigned char *	p1;
	COM_ASSIGN *		ap;

	ap = (COM_ASSIGN*)_recp;
	ap->typ = comType;
	p1 = ap->name;
	while(*name) {
		*p1++ = *name++;
	}
	*p1++ = 0;
	if ((short)p1 & 1) {
		p1++;
	}

	_recp = AddComSeq(p1,_recpend,typ,val);
	ap->opoffset = (int)(p1 - (unsigned char*)ap);
	ap->size = (int)(_recp - (unsigned char*)ap);
}

/*--------------------------------------------------------------------------
 * PushAssign()
 */
void PushAssign(char *name, int typ, intptr_t val)
{

	PushTypedAssign(C_ASSIGN, name, typ, val);
}

/*--------------------------------------------------------------------------
 * PushAssign()
 */
void PushCreateVariable(char *name, int typ, intptr_t val)
{
	unsigned char *	p1;
	COM_CREATESYM *	ap;

	ap = (COM_CREATESYM*)_recp;
	ap->typ = C_CREATESYM;
	ap->symtype = typ;
	ap->value = (long)val;

	p1 = ap->name;
	while(*name) {
		*p1++ = *name++;
	}
	*p1++ = 0;
	if ((short)p1 & 1) {
		p1++;
	}

	_recp = p1;
	ap->size = (int)(_recp - (unsigned char*)ap);
}

/*--------------------------------------------------------------------------
 * PushBinop()
 */
extern int vname_count;
TYPEDVAL PushBinop(int opd_typ, TYPEDVAL*v1, TYPEDVAL*v2) {
	unsigned char *	p1;
	unsigned char *	p2;
	COM_BINOP *		bp;
	TYPEDVAL		ret;

	bp = (COM_BINOP*)_recp;
	bp->typ = C_BINOP;
	bp->op = opd_typ;
	sprintf(bp->result,"__ret%d",vname_count++);
	ret.val = (intptr_t)bp->result;
	ret.type = (macro_isParameterStringType(v1->type) && (!v2 || macro_isParameterStringType(v2->type))) ?
		C_STRINGVAR : C_LONGVAR;
	p1 = bp->result;

	while(*p1++)
		;
	if ((short)p1 & 1) {
		p1++;
	}

	p2 = AddComSeq(p1,_recpend,v1->type,v1->val);
	freeval(v1);
	if (v2) {
		_recp = AddComSeq(p2,_recpend,v2->type,v2->val);
		freeval(v2);
	} else {
		_recp = AddComSeq(p2,_recpend,C_LONG1PAR,0);
	}

	bp->op1offset = (int)(p1 - (unsigned char*)bp);
	bp->op2offset = (int)(p2 - (unsigned char*)bp);
	bp->size = (int)(_recp - (unsigned char*)bp);

	return ret;
}

/* 
 * mac_validatename
 */
int mac_validatename(unsigned char *name)
{	int mod = 0, ret = 1;
	char *start = name;

	while(*name) {
		if (! ((*name >= 'a' && *name <= 'z') ||
			  (*name >= 'A' && *name <= 'Z') ||
		       (*name == '-' || *name == '-') ||
			  (*name >= '0' && *name <= '9'))) {
			mod = 1;
			ret = 0;
			*name = '-';
		}
		name++;
	}

	if (mod) {
		yyerror("illegal characters in Macroname. corrected to \"%s\"",start);
	}

	return ret;
}


