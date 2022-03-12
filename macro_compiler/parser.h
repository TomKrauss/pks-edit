
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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
     T_NMATCH = 293,
     T_ASSIGN = 294,
     T_NE = 295,
     T_EQ = 296,
     T_SHIFT_LEFT = 297,
     T_SHIFT_RIGHT = 298,
     T_OR = 299,
     T_AND = 300,
     T_LE = 301,
     T_GE = 302,
     T_POWER_TO = 303,
     T_VOID = 304,
     T_CMDSEQPREFIX = 305,
     T_GT = 306,
     T_LT = 307
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


