
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
#line 48 "Parser.y"


#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"
#include "arraylist.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_PUSH_STRING_LITERAL||type == C_PUSH_VARIABLE) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_PUSH_LONG_LITERAL|| t == C_PUSH_INTEGER_LITERAL || \
								 t == C_PUSH_SMALL_INT_LITERAL)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	REC_SPACE			24000

static BYTECODE_BUFFER  _bytecodeBuffer;
static void		*_currentNativeMethodCalled;
static int		_nparam,
				_breaklevel,
				_iflevel,
				_stringflg;
static      int  _currentFunctionCallParamIndices[32];
static      int  *_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
static 		void init_header(void);
static 		void finalize_header(void);
static 		void init_macroDefinition(void);
static 		void finalize_macroDefinition(void);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int _arraySize;
static MACRO_SCOPE _macroScope;
static MACROC_INSTRUCTION_OP_CODE _functionCallOp;
static int	_currentNamespaceIndex;
static int  _foreachCursorIndex;
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
extern int macro_lookupNamespace(const char* pszNamespacename);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
/*
 * Destroy an array list containing TYPE_OBJECT_POINTERs.
 */
extern void bytecode_destroyArraylistWithPointers(ARRAY_LIST* pList);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, long arraySizeOrParamIndex);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);
static ARRAY_LIST* _currentArrayLiteral;

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
	if (_arraySize < 0) {
		yyerror("Illegal negative array size %d", _arraySize);
		_arraySize = 0;
	}
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, _arraySize);
	_arraySize = 0;
	IDENTIFIER_CONTEXT* pContext = sym_getContext(_currentIdentifierContext, (char*)pszName);
	if (pContext && pContext != sym_getGlobalContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
	if (sType == S_STRING) {
		if (!tVal) {
			tVal = (intptr_t)"";
		}
			

	}
	sym_createSymbol(_currentIdentifierContext, (char*) pszName, sType, (GENERIC_DATA) {.val = tVal});
}

static SYMBOL_TYPE constantTypeFor(SYMBOL_TYPE aValueType) {
	switch(aValueType) {
	case S_FLOAT: return S_CONSTFLOAT;
	case S_STRING: return S_CONSTSTRING;
	case S_CHARACTER: return S_CONSTCHARACTER;
	}
	return S_CONSTNUM;
}

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}



