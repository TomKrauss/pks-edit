/*
 * MacroInterpreter.c
 *
 * PKS-EDIT interpreter engine
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 11/89
 */

#include <string.h>
#include <windows.h>
#include <setjmp.h>

#include "winfo.h"
#include "edierror.h"
#include "errordialogs.h"
#include "stringutil.h"

#include "pksedit.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "symbols.h"
#include "funcdef.h"
#include "mouseutil.h"
#include "caretmovement.h"
#include "arraylist.h"

 /*--------------------------------------------------------------------------
  * recorder_pushSequence()
  *
  * this is called, when recording
  * we do not need, to handle all command types, cause in ineractive
  * mode, delta like test or binop are not possible
  */
extern void recorder_pushSequence(unsigned char typ, void* par);

/*
 * Answers true if we are currently recording macros
 */
extern BOOL recorder_isRecording();
/*
 * Start/stop the macro recorder.
 */
extern void recorder_setRecording(BOOL bStart);
extern void recorder_recordFunctionWithParameters(int fnum, int p, intptr_t p2, char* s1, char* s2);
extern int  ft_selectWindowWithId(int winid, BOOL bPopup);

/*--------------------------------------------------------------------------
 * interpreter_testExpression()
 * Test an expression in a macro.
 */
extern int interpreter_testExpression(EXECUTION_CONTEXT* pContext, COM_BINOP* sp);
extern void interpreter_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp);

extern int 	progress_cancelMonitor(BOOL bRedraw);
extern void ww_redrawAllWindows(int update);
extern void undo_startModification(FTABLE *fp);
extern void ft_settime(EDTIME *tp);

extern long	_multiplier;
extern int	_lastinsertedmac;

extern BYTECODE_BUFFER _currentRecordingBuffer;

int			_playing;
static ARRAY_LIST* _contextStack;
static EXECUTION_CONTEXT* _currentExecutionContext;
static jmp_buf _currentJumpBuffer;
static COM_FORM* _currentFormInstruction;

/*
 * An error has occurred during execution of a macro. Display a descriptive error and abort the execution.
 */
void interpreter_raiseError(const char* pFormat, ...) {
	char szBuffer[256];
	char szMessage[512];
	va_list ap;

	va_start(ap, pFormat);
	vsprintf(szBuffer, pFormat, ap);
	va_end(ap);
	sprintf(szMessage, "Exception occurred when executing macro/function '%s'\n%s", _currentExecutionContext ? _currentExecutionContext->ec_currentFunction : "?", szBuffer);
	error_displayAlertDialog(szMessage);
	longjmp(_currentJumpBuffer, 1);
}

/*
 * Decode a push-arraylist operation and convert to an array list. 
 */
static ARRAY_LIST* interpreter_decodeArrayList(EXECUTION_CONTEXT* pContext, COM_ARRAYLITERAL* pPointer) {
	ARRAY_LIST* pList = arraylist_create(pPointer->length);
	char* p = pPointer->strings;
	for (int i = 0; i < pPointer->length; i++) {
		arraylist_add(pList, _strdup(p));
		while (*p++) {

		}
	}
	memory_add(pContext, (PKS_VALUE) { .sym_type = S_STRING_ARRAY, .sym_data.stringList = pList });
	return pList;
}

/*--------------------------------------------------------------------------
 * interpreter_getValueForOpCode()
 * pop data from execution stack
 */
static PKS_VALUE interpreter_getValueForOpCode(EXECUTION_CONTEXT* pContext, unsigned char* pInstructionPointer) {
	unsigned char typ = *pInstructionPointer;

	switch (typ) {
	case C_FORM_START:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.string = pInstructionPointer};
	case C_PUSH_BOOLEAN_LITERAL:
		return (PKS_VALUE) { .sym_type = S_BOOLEAN, .sym_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val};
	case C_PUSH_SMALL_INT_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val };
	case C_PUSH_CHARACTER_LITERAL:
		return (PKS_VALUE) { .sym_type = S_CHARACTER, .sym_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val };
	case C_PUSH_INTEGER_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = ((COM_INT1*)pInstructionPointer)->val };
	case C_PUSH_LONG_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = ((COM_LONG1*)pInstructionPointer)->val };
	case C_PUSH_FLOAT_LITERAL: {
		double dDouble = ((COM_FLOAT1*)pInstructionPointer)->val;
		return (PKS_VALUE) { .sym_type = S_FLOAT, .sym_data.doubleValue = dDouble };
	}
	case C_PUSH_ARRAY_LITERAL:
		return (PKS_VALUE) {
			// TODO
			.sym_type = S_STRING_ARRAY, .sym_data.stringList = interpreter_decodeArrayList(pContext, (COM_ARRAYLITERAL*)pInstructionPointer)
		};
	case C_PUSH_STRING_LITERAL:
		return (PKS_VALUE) {
			.sym_type = S_STRING, .sym_data.string = ((COM_STRING1*)pInstructionPointer)->s
		};
	case C_PUSH_VARIABLE:
		return sym_getVariable(pContext->ec_identifierContext, ((COM_STRING1*)pInstructionPointer)->s);
	default:
		interpreter_raiseError("Illegal push operation %d", typ);
		// not reached
		return (PKS_VALUE) { 0 };
	}
}

