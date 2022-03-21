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
    T_FOR = 260,                   /* T_FOR  */
    T_NEW = 261,                   /* T_NEW  */
    T_STATIC = 262,                /* T_STATIC  */
    T_VOID = 263,                  /* T_VOID  */
    T_TYPE_IDENTIFIER = 264,       /* T_TYPE_IDENTIFIER  */
    T_IDENT = 265,                 /* T_IDENT  */
    T_NUM = 266,                   /* T_NUM  */
    T_FLOATING_POINT_NUMBER = 267, /* T_FLOATING_POINT_NUMBER  */
    T_STRING = 268,                /* T_STRING  */
    T_CHARACTER = 269,             /* T_CHARACTER  */
    T_CONST = 270,                 /* T_CONST  */
    T_TRUE = 271,                  /* T_TRUE  */
    T_FALSE = 272,                 /* T_FALSE  */
    T_FUNC = 273,                  /* T_FUNC  */
    T_DOTDOT = 274,                /* T_DOTDOT  */
    T_PLUSPLUS = 275,              /* T_PLUSPLUS  */
    T_MINUSMINUS = 276,            /* T_MINUSMINUS  */
    T_IF = 277,                    /* T_IF  */
    T_ELSE = 278,                  /* T_ELSE  */
    T_WHILE = 279,                 /* T_WHILE  */
    T_GOTO = 280,                  /* T_GOTO  */
    T_SH_ASSIGN_PLUS = 281,        /* T_SH_ASSIGN_PLUS  */
    T_SH_ASSIGN_MINUS = 282,       /* T_SH_ASSIGN_MINUS  */
    T_SH_ASSIGN_MULT = 283,        /* T_SH_ASSIGN_MULT  */
    T_SH_ASSIGN_DIV = 284,         /* T_SH_ASSIGN_DIV  */
    T_SH_ASSIGN_MOD = 285,         /* T_SH_ASSIGN_MOD  */
    T_SWITCH = 286,                /* T_SWITCH  */
    T_CASE = 287,                  /* T_CASE  */
    T_DEFAULT = 288,               /* T_DEFAULT  */
    T_BREAK = 289,                 /* T_BREAK  */
    T_CONTINUE = 290,              /* T_CONTINUE  */
    T_RETURN = 291,                /* T_RETURN  */
    T_VARIABLE = 292,              /* T_VARIABLE  */
    T_LOCAL_VARIABLE = 293,        /* T_LOCAL_VARIABLE  */
    T_ASSOC_ARROW = 294,           /* T_ASSOC_ARROW  */
    T_NMATCH = 295,                /* T_NMATCH  */
    T_ASSIGN = 296,                /* T_ASSIGN  */
    T_NE = 297,                    /* T_NE  */
    T_EQ = 298,                    /* T_EQ  */
    T_SHIFT_LEFT = 299,            /* T_SHIFT_LEFT  */
    T_SHIFT_RIGHT = 300,           /* T_SHIFT_RIGHT  */
    T_OR = 301,                    /* T_OR  */
    T_AND = 302,                   /* T_AND  */
    T_LE = 303,                    /* T_LE  */
    T_GE = 304,                    /* T_GE  */
    T_POWER_TO = 305               /* T_POWER_TO  */
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
