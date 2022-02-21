%{
/*
 * PARSER.Y
 *
 * Grammar
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 18.05.1991
 *									
 */
%}

%token T_TSTRING T_TINT T_TCHAR T_TLONG T_TFLOAT
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_OCTNUM T_CONST
%token T_TRUE T_FALSE T_BOOLEAN
%token T_FUNC T_MACRO T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELIF T_ELSE T_WHILE T_GOTO T_BRAEQ 
%token T_LOCAL T_SWITCH T_BREAK T_CONTINUE T_RETURN 
%token T_NUMVAR T_FLOATVAR T_STRINGVAR T_BOOLEANVAR
%token T_NMATCH
%token T_ASSIGN T_NE T_EQ
%token T_OR T_AND
%token T_DEFMACRO 
%token T_CMDSEQPREFIX

%nonassoc '<' T_NE T_EQ '>' '~' T_NMATCH
%left     T_OR T_AND
%left	'&' '|'
%left	'+' '-' '^'
%left	'*' '/' '%'

/* %right	T_NOTGLITCH */

%{

#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_STRING_LITERAL||type == C_STRINGVAR) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_LONG_LITERAL|| t == C_INTEGER_LITERAL || \
						 t == C_CHARACTER_LITERAL)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	MODE_OVERRIDE		0
# define	MODE_AUGMENT		1
# define	MODE_REPLACE		2

# define	REC_SPACE			24000

static BYTECODE_BUFFER  _bytecodeBuffer;
static void		*_lastfuncp;
static int		_override,
				_nparam,
				_breaklevel,
				_parno,
				_iflevel,
				_exprlevel,
				_stringflg,
				_kmod_control;
static COM_TEST	*_exprstack[MAXEXPR];

static 		void endmacro(void);
static 		void startmacro(void);
			void freeval(TYPEDVAL *v);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static 		void bytecode_beginTestOperation(void);
static 		void bytecode_endTestOperation(unsigned char type);
static 		void binaryOperation1Found(unsigned char type);
static unsigned char bytecode_emitPushValueInstruction(TYPEDVAL *vp);
static void 	bytecode_emitPushParameterInstruction(TYPEDVAL *vp);

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;
extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern char*    _yyCurrentComment;
extern void		yyerror(char* s, ...);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern TYPEDVAL	bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, TYPEDVAL *v1, TYPEDVAL *v2);
extern void bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL *v1, int nIncrement);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern TYPEDVAL bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

extern int yylex(void );
void 		bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name, int typ, GENERIC_DATA val);
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, intptr_t val);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static SYMBOL_TYPE symbolTypeForLiteralType(int nLiteralType) {
	switch(nLiteralType) {
	   case C_STRING_LITERAL: return S_STRING; 
	   case C_FLOAT_LITERAL: return S_FLOAT; 
	   case C_BOOLEAN_LITERAL: return S_BOOLEAN; 
	   default: return S_NUMBER; 
	}
}

static void makeInternalSymbol(const char* pszName, int sType, intptr_t tVal) {
	SYMBOL_TYPE nSymbolType = symbolTypeForLiteralType(sType);
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, nSymbolType, tVal);
	sym_makeInternalSymbol(_currentIdentifierContext, (char*) pszName, nSymbolType, (GENERIC_DATA) {.val = tVal});
}

%}

%%

resources		: 
			| { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			} resource resources

resource	: macro_definition		
			| constants


/*-- CONSTANTS --------------------------------------------------*/

constants		: T_CONST constdefs ';'

constdefs		: constdef
			| constdefs ',' constdef

constdef		: { _bDefiningConst = 1; } var_ident { _bDefiningConst = 0; } 
				T_ASSIGN constnum_or_string
			{
				sym_makeInternalSymbol(_currentIdentifierContext, $2.ident.s,
					($5.v.type == C_STRING_LITERAL) ? 
					S_CONSTSTRING : S_CONSTNUM, $5.v.data);
				freeval(&$5.v);
			}

var_ident:	T_IDENT 	{	$$.ident = $1.ident; }
			| var		{	$$.ident = $1.ident; }

var:		T_STRINGVAR 	{	$$.ident = $1.ident; }
			| T_NUMVAR 		{	$$.ident = $1.ident; }
			| T_FLOATVAR 	{	$$.ident = $1.ident; }

