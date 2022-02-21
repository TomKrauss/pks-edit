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
#include "scanner.h"
#include "funcdef.h"
#include "mouseutil.h"
#include "caretmovement.h"

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
 * macro_testExpression()
 * Test an expression in a macro.
 */
extern int macro_testExpression(EXECUTION_CONTEXT* pContext, COM_TEST* sp);
extern void 			macro_evaluateBinaryExpression(EXECUTION_CONTEXT* pContext, COM_BINOP *sp);

extern int 			progress_cancelMonitor(BOOL bRedraw);
extern void 			ww_redrawAllWindows(int update);
extern void 			undo_startModification(FTABLE *fp);
extern void 			ft_settime(EDTIME *tp);
extern int 			macro_executeByName(char *name);

int					_playing;

extern long			_multiplier;
extern int			_lastinsertedmac;

extern BYTECODE_BUFFER _currentRecordingBuffer;
static unsigned char* _dialogExecutionBytecodePointer;

static EXECUTION_CONTEXT* _currentExecutionContext;

/*
 * Interpreter error handling.
 */
static void interpreter_raiseError(const char* pMessage) {
	error_displayAlertDialog(pMessage);
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
	} else {
		PKS_VALUE* pStack = calloc(MAX_STACK_SIZE, sizeof(*pStack));
		pResult->ec_stackBottom = pStack;
		pResult->ec_stackCurrent = pStack;
		pResult->ec_stackFrame = pStack;
		pResult->ec_stackMax = pStack + MAX_STACK_SIZE;
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
	}
	free(pContext);
	_currentExecutionContext = pPreviousContext;
}

/*
 * interpreter_getParameterSize()
 * typ: the bytecode
 * s: pointer to the bytecode buffer past(!) the opcode (instructionPointer+1)
 */
int interpreter_getParameterSize(unsigned char typ, const char *s)
{	int size;

	switch(typ) {
		case C_FURET:
		case C_0FUNC:
		case C_CHARACTER_LITERAL:
		case C_BOOLEAN_LITERAL:
			return sizeof(COM_CHAR1);
		case C_1FUNC:
			return sizeof(COM_1FUNC);
		case C_FORMSTART:
			return sizeof(COM_FORM);
		case C_GOTO:
			return sizeof(COM_GOTO);
		case C_STOP:
			return sizeof(COM_STOP);
		case C_FLOAT_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_FLOAT1);
		case C_INTEGER_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_INT1);
		case C_DEFINE_PARAMETER:
		case C_DEFINE_VARIABLE:
			return (((COM_CREATESYM*)(s-1))->size);
		case C_LONG_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_STRINGVAR:
		case C_FLOATVAR:
		case C_LONGVAR:
		case C_BOOLEANVAR:
		case C_MACRO:
		case C_STRING_LITERAL:
			size = (int)((sizeof(struct c_ident) - 1 /* pad byte*/) +
					(strlen(s)+1)*sizeof(*s));
			if (size & 1) size++;
			return size;
		case C_BINOP:
			return ((COM_BINOP*)(s-1))->size;
		case C_ASSIGN:
			return ((COM_ASSIGN*)(s-1))->size;
		case C_TEST:
			return sizeof(COM_TEST);
	}
	/*
	 * oops: this is an error
	 */
	macro_reportError();
	return 10000L;
}

