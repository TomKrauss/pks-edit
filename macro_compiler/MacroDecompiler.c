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

#include "alloc.h"
#include "pksmacrocvm.h"
#include "pksmacro.h"
#include "funcdef.h"
#include "pksmod.h"
#include "symbols.h"
#include "test.h"
#include "funcdef.h"
#include "stringutil.h"
#include "errordialogs.h"

int  bytecode_makeAutoLabel(COM_GOTO *cp);
void bytecode_initializeAutoLabels(void);
void bytecode_startNextAutoLabel(char **name, COM_GOTO **cp);
void bytecode_closeAutoLabels(void);
char *bytecode_findAutoLabelForInstruction(COM_GOTO *cp);

typedef enum {
	CFM_IF,					// if () ...
	CFM_WHILE,				// while () ...
	CFM_ELSE,				// } else { ...
	CFM_CONTINUE,			// continue...
	CFM_BREAK,				// break...
	CFM_END_BLOCK,			// }
} CONTROL_FLOW_MARK;

typedef struct tagCONTROL_FLOW_MARK_INDEX {
	void *				cfmi_bytecodeAddress;		// Address in the bytecode
	CONTROL_FLOW_MARK	cfmi_mark;					// The marker associated dwith the offset.
} CONTROL_FLOW_MARK_INDEX;

typedef struct tagDECOMPILATION_STACK_ELEMENT {
	COM_0FUNC* dse_instruction;
	char* dse_printed;
} DECOMPILATION_STACK_ELEMENT;

/*
 * decompile_stringForTestOperator()
 */
static char *decompile_stringForTestOperator(unsigned char op)
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
 * decompile_getFunctionForIndex()
 */
static EDFUNC *decompile_getFunctionForIndex(int idx)
{
	if (idx >= 0 && idx < _functionTableSize) {
		return &_functionTable[idx];
	}
	return 0;
}


/*
 * decompile_quoteString()
 *
 * decompile_quoteString the following chars: <"\>
 */