/*-- MACROS -----------------------------------------------------*/

macro_declaration: T_DEFMACRO T_IDENT {
				$$.ident = $2.ident;
			}
			| T_DEFMACRO T_FUNC {
				yyerror("Illegal attempt to redefine native method");
				$$.ident.s = 0;
				$$.ident.stringIsAlloced = 0;
			}

macrostart: { startmacro(); } macro_declaration '(' parameter_list ')' 
			  { $$.ident = $2.ident; }

parameter_list:
			| non_empty_pardecl

non_empty_pardecl: par_decl
			| par_decl ',' non_empty_pardecl

par_decl: type_ident var_ident {
				int typ;
				
				typ = ($1.type == C_STRING_LITERAL) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol(_currentIdentifierContext, $2.ident.s,typ,(GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, $2.ident.s,C_DEFINE_PARAMETER,typ,_nparam);
				_nparam++;
			}

macro_definition: macrostart block {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName($1.ident.s, -1, 1);
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",$1.ident.s);
					_compilerConfiguration->cb_insertNewMacro($1.ident.s, _yyCurrentComment, _bytecodeBuffer.bb_start,(int)(_bytecodeBuffer.bb_current-_bytecodeBuffer.bb_start));
				}
				freeitem(&$1.ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				endmacro();
			} 

block		: '{' var_decls stmntlist '}'

var_decls		:
			| var_decl var_decls

var_decl		: type_ident var_ident assgn ';'	{
				makeInternalSymbol($2.ident.s,
					$1.type,
					($1.type == C_STRING_LITERAL) ? (intptr_t)"" : 0);
				bytecode_emitAssignment(&_bytecodeBuffer, $2.ident.s,$3.v.type, $3.v.data);
				freeval(&$3.v);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				vname_count = 0;
			} | type_ident var_ident ';'	{
				if ($1.type == C_STRING_LITERAL) {
					makeInternalSymbol($2.ident.s, $1.type, (intptr_t)"");
				} else {
					makeInternalSymbol($2.ident.s, $1.type, (intptr_t)0);
				}
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				vname_count = 0;
			}

stmnt		:            	';'
			| call_expression ';'
			| assignment
			| if
			| while
			| case
			| break
			| continue
			| return
			| goto
			| block
			| increment_expression
			| label
			| error {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			}


assignment	: var assgn {
				bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s,$2.v.type, $2.v.data);
				vname_count = 0;
			} ';'

assgn		: T_ASSIGN arithmetic_expression { 	$$.v = $2.v;	}

arithmetic_expression: simple_expression	{ $$.v = $1.v; }
			| '(' simple_expression ')'		{ $$.v = $2.v; }

simple_expression: bterm					{ $$.v = $1.v; }
			| '~' arithmetic_expression 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_NOT,&$2.v,0); 	}

increment_expression: 
			value T_MINUSMINUS  { bytecode_emitIncrementExpression(&_bytecodeBuffer, &$1.v, -1); }
			|
			value T_PLUSPLUS { bytecode_emitIncrementExpression(&_bytecodeBuffer, &$1.v, 1); }

s_bterm: 	value { $$.v = $1.v; }
			| {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_FURET, (GENERIC_DATA){vname_count});
			} call_expression {
				if ($2.funcp && function_returnsString($2.funcp)) {
					$$.v.type = C_STRINGVAR;
				} else {
					$$.v.type = C_LONGVAR;
				}
				$$.v.data.val = (intptr_t)"__ret__";
				((char*)$$.v.data.val)[6] = '0' + vname_count++;
			}

bterm		: s_bterm 		{ $$.v = $1.v;	}
			| cast s_bterm		{
				if (macro_isParameterStringType($1.type) ==
				    macro_isParameterStringType($2.v.type)) {
					$$.v = $2.v;
				} else {
					$$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST,&$2.v,0);
					$$.v.type = ($1.type == C_STRING_LITERAL) ?
						C_STRINGVAR : (($1.type == C_FLOAT_LITERAL) ? C_FLOATVAR : C_LONGVAR);
				}
			}
			| '+' bterm { $$.v = $2.v }
			| '-' bterm { $$.v = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &$2.v, -1); }
			| bterm '&' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_AND,&$1.v,&$3.v); }
			| bterm '|' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_OR,&$1.v,&$3.v); }
			| bterm '+' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_ADD,&$1.v,&$3.v); }
			| bterm '^' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_XOR,&$1.v,&$3.v); }
			| bterm '-' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_SUB,&$1.v,&$3.v); }
			| bterm '*' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_MUL,&$1.v,&$3.v); }
			| bterm '/' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_DIV,&$1.v,&$3.v); }
			| bterm '%' bterm 	{ $$.v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_MOD,&$1.v,&$3.v); }
			| '(' bterm ')'	{ $$.v = $2.v; }

