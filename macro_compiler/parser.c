
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
 * (c) Pahlen & Krauž					Author : TOM
 * All Rights Reserved.					created: 18.05.1991
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
#include "binop.h"
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
void 		menu_startdefine(char *szMenu);
int 			menu_addentry(char *pszString, int menutype, 
				MACROREFTYPE mactype, MACROREFTYPE macidx);


/* Line 189 of yacc.c  */
#line 185 "Parser.tab.c"

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
     T_MENUPOPUP = 290,
     T_MENUSEPERATOR = 291,
     T_MENUBREAK = 292,
     T_MENUBARBREAK = 293,
     T_ASSIGN = 294,
     T_NE = 295,
     T_EQ = 296,
     T_OR = 297,
     T_AND = 298,
     T_DEFMACRO = 299,
     T_DEFICON = 300,
     T_DEFALTPATH = 301,
     T_DEFCOMPILER = 302,
     T_DEFDOCTYPES = 303,
     T_DEFMENUS = 304,
     T_DEFKEYS = 305,
     T_DEFMOUSE = 306,
     T_DEFTAGS = 307,
     T_DEFABBREVS = 308,
     T_DEFBRACKETS = 309,
     T_DEFESCAPES = 310,
     T_CMDSEQPREFIX = 311
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
#line 283 "Parser.tab.c"

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
#define YYLAST   339

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  207
/* YYNRULES -- Number of states.  */
#define YYNSTATES  315

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,     2,     2,     2,    67,    60,     2,
      73,    74,    65,    62,    70,    63,     2,    66,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    68,    69,
      57,     2,    58,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    64,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,    61,    72,    59,     2,     2,     2,
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
      55,    56
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
     154,   158,   160,   162,   164,   168,   169,   173,   179,   181,
     185,   187,   191,   192,   193,   199,   201,   203,   205,   207,
     208,   215,   216,   218,   220,   224,   227,   231,   232,   234,
     235,   237,   242,   243,   246,   251,   255,   257,   260,   261,
     265,   267,   269,   271,   273,   275,   277,   279,   281,   283,
     285,   288,   291,   293,   297,   299,   302,   304,   305,   308,
     310,   313,   317,   321,   325,   329,   333,   337,   341,   345,
     349,   350,   351,   357,   359,   361,   362,   366,   367,   371,
     372,   373,   377,   379,   381,   382,   383,   388,   389,   392,
     394,   396,   398,   400,   402,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   423,   427,   431,   434,   435,   437,
     443,   444,   448,   451,   454,   457,   458,   459,   465,   466,
     467,   474,   475,   478,   479,   482,   483,   489,   490,   496,
     497,   499,   501,   505,   507,   510,   512,   516,   518,   520,
     522,   524,   526,   529,   531,   533,   535,   537
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      77,     0,    -1,    -1,    -1,    78,    79,    77,    -1,   122,
      -1,   107,    -1,    81,    -1,    98,    -1,    90,    -1,   110,
      -1,    -1,    68,     7,    -1,    -1,    49,     7,    80,    82,
      83,    69,    -1,    -1,    84,    -1,    84,    70,    83,    -1,
      -1,    35,     9,    85,    86,    71,    87,    72,    -1,    -1,
      68,     9,    -1,    -1,    88,    -1,    87,    70,    88,    -1,
      36,    -1,    38,    -1,    37,    -1,     9,    89,    39,   180,
      -1,    84,    -1,     1,    -1,    -1,    89,    70,     7,    -1,
      -1,   124,    51,     7,    91,    80,    92,    69,    -1,    -1,
      93,    -1,    93,    70,    92,    -1,    97,    39,    94,   180,
      -1,     1,    -1,    -1,    62,    -1,    32,    -1,    34,    -1,
      33,    -1,    95,    -1,    95,    62,    96,    -1,   105,    96,
       8,    -1,    -1,   124,    50,     7,    99,    80,   100,    69,
      -1,    -1,   101,    -1,   100,    70,   101,    -1,   103,    39,
     180,    -1,     1,    -1,     7,    -1,     8,    -1,    -1,   105,
     104,   102,    -1,    -1,   106,    62,   105,    -1,    29,    -1,
      30,    -1,    31,    -1,    45,   108,    69,    -1,    -1,   109,
      70,   108,    -1,     7,     7,     7,    39,     7,    -1,     1,
      -1,    11,   111,    69,    -1,   112,    -1,   111,    70,   112,
      -1,    -1,    -1,   113,   115,   114,    39,   182,    -1,     7,
      -1,   116,    -1,    27,    -1,    26,    -1,    -1,   118,    44,
       7,    73,   119,    74,    -1,    -1,   120,    -1,   121,    -1,
     121,    70,   120,    -1,   179,   115,    -1,   117,   123,   125,
      -1,    -1,     9,    -1,    -1,     7,    -1,    71,   126,   171,
      72,    -1,    -1,   127,   126,    -1,   179,   115,   131,    69,
      -1,   179,   115,    69,    -1,    69,    -1,   172,    69,    -1,
      -1,   130,   129,    69,    -1,   167,    -1,   164,    -1,   160,
      -1,   156,    -1,   157,    -1,   158,    -1,   163,    -1,   125,
      -1,   162,    -1,     1,    -1,   116,   131,    -1,    39,   132,
      -1,   133,    -1,    73,   133,    74,    -1,   136,    -1,    59,
     132,    -1,   154,    -1,    -1,   135,   172,    -1,   134,    -1,
     178,   134,    -1,   136,    60,   136,    -1,   136,    61,   136,
      -1,   136,    62,   136,    -1,   136,    64,   136,    -1,   136,
      63,   136,    -1,   136,    65,   136,    -1,   136,    66,   136,
      -1,   136,    67,   136,    -1,    73,   136,    74,    -1,    -1,
      -1,    73,   138,   140,   139,    74,    -1,   142,    -1,   137,
      -1,    -1,    75,   141,   140,    -1,    -1,   143,   147,   144,
      -1,    -1,    -1,   146,   145,   140,    -1,    43,    -1,    42,
      -1,    -1,    -1,   148,   153,   149,   150,    -1,    -1,   151,
     153,    -1,    57,    -1,    58,    -1,    41,    -1,    40,    -1,
     152,    -1,    59,    -1,    28,    -1,   172,    -1,   154,    -1,
      26,    -1,    27,    -1,   155,    -1,   183,    -1,     9,    -1,
     155,     9,    -1,    23,   159,    69,    -1,    24,   159,    69,
      -1,    25,   159,    -1,    -1,     8,    -1,    22,   137,    71,
     161,    72,    -1,    -1,   162,   125,   161,    -1,     7,    68,
      -1,    19,     7,    -1,    20,     7,    -1,    -1,    -1,    18,
     165,   137,   166,   128,    -1,    -1,    -1,    15,   137,   168,
     128,   169,   170,    -1,    -1,    17,   128,    -1,    -1,   128,
     171,    -1,    -1,    12,   173,    73,   175,    74,    -1,    -1,
       7,   174,    73,   175,    74,    -1,    -1,   176,    -1,   177,
      -1,   176,    70,   177,    -1,     1,    -1,   178,   154,    -1,
     154,    -1,    73,   179,    74,    -1,     5,    -1,     4,    -1,
       6,    -1,     3,    -1,     7,    -1,    56,     7,    -1,     8,
      -1,   183,    -1,     9,    -1,   181,    -1,   183,    61,   181,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   137,   137,   138,   138,   144,   145,   146,   147,   148,
     149,   151,   152,   251,   250,   257,   258,   259,   262,   261,
     272,   273,   275,   276,   277,   279,   283,   284,   285,   290,
     291,   293,   294,   299,   299,   309,   310,   311,   313,   321,
     323,   324,   326,   327,   328,   330,   331,   333,   342,   342,
     352,   353,   354,   356,   367,   369,   375,   377,   377,   382,
     383,   385,   386,   387,   391,   393,   394,   396,   404,   408,
     410,   411,   413,   413,   413,   422,   423,   425,   426,   430,
     430,   433,   434,   436,   437,   439,   448,   464,   465,   467,
     468,   473,   478,   479,   481,   488,   499,   500,   501,   501,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     514,   518,   520,   521,   523,   524,   526,   527,   527,   539,
     540,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     562,   562,   562,   564,   565,   566,   566,   568,   568,   570,
     571,   571,   573,   574,   576,   577,   576,   580,   587,   594,
     595,   596,   597,   598,   604,   605,   607,   608,   610,   614,
     618,   622,   624,   625,   636,   639,   642,   646,   647,   649,
     651,   652,   654,   659,   663,   668,   670,   668,   681,   684,
     681,   694,   695,   697,   698,   700,   700,   707,   707,   714,
     715,   717,   720,   723,   727,   744,   746,   748,   749,   750,
     751,   753,   761,   767,   773,   774,   779,   780
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
  "T_ALT", "T_LEFT", "T_MIDDLE", "T_RIGHT", "T_MENUPOPUP",
  "T_MENUSEPERATOR", "T_MENUBREAK", "T_MENUBARBREAK", "T_ASSIGN", "T_NE",
  "T_EQ", "T_OR", "T_AND", "T_DEFMACRO", "T_DEFICON", "T_DEFALTPATH",
  "T_DEFCOMPILER", "T_DEFDOCTYPES", "T_DEFMENUS", "T_DEFKEYS",
  "T_DEFMOUSE", "T_DEFTAGS", "T_DEFABBREVS", "T_DEFBRACKETS",
  "T_DEFESCAPES", "T_CMDSEQPREFIX", "'<'", "'>'", "'~'", "'&'", "'|'",
  "'+'", "'-'", "'^'", "'*'", "'/'", "'%'", "':'", "';'", "','", "'{'",
  "'}'", "'('", "')'", "'!'", "$accept", "resources", "$@1", "resource",
  "inherit", "menus", "$@2", "menudefs", "submenu", "$@3", "menuconnect",
  "menuitems", "menuitem", "menuoptions", "mouses", "$@4", "mousedefs",
  "mouse", "opt_plus", "m_button", "buttons", "mouse_button", "keys",
  "$@5", "keydefs", "key", "ident_or_num", "key_ident", "$@6", "kmod_list",
  "k_modifier", "icons", "icondefs", "icon", "constants", "constdefs",
  "constdef", "$@7", "$@8", "var_ident", "var", "macrostart", "$@9",
  "par_decls", "non_empty_pardecl", "par_decl", "macros", "opt_string",
  "opt_ident", "block", "var_decls", "var_decl", "stmnt", "$@10",
  "assignment", "assgn", "arith", "aexpr", "s_bterm", "$@11", "bterm",
  "cond", "$@12", "$@13", "expr", "$@14", "optandorexpr", "$@15",
  "optandor", "@16", "and_or", "op2", "$@17", "$@18", "optterm2", "relop",
  "strmatchop", "term", "value", "string", "break", "continue", "return",
  "opt_num", "case", "caselabels", "label", "gotolabel", "while", "$@19",
  "$@20", "if", "$@21", "$@22", "elseclause", "stmntlist", "fcall", "@23",
  "@24", "parlist", "nonemptypars", "par", "cast", "type_ident",
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
     305,   306,   307,   308,   309,   310,   311,    60,    62,   126,
      38,   124,    43,    45,    94,    42,    47,    37,    58,    59,
      44,   123,   125,    40,    41,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    77,    79,    79,    79,    79,    79,
      79,    80,    80,    82,    81,    83,    83,    83,    85,    84,
      86,    86,    87,    87,    87,    88,    88,    88,    88,    88,
      88,    89,    89,    91,    90,    92,    92,    92,    93,    93,
      94,    94,    95,    95,    95,    96,    96,    97,    99,    98,
     100,   100,   100,   101,   101,   102,   102,   104,   103,   105,
     105,   106,   106,   106,   107,   108,   108,   109,   109,   110,
     111,   111,   113,   114,   112,   115,   115,   116,   116,   118,
     117,   119,   119,   120,   120,   121,   122,   123,   123,   124,
     124,   125,   126,   126,   127,   127,   128,   128,   129,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     130,   131,   132,   132,   133,   133,   134,   135,   134,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     138,   139,   137,   140,   140,   141,   140,   143,   142,   144,
     145,   144,   146,   146,   148,   149,   147,   150,   150,   151,
     151,   151,   151,   151,   152,   152,   153,   153,   154,   154,
     154,   154,   155,   155,   156,   157,   158,   159,   159,   160,
     161,   161,   162,   163,   163,   165,   166,   164,   168,   169,
     167,   170,   170,   171,   171,   173,   172,   174,   172,   175,
     175,   176,   176,   176,   177,   177,   178,   179,   179,   179,
     179,   180,   180,   181,   182,   182,   183,   183
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
       3,     1,     1,     1,     3,     0,     3,     5,     1,     3,
       1,     3,     0,     0,     5,     1,     1,     1,     1,     0,
       6,     0,     1,     1,     3,     2,     3,     0,     1,     0,
       1,     4,     0,     2,     4,     3,     1,     2,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     3,     1,     2,     1,     0,     2,     1,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       0,     0,     5,     1,     1,     0,     3,     0,     3,     0,
       0,     3,     1,     1,     0,     0,     4,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     2,     0,     1,     5,
       0,     3,     2,     2,     2,     0,     0,     5,     0,     0,
       6,     0,     2,     0,     2,     0,     5,     0,     5,     0,
       1,     1,     3,     1,     2,     1,     3,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    89,     1,    90,    72,     0,     0,     3,     7,
       9,     8,     6,    10,    87,     0,     5,     0,     0,    70,
       0,    68,     0,     0,     0,    11,     4,    88,     0,     0,
       0,     0,    69,    72,    75,    78,    77,    73,    76,     0,
      64,     0,     0,    13,    92,    86,     0,    48,    33,    71,
       0,     0,    66,    12,    15,   200,   198,   197,   199,     0,
      92,     0,    81,    11,    11,     0,     0,     0,     0,    16,
     109,   187,   185,     0,   175,     0,     0,     0,   167,   167,
     167,    96,     0,   107,     0,    98,   103,   104,   105,   102,
     108,   106,   101,   100,     0,     0,    93,     0,     0,    82,
      83,     0,     0,     0,   203,   205,   206,    74,   204,    67,
      18,    14,    15,   172,     0,     0,   130,   178,     0,   173,
     174,     0,   168,     0,     0,   166,   117,   110,   184,     0,
      91,    97,    95,     0,    80,     0,    85,    54,    61,    62,
      63,     0,    51,     0,    57,     0,    39,     0,    36,     0,
       0,     0,    20,    17,     0,     0,   137,     0,   176,   170,
     164,   165,   162,   158,   159,   117,   117,   111,   112,   119,
       0,   114,   116,   160,   117,   161,    99,    94,    84,    49,
       0,     0,     0,    59,    34,     0,    40,    42,    44,    43,
      45,     0,   207,     0,     0,   193,     0,   195,     0,   190,
     191,     0,     0,   135,   134,   131,   133,   144,   179,     0,
       0,     0,     0,   115,   117,     0,     0,     0,   187,   118,
     117,   117,   117,   117,   117,   117,   117,   117,   163,   120,
      52,   201,     0,    53,    55,    56,    58,    60,    37,    41,
       0,     0,    47,    21,     0,   188,     0,   194,   186,   137,
       0,   139,     0,   181,   177,   169,   170,     0,   113,   129,
     196,   121,   122,   123,   125,   124,   126,   127,   128,   202,
      38,    46,    30,    31,    25,    27,    26,    29,     0,    23,
     192,   136,   132,   143,   142,   138,   140,   145,   157,   156,
       0,   180,   171,     0,     0,    19,   137,   147,   182,     0,
       0,    24,   141,   155,   152,   151,   149,   150,   154,   146,
       0,   153,    28,    32,   148
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,    43,     9,    54,    68,    69,   152,
     194,   278,   279,   293,    10,    64,   147,   148,   240,   190,
     191,   149,    11,    63,   141,   142,   236,   143,   182,   144,
     145,    12,    23,    24,    13,    18,    19,    20,    50,    37,
      82,    14,    15,    98,    99,   100,    16,    28,    17,    83,
      59,    60,    84,   129,    85,   127,   167,   168,   169,   170,
     171,   204,   156,   250,   205,   249,   206,   207,   285,   296,
     286,   251,   252,   297,   309,   310,   311,   287,   172,   173,
      86,    87,    88,   123,    89,   211,    90,    91,    92,   118,
     209,    93,   157,   253,   291,    94,    95,   115,   114,   198,
     199,   200,   174,   217,   233,   106,   107,   175
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -228
static const yytype_int16 yypact[] =
{
      22,    28,   145,  -228,  -228,  -228,    13,    25,    22,  -228,
    -228,  -228,  -228,  -228,     8,    -9,  -228,    89,   116,  -228,
      23,  -228,    46,    39,    27,     7,  -228,  -228,    15,    85,
     109,   130,  -228,  -228,  -228,  -228,  -228,  -228,  -228,   137,
    -228,    13,   148,  -228,   155,  -228,    61,  -228,  -228,  -228,
     126,   156,  -228,  -228,   132,  -228,  -228,  -228,  -228,    40,
     155,    23,   155,     7,     7,   179,   194,   190,   143,   146,
    -228,   163,  -228,   161,  -228,   228,   253,   161,   206,   206,
     206,  -228,   222,  -228,    40,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,   193,   212,  -228,   -15,   188,  -228,
     214,    23,   141,   144,  -228,  -228,  -228,  -228,   221,  -228,
    -228,  -228,   132,  -228,   210,   215,  -228,  -228,   161,  -228,
    -228,   216,  -228,   217,   220,  -228,    68,  -228,  -228,   223,
    -228,  -228,  -228,   225,  -228,   155,  -228,  -228,  -228,  -228,
    -228,   128,  -228,   246,  -228,   229,  -228,   226,   227,   251,
       6,   285,   230,  -228,    10,    10,    31,    95,  -228,   289,
    -228,  -228,  -228,  -228,  -228,    68,   120,  -228,  -228,  -228,
      14,   207,  -228,   290,   224,   221,  -228,  -228,  -228,  -228,
     195,     5,   199,    50,  -228,   144,   238,  -228,  -228,  -228,
     239,   294,  -228,   295,   232,  -228,   155,  -228,   231,   236,
    -228,   224,   233,  -228,  -228,  -228,  -228,  -228,  -228,    95,
     163,   240,    15,  -228,   127,   234,   192,   235,  -228,  -228,
     142,   142,   142,   142,   142,   142,   142,   142,  -228,  -228,
    -228,  -228,   303,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
       5,     6,  -228,  -228,    33,  -228,   154,  -228,  -228,    31,
     241,   166,   237,   296,  -228,  -228,   289,   192,  -228,  -228,
    -228,   213,   213,   -22,   -22,   -22,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,    17,  -228,
    -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
      95,  -228,  -228,   -24,   182,  -228,    31,   164,  -228,     5,
     304,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
     237,  -228,  -228,  -228,  -228
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -228,   306,  -228,  -228,   165,  -228,  -228,   204,  -221,  -228,
    -228,  -228,    24,  -228,  -228,  -228,   134,  -228,  -228,  -228,
      76,  -228,  -228,  -228,  -228,   140,  -228,  -228,  -228,   -95,
    -228,  -228,   280,  -228,  -228,  -228,   291,  -228,  -228,   -45,
     -10,  -228,  -228,  -228,   187,  -228,  -228,  -228,  -228,   -28,
     263,  -228,  -152,  -228,  -228,   242,   160,   162,   152,  -228,
      16,   -44,  -228,  -228,  -218,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,  -228,  -228,  -228,    19,  -153,  -228,
    -228,  -228,  -228,   168,  -228,    71,  -155,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,  -228,   247,  -167,  -228,  -228,   175,
    -228,    86,  -148,   -35,  -227,   183,  -228,   268
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -190
static const yytype_int16 yytable[] =
{
      45,   197,   197,   219,   212,   208,   201,   201,   150,    61,
      38,   195,   231,   270,    21,   299,    97,    27,   104,   162,
      22,   218,    -2,   277,   126,    61,    72,   101,     3,   117,
      34,   281,    25,   121,   272,    29,   163,   164,   187,   188,
     189,    70,   273,   225,   226,   227,   300,    71,   247,    35,
      36,    38,    72,    39,   132,    73,   136,   254,    74,    75,
      76,   232,    77,    78,    79,    80,    35,    36,    67,   274,
     275,   276,   312,   277,   158,    42,   104,   162,   302,   138,
     139,   140,   -65,   196,  -189,   289,    44,   294,   237,   295,
     150,    38,    46,   197,   163,   164,    70,    41,   201,   288,
     101,   212,    71,   -22,   116,   -22,   203,    72,    40,    81,
      73,    44,  -183,    74,    75,    76,    47,    77,    78,    79,
      80,    35,    36,    55,    56,    57,    58,   165,   104,   162,
      55,    56,    57,    58,    62,   104,   162,    48,   298,    30,
      31,   166,   137,   289,    51,   146,   163,   164,   -59,   -59,
     104,   162,     4,   163,   164,    53,     5,   288,    55,    56,
      57,    58,   104,   162,    81,    65,    44,    67,   163,   164,
     138,   139,   140,   138,   139,   140,   -59,   -59,   -59,   165,
     163,   164,   216,   272,   256,    32,    33,   104,   105,   -79,
       6,   273,   303,   214,     7,    66,   137,   179,   180,   110,
     214,   109,   -59,   -59,   304,   305,   234,   235,   283,   284,
     -50,   -50,   111,   -35,   122,   214,   112,    67,   274,   275,
     276,   306,   307,   308,   138,   139,   140,   196,   102,   103,
     257,   113,   104,   162,   116,   119,   261,   262,   263,   264,
     265,   266,   267,   268,   218,   104,   162,   124,   125,    72,
     163,   164,   220,   221,   222,   223,   224,   225,   226,   227,
     120,   126,   134,   163,   164,   130,   259,   220,   221,   222,
     223,   224,   225,   226,   227,   222,   223,   224,   225,   226,
     227,   131,   151,   154,   135,   181,   160,   159,   155,   161,
     186,   183,   176,   104,   177,   184,   210,   185,   193,   228,
     239,   241,   242,   244,   243,   245,   246,   248,   258,   260,
     269,   313,   255,   290,    26,   282,   153,   271,   301,   238,
     230,    52,   178,    96,    49,   213,   229,   292,   215,   314,
     202,   128,   280,   108,   192,     0,     0,     0,     0,   133
};

static const yytype_int16 yycheck[] =
{
      28,   154,   155,   170,   159,   157,   154,   155,   103,    44,
      20,     1,     7,   240,     1,    39,    61,     9,     8,     9,
       7,     7,     0,   244,    39,    60,    12,    62,     0,    73,
       7,   249,     7,    77,     1,    44,    26,    27,    32,    33,
      34,     1,     9,    65,    66,    67,    70,     7,   201,    26,
      27,    61,    12,     7,    69,    15,   101,   209,    18,    19,
      20,    56,    22,    23,    24,    25,    26,    27,    35,    36,
      37,    38,   299,   294,   118,    68,     8,     9,   296,    29,
      30,    31,    69,    73,    74,   252,    71,    70,   183,    72,
     185,   101,     7,   246,    26,    27,     1,    70,   246,   252,
     135,   256,     7,    70,    73,    72,    75,    12,    69,    69,
      15,    71,    72,    18,    19,    20,     7,    22,    23,    24,
      25,    26,    27,     3,     4,     5,     6,    59,     8,     9,
       3,     4,     5,     6,    73,     8,     9,     7,   290,    50,
      51,    73,     1,   310,     7,     1,    26,    27,     7,     8,
       8,     9,     7,    26,    27,     7,    11,   310,     3,     4,
       5,     6,     8,     9,    69,    39,    71,    35,    26,    27,
      29,    30,    31,    29,    30,    31,    32,    33,    34,    59,
      26,    27,   166,     1,   212,    69,    70,     8,     9,    44,
      45,     9,    28,    73,    49,    39,     1,    69,    70,     9,
      73,     7,     7,     8,    40,    41,     7,     8,    42,    43,
      69,    70,    69,    69,     8,    73,    70,    35,    36,    37,
      38,    57,    58,    59,    29,    30,    31,    73,    63,    64,
     214,    68,     8,     9,    73,     7,   220,   221,   222,   223,
     224,   225,   226,   227,     7,     8,     9,    79,    80,    12,
      26,    27,    60,    61,    62,    63,    64,    65,    66,    67,
       7,    39,    74,    26,    27,    72,    74,    60,    61,    62,
      63,    64,    65,    66,    67,    62,    63,    64,    65,    66,
      67,    69,    61,    73,    70,    39,    69,    71,    73,    69,
      39,    62,    69,     8,    69,    69,     7,    70,    68,     9,
      62,    62,     8,    71,     9,    74,    70,    74,    74,    74,
       7,     7,    72,    17,     8,    74,   112,   241,   294,   185,
     180,    41,   135,    60,    33,   165,   174,   256,   166,   310,
     155,    84,   246,    65,   151,    -1,    -1,    -1,    -1,    97
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    77,    78,     0,     7,    11,    45,    49,    79,    81,
      90,    98,   107,   110,   117,   118,   122,   124,   111,   112,
     113,     1,     7,   108,   109,     7,    77,     9,   123,    44,
      50,    51,    69,    70,     7,    26,    27,   115,   116,     7,
      69,    70,    68,    80,    71,   125,     7,     7,     7,   112,
     114,     7,   108,     7,    82,     3,     4,     5,     6,   126,
     127,   179,    73,    99,    91,    39,    39,    35,    83,    84,
       1,     7,    12,    15,    18,    19,    20,    22,    23,    24,
      25,    69,   116,   125,   128,   130,   156,   157,   158,   160,
     162,   163,   164,   167,   171,   172,   126,   115,   119,   120,
     121,   179,    80,    80,     8,     9,   181,   182,   183,     7,
       9,    69,    70,    68,   174,   173,    73,   137,   165,     7,
       7,   137,     8,   159,   159,   159,    39,   131,   171,   129,
      72,    69,    69,   131,    74,    70,   115,     1,    29,    30,
      31,   100,   101,   103,   105,   106,     1,    92,    93,    97,
     105,    61,    85,    83,    73,    73,   138,   168,   137,    71,
      69,    69,     9,    26,    27,    59,    73,   132,   133,   134,
     135,   136,   154,   155,   178,   183,    69,    69,   120,    69,
      70,    39,   104,    62,    69,    70,    39,    32,    33,    34,
      95,    96,   181,    68,    86,     1,    73,   154,   175,   176,
     177,   178,   175,    75,   137,   140,   142,   143,   128,   166,
       7,   161,   162,   132,    73,   133,   136,   179,     7,   172,
      60,    61,    62,    63,    64,    65,    66,    67,     9,   134,
     101,     7,    56,   180,     7,     8,   102,   105,    92,    62,
      94,    62,     8,     9,    71,    74,    70,   154,    74,   141,
     139,   147,   148,   169,   128,    72,   125,   136,    74,    74,
      74,   136,   136,   136,   136,   136,   136,   136,   136,     7,
     180,    96,     1,     9,    36,    37,    38,    84,    87,    88,
     177,   140,    74,    42,    43,   144,   146,   153,   154,   172,
      17,   170,   161,    89,    70,    72,   145,   149,   128,    39,
      70,    88,   140,    28,    40,    41,    57,    58,    59,   150,
     151,   152,   180,     7,   153
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
#line 138 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 251 "Parser.y"
    {
				menu_startdefine((yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(2) - (3)]).s);
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 262 "Parser.y"
    {
				macro_showStatus("Defining sub-menu %s",(yyvsp[(2) - (2)]).s);
				menu_addentry((yyvsp[(2) - (2)]).s, UM_POPUP, 0, 0);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
    {
				menu_addentry((char*)0, UM_ENDPOPUP, 0, 0);
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    {
				menu_addentry((char *)0, UM_SEPARATOR, 0, 0);
			;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    {
				menu_addentry((yyvsp[(1) - (4)]).s, UM_ITEM, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index);
				freeitem(&(yyvsp[(1) - (4)]).s);
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 291 "Parser.y"
    { yyerror("invalid menu item"); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 299 "Parser.y"
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
#line 314 "Parser.y"
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
#line 321 "Parser.y"
    { yyerror("mouse binding"); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    { (yyval).num = 0; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 324 "Parser.y"
    { (yyval).num = MO_FINDCURS; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    { (yyval).num = MBUT_L; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 327 "Parser.y"
    { (yyval).num = MBUT_R; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    { (yyval).num = MBUT_M; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (1)]).num; 		  ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 331 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    {
				(yyval).mousecode.shift = (yyvsp[(1) - (3)]).num >> 8;
				(yyval).mousecode.button = (yyvsp[(2) - (3)]).num;
				(yyval).mousecode.nclicks = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 342 "Parser.y"
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
#line 357 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).num != K_INVALID && 
				    (yyvsp[(3) - (3)]).macref.index != (MACROREFIDX) -1) {
					if (!bind_key((yyvsp[(1) - (3)]).num, (yyvsp[(3) - (3)]).macref.typ, 
							   (yyvsp[(3) - (3)]).macref.index,
							   _override == MODE_AUGMENT)) {
						yyerror("cannot bind key 0x%lx",(yyvsp[(1) - (3)]).num);
					}
				}
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    { yyerror("key binding"); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 370 "Parser.y"
    {
				if (((yyval).num = key2code((yyvsp[(1) - (1)]).s,_kmod_control)) == K_INVALID)
					yyerror("bad key %s",(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 375 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num|_kmod_control; 	;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    {	_kmod_control = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 378 "Parser.y"
    { 
				(yyval).num = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 382 "Parser.y"
    {	(yyval).num = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 383 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    { (yyval).num = /* K_SHIFT */ 0x300; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 386 "Parser.y"
    { (yyval).num = K_CONTROL; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    { (yyval).num = K_ALTERNATE; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    {	
				macro_showStatus("Defining icon for macro %s",(yyvsp[(5) - (5)]).s);
				freeitem(&(yyvsp[(1) - (5)]).s);
				freeitem(&(yyvsp[(2) - (5)]).s);
				freeitem(&(yyvsp[(3) - (5)]).s);
				freeitem(&(yyvsp[(5) - (5)]).s);
			;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    { yyerror("icon definition"); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 423 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    { startmacro(); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 431 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).s,typ,_nparam);
				PushCreateVariable((yyvsp[(2) - (2)]).s,typ,_nparam);
				_nparam++;
			;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    {
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).s, -1);
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

  case 87:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 465 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 467 "Parser.y"
    {	(yyval).s = ""; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 468 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (4)]).s,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign((yyvsp[(2) - (4)]).s,(yyvsp[(3) - (4)]).v.type,(intptr_t)(yyvsp[(3) - (4)]).v.val);
				freeval(&(yyvsp[(3) - (4)]).v);
				vname_count = 0;
			;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 488 "Parser.y"
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

  case 98:

/* Line 1455 of yacc.c  */
#line 501 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    { yyerror("invalid statement"); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 514 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 518 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 520 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 523 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 524 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 526 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 527 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 529 "Parser.y"
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

  case 119:

/* Line 1455 of yacc.c  */
#line 539 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 540 "Parser.y"
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

  case 121:

/* Line 1455 of yacc.c  */
#line 550 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 551 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 552 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 553 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 556 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 557 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 558 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
    { TestOp(); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 566 "Parser.y"
    { TestOp(); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 566 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 568 "Parser.y"
    { TestOp(); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 568 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 570 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 571 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 573 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 574 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 576 "Parser.y"
    { TestOp(); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 577 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 578 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 580 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 587 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				(yyval).type = (yyvsp[(1) - (2)]).type | flg;
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 594 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 595 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 596 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 598 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 604 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 605 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 607 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 608 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 610 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 614 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 618 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 622 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 624 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 625 "Parser.y"
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

  case 164:

/* Line 1455 of yacc.c  */
#line 637 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 640 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 642 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 646 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 647 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 654 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 659 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 663 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 668 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 670 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 673 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 681 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 684 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 689 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 700 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 707 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 717 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 720 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 723 "Parser.y"
    {
				yyerror("illegal parameters");
			;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 727 "Parser.y"
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

  case 195:

/* Line 1455 of yacc.c  */
#line 744 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 746 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 748 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 749 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 750 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 751 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 753 "Parser.y"
    {
				(yyval).macref.typ = CMD_MACRO; 
				if (((yyval).macref.index = macro_getInternalIndexByName((yyvsp[(1) - (1)]).s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",(yyvsp[(1) - (1)]).s);
				}
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 761 "Parser.y"
    {
				(yyval).macref.typ = CMD_CMDSEQ; 
				(yyval).macref.index = macro_getCmdIndexByName((yyvsp[(2) - (2)]).s); 
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 767 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 773 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 774 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 779 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 780 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2995 "Parser.tab.c"
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
#line 784 "Parser.y"


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


