/*
 * funcs.y
 *
 * generate command function table source files
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 07.02.1991
 *											
 */

%token MOUSE _ICON ID CHAR IDENT FORM 
%token _MENU FLAG KEY CMD STRING NUMBER DOTDOT
%token T_SHIFT T_CTRL T_ALT T_LEFT T_RIGHT T_MIDDLE
%token ENUM OPTION

%{

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <ctype.h>
# include "edfuncs.h"
# include "parsefu.h"
# include "mylib.h"
# include "stringutil.h"

# define	EO_ARRAY(a)			&a[DIM(a)]

	  FILE 		*yyfp;
	  int  		yylineno = 1;

static char 		yytext[512];
static int  		_inbrackets,_currfindex;
static signed char	*fpars = _allfuncs[0].fpars;
static int		_checktypes = 0;
static char		*Rename(char *s);

struct mcode {
	unsigned char	but,shift,click;
};

typedef union yystype {
	char 	*string;
	struct mp *mp;
	int		flags;
	char		c;
	struct cmd *cp;
	struct func {
		char	*name,*par;
	} f;
	long		number;
	struct mcode mousecode;
} _YYSTYPE;

extern void inskey(char *keystring, char *cmdname, int i);

static int parse_flags(char *s);
static int lookup_type(char *typename);
static int close_type(void);
static int new_type(int typ,char *s);
struct llist *make_list(struct llist* list,char *name);

#define alloc(len)	malloc(len)

#define YYSTYPE _YYSTYPE

%}

%%
script		: def
			| script def

def			: STRING '{' IDENT '(' param_list ')' STRING STRING '}'
			{
				if (_currfunc >= MAXFUNCS)
					yyerror("too many function definitions");

				_allfuncs[_currfunc].ident = Rename($1.string);
				_allfuncs[_currfunc].name  = $3.string;
				_allfuncs[_currfunc].plist = (struct llist*)$5.string;
				_allfuncs[_currfunc].flags = parse_flags($7.string);
				_allfuncs[_currfunc].comment = $8.string;
				_allfuncs[_currfunc].idx = _currfindex;
				_currfunc++;
				_currfindex++;
				fpars = _allfuncs[_currfunc].fpars;
			}
			| CMD '{' cmdseqlist '}'
			| _MENU IDENT '{' menulist '}'
			{	struct mp *mp = $4.mp;
	
				while (mp != 0) {
					if (_currmenu >= MAXFUNCS)
						yyerror("too many menu definitions");
	
					_allmenus[_currmenu].menu     = mp->menu;
					_allmenus[_currmenu].cmdname  = mp->cmdname;
					_allmenus[_currmenu].title    = $2.string;
					_currmenu++;
					mp = mp->next;
				}
			}
			| _ICON '{' iconlist '}'
			| MOUSE '{' mouselist '}'
			| typedef enumerators '}'
				{ close_type(); }

typedef		: ENUM IDENT '{' 
				{	new_type(OT_ENUM,$2.string);	}
			| OPTION IDENT '{' 
				{	new_type(OT_OPTION,$2.string);	}

enumerators	: 
			| enum enumerators

enum			: IDENT '=' NUMBER ','	{	
				if (_currenelem >= MAXENUMS) {
					yyerror("too many enumerators");
					return 0;
				}
				_localenelemtab[_currenelem].te_name = $1.string;
				_localenelemtab[_currenelem].te_val = $3.number;
				_currenelem++;
			}

param_list	: stringorident	
			{	$$.string = (char *)make_list((struct llist*)0,$1.string);
				lookup_type($1.string);
			}
			| param_list ','
			  stringorident
			{
				$$.string = (char *)make_list((struct llist*)$1.string,$3.string);	
				lookup_type($3.string);
			}

params		: '(' IDENT ')'	{ $$.string = $2.string; }

opt_dlg		: /**/			{ $$.string = 0; }
			| '(' IDENT ')'	{ $$.string = $2.string; }

opt_ident		: /**/			{ $$.string = 0; }
			| IDENT 			{ $$.string = $1.string; }

opt_string	: STRING { $$.string = $1.string; }
			| /* */  { $$.string = 0; }

stringorident	: IDENT			{ $$.string = $1.string; }
			| STRING			{ $$.string = $1.string; }

opt_stringorident	: stringorident { $$.string = $1.string; }
			| /* */  { $$.string = 0; }

menulist		: /* epsilon */	{ $$.mp = 0; }
			| stringorident '=' IDENT menulist
			{
				$$.mp = (struct mp *)alloc(sizeof (struct mp));
				$$.mp->next = $4.mp;
				$$.mp->menu = $1.string;
				$$.mp->cmdname = $3.string;
			}