/*----- conditions ------------------------------------------*/

cond		: '(' { bytecode_beginTestOperation(); } expr { bytecode_endTestOperation(CT_BRACKETS); } ')'

expr		: optandorexpr
			| cond
			| '!' { bytecode_beginTestOperation(); } expr { bytecode_endTestOperation(CT_NOT); }

optandorexpr: { bytecode_beginTestOperation(); } op2 optandor { bytecode_endTestOperation($3.type); }

optandor	: { $$.type = CT_BRACKETS; }
			| and_or { $$.type = $1.type; binaryOperation1Found($1.type); } expr

and_or		: T_AND		{	$$.type = CT_AND;	}
			| T_OR		{	$$.type = CT_OR;	}

op2			: { bytecode_beginTestOperation(); }
			  term      { binaryOperation1Found($2.type);  }
			  optterm2  { bytecode_endTestOperation($4.type); }

optterm2		: { 	
				if (_stringflg)
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STRING_LITERAL,(GENERIC_DATA){.string=""});
				else
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_LONG_LITERAL,(GENERIC_DATA){0});
				$$.type = CT_NE;
			}
			| relop term 	{
				int flg = STRINGTYPE($2.type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				int nType = $1.type;
				$$.type = (nType | flg);
			}

relop		: '<'		{	$$.type = CT_LT;	}
			| '>'		{	$$.type = CT_GT;	}
			| T_EQ		{	$$.type = CT_EQ;	}
			| T_NE		{	$$.type = CT_NE;	}
			| string_match_operator {
				if (_stringflg == CT_NUM)
					yyerror("String match operator not allowed on numbers");
				$$.type = $1.type;
			}

string_match_operator: '~'	{	$$.type = CT_MATCH;			}
			| T_NMATCH	{	$$.type = CT_NMATCH;		}

term:		call_expression { $$.type = C_LONG_LITERAL; 	 	}
			| value		{	$$.type = bytecode_emitPushValueInstruction(&$1.v);	}

value:		T_NUMVAR {
				$$.v.type = C_LONGVAR;  
				$$.v.data.string = $1.ident.s;
			}
			| T_FLOATVAR 	{
				$$.v.type = C_FLOATVAR;  
				$$.v.data.string = $1.ident.s;
			}
			| T_STRINGVAR 	{
				$$.v.type = C_STRINGVAR;  
				$$.v.data.string  = $1.ident.s;
			}
			| T_BOOLEANVAR {
				$$.v.type = C_BOOLEANVAR;  
				$$.v.data.string  = $1.ident.s;
			}
			| string 	{
				$$.v.type = C_STRING_LITERAL;
				$$.v.data.string  = $1.ident.s;
			}
			| constexpr {	$$.v = $1.v;	}
			| T_IDENT {
				yyerror("Undefined identifier %s", $1.ident.s);
				$$.v.type = C_STRINGVAR;  
				$$.v.data.string  = $1.ident.s;
			}

string		: T_STRING	{	$$.ident = $1.ident;	}
			| string	T_STRING { 
				if (($$.ident.s = malloc(strlen($1.ident.s)+strlen($2.ident.s)+1)) != 0) {
					strcat(strcpy($$.ident.s,$1.ident.s),$2.ident.s);
					free($1.ident.s);
					free($2.ident.s);
				} else {
					yyerror("buffer overlow");
					$$.ident = $2.ident;
				}
			}

break		: T_BREAK opt_num ';'
			{	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

continue		: T_CONTINUE opt_num ';'
			{	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

return		: T_RETURN opt_num {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STOP,(GENERIC_DATA){.longValue=$2.num});
			}

opt_num		: 		{	$$.num = 1;	}
			| T_NUM	{	$$.num = $1.num; }

case			: T_SWITCH cond '{' caselabels '}'

caselabels	:
			| label block caselabels

label:		T_IDENT ':' {
				bytecode_createBranchLabel(&_bytecodeBuffer, $1.ident.s);
				freeitem(&$1.ident.s);
			}

goto:		T_GOTO T_IDENT {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&$2.ident.s);
			}
			| T_BRAEQ T_IDENT {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_EQ);
				freeitem(&$2.ident.s);
			} ';'

