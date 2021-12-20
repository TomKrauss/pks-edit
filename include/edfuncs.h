/*
 * edfuncs.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: macro language related definitions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef	_EDFUNCS_H

#include <windows.h>

#define	BIN_ADD			'+'
#define	BIN_AND			'&'
#define	BIN_OR			'|'
#define	BIN_MUL			'*'
#define	BIN_DIV			'/'
#define	BIN_MOD			'%'
#define	BIN_OR			'|'
#define	BIN_AND			'&'
#define	BIN_BRACKETS		'('
#define	BIN_SUB			'-'
#define	BIN_NOT			'~'
#define	BIN_XOR			'^'
#define	BIN_CONVERT		'C'

#define	EW_MODIFY		0x1		/* function modifies text */
#define	EW_NOCASH		0x2		/* dont EdMacroRecord them */
#define	EW_NEEDSCURRF	0x4		/* needs a current file */
#define	EW_UNDOFLSH		0x8		/* flush undo buffer before */
#define	EW_NEEDSBLK		0x10		/* needs a current text block */
#define	EW_HASFORM		0x20		/* has a formula */
#define	EW_CCASH		0x40		/* put them into char buffer */
#define	EW_MULTI		0x80		/* has a Multiplier */
#define EW_UNDO_AVAILABLE 0x100	/* undo must be possible */
#define EW_REDO_AVAILABLE 0x200	/* redo must be possible */
#define	EW_FINDCURS		0x400		/* applies to command executed using the mouse: before executing the command position the caret to the clicked point */
#define	EW_COMPARISON_MODE	0x800		// only available in comparison mode

#define	MAXMOUSEBIND	32
#define	MAXMAPMOUSE	MAXMOUSEBIND
#define	MAXMAPKEY		200
#define	MAXMACRO		64

#ifndef DIM
#define	DIM(tab)		(sizeof(tab)/sizeof(tab[0]))
#endif

#define	RECORDERSPACE	512

/* "macro reference" types */
#define	CMD_NONE		0
#define	CMD_MENU		0x1
#define	CMD_CMDSEQ	0x2
#define	CMD_MACRO		0x3
#define	CMD_BUFFER	0x4
#define	CMD_KILLKEY	0x5

typedef unsigned char 	MACROREFTYPE;
typedef unsigned char  	MACROREFIDX;
typedef struct tagMACROREF {
	MACROREFTYPE	typ;
	MACROREFIDX	index;
} MACROREF;

/*
 * KEYS ---------------------------------------------------------------
 */

typedef unsigned short KEYCODE;

/* ASCII representation */
#define ALT_MARK		'~'
#define CTL_MARK		'^'
#define SFT_MARK		'#'
#define SELECTED_MARK	'*'

#define	K_SHIFT		0x300
#define	K_CONTROL		0x400
#define	K_ALTERNATE	0x800
#define	K_HAS_SELECTION 0x1000

#define	K_DELETED		0x1FFF		/* Impossible Key Combination	*/
#define	K_INVALID		K_DELETED

typedef struct tagKEYBIND {
	KEYCODE		keycode;
	MACROREF	macref;
} KEYBIND;

KEYBIND *keygetbind(void);
KEYBIND *keybound(KEYCODE code);
char    *code2key(KEYCODE code);
KEYCODE macro_addModifierKeys(KEYCODE code);

/*
 * MACROS -----------------------------------------------------------
 */

/*

 typical seq. in the cmd. BUF:
 C_LOOP multiplier CMC_{MENU,CMDSEQ} funcnum {par} C_STR1PAR string ...
 all entities padded to ensure int alignment !!

 */

#define	C_STOP		0	/* eof sequence */
#define	C_0FUNC		0x1	/* Function # (char) */
#define	C_1FUNC		0x2	/* Function # (char) + 1 int Param */
#define	C_MACRO		0x3	/* macro "macroname" */
#define	C_GOTO		0x4	/* (conditionally) goto offset */
#define	C_TEST		0x6	/* Test: testop p1 p2 */
#define	C_BINOP		0x7	/* binary operation: binop a b */
#define	C_ASSIGN		0x8	/* assign: a = stackval */
#define	C_CREATESYM	0x9	/* create symbol with type and value */

