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
#include "funcdef.h"
#include "symbols.h"
#include "arraylist.h"
#include "hashmap.h"

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
extern 			int	_bInNativeDefinition;
extern char*    _yyCurrentComment;
extern void		yyerror(const char* s, ...);
extern void		yywarning(char* s, ...);
extern void		yyrequire(const char* pszNamespace);
int				function_getIndexOfFunction(void *ep);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern void bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char *prefix, int level, int bratyp);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nHeapIndex, int bLocalVar, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitLocalAssignment(BYTECODE_BUFFER* pBuffer, int nHeapIndex);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Eliminate unneccessary instructions from the byte codes generated.
 */
extern void bytecode_optimizeInstructions(BYTECODE_BUFFER* pBuffer);
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
static ARRAY_LIST* _currentArrayLiteral;
static HASHMAP*	   _currentNativeMethodAnnotation;
static int			_localVariableIndex;
static char _nativeMethodSignature[128];
IDENTIFIER_CONTEXT* _currentIdentifierContext;

static char* bytecode_signatureForType(PKS_VALUE_TYPE type) {
	char* pSig = "i";
	if (type == VT_STRING) {
		pSig = "s";
	} else if (type == VT_EDITOR_HANDLE) {
		pSig = "W";
	}
	return pSig;
}

static void bytecode_nativeMethodAddParam(PKS_VALUE_TYPE type, char* pszParamName) {
	strcat(_nativeMethodSignature, bytecode_signatureForType(type));
}

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


