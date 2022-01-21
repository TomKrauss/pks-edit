
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
#line 43 "Parser.y"


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
int 			IsFormStart(void *ep,int parno);
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


/* Line 189 of yacc.c  */
#line 182 "Parser.tab.c"

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
     T_IDENT = 262,
     T_NUM = 263,
     T_STRING = 264,
     T_OCTNUM = 265,
     T_CONST = 266,
     T_FUNC = 267,
     T_MACRO = 268,
     T_DOTDOT = 269,
     T_IF = 270,
     T_ELIF = 271,
     T_ELSE = 272,
     T_WHILE = 273,
     T_GOTO = 274,
     T_BRAEQ = 275,
     T_LOCAL = 276,
     T_SWITCH = 277,
     T_BREAK = 278,
     T_CONTINUE = 279,
     T_RETURN = 280,
     T_NUMVAR = 281,
     T_STRINGVAR = 282,
     T_NMATCH = 283,
     T_SHIFT = 284,
     T_CTRL = 285,
     T_ALT = 286,
     T_LEFT = 287,
     T_MIDDLE = 288,
     T_RIGHT = 289,
     T_HAS_SELECTION = 290,
     T_MENUPOPUP = 291,
     T_MENUSEPERATOR = 292,
     T_MENUBREAK = 293,
     T_MENUBARBREAK = 294,
     T_ASSIGN = 295,
     T_NE = 296,
     T_EQ = 297,
     T_OR = 298,
     T_AND = 299,
     T_DEFMACRO = 300,
     T_DEFICON = 301,
     T_DEFALTPATH = 302,
     T_DEFCOMPILER = 303,
     T_DEFDOCTYPES = 304,
     T_DEFMENUS = 305,
     T_DEFTAGS = 306,
     T_DEFESCAPES = 307,
     T_CMDSEQPREFIX = 308
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
#line 277 "Parser.tab.c"

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
#define YYLAST   269

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  87
/* YYNRULES -- Number of rules.  */
#define YYNRULES  166
/* YYNRULES -- Number of states.  */
#define YYNSTATES  250

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
       2,     2,     2,    72,     2,     2,     2,    64,    57,     2,
      70,    71,    62,    59,    67,    60,     2,    63,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    65,    66,
      54,     2,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    61,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,    58,    69,    56,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    15,    16,
      19,    20,    27,    28,    30,    34,    35,    43,    44,    47,
      48,    50,    54,    56,    58,    60,    65,    67,    69,    70,
      74,    78,    80,    84,    85,    86,    92,    94,    96,    98,
     100,   101,   108,   109,   111,   113,   117,   120,   124,   125,
     127,   132,   133,   136,   141,   145,   147,   150,   151,   155,
     157,   159,   161,   163,   165,   167,   169,   171,   173,   175,
     178,   181,   183,   187,   189,   192,   194,   195,   198,   200,
     203,   207,   211,   215,   219,   223,   227,   231,   235,   239,
     240,   241,   247,   249,   251,   252,   256,   257,   261,   262,
     263,   267,   269,   271,   272,   273,   278,   279,   282,   284,
     286,   288,   290,   292,   294,   296,   298,   300,   302,   304,
     306,   308,   310,   313,   317,   321,   324,   325,   327,   333,
     334,   338,   341,   344,   347,   348,   349,   355,   356,   357,
     364,   365,   368,   369,   372,   373,   379,   380,   386,   387,
     389,   391,   395,   397,   400,   402,   406,   408,   410,   412,
     414,   416,   419,   421,   423,   425,   427
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      74,     0,    -1,    -1,    -1,    75,    76,    74,    -1,    99,
      -1,    78,    -1,    87,    -1,    -1,    65,     7,    -1,    -1,
      50,     7,    77,    79,    80,    66,    -1,    -1,    81,    -1,
      81,    67,    80,    -1,    -1,    36,     9,    82,    83,    68,
      84,    69,    -1,    -1,    65,     9,    -1,    -1,    85,    -1,
      84,    67,    85,    -1,    37,    -1,    39,    -1,    38,    -1,
       9,    86,    40,   156,    -1,    81,    -1,     1,    -1,    -1,
      86,    67,     7,    -1,    11,    88,    66,    -1,    89,    -1,
      88,    67,    89,    -1,    -1,    -1,    90,    92,    91,    40,
     158,    -1,     7,    -1,    93,    -1,    27,    -1,    26,    -1,
      -1,    95,    45,     7,    70,    96,    71,    -1,    -1,    97,
      -1,    98,    -1,    98,    67,    97,    -1,   155,    92,    -1,
      94,   100,   101,    -1,    -1,     9,    -1,    68,   102,   147,
      69,    -1,    -1,   103,   102,    -1,   155,    92,   107,    66,
      -1,   155,    92,    66,    -1,    66,    -1,   148,    66,    -1,
      -1,   106,   105,    66,    -1,   143,    -1,   140,    -1,   136,
      -1,   132,    -1,   133,    -1,   134,    -1,   139,    -1,   101,
      -1,   138,    -1,     1,    -1,    93,   107,    -1,    40,   108,
      -1,   109,    -1,    70,   109,    71,    -1,   112,    -1,    56,
     108,    -1,   130,    -1,    -1,   111,   148,    -1,   110,    -1,
     154,   110,    -1,   112,    57,   112,    -1,   112,    58,   112,
      -1,   112,    59,   112,    -1,   112,    61,   112,    -1,   112,
      60,   112,    -1,   112,    62,   112,    -1,   112,    63,   112,
      -1,   112,    64,   112,    -1,    70,   112,    71,    -1,    -1,
      -1,    70,   114,   116,   115,    71,    -1,   118,    -1,   113,
      -1,    -1,    72,   117,   116,    -1,    -1,   119,   123,   120,
      -1,    -1,    -1,   122,   121,   116,    -1,    44,    -1,    43,
      -1,    -1,    -1,   124,   129,   125,   126,    -1,    -1,   127,
     129,    -1,    54,    -1,    55,    -1,    42,    -1,    41,    -1,
     128,    -1,    56,    -1,    28,    -1,   148,    -1,   130,    -1,
      26,    -1,    27,    -1,   131,    -1,   159,    -1,     9,    -1,
     131,     9,    -1,    23,   135,    66,    -1,    24,   135,    66,
      -1,    25,   135,    -1,    -1,     8,    -1,    22,   113,    68,
     137,    69,    -1,    -1,   138,   101,   137,    -1,     7,    65,
      -1,    19,     7,    -1,    20,     7,    -1,    -1,    -1,    18,
     141,   113,   142,   104,    -1,    -1,    -1,    15,   113,   144,
     104,   145,   146,    -1,    -1,    17,   104,    -1,    -1,   104,
     147,    -1,    -1,    12,   149,    70,   151,    71,    -1,    -1,
       7,   150,    70,   151,    71,    -1,    -1,   152,    -1,   153,
      -1,   152,    67,   153,    -1,     1,    -1,   154,   130,    -1,
     130,    -1,    70,   155,    71,    -1,     5,    -1,     4,    -1,
       6,    -1,     3,    -1,     7,    -1,    53,     7,    -1,     8,
      -1,   159,    -1,     9,    -1,   157,    -1,   159,    58,   157,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   134,   134,   135,   135,   141,   142,   143,   145,   146,
     201,   200,   207,   208,   209,   212,   211,   222,   223,   225,
     226,   227,   229,   233,   234,   235,   240,   241,   243,   244,
     249,   251,   252,   254,   254,   254,   263,   264,   266,   267,
     271,   271,   274,   275,   277,   278,   280,   289,   305,   306,
     311,   313,   314,   316,   323,   334,   335,   336,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   351,
     355,   357,   358,   360,   361,   363,   364,   364,   376,   377,
     387,   388,   389,   390,   391,   392,   393,   394,   395,   399,
     399,   399,   401,   402,   403,   403,   405,   405,   407,   408,
     408,   410,   411,   413,   414,   413,   417,   424,   432,   433,
     434,   435,   436,   442,   443,   445,   446,   448,   452,   456,
     460,   462,   463,   474,   477,   480,   484,   485,   487,   489,
     490,   492,   497,   501,   506,   508,   506,   519,   522,   519,
     532,   533,   535,   536,   538,   538,   545,   545,   552,   553,
     555,   558,   561,   565,   582,   584,   586,   587,   588,   589,
     591,   599,   605,   611,   612,   617,   618
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_IDENT", "T_NUM", "T_STRING", "T_OCTNUM", "T_CONST",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_BRAEQ", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_STRINGVAR", "T_NMATCH", "T_SHIFT", "T_CTRL",
  "T_ALT", "T_LEFT", "T_MIDDLE", "T_RIGHT", "T_HAS_SELECTION",
  "T_MENUPOPUP", "T_MENUSEPERATOR", "T_MENUBREAK", "T_MENUBARBREAK",
  "T_ASSIGN", "T_NE", "T_EQ", "T_OR", "T_AND", "T_DEFMACRO", "T_DEFICON",
  "T_DEFALTPATH", "T_DEFCOMPILER", "T_DEFDOCTYPES", "T_DEFMENUS",
  "T_DEFTAGS", "T_DEFESCAPES", "T_CMDSEQPREFIX", "'<'", "'>'", "'~'",
  "'&'", "'|'", "'+'", "'-'", "'^'", "'*'", "'/'", "'%'", "':'", "';'",
  "','", "'{'", "'}'", "'('", "')'", "'!'", "$accept", "resources", "$@1",
  "resource", "inherit", "menus", "$@2", "menudefs", "submenu", "$@3",
  "menuconnect", "menuitems", "menuitem", "menuoptions", "constants",
  "constdefs", "constdef", "$@4", "$@5", "var_ident", "var", "macrostart",
  "$@6", "par_decls", "non_empty_pardecl", "par_decl", "macros",
  "opt_string", "block", "var_decls", "var_decl", "stmnt", "$@7",
  "assignment", "assgn", "arith", "aexpr", "s_bterm", "$@8", "bterm",
  "cond", "$@9", "$@10", "expr", "$@11", "optandorexpr", "$@12",
  "optandor", "@13", "and_or", "op2", "$@14", "$@15", "optterm2", "relop",
  "strmatchop", "term", "value", "string", "break", "continue", "return",
  "opt_num", "case", "caselabels", "label", "gotolabel", "while", "$@16",
  "$@17", "if", "$@18", "$@19", "elseclause", "stmntlist", "fcall", "@20",
  "@21", "parlist", "nonemptypars", "par", "cast", "type_ident",
  "cmd_or_macro", "constval", "constnum_or_string", "constexpr", 0
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
     305,   306,   307,   308,    60,    62,   126,    38,   124,    43,
      45,    94,    42,    47,    37,    58,    59,    44,   123,   125,
      40,    41,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    75,    74,    76,    76,    76,    77,    77,
      79,    78,    80,    80,    80,    82,    81,    83,    83,    84,
      84,    84,    85,    85,    85,    85,    85,    85,    86,    86,
      87,    88,    88,    90,    91,    89,    92,    92,    93,    93,
      95,    94,    96,    96,    97,    97,    98,    99,   100,   100,
     101,   102,   102,   103,   103,   104,   104,   105,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   106,
     107,   108,   108,   109,   109,   110,   111,   110,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   114,
     115,   113,   116,   116,   117,   116,   119,   118,   120,   121,
     120,   122,   122,   124,   125,   123,   126,   126,   127,   127,
     127,   127,   127,   128,   128,   129,   129,   130,   130,   130,
     130,   131,   131,   132,   133,   134,   135,   135,   136,   137,
     137,   138,   139,   139,   141,   142,   140,   144,   145,   143,
     146,   146,   147,   147,   149,   148,   150,   148,   151,   151,
     152,   152,   152,   153,   153,   154,   155,   155,   155,   155,
     156,   156,   157,   158,   158,   159,   159
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     1,     0,     2,
       0,     6,     0,     1,     3,     0,     7,     0,     2,     0,
       1,     3,     1,     1,     1,     4,     1,     1,     0,     3,
       3,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       0,     6,     0,     1,     1,     3,     2,     3,     0,     1,
       4,     0,     2,     4,     3,     1,     2,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     3,     1,     2,     1,     0,     2,     1,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     0,
       0,     5,     1,     1,     0,     3,     0,     3,     0,     0,
       3,     1,     1,     0,     0,     4,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     2,     0,     1,     5,     0,
       3,     2,     2,     2,     0,     0,     5,     0,     0,     6,
       0,     2,     0,     2,     0,     5,     0,     5,     0,     1,
       1,     3,     1,     2,     1,     3,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    40,     1,    33,     0,     3,     6,     7,    48,
       0,     5,     0,    31,     0,     8,     4,    49,     0,     0,
      30,    33,    36,    39,    38,    34,    37,     0,    10,    51,
      47,     0,    32,     0,     9,    12,   159,   157,   156,   158,
       0,    51,     0,    42,     0,     0,     0,    13,    68,   146,
     144,     0,   134,     0,     0,     0,   126,   126,   126,    55,
       0,    66,     0,    57,    62,    63,    64,    61,    67,    65,
      60,    59,     0,     0,    52,     0,     0,    43,    44,     0,
     162,   164,   165,    35,   163,    15,    11,    12,   131,     0,
       0,    89,   137,     0,   132,   133,     0,   127,     0,     0,
     125,    76,    69,   143,     0,    50,    56,    54,     0,    41,
       0,    46,     0,    17,    14,     0,     0,    96,     0,   135,
     129,   123,   124,   121,   117,   118,    76,    76,    70,    71,
      78,     0,    73,    75,   119,    76,   120,    58,    53,    45,
     166,     0,     0,   152,     0,   154,     0,   149,   150,     0,
       0,    94,    93,    90,    92,   103,   138,     0,     0,     0,
       0,    74,    76,     0,     0,     0,   146,    77,    76,    76,
      76,    76,    76,    76,    76,    76,   122,    79,    18,     0,
     147,     0,   153,   145,    96,     0,    98,     0,   140,   136,
     128,   129,     0,    72,    88,   155,    80,    81,    82,    84,
      83,    85,    86,    87,    27,    28,    22,    24,    23,    26,
       0,    20,   151,    95,    91,   102,   101,    97,    99,   104,
     116,   115,     0,   139,   130,     0,     0,    16,    96,   106,
     141,     0,     0,    21,   100,   114,   111,   110,   108,   109,
     113,   105,     0,   112,   160,     0,    25,    29,   107,   161
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,    28,     7,    35,    46,    47,   113,
     142,   210,   211,   225,     8,    12,    13,    14,    33,    25,
      60,     9,    10,    76,    77,    78,    11,    18,    61,    40,
      41,    62,   104,    63,   102,   128,   129,   130,   131,   132,
     152,   117,   185,   153,   184,   154,   155,   217,   228,   218,
     186,   187,   229,   241,   242,   243,   219,   133,   134,    64,
      65,    66,    98,    67,   159,    68,    69,    70,    93,   157,
      71,   118,   188,   223,    72,    73,    90,    89,   146,   147,
     148,   135,   165,   246,    82,    83,   136
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -170
static const yytype_int16 yypact[] =
{
      19,    22,     6,  -170,  -170,    36,    19,  -170,  -170,    31,
       3,  -170,    20,  -170,     0,    -7,  -170,  -170,    -2,    65,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,    77,  -170,   186,
    -170,    34,  -170,   103,  -170,   128,  -170,  -170,  -170,  -170,
      27,   186,     0,   186,    89,   145,   101,   111,  -170,   133,
    -170,   117,  -170,   187,   190,   117,   192,   192,   192,  -170,
     183,  -170,    27,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,   155,   159,  -170,   -11,   156,  -170,   161,     0,
    -170,  -170,  -170,  -170,   168,  -170,  -170,   128,  -170,   160,
     162,  -170,  -170,   117,  -170,  -170,   163,  -170,   167,   169,
    -170,    80,  -170,  -170,   170,  -170,  -170,  -170,   171,  -170,
     186,  -170,   221,   173,  -170,     5,     5,   -34,    93,  -170,
     227,  -170,  -170,  -170,  -170,  -170,    80,   118,  -170,  -170,
    -170,    18,   152,  -170,   230,   131,   168,  -170,  -170,  -170,
    -170,   231,   174,  -170,   186,  -170,   172,   177,  -170,   131,
     175,  -170,  -170,  -170,  -170,  -170,  -170,    93,   133,   176,
      -2,  -170,   126,   178,   122,   179,  -170,  -170,    55,    55,
      55,    55,    55,    55,    55,    55,  -170,  -170,  -170,    32,
    -170,   129,  -170,  -170,   -34,   180,    59,   139,   224,  -170,
    -170,   227,   122,  -170,  -170,  -170,   158,   158,    28,    28,
      28,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
      11,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,    93,  -170,  -170,    43,   132,  -170,   -34,   121,
    -170,     8,   240,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,   139,  -170,  -170,   241,  -170,  -170,  -170,  -170
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -170,   246,  -170,  -170,  -170,  -170,  -170,   166,  -169,  -170,
    -170,  -170,    29,  -170,  -170,  -170,   233,  -170,  -170,   -19,
      -5,  -170,  -170,  -170,   146,  -170,  -170,  -170,   -18,   216,
    -170,  -113,  -170,  -170,   184,   134,   135,   123,  -170,    33,
     -31,  -170,  -170,  -163,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,    21,  -114,  -170,  -170,
    -170,  -170,   115,  -170,    70,  -112,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,   202,  -128,  -170,  -170,   149,  -170,
      85,  -104,   -25,  -170,   157,  -170,   223
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -149
static const yytype_int16 yytable[] =
{
      30,   145,   145,   167,    42,   156,   143,    22,   160,    26,
     209,   149,   149,    80,   123,   244,    42,     4,    79,    -2,
      92,   213,     3,    75,    96,   166,    23,    24,    48,   101,
      50,   124,   125,   204,    49,   182,    91,    26,   151,    50,
      17,   205,    51,    15,   189,    52,    53,    54,    19,    55,
      56,    57,    58,    23,    24,   107,     5,   209,    27,   221,
     111,   245,   119,    80,   123,   234,    29,   145,    45,   206,
     207,   208,    31,   220,    26,   144,  -148,   149,   226,   160,
     227,   124,   125,   231,    34,    79,    20,    21,    80,   123,
     173,   174,   175,    59,    48,    29,  -142,    80,    81,   -19,
      49,   -19,   215,   216,    43,    50,   124,   125,    51,   230,
     232,    52,    53,    54,   221,    55,    56,    57,    58,    23,
      24,    36,    37,    38,    39,   162,    80,   123,   220,    36,
      37,    38,    39,   204,    80,   123,   126,    80,   123,    80,
     123,   205,   191,    44,   124,   125,   166,    80,   123,   235,
     127,    50,   124,   125,    85,   124,   125,   124,   125,    59,
     164,    29,   236,   237,    45,   124,   125,    86,    45,   206,
     207,   208,    99,   100,   126,   238,   239,   240,    87,   168,
     169,   170,   171,   172,   173,   174,   175,    91,   162,    36,
      37,    38,    39,   194,    94,   192,   162,    95,    88,   144,
      97,   196,   197,   198,   199,   200,   201,   202,   203,   168,
     169,   170,   171,   172,   173,   174,   175,   170,   171,   172,
     173,   174,   175,   101,   105,   106,   112,   109,   110,    80,
     115,   120,   116,   121,   158,   122,   137,   138,   141,   176,
     178,   222,   179,   180,   181,   190,   183,   247,   249,   193,
     195,   214,    16,   114,    32,   233,   139,    74,   177,   108,
     161,   224,   163,   248,   103,   150,   212,    84,     0,   140
};

static const yytype_int16 yycheck[] =
{
      18,   115,   116,   131,    29,   118,     1,     7,   120,    14,
     179,   115,   116,     8,     9,     7,    41,    11,    43,     0,
      51,   184,     0,    42,    55,     7,    26,    27,     1,    40,
      12,    26,    27,     1,     7,   149,    70,    42,    72,    12,
       9,     9,    15,     7,   157,    18,    19,    20,    45,    22,
      23,    24,    25,    26,    27,    66,    50,   226,    65,   187,
      79,    53,    93,     8,     9,   228,    68,   181,    36,    37,
      38,    39,     7,   187,    79,    70,    71,   181,    67,   191,
      69,    26,    27,    40,     7,   110,    66,    67,     8,     9,
      62,    63,    64,    66,     1,    68,    69,     8,     9,    67,
       7,    69,    43,    44,    70,    12,    26,    27,    15,   222,
      67,    18,    19,    20,   242,    22,    23,    24,    25,    26,
      27,     3,     4,     5,     6,    70,     8,     9,   242,     3,
       4,     5,     6,     1,     8,     9,    56,     8,     9,     8,
       9,     9,   160,    40,    26,    27,     7,     8,     9,    28,
      70,    12,    26,    27,     9,    26,    27,    26,    27,    66,
     127,    68,    41,    42,    36,    26,    27,    66,    36,    37,
      38,    39,    57,    58,    56,    54,    55,    56,    67,    57,
      58,    59,    60,    61,    62,    63,    64,    70,    70,     3,
       4,     5,     6,    71,     7,   162,    70,     7,    65,    70,
       8,   168,   169,   170,   171,   172,   173,   174,   175,    57,
      58,    59,    60,    61,    62,    63,    64,    59,    60,    61,
      62,    63,    64,    40,    69,    66,    58,    71,    67,     8,
      70,    68,    70,    66,     7,    66,    66,    66,    65,     9,
       9,    17,    68,    71,    67,    69,    71,     7,     7,    71,
      71,    71,     6,    87,    21,   226,   110,    41,   135,    75,
     126,   191,   127,   242,    62,   116,   181,    44,    -1,   112
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,    75,     0,    11,    50,    76,    78,    87,    94,
      95,    99,    88,    89,    90,     7,    74,     9,   100,    45,
      66,    67,     7,    26,    27,    92,    93,    65,    77,    68,
     101,     7,    89,    91,     7,    79,     3,     4,     5,     6,
     102,   103,   155,    70,    40,    36,    80,    81,     1,     7,
      12,    15,    18,    19,    20,    22,    23,    24,    25,    66,
      93,   101,   104,   106,   132,   133,   134,   136,   138,   139,
     140,   143,   147,   148,   102,    92,    96,    97,    98,   155,
       8,     9,   157,   158,   159,     9,    66,    67,    65,   150,
     149,    70,   113,   141,     7,     7,   113,     8,   135,   135,
     135,    40,   107,   147,   105,    69,    66,    66,   107,    71,
      67,    92,    58,    82,    80,    70,    70,   114,   144,   113,
      68,    66,    66,     9,    26,    27,    56,    70,   108,   109,
     110,   111,   112,   130,   131,   154,   159,    66,    66,    97,
     157,    65,    83,     1,    70,   130,   151,   152,   153,   154,
     151,    72,   113,   116,   118,   119,   104,   142,     7,   137,
     138,   108,    70,   109,   112,   155,     7,   148,    57,    58,
      59,    60,    61,    62,    63,    64,     9,   110,     9,    68,
      71,    67,   130,    71,   117,   115,   123,   124,   145,   104,
      69,   101,   112,    71,    71,    71,   112,   112,   112,   112,
     112,   112,   112,   112,     1,     9,    37,    38,    39,    81,
      84,    85,   153,   116,    71,    43,    44,   120,   122,   129,
     130,   148,    17,   146,   137,    86,    67,    69,   121,   125,
     104,    40,    67,    85,   116,    28,    41,    42,    54,    55,
      56,   126,   127,   128,     7,    53,   156,     7,   129,     7
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
#line 135 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    {
				menu_initializeDefinition((yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(2) - (3)]).s);
			;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 212 "Parser.y"
    {
				_compilerConfiguration->cb_showStatus("Defining sub-menu %s",(yyvsp[(2) - (2)]).s);
				menu_addMenuMacroItem((yyvsp[(2) - (2)]).s, UM_POPUP, 0, 0);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 218 "Parser.y"
    {
				menu_addMenuMacroItem((char*)0, UM_ENDPOPUP, 0, 0);
			;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 230 "Parser.y"
    {
				menu_addMenuMacroItem((char *)0, UM_SEPARATOR, 0, 0);
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 236 "Parser.y"
    {
				menu_addMenuMacroItem((yyvsp[(1) - (4)]).s, UM_ITEM, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index);
				freeitem(&(yyvsp[(1) - (4)]).s);
			;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 241 "Parser.y"
    { yyerror("invalid menu item"); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 256 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 264 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 266 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 267 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 271 "Parser.y"
    { startmacro(); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 272 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).s,typ,_nparam);
				PushCreateVariable((yyvsp[(2) - (2)]).s,typ,_nparam);
				_nparam++;
			;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 290 "Parser.y"
    {
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).s, -1, 1);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (3)]).s);
					_compilerConfiguration->cb_insertNewMacro((yyvsp[(1) - (3)]).s,(yyvsp[(2) - (3)]).s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&(yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(1) - (3)]).s);
				CloseLabelList();
				endmacro();
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 305 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 306 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 316 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (4)]).s,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign((yyvsp[(2) - (4)]).s,(yyvsp[(3) - (4)]).v.type,(intptr_t)(yyvsp[(3) - (4)]).v.val);
				freeval(&(yyvsp[(3) - (4)]).v);
				vname_count = 0;
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).type == C_STRING1PAR) {
					sym_makeInternalSymbol((yyvsp[(2) - (3)]).s, S_STRING, (intptr_t)"");
					PushAssign((yyvsp[(2) - (3)]).s, C_STRING1PAR, (intptr_t)"");
				} else {
					sym_makeInternalSymbol((yyvsp[(2) - (3)]).s, S_NUMBER, (intptr_t)0);
					PushAssign((yyvsp[(2) - (3)]).s,C_LONG1PAR,(intptr_t)0);
				}
				vname_count = 0;
			;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 346 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, block or label."); 
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 366 "Parser.y"
    {
				if ((yyvsp[(2) - (2)]).funcp && IsStringFunc((yyvsp[(2) - (2)]).funcp)) {
					(yyval).v.type = C_STRINGVAR;
				} else {
					(yyval).v.type = C_LONGVAR;
				}
				(yyval).v.val = (intptr_t)"__ret__";
				((char*)(yyval).v.val)[6] = '0' + vname_count++;
			;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 376 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    {
				if (macro_isParameterStringType((yyvsp[(1) - (2)]).type) ==
				    macro_isParameterStringType((yyvsp[(2) - (2)]).v.type)) {
					(yyval).v = (yyvsp[(2) - (2)]).v;
				} else {
					(yyval).v = PushBinop(BIN_CONVERT,&(yyvsp[(2) - (2)]).v,0);
					(yyval).v.type = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ?
						C_STRINGVAR : C_LONGVAR;
				}
			;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 388 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 392 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { TestOp(); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    { TestOp(); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 405 "Parser.y"
    { TestOp(); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 405 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 407 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 408 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 410 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 411 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    { TestOp(); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 414 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				int nType = (yyvsp[(1) - (2)]).type;
				(yyval).type = (nType | flg);
			;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 432 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 433 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 434 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 435 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 436 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 462 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 463 "Parser.y"
    { 
				if (((yyval).s = malloc(strlen((yyvsp[(1) - (2)]).s)+strlen((yyvsp[(2) - (2)]).s)+1)) != 0) {
					strcat(strcpy((yyval).s,(yyvsp[(1) - (2)]).s),(yyvsp[(2) - (2)]).s);
					free((yyvsp[(1) - (2)]).s);
					free((yyvsp[(2) - (2)]).s);
				} else {
					yyerror("buffer overlow");
					(yyval).s = (yyvsp[(2) - (2)]).s;
				}
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 475 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 478 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 480 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 484 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 485 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 492 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 497 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 501 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 506 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 519 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 522 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 527 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 538 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 545 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 558 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 561 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 565 "Parser.y"
    {
				if (ISCONSTINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_CHAR1PAR &&
					    ((yyvsp[(2) - (2)]).v.val < 0 || (yyvsp[(2) - (2)]).v.val > 255))
					    	yyerror("constant out of range");
					if ((yyvsp[(1) - (2)]).type == C_STRING1PAR && (yyvsp[(2) - (2)]).num) {
						yyerror("invalid string cast");
						(yyvsp[(2) - (2)]).v.val = 0;
					}
				} else if (ISVARINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_STRING1PAR) {
						yyerror("unsupported cast");
					}
				}
				(yyval).v.type = (yyvsp[(1) - (2)]).type;
				(yyval).v.val = (yyvsp[(2) - (2)]).num;
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 582 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 584 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 586 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 587 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 588 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 589 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 591 "Parser.y"
    {
				(yyval).macref.typ = CMD_MACRO; 
				if (((yyval).macref.index = macro_getInternalIndexByName((yyvsp[(1) - (1)]).s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",(yyvsp[(1) - (1)]).s);
				}
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 599 "Parser.y"
    {
				(yyval).macref.typ = CMD_CMDSEQ; 
				(yyval).macref.index = macro_getCmdIndexByName((yyvsp[(2) - (2)]).s); 
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 605 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 611 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 612 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 617 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 618 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2687 "Parser.tab.c"
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
#line 622 "Parser.y"


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


