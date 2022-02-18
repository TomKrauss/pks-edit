/*
 * MacroExpressions.c
 *
 * conditions in EPL...
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.					
 * 
 * author: Tom
 * created: 13.03.1991
 */

/* #define	DEBUG */

#include <stdio.h>
#include <string.h>

#include "edfuncs.h"
#include "regexp.h"
#include "pksmod.h"
#include "test.h"
#include "symbols.h"
#include "pkscc.h"
#include "errordialogs.h"

extern intptr_t		macro_doMacroFunctions(COM_1FUNC **sp, COM_1FUNC *spend);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
int macro_isParameterStringType(unsigned char typ) {
	return typ == C_STRING_LITERAL || typ == C_STRINGVAR;
}

/*--------------------------------------------------------------------------
 * macro_isParameterFloatType()
 */
int macro_isParameterFloatType(unsigned char typ) {
	return typ == C_FLOAT_LITERAL || typ == C_FLOATVAR;
}

/*
 * macro_getNumberParameter()
 */
static long long macro_getNumberParameter(unsigned char *sp,unsigned char *spend) {
	char *		s;
	extern long long number(char *s);

	switch(*sp) {
		case C_STRING_LITERAL:
		case C_STRINGVAR:
			s = (char *)macro_popParameter(&sp).string;
			return number(s);
		case C_CHARACTER_LITERAL:
		case C_INTEGER_LITERAL:
		case C_FLOAT_LITERAL:
		case C_LONG_LITERAL:
		case C_FLOATVAR:
		case C_LONGVAR:
			return macro_popParameter(&sp).longValue;
		case C_MACRO:
		case C_0FUNC:
		case C_1FUNC:
			return (long long)macro_doMacroFunctions((COM_1FUNC**)&sp, (COM_1FUNC*)spend);
	}
	return 0L;
}

/*
 * macro_getDoubleParameter()
 */
static double macro_getDoubleParameter(unsigned char* sp, unsigned char* spend) {
	char* s;
	double d;

	switch (*sp) {
	case C_STRING_LITERAL:
	case C_STRINGVAR:
		s = (char*)macro_popParameter(&sp).string;
		sscanf(s, "%lf", &d);
		return d;
	case C_FLOAT_LITERAL:
	case C_FLOATVAR:
		return macro_popParameter(&sp).doubleValue;
	default:
		return (double)macro_getNumberParameter(sp, spend);
	}
}

/*
 * macro_getStringParameter()
 */
char *macro_getStringParameter(unsigned char *sp) {
	long long lVal;
	static char buf[20];

	switch(*sp) {
		case C_STRING_LITERAL:
		case C_STRINGVAR:
			return (char *)macro_popParameter(&sp).string;
		case C_FLOAT_LITERAL:
		case C_FLOATVAR:
			sprintf(buf, "%.2lf", macro_popParameter(&sp).doubleValue);
			return buf;
		case C_CHARACTER_LITERAL:
		case C_INTEGER_LITERAL:
		case C_LONG_LITERAL:
		case C_LONGVAR:
			lVal = macro_popParameter(&sp).longValue;
			sprintf(buf,"%lld", lVal);
			return buf;
	}
	return "";
}

/*
 * strmatch()
 */
static int strmatch(char *s1,char *s2) {	
	char   ebuf[256],*eol;
	RE_PATTERN pattern;
	RE_OPTIONS options;

	options.eof = 0;
	options.patternBuf = ebuf;
	options.endOfPatternBuf = &ebuf[sizeof ebuf];
	options.flags = RE_DOREX;
	options.expression = s2;
	eol = &s1[strlen(s1)];
	if (regex_compile(&options,&pattern) == 0) {
		error_displayAlertDialog("illegal RE");
		return -1;
	}
	RE_MATCH match;

#if defined(ATARI_ST)
	if regex_match(&pattern, s1, (long)eol) &&
#else
	if (regex_match(&pattern, s1, eol, &match) &&
#endif
	    match.loc1 == s1 && match.loc2 == eol)
		return 1;

	return 0;
}

/*--------------------------------------------------------------------------
 * macro_testExpression()
 * Test an expression in a macro.
 */