while		: T_WHILE {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			} cond {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

local_block: stmnt | '{' stmntlist '}'

if			: T_IF cond {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			} stmnt {
				_iflevel--;
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			} elseclause {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

elseclause	: 
			| T_ELSE stmnt 

stmntlist		:
			| stmnt stmntlist

call_expression: T_FUNC {
				// Native call
				_lastfuncp = $1.funcp;
				_parno = 0;
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_0FUNC,
							   (GENERIC_DATA){function_getIndexOfFunction($1.funcp)});
				$$.funcp = $1.funcp;
			} '(' parlist ')'
			| T_IDENT {
				_lastfuncp = 0;
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_MACRO,(GENERIC_DATA){.string = $1.ident.s});
				freeitem(&$1.ident.s);
				$$.funcp = 0;
			} '(' parlist ')'

parlist		:
			| nonemptypars

nonemptypars: par {
				bytecode_emitPushParameterInstruction(&$1.v);
			}
			| nonemptypars ',' par {
				bytecode_emitPushParameterInstruction(&$3.v);
			}
			| error {
				yyerror("illegal parameters for method call");
			}

par			: cast value {
				if (ISCONSTINT($2.v.type)) {
					if ($1.type == C_CHARACTER_LITERAL &&
					    ($2.v.data.val < 0 || $2.v.data.val > 255))
					    	yyerror("constant out of range");
					if ($1.type == C_STRING_LITERAL && $2.num) {
						yyerror("invalid string cast");
						$2.v.data.val = 0;
					}
				} else if (ISVARINT($2.v.type)) {
					if ($1.type == C_STRING_LITERAL) {
						yyerror("unsupported cast");
					}
				}
				$$.v.type = $1.type;
				$$.v.data.val = $2.num;
			}
			| value	 { $$.v = $1.v; }

cast			: '(' type_ident ')'	{	$$.type = $2.type;		}

type_ident	: T_TCHAR				{	$$.type = C_CHARACTER_LITERAL;	}
			| T_TINT				{	$$.type = C_INTEGER_LITERAL;	}
			| T_TLONG				{	$$.type = C_LONG_LITERAL;	}
			| T_TFLOAT				{	$$.type = C_FLOAT_LITERAL;	}
			| T_BOOLEAN				{	$$.type = C_BOOLEAN_LITERAL; }
			| T_TSTRING				{	$$.type = C_STRING_LITERAL;	}

boolean_literal: T_TRUE {
			$$.v.type = C_BOOLEAN_LITERAL; 
			$$.v.data.booleanValue = 1;
		} | T_FALSE {
			$$.v.type = C_BOOLEAN_LITERAL; 
			$$.v.data.booleanValue = 0;
		}

float_literal: T_FLOATING_POINT_NUMBER {
				$$.v.type = C_FLOAT_LITERAL; 
				$$.v.data.doubleValue = $1.v.data.doubleValue;
			}

integer_literal: T_NUM {
				$$.v.type = C_LONG_LITERAL; 
				$$.v.data.longValue  = $1.num;
			}

constnum_or_string:
			  constexpr 	{ $$.v = $1.v; }
			| T_STRING	{
				$$.v.type = C_STRING_LITERAL;
				$$.v.data.string = $1.ident.s;
			}

constexpr: integer_literal 	{ $$.v = $1.v;	}
			| float_literal { $$.v = $1.v; }
			| boolean_literal { $$.v = $1.v; }
			| constexpr '|' integer_literal {
				$$.v.type = $1.v.type;
				$$.v.data.longValue = $1.v.data.longValue | $3.v.data.longValue;
			}
%%

/*---------------------------------*/
/* binaryOperation1Found()					*/
/*---------------------------------*/
static void binaryOperation1Found(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (cp) {
		_stringflg = STRINGTYPE(type);
		cp->p2offset = (int)(_bytecodeBuffer.bb_current-(unsigned char *)cp);
	}
}

