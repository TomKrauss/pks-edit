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

#include "winfo.h"
#include "edierror.h"
#include "errordialogs.h"

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
void recorder_recordFunctionWithParameters(int fnum, int p, intptr_t p2, char* s1, char* s2);

extern void 			macro_reportError(void);

extern int 				ft_selectWindowWithId(int winid, BOOL bPopup);


/*--------------------------------------------------------------------------
 * interpreter_testExpression()
 * Test an expression in a macro.
 */
extern int interpreter_testExpression(EXECUTION_CONTEXT* pContext, COM_BINOP* sp);
extern void macro_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp);

extern int 	progress_cancelMonitor(BOOL bRedraw);
extern void ww_redrawAllWindows(int update);
extern void undo_startModification(FTABLE *fp);
extern void ft_settime(EDTIME *tp);
extern int 	macro_executeByName(char *name);

int			_playing;

extern long	_multiplier;
extern int	_lastinsertedmac;

extern BYTECODE_BUFFER _currentRecordingBuffer;
static unsigned char* _dialogExecutionBytecodePointer;
static EXECUTION_CONTEXT* _currentExecutionContext;

/*
 * Interpreter error handling.
 */
static void interpreter_raiseError(const char* pMessage) {
	error_displayAlertDialog(pMessage);
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
	case C_PUSH_CHARACTER_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val };
	case C_PUSH_INTEGER_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = ((COM_INT1*)pInstructionPointer)->val };
	case C_PUSH_LONG_LITERAL:
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.longValue = ((COM_LONG1*)pInstructionPointer)->val };
	case C_PUSH_FLOAT_LITERAL: {
		double dDouble = ((COM_FLOAT1*)pInstructionPointer)->val;
		return (PKS_VALUE) { .sym_type = S_NUMBER, .sym_data.doubleValue = dDouble };
	}
	case C_PUSH_STRING_LITERAL:
		return (PKS_VALUE) {
			.sym_type = S_STRING, .sym_data.string = ((COM_STRING1*)pInstructionPointer)->s
		};
	case C_PUSH_VARIABLE:
		return sym_getVariable(pContext->ec_identifierContext, ((COM_VAR*)pInstructionPointer)->name);
	default:
		macro_reportError();
		return (PKS_VALUE) { 0 };
	}
}

/*
 * Creates an execution context for executing a macro function.
 */
static EXECUTION_CONTEXT* interpreter_pushExecutionContext() {
	EXECUTION_CONTEXT* pResult = calloc(1, sizeof * pResult);
	pResult->ec_identifierContext = sym_pushContext(sym_getGlobalContext());
	if (_currentExecutionContext != NULL) {
		pResult->ec_stackBottom = _currentExecutionContext->ec_stackBottom;
		pResult->ec_stackMax = _currentExecutionContext->ec_stackMax;
		pResult->ec_stackCurrent = _currentExecutionContext->ec_stackCurrent;
		pResult->ec_stackFrame = _currentExecutionContext->ec_stackFrame;
		pResult->ec_parameterStack = _currentExecutionContext->ec_parameterStack;
		pResult->ec_allocations = _currentExecutionContext->ec_allocations;
	} else {
		PKS_VALUE* pStack = calloc(MAX_STACK_SIZE, sizeof(*pStack));
		pResult->ec_stackBottom = pStack;
		pResult->ec_stackCurrent = pStack;
		pResult->ec_stackFrame = pStack;
		pResult->ec_parameterStack = pStack;
		pResult->ec_stackMax = pStack + MAX_STACK_SIZE;
		pResult->ec_allocations = arraylist_create(37);
	}
	_currentExecutionContext = pResult;
	return pResult;
}

/*
 * Destroy a previousy allocated execution context.
 */