#line 293 "parser.c"

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
  YYSYMBOL_T_ANNOTATION = 6,               /* T_ANNOTATION  */
  YYSYMBOL_T_NATIVE = 7,                   /* T_NATIVE  */
  YYSYMBOL_T_NEW = 8,                      /* T_NEW  */
  YYSYMBOL_T_STATIC = 9,                   /* T_STATIC  */
  YYSYMBOL_T_VOID = 10,                    /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 11,         /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_IDENT = 12,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 13,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 14,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 15,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 16,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 17,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 18,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 19,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 20,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 21,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 22,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 23,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 24,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 25,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 26,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 27,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 28,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 29,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 30,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 31,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 32,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 33,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 34,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 35,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 36,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 37,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 38,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 39,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 40,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 41,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 42,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 43,                      /* T_NE  */
  YYSYMBOL_T_EQ = 44,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 45,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 46,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 47,                      /* T_OR  */
  YYSYMBOL_T_AND = 48,                     /* T_AND  */
  YYSYMBOL_T_LE = 49,                      /* T_LE  */
  YYSYMBOL_T_GE = 50,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 51,                /* T_POWER_TO  */
  YYSYMBOL_52_ = 52,                       /* '&'  */
  YYSYMBOL_53_ = 53,                       /* '|'  */
  YYSYMBOL_54_ = 54,                       /* '^'  */
  YYSYMBOL_55_ = 55,                       /* '~'  */
  YYSYMBOL_56_ = 56,                       /* '<'  */
  YYSYMBOL_57_ = 57,                       /* '>'  */
  YYSYMBOL_58_ = 58,                       /* '+'  */
  YYSYMBOL_59_ = 59,                       /* '-'  */
  YYSYMBOL_60_ = 60,                       /* '*'  */
  YYSYMBOL_61_ = 61,                       /* '/'  */
  YYSYMBOL_62_ = 62,                       /* '%'  */
  YYSYMBOL_63_ = 63,                       /* '['  */
  YYSYMBOL_64_ = 64,                       /* '!'  */
  YYSYMBOL_65_ = 65,                       /* ';'  */
  YYSYMBOL_66_ = 66,                       /* ','  */
  YYSYMBOL_67_ = 67,                       /* '('  */
  YYSYMBOL_68_ = 68,                       /* ')'  */
  YYSYMBOL_69_ = 69,                       /* '{'  */
  YYSYMBOL_70_ = 70,                       /* '.'  */
  YYSYMBOL_71_ = 71,                       /* ']'  */
  YYSYMBOL_72_ = 72,                       /* ':'  */
  YYSYMBOL_73_ = 73,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 74,                  /* $accept  */
  YYSYMBOL_file_structure = 75,            /* file_structure  */
  YYSYMBOL_76_1 = 76,                      /* $@1  */
  YYSYMBOL_77_2 = 77,                      /* $@2  */
  YYSYMBOL_header_delimiter = 78,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 79,         /* macro_definitions  */
  YYSYMBOL_namespace = 80,                 /* namespace  */
  YYSYMBOL_prerequisites = 81,             /* prerequisites  */
  YYSYMBOL_require = 82,                   /* require  */
  YYSYMBOL_global_declarations = 83,       /* global_declarations  */
  YYSYMBOL_scope = 84,                     /* scope  */
  YYSYMBOL_constants = 85,                 /* constants  */
  YYSYMBOL_constant_list = 86,             /* constant_list  */
  YYSYMBOL_constdef = 87,                  /* constdef  */
  YYSYMBOL_88_3 = 88,                      /* $@3  */
  YYSYMBOL_89_4 = 89,                      /* $@4  */
  YYSYMBOL_constant_literal = 90,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 91,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 92,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 93,         /* macro_declaration  */
  YYSYMBOL_macro_type = 94,                /* macro_type  */
  YYSYMBOL_macrostart = 95,                /* macrostart  */
  YYSYMBOL_global_var = 96,                /* global_var  */
  YYSYMBOL_var_decl = 97,                  /* var_decl  */
  YYSYMBOL_optional_description = 98,      /* optional_description  */
  YYSYMBOL_parameter_list = 99,            /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 100,        /* non_empty_pardecl  */
  YYSYMBOL_parameter_declaration = 101,    /* parameter_declaration  */
  YYSYMBOL_annotations = 102,              /* annotations  */
  YYSYMBOL_annotation = 103,               /* annotation  */
  YYSYMBOL_104_5 = 104,                    /* $@5  */
  YYSYMBOL_annotation_value = 105,         /* annotation_value  */
  YYSYMBOL_annotation_values = 106,        /* annotation_values  */
  YYSYMBOL_native_macro_definition = 107,  /* native_macro_definition  */
  YYSYMBOL_108_6 = 108,                    /* $@6  */
  YYSYMBOL_macro_definition = 109,         /* macro_definition  */
  YYSYMBOL_110_7 = 110,                    /* $@7  */
  YYSYMBOL_block = 111,                    /* block  */
  YYSYMBOL_var_decls = 112,                /* var_decls  */
  YYSYMBOL_statement = 113,                /* statement  */
  YYSYMBOL_assignment = 114,               /* assignment  */
  YYSYMBOL_115_8 = 115,                    /* $@8  */
  YYSYMBOL_shorthand_assignment = 116,     /* shorthand_assignment  */
  YYSYMBOL_117_9 = 117,                    /* $@9  */
  YYSYMBOL_assignment_target = 118,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 119, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 120,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 121,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 122, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 123,         /* range_expression  */
  YYSYMBOL_124_10 = 124,                   /* $@10  */
  YYSYMBOL_range_increment = 125,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 126,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 127,     /* increment_expression  */
  YYSYMBOL_s_bterm = 128,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 129,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 130,        /* binary_expression  */
  YYSYMBOL_condition = 131,                /* condition  */
  YYSYMBOL_value = 132,                    /* value  */
  YYSYMBOL_map_literal = 133,              /* map_literal  */
  YYSYMBOL_134_11 = 134,                   /* $@11  */
  YYSYMBOL_optional_map_associates = 135,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 136,           /* map_associates  */
  YYSYMBOL_map_associate = 137,            /* map_associate  */
  YYSYMBOL_array_literal = 138,            /* array_literal  */
  YYSYMBOL_139_12 = 139,                   /* $@12  */
  YYSYMBOL_140_13 = 140,                   /* $@13  */
  YYSYMBOL_array_elements = 141,           /* array_elements  */
  YYSYMBOL_array_element = 142,            /* array_element  */
  YYSYMBOL_simple_array_element = 143,     /* simple_array_element  */
  YYSYMBOL_string = 144,                   /* string  */
  YYSYMBOL_break = 145,                    /* break  */
  YYSYMBOL_continue = 146,                 /* continue  */
  YYSYMBOL_return_expression = 147,        /* return_expression  */
  YYSYMBOL_optional_bterm = 148,           /* optional_bterm  */
  YYSYMBOL_opt_num = 149,                  /* opt_num  */
  YYSYMBOL_case_clauses = 150,             /* case_clauses  */
  YYSYMBOL_case_clause = 151,              /* case_clause  */
  YYSYMBOL_case_selector = 152,            /* case_selector  */
  YYSYMBOL_case_condition = 153,           /* case_condition  */
  YYSYMBOL_label = 154,                    /* label  */
  YYSYMBOL_goto = 155,                     /* goto  */
  YYSYMBOL_switch_expression = 156,        /* switch_expression  */
  YYSYMBOL_157_14 = 157,                   /* $@14  */
  YYSYMBOL_for_loop_expression = 158,      /* for_loop_expression  */
  YYSYMBOL_159_15 = 159,                   /* $@15  */
  YYSYMBOL_for_increment = 160,            /* for_increment  */
  YYSYMBOL_for_to_clause = 161,            /* for_to_clause  */
  YYSYMBOL_162_16 = 162,                   /* $@16  */
  YYSYMBOL_163_17 = 163,                   /* $@17  */
  YYSYMBOL_164_18 = 164,                   /* $@18  */
  YYSYMBOL_opt_for_initializer = 165,      /* opt_for_initializer  */
  YYSYMBOL_166_19 = 166,                   /* $@19  */
  YYSYMBOL_for_clause = 167,               /* for_clause  */
  YYSYMBOL_in_clause = 168,                /* in_clause  */
  YYSYMBOL_169_20 = 169,                   /* $@20  */
  YYSYMBOL_while = 170,                    /* while  */
  YYSYMBOL_171_21 = 171,                   /* $@21  */
  YYSYMBOL_172_22 = 172,                   /* $@22  */
  YYSYMBOL_local_block = 173,              /* local_block  */
  YYSYMBOL_closing_brace = 174,            /* closing_brace  */
  YYSYMBOL_if_expression = 175,            /* if_expression  */
  YYSYMBOL_176_23 = 176,                   /* $@23  */
  YYSYMBOL_177_24 = 177,                   /* $@24  */
  YYSYMBOL_else_clause = 178,              /* else_clause  */
  YYSYMBOL_179_25 = 179,                   /* $@25  */
  YYSYMBOL_stmntlist = 180,                /* stmntlist  */
  YYSYMBOL_181_26 = 181,                   /* $@26  */
  YYSYMBOL_closing_paren = 182,            /* closing_paren  */
  YYSYMBOL_call_expression = 183,          /* call_expression  */
  YYSYMBOL_184_27 = 184,                   /* @27  */
  YYSYMBOL_185_28 = 185,                   /* @28  */
  YYSYMBOL_function_id_or_pointer = 186,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 187,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 188,                /* parameter  */
  YYSYMBOL_type_cast = 189,                /* type_cast  */
  YYSYMBOL_type_name = 190,                /* type_name  */
  YYSYMBOL_array_size = 191,               /* array_size  */
  YYSYMBOL_boolean_literal = 192,          /* boolean_literal  */
  YYSYMBOL_float_literal = 193,            /* float_literal  */
  YYSYMBOL_integer_literal = 194,          /* integer_literal  */
  YYSYMBOL_character_literal = 195,        /* character_literal  */
  YYSYMBOL_simple_literal = 196            /* simple_literal  */
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
#define YYLAST   1055

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  123
/* YYNRULES -- Number of rules.  */
#define YYNRULES  247
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  383

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


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
       2,     2,     2,    64,     2,     2,     2,    62,    52,     2,
      67,    68,    60,    58,    66,    59,    70,    61,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,    65,
      56,     2,    57,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    71,    54,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,    53,    73,    55,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   251,   251,   257,   251,   261,   262,   264,   265,   267,
     270,   275,   276,   278,   283,   284,   285,   286,   288,   289,
     293,   296,   297,   299,   299,   299,   313,   316,   322,   327,
     334,   341,   342,   352,   355,   361,   370,   379,   386,   388,
     394,   401,   403,   408,   409,   412,   414,   417,   428,   429,
     431,   431,   438,   447,   448,   449,   451,   451,   484,   484,
     509,   511,   512,   514,   515,   516,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   526,   527,   528,   531,   537,
     541,   541,   547,   550,   553,   553,   560,   565,   566,   567,
     568,   569,   571,   573,   574,   577,   578,   580,   581,   581,
     583,   584,   587,   588,   591,   595,   596,   597,   599,   604,
     605,   610,   614,   615,   616,   617,   618,   619,   620,   621,
     622,   623,   624,   625,   626,   627,   628,   629,   630,   631,
     632,   633,   634,   635,   636,   637,   638,   639,   640,   641,
     647,   648,   652,   656,   661,   662,   668,   674,   680,   680,
     688,   690,   693,   695,   698,   702,   707,   707,   711,   711,
     716,   717,   720,   724,   729,   730,   731,   732,   734,   735,
     746,   749,   753,   757,   758,   760,   761,   763,   764,   766,
     768,   770,   775,   778,   781,   785,   790,   797,   796,   813,
     813,   825,   826,   828,   830,   832,   828,   837,   838,   838,
     843,   844,   845,   850,   850,   865,   867,   865,   878,   878,
     880,   883,   886,   883,   893,   897,   897,   904,   905,   905,
     911,   913,   913,   924,   924,   940,   950,   954,   960,   962,
     963,   964,   968,   970,   975,   978,   983,   984,   988,   991,
     996,  1001,  1005,  1012,  1017,  1025,  1033,  1041
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
  "T_FOR", "T_ANNOTATION", "T_NATIVE", "T_NEW", "T_STATIC", "T_VOID",
  "T_TYPE_IDENTIFIER", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_CHARACTER", "T_CONST", "T_TRUE", "T_FALSE", "T_FUNC",
  "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_SH_ASSIGN_PLUS", "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT",
  "T_SH_ASSIGN_DIV", "T_SH_ASSIGN_MOD", "T_SWITCH", "T_CASE", "T_DEFAULT",
  "T_BREAK", "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_ASSOC_ARROW",
  "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT",
  "T_OR", "T_AND", "T_LE", "T_GE", "T_POWER_TO", "'&'", "'|'", "'^'",
  "'~'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "'!'",
  "';'", "','", "'('", "')'", "'{'", "'.'", "']'", "':'", "'}'", "$accept",
  "file_structure", "$@1", "$@2", "header_delimiter", "macro_definitions",
  "namespace", "prerequisites", "require", "global_declarations", "scope",
  "constants", "constant_list", "constdef", "$@3", "$@4",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "global_var",
  "var_decl", "optional_description", "parameter_list",
  "non_empty_pardecl", "parameter_declaration", "annotations",
  "annotation", "$@5", "annotation_value", "annotation_values",
  "native_macro_definition", "$@6", "macro_definition", "$@7", "block",
  "var_decls", "statement", "assignment", "$@8", "shorthand_assignment",
  "$@9", "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@10",
  "range_increment", "minusminus_plusplus", "increment_expression",
  "s_bterm", "constructor_expression", "binary_expression", "condition",
  "value", "map_literal", "$@11", "optional_map_associates",
  "map_associates", "map_associate", "array_literal", "$@12", "$@13",
  "array_elements", "array_element", "simple_array_element", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto", "switch_expression", "$@14", "for_loop_expression",
  "$@15", "for_increment", "for_to_clause", "$@16", "$@17", "$@18",
  "opt_for_initializer", "$@19", "for_clause", "in_clause", "$@20",
  "while", "$@21", "$@22", "local_block", "closing_brace", "if_expression",
  "$@23", "$@24", "else_clause", "$@25", "stmntlist", "$@26",
  "closing_paren", "call_expression", "@27", "@28",
  "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-258)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-228)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -258,    20,    86,  -258,    90,  -258,    33,   100,  -258,   110,
    -258,   128,    63,  -258,  -258,    67,   133,    91,    93,    96,
     143,  -258,   106,  -258,     9,  -258,   160,   103,  -258,     9,
    -258,  -258,  -258,  -258,   166,   143,  -258,  -258,  -258,  -258,
     134,  -258,   160,   171,     1,  -258,   139,   116,   171,  -258,
    -258,    16,  -258,    84,     5,   120,   104,  -258,   270,  -258,
     175,  -258,  -258,  -258,  -258,  -258,  -258,  -258,   180,  -258,
    -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,   132,
    -258,  -258,  -258,  -258,   191,   -27,  -258,   138,   164,   633,
     633,    75,    80,   136,   633,   568,  -258,   139,  -258,  -258,
    -258,  -258,   753,  -258,  -258,  -258,   193,  -258,   152,   726,
    -258,   168,   154,   156,  -258,  -258,   472,   158,   159,  -258,
     710,   321,  -258,  -258,  -258,  -258,   157,   185,   896,   812,
     161,   217,  -258,   633,   633,   633,   633,   633,   633,   633,
     633,   633,   633,   633,   633,   633,   633,   633,   633,   633,
     633,   633,   633,   633,   633,   439,   221,  -258,  -258,   167,
    -258,  -258,   220,   175,  -258,   169,   172,   173,  -258,  -258,
     176,  -258,     9,   183,    12,   -49,   178,  -258,   237,   190,
     239,   239,   341,  -258,    94,   195,  -258,  -258,   196,  -258,
     -37,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,
    -258,   177,   197,   198,   472,  -258,  -258,  -258,  -258,  -258,
      15,  -258,  -258,  -258,  -258,  -258,   218,   177,   199,  -258,
     896,   710,   942,   942,    92,    92,   919,   919,   985,   985,
      10,   965,   965,   965,   710,   985,   985,   321,   321,    10,
      10,    10,   192,  -258,   753,  -258,   472,  -258,  -258,   249,
     133,   504,  -258,  -258,  -258,   133,  -258,  -258,  -258,  -258,
     633,  -258,   178,  -258,   108,  -258,   203,   204,   896,  -258,
    -258,  -258,   184,  -258,  -258,  -258,    73,  -258,   633,   258,
    -258,  -258,  -258,  -258,   208,   185,  -258,    23,  -258,   217,
     256,  -258,   208,  -258,  -258,  -258,  -258,  -258,     9,   693,
     840,   693,  -258,  -258,   211,  -258,  -258,  -258,  -258,  -258,
    -258,  -258,   536,  -258,  -258,   781,  -258,  -258,  -258,  -258,
    -258,  -258,  -258,   633,  -258,  -258,   215,   226,   406,  -258,
    -258,  -258,  -258,   693,  -258,  -258,   139,   139,   223,   633,
     139,   227,   177,   262,  -258,   231,  -258,  -258,   868,  -258,
     601,  -258,  -258,  -258,    95,  -258,   233,   693,    31,   230,
     271,    95,    73,    80,  -258,  -258,   289,  -258,   240,  -258,
    -258,  -258,  -258,  -258,   118,  -258,  -258,  -258,   332,  -258,
     279,  -258,  -258
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    19,    23,     5,     0,     0,     0,     0,
      48,    13,    20,    21,     0,     6,    58,   234,    38,     0,
      15,    16,    17,    50,     0,    48,    23,    28,    29,    30,
       0,     4,    58,    18,     0,   235,    40,     0,    18,    49,
      22,     0,     8,     0,     0,     0,     0,   236,     0,    39,
      53,    57,   241,   240,    27,   243,   238,   239,     0,    24,
     247,   245,   244,   246,    26,    35,    36,    33,    34,     0,
      61,    59,   237,   227,     0,   147,   168,     0,   142,     0,
       0,     0,     0,   158,     0,     0,   148,     0,    92,    94,
     109,   107,    93,   105,   145,   146,   143,   106,     0,     0,
     144,     0,    54,     0,   242,    25,     0,     0,     0,   221,
     114,   115,    32,    31,   116,   225,     0,     0,   113,     0,
       0,   150,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   169,   223,   147,
     142,   110,     0,    53,    51,   231,     0,    44,    45,   232,
     228,   229,     0,   227,     0,    32,     0,   205,     0,     0,
     175,   175,     0,    63,    84,    77,    74,   218,     0,    83,
       0,    75,    70,    71,    72,    76,    73,    69,    68,    67,
      66,     0,     0,     0,     0,   157,   163,   165,   166,   167,
       0,   161,   162,   164,   138,   233,     0,     0,   151,   152,
      98,   118,   137,   136,   127,   128,   131,   130,   134,   135,
     129,   119,   120,   122,   117,   132,   133,   121,   123,   124,
     125,   126,     0,    96,    95,   111,     0,    52,    55,    41,
       0,     0,    47,    78,   202,   197,   200,   189,   201,   185,
       0,   211,     0,   186,     0,   176,     0,     0,   174,   172,
     103,   102,     0,    79,   104,    62,     0,    65,     0,     0,
     210,    60,    64,   108,     0,     0,   159,     0,   149,     0,
     100,   112,     0,    42,    37,    46,   230,   193,     0,     0,
     141,     0,   206,   147,     0,   170,   171,    90,    91,    87,
      88,    89,     0,    77,   219,     0,    80,   220,   222,   160,
     155,   154,   153,     0,    99,   224,     0,   198,     0,   208,
     190,   140,   212,     0,   187,    85,     0,     0,   101,     0,
       0,     0,     0,   214,   207,     0,    82,    81,     0,   199,
       0,   209,   215,   213,   177,   194,     0,     0,     0,     0,
       0,   177,     0,     0,   204,   216,   241,   183,     0,   182,
     181,   188,   178,   179,    84,   192,   191,   195,     0,   180,
       0,   184,   196
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -258,  -258,  -258,  -258,  -258,   306,  -258,  -258,  -258,  -258,
     339,  -258,  -258,   315,  -258,  -258,  -258,   -24,   -90,  -258,
    -258,   310,  -258,    -4,  -258,  -128,  -258,   112,   328,  -258,
    -258,  -258,   201,  -258,  -258,  -258,  -258,   314,  -258,  -256,
    -258,  -258,     7,  -258,  -258,  -258,   -81,   -58,  -258,   216,
    -258,  -258,  -258,    22,   264,  -258,   -86,   124,   123,  -258,
    -258,  -258,  -258,    99,  -258,  -258,  -258,  -258,   105,  -258,
    -258,  -258,  -258,  -258,  -258,   212,    28,  -258,  -258,  -258,
    -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,
    -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -258,  -218,
    -210,  -258,  -258,  -258,  -258,  -258,  -257,  -258,   102,  -111,
    -258,  -258,  -258,  -258,   141,  -258,   -25,  -258,  -258,  -258,
    -117,  -258,   -40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    26,    41,     5,     7,    10,    11,
      53,    17,    22,    23,    24,   115,    69,    40,    97,    79,
      54,    55,    18,   313,   294,   166,   167,   168,    34,    35,
      47,   112,   113,    19,    20,    42,    43,   186,   117,   187,
     188,   337,   189,   272,   190,   312,    59,   169,   242,    99,
     290,   324,   274,   191,   100,   101,   102,   261,   103,   104,
     131,   217,   218,   219,   105,   126,   127,   210,   211,   212,
     106,   192,   193,   194,   269,   266,   360,   361,   362,   368,
     195,   196,   197,   345,   198,   299,   377,   256,   326,   363,
     380,   297,   340,   257,   258,   341,   199,   262,   333,   330,
     281,   200,   301,   343,   353,   357,   201,   276,   318,   107,
     204,   246,   108,   170,   171,   109,    29,    45,    70,    71,
      72,    73,   110
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      98,   124,   125,   120,   121,    46,   202,   288,   128,   129,
     213,    74,    28,   254,    56,   -32,   132,    77,  -226,   314,
       3,    37,    38,   259,    39,    78,   278,   184,    76,    62,
      63,    64,    65,   279,    66,    67,    62,    63,   320,    65,
    -226,    66,    67,   329,   366,   329,   367,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   244,
     130,   342,    57,   155,   173,    68,   284,   329,   174,   255,
     156,   285,    68,   332,    27,   175,   286,   122,   114,     4,
      68,   172,   122,    87,    75,    27,   268,   176,     8,   177,
     178,   329,     6,   273,     9,   373,   179,  -217,  -217,   180,
     181,   182,   123,   185,   123,   344,   270,   271,   292,   123,
     303,    62,    63,    86,    65,    12,    66,    67,    21,   358,
     359,    25,   351,    92,   -56,   -56,    58,    13,   183,   365,
     270,   271,    80,   143,    27,    14,  -217,   160,   252,    33,
     150,   151,   152,   153,   154,   155,    30,   -86,    31,   173,
      -7,    32,   156,   174,   -86,   202,    44,    68,   213,    27,
     175,    93,    36,    48,   300,    82,    51,    96,    87,   172,
      13,    58,   176,    60,   177,   178,   184,   111,   202,    80,
     202,   179,   315,   114,   180,   181,   182,   123,    62,   116,
     206,   207,   118,   208,   209,   119,   -31,  -156,   157,   184,
     162,   184,   307,   308,   309,   310,   311,   202,    92,   158,
     163,   172,   202,   183,   164,   172,   203,    80,   205,   215,
     298,  -217,   216,   245,  -226,   247,  -227,   338,   184,   250,
     249,   369,   251,   184,    68,   260,   202,   321,   253,   263,
     280,   202,   265,   348,   335,   346,   347,   264,   287,   349,
     275,   277,   282,   291,   293,   289,   283,   184,   305,   306,
     316,    83,   184,   374,   327,   -97,   317,   323,    84,   334,
     339,   -97,    85,    62,    63,    86,    65,   352,    66,    67,
      87,   -97,   356,   156,   -97,   -97,   -97,   -97,  -203,   350,
     354,   364,   370,   -97,   -97,   -97,   -97,   -97,   -97,    88,
     378,   -97,   379,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,    89,   -97,   -97,    90,    91,
      92,   -97,   -97,    93,    94,   -97,   -97,    95,   -97,    96,
     -97,   -97,    83,   -97,   371,   381,  -173,   382,    52,    84,
      16,    50,  -173,    85,    62,    63,    86,    65,    61,    66,
      67,    87,   295,    49,   248,  -173,  -173,  -173,  -173,    81,
     375,   243,   143,   161,  -173,  -173,  -173,  -173,  -173,  -173,
      88,   152,   153,   154,   155,   376,   302,   304,   322,   372,
     319,   156,   296,   267,   325,     0,    89,     0,     0,    90,
      91,    92,     0,     0,    93,    94,  -173,   173,    95,     0,
      96,   174,     0,     0,  -173,     0,     0,    27,   175,     0,
       0,     0,     0,     0,     0,     0,    87,     0,     0,     0,
     176,     0,   177,   178,     0,     0,     0,     0,     0,   179,
      83,     0,   180,   181,   182,   123,     0,    84,     0,     0,
       0,    85,    62,    63,    86,    65,     0,    66,    67,    87,
       0,     0,     0,     0,     0,     0,    92,     0,     0,     0,
       0,   183,     0,   165,     0,    80,     0,     0,    88,   -61,
      84,     0,     0,    27,    85,    62,    63,    86,    65,     0,
      66,    67,    87,     0,    89,     0,     0,    90,    91,    92,
       0,     0,    93,    94,     0,    83,    95,     0,    96,     0,
     -97,    88,    84,     0,     0,     0,    85,    62,    63,    86,
      65,     0,    66,    67,    87,     0,     0,    89,     0,     0,
      90,    91,    92,     0,     0,    93,    94,    83,   -97,    95,
     -43,    96,     0,    88,    84,     0,     0,     0,    85,    62,
      63,    86,    65,     0,    66,    67,    87,     0,     0,    89,
       0,     0,    90,    91,    92,     0,     0,    93,    94,    83,
     -97,    95,   -97,    96,     0,    88,    84,     0,     0,    27,
      85,    62,    63,    86,    65,     0,    66,    67,    87,     0,
       0,    89,     0,     0,    90,    91,    92,     0,     0,    93,
      94,   -97,    83,    95,   -97,    96,     0,    88,     0,    84,
       0,     0,     0,    85,    62,    63,    86,    65,     0,    66,
      67,    87,     0,    89,     0,     0,    90,    91,    92,     0,
       0,    93,    94,     0,    83,    95,     0,    96,     0,     0,
      88,    84,     0,     0,     0,    85,    62,    63,    86,    65,
       0,    66,    67,    87,     0,     0,    89,     0,     0,    90,
      91,    92,     0,     0,    93,    94,     0,     0,    95,   -97,
      96,     0,    88,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    89,     0,
       0,    90,    91,    92,   173,     0,    93,    94,   174,     0,
      95,     0,    96,     0,    27,   175,     0,     0,     0,     0,
       0,     0,     0,    87,     0,     0,     0,   176,     0,   177,
     178,     0,     0,     0,     0,     0,   179,    83,     0,   180,
     181,   182,   123,     0,    84,     0,     0,     0,   159,    62,
      63,    86,    65,     0,    66,    67,    87,     0,     0,     0,
       0,     0,     0,    92,     0,   137,   138,     0,   183,   141,
     142,   143,   328,     0,     0,   160,   148,   149,   150,   151,
     152,   153,   154,   155,   133,     0,     0,     0,     0,     0,
     156,     0,     0,     0,     0,    68,    92,     0,     0,    93,
       0,     0,     0,     0,   134,    96,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,     0,     0,
       0,     0,   134,   156,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,     0,     0,     0,
       0,   156,   336,   134,     0,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,     0,     0,
     214,   134,   156,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,     0,     0,   331,   134,
     156,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,   355,     0,     0,     0,   134,   156,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     134,     0,   135,   136,   137,   138,   156,     0,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   134,     0,  -228,  -228,   137,   138,   156,
       0,   141,   142,   143,     0,     0,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   134,     0,   135,   136,
     137,   138,   156,     0,   141,   142,   143,     0,     0,     0,
     147,   148,   149,   150,   151,   152,   153,   154,   155,     0,
     137,   138,     0,     0,     0,   156,   143,     0,     0,     0,
       0,     0,     0,   150,   151,   152,   153,   154,   155,     0,
       0,     0,     0,     0,     0,   156
};