/*---------------------------------*/
/* bytecode_endTestOperation()				*/
/*---------------------------------*/
static void bytecode_endTestOperation(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (_exprlevel <= 0) {
		yyerror("bad expression");
	} else {
		cp->testop = type;
		cp->size = (int)(_bytecodeBuffer.bb_current - (unsigned char *)cp);
		_exprlevel--;
	}
}

/*---------------------------------*/
/* bytecode_beginTestOperation()					*/
/*---------------------------------*/
static void bytecode_beginTestOperation()
{
	if (_exprlevel >= MAXEXPR-1) {
		yyerror("expression too complex");
	} else {
		_exprlevel++;
		_exprstack[_exprlevel] = (COM_TEST*)_bytecodeBuffer.bb_current;
		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_TEST,(GENERIC_DATA){CT_BRACKETS});
	}
}

/*---------------------------------*/
/* bytecode_emitPushValueInstruction()					*/
/*---------------------------------*/
static unsigned char bytecode_emitPushValueInstruction(TYPEDVAL *vp)
{
	_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,vp->type,vp->data);
	unsigned char type = vp->type;
	freeval(vp);
	return type;
}

/*---------------------------------*/
/* bytecode_emitPushParameterInstruction()					*/
/*---------------------------------*/
static void bytecode_emitPushParameterInstruction(TYPEDVAL *vp)
{
	if (_lastfuncp && function_parameterIsFormStart(_lastfuncp,_parno))
		vp->type = C_FORMSTART;
	bytecode_emitPushValueInstruction(vp);
	_parno++;
}

/*---------------------------------*/
/* YYtrace()					*/
/*---------------------------------*/
#ifdef YYDEBUG
void YYtrace(char *fmt, ...)
{	va_list ap;
	static FILE *fp;

	if (fp == 0 && (fp = fopen("O:\\PROT","w")) == 0)
		return;
	va_start(fmt, ap);
	vfprintf(fp,fmt,ap);
	va_end(ap);
	fflush(fp);
}
#endif

/*---------------------------------*/
/* bytecode_emitGotoInstruction()					*/
/*---------------------------------*/
static char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(name,&_bytecodeBuffer,bratyp);
	return _bytecodeBuffer.bb_current;
}

/*---------------------------------*/
/* bytecode_generateAutoLabelNamePrefix()					*/
/*---------------------------------*/
static int bytecode_generateAutoLabelNamePrefix(char *prefix,int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	return bytecode_createBranchLabel(&_bytecodeBuffer, name);
}

/*---------------------------------*/
/* bytecode_destroyAutoLabelNamePrefix()					*/
/*---------------------------------*/
static void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	/*
	 * autolabel is not used any more: kill it
	 */
	bytecode_destroyLabelNamed(name);
}

/*---------------------------------*/
/* freeitem()					*/
/*---------------------------------*/
static void freeitem(char **p)
{
	if (*p) {
		free(*p);
		*p = 0;
	}
}

/*---------------------------------*/
/* freeval()					*/
/*---------------------------------*/
void freeval(TYPEDVAL *v)
{
	if ((v->type == C_STRING_LITERAL) && *v->data.string) {
		freeitem((char **)(&v->data.string));
	}
}

/*---------------------------------*/
/* startmacro()				*/
/*---------------------------------*/
static void startmacro(void)
{
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	if (!_bytecodeBuffer.bb_start) {
		_bytecodeBuffer.bb_end = ((_bytecodeBuffer.bb_start = malloc(REC_SPACE)) == 0) ? 
			_bytecodeBuffer.bb_start : _bytecodeBuffer.bb_start + REC_SPACE;
	}
	_bytecodeBuffer.bb_current = _bytecodeBuffer.bb_start; 
	_nparam = 0;
	_lastfuncp  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_exprlevel = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
}

/*---------------------------------*/
/* endmacro()					*/
/*---------------------------------*/
static void endmacro(void) {
	if (_bytecodeBuffer.bb_start) {
		free(_bytecodeBuffer.bb_start);
		_bytecodeBuffer.bb_start = 0;
		_bytecodeBuffer.bb_end = 0;
	}
	_currentIdentifierContext = sym_popContext(_currentIdentifierContext);
}

#if 0
/*---------------------------------*/
/* main()						*/
/*---------------------------------*/
main(int ac, char **av)
{

	av = av;
	ac = ac;

	printf("yyparse ret == %d\n",yyparse());
	return 0;
}
#endif

