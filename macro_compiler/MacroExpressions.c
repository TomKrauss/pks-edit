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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "regexp.h"
#include "pksmod.h"
#include "test.h"
#include "stringutil.h"
#include "symbols.h"
#include "errordialogs.h"

extern long long number(char* s);

static void interpreter_asString(char* pBuf, char* pBufEnd, PKS_VALUE v);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
int macro_isParameterStringType(unsigned char typ) {
	return typ == C_PUSH_STRING_LITERAL || typ == C_PUSH_VARIABLE;
}

static void interpreter_printString(char* pTargetBuf, size_t nSize, PKS_VALUE nValue) {
	if (nValue.pkv_type == VT_FLOAT) {
		sprintf(pTargetBuf, "%.2lf", nValue.pkv_data.doubleValue);
	}
	else if (nValue.pkv_type == VT_CHAR) {
		sprintf(pTargetBuf, "%c", nValue.pkv_data.uchar);
	}
	else if (nValue.pkv_type == VT_BOOLEAN) {
		sprintf(pTargetBuf, "%s", nValue.pkv_data.booleanValue ? "true" : "false");
	}
	else if (nValue.pkv_type == VT_STRING) {
		const char* pszS = memory_accessString(nValue);
		size_t nLen = memory_size(nValue);
		if (nLen +3 < nSize) {
			strcpy(pTargetBuf, pszS);
		}
	}
	else if (nValue.pkv_type == VT_OBJECT_ARRAY) {
		interpreter_asString(pTargetBuf, &pTargetBuf[nSize], nValue);
	}
	else {
		sprintf(pTargetBuf, "%lld", nValue.pkv_data.longValue);
	}
}

static void interpreter_asString(char* pBuf, char* pBufEnd, PKS_VALUE v) {
	size_t nSize = memory_size(v);
	int i = 0;
	while (i < nSize && pBuf < pBufEnd - 4) {
		if (i > 0) {
			*pBuf++ = ',';
			*pBuf++ = ' ';
		}
		PKS_VALUE nNested = memory_getNestedObject(v, i);
		char buf[200];
		interpreter_printString(buf, sizeof buf, nNested);
		size_t len = strlen(buf);
		if (pBuf + len > pBufEnd - 1) {
			if (pBuf + 3 < pBufEnd - 1) {
				strcpy(pBuf, "...");
			}
			break;
		}
		strcpy(pBuf, buf);
		pBuf += len;
		i++;
	}
	*pBuf = 0;
}

/*
 * Generic coercion function to coerce PKS Edit values.
 */
PKS_VALUE interpreter_coerce(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue, PKS_VALUE_TYPE tTargetType) {
	if (nValue.pkv_type == tTargetType) {
		return nValue;
	}
	if (tTargetType == VT_FLOAT) {
		if (nValue.pkv_type == VT_STRING) {
			double d;
			if (sscanf(memory_accessString(nValue), "%lf", &d) < 1) {
				d = 0;
			}
			return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.doubleValue = d };
		}
		return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.doubleValue = (double)nValue.pkv_data.longValue };
	}
	if (tTargetType == VT_NUMBER) {
		if (nValue.pkv_type == VT_STRING) {
			return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.longValue = number((char*)memory_accessString(nValue)) };
		}
		if (nValue.pkv_type == VT_FLOAT) {
			return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.longValue = (long long)nValue.pkv_data.doubleValue };
		}
		return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.longValue = nValue.pkv_data.longValue };
	}
	if (tTargetType == VT_BOOLEAN) {
		if (types_isHandleType(nValue.pkv_type)) {
			return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.booleanValue = memory_handleForValue(nValue) != 0 };
		}
		return (PKS_VALUE) { .pkv_type = tTargetType, .pkv_data.booleanValue = nValue.pkv_data.longValue != 0 };
	}
	if (tTargetType == VT_STRING) {
		char buf[200];
		interpreter_printString(buf, sizeof buf, nValue);
		return interpreter_allocateString(pContext, buf);
	}
	return nValue;
}

/*
 * strmatch()
 */