static void interpreter_deallocateStack(EXECUTION_CONTEXT* pContext) {
	free(pContext->ec_stackBottom);
	memory_destroy(pContext->ec_allocations);
}

/*
 * Initialize a new context - its stack and allocation values.
 */
static void interpreter_allocateStack(EXECUTION_CONTEXT* pResult) {
	PKS_VALUE* pStack = calloc(MAX_STACK_SIZE, sizeof(*pStack));
	pResult->ec_stackBottom = pStack;
	pResult->ec_stackCurrent = pStack;
	pResult->ec_stackFrame = pStack;
	pResult->ec_stackMax = pStack + MAX_STACK_SIZE;
	pResult->ec_allocations = memory_create();
}

/*
 * Creates an execution context for executing a macro function.
 */
static EXECUTION_CONTEXT* interpreter_pushExecutionContext(const char* pszFunctionName) {
	EXECUTION_CONTEXT* pResult = calloc(1, sizeof * pResult);
	pResult->ec_identifierContext = sym_pushContext(sym_getGlobalContext());
	if (_contextStack == NULL) {
		_contextStack = arraylist_create(7);
	}
	arraylist_add(_contextStack, pResult);
	if (_currentExecutionContext != NULL) {
		pResult->ec_stackBottom = _currentExecutionContext->ec_stackBottom;
		pResult->ec_stackMax = _currentExecutionContext->ec_stackMax;
		pResult->ec_stackCurrent = _currentExecutionContext->ec_stackCurrent;
		pResult->ec_stackFrame = _currentExecutionContext->ec_stackFrame;
		pResult->ec_allocations = _currentExecutionContext->ec_allocations;
	} else {
		interpreter_allocateStack(pResult);
	}
	pResult->ec_currentFunction = pszFunctionName;
	_currentExecutionContext = pResult;
	return pResult;
}

/*
 * Destroy a previousy allocated execution context.
 */
static void interpreter_popExecutionContext(EXECUTION_CONTEXT* pContext, EXECUTION_CONTEXT* pPreviousContext) {
	sym_popContext(pContext->ec_identifierContext);
	if (pPreviousContext == NULL) {
		interpreter_deallocateStack(pContext);
	}
	free(pContext);
	if (_contextStack) {
		arraylist_remove(_contextStack, pContext);
		if (!pPreviousContext) {
			arraylist_destroy(_contextStack);
			_contextStack = NULL;
		}
	}
	_currentExecutionContext = pPreviousContext;
}

/*
 * Used to release resources after an exception.
 */
static void interpreter_cleanupContextStacks() {
	if (_contextStack) {
		int nLast = (int)arraylist_size(_contextStack);
		while (--nLast >= 0) {
			EXECUTION_CONTEXT* pPrevious = nLast - 1 >= 0 ? arraylist_get(_contextStack, nLast - 1) : 0;
			interpreter_popExecutionContext(arraylist_get(_contextStack, nLast), pPrevious);
		}
	}
}

/*
 * Implements the size() method used to determine the length of a string or the size of an array or object.
 */
PKS_VALUE interpreter_size(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 1) {
		return (PKS_VALUE) { .sym_type = S_BOOLEAN, .sym_data.booleanValue = 0 };
	}
	PKS_VALUE v = pValues[0];
	size_t nLen = 0;
	if (v.sym_type == S_STRING) {
		nLen = strlen(v.sym_data.string);
	} else if (v.sym_type == S_STRING_ARRAY) {
		nLen = arraylist_size(v.sym_data.stringList);
	}
	return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = nLen };
}

/*
 * Implements the foreach() method used in for(x : y) loops.
 */