static const yytype_int16 yycheck[] =
{
      58,    91,    92,    89,    90,    29,   117,   217,    94,    95,
     127,    51,    16,     1,    13,    42,    97,    12,    67,   276,
       0,    12,    13,    72,    15,    20,    63,   117,    53,    13,
      14,    15,    16,    70,    18,    19,    13,    14,    15,    16,
      67,    18,    19,   299,    13,   301,    15,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
      95,   328,    71,    63,     1,    59,   204,   333,     5,    67,
      70,    66,    59,   301,    11,    12,    71,    12,    13,     3,
      59,   116,    12,    20,    10,    11,   182,    24,    65,    26,
      27,   357,    12,   184,     4,   362,    33,    34,    35,    36,
      37,    38,    39,   117,    39,   333,    22,    23,   246,    39,
      12,    13,    14,    15,    16,    15,    18,    19,    65,    34,
      35,    64,   342,    60,     6,     7,    42,     9,    65,   357,
      22,    23,    69,    51,    11,    17,    73,    39,   172,     6,
      58,    59,    60,    61,    62,    63,    65,    63,    65,     1,
       0,    65,    70,     5,    70,   276,    63,    59,   285,    11,
      12,    63,    66,     7,   260,    71,    42,    69,    20,   204,
       9,    42,    24,    67,    26,    27,   276,    12,   299,    69,
     301,    33,   278,    13,    36,    37,    38,    39,    13,    67,
      15,    16,    11,    18,    19,    67,    42,    71,    15,   299,
      42,   301,    28,    29,    30,    31,    32,   328,    60,    67,
      66,   246,   333,    65,    68,   250,    67,    69,    71,    68,
     255,    73,    15,    12,    67,    15,    67,   323,   328,    66,
      68,   358,    66,   333,    59,    67,   357,   287,    65,    12,
      73,   362,    13,   339,   312,   336,   337,    67,    40,   340,
      65,    65,    65,    71,    15,    66,    68,   357,    65,    65,
      12,     1,   362,   363,   298,     5,    68,    21,     8,    68,
      65,    11,    12,    13,    14,    15,    16,    25,    18,    19,
      20,    21,   350,    70,    24,    25,    26,    27,    72,    72,
      69,    68,    72,    33,    34,    35,    36,    37,    38,    39,
      21,    41,    72,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,     1,    73,    73,    13,     5,    68,    42,     8,
      11,    36,    11,    12,    13,    14,    15,    16,    48,    18,
      19,    20,   250,    35,   163,    24,    25,    26,    27,    55,
     363,   155,    51,   109,    33,    34,    35,    36,    37,    38,
      39,    60,    61,    62,    63,   363,   262,   264,   289,   361,
     285,    70,   251,   181,   292,    -1,    55,    -1,    -1,    58,
      59,    60,    -1,    -1,    63,    64,    65,     1,    67,    -1,
      69,     5,    -1,    -1,    73,    -1,    -1,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,
      24,    -1,    26,    27,    -1,    -1,    -1,    -1,    -1,    33,
       1,    -1,    36,    37,    38,    39,    -1,     8,    -1,    -1,
      -1,    12,    13,    14,    15,    16,    -1,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      -1,    65,    -1,     1,    -1,    69,    -1,    -1,    39,    73,
       8,    -1,    -1,    11,    12,    13,    14,    15,    16,    -1,
      18,    19,    20,    -1,    55,    -1,    -1,    58,    59,    60,
      -1,    -1,    63,    64,    -1,     1,    67,    -1,    69,    -1,
      71,    39,     8,    -1,    -1,    -1,    12,    13,    14,    15,
      16,    -1,    18,    19,    20,    -1,    -1,    55,    -1,    -1,
      58,    59,    60,    -1,    -1,    63,    64,     1,    66,    67,
      68,    69,    -1,    39,     8,    -1,    -1,    -1,    12,    13,
      14,    15,    16,    -1,    18,    19,    20,    -1,    -1,    55,
      -1,    -1,    58,    59,    60,    -1,    -1,    63,    64,     1,
      66,    67,    68,    69,    -1,    39,     8,    -1,    -1,    11,
      12,    13,    14,    15,    16,    -1,    18,    19,    20,    -1,
      -1,    55,    -1,    -1,    58,    59,    60,    -1,    -1,    63,
      64,    65,     1,    67,    68,    69,    -1,    39,    -1,     8,
      -1,    -1,    -1,    12,    13,    14,    15,    16,    -1,    18,
      19,    20,    -1,    55,    -1,    -1,    58,    59,    60,    -1,
      -1,    63,    64,    -1,     1,    67,    -1,    69,    -1,    -1,
      39,     8,    -1,    -1,    -1,    12,    13,    14,    15,    16,
      -1,    18,    19,    20,    -1,    -1,    55,    -1,    -1,    58,
      59,    60,    -1,    -1,    63,    64,    -1,    -1,    67,    68,
      69,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    58,    59,    60,     1,    -1,    63,    64,     5,    -1,
      67,    -1,    69,    -1,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    24,    -1,    26,
      27,    -1,    -1,    -1,    -1,    -1,    33,     1,    -1,    36,
      37,    38,    39,    -1,     8,    -1,    -1,    -1,    12,    13,
      14,    15,    16,    -1,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    -1,    45,    46,    -1,    65,    49,
      50,    51,    69,    -1,    -1,    39,    56,    57,    58,    59,
      60,    61,    62,    63,    21,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    -1,    -1,    -1,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    41,    69,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    -1,    -1,    -1,
      -1,    -1,    41,    70,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    -1,    -1,    -1,    -1,
      68,    41,    70,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    -1,    -1,    -1,    -1,    68,    41,
      70,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    -1,    65,    -1,    -1,    -1,    41,    70,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      41,    -1,    43,    44,    45,    46,    70,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    41,    -1,    43,    44,    45,    46,    70,
      -1,    49,    50,    51,    -1,    -1,    -1,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    41,    -1,    43,    44,
      45,    46,    70,    -1,    49,    50,    51,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    -1,
      45,    46,    -1,    -1,    -1,    70,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    59,    60,    61,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    70
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,    76,     0,     3,    80,    12,    81,    65,     4,
      82,    83,    15,     9,    17,    77,    84,    85,    96,   107,
     108,    65,    86,    87,    88,    64,    78,    11,    97,   190,
      65,    65,    65,     6,   102,   103,    66,    12,    13,    15,
      91,    79,   109,   110,    63,   191,    91,   104,     7,   102,
      87,    42,    79,    84,    94,    95,    13,    71,    42,   120,
      67,    95,    13,    14,    15,    16,    18,    19,    59,    90,
     192,   193,   194,   195,   196,    10,   190,    12,    20,    93,
      69,   111,    71,     1,     8,    12,    15,    20,    39,    55,
      58,    59,    60,    63,    64,    67,    69,    92,   121,   123,
     128,   129,   130,   132,   133,   138,   144,   183,   186,   189,
     196,    12,   105,   106,    13,    89,    67,   112,    11,    67,
     130,   130,    12,    39,    92,    92,   139,   140,   130,   130,
     190,   134,   120,    21,    41,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    70,    15,    67,    12,
      39,   128,    42,    66,    68,     1,    99,   100,   101,   121,
     187,   188,   190,     1,     5,    12,    24,    26,    27,    33,
      36,    37,    38,    65,    92,    97,   111,   113,   114,   116,
     118,   127,   145,   146,   147,   154,   155,   156,   158,   170,
     175,   180,   183,    67,   184,    71,    15,    16,    18,    19,
     141,   142,   143,   194,    68,    68,    15,   135,   136,   137,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   122,   123,   130,    12,   185,    15,   106,    68,
      66,    66,    91,    65,     1,    67,   161,   167,   168,    72,
      67,   131,   171,    12,    67,    13,   149,   149,   130,   148,
      22,    23,   117,   120,   126,    65,   181,    65,    63,    70,
      73,   174,    65,    68,    99,    66,    71,    40,   174,    66,
     124,    71,    99,    15,    98,   101,   188,   165,   190,   159,
     130,   176,   131,    12,   132,    65,    65,    28,    29,    30,
      31,    32,   119,    97,   180,   130,    12,    68,   182,   142,
      15,   196,   137,    21,   125,   182,   162,    91,    69,   113,
     173,    68,   173,   172,    68,   121,    71,   115,   130,    65,
     166,   169,   180,   177,   173,   157,   120,   120,   130,   120,
      72,   174,    25,   178,    69,    65,   121,   179,    34,    35,
     150,   151,   152,   163,    68,   173,    13,    15,   153,   194,
      72,    73,   150,   180,    92,   116,   127,   160,    21,    72,
     164,    13,    68
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    76,    77,    75,    78,    78,    79,    79,    80,
      80,    81,    81,    82,    83,    83,    83,    83,    84,    84,
      85,    86,    86,    88,    89,    87,    90,    90,    91,    91,
      91,    92,    92,    93,    93,    94,    94,    95,    96,    97,
      97,    98,    98,    99,    99,   100,   100,   101,   102,   102,
     104,   103,   105,   106,   106,   106,   108,   107,   110,   109,
     111,   112,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   114,
     115,   114,   114,   114,   117,   116,   118,   119,   119,   119,
     119,   119,   120,   121,   121,   122,   122,   123,   124,   123,
     125,   125,   126,   126,   127,   128,   128,   128,   129,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     131,   131,   132,   132,   132,   132,   132,   132,   134,   133,
     135,   135,   136,   136,   137,   137,   139,   138,   140,   138,
     141,   141,   142,   142,   143,   143,   143,   143,   144,   144,
     145,   146,   147,   148,   148,   149,   149,   150,   150,   151,
     152,   152,   153,   153,   153,   154,   155,   157,   156,   159,
     158,   160,   160,   162,   163,   164,   161,   165,   166,   165,
     167,   167,   167,   169,   168,   171,   172,   170,   173,   173,
     174,   176,   177,   175,   178,   179,   178,   180,   181,   180,
     182,   184,   183,   185,   183,   186,   186,   186,    99,   187,
     187,   187,   188,   189,   190,   190,   191,   191,   192,   192,
     193,   194,   194,   195,   196,   196,   196,   196
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     0,
       3,     0,     2,     3,     0,     3,     3,     3,     0,     1,
       2,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     6,     2,     3,
       2,     0,     1,     0,     1,     1,     3,     2,     0,     2,
       0,     5,     3,     0,     1,     3,     0,     4,     0,     3,
       4,     0,     3,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       0,     5,     5,     1,     0,     4,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     0,     0,     5,
       0,     2,     1,     1,     2,     1,     1,     1,     4,     1,
       2,     3,     4,     2,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     2,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     1,     1,     3,     3,     3,     0,     3,     0,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     0,     1,     0,     1,     0,     2,     2,
       3,     2,     1,     1,     3,     2,     2,     0,     8,     0,
       4,     1,     1,     0,     0,     0,    10,     0,     0,     4,
       1,     1,     1,     0,     7,     0,     0,     5,     1,     3,
       1,     0,     0,     6,     0,     0,     3,     0,     0,     3,
       1,     0,     5,     0,     5,     2,     1,     1,     1,     1,
       3,     1,     1,     3,     1,     2,     2,     3,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1
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
#line 251 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2150 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 257 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2158 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 267 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2166 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 270 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2175 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 278 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2184 "parser.c"
    break;

  case 18: /* scope: %empty  */
#line 288 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2190 "parser.c"
    break;

  case 19: /* scope: T_STATIC  */
#line 289 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2196 "parser.c"
    break;

  case 23: /* $@3: %empty  */
#line 299 "Parser.y"
                { _bDefiningConst = 1; }
#line 2202 "parser.c"
    break;

  case 24: /* $@4: %empty  */
#line 299 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2208 "parser.c"
    break;

  case 25: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 300 "Parser.y"
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
#line 2224 "parser.c"
    break;

  case 26: /* constant_literal: simple_literal  */
#line 313 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2232 "parser.c"
    break;

  case 27: /* constant_literal: T_STRING  */
#line 316 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2241 "parser.c"
    break;

  case 28: /* variable_identifier: T_IDENT  */
#line 322 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2251 "parser.c"
    break;

  case 29: /* variable_identifier: T_NUM  */
#line 328 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2262 "parser.c"
    break;

  case 30: /* variable_identifier: T_STRING  */
#line 335 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2271 "parser.c"
    break;

  case 31: /* variable_reference: T_VARIABLE  */
#line 341 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2277 "parser.c"
    break;

  case 32: /* variable_reference: T_IDENT  */
#line 342 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2289 "parser.c"
    break;

  case 33: /* macro_declaration: T_IDENT  */
#line 352 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2297 "parser.c"
    break;

  case 34: /* macro_declaration: T_FUNC  */
#line 355 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2307 "parser.c"
    break;

  case 35: /* macro_type: scope T_VOID  */
#line 361 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					strcpy(_nativeMethodSignature, bytecode_signatureForType(VT_NIL));
				}
			}
