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
#include "arraylist.h"
#include "scanner.h"

extern void 		yyerror(char *s, ...);
extern int 			macro_isParameterStringType(unsigned char typ);
extern char*		bytecode_generateAutoLabelName(char* prefix, int num);
extern void			bytecode_destroyLabelNamed(char* name);
extern int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char* name);
extern void			bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char* prefix, int level, int bratyp);
extern char*		bytecode_emitGotoLabelInstruction(char* name, BYTECODE_BUFFER* pBuffer, int branchType);

typedef struct tagSWITCH_LABEL {
	int  sl_index;
	PKS_VALUE sl_value;
} SWITCH_LABEL;

static SWITCH_LABEL	_switchValues[32];
static int			_currentSwitchValue = -1;

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

	if (typ == C_PUSH_ARRAY_LITERAL  || typ == C_PUSH_MAP_LITERAL) {
		ARRAY_LIST* pList = data.stringList;
		size_t nElements = arraylist_size(pList);
		COM_ARRAYLITERAL* pLiteral = (COM_ARRAYLITERAL * )sp;
		spret = (char*)((COM_ARRAYLITERAL*)sp)->strings;
		pLiteral->length = (int)nElements;
		for (int i = 0; i < nElements; i++) {
			TYPED_OBJECT_POINTER top = (TYPED_OBJECT_POINTER) arraylist_get(pList, i);
			PKS_VALUE_TYPE pType = TOP_TYPE(top);
			*spret++ = pType;
			if (pType == VT_STRING) {
				char* pString = (char*)TOP_DATA_POINTER(top);
				size_t nLen = strlen(pString);
				strcpy(spret, pString);
				spret += nLen;
				*spret++ = 0;
			} else if (pType == VT_NUMBER || pType == VT_CHAR || pType == VT_BOOLEAN) {
				long lValue = (long)(uintptr_t)TOP_DATA_POINTER(top);
				memcpy(spret, &lValue, sizeof(lValue));
				spret += sizeof(lValue);
			}
		}
		s = (int)(spret - sp);
		pLiteral->totalBytes = s;
	} else if (typ == C_MACRO_REF_LOCAL) {
		s = (long)(sizeof(COM_MAC));
	} else if (typ == C_MACRO || typ == C_MACRO_REF) {
		if (data.string == 0) {
			yyerror("Illegal %s without name.", typ == C_MACRO ? "macro call" : "eval call");
			return sp;
		}
		s = (long)(strlen(data.string) + sizeof(COM_MAC));
	} else {
		s = interpreter_getParameterSize(typ, data.string);
	}
	if ((spret = sp+s) > pBuffer->bb_end) {
		yyerror(/*STR*/"Code generation buffer overflow. Macro to big.");
		return pBuffer->bb_end;
	}

	switch(typ) {
		case C_PUSH_MAP_LITERAL:
		case C_PUSH_ARRAY_LITERAL:
			break;
		case C_STOP:
		case C_ASSIGN_SLOT:
		case C_SET_STACKFRAME:
		case C_PUSH_BOOLEAN_LITERAL:
		case C_ASSIGN_LOCAL_VAR:
		case C_PUSH_LOCAL_VARIABLE:
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
		case C_PUSH_NEW_INSTANCE:
			((COM_INT1*)sp)->c_valueType = (int)data.intValue;
			break;
		case C_PUSH_INTEGER_LITERAL:
			((COM_INT1 *)sp)->val = (int)data.intValue;
			break;
		case C_PUSH_FLOAT_LITERAL:
			((COM_FLOAT1*)sp)->val = data.doubleValue;
			break;
		case C_GOTO:
			((COM_GOTO *)sp)->branchType = data.uchar;
			((COM_GOTO *)sp)->offset = sizeof(COM_GOTO);
			break;
		case C_PUSH_LONG_LITERAL:
			((COM_LONG1 *)sp)->val = data.longValue;
			break;
		case C_ASSIGN:
		case C_PUSH_VARIABLE:
			strcpy(((COM_STRING1*)sp)->s, data.string);
			break;
		case C_MACRO_REF_LOCAL:
			((COM_MAC*)sp)->heapIndex = data.intValue;
			break;
		case C_MACRO_REF:
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
 * bytecode_emitLocalAssignment()
 */
unsigned char* bytecode_emitLocalAssignment(BYTECODE_BUFFER* pBuffer, int nHeapIndex) {
	return bytecode_emitInstruction(pBuffer, C_ASSIGN_LOCAL_VAR, (GENERIC_DATA) { .intValue = nHeapIndex });
}

/*---------------------------------*/
/* bytecode_generateAutoLabelNamePrefix()					*/
/*---------------------------------*/
int bytecode_generateAutoLabelNamePrefix(BYTECODE_BUFFER* pBuffer, char* prefix, int level)
{
	char* name = bytecode_generateAutoLabelName(prefix, level);

	return bytecode_createBranchLabel(pBuffer, name);
}

/*---------------------------------*/
/* bytecode_destroyAutoLabelNamePrefix()					*/
/*---------------------------------*/
void bytecode_destroyAutoLabelNamePrefix(char* prefix, int level) {
	char* name = bytecode_generateAutoLabelName(prefix, level);

	/*
	 * autolabel is not used any more: kill it
	 */
	bytecode_destroyLabelNamed(name);
}

/*---------------------------------*/
/* bytecode_emitGotoInstruction()					*/
/*---------------------------------*/
void bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char* prefix, int level, int bratyp) {
	char* name = bytecode_generateAutoLabelName(prefix, level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		pBuffer->bb_current = bytecode_emitGotoLabelInstruction(name, pBuffer, bratyp);
}


static char* _caseLabelId = "%case%";

/*
 * Calculate the number range of a value.
 */
static void bytecode_calculateRange(PKS_VALUE_TYPE t, GENERIC_DATA data, int* pLow, int* pHigh) {
	if (t == VT_NUMBER) {
		*pLow = *pHigh = data.intValue;
	}
	else if (t == VT_RANGE) {
		*pLow = data.range.r_start;
		*pHigh = data.range.r_end;
	}
}
/*
 * Add a switch case condition to the current switch table being constructed.
 */
void bytecode_addSwitchCondition(BYTECODE_BUFFER* pBuffer, int aLevel, PKS_VALUE_TYPE t, GENERIC_DATA data) {
	if (_currentSwitchValue >= DIM(_switchValues)) {
		yyerror("Too many case labels");
		return;
	}
	bytecode_generateAutoLabelNamePrefix(pBuffer, _caseLabelId, _currentSwitchValue);
	int i = 0;
	for (; i < _currentSwitchValue; i++) {
		PKS_VALUE vExist = _switchValues[i].sl_value;
		if (t == VT_STRING) {
			if (vExist.pkv_type == VT_STRING && strcmp(vExist.pkv_data.string, data.string) == 0) {
				yyerror("Illegal redefinition of case label value %s", data.string);
			}
		} else {
			int nExistLow;
			int nExistHigh;
			int nNewLow;
			int nNewHigh;
			bytecode_calculateRange(vExist.pkv_type, vExist.pkv_data, &nExistLow, &nExistHigh);
			bytecode_calculateRange(t, data, &nNewLow, &nNewHigh);
			if ((nNewLow >= nExistLow && nNewLow <= nExistHigh) || (nNewHigh >= nExistLow && nNewHigh <= nExistHigh))  {
				yyerror("Illegal redefinition of case label value range %d..%d", nNewLow, nNewHigh);
			}
		}
		if (vExist.pkv_type == VT_NIL) {
			// Insert switch condition always before default condition
			break;
		}
	}
	if (i < _currentSwitchValue) {
		memmove(&_switchValues[i + 1], &_switchValues[i], (_currentSwitchValue - i) * sizeof _switchValues[0]);
	}
	_switchValues[i] = (SWITCH_LABEL){ .sl_index = _currentSwitchValue, .sl_value.pkv_type = t, .sl_value.pkv_data = data };
	_currentSwitchValue++;
}

void bytecode_startSwitchTable(int aLevel) {
	if (_currentSwitchValue >= 0) {
		yyerror("Nested switch expressions currently not supported");
		return;
	}
	_currentSwitchValue = 0;
}

/*
 * Write out the recorded case jumps and cleanup switch table.
 */
void bytecode_flushSwitchTable(BYTECODE_BUFFER* pBuffer, int aLevel) {

	for (int i = 0; i < _currentSwitchValue; i++) {
		PKS_VALUE v = _switchValues[i].sl_value;
		int nLabelIndex = _switchValues[i].sl_index;
		if (v.pkv_type == VT_NIL) {
			// default case
			bytecode_emitGotoInstruction(pBuffer, _caseLabelId, nLabelIndex, BRA_ALWAYS);
		} else {
			if (v.pkv_type == VT_RANGE) {
				// TODO: we could introduce a PUS_RANGE Operation to handle this more efficiently.
				pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_INTEGER_LITERAL, (GENERIC_DATA) { .intValue = v.pkv_data.range.r_start });
				pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_INTEGER_LITERAL, (GENERIC_DATA) { .intValue = v.pkv_data.range.r_end });
				pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_BINOP, (GENERIC_DATA) { BIN_RANGE });
			}
			else {
				pBuffer->bb_current = bytecode_emitInstruction(pBuffer, v.pkv_type == VT_STRING ? C_PUSH_STRING_LITERAL : C_PUSH_INTEGER_LITERAL, v.pkv_data);
			}
			if (v.pkv_type == VT_STRING) {
				free(v.pkv_data.string);
			}
			bytecode_emitGotoInstruction(pBuffer, _caseLabelId, nLabelIndex, BRA_CASE);

		}
		bytecode_destroyAutoLabelNamePrefix(_caseLabelId, i);
	}
	_currentSwitchValue = -1;
}