static int strmatch(const char *s1,const char *s2) {	
	char ebuf[512];
	ebuf[0] = 0;
	const char *eol;
	RE_PATTERN pattern;
	RE_OPTIONS options = {
		.eof = 0,
		.patternBuf = ebuf,
		.endOfPatternBuf = &ebuf[sizeof ebuf],
		.flags = RE_DOREX,
		.expression = (char*)s2
	};

	eol = &s1[strlen(s1)];
	if (regex_compile(&options,&pattern) == 0) {
		interpreter_raiseError("Illegal regular expression %s", s2);
		return -1;
	}
	RE_MATCH match;

	return regex_match(&pattern, s1, eol, &match) &&
	    match.loc1 == s1 && 
		match.loc2 == eol;
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
		v1 = (PKS_VALUE){.pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = v1.pkv_data.longValue != 0};
		v2 = v1;
	}
	if (v1.pkv_type != VT_STRING && !CT_IS_LOGICAL(op)) {
		if (v1.pkv_type == v2.pkv_type && types_isHandleType(v1.pkv_type)) {
			void* p1 = memory_handleForValue(v1);
			void* p2 = memory_handleForValue(v2);
			if (op == CT_EQ) {
				bResult = p1 == p2;
			} else if (op == CT_NE) {
				bResult = p1 != p2;
			} else {
				goto notimpl;
			}
		} else {
			long long r1, r2;
			v1 = interpreter_coerce(pContext, v1, VT_NUMBER);
			v2 = interpreter_coerce(pContext, v2, VT_NUMBER);
			r1 = v1.pkv_data.longValue;
			r2 = v2.pkv_data.longValue;
			switch(op) {
				case CT_EQ: bResult = r1 == r2; break;
				case CT_NE: bResult = r1 != r2; break;
				case CT_GE: bResult = r1 >= r2; break;
				case CT_GT: bResult = r1 > r2; break;
				case CT_LT: bResult = r1 < r2; break;
				case CT_LE: bResult = r1 <= r2; break;
				default   : goto notimpl;
			}
		}
	} else if (v1.pkv_type == VT_STRING) {
		int r1;
		v1 = interpreter_coerce(pContext, v1, VT_STRING);
		v2 = interpreter_coerce(pContext, v2, VT_STRING);
		unsigned const char* s1, * s2;
		s1 = memory_accessString(v1);
		s2 = memory_accessString(v2);
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
		v1 = interpreter_coerce(pContext, v1, VT_BOOLEAN);
		BOOL bBool1 = v1.pkv_data.booleanValue;
		BOOL bBool2;
		if (!CT_IS_UNARY(op)) {
			v2 = interpreter_coerce(pContext, v2, VT_BOOLEAN);
			bBool2 = v2.pkv_data.booleanValue;
		}
		switch (op) {
		case CT_NOT: bResult = !bBool1; break;
		case CT_BRACKETS: bResult = bBool1;
		default: goto notimpl;
		}
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) {.pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = bResult});
	return 1;
}

/**
 * Evaluate expressions like 7 * "xy" or 4 * '\t'
 */
static void interpreter_evaluateMultiplicationWithStrings(EXECUTION_CONTEXT* pContext, PKS_VALUE v1, PKS_VALUE v2) {
	char *buf;
	v1 = interpreter_coerce(pContext, v1, VT_NUMBER);
	int l1 = v1.pkv_data.intValue;
	int nMult = v2.pkv_type == VT_CHAR ? 1 : (int)memory_size(v2);
	buf = calloc(1, (size_t)l1*nMult+5);
	if (!buf) {
		interpreter_raiseError("out of memory");
		return;
	}
	char* d = buf;
	for (int i = 0; i < l1; i++) {
		if (v2.pkv_type == VT_CHAR) {
			*d++ = v2.pkv_data.uchar;
		}
		else {
			strcpy(d, memory_accessString(v2));
			d += nMult;
		}
	}
	*d = 0;
	interpreter_pushValueOntoStack(pContext, interpreter_allocateString(pContext, buf));
	free(buf);
}

/*
 * Create a range.
 */