/* Line 189 of yacc.c  */
#line 221 "Parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
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
     T_NAMESPACE = 258,
     T_STATIC = 259,
     T_FOR = 260,
     T_TAUTO = 261,
     T_TSTRING = 262,
     T_TINT = 263,
     T_TCHAR = 264,
     T_TLONG = 265,
     T_TFLOAT = 266,
     T_RANGE = 267,
     T_IDENT = 268,
     T_NUM = 269,
     T_FLOATING_POINT_NUMBER = 270,
     T_STRING = 271,
     T_CHARACTER = 272,
     T_CONST = 273,
     T_TRUE = 274,
     T_FALSE = 275,
     T_TBOOLEAN = 276,
     T_FUNC = 277,
     T_MACRO = 278,
     T_DOTDOT = 279,
     T_PLUSPLUS = 280,
     T_MINUSMINUS = 281,
     T_IF = 282,
     T_ELIF = 283,
     T_ELSE = 284,
     T_WHILE = 285,
     T_GOTO = 286,
     T_BRAEQ = 287,
     T_SH_ASSIGN_PLUS = 288,
     T_SH_ASSIGN_MINUS = 289,
     T_SH_ASSIGN_MULT = 290,
     T_SH_ASSIGN_DIV = 291,
     T_SH_ASSIGN_MOD = 292,
     T_LOCAL = 293,
     T_SWITCH = 294,
     T_BREAK = 295,
     T_CONTINUE = 296,
     T_RETURN = 297,
     T_NUMVAR = 298,
     T_RANGEVAR = 299,
     T_FLOATVAR = 300,
     T_STRINGVAR = 301,
     T_STRINGARRAYVAR = 302,
     T_BOOLEANVAR = 303,
     T_CHARACTERVAR = 304,
     T_NMATCH = 305,
     T_ASSIGN = 306,
     T_NE = 307,
     T_EQ = 308,
     T_SHIFT_LEFT = 309,
     T_SHIFT_RIGHT = 310,
     T_OR = 311,
     T_AND = 312,
     T_LE = 313,
     T_GE = 314,
     T_POWER_TO = 315,
     T_VOID = 316,
     T_CMDSEQPREFIX = 317,
     T_GT = 318,
     T_LT = 319
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
#line 327 "Parser.tab.c"

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
#define YYLAST   1499

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  96
/* YYNRULES -- Number of rules.  */
#define YYNRULES  229
/* YYNRULES -- Number of states.  */
#define YYNSTATES  338

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,     2,     2,     2,    75,    63,     2,
      80,    81,    73,    71,    78,    72,     2,    74,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,    77,
      67,     2,    68,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    76,     2,    79,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,    64,    84,    66,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    69,    70
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    14,    18,    19,
      22,    25,    26,    29,    32,    33,    35,    36,    39,    43,
      45,    49,    50,    51,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    77,    79,    81,    83,    85,    88,
      91,    93,    95,    99,   101,   103,   105,   107,   109,   110,
     116,   117,   119,   123,   125,   128,   132,   137,   138,   141,
     146,   150,   152,   155,   158,   160,   162,   164,   166,   168,
     170,   172,   174,   176,   178,   180,   182,   185,   187,   190,
     193,   199,   200,   205,   207,   209,   211,   213,   215,   218,
     220,   224,   226,   228,   230,   232,   233,   234,   240,   241,
     244,   247,   250,   252,   254,   256,   259,   262,   267,   270,
     273,   276,   280,   284,   288,   292,   296,   300,   304,   308,
     312,   316,   320,   324,   328,   332,   336,   340,   344,   348,
     352,   356,   360,   364,   368,   372,   375,   376,   378,   381,
     383,   385,   387,   389,   391,   393,   395,   397,   399,   401,
     403,   405,   407,   408,   412,   413,   418,   422,   424,   426,
     428,   431,   435,   439,   442,   443,   445,   446,   448,   454,
     455,   459,   462,   465,   466,   471,   472,   480,   481,   482,
     488,   489,   490,   496,   498,   502,   503,   505,   506,   507,
     514,   515,   516,   520,   521,   522,   526,   527,   533,   534,
     540,   543,   545,   547,   549,   551,   555,   557,   559,   563,
     565,   567,   569,   571,   573,   575,   578,   581,   583,   585,
     588,   592,   594,   596,   598,   600,   602,   604,   606,   608
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    -1,    -1,    88,    90,    89,    96,    -1,
      91,    92,    -1,    -1,     3,    13,    77,    -1,    -1,    92,
      97,    -1,    92,    93,    -1,    -1,    94,    93,    -1,    95,
     115,    -1,    -1,     4,    -1,    -1,    96,   112,    -1,    18,
      98,    77,    -1,    99,    -1,    98,    78,    99,    -1,    -1,
      -1,   100,   103,    51,   102,   101,    -1,   181,    -1,    16,
      -1,    13,    -1,   104,    -1,    14,    -1,    16,    -1,    46,
      -1,    47,    -1,    43,    -1,    44,    -1,    49,    -1,    48,
      -1,    45,    -1,    13,    -1,   106,    13,    -1,   106,    22,
      -1,    61,    -1,     7,    -1,     7,    76,    79,    -1,    21,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,    -1,
     108,   105,    80,   109,    81,    -1,    -1,   110,    -1,   110,
      78,   111,    -1,   111,    -1,   175,   103,    -1,    95,   107,
     113,    -1,    82,   114,   166,   160,    -1,    -1,   115,   114,
      -1,   175,   103,   121,    77,    -1,   175,   103,    77,    -1,
      77,    -1,   168,    77,    -1,   118,    77,    -1,   161,    -1,
     156,    -1,   153,    -1,   146,    -1,   141,    -1,   142,    -1,
     143,    -1,   149,    -1,   113,    -1,   128,    -1,   148,    -1,
     115,    -1,   117,    77,    -1,     1,    -1,   117,     1,    -1,
     104,   121,    -1,   104,    76,   134,    79,   121,    -1,    -1,
     104,   119,   120,   122,    -1,    35,    -1,    36,    -1,    37,
      -1,    33,    -1,    34,    -1,    51,   122,    -1,   123,    -1,
      80,   123,    81,    -1,   130,    -1,   125,    -1,   130,    -1,
     125,    -1,    -1,    -1,   130,    24,   130,   126,   127,    -1,
      -1,    24,   130,    -1,   104,    26,    -1,   104,    25,    -1,
     134,    -1,   168,    -1,   129,    -1,   174,   129,    -1,    72,
      14,    -1,   130,    76,   124,    79,    -1,    66,   130,    -1,
      71,   130,    -1,    72,   130,    -1,   130,    66,   130,    -1,
     130,    50,   130,    -1,   130,    63,   130,    -1,   130,    64,
     130,    -1,   130,    71,   130,    -1,   130,    65,   130,    -1,
     130,    72,   130,    -1,   130,    73,   130,    -1,   130,    74,
     130,    -1,   130,    75,   130,    -1,   130,    54,   130,    -1,
     130,    55,   130,    -1,   130,    60,   130,    -1,   130,    57,
     130,    -1,   130,    56,   130,    -1,   130,    67,   130,    -1,
     130,    68,   130,    -1,   130,    58,   130,    -1,   130,    59,
     130,    -1,   130,    53,   130,    -1,   130,    52,   130,    -1,
      80,   130,    81,    -1,    80,   132,    81,    -1,    80,   130,
      81,    -1,    80,   132,    -1,    -1,   133,    -1,    83,   132,
      -1,   168,    -1,   134,    -1,    43,    -1,    45,    -1,    47,
      -1,    44,    -1,    46,    -1,    49,    -1,    48,    -1,   140,
      -1,   181,    -1,   135,    -1,    13,    -1,    -1,    82,   136,
     160,    -1,    -1,    82,   137,   138,   160,    -1,   138,    78,
     139,    -1,   139,    -1,    16,    -1,    16,    -1,   140,    16,
      -1,    40,   145,    77,    -1,    41,   145,    77,    -1,    42,
     144,    -1,    -1,   130,    -1,    -1,    14,    -1,    39,   131,
      82,   147,    84,    -1,    -1,   148,   113,   147,    -1,    13,
      85,    -1,    31,    13,    -1,    -1,    32,    13,   150,    77,
      -1,    -1,    80,   175,   103,   152,    85,   123,    81,    -1,
      -1,    -1,     5,   154,   151,   155,   159,    -1,    -1,    -1,
      30,   157,   131,   158,   159,    -1,   116,    -1,    82,   166,
     160,    -1,    -1,    84,    -1,    -1,    -1,    27,   131,   162,
     159,   163,   164,    -1,    -1,    -1,    29,   165,   159,    -1,
      -1,    -1,   116,   167,   166,    -1,    -1,    22,    80,   169,
     109,    81,    -1,    -1,   171,    80,   170,   109,    81,    -1,
      73,   104,    -1,    13,    -1,     1,    -1,   172,    -1,   173,
      -1,   172,    78,   173,    -1,     1,    -1,   123,    -1,    80,
     175,    81,    -1,     9,    -1,     8,    -1,    10,    -1,    11,
      -1,    21,    -1,     7,    -1,     7,   176,    -1,     8,   176,
      -1,     6,    -1,    12,    -1,    76,    79,    -1,    76,    14,
      79,    -1,    19,    -1,    20,    -1,    15,    -1,    14,    -1,
      17,    -1,   179,    -1,   178,    -1,   180,    -1,   177,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   178,   178,   184,   178,   190,   192,   195,   200,   201,
     202,   204,   205,   207,   209,   210,   212,   213,   217,   220,
     221,   223,   223,   223,   235,   238,   244,   245,   246,   253,
     260,   261,   262,   263,   264,   265,   266,   267,   277,   280,
     286,   287,   288,   289,   290,   291,   292,   293,   295,   295,
     300,   301,   304,   306,   308,   315,   338,   340,   341,   343,
     350,   358,   359,   360,   361,   362,   363,   364,   365,   366,
     367,   368,   369,   370,   371,   372,   375,   381,   382,   384,
     389,   394,   394,   403,   404,   405,   406,   407,   409,   411,
     412,   414,   415,   418,   419,   421,   422,   422,   424,   425,
     428,   430,   432,   433,   435,   436,   440,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   465,   466,   467,   468,   469,   470,   471,
     472,   473,   474,   478,   479,   480,   484,   485,   486,   488,
     489,   491,   495,   499,   503,   507,   511,   515,   519,   524,
     525,   531,   537,   537,   541,   541,   546,   547,   550,   554,
     555,   566,   569,   573,   577,   578,   580,   581,   583,   585,
     586,   588,   593,   597,   597,   602,   602,   616,   618,   616,
     629,   631,   629,   642,   642,   645,   647,   650,   653,   650,
     660,   664,   664,   671,   672,   672,   678,   678,   689,   689,
     701,   707,   711,   717,   719,   720,   721,   725,   727,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   740,
     741,   745,   748,   753,   758,   763,   768,   776,   784,   792
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NAMESPACE", "T_STATIC", "T_FOR",
  "T_TAUTO", "T_TSTRING", "T_TINT", "T_TCHAR", "T_TLONG", "T_TFLOAT",
  "T_RANGE", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING",
  "T_CHARACTER", "T_CONST", "T_TRUE", "T_FALSE", "T_TBOOLEAN", "T_FUNC",
  "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF",
  "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_RANGEVAR", "T_FLOATVAR", "T_STRINGVAR",
  "T_STRINGARRAYVAR", "T_BOOLEANVAR", "T_CHARACTERVAR", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_VOID", "T_CMDSEQPREFIX", "'&'",
  "'|'", "'^'", "'~'", "'<'", "'>'", "T_GT", "T_LT", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'['", "';'", "','", "']'", "'('", "')'", "'{'", "'!'",
  "'}'", "':'", "$accept", "file_structure", "$@1", "$@2",
  "header_declarations", "namespace", "global_declarations", "global_vars",
  "global_var", "scope", "macro_definitions", "constants", "constant_list",
  "constdef", "$@3", "$@4", "constant_literal", "variable_identifier",
  "variable_reference", "macro_declaration", "macro_type", "macrostart",
  "$@5", "parameter_list", "non_empty_pardecl", "par_decl",
  "macro_definition", "block", "var_decls", "var_decl", "statement",
  "errors", "assignment", "$@6", "shorthand_assignment_operator",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@7",
  "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "logical_expression", "term", "value",
  "array_literal", "$@8", "$@9", "array_elements", "array_element",
  "string", "break", "continue", "return_expression", "optional_bterm",
  "opt_num", "case", "caselabels", "label", "goto", "$@10", "in_clause",
  "$@11", "foreach", "$@12", "$@13", "while", "$@14", "$@15",
  "local_block", "closing_brace", "if_expression", "$@16", "$@17",
  "else_clause", "$@18", "stmntlist", "$@19", "call_expression", "@20",
  "@21", "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_dimension", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,    38,   124,    94,   126,    60,    62,   318,
     319,    43,    45,    42,    47,    37,    91,    59,    44,    93,
      40,    41,   123,    33,   125,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    88,    89,    87,    90,    91,    91,    92,    92,
      92,    93,    93,    94,    95,    95,    96,    96,    97,    98,
      98,   100,   101,    99,   102,   102,   103,   103,   103,   103,
     104,   104,   104,   104,   104,   104,   104,   104,   105,   105,
     106,   106,   106,   106,   106,   106,   106,   106,   108,   107,
     109,   109,   110,   110,   111,   112,   113,   114,   114,   115,
     115,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   117,   117,   118,
     118,   119,   118,   120,   120,   120,   120,   120,   121,   122,
     122,   123,   123,   124,   124,   125,   126,   125,   127,   127,
     128,   128,   129,   129,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   131,   131,   131,   132,   132,   132,   133,
     133,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   136,   135,   137,   135,   138,   138,   139,   140,
     140,   141,   142,   143,   144,   144,   145,   145,   146,   147,
     147,   148,   149,   150,   149,   152,   151,   154,   155,   153,
     157,   158,   156,   159,   159,   160,   160,   162,   163,   161,
     164,   165,   164,   166,   167,   166,   169,   168,   170,   168,
     171,   171,   171,   109,   172,   172,   172,   173,   174,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   176,
     176,   177,   177,   178,   179,   180,   181,   181,   181,   181
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     0,     1,     0,     2,     3,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     1,     3,     1,     1,     1,     1,     1,     0,     5,
       0,     1,     3,     1,     2,     3,     4,     0,     2,     4,
       3,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       5,     0,     4,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     1,     1,     1,     0,     0,     5,     0,     2,
       2,     2,     1,     1,     1,     2,     2,     4,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     0,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     0,     4,     3,     1,     1,     1,
       2,     3,     3,     2,     0,     1,     0,     1,     5,     0,
       3,     2,     2,     0,     4,     0,     7,     0,     0,     5,
       0,     0,     5,     1,     3,     0,     1,     0,     0,     6,
       0,     0,     3,     0,     0,     3,     0,     5,     0,     5,
       2,     1,     1,     1,     1,     3,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    16,     5,
       7,    14,    15,    21,    10,    11,     0,     9,    48,    17,
       0,    19,     0,    12,   217,   214,   210,   209,   211,   212,
     218,   213,    13,     0,     0,     0,    18,    21,    26,    28,
      29,    32,    33,    36,    30,    31,    35,    34,     0,    27,
       0,   215,   216,     0,    57,    55,    41,    44,    45,    46,
      47,    43,    40,     0,     0,    20,     0,     0,   219,     0,
      60,     0,     0,    57,     0,     0,    38,    39,   224,   223,
      25,   225,   221,   222,    22,   229,   227,   226,   228,    24,
     220,   202,   151,   159,     0,   141,   144,   142,   145,   143,
     147,   146,     0,     0,     0,     0,     0,   152,    88,    89,
      92,   104,    91,   102,   150,   148,   103,     0,     0,   149,
      59,    77,   177,    37,     0,   180,     0,     0,     0,   166,
     166,     0,    61,    81,    72,    75,   194,     0,     0,    73,
      68,    69,    70,    67,    74,    71,    66,    65,    64,   185,
       0,    58,    42,   206,     0,     0,    51,    53,   207,   203,
     204,     0,    23,   196,   108,   109,   106,   110,    37,   200,
       0,     0,     0,   185,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   160,   198,
     105,     0,   171,     0,   187,     0,   172,   173,     0,   167,
       0,     0,   165,   163,   101,   100,     0,     0,    79,     0,
      78,    76,    63,   186,    56,    62,     0,    49,     0,     0,
      54,     0,    90,   132,   208,   153,   158,   185,   157,    96,
     112,   131,   130,   121,   122,   125,   124,   128,   129,   123,
     113,   114,   116,   111,   126,   127,   115,   117,   118,   119,
     120,     0,    94,    93,     0,     0,   178,     0,     0,   135,
     137,   140,   139,     0,   181,     0,   169,   161,   162,   151,
       0,    86,    87,    83,    84,    85,     0,   195,    52,   205,
       0,     0,   155,    98,   107,     0,     0,     0,   138,   140,
     139,   134,   133,     0,   183,   188,     0,   174,     0,     0,
       0,     0,    82,   197,   156,     0,    97,   199,   175,   179,
      57,   185,   190,   182,   168,   169,    80,    99,     0,   184,
     191,   189,   170,     0,     0,     0,   192,   176
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    14,    15,    16,
      11,    17,    20,    21,    22,   162,    84,    48,   133,    63,
      64,    34,    35,   155,   156,   157,    19,   134,    72,   135,
     304,   137,   138,   217,   286,    71,   108,   158,   261,   110,
     293,   316,   139,   111,   112,   204,   269,   270,   113,   114,
     173,   174,   237,   238,   115,   140,   141,   142,   213,   210,
     143,   309,   144,   145,   275,   266,   328,   146,   201,   297,
     147,   205,   306,   305,   224,   148,   273,   322,   331,   334,
     149,   219,   116,   231,   264,   117,   159,   160,   118,    33,
      51,    85,    86,    87,    88,   119
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -277
static const yytype_int16 yypact[] =
{
    -277,    28,    31,  -277,    18,  -277,  -277,   -23,  -277,    21,
    -277,   134,  -277,  -277,  -277,    40,   110,  -277,  -277,  -277,
      59,  -277,   116,  -277,  -277,   -11,   -11,  -277,  -277,  -277,
    -277,  -277,  -277,   116,   -22,    62,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,    16,  -277,
       3,  -277,  -277,   -27,   110,  -277,    63,  -277,  -277,  -277,
    -277,  -277,  -277,    60,    42,  -277,   217,    71,  -277,   891,
    -277,    65,   386,   110,    77,   656,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,    86,  -277,    92,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  1044,  1044,  1117,   100,   734,   141,  -277,  -277,
    -277,  -277,  1195,  -277,  -277,   157,  -277,   104,  1127,  -277,
    -277,   106,  -277,   -24,   109,  -277,   161,   177,   109,   179,
     179,   301,  -277,   -14,  -277,  -277,  -277,    35,   117,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,   111,
     119,  -277,  -277,   106,   811,   122,   120,  -277,  -277,   126,
    -277,   116,  -277,  -277,   224,    94,  -277,    94,  -277,  -277,
     125,  1158,   127,   111,   191,  1044,  1044,  1044,  1044,  1044,
    1044,  1044,  1044,  1044,  1044,  1044,  1044,  1044,  1044,  1044,
    1044,  1044,  1044,  1044,  1044,  1044,  1044,   964,  -277,  -277,
    -277,   143,  -277,   524,  -277,   109,  -277,  -277,   142,  -277,
     148,   149,  1342,  -277,  -277,  -277,    32,    91,  -277,   386,
    -277,  -277,  -277,  -277,  -277,  -277,  1222,  -277,   110,   851,
    -277,   656,  -277,  -277,  -277,  -277,  -277,   -55,  -277,  1342,
     224,  1396,  1396,   436,   436,  1369,  1369,  1342,  1342,   -17,
    1423,  1423,  1423,   224,   186,   186,    94,    94,   -17,   -17,
     -17,   151,  -277,  1195,   656,   110,  -277,   607,  1252,   147,
    -277,  1282,  1312,   170,  -277,   165,   235,  -277,  -277,  -277,
     171,  -277,  -277,  -277,  -277,  -277,   891,  -277,  -277,  -277,
     168,   191,  -277,   227,  -277,   173,   116,   170,  -277,  -277,
    -277,  -277,  -277,   444,  -277,  -277,   170,  -277,   178,   172,
     -22,   204,  -277,  -277,  -277,  1044,  -277,  -277,  -277,  -277,
     110,   111,   236,  -277,  -277,   235,  -277,  1342,   184,  -277,
    -277,  -277,  -277,  1004,   170,   183,  -277,  -277
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -277,  -277,  -277,  -277,  -277,  -277,  -277,   255,  -277,   260,
    -277,  -277,  -277,   237,  -277,  -277,  -277,   -28,   -20,  -277,
    -277,  -277,  -277,  -211,  -277,    44,  -277,   -33,   -67,   -16,
     -64,  -277,  -277,  -277,  -277,  -124,   -13,   -66,  -277,    78,
    -277,  -277,  -277,   162,   -88,  -121,    14,  -277,  -181,  -277,
    -277,  -277,  -277,    -6,  -277,  -277,  -277,  -277,  -277,   156,
    -277,   -37,  -257,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -276,  -163,  -277,  -277,  -277,  -277,  -277,
    -193,  -277,   -68,  -277,  -277,  -277,  -277,    61,  -277,   -43,
     263,  -277,  -277,  -277,  -277,   228
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -203
static const yytype_int16 yytable[] =
{
      32,    55,    49,   109,   150,    53,   151,   208,   136,   218,
     235,   214,   215,    49,   164,   165,   167,    67,   171,   310,
     290,   319,   271,   291,    69,    12,   287,   -11,     3,   223,
     323,     7,   161,   -11,     4,   280,   220,    69,    73,    13,
     170,   183,   184,   212,    12,   279,    78,    79,    93,    81,
      70,    82,    83,   295,    10,    76,  -201,    73,   336,   197,
      54,   202,   216,   172,    77,    50,   226,    66,   310,    56,
      57,    58,    59,    60,   292,    95,    96,    97,    98,    99,
     100,   101,    68,    61,   274,   169,   299,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   263,
     321,   172,   221,   168,   107,   268,    24,    25,    26,    27,
      28,    29,    30,    62,   281,   282,   283,   284,   285,    38,
      39,    31,    40,   230,    -4,   272,    36,    37,    12,    74,
      75,    49,   120,    41,    42,    43,    44,    45,    46,    47,
      90,   150,   183,   184,   185,   136,   152,  -154,   329,    41,
      42,    43,    44,    45,    46,    47,  -201,   194,   195,   196,
     197,   121,   163,   198,   206,   122,    24,    25,    26,    27,
      28,    29,    30,   123,   199,   161,  -202,   326,   161,   203,
     207,    31,    94,   209,   222,   223,   225,   124,   228,   300,
     125,   126,   127,   227,   229,   150,   232,   236,   234,   128,
     129,   130,   131,    41,    42,    43,    44,    45,    46,    47,
     109,   161,   296,   265,   276,   277,   278,   327,   302,   150,
     294,    78,    79,    80,    81,   150,    82,    83,   150,   136,
     179,   180,   307,   105,   183,   184,   185,   132,   308,   313,
     311,   315,   303,   151,   317,    69,   324,   192,   193,   194,
     195,   196,   197,   202,   337,   330,   150,   335,   318,   333,
      23,    18,   288,   312,    65,   262,    49,   325,   179,   180,
     200,   298,   183,   184,   185,   314,   211,   320,   332,    52,
     289,   190,   191,     0,    89,   192,   193,   194,   195,   196,
     197,  -164,    91,     0,    73,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,    92,    78,    79,    93,    81,     0,
      82,    83,  -164,    94,     0,     0,     0,     0,  -164,     0,
    -164,  -164,  -164,  -164,     0,     0,     0,     0,     0,     0,
    -164,  -164,  -164,  -164,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -164,     0,     0,     0,     0,   102,     0,     0,
       0,     0,   103,   104,   105,     0,     0,     0,  -164,     0,
       0,   154,     0,   107,     0,  -164,  -193,   121,     0,     0,
    -193,   122,    24,    25,    26,    27,    28,    29,    30,   123,
       0,     0,     0,     0,     0,     0,     0,    31,    94,     0,
       0,     0,     0,   124,     0,  -193,   125,   126,   127,     0,
       0,     0,     0,     0,     0,   128,   129,   130,   131,    41,
      42,    43,    44,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,   -57,   121,     0,  -193,   -57,   122,
      24,    25,    26,    27,    28,    29,    30,   123,     0,   105,
       0,     0,     0,   132,     0,    31,    94,     0,    54,     0,
    -193,   124,     0,   -57,   125,   126,   127,     0,     0,     0,
       0,     0,     0,   128,   129,   130,   131,    41,    42,    43,
      44,    45,    46,    47,   183,   184,   185,     0,     0,     0,
       0,     0,     0,     0,     0,   -57,     0,   192,   193,   194,
     195,   196,   197,     0,     0,     0,     0,   105,     0,     0,
       0,   132,     0,     0,     0,    91,    54,     0,   -57,  -136,
    -136,  -136,  -136,  -136,  -136,  -136,  -136,    92,    78,    79,
      93,    81,     0,    82,    83,  -136,    94,     0,     0,     0,
       0,  -136,     0,     0,  -136,  -136,  -136,     0,     0,     0,
       0,     0,     0,  -136,  -136,  -136,  -136,    95,    96,    97,
      98,    99,   100,   101,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,     0,     0,     0,     0,   103,   104,   105,     0,     0,
       0,  -136,     0,     0,   154,  -136,   107,   267,    91,     0,
       0,     0,  -136,  -136,  -136,  -136,  -136,  -136,  -136,  -136,
      92,    78,    79,    93,    81,     0,    82,    83,  -136,    94,
       0,     0,     0,     0,  -136,     0,     0,  -136,  -136,  -136,
       0,     0,     0,     0,     0,     0,  -136,  -136,  -136,  -136,
      95,    96,    97,    98,    99,   100,   101,   153,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,    30,    92,
      78,    79,    93,    81,     0,    82,    83,    31,    94,     0,
     105,     0,     0,     0,  -136,     0,     0,     0,  -136,   107,
     267,     0,     0,     0,     0,     0,     0,     0,     0,    95,
      96,    97,    98,    99,   100,   101,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   102,     0,     0,     0,     0,   103,   104,   105,
       0,     0,     0,     0,   -95,    91,   154,   -50,   107,     0,
      24,    25,    26,    27,    28,    29,    30,    92,    78,    79,
      93,    81,     0,    82,    83,    31,    94,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    95,    96,    97,
      98,    99,   100,   101,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,     0,     0,     0,     0,   103,   104,   105,     0,     0,
       0,     0,    91,     0,   154,   -95,   107,    24,    25,    26,
      27,    28,    29,    30,    92,    78,    79,    93,    81,     0,
      82,    83,    31,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    91,     0,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,    92,    78,    79,    93,    81,     0,
      82,    83,     0,    94,     0,     0,     0,   102,     0,     0,
       0,     0,   103,   104,   105,     0,     0,     0,     0,     0,
       0,   154,    91,   107,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,    92,    78,    79,    93,    81,     0,
      82,    83,     0,    94,     0,     0,     0,   102,     0,     0,
       0,     0,   103,   104,   105,     0,     0,     0,     0,   -95,
       0,   154,   -95,   107,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   102,     0,     0,
       0,     0,   103,   104,   105,    91,     0,     0,   -95,     0,
       0,   106,     0,   107,     0,     0,     0,    92,    78,    79,
      93,    81,     0,    82,    83,     0,    94,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    91,     0,    95,    96,    97,
      98,    99,   100,   101,     0,     0,     0,    92,    78,    79,
      93,    81,     0,    82,    83,     0,    94,     0,     0,     0,
     102,     0,     0,     0,     0,   103,   104,   105,     0,     0,
       0,     0,     0,   -95,   154,    91,   107,    95,    96,    97,
      98,    99,   100,   101,     0,     0,     0,    92,    78,    79,
      93,    81,     0,    82,    83,     0,    94,     0,     0,     0,
     102,     0,     0,     0,     0,   103,   104,   105,     0,     0,
       0,     0,     0,     0,   154,   -95,   107,    95,    96,    97,
      98,    99,   100,   101,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,     0,     0,     0,     0,   103,   104,   105,    91,     0,
       0,     0,     0,     0,   154,     0,   107,     0,    91,     0,
      92,   166,    79,    93,    81,     0,    82,    83,     0,    94,
      92,    78,    79,    93,    81,     0,    82,    83,     0,    94,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,    96,    97,    98,    99,   100,   101,     0,     0,     0,
      95,    96,    97,    98,    99,   100,   101,     0,     0,     0,
       0,     0,   175,   102,     0,     0,     0,     0,   103,   104,
     105,     0,     0,     0,     0,     0,     0,   154,     0,   107,
     105,     0,     0,     0,     0,     0,     0,     0,   176,   107,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   175,
       0,   186,   187,   188,   189,   190,   191,     0,     0,   192,
     193,   194,   195,   196,   197,     0,     0,     0,     0,   233,
       0,     0,     0,     0,     0,   176,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,     0,     0,   186,   187,
     188,   189,   190,   191,     0,     0,   192,   193,   194,   195,
     196,   197,   176,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,     0,     0,   186,   187,   188,   189,   190,
     191,     0,     0,   192,   193,   194,   195,   196,   197,     0,
       0,     0,   176,   233,   177,   178,   179,   180,   181,   182,
     183,   184,   185,     0,     0,   186,   187,   188,   189,   190,
     191,     0,     0,   192,   193,   194,   195,   196,   197,     0,
       0,     0,  -102,   301,  -102,  -102,  -102,  -102,  -102,  -102,
    -102,  -102,  -102,     0,     0,  -102,  -102,  -102,  -102,  -102,
    -102,     0,     0,  -102,  -102,  -102,  -102,  -102,  -102,     0,
       0,     0,  -103,  -102,  -103,  -103,  -103,  -103,  -103,  -103,
    -103,  -103,  -103,     0,     0,  -103,  -103,  -103,  -103,  -103,
    -103,     0,     0,  -103,  -103,  -103,  -103,  -103,  -103,     0,
       0,     0,   176,  -103,   177,   178,   179,   180,   181,   182,
     183,   184,   185,     0,     0,   186,   187,   188,   189,   190,
     191,     0,     0,   192,   193,   194,   195,   196,   197,   176,
       0,   177,   178,   179,   180,     0,     0,   183,   184,   185,
       0,     0,   186,   187,   188,   189,   190,   191,     0,     0,
     192,   193,   194,   195,   196,   197,   176,     0,  -203,  -203,
     179,   180,     0,     0,   183,   184,   185,     0,     0,     0,
       0,     0,   189,   190,   191,     0,     0,   192,   193,   194,
     195,   196,   197,   176,     0,   177,   178,   179,   180,     0,
       0,   183,   184,   185,     0,     0,     0,     0,     0,   189,
     190,   191,     0,     0,   192,   193,   194,   195,   196,   197
};

static const yytype_int16 yycheck[] =
{
      16,    34,    22,    69,    72,    33,    73,   128,    72,   133,
     173,    25,    26,    33,   102,   103,   104,    14,   106,   276,
     231,   297,   203,    78,    51,     4,   219,     6,     0,    84,
     306,    13,    75,    12,     3,   216,     1,    51,    54,    18,
     106,    58,    59,   131,     4,    13,    14,    15,    16,    17,
      77,    19,    20,   264,    77,    13,    80,    73,   334,    76,
      82,    85,    76,   106,    22,    76,   154,    51,   325,     7,
       8,     9,    10,    11,   237,    43,    44,    45,    46,    47,
      48,    49,    79,    21,   205,   105,   267,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     303,   154,    77,    13,    82,   203,     6,     7,     8,     9,
      10,    11,    12,    61,    33,    34,    35,    36,    37,    13,
      14,    21,    16,   161,     0,   203,    77,    78,     4,    76,
      80,   161,    77,    43,    44,    45,    46,    47,    48,    49,
      79,   219,    58,    59,    60,   219,    79,    16,   321,    43,
      44,    45,    46,    47,    48,    49,    80,    73,    74,    75,
      76,     1,    80,    16,    13,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    80,   228,    80,   311,   231,    80,
      13,    21,    22,    14,    77,    84,    77,    27,    78,   267,
      30,    31,    32,    81,    78,   273,    81,    16,    81,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
     286,   264,   265,    80,    82,    77,    77,   315,    81,   297,
      79,    14,    15,    16,    17,   303,    19,    20,   306,   303,
      54,    55,    77,    73,    58,    59,    60,    77,    13,    81,
      79,    24,    82,   320,    81,    51,    84,    71,    72,    73,
      74,    75,    76,    85,    81,    29,   334,   333,   296,    85,
      15,    11,   228,   286,    37,   197,   296,   310,    54,    55,
     118,   267,    58,    59,    60,   291,   130,   303,   325,    26,
     229,    67,    68,    -1,    66,    71,    72,    73,    74,    75,
      76,     0,     1,    -1,   320,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    -1,
      19,    20,    21,    22,    -1,    -1,    -1,    -1,    27,    -1,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    71,    72,    73,    -1,    -1,    -1,    77,    -1,
      -1,    80,    -1,    82,    -1,    84,     0,     1,    -1,    -1,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    22,    -1,
      -1,    -1,    -1,    27,    -1,    29,    30,    31,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     0,     1,    -1,    61,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    73,
      -1,    -1,    -1,    77,    -1,    21,    22,    -1,    82,    -1,
      84,    27,    -1,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    58,    59,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    71,    72,    73,
      74,    75,    76,    -1,    -1,    -1,    -1,    73,    -1,    -1,
      -1,    77,    -1,    -1,    -1,     1,    82,    -1,    84,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    -1,    19,    20,    21,    22,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    77,    -1,    -1,    80,    81,    82,    83,     1,    -1,
      -1,    -1,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    22,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,     1,    -1,    -1,
      -1,    -1,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    -1,    19,    20,    21,    22,    -1,
      73,    -1,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    -1,    -1,    -1,    -1,    71,    72,    73,
      -1,    -1,    -1,    -1,    78,     1,    80,    81,    82,    -1,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    -1,    19,    20,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,     1,    -1,    80,    81,    82,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    -1,
      19,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,    43,    44,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    13,    14,    15,    16,    17,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    71,    72,    73,    -1,    -1,    -1,    -1,    -1,
      -1,    80,     1,    82,    43,    44,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    13,    14,    15,    16,    17,    -1,
      19,    20,    -1,    22,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    71,    72,    73,    -1,    -1,    -1,    -1,    78,
      -1,    80,    81,    82,    43,    44,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    71,    72,    73,     1,    -1,    -1,    77,    -1,
      -1,    80,    -1,    82,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    79,    80,     1,    82,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,     1,    -1,
      -1,    -1,    -1,    -1,    80,    -1,    82,    -1,     1,    -1,
      13,    14,    15,    16,    17,    -1,    19,    20,    -1,    22,
      13,    14,    15,    16,    17,    -1,    19,    20,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    -1,
      -1,    -1,    24,    66,    -1,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    -1,    -1,    -1,    -1,    80,    -1,    82,
      73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    82,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    24,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    76,    -1,    -1,    -1,    -1,    81,
      -1,    -1,    -1,    -1,    -1,    50,    -1,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    64,
      65,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      75,    76,    50,    -1,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    76,    -1,
      -1,    -1,    50,    81,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    76,    -1,
      -1,    -1,    50,    81,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    76,    -1,
      -1,    -1,    50,    81,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    76,    -1,
      -1,    -1,    50,    81,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    76,    50,
      -1,    52,    53,    54,    55,    -1,    -1,    58,    59,    60,
      -1,    -1,    63,    64,    65,    66,    67,    68,    -1,    -1,
      71,    72,    73,    74,    75,    76,    50,    -1,    52,    53,
      54,    55,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    75,    76,    50,    -1,    52,    53,    54,    55,    -1,
      -1,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,    75,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    87,    88,     0,     3,    90,    91,    13,    89,    92,
      77,    96,     4,    18,    93,    94,    95,    97,    95,   112,
      98,    99,   100,    93,     6,     7,     8,     9,    10,    11,
      12,    21,   115,   175,   107,   108,    77,    78,    13,    14,
      16,    43,    44,    45,    46,    47,    48,    49,   103,   104,
      76,   176,   176,   103,    82,   113,     7,     8,     9,    10,
      11,    21,    61,   105,   106,    99,    51,    14,    79,    51,
      77,   121,   114,   115,    76,    80,    13,    22,    14,    15,
      16,    17,    19,    20,   102,   177,   178,   179,   180,   181,
      79,     1,    13,    16,    22,    43,    44,    45,    46,    47,
      48,    49,    66,    71,    72,    73,    80,    82,   122,   123,
     125,   129,   130,   134,   135,   140,   168,   171,   174,   181,
      77,     1,     5,    13,    27,    30,    31,    32,    39,    40,
      41,    42,    77,   104,   113,   115,   116,   117,   118,   128,
     141,   142,   143,   146,   148,   149,   153,   156,   161,   166,
     168,   114,    79,     1,    80,   109,   110,   111,   123,   172,
     173,   175,   101,    80,   130,   130,    14,   130,    13,   104,
     123,   130,   175,   136,   137,    24,    50,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    63,    64,    65,    66,
      67,    68,    71,    72,    73,    74,    75,    76,    16,    80,
     129,   154,    85,    80,   131,   157,    13,    13,   131,    14,
     145,   145,   130,   144,    25,    26,    76,   119,   121,   167,
       1,    77,    77,    84,   160,    77,   130,    81,    78,    78,
     103,   169,    81,    81,    81,   160,    16,   138,   139,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   124,   125,   130,   170,    80,   151,    83,   130,   132,
     133,   134,   168,   162,   131,   150,    82,    77,    77,    13,
     134,    33,    34,    35,    36,    37,   120,   166,   111,   173,
     109,    78,   160,   126,    79,   109,   175,   155,   132,   134,
     168,    81,    81,    82,   116,   159,   158,    77,    13,   147,
     148,    79,   122,    81,   139,    24,   127,    81,   103,   159,
     115,   166,   163,   159,    84,   113,   121,   130,   152,   160,
      29,   164,   147,    85,   165,   123,   159,    81
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
        case 2:

/* Line 1455 of yacc.c  */
#line 178 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 192 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 195 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace((yyvsp[(2) - (3)]).ident.s);
				free((yyvsp[(2) - (3)]).ident.s);
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 209 "Parser.y"
    { _macroScope = MS_GLOBAL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 210 "Parser.y"
    { _macroScope = MS_LOCAL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 223 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 223 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 224 "Parser.y"
    {
				BOOL bString = ((yyvsp[(4) - (5)]).v.type == S_STRING);
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					constantTypeFor((yyvsp[(4) - (5)]).v.type), (yyvsp[(4) - (5)]).v.data);
				free((yyvsp[(2) - (5)]).ident.s);
				if (bString) {
					free((yyvsp[(4) - (5)]).v.data.string);
				}
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 235 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 244 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 245 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 247 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 260 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 261 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 262 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 264 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 266 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 267 "Parser.y"
    {   
				yyerror("Using undeclared variable %s", (yyvsp[(1) - (1)]).ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(1) - (1)]).ident.s, S_NUMBER, (GENERIC_DATA) {0});
				free((yyvsp[(1) - (1)]).ident.s);
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 295 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 308 "Parser.y"
    {
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 315 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(2) - (3)]).ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(2) - (3)]).ident.s);
					MACRO_PARAM param = {
						.mp_name = (yyvsp[(2) - (3)]).ident.s,
						.mp_comment = _yyCurrentComment,
						.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
						.mp_buffer = _bytecodeBuffer.bb_start,
						.mp_scope = _macroScope,
						.mp_namespaceIdx = _currentNamespaceIndex
					};
					_compilerConfiguration->cb_insertNewMacro(&param);
				}
				freeitem(&(yyvsp[(2) - (3)]).ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s, (yyvsp[(1) - (4)]).type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(2) - (4)]).ident.s);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, 0);
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 366 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 368 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 369 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 370 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 375 "Parser.y"
    {
				(yyval).needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)(yyvsp[(1) - (5)]).ident.s });
				vname_count = 0;
			;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
			;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    {
				YY_EMIT(C_BINOP, (GENERIC_DATA){(yyvsp[(3) - (4)]).type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (4)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 405 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 406 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 407 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 409 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 411 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 412 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 414 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 428 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 432 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 435 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 436 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-(yyvsp[(2) - (2)]).num);
				} else {
					(yyval).type = S_NUMBER;
					(yyval).v.type = C_PUSH_LONG_LITERAL; 
					(yyval).v.data.longValue  = -(yyvsp[(2) - (2)]).num;
				}
			;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 455 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 458 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 459 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 462 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 463 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 465 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 466 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 467 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 468 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 470 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 471 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 472 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 474 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 480 "Parser.y"
    {
				yyerror("Missing closing parenthesis )");
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 488 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 489 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 495 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 499 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_ARRAY;
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 503 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_RANGE;
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 507 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 515 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 519 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 524 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {	
				(yyval).type = S_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 537 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(1);
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 541 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(3);
			;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 550 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, S_STRING, (yyvsp[(1) - (1)]).ident.s));
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
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

  case 161:

/* Line 1455 of yacc.c  */
#line 567 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 570 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 573 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 580 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 581 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 588 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 593 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 602 "Parser.y"
    {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(3) - (3)]).ident.s});
				makeInternalSymbol((yyvsp[(3) - (3)]).ident.s, (yyvsp[(2) - (3)]).type, (intptr_t)0);
				if ((yyvsp[(3) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(3) - (3)]).ident.s);
				}
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 609 "Parser.y"
    {
				PKS_VALUE 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 616 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 618 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 621 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 629 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 631 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 634 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 645 "Parser.y"
    { yyerror("Missing closing brace '}'"); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 650 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 653 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 655 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 660 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 664 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 671 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 672 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 678 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (2)]).funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = (yyvsp[(1) - (2)]).funcp;
			;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 684 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 689 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = 0;
			;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 694 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = (yyvsp[(1) - (5)]).ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&(yyvsp[(1) - (5)]).ident.s);
			;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 701 "Parser.y"
    {
				_functionCallOp = C_MACRO_REF;
				(yyval).ident.s = _strdup((yyvsp[(2) - (2)]).ident.s);
				(yyval).ident.stringIsAlloced = 1;
			;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 707 "Parser.y"
    {
				_functionCallOp = C_MACRO;
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 711 "Parser.y"
    {
				yyerror("Illegal function call expression");
				yyerrok;
				(yyval).ident.s = 0;
			;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 721 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 725 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; (*_currentFunctionCallParamIndexP)++; ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 727 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 729 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 730 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 731 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 732 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 733 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 734 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 735 "Parser.y"
    {	(yyval).type = S_ARRAY;	;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 736 "Parser.y"
    {	(yyval).type = S_ARRAY;	;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 737 "Parser.y"
    {	(yyval).type = S_AUTO; ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 738 "Parser.y"
    {	(yyval).type = S_RANGE;	;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 741 "Parser.y"
    {
				_arraySize = (long)(yyvsp[(2) - (3)]).num;
			;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 745 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 748 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 753 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 758 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 763 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 768 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 776 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 784 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 792 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_BOOLEAN;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3591 "Parser.tab.c"
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
#line 800 "Parser.y"


/**
 * bytecode_emitPushParameterInstruction()
 * Special processing when pushing integer literals: if the integer literal is pushed as 
 * parameter of a native function and this is form-related native function push a special
 * byte code as a marker.
 */
static void bytecode_emitPushParameterInstruction(long long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,*_currentFunctionCallParamIndexP+1)) {
		opCode = C_FORM_START;
	}
	YY_EMIT(opCode, (GENERIC_DATA){.longValue = lValue});
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

static void init_bytecodeBuffer() {
	if (!_bytecodeBuffer.bb_start) {
		_bytecodeBuffer.bb_end = ((_bytecodeBuffer.bb_start = malloc(REC_SPACE)) == 0) ? 
			_bytecodeBuffer.bb_start : _bytecodeBuffer.bb_start + REC_SPACE;
	}
	_bytecodeBuffer.bb_current = _bytecodeBuffer.bb_start; 
}

static void destroy_bytecodeBuffer() {
	if (_bytecodeBuffer.bb_start) {
		free(_bytecodeBuffer.bb_start);
		_bytecodeBuffer.bb_start = 0;
		_bytecodeBuffer.bb_end = 0;
	}
}

static void init_header() {
	init_bytecodeBuffer(); 
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _bytecodeBuffer.bb_start, _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start);
	destroy_bytecodeBuffer();
}

/*---------------------------------*/
/* init_macroDefinition()				*/
/*---------------------------------*/
static void init_macroDefinition(void)
{
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	_foreachCursorIndex = 0;
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	init_bytecodeBuffer();
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
}

/*---------------------------------*/
/* finalize_macroDefinition()					*/
/*---------------------------------*/
static void finalize_macroDefinition(void) {
	destroy_bytecodeBuffer();
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