/*
 * Destroy an array list containing TYPE_OBJECT_POINTERs.
 */
void bytecode_destroyArraylistWithPointers(ARRAY_LIST* pList) {
	if (!pList) {
		return;
	}
	ARRAY_ITERATOR iterator = arraylist_iterator(pList);
	void** p = iterator.i_buffer;
	while (p < iterator.i_bufferEnd) {
		TYPED_OBJECT_POINTER pszPointer = (TYPED_OBJECT_POINTER) * p++;
		if (TOP_IS_POINTER(pszPointer)) {
			free(TOP_DATA_POINTER(pszPointer));
		}
	}
	arraylist_destroy(pList);
}
/*--------------------------------------------------------------------------
 * bytecode_defineVariable()
 * Defines a variable or a parameter depending on nBytecode
 */
void bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, PKS_VALUE_TYPE nType, unsigned short nArraySizeOrParamIndex, int nHeapIndex) {
	unsigned char *	p1;
	COM_DEFINE_SYMBOL *	ap;

	ap = (COM_DEFINE_SYMBOL*)pBuffer->bb_current;
	ap->typ = nBytecode;
	ap->vartype = nType;
	ap->value = nArraySizeOrParamIndex;
	ap->heapIndex = nHeapIndex;
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

unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nHeapIndex, int bLocalVar, int nIncrement) {
	if (bLocalVar) {
		pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_LOCAL_VARIABLE, (GENERIC_DATA) { .intValue = nHeapIndex });
	} else {
		pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_VARIABLE, (GENERIC_DATA) { .string = pszName });
	}
	pBuffer->bb_current = bytecode_emitInstruction(pBuffer, C_PUSH_INTEGER_LITERAL, (GENERIC_DATA) { .intValue = nIncrement });
	pBuffer->bb_current = bytecode_emitBinaryOperation(pBuffer, BIN_ADD, VT_NUMBER);
	if (bLocalVar) {
		return bytecode_emitLocalAssignment(pBuffer, nHeapIndex);
	}
	return bytecode_emitAssignment(pBuffer, pszName);
}

