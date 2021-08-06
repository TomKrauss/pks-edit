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

%token T_TSTRING T_TINT T_TCHAR T_TLONG
%token T_IDENT T_NUM T_STRING T_OCTNUM T_CONST
%token T_FUNC T_MACRO T_DOTDOT
%token T_IF T_ELIF T_ELSE T_WHILE T_GOTO T_BRAEQ 
%token T_LOCAL T_SWITCH T_BREAK T_CONTINUE T_RETURN 
%token T_NUMVAR T_STRINGVAR
%token T_NMATCH
%token T_SHIFT T_CTRL T_ALT T_LEFT T_MIDDLE T_RIGHT T_HAS_SELECTION
%token T_MENUPOPUP T_MENUSEPERATOR T_MENUBREAK T_MENUBARBREAK
%token T_ASSIGN T_NE T_EQ
%token T_OR T_AND
%token T_DEFMACRO T_DEFICON T_DEFALTPATH T_DEFCOMPILER T_DEFDOCTYPES
%token T_DEFMENUS T_DEFKEYS T_DEFMOUSE T_DEFTAGS
%token T_DEFESCAPES
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
#include "edfuncs.h"
#include "scanner.h"
#include "test.h"
#include "pkscc.h"
#include "sym.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_STRING1PAR||type == C_STRINGVAR) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_LONG1PAR|| t == C_INT1PAR || \
						 t == C_CHAR1PAR)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	MODE_OVERRIDE		0
# define	MODE_AUGMENT		1
# define	MODE_REPLACE		2

# define	REC_SPACE			24000

static unsigned char *_recspace;
unsigned char *_recp;
unsigned char *_recpend;
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
static 		char *gotolabel(char *prefix, int level, int bratyp);
static		int makelabel(char *prefix,int level);
static		void killabel(char *prefix, int level);
static 		void TestOp(void);
static 		void CloseTestOp(unsigned char type);
static 		int SetOverride(char *s);
static 		void BinOp1(unsigned char type);
static unsigned char PushVal(TYPEDVAL *vp);
static void 	PushParam(TYPEDVAL *vp);

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;

extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern void yyerror(char* s, ...);
unsigned char 	*AddComSeq(unsigned char *sp, unsigned char *spend,
				     	 unsigned char typ, intptr_t par);
int				FuncIdx(void *ep);
int 			bind_key(KEYCODE keycode, MACROREFTYPE typ, MACROREFIDX idx, int augment);
int 			bind_mouse(MOUSECODE code, MACROREFTYPE typ, MACROREFIDX idx, int flags, int augment);
int 			sym_sym_makeInternalSymbolbol(char *name, char ed_typ, intptr_t value);
int 			IsFormStart(void *ep,int parno);
int 			key_switchtotable(char *name);
int 			mouse_switchtotable(char *name);
void 		mouse_destroyMouseBindings(void);
KEYCODE 		key2code(char *K, int control);
void 		key_destroytable(char *name);
TYPEDVAL		PushBinop(int opd_typ, TYPEDVAL *v1, 
					TYPEDVAL *v2);
extern int yylex(void );
void 		PushAssign(char *name, int typ, intptr_t val);
void 		PushCreateVariable(char *name, int typ, long val);

char		 	*GotoLabel(char *name, char *sp, char *spend, int typ);
int			MakeLabel(char *name, COM_GOTO *recp);
void 		CloseLabelList(void);
void 		KillLabel(char *name);
char			*LabelAutoName(char *prefix, int num);
int 			IsStringFunc(void *ep);
void 		menu_initializeDefinition(char *szMenu);
int 			menu_addMenuMacroItem(char *pszString, int menutype, 
				MACROREFTYPE mactype, MACROREFTYPE macidx);
%}

%%

resources		: 
			| { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			} resource resources

resource	: macros		
			| icons	
			| menus
			| keys		
			| mouses
			| constants

inherit		:
			| ':' T_IDENT

/*!!!!!!
 * the followin stuff is commented out, because it is unneeded currently
 *!!!!!!
 */
