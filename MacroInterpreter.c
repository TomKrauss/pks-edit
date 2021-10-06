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

#include "lineoperations.h"
#include "winfo.h"
#include "edierror.h"
#include "errordialogs.h"

#include "pksedit.h"
#include "edfuncs.h"
#include "functab.h"
#include "mouseutil.h"
#include "sym.h"
#include "markpositions.h"

extern void 			macro_reportError(void);
extern int 			EdMacroRecord(void);
extern int 			ft_selectWindowWithId(int winid, BOOL bPopup);
extern int 			dlg_displayRecordMacroOptions(int *o);

extern long 			sym_integerForSymbol(char *p);
extern intptr_t			sym_stringForSymbol(char *p);
extern long 			sym_assignSymbol(char *name, COM_LONG1 *v);
/*--------------------------------------------------------------------------
 * macro_testExpression()
 * Test an expression in a macro.
 */
extern int macro_testExpression(COM_TEST* sp);
extern void 			macro_evaluateBinaryExpression(COM_BINOP *sp);

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
extern unsigned char *	_readparamp;
extern MACRO *			_macrotab[];
extern int			_lastinsertedmac;

typedef struct ccash {
	unsigned char low;
	COM_1FUNC	    b[200];
} CCASH;

static CCASH			_ccash;

/*---------------------------------*/
/* macro_getParameterSize()				*/
/*---------------------------------*/
int macro_getParameterSize(unsigned char typ, char *s)
{	int size;

	switch(typ) {
		case C_FURET:
		case C_0FUNC:
		case C_CHAR1PAR:
			return sizeof(COM_CHAR1);
		case C_1FUNC:
			return sizeof(COM_1FUNC);
		case C_FORMSTART:
			return sizeof(COM_FORM);
		case C_GOTO:
			return sizeof(COM_GOTO);
		case C_STOP:
			return sizeof(COM_STOP);
		case C_INT1PAR:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_INT1);
		case C_LONG1PAR:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_STRINGVAR:
		case C_LONGVAR:
		case C_MACRO:
		case C_STRING1PAR:
			size = (int)((sizeof(struct c_ident) - 1 /* pad byte*/) +
					(strlen(s)+1)*sizeof(*s));
			if (size & 1) size++;
			return size;
		case C_BINOP:
			return ((COM_BINOP*)(s-1))->size;
		case C_CREATESYM:
			return ((COM_CREATESYM*)(s-1))->size;
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
		case C_CHAR1PAR: 
			((COM_CHAR1 *)sp)->val = (unsigned char)par;
			break;
		case C_FORMSTART:
			*(COM_FORM *)sp = *(COM_FORM *)par;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)par;
			break;
		case C_INT1PAR:
			((COM_INT1 *)sp)->val = (int) (intptr_t)par;
			break;
		case C_LONG1PAR:
			((COM_LONG1 *)sp)->val = (long) (intptr_t) par;
			break;
		case C_STRING1PAR:
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
	intptr_t	par;
	COM_FORM	*cp;
	unsigned char	type;

	if (!_playing)
		return FORM_SHOW;

	if (*_readparamp != C_FORMSTART) {
err:		return FORM_SHOW;
	}

	cp = (COM_FORM *) macro_popParameter(&_readparamp);

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
		par = macro_popParameter(&_readparamp);
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_INT1PAR:  *dp->p.i = (int)par; break;
			case C_CHAR1PAR: *dp->p.c = (char) par; break;
			case C_LONG1PAR: *dp->p.l = (long) par; break;
			case C_STRING1PAR:
				if ((type == C_STRING1PAR || type == C_STRINGVAR) && par) {
					strcpy(dp->p.s,(char *)par);
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
			case C_INT1PAR:
				macro_pushSequence(C_INT1PAR,(void*)((intptr_t) *dp->p.i));
				break;
			case C_CHAR1PAR:
				macro_pushSequence(C_CHAR1PAR, (void*)((intptr_t)*dp->p.c));
				break;
			case C_LONG1PAR:
				macro_pushSequence(C_LONG1PAR, (void*)((intptr_t)*dp->p.l));
				break;
			case C_STRING1PAR:
				macro_pushSequence(C_STRING1PAR, (void*)dp->p.s);
				break;
		}		
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * macro_popParameter()
 * pop data from execution stack
 */
