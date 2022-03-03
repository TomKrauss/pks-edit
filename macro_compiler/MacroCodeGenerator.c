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
extern int 		macro_isParameterStringType(unsigned char typ);

/*
 * bytecode_emitInstruction()
 */
static COM_FORM *_lastformstart;
unsigned char *bytecode_emitInstruction(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data) {	
	long s;
	unsigned char *spret;
	unsigned char* sp = pBuffer->bb_current;

	if (sp == 0) {
		yyerror("Illegal expression outside the context of a macro definition.");
		return 0;
	}

	/*
	 * pack longs
	 */
	if (typ == C_PUSH_LONG_LITERAL || typ == C_PUSH_INTEGER_LITERAL) {
		if (data.longValue >= 0 && data.longValue <= 255)
			typ = C_PUSH_SMALL_INT_LITERAL;
	}

	if (typ == C_MACRO) {
		s = (long)(strlen(data.string) + sizeof(COM_MAC));
	} else {
		s = interpreter_getParameterSize(typ, data.string);
	}
	if ((spret = sp+s) > pBuffer->bb_end) {
		yyerror(/*STR*/"Code generation buffer overflow. Macro to big.");
		return 0;
	}

	switch(typ) {
		case C_STOP:
		case C_SET_STACKFRAME:
		case C_PUSH_CHARACTER_LITERAL: 
		case C_PUSH_SMALL_INT_LITERAL:
			((COM_CHAR1 *)sp)->val = data.uchar;
			break;
		case C_0FUNC:
			((COM_0FUNC*)sp)->funcnum = data.intValue;
			break;
		case C_FORM_START:
			_lastformstart = (COM_FORM *)sp;
			_lastformstart->options = data.uchar;
			_lastformstart->nfields = 0;
			break;
		case C_LOGICAL_OPERATION:
		case C_BINOP:
			((COM_BINOP*)sp)->op = data.intValue;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)data.val;
			break;
		case C_PUSH_INTEGER_LITERAL:
			((COM_INT1 *)sp)->val = (int)data.intValue;
			break;
		case C_PUSH_FLOAT_LITERAL:
			((COM_FLOAT1*)sp)->val = data.doubleValue;
			break;
		case C_GOTO:
			((COM_GOTO *)sp)->bratyp = data.uchar;
			((COM_GOTO *)sp)->offset = sizeof(COM_GOTO);
			break;
		case C_PUSH_LONG_LITERAL:
			((COM_LONG1 *)sp)->val = data.longValue;
			break;
		case C_ASSIGN:
		case C_PUSH_VARIABLE:
			strcpy(((COM_VAR*)sp)->name, data.string);
			break;
		case C_MACRO:
			strcpy(((COM_MAC*)sp)->name, data.string);
			break;
		case C_PUSH_STRING_LITERAL:
			if (data.string)
				strcpy(((COM_STRING1*)sp)->s, data.string);
			else
				((COM_STRING1*)sp)->s[0] = 0;
			break;
	}

	if (C_ISCMD(typ)) {
		_lastformstart = 0;
	}

	if (typ != C_FORM_START) {
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

/*
 * Emit a function call with a number of arguments.
 */
unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs) {
	unsigned char* pResult = bytecode_emitInstruction(pBuffer, typ, data);
	COM_0FUNC* pFunc = (COM_0FUNC*)pBuffer->bb_current;
	pFunc->func_nargs = nArgs;
	return pResult;
}

/*--------------------------------------------------------------------------
 * bytecode_emitAssignment()
 */
unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name) {
	return bytecode_emitInstruction(pBuffer, C_ASSIGN, (GENERIC_DATA) { .string = (char*)name });
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
	ap->value = (long long)val;

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
unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType) {
	unsigned char* pResult = bytecode_emitInstruction(pBuffer, C_BINOP, (GENERIC_DATA) {nOperationType});
	((COM_BINOP*)pBuffer->bb_current)->targetType = nTargetType;
	return pResult;
}

/*
 * Multiply an expression and return the TYPEDVAL result
 */
unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber) {
	pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_INTEGER_LITERAL, (GENERIC_DATA) { .longValue = nNumber });
	return bytecode_emitBinaryOperation(pBuffer, BIN_MUL, 0);
}

unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement) {
	pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_VARIABLE, (GENERIC_DATA) { .string=pszName });
	pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_INTEGER_LITERAL, (GENERIC_DATA) { .intValue = nIncrement });
	pBuffer->bb_current = bytecode_emitBinaryOperation(pBuffer, BIN_ADD, VT_NUMBER);
	return bytecode_emitAssignment(pBuffer, pszName);
}

unsigned char* bytecode_emitShorthandAssignment(BYTECODE_BUFFER* pBuffer, int nBinaryOp, char* pszName) {
	pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_VARIABLE, (GENERIC_DATA) { .string = pszName });
	pBuffer->bb_current = bytecode_emitBinaryOperation(pBuffer, nBinaryOp, VT_NUMBER);
	return bytecode_emitAssignment(pBuffer, pszName);
}