/*---------------------------------*/
/* macro_openDialog()				*/
/*---------------------------------*/
int macro_openDialog(PARAMS *pp)
{	int 		i;
	struct des 	*dp;
	GENERIC_DATA	par;
	COM_FORM	*cp;
	unsigned char	type;

	if (!_playing)
		return FORM_SHOW;

	if (*_dialogExecutionBytecodePointer != C_FORMSTART) {
err:		return FORM_SHOW;
	}

	cp = (COM_FORM *) interpreter_popParameter(_currentExecutionContext, &_dialogExecutionBytecodePointer).string;

	if (!cp)
		goto err;

	if (cp->options & FORM_REDRAW) {
		ww_redrawAllWindows(1);
	}

	/*
	 * use form, if no fields known
	 */

	for (i = cp->nfields, dp = pp->el; i > 0; i--, dp++) {
		type = *_dialogExecutionBytecodePointer;
		par = interpreter_popParameter(_currentExecutionContext, &_dialogExecutionBytecodePointer);
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_INTEGER_LITERAL:  *dp->p.i = par.intValue; break;
			case C_CHARACTER_LITERAL: *dp->p.c = par.uchar; break;
			case C_FLOAT_LITERAL: *dp->p.d = par.doubleValue; break;
			case C_LONG_LITERAL: *dp->p.l = (long)par.longValue; break;
			case C_STRING_LITERAL:
				if ((type == C_STRING_LITERAL || type == C_STRINGVAR) && par.string) {
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

/*--------------------------------------------------------------------------
 * interpreter_popParameter()
 * pop data from execution stack
 */
GENERIC_DATA interpreter_popParameter(EXECUTION_CONTEXT* pContext, unsigned char** Sp) {
	unsigned char* sp = *Sp;
	unsigned char typ = *sp;

	*Sp += interpreter_getParameterSize(typ,&((COM_CHAR1 *)sp)->val);
	switch(typ) {
		case C_FORMSTART:
			return (GENERIC_DATA) { .string = sp };
		case C_CHARACTER_LITERAL:
		case C_FURET:
			return (GENERIC_DATA) { .uchar = ((COM_CHAR1*)sp)->val };
		case C_INTEGER_LITERAL:
			return (GENERIC_DATA) { .longValue = ((COM_INT1*)sp)->val };
		case C_LONG_LITERAL:
			return (GENERIC_DATA) { .longValue = ((COM_LONG1*)sp)->val };
		case C_FLOAT_LITERAL: {
			double dDouble = ((COM_FLOAT1*)sp)->val;
			return (GENERIC_DATA) { .doubleValue = dDouble };
		}
		case C_FLOATVAR:
			return (GENERIC_DATA) { .doubleValue = sym_floatForSymbol(pContext->ec_identifierContext, ((COM_VAR*)sp)->name) };
		case C_LONGVAR:
			return (GENERIC_DATA) { .longValue = sym_integerForSymbol(pContext->ec_identifierContext, ((COM_VAR*)sp)->name) };
		case C_STRINGVAR:
			return (GENERIC_DATA) { .val = sym_stringForSymbol(pContext->ec_identifierContext, ((COM_VAR*)sp)->name) };
		case C_STRING_LITERAL:
			return (GENERIC_DATA) {
				.val = (intptr_t) & ((COM_STRING1*)sp)->s
			};
		default:
			macro_reportError();
			return (GENERIC_DATA) { 0 };
	}
}

/*
 * Returns FALSE; if the function described by the function pointer cannot 
 * be executed.
 */
int macro_isFunctionEnabled(EDFUNC* fup, long long pParam, int warn) {
	FTABLE* fp = ft_getCurrentDocument();
	WINFO* wp = ww_getCurrentEditorWindow();

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
	return macro_isFunctionEnabled(fup, pParam, warn);
}

/*---------------------------------*/
/* interpreter_executeFunction()					*/
/*---------------------------------*/
int cdecl interpreter_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3)
{
	EDFUNC *	fup = &_functionTable[num];
	int 		ret = 0;
	int			i;

	if (!interpreter_canExecuteFunction(num, (long long)p1, 1)) {
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
		while (i-- > 0 && (ret = (* fup->execute)(p1,p2,s1,s2,s3)) != 0)
			;
		_multiplier = 1;
	} else {
		if (fup->execute) {
			ret = (*fup->execute)(p1, p2, s1, s2, s3);
		}
	}

	FTABLE* fp;
	if (ret > 0 && (fup->flags & EW_MODIFY) && (fp = ft_getCurrentDocument()) != NULL) {
		ft_settime(&fp->ti_lastChanged);
	}

	if (i > 1)
		_multiplier = 1;
	return ret;
}

/*--------------------------------------------------------------------------
 * interpreter_getDollarParameter()
 */
int interpreter_getDollarParameter(intptr_t offset, PKS_VALUE *pValue) {
	if (!_currentExecutionContext) {
		interpreter_raiseError("No current execution stack");
		return 0;
	}
	*pValue = _currentExecutionContext->ec_stackFrame[offset];
	return 1;
}

/*
 * Push one value onto the stack and raise an error, if that fails. 
 */
static int interpreter_pushValue(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue) {
	if (pContext->ec_stackCurrent >= pContext->ec_stackMax) {
		interpreter_raiseError("Stack overflow");
		return 0;
	}
	*pContext->ec_stackCurrent++ = nValue;
	return 1;
}

/*---------------------------------*/
/* interpreter_doMacroFunctions()				*/
/*---------------------------------*/
intptr_t interpreter_doMacroFunctions(EXECUTION_CONTEXT* pContext, COM_1FUNC **pInstructionPointer, const COM_1FUNC *cpmax) {
	intptr_t	nativeStack[5];
	PKS_VALUE*	pStack;
	intptr_t	rc;
	intptr_t *	functionParameters;
	intptr_t*	stringFunctionParameters;
	unsigned char 	typ;
	int  		funcnum;
	COM_1FUNC *	pLocalInstructionPointer = *pInstructionPointer;

	typ = pLocalInstructionPointer->typ;

	functionParameters = nativeStack;
	funcnum = pLocalInstructionPointer->funcnum;
	if (typ != C_MACRO) {
		stringFunctionParameters = functionParameters + 2;
		memset(nativeStack, 0, sizeof nativeStack);
		if (typ == C_1FUNC) {
			*functionParameters++ = pLocalInstructionPointer->p;
		}
	} else {
		pStack = _currentExecutionContext->ec_stackCurrent;
		int nMax = (int)(_currentExecutionContext->ec_stackMax - _currentExecutionContext->ec_stackCurrent);
		// TODO: somewhat inefficient would be nice to know the number of parameters a macro has to clear out the corresponding number
		// of stack elements only.
		memset(pStack, 0, nMax * sizeof(_currentExecutionContext->ec_stackBottom[0]));
		_currentExecutionContext->ec_stackFrame = pStack;
	}

	pLocalInstructionPointer = (COM_1FUNC*)((unsigned char*)pLocalInstructionPointer + interpreter_getParameterSize(typ, &pLocalInstructionPointer->funcnum));
	while(pLocalInstructionPointer < cpmax && C_IS1PAR(pLocalInstructionPointer->typ)) {
		switch(pLocalInstructionPointer->typ) {
			case C_STRINGVAR:
			case C_STRING_LITERAL:
				if (typ == C_MACRO) {
					PKS_VALUE val;
					val.sym_data = interpreter_popParameter(pContext, (unsigned char**)&pLocalInstructionPointer);
					val.sym_type = S_STRING;
					interpreter_pushValue(pContext, val);
				} else {
					*stringFunctionParameters++ = interpreter_popParameter(pContext, (unsigned char **)&pLocalInstructionPointer).val;
				}
				break;
			case C_LONGVAR:
			case C_FLOATVAR:
			case C_FLOAT_LITERAL:
			case C_CHARACTER_LITERAL:
			case C_INTEGER_LITERAL:
			case C_LONG_LITERAL:
				if (typ == C_MACRO) {
					PKS_VALUE val;
					val.sym_data = interpreter_popParameter(pContext, (unsigned char**)&pLocalInstructionPointer);
					// TODO: support other types
					val.sym_type = S_NUMBER;
					interpreter_pushValue(pContext, val);
				} else {
					*functionParameters++ = interpreter_popParameter(pContext, (unsigned char**)&pLocalInstructionPointer).val;
				}
				break;
			default:
				goto out;
		}
	}

out:
	if (typ != C_MACRO) {
		unsigned char* readparamp = (unsigned char *)pLocalInstructionPointer;
		_dialogExecutionBytecodePointer = readparamp;
		rc = interpreter_executeFunction(funcnum,
						nativeStack[0], 
						nativeStack[1],
						 (void*)nativeStack[2],
						 (void*)nativeStack[3],
						 (void*)nativeStack[4]);

		/*
		 * TODO: function execution may pop a parameter in case a form is opened
		 * This is currently broken.
		 */
		*pInstructionPointer = (COM_1FUNC*)readparamp;
	}
	else {
		rc = macro_executeByName(((COM_MAC*)*pInstructionPointer)->name);
		*pInstructionPointer = pLocalInstructionPointer;
		_currentExecutionContext->ec_stackCurrent = pStack;
	}
	return rc;
}

/*--------------------------------------------------------------------------
 * interpreter_assignSymbol()
 */
static long interpreter_assignSymbol(EXECUTION_CONTEXT* pContext, char* name, COM_LONG1* v) {
	SYMBOL_TYPE 	sSymbolType;
	GENERIC_DATA value;

	sSymbolType = macro_isParameterFloatType(v->typ) ? S_FLOAT : (macro_isParameterStringType(v->typ) ? S_STRING : S_NUMBER);
	value = interpreter_popParameter(pContext, (unsigned char**)&v);
	return sym_makeInternalSymbol(pContext->ec_identifierContext, name, sSymbolType, value);
}


/*--------------------------------------------------------------------------
 * interpreter_returnFunctionValue()
 */
static intptr_t _fncmarker = -1;
static void interpreter_returnFunctionValue(EXECUTION_CONTEXT* pContext, unsigned char typ, intptr_t v)
{
	char 		vname[] = "__ret__";
	COM_STRING1  * value;
	char 	  	sp[512];

	if (_fncmarker < 0) {
		return;
	}

	value = (COM_STRING1 *)sp;
	if ((value->typ = typ) == C_STRING_LITERAL) {
		strcpy(value->s,(char *)v);
	} else {
		((COM_LONG1 *)value)->val = v;
	}
	vname[6] = '0' + (int)_fncmarker;
	interpreter_assignSymbol(pContext, vname,(COM_LONG1 *)value);
	_fncmarker = -1;
}

/**
 * macro_returnString()
 * Return the passed String to the macro interpreter so it can be used for further processing.
 */
void macro_returnString(char *string) {
	interpreter_returnFunctionValue(_currentExecutionContext, C_STRING_LITERAL, (intptr_t)string);
}

/*---------------------------------*/
/* macro_interpretByteCodes()					*/
/*---------------------------------*/
#define COM1_INCR(pLocalInstructionPointer,type,offset) (COM_1FUNC*)(((unsigned char *)pLocalInstructionPointer)+((type *)pLocalInstructionPointer)->offset)
#define COM_PARAMINCR(pLocalInstructionPointer)		(COM_1FUNC*)(((unsigned char *)pLocalInstructionPointer)+interpreter_getParameterSize(pLocalInstructionPointer->typ,&pLocalInstructionPointer->funcnum));
static int _macaborted;
static int macro_interpretByteCodes(COM_1FUNC *cp,COM_1FUNC *cpmax) {
	static int 	level;
	intptr_t	val;
	if (level > 10) {
		// TODO: make this dependent on stack
		error_showErrorById(IDS_MSGMACRORECURSION);
		return 0;
	}
	EXECUTION_CONTEXT* pOld = _currentExecutionContext;
	EXECUTION_CONTEXT* pContext = interpreter_pushExecutionContext();

	level++;
	for (val = 1; cp < cpmax; ) {
		if (_macaborted || (_macaborted = progress_cancelMonitor(FALSE)) != 0) {
			val = -1;
			goto end;
		}
		switch(cp->typ) {
			case C_GOTO:
				if ( ((COM_GOTO *)cp)->bratyp == BRA_ALWAYS ||
				    (((COM_GOTO *)cp)->bratyp == BRA_EQ && val == 0) ||
				    (((COM_GOTO *)cp)->bratyp == BRA_NE && val != 0) )
					cp = COM1_INCR(cp,COM_GOTO,offset);
				else
					cp = COM_PARAMINCR(cp);
				val = 1;
				break;
			case C_TEST:
				val = macro_testExpression(pContext, (COM_TEST*)cp);
				cp = COM1_INCR(cp,COM_TEST,size);
				continue;
			case C_BINOP:
				macro_evaluateBinaryExpression(pContext, (COM_BINOP*)cp);
				val = 1;
				cp = COM1_INCR(cp,COM_BINOP,size);
				continue;
			case C_DEFINE_VARIABLE:
			case C_DEFINE_PARAMETER:
				sym_makeInternalSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
					((COM_CREATESYM*)cp)->symtype,
					(GENERIC_DATA) {.longValue = ((COM_CREATESYM*)cp)->value});
				cp = COM1_INCR(cp,COM_CREATESYM,size);
				val = 1;
				break;
			case C_ASSIGN:
				interpreter_assignSymbol(pContext, ((COM_ASSIGN*)cp)->name,
					(COM_LONG1 *)COM1_INCR(cp,COM_ASSIGN,opoffset));
				cp = COM1_INCR(cp,COM_ASSIGN,size);
				val = 1;
				break;
			case C_STOP:
				val = ((COM_STOP *)cp)->rc;
				goto end;
			case C_MACRO: case C_0FUNC: case C_1FUNC:
				val = interpreter_doMacroFunctions(pContext, &cp,cpmax);
				interpreter_returnFunctionValue(pContext, C_LONG_LITERAL,val);
				continue;
			case C_FURET:
				_fncmarker = interpreter_popParameter(pContext, (unsigned char **)&cp).val;
				break;
			case C_FORMSTART:
				// TODO: not really - executing macros with forms is broken - should change the whole
				// macro with forms mechanism, on a long term run.
				goto end;
			default:
				macro_reportError();
				val = 0;
				goto end;
		}
	}
end:
	level--;
	if (level == 0) {
		error_setShowMessages(TRUE);
	}
	interpreter_popExecutionContext(pContext, pOld);
	return (int)val;
}

/*---------------------------------
 * macro_executeMacroByIndex()
 * Execute a macro given its logical
 * internal macro number.
 */
int macro_executeMacroByIndex(int macroindex)
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
int macro_executeMacro(MACROREF* mp)
{
	COM_1FUNC* cp;

	switch (mp->typ) {
	case CMD_CMDSEQ:
		cp = &_commandTable[mp->index].c_functionDef;
		return macro_interpretByteCodes(cp, cp + 1);
	case CMD_MACRO:
		return macro_executeMacroByIndex(mp->index);
	default:
		error_displayAlertDialog("Illegal command type to execute.");
	}
	return 0;
}