intptr_t macro_popParameter(unsigned char **Sp)
{
	unsigned char *sp = *Sp;
	unsigned char typ = *sp;

	*Sp += macro_getParameterSize(typ,&((COM_CHAR1 *)sp)->val);

	switch(typ) {
		case C_DATA:
		case C_FORMSTART:
			return (intptr_t)sp;
		case C_CHAR1PAR:
		case C_FURET:
			return ((COM_CHAR1 *)sp)->val;
		case C_INT1PAR:
			return ((COM_INT1 *)sp)->val;
		case C_LONG1PAR:
			return ((COM_LONG1 *)sp)->val;
		case C_LONGVAR:
			return sym_integerForSymbol(((COM_VAR *)sp)->name);
		case C_STRINGVAR:
			return sym_stringForSymbol(((COM_VAR *)sp)->name);
		case C_STRING1PAR:
			return (intptr_t)&((COM_STRING1 *)sp)->s;
		default:
			macro_reportError();
			return 0;
	}
}

/*---------------------------------*/
/* macro_recordFunctionWithParameters()						*/
/*---------------------------------*/
static void macro_recordFunctionWithParameters(int fnum, int p, intptr_t p2, char *s1, char *s2)
{	COM_1FUNC c;

	if (_edfunctab[fnum].flags & EW_NOCASH)	/* avoid recursion	*/
		return;

	c.funcnum = fnum;
	c.p = p;

	macro_pushSequence(C_1FUNC,(void*) &c);
	if (p2) macro_pushSequence(C_LONG1PAR, (void*) p2);
	if (s1) {
		macro_pushSequence(C_STRING1PAR,(void*)s1);
		if (s2)
			macro_pushSequence(C_STRING1PAR,(void*)s2);
	}
}

