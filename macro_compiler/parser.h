
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
     T_TSTRING = 258,
     T_TINT = 259,
     T_TCHAR = 260,
     T_TLONG = 261,
     T_TFLOAT = 262,
     T_RANGE = 263,
     T_IDENT = 264,
     T_NUM = 265,
     T_FLOATING_POINT_NUMBER = 266,
     T_STRING = 267,
     T_CHARACTER = 268,
     T_CONST = 269,
     T_TRUE = 270,
     T_FALSE = 271,
     T_TBOOLEAN = 272,
     T_FUNC = 273,
     T_MACRO = 274,
     T_DOTDOT = 275,
     T_PLUSPLUS = 276,
     T_MINUSMINUS = 277,
     T_IF = 278,
     T_ELIF = 279,
     T_ELSE = 280,
     T_WHILE = 281,
     T_GOTO = 282,
     T_BRAEQ = 283,
     T_SH_ASSIGN_PLUS = 284,
     T_SH_ASSIGN_MINUS = 285,
     T_SH_ASSIGN_MULT = 286,
     T_SH_ASSIGN_DIV = 287,
     T_SH_ASSIGN_MOD = 288,
     T_LOCAL = 289,
     T_SWITCH = 290,
     T_BREAK = 291,
     T_CONTINUE = 292,
     T_RETURN = 293,
     T_NUMVAR = 294,
     T_RANGEVAR = 295,
     T_FLOATVAR = 296,
     T_STRINGVAR = 297,
     T_BOOLEANVAR = 298,
     T_CHARACTERVAR = 299,
     T_NMATCH = 300,
     T_ASSIGN = 301,
     T_NE = 302,
     T_EQ = 303,
     T_SHIFT_LEFT = 304,
     T_SHIFT_RIGHT = 305,
     T_OR = 306,
     T_AND = 307,
     T_LE = 308,
     T_GE = 309,
     T_POWER_TO = 310,
     T_VOID = 311,
     T_CMDSEQPREFIX = 312,
     T_GT = 313,
     T_LT = 314
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