PKS_VALUE interpreter_foreach(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 3) {
		return (PKS_VALUE) { .sym_type=S_BOOLEAN, .sym_data.booleanValue = 0 };
	}
	char* pszCaretVar = pValues[0].sym_data.string;
	char* pszResultVar = pValues[1].sym_data.string;
	PKS_VALUE v = pValues[2];
	int idxLast;
	if (v.sym_type == S_RANGE) {
		idxLast = v.sym_data.range.r_end + 1;
	}
	else if (v.sym_type == S_STRING) {
		idxLast = (int)strlen(v.sym_data.string);
	} else if (v.sym_type == S_STRING_ARRAY) {
		idxLast = (int)arraylist_size(v.sym_data.stringList);
	} else {
		return (PKS_VALUE) { .sym_type = S_BOOLEAN, .sym_data.booleanValue = 0 };
	}
	char* pszUnused;
	PKS_VALUE caretV = sym_find(pContext->ec_identifierContext, pszCaretVar, &pszUnused);
	if (NULLSYM(caretV)) {
		caretV = (PKS_VALUE){.sym_type = S_NUMBER, .sym_data.intValue = 0};
		sym_createSymbol(pContext->ec_identifierContext, pszCaretVar, S_NUMBER, caretV.sym_data);
	}
	int idx = caretV.sym_data.intValue;
	if (v.sym_type == S_RANGE) {
		int nMult = v.sym_data.range.r_increment > 0 ? v.sym_data.range.r_increment : 1;
		idx = v.sym_data.range.r_start + (nMult*idx);
	}
	if (idx >= idxLast) {
		sym_remove(pContext->ec_identifierContext, pszCaretVar);
		return (PKS_VALUE) { .sym_type = S_BOOLEAN, .sym_data.booleanValue = 0 };
	}
	if (v.sym_type == S_RANGE) {
		sym_createSymbol(pContext->ec_identifierContext, pszResultVar, S_NUMBER, (GENERIC_DATA) { .intValue = idx });
	} else if (v.sym_type == S_STRING_ARRAY) {
		sym_createSymbol(pContext->ec_identifierContext, pszResultVar, S_STRING, 
			(GENERIC_DATA) { .string = arraylist_get(v.sym_data.stringList, idx) });
	} else {		// string
		sym_createSymbol(pContext->ec_identifierContext, pszResultVar, S_CHARACTER, (GENERIC_DATA) { .uchar = v.sym_data.string[idx] });
	}
	caretV.sym_data.intValue++;
	sym_createSymbol(pContext->ec_identifierContext, pszCaretVar, S_NUMBER, caretV.sym_data);

	return (PKS_VALUE) { .sym_type = S_BOOLEAN, .sym_data.booleanValue = 1 };
}

/*
 * Implements the typeof() method.
 */
PKS_VALUE interpreter_typeOf(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 1) {
		return (PKS_VALUE) {0};
	}
	const char* pszName = "string";
	switch (pValues->sym_type) {
	case S_RANGE: pszName = "range"; break;
	case S_NUMBER: pszName = "int"; break;
	case S_FLOAT: pszName = "float"; break;
	case S_BOOLEAN: pszName = "boolean"; break;
	case S_CHARACTER: pszName = "char"; break;
	case S_STRING_ARRAY: pszName = "string[]"; break;
	}
	return (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, pszName) };
}

/*--------------------------------------------------------------------------
 * macroc_sprintf()
 * Similar to C sprintf, this will take a format and two parameters and print the result
 * and return the formatted result.
 */
PKS_VALUE interpreter_sprintf(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	char buf[1024];

	if (nArgs < 1 || pValues->sym_type != S_STRING) {
		return (PKS_VALUE) { .sym_type = S_NUMBER, 0 };
	}
	char* pszFormat = pValues->sym_data.string;
	union U_ARG_VALUE* values = calloc(nArgs, sizeof (* values));
	for (int i = 1; i < nArgs; i++) {
		if (pValues[i].sym_type == S_FLOAT) {
			values[i - 1].v_d = pValues[i].sym_data.doubleValue;
		} else if (pValues[i].sym_type == S_STRING) {
			values[i - 1].v_s = pValues[i].sym_data.string;
		} else {
			values[i - 1].v_l = (long)pValues[i].sym_data.longValue;
		}
	}
	mysprintf(buf, pszFormat, &(SPRINTF_ARGS){.sa_wp = ww_getCurrentEditorWindow(), .sa_values = values});
	free(values);
	return (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = interpreter_allocateString(pContext, buf) };
}


/*
 * Pop one value of the stack of our stack machine 
 */
PKS_VALUE interpreter_popStackValue(EXECUTION_CONTEXT* pContext) {
	if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
		return *--pContext->ec_stackCurrent;
	}
	return (PKS_VALUE) {.sym_data = 0, .sym_type = S_NUMBER};
}

/*
 * Te the current stack top value.
 */
PKS_VALUE interpreter_peekStackValue(EXECUTION_CONTEXT* pContext) {
	if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
		return pContext->ec_stackCurrent[-1];
	}
	return (PKS_VALUE) { .sym_data = 0, .sym_type = S_NUMBER };
}

/*
 * Push one value onto the stack and raise an error, if that fails.
 */
int interpreter_pushValueOntoStack(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue) {
	if (pContext->ec_stackCurrent >= pContext->ec_stackMax) {
		interpreter_raiseError("Stack overflow");
		return 0;
	}
	*pContext->ec_stackCurrent++ = nValue;
	return 1;
}

/*
 * Mini memory management function of PKS MacroC, which temporarily allocates a string to
 * be released later.
 */
char* interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource) {
	const char* pszResult = _strdup(pszSource);
	memory_add(pContext, (PKS_VALUE) { .sym_type = S_STRING, .sym_data.string = (char*)pszResult });
	return (char* )pszResult;
}

/*
 * interpreter_getParameterSize()
 * typ: the bytecode
 * s: pointer to the bytecode buffer past(!) the opcode (instructionPointer+1)
 */
