/*
 * DecompileMacros.c
 *
 * decompile macros
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author : TOM
 * created: 13.03.1991
 */

#include <stdio.h>
#include <string.h>

#include "edfuncs.h"
#include "pksmod.h"

#include "pkscc.h"
#include "test.h"
#include "funcdef.h"
#include "stringutil.h"
#include "errordialogs.h"

int  MakeAutoLabel(COM_GOTO *cp);
void StartAutoLabel(void);
void NextAutoLabel(char **name, COM_GOTO **cp);
void CloseAutoLabels(void);
char *FindAutoLabel(COM_GOTO *cp);

extern EDBINDS	_bindings;

/*
 * TestOp2Str()
 */
static char *TestOp2Str(unsigned char op)
{
	switch(op & (~0x10)) {
		case CT_AND:	return "&&";
		case CT_OR:	return "||";
		case CT_BRACKETS: return "()";
		case CT_NOT:   return "!";
		case CT_EQ:	return "==";
		case CT_NE:	return "!=";
		case CT_GT:	return ">";
		case CT_LT:	return "<";
		case CT_GE:	return ">=";
		case CT_LE:	return "<=";
		case CT_MATCH: return "~";
		case CT_NMATCH:return "!~";
		default: 		return "?";
	}
}

/*
 * idx2edfunc()
 */
static EDFUNCDEF *idx2edfunc(int idx)
{	int i;

	for (i = 0; i < _nfunctions; i++) {
		if (_functab[i].idx == idx) {
			return &_functab[i];
		}
	}
	return 0;
}


/*
 * quote()
 *
 * quote the following chars: <"\>
 */
static char *quote(unsigned char *name)
{	static char qbuf[512];
	unsigned char *d,*dend;
	
	for (d = qbuf, dend= &qbuf[sizeof qbuf-2]; d < dend && *name; name++) {
		if (*name == '\\' || *name == '"')
			*d++ = '\\';
		*d++ = *name;
	}
	*d = 0;
	return qbuf;
}

/*--------------------------------------------------------------------------
 * pr_xlated()
 */
static int pr_xlated(FILE *fp,long val,signed char partyp)
{
	OWNTYPE	*op;
	TYPEELEM	*ep,*epend;
	int printed = 0;

	partyp -= PAR_USER;
	if (partyp < 0 || partyp >= _ntypes) {
		error_displayAlertDialog("Bad Parameter type %x",(unsigned)partyp);
		return -1;
	}

	op = _typetab + partyp;
	ep = &_enelemtab[op->ot_idx];
	epend = ep+op->ot_nelem;

	if (op->ot_typ == OT_OPTION) {
		while(ep < epend) {
			if (val & ep->te_val) {
				if (printed)
					fputc('|',fp);
				fputs(ep->te_name,fp);
				printed++;
				val &= (~ep->te_val);
				if (!val)
					return 1;
			}
			ep++;
		}
	} else {
		/* OT_ENUM */
		while(ep < epend) {
			if (val == ep->te_val) {
				fputs(ep->te_name,fp);
				return 1;
			}
			ep++;
		}
	}
	if (val || !printed) {
		if (printed)
			fputc('|',fp);
		fprintf(fp,"%ld",val);
	}
	return 1;
}

/*
 * pr_param()
 */
static int pr_param(FILE *fp, unsigned char *sp, signed char partyp)
{
	unsigned char typ;
	long val;

	if (((intptr_t)sp) & 1)
		goto err;

	typ = ((COM_CHAR1 *)sp)->typ;
	switch(typ) {
		case C_FORMSTART:
			val = ((COM_FORM *)sp)->options;
			break;
		case C_1FUNC:
			val = ((COM_1FUNC *)sp)->p;
			break;
		case C_CHAR1PAR:
			val = ((COM_CHAR1 *)sp)->val;
			break;
		case C_INT1PAR:
			val = ((COM_INT1 *)sp)->val;
			break;
		case C_LONG1PAR:
			val = (long)((COM_LONG1 *)sp)->val;
			break;
		case C_STRING1PAR:
			if (partyp != PAR_STRING)
				goto err;
			fprintf(fp,"\"%s\"",quote(((COM_STRING1*)sp)->s));
			return 1;
		case C_STRINGVAR:
			if (partyp != PAR_STRING)
				goto err;
			fputs(((COM_VAR*)sp)->name,fp);
			return 1;
		case C_LONGVAR:
			if (partyp == PAR_STRING)
				goto err;
			fputs(((COM_VAR*)sp)->name,fp);
			return 1;
		default:
			err:
			error_displayAlertDialog("format error in params");
			return -1;
	}

	if (partyp == PAR_STRING)
		goto err;
	
	if (partyp >= PAR_USER) 
	    return pr_xlated(fp,val,partyp);

	if (val >= ' ' && val < 127) {
		fprintf(fp,"'%c'",(char)val);
		return 1;
	}
	fprintf(fp,"%ld",val);
	return 1;
}

/*
 * pr_func
 */
static void pr_func(FILE *fp, int idx)
{
	EDFUNCDEF	*ep = idx2edfunc(idx);

	if (!ep) {
		error_displayAlertDialog("format error: bad function");
		return;
	}
	fprintf(fp,"\t%s",macro_loadStringResource(ep->idx));
}

