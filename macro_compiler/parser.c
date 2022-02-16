
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
#line 39 "Parser.y"


#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "edfuncs.h"
#include "scanner.h"
#include "test.h"
#include "pkscc.h"
#include "sym.h"

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
extern void yyerror(char* s, ...);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern TYPEDVAL		bytecode_emitBinaryOperation(int nOperationType, TYPEDVAL *v1, TYPEDVAL *v2);
extern void bytecode_emitIncrementExpression(TYPEDVAL *v1, int nIncrement);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern TYPEDVAL bytecode_emitMultiplyWithLiteralExpression(TYPEDVAL* v1, int nNumber);

extern int yylex(void );
void 		bytecode_emitAssignment(const char *name, int typ, GENERIC_DATA val);
void 		bytecode_defineVariable(const char *name, int nBytecode, int typ, intptr_t val);

char		 	*bytecode_emitGotoLabelInstruction(char *name, char *sp, char *spend, int typ);
int			bytecode_createBranchLabel(char *name, COM_GOTO *recp);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
 char			*bytecode_generateAutoLabelName(char *prefix, int num);
int 			function_returnsString(void *ep);

static void makeInternalSymbol(const char* pszName, int sType, intptr_t tVal) {
	SYMBOL_TYPE nSymbolType;
	switch(sType) {
	   case C_STRING_LITERAL: nSymbolType = S_STRING; break;
	   case C_FLOAT_LITERAL: nSymbolType = S_FLOAT; break;
	   default: nSymbolType = S_NUMBER; break;
	}
	bytecode_defineVariable(pszName, C_DEFINE_VARIABLE, nSymbolType, tVal);
	sym_makeInternalSymbol((char*) pszName, nSymbolType, (GENERIC_DATA) {.val = tVal});
}