/*-- TAGS -------------------------------------------

tags			: T_DEFTAGS T_IDENT inherit tagdefs ';'

tagdefs		:
			| T_STRING
			| T_STRING ',' tagdefs
			| error { yyerror("invalid tag definition"); }

/*-- ESCAPES -------------------------------------------

escapes		: T_DEFESCAPES T_IDENT inherit escapedefs ';'

escapedefs	:
			| escape
			| escapedefs ',' escape

escape		: T_STRING T_ASSIGN T_STRING
			{
				bind_escape(*$1.s, $3.s);
				freeval(&$1.s);
				freeval(&$3.s);
			}
			| error { yyerror("invalid escape definition"); }


/*-- DOCTYPE -------------------------------------------

doctypes		: T_DEFDOCTYPES docdefs ';'

docdefs		:
			| doctype ',' docdefs

doctype		: T_IDENT T_ASSIGN T_STRING strlist
			{
				macro_showStatus("Defining documenttype %s",$1.s);
				freeitem(&$1.s);
				freeitem(&$3.s);
			}
			| error { yyerror("document type definition"); }

strlist		: '{' strings '}'

strings		:
			| T_STRING strings

/*-- MENUS -------------------------------------------*/

menus		: T_DEFMENUS T_IDENT inherit 
			{
				menu_initializeDefinition($2.s);
				freeitem(&$2.s);
			}
			menudefs ';'

menudefs		:
			| submenu
			| submenu ',' menudefs

submenu		: T_MENUPOPUP T_STRING 
			{
				macro_showStatus("Defining sub-menu %s",$2.s);
				menu_addMenuMacroItem($2.s, UM_POPUP, 0, 0);
				freeitem(&$2.s);
			}
			menuconnect '{' menuitems '}'
			{
				menu_addMenuMacroItem((char*)0, UM_ENDPOPUP, 0, 0);
			}

menuconnect	:
			| ':' T_STRING

menuitems		: 
			| menuitem
			| menuitems ',' menuitem

menuitem		: T_MENUSEPERATOR
			{
				menu_addMenuMacroItem((char *)0, UM_SEPARATOR, 0, 0);
			}
			| T_MENUBARBREAK
			| T_MENUBREAK
			| T_STRING menuoptions T_ASSIGN cmd_or_macro
			{
				menu_addMenuMacroItem($1.s, UM_ITEM, $4.macref.typ, $4.macref.index);
				freeitem(&$1.s);
			}
			| submenu
			| error { yyerror("invalid menu item"); }

menuoptions	:
			| menuoptions ',' T_IDENT  
			/* GRAYED */

/*-- MOUSE -------------------------------------------*/

mouses		: opt_ident T_DEFMOUSE T_IDENT	{
				macro_showStatus("Compile mouse table %s",$3.s);
				mouse_switchtotable($3.s);
				if (SetOverride($1.s) == MODE_OVERRIDE) {
					mouse_destroyMouseBindings();
				}
				freeitem(&$3.s);
			}
			inherit mousedefs ';'

mousedefs		:
			| mouse
			| mouse ',' mousedefs

mouse		: mouse_button T_ASSIGN opt_plus cmd_or_macro
			{
				if ($1.mousecode.button && 
				    $3.macref.index != (MACROREFIDX)-1) {
					bind_mouse($1.mousecode, $4.macref.typ, $4.macref.index,
						$3.num, _override == MODE_AUGMENT);
				}
			}
			| error { yyerror("mouse binding"); }

opt_plus		:		{ $$.num = 0; }
			| '+'	{ $$.num = 0; }

m_button		: T_LEFT 	{ $$.num = MBUT_L; }
			| T_RIGHT	{ $$.num = MBUT_R; }
			| T_MIDDLE { $$.num = MBUT_M; }

buttons		:  m_button 		    { $$.num = $1.num; 		  }
			|  m_button '+' buttons { $$.num = $1.num | $3.num; }

mouse_button	: kmod_list buttons T_NUM
			{
				$$.mousecode.shift = $1.num >> 8;
				$$.mousecode.button = $2.num;
				$$.mousecode.nclicks = $3.num;
			}

/*-- KEYS -------------------------------------------*/

keys			: opt_ident T_DEFKEYS T_IDENT	  {
				macro_showStatus("Compile key table %s",$3.s);
				key_switchtotable($3.s);
				if (SetOverride($1.s) == MODE_OVERRIDE) {
					key_destroytable($3.s);
				}
				freeitem(&$3.s);
			}
			inherit keydefs ';'

