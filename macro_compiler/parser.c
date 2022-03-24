/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
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
#line 44 "Parser.y"


#include <windows.h>
#include <stdio.h>
#include <stddef.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"
#include "arraylist.h"

# define	YYPTRDIFF_T ptrdiff_t
# define	YYPTRDIFF_MAXIMUM SIZE_MAX

# define 	YYSTYPE _YYSTYPE

# define 	MAX_BREAK_LEVEL	6
# define	REC_SPACE		32000

static BYTECODE_BUFFER  _buffers[MAX_BREAK_LEVEL];
static BYTECODE_BUFFER* _currentBytecodeBuffer;
static void		*_currentNativeMethodCalled;
static int		_nparam,
				_breaklevel,
				_iflevel,
				_stringflg;
static      int  _currentFunctionCallParamIndices[32];
static      int  *_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
static 		void init_header(void);
static 		void finalize_header(void);
static 		void init_macroDefinition(void);
static 		void finalize_macroDefinition(void);
static 		void freeitem(char **p);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int	_currentNamespaceIndex;
static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				 *switchTableId  = "%sw%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

extern 			int	_bDefiningConst;
extern 			int	_bInHeader;
extern char*    _yyCurrentComment;
extern void		yyerror(const char* s, ...);
extern void		yywarning(char* s, ...);
extern void		yyrequire(const char* pszNamespace);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern void bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char *prefix, int level, int bratyp);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nHeapIndex, int bLocalVar, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitLocalAssignment(BYTECODE_BUFFER* pBuffer, int nHeapIndex);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
extern void bytecode_startSwitchTable(int aLevel);
extern int bytecode_generateAutoLabelNamePrefix(BYTECODE_BUFFER* pBuffer, char *prefix,int level);
extern void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
extern const char* yy_getCurrentInputFilename();

/*
 * Write out the recorded case jumps and cleanup switch table.
 */
extern void bytecode_flushSwitchTable(BYTECODE_BUFFER* pBuffer, int aLevel);

/*
 * Add a switch case condition to the current switch table being constructed.
 */
extern void bytecode_addSwitchCondition(BYTECODE_BUFFER* pBuffer, int aLevel, PKS_VALUE_TYPE t, GENERIC_DATA data);

/*
 * Destroy an array list containing TYPE_OBJECT_POINTERs.
 */
extern void bytecode_destroyArraylistWithPointers(ARRAY_LIST* pList);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, unsigned short arraySizeOrParamIndex, int nHeapIndex);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);
static ARRAY_LIST* _currentArrayLiteral;
static int			_localVariableIndex;

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void parser_switchToBuffer(int aLevel) {
	if (aLevel >= DIM(_buffers)) {
		yyerror("Nesting level of pushbacks exceeded. (maybe to many nested loops)");
		return;
	}
	_currentBytecodeBuffer = &_buffers[aLevel];
	if (!_currentBytecodeBuffer->bb_start) {
		_currentBytecodeBuffer->bb_end = ((_currentBytecodeBuffer->bb_start = malloc(REC_SPACE)) == 0) ? 
			_currentBytecodeBuffer->bb_start : _currentBytecodeBuffer->bb_start + REC_SPACE;
		_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
	}
}

static void parser_flushBuffer(int aLevel) {
	if (aLevel <= 0 ||aLevel >= DIM(_buffers) || !_buffers[aLevel].bb_start) {
		return;
	}
	size_t nBytes = _buffers[aLevel].bb_current - _buffers[aLevel].bb_start;
	if (nBytes != 0) {
		memcpy(_buffers[0].bb_current, _buffers[aLevel].bb_start, nBytes);
		_buffers[0].bb_current += nBytes;
		_buffers[aLevel].bb_current = _buffers[aLevel].bb_start;
	}
}

static void parser_emitAssignment(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_currentBytecodeBuffer->bb_current = bytecode_emitLocalAssignment(_currentBytecodeBuffer, pIdent->heapIndex);
	} else {
		_currentBytecodeBuffer->bb_current = bytecode_emitAssignment(_currentBytecodeBuffer, pIdent->s);
	}
}

static void parser_emitPushVariable(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_PUSH_LOCAL_VARIABLE, (GENERIC_DATA){.intValue=pIdent->heapIndex});
	} else {
		YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=pIdent->s});
	}
}

static void parser_defineVariable(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal, int arraySize) {
	if (arraySize < 0) {
		yyerror("Illegal negative array size %d", arraySize);
	}
	if (_bInHeader) {
		bytecode_defineVariable(_currentBytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, arraySize, 0);
	} else {
		bytecode_defineVariable(_currentBytecodeBuffer, pszName, C_DEFINE_LOCAL_VARIABLE, sType, arraySize, _localVariableIndex);
	}
	IDENTIFIER_CONTEXT* pContext = sym_getContext(_currentIdentifierContext, (char*)pszName);
	if (pContext && pContext != sym_getGlobalCompilerContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
	if (sType == VT_STRING && !tVal) {
		tVal = (intptr_t)"";
	}
	if (_bInHeader) {
		sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal}, 0);
	} else {
		sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_LOCAL_VARIABLE, sType, (GENERIC_DATA) {.val = tVal}, _localVariableIndex++);
	}
}

static void push_newForeachCursor() {
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex++});
}

static PKS_VALUE_TYPE types_pushFieldIndexWithError(PKS_VALUE_TYPE t, const char* pszFieldName) {
	PKS_VALUE_TYPE tProp;
	int nIndex = types_indexForProperty(t, pszFieldName, &tProp);
	if (nIndex < 0) {
		yyerror("unknown field %s of type %s", pszFieldName, types_nameFor(t));
	} else {
		YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue = nIndex});
	}
	free((char*)pszFieldName);
	return tProp;
}