static void interpreter_createRange(EXECUTION_CONTEXT* pContext, PKS_VALUE v1, PKS_VALUE v2) {
	int nEnd = interpreter_coerce(pContext, v2, VT_NUMBER).pkv_data.intValue;
	if (v1.pkv_type == VT_RANGE) {
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .pkv_type = VT_RANGE, .pkv_data.range.r_start = v1.pkv_data.range.r_start, 
			.pkv_data.range.r_end = v1.pkv_data.range.r_end, .pkv_data.range.r_increment = nEnd });
	}
	else {
		int nStart = interpreter_coerce(pContext, v1, VT_NUMBER).pkv_data.intValue;
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .pkv_type = VT_RANGE, .pkv_data.range.r_start = nStart, .pkv_data.range.r_end = nEnd });
	}
}

/*
 * interpreter_extractArrayElementsAndPush
 */
static void interpreter_extractArrayElementsAndPush(EXECUTION_CONTEXT* pContext, int bOperation, PKS_VALUE vSource, PKS_VALUE vIndex) {
	size_t nMax = memory_size(vSource);
	if (bOperation == BIN_ADD) {
		if (vIndex.pkv_type == VT_OBJECT_ARRAY) {
			nMax = (int)memory_size(vIndex);
			for (int i = 0; i < nMax; i++) {
				memory_addObject(pContext, &vSource, memory_getNestedObject(vIndex, i));
			}
		} else {
			memory_addObject(pContext, &vSource, vIndex);
		}
		interpreter_pushValueOntoStack(pContext, vSource);
		return;
	}
	if (vIndex.pkv_type == VT_RANGE) {
		int idxStart = vIndex.pkv_data.range.r_start;
		int idxEnd = vIndex.pkv_data.range.r_end;
		int iIncr = vIndex.pkv_data.range.r_increment;
		if (iIncr <= 0) {
			iIncr = 1;
		}
		PKS_VALUE vResult = memory_createObject(pContext, VT_OBJECT_ARRAY, (idxEnd-idxStart)/iIncr +1, 0);
		while (idxStart <= idxEnd && idxStart < nMax) {
			memory_addObject(pContext, &vResult, memory_getNestedObject(vSource, idxStart));
			idxStart += iIncr;
		}
		interpreter_pushValueOntoStack(pContext, vResult);
		return;
	}
	else {
		vIndex = interpreter_coerce(pContext, vIndex, VT_NUMBER);
		int nIndex = vIndex.pkv_data.intValue;
		if (nIndex >= 0 && nIndex < nMax) {
			interpreter_pushValueOntoStack(pContext, memory_getNestedObject(vSource, nIndex));
			return;
		}
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { 
			.pkv_type = VT_NUMBER, .pkv_data.intValue = 0 });
}

/*--------------------------------------------------------------------------
 * interpreter_evaluateBinaryExpression()
 */