int interpreter_getParameterSize(unsigned char typ, const char *s)
{	int size;

	switch(typ) {
		case C_0FUNC:
			return sizeof(COM_0FUNC);
		case C_POP_STACK:
		case C_SET_STACKFRAME:
		case C_PUSH_CHARACTER_LITERAL:
		case C_PUSH_SMALL_INT_LITERAL:
		case C_PUSH_BOOLEAN_LITERAL:
			return sizeof(COM_CHAR1);
		case C_PUSH_ARRAY_LITERAL:
			return ((COM_ARRAYLITERAL*)(s - 1))->totalBytes;
		case C_1FUNC:
			return sizeof(COM_1FUNC);
		case C_FORM_START:
			return sizeof(COM_FORM);
		case C_GOTO:
			return sizeof(COM_GOTO);
		case C_STOP:
			return sizeof(COM_STOP);
		case C_PUSH_FLOAT_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_FLOAT1);
		case C_PUSH_INTEGER_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_INT1);
		case C_DEFINE_PARAMETER:
		case C_DEFINE_VARIABLE:
			return (((COM_CREATESYM*)(s-1))->size);
		case C_PUSH_LONG_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_MACRO:
		case C_MACRO_REF:
			s = ((COM_MAC*)(s - 1))->name;
			return (int)(sizeof(COM_MAC) + strlen(s));
		case C_PUSH_VARIABLE:
		case C_ASSIGN:
		case C_PUSH_STRING_LITERAL:
			size = (int)((sizeof(COM_STRING1) - 1 /* pad byte*/) +
					(strlen(s)+1)*sizeof(*s));
			if (size & 1) size++;
			return size;
		case C_LOGICAL_OPERATION:
		case C_BINOP:
			return sizeof(COM_BINOP);
	}
	/*
	 * oops: this is an error
	 */
	interpreter_raiseError("Bad parameter size opcode %x", typ);
	return 10000L;
}

/*---------------------------------*/
/* interpreter_openDialog()				*/
/*---------------------------------*/
int interpreter_openDialog(PARAMS *pp)
{	int 		i;
	struct des 	*dp;
	GENERIC_DATA	par;
	COM_FORM	*cp;

	if (!_playing || !_currentFormInstruction || !_currentExecutionContext)
		return FORM_SHOW;

	cp = _currentFormInstruction;
	_currentFormInstruction = 0;

	if (cp->options & FORM_REDRAW) {
		ww_redrawAllWindows(1);
	}

	PKS_VALUE* pParams = _currentExecutionContext->ec_stackCurrent-cp->nfields;
	/*
	 * use form, if no fields known
	 */
	for (i = cp->nfields, dp = pp->el; i > 0; i--, dp++) {
		PKS_VALUE v = *pParams++;
		par = v.sym_data;
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_PUSH_INTEGER_LITERAL:  *dp->p.i = par.intValue; break;
			case C_PUSH_SMALL_INT_LITERAL:
			case C_PUSH_CHARACTER_LITERAL: *dp->p.c = par.uchar; break;
			case C_PUSH_FLOAT_LITERAL: *dp->p.d = par.doubleValue; break;
			case C_PUSH_LONG_LITERAL: *dp->p.l = (long)par.longValue; break;
			case C_PUSH_STRING_LITERAL:
				if ((v.sym_type == S_STRING) && par.string) {
					strcpy(dp->p.s, par.string);
				}
				else {
					error_showErrorById(IDS_MSGMACFORMATERR);
				}
				break;
		   }
		}
	}
	
	recorder_recordOperation(pp);

	return cp->options & FORM_SHOW;
}

/*
 * Returns FALSE; if the function described by the function pointer cannot 
 * be executed.
 */