#line 272 "parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_NAMESPACE = 3,                /* T_NAMESPACE  */
  YYSYMBOL_T_REQUIRE = 4,                  /* T_REQUIRE  */
  YYSYMBOL_T_FOR = 5,                      /* T_FOR  */
  YYSYMBOL_T_NEW = 6,                      /* T_NEW  */
  YYSYMBOL_T_STATIC = 7,                   /* T_STATIC  */
  YYSYMBOL_T_VOID = 8,                     /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 9,          /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_IDENT = 10,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 11,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 12,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 13,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 14,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 15,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 16,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 17,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 18,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 19,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 20,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 21,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 22,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 23,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 24,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 25,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 26,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 27,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 28,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 29,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 30,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 31,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 32,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 33,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 34,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 35,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 36,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 37,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 38,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 39,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 40,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 41,                      /* T_NE  */
  YYSYMBOL_T_EQ = 42,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 43,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 44,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 45,                      /* T_OR  */
  YYSYMBOL_T_AND = 46,                     /* T_AND  */
  YYSYMBOL_T_LE = 47,                      /* T_LE  */
  YYSYMBOL_T_GE = 48,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 49,                /* T_POWER_TO  */
  YYSYMBOL_50_ = 50,                       /* '&'  */
  YYSYMBOL_51_ = 51,                       /* '|'  */
  YYSYMBOL_52_ = 52,                       /* '^'  */
  YYSYMBOL_53_ = 53,                       /* '~'  */
  YYSYMBOL_54_ = 54,                       /* '<'  */
  YYSYMBOL_55_ = 55,                       /* '>'  */
  YYSYMBOL_56_ = 56,                       /* '+'  */
  YYSYMBOL_57_ = 57,                       /* '-'  */
  YYSYMBOL_58_ = 58,                       /* '*'  */
  YYSYMBOL_59_ = 59,                       /* '/'  */
  YYSYMBOL_60_ = 60,                       /* '%'  */
  YYSYMBOL_61_ = 61,                       /* '['  */
  YYSYMBOL_62_ = 62,                       /* '!'  */
  YYSYMBOL_63_ = 63,                       /* ';'  */
  YYSYMBOL_64_ = 64,                       /* ','  */
  YYSYMBOL_65_ = 65,                       /* '('  */
  YYSYMBOL_66_ = 66,                       /* ')'  */
  YYSYMBOL_67_ = 67,                       /* '{'  */
  YYSYMBOL_68_ = 68,                       /* '.'  */
  YYSYMBOL_69_ = 69,                       /* ']'  */
  YYSYMBOL_70_ = 70,                       /* ':'  */
  YYSYMBOL_71_ = 71,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 72,                  /* $accept  */
  YYSYMBOL_file_structure = 73,            /* file_structure  */
  YYSYMBOL_74_1 = 74,                      /* $@1  */
  YYSYMBOL_75_2 = 75,                      /* $@2  */
  YYSYMBOL_header_delimiter = 76,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 77,         /* macro_definitions  */
  YYSYMBOL_namespace = 78,                 /* namespace  */
  YYSYMBOL_prerequisites = 79,             /* prerequisites  */
  YYSYMBOL_require = 80,                   /* require  */
  YYSYMBOL_global_declarations = 81,       /* global_declarations  */
  YYSYMBOL_scope = 82,                     /* scope  */
  YYSYMBOL_constants = 83,                 /* constants  */
  YYSYMBOL_constant_list = 84,             /* constant_list  */
  YYSYMBOL_constdef = 85,                  /* constdef  */
  YYSYMBOL_86_3 = 86,                      /* $@3  */
  YYSYMBOL_87_4 = 87,                      /* $@4  */
  YYSYMBOL_constant_literal = 88,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 89,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 90,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 91,         /* macro_declaration  */
  YYSYMBOL_macro_type = 92,                /* macro_type  */
  YYSYMBOL_macrostart = 93,                /* macrostart  */
  YYSYMBOL_global_var = 94,                /* global_var  */
  YYSYMBOL_var_decl = 95,                  /* var_decl  */
  YYSYMBOL_optional_description = 96,      /* optional_description  */
  YYSYMBOL_parameter_list = 97,            /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 98,         /* non_empty_pardecl  */
  YYSYMBOL_par_decl = 99,                  /* par_decl  */
  YYSYMBOL_macro_definition = 100,         /* macro_definition  */
  YYSYMBOL_101_5 = 101,                    /* $@5  */
  YYSYMBOL_block = 102,                    /* block  */
  YYSYMBOL_var_decls = 103,                /* var_decls  */
  YYSYMBOL_statement = 104,                /* statement  */
  YYSYMBOL_assignment = 105,               /* assignment  */
  YYSYMBOL_106_6 = 106,                    /* $@6  */
  YYSYMBOL_shorthand_assignment = 107,     /* shorthand_assignment  */
  YYSYMBOL_108_7 = 108,                    /* $@7  */
  YYSYMBOL_assignment_target = 109,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 110, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 111,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 112,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 113, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 114,         /* range_expression  */
  YYSYMBOL_115_8 = 115,                    /* $@8  */
  YYSYMBOL_range_increment = 116,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 117,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 118,     /* increment_expression  */
  YYSYMBOL_s_bterm = 119,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 120,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 121,        /* binary_expression  */
  YYSYMBOL_condition = 122,                /* condition  */
  YYSYMBOL_value = 123,                    /* value  */
  YYSYMBOL_map_literal = 124,              /* map_literal  */
  YYSYMBOL_125_9 = 125,                    /* $@9  */
  YYSYMBOL_optional_map_associates = 126,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 127,           /* map_associates  */
  YYSYMBOL_map_associate = 128,            /* map_associate  */
  YYSYMBOL_array_literal = 129,            /* array_literal  */
  YYSYMBOL_130_10 = 130,                   /* $@10  */
  YYSYMBOL_131_11 = 131,                   /* $@11  */
  YYSYMBOL_array_elements = 132,           /* array_elements  */
  YYSYMBOL_array_element = 133,            /* array_element  */
  YYSYMBOL_simple_array_element = 134,     /* simple_array_element  */
  YYSYMBOL_string = 135,                   /* string  */
  YYSYMBOL_break = 136,                    /* break  */
  YYSYMBOL_continue = 137,                 /* continue  */
  YYSYMBOL_return_expression = 138,        /* return_expression  */
  YYSYMBOL_optional_bterm = 139,           /* optional_bterm  */
  YYSYMBOL_opt_num = 140,                  /* opt_num  */
  YYSYMBOL_case_clauses = 141,             /* case_clauses  */
  YYSYMBOL_case_clause = 142,              /* case_clause  */
  YYSYMBOL_case_selector = 143,            /* case_selector  */
  YYSYMBOL_case_condition = 144,           /* case_condition  */
  YYSYMBOL_label = 145,                    /* label  */
  YYSYMBOL_goto = 146,                     /* goto  */
  YYSYMBOL_switch_expression = 147,        /* switch_expression  */
  YYSYMBOL_148_12 = 148,                   /* $@12  */
  YYSYMBOL_for_loop_expression = 149,      /* for_loop_expression  */
  YYSYMBOL_150_13 = 150,                   /* $@13  */
  YYSYMBOL_for_increment = 151,            /* for_increment  */
  YYSYMBOL_for_to_clause = 152,            /* for_to_clause  */
  YYSYMBOL_153_14 = 153,                   /* $@14  */
  YYSYMBOL_154_15 = 154,                   /* $@15  */
  YYSYMBOL_155_16 = 155,                   /* $@16  */
  YYSYMBOL_opt_for_initializer = 156,      /* opt_for_initializer  */
  YYSYMBOL_157_17 = 157,                   /* $@17  */
  YYSYMBOL_for_clause = 158,               /* for_clause  */
  YYSYMBOL_in_clause = 159,                /* in_clause  */
  YYSYMBOL_160_18 = 160,                   /* $@18  */
  YYSYMBOL_while = 161,                    /* while  */
  YYSYMBOL_162_19 = 162,                   /* $@19  */
  YYSYMBOL_163_20 = 163,                   /* $@20  */
  YYSYMBOL_local_block = 164,              /* local_block  */
  YYSYMBOL_closing_brace = 165,            /* closing_brace  */
  YYSYMBOL_if_expression = 166,            /* if_expression  */
  YYSYMBOL_167_21 = 167,                   /* $@21  */
  YYSYMBOL_168_22 = 168,                   /* $@22  */
  YYSYMBOL_else_clause = 169,              /* else_clause  */
  YYSYMBOL_170_23 = 170,                   /* $@23  */
  YYSYMBOL_stmntlist = 171,                /* stmntlist  */
  YYSYMBOL_172_24 = 172,                   /* $@24  */
  YYSYMBOL_closing_paren = 173,            /* closing_paren  */
  YYSYMBOL_call_expression = 174,          /* call_expression  */
  YYSYMBOL_175_25 = 175,                   /* @25  */
  YYSYMBOL_176_26 = 176,                   /* @26  */
  YYSYMBOL_function_id_or_pointer = 177,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 178,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 179,                /* parameter  */
  YYSYMBOL_type_cast = 180,                /* type_cast  */
  YYSYMBOL_type_name = 181,                /* type_name  */
  YYSYMBOL_array_size = 182,               /* array_size  */
  YYSYMBOL_boolean_literal = 183,          /* boolean_literal  */
  YYSYMBOL_float_literal = 184,            /* float_literal  */
  YYSYMBOL_integer_literal = 185,          /* integer_literal  */
  YYSYMBOL_character_literal = 186,        /* character_literal  */
  YYSYMBOL_simple_literal = 187            /* simple_literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1029

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  236
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  364

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   304


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,    60,    50,     2,
      65,    66,    58,    56,    64,    57,    68,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    63,
      54,     2,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,    69,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,    51,    71,    53,     2,     2,     2,
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
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   230,   230,   236,   230,   240,   241,   243,   244,   246,
     249,   254,   255,   257,   262,   263,   264,   266,   267,   271,
     274,   275,   277,   277,   277,   291,   294,   300,   305,   312,
     319,   320,   330,   333,   339,   344,   349,   355,   357,   363,
     370,   372,   377,   378,   381,   383,   386,   393,   393,   417,
     419,   420,   422,   423,   424,   425,   426,   427,   428,   429,
     430,   431,   432,   433,   434,   435,   436,   439,   445,   449,
     449,   455,   458,   461,   461,   468,   473,   474,   475,   476,
     477,   479,   481,   482,   485,   486,   488,   489,   489,   491,
     492,   495,   496,   499,   503,   504,   505,   507,   512,   513,
     518,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   555,
     556,   560,   564,   569,   570,   576,   582,   588,   588,   596,
     598,   601,   603,   606,   610,   615,   615,   619,   619,   624,
     625,   628,   632,   637,   638,   639,   640,   642,   643,   654,
     657,   661,   665,   666,   668,   669,   671,   672,   674,   676,
     678,   683,   686,   689,   693,   698,   705,   704,   721,   721,
     733,   734,   736,   738,   740,   736,   744,   745,   745,   750,
     751,   752,   757,   757,   772,   774,   772,   785,   785,   787,
     790,   793,   790,   800,   804,   804,   811,   812,   812,   818,
     820,   820,   831,   831,   847,   857,   861,   867,   869,   870,
     871,   875,   877,   882,   885,   890,   891,   895,   898,   903,
     908,   912,   919,   924,   932,   940,   948
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  static const char *const yy_sname[] =
  {
  "end of file", "error", "invalid token", "T_NAMESPACE", "T_REQUIRE",
  "T_FOR", "T_NEW", "T_STATIC", "T_VOID", "T_TYPE_IDENTIFIER", "T_IDENT",
  "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_FUNC", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS",
  "T_IF", "T_ELSE", "T_WHILE", "T_GOTO", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_SWITCH", "T_CASE", "T_DEFAULT", "T_BREAK",
  "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_ASSOC_ARROW", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "'&'", "'|'", "'^'", "'~'", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "'!'", "';'", "','",
  "'('", "')'", "'{'", "'.'", "']'", "':'", "'}'", "$accept",
  "file_structure", "$@1", "$@2", "header_delimiter", "macro_definitions",
  "namespace", "prerequisites", "require", "global_declarations", "scope",
  "constants", "constant_list", "constdef", "$@3", "$@4",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "global_var",
  "var_decl", "optional_description", "parameter_list",
  "non_empty_pardecl", "par_decl", "macro_definition", "$@5", "block",
  "var_decls", "statement", "assignment", "$@6", "shorthand_assignment",
  "$@7", "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@8",
  "range_increment", "minusminus_plusplus", "increment_expression",
  "s_bterm", "constructor_expression", "binary_expression", "condition",
  "value", "map_literal", "$@9", "optional_map_associates",
  "map_associates", "map_associate", "array_literal", "$@10", "$@11",
  "array_elements", "array_element", "simple_array_element", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto", "switch_expression", "$@12", "for_loop_expression",
  "$@13", "for_increment", "for_to_clause", "$@14", "$@15", "$@16",
  "opt_for_initializer", "$@17", "for_clause", "in_clause", "$@18",
  "while", "$@19", "$@20", "local_block", "closing_brace", "if_expression",
  "$@21", "$@22", "else_clause", "$@23", "stmntlist", "$@24",
  "closing_paren", "call_expression", "@25", "@26",
  "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-241)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-217)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -241,    23,    34,  -241,    65,  -241,   -19,    77,  -241,    78,
    -241,   111,    54,  -241,  -241,    45,   121,    70,    84,  -241,
      90,  -241,   156,  -241,   148,    91,  -241,   156,  -241,  -241,
    -241,  -241,  -241,  -241,   118,  -241,   148,   154,     4,  -241,
     124,  -241,    15,  -241,    13,   147,   103,   102,  -241,   261,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,   161,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,   109,  -241,
    -241,  -241,  -241,   167,    44,  -241,   112,   141,   686,   686,
       8,     6,   113,   686,   610,  -241,   124,  -241,  -241,  -241,
    -241,   727,  -241,  -241,  -241,   175,  -241,   125,   700,  -241,
    -241,  -241,   512,   374,   126,  -241,   961,   193,  -241,  -241,
    -241,  -241,   120,   123,   870,   786,   127,   181,  -241,   686,
     686,   686,   686,   686,   686,   686,   686,   686,   686,   686,
     686,   686,   686,   686,   686,   686,   686,   686,   686,   686,
     686,   450,   185,  -241,  -241,   132,  -241,  -241,   135,   136,
     140,  -241,  -241,   142,  -241,   156,   144,    12,   -45,   151,
    -241,   202,   153,   209,   209,    88,  -241,    95,   150,  -241,
    -241,   158,  -241,    10,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,   157,   166,   168,   512,  -241,  -241,
    -241,  -241,  -241,    21,  -241,  -241,  -241,  -241,  -241,   198,
     157,   176,  -241,   870,   961,   916,   916,   357,   357,   893,
     893,   434,   434,    19,   939,   939,   939,   961,   434,   434,
     193,   193,    19,    19,    19,   170,  -241,   727,  -241,   512,
     237,   121,   530,  -241,  -241,  -241,   121,  -241,  -241,  -241,
    -241,   686,  -241,   151,  -241,   333,  -241,   194,   195,   870,
    -241,  -241,  -241,   197,  -241,  -241,  -241,   330,  -241,   686,
     246,  -241,  -241,  -241,  -241,   203,   123,  -241,    22,  -241,
     181,   241,  -241,   203,  -241,  -241,  -241,  -241,  -241,   156,
     174,   814,   174,  -241,  -241,   210,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,   592,  -241,  -241,   755,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,   686,  -241,  -241,   200,   211,   418,
    -241,  -241,  -241,  -241,   174,  -241,  -241,   124,   124,   219,
     686,   124,   218,   157,   245,  -241,   222,  -241,  -241,   842,
    -241,   668,  -241,  -241,  -241,    96,  -241,   224,   174,    29,
     221,   228,    96,   330,     6,  -241,  -241,   282,  -241,   263,
    -241,  -241,  -241,  -241,  -241,   122,  -241,  -241,  -241,   323,
    -241,   270,  -241,  -241
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    18,    22,     5,     0,     0,     0,    13,
      19,    20,     0,     6,    47,   223,    37,     0,    15,    16,
      22,    27,    28,    29,     0,     4,    47,    17,     0,   224,
      39,    21,     0,     8,     0,     0,     0,     0,   225,     0,
      38,   230,   229,    26,   232,   227,   228,     0,    23,   236,
     234,   233,   235,    25,    34,    35,    32,    33,     0,    50,
      48,   226,   216,     0,   136,   157,     0,   131,     0,     0,
       0,     0,   147,     0,     0,   137,     0,    81,    83,    98,
      96,    82,    94,   134,   135,   132,    95,     0,     0,   133,
     231,    24,     0,     0,     0,   210,   103,   104,    31,    30,
     105,   214,     0,     0,   102,     0,     0,   139,   128,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   158,   212,   136,   131,    99,   220,     0,
      43,    44,   221,   217,   218,     0,   216,     0,    31,     0,
     194,     0,     0,   164,   164,     0,    52,    73,    66,    63,
     207,     0,    72,     0,    64,    59,    60,    61,    65,    62,
      58,    57,    56,    55,     0,     0,     0,     0,   146,   152,
     154,   155,   156,     0,   150,   151,   153,   127,   222,     0,
       0,   140,   141,    87,   107,   126,   125,   116,   117,   120,
     119,   123,   124,   118,   108,   109,   111,   106,   121,   122,
     110,   112,   113,   114,   115,     0,    85,    84,   100,     0,
      40,     0,     0,    46,    67,   191,   186,   189,   178,   190,
     174,     0,   200,     0,   175,     0,   165,     0,     0,   163,
     161,    92,    91,     0,    68,    93,    51,     0,    54,     0,
       0,   199,    49,    53,    97,     0,     0,   148,     0,   138,
       0,    89,   101,     0,    41,    36,    45,   219,   182,     0,
       0,   130,     0,   195,   136,     0,   159,   160,    79,    80,
      76,    77,    78,     0,    66,   208,     0,    69,   209,   211,
     149,   144,   143,   142,     0,    88,   213,     0,   187,     0,
     197,   179,   129,   201,     0,   176,    74,     0,     0,    90,
       0,     0,     0,     0,   203,   196,     0,    71,    70,     0,
     188,     0,   198,   204,   202,   166,   183,     0,     0,     0,
       0,     0,   166,     0,     0,   193,   205,   230,   172,     0,
     171,   170,   177,   167,   168,    73,   181,   180,   184,     0,
     169,     0,   173,   185
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -241,  -241,  -241,  -241,  -241,   301,  -241,  -241,  -241,  -241,
     304,  -241,  -241,   308,  -241,  -241,  -241,   -24,   -79,  -241,
    -241,  -241,  -241,    -7,  -241,  -146,  -241,   128,  -241,  -241,
     296,  -241,  -206,  -241,  -241,     7,  -241,  -241,  -241,   -72,
     -49,  -241,   212,  -241,  -241,  -241,    14,   258,  -241,   -73,
     114,   115,  -241,  -241,  -241,  -241,    98,  -241,  -241,  -241,
    -241,   105,  -241,  -241,  -241,  -241,  -241,  -241,   205,    30,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -200,  -196,  -241,  -241,  -241,  -241,  -241,  -240,
    -241,   100,   -95,  -241,  -241,  -241,  -241,   145,  -241,   -14,
    -241,  -241,  -241,  -106,  -241,   -30
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    24,    35,     5,     7,    10,    11,
      16,    17,    20,    21,    22,   101,    58,    34,    86,    68,
      45,    46,    18,   294,   275,   149,   150,   151,    36,    37,
     169,   103,   170,   171,   318,   172,   253,   173,   293,    50,
     152,   225,    88,   271,   305,   255,   174,    89,    90,    91,
     242,    92,    93,   117,   200,   201,   202,    94,   112,   113,
     193,   194,   195,    95,   175,   176,   177,   250,   247,   341,
     342,   343,   349,   178,   179,   180,   326,   181,   280,   358,
     237,   307,   344,   361,   278,   321,   238,   239,   322,   182,
     243,   314,   311,   262,   183,   282,   324,   334,   338,   184,
     257,   299,    96,   187,   229,    97,   153,   154,    98,    27,
      39,    59,    60,    61,    62,    99
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      87,   110,   111,    40,   269,   106,   107,   196,   185,    26,
     114,   115,    63,   235,   118,    47,   108,   295,   108,   100,
    -215,    64,    25,     3,   167,   240,    51,    52,    53,    54,
      65,    55,    56,    51,    52,   301,    54,     4,    55,    56,
     347,   265,   348,   109,     8,   109,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   227,   323,
     116,   259,    57,    48,   310,     6,   310,   236,   260,    57,
     141,     9,   313,   273,   -31,   266,    57,   142,   155,    72,
     267,    12,   249,  -162,    73,   254,   168,  -162,    74,    51,
      52,    75,    54,   354,    55,    56,    76,    23,   310,  -215,
    -162,  -162,  -162,  -162,   325,   251,   252,    19,    13,  -162,
    -162,  -162,  -162,  -162,  -162,    77,    14,   332,   339,   340,
      25,   233,   310,    28,    51,    49,   189,   190,   346,   191,
     192,    78,   251,   252,    79,    80,    81,    29,    -7,    82,
      83,  -162,    38,    84,    30,    85,   -75,    66,    42,  -162,
     196,    13,   185,   -75,    49,    67,    31,    32,   281,    33,
      69,    71,   100,   155,   102,   156,   104,   105,   167,   157,
      57,   -30,  -145,    25,   158,   185,   296,   185,   143,   188,
     144,   186,    76,   198,   199,   228,   159,  -215,   160,   161,
    -216,   167,   230,   167,   231,   162,   232,   234,   163,   164,
     165,   109,   244,   256,   185,   155,   241,   155,   245,   185,
     246,   258,   279,   288,   289,   290,   291,   292,   261,   263,
     167,   319,    81,   350,   264,   167,   268,   166,   302,   272,
     270,   309,   129,   185,   316,   327,   328,   329,   185,   330,
     274,   138,   139,   140,   141,   308,   297,   286,   287,   167,
     304,   142,    72,   320,   167,   355,   -86,    73,   333,   298,
     -86,    74,    51,    52,    75,    54,   315,    55,    56,    76,
     -86,  -192,   337,   -86,   -86,   -86,   -86,   142,   331,   335,
     345,   351,   -86,   -86,   -86,   -86,   -86,   -86,    77,   352,
     -86,   359,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,    78,   -86,   -86,    79,    80,    81,
     -86,   -86,    82,    83,   -86,   -86,    84,   -86,    85,   -86,
     -86,   156,   -86,   360,   362,   157,   363,    43,    41,    25,
     158,    44,    70,   284,    51,    52,    75,    54,    76,    55,
      56,   356,   159,   226,   160,   161,   147,   283,   357,   276,
     285,   162,  -206,  -206,   163,   164,   165,   109,   303,   248,
     146,   300,   353,   306,     0,   156,     0,   277,     0,   157,
       0,     0,     0,    25,   158,     0,     0,     0,    81,     0,
      57,     0,    76,   166,    82,     0,   159,    69,   160,   161,
      85,  -206,     0,     0,     0,   162,   129,     0,   163,   164,
     165,   109,     0,   136,   137,   138,   139,   140,   141,   156,
       0,     0,     0,   157,     0,   142,     0,    25,   158,     0,
       0,     0,    81,     0,     0,     0,    76,   166,     0,     0,
     159,    69,   160,   161,     0,  -206,     0,     0,     0,   162,
       0,    72,   163,   164,   165,   109,    73,     0,     0,     0,
      74,    51,    52,    75,    54,     0,    55,    56,    76,     0,
       0,     0,     0,     0,     0,     0,    81,   123,   124,     0,
       0,   166,     0,   129,     0,    69,     0,    77,     0,   -50,
     136,   137,   138,   139,   140,   141,     0,     0,     0,     0,
       0,     0,   142,    78,     0,     0,    79,    80,    81,     0,
       0,    82,    83,   148,     0,    84,     0,    85,    73,   -86,
       0,    25,    74,    51,    52,    75,    54,     0,    55,    56,
      76,    72,     0,     0,     0,     0,    73,     0,     0,     0,
      74,    51,    52,    75,    54,     0,    55,    56,    76,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    78,     0,    77,    79,    80,
      81,     0,     0,    82,    83,     0,   -86,    84,   -42,    85,
       0,     0,     0,    78,     0,     0,    79,    80,    81,     0,
       0,    82,    83,    72,   -86,    84,   -86,    85,    73,     0,
       0,     0,    74,    51,    52,    75,    54,     0,    55,    56,
      76,    72,     0,     0,     0,     0,    73,     0,     0,    25,
      74,    51,    52,    75,    54,     0,    55,    56,    76,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    78,     0,    77,    79,    80,
      81,     0,     0,    82,    83,   -86,     0,    84,   -86,    85,
       0,     0,     0,    78,     0,     0,    79,    80,    81,    72,
       0,    82,    83,     0,    73,    84,     0,    85,    74,    51,
      52,    75,    54,     0,    55,    56,    76,    72,     0,     0,
       0,     0,    73,     0,     0,     0,    74,    51,    52,    75,
      54,    72,    55,    56,    76,    77,    73,     0,     0,     0,
     145,    51,    52,    75,    54,     0,    55,    56,    76,     0,
       0,    78,     0,    77,    79,    80,    81,     0,     0,    82,
      83,     0,     0,    84,   -86,    85,     0,   146,     0,    78,
       0,     0,    79,    80,    81,     0,   119,    82,    83,     0,
       0,    84,     0,    85,     0,     0,     0,    57,    81,     0,
       0,    82,     0,     0,     0,     0,   120,    85,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,     0,
       0,     0,     0,     0,   120,   142,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,     0,     0,     0,
       0,     0,     0,   142,   317,   120,     0,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,     0,     0,
       0,     0,   197,   120,   142,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,     0,     0,     0,     0,
     312,   120,   142,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,     0,   336,     0,     0,     0,   120,
     142,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   120,     0,   121,   122,   123,   124,   142,     0,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   120,     0,  -217,  -217,   123,
     124,   142,     0,   127,   128,   129,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   120,     0,
     121,   122,   123,   124,   142,     0,   127,   128,   129,     0,
       0,     0,   133,   134,   135,   136,   137,   138,   139,   140,
     141,     0,     0,     0,   123,   124,     0,   142,   127,   128,
     129,     0,     0,     0,     0,   134,   135,   136,   137,   138,
     139,   140,   141,     0,     0,     0,     0,     0,     0,   142
};

static const yytype_int16 yycheck[] =
{
      49,    80,    81,    27,   200,    78,    79,   113,   103,    16,
      83,    84,    42,     1,    86,    11,    10,   257,    10,    11,
      65,     8,     9,     0,   103,    70,    11,    12,    13,    14,
      44,    16,    17,    11,    12,    13,    14,     3,    16,    17,
      11,   187,    13,    37,    63,    37,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   309,
      84,    61,    57,    69,   280,    10,   282,    65,    68,    57,
      61,     4,   282,   229,    40,    64,    57,    68,   102,     1,
      69,    13,   165,     5,     6,   167,   103,     9,    10,    11,
      12,    13,    14,   343,    16,    17,    18,    62,   314,    65,
      22,    23,    24,    25,   314,    20,    21,    63,     7,    31,
      32,    33,    34,    35,    36,    37,    15,   323,    32,    33,
       9,   155,   338,    63,    11,    40,    13,    14,   338,    16,
      17,    53,    20,    21,    56,    57,    58,    63,     0,    61,
      62,    63,    61,    65,    64,    67,    61,    10,    40,    71,
     266,     7,   257,    68,    40,    18,    10,    11,   241,    13,
      67,    69,    11,   187,    65,     1,     9,    65,   257,     5,
      57,    40,    69,     9,    10,   280,   259,   282,    13,    69,
      65,    65,    18,    66,    13,    10,    22,    65,    24,    25,
      65,   280,    66,   282,    64,    31,    64,    63,    34,    35,
      36,    37,    10,    63,   309,   229,    65,   231,    65,   314,
      11,    63,   236,    26,    27,    28,    29,    30,    71,    63,
     309,   304,    58,   339,    66,   314,    38,    63,   268,    69,
      64,    67,    49,   338,   293,   317,   318,   320,   343,   321,
      13,    58,    59,    60,    61,   279,    10,    63,    63,   338,
      19,    68,     1,    63,   343,   344,     5,     6,    23,    66,
       9,    10,    11,    12,    13,    14,    66,    16,    17,    18,
      19,    70,   331,    22,    23,    24,    25,    68,    70,    67,
      66,    70,    31,    32,    33,    34,    35,    36,    37,    71,
      39,    19,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,     1,    71,    70,    11,     5,    66,    36,    30,     9,
      10,    37,    46,    10,    11,    12,    13,    14,    18,    16,
      17,   344,    22,   141,    24,    25,    98,   243,   344,   231,
     245,    31,    32,    33,    34,    35,    36,    37,   270,   164,
      37,   266,   342,   273,    -1,     1,    -1,   232,    -1,     5,
      -1,    -1,    -1,     9,    10,    -1,    -1,    -1,    58,    -1,
      57,    -1,    18,    63,    61,    -1,    22,    67,    24,    25,
      67,    71,    -1,    -1,    -1,    31,    49,    -1,    34,    35,
      36,    37,    -1,    56,    57,    58,    59,    60,    61,     1,
      -1,    -1,    -1,     5,    -1,    68,    -1,     9,    10,    -1,
      -1,    -1,    58,    -1,    -1,    -1,    18,    63,    -1,    -1,
      22,    67,    24,    25,    -1,    71,    -1,    -1,    -1,    31,
      -1,     1,    34,    35,    36,    37,     6,    -1,    -1,    -1,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    43,    44,    -1,
      -1,    63,    -1,    49,    -1,    67,    -1,    37,    -1,    71,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    53,    -1,    -1,    56,    57,    58,    -1,
      -1,    61,    62,     1,    -1,    65,    -1,    67,     6,    69,
      -1,     9,    10,    11,    12,    13,    14,    -1,    16,    17,
      18,     1,    -1,    -1,    -1,    -1,     6,    -1,    -1,    -1,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    -1,    37,    56,    57,
      58,    -1,    -1,    61,    62,    -1,    64,    65,    66,    67,
      -1,    -1,    -1,    53,    -1,    -1,    56,    57,    58,    -1,
      -1,    61,    62,     1,    64,    65,    66,    67,     6,    -1,
      -1,    -1,    10,    11,    12,    13,    14,    -1,    16,    17,
      18,     1,    -1,    -1,    -1,    -1,     6,    -1,    -1,     9,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    -1,    37,    56,    57,
      58,    -1,    -1,    61,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    53,    -1,    -1,    56,    57,    58,     1,
      -1,    61,    62,    -1,     6,    65,    -1,    67,    10,    11,
      12,    13,    14,    -1,    16,    17,    18,     1,    -1,    -1,
      -1,    -1,     6,    -1,    -1,    -1,    10,    11,    12,    13,
      14,     1,    16,    17,    18,    37,     6,    -1,    -1,    -1,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    -1,
      -1,    53,    -1,    37,    56,    57,    58,    -1,    -1,    61,
      62,    -1,    -1,    65,    66,    67,    -1,    37,    -1,    53,
      -1,    -1,    56,    57,    58,    -1,    19,    61,    62,    -1,
      -1,    65,    -1,    67,    -1,    -1,    -1,    57,    58,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    39,    67,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    -1,
      -1,    -1,    -1,    -1,    39,    68,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    -1,    -1,    -1,
      -1,    -1,    -1,    68,    69,    39,    -1,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    -1,    -1,
      -1,    -1,    66,    39,    68,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
      66,    39,    68,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    -1,    63,    -1,    -1,    -1,    39,
      68,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    39,    -1,    41,    42,    43,    44,    68,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    39,    -1,    41,    42,    43,
      44,    68,    -1,    47,    48,    49,    -1,    -1,    -1,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    39,    -1,
      41,    42,    43,    44,    68,    -1,    47,    48,    49,    -1,
      -1,    -1,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    -1,    -1,    -1,    43,    44,    -1,    68,    47,    48,
      49,    -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,    68
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    73,    74,     0,     3,    78,    10,    79,    63,     4,
      80,    81,    13,     7,    15,    75,    82,    83,    94,    63,
      84,    85,    86,    62,    76,     9,    95,   181,    63,    63,
      64,    10,    11,    13,    89,    77,   100,   101,    61,   182,
      89,    85,    40,    77,    82,    92,    93,    11,    69,    40,
     111,    11,    12,    13,    14,    16,    17,    57,    88,   183,
     184,   185,   186,   187,     8,   181,    10,    18,    91,    67,
     102,    69,     1,     6,    10,    13,    18,    37,    53,    56,
      57,    58,    61,    62,    65,    67,    90,   112,   114,   119,
     120,   121,   123,   124,   129,   135,   174,   177,   180,   187,
      11,    87,    65,   103,     9,    65,   121,   121,    10,    37,
      90,    90,   130,   131,   121,   121,   181,   125,   111,    19,
      39,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    68,    13,    65,    10,    37,   119,     1,    97,
      98,    99,   112,   178,   179,   181,     1,     5,    10,    22,
      24,    25,    31,    34,    35,    36,    63,    90,    95,   102,
     104,   105,   107,   109,   118,   136,   137,   138,   145,   146,
     147,   149,   161,   166,   171,   174,    65,   175,    69,    13,
      14,    16,    17,   132,   133,   134,   185,    66,    66,    13,
     126,   127,   128,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   113,   114,   121,    10,   176,
      66,    64,    64,    89,    63,     1,    65,   152,   158,   159,
      70,    65,   122,   162,    10,    65,    11,   140,   140,   121,
     139,    20,    21,   108,   111,   117,    63,   172,    63,    61,
      68,    71,   165,    63,    66,    97,    64,    69,    38,   165,
      64,   115,    69,    97,    13,    96,    99,   179,   156,   181,
     150,   121,   167,   122,    10,   123,    63,    63,    26,    27,
      28,    29,    30,   110,    95,   171,   121,    10,    66,   173,
     133,    13,   187,   128,    19,   116,   173,   153,    89,    67,
     104,   164,    66,   164,   163,    66,   112,    69,   106,   121,
      63,   157,   160,   171,   168,   164,   148,   111,   111,   121,
     111,    70,   165,    23,   169,    67,    63,   112,   170,    32,
      33,   141,   142,   143,   154,    66,   164,    11,    13,   144,
     185,    70,    71,   141,   171,    90,   107,   118,   151,    19,
      70,   155,    11,    66
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    74,    75,    73,    76,    76,    77,    77,    78,
      78,    79,    79,    80,    81,    81,    81,    82,    82,    83,
      84,    84,    86,    87,    85,    88,    88,    89,    89,    89,
      90,    90,    91,    91,    92,    92,    93,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,   101,   100,   102,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   105,   106,
     105,   105,   105,   108,   107,   109,   110,   110,   110,   110,
     110,   111,   112,   112,   113,   113,   114,   115,   114,   116,
     116,   117,   117,   118,   119,   119,   119,   120,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   122,
     122,   123,   123,   123,   123,   123,   123,   125,   124,   126,
     126,   127,   127,   128,   128,   130,   129,   131,   129,   132,
     132,   133,   133,   134,   134,   134,   134,   135,   135,   136,
     137,   138,   139,   139,   140,   140,   141,   141,   142,   143,
     143,   144,   144,   144,   145,   146,   148,   147,   150,   149,
     151,   151,   153,   154,   155,   152,   156,   157,   156,   158,
     158,   158,   160,   159,   162,   163,   161,   164,   164,   165,
     167,   168,   166,   169,   170,   169,   171,   172,   171,   173,
     175,   174,   176,   174,   177,   177,   177,    97,   178,   178,
     178,   179,   180,   181,   181,   182,   182,   183,   183,   184,
     185,   185,   186,   187,   187,   187,   187
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     0,
       3,     0,     2,     3,     0,     3,     3,     0,     1,     2,
       1,     3,     0,     0,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     6,     2,     3,     2,
       0,     1,     0,     1,     1,     3,     2,     0,     3,     4,
       0,     3,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     0,
       5,     5,     1,     0,     4,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     0,     0,     5,     0,
       2,     1,     1,     2,     1,     1,     1,     4,     1,     2,
       3,     4,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       2,     1,     1,     1,     1,     1,     1,     0,     4,     0,
       1,     1,     3,     3,     3,     0,     3,     0,     4,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     0,     2,     2,     3,
       2,     1,     1,     3,     2,     2,     0,     8,     0,     4,
       1,     1,     0,     0,     0,    10,     0,     0,     4,     1,
       1,     1,     0,     7,     0,     0,     5,     1,     3,     1,
       0,     0,     6,     0,     0,     3,     0,     0,     3,     1,
       0,     5,     0,     5,     2,     1,     1,     1,     1,     3,
       1,     1,     3,     1,     2,     2,     3,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif



static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yystrlen (yysymbol_name (yyarg[yyi]));
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp = yystpcpy (yyp, yysymbol_name (yyarg[yyi++]));
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 230 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2101 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 236 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2109 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 246 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2117 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 249 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2126 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 257 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2135 "parser.c"
    break;

  case 17: /* scope: %empty  */