#line 2320 "parser.c"
    break;

  case 36: /* macro_type: scope type_name  */
#line 370 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					strcpy(_nativeMethodSignature, bytecode_signatureForType(yyvsp[0].ident.type));
				}
			}
#line 2333 "parser.c"
    break;

  case 37: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 380 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2343 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier assignment_expression  */
#line 388 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2355 "parser.c"
    break;

  case 40: /* var_decl: type_name variable_identifier  */
#line 394 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2366 "parser.c"
    break;

  case 42: /* optional_description: T_STRING  */
#line 403 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2375 "parser.c"
    break;

  case 47: /* parameter_declaration: type_name variable_identifier  */
#line 417 "Parser.y"
                                                      {
				if (_bInNativeDefinition) {
					bytecode_nativeMethodAddParam(yyvsp[-1].ident.type, yyvsp[0].ident.s);
				} else {
					sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
					bytecode_defineVariable(_currentBytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER, yyvsp[-1].ident.type,_nparam, _localVariableIndex++);
				}
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2390 "parser.c"
    break;

  case 50: /* $@5: %empty  */
#line 431 "Parser.y"
                         {
				if (strcmp(yyvsp[0].ident.s, "@Method") == 0) {
					_currentNativeMethodAnnotation = hashmap_create(3, 0 ,0);
				}
				free(yyvsp[0].ident.s);
			}
#line 2401 "parser.c"
    break;

  case 52: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 438 "Parser.y"
                                            {
				if (_currentNativeMethodAnnotation) {
					hashmap_put(_currentNativeMethodAnnotation, (intptr_t)yyvsp[-2].ident.s, (intptr_t)yyvsp[0].ident.s);
				} else {
					free(yyvsp[-2].ident.s);
					free(yyvsp[0].ident.s);
				}
			}
#line 2414 "parser.c"
    break;

  case 56: /* $@6: %empty  */
#line 451 "Parser.y"
                          {
					_bInNativeDefinition = 1;
				}
#line 2422 "parser.c"
    break;

  case 57: /* native_macro_definition: $@6 annotations T_NATIVE macrostart  */
#line 454 "Parser.y"
                                                                {
					if (!_currentNativeMethodAnnotation) {
						_currentNativeMethodAnnotation = hashmap_create(3, 0, 0);
					}
					char* pszLib = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"lib");
					char* pszMacroCName = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"name");
					char* pszSignature = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"signature");
					if (!pszSignature) {
						pszSignature = _nativeMethodSignature;
					}
					if (!pszMacroCName) {
						pszMacroCName = yyvsp[0].ident.s;
					}
					_bInNativeDefinition = 0;
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszMacroCName});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
					if (!pszLib) {
						pszLib = "PKSEDIT";
					}
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszLib});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszSignature});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=_yyCurrentComment});
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
							C_0FUNC, (GENERIC_DATA){ FUNC_RegisterNative}, 5);
					// register right away as well, so we can use it.
					function_registerNativeFunction(pszMacroCName, yyvsp[0].ident.s, pszLib, pszSignature, _yyCurrentComment);
					free(yyvsp[0].ident.s);
					hashmap_destroyWithKeysAndValues(_currentNativeMethodAnnotation);
				}