static char *decompile_quoteString(unsigned char *name)
{	static char qbuf[1024];
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
 * decompile_printParameterAsConstant()
 */
static void decompile_printParameterAsConstant(char* pTargetString, long long val, PARAMETER_TYPE_DESCRIPTOR partyp) {
	PARAMETER_ENUM_VALUE	*ep,*epend;
	int printed = 0;

	ep = partyp.pt_enumVal;
	epend = partyp.pt_enumVal + partyp.pt_enumCount;
	pTargetString[0] = 0;
	if (partyp.pt_type == PARAM_TYPE_BITSET) {
		while(ep < epend) {
			if (val & ep->pev_val) {
				if (printed)
					strcat(pTargetString, "|");
				strcat(pTargetString, ep->pev_name);
				printed++;
				val &= (~ep->pev_val);
				if (!val)
					return;
			}
			ep++;
		}
	} else {
		/* OT_ENUM */
		while(ep < epend) {
			if (val == ep->pev_val) {
				strcpy(pTargetString, ep->pev_name);
				return;
			}
			ep++;
		}
	}
	if (val || !printed) {
		if (printed)
			strcat(pTargetString, "|");
		char szBuf[39];
		sprintf(szBuf,"%lld",val);
		strcat(pTargetString, szBuf);
	}
}

/*
 * decompile_printParameter()
 */
static int decompile_printParameter(char* pszBuf, unsigned char *sp, PARAMETER_TYPE_DESCRIPTOR partyp) {
	unsigned char typ;
	long long val;

	if (((intptr_t)sp) & 1)
		goto err;

	typ = ((COM_CHAR1 *)sp)->typ;
	switch(typ) {
		case C_FORM_START:
			val = ((COM_FORM *)sp)->options;
			break;
		case C_1FUNC:
			val = ((COM_1FUNC *)sp)->p;
			break;
		case C_PUSH_SMALL_INT_LITERAL:
		case C_PUSH_CHARACTER_LITERAL:
			val = ((COM_CHAR1 *)sp)->val;
			break;
		case C_PUSH_FLOAT_LITERAL:
			sprintf(pszBuf, "%lf", ((COM_FLOAT1*)sp)->val);
			return 1;
		case C_PUSH_BOOLEAN_LITERAL:
			sprintf(pszBuf, "%s", ((COM_FLOAT1*)sp)->val ? "true" : "false");
			return 1;
		case C_PUSH_INTEGER_LITERAL:
			val = ((COM_INT1 *)sp)->val;
			break;
		case C_PUSH_LONG_LITERAL:
			val = (long long)((COM_LONG1 *)sp)->val;
			break;
		case C_PUSH_STRING_LITERAL:
			sprintf(pszBuf, "\"%s\"", decompile_quoteString(((COM_STRING1*)sp)->s));
			return 1;
		case C_PUSH_VARIABLE:
			strcpy(pszBuf, ((COM_VAR*)sp)->name);
			return 1;
		default:
err:		error_displayAlertDialog("Format error in parameters of function to decompile. Bytecode is 0x%x", typ);
			return -1;
	}

	if (partyp.pt_type == PARAM_TYPE_STRING) {
		sprintf(pszBuf, "\"%d\"", ((COM_INT1*)sp)->val);
	} else if ((partyp.pt_type == PARAM_TYPE_ENUM || partyp.pt_type == PARAM_TYPE_BITSET) && partyp.pt_enumVal) {
		decompile_printParameterAsConstant(pszBuf, val, partyp);
		return 1;
	}

	if (typ == C_PUSH_CHARACTER_LITERAL) {
		char escape = 0;
		if ((char)val == '\\') {
			escape = '\\';
		} else if ((char)val == '\t') {
			escape = '\t';
		}
		else if ((char)val == '\r') {
			escape = 'r';
		}
		else if ((char)val == '\f') {
			escape = 'f';
		}
		else if ((char)val == '\a') {
			escape = 'a';
		}
		else if ((char)val == '\n') {
			escape = 'n';
		}
		if (escape) {
			sprintf(pszBuf, "'\\%c'", escape);
		} else {
			sprintf(pszBuf, "'%c'", (char)val);
		}
	} else {
		sprintf(pszBuf, "%lld", val);
	}
	return 1;
}

/*
 * decompile_printFunctionName
 */
static void decompile_printFunctionName(char* pszBuf, int idx)
{
	EDFUNC	*ep = decompile_getFunctionForIndex(idx);

	if (!ep) {
		error_displayAlertDialog("format error: bad function");
		return;
	}
	sprintf(pszBuf,"%s",macro_loadStringResource(idx));
}

static DECOMPILATION_STACK_ELEMENT* decompile_popStack(DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	if (pStackCurrent > pStack) {
		--pStackCurrent;
		free(pStackCurrent->dse_printed);
		pStackCurrent->dse_instruction = NULL;
		pStackCurrent->dse_printed = NULL;
	}
	return pStackCurrent;
}

static DECOMPILATION_STACK_ELEMENT* decompile_findParamStack(DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	while (--pStackCurrent >= pStack) {
		if (!pStackCurrent->dse_printed && pStackCurrent->dse_instruction && pStackCurrent->dse_instruction->typ == C_SET_PARAMETER_STACK) {
			return pStackCurrent;
		}
	}
	return pStack;
}
/*
 * decompile_function()
 */
static DECOMPILATION_STACK_ELEMENT* decompile_function(COM_1FUNC* sp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	EDFUNC	*		ep;
	int 			npars;
	PARAMETER_TYPE_DESCRIPTOR partyp;
	char szFunctionCall[512];
	char szParam[128];

	if (sp->typ == C_MACRO) {
		sprintf(szFunctionCall,"%s",((COM_MAC*)sp)->name);
		ep = NULL;
	} else {
		decompile_printFunctionName(szFunctionCall, sp->funcnum);
		ep = decompile_getFunctionForIndex(sp->funcnum);
	}
	strcat(szFunctionCall, "(");

	npars = 0;
	if (sp->typ == C_1FUNC) {
		partyp = function_getParameterTypeDescriptor(ep, 1);
		if (decompile_printParameter(szParam,(unsigned char*)sp,partyp) < 0)
			return 0;
		strcat(szFunctionCall, szParam);
		npars++;
	}

	DECOMPILATION_STACK_ELEMENT* pStackPointer = ep ? (pStackCurrent-function_getParameterCount(ep)) : decompile_findParamStack(pStackCurrent, pStack);
	if (pStackPointer < pStack) {
		pStackPointer = pStack;
	}
	DECOMPILATION_STACK_ELEMENT* pStackFrame = pStackPointer;
	while(pStackPointer < pStackCurrent) {
		if (!pStackPointer->dse_printed) {
			pStackPointer++;
			continue;
		}
		partyp = ep == NULL ? (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = PARAM_TYPE_VOID} : function_getParameterTypeDescriptor(ep, npars + 1);
		if (npars) {
			strcat(szFunctionCall, ",");
		}
		size_t nBytes = strlen(szFunctionCall);
		strncat(szFunctionCall, pStackPointer->dse_printed, sizeof(szFunctionCall) - 3 - nBytes);
		pStackPointer++;
		npars++;
	}
	while (pStackCurrent > pStackFrame) {
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
	}

	strcat(szFunctionCall, ")");
	pStackCurrent->dse_instruction = (COM_0FUNC*)sp;
	pStackCurrent->dse_printed = _strdup(szFunctionCall);
	pStackCurrent++;
	return pStackCurrent;
}

/**
 * Return a type name for a symbol type.
 */
static char* decompile_typenameFor(PKS_VALUE_TYPE nSymbolType) {
	switch (nSymbolType) {
	case VT_FLOAT: return "float";
	case VT_NUMBER: return "int";
	case VT_BOOLEAN: return "boolean";
	case VT_CHAR: return "char";
	default: return "string";
	}
}

/*
 * Return a string representation of a test instruction.
 */
static char* decompile_testOperationAsString(unsigned char op) {

	switch (op) {
	case CT_AND: return "and";
	case CT_OR: return "or";
	case CT_BRACKETS: return "()";
	case CT_EQ: return "equals";
	case CT_NE: return "not-equals";
	case CT_GE: return "greater-equals";
	case CT_GT: return "greater";
	case CT_LE: return "less-equals";
	case CT_LT: return "<";
	case CT_NMATCH: return "!matches";
	case CT_MATCH: return "matches";
	case CT_NOT: return "not";
	}
	return "??";
}

static BOOL decompile_expressionNeedsBrackets(COM_BINOP* cp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	if (pStackCurrent <= pStack) {
		return FALSE;
	}
	DECOMPILATION_STACK_ELEMENT* pPrevious = &pStackCurrent[-1];
	if (pStackCurrent > pStack && pPrevious->dse_instruction->typ == C_BINOP) {
		COM_BINOP* pExpr = (COM_BINOP * )pPrevious->dse_instruction;
		// TODO: other precedences to be handled here?
		if (pExpr->op == BIN_ADD || pExpr->op == BIN_SUB) {
			return TRUE;
		}
	}
	return FALSE;
}
/*
 * decompile_printBinaryExpression()
 */
static DECOMPILATION_STACK_ELEMENT* decompile_printTestExpression(COM_BINOP* cp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	char* pRight = pStackCurrent > pStack ? pStackCurrent[-1].dse_printed : "";
	char* pLeft = cp->op != CT_NOT && pStackCurrent > pStack-1 ? pStackCurrent[-2].dse_printed : "";
	char szBuf[512];

	szBuf[0] = 0;
	switch (cp->op) {
	default:
		sprintf(szBuf, "%s %s %s", pLeft, decompile_stringForTestOperator(cp->op), pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case CT_NOT:
		sprintf(szBuf, "!%s", pRight);
		break;
	}
	pStackCurrent = decompile_popStack(pStackCurrent, pStack);
	pStackCurrent->dse_instruction = (COM_0FUNC*)cp;
	pStackCurrent->dse_printed = _strdup(szBuf);
	pStackCurrent++;
	return pStackCurrent;
}

/*
 * decompile_printBinaryExpression()
 */
static DECOMPILATION_STACK_ELEMENT* decompile_printBinaryExpression(COM_BINOP *cp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	char* pRight = pStackCurrent > pStack ? pStackCurrent[-1].dse_printed : "";
	char* pLeft = cp->op != BIN_CAST && pStackCurrent > pStack-1 ? pStackCurrent[-2].dse_printed : "";
	char szBuf[512];
	char szBrackets[512];

	szBuf[0] = 0;
	char* pszShift = "<<";
	BOOL bNeedsBrackets = decompile_expressionNeedsBrackets(cp, pStackCurrent, pStack);
	if (bNeedsBrackets) {
		sprintf(szBrackets, "(%s)", pRight);
		pRight = szBrackets;
	}
	switch(cp->op) {
	case BIN_ADD: 
	case BIN_SUB:
	case BIN_MUL:
	case BIN_DIV:
	case BIN_MOD:
	case BIN_XOR:
	case BIN_AND:
	case BIN_OR:
		sprintf(szBuf,"%s %c %s",pLeft, cp->op, pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case BIN_AT:
		sprintf(szBuf, "%s[%s]", pLeft, pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case BIN_RANGE:
		sprintf(szBuf, "%s .. %s", pLeft, pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case BIN_POWER:
		sprintf(szBuf, "%s ** %s", pLeft, pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case BIN_SHIFT_RIGHT:
		pszShift = ">>";
		// drop through
	case BIN_SHIFT_LEFT:
		sprintf(szBuf, "%s %s %s", pLeft, pszShift, pRight);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
		break;
	case BIN_CAST:
		sprintf(szBuf,"(%s) %s", decompile_typenameFor(cp->targetType), pRight);
		break;
	}
	pStackCurrent = decompile_popStack(pStackCurrent, pStack);
	pStackCurrent->dse_instruction = (COM_0FUNC*)cp;
	pStackCurrent->dse_printed = _strdup(szBuf);
	pStackCurrent++;
	return pStackCurrent;
}

/*
 * decompile_printPushOperation()
 */
static DECOMPILATION_STACK_ELEMENT* decompile_printPushOperation(COM_CHAR1* cp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack, EDFUNC* pFunc, int nParamIndex) {
	char szBuf[512];
	szBuf[0] = 0;
	PARAMETER_TYPE_DESCRIPTOR pDescriptor = (PARAMETER_TYPE_DESCRIPTOR) {
		.pt_type = PARAM_TYPE_VOID
	};
	if (pFunc) {
		pDescriptor = function_getParameterTypeDescriptor(pFunc, nParamIndex);
	}
	decompile_printParameter(szBuf, (unsigned char*)cp, pDescriptor);
	pStackCurrent->dse_instruction = (COM_0FUNC*)cp;
	pStackCurrent->dse_printed = _strdup(szBuf);
	pStackCurrent++;
	return pStackCurrent;
}

/*
 * Return a string representation of a BINOP instruction.
 */
static char* decompile_binaryOperationAsString(unsigned char op) {
	static char szTemp[2];

	if (op == BIN_CAST) {
		return "cast";
	}
	if (op == BIN_SHIFT_LEFT) {
		return "<<";
	}
	if (op == BIN_SHIFT_RIGHT) {
		return ">>";
	}
	szTemp[0] = op;
	return szTemp;
}

/*
 * decompile_macroInstructionsToFile
 */
static void (*_decompileFunction)(FILE* fp, MACRO* mp);
static void decompile_macroInstructionsToFile(FILE* fp, MACRO* mp)
{
	unsigned char* sp, * spend, * data;
	MACROC_INSTRUCTION_OP_CODE t;

	fprintf(fp, "%s:", decompile_quoteString(MAC_NAME(mp)));
	data = MAC_DATA(mp);

	for (sp = data, spend = sp + mp->mc_size; sp < spend; ) {
		t = ((COM_1FUNC*)sp)->typ;
		int nOffs = (int)(sp - data);
		fprintf(fp, "\n0x%04x:   ", nOffs);
		switch (t) {
		case C_STOP: fprintf(fp, "stop"); break;
		case C_LOGICAL_OPERATION: fprintf(fp, "logicalOperation %s", decompile_testOperationAsString(((COM_BINOP*)sp)->op)); break;
		case C_GOTO:
			switch (((COM_GOTO*)sp)->bratyp) {
			case BRA_ALWAYS: fprintf(fp, "goto 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			case BRA_IF_TRUE: fprintf(fp, "branchIfTrue 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			case BRA_IF_FALSE: fprintf(fp, "branchIfFalse 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			}
			break;
		case C_BINOP: fprintf(fp, "operation %s", decompile_binaryOperationAsString(((COM_BINOP*)sp)->op)); break;
		case C_ASSIGN: fprintf(fp, "assignVariable %s", ((COM_ASSIGN*)sp)->name); break;
		case C_MACRO:fprintf(fp, "call %s", ((COM_MAC*)sp)->name); break;
		case C_0FUNC: fprintf(fp, "nativeCall0 %d (%s) (%d params)", ((COM_0FUNC*)sp)->funcnum, _functionTable[((COM_0FUNC*)sp)->funcnum].f_name, ((COM_0FUNC*)sp)->func_nargs); break;
		case C_1FUNC: fprintf(fp, "nativeCall1 %d (%s) %d", ((COM_1FUNC*)sp)->funcnum, _functionTable[((COM_0FUNC*)sp)->funcnum].f_name, ((COM_1FUNC*)sp)->p); break;
		case C_PUSH_VARIABLE: fprintf(fp, "pushVariable "); break;
		case C_PUSH_LONG_LITERAL: fprintf(fp, "pushLongLiteral %lld", (long long)((COM_LONG1*)sp)->val); break;
		case C_PUSH_SMALL_INT_LITERAL: fprintf(fp, "pushSmallIntLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_INTEGER_LITERAL: fprintf(fp, "pushIntLiteral %d", ((COM_INT1*)sp)->val); break;
		case C_PUSH_FLOAT_LITERAL: fprintf(fp, "pushFloatLiteral %f", ((COM_FLOAT1*)sp)->val); break;
		case C_PUSH_BOOLEAN_LITERAL: fprintf(fp, "pushBooleanLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_CHARACTER_LITERAL: fprintf(fp, "pushCharLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_STRING_LITERAL: fprintf(fp, "pushStringLiteral %s", ((COM_STRING1*)sp)->s); break;
		case C_DEFINE_VARIABLE: fprintf(fp, "defineVariable %s", ((COM_CREATESYM*)sp)->name); break;
		case C_DEFINE_PARAMETER: fprintf(fp, "defineParameter %s", ((COM_CREATESYM*)sp)->name); break;
		case C_FORM_START: fprintf(fp, "beginFormParameters"); break;
		case C_SET_STACKFRAME: fprintf(fp, "setStackFrame"); break;
		case C_SET_PARAMETER_STACK: fprintf(fp, "startParameters"); break;
		case C_POP_STACK: fprintf(fp, "pop"); break;
		default: fprintf(fp, "opcode 0x%x", t); break;
		}
		if (t == C_PUSH_VARIABLE) {
			char szBuf[128];
			decompile_printParameter(szBuf, sp, (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = macro_isParameterStringType(t) ? PARAM_TYPE_STRING : PARAM_TYPE_INT });
			fputs(szBuf, fp);
		}
		sp += interpreter_getParameterSize(*sp, sp+1);
	}
}

static void decompile_printComment(FILE* fp, const char* pszComment) {
	fprintf(fp, "/*\n");
	int c;
	BOOLEAN bNewLine = TRUE;
	while (*pszComment == '\n') {
		pszComment++;
	}
	while ((c = *pszComment++) != 0) {
		if (bNewLine) {
			fprintf(fp, " * ");
			bNewLine = FALSE;
			if (c == ' ') {
				continue;
			}
		}
		fputc(c, fp);
		bNewLine = c == '\n';
	}
	fprintf(fp, "\n */\n");
}

/*
 * Flush the current code generation stack. 
 */
static DECOMPILATION_STACK_ELEMENT* decompile_flushStack(FILE* fp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	if (pStackCurrent > pStack) {
		fprintf(fp, "%s", pStackCurrent[-1].dse_printed);
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
	}
	while (pStackCurrent > pStack) {
		pStackCurrent = decompile_popStack(pStackCurrent, pStack);
	}
	return pStackCurrent;
}

/*
 * Returns a control flow mark for a given byte code offset or null if no mark can be found.
 */
static CONTROL_FLOW_MARK_INDEX* decompile_controlFlowMarkForOffset(CONTROL_FLOW_MARK_INDEX* pTable, int nTableSize, const void* pAddress) {
	for (int i = 0; i < nTableSize; i++) {
		if (pTable[i].cfmi_bytecodeAddress == pAddress) {
			return &pTable[i];
		}
	}
	return NULL;
}

/*
 * Analyse the byte code jump instructions and "guess" the original control flow from which they were created.
 */
static CONTROL_FLOW_MARK_INDEX* decompile_analyseControlFlowMarks(unsigned char* pBytecode, unsigned char* pBytecodeEnd, int* nMarks) {
	unsigned char* pStart = pBytecode;
	int nMax = 128;
	CONTROL_FLOW_MARK_INDEX* pResult = calloc(nMax, sizeof * pResult);
	int nFound = 0;

	while (pBytecode < pBytecodeEnd) {
		if (*pBytecode == C_GOTO) {
			COM_GOTO* pGoto = (COM_GOTO*)pBytecode;
			if (nFound > nMax - 2) {
				nMax += 100;
				pResult = realloc(pResult, nMax * sizeof *pResult);
			}
			if (pGoto->bratyp == BRA_ALWAYS) {
				CONTROL_FLOW_MARK_INDEX* pFoundMark = decompile_controlFlowMarkForOffset(pResult, nFound, pGoto);
				if (!pFoundMark) {
					pFoundMark = decompile_controlFlowMarkForOffset(pResult, nFound, pBytecode+pGoto->offset);
					if (pFoundMark) {
						if (pFoundMark->cfmi_mark == CFM_WHILE) {
							pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_CONTINUE, .cfmi_bytecodeAddress = (unsigned char*)pGoto };
						} else if (pFoundMark->cfmi_mark == CFM_END_BLOCK) {
							pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_BREAK, .cfmi_bytecodeAddress = (unsigned char*)pGoto };
						}
					}
				}
			} else if (pGoto->bratyp == BRA_IF_FALSE && pGoto->offset > 0) {
				BOOL bDone = FALSE;
				int nOffs = pGoto->offset - sizeof(COM_GOTO);
				if (nOffs > 0) {
					COM_GOTO* pBack = (COM_GOTO*)(pBytecode + nOffs);
					if (pBack->typ == C_GOTO && pBack->bratyp == BRA_ALWAYS && pBack->offset < 0) {
						if (pBytecode + nOffs + pBack->offset < (unsigned char*)pGoto) {
							pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_WHILE, .cfmi_bytecodeAddress = (unsigned char*)pGoto };
							pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_END_BLOCK, .cfmi_bytecodeAddress = (unsigned char*)pBack };
							bDone = TRUE;
						}
					}
				}
				if (!bDone) {
					pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_IF, .cfmi_bytecodeAddress = (unsigned char*)pGoto};
					COM_GOTO* pElse = (COM_GOTO*)(pBytecode + pGoto->offset - sizeof(COM_GOTO));
					if (pElse->typ == C_GOTO && pElse->bratyp == BRA_ALWAYS && pElse->offset > 0) {
						pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_ELSE, .cfmi_bytecodeAddress = (unsigned char*)pElse};
						pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_END_BLOCK, .cfmi_bytecodeAddress = ((unsigned char*)pElse)+pElse->offset };
					}
					else {
						pResult[nFound++] = (CONTROL_FLOW_MARK_INDEX){ .cfmi_mark = CFM_END_BLOCK, .cfmi_bytecodeAddress = ((unsigned char*)pElse)+sizeof(COM_GOTO)};
					}

				}
			}
		}
		pBytecode += interpreter_getParameterSize(*pBytecode, pBytecode + 1);
	}
	*nMarks = nFound;
	return pResult;
}

static decompile_indent(FILE* fp, int nIndent) {
	while (--nIndent >= 0) {
		fputc('\t', fp);
	}
}

/*
 * Finc the next native function call statement and return the function descriptor, if a call statement was found.
 */
static EDFUNC* decompile_findFunctionDescriptor(unsigned char* pBytecode, unsigned char* spend) {
	while (pBytecode < spend) {
		if (!C_IS_PUSH_OPCODE(*pBytecode)) {
			if (*pBytecode == C_0FUNC) {
				return decompile_getFunctionForIndex(((COM_0FUNC*)pBytecode)->funcnum);
			}
			if (*pBytecode == C_1FUNC) {
				return decompile_getFunctionForIndex(((COM_1FUNC*)pBytecode)->funcnum);
			}
			break;
		}
		pBytecode += interpreter_getParameterSize(*pBytecode, pBytecode + 1);
	}
	return NULL;
}

/*
 * decompile_makeAutoLabels()
 */
static void decompile_makeAutoLabels(const char* start, const char* end, CONTROL_FLOW_MARK_INDEX* pMarks, int nMarks)
{
	while (start < end && *start != C_STOP) {
		if (*start == C_GOTO && ((COM_GOTO*)start)->offset != sizeof(COM_GOTO)) {
			const unsigned char* pOffset = (start + ((COM_GOTO*)start)->offset);
			CONTROL_FLOW_MARK_INDEX* pFoundMark = decompile_controlFlowMarkForOffset(pMarks, nMarks, start);
			if (!pFoundMark) {
				bytecode_makeAutoLabel((COM_GOTO*)pOffset);
			}
		}
		start += interpreter_getParameterSize(*start, start + 1);
	}
}

/*
 * decompile_macroToFile
 */
#define COM1_INCR(cp,type,offset) (unsigned char *)(cp+((type *)cp)->offset)
static void decompile_macroToFile(FILE *fp, MACRO *mp)
{	unsigned char 	*sp,*spend,*gop,*data,*comment;
	MACROC_INSTRUCTION_OP_CODE 	opCode;
	EDFUNC* pCurrentFunctionDescriptor = NULL;
	char	*lname;
	int nIndent = 1;
	CONTROL_FLOW_MARK_INDEX* pFlowMarks;
	int     nMarks;
	DECOMPILATION_STACK_ELEMENT  pStack[80];
	DECOMPILATION_STACK_ELEMENT* pStackCurrent = pStack;
	DECOMPILATION_STACK_ELEMENT* pStackEnd = pStack + DIM(pStack);

	memset(pStack, 0, sizeof pStack);
	comment = MAC_COMMENT(mp);
	if (*comment) {
		decompile_printComment(fp, comment);
	}
	data = MAC_DATA(mp);
	sp = data;
	spend = sp + mp->mc_size;

	pFlowMarks = decompile_analyseControlFlowMarks(sp, spend, &nMarks);

	// (T) find out the return type of the macro - is currently not stored.
	fprintf(fp, "void %s(", decompile_quoteString(MAC_NAME(mp)));
	while (sp < spend) {
		if (*sp != C_DEFINE_PARAMETER) {
			break;
		}
		COM_CREATESYM* pSym = (COM_CREATESYM*)sp;
		if (sp > data) {
			fprintf(fp, ", ");
		}
		fprintf(fp, "%s %s", decompile_typenameFor(pSym->typ), pSym->name);
		sp += interpreter_getParameterSize(*sp, sp + 1);
	}
	fprintf(fp, ") {\n");

	decompile_makeAutoLabels(data, spend, pFlowMarks, nMarks);
	bytecode_initializeAutoLabels();
	bytecode_startNextAutoLabel(&lname, (COM_GOTO**)&gop);
	int nParamIndex = 1;
	while (sp < spend) {
		CONTROL_FLOW_MARK_INDEX* pFoundMark = decompile_controlFlowMarkForOffset(pFlowMarks, nMarks, sp);
		if (gop <= sp && lname) {
			if (gop < sp) {
				error_displayAlertDialog("format error: bad goto");
			}
			if (!pFoundMark) {
				fprintf(fp, "%s:\n", lname);
			}
			bytecode_startNextAutoLabel(&lname, (COM_GOTO**)&gop);
		}

		opCode = ((COM_1FUNC*)sp)->typ;
		if (C_IS_PUSH_OPCODE(opCode)) {
			if (!pCurrentFunctionDescriptor && nParamIndex == 1) {
				pCurrentFunctionDescriptor = decompile_findFunctionDescriptor(sp, spend);
			}
			pStackCurrent = decompile_printPushOperation((COM_CHAR1*)sp, pStackCurrent, pStack, pCurrentFunctionDescriptor, nParamIndex);
			sp += interpreter_getParameterSize(*sp, sp + 1);
			nParamIndex++;
			continue;
		}
		nParamIndex = 1;
		pCurrentFunctionDescriptor = NULL;
		if (C_ISCMD(opCode)) {
			pStackCurrent = decompile_function((COM_1FUNC*)sp, pStackCurrent, pStack);
		} else if (opCode == C_GOTO) {
			COM_GOTO* cp = (COM_GOTO*)sp;
			if (pStackCurrent > pStack && pFoundMark) {
				decompile_indent(fp, nIndent);
				fprintf(fp, "%s (", pFoundMark->cfmi_mark == CFM_WHILE ? "while" : "if");
				nIndent++;
				pStackCurrent = decompile_flushStack(fp, pStackCurrent, pStack);
				fprintf(fp, ") {\n");
			}
			else if (pFoundMark) {
				if (pFoundMark->cfmi_mark == CFM_ELSE) {
					decompile_indent(fp, nIndent - 1);
					fprintf(fp, "} else {\n");
				} else if (pFoundMark->cfmi_mark == CFM_BREAK) {
					decompile_indent(fp, nIndent - 1);
					fprintf(fp, "break;\n");
				} else if (pFoundMark->cfmi_mark == CFM_CONTINUE) {
					decompile_indent(fp, nIndent - 1);
					fprintf(fp, "continue;\n");
				}
				else {
					pFoundMark = 0;
				}
			}
			if (!pFoundMark && cp->offset != sizeof(COM_GOTO)) {
				decompile_indent(fp, nIndent);
				fprintf(fp, "%s %s;\n",
					(cp->bratyp == BRA_ALWAYS) ? "goto" : "braeq",
					bytecode_findAutoLabelForInstruction((COM_GOTO*)(sp + cp->offset)));
			}
		}
		else if (opCode == C_STOP) {
			if (sp + sizeof(COM_STOP) < spend) {
				decompile_indent(fp, nIndent);
				fprintf(fp, "return ");
				pStackCurrent = decompile_flushStack(fp, pStackCurrent, pStack);
				fprintf(fp, ";\n");
			}
		}
		else if (opCode == C_LOGICAL_OPERATION) {
			pStackCurrent = decompile_printTestExpression((COM_BINOP*)sp, pStackCurrent, pStack);
		} 
		else if (opCode == C_BINOP) {
			pStackCurrent = decompile_printBinaryExpression((COM_BINOP*)sp, pStackCurrent, pStack);
		}
		else if (opCode == C_DEFINE_VARIABLE) {
			char* pszType = decompile_typenameFor(((COM_CREATESYM*)sp)->symtype);
			decompile_indent(fp, nIndent);
			fprintf(fp, "%s %s;\n", pszType, ((COM_CREATESYM*)sp)->name);
		}
		else if (opCode == C_ASSIGN) {
			decompile_indent(fp, nIndent);
			fprintf(fp, "%s = ", ((COM_ASSIGN*)sp)->name);
			pStackCurrent = decompile_flushStack(fp, pStackCurrent, pStack);
			fprintf(fp, ";\n");
		}
		else if (opCode == C_POP_STACK) {
			if (pStackCurrent > pStack) {
				decompile_indent(fp, nIndent);
				pStackCurrent = decompile_flushStack(fp, pStackCurrent, pStack);
				fprintf(fp, ";\n");
			}
		}
		else if (opCode == C_SET_PARAMETER_STACK) {
			pStackCurrent->dse_printed = 0;
			pStackCurrent->dse_instruction = (COM_0FUNC*)sp;
			pStackCurrent++;
		} else if (opCode == C_SET_STACKFRAME) {
		} else {
			decompile_indent(fp, nIndent);
			fprintf(fp, "// format error in %s type=%x",MAC_NAME(mp),opCode);
		}
		if (pFoundMark && pFoundMark->cfmi_mark == CFM_END_BLOCK) {
			nIndent--;
			decompile_indent(fp, nIndent);
			fprintf(fp, "}\n");
		}
		sp += interpreter_getParameterSize(*sp,sp+1);
	}
	free(pFlowMarks);
	fprintf(fp,"}\n\n");
	bytecode_closeAutoLabels();
}

/*
 * decompile_printMacrosCallback
 */
static char *_macroname;
static long decompile_printMacrosCallback(FILE *fp)
{	MACRO	**mtable,*mp;
	int	    	i;

	for (mtable = _macroTable, i = 0; i < _macroTableSize; i++) {
		if ((mp = mtable[i]) != 0) {
			if (!_macroname ||
			    strcmp(_macroname,MAC_NAME(mp)) == 0)
				_decompileFunction(fp,mp);
		}
	}
	return 1;
}

/*
 * decompile_saveMacrosAndDisplay
 * Decompiles one or all macros and prints the decompilation result to a given file.
 * Decompilation may be performed by printing the code as source code or by printing the low level instructions
 */
int decompile_saveMacrosAndDisplay(char *macroname, DECOMPILATION_MODE nMode) {
	char szBuf[128];
	char* pszExtension = nMode == DM_CODE ? ".pkc" : ".pkobj";

	_macroname = macroname;
	if (macroname) {
		strmaxcpy(szBuf, macroname, sizeof szBuf-5);
	} else {
		strcpy(szBuf, "macros");
	}
	strcat(szBuf, pszExtension);
	_decompileFunction = nMode == DM_CODE ? decompile_macroToFile : decompile_macroInstructionsToFile;
	return macro_createFileAndDisplay(szBuf, decompile_printMacrosCallback);
}