#line 266 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2141 "parser.c"
    break;

  case 18: /* scope: T_STATIC  */
#line 267 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2147 "parser.c"
    break;

  case 22: /* $@3: %empty  */
#line 277 "Parser.y"
                { _bDefiningConst = 1; }
#line 2153 "parser.c"
    break;

  case 23: /* $@4: %empty  */
#line 277 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2159 "parser.c"
    break;

  case 24: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 278 "Parser.y"
                        {
				BOOL bString = (yyvsp[-1].v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, yyvsp[-3].ident.s,
					S_CONSTANT, yyvsp[-1].v.type, yyvsp[-1].v.data, 0);
				if (yyvsp[-3].ident.stringIsAlloced) {
					free(yyvsp[-3].ident.s);
				}
				if (bString) {
					free(yyvsp[-1].v.data.string);
				}
			}
#line 2175 "parser.c"
    break;

  case 25: /* constant_literal: simple_literal  */
#line 291 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2183 "parser.c"
    break;

  case 26: /* constant_literal: T_STRING  */
#line 294 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2192 "parser.c"
    break;

  case 27: /* variable_identifier: T_IDENT  */
#line 300 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2202 "parser.c"
    break;

  case 28: /* variable_identifier: T_NUM  */
#line 306 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2213 "parser.c"
    break;

  case 29: /* variable_identifier: T_STRING  */