keydefs		:
			| key 
			| keydefs ',' key

key			: key_ident T_ASSIGN cmd_or_macro
			{
				if ($1.num != K_INVALID && 
				    $3.macref.index != (MACROREFIDX) -1) {
					if (!bind_key((KEYCODE)$1.num, $3.macref.typ, 
							   $3.macref.index,
							   _override == MODE_AUGMENT)) {
						yyerror("cannot bind key 0x%lx",$1.num);
					}
				}
			}
			| error { yyerror("key binding"); }

ident_or_num:	T_IDENT
			{
				if (($$.num = key2code($1.s,_kmod_control)) == K_INVALID)
					yyerror("bad key %s",$1.s);
				freeitem(&$1.s);
			} 
			| T_NUM	{	$$.num = $1.num|_kmod_control; 	}

key_ident		: kmod_list {	_kmod_control = $1.num; } ident_or_num
			{ 
				$$.num = $3.num;
			}

kmod_list		:  {	$$.num = 0; }
			|  k_modifier '+' kmod_list { $$.num = $1.num | $3.num; }

k_modifier	: T_SHIFT { $$.num = /* K_SHIFT */ 0x300; }
			| T_CTRL	{ $$.num = K_CONTROL; }
			| T_ALT	{ $$.num = K_ALTERNATE; }
			| T_HAS_SELECTION { $$.num = K_HAS_SELECTION; }

/*-- ICONS -----------------------------------------------------*/

icons		: T_DEFICON icondefs ';'

icondefs		:
			| icon ',' icondefs

icon			: T_IDENT T_IDENT T_IDENT T_ASSIGN T_IDENT
			{	
				macro_showStatus("Defining icon for macro %s",$5.s);
				freeitem(&$1.s);
				freeitem(&$2.s);
				freeitem(&$3.s);
				freeitem(&$5.s);
			}
			| error { yyerror("icon definition"); }

/*-- CONSTANTS --------------------------------------------------*/

constants		: T_CONST constdefs ';'

constdefs		: constdef
			| constdefs ',' constdef

