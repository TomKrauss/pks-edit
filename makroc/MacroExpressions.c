/*
 * MacroExpressions.c
 *
 * conditions in EPL...
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 13.03.1991
 *									
 */

/* # define	DEBUG */

# include	<stdio.h>
# include	<string.h>

# include	"edfuncs.h"
# include	"regcmp.h"
# include	"pksmod.h"
# include	"test.h"
# include	"binop.h"
# include "sym.h"
# include "pkscc.h"

extern long 	param_pop(unsigned char **sp);
extern long 	do_macfunc(unsigned char **sp,unsigned char *spend);
extern void 	Alert(char *s, ...);
extern int 	MakeInternalSym(char *name, char ed_typ, long value);

/*--------------------------------------------------------------------------
 * IsStringType()
 */
int IsStringType(unsigned char typ)
{
	return typ == C_STRING1PAR || typ == C_STRINGVAR;
}

/*
 * NumVal()
 */
long NumVal(unsigned char *sp,unsigned char *spend)
{
	char *		s;
	extern long 	number(char *s);

	switch(*sp) {
		case C_STRING1PAR:
		case C_STRINGVAR:
			s = (char *)param_pop(&sp);
			return number(s);
		case C_CHAR1PAR:
		case C_INT1PAR:
		case C_LONG1PAR:
		case C_LONGVAR:
			return param_pop(&sp);
		case C_MACRO:
		case C_0FUNC:
		case C_1FUNC:
			return do_macfunc(&sp,spend);
	}
	return 0L;
}

/*
 * StrVal()
 */
char *StrVal(unsigned char *sp)
{
	long v;
	static char buf[20];
		

	switch(*sp) {
		case C_STRING1PAR:
		case C_STRINGVAR:
			return (char *)param_pop(&sp);
		case C_CHAR1PAR:
		case C_INT1PAR:
		case C_LONG1PAR:
		case C_LONGVAR:
			v = param_pop(&sp);
			sprintf(buf,"%ld",v);
			return buf;
	}
	return "";
}

/*
 * strmatch()
 */
int strmatch(char *s1,char *s2)
{	REGCMP *rp = &_regcmp;
	char   ebuf[256],*eol;

	eol = &s1[strlen(s1)];
	if ((*(rp->compile))(s2,ebuf,&ebuf[sizeof ebuf],0,RE_DOREX) == 0) {
		Alert("illegal RE");
		return -1;
	}

#if defined(ATARI_ST)
	if ((*(rp->step))(s1,ebuf,(long)eol) &&
#else
	if ((*(rp->step))(s1,ebuf,eol) &&
#endif
	    *(rp->loc1) == s1 && *(rp->loc2) == eol)
		return 1;

	return 0;
}

/*--------------------------------------------------------------------------
 * Test()
 */
int Test(COM_TEST *sp)
{	long r1,r2;
	unsigned char *s1,*s2,*p2,*pend;
	unsigned char op;

	if (!sp)
		return 0;

	op = sp->testop;
	p2 = (unsigned char *)(sp)+sp->p2offset;
	pend = (unsigned char *)(sp)+sp->size; 
	sp++;
	if (CT_HAS2NUMOPNDS(op)) {
		r1 = NumVal((unsigned char *)sp,p2);
		r2 = NumVal(p2,pend);
		switch(op) {
			case CT_EQ: return r1 == r2;
			case CT_NE: return r1 != r2;
			case CT_GT: return r1 > r2;
			case CT_LT: return r1 < r2;
			default   : goto notimpl;
		}
	} else if (CT_HAS2STROPNDS(op)) {
		s1 = StrVal((unsigned char *)sp);
		s2 = StrVal(p2);
		if (op != CT_SMATCH && op != CT_SNMATCH)
			r1 = strcmp(s1,s2);
		else
			r1 = strmatch(s1,s2);
		switch(op) {
			case CT_SEQ: return (r1 == 0);
			case CT_SNE: return (r1 != 0);
			case CT_SGT: return (r1 > 0);
			case CT_SLT: return (r1 < 0);
			case CT_SMATCH:  return r1 == 1;
			case CT_SNMATCH: return r1 == 0;
			default    : 
notimpl:				Alert("test: ~ OP 0x%x not implemented",op);
					return 0;
		}
	} else
		switch(op) {
			case CT_NOT: return !Test(sp);
			case CT_AND: return Test(sp) && Test((COM_TEST*)p2);
			case CT_OR:  return Test(sp) || Test((COM_TEST*)p2);
			case CT_BRACKETS: return Test(sp);
			default: goto notimpl;
		}
}

/*--------------------------------------------------------------------------
 * Binop()
 */
void Binop(COM_BINOP *sp)
{	long 			r1;
	long 			r2;
	int				typ1;
	int				typ2;
	unsigned char 		op;
	unsigned char *	p1;
	unsigned char *	p2;
	unsigned char *	pend;
	char				buf[1024];

	if (!sp)
		return;

	op = sp->op;
	
	p1 = (unsigned char *)(sp)+sp->op1offset;
	pend = (unsigned char *)(sp)+sp->size;
	p2 = (unsigned char *)(sp)+sp->op2offset;
	typ1 = *p1;
	typ2 = *p2;

	if (op == BIN_CONVERT) {
		if (IsStringType(typ1)) {
			MakeInternalSym(sp->result,S_NUMBER,NumVal(p1,p2));
		} else {
			MakeInternalSym(sp->result,S_STRING,(long)StrVal(p1));
		}
		return;	
	}

	if (!IsStringType(typ1) || !IsStringType(typ2)) {

	    	/* one operand at least is numeric - force numeric calculations */

		r1 = NumVal(p1,p2);
		r2 = NumVal(p2,pend);

# ifdef DEBUG
	Alert("numeric binop %s = %ld %c %ld|t1:%d - t2:%d",
		  sp->result,r1,op,r2,typ1,typ2);
# endif

		switch(op) {

		case BIN_NOT: r1 = !r1; break;
		case BIN_XOR: r1 ^= r2; break;
		case BIN_OR:  r1 |= r2; break;
		case BIN_AND: r1 &= r2; break;
		case BIN_ADD: r1 += r2; break;
		case BIN_SUB: r1 -= r2; break;
		case BIN_MUL: r1 *= r2; break;
		case BIN_DIV: 
			if (!r2) {
		zero:	Alert("division by zero");
				return;
			}
			r1 /= r2; 
			break;
		case BIN_MOD:
			if (!r2) {
				goto zero;
			}
			r1 %= r2; 
			break;
		default: 
			Alert("binop: ~ OP %c not implemented",op);
			r1 = 0;
		}
		MakeInternalSym(sp->result,S_NUMBER,r1);
	} else {
		p1 = StrVal(p1);
		p2 = StrVal(p2);

# ifdef DEBUG
	Alert("string binop %s = \"%s\" %c \"%s\"|t1:%d - t2:%d",
		  sp->result,p1,op,p2,typ1,typ2);
# endif
		*buf = 0;

		switch(op) {

		case BIN_ADD: 
			if (strlen(p1) + strlen(p2) > sizeof buf) {
				Alert("+: result to large");
			} else {
				strcat(strcpy(buf,p1),p2);
			}
			break;
		case BIN_SUB:
			if (strlen(p1) > sizeof buf) {
				Alert("-: result to large");
			} else {
				strcpy(buf,p1);
				if ((p1 = strstr(buf,p2)) != 0) {
					strcpy(p1,p1+strlen(p2));
				}
			}
			break;
		default: 
			Alert("string binop %c not impl.",op);

		}
		MakeInternalSym(sp->result,S_STRING,(long)buf);
	}
}

