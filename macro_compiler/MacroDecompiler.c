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
#include "arraylist.h"

void bytecode_closeAutoLabels(void);
int  bytecode_makeAutoLabel(COM_GOTO *cp);
void bytecode_initializeAutoLabels(void);
void bytecode_startNextAutoLabel(char **name, COM_GOTO **cp);
char *bytecode_findAutoLabelForInstruction(COM_GOTO *cp);

typedef struct tagDECOMPILE_OPTIONS {
	MACRO*			do_macro;								// The macro to decompile
	const char*		do_instructionPointer;					// the current instruction pointer - if != 0, the decompiler tries to find the line in the 
															// output generated for this instruction pointer
	int				do_lineNumberForInstructionPointer;		// if above != 0, the line number for the instruction pointer.
} DECOMPILE_OPTIONS;

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

static void decompile_print(STRING_BUF* pBuf, const char* fmt, ...) {
	char szBuffer[1024];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(szBuffer, fmt, ap);
	va_end(ap);
	stringbuf_appendString(pBuf, szBuffer);
}

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
static char *decompile_quoteString(const unsigned char *name)
{	static char qbuf[1024];
	unsigned char *d,*dend;
	
	for (d = qbuf, dend= &qbuf[sizeof qbuf-2]; d < dend && *name; name++) {
		if (*name == '\n') {
			*d++ = '\\';
			*d++ = 'n';
			continue;
		}
		if (*name == '\\' || *name == '"') {
			*d++ = '\\';
		}
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

void decompile_printValue(char* pszBuf, PKS_VALUE v) {
	if (v.pkv_type == VT_BOOLEAN) {
		sprintf(pszBuf, "%s", v.pkv_data.booleanValue ? "true" : "false");
	}
	else if (v.pkv_type == VT_FLOAT) {
		sprintf(pszBuf, "%lf", v.pkv_data.doubleValue);
	}
	else if (v.pkv_type == VT_STRING) {
		// check size and buffer overflow
		const char* pString = v.pkv_managed ? memory_accessString(v) : v.pkv_data.string;
		sprintf(pszBuf, "\"%.*s\"", 200, decompile_quoteString(pString));
	} else {
		sprintf(pszBuf, "%lld", v.pkv_data.longValue);
	}
}

/*
 * decompile_printParameter()
 */
static int decompile_printParameter(char* pszBuf, unsigned char *sp, PARAMETER_TYPE_DESCRIPTOR partyp) {
	unsigned char typ;
	long long val;

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
		case C_PUSH_ARRAY_LITERAL:
			sprintf(pszBuf, "{ ... }");
			return 1;
		case C_PUSH_MAP_LITERAL:
			sprintf(pszBuf, "[ \"x\" => 2 ]");
			return 1;
		case C_PUSH_FLOAT_LITERAL:
			decompile_printValue(pszBuf, (PKS_VALUE) {
				.pkv_type = VT_FLOAT, .pkv_data.doubleValue = ((COM_FLOAT1*)sp)->val
			});
			return 1;
		case C_PUSH_BOOLEAN_LITERAL:
			decompile_printValue(pszBuf, (PKS_VALUE) {
				.pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = ((COM_CHAR1*)sp)->val
			});
			return 1;
		case C_PUSH_NEW_INSTANCE:
			sprintf(pszBuf, "new %s()", types_nameFor(((COM_INT1*)sp)->c_valueType));
			return 1;
		case C_PUSH_INTEGER_LITERAL:
			val = ((COM_INT1 *)sp)->val;
			break;
		case C_PUSH_LONG_LITERAL:
			val = (long long)((COM_LONG1 *)sp)->val;
			break;
		case C_PUSH_STRING_LITERAL:
			decompile_printValue(pszBuf, (PKS_VALUE) {
				.pkv_type = VT_STRING, .pkv_data.string = ((COM_STRING1*)sp)->s
			});
			return 1;
		case C_PUSH_VARIABLE:
			strcpy(pszBuf, ((COM_STRING1*)sp)->s);
			return 1;
		default:
			error_displayAlertDialog("Format error in parameters of function to decompile. Bytecode is 0x%x", typ);
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
		}
		else if ((char)val == '\t') {
			escape = '\t';
		}
		else if ((char)val == '\'') {
			escape = '\'';
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

/*
 * decompile_function()
 */
static DECOMPILATION_STACK_ELEMENT* decompile_function(COM_1FUNC* sp, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	EDFUNC	*		ep;
	int 			npars;
	PARAMETER_TYPE_DESCRIPTOR partyp;
	char szFunctionCall[512];
	char szParam[128];

	if (sp->typ == C_MACRO || sp->typ == C_MACRO_REF) {
		char* pszFunction = ((COM_MAC*)sp)->name;
		char szFunctionPrinted[128];
		if (sp->typ == C_MACRO_REF) {
			sprintf(szFunctionPrinted, "*%s", pszFunction);
			pszFunction = szFunctionPrinted;
		}
		strcpy(szFunctionCall, pszFunction);
		ep = NULL;
	} else {
		decompile_printFunctionName(szFunctionCall, sp->funcnum);
		ep = decompile_getFunctionForIndex(sp->funcnum);
	}
	strcat(szFunctionCall, "(");

	npars = 0;
	int passedArgs = ((COM_0FUNC*)sp)->func_nargs;
	if (sp->typ == C_1FUNC) {
		partyp = function_getParameterTypeDescriptor(ep, 1);
		if (decompile_printParameter(szParam,(unsigned char*)sp,partyp) < 0)
			return 0;
		strcat(szFunctionCall, szParam);
		npars++;
		passedArgs--;
	}

	DECOMPILATION_STACK_ELEMENT* pParamStack = pStackCurrent-passedArgs;
	if (pParamStack < pStack) {
		pParamStack = pStack;
	}
	DECOMPILATION_STACK_ELEMENT* pStackFrame = pParamStack;
	if (passedArgs == 3 && sp->typ == C_0FUNC && sp->funcnum == FUNC_Foreach) {
		strcpy(szFunctionCall, "for (");
		pParamStack++;
		strcat(szFunctionCall, "auto ");
		strcat(szFunctionCall, ((COM_STRING1*) pStack[1].dse_instruction)->s);
		strcat(szFunctionCall, " : ");
		pParamStack++;
		strcat(szFunctionCall, pParamStack->dse_printed);
	}
	else {
		while(pParamStack < pStackCurrent) {
			if (!pParamStack->dse_printed) {
				break;
			}
			partyp = ep == NULL ? (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = PARAM_TYPE_VOID} : function_getParameterTypeDescriptor(ep, npars + 1);
			if (npars) {
				strcat(szFunctionCall, ",");
			}
			size_t nBytes = strlen(szFunctionCall);
			strncat(szFunctionCall, pParamStack->dse_printed, sizeof(szFunctionCall) - 3 - nBytes);
			pParamStack++;
			npars++;
		}
	}

	strcat(szFunctionCall, ")");
	pStackCurrent->dse_instruction = (COM_0FUNC*)sp;
	pStackCurrent->dse_printed = _strdup(szFunctionCall);
	pStackCurrent++;
	return pStackCurrent;
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
		sprintf(szBuf,"(%s) %s", types_nameFor(cp->targetType), pRight);
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
	if (op == BIN_POWER) {
		return "**";
	}
	if (op == BIN_RANGE) {
		return "..";
	}
	szTemp[0] = op;
	return szTemp;
}

/*
 * decompile_macroInstructions
 */
static void (*_decompileFunction)(STRING_BUF* pBuf, DECOMPILE_OPTIONS* pOptions);
static void decompile_macroInstructions(STRING_BUF* pBuf, DECOMPILE_OPTIONS* pOptions)
{
	unsigned char* sp, * spend, * data;
	MACROC_INSTRUCTION_OP_CODE t;
	MACRO* mp = pOptions->do_macro;

	decompile_print(pBuf, "%s:", decompile_quoteString(MAC_NAME(mp)));
	data = MAC_DATA(mp);

	for (sp = data, spend = sp + mp->mc_bytecodeLength; sp < spend; ) {
		t = ((COM_1FUNC*)sp)->typ;
		int nOffs = (int)(sp - data);
		decompile_print(pBuf, "\n0x%04x:   ", nOffs);
		if (pOptions->do_instructionPointer == sp) {
			pOptions->do_lineNumberForInstructionPointer = stringbuf_getLineNuber(pBuf);
		}
		switch (t) {
		case C_STOP: decompile_print(pBuf, "stop"); break;
		case C_LOGICAL_OPERATION: decompile_print(pBuf, "logicalOperation %s", decompile_testOperationAsString(((COM_BINOP*)sp)->op)); break;
		case C_GOTO:
			switch (((COM_GOTO*)sp)->branchType) {
			case BRA_ALWAYS: decompile_print(pBuf, "goto 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			case BRA_CASE: decompile_print(pBuf, "branchCase 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			case BRA_IF_FALSE: decompile_print(pBuf, "branchIfFalse 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
			}
			break;
		case C_BINOP: decompile_print(pBuf, "operation %s", decompile_binaryOperationAsString(((COM_BINOP*)sp)->op)); break;
		case C_ASSIGN: decompile_print(pBuf, "assignVariable %s", ((COM_ASSIGN*)sp)->name); break;
		case C_ASSIGN_SLOT: decompile_print(pBuf, "assignSlot %s", ((COM_ASSIGN*)sp)->name); break;
		case C_MACRO:decompile_print(pBuf, "call %s", ((COM_MAC*)sp)->name); break;
		case C_MACRO_REF:decompile_print(pBuf, "eval %s", ((COM_MAC*)sp)->name); break;
		case C_0FUNC: decompile_print(pBuf, "nativeCall0 %d (%s) (%d params)", ((COM_0FUNC*)sp)->funcnum, _functionTable[((COM_0FUNC*)sp)->funcnum].f_name, ((COM_0FUNC*)sp)->func_nargs); break;
		case C_1FUNC: decompile_print(pBuf, "nativeCall1 %d (%s) (%d params) %d", ((COM_1FUNC*)sp)->funcnum, 
				_functionTable[((COM_0FUNC*)sp)->funcnum].f_name, ((COM_0FUNC*)sp)->func_nargs, ((COM_1FUNC*)sp)->p); break;
		case C_PUSH_VARIABLE: decompile_print(pBuf, "pushVariable "); break;
		case C_PUSH_LONG_LITERAL: decompile_print(pBuf, "pushLongLiteral %lld", (long long)((COM_LONG1*)sp)->val); break;
		case C_PUSH_SMALL_INT_LITERAL: decompile_print(pBuf, "pushSmallIntLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_NEW_INSTANCE: decompile_print(pBuf, "pushNewInstance %s", types_nameFor(((COM_INT1*)sp)->c_valueType)); break;
		case C_PUSH_INTEGER_LITERAL: decompile_print(pBuf, "pushIntLiteral %d", ((COM_INT1*)sp)->val); break;
		case C_PUSH_FLOAT_LITERAL: decompile_print(pBuf, "pushFloatLiteral %f", ((COM_FLOAT1*)sp)->val); break;
		case C_PUSH_ARRAY_LITERAL: decompile_print(pBuf, "pushArrayLiteral size==%d", ((COM_ARRAYLITERAL*)sp)->length); break;
		case C_PUSH_MAP_LITERAL: decompile_print(pBuf, "pushMapLiteral size==%d", ((COM_ARRAYLITERAL*)sp)->length); break;
		case C_PUSH_BOOLEAN_LITERAL: decompile_print(pBuf, "pushBooleanLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_CHARACTER_LITERAL: decompile_print(pBuf, "pushCharLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_PUSH_STRING_LITERAL: decompile_print(pBuf, "pushStringLiteral %s", ((COM_STRING1*)sp)->s); break;
		case C_DEFINE_VARIABLE: decompile_print(pBuf, "defineVariable %s", ((COM_DEFINE_SYMBOL*)sp)->name); break;
		case C_DEFINE_PARAMETER: decompile_print(pBuf, "defineParameter %s", ((COM_DEFINE_SYMBOL*)sp)->name); break;
		case C_FORM_START: decompile_print(pBuf, "beginFormParameters"); break;
		case C_SET_STACKFRAME: decompile_print(pBuf, "setStackFrame"); break;
		case C_POP_STACK: decompile_print(pBuf, "pop"); break;
		default: decompile_print(pBuf, "opcode 0x%x", t); break;
		}
		if (t == C_PUSH_VARIABLE) {
			char szBuf[128];
			decompile_printParameter(szBuf, sp, (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = macro_isParameterStringType(t) ? PARAM_TYPE_STRING : PARAM_TYPE_INT });
			stringbuf_appendString(pBuf, szBuf);
		}
		sp += interpreter_getParameterSize(*sp, sp+1);
	}
}

static void decompile_printComment(STRING_BUF* pBuf, const char* pszComment) {
	decompile_print(pBuf, "/*\n");
	int c;
	BOOLEAN bNewLine = TRUE;
	while (*pszComment == '\n') {
		pszComment++;
	}
	while ((c = *pszComment++) != 0) {
		if (bNewLine) {
			decompile_print(pBuf, " * ");
			bNewLine = FALSE;
			if (c == ' ') {
				continue;
			}
		}
		stringbuf_appendChar(pBuf, c);
		bNewLine = c == '\n';
	}
	decompile_print(pBuf, "\n */\n");
}

/*
 * Flush the current code generation stack. 
 */
static DECOMPILATION_STACK_ELEMENT* decompile_flushStack(STRING_BUF* pBuf, DECOMPILATION_STACK_ELEMENT* pStackCurrent, DECOMPILATION_STACK_ELEMENT* pStack) {
	if (pStackCurrent > pStack) {
		decompile_print(pBuf, "%s", pStackCurrent[-1].dse_printed);
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
			if (pGoto->branchType == BRA_ALWAYS) {
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
			} else if (pGoto->branchType == BRA_IF_FALSE && pGoto->offset > 0) {
				BOOL bDone = FALSE;
				int nOffs = pGoto->offset - sizeof(COM_GOTO);
				if (nOffs > 0) {
					COM_GOTO* pBack = (COM_GOTO*)(pBytecode + nOffs);
					if (pBack->typ == C_GOTO && pBack->branchType == BRA_ALWAYS && pBack->offset < 0) {
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
					if (pElse->typ == C_GOTO && pElse->branchType == BRA_ALWAYS && pElse->offset > 0) {
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

static decompile_indent(STRING_BUF* pBuf, int nIndent) {
	while (--nIndent >= 0) {
		stringbuf_appendChar(pBuf, '\t');
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
 * decompile_macroCode
 */
#define COM1_INCR(cp,type,offset) (unsigned char *)(cp+((type *)cp)->offset)
static void decompile_macroCode(STRING_BUF* pBuf, DECOMPILE_OPTIONS *pOptions)
{	unsigned char 	*sp,*spend,*gop,*data,*comment;
	MACROC_INSTRUCTION_OP_CODE 	opCode;
	EDFUNC* pCurrentFunctionDescriptor = NULL;
	char	*lname;
	int nIndent = 1;
	CONTROL_FLOW_MARK_INDEX* pFlowMarks;
	int     nMarks;
	MACRO* mp = pOptions->do_macro;
	DECOMPILATION_STACK_ELEMENT  pStack[80];
	DECOMPILATION_STACK_ELEMENT* pStackCurrent = pStack;
	DECOMPILATION_STACK_ELEMENT* pStackEnd = pStack + DIM(pStack);

	memset(pStack, 0, sizeof pStack);
	comment = MAC_COMMENT(mp);
	if (*comment) {
		decompile_printComment(pBuf, comment);
	}
	data = MAC_DATA(mp);
	sp = data;
	spend = sp + mp->mc_bytecodeLength;

	pFlowMarks = decompile_analyseControlFlowMarks(sp, spend, &nMarks);

	// (T) find out the return type of the macro - is currently not stored.
	decompile_print(pBuf, "void %s(", decompile_quoteString(MAC_NAME(mp)));
	while (sp < spend) {
		if (*sp != C_DEFINE_PARAMETER) {
			break;
		}
		COM_DEFINE_SYMBOL* pSym = (COM_DEFINE_SYMBOL*)sp;
		if (sp > data) {
			decompile_print(pBuf, ", ");
		}
		decompile_print(pBuf, "%s %s", types_nameFor(pSym->typ), pSym->name);
		sp += interpreter_getParameterSize(*sp, sp + 1);
	}
	decompile_print(pBuf, ") {\n");

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
				decompile_print(pBuf, "%s:\n", lname);
			}
			bytecode_startNextAutoLabel(&lname, (COM_GOTO**)&gop);
		}
		if (sp == pOptions->do_instructionPointer) {
			pOptions->do_lineNumberForInstructionPointer = stringbuf_getLineNuber(pBuf);
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
				decompile_indent(pBuf, nIndent);
				BOOL bWhile = pFoundMark->cfmi_mark == CFM_WHILE;
				if (bWhile && pStackCurrent[-1].dse_instruction->typ == C_0FUNC && pStackCurrent[-1].dse_instruction->funcnum == FUNC_Foreach) {
					pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
					nIndent++;
					decompile_print(pBuf, " {\n");
				}
				else {
					decompile_print(pBuf, "%s (", bWhile ? "while" : "if");
					nIndent++;
					pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
					decompile_print(pBuf, ") {\n");
				}
			}
			else if (pFoundMark) {
				if (pFoundMark->cfmi_mark == CFM_ELSE) {
					decompile_indent(pBuf, nIndent - 1);
					decompile_print(pBuf, "} else {\n");
				} else if (pFoundMark->cfmi_mark == CFM_BREAK) {
					decompile_indent(pBuf, nIndent - 1);
					decompile_print(pBuf, "break;\n");
				} else if (pFoundMark->cfmi_mark == CFM_CONTINUE) {
					decompile_indent(pBuf, nIndent - 1);
					decompile_print(pBuf, "continue;\n");
				}
				else if (pFoundMark->cfmi_mark != CFM_END_BLOCK) {
					pFoundMark = 0;
				}
			}
			if (!pFoundMark && cp->offset != sizeof(COM_GOTO)) {
				decompile_indent(pBuf, nIndent);
				decompile_print(pBuf, "%s %s;\n",
					(cp->branchType == BRA_ALWAYS) ? "goto" : "braeq",
					bytecode_findAutoLabelForInstruction((COM_GOTO*)(sp + cp->offset)));
			}
		}
		else if (opCode == C_STOP) {
			if (sp + sizeof(COM_STOP) < spend) {
				decompile_indent(pBuf, nIndent);
				decompile_print(pBuf, "return ");
				pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
				decompile_print(pBuf, ";\n");
			}
		}
		else if (opCode == C_LOGICAL_OPERATION) {
			pStackCurrent = decompile_printTestExpression((COM_BINOP*)sp, pStackCurrent, pStack);
		} 
		else if (opCode == C_BINOP) {
			pStackCurrent = decompile_printBinaryExpression((COM_BINOP*)sp, pStackCurrent, pStack);
		}
		else if (opCode == C_DEFINE_VARIABLE) {
			const char* pszType = types_nameFor(((COM_DEFINE_SYMBOL*)sp)->symtype);
			decompile_indent(pBuf, nIndent);
			decompile_print(pBuf, "%s %s;\n", pszType, ((COM_DEFINE_SYMBOL*)sp)->name);
		}
		else if (opCode == C_ASSIGN) {
			decompile_indent(pBuf, nIndent);
			decompile_print(pBuf, "%s = ", ((COM_ASSIGN*)sp)->name);
			pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
			decompile_print(pBuf, ";\n");
		}
		else if (opCode == C_ASSIGN_SLOT) {
			decompile_indent(pBuf, nIndent);
			decompile_print(pBuf, "%s[%s] = ", ((COM_ASSIGN*)sp)->name, pStackCurrent[-2].dse_printed);
			pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
			decompile_print(pBuf, ";\n");
		}
		else if (opCode == C_POP_STACK) {
			if (pStackCurrent > pStack) {
				decompile_indent(pBuf, nIndent);
				pStackCurrent = decompile_flushStack(pBuf, pStackCurrent, pStack);
				decompile_print(pBuf, ";\n");
			}
		}
		else if (opCode == C_SET_STACKFRAME) {
		} else {
			decompile_indent(pBuf, nIndent);
			decompile_print(pBuf, "// format error in %s type=%x",MAC_NAME(mp),opCode);
		}
		if (pFoundMark && pFoundMark->cfmi_mark == CFM_END_BLOCK) {
			nIndent--;
			decompile_indent(pBuf, nIndent);
			decompile_print(pBuf, "}\n");
		}
		sp += interpreter_getParameterSize(*sp,sp+1);
	}
	free(pFlowMarks);
	decompile_print(pBuf,"}\n\n");
	bytecode_closeAutoLabels();
}

/*
 * decompile_printMacrosCallback
 */
static char *_macroname;
static long decompile_printMacrosCallback(FILE *fp)
{
	int nMacros = macro_getNumberOfMacros();
	MACRO		*mp;
	int	    	i;

	for (i = 0; i < nMacros; i++) {
		if ((mp = macro_getByIndex(i)) != 0) {
			if (!_macroname ||
				strcmp(_macroname, MAC_NAME(mp)) == 0) {
				DECOMPILE_OPTIONS options = { .do_macro = mp};
				STRING_BUF* pBuf = stringbuf_create(512);
				stringbuf_setFlags(pBuf, SB_COUNT_LINE_NUMBERS);
				_decompileFunction(pBuf, &options);
				fputs(stringbuf_getString(pBuf), fp);
				stringbuf_destroy(pBuf);
			}
		}
	}
	return 1;
}

/*
 * Decompile a macro with the given mode and return the result of the decompilation
 * in an array list. The array list must be destroyed by the caller using arraylist_destroyStringList.
 * The decompilation with also return the line number containing the instruction pointer passed.
 */
ARRAY_LIST* deccompile_macroNamed(const char* pszName, DECOMPILATION_MODE nMode, const char* pszInstructionPointer, int *pNLine) {
	int nIndex = macro_getInternalIndexByName(pszName);
	*pNLine = 0;
	ARRAY_LIST* pResult = arraylist_create(32);
	if (nIndex < 0) {
		return pResult;
	}
	MACRO* mp = macro_getByIndex(nIndex);
	DECOMPILE_OPTIONS options = { .do_macro = mp, .do_instructionPointer = pszInstructionPointer};
	STRING_BUF* pBuf = stringbuf_create(512);
	stringbuf_setFlags(pBuf, SB_COUNT_LINE_NUMBERS);
	if (nMode == DM_CODE) {
		decompile_macroCode(pBuf, &options);
	} else {
		decompile_macroInstructions(pBuf, &options);
	}
	*pNLine = options.do_lineNumberForInstructionPointer;
	char* pszRun = stringbuf_getString(pBuf);
	char* pszNL;
	while ((pszNL = strchr(pszRun, '\n')) != 0) {
		*pszNL++ = 0;
		arraylist_add(pResult, _strdup(pszRun));
		pszRun = pszNL;
	}
	arraylist_add(pResult, _strdup(pszRun));
	stringbuf_destroy(pBuf);
	return pResult;
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
	_decompileFunction = nMode == DM_CODE ? decompile_macroCode : decompile_macroInstructions;
	return macro_createFileAndDisplay(szBuf, decompile_printMacrosCallback);
}


