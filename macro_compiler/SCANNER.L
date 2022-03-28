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
#include <io.h>

#include "alloc.h"
#include "stringutil.h"
#include "arraylist.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "symbols.h"
#include "scanner.h"
#define YYSTYPE _YYSTYPE
#include "parser.h"

#define isatty _isatty

#define YY_NO_UNISTD_H
#define fileno(x)		_fileno(x)
#define YY_FATAL_ERROR(msg) yyerror( msg )
#define MAX_COMMENT_SIZE		16000

#define YY_INPUT(buf,result,max_size) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < (int)max_size && \
			     (c = _compilerInputStream->cis_next(_compilerInputStream)) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		result = n; \
		} 

typedef struct yyerrstruct {
	char		errname[200];
	FILE		*errfp;
	int			yynerr,yynwarn;
	int			yymaxerr,yymaxwarn;
	jmp_buf		*failpt;
} YYERRSTRUCT;

static YYERRSTRUCT	yyerr;
int				_bDefiningConst;
int				_bInHeader;
int				_bInNativeDefinition;

typedef void EDFUNCDEF;

extern IDENTIFIER_CONTEXT* _currentIdentifierContext;
extern FILE		*file_createTempFile(char *fnd, char *fn);
extern int 		function_initializeFunctionsAndTypes(void);
extern long 	function_enumValueFor(void *enp);
extern void		types_registerDefaultTypes();
extern int yylex_destroy ( void );

COMPILER_CONFIGURATION* _compilerConfiguration;

const char* yy_getCurrentInputFilename() {
	return _compilerConfiguration->cb_source;
}

/*---------------------------------*/
/* keyword()					*/
/*---------------------------------*/
static struct kw {
	int			toknum;
	char *	name;
} keywords[] = {
	T_VOID,		"void",
	T_CONST,	"const",
	T_REQUIRE,	"require",
	T_IF,		"if",
	T_NEW,		"new",
	T_ELSE,		"else",
	T_SWITCH,	"switch",
	T_CASE,		"case",
	T_DEFAULT,	"default",
	T_WHILE,	"while",
	T_NATIVE,	"native",
	T_CONTINUE,	"continue",
	T_BREAK,	"break",
	T_RETURN,	"return",
	T_GOTO,		"goto",
	T_NAMESPACE, "namespace",
	T_FOR,		"for",
	T_STATIC,	"static",
	T_TRUE,		"true",
	T_FALSE,	"false",
	-1,			0
};

static int init_keywords(void)
{	struct kw *kp = keywords;

	while(kp->name) {
		if (!sym_createSymbol(sym_getKeywordContext(), kp->name,S_KEYWORD,0,(GENERIC_DATA){.intValue = kp->toknum}, 0))
			return 0;
		kp++;
	}
	return 1;
}

static void yyreport(const char* s, int bError, va_list ap) {
	char buf1[512];
	if (yyerr.errfp != 0 ||
		(yyerr.errfp = file_createTempFile(yyerr.errname, "pksmacro.err")) != 0) {
		wvsprintf(buf1, s, ap);
		fprintf(yyerr.errfp, "%s %s %d: %s\n", (bError) ? "Error" : "Warning", yy_getCurrentInputFilename(), yylineno, buf1);
		fflush(yyerr.errfp);
		if (bError && ++yyerr.yynerr >= yyerr.yymaxerr) {
			fprintf(yyerr.errfp, "\n*** too many errors ***\n");
			if (yyerr.failpt)
				longjmp(*yyerr.failpt, ERR_TOOMANYERR);
		}
		if (!bError && ++yyerr.yynwarn  >= yyerr.yymaxwarn) {
			yyerr.yynerr++;
			fprintf(yyerr.errfp, "\n*** too many warnings ***\n");
			if (yyerr.failpt)
				longjmp(*yyerr.failpt, ERR_TOOMANYERR);
		}
	}
}

/*---------------------------------*/
/* yyerror()					*/
/*---------------------------------*/
void yyerror(const char* s, ...) {
	va_list ap;

    va_start(ap,s);
	yyreport(s, 1, ap);
	va_end(ap);
}