static void interpreter_popExecutionContext(EXECUTION_CONTEXT* pContext, EXECUTION_CONTEXT* pPreviousContext) {
	sym_popContext(pContext->ec_identifierContext);
	if (pPreviousContext == NULL) {
		free(pContext->ec_stackBottom);
		size_t nSize = arraylist_size(pContext->ec_allocations);
		for (int i = 0; i < nSize; i++) {
			free(arraylist_get(pContext->ec_allocations, i));
		}
		arraylist_destroy(pContext->ec_allocations);
	}
	free(pContext);
	_currentExecutionContext = pPreviousContext;
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
 * Try to get rid of some allocated memory not referenced by the stack any more.
 */
static void interpreter_garbaggeCollect(EXECUTION_CONTEXT* pContext) {
	size_t nSize = arraylist_size(pContext->ec_allocations);
	if (nSize > 15) {
		for (int i = (int)nSize; --i >= 0; ) {
			char* pszEntry = arraylist_get(pContext->ec_allocations, i);
			PKS_VALUE* pStackValues = pContext->ec_stackBottom;
			BOOL bDelete = TRUE;
			while (pStackValues < pContext->ec_stackCurrent) {
				PKS_VALUE v = *pStackValues++;
				if (v.sym_type == S_STRING && v.sym_data.string == pszEntry) {
					bDelete = FALSE;
					break;
				}
			}
			if (bDelete) {
				arraylist_removeAt(pContext->ec_allocations, i);
				free(pszEntry);
			}
		}
	}
}

/*
 * Mini memory management function of PKS MacroC, which temporarily allocates a string to
 * be released later.
 */
char* interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource) {
	interpreter_garbaggeCollect(pContext);
	char* pszResult = _strdup(pszSource);
	arraylist_add(pContext->ec_allocations, pszResult);
	return pszResult;
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
		case C_SET_PARAMETER_STACK:
		case C_PUSH_CHARACTER_LITERAL:
		case C_PUSH_BOOLEAN_LITERAL:
			return sizeof(COM_CHAR1);
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
		case C_PUSH_VARIABLE:
		case C_MACRO:
		case C_ASSIGN:
		case C_PUSH_STRING_LITERAL:
			size = (int)((sizeof(struct c_ident) - 1 /* pad byte*/) +
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
	macro_reportError();
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
	unsigned char	type;

	if (!_playing)
		return FORM_SHOW;

	if (*_dialogExecutionBytecodePointer != C_FORM_START) {
		return FORM_SHOW;
	}

	cp = (COM_FORM *) (interpreter_getValueForOpCode(_currentExecutionContext, _dialogExecutionBytecodePointer).sym_data.string);
	_dialogExecutionBytecodePointer += interpreter_getParameterSize(*_dialogExecutionBytecodePointer, _dialogExecutionBytecodePointer + 1);
	if (!cp) {
		return FORM_SHOW;
	}

	if (cp->options & FORM_REDRAW) {
		ww_redrawAllWindows(1);
	}

	/*
	 * use form, if no fields known
	 */

	for (i = cp->nfields, dp = pp->el; i > 0; i--, dp++) {
		type = *_dialogExecutionBytecodePointer;
		par = interpreter_getValueForOpCode(_currentExecutionContext, _dialogExecutionBytecodePointer).sym_data;
		_dialogExecutionBytecodePointer += interpreter_getParameterSize(*_dialogExecutionBytecodePointer, _dialogExecutionBytecodePointer + 1);
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_PUSH_INTEGER_LITERAL:  *dp->p.i = par.intValue; break;
			case C_PUSH_CHARACTER_LITERAL: *dp->p.c = par.uchar; break;
			case C_PUSH_FLOAT_LITERAL: *dp->p.d = par.doubleValue; break;
			case C_PUSH_LONG_LITERAL: *dp->p.l = (long)par.longValue; break;
			case C_PUSH_STRING_LITERAL:
				if ((type == C_PUSH_STRING_LITERAL || type == C_PUSH_VARIABLE) && par.string) {
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
		if (pd2.pt_type == PARAM_TYPE_STRING) {
			if (pd1.pt_type == PARAM_TYPE_STRING) {
				return (*pFunc->execute)(s1, s2);
			}
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
static void interpreter_returnFunctionValue(EXECUTION_CONTEXT* pContext, unsigned char typ, intptr_t v) {
	if (typ == S_STRING) {
		v = (intptr_t)interpreter_allocateString(pContext, (const char*)v);
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .sym_type = typ, .sym_data.longValue = v });
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
	PKS_VALUE   returnValue;
	COM_1FUNC *	pLocalInstructionPointer = *pInstructionPointer;

	typ = pLocalInstructionPointer->typ;
	functionParameters = nativeStack;
	funcnum = pLocalInstructionPointer->funcnum;
	if (typ != C_MACRO) {
		stringFunctionParameters = (char**)(functionParameters + 2);
		memset(nativeStack, 0, sizeof nativeStack);
		if (typ == C_1FUNC) {
			*functionParameters++ = pLocalInstructionPointer->p;
		}
	}

	if (typ != C_MACRO) {
		EDFUNC* fup = &_functionTable[funcnum];
		int i;
		int nMax = ((COM_0FUNC*)pLocalInstructionPointer)->func_nargs;
		int nNativeParams = function_getParameterCount(fup);
		if (nMax > nNativeParams) {
			nMax = nNativeParams;
		}
		for (i = 0; i < nMax; i++) {
			tempStack[i] = interpreter_popStackValue(pContext);
		}
		for (i = 0; i < nMax; i++) {
			PKS_VALUE v = tempStack[nMax-i-1];
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
	if (typ != C_MACRO) {
		unsigned char* readparamp = (unsigned char *)pLocalInstructionPointer;
		_dialogExecutionBytecodePointer = readparamp;
		EDFUNC* fup = &_functionTable[funcnum];
		rc = interpreter_executeFunction(funcnum,
						nativeStack[0], 
						nativeStack[1],
						 (void*)nativeStack[2],
						 (void*)nativeStack[3],
						 (void*)nativeStack[4]);
		interpreter_returnFunctionValue(pContext, function_returnsString(fup) ? S_STRING : S_NUMBER, rc);
		/*
		 * TODO: function execution may pop a parameter in case a form is opened
		 * This is currently broken.
		 */
		*pInstructionPointer = (COM_1FUNC*)readparamp;
	} else {
		rc = macro_executeByName(((COM_MAC*)*pInstructionPointer)->name);
		*pInstructionPointer = pLocalInstructionPointer;
		// Work around for wrong stack manipulation on call site.
		pContext->ec_stackCurrent++;
		returnValue = interpreter_popStackValue(pContext);
		pContext->ec_stackCurrent = pContext->ec_stackFrame;
		interpreter_pushValueOntoStack(pContext, returnValue);
	}
	return rc;
}

/*--------------------------------------------------------------------------
 * interpreter_assignSymbol()
 */
static long interpreter_assignSymbol(EXECUTION_CONTEXT* pContext, char* name) {
	PKS_VALUE v = interpreter_peekStackValue(pContext);

	return sym_makeInternalSymbol(pContext->ec_identifierContext, name, v.sym_type, v.sym_data);
}


/*
 * Returns a value from the parameter stack. 
 */
static PKS_VALUE interpreter_getParameterStackValue(EXECUTION_CONTEXT* pContext, int nParamIndex) {
	PKS_VALUE* pSlot = pContext->ec_parameterStack + nParamIndex;
	if (pSlot < pContext->ec_stackCurrent) {
		return *pSlot;
	}
	return (PKS_VALUE) {.sym_type = S_NUMBER, .sym_data.longValue = 0};
}

/*---------------------------------*/
/* macro_interpretByteCodes()					*/
/*---------------------------------*/
#define COM1_INCR(pLocalInstructionPointer,type,offset) (((unsigned char *)pLocalInstructionPointer)+((type *)pLocalInstructionPointer)->offset)
#define COM_PARAMINCR(pLocalInstructionPointer)		(((unsigned char *)pLocalInstructionPointer)+interpreter_getParameterSize(pLocalInstructionPointer->typ,&pLocalInstructionPointer->funcnum));
static int _macaborted;
static int macro_interpretByteCodes(COM_1FUNC *cp,COM_1FUNC *cpmax) {
	static int 	level;
	unsigned char* pInstr;
	unsigned char* pInstrMax;
	if (level > 10) {
		// TODO: make this dependent on stack
		error_showErrorById(IDS_MSGMACRORECURSION);
		return 0;
	}
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT* pContext = interpreter_pushExecutionContext();

	level++;
	pInstr = (unsigned char*)cp;
	pInstrMax = (unsigned char*)cpmax;
	while (pInstr < pInstrMax) {
		cp = (COM_1FUNC*) pInstr;
		if (_macaborted || (_macaborted = progress_cancelMonitor(FALSE)) != 0) {
			goto end;
		}
		switch(cp->typ) {
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
			case C_SET_PARAMETER_STACK:
				pContext->ec_parameterStack = pContext->ec_stackCurrent;
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			case C_SET_STACKFRAME:
				pContext->ec_stackFrame = pContext->ec_parameterStack;
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			case C_LOGICAL_OPERATION:
				interpreter_testExpression(pContext, (COM_BINOP*)cp);
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				continue;
			case C_BINOP:
				macro_evaluateBinaryExpression(pContext, (COM_BINOP*)cp);
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				continue;
			case C_PUSH_BOOLEAN_LITERAL:
			case C_PUSH_LONG_LITERAL:
			case C_PUSH_CHARACTER_LITERAL:
			case C_PUSH_FLOAT_LITERAL:
			case C_PUSH_INTEGER_LITERAL:
			case C_PUSH_VARIABLE:
			case C_PUSH_STRING_LITERAL: {
				PKS_VALUE value = interpreter_getValueForOpCode(pContext, pInstr);
				interpreter_pushValueOntoStack(pContext, value);
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			}
			case C_DEFINE_PARAMETER: {
				PKS_VALUE value = interpreter_getParameterStackValue(pContext, (int)((COM_CREATESYM*)cp)->value);
				// automatic type coercion of parameter types.
				value = interpreter_coerce(pContext, value, ((COM_CREATESYM*)cp)->symtype);
				sym_makeInternalSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
					value.sym_type,
					value.sym_data);
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			}
			case C_DEFINE_VARIABLE:
				sym_makeInternalSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
					((COM_CREATESYM*)cp)->symtype,
					(GENERIC_DATA) {.longValue = ((COM_CREATESYM*)cp)->value});
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			case C_ASSIGN:
				interpreter_assignSymbol(pContext, ((COM_ASSIGN*)cp)->name);
				pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
				break;
			case C_STOP:
				goto end;
			case C_MACRO: case C_0FUNC: case C_1FUNC:
				interpreter_doMacroFunctions(pContext, &cp,cpmax);
				pInstr = (unsigned char*)cp;
				continue;
			case C_FORM_START:
				// TODO: not really - executing macros with forms is broken - should change the whole
				// macro with forms mechanism, on a long term run.
				goto end;
			default:
				macro_reportError();
				goto end;
		}
	}
end:
	level--;
	if (level == 0) {
		error_setShowMessages(TRUE);
	}
	interpreter_popExecutionContext(pContext, pOld);
	return (int)1;
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
	long   		bWasRecording = recorder_isRecording();

	if (macroindex == MAC_LASTREC && 
	    (macroindex = _lastinsertedmac) < 0)
		return 0;

	if ((wasplaying = _playing) == 0 && ft_getCurrentDocument()) {
		undo_startModification(ft_getCurrentDocument());
	} 

	if (bWasRecording && macroindex >= 0) {
		recorder_pushSequence(C_MACRO,MAC_NAME(_macroTable[macroindex]));
		recorder_setRecording(FALSE);
	}

	_playing = 1;
	count = _multiplier;
	_multiplier = 1;
	for (i = 0; i < count; i++) {
		switch(macroindex) {
			case MAC_AUTO:	
				recorder_stopAutoInsertRecording(&cp, &cpmax);
				_playing = 2;
				break;
			default:
				if ((mp = _macroTable[macroindex]) == 0)
					return 0;
				cp = (COM_1FUNC *)MAC_DATA(mp);
				cpmax = (COM_1FUNC *)((char *)cp+mp->mc_size);
				break;
		}
		ret = macro_interpretByteCodes(cp, cpmax);
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
		return macro_interpretByteCodes(cp, cp + 1);
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


