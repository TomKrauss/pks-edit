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
#include "stringutil.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "symbols.h"
#include "scanner.h"
#define YYSTYPE _YYSTYPE
#include "parser.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "crossreferencelinks.h"

//
// work around for flex generating redundant goto label, when line numbers option is used
//
#pragma warning(disable: 4102)

#define fileno(x)		_fileno(x)
#define YY_FATAL_ERROR(msg) yyerror( msg )

#define YY_INPUT(buf,result,max_size) \
	if ( 1) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < (int)max_size && \
			     (c = cmdgetc()) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else if ( ((result = (int)fread( buf, 1, max_size, yyin )) == 0) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" );

typedef struct yyerrstruct {
	char	*srcname;
	char	errname[84];
	FILE	*errfp;
	int		yynerr,yynwarn;
	int		yymaxerr,yymaxwarn;
	jmp_buf	*failpt;
} YYERRSTRUCT;

static YYERRSTRUCT	yyerr;
int	  			yyerrflg;
int				_bDefiningConst;

typedef void EDFUNCDEF;

extern IDENTIFIER_CONTEXT* _currentIdentifierContext;
extern FILE		*file_createTempFile(char *fnd, char *fn);
extern int 		function_initializeFunctionsAndTypes(void);
extern long 	function_enumValueFor(void *enp);
extern int		_macrosWereChanged;

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
	T_TAUTO,	"auto",
	T_VOID,		"void",
	T_CONST,	"const",
	T_IF,		"if",
	T_ELSE,		"else",
	T_ELIF,		"elif",
	T_SWITCH,	"switch",
	T_WHILE,	"while",
	T_CONTINUE,	"continue",
	T_BREAK,	"break",
	T_RETURN,	"return",
	T_LOCAL,	"local",
	T_GOTO,		"goto",
	T_BRAEQ,	"braeq",
	T_RANGE,	"range",
	T_TCHAR,	"char",
	T_TINT,		"int",
	T_TLONG,	"long",
	T_TFLOAT,	"float",
	T_TBOOLEAN,	"boolean",
	T_NAMESPACE, "namespace",
	T_FOR,		"for",
	T_STATIC,	"static",
	T_TRUE,		"true",
	T_FALSE,	"false",
	T_TSTRING,	"string",
	-1,			0
};

static int init_keywords(void)
{	struct kw *kp = keywords;

	while(kp->name) {
		if (!sym_createSymbol(sym_getKeywordContext(), kp->name,S_KEYWORD,(GENERIC_DATA){.intValue = kp->toknum}))
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

	return fgetc(_lexFp);
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
		(yyerr.errfp = file_createTempFile(yyerr.errname, "pksmacro.err")) != 0) {
		yyerrflg = 1;
		wvsprintf(buf1, s, ap);
		sprintf(buf, "Error %s %d: %s\n", yyerr.srcname, yylineno, buf1);
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
			LexInit(lp,_lexLastLine);
			return '\n';
		}
		return EOF;
	}
	return *_lexStart++;
}

# endif

COMPILER_CONFIGURATION* _compilerConfiguration;
char * _yyCurrentComment;

/*---------------------------------*/
/* yyinit()					*/
/*---------------------------------*/
int yyinit(jmp_buf *errb, COMPILER_CONFIGURATION* pConfig, LINE *lps, LINE *lpe) {
	static tables_inited;
	YY_FLUSH_BUFFER;
	_compilerConfiguration = pConfig;
	_currentIdentifierContext = sym_getGlobalContext();
	if (!tables_inited) {
		if (!init_keywords() ||
		    !function_initializeFunctionsAndTypes()) {
			return 0;
		}
		tables_inited = 1;
	}
	yyerrflg = 0;
	yylineno = 1;
	yyerr.yynwarn = 0;
	yyerr.yynerr = 0;
	yyerr.yymaxerr = 30;
	yyerr.yymaxwarn= 30;
	yyerr.failpt = errb;
	if (yyerr.errfp) {
		fclose(yyerr.errfp);
		yyerr.errfp = 0;
	}
	yyerr.srcname = pConfig->cb_source;

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

	if (_yyCurrentComment) {
		free(_yyCurrentComment);
		_yyCurrentComment = NULL;
	}
	yy_delete_buffer(yy_current_buffer);
	yy_init = 1;
	if (yyerr.errfp) {
		fprintf(yyerr.errfp,"\nTOTAL %d Errors\n",yyerr.yynerr);
		fclose(yyerr.errfp);
		yyerr.errfp = 0;
		yyerr.failpt = 0;
		if (_compilerConfiguration->cb_openErrorList) {
			xref_openSearchList(yyerr.errname,1);
		}
		return 0;
	} else {
		_compilerConfiguration->cb_showStatus("No Errors detected");
	}
	return 1;
}

