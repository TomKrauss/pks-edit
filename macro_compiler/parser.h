
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
     T_IN = 260,
     T_FOREACH = 261,
     T_TSTRING = 262,
     T_TINT = 263,
     T_TCHAR = 264,
     T_TLONG = 265,
     T_TFLOAT = 266,
     T_RANGE = 267,
     T_IDENT = 268,
     T_NUM = 269,
     T_FLOATING_POINT_NUMBER = 270,
     T_STRING = 271,
     T_CHARACTER = 272,
     T_CONST = 273,
     T_TRUE = 274,
     T_FALSE = 275,
     T_TBOOLEAN = 276,
     T_FUNC = 277,
     T_MACRO = 278,
     T_DOTDOT = 279,
     T_PLUSPLUS = 280,
     T_MINUSMINUS = 281,
     T_IF = 282,
     T_ELIF = 283,
     T_ELSE = 284,
     T_WHILE = 285,
     T_GOTO = 286,
     T_BRAEQ = 287,
     T_SH_ASSIGN_PLUS = 288,
     T_SH_ASSIGN_MINUS = 289,
     T_SH_ASSIGN_MULT = 290,
     T_SH_ASSIGN_DIV = 291,
     T_SH_ASSIGN_MOD = 292,
     T_LOCAL = 293,
     T_SWITCH = 294,
     T_BREAK = 295,
     T_CONTINUE = 296,
     T_RETURN = 297,
     T_NUMVAR = 298,
     T_RANGEVAR = 299,
     T_FLOATVAR = 300,
     T_STRINGVAR = 301,
     T_BOOLEANVAR = 302,
     T_CHARACTERVAR = 303,
     T_NMATCH = 304,
     T_ASSIGN = 305,
     T_NE = 306,
     T_EQ = 307,
     T_SHIFT_LEFT = 308,
     T_SHIFT_RIGHT = 309,
     T_OR = 310,
     T_AND = 311,
     T_LE = 312,
     T_GE = 313,
     T_POWER_TO = 314,
     T_VOID = 315,
     T_CMDSEQPREFIX = 316,
     T_GT = 317,
     T_LT = 318
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