#define	C_CHAR1PAR	0x10 /* 1 Ascii character follows */
#define	C_STRING1PAR	0x11	/* 1 string Asciistring\0 follows {pad} */
#define	C_INT1PAR		0x12 /* pad, 1 int Parameter follows */
#define	C_LONG1PAR	0x13 /* pad, 1 long Parameter follows */
#define	C_STRINGVAR	0x14	/* variable reference to string */
#define	C_LONGVAR		0x15	/* variable reference to long value */
#define	C_FORMSTART	0x16	/* formular with parameters ... */
#define	C_DATA		0x17	/* any data ... */
#define	C_FURET		0x18 /* next function return is saved */

#define	C_IS1PAR(typ)	(typ & 0x10)
#define	C_ISCMD(typ)	(typ >= C_0FUNC && typ <= C_MACRO)

#define	C_NONE			0xFF

#define	FORM_SHOW		0x40	/* form should be opened */
#define	FORM_INIT		0x1	/* form " and be prefilled */
#define	FORM_REDRAW	0x2	/* force redraw */

int  macro_getParameterSize(unsigned char typ, const char *s);
/*--------------------------------------------------------------------------
 * macro_popParameter()
 * pop data from execution stack
 */
intptr_t macro_popParameter(unsigned char** sp);

typedef struct c_1func {
	unsigned char  typ;		/* C_1FUNC or C_0FUNC - defines the number of parameters to pass */
	unsigned char  funcnum;	/* index into editor function table _edfunctab */
	long			p;		/* optional parameter to pass */
} COM_1FUNC;

/*
 * Describes an editor command. 
 */
typedef struct tagCOMMAND {
	int		  c_index;					// the command index used internally.
	COM_1FUNC c_functionDef;			// the actual functionto execute including flags.
	char* c_name;						// name of the command used in macros
} COMMAND;

typedef struct c_0func {
	unsigned char typ;		/* C_0FUNC */
	unsigned char funcnum;	/* index in function table */
} COM_0FUNC;

typedef struct c_ident {
	unsigned char typ;		/* C_MACRO */
	unsigned char name[1];	/* 0-term. string padded to even # */
} COM_MAC;

typedef struct c_ident COM_VAR;

typedef struct c_assign {
	unsigned char 	typ;		/* C_ASSIGN */
	unsigned char 	res;
	int		    	opoffset;	/* source operand offset */
	int		    	size;	/* size of total assignment */
	unsigned char	name[1];	/* variable name */
						/* operand (src) follows */
} COM_ASSIGN;

typedef struct c_createsym {
	unsigned char 	typ;		/* C_CREATESYM */
	unsigned char 	symtype;
	long			value;	/* value */
	int		    	size;	/* size of total structure */
	unsigned char	name[1];	/* variable name */
} COM_CREATESYM;

#define	BRA_ALWAYS		0
#define	BRA_NE			1
#define	BRA_EQ			2

typedef struct c_goto {
	unsigned char typ;		/* C_GOTO, C_GOCOND */
	unsigned char bratyp;	/* BRA_ALWAYS, BRA_NE, BRA_EQ, */
	int		    offset;
} COM_GOTO;

typedef struct c_test {
	unsigned char typ;		/* C_TEST */
	unsigned char testop;	/* see test.h */
	int		    p2offset;
	int		    size;
} COM_TEST;

typedef struct c_binop {
	unsigned char typ;		/* C_BINOP */
	unsigned char op;		/* see above BIN_ADD etc... */
	int           op1offset;	/* points to COM_1STRING,.... */
	int           op2offset;	/* points to COM_1STRING,.... */
	int		    size;		/* total size of binop */
	unsigned char result[1]; /* result variable name */
} COM_BINOP;

typedef struct c_char1 {
	unsigned char typ;		/* C_CHAR1PAR */
	unsigned char val;
} COM_CHAR1;

typedef struct c_char1 COM_FURET;

typedef struct c_stop {
	unsigned char typ;
	unsigned char rc;
} COM_STOP;

typedef struct c_int1 {
	unsigned char typ;		/* CMD_INT1PAR */
	unsigned char res;
	int		    val;
} COM_INT1;

typedef struct c_long1 {
	unsigned char typ;		/* CMD_LONG1PAR */
	unsigned char res;
	intptr_t  	  val;
} COM_LONG1;

typedef struct c_string1 {
	unsigned char typ;		/* C_STRING1 */
	unsigned char s[1];		/* 0-term. string padded to even # */
} COM_STRING1;

typedef struct c_anydata {
	unsigned char typ;		/* C_DATA */
	unsigned char subtype;
	unsigned long size;		/* sizeof data */
	char		    data[1];	/* ... */
} COM_DATA;