/*
 * makeautolabels()
 */
static void makeautolabels(char *start, char *end)
{
	while(start < end && *start != C_STOP) {
		if (*start == C_GOTO)
			MakeAutoLabel((COM_GOTO*)(start+((COM_GOTO *)start)->offset));
		start += macro_getParameterSize(*start,start+1);
	}
}

/*
 * pr_function()
 */
static unsigned char *pr_function(FILE *fp, unsigned char *sp, 
								    unsigned char *spend)
{
	COM_1FUNC *		cp = (COM_1FUNC *)sp;
	EDFUNCDEF	*	ep;
	int 			npars;
	char 			partyp;

	if (cp->typ == C_MACRO) {
		fprintf(fp,"\t%s()",((COM_MAC*)sp)->name);
		return sp+macro_getParameterSize(*sp,sp+1);
	}

	pr_func(fp,cp->funcnum);
	ep = idx2edfunc(cp->funcnum);
	fputc('(',fp);

	npars = 0;
	if (cp->typ == C_1FUNC) {
		partyp = ep->ftyps[1];
		/* c0_func glitch */
		if (partyp != PAR_VOID &&
		    (partyp < PAR_USER || (_typetab[partyp-PAR_USER].ot_flags & OF_ELIPSIS) == 0)) {
			if (pr_param(fp,sp,partyp) < 0)
				return 0;
			npars++;
		}
	}
	sp += macro_getParameterSize(*sp,sp+1);

	while(sp < spend && C_IS1PAR(*sp)) {
		if ((partyp = ep->ftyps[npars+1]) != PAR_VOID) {
			if (npars)
				fputc(',',fp);
			if (pr_param(fp,sp,partyp) < 0)
				return 0;
			npars++;
		}
		sp += macro_getParameterSize(*sp,sp+1);
	}

	fputc(')',fp);
	return sp;
}

/*
 * pr_testopnd()
 */
static void pr_testopnd(FILE *fp, unsigned char *sp,unsigned char *spend)
{	unsigned char t;

	t = ((COM_1FUNC*)sp)->typ;
	if (C_ISCMD(t))
		pr_function(fp,sp,spend);
	else {
		pr_param(fp,sp,macro_isParameterStringType(*sp) ? PAR_STRING : PAR_INT);
	}
}

/*
 * pr_expr()
 */
static unsigned char *pr_expr(FILE *fp, COM_TEST *cp, int not)
{	unsigned char *p2,*pend;
	COM_TEST *p1;
	int top;

	p1 = cp+1;
	p2 = (unsigned char *)cp + cp->p2offset;
	pend = (unsigned char *)cp + cp->size;
	switch(cp->testop & (~CT_STRING)) {
		case CT_NOT:
			if (!not)
				fprintf(fp,"%s",TestOp2Str(cp->testop));
			pr_expr(fp,p1,0);
			break;			
		case CT_OR:
		case CT_AND:
			pr_expr(fp,p1,0);
			fprintf(fp,"%s\n\t\t",TestOp2Str(cp->testop));
			if (p2 > (unsigned char *)p1)
				pr_expr(fp,(COM_TEST*)p2,0);
			else
				error_displayAlertDialog("expr: bad AND/OR|(p2 == 0x%lx)(p1 == 0x%lx)",
					p2,p1);
			break;
		case CT_BRACKETS:
			fprintf(fp,"(");
			pr_expr(fp,p1,not);
			fprintf(fp,")");
			break;
		case CT_GT:	top = (not) ? CT_LE : CT_GT; goto prdo;
		case CT_LT:	top = (not) ? CT_GE : CT_LT; goto prdo;
		case CT_EQ:	top = (not) ? CT_NE : CT_EQ; goto prdo;
		case CT_NE:	top = (not) ? CT_EQ : CT_NE; goto prdo;
		case CT_MATCH:	top = (not) ? CT_NMATCH : CT_MATCH; goto prdo;
		case CT_NMATCH:top = (not) ? CT_MATCH : CT_NMATCH;
prdo:		pr_testopnd(fp,(unsigned char *)p1,p2);
			fprintf(fp," %s ",TestOp2Str(top));
			pr_testopnd(fp,p2,pend);
			break;
		default:
			fprintf(fp,"?expr 0x%x?",cp->testop);
	}
	return pend;
}

/*
 * pr_binop()
 */
static unsigned char *pr_binop(FILE *fp, COM_BINOP *cp)
{
	unsigned char *	p1;
	unsigned char *	p2;
	unsigned char *	pend;

	p1 = (unsigned char *)(cp)+cp->op1offset;
	pend = (unsigned char *)(cp)+cp->size;
	p2 = (unsigned char *)(cp)+cp->op2offset;

	fprintf(fp,"\t%s = ",cp->result);

	switch(cp->op) {

	case BIN_BRACKETS: 
		fprintf(fp,"("); 
		pend = pr_binop(fp,(COM_BINOP*)p1);
		fprintf(fp,")"); 
		return pend;
	case BIN_XOR: 
	case BIN_ADD: 
	case BIN_SUB: 
	case BIN_MUL: 
	case BIN_DIV: 
	case BIN_MOD: 
		pr_testopnd(fp,p1,p2);
		fprintf(fp,"%c",cp->op);
		pr_testopnd(fp,p2,pend);
		break;
	case BIN_CONVERT:
		fprintf(fp,"(%s) %s", (!macro_isParameterStringType(*p1)) ? "string" : "long",
			cp->result);
		break;
	}

	return pend;
}