/* Line 189 of yacc.c  */
#line 191 "Parser.tab.c"

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
     T_FUNC = 269,
     T_MACRO = 270,
     T_DOTDOT = 271,
     T_PLUSPLUS = 272,
     T_MINUSMINUS = 273,
     T_IF = 274,
     T_ELIF = 275,
     T_ELSE = 276,
     T_WHILE = 277,
     T_GOTO = 278,
     T_BRAEQ = 279,
     T_LOCAL = 280,
     T_SWITCH = 281,
     T_BREAK = 282,
     T_CONTINUE = 283,
     T_RETURN = 284,
     T_NUMVAR = 285,
     T_FLOATVAR = 286,
     T_STRINGVAR = 287,
     T_NMATCH = 288,
     T_ASSIGN = 289,
     T_NE = 290,
     T_EQ = 291,
     T_OR = 292,
     T_AND = 293,
     T_DEFMACRO = 294,
     T_CMDSEQPREFIX = 295
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
#line 273 "Parser.tab.c"

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
#define YYLAST   347

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  79
/* YYNRULES -- Number of rules.  */
#define YYNRULES  153
/* YYNRULES -- Number of states.  */
#define YYNSTATES  232

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,     2,     2,     2,    51,    44,     2,
      54,    55,    49,    46,    53,    47,     2,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    52,
      41,     2,    42,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    48,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,    45,    57,    43,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    17,    19,
      23,    24,    25,    31,    33,    35,    37,    39,    41,    42,
      49,    50,    52,    54,    58,    61,    65,    66,    68,    73,
      74,    77,    82,    86,    88,    91,    92,    96,    98,   100,
     102,   104,   106,   108,   110,   112,   114,   116,   118,   121,
     124,   126,   130,   132,   135,   138,   141,   143,   144,   147,
     149,   152,   155,   158,   162,   166,   170,   174,   178,   182,
     186,   190,   194,   195,   196,   202,   204,   206,   207,   211,
     212,   216,   217,   218,   222,   224,   226,   227,   228,   233,
     234,   237,   239,   241,   243,   245,   247,   249,   251,   253,
     255,   257,   259,   261,   263,   265,   267,   270,   274,   278,
     281,   282,   284,   290,   291,   295,   298,   301,   304,   305,
     306,   312,   314,   318,   319,   320,   327,   328,   331,   332,
     335,   336,   342,   343,   349,   350,   352,   354,   358,   360,
     363,   365,   369,   371,   373,   375,   377,   379,   381,   383,
     385,   387,   389,   391
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      61,     0,    -1,    -1,    -1,    62,    63,    61,    -1,    76,
      -1,    64,    -1,    13,    65,    52,    -1,    66,    -1,    65,
      53,    66,    -1,    -1,    -1,    67,    69,    68,    34,   137,
      -1,     8,    -1,    70,    -1,    32,    -1,    30,    -1,    31,
      -1,    -1,    72,    39,     8,    54,    73,    55,    -1,    -1,
      74,    -1,    75,    -1,    75,    53,    74,    -1,   134,    69,
      -1,    71,    77,    78,    -1,    -1,    11,    -1,    56,    79,
     126,    57,    -1,    -1,    80,    79,    -1,   134,    69,    84,
      52,    -1,   134,    69,    52,    -1,    52,    -1,   127,    52,
      -1,    -1,    83,    82,    52,    -1,   122,    -1,   118,    -1,
     114,    -1,   110,    -1,   111,    -1,   112,    -1,   117,    -1,
      78,    -1,    87,    -1,   116,    -1,     1,    -1,    70,    84,
      -1,    34,    85,    -1,    86,    -1,    54,    86,    55,    -1,
      90,    -1,    43,    85,    -1,   108,    18,    -1,   108,    17,
      -1,   108,    -1,    -1,    89,   127,    -1,    88,    -1,   133,
      88,    -1,    46,    90,    -1,    47,    90,    -1,    90,    44,
      90,    -1,    90,    45,    90,    -1,    90,    46,    90,    -1,
      90,    48,    90,    -1,    90,    47,    90,    -1,    90,    49,
      90,    -1,    90,    50,    90,    -1,    90,    51,    90,    -1,
      54,    90,    55,    -1,    -1,    -1,    54,    92,    94,    93,
      55,    -1,    96,    -1,    91,    -1,    -1,    58,    95,    94,
      -1,    -1,    97,   101,    98,    -1,    -1,    -1,   100,    99,
      94,    -1,    38,    -1,    37,    -1,    -1,    -1,   102,   107,
     103,   104,    -1,    -1,   105,   107,    -1,    41,    -1,    42,
      -1,    36,    -1,    35,    -1,   106,    -1,    43,    -1,    33,
      -1,   127,    -1,   108,    -1,    30,    -1,    31,    -1,    32,
      -1,   109,    -1,   138,    -1,    11,    -1,   109,    11,    -1,
      27,   113,    52,    -1,    28,   113,    52,    -1,    29,   113,
      -1,    -1,     9,    -1,    26,    91,    56,   115,    57,    -1,
      -1,   116,    78,   115,    -1,     8,    59,    -1,    23,     8,
      -1,    24,     8,    -1,    -1,    -1,    22,   119,    91,   120,
     121,    -1,    81,    -1,    56,   126,    57,    -1,    -1,    -1,
      19,    91,   123,    81,   124,   125,    -1,    -1,    21,    81,
      -1,    -1,    81,   126,    -1,    -1,    14,   128,    54,   130,
      55,    -1,    -1,     8,   129,    54,   130,    55,    -1,    -1,
     131,    -1,   132,    -1,   131,    53,   132,    -1,     1,    -1,
     133,   108,    -1,   108,    -1,    54,   134,    55,    -1,     5,
      -1,     4,    -1,     6,    -1,     7,    -1,     3,    -1,    10,
      -1,     9,    -1,   138,    -1,    11,    -1,   136,    -1,   135,
      -1,   138,    45,   136,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   139,   139,   140,   140,   146,   147,   152,   154,   155,
     157,   157,   157,   166,   167,   169,   170,   171,   175,   175,
     178,   179,   181,   182,   184,   193,   209,   210,   212,   214,
     215,   217,   227,   241,   242,   243,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   259,   263,
     265,   266,   268,   269,   272,   274,   276,   277,   277,   289,
     290,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   314,   314,   314,   316,   317,   318,   318,   320,
     320,   322,   323,   323,   325,   326,   328,   329,   328,   332,
     339,   347,   348,   349,   350,   351,   357,   358,   360,   361,
     363,   367,   371,   375,   379,   381,   382,   393,   396,   399,
     403,   404,   406,   408,   409,   411,   416,   420,   425,   427,
     425,   438,   438,   440,   443,   440,   453,   454,   456,   457,
     459,   459,   466,   466,   473,   474,   476,   479,   482,   486,
     503,   505,   507,   508,   509,   510,   511,   513,   518,   524,
     525,   530,   531,   532
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_TFLOAT", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_OCTNUM", "T_CONST", "T_FUNC", "T_MACRO", "T_DOTDOT",
  "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_BRAEQ", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_FLOATVAR", "T_STRINGVAR", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_OR", "T_AND", "T_DEFMACRO",
  "T_CMDSEQPREFIX", "'<'", "'>'", "'~'", "'&'", "'|'", "'+'", "'-'", "'^'",
  "'*'", "'/'", "'%'", "';'", "','", "'('", "')'", "'{'", "'}'", "'!'",
  "':'", "$accept", "resources", "$@1", "resource", "constants",
  "constdefs", "constdef", "$@2", "$@3", "var_ident", "var", "macrostart",
  "$@4", "par_decls", "non_empty_pardecl", "par_decl", "macros",
  "opt_string", "block", "var_decls", "var_decl", "stmnt", "$@5",
  "assignment", "assgn", "arithmetic_expression", "simple_expression",
  "increment_expression", "s_bterm", "$@6", "bterm", "cond", "$@7", "$@8",
  "expr", "$@9", "optandorexpr", "$@10", "optandor", "@11", "and_or",
  "op2", "$@12", "$@13", "optterm2", "relop", "string_match_operator",
  "term", "value", "string", "break", "continue", "return", "opt_num",
  "case", "caselabels", "label", "goto", "while", "$@14", "$@15",
  "local_block", "if", "$@16", "$@17", "elseclause", "stmntlist", "fcall",
  "@18", "@19", "parlist", "nonemptypars", "par", "cast", "type_ident",
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
     295,    60,    62,   126,    38,   124,    43,    45,    94,    42,
      47,    37,    59,    44,    40,    41,   123,   125,    33,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    60,    61,    62,    61,    63,    63,    64,    65,    65,
      67,    68,    66,    69,    69,    70,    70,    70,    72,    71,
      73,    73,    74,    74,    75,    76,    77,    77,    78,    79,
      79,    80,    80,    81,    81,    82,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    83,    84,
      85,    85,    86,    86,    87,    87,    88,    89,    88,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    92,    93,    91,    94,    94,    95,    94,    97,
      96,    98,    99,    98,   100,   100,   102,   103,   101,   104,
     104,   105,   105,   105,   105,   105,   106,   106,   107,   107,
     108,   108,   108,   108,   108,   109,   109,   110,   111,   112,
     113,   113,   114,   115,   115,   116,   117,   117,   119,   120,
     118,   121,   121,   123,   124,   122,   125,   125,   126,   126,
     128,   127,   129,   127,   130,   130,   131,   131,   131,   132,
     132,   133,   134,   134,   134,   134,   134,   135,   136,   137,
     137,   138,   138,   138
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     1,     0,     6,
       0,     1,     1,     3,     2,     3,     0,     1,     4,     0,
       2,     4,     3,     1,     2,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     3,     1,     2,     2,     2,     1,     0,     2,     1,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     0,     0,     5,     1,     1,     0,     3,     0,
       3,     0,     0,     3,     1,     1,     0,     0,     4,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     2,
       0,     1,     5,     0,     3,     2,     2,     2,     0,     0,
       5,     1,     3,     0,     0,     6,     0,     2,     0,     2,
       0,     5,     0,     5,     0,     1,     1,     3,     1,     2,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    18,     1,    10,     3,     6,    26,     0,     5,
       0,     8,     0,     4,    27,     0,     0,     7,    10,    13,
      16,    17,    15,    11,    14,    29,    25,     0,     9,     0,
     146,   143,   142,   144,   145,     0,    29,     0,    20,     0,
      47,   132,   148,   147,   105,   130,     0,   118,     0,     0,
       0,   110,   110,   110,   100,   101,   102,    33,     0,    44,
       0,    35,    45,     0,   103,    40,    41,    42,    39,    46,
      43,    38,    37,     0,     0,   152,   151,   104,    30,     0,
       0,    21,    22,     0,   150,    12,   149,   115,     0,     0,
      72,   123,     0,   116,   117,     0,   111,     0,     0,   109,
      57,    48,   129,     0,    55,    54,   106,    28,    34,     0,
      32,     0,    19,     0,    24,     0,     0,    79,     0,   119,
     113,   107,   108,   100,   101,   102,    57,    57,    57,    57,
      49,    50,    59,     0,    52,    56,    57,    36,   153,    31,
      23,   138,     0,   140,     0,   135,   136,     0,     0,    77,
      76,    73,    75,    86,   124,     0,     0,     0,     0,    53,
      57,    61,    62,     0,     0,     0,   132,    58,    57,    57,
      57,    57,    57,    57,    57,    57,    60,   133,     0,   139,
     131,    79,     0,    81,     0,   126,     0,   121,   120,   112,
     113,     0,    51,    71,   141,    63,    64,    65,    67,    66,
      68,    69,    70,   137,    78,    74,    85,    84,    80,    82,
      87,    99,    98,     0,   125,     0,   114,    79,    89,   127,
     122,    83,    97,    94,    93,    91,    92,    96,    88,     0,
      95,    90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,    29,    23,
      58,     7,     8,    80,    81,    82,     9,    15,    59,    35,
      36,    60,   103,    61,   101,   130,   131,    62,   132,   133,
     134,   150,   117,   182,   151,   181,   152,   153,   208,   217,
     209,   183,   184,   218,   228,   229,   230,   210,   135,    64,
      65,    66,    67,    97,    68,   157,    69,    70,    71,    92,
     155,   188,    72,   118,   185,   214,    73,    74,    89,    88,
     144,   145,   146,   136,    37,    75,    76,    85,    77
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -166
static const yytype_int16 yypact[] =
{
      24,    32,    23,  -166,  -166,    24,  -166,    71,    48,  -166,
      37,  -166,    43,  -166,  -166,   -13,    83,  -166,  -166,  -166,
    -166,  -166,  -166,  -166,  -166,   215,  -166,    40,  -166,    88,
    -166,  -166,  -166,  -166,  -166,    36,   215,    43,   215,    19,
    -166,    62,  -166,  -166,  -166,  -166,    73,  -166,    94,   123,
      73,   124,   124,   124,   112,   113,   114,  -166,   118,  -166,
      36,  -166,  -166,    31,   168,  -166,  -166,  -166,  -166,  -166,
    -166,  -166,  -166,   127,   128,  -166,  -166,   136,  -166,     4,
     130,  -166,   133,    43,  -166,  -166,   136,  -166,   139,   142,
    -166,  -166,    73,  -166,  -166,   141,  -166,   146,   147,  -166,
     264,  -166,  -166,   149,  -166,  -166,  -166,  -166,  -166,   193,
    -166,   154,  -166,   215,  -166,    75,    75,   -32,   181,  -166,
     206,  -166,  -166,  -166,  -166,  -166,   264,   273,   273,   246,
    -166,  -166,  -166,    13,   296,  -166,    87,  -166,  -166,  -166,
    -166,  -166,   215,  -166,   160,   163,  -166,    87,   173,  -166,
    -166,  -166,  -166,  -166,  -166,   216,    62,   166,   -13,  -166,
     255,    20,    20,   174,   284,   176,  -166,  -166,   273,   273,
     273,   273,   273,   273,   273,   273,  -166,  -166,     3,  -166,
    -166,   -32,   177,    76,     9,   213,   131,  -166,  -166,  -166,
     206,   284,  -166,  -166,  -166,   126,   126,    20,    20,    20,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,
    -166,  -166,  -166,   181,  -166,   179,  -166,   -32,    68,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,     9,
    -166,  -166
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -166,   236,  -166,  -166,  -166,  -166,   245,  -166,  -166,   -22,
      -6,  -166,  -166,  -166,   156,  -166,  -166,  -166,   -14,   218,
    -166,  -113,  -166,  -166,   188,   144,   150,  -166,   135,  -166,
      -4,   -39,  -166,  -166,  -165,  -166,  -166,  -166,  -166,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,    51,   -35,  -166,
    -166,  -166,  -166,    63,  -166,    91,  -112,  -166,  -166,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,   -58,  -130,  -166,  -166,
     172,  -166,   119,  -106,   -34,  -166,   182,  -166,   251
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -135
static const yytype_int16 yytable[] =
{
      63,    26,   102,   167,    83,   154,    24,    91,   158,   147,
     147,    95,    42,    43,    44,    79,   204,   166,    42,    43,
      44,   166,    90,    45,    -2,    63,   149,    45,    42,    43,
      84,    24,     3,   123,   124,   125,     4,    40,   100,   123,
     124,   125,   187,    25,    41,    42,    43,    44,   104,   105,
      45,    19,   221,   119,   212,    46,   110,   142,    47,    48,
      49,   114,    50,    51,    52,    53,    54,    55,    56,   173,
     174,   175,   147,    20,    21,    22,   141,    24,   158,    83,
     143,   143,    14,    63,    42,    43,    44,    16,    57,    17,
      18,    27,    25,  -128,    38,   165,    42,    43,    44,   212,
     219,   222,    93,   223,   224,   123,   124,   125,   165,   225,
     226,   227,   179,   206,   207,    98,    99,   123,   124,   125,
      63,    87,    39,   161,   162,   164,   165,    90,   215,   142,
    -134,    94,    40,    96,    30,    31,    32,    33,    34,    41,
      42,    43,    44,   143,   190,    45,   -16,   -17,   -15,   211,
      46,    63,   100,    47,    48,    49,   191,    50,    51,    52,
      53,    54,    55,    56,   195,   196,   197,   198,   199,   200,
     201,   202,   170,   171,   172,   173,   174,   175,    63,   106,
     108,   109,    40,    57,   107,   112,   113,    25,   -29,    41,
      42,    43,    44,   115,   211,    45,   116,   120,   121,   122,
      46,   137,    42,    47,    48,    49,   139,    50,    51,    52,
      53,    54,    55,    56,   156,   177,   178,    40,    30,    31,
      32,    33,    34,   189,    41,    42,    43,    44,   180,   192,
      45,   194,   205,    57,   213,    46,   220,    25,    47,    48,
      49,    13,    50,    51,    52,    53,    54,    55,    56,    30,
      31,    32,    33,    34,    78,    42,    43,    44,    30,    31,
      32,    33,    34,    28,    42,    43,    44,   111,    57,   140,
     159,   176,   186,    42,    43,    44,   123,   124,   125,   163,
     231,   216,    42,    43,    44,   123,   124,   125,   148,   126,
      86,   138,   127,   128,   123,   124,   125,   203,     0,     0,
     160,   127,   128,   123,   124,   125,     0,   126,     0,   160,
     127,   128,     0,     0,     0,     0,     0,     0,   129,   127,
     128,     0,     0,     0,     0,     0,     0,   160,   168,   169,
     170,   171,   172,   173,   174,   175,     0,     0,     0,   193,
     168,   169,   170,   171,   172,   173,   174,   175
};

static const yytype_int16 yycheck[] =
{
      35,    15,    60,   133,    38,   118,    12,    46,   120,   115,
     116,    50,     9,    10,    11,    37,   181,     8,     9,    10,
      11,     8,    54,    14,     0,    60,    58,    14,     9,    10,
      11,    37,     0,    30,    31,    32,    13,     1,    34,    30,
      31,    32,   155,    56,     8,     9,    10,    11,    17,    18,
      14,     8,   217,    92,   184,    19,    52,    54,    22,    23,
      24,    83,    26,    27,    28,    29,    30,    31,    32,    49,
      50,    51,   178,    30,    31,    32,     1,    83,   190,   113,
     115,   116,    11,   118,     9,    10,    11,    39,    52,    52,
      53,     8,    56,    57,    54,   129,     9,    10,    11,   229,
     213,    33,     8,    35,    36,    30,    31,    32,   142,    41,
      42,    43,   147,    37,    38,    52,    53,    30,    31,    32,
     155,    59,    34,   127,   128,   129,   160,    54,   186,    54,
      55,     8,     1,     9,     3,     4,     5,     6,     7,     8,
       9,    10,    11,   178,   158,    14,    34,    34,    34,   184,
      19,   186,    34,    22,    23,    24,   160,    26,    27,    28,
      29,    30,    31,    32,   168,   169,   170,   171,   172,   173,
     174,   175,    46,    47,    48,    49,    50,    51,   213,    11,
      52,    45,     1,    52,    57,    55,    53,    56,    57,     8,
       9,    10,    11,    54,   229,    14,    54,    56,    52,    52,
      19,    52,     9,    22,    23,    24,    52,    26,    27,    28,
      29,    30,    31,    32,     8,    55,    53,     1,     3,     4,
       5,     6,     7,    57,     8,     9,    10,    11,    55,    55,
      14,    55,    55,    52,    21,    19,    57,    56,    22,    23,
      24,     5,    26,    27,    28,    29,    30,    31,    32,     3,
       4,     5,     6,     7,    36,     9,    10,    11,     3,     4,
       5,     6,     7,    18,     9,    10,    11,    79,    52,   113,
     126,   136,    56,     9,    10,    11,    30,    31,    32,   129,
     229,   190,     9,    10,    11,    30,    31,    32,   116,    43,
      39,   109,    46,    47,    30,    31,    32,   178,    -1,    -1,
      54,    46,    47,    30,    31,    32,    -1,    43,    -1,    54,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    54,    46,
      47,    -1,    -1,    -1,    -1,    -1,    -1,    54,    44,    45,
      46,    47,    48,    49,    50,    51,    -1,    -1,    -1,    55,
      44,    45,    46,    47,    48,    49,    50,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    61,    62,     0,    13,    63,    64,    71,    72,    76,
      65,    66,    67,    61,    11,    77,    39,    52,    53,     8,
      30,    31,    32,    69,    70,    56,    78,     8,    66,    68,
       3,     4,     5,     6,     7,    79,    80,   134,    54,    34,
       1,     8,     9,    10,    11,    14,    19,    22,    23,    24,
      26,    27,    28,    29,    30,    31,    32,    52,    70,    78,
      81,    83,    87,   108,   109,   110,   111,   112,   114,   116,
     117,   118,   122,   126,   127,   135,   136,   138,    79,    69,
      73,    74,    75,   134,    11,   137,   138,    59,   129,   128,
      54,    91,   119,     8,     8,    91,     9,   113,   113,   113,
      34,    84,   126,    82,    17,    18,    11,    57,    52,    45,
      52,    84,    55,    53,    69,    54,    54,    92,   123,    91,
      56,    52,    52,    30,    31,    32,    43,    46,    47,    54,
      85,    86,    88,    89,    90,   108,   133,    52,   136,    52,
      74,     1,    54,   108,   130,   131,   132,   133,   130,    58,
      91,    94,    96,    97,    81,   120,     8,   115,   116,    85,
      54,    90,    90,    86,    90,   134,     8,   127,    44,    45,
      46,    47,    48,    49,    50,    51,    88,    55,    53,   108,
      55,    95,    93,   101,   102,   124,    56,    81,   121,    57,
      78,    90,    55,    55,    55,    90,    90,    90,    90,    90,
      90,    90,    90,   132,    94,    55,    37,    38,    98,   100,
     107,   108,   127,    21,   125,   126,   115,    99,   103,    81,
      57,    94,    33,    35,    36,    41,    42,    43,   104,   105,
     106,   107
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
#line 140 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 157 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 157 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 159 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).ident.s,
					((yyvsp[(5) - (5)]).v.type == C_STRING_LITERAL) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.data);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 166 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 167 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 169 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 170 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 171 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 175 "Parser.y"
    { startmacro(); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 176 "Parser.y"
    { (yyval).ident = (yyvsp[(3) - (6)]).ident; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).ident.s,typ,(GENERIC_DATA) {_nparam});
				bytecode_defineVariable((yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,typ,_nparam);
				_nparam++;
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 194 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).ident.s, -1, 1);
					_recp = bytecode_emitInstruction(_recp,_recpend,C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (3)]).ident.s);
					_compilerConfiguration->cb_insertNewMacro((yyvsp[(1) - (3)]).ident.s,(yyvsp[(2) - (3)]).ident.s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&(yyvsp[(2) - (3)]).ident.s);
				freeitem(&(yyvsp[(1) - (3)]).ident.s);
				bytecode_closeOpenLabels();
				endmacro();
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 209 "Parser.y"
    {	(yyval).ident.s = 0; (yyval).ident.stringIsAlloced = 0; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 210 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 217 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s,
					(yyvsp[(1) - (4)]).type,
					((yyvsp[(1) - (4)]).type == C_STRING_LITERAL) ? (intptr_t)"" : 0);
				bytecode_emitAssignment((yyvsp[(2) - (4)]).ident.s,(yyvsp[(3) - (4)]).v.type, (yyvsp[(3) - (4)]).v.data);
				freeval(&(yyvsp[(3) - (4)]).v);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 227 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).type == C_STRING_LITERAL) {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)"");
					bytecode_emitAssignment((yyvsp[(2) - (3)]).ident.s, C_STRING_LITERAL, (GENERIC_DATA){.string = ""});
				} else {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)0);
					bytecode_emitAssignment((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (GENERIC_DATA){ 0 });
				}
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 243 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 259 "Parser.y"
    {
				bytecode_emitAssignment((yyvsp[(1) - (2)]).ident.s,(yyvsp[(2) - (2)]).v.type, (yyvsp[(2) - (2)]).v.data);
			;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 266 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 269 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 272 "Parser.y"
    { bytecode_emitIncrementExpression(&(yyvsp[(1) - (2)]).v, -1); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 274 "Parser.y"
    { bytecode_emitIncrementExpression(&(yyvsp[(1) - (2)]).v, 1); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 276 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    {
				_recp = bytecode_emitInstruction(_recp,_recpend,C_FURET, (GENERIC_DATA){vname_count});
			;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 279 "Parser.y"
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
#line 289 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 290 "Parser.y"
    {
				if (macro_isParameterStringType((yyvsp[(1) - (2)]).type) ==
				    macro_isParameterStringType((yyvsp[(2) - (2)]).v.type)) {
					(yyval).v = (yyvsp[(2) - (2)]).v;
				} else {
					(yyval).v = bytecode_emitBinaryOperation(BIN_CONVERT,&(yyvsp[(2) - (2)]).v,0);
					(yyval).v.type = ((yyvsp[(1) - (2)]).type == C_STRING_LITERAL) ?
						C_STRINGVAR : (((yyvsp[(1) - (2)]).type == C_FLOAT_LITERAL) ? C_FLOATVAR : C_LONGVAR);
				}
			;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 300 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
    { (yyval).v = bytecode_emitMultiplyWithLiteralExpression(&(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 303 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 304 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 305 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 306 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 307 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 308 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 309 "Parser.y"
    { (yyval).v = bytecode_emitBinaryOperation(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 310 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { bytecode_endTestOperation(CT_BRACKETS); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 318 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 318 "Parser.y"
    { bytecode_endTestOperation(CT_NOT); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { bytecode_endTestOperation((yyvsp[(3) - (3)]).type); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 322 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; binaryOperation1Found((yyvsp[(1) - (1)]).type); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 325 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    { bytecode_beginTestOperation(); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    { binaryOperation1Found((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { bytecode_endTestOperation((yyvsp[(4) - (4)]).type); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { 	
				if (_stringflg)
					_recp = bytecode_emitInstruction(_recp,_recpend,C_STRING_LITERAL,(GENERIC_DATA){.string=""});
				else
					_recp = bytecode_emitInstruction(_recp,_recpend,C_LONG_LITERAL,(GENERIC_DATA){0});
				(yyval).type = CT_NE;
			;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
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
#line 347 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 348 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("String match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    {	(yyval).type = C_LONG_LITERAL; 	 	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    {	(yyval).type = bytecode_emitPushValueInstruction(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    {
				(yyval).v.type = C_FLOATVAR;  
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 375 "Parser.y"
    {
				(yyval).v.type = C_STRING_LITERAL;
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 382 "Parser.y"
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

  case 107:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    {
				_recp = bytecode_emitInstruction(_recp,_recpend,C_STOP,(GENERIC_DATA){.longValue=(yyvsp[(2) - (2)]).num});
			;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 411 "Parser.y"
    {
				bytecode_createBranchLabel((yyvsp[(1) - (2)]).ident.s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 416 "Parser.y"
    {
				_recp = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    {
				_recp = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    {
				_iflevel--;
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 459 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = bytecode_emitInstruction(_recp,_recpend,C_0FUNC,
							   (GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (1)]).funcp)});
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 466 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = bytecode_emitInstruction(_recp,_recpend,C_MACRO,(GENERIC_DATA){.string = (yyvsp[(1) - (1)]).ident.s});
				freeitem(&(yyvsp[(1) - (1)]).ident.s);
				(yyval).funcp = 0;
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    {
				bytecode_emitPushParameterInstruction(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {
				bytecode_emitPushParameterInstruction(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
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

  case 140:

/* Line 1455 of yacc.c  */
#line 503 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 505 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 507 "Parser.y"
    {	(yyval).type = C_CHARACTER_LITERAL;	;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    {	(yyval).type = C_INTEGER_LITERAL;	;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 509 "Parser.y"
    {	(yyval).type = C_LONG_LITERAL;	;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 510 "Parser.y"
    {	(yyval).type = C_FLOAT_LITERAL;	;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    {	(yyval).type = C_STRING_LITERAL;	;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 513 "Parser.y"
    {
				(yyval).v.type = C_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 518 "Parser.y"
    {
				(yyval).v.type = C_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 524 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {
				(yyval).v.type = C_STRING_LITERAL;
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 530 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 532 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.data.longValue = (yyvsp[(1) - (3)]).v.data.longValue | (yyvsp[(3) - (3)]).v.data.longValue;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2674 "Parser.tab.c"
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
#line 536 "Parser.y"


/*---------------------------------*/
/* binaryOperation1Found()					*/
/*---------------------------------*/
static void binaryOperation1Found(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (cp) {
		_stringflg = STRINGTYPE(type);
		cp->p2offset = (int)(_recp-(unsigned char *)cp);
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
		cp->size = (int)(_recp - (unsigned char *)cp);
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
		_exprstack[_exprlevel] = (COM_TEST*)_recp;
		_recp = bytecode_emitInstruction(_recp,_recpend,C_TEST,(GENERIC_DATA){CT_BRACKETS});
	}
}

/*---------------------------------*/
/* bytecode_emitPushValueInstruction()					*/
/*---------------------------------*/
static unsigned char bytecode_emitPushValueInstruction(TYPEDVAL *vp)
{
	_recp = bytecode_emitInstruction(_recp,_recpend,vp->type,vp->data);
	freeval(vp);
	return vp->type;
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
		_recp = bytecode_emitGotoLabelInstruction(name,_recp,_recpend,bratyp);
	return _recp;
}

/*---------------------------------*/
/* bytecode_generateAutoLabelNamePrefix()					*/
/*---------------------------------*/
static int bytecode_generateAutoLabelNamePrefix(char *prefix,int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	return bytecode_createBranchLabel(name,(COM_GOTO *)_recp);
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