int interpreter_isFunctionEnabled(EDFUNC* fup, long long pParam, int warn) {
	if (!fup->flags) {
		return 1;
	}
	WINFO* wp = ww_getCurrentEditorWindow();

	FTABLE* fp = wp ? wp->fp : NULL;

	if (fup->flags & EW_NEEDSCURRF && fp == 0) {
		return 0;
	}
	if ((fup->flags & EW_MODIFY) && (ft_isReadonly(fp) || !wp->renderer->r_canEdit)) {
		if (warn) {
			ft_checkReadonlyWithError(fp);
		}
		return 0;
	}

	if (fup->flags & EW_NEEDSBLK && (!wp || !ww_hasSelection(wp))) {
		return 0;
	}
	if ((fup->flags & EW_REDO_AVAILABLE) && !undo_isRedoAvailable(fp)) {
		return 0;
	}
	if ((fup->flags & EW_UNDO_AVAILABLE) && !undo_isUndoAvailable(fp)) {
		if (warn) {
			ww_checkSelectionWithError(wp);
		}
		return 0;
	}
	if (fup->flags & EW_COMPARISON_MODE && (wp == NULL || wp->comparisonLink == NULL)) {
		return 0;
	}
	if (fup->flags & EW_CUSTOM_ENABLEMENT && !fup->isenabled(pParam)) {
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * interpreter_canExecuteFunction()
 */
int interpreter_canExecuteFunction(int num, long long pParam, int warn) {
	EDFUNC *	fup = &_functionTable[num];
	return interpreter_isFunctionEnabled(fup, pParam, warn);
}

/*
 * Call a method generically (foreign function interface) - should use libffi on a long term run.
 */
static long long cdecl interpreter_callFfi(EDFUNC* pFunc, intptr_t p1, intptr_t p2, void* s1, void* s2, void* s3) {
	if (!pFunc->execute) {
		return 0;
	}
	// Handle typical function prototypes explicitly
	int nParams = function_getParameterCount(pFunc);
	if (nParams == 1) {
		if (function_getParameterTypeDescriptor(pFunc, 1).pt_type == PARAM_TYPE_STRING) {
			return (*pFunc->execute)(s1);
		}
	}
	else if (nParams == 2) {
		PARAMETER_TYPE_DESCRIPTOR pd1 = function_getParameterTypeDescriptor(pFunc, 1);
		PARAMETER_TYPE_DESCRIPTOR pd2 = function_getParameterTypeDescriptor(pFunc, 2);
		if (pd1.pt_type == PARAM_TYPE_STRING) {
			if (pd2.pt_type == PARAM_TYPE_STRING) {
				return (*pFunc->execute)(s1, s2);
			}
			return (*pFunc->execute)(s1, p1);
		}
		if (pd2.pt_type == PARAM_TYPE_STRING) {
			if (pd1.pt_type != PARAM_TYPE_STRING) {
				return (*pFunc->execute)(p1, s1);
			}
		}
	}
	return (*pFunc->execute)(p1, p2, s1, s2, s3);
}

/*---------------------------------*/
/* interpreter_executeFunction()					*/
/*---------------------------------*/
long long cdecl interpreter_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3) {
	EDFUNC *	fup = &_functionTable[num];
	long long 	ret = 0;
	int			i;

	if (!interpreter_isFunctionEnabled(fup, (long long)p1, 1)) {
		return 0;
	}

	if (fup->flags & EW_UNDOFLSH) {
		undo_startModification(ft_getCurrentDocument());
	}

	if ((fup->flags & EW_CCASH) == 0) {
		recorder_stopAutoInsertRecording(NULL, NULL);
		caret_removeSecondaryCarets();
	}

	recorder_recordFunctionWithParameters(num,(int)p1,p2,s1,s2);

	if ((i = _multiplier) > 1 && (fup->flags & EW_MULTI) == 0) {
		while (i-- > 0 && (ret = interpreter_callFfi(fup, p1,p2,s1,s2,s3)) != 0)
			;
		_multiplier = 1;
	} else {
		ret = interpreter_callFfi(fup, p1, p2, s1, s2, s3);
	}

	FTABLE* fp;
	if (ret > 0 && (fup->flags & EW_MODIFY) && (fp = ft_getCurrentDocument()) != NULL) {
		ft_settime(&fp->ti_lastChanged);
	}

	if (i > 1) {
		_multiplier = 1;
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * interpreter_returnFunctionValue()
 */
static void interpreter_returnFunctionValue(EXECUTION_CONTEXT* pContext, PARAMETER_TYPE typ, intptr_t v) {
	PKS_VALUE_TYPE vt = VT_NUMBER;
	if (typ == PARAM_TYPE_STRING) {
		v = (intptr_t)interpreter_allocateString(pContext, (const char*)v);
		vt = VT_STRING;
	} else if (typ == PARAM_TYPE_STRING_ARRAY) {
		// (A) . string arrays will always be allocated by caller
		memory_add(pContext, (PKS_VALUE) { .sym_type = S_STRING_ARRAY, .sym_data.stringList = (void*)v });
		vt = VT_STRING_ARRAY;
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = vt, .sym_data.longValue = v });
}

/*
 * Interpreter internal function to execute a macro.
 */
static int interpreter_executeMacroByName(char* name) {
	MACROREF macref;
	int i;
	if (name[0] == '@') {
		macref.typ = CMD_CMDSEQ;
		i = macro_getCmdIndexByName(name + 1);
	}
	else {
		macref.typ = CMD_MACRO;
		i = macro_getInternalIndexByName(name);
	}
	if (i < 0) {
		interpreter_raiseError("Undefined macro function %s", name);
		return 0;
	}
	macref.index = i;
	return (int)macro_executeMacro(&macref);
}

/*---------------------------------*/
/* interpreter_doMacroFunctions()				*/
/*---------------------------------*/
intptr_t interpreter_doMacroFunctions(EXECUTION_CONTEXT* pContext, COM_1FUNC **pInstructionPointer, const COM_1FUNC *cpmax) {
	intptr_t	nativeStack[5];
	PKS_VALUE	tempStack[5];
	intptr_t	rc;
	intptr_t *	functionParameters;
	char**		stringFunctionParameters;
	unsigned char 	typ;
	int  		funcnum;
	BOOL		bNativeCall;
	BOOL		bInternalNativeCall = FALSE;
	PKS_VALUE   returnValue;
	COM_1FUNC *	pLocalInstructionPointer = *pInstructionPointer;

	typ = pLocalInstructionPointer->typ;
	functionParameters = nativeStack;
	char* pszMacro = ((COM_MAC*)*pInstructionPointer)->name;
	funcnum = pLocalInstructionPointer->funcnum;
	EDFUNC* fup = &_functionTable[funcnum];
	bNativeCall = typ != C_MACRO && typ != C_MACRO_REF;
	if (typ == C_MACRO_REF) {
		PKS_VALUE v = sym_getVariable(pContext->ec_identifierContext, pszMacro);
		if (v.sym_type != S_STRING) {
			interpreter_raiseError("Illegal reference to macro function named %s", pszMacro);
		}
		pszMacro = v.sym_data.string;
		char* ret;
		PKS_VALUE sym = sym_find(sym_getGlobalContext(), pszMacro, &ret);
		if (sym.sym_type == S_EDFUNC) {
			fup = (void*)VALUE(sym);
			bNativeCall = TRUE;
		}
	}
	if (bNativeCall) {
		if (function_hasInternalVMPrototype(fup)) {
			bInternalNativeCall = TRUE;
			bNativeCall = FALSE;
		}
	}
	int nParametersPassed = ((COM_0FUNC*)pLocalInstructionPointer)->func_nargs;
	if (bNativeCall) {
		stringFunctionParameters = (char**)(functionParameters + 2);
		memset(nativeStack, 0, sizeof nativeStack);
		if (typ == C_1FUNC) {
			*functionParameters++ = pLocalInstructionPointer->p;
			nParametersPassed--;
		}
	}

	if (bNativeCall && !_currentFormInstruction) {
		EDFUNC* fup = &_functionTable[funcnum];
		int i;
		for (i = 0; i < nParametersPassed; i++) {
			tempStack[i] = interpreter_popStackValue(pContext);
		}
		for (i = 0; i < nParametersPassed; i++) {
			PKS_VALUE v = tempStack[nParametersPassed-i-1];
			if (function_getParameterTypeDescriptor(fup, i + 1).pt_type == PARAM_TYPE_STRING) {
				v = interpreter_coerce(pContext, v, S_STRING);
			}
			if (v.sym_type == S_STRING) {
				*stringFunctionParameters++ = v.sym_data.string;
			} else {
				*functionParameters++ = v.sym_data.intValue;
			}
		}
	}
	pLocalInstructionPointer = (COM_1FUNC*)((unsigned char*)pLocalInstructionPointer + interpreter_getParameterSize(typ, &pLocalInstructionPointer->funcnum));
	if (bNativeCall) {
		EDFUNC* fup = &_functionTable[funcnum];
		rc = interpreter_executeFunction(funcnum,
						nativeStack[0], 
						nativeStack[1],
						 (void*)nativeStack[2],
						 (void*)nativeStack[3],
						 (void*)nativeStack[4]);
		PARAMETER_TYPE pReturnType = function_getParameterTypeDescriptor(fup, 0).pt_type;
		interpreter_returnFunctionValue(pContext, pReturnType, rc);
	} else {
		if (bInternalNativeCall) {
			memset(tempStack, 0, sizeof tempStack);
			for (int i = 0; i < nParametersPassed; i++) {
				tempStack[nParametersPassed -i-1] = interpreter_popStackValue(pContext);
			}
			EDFUNC* fup = &_functionTable[funcnum];
			returnValue = ((PKS_VALUE (*)())fup->execute)(pContext, tempStack, nParametersPassed);
		} else {
			PKS_VALUE* pOld = pContext->ec_stackFrame;
			pContext->ec_stackFrame = pContext->ec_stackCurrent - nParametersPassed;
			if (pContext->ec_stackFrame < pContext->ec_stackBottom) {
				pContext->ec_stackFrame = pContext->ec_stackBottom;
			}
			rc = interpreter_executeMacroByName(pszMacro);
			pContext->ec_stackCurrent++;
			returnValue = interpreter_popStackValue(pContext);
			pContext->ec_stackCurrent -= nParametersPassed;
			if (pContext->ec_stackCurrent < pContext->ec_stackBottom) {
				pContext->ec_stackCurrent = pContext->ec_stackBottom;
			}
			pContext->ec_stackFrame = pOld;
		}
		interpreter_pushValueOntoStack(pContext, returnValue);
	}
	*pInstructionPointer = pLocalInstructionPointer;
	return rc;
}

/*--------------------------------------------------------------------------
 * interpreter_assignSymbol()
 */
static long interpreter_assignSymbol(EXECUTION_CONTEXT* pContext, char* name) {
	PKS_VALUE v = interpreter_peekStackValue(pContext);

	return sym_createSymbol(pContext->ec_identifierContext, name, v.sym_type, v.sym_data);
}


/*
 * Returns a value from the parameter stack. 
 */
static PKS_VALUE interpreter_getParameterStackValue(EXECUTION_CONTEXT* pContext, int nParamIndex) {
	PKS_VALUE* pSlot = pContext->ec_stackFrame + nParamIndex;
	if (pSlot < pContext->ec_stackCurrent) {
		return *pSlot;
	}
	// NIL
	return (PKS_VALUE) {.sym_type = 0, .sym_data.longValue = 0};
}

/*---------------------------------*/
/* macro_interpretByteCodes()      */
/*---------------------------------*/
#define COM1_INCR(pLocalInstructionPointer,type,offset) (((unsigned char *)pLocalInstructionPointer)+((type *)pLocalInstructionPointer)->offset)
#define COM_PARAMINCR(pLocalInstructionPointer)		(((unsigned char *)pLocalInstructionPointer)+interpreter_getParameterSize(pLocalInstructionPointer->typ,&pLocalInstructionPointer->funcnum));
static int _macaborted;
static int macro_interpretByteCodesContext(EXECUTION_CONTEXT* pContext, const char* pszFunctionName, COM_1FUNC* cp, COM_1FUNC* cpmax) {

	unsigned char* pInstr = (unsigned char*)cp;
	unsigned char* pInstrMax = (unsigned char*)cpmax;
	while (pInstr < pInstrMax) {
		cp = (COM_1FUNC*)pInstr;
		if (_macaborted || (_macaborted = progress_cancelMonitor(FALSE)) != 0) {
			goto end;
		}
		switch (cp->typ) {
		case C_GOTO:
			if (((COM_GOTO*)cp)->bratyp == BRA_ALWAYS) {
				pInstr = COM1_INCR(cp, COM_GOTO, offset);
			}
			else {
				PKS_VALUE stackTop = interpreter_popStackValue(pContext);
				stackTop = interpreter_coerce(pContext, stackTop, S_BOOLEAN);
				int val = stackTop.sym_data.booleanValue;
				if ((((COM_GOTO*)cp)->bratyp == BRA_IF_TRUE && val != 0) ||
					(((COM_GOTO*)cp)->bratyp == BRA_IF_FALSE && val == 0))
					pInstr = COM1_INCR(cp, COM_GOTO, offset);
				else
					pInstr = COM_PARAMINCR(cp);
			}
			break;
		case C_POP_STACK:
			if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
				pContext->ec_stackCurrent--;
			}
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		case C_SET_STACKFRAME:
			pContext->ec_stackFrame = pContext->ec_stackCurrent;
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		case C_LOGICAL_OPERATION:
			interpreter_testExpression(pContext, (COM_BINOP*)cp);
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			continue;
		case C_BINOP:
			interpreter_evaluateBinaryExpression(pContext, (COM_BINOP*)cp);
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			continue;
		case C_FORM_START:
			_currentFormInstruction = (COM_FORM*)pInstr;
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		case C_PUSH_BOOLEAN_LITERAL:
		case C_PUSH_LONG_LITERAL:
		case C_PUSH_SMALL_INT_LITERAL:
		case C_PUSH_CHARACTER_LITERAL:
		case C_PUSH_FLOAT_LITERAL:
		case C_PUSH_INTEGER_LITERAL:
		case C_PUSH_VARIABLE:
		case C_PUSH_ARRAY_LITERAL: 
		case C_PUSH_STRING_LITERAL: {
			PKS_VALUE value = interpreter_getValueForOpCode(pContext, pInstr);
			interpreter_pushValueOntoStack(pContext, value);
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		}
		case C_DEFINE_PARAMETER: {
			PKS_VALUE value = interpreter_getParameterStackValue(pContext, (int)((COM_CREATESYM*)cp)->value);
			// automatic type coercion of parameter types.
			if (!value.sym_type && ((COM_CREATESYM*)cp)->symtype == S_STRING) {
				// TODO: this is a hack - if the parameter was not passed we need to define the symbol and assign it a NIL value, 
				// which is currently not supported. So convert to false to allow us for testing, whether the parameter was passed.
				value = (PKS_VALUE){.sym_type = S_BOOLEAN, .sym_data.booleanValue = 0};
			}
			else if (((COM_CREATESYM*)cp)->symtype != S_AUTO) {
				value = interpreter_coerce(pContext, value, ((COM_CREATESYM*)cp)->symtype);
			}
			sym_createSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
				value.sym_type,
				value.sym_data);
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		}
		case C_DEFINE_VARIABLE:
			sym_createSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
				((COM_CREATESYM*)cp)->symtype,
				(GENERIC_DATA) {
				.longValue = ((COM_CREATESYM*)cp)->value
			});
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		case C_ASSIGN:
			interpreter_assignSymbol(pContext, ((COM_ASSIGN*)cp)->name);
			pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
			break;
		case C_STOP:
			goto end;
		case C_MACRO_REF: case C_MACRO: case C_0FUNC: case C_1FUNC:
			interpreter_doMacroFunctions(pContext, &cp, cpmax);
			pInstr = (unsigned char*)cp;
			continue;
		default:
			interpreter_raiseError("Corrupted bytecodes - cannot continue.");
			// never reached
			goto end;
		}
	}