#line 313 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2222 "parser.c"
    break;

  case 30: /* variable_reference: T_VARIABLE  */
#line 319 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2228 "parser.c"
    break;

  case 31: /* variable_reference: T_IDENT  */
#line 320 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2240 "parser.c"
    break;

  case 32: /* macro_declaration: T_IDENT  */
#line 330 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2248 "parser.c"
    break;

  case 33: /* macro_declaration: T_FUNC  */
#line 333 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2258 "parser.c"
    break;

  case 34: /* macro_type: scope T_VOID  */
#line 339 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
			}
#line 2267 "parser.c"
    break;

  case 35: /* macro_type: scope type_name  */
#line 344 "Parser.y"
                                        {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 2276 "parser.c"
    break;

  case 36: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 350 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2285 "parser.c"
    break;

  case 38: /* var_decl: type_name variable_identifier assignment_expression  */
#line 357 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2297 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier  */
#line 363 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2308 "parser.c"
    break;

  case 41: /* optional_description: T_STRING  */
#line 372 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2317 "parser.c"
    break;

  case 46: /* par_decl: type_name variable_identifier  */
#line 386 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
				bytecode_defineVariable(_currentBytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].ident.type,_nparam, _localVariableIndex++);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2328 "parser.c"
    break;

  case 47: /* $@5: %empty  */
