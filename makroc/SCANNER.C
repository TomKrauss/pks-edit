#include <stdio.h>
# define U(x) x
#ifdef TURBO
int yyback(int *p,int m);
int yylook(void );
int yywrap(void );
#endif /* TURBO */

# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
static FILE *yyin;
static FILE *yyout;
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*
 * SCANNER.L
 *
 * Scanner
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 05.02.1991
 *									
 */

# include	<stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>
# include	"sym.h"
# include	"edfuncs.h"
# include	"scanner.h"
# include "parser.h"
# include	"editab.h"
# include "pkscc.h"

# undef  input
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):cmdgetc()))==EOF?\
					0:yytchar)

typedef struct yyerrstruct {
	char		*srcname;
	char		errname[84];
	FILE		*errfp;
	int		yylineno;
	int		yynerr,yynwarn;
	int		yymaxerr,yymaxwarn;
	jmp_buf	*failpt;
} YYERRSTRUCT;

static YYERRSTRUCT	yyerr;
int	  			yyerrflg;
int				_bDefiningConst;

typedef void EDFUNCDEF;

extern unsigned char* stralloc(unsigned char* buf);
extern EDFUNCDEF 	*Name2Findex(char *name);
extern FILE		*createtmp(char *fnd, char *fn);
extern int 		init_funcs(void);
extern long 		EnumValue(void *enp);
extern void 		mac_switchtodefaulttables(void);
extern void 		stepselectcompiler(char *pszName);

#if defined(STAND_ALONE)
static FILE *_lexFp;
#endif

/*---------------------------------*/
/* yyerror()					*/
/*---------------------------------*/
void yyerror(char *s, ...)
{	char buf[512];
	va_list	ap;

	if (!yyerr.srcname)
		yyerr.srcname = "STDIN";

	if ( yyerr.errfp != 0 || 
	    (yyerr.errfp = createtmp(yyerr.errname,"PKSMAKRO.ERR")) != 0) {
		yyerrflg = 1;
		sprintf(buf,"Error %s %d: %s\n",yyerr.srcname,yyerr.yylineno,s);
	     va_start(ap, s);
		vfprintf(yyerr.errfp,buf,ap);
	     va_end(ap);
		fflush(yyerr.errfp);
		if (++yyerr.yynerr >= yyerr.yymaxerr) {
			fprintf(yyerr.errfp,"\n*** too many errors ***\n");
			if (yyerr.failpt)
				longjmp(*yyerr.failpt,ERR_TOOMANYERR);
		}
	}
}

/*---------------------------------*/
/* keyword()					*/
/*---------------------------------*/
static struct kw {
	int			toknum;
	char  *		name;
} keywords[] = {
	T_DEFMACRO,	"macro",
	T_MENUPOPUP,	"POPUP",
	T_DEFMENUS,	"MENU",
	T_DEFICON,	"ICONS",
	T_CONST,		"const",
	T_DEFALTPATH,	"ALTPATH",
	T_DEFCOMPILER,	"COMPILER",
	T_DEFDOCTYPES,	"DOCTYPES",
	T_DEFKEYS,	"KEYS",
	T_DEFMOUSE,	"MOUSE",
	T_DEFTAGS,	"TAGS",
	T_DEFABBREVS,	"ABBREVS",
	T_DEFBRACKETS,	"BRACKETS",
	T_DEFESCAPES,	"TEXTBUFFER",
	T_IF,		"if",
	T_ELSE,		"else",
	T_ELIF,		"elif",
	T_SWITCH,		"switch",
	T_WHILE,		"while",
	T_CONTINUE,	"continue",
	T_BREAK,		"break",
	T_RETURN,		"return",
	T_LOCAL,		"local",
	T_GOTO,		"goto",
	T_BRAEQ,		"braeq",
	T_TCHAR,		"char",
	T_TINT,		"int",
	T_TLONG,		"long",
	T_TSTRING,	"string",
	-1,			0
};

char *tok2sym(int token)
{	struct kw *kp = keywords;

	while(kp->name) {
		if (kp->toknum == token)
			return kp->name;
		kp++;
	}
	return (char*)0;
}

static int init_keywords(void)
{	struct kw *kp = keywords;

	while(kp->name) {
		if (!sym_insert(kp->name,S_KEYWORD,kp->toknum))
			return 0;
		kp++;
	}
	return 1;
}

