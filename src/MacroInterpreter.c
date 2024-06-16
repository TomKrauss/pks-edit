/*
 * MacroInterpreter.c
 *
 * PKS Edit interpreter engine
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

#include "alloc.h"
#include <string.h>
#include <windows.h>
#include <setjmp.h>

#include "winfo.h"
#include "edierror.h"
#include "publicapi.h"
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

typedef struct tagDEBUG_CONFIGURATION {
	BOOL dc_stepping;
} DEBUG_CONFIGURATION;

static DEBUG_CONFIGURATION _debugConfiguration;

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
extern void recorder_recordFunctionWithParameters(int fnum, intptr_t p, intptr_t p2, char* s1, char* s2);

/*--------------------------------------------------------------------------
 * interpreter_testExpression()
 * Test an expression in a macro.
 */
extern int interpreter_testExpression(EXECUTION_CONTEXT* pContext, COM_BINOP* sp);
extern void interpreter_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp);

extern int 	progress_cancelMonitor(BOOL bRedraw);
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
static const PKS_VALUE NIL = { .pkv_type = VT_NIL };

/*
 * An error has occurred during execution of a macro. Display a descriptive error and abort the execution.
 */
void interpreter_raiseError(const char* pFormat, ...) {
	char szBuffer[1024];
	char szMessage[1024];
	va_list ap;

	va_start(ap, pFormat);
	vsprintf(szBuffer, pFormat, ap);
	va_end(ap);
	if (_currentExecutionContext == 0) {
		error_showMessage(szBuffer);
		return;
	}
	sprintf(szMessage, "%.256s during execution of %.256s", szBuffer, _currentExecutionContext->ec_currentFunction);
	DEBUG_ACTION action = debugger_open(_currentExecutionContext, szMessage);
	if (action == DA_ABORT) {
		longjmp(_currentJumpBuffer, 1);
		// not reached
	}
	if (action == DA_STEP) {
		_debugConfiguration.dc_stepping = TRUE;
	}
}

static WINFO* _contextWindow;

/*
 * Returns the window on which all functions should be executed by default.
 * This is either a window passed on from an e.g. context menu action or the currently
 * active window.
 */
static WINFO* interpreter_getCurrentEditorWindow() {
	return _contextWindow ? _contextWindow : ww_getCurrentEditorWindow();
}

/*--------------------------------------------------------------------------
 * interpreter_getValueForOpCode()
 * pop data from execution stack
 */
static PKS_VALUE interpreter_getValueForOpCode(EXECUTION_CONTEXT* pContext, unsigned char* pInstructionPointer) {
	unsigned char typ = *pInstructionPointer;

	switch (typ) {
	case C_FORM_START:
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = (long long)pInstructionPointer};
	case C_PUSH_BOOLEAN_LITERAL:
		return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val};
	case C_PUSH_SMALL_INT_LITERAL:
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val };
	case C_PUSH_LOCAL_VARIABLE:
		return pContext->ec_localVariables[((COM_CHAR1*)pInstructionPointer)->val];
	case C_PUSH_CHARACTER_LITERAL:
		return (PKS_VALUE) { .pkv_type = VT_CHAR, .pkv_data.uchar = ((COM_CHAR1*)pInstructionPointer)->val };
	case C_PUSH_INTEGER_LITERAL:
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = ((COM_INT1*)pInstructionPointer)->val };
	case C_PUSH_LONG_LITERAL:
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = ((COM_LONG1*)pInstructionPointer)->val };
	case C_PUSH_FLOAT_LITERAL: {
		double dDouble = ((COM_FLOAT1*)pInstructionPointer)->val;
		return (PKS_VALUE) { .pkv_type = VT_FLOAT, .pkv_data.doubleValue = dDouble };
	}
	case C_PUSH_NEW_INSTANCE: {
		PKS_VALUE_TYPE t = ((COM_INT1*)pInstructionPointer)->c_valueType;
		return memory_createObject(pContext, t, types_getObjectSize(t), 0);
	}
	case C_PUSH_STRING_LITERAL:
		return memory_createObject(pContext, VT_STRING, 0, ((COM_STRING1*)pInstructionPointer)->s);
	case C_PUSH_VARIABLE:
		return sym_getVariable(sym_getGlobalContext(), ((COM_STRING1*)pInstructionPointer)->s);
	default:
		interpreter_raiseError("Illegal push operation %d", typ);
		// not reached
		return (PKS_VALUE) { 0 };
	}
}

static void interpreter_deallocateStack(EXECUTION_CONTEXT* pContext) {
	free(pContext->ec_stackBottom);
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
}

/*
 * Creates an execution context for executing a macro function.
 */
static EXECUTION_CONTEXT* interpreter_pushExecutionContext(MACRO* mpMacro) {
	EXECUTION_CONTEXT* pResult = calloc(1, sizeof * pResult);
	if (_contextStack == NULL) {
		_contextStack = arraylist_create(7);
	}
	arraylist_add(_contextStack, pResult);
	if (_currentExecutionContext != NULL) {
		pResult->ec_stackBottom = _currentExecutionContext->ec_stackBottom;
		pResult->ec_stackMax = _currentExecutionContext->ec_stackMax;
		pResult->ec_stackCurrent = _currentExecutionContext->ec_stackCurrent;
		pResult->ec_stackFrame = _currentExecutionContext->ec_stackFrame;
	} else {
		interpreter_allocateStack(pResult);
	}
	int nLocalVars = mpMacro->mc_numberOfLocalVars;
	if (nLocalVars > 0) {
		pResult->ec_localVariables = calloc(nLocalVars, sizeof * pResult->ec_localVariables);
		pResult->ec_localVariableCount = mpMacro->mc_numberOfLocalVars;
	}
	pResult->ec_currentFunction = mpMacro->mc_name;
	pResult->ec_parent = _currentExecutionContext;
	_currentExecutionContext = pResult;
	return pResult;
}