#line 2456 "parser.c"
    break;

  case 58: /* $@7: %empty  */
#line 484 "Parser.y"
                  { init_macroDefinition(); }
#line 2462 "parser.c"
    break;

  case 59: /* macro_definition: $@7 macrostart block  */
#line 485 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				bytecode_optimizeInstructions(_currentBytecodeBuffer);
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
#line 2490 "parser.c"
    break;

  case 64: /* statement: call_expression ';'  */
#line 515 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2496 "parser.c"
    break;

  case 65: /* statement: assignment ';'  */
#line 516 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2502 "parser.c"
    break;

  case 66: /* statement: if_expression  */
#line 517 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2508 "parser.c"
    break;

  case 67: /* statement: while  */
#line 518 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2514 "parser.c"
    break;

  case 68: /* statement: for_loop_expression  */
#line 519 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2520 "parser.c"
    break;

  case 69: /* statement: switch_expression  */
#line 520 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2526 "parser.c"
    break;

  case 70: /* statement: break  */
#line 521 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2532 "parser.c"
    break;

  case 71: /* statement: continue  */
#line 522 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2538 "parser.c"
    break;

  case 72: /* statement: return_expression  */
#line 523 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2544 "parser.c"
    break;

  case 73: /* statement: goto  */
#line 524 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2550 "parser.c"
    break;

  case 74: /* statement: block  */