/*
 * pr_mac
 */
#define COM1_INCR(cp,type,offset) (unsigned char *)(cp+((type *)cp)->offset)
static void pr_mac(FILE *fp, MACRO *mp)
{	unsigned char 	*sp,*sp2,*spend,*gop,*data,*comment;
	unsigned char 	t;
	char			*lname;

	fprintf(fp,"macro %s()",quote(MAC_NAME(mp)));
	comment = MAC_COMMENT(mp);
	if (*comment)
		fprintf(fp," \"%s\"",quote(comment));
	fprintf(fp," {\n");
	data = MAC_DATA(mp);

	makeautolabels(data,&data[mp->size]);
	StartAutoLabel();
	NextAutoLabel(&lname,(COM_GOTO**)&gop);

	for (sp = data, spend = sp+mp->size; sp < spend; ) {
		if (gop <= sp && lname) {
			if (gop < sp) {
				error_displayAlertDialog("format error: bad goto");
			}
			fprintf(fp,"%s:\n",lname);
			NextAutoLabel(&lname, (COM_GOTO**)&gop);
		}

		t = ((COM_1FUNC*)sp)->typ;
		if (C_ISCMD(t)) {
			if ((sp = pr_function(fp,sp,spend)) == 0)
				goto out;
			fprintf(fp,";\n");
			continue;
		}
		if (t == C_GOTO) {
			COM_GOTO *cp = (COM_GOTO *)sp;

			fprintf(fp,"\t%s %s;\n",
				(cp->bratyp == BRA_ALWAYS) ? "goto" : "braeq",
			FindAutoLabel((COM_GOTO*)(sp+cp->offset)));
		} else if (t == C_STOP) {
			if (sp + sizeof(COM_STOP) < spend)
				fprintf(fp,"\treturn %d;\n",((COM_STOP*)sp)->rc);
		} else if (t == C_TEST) {
			COM_GOTO *cp =  (COM_GOTO *)(sp + ((COM_TEST*)sp)->size);

			/*
			 * generated if,while code
			 */
			if (cp->typ == C_GOTO &&
			    cp->bratyp == BRA_EQ) {
				fprintf(fp,"\tif ");
				sp = pr_expr(fp,(COM_TEST*)sp,1);
				fprintf(fp,"\n\t\tgoto %s;\n",
						FindAutoLabel((COM_GOTO*)(sp+cp->offset)));
				sp = (unsigned char *)(cp+1);
			} else {
				fprintf(fp,"\tif ");
				sp = pr_expr(fp,(COM_TEST*)sp,0);
				fprintf(fp,"\n");
			}
			continue;
		} else if (t == C_BINOP) {
			pr_binop(fp, (COM_BINOP*)sp);
			fprintf(fp,";\n");
			sp = COM1_INCR(sp,COM_BINOP,size);
			continue;
		} else if (t == C_ASSIGN) {
			fprintf(fp,"\t%s = ",((COM_ASSIGN*)sp)->name);
			sp2 = COM1_INCR(sp,COM_ASSIGN,size);
			pr_testopnd(fp,COM1_INCR(sp,COM_ASSIGN,opoffset),sp2);
			sp = sp2;
			fprintf(fp,";\n");
			continue;
		} else if (t == C_CREATESYM) {
			sp = COM1_INCR(sp, COM_CREATESYM, size);
		} else if (t == C_FURET) {
		
		} else {
			error_displayAlertDialog("format error in %s type=%x",MAC_NAME(mp),t);
		}
		sp += macro_getParameterSize(*sp,sp+1);
	}

out:
	fprintf(fp,"}\n\n");
	CloseAutoLabels();
}

/*
 * macro_printMacrosCallback
 */
static char *_macroname;
static long macro_printMacrosCallback(FILE *fp)
{	EDBINDS	*ep = &_bindings;
	MACRO	**mtable,*mp;
	int	    	i;

	for (mtable = ep->macp, i = 0; i < *(ep->nmacp); i++) {
		if ((mp = mtable[i]) != 0) {
			if (!_macroname ||
			    strcmp(_macroname,MAC_NAME(mp)) == 0)
				pr_mac(fp,mp);
		}
	}
	return 1;
}

/*
 * macro_saveMacrosAndDisplay
 */
int macro_saveMacrosAndDisplay(char *macroname)
{
	char szBuf[16];

	_macroname = macroname;
	if (macroname) {
		strmaxcpy(szBuf, _macroname, 8);
		strcat(szBuf, ".PKC");
	} else {
		strcpy(szBuf, "MACROS.PKC");
	}
	return macro_createFileAndDisplay(szBuf, macro_printMacrosCallback);
}