end:
	return (int)1;
}


/*---------------------------------*/
/* macro_interpretByteCodes()      */
/*---------------------------------*/
static int macro_interpretByteCodes(const char* pszFunctionName, COM_1FUNC *cp,COM_1FUNC *cpmax) {
	static int 	level;
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT* pContext = interpreter_pushExecutionContext(pszFunctionName);

	if (level == 0 && setjmp(_currentJumpBuffer)) {
		// TODO: more cleanup!
		interpreter_cleanupContextStacks();
		level = 0;
		return -1;
	}
	level++;
	int ret = macro_interpretByteCodesContext(pContext, pszFunctionName, cp, cpmax);
	level--;
	if (level == 0) {
		error_setShowMessages(TRUE);
	}
	interpreter_popExecutionContext(pContext, pOld);
	return (int)1;
}

/*
 * Initialize a namespace.
 */
static int interpreter_initializeNamespace(MACRO* mpNamespace) {
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT ecNamespace;
	ecNamespace.ec_identifierContext = sym_getGlobalContext();
	_currentExecutionContext = &ecNamespace;
	interpreter_allocateStack(_currentExecutionContext);
	int ret = macro_interpretByteCodesContext(_currentExecutionContext, mpNamespace->mc_name, 
		(COM_1FUNC*)mpNamespace->mc_bytecodes, (COM_1FUNC*)(mpNamespace->mc_bytecodes+ mpNamespace->mc_bytecodeLength));
	interpreter_deallocateStack(_currentExecutionContext);
	_currentExecutionContext = pOld;
	mpNamespace->mc_isInitialized = TRUE;
	return 1;
}

