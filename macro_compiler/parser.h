/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_NAMESPACE = 258,             /* T_NAMESPACE  */
    T_REQUIRE = 259,               /* T_REQUIRE  */
    T_STATIC = 260,                /* T_STATIC  */
    T_FOR = 261,                   /* T_FOR  */
    T_TYPE_IDENTIFIER = 262,       /* T_TYPE_IDENTIFIER  */
    T_IDENT = 263,                 /* T_IDENT  */
    T_NUM = 264,                   /* T_NUM  */
    T_FLOATING_POINT_NUMBER = 265, /* T_FLOATING_POINT_NUMBER  */
    T_STRING = 266,                /* T_STRING  */
    T_CHARACTER = 267,             /* T_CHARACTER  */
    T_CONST = 268,                 /* T_CONST  */
    T_TRUE = 269,                  /* T_TRUE  */
    T_FALSE = 270,                 /* T_FALSE  */
    T_TBOOLEAN = 271,              /* T_TBOOLEAN  */
    T_FUNC = 272,                  /* T_FUNC  */
    T_MACRO = 273,                 /* T_MACRO  */
    T_DOTDOT = 274,                /* T_DOTDOT  */
    T_PLUSPLUS = 275,              /* T_PLUSPLUS  */
    T_MINUSMINUS = 276,            /* T_MINUSMINUS  */
    T_IF = 277,                    /* T_IF  */
    T_ELIF = 278,                  /* T_ELIF  */
    T_ELSE = 279,                  /* T_ELSE  */
    T_WHILE = 280,                 /* T_WHILE  */
    T_GOTO = 281,                  /* T_GOTO  */
    T_BRAEQ = 282,                 /* T_BRAEQ  */
    T_SH_ASSIGN_PLUS = 283,        /* T_SH_ASSIGN_PLUS  */
    T_SH_ASSIGN_MINUS = 284,       /* T_SH_ASSIGN_MINUS  */
    T_SH_ASSIGN_MULT = 285,        /* T_SH_ASSIGN_MULT  */
    T_SH_ASSIGN_DIV = 286,         /* T_SH_ASSIGN_DIV  */
    T_SH_ASSIGN_MOD = 287,         /* T_SH_ASSIGN_MOD  */
    T_LOCAL = 288,                 /* T_LOCAL  */
    T_SWITCH = 289,                /* T_SWITCH  */
    T_CASE = 290,                  /* T_CASE  */
    T_BREAK = 291,                 /* T_BREAK  */
    T_CONTINUE = 292,              /* T_CONTINUE  */
    T_RETURN = 293,                /* T_RETURN  */
    T_VARIABLE = 294,              /* T_VARIABLE  */
    T_ASSOC_ARROW = 295,           /* T_ASSOC_ARROW  */
    T_NMATCH = 296,                /* T_NMATCH  */
    T_ASSIGN = 297,                /* T_ASSIGN  */
    T_NE = 298,                    /* T_NE  */
    T_EQ = 299,                    /* T_EQ  */
    T_SHIFT_LEFT = 300,            /* T_SHIFT_LEFT  */
    T_SHIFT_RIGHT = 301,           /* T_SHIFT_RIGHT  */
    T_OR = 302,                    /* T_OR  */
    T_AND = 303,                   /* T_AND  */
    T_LE = 304,                    /* T_LE  */
    T_GE = 305,                    /* T_GE  */
    T_POWER_TO = 306,              /* T_POWER_TO  */
    T_VOID = 307,                  /* T_VOID  */
    T_CMDSEQPREFIX = 308,          /* T_CMDSEQPREFIX  */
    T_LT = 309,                    /* T_LT  */
    T_GT = 310                     /* T_GT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
