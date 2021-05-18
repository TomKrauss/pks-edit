
# ifndef	_EDFUNCS_H

#include <vcruntime.h>

#define	EW_MODIFY		0x1		/* function modifies text */
#define	EW_NOCASH		0x2		/* dont EdMacroRecord them */
#define	EW_NEEDSCURRF	0x4		/* needs a current file */
#define	EW_UNDOFLSH	0x8		/* flush undo buffer before */
#define	EW_NEEDSBLK	0x10		/* needs a current text block */
#define	EW_HASFORM	0x20		/* has a formula */
#define	EW_CCASH		0x40		/* put them into char buffer */
#define	EW_MULTI		0x80		/* has a Multiplier */

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

#define	K_SHIFT		0x300
#define	K_CONTROL		0x400
#define	K_ALTERNATE	0x800

#define	K_DELETED		0x1FFF		/* Impossible Key Combination	*/
#define	K_INVALID		K_DELETED

typedef struct keymap {
	KEYCODE		keycode;
	MACROREF		macref;
} KEYBIND;

KEYBIND *keygetbind(void);
KEYBIND *keybound(KEYCODE code);
char    *code2key(KEYCODE code);
KEYCODE mac_addshift(KEYCODE code);

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

int  param_space(unsigned char typ, char *s);
/*--------------------------------------------------------------------------
 * param_pop()
 * pop data from execution stack
 */
intptr_t param_pop(unsigned char** sp);

typedef struct c_1func {
	unsigned char  typ;		/* C_1FUNC */
	unsigned char  funcnum;	/* index in function table */
	long			p;
} COM_1FUNC;

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
	unsigned char op;		/* see binop.h */
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
	int	(*func)();
	unsigned char id;		/* for ref. */
	unsigned char flags;	/* see above */
} EDFUNC;

typedef struct macro {
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
typedef struct menu {
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
 * icon values used for the call...
 */
#define	IPCF_NONE				0
#define	IPCF_DROPCHAR			0x1
#define	IPCF_DROPHWND			0x2
#define	IPCF_DROPSTRING1		0x4
#define	IPCF_DROPSTRING2		0x8
#define	IPCF_DROPLONG1			0x10
#define	IPCF_DROPLONG2			0x20
#define	IPCF_SRCCHAR			0x100
#define	IPCF_SRCHWND			0x200
#define	IPCF_SRCSTRING1		0x400
#define	IPCF_SRCSTRING2		0x800
#define	IPCF_SRCLONG1			0x1000
#define	IPCF_SRCLONG2			0x2000
#define	IPCF_USERDEF			0x4000

typedef struct iconbind {
	unsigned char id,dropped;	/* icon types - src and dest */
	int pflags;				/* paramter construction flags s. above */
	int index;				/* points to cmdseq */
} ICONBIND;

/*
 * MOUSEBUTTONS -----------------------------------------------------------
 */

#define	MBUT_L	0x1
#define	MBUT_R	0x2
#define	MBUT_M	0x4
#define	MBUT_RL	(MBUT_L|MBUT_R)

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
	unsigned char 	flags;				/* flags s.b. */
	MACROREF		macref;
	char *		msg;					/* optional message */
} MOUSEBIND;

#define	MO_FINDCURS		0x1

/*
 *  ALL BINDING INFORMATIONS ----------------------------------------------
 */
typedef struct edbinds {
	EDFUNC		*ep;
	COM_1FUNC		*cp;
	MACRO		**macp;
	KEYBIND		*kp;
	MENUBIND		*mp;
	ICONBIND		*ip;
	MOUSEBIND		*mop;
	int			*nep,*ncp,*nmacp,*nkp,*nmp,*nip,*nmop;
} EDBINDS;

extern EDFUNC		_edfunctab[];
extern COM_1FUNC	_cmdseqtab[];
extern KEYBIND		_keymaptab[MAXMAPKEY];
extern MENUBIND	_menutab[];
extern ICONBIND	_ictab[];
extern MOUSEBIND	_mousetab[];

extern EDBINDS		_bindings;

extern int		 _nfuncs,_ncmdseq,
				 _nkeys,_nmenus,_nicbind,_nmousebind,
				 _ncb_do_char,_lcomseq;

extern char		_recorder[];
extern unsigned char _cfdisable[],_blkdisable[],_rdodisable[];
extern KEYBIND		_cmdorig[];

#define	IDM_CMDNAME		1024
#define	IDM_CMDCOMMENT		1270
#define	IDM_LOWFUNCNAME	2048
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

extern int param_dialopen(PARAMS *p);
extern int cdecl do_func(int num, long p1, long p2, void *s1, void *s2, void *s3);

#define	_EDFUNCS_H
# endif