iconlist		: /**/
			| IDENT IDENT IDENT IDENT iconlist
			{
				_allicons[_curric].cmdname = $1.string;
				_allicons[_curric].flagname = $2.string;
				_allicons[_curric].idname = $3.string;
				_allicons[_curric].dropname = $4.string;
				if (++_curric >= MAXICON) 
					yyerror("too many icon bindings");
			}

mouselist		: /**/
			| IDENT STRING mouse_key mouselist
			{
				_allmice[_currmouse].cmdname  = $1.string;
				_allmice[_currmouse].flagname = $2.string;
				_allmice[_currmouse].but   	= $3.mousecode.but;
				_allmice[_currmouse].shift 	= $3.mousecode.shift;
				_allmice[_currmouse].click 	= $3.mousecode.click;
				if (++_currmouse >= MAXMOUSE) 
					yyerror("too many mouse bindings");
			}

k_modifier	: T_SHIFT { $$.number = K_SHIFT; }
			| T_CTRL	{ $$.number = K_CONTROL; }
			| T_ALT	{ $$.number = K_ALTERNATE; }

kmod_list		:  {	$$.number = 0; }
			|  k_modifier '+' kmod_list { 
				$$.number = $1.number | $3.number; 
			}

m_button		: T_LEFT 	{ $$.number = MBUT_L; }
			| T_RIGHT	{ $$.number = MBUT_R; }
			| T_MIDDLE { $$.number = MBUT_M; }

mouse_buttons	:  {	$$.number = 0; }
			|  m_button '+' mouse_buttons { $$.number = $1.number | $3.number; }

mouse_key		: kmod_list mouse_buttons '(' NUMBER ')'
			{
				$$.mousecode.shift = (int)($1.number >> 8L);
				$$.mousecode.but   = $2.number;
				$$.mousecode.click = $4.number;
			}

cmdseqlist	: /**/
			| IDENT ':' cmdlist opt_string '=' 
			{
				hash_comment(_currcmd,$4.string);
				_allcmd[_currcmd] = $3.cp;
				_allcmd[_currcmd]->cmdname = $1.string;
			} key_list
			{
				if (++_currcmd >= MAXCMDS)
					yyerror("too many commands");
			} cmdseqlist

key_list		: KEY	
			{ inskey($1.string,_allcmd[_currcmd]->cmdname,_currcmd); }
			| KEY key_list
			{ inskey($1.string,_allcmd[_currcmd]->cmdname,_currcmd); }

cmdlist		: /* epsilon */	{ $$.cp = 0; }
			| fcall cmdlist
			{
				$$.cp = (struct cmd *)alloc(sizeof (struct cmd));
				$$.cp->cmdname = 0;
				$$.cp->next = $2.cp;
				$$.cp->name = $1.f.name;
				$$.cp->par  = $1.f.par;
			}

fcall		: IDENT '(' { _inbrackets =  1; } 
			  opt_stringorident { _inbrackets = 0; } ')' 
			{
				$$.f.name = $1.string;
				$$.f.par	= $4.string;
			}

key			: /* epsilon */	{ $$.string  = "";			}
			| KEY			{ $$.string  = $1.string;	}

%%

void yyerror(char *msg, ...) 
{
	va_list ap;
	char buf[128];
	extern int _excode;

	va_start(ap, msg);
	vsprintf(buf, msg, ap); 
	va_end(ap);
	fprintf(stderr,"%s in line %d\n",buf,yylineno);
	_excode = 1;
}

/*---------------------------------*/
/* yylex()					*/
/* handmade scanner				*/
/*---------------------------------*/
#define	YYGETC()	fgetc(yyfp)
#define	YLEOF	EOF
static int lookahead;
static int  yylen;

static iscident(int c) {
	return c == '-' || c == '|' || c == '_' || isalnum(c);
}

static int isnospace(int c) {
	return c != EOF && c != '\n' && c != '\r' && c != ' ' && c != '\t';
}

static int isnoquote(int c) {
	return c != '"';
}

static int isnoclose(int c) {
	return c != '>';
}

static int isnumber(int c) {
	return (c >= '0' && c <= '9') || c == 'x' || c == 'X';
}

static void yygets(int c, int (*f)(int c)) 
{
	yylen = 0;
	do {
		yytext[yylen] = c;
		yylen++;
		c = YYGETC();
	} while ((*f)(c));
	yytext[yylen] = 0;
	lookahead     = c;
}

#define S_MOUSE		1