void yywarning(const char* s, ...) {
	va_list ap;

    va_start(ap,s);
	yyreport(s, 0, ap);
	va_end(ap);
}

static COMPILER_INPUT_STREAM	*_compilerInputStream;

char * _yyCurrentComment;

void yyrequire(const char* pszNamespace) {
	if (!compiler_requireNamespace(_compilerConfiguration->cb_dependencies, yy_getCurrentInputFilename(), pszNamespace)) {
		yyerror("Cannot locate source file for required namespace %s", pszNamespace);
	}
}

/*---------------------------------*/
/* yyinit()					*/
/*---------------------------------*/
int yyinit(jmp_buf *errb, COMPILER_CONFIGURATION* pConfig) {
	static tables_inited;
	YY_FLUSH_BUFFER;
	_compilerConfiguration = pConfig;
	_compilerInputStream = pConfig->cb_stream;
	_currentIdentifierContext = sym_getGlobalCompilerContext();
	if (!tables_inited) {
		if (!init_keywords() ||
		    !function_initializeFunctionsAndTypes()) {
			return 0;
		}
		types_registerDefaultTypes();
		tables_inited = 1;
	}
	yylineno = 1;
	if (pConfig->cb_topLevelFile) {
		pConfig->cb_dependencies = arraylist_create(11);
		if (yyerr.errfp) {
			fclose(yyerr.errfp);
			yyerr.errfp = 0;
		}
	}
	yyerr.yynwarn = 0;
	yyerr.yynerr = 0;
	yyerr.yymaxerr = 30;
	yyerr.yymaxwarn = 30;
	yyerr.failpt = errb;
	return 1;
}