typedef struct c_form {
	unsigned char typ;		/* CMD_FORMSTART */
	unsigned char options;	/* FORM_SHOW,... */
	int		    nfields;	/* # of fields in formular */
} COM_FORM;

typedef union c_seq {
	COM_1FUNC		*fu1;
	COM_0FUNC		*fu0;
	COM_MAC		*ma;
	COM_GOTO		*go;
	COM_FORM		*fo;
	COM_STRING1	*st1;
	COM_LONG1		*lo1;
	COM_INT1		*in1;
	COM_CHAR1		*ch1;
	unsigned char  *sp;
} COM_SEQ;

typedef struct edfunc {
	int	(*execute)();		// the actual callback to invoke 
	unsigned char id;		// logical id for referencing it 
	int		 flags;			// see EW_... flags above
	const char* f_name;		// the name as it can be used inside the PKS Edit macro language to execute this function.
} EDFUNC;

typedef struct tagMACRO {
	unsigned char namelen;
	unsigned char dstart;
	unsigned 	    size;
	unsigned char name[2];
} MACRO;

#define	MAC_COMMENTLEN		220
#define	MAC_NAMELEN		32
#define	MAC_NAME(mp)		((mp)->name)
#define	MAC_SIZE(size)		((size) + sizeof(MACRO))
#define	MAC_DATA(mp)		((unsigned char *)mp+mp->dstart)
#define	MAC_COMMENT(mp)	((unsigned char *)mp+sizeof *mp+mp->namelen)

/*
 * MENUS ---------------------------------------------------------------
 */
typedef struct tagMENUBIND {
	/* unsigned int due too submenu high bit marks !!!! */
	unsigned 	     menunum;			/* ref. to menutree */
#if !defined(_Windows) && !defined(WIN32)
	unsigned char  mtitlenum;		/* the title */
#endif
	unsigned 		index;			/* points to cmdseqtab */
} MENUBIND;

#define	MAX_MENU_STRING	30

#define	UM_ITEM		MF_STRING		/* "normal" item */
#define	UM_SEPARATOR	MF_SEPARATOR	/* --------- */
#define	UM_POPUP		MF_POPUP		/* -> popup */
#define	UM_ENDPOPUP	-1			/* terminates popup in linear menu chain */
#define	UM_ENDMENU	-2

typedef struct tagUSERMENUBIND {
	int				type;
	MACROREF		macref;
	void*			handle;			/* submenu handle, or item identifier */
	char			szString[MAX_MENU_STRING];
} USERMENUBIND, * PUSERMENUBIND;

/*
 * ICONS ---------------------------------------------------------------
 */

/*
 * MOUSEBUTTONS -----------------------------------------------------------
 */

#define	MBUT_L	0x1
#define	MBUT_R	0x2
#define	MBUT_M	0x4
#define	MBUT_RL	(MBUT_L|MBUT_R)

#define M_CONTROL	0x4
#define M_SHIFT		0x2
#define M_ALT		0x8

typedef struct tagMOUSECODE {
	unsigned	button  : 4,
			nclicks : 4,			/* # of button clicks */
			shift   : 8;			/* kb-state */
} MOUSECODE;

#if defined (MACROS20)
typedef struct mousebind {
	MOUSECODE		mousecode;
	MACROREF		macref;
} MOUSEBIND;
# endif

typedef struct mousebind {
	char 		button;				/* mousebutton */
	char 		shift;				/* kb-state */
	char 		nclicks;				/* # of button clicks */
	unsigned char 	unused;				/* currently unused */
	MACROREF		macref;
	char *		msg;					/* optional message */
} MOUSEBIND;

/*
 *  ALL BINDING INFORMATIONS ----------------------------------------------
 */
typedef struct edbinds {
	EDFUNC		*ep;
	COMMAND		*cp;
	MACRO		**macp;
	KEYBIND		*kp;
	MENUBIND		*mp;
	MOUSEBIND		*mop;
	int			*nep,*ncp,*nmacp,*nkp,*nmp,*nip,*nmop;
} EDBINDS;

extern EDFUNC		_edfunctab[];
extern COMMAND		_cmdseqtab[];
extern KEYBIND		_keymaptab[MAXMAPKEY];
extern MENUBIND	_menutab[];
extern MOUSEBIND	_mousetab[];

extern EDBINDS		_bindings;

