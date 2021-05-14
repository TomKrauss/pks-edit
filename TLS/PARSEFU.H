
#ifndef PARSEFU_H

#define MAXTYPES		64
#define MAXENUMS		300

#define MAXFUNCS		128
#define MAXCMDS		256
#define MAXICON		64
#define MAXMOUSE		64

struct llist {
	struct llist	*next;
	char			*name;
};

struct mp {
	char		*menu;
	char		*cmdname;
	struct    mp *next;
};

struct cmd {
	char *cmdname;
	char	*name;
	char	*par;
	struct cmd *next;
};

struct icdef {
	char *cmdname,*idname,*dropname,*flagname;
};

struct mousedef {
	char *cmdname;
	char *flagname;
	int  but,shift,click;
	int	cmdindex;
};

# include "funcdef.h"

typedef struct _assgn {
	char *name;
	char *comment;
	int  val;
} _ASSGN;

typedef struct _tagTYPEELEM {
	char *		te_name;
	long			te_val;
} _TYPEELEM;

typedef struct funcdef {
	char			*ident;
	char 		*name;
	char			*comment;
	struct  llist  *plist;
	signed char   	fpars[MAXPARS];
	unsigned char 	flags;
	unsigned char 	idx;
} FUNCDEF;

struct menudef {
	char *cmdname,*menu,*title;
};

#ifndef Extern
#define Extern		extern
#endif

Extern int _currvar,_currfunc,_currmenu,_currcmd,_curric,_currmouse,
		 _currtype,_currenelem;

Extern struct funcdef _allfuncs[MAXFUNCS];
Extern struct menudef _allmenus[MAXFUNCS];
Extern struct cmd     *_allcmd[MAXCMDS];
Extern unsigned char  _fdfunctab[64];

Extern OWNTYPE 		_typetab[MAXTYPES];
Extern _TYPEELEM		_localenelemtab[MAXENUMS];

Extern struct icdef	   _allicons[MAXICON];
Extern struct mousedef _allmice[MAXMOUSE];

Extern int    		  _nfdfuncs;

int printfuncs(char *fname);
int printcmdtable(char *fname);
int printfunctable(char *fname);
int printfuncdefines(char *name);
int printhelp(char *keys);

# define	PARSEFU_H

# endif