static int bytecode_getPushInt(char* pInstr, int* pVal) {
	if (*pInstr == C_PUSH_SMALL_INT_LITERAL) {
		*pVal = ((COM_CHAR1*)pInstr)->val;
		return 1;
	}
	if (*pInstr == C_PUSH_INTEGER_LITERAL) {
		*pVal = ((COM_INT1*)pInstr)->val;
		return 1;
	}
	return 0;
}

/*
 * Eliminate unneccessary instructions from the byte codes generated.
 * Should be done on a long term run based on an intermediate AST created by the parser.
 */
void bytecode_optimizeInstructions(BYTECODE_BUFFER* pBuffer) {
	int bOptimizeMore = 1;

	while (bOptimizeMore) {
		bOptimizeMore = 0;
		char* pSource = pBuffer->bb_start;
		char* pTarget = pBuffer->bb_start;
		while (pSource < pBuffer->bb_current) {
			char* pI2;
			char* pI3;
			int v1;
			int v2;
			size_t nSize = interpreter_getParameterSize(*pSource, pSource + 1);
			pI2 = pSource + nSize;
			if (pI2 < pBuffer->bb_current) {
				pI3 = pI2 + interpreter_getParameterSize(*pI2, pI2 + 1);
				if (bytecode_getPushInt(pSource, &v1) && bytecode_getPushInt(pI2, &v2) && *pI3 == C_BINOP) {
					COM_BINOP* pOp = (COM_BINOP*)pI3;
					int nVal;
					int bOptimize = 1;
					if (pOp->op == BIN_OR) {
						nVal = v1 | v2;
					}
					else if (pOp->op == BIN_ADD) {
						nVal = v1 + v2;
					}
					else if (pOp->op == BIN_AND) {
						nVal = v1 & v2;
					}
					else if (pOp->op == BIN_SHIFT_LEFT) {
						nVal = v1 << v2;
					}
					else if (pOp->op == BIN_SHIFT_RIGHT) {
						nVal = v1 >> v2;
					}
					else if (pOp->op == BIN_DIV && v2) {
						nVal = v1 / v2;
					}
					else if (pOp->op == BIN_MOD && v2) {
						nVal = v1 % v2;
					}
					else if (pOp->op == BIN_MUL) {
						nVal = v1 * v2;
					}
					else {
						bOptimize = 0;
					}
					if (bOptimize) {
						if (nVal < 256) {
							*pTarget++ = C_PUSH_SMALL_INT_LITERAL;
							*pTarget++ = nVal;
						}
						else {
							*pTarget = C_PUSH_INTEGER_LITERAL;
							((COM_INT1*)pTarget)->val = (int)nVal;
							pTarget += interpreter_getParameterSize(*pTarget, pTarget + 1);
						}
						pSource = pI3 + interpreter_getParameterSize(*pI3, pI3 + 1);
						if (pSource < pBuffer->bb_current && (*pSource == C_PUSH_SMALL_INT_LITERAL || *pSource == C_PUSH_INTEGER_LITERAL)) {
							bOptimizeMore = 1;
						}
						continue;
					}
				}
			}
			if (pTarget != pSource) {
				memcpy(pTarget, pSource, nSize);
			}
			pSource += nSize;
			pTarget += nSize;
		}
		pBuffer->bb_current = pTarget;
	}
}