extern int		 _nfuncs,_ncmdseq,
				 _nkeys,_nmenus,_nmousebind,
				 _ncb_do_char,_lcomseq;

extern char		_recorder[];
extern unsigned char _cfdisable[],_blkdisable[],_rdodisable[];
extern KEYBIND		_cmdorig[];

#define	IDM_CMDNAME		1024
#define	IDM_LOWENUMNAME	2512

/*
 *  PARAMETERS (RECORDING DIALOGS) ---------------------------------------
 */
#define	P_MAYOPEN				0x1
#define	P_MAYPRE				0x2

typedef struct des {
	char		cmd_type;
	union {
		unsigned char  *s;
		char			*c;
		unsigned int	*i;
		long			*l;
	} p;
} ITEMS[];

typedef struct params {
	int		nel;
	int		flags;
	struct	des *el;
} PARAMS;

typedef KEYBIND* (*KEYBINDING_FUNCTION)(WPARAM key);
typedef MOUSEBIND* (*MOUSE_BINDING_FUNCTION)(int nButton, int nModifiers, int ncClicks);

/*
 * A controller provides key and mouse bindings (for now) and can be used as a delegate 
 * to customize key / mousebindings per delegate (etc. per view).
 */
typedef struct tagCONTROLLER {
	const KEYBINDING_FUNCTION c_getKeyBinding;
	const MOUSE_BINDING_FUNCTION c_getMouseBinding;
} CONTROLLER;

typedef struct tagCOMPILER_CONFIGURATION {
	int (*cb_insertNewMacro)(char* name, char* comment, char* macdata, int size);
	void  (*cb_showStatus)(char* s, ...);
	BOOL cb_openErrorList;
	char* cb_source;
} COMPILER_CONFIGURATION;

extern COMPILER_CONFIGURATION* _compilerConfiguration;

extern int macro_openDialog(PARAMS *p);
extern int cdecl macro_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3);

/*------------------------------------------------------------
 * macro_getByIndex()
 * Return a macro by its internal index.
 */
extern MACRO* macro_getByIndex(int idx);

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
extern int macro_getInternalIndexByName(char* name);

extern int 		macro_saveMacrosAndDisplay(char* macroname);
extern int 		macro_saveMouseBindingsAndDisplay(void);
extern int 		macro_saveKeyBindingsAndDisplay(void);
extern int 		macro_saveMenuBindingsAndDisplay(void);
/*
 * Returns FALSE; if the function described by the function pointer cannot
 * be executed.
 */
extern int macro_isFunctionEnabled(EDFUNC* fup, int warn);

/*------------------------------------------------------------
 * macro_autosaveAllBindings()
 * Save all changed macro bindings and new macros to the corresponding file.
 * If warnflag is specified ask user, whether macros should be saved.
 */
extern void macro_autosaveAllBindings(int warnFlag);

/*------------------------------------------------------------
 * macro_deleteByName()
 * Delete the macro (including all its bindings) with the given name. Return
 * 1 if successfully deleted.
 */
extern int macro_deleteByName(char* name);

/*------------------------------------------------------------
 * macro_validateMacroName()
 * Check, whether a mcro name is valid and whether it exists already.
 */
extern int macro_validateMacroName(char* name, int origidx, int bOverride);

/*--------------------------------------------------------------------------
 * macro_onKeybindingChanged()
 * invoked, when a keybinding has changed. May flag e.g. the function keyboard of PKSEDIT
 * as dirty.
 */
extern void macro_onKeybindingChanged(KEYCODE key);

/*------------------------------------------------------------
 * macro_bindOrUnbindKey()
 * index < 0:	delete key-binding
 * index >=0:  add key-binding
 */
extern int macro_bindOrUnbindKey(KEYCODE key, int index, MACROREFTYPE typ);

/*--------------------------------------------------------------------------
 * macro_selectDefaultBindings()
 * Select the default key- / menu- / mouse bindings for PKS Edit.
 */
extern void macro_selectDefaultBindings(void);

/*------------------------------------------------------------
 * macro_readBindingsFromFile()
 * Read new bindings from a file. If the current bindings are "dirty" they
 * are flushed before.
 */
extern int macro_readBindingsFromFile(char* fn);

/*------------------------------------------------------------
 * macro_createWithParams()
 * create a new macro with a name, a comment and the actual byte codes
 * to execute.
 */
extern MACRO* macro_createWithParams(char* szName, char* szComment, char* bData, int size);

