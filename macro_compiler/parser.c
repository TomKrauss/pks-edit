
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
int 			bind_key(KEYCODE keycode, MACROREFTYPE typ, MACROREFIDX idx, int augment);
int 			bind_mouse(MOUSECODE code, MACROREFTYPE typ, MACROREFIDX idx, int flags, int augment);
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


/* Line 189 of yacc.c  */
#line 187 "Parser.tab.c"

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
     T_DEFKEYS = 306,
     T_DEFMOUSE = 307,
     T_DEFTAGS = 308,
     T_DEFESCAPES = 309,
     T_CMDSEQPREFIX = 310
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
#line 284 "Parser.tab.c"

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
#define YYLAST   340

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  208
/* YYNRULES -- Number of states.  */
#define YYNSTATES  316

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,     2,     2,     2,    66,    59,     2,
      72,    73,    64,    61,    69,    62,     2,    65,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    68,
      56,     2,    57,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    60,    71,    58,     2,     2,     2,
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
      55
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    15,    17,
      19,    21,    22,    25,    26,    33,    34,    36,    40,    41,
      49,    50,    53,    54,    56,    60,    62,    64,    66,    71,
      73,    75,    76,    80,    81,    89,    90,    92,    96,   101,
     103,   104,   106,   108,   110,   112,   114,   118,   122,   123,
     131,   132,   134,   138,   142,   144,   146,   148,   149,   153,
     154,   158,   160,   162,   164,   166,   170,   171,   175,   181,
     183,   187,   189,   193,   194,   195,   201,   203,   205,   207,
     209,   210,   217,   218,   220,   222,   226,   229,   233,   234,
     236,   237,   239,   244,   245,   248,   253,   257,   259,   262,
     263,   267,   269,   271,   273,   275,   277,   279,   281,   283,
     285,   287,   290,   293,   295,   299,   301,   304,   306,   307,
     310,   312,   315,   319,   323,   327,   331,   335,   339,   343,
     347,   351,   352,   353,   359,   361,   363,   364,   368,   369,
     373,   374,   375,   379,   381,   383,   384,   385,   390,   391,
     394,   396,   398,   400,   402,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   422,   425,   429,   433,   436,   437,
     439,   445,   446,   450,   453,   456,   459,   460,   461,   467,
     468,   469,   476,   477,   480,   481,   484,   485,   491,   492,
     498,   499,   501,   503,   507,   509,   512,   514,   518,   520,
     522,   524,   526,   528,   531,   533,   535,   537,   539
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      76,     0,    -1,    -1,    -1,    77,    78,    76,    -1,   121,
      -1,   106,    -1,    80,    -1,    97,    -1,    89,    -1,   109,
      -1,    -1,    67,     7,    -1,    -1,    50,     7,    79,    81,
      82,    68,    -1,    -1,    83,    -1,    83,    69,    82,    -1,
      -1,    36,     9,    84,    85,    70,    86,    71,    -1,    -1,
      67,     9,    -1,    -1,    87,    -1,    86,    69,    87,    -1,
      37,    -1,    39,    -1,    38,    -1,     9,    88,    40,   179,
      -1,    83,    -1,     1,    -1,    -1,    88,    69,     7,    -1,
      -1,   123,    52,     7,    90,    79,    91,    68,    -1,    -1,
      92,    -1,    92,    69,    91,    -1,    96,    40,    93,   179,
      -1,     1,    -1,    -1,    61,    -1,    32,    -1,    34,    -1,
      33,    -1,    94,    -1,    94,    61,    95,    -1,   104,    95,
       8,    -1,    -1,   123,    51,     7,    98,    79,    99,    68,
      -1,    -1,   100,    -1,    99,    69,   100,    -1,   102,    40,
     179,    -1,     1,    -1,     7,    -1,     8,    -1,    -1,   104,
     103,   101,    -1,    -1,   105,    61,   104,    -1,    29,    -1,
      30,    -1,    31,    -1,    35,    -1,    46,   107,    68,    -1,
      -1,   108,    69,   107,    -1,     7,     7,     7,    40,     7,
      -1,     1,    -1,    11,   110,    68,    -1,   111,    -1,   110,
      69,   111,    -1,    -1,    -1,   112,   114,   113,    40,   181,
      -1,     7,    -1,   115,    -1,    27,    -1,    26,    -1,    -1,
     117,    45,     7,    72,   118,    73,    -1,    -1,   119,    -1,
     120,    -1,   120,    69,   119,    -1,   178,   114,    -1,   116,
     122,   124,    -1,    -1,     9,    -1,    -1,     7,    -1,    70,
     125,   170,    71,    -1,    -1,   126,   125,    -1,   178,   114,
     130,    68,    -1,   178,   114,    68,    -1,    68,    -1,   171,
      68,    -1,    -1,   129,   128,    68,    -1,   166,    -1,   163,
      -1,   159,    -1,   155,    -1,   156,    -1,   157,    -1,   162,
      -1,   124,    -1,   161,    -1,     1,    -1,   115,   130,    -1,
      40,   131,    -1,   132,    -1,    72,   132,    73,    -1,   135,
      -1,    58,   131,    -1,   153,    -1,    -1,   134,   171,    -1,
     133,    -1,   177,   133,    -1,   135,    59,   135,    -1,   135,
      60,   135,    -1,   135,    61,   135,    -1,   135,    63,   135,
      -1,   135,    62,   135,    -1,   135,    64,   135,    -1,   135,
      65,   135,    -1,   135,    66,   135,    -1,    72,   135,    73,
      -1,    -1,    -1,    72,   137,   139,   138,    73,    -1,   141,
      -1,   136,    -1,    -1,    74,   140,   139,    -1,    -1,   142,
     146,   143,    -1,    -1,    -1,   145,   144,   139,    -1,    44,
      -1,    43,    -1,    -1,    -1,   147,   152,   148,   149,    -1,
      -1,   150,   152,    -1,    56,    -1,    57,    -1,    42,    -1,
      41,    -1,   151,    -1,    58,    -1,    28,    -1,   171,    -1,
     153,    -1,    26,    -1,    27,    -1,   154,    -1,   182,    -1,
       9,    -1,   154,     9,    -1,    23,   158,    68,    -1,    24,
     158,    68,    -1,    25,   158,    -1,    -1,     8,    -1,    22,
     136,    70,   160,    71,    -1,    -1,   161,   124,   160,    -1,
       7,    67,    -1,    19,     7,    -1,    20,     7,    -1,    -1,
      -1,    18,   164,   136,   165,   127,    -1,    -1,    -1,    15,
     136,   167,   127,   168,   169,    -1,    -1,    17,   127,    -1,
      -1,   127,   170,    -1,    -1,    12,   172,    72,   174,    73,
      -1,    -1,     7,   173,    72,   174,    73,    -1,    -1,   175,
      -1,   176,    -1,   175,    69,   176,    -1,     1,    -1,   177,
     153,    -1,   153,    -1,    72,   178,    73,    -1,     5,    -1,
       4,    -1,     6,    -1,     3,    -1,     7,    -1,    55,     7,
      -1,     8,    -1,   182,    -1,     9,    -1,   180,    -1,   182,
      60,   180,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   139,   139,   140,   140,   146,   147,   148,   149,   150,
     151,   153,   154,   209,   208,   215,   216,   217,   220,   219,
     230,   231,   233,   234,   235,   237,   241,   242,   243,   248,
     249,   251,   252,   257,   257,   267,   268,   269,   271,   279,
     281,   282,   284,   285,   286,   288,   289,   291,   300,   300,
     310,   311,   312,   314,   325,   327,   333,   335,   335,   340,
     341,   343,   344,   345,   346,   350,   352,   353,   355,   363,
     367,   369,   370,   372,   372,   372,   381,   382,   384,   385,
     389,   389,   392,   393,   395,   396,   398,   407,   423,   424,
     426,   427,   429,   431,   432,   434,   441,   452,   453,   454,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   469,   473,   475,   476,   478,   479,   481,   482,   482,
     494,   495,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   517,   517,   517,   519,   520,   521,   521,   523,   523,
     525,   526,   526,   528,   529,   531,   532,   531,   535,   542,
     549,   550,   551,   552,   553,   559,   560,   562,   563,   565,
     569,   573,   577,   579,   580,   591,   594,   597,   601,   602,
     604,   606,   607,   609,   614,   618,   623,   625,   623,   636,
     639,   636,   649,   650,   652,   653,   655,   655,   662,   662,
     669,   670,   672,   675,   678,   682,   699,   701,   703,   704,
     705,   706,   708,   716,   722,   728,   729,   734,   735
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
  "T_DEFKEYS", "T_DEFMOUSE", "T_DEFTAGS", "T_DEFESCAPES", "T_CMDSEQPREFIX",
  "'<'", "'>'", "'~'", "'&'", "'|'", "'+'", "'-'", "'^'", "'*'", "'/'",
  "'%'", "':'", "';'", "','", "'{'", "'}'", "'('", "')'", "'!'", "$accept",
  "resources", "$@1", "resource", "inherit", "menus", "$@2", "menudefs",
  "submenu", "$@3", "menuconnect", "menuitems", "menuitem", "menuoptions",
  "mouses", "$@4", "mousedefs", "mouse", "opt_plus", "m_button", "buttons",
  "mouse_button", "keys", "$@5", "keydefs", "key", "ident_or_num",
  "key_ident", "$@6", "kmod_list", "k_modifier", "icons", "icondefs",
  "icon", "constants", "constdefs", "constdef", "$@7", "$@8", "var_ident",
  "var", "macrostart", "$@9", "par_decls", "non_empty_pardecl", "par_decl",
  "macros", "opt_string", "opt_ident", "block", "var_decls", "var_decl",
  "stmnt", "$@10", "assignment", "assgn", "arith", "aexpr", "s_bterm",
  "$@11", "bterm", "cond", "$@12", "$@13", "expr", "$@14", "optandorexpr",
  "$@15", "optandor", "@16", "and_or", "op2", "$@17", "$@18", "optterm2",
  "relop", "strmatchop", "term", "value", "string", "break", "continue",
  "return", "opt_num", "case", "caselabels", "label", "gotolabel", "while",
  "$@19", "$@20", "if", "$@21", "$@22", "elseclause", "stmntlist", "fcall",
  "@23", "@24", "parlist", "nonemptypars", "par", "cast", "type_ident",
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
     305,   306,   307,   308,   309,   310,    60,    62,   126,    38,
     124,    43,    45,    94,    42,    47,    37,    58,    59,    44,
     123,   125,    40,    41,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    77,    76,    78,    78,    78,    78,    78,
      78,    79,    79,    81,    80,    82,    82,    82,    84,    83,
      85,    85,    86,    86,    86,    87,    87,    87,    87,    87,
      87,    88,    88,    90,    89,    91,    91,    91,    92,    92,
      93,    93,    94,    94,    94,    95,    95,    96,    98,    97,
      99,    99,    99,   100,   100,   101,   101,   103,   102,   104,
     104,   105,   105,   105,   105,   106,   107,   107,   108,   108,
     109,   110,   110,   112,   113,   111,   114,   114,   115,   115,
     117,   116,   118,   118,   119,   119,   120,   121,   122,   122,
     123,   123,   124,   125,   125,   126,   126,   127,   127,   128,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   129,   130,   131,   131,   132,   132,   133,   134,   133,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   137,   138,   136,   139,   139,   140,   139,   142,   141,
     143,   144,   143,   145,   145,   147,   148,   146,   149,   149,
     150,   150,   150,   150,   150,   151,   151,   152,   152,   153,
     153,   153,   153,   154,   154,   155,   156,   157,   158,   158,
     159,   160,   160,   161,   162,   162,   164,   165,   163,   167,
     168,   166,   169,   169,   170,   170,   172,   171,   173,   171,
     174,   174,   175,   175,   175,   176,   176,   177,   178,   178,
     178,   178,   179,   179,   180,   181,   181,   182,   182
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     1,     1,     1,
       1,     0,     2,     0,     6,     0,     1,     3,     0,     7,
       0,     2,     0,     1,     3,     1,     1,     1,     4,     1,
       1,     0,     3,     0,     7,     0,     1,     3,     4,     1,
       0,     1,     1,     1,     1,     1,     3,     3,     0,     7,
       0,     1,     3,     3,     1,     1,     1,     0,     3,     0,
       3,     1,     1,     1,     1,     3,     0,     3,     5,     1,
       3,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       0,     6,     0,     1,     1,     3,     2,     3,     0,     1,
       0,     1,     4,     0,     2,     4,     3,     1,     2,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     3,     1,     2,     1,     0,     2,
       1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     0,     0,     5,     1,     1,     0,     3,     0,     3,
       0,     0,     3,     1,     1,     0,     0,     4,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     3,     2,     0,     1,
       5,     0,     3,     2,     2,     2,     0,     0,     5,     0,
       0,     6,     0,     2,     0,     2,     0,     5,     0,     5,
       0,     1,     1,     3,     1,     2,     1,     3,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    90,     1,    91,    73,     0,     0,     3,     7,
       9,     8,     6,    10,    88,     0,     5,     0,     0,    71,
       0,    69,     0,     0,     0,    11,     4,    89,     0,     0,
       0,     0,    70,    73,    76,    79,    78,    74,    77,     0,
      65,     0,     0,    13,    93,    87,     0,    48,    33,    72,
       0,     0,    67,    12,    15,   201,   199,   198,   200,     0,
      93,     0,    82,    11,    11,     0,     0,     0,     0,    16,
     110,   188,   186,     0,   176,     0,     0,     0,   168,   168,
     168,    97,     0,   108,     0,    99,   104,   105,   106,   103,
     109,   107,   102,   101,     0,     0,    94,     0,     0,    83,
      84,     0,     0,     0,   204,   206,   207,    75,   205,    68,
      18,    14,    15,   173,     0,     0,   131,   179,     0,   174,
     175,     0,   169,     0,     0,   167,   118,   111,   185,     0,
      92,    98,    96,     0,    81,     0,    86,    54,    61,    62,
      63,    64,     0,    51,     0,    57,     0,    39,     0,    36,
       0,     0,     0,    20,    17,     0,     0,   138,     0,   177,
     171,   165,   166,   163,   159,   160,   118,   118,   112,   113,
     120,     0,   115,   117,   161,   118,   162,   100,    95,    85,
      49,     0,     0,     0,    59,    34,     0,    40,    42,    44,
      43,    45,     0,   208,     0,     0,   194,     0,   196,     0,
     191,   192,     0,     0,   136,   135,   132,   134,   145,   180,
       0,     0,     0,     0,   116,   118,     0,     0,     0,   188,
     119,   118,   118,   118,   118,   118,   118,   118,   118,   164,
     121,    52,   202,     0,    53,    55,    56,    58,    60,    37,
      41,     0,     0,    47,    21,     0,   189,     0,   195,   187,
     138,     0,   140,     0,   182,   178,   170,   171,     0,   114,
     130,   197,   122,   123,   124,   126,   125,   127,   128,   129,
     203,    38,    46,    30,    31,    25,    27,    26,    29,     0,
      23,   193,   137,   133,   144,   143,   139,   141,   146,   158,
     157,     0,   181,   172,     0,     0,    19,   138,   148,   183,
       0,     0,    24,   142,   156,   153,   152,   150,   151,   155,
     147,     0,   154,    28,    32,   149
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,    43,     9,    54,    68,    69,   153,
     195,   279,   280,   294,    10,    64,   148,   149,   241,   191,
     192,   150,    11,    63,   142,   143,   237,   144,   183,   145,
     146,    12,    23,    24,    13,    18,    19,    20,    50,    37,
      82,    14,    15,    98,    99,   100,    16,    28,    17,    83,
      59,    60,    84,   129,    85,   127,   168,   169,   170,   171,
     172,   205,   157,   251,   206,   250,   207,   208,   286,   297,
     287,   252,   253,   298,   310,   311,   312,   288,   173,   174,
      86,    87,    88,   123,    89,   212,    90,    91,    92,   118,
     210,    93,   158,   254,   292,    94,    95,   115,   114,   199,
     200,   201,   175,   218,   234,   106,   107,   176
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -230
static const yytype_int16 yypact[] =
{
      21,    24,   149,  -230,  -230,  -230,    12,    32,    21,  -230,
    -230,  -230,  -230,  -230,    37,   -14,  -230,    57,    48,  -230,
      76,  -230,    47,    -8,    15,    39,  -230,  -230,    18,   105,
     151,   157,  -230,  -230,  -230,  -230,  -230,  -230,  -230,   160,
    -230,    12,   163,  -230,    91,  -230,    52,  -230,  -230,  -230,
      95,   132,  -230,  -230,   167,  -230,  -230,  -230,  -230,    43,
      91,    76,    91,    39,    39,   111,   224,   201,   144,   170,
    -230,   174,  -230,   178,  -230,   272,   273,   178,   274,   274,
     274,  -230,   241,  -230,    43,  -230,  -230,  -230,  -230,  -230,
    -230,  -230,  -230,  -230,   213,   215,  -230,    19,   214,  -230,
     216,    76,   145,   158,  -230,  -230,  -230,  -230,   226,  -230,
    -230,  -230,   167,  -230,   218,   219,  -230,  -230,   178,  -230,
    -230,   223,  -230,   220,   221,  -230,     6,  -230,  -230,   227,
    -230,  -230,  -230,   229,  -230,    91,  -230,  -230,  -230,  -230,
    -230,  -230,   110,  -230,   252,  -230,   233,  -230,   230,   231,
     256,   117,   291,   234,  -230,     9,     9,   -44,   103,  -230,
     295,  -230,  -230,  -230,  -230,  -230,     6,   128,  -230,  -230,
    -230,    35,   199,  -230,   294,   175,   226,  -230,  -230,  -230,
    -230,   222,    22,   217,   203,  -230,   158,   243,  -230,  -230,
    -230,   244,   298,  -230,   299,   237,  -230,    91,  -230,   236,
     242,  -230,   175,   239,  -230,  -230,  -230,  -230,  -230,  -230,
     103,   174,   245,    18,  -230,   136,   240,   183,   246,  -230,
    -230,    14,    14,    14,    14,    14,    14,    14,    14,  -230,
    -230,  -230,  -230,   303,  -230,  -230,  -230,  -230,  -230,  -230,
    -230,    22,   117,  -230,  -230,    36,  -230,   139,  -230,  -230,
     -44,   247,   184,   228,   297,  -230,  -230,   295,   183,  -230,
    -230,  -230,   205,   205,   208,   208,   208,  -230,  -230,  -230,
    -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,   -18,
    -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,
    -230,   103,  -230,  -230,   -20,   168,  -230,   -44,   140,  -230,
      22,   308,  -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,
    -230,   228,  -230,  -230,  -230,  -230
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -230,   309,  -230,  -230,   212,  -230,  -230,   206,  -219,  -230,
    -230,  -230,    26,  -230,  -230,  -230,   137,  -230,  -230,  -230,
      80,  -230,  -230,  -230,  -230,   143,  -230,  -230,  -230,   -95,
    -230,  -230,   284,  -230,  -230,  -230,   293,  -230,  -230,   -45,
      -9,  -230,  -230,  -230,   192,  -230,  -230,  -230,  -230,   -28,
     268,  -230,  -153,  -230,  -230,   232,   164,   165,   156,  -230,
      -6,   -39,  -230,  -230,  -207,  -230,  -230,  -230,  -230,  -230,
    -230,  -230,  -230,  -230,  -230,  -230,  -230,    23,  -154,  -230,
    -230,  -230,  -230,   198,  -230,    78,  -156,  -230,  -230,  -230,
    -230,  -230,  -230,  -230,  -230,   249,  -168,  -230,  -230,   180,
    -230,    90,  -149,   -35,  -229,   186,  -230,   275
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -191
static const yytype_int16 yytable[] =
{
      45,   198,   198,   220,   213,   209,   202,   202,   151,    61,
     196,    38,   271,    21,   104,   163,    97,   104,   163,    22,
     300,    -2,   104,   163,     3,    61,   278,   101,   116,   232,
     204,    29,   164,   165,   117,   164,   165,   273,   121,    25,
     164,   165,   219,   282,    70,   274,    27,    72,   248,   301,
      71,   295,    38,   296,    39,    72,   136,   255,    73,   126,
      40,    74,    75,    76,   166,    77,    78,    79,    80,    35,
      36,   313,    67,   275,   276,   277,   278,   233,   167,   159,
     -66,   197,  -190,    34,    41,   290,   215,   132,    44,   238,
     303,   151,    38,   198,    55,    56,    57,    58,   202,   289,
     101,   213,    35,    36,    70,   -22,    42,   -22,    30,    31,
      71,    81,    46,    44,  -184,    72,    32,    33,    73,   104,
     105,    74,    75,    76,    62,    77,    78,    79,    80,    35,
      36,    55,    56,    57,    58,    65,   104,   163,   299,    55,
      56,    57,    58,   290,   104,   163,   137,   104,   163,   188,
     189,   190,   -59,   -59,   164,   165,     4,   289,    47,   147,
       5,   217,   164,   165,    48,   164,   165,    51,   304,   273,
      53,    81,    66,    44,   138,   139,   140,   274,   180,   181,
     141,   305,   306,   104,   163,   257,   166,   138,   139,   140,
     -59,   -59,   -59,   141,   -80,     6,   307,   308,   309,     7,
     215,   164,   165,    67,    67,   275,   276,   277,   215,   258,
     110,   197,   111,   -50,   -50,   262,   263,   264,   265,   266,
     267,   268,   269,   137,   235,   236,   -35,   284,   285,   -59,
     -59,   109,   138,   139,   140,   219,   104,   163,   141,   112,
      72,   113,   221,   222,   223,   224,   225,   226,   227,   228,
     116,   138,   139,   140,   164,   165,   260,   141,   221,   222,
     223,   224,   225,   226,   227,   228,   223,   224,   225,   226,
     227,   228,   226,   227,   228,   102,   103,   124,   125,   119,
     120,   126,   122,   131,   130,   135,   152,   134,   161,   162,
     155,   156,   182,   160,   184,   177,   187,   178,   185,   104,
     186,   194,   211,   229,   240,   242,   243,   245,   244,   246,
     270,   247,   249,   259,   291,   314,   256,    26,   154,   261,
     283,   302,   272,   239,   231,    52,    49,   179,    96,   133,
     214,   230,   216,   128,   315,   293,   203,   281,   193,     0,
     108
};

static const yytype_int16 yycheck[] =
{
      28,   155,   156,   171,   160,   158,   155,   156,   103,    44,
       1,    20,   241,     1,     8,     9,    61,     8,     9,     7,
      40,     0,     8,     9,     0,    60,   245,    62,    72,     7,
      74,    45,    26,    27,    73,    26,    27,     1,    77,     7,
      26,    27,     7,   250,     1,     9,     9,    12,   202,    69,
       7,    69,    61,    71,     7,    12,   101,   210,    15,    40,
      68,    18,    19,    20,    58,    22,    23,    24,    25,    26,
      27,   300,    36,    37,    38,    39,   295,    55,    72,   118,
      68,    72,    73,     7,    69,   253,    72,    68,    70,   184,
     297,   186,   101,   247,     3,     4,     5,     6,   247,   253,
     135,   257,    26,    27,     1,    69,    67,    71,    51,    52,
       7,    68,     7,    70,    71,    12,    68,    69,    15,     8,
       9,    18,    19,    20,    72,    22,    23,    24,    25,    26,
      27,     3,     4,     5,     6,    40,     8,     9,   291,     3,
       4,     5,     6,   311,     8,     9,     1,     8,     9,    32,
      33,    34,     7,     8,    26,    27,     7,   311,     7,     1,
      11,   167,    26,    27,     7,    26,    27,     7,    28,     1,
       7,    68,    40,    70,    29,    30,    31,     9,    68,    69,
      35,    41,    42,     8,     9,   213,    58,    29,    30,    31,
      32,    33,    34,    35,    45,    46,    56,    57,    58,    50,
      72,    26,    27,    36,    36,    37,    38,    39,    72,   215,
       9,    72,    68,    68,    69,   221,   222,   223,   224,   225,
     226,   227,   228,     1,     7,     8,    68,    43,    44,     7,
       8,     7,    29,    30,    31,     7,     8,     9,    35,    69,
      12,    67,    59,    60,    61,    62,    63,    64,    65,    66,
      72,    29,    30,    31,    26,    27,    73,    35,    59,    60,
      61,    62,    63,    64,    65,    66,    61,    62,    63,    64,
      65,    66,    64,    65,    66,    63,    64,    79,    80,     7,
       7,    40,     8,    68,    71,    69,    60,    73,    68,    68,
      72,    72,    40,    70,    61,    68,    40,    68,    68,     8,
      69,    67,     7,     9,    61,    61,     8,    70,     9,    73,
       7,    69,    73,    73,    17,     7,    71,     8,   112,    73,
      73,   295,   242,   186,   181,    41,    33,   135,    60,    97,
     166,   175,   167,    84,   311,   257,   156,   247,   152,    -1,
      65
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    76,    77,     0,     7,    11,    46,    50,    78,    80,
      89,    97,   106,   109,   116,   117,   121,   123,   110,   111,
     112,     1,     7,   107,   108,     7,    76,     9,   122,    45,
      51,    52,    68,    69,     7,    26,    27,   114,   115,     7,
      68,    69,    67,    79,    70,   124,     7,     7,     7,   111,
     113,     7,   107,     7,    81,     3,     4,     5,     6,   125,
     126,   178,    72,    98,    90,    40,    40,    36,    82,    83,
       1,     7,    12,    15,    18,    19,    20,    22,    23,    24,
      25,    68,   115,   124,   127,   129,   155,   156,   157,   159,
     161,   162,   163,   166,   170,   171,   125,   114,   118,   119,
     120,   178,    79,    79,     8,     9,   180,   181,   182,     7,
       9,    68,    69,    67,   173,   172,    72,   136,   164,     7,
       7,   136,     8,   158,   158,   158,    40,   130,   170,   128,
      71,    68,    68,   130,    73,    69,   114,     1,    29,    30,
      31,    35,    99,   100,   102,   104,   105,     1,    91,    92,
      96,   104,    60,    84,    82,    72,    72,   137,   167,   136,
      70,    68,    68,     9,    26,    27,    58,    72,   131,   132,
     133,   134,   135,   153,   154,   177,   182,    68,    68,   119,
      68,    69,    40,   103,    61,    68,    69,    40,    32,    33,
      34,    94,    95,   180,    67,    85,     1,    72,   153,   174,
     175,   176,   177,   174,    74,   136,   139,   141,   142,   127,
     165,     7,   160,   161,   131,    72,   132,   135,   178,     7,
     171,    59,    60,    61,    62,    63,    64,    65,    66,     9,
     133,   100,     7,    55,   179,     7,     8,   101,   104,    91,
      61,    93,    61,     8,     9,    70,    73,    69,   153,    73,
     140,   138,   146,   147,   168,   127,    71,   124,   135,    73,
      73,    73,   135,   135,   135,   135,   135,   135,   135,   135,
       7,   179,    95,     1,     9,    37,    38,    39,    83,    86,
      87,   176,   139,    73,    43,    44,   143,   145,   152,   153,
     171,    17,   169,   160,    88,    69,    71,   144,   148,   127,
      40,    69,    87,   139,    28,    41,    42,    56,    57,    58,
     149,   150,   151,   179,     7,   152
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

  case 13:

/* Line 1455 of yacc.c  */
#line 209 "Parser.y"
    {
				menu_initializeDefinition((yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(2) - (3)]).s);
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 220 "Parser.y"
    {
				macro_showStatus("Defining sub-menu %s",(yyvsp[(2) - (2)]).s);
				menu_addMenuMacroItem((yyvsp[(2) - (2)]).s, UM_POPUP, 0, 0);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 226 "Parser.y"
    {
				menu_addMenuMacroItem((char*)0, UM_ENDPOPUP, 0, 0);
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {
				menu_addMenuMacroItem((char *)0, UM_SEPARATOR, 0, 0);
			;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 244 "Parser.y"
    {
				menu_addMenuMacroItem((yyvsp[(1) - (4)]).s, UM_ITEM, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index);
				freeitem(&(yyvsp[(1) - (4)]).s);
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 249 "Parser.y"
    { yyerror("invalid menu item"); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 257 "Parser.y"
    {
				macro_showStatus("Compile mouse table %s",(yyvsp[(3) - (3)]).s);
				mouse_switchtotable((yyvsp[(3) - (3)]).s);
				if (SetOverride((yyvsp[(1) - (3)]).s) == MODE_OVERRIDE) {
					mouse_destroyMouseBindings();
				}
				freeitem(&(yyvsp[(3) - (3)]).s);
			;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 272 "Parser.y"
    {
				if ((yyvsp[(1) - (4)]).mousecode.button && 
				    (yyvsp[(3) - (4)]).macref.index != (MACROREFIDX)-1) {
					bind_mouse((yyvsp[(1) - (4)]).mousecode, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index,
						(yyvsp[(3) - (4)]).num, _override == MODE_AUGMENT);
				}
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 279 "Parser.y"
    { yyerror("mouse binding"); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 281 "Parser.y"
    { (yyval).num = 0; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 282 "Parser.y"
    { (yyval).num = 0; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 284 "Parser.y"
    { (yyval).num = MBUT_L; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    { (yyval).num = MBUT_R; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { (yyval).num = MBUT_M; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 288 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (1)]).num; 		  ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 289 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    {
				(yyval).mousecode.shift = (yyvsp[(1) - (3)]).num >> 8;
				(yyval).mousecode.button = (yyvsp[(2) - (3)]).num;
				(yyval).mousecode.nclicks = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 300 "Parser.y"
    {
				macro_showStatus("Compile key table %s",(yyvsp[(3) - (3)]).s);
				key_switchtotable((yyvsp[(3) - (3)]).s);
				if (SetOverride((yyvsp[(1) - (3)]).s) == MODE_OVERRIDE) {
					key_destroytable((yyvsp[(3) - (3)]).s);
				}
				freeitem(&(yyvsp[(3) - (3)]).s);
			;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 315 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).num != K_INVALID && 
				    (yyvsp[(3) - (3)]).macref.index != (MACROREFIDX) -1) {
					if (!bind_key((KEYCODE)(yyvsp[(1) - (3)]).num, (yyvsp[(3) - (3)]).macref.typ, 
							   (yyvsp[(3) - (3)]).macref.index,
							   _override == MODE_AUGMENT)) {
						yyerror("cannot bind key 0x%lx",(yyvsp[(1) - (3)]).num);
					}
				}
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 325 "Parser.y"
    { yyerror("key binding"); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    {
				if (((yyval).num = key2code((yyvsp[(1) - (1)]).s,_kmod_control)) == K_INVALID)
					yyerror("bad key %s",(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num|_kmod_control; 	;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    {	_kmod_control = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { 
				(yyval).num = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    {	(yyval).num = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    { (yyval).num = /* K_SHIFT */ 0x300; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 344 "Parser.y"
    { (yyval).num = K_CONTROL; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 345 "Parser.y"
    { (yyval).num = K_ALTERNATE; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 346 "Parser.y"
    { (yyval).num = K_HAS_SELECTION; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    {	
				macro_showStatus("Defining icon for macro %s",(yyvsp[(5) - (5)]).s);
				freeitem(&(yyvsp[(1) - (5)]).s);
				freeitem(&(yyvsp[(2) - (5)]).s);
				freeitem(&(yyvsp[(3) - (5)]).s);
				freeitem(&(yyvsp[(5) - (5)]).s);
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    { yyerror("icon definition"); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 374 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 382 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { startmacro(); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 398 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).s,typ,_nparam);
				PushCreateVariable((yyvsp[(2) - (2)]).s,typ,_nparam);
				_nparam++;
			;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 408 "Parser.y"
    {
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).s, -1, 1);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					macro_showStatus("Defining macro %s",(yyvsp[(1) - (3)]).s);
					macro_insertNewMacro((yyvsp[(1) - (3)]).s,(yyvsp[(2) - (3)]).s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&(yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(1) - (3)]).s);
				CloseLabelList();
				endmacro();
			;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 423 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {	(yyval).s = ""; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 434 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (4)]).s,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign((yyvsp[(2) - (4)]).s,(yyvsp[(3) - (4)]).v.type,(intptr_t)(yyvsp[(3) - (4)]).v.val);
				freeval(&(yyvsp[(3) - (4)]).v);
				vname_count = 0;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
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

  case 99:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {
				yyerror("invalid statement"); 
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 475 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 478 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 484 "Parser.y"
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

  case 120:

/* Line 1455 of yacc.c  */
#line 494 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 495 "Parser.y"
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

  case 122:

/* Line 1455 of yacc.c  */
#line 505 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 506 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 507 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 509 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 510 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 512 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 513 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 517 "Parser.y"
    { TestOp(); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 517 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    { TestOp(); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 523 "Parser.y"
    { TestOp(); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 523 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 526 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 528 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 529 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    { TestOp(); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 532 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 533 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 535 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 542 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				(yyval).type = (yyvsp[(1) - (2)]).type | flg;
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 549 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 550 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 551 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 552 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 553 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 559 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 563 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 565 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 569 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 573 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 577 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 579 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 580 "Parser.y"
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

  case 165:

/* Line 1455 of yacc.c  */
#line 592 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 595 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 601 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 602 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 609 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 614 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 618 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 623 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 625 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 628 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 636 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 639 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 644 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 655 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 662 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 672 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 675 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 678 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 682 "Parser.y"
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

  case 196:

/* Line 1455 of yacc.c  */
#line 699 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 701 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 703 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 704 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 705 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 706 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 708 "Parser.y"
    {
				(yyval).macref.typ = CMD_MACRO; 
				if (((yyval).macref.index = macro_getInternalIndexByName((yyvsp[(1) - (1)]).s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",(yyvsp[(1) - (1)]).s);
				}
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 716 "Parser.y"
    {
				(yyval).macref.typ = CMD_CMDSEQ; 
				(yyval).macref.index = macro_getCmdIndexByName((yyvsp[(2) - (2)]).s); 
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 722 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 728 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 729 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 734 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 735 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3006 "Parser.tab.c"
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
#line 739 "Parser.y"


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