#line 393 "Parser.y"
                  { init_macroDefinition(); }
#line 2334 "parser.c"
    break;

  case 48: /* macro_definition: $@5 macrostart block  */
#line 394 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				MACRO_PARAM param = {
					.mp_name = yyvsp[-1].ident.s,
					.mp_comment = _yyCurrentComment,
					.mp_numberOfLocalVariables = _localVariableIndex,
					.mp_bytecodeLength = _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start,
					.mp_buffer = _currentBytecodeBuffer->bb_start,
					.mp_scope = yyvsp[-1].ident.scope,
					.mp_sourceFile = yy_getCurrentInputFilename(),
					.mp_namespaceIdx = _currentNamespaceIndex
				};
				_compilerConfiguration->cb_insertNewMacro(&param);
				freeitem(&yyvsp[-1].ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			}
#line 2361 "parser.c"
    break;

  case 53: /* statement: call_expression ';'  */
#line 423 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2367 "parser.c"
    break;

  case 54: /* statement: assignment ';'  */
#line 424 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2373 "parser.c"
    break;

  case 55: /* statement: if_expression  */
#line 425 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2379 "parser.c"
    break;

  case 56: /* statement: while  */
#line 426 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2385 "parser.c"
    break;

  case 57: /* statement: for_loop_expression  */