#if defined(STAND_ALONE)
/*---------------------------------*/
/* cmdgetc()					*/
/*---------------------------------*/
static int cmdgetc(void )
{	
	int	c;

	if (yyleng > YYLMAX-4) {
		yyerror("token too long");
		yyleng = 0;
	}

	if ((c = fgetc(_lexFp)) == '\n')
		yyerr.yylineno++;
	return c;
}

# else
static unsigned char 	*_lexStart,*_lexEnd;
static LINE			*_lexLine,*_lexLastLine;
/*---------------------------------*/
/* LexInit()					*/
/*---------------------------------*/
static void LexInit(LINE *lps, LINE *lpe)
{
	_lexLine  	= lps;
	_lexLastLine 	= lpe;
	_lexStart 	= lps->lbuf;
	_lexEnd   	= &_lexStart[lps->len];
}

/*---------------------------------*/
/* cmdgetc()					*/
/*---------------------------------*/
static int cmdgetc(void )
{	LINE *lp;

	if (yyleng > YYLMAX-4) {
		yyerror("token too long");
		yyleng = 0;
	}

	if (_lexStart >= _lexEnd) {
		if (_lexLine != _lexLastLine &&
		    (lp = _lexLine->next) != 0 && 
		    lp->next != 0) {
			yyerr.yylineno++;
			LexInit(lp,_lexLastLine);
			return '\n';
		}
		return EOF;
	}
	return *_lexStart++;
}

# endif

/*---------------------------------*/
/* yyinit()					*/
/*---------------------------------*/
int yyinit(jmp_buf *errb, char *sourcefile, LINE *lps, LINE *lpe)
{
	static tables_inited;
	yyin = stdin;
	yyout = stdout;

	if (!tables_inited) {
		if (hash_create(HSIZE) == 0 ||
		    !init_keywords()	   ||
		    !init_funcs()) {
			return 0;
		}
		tables_inited = 1;
	}

	yyerr.yynwarn  = 0;
	yyerr.yynerr	= 0;
	yyerr.yymaxerr	= 30;
	yyerr.yymaxwarn= 30;
	yyerr.yylineno = 1;
	yyerr.failpt   = errb;
	if (yyerr.errfp) {
		fclose(yyerr.errfp);
		yyerr.errfp = 0;
	}
	yyerr.srcname  = sourcefile;

#if defined(STAND_ALONE)
	if ((_lexFp = fopen(sourcefile,"r")) == 0) {
		yyerror("cannot open \"%s\"\n",sourcefile);
		return 0;
	}
# else
	LexInit(lps,lpe);
# endif
	return 1;
}

/*---------------------------------*/
/* yyfinish()					*/
/*---------------------------------*/
int yyfinish(void)
{
#if defined(STAND_ALONE)
	if (_lexFp) {
		fclose(_lexFp);
		_lexFp = 0;
	}
#endif

	mac_setmenukeys();
	mac_switchtodefaulttables();
	if (yyerr.errfp) {
		fprintf(yyerr.errfp,"\nTOTAL %d Errors\n",yyerr.yynerr);
		fclose(yyerr.errfp);
		yyerr.errfp = 0;
		yyerr.failpt = 0;
		stepselectcompiler("PKSMAKROC");
		stepnofsel(yyerr.errname,1);
		return 0;
	} else {
		protokoll("No Errors detected");
	}
	return 1;
}

/*---------------------------------*/
/* yystralloc()				*/
/*---------------------------------*/
unsigned char *yystralloc(unsigned char *s)
{	unsigned char *d;

	if ((d = stralloc(s)) == 0) {
		if (yyerr.failpt)
			longjmp(*yyerr.failpt,ERR_SPACE);
		return 0;
	}
	return d;
}

/*---------------------------------*/
/* unquotealloc()				*/
/*---------------------------------*/
static char *unquotealloc(char *name, int len)
{	char qbuf[256],*d,*dend,*send;
	int  cnt,i;

	for (d = qbuf, dend= &qbuf[sizeof qbuf-2], send = &name[len]; 	
		name < send; name++) {
		if (d >= dend) {
			yyerror("unterminated string");
			break;
		}
		if (*name == '\\') {
			switch(name[1]) {
				case 'n': *d++ = '\n'; break;
				case 'r': *d++ = '\r'; break;
				case 't': *d++ = '\t'; break;
				case '0': 
					for (i = 0, cnt = 0; cnt < 3; cnt++) {
						if (name[2] >= '0' && name[2] <= '7') {
							i = 8*i + (name[2] - '0');
							name++;
						}
					}
					*d++ = i;
					break;
				default:
					*d++ = name[1];
					break;
			}
			name++;
		} else {
			*d++ = *name;
		}
	}
	*d++ = 0;
	i = d-qbuf;
	if ((d = _alloc(i)) == 0) {
		if (yyerr.failpt)
			longjmp(*yyerr.failpt,ERR_SPACE);
		return 0;
	}
	memcpy(d,qbuf,i);
	return d;
}


