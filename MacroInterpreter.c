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

#include "documentmodel.h"
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
#include "markpositions.h"
#include "caretmovement.h"

extern void 			macro_reportError(void);
extern int 				EdMacroRecord(void);
extern int 				ft_selectWindowWithId(int winid, BOOL bPopup);
extern int 				dlg_displayRecordMacroOptions(int *o);

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
void 				macro_stopRecordingFunctions(void);

int					_playing;

extern long			_multiplier;
extern int			_recording;
extern int			_nmacros;
extern char *			_macroname;
extern char *			_cmdfuncp;
extern char *			_cmdmaxp;
extern char *			_cmdparamp;
extern MACRO *			_macrotab[];
extern int			_lastinsertedmac;
extern unsigned char* _readparamp;

typedef struct ccash {
	unsigned char low;
	COM_1FUNC	    b[200];
} CCASH;

static CCASH			_ccash;

static EXECUTION_CONTEXT* macro_getRecordingContext() {
	static EXECUTION_CONTEXT _recordingContext;
	_recordingContext.ec_identifierContext = sym_getGlobalContext();
	return &_recordingContext;
}

/*---------------------------------*/
/* macro_getParameterSize()				*/
/*---------------------------------*/
int macro_getParameterSize(unsigned char typ, const char *s)
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
			return (((COM_CREATESYM*)s)->size);
		case C_LONG_LITERAL:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_STRINGVAR:
		case C_FLOATVAR:
		case C_LONGVAR:
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
		case C_DATA:
			return sizeof(COM_DATA) + ((COM_DATA*)s)->size;
	}
	/*
	 * oops: this is an error
	 */
	macro_reportError();
	return 10000L;
}

/*
 * Creates an execution context for executing a macro function.
 */
static EXECUTION_CONTEXT* macro_createExecutionContext() {
	EXECUTION_CONTEXT* pResult = calloc(1, sizeof * pResult);
	pResult->ec_identifierContext = sym_pushContext(sym_getGlobalContext());
	return pResult;
}

/*
 * Destroy a previousy allocated execution context.
 */
static void macro_destroyExecutionContext(EXECUTION_CONTEXT* pContext) {
	sym_popContext(pContext->ec_identifierContext);
	free(pContext);
}

/*--------------------------------------------------------------------------
 * macro_pushSequence()
 *
 * this is called, when recording
 * we do not need, to handle all command types, cause in ineractive
 * mode, delta like test or binop are not possible
 */
static void macro_pushSequence(unsigned char typ, void* par)
{	char *spend;
	char *sp;
	int  s;

	if (_recording == 0) {
		return;
	}

	sp = _cmdparamp;
	s  = macro_getParameterSize(typ,(char *)par);

	if ((spend = sp+s) > _cmdmaxp) {
		error_showErrorById(IDS_MSGMACOVERFLOW);
		EdMacroRecord();			/* STOP the recording machine */
		return;
	}

	switch(typ) {
		case C_FURET:
		case C_0FUNC:
		case C_CHARACTER_LITERAL: 
			((COM_CHAR1 *)sp)->val = (unsigned char)par;
			break;
		case C_FORMSTART:
			*(COM_FORM *)sp = *(COM_FORM *)par;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)par;
			break;
		case C_INTEGER_LITERAL:
			((COM_INT1 *)sp)->val = (int) (intptr_t)par;
			break;
		case C_LONG_LITERAL:
			((COM_LONG1 *)sp)->val = (long) (intptr_t) par;
			break;
		case C_FLOAT_LITERAL:
			((COM_FLOAT1*)sp)->val = (double)(intptr_t)par;
			break;
		case C_STRING_LITERAL:
		case C_MACRO:
			strcpy(((COM_MAC*)sp)->name,(char *)par);
			break;
	}

	if (C_ISCMD(typ))
		_cmdfuncp = sp;

	*sp = typ;
	*spend = C_STOP;
	_cmdparamp = spend;
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

	if (*_readparamp != C_FORMSTART) {
err:		return FORM_SHOW;
	}

	cp = (COM_FORM *) macro_popParameter(macro_getRecordingContext(), &_readparamp).string;

	if (!cp)
		goto err;

	if (cp->options & FORM_REDRAW) {
		ww_redrawAllWindows(1);
	}

	/*
	 * use form, if no fields known
	 */

	for (i = cp->nfields, dp = pp->el; i > 0; i--, dp++) {
		type = *_readparamp;
		par = macro_popParameter(macro_getRecordingContext(), &_readparamp);
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
	
	if (_recording) {
		macro_recordOperation(pp);
	}

	return cp->options & FORM_SHOW;
}