/*---------------------------------*/
/* yystralloc()				*/
/*---------------------------------*/
unsigned char *yystralloc(unsigned char *s)
{	unsigned char *d;

	if ((d = _strdup(s)) == 0) {
		if (yyerr.failpt)
			longjmp(*yyerr.failpt,ERR_SPACE);
		return 0;
	}
	return d;
}

/*
 * Decode a quoted character in string and character literals.
 */
static int quotedchar(int c) {
	switch(c) {
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	}
	return c;
}

/*---------------------------------*/
/* unquotealloc()				*/
/*---------------------------------*/
static char *unquotealloc(char *name, int len)
{	char qbuf[512],*d,*dend,*send;
	int  cnt,i;

	for (d = qbuf, dend= &qbuf[sizeof qbuf-2], send = &name[len]; 	
		name < send; name++) {
		if (d >= dend) {
			yyerror("unterminated string");
			break;
		}
		if (*name == '\\') {
			if (name[1] != '0') {
				*d++ = quotedchar(name[1]); 
			} else {
				for (i = 0, cnt = 0; cnt < 3; cnt++) {
					if (name[2] >= '0' && name[2] <= '7') {
						i = 8*i + (name[2] - '0');
						name++;
					}
				}
				*d++ = i;
			}
			name++;
		} else {
			*d++ = *name;
		}
	}
	*d++ = 0;
	i = (int)(d-qbuf);
	if ((d = malloc(i)) == 0) {
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
long long number(char *s) {
	char* pszStart = s;
	if (*s == '\\') {
		s++;
	}
	int nBase = 10;
	if (*s == '0') {
		if (s[1] == 'x' || s[1] == 'X') {
			s += 2;
			nBase = 16;
		} else {
			s++;
			nBase = 8;
		}
	} 
	long long i = 0;
	int o;
	for (;;) {
		if (*s >= '0' && *s <= '9')
			o = (*s - '0');
		else if (*s >= 'A' && *s <= 'F')
			o = (*s - 'A' + 10);
		else if (*s >= 'a' && *s <= 'f')
			o = (*s - 'a' + 10);
		else break;
		i *= nBase;
		i += o;
		if (i < 0) {
			yyerror("Integer overflow while converting %s", pszStart);
			break;
		}
		s++;
	}
	return i;
}

/*------------------------------------------------------------
 * number()
 */
static void numberOrFloat(char *s, _YYSTYPE* pType, int *bFloat) {
	if (!*s) {
		*bFloat = 0;
		pType->num = 0;
		return;
	}
	size_t nLen = strlen(s);
	char* pszDot = strchr(s, '.');
	*bFloat = pszDot || s[nLen-1] == 'd' ? 1 : 0;
	if (pszDot) {
		sscanf(s, "%lf", &pType->v.data.doubleValue);
	} else {
		pType->num = number(s);
	}
}

/*---------------------------------*/
/* yywrap()					*/
/*---------------------------------*/
int yywrap(void )
{
	return 1;
}

%}
OKTNUM	[0-7]
NUM		[0-9]
HEXNUM	[0-9a-fA-F]
CH		[a-z_A-Z]
CHN		[0-9a-z_A-Z]+(-[0-9a-z_A-Z]+)*
QUOTED	\\.
NOS		[^"]
WS		[ \t]

%option yylineno

%%
<INITIAL>-	{
				return '-';
			}
<INITIAL>\/\* {
				int c, lastC = 0;
				int bSkip = 0;

				yyleng = 0;
				while((c = input()) != 0) {
					if (c == '\n') {
						if (yyleng) {
							yytext[yyleng++] = '\r';
							yytext[yyleng++] = '\n';
						}
						bSkip = 1;
						continue;
					}
					if (c == '/' && lastC == '*') {
						yytext[yyleng - 1] = 0;
						break;
					}
					lastC = c;
					if (bSkip && c != ' ' && c != '*' && c != '\r') {
						bSkip = 0;
					}
					if (!bSkip) {
						yytext[yyleng++] = c;
					}
				}
				yytext[yyleng] = 0;
				if (_yyCurrentComment) {
					free(_yyCurrentComment);
				}
				_yyCurrentComment = _strdup(yytext);
}
\" 			{
				int c;

				yyleng = 0;
				while((c = input()) > 0 && c != '"') {
					yytext[yyleng++] = c;
					if (c == '\\') {
						c = input();
						if (c > 0) {
							yytext[yyleng++] = c;
						}
					}
				}
				yylval.ident.stringIsAlloced = 0;
				yylval.ident.s = unquotealloc(yytext,yyleng);
				return T_STRING;
			}
'([^'\\]|\\['\\abnrftv])'	{	/* Character literal */
				if (yytext[1] == '\\') {
					yylval.num = quotedchar(yytext[2]);
				} else {
					yylval.num = yytext[1];
				}
				return T_CHARACTER;
			}
0[xX]{HEXNUM}+ {
				yylval.num = number(yytext);
				return T_NUM;
			}
<INITIAL>0{OKTNUM}+	{
				yylval.num = number(yytext);
				return T_NUM;
			}
\.\.		return T_DOTDOT;
<INITIAL>{NUM}+(\.{NUM}+)?d? {
				int bFloat;
				numberOrFloat(yytext, &yylval, &bFloat);
				return bFloat ? T_FLOATING_POINT_NUMBER : T_NUM;
			}
@			{	return T_CMDSEQPREFIX; }
#[^\n]*		{	;	}
\/\/[^\n]*	{	;	}
{WS}		{	;	}
\n 			{	;	}	/* skip comments and white space */
\r 			{	;	}	/* skip comments and white space */
\+\+		{	return T_PLUSPLUS; }
\-\-		{	return T_MINUSMINUS; }
{CHN}+		{	PKS_VALUE 	sym;
				char *	key;

				sym = sym_find(_currentIdentifierContext, yytext,&key);
				if (key == 0) {
					key = yytext;
				}
				if (VALUE(sym) == 0 && TYPEOF(sym) == 0) {
retIdent:			yylval.ident.s = yystralloc(yytext);
					yylval.ident.stringIsAlloced = 1;
					return T_IDENT;
				}

				if (TYPEOF(sym) == S_KEYWORD) {
					return sym.sym_data.intValue;
				}

				if (TYPEOF(sym) == S_EDFUNC) {
					yylval.funcp = (void*)VALUE(sym);
					return T_FUNC;
				}

				if (TYPEOF(sym) == S_ENUM) {
					yylval.num = function_enumValueFor((void*)VALUE(sym));
					return T_NUM;
				}
				
				if (TYPEOF(sym) == S_CONSTNUM) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.num = sym.sym_data.longValue;
					return T_NUM;
				}
	
				if (TYPEOF(sym) == S_CONSTFLOAT) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.v.data.doubleValue = sym.sym_data.doubleValue;
					return T_FLOATING_POINT_NUMBER;
				}
	
				if (TYPEOF(sym) == S_CONSTCHARACTER) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.num = sym.sym_data.uchar;
					return T_CHARACTER;
				}
	
				if (TYPEOF(sym) == S_CONSTSTRING) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					yylval.ident.stringIsAlloced = 1;
					yylval.ident.s = yystralloc((char *)VALUE(sym));
					return T_STRING;
				}

				if (TYPEOF(sym) == S_FLOAT) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_FLOATVAR;
				}
				if (TYPEOF(sym) == S_BOOLEAN) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_BOOLEANVAR;
				}
				if (TYPEOF(sym) == S_CHARACTER) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_CHARACTERVAR;
				}
				if (TYPEOF(sym) == S_NUMBER) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_NUMVAR;
				}
				if (TYPEOF(sym) == S_RANGE) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_RANGEVAR;
				}

				if (TYPEOF(sym) == S_STRING) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_STRINGVAR;
				}
				if (TYPEOF(sym) == S_ARRAY) {
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					return T_STRINGARRAYVAR;
				}


				yyerror("bad symbol %s type %d",yytext,TYPEOF(sym));
				yylval.num = 0;
				return T_NUM;
			}
==			return T_EQ;
!=			return T_NE;
\<\<		return T_SHIFT_LEFT;
\>\>		return T_SHIFT_RIGHT;
\*\*		return T_POWER_TO;
\<=			return T_LE;
\>=			return T_GE;
\*=			return T_SH_ASSIGN_MULT;
\+=			return T_SH_ASSIGN_PLUS;
\-=			return T_SH_ASSIGN_MINUS;
\/=			return T_SH_ASSIGN_DIV;
\%=			return T_SH_ASSIGN_MOD;
!~			return T_NMATCH;
&&			return T_AND;
\|\|		return T_OR;
=			return T_ASSIGN;
.			return yytext[0];
%%