/*
 * Destroy a previousy allocated execution context.
 */
static void interpreter_popExecutionContext(EXECUTION_CONTEXT* pContext, EXECUTION_CONTEXT* pPreviousContext) {
	if (pPreviousContext == NULL) {
		interpreter_deallocateStack(pContext);
	}
	free(pContext->ec_localVariables);
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
 * Implements the registerType native method. Parameters are: typename, typedoc, isenum, numberOfProperties, [propertyDescriptor[]]
 * where each property is defined using its name, documentation and value/type (depending on whether the property represents an enum
 * value or a struct field).
 */
PKS_VALUE interpreter_registerType(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 4) {
		interpreter_raiseError("Illegal type registration - not enough parameters");
		return NIL;
	}
	int nProps = pValues[3].pkv_data.intValue;
	int nIdx = 4;
	if (nIdx + nProps * 3 != nArgs) {
		interpreter_raiseError("Illegal type registration - wrong number of params");
		return NIL;
	}
	PKS_TYPE_DESCRIPTOR descriptor = {
		.ptd_name = memory_accessString(pValues[0]),
		.ptd_documentation = memory_accessString(pValues[1]),
		.ptd_isEnumType = pValues[2].pkv_data.booleanValue,
		.ptd_numberOfProperties = nProps
	};
	if (nProps) {
		descriptor.ptd_elements.ptd_enumValues = calloc(nProps, sizeof(*descriptor.ptd_elements.ptd_enumValues));
		for (int i = 0; i < nProps; i++) {
			PARAMETER_ENUM_VALUE* pDescriptor = &descriptor.ptd_elements.ptd_enumValues[i];
			pDescriptor->pev_name = (char*)memory_accessString(pValues[nIdx++]);
			pDescriptor->pev_documentation = (char*)memory_accessString(pValues[nIdx++]);
			pDescriptor->pev_val = (long)pValues[nIdx].pkv_data.longValue;
			nIdx++;
		}
	}
	types_register(-1, &descriptor);
	free(descriptor.ptd_elements.ptd_enumValues);
	return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 1 };
}
/*
 * Implements the size() method used to determine the length of a string or the size of an array or object.
 */
PKS_VALUE interpreter_size(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 1) {
		return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 0 };
	}
	PKS_VALUE v = pValues[0];
	if (v.pkv_type == VT_RANGE) {
		int nCount = (v.pkv_data.range.r_end - v.pkv_data.range.r_start + 1);
		int nDelta = v.pkv_data.range.r_increment;
		if (nDelta > 1) {
			nCount /= nDelta;
		}
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = nCount };
	}
	size_t nLen = memory_size(v);
	return (PKS_VALUE) { .pkv_type = VT_NUMBER, .pkv_data.longValue = nLen };
}

/*
 * Implements the CreateArray() method used when arrays are created: [...].
 */
PKS_VALUE interpreter_createArray(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	PKS_VALUE vArray = memory_createObject(pContext, VT_OBJECT_ARRAY, nArgs, 0);
	for (int i = 0; i < nArgs; i++) {
		memory_setNestedObject(vArray, i, pValues[i]);
	}
	return vArray;
}

/*
 * Implements the CreateMap() method used when maps are created: ´{"x": x}.
 */
PKS_VALUE interpreter_createMap(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	PKS_VALUE vArray = memory_createObject(pContext, VT_MAP, nArgs, 0);
	for (int i = 0; i < nArgs; i += 2) {
		memory_atPutObject(vArray, pValues[i], pValues[i+1]);
	}
	return vArray;
}

/*
 * Implements the foreach() method used in for(x : y) loops.
 */
PKS_VALUE interpreter_foreach(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 3) {
		return (PKS_VALUE) { .pkv_type=VT_BOOLEAN, .pkv_data.booleanValue = 0 };
	}
	int nCaretVarHeapOffset = pValues[0].pkv_data.intValue;
	int nResultVarOffset = pValues[1].pkv_data.intValue;
	PKS_VALUE v = pValues[2];
	int idxLast;
	if (v.pkv_type == VT_RANGE) {
		idxLast = v.pkv_data.range.r_end + 1;
	}
	else if (v.pkv_type == VT_STRING || v.pkv_type == VT_OBJECT_ARRAY) {
		idxLast = (int)memory_size(v);
	} else {
		return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 0 };
	}
	PKS_VALUE caretV = pContext->ec_localVariables[nCaretVarHeapOffset];
	if (caretV.pkv_type != VT_NUMBER) {
		caretV = (PKS_VALUE){.pkv_type = VT_NUMBER, .pkv_data.intValue = 0};
	}
	int idx = caretV.pkv_data.intValue;
	if (v.pkv_type == VT_RANGE) {
		int nMult = v.pkv_data.range.r_increment > 0 ? v.pkv_data.range.r_increment : 1;
		idx = v.pkv_data.range.r_start + (nMult*idx);
	}
	if (idx >= idxLast) {
		pContext->ec_localVariables[nCaretVarHeapOffset] = (PKS_VALUE){ 0 };
		return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 0 };
	}
	PKS_VALUE vResult;
	if (v.pkv_type == VT_RANGE) {
		vResult = (PKS_VALUE){.pkv_type = VT_NUMBER, .pkv_data.intValue = idx };
	} else if (v.pkv_type == VT_OBJECT_ARRAY) {
		vResult = memory_getNestedObject(v, idx);
	} else {		// string
		vResult = (PKS_VALUE){.pkv_type = VT_CHAR, .pkv_data.uchar = memory_accessString(v)[idx] };
	}
	pContext->ec_localVariables[nResultVarOffset] = vResult;
	caretV.pkv_data.intValue++;
	pContext->ec_localVariables[nCaretVarHeapOffset] = caretV;

	return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 1 };
}

