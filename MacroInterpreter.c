/*
 * MacroInterpreter.c
 *
 * PKS-EDIT interpreter engine
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 11/89
 *									
 */

# include	<string.h>
# include	<windows.h>

# include	"editab.h"
# include "winfo.h"
# include	"edierror.h"

# include	"pksedit.h"
# include	"edfuncs.h"
# include	"functab.h"

extern void 			MacFormatErr(void);
extern int 			EdMacroRecord(void);
extern int 			SelectWindow(int winid, BOOL bPopup);
extern void 			changemouseform(void);
extern int 			RecordOptions(int *o);

extern long 			MakeInteger(void *p);
extern long 			MakeString(void *p);
extern long 			Assign(char *name, COM_LONG1 *v);
extern int 			MakeInternalSym(char *name, char ed_typ, long value);
extern int 			Test(COM_TEST *sp);
extern void 			Binop(COM_BINOP *sp);

extern int 			ed_abort(BOOL bRedraw);
extern void 			redrawallwi(int update);
extern void 			u_init(FTABLE *fp);
extern int 			chkblk(FTABLE *fp);
extern void 			ft_settime(EDTIME *tp);
extern int 			do_macbyname(char *name);

int 					param_record(PARAMS *pp);
void 				stopcash(void);

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
/* param_space()				*/
/*---------------------------------*/
int param_space(unsigned char typ, char *s)
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
			size = (sizeof(struct c_ident) - 1 /* pad byte*/) +
					(strlen(s)+1)*sizeof(*s);
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
	MacFormatErr();
	return 10000L;
}

/*--------------------------------------------------------------------------
 * push_sequence()
 *
 * this is called, when recording
 * we do not need, to handle all command types, cause in ineractive
 * mode, commands like test or binop are not possible
 */