/*------------------------------------------------------------
 * macro_renameAndChangeComment()
 */
extern void macro_renameAndChangeComment(int nIndex, char* szName, char* szComment);

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
extern int macro_insertNewMacro(char* name, char* comment, char* macdata, int size);

/*------------------------------------------------------------
 * macro_toggleRecordMaco()
 * start/stops the macro recorder.
 */
extern int macro_toggleRecordMaco(void);

/*--------------------------------------------------------------------------
 * macro_getMacroIndexForMenu()
 * Return the macro reference given a menu id.
 */
extern MACROREF* macro_getMacroIndexForMenu(int nId);

/*--------------------------------------------------------------------------
 * macro_translateToOriginalMenuIndex()
 * try to find an internal command/standard menu binding, which
 * allows us to display an appropriate help for synthetic menus
 */
extern WORD macro_translateToOriginalMenuIndex(WORD wParam);

/*---------------------------------
 * macro_onMenuAction()
 * Invoke the macro bound to a menu index.
 *---------------------------------*/
extern int macro_onMenuAction(int menunum);

/*---------------------------------
 * macro_addModifierKeys()
 * Add the modifier key bits depending on whether
 * Shift, Control or Alt was pressed together with
 * the key passed as an argument.
 */
extern KEYCODE macro_addModifierKeys(KEYCODE key);

/*---------------------------------*/
/* macro_getKeyBinding()				*/
/*---------------------------------*/
extern void* macro_getKeyBinding(WPARAM key);

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
extern int macro_executeMacro(MACROREF* mp);

/*---------------------------------*
 * macro_onKeyPressed()
 * Execute a keybinding and return 1 if successful.
 *---------------------------------*/
extern int macro_onKeyPressed(void* keybind);

/*------------------------------------------------------------
 * macro_onCharacterInserted()
 */
extern int macro_onCharacterInserted(WORD c);

/*---------------------------------*/
/* macro_executeByName()				*/
/*---------------------------------*/
extern int macro_executeByName(char* name);

/*------------------------------------------------------------
 * macro_assignAcceleratorTextOnMenu()
 * this function sets the menu accelerator text, each time a
 * menu popup is opened
 */
extern void macro_assignAcceleratorTextOnMenu(HMENU hMenu);

/*
 * Returns the keyboard binding text for a given internal command.
 */
extern char* macro_getKeyText(int nCmd);

/*------------------------------------------------------------
 * macro_getComment()
 * Returns the command for a given macro index and type.
 * The result is copied into the space passed with szBuf.
 */
extern char* macro_getComment(char* szBuf, char* szB2, int nIndex, int type);

/*
 * Returns a tool tip for a given menu id.
 */
extern char* macro_getMenuTooltTip(int dMenuId);

/*--------------------------------------------------------------------------
 * macro_showHelpForMenu()
 * Display help for a given menu id
 */
extern void macro_showHelpForMenu(int dMenuId);

/*--------------------------------------------------------------------------
 * macro_getCmdIndexByName()
 */
extern int macro_getCmdIndexByName(char* name);

/*
 * Returns a macro by a given index or NULL if the index does not
 * lie in the valid index range. Can be used to iterate all macros.
 */
extern char* macro_getCommandByIndex(int nIndex);

/**
* Execute a macro given a single line text to execute.
* pszCode ist the string of code to execute. If bUnescape is true, we treat \ and " special
* and assume, the command to execute was passed on the command line (and will be escaped).
* 'pszContext' is the name of the context in which the execution will be performed.
 */
extern int macro_executeSingleLineMacro(const char* pszCode, BOOL bUnescape, const char* pszContext);

/*--------------------------------------------------------------------------
 * macro_getIndexForKeycode()
 * Return the internall index for a given macro keycode and name.
 */
extern int macro_getIndexForKeycode(KEYCODE* scan, char* name, int oldidx);

/**
 * macro_returnString()
 * Return the passed String to the macro interpreter so it can be used for further processing.
 */
extern void macro_returnString(char* string);

/*---------------------------------*/
/* macro_recordOperation()				*/
/*---------------------------------*/
extern int macro_recordOperation(PARAMS* pp);

extern void macro_stopRecordingFunctions();

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
extern int macro_isParameterStringType(unsigned char typ);

/*
 * Returns a custom controller with custom mouse and keybindings used in search lists.
 */
extern CONTROLLER* macro_getSearchListController();


#define	_EDFUNCS_H
#endif