/*
 * Implements the typeof() method.
 */
PKS_VALUE interpreter_typeOf(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (nArgs < 1) {
		return (PKS_VALUE) {0};
	}
	const char* pszName = types_nameFor(pValues->pkv_type);
	return interpreter_allocateString(pContext, pszName);
}

/*--------------------------------------------------------------------------
 * macroc_sprintf()
 * Similar to C sprintf, this will take a format and two parameters and print the result
 * and return the formatted result.
 */
PKS_VALUE interpreter_sprintf(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	char buf[1024];

	if (nArgs < 1 || pValues->pkv_type != VT_STRING) {
		return (PKS_VALUE) { .pkv_type = VT_NUMBER, 0 };
	}
	const char* pszFormat = memory_accessString(pValues[0]);
	union U_ARG_VALUE* values = calloc(nArgs, sizeof (* values));
	for (int i = 1; i < nArgs; i++) {
		if (pValues[i].pkv_type == VT_FLOAT) {
			values[i - 1].v_d = pValues[i].pkv_data.doubleValue;
		} else if (pValues[i].pkv_type == VT_STRING) {
			values[i - 1].v_s = (char*)memory_accessString(pValues[i]);
		} else {
			values[i - 1].v_l = (long)pValues[i].pkv_data.longValue;
		}
	}
	mysprintf(buf, (char*)pszFormat, &(SPRINTF_ARGS){.sa_wp = interpreter_getCurrentEditorWindow(), .sa_values = values});
	free(values);
	return interpreter_allocateString(pContext, buf);
}


/*
 * Pop one value of the stack of our stack machine
 */
PKS_VALUE interpreter_popStackValue(EXECUTION_CONTEXT* pContext) {
	if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
		return *--pContext->ec_stackCurrent;
	}
	return (PKS_VALUE) {.pkv_data = 0, .pkv_type = VT_NUMBER};
}

/*
 * Te the current stack top value.
 */
PKS_VALUE interpreter_peekStackValue(EXECUTION_CONTEXT* pContext) {
	if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
		return pContext->ec_stackCurrent[-1];
	}
	return (PKS_VALUE) { .pkv_data = 0, .pkv_type = VT_NUMBER };
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
 * Mini memory management function of PKSMacroC, which temporarily allocates a string to
 * be released later.
 */