static void push_sequence(unsigned char typ, long par)
{	char *spend;
	char *sp;
	int  s,slen;

	if (_recording == 0)
		return;

	sp = _cmdparamp;
	s  = param_space(typ,(char *)par);

	if ((spend = sp+s) > _cmdmaxp) {
		ed_error(IDS_MSGMACOVERFLOW);
		EdMacroRecord();			/* STOP the recording machine */
		return;
	}

	switch(typ) {
		case C_FURET:
		case C_0FUNC:
		case C_CHAR1PAR: 
			((COM_CHAR1 *)sp)->val = par;
			break;
		case C_FORMSTART:
			*(COM_FORM *)sp = *(COM_FORM *)par;
			break;
		case C_1FUNC:
			*(COM_1FUNC *)sp = *(COM_1FUNC *)par;
			break;
		case C_INT1PAR:
			((COM_INT1 *)sp)->val = par;
			break;
		case C_LONG1PAR:
			((COM_LONG1 *)sp)->val = par;
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
/* param_dialopen()				*/
/*---------------------------------*/
int param_dialopen(PARAMS *pp)
{	int 			i;
	struct des 	*dp;
	long			par;
	COM_FORM		*cp;
	unsigned char	type;

	if (!_playing)
		return FORM_SHOW;

	if (*_readparamp != C_FORMSTART) {
err:		return FORM_SHOW;
	}

	cp = (COM_FORM *) param_pop(&_readparamp);

	if (!cp)
		goto err;

	if (cp->options & FORM_REDRAW) {
#if defined(ATARI_ST)
		forceredraw();
#else
		redrawallwi(1);
#endif
	}

	/*
	 * use form, if no fields known
	 */

	for (i = cp->nfields, dp = pp->el; i > 0; i--, dp++) {
		type = *_readparamp;
		par = param_pop(&_readparamp);
		if (cp->options & FORM_INIT) {
		   switch(dp->cmd_type) {
			case C_INT1PAR:  *dp->p.i = par; break;
			case C_CHAR1PAR: *dp->p.c = par; break;
			case C_LONG1PAR: *dp->p.l = par; break;
			case C_STRING1PAR:
				if ((type == C_STRING1PAR || type == C_STRINGVAR) && par) {
					strcpy(dp->p.s,(char *)par);
				}
				else {
					ed_error(IDS_MSGMACFORMATERR);
				}
				break;
		   }
		}
	}
	
	if (_recording) {
		param_record(pp);
	}

	return cp->options & FORM_SHOW;
}

/*---------------------------------*/
/* param_record()				*/
/*---------------------------------*/
int param_record(PARAMS *pp)
{	int 		i,opt;
	struct 	des *dp;
	char		*savepos;
	COM_FORM	cf;

	if (/*_playing || */
	    _recording == 0)
		return 0;

	savepos = _cmdparamp - param_space(C_1FUNC,(char *)0);
	opt = 0;

	if (pp->flags & P_MAYOPEN) {
#if defined(ATARI_ST)
		if (recopt(&opt) == 0) {
#else
		if (RecordOptions(&opt) == 0) {
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

	push_sequence(C_FORMSTART,(long)&cf);

	dp = pp->el;
	for (i = cf.nfields; i > 0; i--, dp++) {
		switch(dp->cmd_type) {
			case C_INT1PAR:
				push_sequence(C_INT1PAR,*dp->p.i);
				break;
			case C_CHAR1PAR:
				push_sequence(C_CHAR1PAR,*dp->p.c);
				break;
			case C_LONG1PAR:
				push_sequence(C_LONG1PAR,*dp->p.l);
				break;
			case C_STRING1PAR:
				push_sequence(C_STRING1PAR,(long )dp->p.s);
				break;
		}		
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * param_pop()
 * pop data from execution stack
 */
long param_pop(unsigned char **Sp)
{
	long ret;
	unsigned char *sp = *Sp;
	unsigned char typ = *sp;

	*Sp += param_space(typ,&((COM_CHAR1 *)sp)->val);

	switch(typ) {
		case C_DATA:
		case C_FORMSTART:
			return (long)sp;
		case C_CHAR1PAR:
		case C_FURET:
			return ((COM_CHAR1 *)sp)->val;
		case C_INT1PAR:
			return ((COM_INT1 *)sp)->val;
		case C_LONG1PAR:
			return ((COM_LONG1 *)sp)->val;
		case C_LONGVAR:
			return MakeInteger(((COM_VAR *)sp)->name);
		case C_STRINGVAR:
			return MakeString(((COM_VAR *)sp)->name);
		case C_STRING1PAR:
			return (long)&((COM_STRING1 *)sp)->s;
		default:
			MacFormatErr();
			return 0;
	}
}

/*---------------------------------*/
/* cash()						*/
/*---------------------------------*/
static void cash(int fnum, int p, long p2, char *s1, char *s2)
{	COM_1FUNC c;

	if (_edfunctab[fnum].flags & EW_NOCASH)	/* avoid recursion	*/
		return;

	c.funcnum = fnum;
	c.p = p;

	push_sequence(C_1FUNC,(long) &c);
	if (p2) push_sequence(C_LONG1PAR,p2);
	if (s1) {
		push_sequence(C_STRING1PAR,(long)s1);
		if (s2)
			push_sequence(C_STRING1PAR,(long)s2);
	}
}

/*---------------------------------*/
/* cash2()					*/
/*---------------------------------*/
void cash2(FTABLE *fp, int p)
{	COM_1FUNC *cp;

	if (_ccash.low < DIM(_ccash.b)-2) {
		if (!_ccash.low) {
			u_init(fp);
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
/* stopcash()					*/
/*---------------------------------*/
void stopcash()
{
	_ccash.low  = 0;
}

/*---------------------------------*/
/* readonly()					*/
/*---------------------------------*/
int readonly(FTABLE *fp)
{
	if (fp->lin->workmode & O_RDONLY) {
		ed_error(IDS_MSGRDONLY);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * CanExecute()
 */
int CanExecute(int num, int warn)
{
	EDFUNC *	fup = &_edfunctab[num];

	if (fup->flags & EW_NEEDSCURRF) {
#if defined(ATARI_ST)
		if (_currwindow == 0)
			return 0;
#ifdef	ASSERT
		if (_currfile == (FTABLE *) 0) {
			EdAlert("oops: currwindow, but no file");
			return 0;
		}
#endif
#else
		if (_currfile == 0) {
			return 0;
		}
#endif
	}

	if ((fup->flags & EW_MODIFY) && (_currfile->lin->workmode & O_RDONLY)) {
		if (warn) {
			readonly(_currfile);
		}
		return 0;
	}

	if (fup->flags & EW_NEEDSBLK && !_chkblk(_currfile)) {
		if (warn) {
			chkblk(_currfile);
		}
		return 0;
	}
	return 1;
}

/*---------------------------------*/
/* do_func()					*/
/*---------------------------------*/
int cdecl do_func(int num, long p1, long p2, void *s1, void *s2, void *s3)
{
	EDFUNC *	fup = &_edfunctab[num];
	int 		ret,i;

	if (!CanExecute(num, 1)) {
		return 0;
	}

	if (fup->flags & EW_UNDOFLSH) {
		u_init(_currfile);
	}

	if ((fup->flags & EW_CCASH) == 0) {
		stopcash();
	}

	if (_recording)
		cash(num,p1,p2,s1,s2);

	if ((i = _multiplier) > 1 && (fup->flags & EW_MULTI) == 0) {
		while (i-- > 0 && (ret = (* fup->func)(p1,p2,s1,s2,s3)) != 0)
			;
		_multiplier = 1;
	} else {
		ret = (* fup->func)(p1,p2,s1,s2,s3);
	}

	if (ret > 0 && _currfile && (fup->flags & EW_MODIFY)) {
		ft_settime(&_currfile->ti_modified);
	}

	if (i > 1)
		_multiplier = 1;
	return ret;
}

/*--------------------------------------------------------------------------
 * GetDollar()
 */
static long * currentParamStack;
int GetDollar(long offset, int *typ, long *value)
{
	if (!currentParamStack || offset < 0 || offset >= 4) {
		alert("no such parameter passed");
		return 0;
	}

	offset *= 2;
	*typ = (int) currentParamStack[offset++];
	*value = currentParamStack[offset];
	return 1;
}

/*---------------------------------*/
/* do_macfunc()				*/
/*---------------------------------*/
long do_macfunc(COM_1FUNC **Cp, COM_1FUNC *cpmax)
{
	long 		stack[40];
	long *		saveStack;
	long 		rc,*sp,*sp2;
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
	cp = (COM_1FUNC*)((unsigned char *)cp + param_space(typ,&cp->funcnum));

	while(cp < cpmax && C_IS1PAR(cp->typ)) {
		switch(cp->typ) {
			case C_STRINGVAR:
			case C_STRING1PAR:
				if (typ == C_MACRO) {
					*sp++ = 1;
					*sp++ = (long) param_pop((unsigned char **)&cp);
				} else {
					*sp2++ = (long) param_pop((unsigned char **)&cp);
				}
				break;
			case C_LONGVAR:
			case C_CHAR1PAR:
			case C_INT1PAR:
			case C_LONG1PAR:
				if (typ == C_MACRO) {
					*sp++ = 0;
				}
				*sp++ = (long)
					param_pop((unsigned char **)&cp);
				break;
			default:
				goto out;
		}
	}

out:
	if (typ != C_MACRO) {
		_readparamp = (unsigned char *)cp;
		rc = do_func(funcnum,stack[0],stack[1],
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
		rc = do_macbyname(((COM_MAC*)*Cp)->name);
		currentParamStack = saveStack;
		*Cp = cp;
	}
	return rc;
}

/*--------------------------------------------------------------------------
 * ReturnString()
 */
static int _fncmarker = -1;
static void ReturnFuncVal(unsigned char typ, long v)
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
	vname[6] = '0' + _fncmarker;
	Assign(vname,(COM_LONG1 *)value);
	_fncmarker = -1;
}

void ReturnString(char *string)
{
	ReturnFuncVal(C_STRING1PAR,(long)string);
}

/*---------------------------------*/
/* do_seq()					*/
/*---------------------------------*/
# define COM1_INCR(cp,type,offset) (COM_1FUNC*)(((unsigned char *)cp)+((type *)cp)->offset)
# define COM_PARAMINCR(cp)		(COM_1FUNC*)(((unsigned char *)cp)+param_space(cp->typ,&cp->funcnum));
static int _macaborted;
int do_seq(COM_1FUNC *cp,COM_1FUNC *cpmax)
{
	unsigned char *	sp;
	long 			val;
	char				space[512];
	long				ret;

	for (val = 1; cp < cpmax; ) {
		if (_macaborted || (_macaborted = ed_abort(FALSE)) != 0)
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
				val = Test((COM_TEST*)cp);
				cp = COM1_INCR(cp,COM_TEST,size);
				continue;
			case C_BINOP:
				Binop((COM_BINOP*)cp);
				val = 1;
				cp = COM1_INCR(cp,COM_BINOP,size);
				continue;
			case C_CREATESYM:
				MakeInternalSym(((COM_CREATESYM*)cp)->name,
					((COM_CREATESYM*)cp)->symtype,
					((COM_CREATESYM*)cp)->value);
				cp = COM1_INCR(cp,COM_CREATESYM,size);
				val = 1;
				break;
			case C_ASSIGN:
				Assign(((COM_ASSIGN*)cp)->name,
					(COM_LONG1 *)COM1_INCR(cp,COM_ASSIGN,opoffset));
				cp = COM1_INCR(cp,COM_ASSIGN,size);
				val = 1;
				break;
			case C_STOP:
				return ((COM_STOP *)cp)->rc;
			case C_MACRO: case C_0FUNC: case C_1FUNC:
				val = do_macfunc(&cp,cpmax);
				ReturnFuncVal(C_LONG1PAR,val);
				continue;
			case C_FURET:
				_fncmarker = param_pop((unsigned char **)&cp);
				break;
			default:
				MacFormatErr();
				return 0;
		}
	}
	return val;
}

/*---------------------------------*/
/* EdMacroPlay()				*/
/*---------------------------------*/
int EdMacroPlay(int macroindex)
{
	int 			ret = 1;
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
		ed_error(IDS_MSGMACRORECURSION);
		return 0;
	}

	level++;

	if ((wasplaying = _playing) == 0 && _currfile) {
		u_init(_currfile);
# if defined(ATARI_ST)
		rdcash(-2);					/* init redraw cash */
# endif
	} 

	if (hadrecstate && macroindex >= 0) {
		push_sequence(C_MACRO,(long )_macrotab[macroindex]->name);
		_recording = 0;
	}

	_playing = 1;
	count = _multiplier;
	_multiplier = 1;
	for (i = 0; i < count; i++) {
		switch(macroindex) {
			case MAC_AUTO:	
				stopcash();
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
		if ((ret = do_seq(cp,cpmax)) <= 0) {
			ret = 0;
			break;
		}
	}

	if ((_playing = wasplaying) == 0) {
		_macaborted = 0;

# if defined(ATARI_ST)
		EdSelectWindow(_cfileno);
		rdupdate(0);
		changemouseform();
# else
		if (_currfile) {
			SelectWindow(WIPOI(_currfile)->win_id, FALSE);
		}
# if 0
		redrawallwi(0);
# endif
# endif
	}

	_recording = hadrecstate;

	level--;

	return ret;
}