/*---------------------------------*/
/* yyfinish()					*/
/*---------------------------------*/
int yyfinish(void)
{
	if (_compilerInputStream->cis_close) {
		_compilerInputStream->cis_close(_compilerInputStream);
	}
	if (_yyCurrentComment) {
		free(_yyCurrentComment);
		_yyCurrentComment = NULL;
	}
	yy_init = 0;
	yylex_destroy();
	_compilerConfiguration->cb_errorFile = yyerr.errname;
	int nRet = 1;
	_compilerConfiguration->cb_numberOfErrors += yyerr.yynerr;
	_compilerConfiguration->cb_numberOfWarnings += yyerr.yynwarn;
	if (yyerr.yynerr) {
		if (yyerr.errfp) {
			fclose(yyerr.errfp);
			yyerr.errfp = 0;
		}
		yyerr.failpt = 0;
		nRet = 0;
	} else {
		_compilerConfiguration->cb_numberOfFilesCompiled++;
	}
	return nRet;
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
		} else if (s[1] == 'b') {
			s += 2;
			nBase = 2;
		} else {
			s++;
			nBase = 8;
		}
	} 
	long long i = 0;
	int o;
	for (;;) {
		if (*s == '_') {
			s++;
			continue;
		}
		if (*s >= '0' && *s <= '9') {
			o = (*s - '0');
		} else if (nBase == 16) {
			if (*s >= 'A' && *s <= 'F') {
				o = (*s - 'A' + 10);
			} else if (*s >= 'a' && *s <= 'f') {
				o = (*s - 'a' + 10);
			} else {
				break;
			}
		} else {
			break;
		}
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
BINNUM	[01_]
OKTNUM	[0-7_]
NUM		[0-9_]
HEXNUM	[0-9a-fA-F_]
CH		[a-z_A-Z]
CHN		[0-9a-z_A-Z]+(-[0-9a-z_A-Z]+)*
QUOTED	\\.
NOS		[^"]
WS		[ \t]

%option yylineno

%s IN_COMMENT

%%
<INITIAL>"/*"	{	
	if (!_yyCurrentComment) {
		_yyCurrentComment = calloc(1, MAX_COMMENT_SIZE);
	} else {
		_yyCurrentComment[0] = 0;
	}
	BEGIN(IN_COMMENT); 
}
<INITIAL>-		return '-';
<INITIAL>\" {
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
<INITIAL>'([^'\\]|\\['\\abnrftv])'	{	/* Character literal */
			if (yytext[1] == '\\') {
				yylval.num = quotedchar(yytext[2]);
			} else {
				yylval.num = yytext[1];
			}
			return T_CHARACTER;
}
<IN_COMMENT>"*/"      {		BEGIN(INITIAL);	}
<IN_COMMENT>[^*\n]+   {
		if ((strlen(_yyCurrentComment) + strlen(yytext)) < MAX_COMMENT_SIZE-32) {
			strcat(_yyCurrentComment, yytext);	
		}
}
<IN_COMMENT>"*"       // eat the lone star
<IN_COMMENT>\n        {		if (_yyCurrentComment[0]) strcat(_yyCurrentComment, "\r\n");}
<INITIAL>0b{BINNUM}+ {
				yylval.num = number(yytext);
				return T_NUM;
			}
<INITIAL>0[xX]{HEXNUM}+ {
				yylval.num = number(yytext);
				return T_NUM;
			}
<INITIAL>0{OKTNUM}+	{
				yylval.num = number(yytext);
				return T_NUM;
			}
\.\.\.		return T_ELLIPSIS;
\.\.		return T_DOTDOT;
<INITIAL>{NUM}+(\.{NUM}+)?d? {
				int bFloat;
				numberOrFloat(yytext, &yylval, &bFloat);
				return bFloat ? T_FLOATING_POINT_NUMBER : T_NUM;
			}
#[^\n]*		{	;	}
\/\/[^\n]*	{	;	}
{WS}		{	;	}
\n 			{	;	}	/* skip comments and white space */
\r 			{	;	}	/* skip comments and white space */
\+\+		{	return T_PLUSPLUS; }
\-\-		{	return T_MINUSMINUS; }
@{CHN}+		{
				yylval.ident.s = yystralloc(yytext);
				yylval.ident.stringIsAlloced = 1;
				return T_ANNOTATION;
			}
{CHN}+		{	SYMBOL 	sym;
				char *	key;
				SYMBOL_TYPE t;

				sym = sym_find(_currentIdentifierContext, yytext,&key);
				if (key == 0) {
					key = yytext;
				}
				if (NULLSYM(sym)) {
retIdent:			yylval.ident.s = yystralloc(yytext);
					yylval.ident.stringIsAlloced = 1;
					return T_IDENT;
				}
				t = TYPEOF(sym);
				if (t == S_KEYWORD) {
					return sym.s.symbol.s_data.intValue;
				}

				if (t == S_TYPE_IDENTIFIER) {
					yylval.ident.type = VALUE_TYPE(sym);
					return T_TYPE_IDENTIFIER;
				}

				if (t == S_EDFUNC) {
					if (_bInNativeDefinition) {
						goto retIdent;
					}
					yylval.funcp = (void*)VALUE(sym);
					return T_FUNC;
				}

				if (t == S_ENUM) {
					yylval.num = function_enumValueFor((void*)VALUE(sym));
					return T_NUM;
				}
				
				if (t == S_CONSTANT) {
					if (_bDefiningConst) {
						goto retIdent;
					}
					switch (VALUE_TYPE(sym)) {
					case VT_NUMBER: 
						yylval.num = sym.s.symbol.s_data.longValue;
						return T_NUM;
					case VT_FLOAT:
						yylval.v.data.doubleValue = sym.s.symbol.s_data.doubleValue;
						return T_FLOATING_POINT_NUMBER;
					case VT_CHAR:
						yylval.num = sym.s.symbol.s_data.uchar;
						return T_CHARACTER;
					case VT_STRING:
						yylval.ident.stringIsAlloced = 1;
						yylval.ident.s = yystralloc((char *)VALUE(sym));
						return T_STRING;
					default: yyerror("Unsupported constant type %d", VALUE_TYPE(sym));
					}
				}
	
				if (t == S_VARIABLE || t == S_LOCAL_VARIABLE) {
					if (_bInHeader) {
						goto retIdent;
					}
					yylval.ident.stringIsAlloced = 0;
					yylval.ident.s = key;
					yylval.ident.type = VALUE_TYPE(sym);
					yylval.ident.heapIndex = sym.s_index;
					yylval.ident.isLocalVar = t == S_LOCAL_VARIABLE;
					return T_VARIABLE;
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
=\>			return T_ASSOC_ARROW;
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