/*------------------------------------------------------------
 * number()
 */
long number(char *s)
{
	long  sign = 1;

	if (*s == '+' || *s == '\\')
		s++;
	else if (*s == '-') {
		sign = -1;
		s++;
	} 
	return sign * strtol(s,(char **)0,0);
}

/*---------------------------------*/
/* yywrap()					*/
/*---------------------------------*/
int yywrap(void )
{
	return 1;
}

# define keys 2
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
{
				return '-';
			}
break;
case 2:
{
				if (strcmp(yytext,"Alt") == 0)
					return T_ALT;
				if (strcmp(yytext,"Ctrl") == 0)
					return T_CTRL;
				if (strcmp(yytext,"Shift") == 0)
					return T_SHIFT;
				if (strcmp(yytext,"MLeft") == 0)
					return T_LEFT;
				if (strcmp(yytext,"MRight") == 0)
					return T_RIGHT;
				if (strcmp(yytext,"MMiddle") == 0)
					return T_MIDDLE;
				yylval.s = yystralloc(yytext);
				return T_IDENT;
			}
break;
case 3:
{
				yylval.num = yytext[1] - '0';
				return T_NUM;
			}
break;
case 4:
	{
				yylval.num = number(yytext);
				return T_NUM;
			}
break;
case 5:
			{
				int c;

				yyleng = 0;
				while((c = input()) != 0 && c != '"') {
					yytext[yyleng++] = c;
					if (c == '\\')
						yytext[yyleng++] = input();
				}
				yylval.s = unquotealloc(yytext,yyleng);
				return T_STRING;
			}
break;
case 6:
{
				yylval.num = number(yytext);
				return T_NUM;
			}
break;
case 7:
{
				yylval.num = number(yytext);
				return T_NUM;
			}
break;
case 8:
{
				yylval.num = number(yytext);
				return T_NUM;
			}
break;
case 9:
		{	return T_CMDSEQPREFIX; }
break;
case 10:
	{	;	}
break;
case 11:
{	;	}
break;
case 12:
		{	;	}
break;
case 13:
			{	;	}
break;
case 14:
		{	/* Character constant */
				yylval.num = yytext[1];
				return T_NUM;
			}
break;
case 15:
		return T_MENUSEPERATOR;
break;
case 16:
		return T_MENUBREAK;
break;
case 17:
	return T_MENUBARBREAK;
break;
case 18:
	{	SYMBOL 	sym;
				char *	key;

				sym = sym_find(yytext,&key);
				if (VALUE(sym) == 0 && TYPEOF(sym) == 0) {
retIdent:				yylval.s = yystralloc(yytext);
					return T_IDENT;
				}

				if (TYPEOF(sym) == S_KEYWORD) {
					if (VALUE(sym) == T_DEFKEYS || 
					    VALUE(sym) == T_DEFMOUSE) {
						BEGIN keys;
					}
					return VALUE(sym);
				}

				if (TYPEOF(sym) == S_EDFUNC) {
					yylval.funcp = (void*)VALUE(sym);
					return T_FUNC;
				}

				if (TYPEOF(sym) == S_ENUM) {
					yylval.num = EnumValue((void*)VALUE(sym));
					return T_NUM;
				}
				
				if (TYPEOF(sym) == S_CONSTNUM) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.num = VALUE(sym);
					return T_NUM;
				}
	
				if (TYPEOF(sym) == S_CONSTSTRING) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.s = yystralloc((char *)VALUE(sym));
					return T_IDENT;
				}

				if (TYPEOF(sym) == S_NUMBER || TYPEOF(sym) == S_DOLNUMBER) {
# if 0
					yylval.s = yystralloc(yytext);
# else
					yylval.s = key;
# endif
					return T_NUMVAR;
				}

				if (TYPEOF(sym) == S_STRING || TYPEOF(sym) == S_DOLSTRING) {
# if 0
					yylval.s = yystralloc(yytext);
# else
					yylval.s = key;
# endif
					return T_STRINGVAR;
				}

				yyerror("bad symbol %s type %d",yytext,TYPEOF(sym));
				yylval.num = 0;
				return T_NUM;
			}
