%{
/*
 * SCANNER.L
 *
 * Scanner
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 05.02.1991
 *									
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "alloc.h"
#include "sym.h"
#include "edfuncs.h"
#include "scanner.h"
#include "parser.h"
#include "lineoperations.h"
#include "pkscc.h"
#include "stringutil.h"
#include "crossreferencelinks.h"

#define YY_FATAL_ERROR(msg) yyerror( msg )

#define YY_INPUT(buf,result,max_size) \
	if ( 1) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     (c = cmdgetc()) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else if ( ((result = fread( buf, 1, max_size, yyin )) == 0) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" );

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

extern FILE		*createtmp(char *fnd, char *fn);
extern int 		init_funcs(void);
extern long 		EnumValue(void *enp);
extern void 		macro_selectDefaultBindings(void);
extern void 		stepselectcompiler(char *pszName);

#if defined(STAND_ALONE)
static FILE *_lexFp;
#endif

/*---------------------------------*/
/* keyword()					*/
/*---------------------------------*/
static struct kw {
	int			toknum;
	char *	name;
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

	if (yyleng > YY_BUF_SIZE -4) {
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
/* yyerror()					*/
/*---------------------------------*/
void yyerror(char* s, ...)
{
	char buf1[512];
	char buf[512];
	va_list ap;

    va_start(ap,s);
	if (!yyerr.srcname)
		yyerr.srcname = "STDIN";

	if (yyerr.errfp != 0 ||
		(yyerr.errfp = createtmp(yyerr.errname, "PKSMAKRO.ERR")) != 0) {
		yyerrflg = 1;
		wvsprintf(buf1, s, ap);
		sprintf(buf, "Error %s %d: %s\n", yyerr.srcname, yyerr.yylineno, buf1);
		fprintf(yyerr.errfp, buf);
		fflush(yyerr.errfp);
		if (++yyerr.yynerr >= yyerr.yymaxerr) {
			fprintf(yyerr.errfp, "\n*** too many errors ***\n");
			if (yyerr.failpt)
				longjmp(*yyerr.failpt, ERR_TOOMANYERR);
		}
	}
	va_end(ap);
}

/*---------------------------------*/
/* cmdgetc()					*/
/*---------------------------------*/
static int cmdgetc(void )
{	LINE *lp;

	if (yyleng > YY_BUF_SIZE-4) {
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

	macro_selectDefaultBindings();
	if (yyerr.errfp) {
		fprintf(yyerr.errfp,"\nTOTAL %d Errors\n",yyerr.yynerr);
		fclose(yyerr.errfp);
		yyerr.errfp = 0;
		yyerr.failpt = 0;
		xref_selectSearchListFormat("PKSMAKROC");
		xref_openSearchList(yyerr.errname,1);
		return 0;
	} else {
		macro_showStatus("No Errors detected");
	}
	return 1;
}

/*---------------------------------*/
/* yystralloc()				*/
/*---------------------------------*/
unsigned char *yystralloc(unsigned char *s)
{	unsigned char *d;

	if ((d = string_allocate(s)) == 0) {
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
	i = (int)(d-qbuf);
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

%}
SIGN		[-+]?
OKTNUM	[0-7]
NUM		[0-9]
HEXNUM	[0-9a-fA-F]
CH		[a-z_A-Z]
/* CHKEYS	[\-0-9a-z_A-Z”„™Žš] */
CHN		[\-0-9a-z_A-Z]
CHK		[\-0-9a-z_A-Z+]
QUOTED	\\.
NOS		[^"]
WS		[ \t]

%START		keys

%%
<INITIAL>-	{
				return '-';
			}
<keys>{CHN}+	{
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
<keys>\({NUM}\)	{
				yylval.num = yytext[1] - '0';
				return T_NUM;
			}
<keys>\\{NUM}+		{
				yylval.num = number(yytext);
				return T_NUM;
			}
\" 			{
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
{SIGN}0[xX]{HEXNUM}+ {
				yylval.num = number(yytext);
				return T_NUM;
			}
<INITIAL>{SIGN}0{OKTNUM}+	{
				yylval.num = number(yytext);
				return T_NUM;
			}
<INITIAL>{SIGN}{NUM}+	{
				yylval.num = number(yytext);
				return T_NUM;
			}
@			{	return T_CMDSEQPREFIX; }
#[^\n]*		{	;	}
\/\/[^\n]*	{	;	}
{WS}			{	;	}
\n 			{	;	}	/* skip comments and white space */
'.'			{	/* Character constant */
				yylval.num = yytext[1];
				return T_NUM;
			}
----			return T_MENUSEPERATOR;
!!!!			return T_MENUBREAK;
\|\|\|\|		return T_MENUBARBREAK;
{CHN}+		{	SYMBOL 	sym;
				char *	key;

				sym = sym_find(yytext,&key);
				if (VALUE(sym) == 0 && TYPEOF(sym) == 0) {
retIdent:				yylval.s = yystralloc(yytext);
					return T_IDENT;
				}

				if (TYPEOF(sym) == S_KEYWORD) {
					if (VALUE_AS_INT(sym) == T_DEFKEYS || 
					    VALUE_AS_INT(sym) == T_DEFMOUSE) {
						BEGIN keys;
					}
					return VALUE_AS_INT(sym);
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
					yylval.num = VALUE_AS_INT(sym);
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
<keys>\;		{	 BEGIN INITIAL; return yytext[0];	 }
==			return T_EQ;
!=			return T_NE;
!~			return T_NMATCH;
&&			return T_AND;
\|\|			return T_OR;
=			return T_ASSIGN;
\.\.			return T_DOTDOT;
.			return yytext[0];
%%