/*---------------------------------*/
/* macro_recordFunction()					*/
/*---------------------------------*/
void macro_recordFunction(FTABLE *fp, int p)
{	COM_1FUNC *cp;

	if (_ccash.low < DIM(_ccash.b)-2) {
		if (!_ccash.low) {
			fm_savepos(TM_LASTINSERT);
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
/* macro_stopRecordingFunctions()					*/
/*---------------------------------*/
void macro_stopRecordingFunctions()
{
	_ccash.low  = 0;
}

/*
 * Returns FALSE; if the function described by the function pointer cannot 
 * be executed.
 */
int macro_isFunctionEnabled(EDFUNC* fup, int warn) {
	FTABLE* fp = ft_getCurrentDocument();
	WINFO* wp = ww_getCurrentEditorWindow();

	if (fup->flags & EW_NEEDSCURRF && fp == 0) {
		return 0;
	}
	if ((fup->flags & EW_MODIFY) && ft_isReadonly(fp)) {
		if (warn && ft_isReadonly(fp)) {
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
	return 1;
}

/*--------------------------------------------------------------------------
 * macro_canExecuteFunction()
 */
int macro_canExecuteFunction(int num, int warn) {
	EDFUNC *	fup = &_edfunctab[num];
	return macro_isFunctionEnabled(fup, warn);
}

/*---------------------------------*/
/* macro_executeFunction()					*/
/*---------------------------------*/
int cdecl macro_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3)
{
	EDFUNC *	fup = &_edfunctab[num];
	int 		ret,i;

	if (!macro_canExecuteFunction(num, 1)) {
		return 0;
	}

	if (fup->flags & EW_UNDOFLSH) {
		undo_startModification(ft_getCurrentDocument());
	}

	if ((fup->flags & EW_CCASH) == 0) {
		macro_stopRecordingFunctions();
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

	if (ret > 0 && ft_getCurrentDocument() && (fup->flags & EW_MODIFY)) {
		ft_settime(&ft_getCurrentDocument()->ti_modified);
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
intptr_t macro_doMacroFunctions(COM_1FUNC **Cp, COM_1FUNC *cpmax) {
	intptr_t 	stack[40];
	intptr_t*	saveStack;
	intptr_t	rc,*sp,*sp2;
	unsigned char 	typ;
	int  		funcnum;
	COM_1FUNC *	cp = *Cp;

	sp = stack;
	sp2 = sp+2;
	sp[0] = sp[1] = sp[2] = sp[3] = 0;

	if ((typ = cp->typ) == C_1FUNC) {
		*sp++ = cp->p;
	}

	funcnum = cp->funcnum;
	cp = (COM_1FUNC*)((unsigned char *)cp + macro_getParameterSize(typ,&cp->funcnum));

	while(cp < cpmax && C_IS1PAR(cp->typ)) {
		switch(cp->typ) {
			case C_STRINGVAR:
			case C_STRING1PAR:
				if (typ == C_MACRO) {
					*sp++ = 1;
					*sp++ = macro_popParameter((unsigned char **)&cp);
				} else {
					*sp2++ = macro_popParameter((unsigned char **)&cp);
				}
				break;
			case C_LONGVAR:
			case C_CHAR1PAR:
			case C_INT1PAR:
			case C_LONG1PAR:
				if (typ == C_MACRO) {
					*sp++ = 0;
				}
				*sp++ = macro_popParameter((unsigned char **)&cp);
				break;
			default:
				goto out;
		}
	}

out:
	if (typ != C_MACRO) {
		_readparamp = (unsigned char *)cp;
		rc = macro_executeFunction(funcnum,stack[0],stack[1],
						 (void*)stack[2],
						 (void*)stack[3],
						 (void*)stack[4]);
		/*
		 * this is a hack: function execution may pop a parameter
		 */
		*Cp = (COM_1FUNC*)_readparamp;
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
 * macro_returnFunctionValue()
 */
static intptr_t _fncmarker = -1;
static void macro_returnFunctionValue(unsigned char typ, intptr_t v)
{
	char *		vname;
	COM_STRING1  * value;
	char 	  	sp[512];

	if (_fncmarker < 0) {
		return;
	}

	vname = "__ret__";
	value = (COM_STRING1 *)sp;
	if ((value->typ = typ) == C_STRING1PAR) {
		strcpy(value->s,(char *)v);
	} else {
		((COM_LONG1 *)value)->val = v;
	}
	vname[6] = '0' + (int)_fncmarker;
	sym_assignSymbol(vname,(COM_LONG1 *)value);
	_fncmarker = -1;
}

/**
 * macro_returnString()
 * Return the passed String to the macro interpreter so it can be used for further processing.
 */
void macro_returnString(char *string)
{
	macro_returnFunctionValue(C_STRING1PAR,(intptr_t)string);
}

/*---------------------------------*/
/* macro_executeSequence()					*/
/*---------------------------------*/
#define COM1_INCR(cp,type,offset) (COM_1FUNC*)(((unsigned char *)cp)+((type *)cp)->offset)
#define COM_PARAMINCR(cp)		(COM_1FUNC*)(((unsigned char *)cp)+macro_getParameterSize(cp->typ,&cp->funcnum));
static int _macaborted;
int macro_executeSequence(COM_1FUNC *cp,COM_1FUNC *cpmax) {
	intptr_t	val;

	for (val = 1; cp < cpmax; ) {
		if (_macaborted || (_macaborted = progress_cancelMonitor(FALSE)) != 0)
			return -1;
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
				val = macro_testExpression((COM_TEST*)cp);
				cp = COM1_INCR(cp,COM_TEST,size);
				continue;
			case C_BINOP:
				macro_evaluateBinaryExpression((COM_BINOP*)cp);
				val = 1;
				cp = COM1_INCR(cp,COM_BINOP,size);
				continue;
			case C_CREATESYM:
				sym_makeInternalSymbol(((COM_CREATESYM*)cp)->name,
					((COM_CREATESYM*)cp)->symtype,
					((COM_CREATESYM*)cp)->value);
				cp = COM1_INCR(cp,COM_CREATESYM,size);
				val = 1;
				break;
			case C_ASSIGN:
				sym_assignSymbol(((COM_ASSIGN*)cp)->name,
					(COM_LONG1 *)COM1_INCR(cp,COM_ASSIGN,opoffset));
				cp = COM1_INCR(cp,COM_ASSIGN,size);
				val = 1;
				break;
			case C_STOP:
				return ((COM_STOP *)cp)->rc;
			case C_MACRO: case C_0FUNC: case C_1FUNC:
				val = macro_doMacroFunctions(&cp,cpmax);
				macro_returnFunctionValue(C_LONG1PAR,val);
				continue;
			case C_FURET:
				_fncmarker = macro_popParameter((unsigned char **)&cp);
				break;
			default:
				macro_reportError();
				return 0;
		}
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
	static int 	level;

	if (macroindex == MAC_LASTREC && 
	    (macroindex = _lastinsertedmac) < 0)
		return 0;

	if (level > 10) {
		error_showErrorById(IDS_MSGMACRORECURSION);
		return 0;
	}

	level++;

	if ((wasplaying = _playing) == 0 && ft_getCurrentDocument()) {
		undo_startModification(ft_getCurrentDocument());
# if defined(ATARI_ST)
		rdcash(-2);					/* init redraw macro_recordFunctionWithParameters */
# endif
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
		if ((ret = macro_executeSequence(cp,cpmax)) <= 0) {
			ret = 0;
			break;
		}
	}

	if ((_playing = wasplaying) == 0) {
		_macaborted = 0;

# if defined(ATARI_ST)
		EdSelectWindow(_cfileno);
		rdupdate(0);
		mouse_setDefaultCursor();
# else
		WINFO* wp = ww_getCurrentEditorWindow();
		if (wp) {
			ft_selectWindowWithId(wp->win_id, FALSE);
		}
# if 0
		ww_redrawAllWindows(0);
# endif
# endif
	}

	_recording = hadrecstate;

	level--;

	return ret;
}