#line 427 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2391 "parser.c"
    break;

  case 58: /* statement: switch_expression  */
#line 428 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2397 "parser.c"
    break;

  case 59: /* statement: break  */
#line 429 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2403 "parser.c"
    break;

  case 60: /* statement: continue  */
#line 430 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2409 "parser.c"
    break;

  case 61: /* statement: return_expression  */
#line 431 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2415 "parser.c"
    break;

  case 62: /* statement: goto  */
#line 432 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2421 "parser.c"
    break;

  case 63: /* statement: block  */
#line 433 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2427 "parser.c"
    break;

  case 64: /* statement: increment_expression  */
#line 434 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2433 "parser.c"
    break;

  case 65: /* statement: label  */
#line 435 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2439 "parser.c"
    break;

  case 66: /* statement: var_decl  */
#line 436 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2447 "parser.c"
    break;

  case 67: /* statement: error ';'  */
#line 439 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2457 "parser.c"
    break;

  case 68: /* assignment: variable_reference assignment_expression  */
#line 445 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2465 "parser.c"
    break;

  case 69: /* $@6: %empty  */
#line 449 "Parser.y"
                                                      {
				types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2473 "parser.c"
    break;

  case 70: /* assignment: assignment_target '.' T_IDENT $@6 assignment_expression  */
#line 451 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2481 "parser.c"
    break;

  case 71: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 455 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2489 "parser.c"
    break;

  case 73: /* $@7: %empty  */
#line 461 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2497 "parser.c"
    break;

  case 74: /* shorthand_assignment: variable_reference $@7 shorthand_assignment_operator simple_expression  */
#line 463 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2506 "parser.c"
    break;

  case 75: /* assignment_target: variable_reference  */
#line 468 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2514 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 473 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2520 "parser.c"
    break;

  case 77: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 474 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2526 "parser.c"
    break;

  case 78: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 475 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2532 "parser.c"
    break;

  case 79: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 476 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2538 "parser.c"
    break;

  case 80: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 477 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2544 "parser.c"
    break;

  case 81: /* assignment_expression: T_ASSIGN simple_expression  */
#line 479 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2550 "parser.c"
    break;

  case 82: /* simple_expression: binary_expression  */
#line 481 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2556 "parser.c"
    break;

  case 87: /* $@8: %empty  */
#line 489 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2562 "parser.c"
    break;

  case 90: /* range_increment: T_DOTDOT binary_expression  */
#line 492 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2568 "parser.c"
    break;

  case 91: /* minusminus_plusplus: T_MINUSMINUS  */
#line 495 "Parser.y"
                                     { yyval.num = -1; }
#line 2574 "parser.c"
    break;

  case 92: /* minusminus_plusplus: T_PLUSPLUS  */
#line 496 "Parser.y"
                                     { yyval.num = 1; }
#line 2580 "parser.c"
    break;

  case 93: /* increment_expression: variable_reference minusminus_plusplus  */
#line 499 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2588 "parser.c"
    break;

  case 94: /* s_bterm: value  */
#line 503 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2594 "parser.c"
    break;

  case 97: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 507 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2602 "parser.c"
    break;

  case 98: /* binary_expression: s_bterm  */
#line 512 "Parser.y"
                                { yyval.ident.type = yyvsp[0].ident.type;}
#line 2608 "parser.c"
    break;

  case 99: /* binary_expression: type_cast s_bterm  */
#line 513 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2617 "parser.c"
    break;

  case 100: /* binary_expression: binary_expression '.' T_IDENT  */
#line 518 "Parser.y"
                                                      {
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2626 "parser.c"
    break;

  case 101: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 522 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2632 "parser.c"
    break;

  case 102: /* binary_expression: '!' binary_expression  */
#line 523 "Parser.y"
                                                { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2638 "parser.c"
    break;

  case 103: /* binary_expression: '~' binary_expression  */
#line 524 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2644 "parser.c"
    break;

  case 104: /* binary_expression: '+' binary_expression  */
#line 525 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2650 "parser.c"
    break;

  case 105: /* binary_expression: '-' variable_reference  */
#line 526 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2656 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '~' binary_expression  */
#line 527 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2662 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 528 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2668 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '&' binary_expression  */
#line 529 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2674 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression '|' binary_expression  */
#line 530 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2680 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression '+' binary_expression  */
#line 531 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2686 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression '^' binary_expression  */
#line 532 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2692 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '-' binary_expression  */
#line 533 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2698 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression '*' binary_expression  */
#line 534 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2704 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression '/' binary_expression  */
#line 535 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2710 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression '%' binary_expression  */
#line 536 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2716 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 537 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2722 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 538 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2728 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 539 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2734 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression T_AND binary_expression  */
#line 540 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2740 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression T_OR binary_expression  */
#line 541 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2746 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression '<' binary_expression  */
#line 542 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2752 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '>' binary_expression  */
#line 543 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2758 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression T_LE binary_expression  */
#line 544 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2764 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression T_GE binary_expression  */
#line 545 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2770 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 546 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2776 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression T_NE binary_expression  */
#line 547 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2782 "parser.c"
    break;

  case 127: /* binary_expression: '(' binary_expression ')'  */
#line 548 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2788 "parser.c"
    break;

  case 128: /* binary_expression: variable_reference assignment_expression  */
#line 549 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2796 "parser.c"
    break;

  case 130: /* condition: '(' binary_expression  */
#line 556 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2804 "parser.c"
    break;

  case 131: /* value: T_VARIABLE  */
#line 560 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2813 "parser.c"
    break;

  case 132: /* value: string  */
#line 564 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2823 "parser.c"
    break;

  case 133: /* value: simple_literal  */
#line 569 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2829 "parser.c"
    break;

  case 134: /* value: map_literal  */
#line 570 "Parser.y"
                                      {
				yyval.ident.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2840 "parser.c"
    break;

  case 135: /* value: array_literal  */
#line 576 "Parser.y"
                                        {
				yyval.ident.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2851 "parser.c"
    break;

  case 136: /* value: T_IDENT  */
#line 582 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2861 "parser.c"
    break;

  case 137: /* $@9: %empty  */
#line 588 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
				// Hack - avoid push instruction being generated for map literal elements.
				_bDefiningConst = 1;
			}
#line 2871 "parser.c"
    break;

  case 138: /* map_literal: '{' $@9 optional_map_associates closing_brace  */
#line 592 "Parser.y"
                                                                {
				_bDefiningConst = 0;
			}
#line 2879 "parser.c"
    break;

  case 143: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 606 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2888 "parser.c"
    break;

  case 144: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 610 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2897 "parser.c"
    break;

  case 145: /* $@10: %empty  */
#line 615 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2905 "parser.c"
    break;

  case 147: /* $@11: %empty  */
#line 619 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2913 "parser.c"
    break;

  case 151: /* array_element: simple_array_element  */
#line 628 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2921 "parser.c"
    break;

  case 152: /* array_element: T_STRING  */
#line 632 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2929 "parser.c"
    break;

  case 153: /* simple_array_element: integer_literal  */
#line 637 "Parser.y"
                                        { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].v.data.longValue; }
#line 2935 "parser.c"
    break;

  case 154: /* simple_array_element: T_CHARACTER  */
#line 638 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2941 "parser.c"
    break;

  case 155: /* simple_array_element: T_TRUE  */
#line 639 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2947 "parser.c"
    break;

  case 156: /* simple_array_element: T_FALSE  */
#line 640 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2953 "parser.c"
    break;

  case 157: /* string: T_STRING  */
#line 642 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2959 "parser.c"
    break;

  case 158: /* string: string T_STRING  */
#line 643 "Parser.y"
                                          { 
				if ((yyval.ident.s = malloc(strlen(yyvsp[-1].ident.s)+strlen(yyvsp[0].ident.s)+1)) != 0) {
					strcat(strcpy(yyval.ident.s,yyvsp[-1].ident.s),yyvsp[0].ident.s);
					free(yyvsp[-1].ident.s);
					free(yyvsp[0].ident.s);
				} else {
					yyerror("String too long: buffer overflow");
					yyval.ident = yyvsp[0].ident;
				}
			}
#line 2974 "parser.c"
    break;

  case 159: /* break: T_BREAK opt_num ';'  */
#line 655 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2980 "parser.c"
    break;

  case 160: /* continue: T_CONTINUE opt_num ';'  */
#line 658 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2986 "parser.c"
    break;

  case 161: /* return_expression: T_RETURN optional_bterm  */
#line 661 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2994 "parser.c"
    break;

  case 164: /* opt_num: %empty  */
#line 668 "Parser.y"
                {	yyval.num = 1;	}
#line 3000 "parser.c"
    break;

  case 165: /* opt_num: T_NUM  */
#line 669 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3006 "parser.c"
    break;

  case 170: /* case_selector: T_DEFAULT ':'  */
#line 678 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3014 "parser.c"
    break;

  case 171: /* case_condition: integer_literal  */
#line 683 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3022 "parser.c"
    break;

  case 172: /* case_condition: T_STRING  */
#line 686 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3030 "parser.c"
    break;

  case 173: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 689 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3038 "parser.c"
    break;

  case 174: /* label: T_IDENT ':'  */
#line 693 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3047 "parser.c"
    break;

  case 175: /* goto: T_GOTO T_IDENT  */
#line 698 "Parser.y"
                               {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3056 "parser.c"
    break;

  case 176: /* $@12: %empty  */
#line 705 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3066 "parser.c"
    break;

  case 177: /* switch_expression: T_SWITCH '(' value ')' $@12 '{' case_clauses '}'  */
#line 711 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3079 "parser.c"
    break;

  case 178: /* $@13: %empty  */
#line 721 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3088 "parser.c"
    break;

  case 179: /* for_loop_expression: T_FOR for_clause $@13 local_block  */
#line 724 "Parser.y"
                                      {
				parser_flushBuffer(_breaklevel);
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3101 "parser.c"
    break;

  case 182: /* $@14: %empty  */
#line 736 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3109 "parser.c"
    break;

  case 183: /* $@15: %empty  */
#line 738 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3117 "parser.c"
    break;

  case 184: /* $@16: %empty  */
#line 740 "Parser.y"
                                        {
				parser_switchToBuffer(0);
			}
#line 3125 "parser.c"
    break;

  case 187: /* $@17: %empty  */
#line 745 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				free(yyvsp[0].ident.s);
			}
#line 3134 "parser.c"
    break;

  case 191: /* for_clause: error  */
#line 752 "Parser.y"
                                {
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3143 "parser.c"
    break;

  case 192: /* $@18: %empty  */
#line 757 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				push_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 3157 "parser.c"
    break;

  case 193: /* in_clause: '(' type_name variable_identifier $@18 ':' simple_expression ')'  */
#line 765 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 3168 "parser.c"
    break;

  case 194: /* $@19: %empty  */
#line 772 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3176 "parser.c"
    break;

  case 195: /* $@20: %empty  */
#line 774 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3185 "parser.c"
    break;

  case 196: /* while: T_WHILE $@19 condition $@20 local_block  */
#line 777 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3197 "parser.c"
    break;

  case 200: /* $@21: %empty  */
#line 790 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3206 "parser.c"
    break;

  case 201: /* $@22: %empty  */
#line 793 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3214 "parser.c"
    break;

  case 202: /* if_expression: T_IF condition $@21 local_block $@22 else_clause  */
#line 795 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3223 "parser.c"
    break;

  case 203: /* else_clause: %empty  */
#line 800 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3232 "parser.c"
    break;

  case 204: /* $@23: %empty  */
#line 804 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3242 "parser.c"
    break;

  case 206: /* stmntlist: %empty  */
#line 811 "Parser.y"
                { yyval.needsPop = 0; }
#line 3248 "parser.c"
    break;

  case 207: /* $@24: %empty  */
#line 812 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3258 "parser.c"
    break;

  case 210: /* @25: %empty  */
#line 820 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3270 "parser.c"
    break;

  case 211: /* call_expression: T_FUNC '(' @25 parameter_list closing_paren  */
#line 826 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3280 "parser.c"
    break;

  case 212: /* @26: %empty  */
#line 831 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3291 "parser.c"
    break;

  case 213: /* call_expression: function_id_or_pointer '(' @26 parameter_list closing_paren  */
#line 836 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3305 "parser.c"
    break;

  case 214: /* function_id_or_pointer: '*' variable_reference  */
#line 847 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3319 "parser.c"
    break;

  case 215: /* function_id_or_pointer: T_IDENT  */
#line 857 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3328 "parser.c"
    break;

  case 216: /* function_id_or_pointer: error  */
#line 861 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3338 "parser.c"
    break;

  case 220: /* nonempty_parameters: error  */
#line 871 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3346 "parser.c"
    break;

  case 221: /* parameter: simple_expression  */
#line 875 "Parser.y"
                                  { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3352 "parser.c"
    break;

  case 222: /* type_cast: '(' type_name ')'  */
#line 877 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3361 "parser.c"
    break;

  case 223: /* type_name: T_TYPE_IDENTIFIER  */
#line 882 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3369 "parser.c"
    break;

  case 224: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 885 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3378 "parser.c"
    break;

  case 226: /* array_size: '[' T_NUM ']'  */
#line 891 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3386 "parser.c"
    break;

  case 227: /* boolean_literal: T_TRUE  */
#line 895 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3395 "parser.c"
    break;

  case 228: /* boolean_literal: T_FALSE  */
#line 898 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3404 "parser.c"
    break;

  case 229: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 903 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3413 "parser.c"
    break;

  case 230: /* integer_literal: T_NUM  */
#line 908 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3422 "parser.c"
    break;

  case 231: /* integer_literal: '-' T_NUM  */
#line 912 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3432 "parser.c"
    break;

  case 232: /* character_literal: T_CHARACTER  */
#line 919 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3441 "parser.c"
    break;

  case 233: /* simple_literal: integer_literal  */
#line 924 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3454 "parser.c"
    break;

  case 234: /* simple_literal: float_literal  */
#line 932 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3467 "parser.c"
    break;

  case 235: /* simple_literal: character_literal  */
#line 940 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3480 "parser.c"
    break;

  case 236: /* simple_literal: boolean_literal  */
#line 948 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3493 "parser.c"
    break;


#line 3497 "parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 956 "Parser.y"


/**
 * bytecode_emitPushParameterInstruction()
 * Special processing when pushing integer literals: if the integer literal is pushed as 
 * parameter of a native function and this is form-related native function push a special
 * byte code as a marker.
 */
static void bytecode_emitPushParameterInstruction(long long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,*_currentFunctionCallParamIndexP+1)) {
		opCode = C_FORM_START;
	}
	YY_EMIT(opCode, (GENERIC_DATA){.longValue = lValue});
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
/* freeitem()					*/
/*---------------------------------*/
static void freeitem(char **p)
{
	if (*p) {
		free(*p);
		*p = 0;
	}
}


static void init_bytecodeBuffer() {
	parser_switchToBuffer(0);
	_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
}

static void destroy_bytecodeBuffer() {
	for (int i = 0; i < DIM(_buffers); i++) {
		if (_buffers[i].bb_start) {
			free(_buffers[i].bb_start);
			_buffers[i].bb_start = 0;
			_buffers[i].bb_end = 0;
		}
	}
}

static void init_header() {
	init_bytecodeBuffer();
	_bInHeader = 1;
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _currentBytecodeBuffer->bb_start, _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start);
	destroy_bytecodeBuffer();
	_bInHeader = 0;
}

/*---------------------------------*/
/* init_macroDefinition()				*/
/*---------------------------------*/
static void init_macroDefinition(void)
{
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	_localVariableIndex = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext, 0);
	init_bytecodeBuffer();
}

/*---------------------------------*/
/* finalize_macroDefinition()					*/
/*---------------------------------*/
static void finalize_macroDefinition(void) {
	destroy_bytecodeBuffer();
	_currentIdentifierContext = sym_popContext(_currentIdentifierContext);
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