break;
case 19:
	{	 BEGIN INITIAL; return yytext[0];	 }
break;
case 20:
		return T_EQ;
break;
case 21:
		return T_NE;
break;
case 22:
		return T_NMATCH;
break;
case 23:
		return T_AND;
break;
case 24:
		return T_OR;
break;
case 25:
		return T_ASSIGN;
break;
case 26:
		return T_DOTDOT;
break;
case 27:
		return yytext[0];
break;
case -1:
break;
default:
yyerror("syntax-error");
} return(0); }
/* end of yylex */

int yyvstop[] = {
0,

27,
0,

12,
27,
0,

13,
0,

27,
0,

5,
27,
0,

10,
27,
0,

27,
0,

27,
0,

27,
0,

1,
18,
27,
0,

27,
0,

27,
0,

8,
18,
27,
0,

8,
18,
27,
0,

25,
27,
0,

9,
27,
0,

18,
27,
0,

27,
0,

27,
0,

27,
0,

2,
18,
27,
0,

2,
18,
27,
0,

2,
18,
27,
0,

19,
27,
0,

27,
0,

21,
0,

22,
0,

10,
0,

23,
0,

8,
0,

8,
0,

18,
0,

8,
18,
0,

8,
18,
0,

18,
0,

26,
0,

11,
0,

7,
8,
18,
0,

18,
0,

20,
0,

24,
0,

2,
18,
0,

2,
18,
0,

2,
18,
0,

2,
18,
0,

4,
0,

14,
0,

7,
8,
0,

18,
0,

6,
18,
0,

3,
0,

2,
18,
0,

2,
6,
18,
0,

16,
0,

6,
0,

15,
18,
0,

17,
0,

2,
15,
18,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,5,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,7,	
33,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,8,	1,9,	
1,10,	30,55,	55,65,	1,11,	
1,12,	8,30,	11,34,	35,56,	
1,13,	48,62,	1,14,	1,15,	
1,16,	1,17,	1,18,	1,18,	
1,18,	1,18,	1,18,	1,18,	
1,18,	1,18,	15,42,	16,43,	
18,41,	24,49,	1,19,	18,40,	
19,46,	1,20,	1,21,	27,52,	
37,37,	8,31,	27,52,	0,0,	
1,21,	0,0,	2,8,	2,9,	
2,10,	0,0,	40,41,	2,11,	
2,12,	40,40,	50,63,	52,52,	
0,0,	50,52,	52,52,	2,15,	
2,16,	1,21,	2,18,	2,18,	
2,18,	2,18,	2,18,	2,18,	
2,18,	49,58,	3,5,	0,0,	
63,69,	69,52,	2,19,	63,52,	
69,52,	2,20,	3,6,	3,7,	
13,36,	13,37,	13,37,	13,37,	
13,37,	13,37,	13,37,	13,37,	
13,37,	13,37,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
26,52,	1,22,	22,47,	26,52,	
47,61,	49,58,	3,8,	3,9,	
3,10,	61,68,	8,32,	3,11,	
3,12,	3,23,	0,0,	0,0,	
3,24,	0,0,	3,25,	3,15,	
3,16,	3,26,	3,27,	3,27,	
3,27,	3,27,	3,27,	3,27,	
3,27,	3,27,	51,52,	0,0,	
3,28,	51,52,	3,19,	0,0,	
0,0,	3,20,	3,27,	0,0,	
10,33,	2,22,	0,0,	26,53,	
3,27,	0,0,	0,0,	0,0,	
10,33,	10,0,	4,8,	4,9,	
4,10,	0,0,	0,0,	4,11,	
4,12,	4,23,	0,0,	0,0,	
0,0,	3,27,	0,0,	4,15,	
4,16,	3,29,	4,27,	4,27,	
4,27,	4,27,	4,27,	4,27,	
4,27,	51,53,	0,0,	26,53,	
4,28,	0,0,	4,19,	0,0,	
0,0,	4,20,	10,33,	0,0,	
10,33,	0,0,	0,0,	10,33,	
0,0,	0,0,	12,35,	0,0,	
0,0,	0,0,	0,0,	10,33,	
0,0,	3,22,	12,35,	12,0,	
0,0,	0,0,	0,0,	0,0,	
10,33,	51,53,	0,0,	0,0,	
21,41,	4,29,	10,33,	21,41,	
21,41,	21,41,	21,41,	21,41,	
21,41,	21,41,	21,41,	21,41,	
21,41,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	10,33,	
0,0,	0,0,	0,0,	0,0,	
12,35,	0,0,	12,35,	0,0,	
0,0,	12,35,	0,0,	0,0,	
0,0,	4,22,	0,0,	44,41,	
0,0,	12,35,	44,44,	44,44,	
44,44,	44,44,	44,44,	44,44,	
44,44,	44,44,	12,35,	0,0,	
0,0,	0,0,	14,38,	0,0,	
12,35,	14,39,	14,40,	14,40,	
14,40,	14,40,	14,40,	14,40,	
14,40,	14,40,	14,40,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	12,35,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
0,0,	0,0,	0,0,	0,0,	
14,41,	0,0,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
14,41,	14,41,	14,41,	14,41,	
17,41,	0,0,	0,0,	17,44,	
17,44,	17,44,	17,44,	17,44,	
17,44,	17,44,	17,44,	23,48,	
23,48,	23,48,	23,48,	23,48,	
23,48,	23,48,	23,48,	23,48,	
23,48,	0,0,	0,0,	0,0,	
25,50,	0,0,	0,0,	25,51,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	17,45,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	0,0,	0,0,	
0,0,	0,0,	25,52,	17,45,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	25,52,	25,52,	
25,52,	25,52,	29,54,	29,54,	
29,54,	29,54,	29,54,	29,54,	
29,54,	29,54,	29,54,	29,54,	
36,57,	36,57,	36,57,	36,57,	
36,57,	36,57,	36,57,	36,57,	
36,37,	36,37,	38,59,	0,0,	
0,0,	38,41,	38,41,	38,41,	
38,41,	38,41,	38,41,	38,41,	
38,41,	38,41,	38,41,	39,41,	
0,0,	0,0,	39,44,	39,44,	
39,44,	39,44,	39,44,	39,44,	
39,44,	39,44,	0,0,	0,0,	
0,0,	0,0,	41,41,	0,0,	
36,58,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	0,0,	
43,43,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
43,43,	43,0,	0,0,	0,0,	
0,0,	0,0,	39,45,	0,0,	
0,0,	0,0,	0,0,	0,0,	
36,58,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
45,41,	0,0,	0,0,	45,60,	
45,60,	45,60,	45,60,	45,60,	
45,60,	45,60,	45,60,	45,60,	
45,60,	0,0,	43,43,	0,0,	
43,43,	0,0,	39,45,	43,43,	
45,60,	45,60,	45,60,	45,60,	
45,60,	45,60,	0,0,	43,43,	
57,57,	57,57,	57,57,	57,57,	
57,57,	57,57,	57,57,	57,57,	
43,43,	0,0,	0,0,	0,0,	
0,0,	0,0,	43,43,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
45,60,	45,60,	45,60,	45,60,	
45,60,	45,60,	53,52,	43,43,	
0,0,	53,64,	53,64,	53,64,	
53,64,	53,64,	53,64,	53,64,	
53,64,	53,64,	53,64,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	53,64,	53,64,	
53,64,	53,64,	53,64,	53,64,	
58,66,	58,66,	58,66,	58,66,	
58,66,	58,66,	58,66,	58,66,	
58,66,	58,66,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	58,66,	58,66,	58,66,	
58,66,	58,66,	58,66,	0,0,	
0,0,	0,0,	53,64,	53,64,	
53,64,	53,64,	53,64,	53,64,	
59,67,	0,0,	0,0,	59,41,	
59,41,	59,41,	59,41,	59,41,	
59,41,	59,41,	59,41,	59,41,	
59,41,	0,0,	0,0,	0,0,	
0,0,	58,66,	58,66,	58,66,	
58,66,	58,66,	58,66,	60,41,	
0,0,	0,0,	60,60,	60,60,	
60,60,	60,60,	60,60,	60,60,	
60,60,	60,60,	60,60,	60,60,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	60,60,	
60,60,	60,60,	60,60,	60,60,	
60,60,	64,52,	0,0,	0,0,	
64,64,	64,64,	64,64,	64,64,	
64,64,	64,64,	64,64,	64,64,	
64,64,	64,64,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	64,64,	64,64,	64,64,	
64,64,	64,64,	64,64,	60,60,	
60,60,	60,60,	60,60,	60,60,	
60,60,	67,41,	0,0,	0,0,	
67,41,	67,41,	67,41,	67,41,	
67,41,	67,41,	67,41,	67,41,	
67,41,	67,41,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	64,64,	64,64,	64,64,	
64,64,	64,64,	64,64,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-41,	yysvec+1,	0,	
yycrank+-97,	0,		0,	
yycrank+-141,	yysvec+3,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+8,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+10,
yycrank+-163,	0,		yyvstop+13,
yycrank+4,	0,		yyvstop+16,
yycrank+-213,	0,		yyvstop+18,
yycrank+60,	0,		yyvstop+20,
yycrank+237,	0,		yyvstop+22,
yycrank+12,	0,		yyvstop+26,
yycrank+12,	0,		yyvstop+28,
yycrank+315,	yysvec+14,	yyvstop+30,
yycrank+15,	yysvec+14,	yyvstop+34,
yycrank+3,	0,		yyvstop+38,
yycrank+0,	0,		yyvstop+41,
yycrank+187,	yysvec+14,	yyvstop+44,
yycrank+2,	0,		yyvstop+47,
yycrank+323,	0,		yyvstop+49,
yycrank+13,	0,		yyvstop+51,
yycrank+339,	0,		yyvstop+53,
yycrank+79,	yysvec+25,	yyvstop+57,
yycrank+22,	yysvec+25,	yyvstop+61,
yycrank+0,	0,		yyvstop+65,
yycrank+414,	0,		yyvstop+68,
yycrank+4,	0,		0,	
yycrank+0,	0,		yyvstop+70,
yycrank+0,	0,		yyvstop+72,
yycrank+-2,	yysvec+10,	yyvstop+74,
yycrank+0,	0,		yyvstop+76,
yycrank+4,	0,		0,	
yycrank+424,	0,		yyvstop+78,
yycrank+20,	yysvec+13,	yyvstop+80,
yycrank+437,	yysvec+14,	yyvstop+82,
yycrank+450,	yysvec+14,	yyvstop+84,
yycrank+33,	yysvec+14,	yyvstop+87,
yycrank+465,	yysvec+14,	yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+-523,	0,		yyvstop+94,
yycrank+222,	yysvec+14,	yyvstop+96,
yycrank+507,	yysvec+14,	yyvstop+100,
yycrank+0,	0,		yyvstop+102,
yycrank+4,	0,		yyvstop+104,
yycrank+4,	0,		0,	
yycrank+9,	0,		0,	
yycrank+37,	yysvec+25,	yyvstop+106,
yycrank+109,	yysvec+25,	yyvstop+109,
yycrank+38,	yysvec+25,	yyvstop+112,
yycrank+565,	yysvec+25,	yyvstop+115,
yycrank+0,	yysvec+29,	yyvstop+118,
yycrank+5,	0,		0,	
yycrank+0,	0,		yyvstop+120,
yycrank+532,	yysvec+13,	yyvstop+122,
yycrank+588,	0,		0,	
yycrank+623,	yysvec+14,	yyvstop+125,
yycrank+646,	yysvec+14,	yyvstop+127,
yycrank+9,	0,		0,	
yycrank+0,	0,		yyvstop+130,
yycrank+55,	yysvec+25,	yyvstop+132,
yycrank+672,	yysvec+25,	yyvstop+135,
yycrank+0,	0,		yyvstop+139,
yycrank+0,	yysvec+58,	yyvstop+141,
yycrank+704,	yysvec+14,	yyvstop+143,
yycrank+0,	0,		yyvstop+146,
yycrank+56,	yysvec+25,	yyvstop+148,
0,	0,	0};
struct yywork *yytop = yycrank+774;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'+' ,01  ,'-' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'8' ,'8' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'X' ,'G' ,'G' ,01  ,01  ,01  ,01  ,'G' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'X' ,'G' ,'G' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int  *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (long)yyt > (long)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((long)yyt < (long)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) != 0 && 
			    (yyt= yystate->yystoff)      != yycrank) {
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) != 0 && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
void yyoutput(c)
  int c; {
	output(c);
	}
void yyunput(c)
   int c; {
	unput(c);
	}
