
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
extern void		yywarning(char* s, ...);
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
	if (pContext && pContext != sym_getGlobalCompilerContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
	if (sType == VT_STRING) {
		if (!tVal) {
			tVal = (intptr_t)"";
		}
			

	}
	sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal});
}

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}



/* Line 189 of yacc.c  */
#line 213 "Parser.tab.c"

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
     T_TYPE_IDENTIFIER = 261,
     T_IDENT = 262,
     T_NUM = 263,
     T_FLOATING_POINT_NUMBER = 264,
     T_STRING = 265,
     T_CHARACTER = 266,
     T_CONST = 267,
     T_TRUE = 268,
     T_FALSE = 269,
     T_TBOOLEAN = 270,
     T_FUNC = 271,
     T_MACRO = 272,
     T_DOTDOT = 273,
     T_PLUSPLUS = 274,
     T_MINUSMINUS = 275,
     T_IF = 276,
     T_ELIF = 277,
     T_ELSE = 278,
     T_WHILE = 279,
     T_GOTO = 280,
     T_BRAEQ = 281,
     T_SH_ASSIGN_PLUS = 282,
     T_SH_ASSIGN_MINUS = 283,
     T_SH_ASSIGN_MULT = 284,
     T_SH_ASSIGN_DIV = 285,
     T_SH_ASSIGN_MOD = 286,
     T_LOCAL = 287,
     T_SWITCH = 288,
     T_BREAK = 289,
     T_CONTINUE = 290,
     T_RETURN = 291,
     T_VARIABLE = 292,
     T_ASSOC_ARROW = 293,
     T_NMATCH = 294,
     T_ASSIGN = 295,
     T_NE = 296,
     T_EQ = 297,
     T_SHIFT_LEFT = 298,
     T_SHIFT_RIGHT = 299,
     T_OR = 300,
     T_AND = 301,
     T_LE = 302,
     T_GE = 303,
     T_POWER_TO = 304,
     T_VOID = 305,
     T_CMDSEQPREFIX = 306,
     T_GT = 307,
     T_LT = 308
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
#line 308 "Parser.tab.c"

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
#define YYLAST   1181

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  101
/* YYNRULES -- Number of rules.  */
#define YYNRULES  216
/* YYNRULES -- Number of states.  */
#define YYNSTATES  330

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   308

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,     2,     2,     2,    64,    52,     2,
      68,    69,    62,    60,    67,    61,     2,    63,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    74,    66,
      56,     2,    57,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    71,    54,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    53,    73,    55,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    58,    59
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    14,    18,    19,
      22,    25,    26,    29,    32,    33,    35,    36,    39,    43,
      45,    49,    50,    51,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    76,    79,    81,    83,    84,    91,    92,
      94,    95,    97,   101,   103,   106,   110,   115,   116,   119,
     124,   128,   130,   133,   136,   138,   140,   142,   144,   146,
     148,   150,   152,   154,   156,   158,   160,   163,   165,   168,
     171,   177,   178,   183,   185,   187,   189,   191,   193,   196,
     198,   202,   204,   206,   208,   210,   211,   212,   218,   219,
     222,   225,   228,   230,   232,   234,   237,   240,   245,   248,
     251,   254,   258,   262,   266,   270,   274,   278,   282,   286,
     290,   294,   298,   302,   306,   310,   314,   318,   322,   326,
     330,   334,   338,   342,   346,   350,   353,   354,   356,   359,
     361,   363,   365,   367,   369,   371,   373,   375,   379,   383,
     385,   389,   393,   394,   398,   399,   404,   408,   410,   412,
     414,   416,   418,   420,   422,   424,   427,   431,   435,   438,
     439,   441,   442,   444,   450,   451,   455,   458,   461,   462,
     467,   468,   476,   477,   478,   484,   485,   486,   492,   494,
     498,   499,   501,   502,   503,   510,   511,   512,   516,   517,
     518,   522,   523,   529,   530,   536,   539,   541,   543,   545,
     547,   551,   553,   555,   559,   561,   564,   567,   571,   573,
     575,   577,   579,   581,   583,   585,   587
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      76,     0,    -1,    -1,    -1,    77,    79,    78,    85,    -1,
      80,    81,    -1,    -1,     3,     7,    66,    -1,    -1,    81,
      86,    -1,    81,    82,    -1,    -1,    83,    82,    -1,    84,
     105,    -1,    -1,     4,    -1,    -1,    85,   102,    -1,    12,
      87,    66,    -1,    88,    -1,    87,    67,    88,    -1,    -1,
      -1,    89,    92,    40,    91,    90,    -1,   175,    -1,    10,
      -1,     7,    -1,    93,    -1,     8,    -1,    10,    -1,    37,
      -1,     7,    -1,    95,     7,    -1,    95,    16,    -1,    50,
      -1,     6,    -1,    -1,    97,    94,    68,    99,    69,    98,
      -1,    -1,    10,    -1,    -1,   100,    -1,   100,    67,   101,
      -1,   101,    -1,   169,    92,    -1,    84,    96,   103,    -1,
      70,   104,   160,   154,    -1,    -1,   105,   104,    -1,   169,
      92,   111,    66,    -1,   169,    92,    66,    -1,    66,    -1,
     162,    66,    -1,   108,    66,    -1,   155,    -1,   150,    -1,
     147,    -1,   140,    -1,   135,    -1,   136,    -1,   137,    -1,
     143,    -1,   103,    -1,   118,    -1,   142,    -1,   105,    -1,
     107,    66,    -1,     1,    -1,   107,     1,    -1,    93,   111,
      -1,    93,    65,   124,    71,   111,    -1,    -1,    93,   109,
     110,   112,    -1,    29,    -1,    30,    -1,    31,    -1,    27,
      -1,    28,    -1,    40,   112,    -1,   113,    -1,    68,   113,
      69,    -1,   120,    -1,   115,    -1,   120,    -1,   115,    -1,
      -1,    -1,   120,    18,   120,   116,   117,    -1,    -1,    18,
     120,    -1,    93,    20,    -1,    93,    19,    -1,   124,    -1,
     162,    -1,   119,    -1,   168,   119,    -1,    61,     8,    -1,
     120,    65,   114,    71,    -1,    55,   120,    -1,    60,   120,
      -1,    61,   120,    -1,   120,    55,   120,    -1,   120,    39,
     120,    -1,   120,    52,   120,    -1,   120,    53,   120,    -1,
     120,    60,   120,    -1,   120,    54,   120,    -1,   120,    61,
     120,    -1,   120,    62,   120,    -1,   120,    63,   120,    -1,
     120,    64,   120,    -1,   120,    43,   120,    -1,   120,    44,
     120,    -1,   120,    49,   120,    -1,   120,    46,   120,    -1,
     120,    45,   120,    -1,   120,    56,   120,    -1,   120,    57,
     120,    -1,   120,    47,   120,    -1,   120,    48,   120,    -1,
     120,    42,   120,    -1,   120,    41,   120,    -1,    68,   120,
      69,    -1,    68,   122,    69,    -1,    68,   120,    69,    -1,
      68,   122,    -1,    -1,   123,    -1,    72,   122,    -1,   162,
      -1,   124,    -1,    37,    -1,   134,    -1,   175,    -1,   125,
      -1,   128,    -1,     7,    -1,    68,   126,    69,    -1,   126,
      67,   127,    -1,   127,    -1,    10,    38,   175,    -1,    10,
      38,    10,    -1,    -1,    70,   129,   154,    -1,    -1,    70,
     130,   131,   154,    -1,   131,    67,   132,    -1,   132,    -1,
     133,    -1,    10,    -1,     8,    -1,    11,    -1,    13,    -1,
      14,    -1,    10,    -1,   134,    10,    -1,    34,   139,    66,
      -1,    35,   139,    66,    -1,    36,   138,    -1,    -1,   120,
      -1,    -1,     8,    -1,    33,   121,    70,   141,    73,    -1,
      -1,   142,   103,   141,    -1,     7,    74,    -1,    25,     7,
      -1,    -1,    26,     7,   144,    66,    -1,    -1,    68,   169,
      92,   146,    74,   113,    69,    -1,    -1,    -1,     5,   148,
     145,   149,   153,    -1,    -1,    -1,    24,   151,   121,   152,
     153,    -1,   106,    -1,    70,   160,   154,    -1,    -1,    73,
      -1,    -1,    -1,    21,   121,   156,   153,   157,   158,    -1,
      -1,    -1,    23,   159,   153,    -1,    -1,    -1,   106,   161,
     160,    -1,    -1,    16,    68,   163,    99,    69,    -1,    -1,
     165,    68,   164,    99,    69,    -1,    62,    93,    -1,     7,
      -1,     1,    -1,   166,    -1,   167,    -1,   166,    67,   167,
      -1,     1,    -1,   113,    -1,    68,   169,    69,    -1,     6,
      -1,     6,   170,    -1,    65,    71,    -1,    65,     8,    71,
      -1,    13,    -1,    14,    -1,     9,    -1,     8,    -1,    11,
      -1,   173,    -1,   172,    -1,   174,    -1,   171,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   170,   170,   176,   170,   182,   184,   187,   192,   193,
     194,   196,   197,   199,   201,   202,   204,   205,   209,   212,
     213,   215,   215,   215,   229,   232,   238,   239,   240,   247,
     254,   255,   265,   268,   274,   275,   277,   277,   283,   285,
     290,   291,   294,   296,   298,   305,   328,   330,   331,   333,
     340,   348,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   365,   371,   372,   374,
     379,   384,   384,   393,   394,   395,   396,   397,   399,   401,
     402,   404,   405,   408,   409,   411,   412,   412,   414,   415,
     418,   420,   422,   423,   425,   426,   430,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   468,   469,   470,   474,   475,   476,   478,
     479,   481,   485,   490,   491,   495,   501,   507,   510,   511,
     514,   515,   517,   517,   521,   521,   526,   527,   530,   534,
     539,   540,   541,   542,   544,   545,   556,   559,   563,   567,
     568,   570,   571,   573,   575,   576,   578,   583,   587,   587,
     592,   592,   606,   608,   606,   619,   621,   619,   632,   632,
     635,   637,   640,   643,   640,   650,   654,   654,   661,   662,
     662,   668,   668,   679,   679,   691,   697,   701,   707,   709,
     710,   711,   715,   717,   719,   720,   722,   723,   727,   730,
     735,   740,   745,   750,   758,   766,   774
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NAMESPACE", "T_STATIC", "T_FOR",
  "T_TYPE_IDENTIFIER", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_CHARACTER", "T_CONST", "T_TRUE", "T_FALSE", "T_TBOOLEAN",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF",
  "T_ELIF", "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_VARIABLE", "T_ASSOC_ARROW", "T_NMATCH", "T_ASSIGN",
  "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE",
  "T_GE", "T_POWER_TO", "T_VOID", "T_CMDSEQPREFIX", "'&'", "'|'", "'^'",
  "'~'", "'<'", "'>'", "T_GT", "T_LT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'['", "';'", "','", "'('", "')'", "'{'", "']'", "'!'", "'}'", "':'",
  "$accept", "file_structure", "$@1", "$@2", "header_declarations",
  "namespace", "global_declarations", "global_vars", "global_var", "scope",
  "macro_definitions", "constants", "constant_list", "constdef", "$@3",
  "$@4", "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@5",
  "optional_description", "parameter_list", "non_empty_pardecl",
  "par_decl", "macro_definition", "block", "var_decls", "var_decl",
  "statement", "errors", "assignment", "$@6",
  "shorthand_assignment_operator", "assignment_expression",
  "arithmetic_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@7",
  "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "logical_expression", "term", "value",
  "map_literal", "map_associates", "map_associate", "array_literal", "$@8",
  "$@9", "array_elements", "array_element", "simple_array_element",
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
     305,   306,    38,   124,    94,   126,    60,    62,   307,   308,
      43,    45,    42,    47,    37,    91,    59,    44,    40,    41,
     123,    93,    33,   125,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    77,    78,    76,    79,    80,    80,    81,    81,
      81,    82,    82,    83,    84,    84,    85,    85,    86,    87,
      87,    89,    90,    88,    91,    91,    92,    92,    92,    92,
      93,    93,    94,    94,    95,    95,    97,    96,    98,    98,
      99,    99,   100,   100,   101,   102,   103,   104,   104,   105,
     105,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   107,   107,   108,
     108,   109,   108,   110,   110,   110,   110,   110,   111,   112,
     112,   113,   113,   114,   114,   115,   116,   115,   117,   117,
     118,   118,   119,   119,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   121,   121,   121,   122,   122,   122,   123,
     123,   124,   124,   124,   124,   124,   124,   125,   126,   126,
     127,   127,   129,   128,   130,   128,   131,   131,   132,   132,
     133,   133,   133,   133,   134,   134,   135,   136,   137,   138,
     138,   139,   139,   140,   141,   141,   142,   143,   144,   143,
     146,   145,   148,   149,   147,   151,   152,   150,   153,   153,
     154,   154,   156,   157,   155,   158,   159,   158,   160,   161,
     160,   163,   162,   164,   162,   165,   165,   165,    99,   166,
     166,   166,   167,   168,   169,   169,   170,   170,   171,   171,
     172,   173,   174,   175,   175,   175,   175
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     0,     1,     0,     2,     3,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     1,     0,     6,     0,     1,
       0,     1,     3,     1,     2,     3,     4,     0,     2,     4,
       3,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       5,     0,     4,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     1,     1,     1,     0,     0,     5,     0,     2,
       2,     2,     1,     1,     1,     2,     2,     4,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     0,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     3,     0,     3,     0,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     3,     2,     0,
       1,     0,     1,     5,     0,     3,     2,     2,     0,     4,
       0,     7,     0,     0,     5,     0,     0,     5,     1,     3,
       0,     1,     0,     0,     6,     0,     0,     3,     0,     0,
       3,     0,     5,     0,     5,     2,     1,     1,     1,     1,
       3,     1,     1,     3,     1,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    16,     5,
       7,    14,    15,    21,    10,    11,     0,     9,    36,    17,
       0,    19,     0,    12,   204,    13,     0,     0,     0,    18,
      21,    26,    28,    29,    30,     0,    27,     0,   205,     0,
      47,    45,    35,    34,     0,     0,    20,     0,     0,   206,
       0,    50,     0,     0,    47,     0,    32,    33,   211,   210,
      25,   212,   208,   209,    22,   216,   214,   213,   215,    24,
     207,   197,   136,   154,     0,   131,     0,     0,     0,     0,
       0,   142,    78,    79,    82,    94,    81,    92,   134,   135,
     132,    93,     0,     0,   133,    49,    67,   172,    31,     0,
     175,     0,     0,     0,   161,   161,     0,    51,    71,    62,
      65,   189,     0,     0,    63,    58,    59,    60,    57,    64,
      61,    56,    55,    54,   180,     0,    48,   201,     0,     0,
      41,    43,   202,   198,   199,     0,    23,   191,    98,    99,
      96,   100,    31,   195,   154,     0,     0,     0,   139,     0,
     180,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   155,   193,     0,    95,     0,
     166,     0,   182,     0,   167,   168,     0,   162,     0,     0,
     160,   158,    91,    90,     0,     0,    69,     0,    68,    66,
      53,   181,    46,    52,     0,    38,     0,     0,    44,     0,
       0,    80,   122,     0,   137,   203,   143,   150,   149,   151,
     152,   153,   180,   147,   148,    86,   102,   121,   120,   111,
     112,   115,   114,   118,   119,   113,   103,   104,   106,   101,
     116,   117,   105,   107,   108,   109,   110,     0,    84,    83,
       0,     0,     0,   173,     0,     0,   125,   127,    92,    93,
       0,   176,     0,   164,   156,   157,   136,     0,    76,    77,
      73,    74,    75,     0,   190,    39,    37,    42,   200,     0,
     141,   140,   138,     0,   145,    88,    97,     0,     0,     0,
     128,   130,   129,   124,   123,     0,   178,   183,     0,   169,
       0,     0,     0,     0,    72,   192,   146,     0,    87,   194,
     170,   174,    47,   180,   185,   177,   163,   164,    70,    89,
       0,   179,   186,   184,   165,     0,     0,     0,   187,   171
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    14,    15,    16,
      11,    17,    20,    21,    22,   136,    64,    35,   108,    44,
      45,    27,    28,   276,   129,   130,   131,    19,   109,    53,
     110,   296,   112,   113,   195,   273,    52,    82,   132,   247,
      84,   285,   308,   114,    85,    86,   182,   256,   257,    87,
      88,   147,   148,    89,   150,   151,   222,   223,   224,    90,
     115,   116,   117,   191,   188,   118,   301,   119,   120,   262,
     253,   320,   121,   179,   289,   122,   183,   298,   297,   202,
     123,   260,   314,   323,   326,   124,   197,    91,   209,   250,
      92,   133,   134,    93,    26,    38,    65,    66,    67,    68,
      94
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -265
static const yytype_int16 yypact[] =
{
    -265,    37,    47,  -265,    46,  -265,  -265,    -7,  -265,    43,
    -265,    79,  -265,  -265,  -265,    63,    70,  -265,  -265,  -265,
      53,  -265,    19,  -265,    58,  -265,    19,    41,    15,  -265,
    -265,  -265,  -265,  -265,  -265,    84,  -265,    10,  -265,   -12,
      70,  -265,  -265,  -265,    71,    28,  -265,   243,    72,  -265,
     614,  -265,    74,   281,    70,   509,  -265,  -265,  -265,  -265,
    -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,
    -265,  -265,    73,  -265,    80,  -265,   764,   764,   775,    26,
     544,    38,  -265,  -265,  -265,  -265,   927,  -265,  -265,  -265,
     141,  -265,    97,   363,  -265,  -265,   100,  -265,   -38,   101,
    -265,   145,   163,   101,   164,   164,   121,  -265,     3,  -265,
    -265,  -265,    14,   109,  -265,  -265,  -265,  -265,  -265,  -265,
    -265,  -265,  -265,  -265,   104,   112,  -265,   100,   649,   110,
     113,  -265,  -265,   117,  -265,    19,  -265,  -265,   861,    22,
    -265,    22,  -265,  -265,   147,   119,   895,    69,  -265,   124,
     104,    64,   764,   764,   764,   764,   764,   764,   764,   764,
     764,   764,   764,   764,   764,   764,   764,   764,   764,   764,
     764,   764,   764,   764,   200,  -265,  -265,   176,  -265,   134,
    -265,   402,  -265,   101,  -265,  -265,   135,  -265,   138,   149,
    1012,  -265,  -265,  -265,   153,   169,  -265,   281,  -265,  -265,
    -265,  -265,  -265,  -265,   954,   210,    70,   579,  -265,   509,
     311,  -265,  -265,   176,  -265,  -265,  -265,  -265,  -265,  -265,
    -265,  -265,    86,  -265,  -265,  1012,   861,  1066,  1066,   170,
     170,  1039,  1039,  1012,  1012,    -8,  1093,  1093,  1093,   861,
    1116,  1116,    22,    22,    -8,    -8,    -8,   151,  -265,   927,
     509,   147,    70,  -265,   472,   983,   156,  -265,   841,   863,
     700,  -265,   160,   220,  -265,  -265,  -265,   157,  -265,  -265,
    -265,  -265,  -265,   614,  -265,  -265,  -265,  -265,  -265,   171,
    -265,  -265,  -265,    64,  -265,   211,  -265,   173,    19,   700,
    -265,  -265,  -265,  -265,  -265,   332,  -265,  -265,   700,  -265,
     165,   172,    41,   204,  -265,  -265,  -265,   764,  -265,  -265,
    -265,  -265,    70,   104,   223,  -265,  -265,   220,  -265,  1012,
     175,  -265,  -265,  -265,  -265,   686,   700,   189,  -265,  -265
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -265,  -265,  -265,  -265,  -265,  -265,  -265,   244,  -265,   252,
    -265,  -265,  -265,   234,  -265,  -265,  -265,   -22,   -19,  -265,
    -265,  -265,  -265,  -265,  -189,  -265,    59,  -265,   -25,   -45,
     -16,   -47,  -265,  -265,  -265,  -265,  -100,    -1,   -49,  -265,
      99,  -265,  -265,  -265,   181,   -64,   -71,    21,  -265,  -136,
    -265,  -265,    67,  -265,  -265,  -265,  -265,     0,  -265,  -265,
    -265,  -265,  -265,  -265,   179,  -265,   -28,  -244,  -265,  -265,
    -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,  -264,  -140,
    -265,  -265,  -265,  -265,  -265,  -180,  -265,   -48,  -265,  -265,
    -265,  -265,    83,  -265,   -14,  -265,  -265,  -265,  -265,  -265,
     -36
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -198
static const yytype_int16 yytable[] =
{
      25,    83,    41,    36,    39,   125,   111,    36,   196,   126,
     216,    69,   138,   139,   141,   198,   146,   274,    48,   302,
     279,    42,   192,   193,    54,   311,    31,    32,    50,    33,
    -196,   145,   186,   142,   315,    56,   180,     3,    54,   160,
     161,   135,   190,    50,    57,   258,  -144,    12,  -144,  -144,
       4,  -144,  -144,     7,    51,    13,    34,   174,   267,    10,
     143,   287,   328,    34,   204,    43,   149,    12,   194,   160,
     161,   162,   217,   302,   218,   219,    24,   220,   221,    -4,
     199,    49,   284,    12,   171,   172,   173,   174,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     249,    40,   261,   208,   149,   313,    36,   255,   291,    29,
      30,  -159,    71,    37,    47,  -159,  -159,  -159,    72,    58,
      59,    73,    61,   259,    62,    63,   213,    74,   214,    55,
      95,  -196,  -159,    70,  -159,  -159,  -159,  -159,   137,   125,
     111,   175,   184,   283,  -159,  -159,  -159,  -159,    75,   201,
     266,    58,    59,    73,    61,   176,    62,    63,  -197,   181,
     185,  -159,   187,   321,   281,   200,    76,   201,   203,   205,
     206,    77,    78,    79,   207,   210,   251,  -159,   211,   128,
      75,    81,   135,   215,  -159,   135,   268,   269,   270,   271,
     272,    71,   252,   318,   264,   263,   292,    72,    58,    59,
      73,    61,   125,    62,    63,   265,    74,   160,   161,   162,
     275,   177,   286,    81,    83,   294,   299,   300,   303,   307,
     169,   170,   171,   172,   173,   174,   135,    75,   288,   180,
     305,   125,   309,   319,    50,   316,   322,   125,   111,   325,
     125,    58,    59,    60,    61,    76,    62,    63,   329,    23,
      77,    78,    79,    18,    46,   277,   310,   126,   128,    36,
      81,   -85,   304,   248,   178,   290,   327,   317,   125,   312,
     282,  -188,    96,   306,   189,  -188,    97,    24,    98,   324,
     278,     0,     0,     0,     0,     0,    54,    74,     0,     0,
       0,     0,    99,     0,  -188,   100,   101,   102,     0,     0,
       0,     0,     0,     0,   103,   104,   105,   106,    34,    58,
      59,   280,    61,     0,    62,    63,     0,     0,     0,     0,
       0,  -188,   -47,    96,     0,     0,   -47,    97,    24,    98,
       0,     0,     0,    79,     0,     0,     0,   107,    74,     0,
       0,    40,     0,    99,  -188,   -47,   100,   101,   102,     0,
       0,     0,     0,     0,    71,   103,   104,   105,   106,    34,
      72,    58,    59,    73,    61,     0,    62,    63,     0,    74,
       0,     0,   -47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    79,     0,     0,     0,   107,     0,
      75,     0,    40,    71,     0,   -47,     0,  -126,  -126,    72,
      58,    59,    73,    61,     0,    62,    63,     0,    74,     0,
       0,     0,     0,  -126,     0,    79,  -126,  -126,  -126,     0,
       0,   177,     0,    81,     0,  -126,  -126,  -126,  -126,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    76,     0,     0,
       0,     0,    77,    78,    79,     0,     0,     0,  -126,     0,
     128,  -126,    81,    71,   254,     0,     0,  -126,  -126,    72,
      58,    59,    73,    61,     0,    62,    63,     0,    74,     0,
       0,     0,     0,  -126,     0,     0,  -126,  -126,  -126,     0,
       0,     0,     0,     0,     0,  -126,  -126,  -126,  -126,    75,
     127,     0,     0,     0,     0,    24,    72,    58,    59,    73,
      61,     0,    62,    63,     0,    74,     0,     0,     0,     0,
       0,     0,     0,     0,    79,     0,     0,     0,  -126,     0,
     177,  -126,    81,     0,   254,    71,    75,     0,     0,     0,
      24,    72,    58,    59,   144,    61,     0,    62,    63,     0,
      74,     0,     0,     0,    76,     0,     0,     0,     0,    77,
      78,    79,     0,     0,     0,     0,   -85,   128,   -40,    81,
      71,    75,     0,     0,     0,     0,    72,    58,    59,    73,
      61,     0,    62,    63,     0,    74,     0,     0,     0,    76,
       0,     0,     0,     0,    77,    78,    79,     0,     0,     0,
       0,     0,   128,   -85,    81,    71,    75,     0,     0,     0,
       0,    72,    58,    59,    73,    61,     0,    62,    63,     0,
      74,     0,     0,     0,    76,     0,     0,     0,     0,    77,
      78,    79,     0,     0,     0,     0,   -85,   128,   -85,    81,
      71,    75,     0,     0,     0,    24,    72,    58,    59,   144,
      61,     0,    62,    63,     0,    74,     0,     0,     0,    76,
       0,     0,     0,     0,    77,    78,    79,     0,     0,     0,
     -85,     0,    80,     0,    81,     0,    75,    71,     0,     0,
       0,     0,     0,    72,    58,    59,    73,    61,     0,    62,
      63,    96,    74,     0,    76,    97,    24,    98,     0,    77,
      78,    79,     0,     0,     0,     0,    74,   128,     0,    81,
       0,    99,     0,    75,   100,   101,   102,     0,     0,     0,
       0,     0,     0,   103,   104,   105,   106,    34,     0,     0,
       0,    76,     0,     0,     0,     0,    77,    78,    79,     0,
       0,     0,     0,     0,   128,   -85,    81,     0,     0,     0,
       0,     0,    79,     0,     0,    71,   107,     0,     0,     0,
     295,    72,    58,    59,    73,    61,    71,    62,    63,     0,
      74,     0,    72,   140,    59,    73,    61,     0,    62,    63,
       0,    74,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    75,     0,     0,     0,     0,     0,     0,    76,
       0,     0,     0,     0,    77,    78,    79,     0,     0,     0,
      76,     0,   128,     0,    81,    77,    78,    79,     0,     0,
       0,     0,  -130,   128,     0,    81,  -130,  -130,  -130,     0,
       0,     0,     0,     0,     0,     0,     0,  -130,     0,     0,
       0,     0,  -130,     0,  -129,  -130,  -130,  -130,  -129,  -129,
    -129,     0,     0,     0,  -130,  -130,  -130,  -130,  -130,  -129,
       0,     0,     0,     0,  -129,     0,     0,  -129,  -129,  -129,
       0,     0,     0,     0,     0,     0,  -129,  -129,  -129,  -129,
    -129,     0,     0,     0,   156,   157,     0,  -130,   160,   161,
     162,  -130,     0,   152,     0,     0,     0,   167,   168,     0,
       0,   169,   170,   171,   172,   173,   174,     0,     0,  -129,
       0,     0,     0,  -129,   153,     0,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   152,     0,   163,   164,   165,
     166,   167,   168,     0,     0,   169,   170,   171,   172,   173,
     174,     0,     0,     0,   212,     0,   153,     0,   154,   155,
     156,   157,   158,   159,   160,   161,   162,     0,     0,   163,
     164,   165,   166,   167,   168,     0,     0,   169,   170,   171,
     172,   173,   174,   153,     0,   154,   155,   156,   157,   158,
     159,   160,   161,   162,     0,     0,   163,   164,   165,   166,
     167,   168,     0,     0,   169,   170,   171,   172,   173,   174,
       0,     0,   153,   212,   154,   155,   156,   157,   158,   159,
     160,   161,   162,     0,     0,   163,   164,   165,   166,   167,
     168,     0,     0,   169,   170,   171,   172,   173,   174,     0,
       0,   153,   293,   154,   155,   156,   157,   158,   159,   160,
     161,   162,     0,     0,   163,   164,   165,   166,   167,   168,
       0,     0,   169,   170,   171,   172,   173,   174,   153,     0,
     154,   155,   156,   157,     0,     0,   160,   161,   162,     0,
       0,   163,   164,   165,   166,   167,   168,     0,     0,   169,
     170,   171,   172,   173,   174,   153,     0,  -198,  -198,   156,
     157,     0,     0,   160,   161,   162,     0,     0,     0,     0,
       0,   166,   167,   168,     0,     0,   169,   170,   171,   172,
     173,   174,   153,     0,   154,   155,   156,   157,     0,     0,
     160,   161,   162,     0,     0,     0,     0,     0,   166,   167,
     168,     0,     0,   169,   170,   171,   172,   173,   174,   156,
     157,     0,     0,   160,   161,   162,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   169,   170,   171,   172,
     173,   174
};

static const yytype_int16 yycheck[] =
{
      16,    50,    27,    22,    26,    53,    53,    26,   108,    54,
     150,    47,    76,    77,    78,     1,    80,   197,     8,   263,
     209,     6,    19,    20,    40,   289,     7,     8,    40,    10,
      68,    80,   103,     7,   298,     7,    74,     0,    54,    47,
      48,    55,   106,    40,    16,   181,     8,     4,    10,    11,
       3,    13,    14,     7,    66,    12,    37,    65,   194,    66,
      79,   250,   326,    37,   128,    50,    80,     4,    65,    47,
      48,    49,     8,   317,    10,    11,     6,    13,    14,     0,
      66,    71,   222,     4,    62,    63,    64,    65,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,    70,   183,   135,   128,   295,   135,   181,   254,    66,
      67,     0,     1,    65,    40,     4,     5,     6,     7,     8,
       9,    10,    11,   181,    13,    14,    67,    16,    69,    68,
      66,    68,    21,    71,    23,    24,    25,    26,    68,   197,
     197,    10,     7,    67,    33,    34,    35,    36,    37,    73,
       7,     8,     9,    10,    11,    68,    13,    14,    68,    68,
       7,    50,     8,   313,   210,    66,    55,    73,    66,    69,
      67,    60,    61,    62,    67,    38,    10,    66,    69,    68,
      37,    70,   206,    69,    73,   209,    27,    28,    29,    30,
      31,     1,    68,   303,    66,    70,   254,     7,     8,     9,
      10,    11,   260,    13,    14,    66,    16,    47,    48,    49,
      10,    68,    71,    70,   273,    69,    66,     7,    71,    18,
      60,    61,    62,    63,    64,    65,   250,    37,   252,    74,
      69,   289,    69,   307,    40,    73,    23,   295,   295,    74,
     298,     8,     9,    10,    11,    55,    13,    14,    69,    15,
      60,    61,    62,    11,    30,   206,   288,   312,    68,   288,
      70,    71,   273,   174,    93,   254,   325,   302,   326,   295,
     213,     0,     1,   283,   105,     4,     5,     6,     7,   317,
     207,    -1,    -1,    -1,    -1,    -1,   312,    16,    -1,    -1,
      -1,    -1,    21,    -1,    23,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,     8,
       9,    10,    11,    -1,    13,    14,    -1,    -1,    -1,    -1,
      -1,    50,     0,     1,    -1,    -1,     4,     5,     6,     7,
      -1,    -1,    -1,    62,    -1,    -1,    -1,    66,    16,    -1,
      -1,    70,    -1,    21,    73,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,     1,    33,    34,    35,    36,    37,
       7,     8,     9,    10,    11,    -1,    13,    14,    -1,    16,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    66,    -1,
      37,    -1,    70,     1,    -1,    73,    -1,     5,     6,     7,
       8,     9,    10,    11,    -1,    13,    14,    -1,    16,    -1,
      -1,    -1,    -1,    21,    -1,    62,    24,    25,    26,    -1,
      -1,    68,    -1,    70,    -1,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    60,    61,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    70,     1,    72,    -1,    -1,     5,     6,     7,
       8,     9,    10,    11,    -1,    13,    14,    -1,    16,    -1,
      -1,    -1,    -1,    21,    -1,    -1,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
       1,    -1,    -1,    -1,    -1,     6,     7,     8,     9,    10,
      11,    -1,    13,    14,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    70,    -1,    72,     1,    37,    -1,    -1,    -1,
       6,     7,     8,     9,    10,    11,    -1,    13,    14,    -1,
      16,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    60,
      61,    62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
       1,    37,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    -1,    13,    14,    -1,    16,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    68,    69,    70,     1,    37,    -1,    -1,    -1,
      -1,     7,     8,     9,    10,    11,    -1,    13,    14,    -1,
      16,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    60,
      61,    62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
       1,    37,    -1,    -1,    -1,     6,     7,     8,     9,    10,
      11,    -1,    13,    14,    -1,    16,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    60,    61,    62,    -1,    -1,    -1,
      66,    -1,    68,    -1,    70,    -1,    37,     1,    -1,    -1,
      -1,    -1,    -1,     7,     8,     9,    10,    11,    -1,    13,
      14,     1,    16,    -1,    55,     5,     6,     7,    -1,    60,
      61,    62,    -1,    -1,    -1,    -1,    16,    68,    -1,    70,
      -1,    21,    -1,    37,    24,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    36,    37,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    -1,    -1,    -1,
      -1,    -1,    62,    -1,    -1,     1,    66,    -1,    -1,    -1,
      70,     7,     8,     9,    10,    11,     1,    13,    14,    -1,
      16,    -1,     7,     8,     9,    10,    11,    -1,    13,    14,
      -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    60,    61,    62,    -1,    -1,    -1,
      55,    -1,    68,    -1,    70,    60,    61,    62,    -1,    -1,
      -1,    -1,     1,    68,    -1,    70,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,
      -1,    -1,    21,    -1,     1,    24,    25,    26,     5,     6,
       7,    -1,    -1,    -1,    33,    34,    35,    36,    37,    16,
      -1,    -1,    -1,    -1,    21,    -1,    -1,    24,    25,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    43,    44,    -1,    66,    47,    48,
      49,    70,    -1,    18,    -1,    -1,    -1,    56,    57,    -1,
      -1,    60,    61,    62,    63,    64,    65,    -1,    -1,    66,
      -1,    -1,    -1,    70,    39,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    18,    -1,    52,    53,    54,
      55,    56,    57,    -1,    -1,    60,    61,    62,    63,    64,
      65,    -1,    -1,    -1,    69,    -1,    39,    -1,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    52,
      53,    54,    55,    56,    57,    -1,    -1,    60,    61,    62,
      63,    64,    65,    39,    -1,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    52,    53,    54,    55,
      56,    57,    -1,    -1,    60,    61,    62,    63,    64,    65,
      -1,    -1,    39,    69,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    -1,    -1,    52,    53,    54,    55,    56,
      57,    -1,    -1,    60,    61,    62,    63,    64,    65,    -1,
      -1,    39,    69,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    -1,    -1,    52,    53,    54,    55,    56,    57,
      -1,    -1,    60,    61,    62,    63,    64,    65,    39,    -1,
      41,    42,    43,    44,    -1,    -1,    47,    48,    49,    -1,
      -1,    52,    53,    54,    55,    56,    57,    -1,    -1,    60,
      61,    62,    63,    64,    65,    39,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    55,    56,    57,    -1,    -1,    60,    61,    62,    63,
      64,    65,    39,    -1,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    -1,    -1,    -1,    -1,    -1,    55,    56,
      57,    -1,    -1,    60,    61,    62,    63,    64,    65,    43,
      44,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,
      64,    65
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    76,    77,     0,     3,    79,    80,     7,    78,    81,
      66,    85,     4,    12,    82,    83,    84,    86,    84,   102,
      87,    88,    89,    82,     6,   105,   169,    96,    97,    66,
      67,     7,     8,    10,    37,    92,    93,    65,   170,    92,
      70,   103,     6,    50,    94,    95,    88,    40,     8,    71,
      40,    66,   111,   104,   105,    68,     7,    16,     8,     9,
      10,    11,    13,    14,    91,   171,   172,   173,   174,   175,
      71,     1,     7,    10,    16,    37,    55,    60,    61,    62,
      68,    70,   112,   113,   115,   119,   120,   124,   125,   128,
     134,   162,   165,   168,   175,    66,     1,     5,     7,    21,
      24,    25,    26,    33,    34,    35,    36,    66,    93,   103,
     105,   106,   107,   108,   118,   135,   136,   137,   140,   142,
     143,   147,   150,   155,   160,   162,   104,     1,    68,    99,
     100,   101,   113,   166,   167,   169,    90,    68,   120,   120,
       8,   120,     7,    93,    10,   113,   120,   126,   127,   169,
     129,   130,    18,    39,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    52,    53,    54,    55,    56,    57,    60,
      61,    62,    63,    64,    65,    10,    68,    68,   119,   148,
      74,    68,   121,   151,     7,     7,   121,     8,   139,   139,
     120,   138,    19,    20,    65,   109,   111,   161,     1,    66,
      66,    73,   154,    66,   120,    69,    67,    67,    92,   163,
      38,    69,    69,    67,    69,    69,   154,     8,    10,    11,
      13,    14,   131,   132,   133,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   114,   115,   120,
     164,    10,    68,   145,    72,   120,   122,   123,   124,   162,
     156,   121,   144,    70,    66,    66,     7,   124,    27,    28,
      29,    30,    31,   110,   160,    10,    98,   101,   167,    99,
      10,   175,   127,    67,   154,   116,    71,    99,   169,   149,
     122,   124,   162,    69,    69,    70,   106,   153,   152,    66,
       7,   141,   142,    71,   112,    69,   132,    18,   117,    69,
      92,   153,   105,   160,   157,   153,    73,   103,   111,   120,
     146,   154,    23,   158,   141,    74,   159,   113,   153,    69
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
#line 170 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 176 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 187 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace((yyvsp[(2) - (3)]).ident.s);
				free((yyvsp[(2) - (3)]).ident.s);
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    { _macroScope = MS_GLOBAL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 202 "Parser.y"
    { _macroScope = MS_LOCAL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 215 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 215 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 216 "Parser.y"
    {
				BOOL bString = ((yyvsp[(4) - (5)]).v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					S_CONSTANT, (yyvsp[(4) - (5)]).v.type, (yyvsp[(4) - (5)]).v.data);
				if ((yyvsp[(2) - (5)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (5)]).ident.s);
				}
				if (bString) {
					free((yyvsp[(4) - (5)]).v.data.string);
				}
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 229 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 232 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = VT_STRING;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 239 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 241 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 248 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 255 "Parser.y"
    {   
				yyerror("Using undeclared variable %s", (yyvsp[(1) - (1)]).ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(1) - (1)]).ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free((yyvsp[(1) - (1)]).ident.s);
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 278 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (6)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    {
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 305 "Parser.y"
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

  case 49:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s, (yyvsp[(1) - (4)]).type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(2) - (4)]).ident.s);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, 0);
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 352 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 353 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 354 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    { (yyval).needsPop = 0; ;}
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
    { (yyval).needsPop = 0; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    {
				(yyval).needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 374 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)(yyvsp[(1) - (5)]).ident.s });
				vname_count = 0;
			;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
			;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 386 "Parser.y"
    {
				YY_EMIT(C_BINOP, (GENERIC_DATA){(yyvsp[(3) - (4)]).type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (4)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 402 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 412 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 418 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-(yyvsp[(2) - (2)]).num);
				} else {
					(yyval).type = VT_NUMBER;
					(yyval).v.type = C_PUSH_LONG_LITERAL; 
					(yyval).v.data.longValue  = -(yyvsp[(2) - (2)]).num;
				}
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 444 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 455 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 458 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 459 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 462 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 463 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 470 "Parser.y"
    {
				yyerror("Missing closing parenthesis )");
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 478 "Parser.y"
    { (yyval).type = VT_NUMBER; 	 	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = (yyvsp[(1) - (1)]).ident.type;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 485 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 490 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {
				(yyval).type = VT_MAP;
				yywarning("Map literals not yet supported");
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 495 "Parser.y"
    {	
				(yyval).type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 501 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 514 "Parser.y"
    { free((yyvsp[(1) - (3)]).ident.s); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 515 "Parser.y"
    { free((yyvsp[(1) - (3)]).ident.s); free((yyvsp[(3) - (3)]).ident.s); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 517 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(1);
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(3);
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 530 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, (yyvsp[(1) - (1)]).v.type, (yyvsp[(1) - (1)]).v.data.longValue));
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 534 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, (yyvsp[(1) - (1)]).ident.s));
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 539 "Parser.y"
    { (yyval).v.type = VT_NUMBER; (yyval).v.data.longValue = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 540 "Parser.y"
    { (yyval).v.type = VT_CHAR; (yyval).v.data.longValue = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 541 "Parser.y"
    { (yyval).v.type = VT_BOOLEAN; (yyval).v.data.longValue = 1; ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 542 "Parser.y"
    { (yyval).v.type = VT_BOOLEAN; (yyval).v.data.longValue = 0; ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 544 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 545 "Parser.y"
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

  case 156:

/* Line 1455 of yacc.c  */
#line 557 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 563 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 570 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 571 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 578 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 583 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 587 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 592 "Parser.y"
    {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(3) - (3)]).ident.s});
				makeInternalSymbol((yyvsp[(3) - (3)]).ident.s, (yyvsp[(2) - (3)]).type, (intptr_t)0);
				if ((yyvsp[(3) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(3) - (3)]).ident.s);
				}
			;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 599 "Parser.y"
    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 606 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 608 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 611 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 619 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 621 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 624 "Parser.y"
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
#line 635 "Parser.y"
    { yyerror("Missing closing brace '}'"); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 640 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 643 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 645 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 650 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 654 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 661 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 662 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 668 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (2)]).funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = (yyvsp[(1) - (2)]).funcp;
			;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 674 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 679 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = 0;
			;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 684 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = (yyvsp[(1) - (5)]).ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&(yyvsp[(1) - (5)]).ident.s);
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 691 "Parser.y"
    {
				_functionCallOp = C_MACRO_REF;
				(yyval).ident.s = _strdup((yyvsp[(2) - (2)]).ident.s);
				(yyval).ident.stringIsAlloced = 1;
			;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 697 "Parser.y"
    {
				_functionCallOp = C_MACRO;
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 701 "Parser.y"
    {
				yyerror("Illegal function call expression");
				yyerrok;
				(yyval).ident.s = 0;
			;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 711 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 715 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; (*_currentFunctionCallParamIndexP)++; ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 717 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 720 "Parser.y"
    {	(yyval).type = VT_OBJECT_ARRAY;	;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 723 "Parser.y"
    {
				_arraySize = (long)(yyvsp[(2) - (3)]).num;
			;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 727 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 730 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 735 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 740 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 745 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 750 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_NUMBER;
			;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 758 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_FLOAT;
			;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 766 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_CHAR;
			;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 774 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_BOOLEAN;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3409 "Parser.tab.c"
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
#line 782 "Parser.y"


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


