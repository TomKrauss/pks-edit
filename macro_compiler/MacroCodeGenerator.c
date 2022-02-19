/*
 * MacroCodeGenerator.c
 *
 * generate code for macros
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

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"

extern void 	yyerror(char *s, ...);
extern int 		macro_insertNewMacro(char *name, char *comment, char *macdata, int size);
extern int 		macro_isParameterStringType(unsigned char typ);
extern void 	freeval(TYPEDVAL *v);

/*
 * bytecode_emitInstruction()
 */
static COM_FORM *_lastformstart;
unsigned char *bytecode_emitInstruction(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data) {	
	long s;
	unsigned char *spret;
	unsigned char* sp = pBuffer->bb_current;

	if (sp == 0) {
		yyerror(/*STR*/"adding to zero buffer?");
		return 0;
	}

	/*
	 * pack longs
	 */
	if (typ == C_LONG_LITERAL || typ == C_INTEGER_LITERAL) {
		if (data.val >= 0 && data.val <= 255)
			typ = C_CHARACTER_LITERAL;
	}

	s = macro_getParameterSize(typ, data.string);
	if ((spret = sp+s) > pBuffer->bb_end) {
		yyerror(/*STR*/"buffer overflow");
		return 0;
	}

	switch(typ) {
		case C_STOP:
		case C_0FUNC:
		case C_CHARACTER_LITERAL: 
		case C_FURET:
			((COM_CHAR1 *)sp)->val = data.uchar;
			break;
		case C_FORMSTART:
			_lastformstart = (COM_FORM *)sp;
			_lastformstart->options = data.uchar;
			_lastformstart->nfields = 0;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)data.val;
			break;
		case C_INTEGER_LITERAL:
			((COM_INT1 *)sp)->val = (int)data.intValue;
			break;
		case C_FLOAT_LITERAL:
			((COM_FLOAT1*)sp)->val = data.doubleValue;
			break;
		case C_TEST:
			((COM_TEST *)sp)->testop = data.uchar;
			((COM_TEST *)sp)->size = sizeof(COM_TEST);
			break;
		case C_GOTO:
			((COM_GOTO *)sp)->bratyp = data.uchar;
			((COM_GOTO *)sp)->offset = sizeof(COM_GOTO);
			break;
		case C_LONG_LITERAL:
			((COM_LONG1 *)sp)->val = data.longValue;
			break;
		case C_STRINGVAR:
		case C_LONGVAR:
		case C_FLOATVAR:
		case C_MACRO:
			strcpy(((COM_MAC*)sp)->name, data.string);
			break;
		case C_STRING_LITERAL:
			if (data.string)
				strcpy(((COM_STRING1*)sp)->s, data.string);
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
 * bytecode_emitTypedAssignment()
 */
static void bytecode_emitTypedAssignment(BYTECODE_BUFFER* pBuffer, int comType, const char *name, int typ, GENERIC_DATA data) {
	unsigned char *	p1;
	COM_ASSIGN *	ap;

	ap = (COM_ASSIGN*)pBuffer->bb_current;
	ap->typ = comType;
	p1 = ap->name;
	while(*name) {
		*p1++ = *name++;
	}
	*p1++ = 0;
	if ((short)p1 & 1) {
		p1++;
	}
	pBuffer->bb_current = p1;

	pBuffer->bb_current = bytecode_emitInstruction(pBuffer,typ,data);
	ap->opoffset = (int)(p1 - (unsigned char*)ap);
	ap->size = (int)(pBuffer->bb_current - (unsigned char*)ap);
}

/*--------------------------------------------------------------------------
 * bytecode_emitAssignment()
 */
void bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name, PKS_VALUE_TYPE typ, GENERIC_DATA data) {
	bytecode_emitTypedAssignment(pBuffer, C_ASSIGN, name, typ, data);
}

/*--------------------------------------------------------------------------
 * bytecode_defineVariable()
 * Defines a variable or a parameter depending on nBytecode
 */
void bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int nSymbolType, intptr_t val) {
	unsigned char *	p1;
	COM_CREATESYM *	ap;

	ap = (COM_CREATESYM*)pBuffer->bb_current;
	ap->typ = nBytecode;
	ap->symtype = nSymbolType;
	ap->value = (long)val;

	p1 = ap->name;
	while(*name) {
		*p1++ = *name++;
	}
	*p1++ = 0;
	if ((short)p1 & 1) {
		p1++;
	}

	pBuffer->bb_current = p1;
	ap->size = (int)(pBuffer->bb_current - (unsigned char*)ap);
}

/*--------------------------------------------------------------------------
 * bytecode_emitBinaryOperation()
 */
extern int vname_count;
TYPEDVAL bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, TYPEDVAL*v1, TYPEDVAL*v2) {
	unsigned char *	p1;
	unsigned char *	p2;
	COM_BINOP *		bp;
	TYPEDVAL		ret;

	if (nOperationType == BIN_DIV || nOperationType == BIN_MOD) {
		if (v2->type == C_LONG_LITERAL && !v2->data.val) {
			yyerror("Division by zero.");
			return *v1;
		}
	}
	if (v1->type == C_LONG_LITERAL && v2->type == C_LONG_LITERAL) {
		switch (nOperationType) {
		case BIN_MUL: return (TYPEDVAL) { .type = C_LONG_LITERAL, .data.val = v1->data.val * v2->data.val };
		case BIN_ADD: return (TYPEDVAL) { .type = C_LONG_LITERAL, .data.val = v1->data.val + v2->data.val };
		case BIN_SUB: return (TYPEDVAL) { .type = C_LONG_LITERAL, .data.val = v1->data.val - v2->data.val };
		case BIN_MOD: return (TYPEDVAL) { .type = C_LONG_LITERAL, .data.val = v1->data.val % v2->data.val };
		case BIN_DIV:
			return (TYPEDVAL) { .type = C_LONG_LITERAL, .data.val = v1->data.val / v2->data.val };
		}
	}
	bp = (COM_BINOP*)pBuffer->bb_current;
	bp->typ = C_BINOP;
	bp->op = nOperationType;
	sprintf(bp->result,"__ret%d",vname_count++);
	ret.data.val = (intptr_t)bp->result;
	ret.type = (macro_isParameterStringType(v1->type) || !v2 || macro_isParameterStringType(v2->type)) ?
		C_STRINGVAR : ((macro_isParameterFloatType(v1->type) || (v2 && macro_isParameterFloatType(v2->type))) ? C_FLOATVAR : C_LONGVAR);
	p1 = bp->result;

	while(*p1++)
		;
	if ((short)p1 & 1) {
		p1++;
	}

	pBuffer->bb_current = p1;
	pBuffer->bb_current = p2 = bytecode_emitInstruction(pBuffer,v1->type,v1->data);
	freeval(v1);
	if (v2) {
		pBuffer->bb_current = bytecode_emitInstruction(pBuffer,v2->type,v2->data);
		freeval(v2);
	} else {
		pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_LONG_LITERAL, (GENERIC_DATA) { 0 });
	}

	bp->op1offset = (int)(p1 - (unsigned char*)bp);
	bp->op2offset = (int)(p2 - (unsigned char*)bp);
	bp->size = (int)(pBuffer->bb_current - (unsigned char*)bp);

	return ret;
}

/*
 * Multiply an expression and return the TYPEDVAL result
 */
TYPEDVAL bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber) {
	if (v1->type == C_LONG_LITERAL) {
		v1->data.val *= nNumber;
		return *v1;
	}
	if (v1->type == C_FLOAT_LITERAL) {
		v1->data.doubleValue *= nNumber;
		return *v1;
	}
	return bytecode_emitBinaryOperation(pBuffer, BIN_MUL, v1, &(TYPEDVAL){.type = C_LONGVAR, .data.val = nNumber});
}

void bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v, int nIncrement) {
	const char* pszName = (const char*) v->data.val;
	TYPEDVAL lResult = bytecode_emitBinaryOperation(pBuffer, BIN_ADD, v, &(TYPEDVAL){.type = C_LONG_LITERAL, .data.val = nIncrement});
	bytecode_emitAssignment(pBuffer, pszName, v->type, lResult.data);
}