int macro_testExpression(COM_TEST *sp) {	
	long long r1,r2;
	unsigned char *s1,*s2,*p2,*pend;
	unsigned char op;

	if (!sp)
		return 0;

	op = sp->testop;
	p2 = (unsigned char *)(sp)+sp->p2offset;
	pend = (unsigned char *)(sp)+sp->size; 
	sp++;
	if (CT_HAS2NUMOPNDS(op)) {
		r1 = macro_getNumberParameter((unsigned char *)sp,p2);
		r2 = macro_getNumberParameter(p2,pend);
		switch(op) {
			case CT_EQ: return r1 == r2;
			case CT_NE: return r1 != r2;
			case CT_GT: return r1 > r2;
			case CT_LT: return r1 < r2;
			default   : goto notimpl;
		}
	} else if (CT_HAS2STROPNDS(op)) {
		s1 = macro_getStringParameter((unsigned char *)sp);
		s2 = macro_getStringParameter(p2);
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
notimpl:			error_displayAlertDialog("test: ~ OP 0x%x not implemented",op);
					return 0;
		}
	} else
		switch(op) {
			case CT_NOT: return !macro_testExpression(sp);
			case CT_AND: return macro_testExpression(sp) && macro_testExpression((COM_TEST*)p2);
			case CT_OR:  return macro_testExpression(sp) || macro_testExpression((COM_TEST*)p2);
			case CT_BRACKETS: return macro_testExpression(sp);
			default: goto notimpl;
		}
}

/*--------------------------------------------------------------------------
 * macro_evaluateBinaryExpression()
 */
void macro_evaluateBinaryExpression(COM_BINOP *sp)
{	long long		r1;
	long long		r2;
	int				typ1;
	int				typ2;
	unsigned char 	op;
	unsigned char *	p1;
	unsigned char *	p2;
	unsigned char *	pend;
	char			buf[1024];

	if (!sp)
		return;

	op = sp->op;
	
	p1 = (unsigned char *)(sp)+sp->op1offset;
	pend = (unsigned char *)(sp)+sp->size;
	p2 = (unsigned char *)(sp)+sp->op2offset;
	typ1 = *p1;
	typ2 = *p2;

	if (op == BIN_CONVERT) {
		if (macro_isParameterStringType(typ1)) {
			// (T) use current method execution context
			sym_makeInternalSymbol(sym_getGlobalContext(), sp->result, S_NUMBER, (GENERIC_DATA) {
				.longValue = macro_getNumberParameter(p1, p2)
			});
		} else {
			sym_makeInternalSymbol(sym_getGlobalContext(), sp->result, S_STRING, (GENERIC_DATA) {
				.string = macro_getStringParameter(p1)
			});
		}
		return;	
	}

	if (!macro_isParameterStringType(typ1) && !macro_isParameterStringType(typ2)) {
		if (macro_isParameterFloatType(typ1) || macro_isParameterFloatType(typ2)) {
			// one operand at least is numeric - force numeric calculations
			double d1 = macro_getDoubleParameter(p1, p2);
			double d2 = macro_getDoubleParameter(p2, pend);
			switch (op) {

			case BIN_NOT: d1 = !d1; break;
			case BIN_ADD: d1 += d2; break;
			case BIN_SUB: d1 -= d2; break;
			case BIN_MUL: d1 *= d2; break;
			case BIN_DIV:
				if (!d2) {
					error_displayAlertDialog("Division by zero");
					break;
				}
				d1 /= d2;
				break;
			default:
				error_displayAlertDialog("binop: ~ OP %c not implemented for float numbers", op);
				d1 = 0;
			}
			sym_makeInternalSymbol(sym_getGlobalContext(), sp->result, S_FLOAT, (GENERIC_DATA) {
				.doubleValue = d1
			});
			return;
		}
		// one operand at least is numeric - force numeric calculations
		r1 = macro_getNumberParameter(p1,p2);
		r2 = macro_getNumberParameter(p2,pend);
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
		zero:	error_displayAlertDialog("Division by zero");
				break;
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
			error_displayAlertDialog("binop: ~ OP %c not implemented",op);
			r1 = 0;
		}
		sym_makeInternalSymbol(sym_getGlobalContext(), sp->result, S_NUMBER, (GENERIC_DATA) {
			.longValue = r1
		});
		return;
	}
	p1 = macro_getStringParameter(p1);
	p2 = macro_getStringParameter(p2);
	*buf = 0;

	switch(op) {

	case BIN_ADD: {
		char* pszP1 = p1 ? p1 : "null";
		char* pszP2 = p2 ? p2 : "null";
		if (strlen(pszP1) + strlen(pszP2) > sizeof buf) {
			error_displayAlertDialog("+: result to large");
		}
		else {
			strcat(strcpy(buf, pszP1), pszP2);
		}
		break;
	}
	case BIN_SUB: {
		char* pszP1 = p1 ? p1 : "null";
		char* pszP2 = p2 ? p2 : "null";
		if (strlen(pszP1) > sizeof buf) {
			error_displayAlertDialog("-: result to large");
		}
		else {
			strcpy(buf, pszP1);
			if ((p1 = strstr(buf, pszP2)) != 0) {
				strcpy(p1, p1 + strlen(pszP2));
			}
		}
		break;
	}
	default: 
		error_displayAlertDialog("string binop %c not impl.",op);

	}
	sym_makeInternalSymbol(sym_getGlobalContext(), sp->result, S_STRING, (GENERIC_DATA) {
		.string = buf
	});
}

