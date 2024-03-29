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
    T_TYPEDEF = 259,               /* T_TYPEDEF  */
    T_ENUM = 260,                  /* T_ENUM  */
    T_STRUCT = 261,                /* T_STRUCT  */
    T_REQUIRE = 262,               /* T_REQUIRE  */
    T_FOR = 263,                   /* T_FOR  */
    T_ANNOTATION = 264,            /* T_ANNOTATION  */
    T_NATIVE = 265,                /* T_NATIVE  */
    T_NEW = 266,                   /* T_NEW  */
    T_STATIC = 267,                /* T_STATIC  */
    T_VOID = 268,                  /* T_VOID  */
    T_TYPE_IDENTIFIER = 269,       /* T_TYPE_IDENTIFIER  */
    T_ELLIPSIS = 270,              /* T_ELLIPSIS  */
    T_IDENT = 271,                 /* T_IDENT  */
    T_NUM = 272,                   /* T_NUM  */
    T_FLOATING_POINT_NUMBER = 273, /* T_FLOATING_POINT_NUMBER  */
    T_STRING = 274,                /* T_STRING  */
    T_CHARACTER = 275,             /* T_CHARACTER  */
    T_CONST = 276,                 /* T_CONST  */
    T_TRUE = 277,                  /* T_TRUE  */
    T_FALSE = 278,                 /* T_FALSE  */
    T_FUNC = 279,                  /* T_FUNC  */
    T_DOTDOT = 280,                /* T_DOTDOT  */
    T_PLUSPLUS = 281,              /* T_PLUSPLUS  */
    T_MINUSMINUS = 282,            /* T_MINUSMINUS  */
    T_IF = 283,                    /* T_IF  */
    T_ELSE = 284,                  /* T_ELSE  */
    T_WHILE = 285,                 /* T_WHILE  */
    T_GOTO = 286,                  /* T_GOTO  */
    T_SH_ASSIGN_PLUS = 287,        /* T_SH_ASSIGN_PLUS  */
    T_SH_ASSIGN_MINUS = 288,       /* T_SH_ASSIGN_MINUS  */
    T_SH_ASSIGN_MULT = 289,        /* T_SH_ASSIGN_MULT  */
    T_SH_ASSIGN_DIV = 290,         /* T_SH_ASSIGN_DIV  */
    T_SH_ASSIGN_MOD = 291,         /* T_SH_ASSIGN_MOD  */
    T_SWITCH = 292,                /* T_SWITCH  */
    T_CASE = 293,                  /* T_CASE  */
    T_DEFAULT = 294,               /* T_DEFAULT  */
    T_BREAK = 295,                 /* T_BREAK  */
    T_CONTINUE = 296,              /* T_CONTINUE  */
    T_RETURN = 297,                /* T_RETURN  */
    T_VARIABLE = 298,              /* T_VARIABLE  */
    T_NMATCH = 299,                /* T_NMATCH  */
    T_ASSIGN = 300,                /* T_ASSIGN  */
    T_NE = 301,                    /* T_NE  */
    T_EQ = 302,                    /* T_EQ  */
    T_SHIFT_LEFT = 303,            /* T_SHIFT_LEFT  */
    T_SHIFT_RIGHT = 304,           /* T_SHIFT_RIGHT  */
    T_OR = 305,                    /* T_OR  */
    T_AND = 306,                   /* T_AND  */
    T_LE = 307,                    /* T_LE  */
    T_GE = 308,                    /* T_GE  */
    T_POWER_TO = 309               /* T_POWER_TO  */
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