int yylex(void )
{	int c;
	long base,sign = 1;
	char *estart,*s;
	static int state = 0;

	for (;;) {
		if (lookahead) {
			c = lookahead;
			lookahead = 0;
		} else c = YYGETC();
		if (c == YLEOF) return 0;
		if (c == '\n')  yylineno++;
		else switch (c) {
			case ' ': case '\r': case '\t':
				continue;
			case '}':
				state = 0;
			case ',': case '(': case ')': case ':': case ';':
			case '{': case '=': case '!': case '+':
			case '*':
				return c;
			case '#':
				while(c != '\n') {
					if ((c = YYGETC()) == YLEOF)
						return 0;
				}
				yylineno++;
				continue;
			case '.':
				if ((c = YYGETC()) == '.')
					return DOTDOT;
				lookahead = '.';
				break;
			case '-':
				if (_inbrackets)
					goto getid;
				c = YYGETC();
				if (!isnumber(c)) {
					lookahead = c;
					return DOTDOT;
				}
				sign = -1;
				/* drop through */
			case '0': 
			case '1': case '2': case '3': 
			case '4': case '5': case '6': 
			case '7': case '8': case '9':
				yygets(c,isnumber);
				if (_inbrackets)
					goto gotid;
				yylval.number = sign * strtol(yytext,(char **)0,0);
				return NUMBER;
			case '[':
				yygets(c,isnospace);
				yylval.string = stralloc(yytext);
				return KEY;
			case '<':
				yygets(c,isnoclose);
				yylval.string = stralloc(yytext+1);
				lookahead = 0;
				return STRING;
			case '"': 
				yygets(c,isnoquote);
				yylval.string = stralloc(yytext+1);
				lookahead = 0;
				return STRING;
			default:
				if (c == '-' || iscident(c)) {
getid:				yygets(c,iscident);
gotid:			
					if (state == S_MOUSE) {
						if (strcmp(yytext,"Alt") == 0)
							return T_ALT;
						if (strcmp(yytext,"Shift") == 0)
							return T_SHIFT;
						if (strcmp(yytext,"Ctrl") == 0)
							return T_CTRL;
						if (strcmp(yytext,"Right") == 0)
							return T_RIGHT;
						if (strcmp(yytext,"Middle") == 0)
							return T_MIDDLE;
						if (strcmp(yytext,"Left") == 0)
							return T_LEFT;
					}
					if (strcmp(yytext,"FLAG") == 0)
						return FLAG;
					if (strcmp(yytext,"MENU") == 0)
						return _MENU;
					if (strcmp(yytext,"ID") == 0)
						return ID;
					if (strcmp(yytext,"CMD") == 0)
						return CMD;
					if (strcmp(yytext,"ICON") == 0)
						return _ICON;
					if (strcmp(yytext,"enum") == 0)
						return ENUM;
					if (strcmp(yytext,"option") == 0)
						return OPTION;
					if (strcmp(yytext,"MOUSE") == 0) {
						state = S_MOUSE;
						return MOUSE;
					}
					yylval.string = stralloc(yytext);
					return IDENT;
				}
		}
	}
}

/*--------------------------------------------------------------------------
 * make_list()
 */
struct llist *make_list(struct llist* list,char *name)
{
	struct llist *lp,*lp2;

	for (lp2 = list; lp2 && lp2->next; lp2 = lp2->next)
		;

	if ((lp = malloc(sizeof *lp)) == 0 ||
	    (lp->name = stralloc(name)) == 0) {
		fprintf(stderr,"no mem\n");
		exit(1);
	}
	lp->next = 0;

