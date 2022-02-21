
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "Parser.y"

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

/* Line 189 of yacc.c  */
#line 40 "Parser.y"


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



/* Line 189 of yacc.c  */
#line 196 "Parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_TSTRING = 258,
     T_TINT = 259,
     T_TCHAR = 260,
     T_TLONG = 261,
     T_TFLOAT = 262,
     T_IDENT = 263,
     T_NUM = 264,
     T_FLOATING_POINT_NUMBER = 265,
     T_STRING = 266,
     T_OCTNUM = 267,
     T_CONST = 268,
     T_TRUE = 269,
     T_FALSE = 270,
     T_BOOLEAN = 271,
     T_FUNC = 272,
     T_MACRO = 273,
     T_DOTDOT = 274,
     T_PLUSPLUS = 275,
     T_MINUSMINUS = 276,
     T_IF = 277,
     T_ELIF = 278,
     T_ELSE = 279,
     T_WHILE = 280,
     T_GOTO = 281,
     T_BRAEQ = 282,
     T_LOCAL = 283,
     T_SWITCH = 284,
     T_BREAK = 285,
     T_CONTINUE = 286,
     T_RETURN = 287,
     T_NUMVAR = 288,
     T_FLOATVAR = 289,
     T_STRINGVAR = 290,
     T_BOOLEANVAR = 291,
     T_NMATCH = 292,
     T_ASSIGN = 293,
     T_NE = 294,
     T_EQ = 295,
     T_OR = 296,
     T_AND = 297,
     T_DEFMACRO = 298,
     T_CMDSEQPREFIX = 299
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 282 "Parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   430

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  81
/* YYNRULES -- Number of rules.  */
#define YYNRULES  160
/* YYNRULES -- Number of states.  */
#define YYNSTATES  241

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,    55,    48,     2,
      58,    59,    53,    50,    57,    51,     2,    54,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    63,    56,
      45,     2,    46,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    60,    49,    61,    47,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    17,    19,
      23,    24,    25,    31,    33,    35,    37,    39,    41,    44,
      47,    48,    54,    55,    57,    61,    63,    66,    69,    74,
      75,    78,    83,    87,    89,    92,    94,    96,    98,   100,
     102,   104,   106,   108,   110,   112,   114,   116,   117,   122,
     125,   127,   131,   133,   136,   139,   142,   144,   145,   148,
     150,   153,   156,   159,   163,   167,   171,   175,   179,   183,
     187,   191,   195,   196,   197,   203,   205,   207,   208,   212,
     213,   217,   218,   219,   223,   225,   227,   228,   229,   234,
     235,   238,   240,   242,   244,   246,   248,   250,   252,   254,
     256,   258,   260,   262,   264,   266,   268,   270,   272,   275,
     279,   283,   286,   287,   289,   295,   296,   300,   303,   306,
     307,   312,   313,   314,   320,   322,   326,   327,   328,   335,
     336,   339,   340,   343,   344,   350,   351,   357,   358,   360,
     362,   366,   368,   371,   373,   377,   379,   381,   383,   385,
     387,   389,   391,   393,   395,   397,   399,   401,   403,   405,
     407
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      65,     0,    -1,    -1,    -1,    66,    67,    65,    -1,    81,
      -1,    68,    -1,    13,    69,    56,    -1,    70,    -1,    69,
      57,    70,    -1,    -1,    -1,    71,    73,    72,    38,   143,
      -1,     8,    -1,    74,    -1,    35,    -1,    33,    -1,    34,
      -1,    43,     8,    -1,    43,    17,    -1,    -1,    77,    75,
      58,    78,    59,    -1,    -1,    79,    -1,    79,    57,    80,
      -1,    80,    -1,   139,    73,    -1,    76,    82,    -1,    60,
      83,   131,    61,    -1,    -1,    84,    83,    -1,   139,    73,
      88,    56,    -1,   139,    73,    56,    -1,    56,    -1,   132,
      56,    -1,    86,    -1,   127,    -1,   123,    -1,   118,    -1,
     114,    -1,   115,    -1,   116,    -1,   121,    -1,    82,    -1,
      91,    -1,   120,    -1,     1,    -1,    -1,    74,    88,    87,
      56,    -1,    38,    89,    -1,    90,    -1,    58,    90,    59,
      -1,    94,    -1,    47,    89,    -1,   112,    21,    -1,   112,
      20,    -1,   112,    -1,    -1,    93,   132,    -1,    92,    -1,
     138,    92,    -1,    50,    94,    -1,    51,    94,    -1,    94,
      48,    94,    -1,    94,    49,    94,    -1,    94,    50,    94,
      -1,    94,    52,    94,    -1,    94,    51,    94,    -1,    94,
      53,    94,    -1,    94,    54,    94,    -1,    94,    55,    94,
      -1,    58,    94,    59,    -1,    -1,    -1,    58,    96,    98,
      97,    59,    -1,   100,    -1,    95,    -1,    -1,    62,    99,
      98,    -1,    -1,   101,   105,   102,    -1,    -1,    -1,   104,
     103,    98,    -1,    42,    -1,    41,    -1,    -1,    -1,   106,
     111,   107,   108,    -1,    -1,   109,   111,    -1,    45,    -1,
      46,    -1,    40,    -1,    39,    -1,   110,    -1,    47,    -1,
      37,    -1,   132,    -1,   112,    -1,    33,    -1,    34,    -1,
      35,    -1,    36,    -1,   113,    -1,   144,    -1,     8,    -1,
      11,    -1,   113,    11,    -1,    30,   117,    56,    -1,    31,
     117,    56,    -1,    32,   117,    -1,    -1,     9,    -1,    29,
      95,    60,   119,    61,    -1,    -1,   120,    82,   119,    -1,
       8,    63,    -1,    26,     8,    -1,    -1,    27,     8,   122,
      56,    -1,    -1,    -1,    25,   124,    95,   125,   126,    -1,
      85,    -1,    60,   131,    61,    -1,    -1,    -1,    22,    95,
     128,    85,   129,   130,    -1,    -1,    24,    85,    -1,    -1,
      85,   131,    -1,    -1,    17,   133,    58,   135,    59,    -1,
      -1,     8,   134,    58,   135,    59,    -1,    -1,   136,    -1,
     137,    -1,   136,    57,   137,    -1,     1,    -1,   138,   112,
      -1,   112,    -1,    58,   139,    59,    -1,     5,    -1,     4,
      -1,     6,    -1,     7,    -1,    16,    -1,     3,    -1,    14,
      -1,    15,    -1,    10,    -1,     9,    -1,   144,    -1,    11,
      -1,   142,    -1,   141,    -1,   140,    -1,   144,    49,   142,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   145,   145,   146,   146,   152,   153,   158,   160,   161,
     163,   163,   163,   172,   173,   175,   176,   177,   181,   184,
     190,   190,   193,   194,   197,   199,   201,   210,   225,   227,
     228,   230,   240,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   270,   270,   275,
     277,   278,   280,   281,   284,   286,   288,   289,   289,   301,
     302,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   326,   326,   326,   328,   329,   330,   330,   332,
     332,   334,   335,   335,   337,   338,   340,   341,   340,   344,
     351,   359,   360,   361,   362,   363,   369,   370,   372,   373,
     375,   379,   383,   387,   391,   395,   396,   402,   403,   414,
     417,   420,   424,   425,   427,   429,   430,   432,   437,   441,
     441,   446,   448,   446,   459,   459,   461,   464,   461,   474,
     475,   477,   478,   480,   480,   488,   488,   495,   496,   498,
     501,   504,   508,   525,   527,   529,   530,   531,   532,   533,
     534,   536,   539,   544,   549,   555,   556,   561,   562,   563,
     564
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_TFLOAT", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_OCTNUM", "T_CONST", "T_TRUE", "T_FALSE", "T_BOOLEAN",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF",
  "T_ELIF", "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_LOCAL",
  "T_SWITCH", "T_BREAK", "T_CONTINUE", "T_RETURN", "T_NUMVAR",
  "T_FLOATVAR", "T_STRINGVAR", "T_BOOLEANVAR", "T_NMATCH", "T_ASSIGN",
  "T_NE", "T_EQ", "T_OR", "T_AND", "T_DEFMACRO", "T_CMDSEQPREFIX", "'<'",
  "'>'", "'~'", "'&'", "'|'", "'+'", "'-'", "'^'", "'*'", "'/'", "'%'",
  "';'", "','", "'('", "')'", "'{'", "'}'", "'!'", "':'", "$accept",
  "resources", "$@1", "resource", "constants", "constdefs", "constdef",
  "$@2", "$@3", "var_ident", "var", "macro_declaration", "macrostart",
  "$@4", "parameter_list", "non_empty_pardecl", "par_decl",
  "macro_definition", "block", "var_decls", "var_decl", "stmnt",
  "assignment", "$@5", "assgn", "arithmetic_expression",
  "simple_expression", "increment_expression", "s_bterm", "$@6", "bterm",
  "cond", "$@7", "$@8", "expr", "$@9", "optandorexpr", "$@10", "optandor",
  "@11", "and_or", "op2", "$@12", "$@13", "optterm2", "relop",
  "string_match_operator", "term", "value", "string", "break", "continue",
  "return", "opt_num", "case", "caselabels", "label", "goto", "$@14",
  "while", "$@15", "$@16", "local_block", "if", "$@17", "$@18",
  "elseclause", "stmntlist", "call_expression", "@19", "@20", "parlist",
  "nonemptypars", "par", "cast", "type_ident", "boolean_literal",
  "float_literal", "integer_literal", "constnum_or_string", "constexpr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,    60,    62,   126,    38,   124,
      43,    45,    94,    42,    47,    37,    59,    44,    40,    41,
     123,   125,    33,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    64,    65,    66,    65,    67,    67,    68,    69,    69,
      71,    72,    70,    73,    73,    74,    74,    74,    75,    75,
      77,    76,    78,    78,    79,    79,    80,    81,    82,    83,
      83,    84,    84,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    87,    86,    88,
      89,    89,    90,    90,    91,    91,    92,    93,    92,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    96,    97,    95,    98,    98,    99,    98,   101,
     100,   102,   103,   102,   104,   104,   106,   107,   105,   108,
     108,   109,   109,   109,   109,   109,   110,   110,   111,   111,
     112,   112,   112,   112,   112,   112,   112,   113,   113,   114,
     115,   116,   117,   117,   118,   119,   119,   120,   121,   122,
     121,   124,   125,   123,   126,   126,   128,   129,   127,   130,
     130,   131,   131,   133,   132,   134,   132,   135,   135,   136,
     136,   136,   137,   137,   138,   139,   139,   139,   139,   139,
     139,   140,   140,   141,   142,   143,   143,   144,   144,   144,
     144
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     1,     2,     2,
       0,     5,     0,     1,     3,     1,     2,     2,     4,     0,
       2,     4,     3,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     4,     2,
       1,     3,     1,     2,     2,     2,     1,     0,     2,     1,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     0,     0,     5,     1,     1,     0,     3,     0,
       3,     0,     0,     3,     1,     1,     0,     0,     4,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     5,     0,     3,     2,     2,     0,
       4,     0,     0,     5,     1,     3,     0,     0,     6,     0,
       2,     0,     2,     0,     5,     0,     5,     0,     1,     1,
       3,     1,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    20,     1,    10,     3,     6,     0,     0,     5,
       0,     8,     0,     4,    29,    27,     0,     0,     7,    10,
      13,    16,    17,    15,    11,    14,   150,   146,   145,   147,
     148,   149,     0,    29,     0,    18,    19,    22,     9,     0,
      46,   106,   154,   153,   107,   151,   152,   133,     0,   121,
       0,     0,     0,   112,   112,   112,   100,   101,   102,   103,
      33,     0,    43,     0,    35,    44,     0,   104,    39,    40,
      41,    38,    45,    42,    37,    36,     0,     0,   159,   158,
     157,   105,    30,     0,     0,    23,    25,     0,     0,   117,
       0,     0,    72,   126,     0,   118,   119,     0,   113,     0,
       0,   111,    57,    47,   132,    55,    54,   108,    28,    34,
       0,    32,     0,    21,     0,    26,   156,    12,   155,     0,
       0,    79,     0,   122,     0,   115,   109,   110,   106,   100,
     101,   102,    57,    57,    57,    57,    49,    50,    59,     0,
      52,    56,    57,     0,   160,    31,    24,   141,     0,   143,
       0,   138,   139,     0,     0,    77,    76,    73,    75,    86,
     127,     0,   120,     0,     0,     0,    53,    57,    61,    62,
       0,     0,     0,   135,    58,    57,    57,    57,    57,    57,
      57,    57,    57,    60,    48,   136,     0,   142,   134,    79,
       0,    81,     0,   129,     0,   124,   123,   114,   115,     0,
      51,    71,   144,    63,    64,    65,    67,    66,    68,    69,
      70,   140,    78,    74,    85,    84,    80,    82,   106,    87,
      99,    98,     0,   128,     0,   116,    79,    89,   130,   125,
      83,    97,    94,    93,    91,    92,    96,    88,     0,    95,
      90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,    39,    24,
      61,    17,     7,     8,    84,    85,    86,     9,    62,    32,
      33,    63,    64,   143,   103,   136,   137,    65,   138,   139,
     140,   156,   121,   190,   157,   189,   158,   159,   216,   226,
     217,   191,   192,   227,   237,   238,   239,   219,   141,    67,
      68,    69,    70,    99,    71,   164,    72,    73,   124,    74,
      94,   161,   196,    75,   122,   193,   223,    76,    77,    91,
      90,   150,   151,   152,   142,    34,    78,    79,    80,   117,
      81
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -179
static const yytype_int16 yypact[] =
{
      12,    47,    76,  -179,  -179,    12,  -179,    13,    55,  -179,
     -24,  -179,    43,  -179,    18,  -179,    10,    52,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,
    -179,  -179,    35,    18,    43,  -179,  -179,    18,  -179,    63,
    -179,    16,  -179,  -179,  -179,  -179,  -179,  -179,    58,  -179,
     111,   112,    58,   116,   116,   116,    88,    92,    93,  -179,
    -179,    94,  -179,    35,  -179,  -179,    33,   124,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,    79,    85,  -179,  -179,
    -179,   104,  -179,   -21,    84,   107,  -179,    43,   113,  -179,
     120,   133,  -179,  -179,    58,  -179,  -179,   105,  -179,   138,
     140,  -179,   349,  -179,  -179,  -179,  -179,  -179,  -179,  -179,
     160,  -179,   142,  -179,    18,  -179,  -179,  -179,   104,   250,
     250,    22,   178,  -179,   143,   208,  -179,  -179,  -179,  -179,
    -179,  -179,   349,     5,     5,   284,  -179,  -179,  -179,    20,
     317,  -179,   394,   161,  -179,  -179,  -179,  -179,    18,  -179,
     159,   162,  -179,   394,   167,  -179,  -179,  -179,  -179,  -179,
    -179,   214,  -179,   157,   166,    13,  -179,   340,    39,    39,
     171,   367,   173,  -179,  -179,     5,     5,     5,     5,     5,
       5,     5,     5,  -179,  -179,  -179,   103,  -179,  -179,    22,
     174,    44,   378,   197,   141,  -179,  -179,  -179,   208,   367,
    -179,  -179,  -179,   130,   130,    39,    39,    39,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,   177,  -179,
    -179,  -179,   178,  -179,   176,  -179,    22,    60,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,   378,  -179,
    -179
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -179,   237,  -179,  -179,  -179,  -179,   233,  -179,  -179,   -28,
      -5,  -179,  -179,  -179,  -179,  -179,   139,  -179,    -6,   221,
    -179,  -119,  -179,  -179,   172,   125,   121,  -179,   126,  -179,
     146,   -22,  -179,  -179,  -178,  -179,  -179,  -179,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,  -179,    24,   -32,  -179,
    -179,  -179,  -179,    54,  -179,    65,  -115,  -179,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,  -179,  -179,   -61,  -134,  -179,
    -179,   147,  -179,    80,  -111,   -33,  -179,  -179,   163,  -179,
     181
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -138
static const yytype_int16 yytable[] =
{
      66,    15,   104,   160,    87,   174,    83,    25,   153,   153,
     165,   212,    -2,   128,    42,    43,    44,   102,    35,    45,
      46,    26,    27,    28,    29,    30,    93,    36,   173,    25,
      97,    66,    18,    19,    31,   111,    40,    47,   129,   130,
     131,    59,   195,    41,    42,    43,    44,     3,   230,    45,
      46,    20,    47,   105,   106,   133,   134,    48,   221,   115,
      49,    50,    51,   167,    52,    53,    54,    55,    56,    57,
      58,    59,   123,    14,  -135,   153,    21,    22,    23,    89,
      92,    87,    25,   165,   155,   214,   215,   149,   149,     4,
      66,    60,   180,   181,   182,    14,  -131,   231,    16,   232,
     233,    88,   172,   228,   221,   234,   235,   236,   100,   101,
      37,   128,    42,    43,    44,   172,    92,    45,    46,    95,
      96,   187,    42,    43,   116,    98,   -16,    45,    46,    66,
     -17,   -15,   102,   224,   172,   107,   129,   130,   131,    59,
     108,   109,    40,   113,    26,    27,    28,    29,    30,    41,
      42,    43,    44,   110,   149,    45,    46,    31,    47,   198,
     220,   148,    66,    48,   114,   125,    49,    50,    51,    42,
      52,    53,    54,    55,    56,    57,    58,    59,   119,    40,
     177,   178,   179,   180,   181,   182,    41,    42,    43,    44,
      66,   120,    45,    46,   126,    47,   127,    60,   145,   162,
      48,    14,   -29,    49,    50,    51,   220,    52,    53,    54,
      55,    56,    57,    58,    59,    40,   163,   184,   185,   186,
      89,   222,    41,    42,    43,    44,   188,   197,    45,    46,
     200,    47,   202,   213,    60,  -135,    48,   229,    14,    49,
      50,    51,    13,    52,    53,    54,    55,    56,    57,    58,
      59,   147,    38,   146,    82,   112,   170,   166,   128,    42,
      43,    44,   240,   225,    45,    46,   211,   154,   183,   118,
      60,     0,     0,   144,   194,     0,     0,     0,     0,   168,
     169,   171,     0,   129,   130,   131,    59,    26,    27,    28,
      29,    30,   128,    42,    43,    44,     0,     0,    45,    46,
      31,     0,     0,     0,     0,     0,     0,     0,   148,  -137,
       0,     0,     0,   199,     0,     0,     0,   129,   130,   131,
      59,   203,   204,   205,   206,   207,   208,   209,   210,     0,
       0,   132,     0,     0,   133,   134,     0,     0,     0,     0,
       0,     0,   167,    26,    27,    28,    29,    30,   128,    42,
      43,    44,     0,     0,    45,    46,    31,   128,    42,    43,
      44,     0,     0,    45,    46,   175,   176,   177,   178,   179,
     180,   181,   182,   129,   130,   131,    59,     0,     0,     0,
       0,     0,   129,   130,   131,    59,   218,    42,    43,    44,
     133,   134,    45,    46,     0,    47,   132,     0,   167,   133,
     134,     0,   128,    42,    43,    44,     0,   135,    45,    46,
       0,   129,   130,   131,    59,   175,   176,   177,   178,   179,
     180,   181,   182,     0,     0,     0,   201,   129,   130,   131,
      59
};

static const yytype_int16 yycheck[] =
{
      32,     7,    63,   122,    37,   139,    34,    12,   119,   120,
     125,   189,     0,     8,     9,    10,    11,    38,     8,    14,
      15,     3,     4,     5,     6,     7,    48,    17,     8,    34,
      52,    63,    56,    57,    16,    56,     1,    17,    33,    34,
      35,    36,   161,     8,     9,    10,    11,     0,   226,    14,
      15,     8,    17,    20,    21,    50,    51,    22,   192,    87,
      25,    26,    27,    58,    29,    30,    31,    32,    33,    34,
      35,    36,    94,    60,    58,   186,    33,    34,    35,    63,
      58,   114,    87,   198,    62,    41,    42,   119,   120,    13,
     122,    56,    53,    54,    55,    60,    61,    37,    43,    39,
      40,    38,   135,   222,   238,    45,    46,    47,    54,    55,
      58,     8,     9,    10,    11,   148,    58,    14,    15,     8,
       8,   153,     9,    10,    11,     9,    38,    14,    15,   161,
      38,    38,    38,   194,   167,    11,    33,    34,    35,    36,
      61,    56,     1,    59,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    49,   186,    14,    15,    16,    17,   165,
     192,    58,   194,    22,    57,    60,    25,    26,    27,     9,
      29,    30,    31,    32,    33,    34,    35,    36,    58,     1,
      50,    51,    52,    53,    54,    55,     8,     9,    10,    11,
     222,    58,    14,    15,    56,    17,    56,    56,    56,    56,
      22,    60,    61,    25,    26,    27,   238,    29,    30,    31,
      32,    33,    34,    35,    36,     1,     8,    56,    59,    57,
      63,    24,     8,     9,    10,    11,    59,    61,    14,    15,
      59,    17,    59,    59,    56,    58,    22,    61,    60,    25,
      26,    27,     5,    29,    30,    31,    32,    33,    34,    35,
      36,     1,    19,   114,    33,    83,   135,   132,     8,     9,
      10,    11,   238,   198,    14,    15,   186,   120,   142,    88,
      56,    -1,    -1,   110,    60,    -1,    -1,    -1,    -1,   133,
     134,   135,    -1,    33,    34,    35,    36,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    -1,    -1,    14,    15,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,
      -1,    -1,    -1,   167,    -1,    -1,    -1,    33,    34,    35,
      36,   175,   176,   177,   178,   179,   180,   181,   182,    -1,
      -1,    47,    -1,    -1,    50,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    58,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    -1,    14,    15,    16,     8,     9,    10,
      11,    -1,    -1,    14,    15,    48,    49,    50,    51,    52,
      53,    54,    55,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,     8,     9,    10,    11,
      50,    51,    14,    15,    -1,    17,    47,    -1,    58,    50,
      51,    -1,     8,     9,    10,    11,    -1,    58,    14,    15,
      -1,    33,    34,    35,    36,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    33,    34,    35,
      36
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    65,    66,     0,    13,    67,    68,    76,    77,    81,
      69,    70,    71,    65,    60,    82,    43,    75,    56,    57,
       8,    33,    34,    35,    73,    74,     3,     4,     5,     6,
       7,    16,    83,    84,   139,     8,    17,    58,    70,    72,
       1,     8,     9,    10,    11,    14,    15,    17,    22,    25,
      26,    27,    29,    30,    31,    32,    33,    34,    35,    36,
      56,    74,    82,    85,    86,    91,   112,   113,   114,   115,
     116,   118,   120,   121,   123,   127,   131,   132,   140,   141,
     142,   144,    83,    73,    78,    79,    80,   139,    38,    63,
     134,   133,    58,    95,   124,     8,     8,    95,     9,   117,
     117,   117,    38,    88,   131,    20,    21,    11,    61,    56,
      49,    56,    88,    59,    57,    73,    11,   143,   144,    58,
      58,    96,   128,    95,   122,    60,    56,    56,     8,    33,
      34,    35,    47,    50,    51,    58,    89,    90,    92,    93,
      94,   112,   138,    87,   142,    56,    80,     1,    58,   112,
     135,   136,   137,   138,   135,    62,    95,    98,   100,   101,
      85,   125,    56,     8,   119,   120,    89,    58,    94,    94,
      90,    94,   139,     8,   132,    48,    49,    50,    51,    52,
      53,    54,    55,    92,    56,    59,    57,   112,    59,    99,
      97,   105,   106,   129,    60,    85,   126,    61,    82,    94,
      59,    59,    59,    94,    94,    94,    94,    94,    94,    94,
      94,   137,    98,    59,    41,    42,   102,   104,     8,   111,
     112,   132,    24,   130,   131,   119,   103,   107,    85,    61,
      98,    37,    39,    40,    45,    46,    47,   108,   109,   110,
     111
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 146 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 163 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 163 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 165 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					((yyvsp[(5) - (5)]).v.type == C_STRING_LITERAL) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.data);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 172 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 173 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 175 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 176 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 177 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 181 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 190 "Parser.y"
    { startmacro(); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 191 "Parser.y"
    { (yyval).ident = (yyvsp[(2) - (5)]).ident; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s,typ,(GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,typ,_nparam);
				_nparam++;
			;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 210 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (2)]).ident.s, -1, 1);
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (2)]).ident.s);
					_compilerConfiguration->cb_insertNewMacro((yyvsp[(1) - (2)]).ident.s, _yyCurrentComment, &_bytecodeBuffer);
				}
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				endmacro();
			;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 230 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s,
					(yyvsp[(1) - (4)]).type,
					((yyvsp[(1) - (4)]).type == C_STRING_LITERAL) ? (intptr_t)"" : 0);
				bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(2) - (4)]).ident.s,(yyvsp[(3) - (4)]).v.type, (yyvsp[(3) - (4)]).v.data);
				freeval(&(yyvsp[(3) - (4)]).v);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 240 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).type == C_STRING_LITERAL) {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)"");
				} else {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)0);
				}
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 270 "Parser.y"
    {
				bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s,(yyvsp[(2) - (2)]).v.type, (yyvsp[(2) - (2)]).v.data);
				vname_count = 0;
			;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 275 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 278 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 281 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 284 "Parser.y"
    { bytecode_emitIncrementExpression(&_bytecodeBuffer, &(yyvsp[(1) - (2)]).v, -1); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { bytecode_emitIncrementExpression(&_bytecodeBuffer, &(yyvsp[(1) - (2)]).v, 1); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 288 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 289 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_FURET, (GENERIC_DATA){vname_count});
			;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 291 "Parser.y"
    {
				if ((yyvsp[(2) - (2)]).funcp && function_returnsString((yyvsp[(2) - (2)]).funcp)) {
					(yyval).v.type = C_STRINGVAR;
				} else {
					(yyval).v.type = C_LONGVAR;
				}
				(yyval).v.data.val = (intptr_t)"__ret__";
				((char*)(yyval).v.data.val)[6] = '0' + vname_count++;
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    {
				if (macro_isParameterStringType((yyvsp[(1) - (2)]).type) ==
				    macro_isParameterStringType((yyvsp[(2) - (2)]).v.type)) {
					(yyval).v = (yyvsp[(2) - (2)]).v;
				} else {
					(yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST,&(yyvsp[(2) - (2)]).v,0);
					(yyval).v.type = ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL) ?
						C_STRINGVAR : (((yyvsp[(1) - (2)]).type == C_FLOAT_LITERAL) ? C_FLOATVAR : C_LONGVAR);
				}
			;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 312 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 313 "Parser.y"
    { (yyval).v = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 315 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 316 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 318 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 319 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 321 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 322 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    { bytecode_endTestOperation(CT_BRACKETS); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { bytecode_endTestOperation(CT_NOT); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { bytecode_endTestOperation((yyvsp[(3) - (3)]).type); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; binaryOperation1Found((yyvsp[(1) - (1)]).type); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 337 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 338 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    { binaryOperation1Found((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 342 "Parser.y"
    { bytecode_endTestOperation((yyvsp[(4) - (4)]).type); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 344 "Parser.y"
    { 	
				if (_stringflg)
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STRING_LITERAL,(GENERIC_DATA){.string=""});
				else
					_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_LONG_LITERAL,(GENERIC_DATA){0});
				(yyval).type = CT_NE;
			;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				int nType = (yyvsp[(1) - (2)]).type;
				(yyval).type = (nType | flg);
			;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("String match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 369 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 370 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    { (yyval).type = C_LONG_LITERAL; 	 	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    {	(yyval).type = bytecode_emitPushValueInstruction(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 375 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    {
				(yyval).v.type = C_FLOATVAR;  
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 383 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    {
				(yyval).v.type = C_BOOLEANVAR;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    {
				(yyval).v.type = C_STRING_LITERAL;
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 402 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    { 
				if (((yyval).ident.s = malloc(strlen((yyvsp[(1) - (2)]).ident.s)+strlen((yyvsp[(2) - (2)]).ident.s)+1)) != 0) {
					strcat(strcpy((yyval).ident.s,(yyvsp[(1) - (2)]).ident.s),(yyvsp[(2) - (2)]).ident.s);
					free((yyvsp[(1) - (2)]).ident.s);
					free((yyvsp[(2) - (2)]).ident.s);
				} else {
					yyerror("buffer overlow");
					(yyval).ident = (yyvsp[(2) - (2)]).ident;
				}
			;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 418 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_STOP,(GENERIC_DATA){.longValue=(yyvsp[(2) - (2)]).num});
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 432 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 437 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {
				_iflevel--;
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 480 "Parser.y"
    {
				// Native call
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 1;
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_0FUNC,
							   (GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (1)]).funcp)});
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 488 "Parser.y"
    {
				_lastfuncp = 0;
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer,C_MACRO,(GENERIC_DATA){.string = (yyvsp[(1) - (1)]).ident.s});
				freeitem(&(yyvsp[(1) - (1)]).ident.s);
				(yyval).funcp = 0;
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 498 "Parser.y"
    {
				bytecode_emitPushParameterInstruction(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 501 "Parser.y"
    {
				bytecode_emitPushParameterInstruction(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 504 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    {
				if (ISCONSTINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_CHARACTER_LITERAL &&
					    ((yyvsp[(2) - (2)]).v.data.val < 0 || (yyvsp[(2) - (2)]).v.data.val > 255))
					    	yyerror("constant out of range");
					if ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL && (yyvsp[(2) - (2)]).num) {
						yyerror("invalid string cast");
						(yyvsp[(2) - (2)]).v.data.val = 0;
					}
				} else if (ISVARINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL) {
						yyerror("unsupported cast");
					}
				}
				(yyval).v.type = (yyvsp[(1) - (2)]).type;
				(yyval).v.data.val = (yyvsp[(2) - (2)]).num;
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 527 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 529 "Parser.y"
    {	(yyval).type = C_CHARACTER_LITERAL;	;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 530 "Parser.y"
    {	(yyval).type = C_INTEGER_LITERAL;	;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    {	(yyval).type = C_LONG_LITERAL;	;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 532 "Parser.y"
    {	(yyval).type = C_FLOAT_LITERAL;	;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 533 "Parser.y"
    {	(yyval).type = C_BOOLEAN_LITERAL; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 534 "Parser.y"
    {	(yyval).type = C_STRING_LITERAL;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 536 "Parser.y"
    {
			(yyval).v.type = C_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 539 "Parser.y"
    {
			(yyval).v.type = C_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 544 "Parser.y"
    {
				(yyval).v.type = C_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 549 "Parser.y"
    {
				(yyval).v.type = C_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 556 "Parser.y"
    {
				(yyval).v.type = C_STRING_LITERAL;
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 561 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 563 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 564 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.data.longValue = (yyvsp[(1) - (3)]).v.data.longValue | (yyvsp[(3) - (3)]).v.data.longValue;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2769 "Parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 568 "Parser.y"


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


