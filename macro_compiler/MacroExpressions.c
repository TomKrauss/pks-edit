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

#include "arraylist.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "regexp.h"
#include "pksmod.h"
#include "test.h"
#include "stringutil.h"
#include "symbols.h"
#include "errordialogs.h"

extern long long number(char* s);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
int macro_isParameterStringType(unsigned char typ) {
	return typ == C_PUSH_STRING_LITERAL || typ == C_PUSH_VARIABLE;
}

static void interpreter_asString(char* pBuf, char* pBufEnd, ARRAY_LIST* pList) {
	size_t nSize = arraylist_size(pList);
	int i = 0;
	while (i < nSize && pBuf < pBufEnd - 4) {
		if (i > 0) {
			*pBuf++ = ',';
			*pBuf++ = ' ';
		}
		char* pszS = arraylist_get(pList, i);
		size_t nLen = strlen(pszS);
		char* pszEnd = pBuf + nLen;
		if (pszEnd > pBufEnd - 4) {
			break;
		}
		strcpy(pBuf, pszS);
		pBuf = pszEnd;
		i++;
	}
	*pBuf = 0;
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
		char buf[200];
		if (nValue.sym_type == S_FLOAT) {
			sprintf(buf, "%.2lf", nValue.sym_data.doubleValue);
		} else if (nValue.sym_type == S_CHARACTER) {
			sprintf(buf, "%c", nValue.sym_data.uchar);
		}
		else if (nValue.sym_type == S_BOOLEAN) {
			sprintf(buf, "%s", nValue.sym_data.booleanValue ? "true" : "false");
		} else if (nValue.sym_type == S_STRING_ARRAY) {
			interpreter_asString(buf, &buf[sizeof buf], (ARRAY_LIST*)nValue.sym_data.stringList);
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
		interpreter_raiseError("Illegal regular expression %s", s2);
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
	} else {
		v1 = (PKS_VALUE){.sym_type = S_BOOLEAN, .sym_data.booleanValue = v1.sym_data.longValue != 0};
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
			case CT_GE: bResult = r1 >= r2; break;
			case CT_GT: bResult = r1 > r2; break;
			case CT_LT: bResult = r1 < r2; break;
			case CT_LE: bResult = r1 <= r2; break;
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
notimpl:		interpreter_raiseError("Test: Operator 0x%x not implemented",op);
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

/**
 * Evaluate expressions like 7 * "xy" or 4 * '\t'
 */
static void interpreter_evaluateMultiplicationWithStrings(EXECUTION_CONTEXT* pContext, PKS_VALUE v1, PKS_VALUE v2) {
	char buf[1024];
	v1 = interpreter_coerce(pContext, v1, S_NUMBER);
	int l1 = v1.sym_data.intValue;
	int nMult = v2.sym_type == S_CHARACTER ? 1 : (int)strlen(v2.sym_data.string);
	if (l1 * nMult > sizeof(buf) - 2) {
		interpreter_raiseError("Attempt to create a string of excessive size %d using a multiplication operator", l1 * nMult);
	}
	char* d = buf;
	for (int i = 0; i < l1; i++) {
		if (v2.sym_type == S_CHARACTER) {
			*d++ = v2.sym_data.uchar;
		}
		else {
			strcpy(d, v2.sym_data.string);
			d += nMult;
		}
	}
	*d = 0;
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, buf) });
}

/*
 * Create a range.
 */
static void interpreter_createRange(EXECUTION_CONTEXT* pContext, PKS_VALUE v1, PKS_VALUE v2) {
	int nEnd = interpreter_coerce(pContext, v2, S_NUMBER).sym_data.intValue;
	if (v1.sym_type == S_RANGE) {
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_RANGE, .sym_data.range.r_start = v1.sym_data.range.r_start, 
			.sym_data.range.r_end = v1.sym_data.range.r_end, .sym_data.range.r_increment = nEnd });
	}
	else {
		int nStart = interpreter_coerce(pContext, v1, S_NUMBER).sym_data.intValue;
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_RANGE, .sym_data.range.r_start = nStart, .sym_data.range.r_end = nEnd });
	}
}

/*
 * interpreter_extractArrayElementsAndPush
 */