	if (lp2) {
		lp2->next = lp;
		return list;
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * new_type()
 */
static int new_type(int typ,char *s)
{
	if (_currtype >= MAXTYPES) {
		yyerror("too many type definitions");
		return 0;
	}
	if (strcmp(s,"DIALOGOPT") == 0)
		_typetab[_currtype].ot_flags = OF_ELIPSIS;
	else
		_typetab[_currtype].ot_flags = 0;
	_typetab[_currtype].ot_typ = typ;
	_typetab[_currtype].ot_id = s;
	_typetab[_currtype].ot_idx = _currenelem;
	return 1;
}

/*--------------------------------------------------------------------------
 * close_type()
 */
static int close_type(void)
{
	_typetab[_currtype].ot_nelem = 
		_currenelem - _typetab[_currtype].ot_idx;
	return ++_currtype;
}

/*--------------------------------------------------------------------------
 * parse_flags()
 */
static int parse_flags(char *s)
{
	int flags = 0;

	while (*s) {
		switch (*s) {
			case 'C':	flags |= EW_NEEDSCURRF; break;
			case 'm':	flags |= EW_MODIFY; 	  break;
			case 'c':	flags |= EW_CCASH; 	  break;
			case 'B':	flags |= EW_NEEDSBLK;   break;
			case 'F':	flags |= EW_HASFORM; 	  break;
			case 'U': flags |= EW_UNDOFLSH;	  break;
			case 'N': flags |= EW_NOCASH;	  break;
			case 'M': flags |= EW_MULTI;	  break;
			case '*': _fdfunctab[_nfdfuncs++] = _currfunc; break;
			case '-': break;
			default : yyerror("invalid Flag %c",*s);
		}
		s++;
	}
	return flags;
}

/*--------------------------------------------------------------------------
 * find_type()
 */
static int find_type(char *typename)
{
	int i;

	for (i = 0; i < _currtype; i++) {
		if (strcmp(_typetab[i].ot_id,typename) == 0) {
			return PAR_USER + i;
		}
	}


	yyerror("illegal parameter type %s",typename);
	return PAR_VOID;
}

/*--------------------------------------------------------------------------
 * lookup_type()
 */
static int lookup_type(char *t)
{
	char typename[128];

	if (!_checktypes)
		return PAR_VOID;

	strcpy(typename,t);
	strtok(typename,":");

	if (strcmp(typename,"int") == 0 ||
	    strcmp(typename,"bool") == 0) {
		return *fpars++ = PAR_INT;
	}
	if (strcmp(typename,"string") == 0) {
		return *fpars++ = PAR_STRING;
	}
	return *fpars++ = find_type(typename);
}

/*---------------------------------*/
/* usage()					*/
/*---------------------------------*/
static char *_name;
static void usage(void )
{
	fprintf(stderr,
	"usage: %s [ -v ] [ -F FUNCDEFS ] [ -P PKSEDSRC ] [ -C CMDSRC ] [ DESCFILE ]\n",
			_name);
	exit(2);
}

/*--------------------------------------------------------------------------
 * Upcase()
 */
static char Upcase(char c)
{
	return (c >= 'a' && c <= 'z') ?
		(c - 'a' + 'A') : c;
}

/*--------------------------------------------------------------------------
 * Rename()
 */
static char *Rename(char *s)
{
	char *D = s, *d = s;

	while(*s) {
		*d++ = Upcase(*s++);
		while(*s && *s != '-') {
			*d++ = *s++;
		}
		if (*s) {
			s++;
		}
	}
	*d = 0;
	return D;
}

/*---------------------------------*/
/* main()						*/
/*---------------------------------*/
main(int ac, char **av)
{	int  i,c;
	int  printkomma = 0;
	char *cmdtable   = 0,
	     *pksedtable = 0,
		*helpfile   = 0,
		*definefile = 0,
		*helpstringfile = 0,
		*helpstrings = 0,
		*functable  = 0;
	yyfp = stdin;

	_name = (av[0][0]) ? av[0] : "funcs";

	while ((c = getopt(ac,av,"o:d:D:h:H:c:C:p:P:f:F:vs:S:")) != EOF) {
		switch(c) {
		case 'h':
		case 'H': _checktypes = 1; helpfile = optarg; break;
		case 'd':
		case 'D': definefile = optarg; break;
		case 'p':
		case 'P': pksedtable = optarg; break;
		case 'c':
		case 'C': cmdtable = optarg; break;
		case 'f':
		case 'F': _checktypes = 1; functable = optarg; break;
#ifdef YYDEBUG
		case 'v': yydebug  = 1; break;
#endif
		case 'o': printkomma = 1;
		case 's':
		case 'S': helpstringfile = optarg; break;
		default:
			usage();
		}
	}
	
	if (optind < ac) {
		if ((yyfp = fopen(av[optind],"r")) == 0) {
			yyerror("cannot open %s",av[optind]);
			return 1;
		}
	}
     setvbuf(yyfp,(char *) 0, _IOFBF, 8192);
	
	yyparse();

	if (cmdtable && printcmdtable(cmdtable))
		return 1;

	if (pksedtable && printfuncs(pksedtable))
		return 1;

	if (functable && printfunctable(functable))
		return 1;
	
	if (helpfile && printhelp(helpfile))
		return 1;

	if (definefile && printfuncdefines(definefile))
		return 1;

	if (helpstrings && printhelpstrings(helpstrings,1))
		return 1;

	if (helpstringfile && printhelpstrings(helpstringfile,printkomma))
		return 1;

	return 0;
}

