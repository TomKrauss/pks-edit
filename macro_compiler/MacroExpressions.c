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
#include <math.h>

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "regexp.h"
#include "pksmod.h"
#include "test.h"
#include "symbols.h"
#include "errordialogs.h"

extern long long number(char* s);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
int macro_isParameterStringType(unsigned char typ) {
	return typ == C_PUSH_STRING_LITERAL || typ == C_PUSH_VARIABLE;
}

/*
 * Generic coercion function to coerce PKS-Edit values.
 */
PKS_VALUE interpreter_coerce(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue, PKS_VALUE_TYPE tTargetType) {
	if (nValue.sym_type == tTargetType) {
		return nValue;
	}
	if (tTargetType == S_FLOAT) {
		if (nValue.sym_type == S_STRING) {
			double d;
			sscanf(nValue.sym_data.string, "%lf", &d);
			return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.doubleValue = d };
		}
		return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.doubleValue = (double)nValue.sym_data.longValue };
	}
	if (tTargetType == S_NUMBER) {
		if (nValue.sym_type == S_STRING) {
			return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.longValue = number(nValue.sym_data.string) };
		}
		if (nValue.sym_type == S_FLOAT) {
			return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.longValue = (long long)nValue.sym_data.doubleValue };
		}
		return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.longValue = nValue.sym_data.longValue };
	}
	if (tTargetType == S_BOOLEAN) {
		return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.booleanValue = nValue.sym_data.longValue != 0 };
	}
	if (tTargetType == S_STRING) {
		char buf[20];
		if (nValue.sym_type == S_FLOAT) {
			sprintf(buf, "%.2lf", nValue.sym_data.doubleValue);
		} else {
			sprintf(buf, "%lld", nValue.sym_data.longValue);
		}
		return (PKS_VALUE) { .sym_type = tTargetType, .sym_data.string = interpreter_allocateString(pContext, buf) };
	}
	return nValue;
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

	if (regex_match(&pattern, s1, eol, &match) &&
	    match.loc1 == s1 && match.loc2 == eol)
		return 1;

	return 0;
}

/*--------------------------------------------------------------------------
 * interpreter_testExpression()
 * Test an expression in a macro.
 */
int interpreter_testExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp) {
	PKS_VALUE v1;
	PKS_VALUE v2;
	BOOL bResult = FALSE;
	unsigned char op;

	if (!sp)
		return 0;

	op = sp->op;
	v1 = interpreter_popStackValue(pContext);
	if (!CT_IS_UNARY(op)) {
		v2 = v1;
		v1 = interpreter_popStackValue(pContext);
	}
	if (v1.sym_type != S_STRING && !CT_IS_LOGICAL(op)) {
		long long r1, r2;
		v1 = interpreter_coerce(pContext, v1, S_NUMBER);
		v2 = interpreter_coerce(pContext, v2, S_NUMBER);
		r1 = v1.sym_data.longValue;
		r2 = v2.sym_data.longValue;
		switch(op) {
			case CT_EQ: bResult = r1 == r2; break;
			case CT_NE: bResult = r1 != r2; break;
			case CT_GT: bResult = r1 > r2; break;
			case CT_LT: bResult = r1 < r2; break;
			default   : goto notimpl;
		}
	} else if (v1.sym_type == S_STRING) {
		int r1;
		v1 = interpreter_coerce(pContext, v1, S_STRING);
		v2 = interpreter_coerce(pContext, v2, S_STRING);
		unsigned char* s1, * s2;
		s1 = v1.sym_data.string;
		s2 = v2.sym_data.string;
		if (op != CT_MATCH && op != CT_NMATCH)
			r1 = strcmp(s1,s2);
		else
			r1 = strmatch(s1,s2);
		switch(op) {
			case CT_EQ: bResult = (r1 == 0); break;
			case CT_NE: bResult = (r1 != 0); break;
			case CT_GT: bResult = (r1 > 0); break;
			case CT_LT: bResult = (r1 < 0); break;
			case CT_MATCH:  bResult = r1 == 1; break;
			case CT_NMATCH: bResult = r1 == 0; break;
			default    : 
notimpl:		error_displayAlertDialog("test: ~ OP 0x%x not implemented",op);
		}
	} else {
		v1 = interpreter_coerce(pContext, v1, S_BOOLEAN);
		BOOL bBool1 = v1.sym_data.booleanValue;
		BOOL bBool2;
		if (!CT_IS_UNARY(op)) {
			v2 = interpreter_coerce(pContext, v2, S_BOOLEAN);
			bBool2 = v2.sym_data.booleanValue;
		}
		switch (op) {
		case CT_NOT: bResult = !bBool1; break;
		case CT_AND: bResult = bBool1 && bBool2; break;
		case CT_OR: bResult = bBool1 || bBool2; break;
		case CT_BRACKETS: bResult = bBool1;
		default: goto notimpl;
		}
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) {.sym_type = S_BOOLEAN, .sym_data.booleanValue = bResult});
	return 1;
}


/*--------------------------------------------------------------------------
 * macro_evaluateBinaryExpression()
 */
void macro_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp) {	
	PKS_VALUE v1;
	PKS_VALUE v2;
	unsigned char 	op;

	if (!sp)
		return;
	v1 = interpreter_popStackValue(pContext);
	op = sp->op;
	if (!IS_UNARY_OPERATOR(op)) {
		v2 = v1;
		v1 = interpreter_popStackValue(pContext);
	}

	if (op == BIN_CAST) {
		if (sp->targetType == v1.sym_type) {
			interpreter_pushValueOntoStack(pContext, v1);
			return;
		}
		interpreter_pushValueOntoStack(pContext, interpreter_coerce(pContext, v1, sp->targetType));
		return;
	}

	if (v1.sym_type != S_STRING && v2.sym_type != S_STRING) {
		if (v1.sym_type == S_FLOAT || v2.sym_type == S_FLOAT) {
			v1 = interpreter_coerce(pContext, v1, S_FLOAT);
			v2 = interpreter_coerce(pContext, v2, S_FLOAT);
			// one operand at least is numeric - force numeric calculations
			double d1 = v1.sym_data.doubleValue;
			double d2 = v2.sym_data.doubleValue;
			switch (op) {
			case BIN_POWER: d1 = pow(d1, d2); break;
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
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) {.sym_type = S_FLOAT, .sym_data.doubleValue = d1});
			return;
		}
		v1 = interpreter_coerce(pContext, v1, S_NUMBER);
		v2 = interpreter_coerce(pContext, v2, S_NUMBER);
		long long		r1;
		long long		r2;
		// one operand at least is numeric - force numeric calculations
		r1 = v1.sym_data.longValue;
		r2 = v2.sym_data.longValue;
		switch(op) {

		case BIN_NOT: r1 = !r1; break;
		case BIN_XOR: r1 ^= r2; break;
		case BIN_OR:  r1 |= r2; break;
		case BIN_AND: r1 &= r2; break;
		case BIN_SHIFT_LEFT: r1 <<= r2; break;
		case BIN_SHIFT_RIGHT: r1 >>= r2; break;
		case BIN_POWER: r1 = (long long)pow((double)r1, (double)r2); break;
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
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = r1 });
		return;
	}
	unsigned char* p1;
	unsigned char* p2;
	char		   buf[1024];
	v1 = interpreter_coerce(pContext, v1, S_STRING);
	v2 = interpreter_coerce(pContext, v2, S_STRING);
	p1 = v1.sym_data.string;
	p2 = v2.sym_data.string;
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
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, buf) });
}