/*---------------------------------*/
/* macro_recordOperation()				*/
/*---------------------------------*/
int macro_recordOperation(PARAMS *pp)
{	int 		i,opt;
	struct 	des *dp;
	char		*savepos;
	COM_FORM	cf;

	if (/*_playing || */
	    _recording == 0)
		return 0;

	savepos = _cmdparamp - macro_getParameterSize(C_1FUNC,(char *)0);
	opt = 0;

	if (pp->flags & P_MAYOPEN) {
#if defined(ATARI_ST)
		if (recopt(&opt) == 0) {
#else
		if (dlg_displayRecordMacroOptions(&opt) == 0) {
#endif
			/* dialog is cancelled */
			_cmdparamp = savepos;
			return 0;
		}
	}

	cf.options = opt;
	cf.nfields = pp->nel;
	/*
	 * slight optimization: if form should be opened uninitialized,
	 * dont EdMacroRecord the parameters
	 */
	if ((opt & FORM_INIT) == 0)
		cf.nfields = 0;

	macro_pushSequence(C_FORMSTART,&cf);

	dp = pp->el;
	for (i = cf.nfields; i > 0; i--, dp++) {
		switch(dp->cmd_type) {
			case C_INTEGER_LITERAL:
				macro_pushSequence(C_INTEGER_LITERAL,(void*)((intptr_t) *dp->p.i));
				break;
			case C_CHARACTER_LITERAL:
				macro_pushSequence(C_CHARACTER_LITERAL, (void*)((intptr_t)*dp->p.c));
				break;
			case C_FLOAT_LITERAL:
				macro_pushSequence(C_FLOAT_LITERAL, (void*)((intptr_t)*dp->p.d));
				break;
			case C_LONG_LITERAL:
				macro_pushSequence(C_LONG_LITERAL, (void*)((intptr_t)*dp->p.l));
				break;
			case C_STRING_LITERAL:
				macro_pushSequence(C_STRING_LITERAL, (void*)dp->p.s);
				break;
		}		
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * macro_popParameter()
 * pop data from execution stack
 */
GENERIC_DATA macro_popParameter(EXECUTION_CONTEXT* pContext, unsigned char** Sp) {
	unsigned char* sp = *Sp;
	unsigned char typ = *sp;

	*Sp += macro_getParameterSize(typ,&((COM_CHAR1 *)sp)->val);
	switch(typ) {
		case C_DATA:
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

/*---------------------------------*/
/* macro_recordFunctionWithParameters()						*/
/*---------------------------------*/
static void macro_recordFunctionWithParameters(int fnum, int p, intptr_t p2, char *s1, char *s2)
{	COM_1FUNC c;

	if (_functionTable[fnum].flags & EW_NOCASH)	/* avoid recursion	*/
		return;

	c.funcnum = fnum;
	c.p = p;

	macro_pushSequence(C_1FUNC,(void*) &c);
	if (p2) macro_pushSequence(C_LONG_LITERAL, (void*) p2);
	if (s1) {
		macro_pushSequence(C_STRING_LITERAL,(void*)s1);
		if (s2)
			macro_pushSequence(C_STRING_LITERAL,(void*)s2);
	}
}

/*---------------------------------*/
/* macro_recordFunction()					*/
/*---------------------------------*/
void macro_recordFunction(FTABLE *fp, int p) {

	if (_ccash.low < DIM(_ccash.b)-2) {
		COM_1FUNC* cp;
		if (!_ccash.low) {
			fm_savepos(MTE_AUTO_LAST_INSERT);
			undo_startModification(fp);
		}
		cp = &_ccash.b[_ccash.low++];
		cp->typ = C_1FUNC;
		cp->funcnum = FUNC_EdCharInsert;
		cp->p = p;
		/*
		 * stop mark
		 */
		cp[1].typ = C_STOP;
	}
}

/*---------------------------------*/
/* macro_stopRecordingFunctions()  */
/*---------------------------------*/
void macro_stopRecordingFunctions() {
	_ccash.low  = 0;
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
 * macro_canExecuteFunction()
 */
int macro_canExecuteFunction(int num, long long pParam, int warn) {
	EDFUNC *	fup = &_functionTable[num];
	return macro_isFunctionEnabled(fup, pParam, warn);
}

/*---------------------------------*/
/* macro_executeFunction()					*/
/*---------------------------------*/
int cdecl macro_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3)
{
	EDFUNC *	fup = &_functionTable[num];
	int 		ret = 0;
	int			i;

	if (!macro_canExecuteFunction(num, (long long)p1, 1)) {
		return 0;
	}

	if (fup->flags & EW_UNDOFLSH) {
		undo_startModification(ft_getCurrentDocument());
	}

	if ((fup->flags & EW_CCASH) == 0) {
		macro_stopRecordingFunctions();
		caret_removeSecondaryCarets();
	}

	if (_recording)
		macro_recordFunctionWithParameters(num,(int)p1,p2,s1,s2);

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
 * macro_getDollarParameter()
 */
static intptr_t * currentParamStack;
int macro_getDollarParameter(intptr_t offset, int *typ, intptr_t *value)
{
	if (!currentParamStack || offset < 0 || offset >= 4) {
		error_displayAlertDialog("no such parameter passed");
		return 0;
	}

	offset *= 2;
	*typ = (int) currentParamStack[offset++];
	*value = currentParamStack[offset];
	return 1;
}

/*---------------------------------*/
/* macro_doMacroFunctions()				*/
/*---------------------------------*/
intptr_t macro_doMacroFunctions(EXECUTION_CONTEXT* pContext, COM_1FUNC **Cp, const COM_1FUNC *cpmax) {
	intptr_t 	stack[40];
	intptr_t*	saveStack;
	intptr_t	rc,*sp,*functionStringParameters;
	unsigned char 	typ;
	int  		funcnum;
	COM_1FUNC *	cp = *Cp;

	sp = stack;
	functionStringParameters = sp+2;
	sp[0] = sp[1] = sp[2] = sp[3] = 0;

	if ((typ = cp->typ) == C_1FUNC) {
		*sp++ = cp->p;
	}

	funcnum = cp->funcnum;
	cp = (COM_1FUNC*)((unsigned char *)cp + macro_getParameterSize(typ,&cp->funcnum));

	while(cp < cpmax && C_IS1PAR(cp->typ)) {
		switch(cp->typ) {
			case C_STRINGVAR:
			case C_STRING_LITERAL:
				if (typ == C_MACRO) {
					*sp++ = 1;
					*sp++ = macro_popParameter(pContext, (unsigned char **)&cp).val;
				} else {
					*functionStringParameters++ = macro_popParameter(pContext, (unsigned char **)&cp).val;
				}
				break;
			case C_LONGVAR:
			case C_FLOATVAR:
			case C_FLOAT_LITERAL:
			case C_CHARACTER_LITERAL:
			case C_INTEGER_LITERAL:
			case C_LONG_LITERAL:
				if (typ == C_MACRO) {
					*sp++ = 0;
				}
				*sp++ = macro_popParameter(pContext, (unsigned char **)&cp).val;
				break;
			default:
				goto out;
		}
	}

out:
	if (typ != C_MACRO) {
		unsigned char* readparamp = (unsigned char *)cp;
		_readparamp = readparamp;
		rc = macro_executeFunction(funcnum,stack[0],stack[1],
						 (void*)stack[2],
						 (void*)stack[3],
						 (void*)stack[4]);
		/*
		 * TODO: function execution may pop a parameter in case a form is opened
		 * This is currently broken.
		 */
		*Cp = (COM_1FUNC*)readparamp;
	}
	else {
		saveStack = currentParamStack;
		currentParamStack = stack;
		rc = macro_executeByName(((COM_MAC*)*Cp)->name);
		currentParamStack = saveStack;
		*Cp = cp;
	}
	return rc;
}

/*--------------------------------------------------------------------------
 * sym_assignSymbol()
 */
static long sym_assignSymbol(EXECUTION_CONTEXT* pContext, char* name, COM_LONG1* v) {
	SYMBOL_TYPE 	sSymbolType;
	GENERIC_DATA value;

	sSymbolType = macro_isParameterFloatType(v->typ) ? S_FLOAT : (macro_isParameterStringType(v->typ) ? S_STRING : S_NUMBER);
	value = macro_popParameter(pContext, (unsigned char**)&v);
	return sym_makeInternalSymbol(pContext->ec_identifierContext, name, sSymbolType, value);
}


/*--------------------------------------------------------------------------
 * macro_returnFunctionValue()
 */
static intptr_t _fncmarker = -1;
static void macro_returnFunctionValue(EXECUTION_CONTEXT* pContext, unsigned char typ, intptr_t v)
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
	sym_assignSymbol(pContext, vname,(COM_LONG1 *)value);
	_fncmarker = -1;
}

/**
 * macro_returnString()
 * Return the passed String to the macro interpreter so it can be used for further processing.
 */
void macro_returnString(char *string) {
	macro_returnFunctionValue(macro_getRecordingContext(), C_STRING_LITERAL, (intptr_t)string);
}

/*---------------------------------*/
/* macro_interpretByteCodes()					*/
/*---------------------------------*/
#define COM1_INCR(cp,type,offset) (COM_1FUNC*)(((unsigned char *)cp)+((type *)cp)->offset)
#define COM_PARAMINCR(cp)		(COM_1FUNC*)(((unsigned char *)cp)+macro_getParameterSize(cp->typ,&cp->funcnum));
static int _macaborted;
static int macro_interpretByteCodes(EXECUTION_CONTEXT* pContext, COM_1FUNC *cp,COM_1FUNC *cpmax) {
	static int 	level;
	intptr_t	val;
	if (level > 10) {
		error_showErrorById(IDS_MSGMACRORECURSION);
		return 0;
	}

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
			case C_DEFINE_PARAMETER:
				sym_makeInternalSymbol(pContext->ec_identifierContext, ((COM_CREATESYM*)cp)->name,
					((COM_CREATESYM*)cp)->symtype,
					(GENERIC_DATA) {.longValue = ((COM_CREATESYM*)cp)->value});
				cp = COM1_INCR(cp,COM_CREATESYM,size);
				val = 1;
				break;
			case C_DEFINE_VARIABLE:
				cp = COM1_INCR(cp, COM_CREATESYM, size);
				val = 1;
				break;
			case C_ASSIGN:
				sym_assignSymbol(pContext, ((COM_ASSIGN*)cp)->name,
					(COM_LONG1 *)COM1_INCR(cp,COM_ASSIGN,opoffset));
				cp = COM1_INCR(cp,COM_ASSIGN,size);
				val = 1;
				break;
			case C_STOP:
				val = ((COM_STOP *)cp)->rc;
				goto end;
			case C_MACRO: case C_0FUNC: case C_1FUNC:
				val = macro_doMacroFunctions(pContext, &cp,cpmax);
				macro_returnFunctionValue(pContext, C_LONG_LITERAL,val);
				continue;
			case C_FURET:
				_fncmarker = macro_popParameter(pContext, (unsigned char **)&cp).val;
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
	long   		hadrecstate = _recording;

	if (macroindex == MAC_LASTREC && 
	    (macroindex = _lastinsertedmac) < 0)
		return 0;

	if ((wasplaying = _playing) == 0 && ft_getCurrentDocument()) {
		undo_startModification(ft_getCurrentDocument());
	} 

	if (hadrecstate && macroindex >= 0) {
		macro_pushSequence(C_MACRO,_macrotab[macroindex]->name);
		_recording = 0;
	}

	_playing = 1;
	count = _multiplier;
	_multiplier = 1;
	for (i = 0; i < count; i++) {
		switch(macroindex) {
			case MAC_AUTO:	
				macro_stopRecordingFunctions();
				_playing = 2;
				cp = (COM_1FUNC *) _ccash.b; 
				cpmax = (COM_1FUNC *)(&_ccash.b[DIM(_ccash.b)]);
				break;
			default:
				if ((mp = _macrotab[macroindex]) == 0)
					return 0;
				cp = (COM_1FUNC *)MAC_DATA(mp);
				cpmax = (COM_1FUNC *)((char *)cp+mp->size);
				break;
		}
		EXECUTION_CONTEXT* pContext = macro_createExecutionContext();
		ret = macro_interpretByteCodes(pContext, cp, cpmax);
		macro_destroyExecutionContext(pContext);
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

	_recording = hadrecstate;

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
		cp = &_cmdseqtab[mp->index].c_functionDef;
		EXECUTION_CONTEXT* pContext = macro_createExecutionContext();
		int nRet = macro_interpretByteCodes(pContext, cp, cp + 1);
		macro_destroyExecutionContext(pContext);
		return nRet;
	case CMD_MACRO:
		return macro_executeMacroByIndex(mp->index);
	default:
		error_displayAlertDialog("bad command or macro");
	}
	return 0;
}