/*---------------------------------
 * macro_executeMacroByIndex()
 * Execute a macro given its logical
 * internal macro number.
 */
long long macro_executeMacroByIndex(int macroindex)
{
	int 		ret = 1;
	int			i;
	int			count;
	int			wasplaying;
	COM_1FUNC *	cp;
	COM_1FUNC *	cpmax;
	MACRO *		mp;
	MACRO*		mpNamespace;
	long   		bWasRecording = recorder_isRecording();

	if (macroindex == MAC_LASTREC && 
	    (macroindex = _lastinsertedmac) < 0)
		return 0;

	if ((wasplaying = _playing) == 0 && ft_getCurrentDocument()) {
		undo_startModification(ft_getCurrentDocument());
	} 

	if (bWasRecording && macroindex >= 0) {
		recorder_pushSequence(C_MACRO,MAC_NAME(macro_getByIndex(macroindex)));
		recorder_setRecording(FALSE);
	}

	_playing = 1;
	count = _multiplier;
	_multiplier = 1;
	for (i = 0; i < count; i++) {
		const char* pszName = "auto";
		switch(macroindex) {
			case MAC_AUTO:	
				recorder_stopAutoInsertRecording(&cp, &cpmax);
				_playing = 2;
				break;
			default:
				if ((mp = macro_getByIndex(macroindex)) == 0)
					return 0;
				cp = (COM_1FUNC *)MAC_DATA(mp);
				cpmax = (COM_1FUNC *)((char *)cp+mp->mc_bytecodeLength);
				pszName = mp->mc_name;
				mpNamespace = macro_getNamespaceByIdx(mp->mc_namespaceIdx);
				if (mpNamespace && !mpNamespace->mc_isInitialized) {
					if (!interpreter_initializeNamespace(mpNamespace)) {
						return 0;
					}
				}
				break;
		}
		ret = macro_interpretByteCodes(pszName, cp, cpmax);
		if (ret <= 0) {
			ret = 0;
			break;
		}
	}

	if ((_playing = wasplaying) == 0) {
		_macaborted = 0;
		WINFO* wp = ww_getCurrentEditorWindow();
		if (wp) {
			ft_selectWindowWithId(wp->win_id, FALSE);
		}
	}

	recorder_setRecording(bWasRecording);

	return ret;
}

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
long long macro_executeMacro(MACROREF* mp) {
	COM_1FUNC* cp;
	long long ret;

	switch (mp->typ) {
	case CMD_CMDSEQ:
		cp = &_commandTable[mp->index].c_functionDef;
		return macro_interpretByteCodes(_commandTable[mp->index].c_name, cp, cp + 1);
	case CMD_MACRO:
		// TODO: need a progress indicator popping up after an initial delay.
		// progress_startMonitor(IDS_ABRTMACRO);
		ret = macro_executeMacroByIndex(mp->index);
		progress_closeMonitor(0);
		return ret;
	default:
		error_displayAlertDialog("Illegal command type to execute.");
	}

	return 0;
}