void interpreter_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp) {	
	PKS_VALUE v1;
	PKS_VALUE v2;
	unsigned char op;
	char buf[1024];

	if (!sp) {
		return;
	}
	v1 = interpreter_popStackValue(pContext);
	v2 = (PKS_VALUE) {
		.pkv_type = VT_NIL

	};
	op = sp->op;
	if (!IS_UNARY_OPERATOR(op)) {
		v2 = v1;
		v1 = interpreter_popStackValue(pContext);
	}

	if (op == BIN_CAST) {
		if (sp->targetType == v1.pkv_type) {
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
	if (op == BIN_MUL && (v2.pkv_type == VT_CHAR || v2.pkv_type == VT_STRING)) {
		interpreter_evaluateMultiplicationWithStrings(pContext, v1, v2);
		return;
	}
	if (op == BIN_AT && v2.pkv_type == VT_NUMBER && v1.pkv_type != VT_STRING) {
		int nIndex = v2.pkv_data.intValue;
		interpreter_pushValueOntoStack(pContext, memory_getNestedObject(v1, nIndex));
		return;
	}
	if (v1.pkv_type == VT_OBJECT_ARRAY && ((op == BIN_AT && (v2.pkv_type == VT_NUMBER || v2.pkv_type == VT_RANGE)) || (op == BIN_ADD))) {
		interpreter_extractArrayElementsAndPush(pContext, op, v1, v2);
		return;
	}
	if (v1.pkv_type == VT_MAP && op == BIN_AT && v2.pkv_type == VT_STRING) {
		interpreter_pushValueOntoStack(pContext, memory_atObject(v1, v2));
		return;
	}
	if (v1.pkv_type != VT_STRING && v2.pkv_type != VT_STRING) {
		if (v1.pkv_type == VT_FLOAT || v2.pkv_type == VT_FLOAT) {
			v1 = interpreter_coerce(pContext, v1, VT_FLOAT);
			v2 = interpreter_coerce(pContext, v2, VT_FLOAT);
			// one operand at least is numeric - force numeric calculations
			double d1 = v1.pkv_data.doubleValue;
			double d2 = v2.pkv_data.doubleValue;
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
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) {.pkv_type = VT_FLOAT, .pkv_data.doubleValue = d1});
			return;
		}
		v1 = interpreter_coerce(pContext, v1, VT_NUMBER);
		v2 = interpreter_coerce(pContext, v2, VT_NUMBER);
		long long		r1;
		long long		r2;
		// one operand at least is numeric - force numeric calculations
		r1 = v1.pkv_data.longValue;
		r2 = v2.pkv_data.longValue;
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
		interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = r1 });
		return;
	}
	unsigned const char* p1;
	unsigned const char* p2;
	v1 = interpreter_coerce(pContext, v1, VT_STRING);
	p1 = memory_accessString(v1);
	if (op == BIN_AT) {
		size_t nLen = strlen(p1);
		if (v2.pkv_type == VT_RANGE) {
			int n1 = v2.pkv_data.range.r_start;
			int n2 = v2.pkv_data.range.r_end;
			if (n1 < 0 || n1 > n2 || n2 > nLen) {
				interpreter_raiseError("Illegal range %d, %d for string %s", n1, n2, p1);
			}
			nLen = (size_t) n2 - n1 + 1;
			if (nLen >= sizeof buf - 2) {
				interpreter_raiseError("At operation fails due to lack of memory.");
			}
			strncpy(buf, &p1[n1], nLen);
			buf[nLen] = 0;
			interpreter_pushValueOntoStack(pContext, interpreter_allocateString(pContext, buf));
		} else {
			v2 = interpreter_coerce(pContext, v2, VT_NUMBER);
			int nIndex = v2.pkv_data.intValue;
			if (nIndex < 0 || nIndex >= nLen) {
				interpreter_raiseError("Index %d out of range for string %s", nIndex, p1);
			}
			char nResult = p1[nIndex];
			interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .pkv_type = VT_CHAR, .pkv_data.uchar = nResult });
		}
		return;
	}
	v2 = interpreter_coerce(pContext, v2, VT_STRING);
	p2 = memory_accessString(v2);
	*buf = 0;
	switch(op) {

	case BIN_ADD: {
		const char* pszP1 = p1 ? p1 : "null";
		const char* pszP2 = p2 ? p2 : "null";
		if (strlen(pszP1) + strlen(pszP2) >= sizeof buf) {
			interpreter_raiseError("+ operation on strings: result to large");
		}
		else {
			strcat(strcpy(buf, pszP1), pszP2);
		}
		break;
	}
	case BIN_SUB: {
		const char* pszP1 = p1 ? p1 : "null";
		const char* pszP2 = p2 ? p2 : "null";
		if (strlen(pszP1) > sizeof buf) {
			interpreter_raiseError("- operation on strings: result to large");
		}
		else {
			strcpy(buf, pszP1);
			if ((p1 = strstr(buf, pszP2)) != 0) {
				strcpy((char*)p1, p1 + strlen(pszP2));
			}
		}
		break;
	}
	default: 
		interpreter_raiseError("Binary operation %c on string arguments not implemented.",op);

	}
	interpreter_pushValueOntoStack(pContext, interpreter_allocateString(pContext, buf));
}