#line 525 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2556 "parser.c"
    break;

  case 75: /* statement: increment_expression  */
#line 526 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2562 "parser.c"
    break;

  case 76: /* statement: label  */
#line 527 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2568 "parser.c"
    break;

  case 77: /* statement: var_decl  */
#line 528 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2576 "parser.c"
    break;

  case 78: /* statement: error ';'  */
#line 531 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2586 "parser.c"
    break;

  case 79: /* assignment: variable_reference assignment_expression  */
#line 537 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2594 "parser.c"
    break;

  case 80: /* $@8: %empty  */
#line 541 "Parser.y"
                                                      {
				types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2602 "parser.c"
    break;

  case 81: /* assignment: assignment_target '.' T_IDENT $@8 assignment_expression  */
#line 543 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2610 "parser.c"
    break;

  case 82: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 547 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2618 "parser.c"
    break;

  case 84: /* $@9: %empty  */
#line 553 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2626 "parser.c"
    break;

  case 85: /* shorthand_assignment: variable_reference $@9 shorthand_assignment_operator simple_expression  */
#line 555 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2635 "parser.c"
    break;

  case 86: /* assignment_target: variable_reference  */
#line 560 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2643 "parser.c"
    break;

  case 87: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 565 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2649 "parser.c"
    break;

  case 88: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 566 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2655 "parser.c"
    break;

  case 89: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 567 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2661 "parser.c"
    break;

  case 90: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 568 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2667 "parser.c"
    break;

  case 91: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 569 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2673 "parser.c"
    break;

  case 92: /* assignment_expression: T_ASSIGN simple_expression  */
#line 571 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2679 "parser.c"
    break;

  case 93: /* simple_expression: binary_expression  */
#line 573 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2685 "parser.c"
    break;

  case 98: /* $@10: %empty  */
#line 581 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2691 "parser.c"
    break;

  case 101: /* range_increment: T_DOTDOT binary_expression  */
#line 584 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2697 "parser.c"
    break;

  case 102: /* minusminus_plusplus: T_MINUSMINUS  */
#line 587 "Parser.y"
                                     { yyval.num = -1; }
#line 2703 "parser.c"
    break;

  case 103: /* minusminus_plusplus: T_PLUSPLUS  */
#line 588 "Parser.y"
                                     { yyval.num = 1; }
#line 2709 "parser.c"
    break;

  case 104: /* increment_expression: variable_reference minusminus_plusplus  */
#line 591 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2717 "parser.c"
    break;

  case 105: /* s_bterm: value  */
#line 595 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2723 "parser.c"
    break;

  case 108: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 599 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2731 "parser.c"
    break;

  case 109: /* binary_expression: s_bterm  */
#line 604 "Parser.y"
                                { yyval.ident.type = yyvsp[0].ident.type;}
#line 2737 "parser.c"
    break;

  case 110: /* binary_expression: type_cast s_bterm  */
#line 605 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2746 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression '.' T_IDENT  */
#line 610 "Parser.y"
                                                      {
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2755 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 614 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2761 "parser.c"
    break;

  case 113: /* binary_expression: '!' binary_expression  */
#line 615 "Parser.y"
                                                { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2767 "parser.c"
    break;

  case 114: /* binary_expression: '~' binary_expression  */
#line 616 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2773 "parser.c"
    break;

  case 115: /* binary_expression: '+' binary_expression  */
#line 617 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2779 "parser.c"
    break;

  case 116: /* binary_expression: '-' variable_reference  */
#line 618 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2785 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression '~' binary_expression  */
#line 619 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2791 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 620 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2797 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression '&' binary_expression  */
#line 621 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2803 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression '|' binary_expression  */
#line 622 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2809 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression '+' binary_expression  */
#line 623 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2815 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '^' binary_expression  */
#line 624 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2821 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression '-' binary_expression  */
#line 625 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2827 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression '*' binary_expression  */
#line 626 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2833 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression '/' binary_expression  */
#line 627 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2839 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression '%' binary_expression  */
#line 628 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2845 "parser.c"
    break;

  case 127: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 629 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2851 "parser.c"
    break;

  case 128: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 630 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2857 "parser.c"
    break;

  case 129: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 631 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2863 "parser.c"
    break;

  case 130: /* binary_expression: binary_expression T_AND binary_expression  */
#line 632 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2869 "parser.c"
    break;

  case 131: /* binary_expression: binary_expression T_OR binary_expression  */
#line 633 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2875 "parser.c"
    break;

  case 132: /* binary_expression: binary_expression '<' binary_expression  */
#line 634 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2881 "parser.c"
    break;

  case 133: /* binary_expression: binary_expression '>' binary_expression  */
#line 635 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2887 "parser.c"
    break;

  case 134: /* binary_expression: binary_expression T_LE binary_expression  */
#line 636 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2893 "parser.c"
    break;

  case 135: /* binary_expression: binary_expression T_GE binary_expression  */
#line 637 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2899 "parser.c"
    break;

  case 136: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 638 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2905 "parser.c"
    break;

  case 137: /* binary_expression: binary_expression T_NE binary_expression  */
#line 639 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2911 "parser.c"
    break;

  case 138: /* binary_expression: '(' binary_expression ')'  */
#line 640 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2917 "parser.c"
    break;

  case 139: /* binary_expression: variable_reference assignment_expression  */
#line 641 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2925 "parser.c"
    break;

  case 141: /* condition: '(' binary_expression  */
#line 648 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2933 "parser.c"
    break;

  case 142: /* value: T_VARIABLE  */
#line 652 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2942 "parser.c"
    break;

  case 143: /* value: string  */
#line 656 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2952 "parser.c"
    break;

  case 144: /* value: simple_literal  */
#line 661 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2958 "parser.c"
    break;

  case 145: /* value: map_literal  */
#line 662 "Parser.y"
                                      {
				yyval.ident.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2969 "parser.c"
    break;

  case 146: /* value: array_literal  */
#line 668 "Parser.y"
                                        {
				yyval.ident.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2980 "parser.c"
    break;

  case 147: /* value: T_IDENT  */
#line 674 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2990 "parser.c"
    break;

  case 148: /* $@11: %empty  */
#line 680 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
				// Hack - avoid push instruction being generated for map literal elements.
				_bDefiningConst = 1;
			}
#line 3000 "parser.c"
    break;

  case 149: /* map_literal: '{' $@11 optional_map_associates closing_brace  */
#line 684 "Parser.y"
                                                                {
				_bDefiningConst = 0;
			}
#line 3008 "parser.c"
    break;

  case 154: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 698 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 3017 "parser.c"
    break;

  case 155: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 702 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 3026 "parser.c"
    break;

  case 156: /* $@12: %empty  */
#line 707 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 3034 "parser.c"
    break;

  case 158: /* $@13: %empty  */
#line 711 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 3042 "parser.c"
    break;

  case 162: /* array_element: simple_array_element  */
#line 720 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 3050 "parser.c"
    break;

  case 163: /* array_element: T_STRING  */
#line 724 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 3058 "parser.c"
    break;

  case 164: /* simple_array_element: integer_literal  */
#line 729 "Parser.y"
                                        { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].v.data.longValue; }
#line 3064 "parser.c"
    break;

  case 165: /* simple_array_element: T_CHARACTER  */
#line 730 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 3070 "parser.c"
    break;

  case 166: /* simple_array_element: T_TRUE  */
#line 731 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 3076 "parser.c"
    break;

  case 167: /* simple_array_element: T_FALSE  */
#line 732 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 3082 "parser.c"
    break;

  case 168: /* string: T_STRING  */
#line 734 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 3088 "parser.c"
    break;

  case 169: /* string: string T_STRING  */
#line 735 "Parser.y"
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
#line 3103 "parser.c"
    break;

  case 170: /* break: T_BREAK opt_num ';'  */
#line 747 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3109 "parser.c"
    break;

  case 171: /* continue: T_CONTINUE opt_num ';'  */
#line 750 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3115 "parser.c"
    break;

  case 172: /* return_expression: T_RETURN optional_bterm  */
#line 753 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3123 "parser.c"
    break;

  case 175: /* opt_num: %empty  */
#line 760 "Parser.y"
                {	yyval.num = 1;	}
#line 3129 "parser.c"
    break;

  case 176: /* opt_num: T_NUM  */
#line 761 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3135 "parser.c"
    break;

  case 181: /* case_selector: T_DEFAULT ':'  */
#line 770 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3143 "parser.c"
    break;

  case 182: /* case_condition: integer_literal  */
#line 775 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3151 "parser.c"
    break;

  case 183: /* case_condition: T_STRING  */
#line 778 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3159 "parser.c"
    break;

  case 184: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 781 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3167 "parser.c"
    break;

  case 185: /* label: T_IDENT ':'  */
#line 785 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3176 "parser.c"
    break;

  case 186: /* goto: T_GOTO T_IDENT  */
#line 790 "Parser.y"
                               {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3185 "parser.c"
    break;

  case 187: /* $@14: %empty  */
#line 797 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3195 "parser.c"
    break;

  case 188: /* switch_expression: T_SWITCH '(' value ')' $@14 '{' case_clauses '}'  */
#line 803 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3208 "parser.c"
    break;

  case 189: /* $@15: %empty  */
#line 813 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3217 "parser.c"
    break;

  case 190: /* for_loop_expression: T_FOR for_clause $@15 local_block  */
#line 816 "Parser.y"
                                      {
				parser_flushBuffer(_breaklevel);
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3230 "parser.c"
    break;

  case 193: /* $@16: %empty  */
#line 828 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3238 "parser.c"
    break;

  case 194: /* $@17: %empty  */
#line 830 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3246 "parser.c"
    break;

  case 195: /* $@18: %empty  */
#line 832 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3255 "parser.c"
    break;

  case 198: /* $@19: %empty  */
#line 838 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				free(yyvsp[0].ident.s);
			}
#line 3264 "parser.c"
    break;

  case 202: /* for_clause: error  */
#line 845 "Parser.y"
                                {
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3273 "parser.c"
    break;

  case 203: /* $@20: %empty  */
#line 850 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				push_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 3287 "parser.c"
    break;

  case 204: /* in_clause: '(' type_name variable_identifier $@20 ':' simple_expression ')'  */
#line 858 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 3298 "parser.c"
    break;

  case 205: /* $@21: %empty  */
#line 865 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3306 "parser.c"
    break;

  case 206: /* $@22: %empty  */
#line 867 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3315 "parser.c"
    break;

  case 207: /* while: T_WHILE $@21 condition $@22 local_block  */
#line 870 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3327 "parser.c"
    break;

  case 211: /* $@23: %empty  */
#line 883 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3336 "parser.c"
    break;

  case 212: /* $@24: %empty  */
#line 886 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3344 "parser.c"
    break;

  case 213: /* if_expression: T_IF condition $@23 local_block $@24 else_clause  */
#line 888 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3353 "parser.c"
    break;

  case 214: /* else_clause: %empty  */
#line 893 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3362 "parser.c"
    break;

  case 215: /* $@25: %empty  */
#line 897 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3372 "parser.c"
    break;

  case 217: /* stmntlist: %empty  */
#line 904 "Parser.y"
                { yyval.needsPop = 0; }
#line 3378 "parser.c"
    break;

  case 218: /* $@26: %empty  */
#line 905 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3388 "parser.c"
    break;

  case 221: /* @27: %empty  */
#line 913 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3400 "parser.c"
    break;

  case 222: /* call_expression: T_FUNC '(' @27 parameter_list closing_paren  */
#line 919 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3410 "parser.c"
    break;

  case 223: /* @28: %empty  */
#line 924 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3421 "parser.c"
    break;

  case 224: /* call_expression: function_id_or_pointer '(' @28 parameter_list closing_paren  */
#line 929 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3435 "parser.c"
    break;

  case 225: /* function_id_or_pointer: '*' variable_reference  */
#line 940 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3449 "parser.c"
    break;

  case 226: /* function_id_or_pointer: T_IDENT  */
#line 950 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3458 "parser.c"
    break;

  case 227: /* function_id_or_pointer: error  */
#line 954 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3468 "parser.c"
    break;

  case 231: /* nonempty_parameters: error  */
#line 964 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3476 "parser.c"
    break;

  case 232: /* parameter: simple_expression  */
#line 968 "Parser.y"
                                  { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3482 "parser.c"
    break;

  case 233: /* type_cast: '(' type_name ')'  */
#line 970 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3491 "parser.c"
    break;

  case 234: /* type_name: T_TYPE_IDENTIFIER  */
#line 975 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3499 "parser.c"
    break;

  case 235: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 978 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3508 "parser.c"
    break;

  case 237: /* array_size: '[' T_NUM ']'  */
#line 984 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3516 "parser.c"
    break;

  case 238: /* boolean_literal: T_TRUE  */
#line 988 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3525 "parser.c"
    break;

  case 239: /* boolean_literal: T_FALSE  */
#line 991 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3534 "parser.c"
    break;

  case 240: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 996 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3543 "parser.c"
    break;

  case 241: /* integer_literal: T_NUM  */
#line 1001 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3552 "parser.c"
    break;

  case 242: /* integer_literal: '-' T_NUM  */
#line 1005 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3562 "parser.c"
    break;

  case 243: /* character_literal: T_CHARACTER  */
#line 1012 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3571 "parser.c"
    break;

  case 244: /* simple_literal: integer_literal  */
#line 1017 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3584 "parser.c"
    break;

  case 245: /* simple_literal: float_literal  */
#line 1025 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3597 "parser.c"
    break;

  case 246: /* simple_literal: character_literal  */
#line 1033 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3610 "parser.c"
    break;

  case 247: /* simple_literal: boolean_literal  */
#line 1041 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3623 "parser.c"
    break;


#line 3627 "parser.c"

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

#line 1049 "Parser.y"


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