static void interpreter_extractArrayElementsAndPush(EXECUTION_CONTEXT* pContext, int bOperation, ARRAY_LIST* pList, PKS_VALUE vIndex) {
	size_t nMax = arraylist_size(pList);
	if (bOperation == BIN_ADD) {
		ARRAY_LIST* pClone = arraylist_cloneStringList(pList);
		if (vIndex.sym_type == S_STRING) {
			arraylist_add(pClone, _strdup(vIndex.sym_data.string));
		} else if (vIndex.sym_type == S_STRING_ARRAY) {
			ARRAY_LIST* pSource = vIndex.sym_data.stringList;
			int nMax = (int)arraylist_size(pSource);
			for (int i = 0; i < nMax; i++) {
				char* pszString = _strdup(arraylist_get(pSource, i));
				arraylist_add(pClone, pszString);
			}
		}
		PKS_VALUE vResult = { .sym_type = S_STRING_ARRAY, .sym_data.stringList = pClone };
		memory_add(pContext, vResult);
		interpreter_pushValueOntoStack(pContext, vResult);
		return;
	}
	if (vIndex.sym_type == S_RANGE) {
		ARRAY_LIST* pClone = arraylist_create(5);
		int idxStart = vIndex.sym_data.range.r_start;
		int idxEnd = vIndex.sym_data.range.r_end;
		int iIncr = vIndex.sym_data.range.r_increment;
		if (iIncr <= 0) {
			iIncr = 1;
		}
		while (idxStart <= idxEnd && idxStart < nMax) {
			char* pszString = _strdup(arraylist_get(pList, idxStart));
			arraylist_add(pClone, pszString);
			idxStart += iIncr;
		}
		PKS_VALUE vResult = { .sym_type = S_STRING_ARRAY, .sym_data.stringList = pClone };
		memory_add(pContext, vResult);
		interpreter_pushValueOntoStack(pContext, vResult);
		return;
	}
	else {
		vIndex = interpreter_coerce(pContext, vIndex, S_NUMBER);
		int nIndex = vIndex.sym_data.intValue;
		if (nIndex >= 0 && nIndex < nMax) {
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = 
				interpreter_allocateString(pContext, arraylist_get(pList, nIndex)) });
			return;
		}
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { 
			.sym_type = S_NUMBER, .sym_data.intValue = 0 });
}

/*--------------------------------------------------------------------------
 * interpreter_evaluateBinaryExpression()
 */
void interpreter_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp) {	
	PKS_VALUE v1;
	PKS_VALUE v2;
	unsigned char op;
	char buf[1024];

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
	if (op == BIN_RANGE) {
		interpreter_createRange(pContext, v1, v2);
		return;
	}
	if (op == BIN_MUL && (v2.sym_type == S_CHARACTER || v2.sym_type == S_STRING)) {
		interpreter_evaluateMultiplicationWithStrings(pContext, v1, v2);
		return;
	}
	if (v1.sym_type == S_STRING_ARRAY && ((op == BIN_AT && (v2.sym_type == S_NUMBER || v2.sym_type == S_RANGE)) || (op == BIN_ADD))) {
		interpreter_extractArrayElementsAndPush(pContext, op, v1.sym_data.stringList, v2);
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
					interpreter_raiseError("Division by zero");
					break;
				}
				d1 /= d2;
				break;
			default:
				interpreter_raiseError("Binary operator: ~ OP %c not implemented for float numbers", op);
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
		zero:	interpreter_raiseError("Division by zero");
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
			interpreter_raiseError("Binary operator: ~ OP %c not implemented",op);
			r1 = 0;
		}
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = r1 });
		return;
	}
	unsigned char* p1;
	unsigned char* p2;
	v1 = interpreter_coerce(pContext, v1, S_STRING);
	p1 = v1.sym_data.string;
	if (op == BIN_AT) {
		size_t nLen = strlen(p1);
		if (v2.sym_type == S_RANGE) {
			int n1 = v2.sym_data.range.r_start;
			int n2 = v2.sym_data.range.r_end;
			if (n1 < 0 || n1 > n2 || n2 > nLen) {
				interpreter_raiseError("Illegal range %d, %d for string %s", n1, n2, p1);
			}
			size_t nLen = n2 - n1 + 1;
			strncpy(buf, &p1[n1], nLen);
			buf[nLen] = 0;
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, buf) });
		} else {
			v2 = interpreter_coerce(pContext, v2, S_NUMBER);
			int nIndex = v2.sym_data.intValue;
			char nResult = (nIndex < 0 && nIndex >= nLen) ? 0 : p1[nIndex];
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_CHARACTER, .sym_data.uchar = nResult });
		}
		return;
	}
	v2 = interpreter_coerce(pContext, v2, S_STRING);
	p2 = v2.sym_data.string;
	*buf = 0;
	switch(op) {

	case BIN_ADD: {
		char* pszP1 = p1 ? p1 : "null";
		char* pszP2 = p2 ? p2 : "null";
		if (strlen(pszP1) + strlen(pszP2) > sizeof buf) {
			interpreter_raiseError("+ operation on strings: result to large");
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
			interpreter_raiseError("- operation on strings: result to large");
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
		interpreter_raiseError("Binary operation %c on string arguments not implemented.",op);

	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, buf) });
}