PKS_VALUE interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource) {
	return memory_createObject(pContext, VT_STRING, 0, pszSource);
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
		case C_PUSH_LOCAL_VARIABLE:
		case C_ASSIGN_LOCAL_VAR:
		case C_PUSH_BOOLEAN_LITERAL:
			return sizeof(COM_CHAR1);
		case C_1FUNC:
			return sizeof(COM_1FUNC);
		case C_FORM_START:
			return sizeof(COM_FORM);
		case C_GOTO:
			return sizeof(COM_GOTO);
		case C_ASSIGN_SLOT:
		case C_STOP:
		case C_SPREAD:
			return sizeof(COM_STOP);
		case C_PUSH_FLOAT_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_FLOAT1);
		case C_PUSH_NEW_INSTANCE:
		case C_PUSH_INTEGER_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_INT1);
		case C_DEFINE_PARAMETER:
		case C_DEFINE_VARIABLE:
		case C_DEFINE_LOCAL_VARIABLE:
			return (((COM_DEFINE_SYMBOL*)(s-1))->size);
		case C_PUSH_LONG_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_MACRO_REF_LOCAL:
			return (int)sizeof(COM_MAC);
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
		par = v.pkv_data;
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_PUSH_INTEGER_LITERAL:  *dp->p.i = par.intValue; break;
			case C_PUSH_SMALL_INT_LITERAL:
			case C_PUSH_CHARACTER_LITERAL: *dp->p.c = par.uchar; break;
			case C_PUSH_FLOAT_LITERAL: *dp->p.d = par.doubleValue; break;
			case C_PUSH_LONG_LITERAL: *dp->p.l = (long)par.longValue; break;
			case C_PUSH_STRING_LITERAL:
				if ((v.pkv_type == VT_STRING) && par.string) {
					strcpy(dp->p.s, memory_accessString(v));
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
 * Hack: can be used to set a "context" window for interpreter operations. 
 */
void interpreter_setContextWindow(WINFO* wp) {
	_contextWindow = wp;
}

/*
 * Returns FALSE; if the function described by the function pointer cannot
 * be executed.
 */
static int interpreter_isFunctionEnabled(int nf_flags, int (*nf_checkEnabled)(intptr_t* p), intptr_t* stack, int warn) {
	if (!nf_flags) {
		return 1;
	}
	WINFO* wp = interpreter_getCurrentEditorWindow();

	FTABLE* fp = wp ? wp->fp : NULL;

	if (nf_flags & EW_NEEDSCURRF && fp == 0) {
		return 0;
	}
	if ((nf_flags & EW_MODIFY) && fp != NULL && (ft_isReadonly(fp) || !wp->renderer->r_canEdit)) {
		if (warn) {
			ft_checkReadonlyWithError(fp);
		}
		return 0;
	}

	if (nf_flags & EW_NEEDSBLK && (!wp || !ww_hasSelection(wp))) {
		return 0;
	}
	if ((nf_flags & EW_REDO_AVAILABLE) && !undo_isRedoAvailable(fp)) {
		return 0;
	}
	if ((nf_flags & EW_UNDO_AVAILABLE) && !undo_isUndoAvailable(fp)) {
		if (warn) {
			ww_checkSelectionWithError(wp);
		}
		return 0;
	}
	if (nf_flags & EW_COMPARISON_MODE && (wp == NULL || wp->linkedWindow == NULL)) {
		return 0;
	}
	if (nf_flags & EW_CUSTOM_ENABLEMENT && !nf_checkEnabled(stack)) {
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * interpreter_canExecuteFunction()
 */
int interpreter_canExecuteNativeFunction(int num, long long pParam, int warn) {
	NATIVE_FUNCTION *	fup = &_functionTable[num];
	intptr_t stack[4];
	PARAMETER_TYPE_DESCRIPTOR pd1;
	stack[0] = pParam;
	int nCount = function_getParameterCount(fup);
	if (nCount > 1) {
		pd1 = function_getParameterTypeDescriptor(fup, 1);
		if (pd1.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
			stack[0] = (intptr_t)ww_getCurrentEditorWindow();
			stack[1] = pParam;
		}
	}
	return interpreter_isFunctionEnabled(fup->nf_flags, fup->nf_checkEnabled, stack, warn);
}

/*
 * Checks, whether the execution of a macro is currently enabled.
 */
int interpreter_canExecuteMacro(int nMacroNum, int warn) {
	MACRO* mp = macro_getByIndex(nMacroNum);
	intptr_t dummy = 0;
	return mp && interpreter_isFunctionEnabled(mp->mc_actionFlags, 0, &dummy, warn);
}

/*
 * Call a method generically (foreign function interface) - should use libffi on a long term run.
 */
static long long cdecl interpreter_callFfi(NATIVE_FUNCTION* pFunc, intptr_t* pStack) {
	if (!pFunc->nf_execute) {
		return 0;
	}

	// Handle typical function prototypes explicitly
	int nParams = function_getParameterCount(pFunc);
	PARAMETER_TYPE_DESCRIPTOR pd1;
	if (nParams >= 1) {
		pd1 = function_getParameterTypeDescriptor(pFunc, 1);
	}
	if (nParams == 1) {
		if (pd1.pt_type == PARAM_TYPE_STRING) {
			return (*pFunc->nf_execute)(((void**)pStack)[0]);
		}
	} else if (nParams == 2) {
		PARAMETER_TYPE_DESCRIPTOR pd2 = function_getParameterTypeDescriptor(pFunc, 2);
		if (pd1.pt_type == PARAM_TYPE_STRING) {
			if (pd2.pt_type == PARAM_TYPE_STRING) {
				return (*pFunc->nf_execute)((void*)pStack[0], (void*)pStack[1]);
			}
			return (*pFunc->nf_execute)((void*)pStack[0], (long)pStack[1]);
		}
		if (pd2.pt_type == PARAM_TYPE_STRING) {
			if (pd1.pt_type != PARAM_TYPE_STRING) {
				return (*pFunc->nf_execute)((long)pStack[0], (void*)pStack[1]);
			}
		}
	}
	return (*pFunc->nf_execute)((long)pStack[0], (long)pStack[1], (void*)pStack[2], (void*)pStack[3], (void*)pStack[4], (void*)pStack[5]);
}

/*---------------------------------*/
/* interpreter_executeFunction()					*/
/*---------------------------------*/
long long cdecl interpreter_executeFunction(int num, intptr_t *stack) {
	NATIVE_FUNCTION *	fup = &_functionTable[num];
	long long 	ret = 0;
	int			i;

	if (!interpreter_isFunctionEnabled(fup->nf_flags, fup->nf_checkEnabled, stack, 1)) {
		return 0;
	}

	if (fup->nf_flags & EW_UNDOFLSH) {
		undo_startModification(ft_getCurrentDocument());
	}

	if ((fup->nf_flags & EW_CCASH) == 0) {
		recorder_stopAutoInsertRecording(NULL, NULL);
		caret_removeSecondaryCarets();
	}

	recorder_recordFunctionWithParameters(num,stack[0], stack[1], (char*)stack[2], (char*)stack[3]);

	if ((i = _multiplier) > 1 && (fup->nf_flags & EW_MULTI) == 0) {
		while (i-- > 0 && (ret = interpreter_callFfi(fup, stack)) != 0)
			;
		_multiplier = 1;
	} else {
		ret = interpreter_callFfi(fup, stack);
	}

	FTABLE* fp;
	if (ret > 0 && (fup->nf_flags & EW_MODIFY) && (fp = ft_getCurrentDocument()) != NULL) {
		ft_settime(&fp->ti_lastChanged);
	}

	if (i > 1) {
		_multiplier = 1;
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * interpreter_returnNativeFunctionResult()
 * Return a value created by a native function
 */
static void interpreter_returnNativeFunctionResult(EXECUTION_CONTEXT* pContext, PARAMETER_TYPE typ, intptr_t v) {
	PKS_VALUE_TYPE vt = VT_NUMBER;

	if (typ == PARAM_TYPE_STRING) {
		// (A) strings are not(!) allocated by caller. Caller returns a static buffer or so.
		interpreter_pushValueOntoStack(pContext, interpreter_allocateString(pContext, (const void*)v));
		return;
	} else if (typ == PARAM_TYPE_STRING_ARRAY) {
		// (A) string arrays will always be allocated by caller - array list including nested elements
		interpreter_pushValueOntoStack(pContext, memory_createObject(pContext, VT_OBJECT_ARRAY, 0, (const void*)v));
		arraylist_destroyStringList((ARRAY_LIST*)v);
		return;
	}
	else if (typ == PARAM_TYPE_EDITOR_WINDOW) {
		interpreter_pushValueOntoStack(pContext, memory_createHandleObject(pContext, VT_EDITOR_HANDLE, (void*)v));
		return;
	}
	interpreter_pushValueOntoStack(pContext, (PKS_VALUE) { .pkv_type = vt, .pkv_data.longValue = v });
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
	return (int)macro_executeMacro(0, &macref);
}

/*---------------------------------*/
/* interpreter_doMacroFunctions()				*/
/*---------------------------------*/
static intptr_t interpreter_doMacroFunctions(EXECUTION_CONTEXT* pContext, COM_1FUNC *pInstructionPointer, int nArgumentDelta) {
	intptr_t	nativeStack[8];
	PKS_VALUE	tempStack[8];
	intptr_t	rc;
	intptr_t *	functionParameters;
	unsigned char 	typ;
	int  		funcnum;
	BOOL		bNativeCall;
	BOOL		bInternalNativeCall = FALSE;
	PKS_VALUE   returnValue;

	typ = pInstructionPointer->typ;
	functionParameters = nativeStack;
	const char* pszMacro = ((COM_MAC*)pInstructionPointer)->name;
	funcnum = pInstructionPointer->funcnum;
	NATIVE_FUNCTION* fup = &_functionTable[funcnum];
	bNativeCall = typ != C_MACRO && typ != C_MACRO_REF && typ != C_MACRO_REF_LOCAL;
	if (typ == C_MACRO_REF || typ == C_MACRO_REF_LOCAL) {
		PKS_VALUE v;
		if (typ == C_MACRO_REF_LOCAL) {
			int idx = ((COM_MAC*)pInstructionPointer)->heapIndex;
			v = pContext->ec_localVariables[idx];
			if (v.pkv_type != VT_STRING) {
				interpreter_raiseError("Illegal reference to macro through instancevar %d", idx);
			}
		} else {
			v = sym_getVariable(sym_getGlobalContext(), (char*)pszMacro);
			if (v.pkv_type != VT_STRING) {
				interpreter_raiseError("Illegal reference to macro function named %s", pszMacro);
			}
		}
		pszMacro = memory_accessString(v);
		char* ret;
		SYMBOL sym = sym_find(sym_getKeywordContext(), pszMacro, &ret);
		if (sym.s_type == S_EDFUNC) {
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
	int nParametersPassed = ((COM_0FUNC*)pInstructionPointer)->func_nargs+nArgumentDelta;
	if (bNativeCall) {
		memset(nativeStack, 0, sizeof nativeStack);
		if (typ == C_1FUNC) {
			PARAMETER_TYPE_DESCRIPTOR pdt = function_getParameterTypeDescriptor(fup, 1);
			if (pdt.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
				*functionParameters++ = (intptr_t) interpreter_getCurrentEditorWindow();
			}
			*functionParameters++ = pInstructionPointer->p;
			nParametersPassed--;
		}
	}

	if (bNativeCall && !_currentFormInstruction) {
		NATIVE_FUNCTION* fup = &_functionTable[funcnum];
		int i;
		if (nParametersPassed > DIM(tempStack)) {
			nParametersPassed = DIM(tempStack);
		}
		for (i = 0; i < nParametersPassed; i++) {
			tempStack[i] = interpreter_popStackValue(pContext);
		}
		int idxParam = 1;
		for (i = 0; i < nParametersPassed; i++) {
			PKS_VALUE v = tempStack[nParametersPassed-i-1];
			PARAMETER_TYPE_DESCRIPTOR pdt = function_getParameterTypeDescriptor(fup, idxParam++);
			if (pdt.pt_type == PARAM_TYPE_STRING) {
				v = interpreter_coerce(pContext, v, VT_STRING);
			}
			if (pdt.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
				WINFO* wp;
				if (v.pkv_type == VT_EDITOR_HANDLE) {
					wp = memory_handleForValue(v);
				} else {
					// assume, that no editor handle param had been passed - use the default editor.
					wp = interpreter_getCurrentEditorWindow();
					i--;
				}
				if (wp == 0) {
					interpreter_raiseError("Null editor handle passed to MacroC function");
				}
				*functionParameters++ = (intptr_t)wp;
			} else if (v.pkv_type == VT_STRING) {
				*functionParameters++ = (intptr_t)memory_accessString(v);
			} else {
				*functionParameters++ = v.pkv_data.intValue;
			}
		}
	}
	if (bNativeCall) {
		NATIVE_FUNCTION* fup = &_functionTable[funcnum];
		rc = interpreter_executeFunction(funcnum, nativeStack);
		PARAMETER_TYPE pReturnType = function_getParameterTypeDescriptor(fup, 0).pt_type;
		interpreter_returnNativeFunctionResult(pContext, pReturnType, rc);
	} else {
		if (bInternalNativeCall) {
			PKS_VALUE* pStack = calloc(nParametersPassed ? nParametersPassed : 1, sizeof * pStack);
			for (int i = 0; i < nParametersPassed; i++) {
				pStack[nParametersPassed -i-1] = interpreter_popStackValue(pContext);
			}
			NATIVE_FUNCTION* fup = &_functionTable[funcnum];
			returnValue = ((PKS_VALUE (*)())fup->nf_execute)(pContext, pStack, nParametersPassed);
			// TODO: memory leak, if an exception occurs during execute
			free(pStack);
		} else {
			PKS_VALUE* pOld = pContext->ec_stackFrame;
			pContext->ec_stackFrame = pContext->ec_stackCurrent - nParametersPassed;
			if (pContext->ec_stackFrame < pContext->ec_stackBottom) {
				pContext->ec_stackFrame = pContext->ec_stackBottom;
			}
			rc = interpreter_executeMacroByName((char*)pszMacro);
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
	return rc;
}

/*--------------------------------------------------------------------------
 * interpreter_assignSymbol()
 */
static long interpreter_assignSymbol(EXECUTION_CONTEXT* pContext, char* name) {
	PKS_VALUE v = interpreter_peekStackValue(pContext);

	return sym_defineVariable(sym_getGlobalContext(), name, v);
}

/*--------------------------------------------------------------------------
 * interpreter_assignLocalVar()
 */
static void interpreter_assignLocalVar(EXECUTION_CONTEXT* pContext, int nOffset) {
	PKS_VALUE v = interpreter_peekStackValue(pContext);

	pContext->ec_localVariables[nOffset] = v;
}

/*--------------------------------------------------------------------------
 * interpreter_assignOffset()
 * x[nOffset] = ....
 * or x["yz"] = ....
 */
static void interpreter_assignOffset(EXECUTION_CONTEXT* pContext) {
	PKS_VALUE v = interpreter_popStackValue(pContext);
	PKS_VALUE vOffset = interpreter_popStackValue(pContext);
	PKS_VALUE target = interpreter_popStackValue(pContext);
	if (target.pkv_type == VT_MAP) {
		memory_atPutObject(target, vOffset, v);
	}
	else {
		if (target.pkv_type != VT_OBJECT_ARRAY && !types_isStructuredType(target.pkv_type)) {
			interpreter_raiseError("Illegal target object of type %s for offset assignment.", types_nameFor(target.pkv_type));
		}
		int nIndex = interpreter_coerce(pContext, vOffset, VT_NUMBER).pkv_data.intValue;
		if (nIndex < 0) {
			interpreter_raiseError("Illegal negative index %d to assign element of type %s.", nIndex, types_nameFor(target.pkv_type));
		}
		memory_setNestedObject(target, nIndex, v);
	}
	interpreter_pushValueOntoStack(pContext, v);
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
	return (PKS_VALUE) {.pkv_type = 0, .pkv_data.longValue = 0};
}

/*
 * Compare two values and return 1 if they are identical.
 */
static int interpreter_testCaseLabelMatch(EXECUTION_CONTEXT* pContext, PKS_VALUE nCaseLabelValue, PKS_VALUE v2) {
	if (nCaseLabelValue.pkv_type == VT_STRING) {
		v2 = interpreter_coerce(pContext, v2, VT_STRING);
		return strcmp(memory_accessString(nCaseLabelValue), memory_accessString(v2)) == 0;
	}
	v2 = interpreter_coerce(pContext, v2, VT_NUMBER);
	if (nCaseLabelValue.pkv_type == VT_RANGE) {
		return nCaseLabelValue.pkv_data.range.r_start <= v2.pkv_data.intValue && nCaseLabelValue.pkv_data.range.r_end >= v2.pkv_data.intValue;
	}
	return nCaseLabelValue.pkv_data.intValue == v2.pkv_data.intValue;
}

/*
 * Implements the spread operator.
 */
static int interpreter_spread(EXECUTION_CONTEXT* pContext) {
	PKS_VALUE v = interpreter_peekStackValue(pContext);
	if (v.pkv_type != VT_OBJECT_ARRAY) {
		interpreter_raiseError("Illegal object type %s for spread operator.", types_nameFor(v.pkv_type));
	}
	interpreter_popStackValue(pContext);
	size_t nSize = memory_size(v);
	int nDelta = nSize ? (int)(nSize - 1) : -1;
	for (int i = 0; i < nSize; i++) {
		PKS_VALUE vElement = memory_getNestedObject(v, i);
		interpreter_pushValueOntoStack(pContext, vElement);
	}
	return nDelta;
}

/*---------------------------------*/
/* macro_interpretByteCodes()      */
/*---------------------------------*/
#define COM1_INCR(pLocalInstructionPointer,type,offset) (((unsigned char *)pLocalInstructionPointer)+((type *)pLocalInstructionPointer)->offset)
#define COM_PARAMINCR(pLocalInstructionPointer)		(((unsigned char *)pLocalInstructionPointer)+interpreter_getParameterSize(pLocalInstructionPointer->typ,&pLocalInstructionPointer->funcnum));
static int macro_interpretByteCodesContext(EXECUTION_CONTEXT* pContext, MACRO* mpMacro) {
	COM_1FUNC* cp = (COM_1FUNC*)mpMacro->mc_bytecodes;
	int nArgumentDelta = 0;
	COM_1FUNC* cpmax = (COM_1FUNC*)(mpMacro->mc_bytecodes + mpMacro->mc_bytecodeLength);
	static int _progressCount;

	unsigned char* pInstr = (unsigned char*)cp;
	unsigned char* pInstrMax = (unsigned char*)cpmax;
	while (pInstr < pInstrMax) {
		cp = (COM_1FUNC*)pInstr;
		pContext->ec_instructionPointer = cp;
		if (_progressCount++ > 1000) {
			if (progress_cancelMonitor(FALSE) != 0) {
				interpreter_raiseError("Macro execution aborted by user.");
			}
			_progressCount = 0;
		}
		if (_debugConfiguration.dc_stepping) {
			_debugConfiguration.dc_stepping = FALSE;
			interpreter_raiseError("Breakpoint reached.");
		}
		switch (cp->typ) {
		case C_GOTO: {
			BRANCH_TYPE bt = ((COM_GOTO*)cp)->branchType;
			PKS_VALUE stackTop;
			int val;
			switch (bt) {
			case BRA_ALWAYS:
					pInstr = COM1_INCR(cp, COM_GOTO, offset);
					continue;
				case BRA_IF_FALSE:
					stackTop = interpreter_popStackValue(pContext);
					stackTop = interpreter_coerce(pContext, stackTop, VT_BOOLEAN);
					val = !stackTop.pkv_data.booleanValue;
					break;
				case BRA_TOS_IF_FALSE:
					stackTop = interpreter_peekStackValue(pContext);
					stackTop = interpreter_coerce(pContext, stackTop, VT_BOOLEAN);
					val = !stackTop.pkv_data.booleanValue;
					break;
				case BRA_TOS_IF_TRUE:
					stackTop = interpreter_peekStackValue(pContext);
					stackTop = interpreter_coerce(pContext, stackTop, VT_BOOLEAN);
					val = stackTop.pkv_data.booleanValue;
					break;
				case BRA_CASE: {
					PKS_VALUE caseLabelValue = interpreter_popStackValue(pContext);
					PKS_VALUE compareWith = interpreter_peekStackValue(pContext);
					val = interpreter_testCaseLabelMatch(pContext, caseLabelValue, compareWith);
				}
				break;
				default:
					val = 0;
					break;
			}
			if (val == TRUE) {
				pInstr = COM1_INCR(cp, COM_GOTO, offset);
			} else {
				pInstr = COM_PARAMINCR(cp);
			}
			continue;
		}
		case C_POP_STACK:
			if (pContext->ec_stackCurrent > pContext->ec_stackFrame) {
				pContext->ec_stackCurrent--;
			}
			memory_garbaggeCollect(pContext);
			break;
		case C_SPREAD:
			nArgumentDelta += interpreter_spread(pContext);
			break;
		case C_SET_STACKFRAME:
			pContext->ec_stackFrame = pContext->ec_stackCurrent;
			break;
		case C_LOGICAL_OPERATION:
			interpreter_testExpression(pContext, (COM_BINOP*)cp);
			break;
		case C_BINOP:
			interpreter_evaluateBinaryExpression(pContext, (COM_BINOP*)cp);
			break;
		case C_FORM_START:
			_currentFormInstruction = (COM_FORM*)pInstr;
			break;
		case C_PUSH_BOOLEAN_LITERAL:
		case C_PUSH_LONG_LITERAL:
		case C_PUSH_LOCAL_VARIABLE:
		case C_PUSH_SMALL_INT_LITERAL:
		case C_PUSH_CHARACTER_LITERAL:
		case C_PUSH_FLOAT_LITERAL:
		case C_PUSH_INTEGER_LITERAL:
		case C_PUSH_NEW_INSTANCE:
		case C_PUSH_VARIABLE:
		case C_PUSH_STRING_LITERAL: {
			PKS_VALUE value = interpreter_getValueForOpCode(pContext, pInstr);
			interpreter_pushValueOntoStack(pContext, value);
			break;
		}
		case C_DEFINE_PARAMETER: {
			PKS_VALUE value = interpreter_getParameterStackValue(pContext, (int)((COM_DEFINE_SYMBOL*)cp)->value);
			// automatic type coercion of parameter types.
			if (!value.pkv_type && ((COM_DEFINE_SYMBOL*)cp)->vartype == VT_STRING) {
				// TODO: this is a hack - if the parameter was not passed we need to define the symbol and assign it a NIL value,
				// which is currently not supported. So convert to false to allow us for testing, whether the parameter was passed.
				value = (PKS_VALUE){.pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 0};
			}
			else if (((COM_DEFINE_SYMBOL*)cp)->vartype != VT_AUTO) {
				value = interpreter_coerce(pContext, value, ((COM_DEFINE_SYMBOL*)cp)->vartype);
			}
			int idx = ((COM_DEFINE_SYMBOL*)cp)->heapIndex;
			pContext->ec_localVariables[idx] = value;
			break;
		}
		case C_DEFINE_LOCAL_VARIABLE:
		case C_DEFINE_VARIABLE: {
			PKS_VALUE_TYPE t = ((COM_DEFINE_SYMBOL*)cp)->vartype;
			if (types_hasDefaultValue(t)) {
				PKS_VALUE v = memory_createObject(pContext, t, ((COM_DEFINE_SYMBOL*)cp)->value, 0);
				if (cp->typ == C_DEFINE_LOCAL_VARIABLE) {
					pContext->ec_localVariables[((COM_DEFINE_SYMBOL*)cp)->heapIndex] = v;
				} else {
					sym_defineVariable(sym_getGlobalContext(), ((COM_DEFINE_SYMBOL*)cp)->name, v);
				}
			}
			break;
		}
		case C_ASSIGN_SLOT:
			interpreter_assignOffset(pContext);
			break;
		case C_ASSIGN:
			interpreter_assignSymbol(pContext, ((COM_ASSIGN*)cp)->name);
			break;
		case C_ASSIGN_LOCAL_VAR:
			interpreter_assignLocalVar(pContext, ((COM_CHAR1*)cp)->val);
			break;
		case C_STOP:
			goto end;
		case C_MACRO_REF: case C_MACRO_REF_LOCAL: case C_MACRO: case C_0FUNC: case C_1FUNC:
			interpreter_doMacroFunctions(pContext, cp, nArgumentDelta);
			nArgumentDelta = 0;
			break;
		default:
			interpreter_raiseError("Corrupted bytecodes - cannot continue.");
		}
		pInstr += interpreter_getParameterSize(cp->typ, pInstr + 1);
	}
end:
	return (int)1;
}

/*---------------------------------*/
/* macro_interpretByteCodes()      */
/*---------------------------------*/
static int macro_interpretByteCodes(MACRO* mp) {
	static int 	level;
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT* pContext = interpreter_pushExecutionContext(mp);

	if (level == 0 && setjmp(_currentJumpBuffer)) {
		// TODO: more cleanup!
		interpreter_cleanupContextStacks();
		level = 0;
		_playing = 0;
		_currentExecutionContext = 0;
		progress_closeMonitor(0);
		return -1;
	}
	level++;
	macro_interpretByteCodesContext(pContext, mp);
	level--;
	if (level == 0) {
		error_setShowMessages(TRUE);
	}
	interpreter_popExecutionContext(pContext, pOld);
	return (int)1;
}

/*
 * Initialize a namespace. If bSetJump == TRUE, prepare for exception handling.
 */
int interpreter_initializeNamespace(MACRO* mpNamespace, BOOL bSetJump) {
	char szBuf[128];
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT ecNamespace;
	memset(&ecNamespace, 0, sizeof ecNamespace);
	sprintf(szBuf, "namespace %.100s", mpNamespace->mc_name);
	ecNamespace.ec_currentFunction = szBuf;
	_currentExecutionContext = &ecNamespace;
	interpreter_allocateStack(_currentExecutionContext);
	if (bSetJump && setjmp(_currentJumpBuffer)) {
		interpreter_deallocateStack(_currentExecutionContext);
		_currentExecutionContext = 0;
		return -1;
	}
	macro_interpretByteCodesContext(_currentExecutionContext, mpNamespace);
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
long long macro_executeMacroByIndex(int macroindex) {
	int 		ret = 1;
	int			i;
	int			count;
	int			wasplaying;
	MACRO		macAuto;
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
		switch(macroindex) {
		case MAC_AUTO: {
			COM_1FUNC* cp;
			COM_1FUNC* cpmax;
			recorder_stopAutoInsertRecording(&cp, &cpmax);
			_playing = 2;
			memset(&macAuto, 0, sizeof macAuto);
			macAuto.mc_bytecodes = (unsigned char*)cp;
			macAuto.mc_bytecodeLength = (int)((unsigned char*)cpmax - (unsigned char*)cp);
			macAuto.mc_name = "auto";
			mp = &macAuto;
			break;

			}
			default:
				if ((mp = macro_getByIndex(macroindex)) == 0)
					return 0;
				mpNamespace = macro_getNamespaceByIdx(mp->mc_namespaceIdx);
				if (mpNamespace && !mpNamespace->mc_isInitialized) {
					if (!interpreter_initializeNamespace(mpNamespace, FALSE)) {
						return 0;
					}
				}
				break;
		}
		ret = macro_interpretByteCodes(mp);
		if (ret <= 0) {
			ret = 0;
			break;
		}
	}

	recorder_setRecording(bWasRecording);
	_playing = wasplaying;
	return ret;
}

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
long long macro_executeMacro(WINFO* wp, MACROREF* mp) {
	COM_1FUNC* cp;
	long long ret;
	MACRO macro;
	_contextWindow = wp;
	switch (mp->typ) {
	case CMD_CMDSEQ:
		cp = &_commandTable[mp->index].c_functionDef;
		memset(&macro, 0, sizeof macro);
		macro.mc_name = _commandTable[mp->index].c_name;
		macro.mc_bytecodes = (unsigned char*)cp;
		macro.mc_bytecodeLength = interpreter_getParameterSize(cp->typ, ((const char*)cp)+1);
		ret = macro_interpretByteCodes(&macro);
		break;
	case CMD_MACRO:
		ret = macro_executeMacroByIndex(mp->index);
		break;
	default:
		ret = 0;
		error_displayAlertDialog("Illegal command type to execute.");
	}
	_contextWindow = 0;
	return ret;
}