constdef		: { _bDefiningConst = 1; } var_ident { _bDefiningConst = 0; } 
				T_ASSIGN constnum_or_string
			{
				sym_makeInternalSymbol($2.s,
					($5.v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, $5.v.val);
				freeval(&$5.v);
			}

var_ident		: T_IDENT 	{	$$.s = $1.s; }
			| var		{	$$.s	= $1.s; }

var			: T_STRINGVAR 	{	$$.s = $1.s; }
			| T_NUMVAR 	{	$$.s = $1.s; }

/*-- MACROS -----------------------------------------------------*/

macrostart	: { startmacro(); } T_DEFMACRO T_IDENT '(' par_decls ')' 
			  { $$.s = $3.s; }

par_decls		:
			| non_empty_pardecl

non_empty_pardecl: par_decl
			| par_decl ',' non_empty_pardecl

par_decl		: type_ident var_ident {
				int typ;
				
				typ = ($1.type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol($2.s,typ,_nparam);
				PushCreateVariable($2.s,typ,_nparam);
				_nparam++;
			}

macros		:  macrostart opt_string block
			{
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName($1.s, -1);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					macro_showStatus("Defining macro %s",$1.s);
					macro_insertNewMacro($1.s,$2.s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&$2.s);
				freeitem(&$1.s);
				CloseLabelList();
				endmacro();
			} 

opt_string	:			{	$$.s = 0; }
			| T_STRING	{	$$.s = $1.s; }

opt_ident	:				{	$$.s = ""; }
			| T_IDENT		{	$$.s = $1.s; }

block		: '{' var_decls stmntlist '}'

var_decls		:
			| var_decl var_decls

var_decl		: type_ident var_ident assgn ';'	{
				sym_makeInternalSymbol($2.s,
					($1.type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					($1.type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign($2.s,$3.v.type,(intptr_t)$3.v.val);
				freeval(&$3.v);
				vname_count = 0;
			} | type_ident var_ident ';'	{
				if ($1.type == C_STRING1PAR) {
					sym_makeInternalSymbol($2.s, S_STRING, (intptr_t)"");
					PushAssign($2.s, C_STRING1PAR, (intptr_t)"");
				} else {
					sym_makeInternalSymbol($2.s, S_NUMBER, (intptr_t)0);
					PushAssign($2.s,C_LONG1PAR,(intptr_t)0);
				}
				vname_count = 0;
			}

stmnt		:            	';'
			| fcall      	';'
			| assignment	{ vname_count = 0; } ';'
			| if
			| while
			| case
			| break
			| continue
			| return
			| gotolabel
			| block
			| label
			| error			{ yyerror("invalid statement"); }


assignment	: var assgn 		{
				PushAssign($1.s,$2.v.type,(intptr_t)$2.v.val);
			}

assgn		: T_ASSIGN arith 	{ 	$$.v = $2.v;	}

arith		: aexpr			{ $$.v = $1.v; }
			| '(' aexpr ')'	{ $$.v = $2.v; }

aexpr		: bterm			{ $$.v = $1.v; }
			| '~' arith 		{ $$.v = PushBinop(BIN_NOT,&$2.v,0); 	}

s_bterm: 		value			{ $$.v = $1.v; }
			| {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			} fcall			{
				if ($2.funcp && IsStringFunc($2.funcp)) {
					$$.v.type = C_STRINGVAR;
				} else {
					$$.v.type = C_LONGVAR;
				}
				$$.v.val = (intptr_t)"__ret__";
				((char*)$$.v.val)[6] = '0' + vname_count++;
			}

bterm		: s_bterm 		{ $$.v = $1.v;	}
			| cast s_bterm		{
				if (macro_isParameterStringType($1.type) ==
				    macro_isParameterStringType($2.v.type)) {
					$$.v = $2.v;
				} else {
					$$.v = PushBinop(BIN_CONVERT,&$2.v,0);
					$$.v.type = ($1.type == C_STRING1PAR) ?
						C_STRINGVAR : C_LONGVAR;
				}
			}
			| bterm '&' bterm 	{ $$.v = PushBinop(BIN_AND,&$1.v,&$3.v); }
			| bterm '|' bterm 	{ $$.v = PushBinop(BIN_OR,&$1.v,&$3.v); }
			| bterm '+' bterm 	{ $$.v = PushBinop(BIN_ADD,&$1.v,&$3.v); }
			| bterm '^' bterm 	{ $$.v = PushBinop(BIN_XOR,&$1.v,&$3.v); }
			| bterm '-' bterm 	{ $$.v = PushBinop(BIN_SUB,&$1.v,&$3.v); }
			| bterm '*' bterm 	{ $$.v = PushBinop(BIN_MUL,&$1.v,&$3.v); }
			| bterm '/' bterm 	{ $$.v = PushBinop(BIN_DIV,&$1.v,&$3.v); }
			| bterm '%' bterm 	{ $$.v = PushBinop(BIN_MOD,&$1.v,&$3.v); }
			| '(' bterm ')'	{ $$.v = $2.v; }

/*----- conditions ------------------------------------------*/

cond			: '(' { TestOp(); } expr { CloseTestOp(CT_BRACKETS); } ')'

expr			: optandorexpr
			| cond
			| '!' { TestOp(); } expr { CloseTestOp(CT_NOT); }

optandorexpr	: { TestOp(); } op2 optandor { CloseTestOp($3.type); }

optandor		: { $$.type = CT_BRACKETS; }
			| and_or { $$.type = $1.type; BinOp1($1.type); } expr

and_or		: T_AND		{	$$.type = CT_AND;	}
			| T_OR		{	$$.type = CT_OR;	}

op2			: { TestOp(); }
			  term      { BinOp1($2.type);  }
			  optterm2  { CloseTestOp($4.type); }

optterm2		: { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				$$.type = CT_NE;
			}
			| relop term 	{
				int flg = STRINGTYPE($2.type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				$$.type = $1.type | flg;
			}

relop		: '<'		{	$$.type = CT_LT;	}
			| '>'		{	$$.type = CT_GT;	}
			| T_EQ		{	$$.type = CT_EQ;	}
			| T_NE		{	$$.type = CT_NE;	}
			| strmatchop	{
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				$$.type = $1.type;
			}

strmatchop	: '~'		{	$$.type = CT_MATCH;			}
			| T_NMATCH	{	$$.type = CT_NMATCH;		}

term			: fcall		{	$$.type = C_LONG1PAR; 	 	}
			| value		{	$$.type = PushVal(&$1.v);	}

value		: T_NUMVAR 	{
				$$.v.type = C_LONGVAR;  
				$$.v.val  = (intptr_t) $1.s;
			}
			| T_STRINGVAR 	{
				$$.v.type = C_STRINGVAR;  
				$$.v.val  = (intptr_t) $1.s;
			}
			| string 	{
				$$.v.type = C_STRING1PAR;
				$$.v.val  = (intptr_t) $1.s;
			}
			| constexpr 	{	$$.v = $1.v;	}

string		: T_STRING	{	$$.s = $1.s;	}
			| string	T_STRING { 
				if (($$.s = malloc(strlen($1.s)+strlen($2.s)+1)) != 0) {
					strcat(strcpy($$.s,$1.s),$2.s);
					free($1.s);
					free($2.s);
				} else {
					yyerror("buffer overlow");
					$$.s = $2.s;
				}
			}

break		: T_BREAK opt_num ';'
			{	gotolabel(lendid,_breaklevel-$2.num,BRA_ALWAYS); }

continue		: T_CONTINUE opt_num ';'
			{	gotolabel(lstartid,_breaklevel-$2.num,BRA_ALWAYS); }

return		: T_RETURN opt_num {
				_recp = AddComSeq(_recp,_recpend,C_STOP,$2.num);
			}

opt_num		: 		{	$$.num = 1;	}
			| T_NUM	{	$$.num = $1.num; }

case			: T_SWITCH cond '{' caselabels '}'

caselabels	:
			| label block caselabels

label		: T_IDENT ':' {
				MakeLabel($1.s,(COM_GOTO *)_recp);
				freeitem(&$1.s);
			}

gotolabel		: T_GOTO T_IDENT {
				_recp = GotoLabel($2.s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&$2.s);
			}
			| T_BRAEQ T_IDENT {
				_recp = GotoLabel($2.s,_recp,_recpend,BRA_EQ);
				freeitem(&$2.s);
			}

while		: T_WHILE {
				makelabel(lstartid,_breaklevel);
			} cond {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			} stmnt {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			}

if			: T_IF cond {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			} stmnt {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			} elseclause {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			}

elseclause	: 
			| T_ELSE stmnt 

stmntlist		:
			| stmnt stmntlist

fcall		: T_FUNC {
				_lastfuncp = $1.funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx($1.funcp));
				$$.funcp = $1.funcp;
			} '(' parlist ')'
			| T_IDENT {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)$1.s);
				freeitem(&$1.s);
				$$.funcp = 0;
			} '(' parlist ')'

parlist		:
			| nonemptypars

nonemptypars	: par {
				PushParam(&$1.v);
			}
			| nonemptypars ',' par {
				PushParam(&$3.v);
			}
			| error {
				yyerror("illegal parameters");
			}

par			: cast value {
				if (ISCONSTINT($2.v.type)) {
					if ($1.type == C_CHAR1PAR &&
					    ($2.v.val < 0 || $2.v.val > 255))
					    	yyerror("constant out of range");
					if ($1.type == C_STRING1PAR && $2.num) {
						yyerror("invalid string cast");
						$2.v.val = 0;
					}
				} else if (ISVARINT($2.v.type)) {
					if ($1.type == C_STRING1PAR) {
						yyerror("unsupported cast");
					}
				}
				$$.v.type = $1.type;
				$$.v.val = $2.num;
			}
			| value	 { $$.v = $1.v; }

cast			: '(' type_ident ')'	{	$$.type = $2.type;		}

type_ident	: T_TCHAR				{	$$.type = C_CHAR1PAR;	}
			| T_TINT				{	$$.type = C_INT1PAR;	}
			| T_TLONG				{	$$.type = C_LONG1PAR;	}
			| T_TSTRING			{	$$.type = C_STRING1PAR;	}

cmd_or_macro	: T_IDENT	 {
				$$.macref.typ = CMD_MACRO; 
				if (($$.macref.index = macro_getInternalIndexByName($1.s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",$1.s);
				}
				freeitem(&$1.s);
			}
			| T_CMDSEQPREFIX T_IDENT {
				$$.macref.typ = CMD_CMDSEQ; 
				$$.macref.index = macro_getCmdIndexByName($2.s); 
				freeitem(&$2.s);
			}

constval		: T_NUM {
				$$.v.type = C_LONG1PAR; 
				$$.v.val  = $1.num;
			}

constnum_or_string:
			  constexpr 	{ $$.v = $1.v; }
			| T_STRING	{
				$$.v.type = C_STRING1PAR;
				$$.v.val  = (intptr_t) $1.s;
			}

constexpr		: constval 	{ $$.v = $1.v;	}
			| constexpr '|' constval {
				$$.v.type = $1.v.type;
				$$.v.val = $1.v.val | $3.v.val;
			}
%%

/*---------------------------------*/
/* BinOp1()					*/
/*---------------------------------*/
static void BinOp1(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (cp) {
		_stringflg = STRINGTYPE(type);
		cp->p2offset = (int)(_recp-(unsigned char *)cp);
	}
}

/*---------------------------------*/
/* CloseTestOp()				*/
/*---------------------------------*/
static void CloseTestOp(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (_exprlevel <= 0) {
		yyerror("bad expression");
	} else {
		cp->testop = type;
		cp->size = (int)(_recp - (unsigned char *)cp);
		_exprlevel--;
	}
}

/*---------------------------------*/
/* TestOp()					*/
/*---------------------------------*/
static void TestOp()
{
	if (_exprlevel >= MAXEXPR-1) {
		yyerror("expression too complex");
	} else {
		_exprlevel++;
		_exprstack[_exprlevel] = (COM_TEST*)_recp;
		_recp = AddComSeq(_recp,_recpend,C_TEST,CT_BRACKETS);
	}
}

/*---------------------------------*/
/* PushVal()					*/
/*---------------------------------*/
static unsigned char PushVal(TYPEDVAL *vp)
{
	_recp = AddComSeq(_recp,_recpend,vp->type,vp->val);
	freeval(vp);
	return vp->type;
}

/*---------------------------------*/
/* PushParam()					*/
/*---------------------------------*/
static void PushParam(TYPEDVAL *vp)
{
	if (_lastfuncp && IsFormStart(_lastfuncp,_parno))
		vp->type = C_FORMSTART;
	PushVal(vp);
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
/* gotolabel()					*/
/*---------------------------------*/
static char *gotolabel(char *prefix, int level, int bratyp)
{	char *name = LabelAutoName(prefix,level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		_recp = GotoLabel(name,_recp,_recpend,bratyp);
	return _recp;
}

/*---------------------------------*/
/* makelabel()					*/
/*---------------------------------*/
static int makelabel(char *prefix,int level)
{	char *name = LabelAutoName(prefix,level);

	return MakeLabel(name,(COM_GOTO *)_recp);
}

/*---------------------------------*/
/* killabel()					*/
/*---------------------------------*/
static void killabel(char *prefix, int level)
{	char *name = LabelAutoName(prefix,level);

	/*
	 * autolabel is not used any more: kill it
	 */
	KillLabel(name);
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

/*--------------------------------------------------------------------------
 * SetOverride()
 */
static int SetOverride(char *s)
{
	_override = MODE_REPLACE;
	if (!*s || strcmp(s,"replace") == 0) {
		;
	} else if (strcmp(s,"augment") == 0) {
		_override = MODE_AUGMENT;
	} else if (strcmp(s,"override") == 0) {
		_override = MODE_OVERRIDE;
	} else {
		yyerror("invalid override mode %s",s);
	}
	return _override;
}

/*---------------------------------*/
/* freeval()					*/
/*---------------------------------*/
void freeval(TYPEDVAL *v)
{
	if ((v->type == C_STRING1PAR /*||
	    v->type == C_STRINGVAR  ||
	    v->type == C_LONGVAR*/) &&
	    *(char*)v->val) {
		freeitem((char **)(&v->val));
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
	if (!_recspace) {
		_recpend = ((_recspace = malloc(REC_SPACE)) == 0) ? 
			_recspace : _recspace + REC_SPACE;
	}
	_nparam = 0;
	_recp = _recspace;
	_lastfuncp  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_exprlevel = 0;
}

/*---------------------------------*/
/* endmacro()					*/
/*---------------------------------*/
static void endmacro(void)
{
	if (_recspace) {
		free(_recspace);
		_recspace = 0;
		_recpend = 0;
	}
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

