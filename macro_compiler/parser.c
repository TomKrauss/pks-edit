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
#include "stringutil.h"
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
				_iflevel;
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

extern 			int	_bInNativeDefinition;
extern 			int	_bDefiningConst;
extern 			int	_bInHeader;

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
static ARRAY_LIST*	_currentArrayLiteral;
static HASHMAP*		_currentNativeMethodAnnotation;
static int			_localVariableIndex;
static STRING_BUF*  _nativeMethodSignature;
static STRING_BUF*	_nativeMethodArgs;
IDENTIFIER_CONTEXT* _currentIdentifierContext;

static char* parser_signatureForType(PKS_VALUE_TYPE type) {
	char* pSig = "i";
	if (type == VT_STRING) {
		pSig = "s";
	} else if (type == VT_EDITOR_HANDLE) {
		pSig = "W";
	}
	return pSig;
}

static void parser_startNativeMethod(PKS_VALUE_TYPE t) {
	_nativeMethodSignature = stringbuf_create(128);
	_nativeMethodArgs = stringbuf_create(128);
	stringbuf_appendString(_nativeMethodSignature, parser_signatureForType(t));
}

static void parser_nativeMethodAddParam(PKS_VALUE_TYPE type, char* pszParamName) {
	if (strcmp("...", pszParamName) == 0) {
		stringbuf_appendString(_nativeMethodArgs, pszParamName);
	} else {
		stringbuf_appendString(_nativeMethodSignature, parser_signatureForType(type));
		stringbuf_appendString(_nativeMethodArgs, (char*)types_nameFor(type));
		stringbuf_appendChar(_nativeMethodArgs,' ');
		stringbuf_appendString(_nativeMethodArgs, pszParamName);
	}
	stringbuf_appendChar(_nativeMethodArgs,',');
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

static void parser_newForeachCursor() {
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex++});
}

static PKS_VALUE_TYPE parser_pushFieldIndexWithError(PKS_VALUE_TYPE t, const char* pszFieldName) {
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


#line 309 "parser.c"

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
  YYSYMBOL_T_ELLIPSIS = 12,                /* T_ELLIPSIS  */
  YYSYMBOL_T_IDENT = 13,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 14,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 15,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 16,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 17,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 18,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 19,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 20,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 21,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 22,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 23,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 24,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 25,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 26,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 27,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 28,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 29,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 30,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 31,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 32,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 33,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 34,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 35,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 36,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 37,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 38,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 39,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 40,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 41,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 42,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 43,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 44,                      /* T_NE  */
  YYSYMBOL_T_EQ = 45,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 46,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 47,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 48,                      /* T_OR  */
  YYSYMBOL_T_AND = 49,                     /* T_AND  */
  YYSYMBOL_T_LE = 50,                      /* T_LE  */
  YYSYMBOL_T_GE = 51,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 52,                /* T_POWER_TO  */
  YYSYMBOL_53_ = 53,                       /* '&'  */
  YYSYMBOL_54_ = 54,                       /* '|'  */
  YYSYMBOL_55_ = 55,                       /* '^'  */
  YYSYMBOL_56_ = 56,                       /* '~'  */
  YYSYMBOL_57_ = 57,                       /* '<'  */
  YYSYMBOL_58_ = 58,                       /* '>'  */
  YYSYMBOL_59_ = 59,                       /* '+'  */
  YYSYMBOL_60_ = 60,                       /* '-'  */
  YYSYMBOL_61_ = 61,                       /* '*'  */
  YYSYMBOL_62_ = 62,                       /* '/'  */
  YYSYMBOL_63_ = 63,                       /* '%'  */
  YYSYMBOL_64_ = 64,                       /* '['  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_66_ = 66,                       /* ';'  */
  YYSYMBOL_67_ = 67,                       /* ','  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ')'  */
  YYSYMBOL_70_ = 70,                       /* '{'  */
  YYSYMBOL_71_ = 71,                       /* '.'  */
  YYSYMBOL_72_ = 72,                       /* ']'  */
  YYSYMBOL_73_ = 73,                       /* ':'  */
  YYSYMBOL_74_ = 74,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 75,                  /* $accept  */
  YYSYMBOL_file_structure = 76,            /* file_structure  */
  YYSYMBOL_77_1 = 77,                      /* $@1  */
  YYSYMBOL_78_2 = 78,                      /* $@2  */
  YYSYMBOL_header_delimiter = 79,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 80,         /* macro_definitions  */
  YYSYMBOL_namespace = 81,                 /* namespace  */
  YYSYMBOL_prerequisites = 82,             /* prerequisites  */
  YYSYMBOL_require = 83,                   /* require  */
  YYSYMBOL_global_declarations = 84,       /* global_declarations  */
  YYSYMBOL_scope = 85,                     /* scope  */
  YYSYMBOL_constants = 86,                 /* constants  */
  YYSYMBOL_constant_list = 87,             /* constant_list  */
  YYSYMBOL_constdef = 88,                  /* constdef  */
  YYSYMBOL_89_3 = 89,                      /* $@3  */
  YYSYMBOL_90_4 = 90,                      /* $@4  */
  YYSYMBOL_constant_literal = 91,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 92,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 93,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 94,         /* macro_declaration  */
  YYSYMBOL_macro_type = 95,                /* macro_type  */
  YYSYMBOL_macrostart = 96,                /* macrostart  */
  YYSYMBOL_global_var = 97,                /* global_var  */
  YYSYMBOL_var_decl = 98,                  /* var_decl  */
  YYSYMBOL_optional_description = 99,      /* optional_description  */
  YYSYMBOL_parameter_list = 100,           /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 101,        /* non_empty_pardecl  */
  YYSYMBOL_parameter_declaration = 102,    /* parameter_declaration  */
  YYSYMBOL_annotations = 103,              /* annotations  */
  YYSYMBOL_annotation = 104,               /* annotation  */
  YYSYMBOL_105_5 = 105,                    /* $@5  */
  YYSYMBOL_annotation_value = 106,         /* annotation_value  */
  YYSYMBOL_annotation_values = 107,        /* annotation_values  */
  YYSYMBOL_native_macro_definition = 108,  /* native_macro_definition  */
  YYSYMBOL_109_6 = 109,                    /* $@6  */
  YYSYMBOL_macro_definition = 110,         /* macro_definition  */
  YYSYMBOL_111_7 = 111,                    /* $@7  */
  YYSYMBOL_block = 112,                    /* block  */
  YYSYMBOL_var_decls = 113,                /* var_decls  */
  YYSYMBOL_statement = 114,                /* statement  */
  YYSYMBOL_assignment = 115,               /* assignment  */
  YYSYMBOL_116_8 = 116,                    /* $@8  */
  YYSYMBOL_shorthand_assignment = 117,     /* shorthand_assignment  */
  YYSYMBOL_118_9 = 118,                    /* $@9  */
  YYSYMBOL_assignment_target = 119,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 120, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 121,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 122,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 123, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 124,         /* range_expression  */
  YYSYMBOL_125_10 = 125,                   /* $@10  */
  YYSYMBOL_range_increment = 126,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 127,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 128,     /* increment_expression  */
  YYSYMBOL_s_bterm = 129,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 130,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 131,        /* binary_expression  */
  YYSYMBOL_condition = 132,                /* condition  */
  YYSYMBOL_value = 133,                    /* value  */
  YYSYMBOL_map_literal = 134,              /* map_literal  */
  YYSYMBOL_135_11 = 135,                   /* $@11  */
  YYSYMBOL_optional_map_associates = 136,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 137,           /* map_associates  */
  YYSYMBOL_map_associate = 138,            /* map_associate  */
  YYSYMBOL_array_literal = 139,            /* array_literal  */
  YYSYMBOL_140_12 = 140,                   /* $@12  */
  YYSYMBOL_141_13 = 141,                   /* $@13  */
  YYSYMBOL_array_elements = 142,           /* array_elements  */
  YYSYMBOL_array_element = 143,            /* array_element  */
  YYSYMBOL_simple_array_element = 144,     /* simple_array_element  */
  YYSYMBOL_string = 145,                   /* string  */
  YYSYMBOL_break = 146,                    /* break  */
  YYSYMBOL_continue = 147,                 /* continue  */
  YYSYMBOL_return_expression = 148,        /* return_expression  */
  YYSYMBOL_optional_bterm = 149,           /* optional_bterm  */
  YYSYMBOL_opt_num = 150,                  /* opt_num  */
  YYSYMBOL_case_clauses = 151,             /* case_clauses  */
  YYSYMBOL_case_clause = 152,              /* case_clause  */
  YYSYMBOL_case_selector = 153,            /* case_selector  */
  YYSYMBOL_case_condition = 154,           /* case_condition  */
  YYSYMBOL_label = 155,                    /* label  */
  YYSYMBOL_goto = 156,                     /* goto  */
  YYSYMBOL_switch_expression = 157,        /* switch_expression  */
  YYSYMBOL_158_14 = 158,                   /* $@14  */
  YYSYMBOL_for_loop_expression = 159,      /* for_loop_expression  */
  YYSYMBOL_160_15 = 160,                   /* $@15  */
  YYSYMBOL_for_increment = 161,            /* for_increment  */
  YYSYMBOL_for_to_clause = 162,            /* for_to_clause  */
  YYSYMBOL_163_16 = 163,                   /* $@16  */
  YYSYMBOL_164_17 = 164,                   /* $@17  */
  YYSYMBOL_165_18 = 165,                   /* $@18  */
  YYSYMBOL_opt_for_initializer = 166,      /* opt_for_initializer  */
  YYSYMBOL_167_19 = 167,                   /* $@19  */
  YYSYMBOL_for_clause = 168,               /* for_clause  */
  YYSYMBOL_in_clause = 169,                /* in_clause  */
  YYSYMBOL_170_20 = 170,                   /* $@20  */
  YYSYMBOL_while = 171,                    /* while  */
  YYSYMBOL_172_21 = 172,                   /* $@21  */
  YYSYMBOL_173_22 = 173,                   /* $@22  */
  YYSYMBOL_local_block = 174,              /* local_block  */
  YYSYMBOL_closing_brace = 175,            /* closing_brace  */
  YYSYMBOL_if_expression = 176,            /* if_expression  */
  YYSYMBOL_177_23 = 177,                   /* $@23  */
  YYSYMBOL_178_24 = 178,                   /* $@24  */
  YYSYMBOL_else_clause = 179,              /* else_clause  */
  YYSYMBOL_180_25 = 180,                   /* $@25  */
  YYSYMBOL_stmntlist = 181,                /* stmntlist  */
  YYSYMBOL_182_26 = 182,                   /* $@26  */
  YYSYMBOL_closing_paren = 183,            /* closing_paren  */
  YYSYMBOL_call_expression = 184,          /* call_expression  */
  YYSYMBOL_185_27 = 185,                   /* @27  */
  YYSYMBOL_186_28 = 186,                   /* @28  */
  YYSYMBOL_function_id_or_pointer = 187,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 188,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 189,                /* parameter  */
  YYSYMBOL_type_cast = 190,                /* type_cast  */
  YYSYMBOL_type_name = 191,                /* type_name  */
  YYSYMBOL_array_size = 192,               /* array_size  */
  YYSYMBOL_boolean_literal = 193,          /* boolean_literal  */
  YYSYMBOL_float_literal = 194,            /* float_literal  */
  YYSYMBOL_integer_literal = 195,          /* integer_literal  */
  YYSYMBOL_character_literal = 196,        /* character_literal  */
  YYSYMBOL_simple_literal = 197            /* simple_literal  */
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
#define YYLAST   1066

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  123
/* YYNRULES -- Number of rules.  */
#define YYNRULES  248
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  384

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


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
       2,     2,     2,    65,     2,     2,     2,    63,    53,     2,
      68,    69,    61,    59,    67,    60,    71,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    66,
      57,     2,    58,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    72,    55,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    54,    74,    56,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   267,   267,   273,   267,   277,   278,   280,   281,   283,
     286,   291,   292,   294,   299,   300,   301,   302,   304,   305,
     309,   312,   313,   315,   315,   315,   329,   332,   338,   343,
     350,   357,   358,   368,   371,   377,   386,   395,   402,   404,
     410,   417,   419,   424,   425,   428,   430,   433,   442,   448,
     449,   451,   451,   458,   467,   468,   469,   471,   471,   510,
     510,   535,   537,   538,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   557,
     563,   567,   567,   573,   576,   579,   579,   586,   591,   592,
     593,   594,   595,   597,   599,   600,   603,   604,   606,   607,
     607,   609,   610,   613,   614,   617,   621,   622,   623,   625,
     630,   631,   636,   640,   641,   642,   643,   644,   645,   646,
     647,   648,   649,   650,   651,   652,   653,   654,   655,   656,
     657,   658,   659,   660,   661,   662,   663,   664,   665,   666,
     667,   673,   674,   678,   682,   687,   688,   694,   700,   706,
     706,   714,   716,   719,   721,   724,   728,   733,   733,   737,
     737,   742,   743,   746,   750,   755,   756,   757,   758,   760,
     761,   772,   775,   779,   783,   784,   786,   787,   789,   790,
     792,   794,   796,   801,   804,   807,   811,   816,   823,   822,
     839,   839,   851,   852,   854,   856,   858,   854,   863,   864,
     864,   869,   870,   871,   876,   876,   891,   893,   891,   904,
     904,   906,   909,   912,   909,   919,   923,   923,   930,   931,
     931,   937,   939,   939,   950,   950,   966,   976,   980,   986,
     988,   989,   990,   994,   996,  1001,  1004,  1009,  1010,  1014,
    1017,  1022,  1027,  1031,  1038,  1043,  1051,  1059,  1067
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
  "T_TYPE_IDENTIFIER", "T_ELLIPSIS", "T_IDENT", "T_NUM",
  "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
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

#define YYPACT_NINF (-263)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-229)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -263,    37,    43,  -263,    30,  -263,    33,    97,  -263,   107,
    -263,    70,    61,  -263,  -263,    65,   122,    71,    75,    77,
     139,  -263,    83,  -263,   112,  -263,   148,    92,  -263,   112,
    -263,  -263,  -263,  -263,   150,   139,  -263,  -263,  -263,  -263,
     116,  -263,   148,   154,     1,  -263,   119,    96,   154,  -263,
    -263,    15,  -263,   100,    68,    95,   102,  -263,   275,  -263,
     155,  -263,  -263,  -263,  -263,  -263,  -263,  -263,   162,  -263,
    -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,   109,
    -263,  -263,  -263,  -263,   167,   -23,  -263,   115,   143,   636,
     636,     4,    74,   118,   636,   602,  -263,   119,  -263,  -263,
    -263,  -263,   761,  -263,  -263,  -263,   172,  -263,   134,   725,
    -263,   149,   137,   140,  -263,  -263,   502,   133,   138,  -263,
     995,   153,  -263,  -263,  -263,  -263,   136,   165,   904,   820,
     142,   197,  -263,   636,   636,   636,   636,   636,   636,   636,
     636,   636,   636,   636,   636,   636,   636,   636,   636,   636,
     636,   636,   636,   636,   636,   437,   206,  -263,  -263,   152,
    -263,  -263,   205,   155,  -263,   159,  -263,   160,   161,  -263,
    -263,   163,  -263,   112,   171,    12,    22,   175,  -263,   227,
     177,   232,   232,   347,  -263,    60,   183,  -263,  -263,   184,
    -263,    29,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,  -263,   179,   185,   189,   502,  -263,  -263,  -263,  -263,
    -263,    40,  -263,  -263,  -263,  -263,  -263,   218,   179,   194,
    -263,   904,   995,   950,   950,   -38,   -38,   927,   927,   709,
     709,    58,   973,   973,   973,   995,   709,   709,   153,   153,
      58,    58,    58,   190,  -263,   761,  -263,   502,  -263,  -263,
     247,   124,   519,  -263,  -263,  -263,   122,  -263,  -263,  -263,
    -263,   636,  -263,   175,  -263,   734,  -263,   198,   199,   904,
    -263,  -263,  -263,   203,  -263,  -263,  -263,    81,  -263,   636,
     253,  -263,  -263,  -263,  -263,   200,   165,  -263,   181,  -263,
     197,   245,  -263,   200,  -263,  -263,  -263,  -263,  -263,   112,
     697,   848,   697,  -263,  -263,   201,  -263,  -263,  -263,  -263,
    -263,  -263,  -263,   536,  -263,  -263,   789,  -263,  -263,  -263,
    -263,  -263,  -263,  -263,   636,  -263,  -263,   211,   208,   409,
    -263,  -263,  -263,  -263,   697,  -263,  -263,   119,   119,   207,
     636,   119,   209,   179,   246,  -263,   214,  -263,  -263,   876,
    -263,   619,  -263,  -263,  -263,   104,  -263,   202,   697,    25,
     212,   213,   104,    81,    74,  -263,  -263,   257,  -263,   225,
    -263,  -263,  -263,  -263,  -263,   129,  -263,  -263,  -263,   285,
    -263,   235,  -263,  -263
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    19,    23,     5,     0,     0,     0,     0,
      49,    13,    20,    21,     0,     6,    59,   235,    38,     0,
      15,    16,    17,    51,     0,    49,    23,    28,    29,    30,
       0,     4,    59,    18,     0,   236,    40,     0,    18,    50,
      22,     0,     8,     0,     0,     0,     0,   237,     0,    39,
      54,    58,   242,   241,    27,   244,   239,   240,     0,    24,
     248,   246,   245,   247,    26,    35,    36,    33,    34,     0,
      62,    60,   238,   228,     0,   148,   169,     0,   143,     0,
       0,     0,     0,   159,     0,     0,   149,     0,    93,    95,
     110,   108,    94,   106,   146,   147,   144,   107,     0,     0,
     145,     0,    55,     0,   243,    25,     0,     0,     0,   222,
     115,   116,    32,    31,   117,   226,     0,     0,   114,     0,
       0,   151,   140,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,   224,   148,
     143,   111,     0,    54,    52,   232,    48,     0,    44,    45,
     233,   229,   230,     0,   228,     0,    32,     0,   206,     0,
       0,   176,   176,     0,    64,    85,    78,    75,   219,     0,
      84,     0,    76,    71,    72,    73,    77,    74,    70,    69,
      68,    67,     0,     0,     0,     0,   158,   164,   166,   167,
     168,     0,   162,   163,   165,   139,   234,     0,     0,   152,
     153,    99,   119,   138,   137,   128,   129,   132,   131,   135,
     136,   130,   120,   121,   123,   118,   133,   134,   122,   124,
     125,   126,   127,     0,    97,    96,   112,     0,    53,    56,
      41,     0,     0,    47,    79,   203,   198,   201,   190,   202,
     186,     0,   212,     0,   187,     0,   177,     0,     0,   175,
     173,   104,   103,     0,    80,   105,    63,     0,    66,     0,
       0,   211,    61,    65,   109,     0,     0,   160,     0,   150,
       0,   101,   113,     0,    42,    37,    46,   231,   194,     0,
       0,   142,     0,   207,   148,     0,   171,   172,    91,    92,
      88,    89,    90,     0,    78,   220,     0,    81,   221,   223,
     161,   156,   155,   154,     0,   100,   225,     0,   199,     0,
     209,   191,   141,   213,     0,   188,    86,     0,     0,   102,
       0,     0,     0,     0,   215,   208,     0,    83,    82,     0,
     200,     0,   210,   216,   214,   178,   195,     0,     0,     0,
       0,     0,   178,     0,     0,   205,   217,   242,   184,     0,
     183,   182,   189,   179,   180,    85,   193,   192,   196,     0,
     181,     0,   185,   197
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -263,  -263,  -263,  -263,  -263,   263,  -263,  -263,  -263,  -263,
     295,  -263,  -263,   271,  -263,  -263,  -263,   -24,   -90,  -263,
    -263,   260,  -263,    -4,  -263,  -169,  -263,    67,   281,  -263,
    -263,  -263,   187,  -263,  -263,  -263,  -263,   296,  -263,  -262,
    -263,  -263,   -11,  -263,  -263,  -263,   -81,   -58,  -263,   204,
    -263,  -263,  -263,   -10,   248,  -263,   -86,    93,   105,  -263,
    -263,  -263,  -263,    79,  -263,  -263,  -263,  -263,    85,  -263,
    -263,  -263,  -263,  -263,  -263,   195,     3,  -263,  -263,  -263,
    -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -260,
    -211,  -263,  -263,  -263,  -263,  -263,  -258,  -263,    86,  -111,
    -263,  -263,  -263,  -263,   126,  -263,   -25,  -263,  -263,  -263,
    -117,  -263,   -40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    26,    41,     5,     7,    10,    11,
      53,    17,    22,    23,    24,   115,    69,    40,    97,    79,
      54,    55,    18,   314,   295,   167,   168,   169,    34,    35,
      47,   112,   113,    19,    20,    42,    43,   187,   117,   188,
     189,   338,   190,   273,   191,   313,    59,   170,   243,    99,
     291,   325,   275,   192,   100,   101,   102,   262,   103,   104,
     131,   218,   219,   220,   105,   126,   127,   211,   212,   213,
     106,   193,   194,   195,   270,   267,   361,   362,   363,   369,
     196,   197,   198,   346,   199,   300,   378,   257,   327,   364,
     381,   298,   341,   258,   259,   342,   200,   263,   334,   331,
     282,   201,   302,   344,   354,   358,   202,   277,   319,   107,
     205,   247,   108,   171,   172,   109,    29,    45,    70,    71,
      72,    73,   110
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      98,   124,   125,   120,   121,    46,   203,   289,   128,   129,
     214,    74,    28,   255,   143,    56,   132,   122,   114,   315,
     -32,   150,   151,   152,   153,   154,   155,   185,    76,    62,
      63,    64,    65,   156,    66,    67,   285,     3,   330,   367,
     330,   368,   333,     6,   123,  -227,     4,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   245,
     130,   343,   330,    57,   345,    68,   -57,   -57,   293,    13,
     256,    77,   174,   271,   272,    68,   175,   122,    14,    78,
    -227,   173,    27,   279,   176,   260,   330,   269,   366,     8,
     280,     9,    87,    58,   274,   374,   177,   286,   178,   179,
      75,    27,   287,   186,   123,   180,  -218,  -218,   181,   182,
     183,   123,   155,    12,   -87,    37,    38,    21,    39,   156,
      25,   -87,   352,    27,   174,    27,   166,    30,   175,   359,
     360,    31,    92,    32,    27,    33,   176,   184,    -7,   253,
      36,    80,   271,   272,    87,  -218,    44,    48,   177,    51,
     178,   179,    58,    13,    60,    80,   203,   180,   111,   214,
     181,   182,   183,   123,    82,   301,   114,   116,   118,    62,
     173,   207,   208,   119,   209,   210,   -31,   185,   157,   203,
    -157,   203,   162,   316,    92,    62,    63,   321,    65,   184,
      66,    67,   158,    80,   163,   143,   204,  -218,   206,   164,
     185,   216,   185,   217,   152,   153,   154,   155,   203,   246,
    -227,   248,   173,   203,   156,    68,   173,  -228,   251,   250,
     252,   299,   308,   309,   310,   311,   312,   254,   339,   185,
     264,    68,   370,   261,   185,   265,   266,   203,   322,   276,
     278,   283,   203,   281,   349,   336,   347,   348,   284,   288,
     350,   290,   292,   294,   306,   307,   317,   324,   185,   318,
     335,   365,   353,   185,   375,   328,    83,   340,   156,   379,
     -98,  -204,   351,    84,   355,   371,   -98,   372,    85,    62,
      63,    86,    65,   357,    66,    67,    87,   -98,   380,   382,
     -98,   -98,   -98,   -98,   383,    52,    16,    50,    61,   -98,
     -98,   -98,   -98,   -98,   -98,    88,    49,   -98,   296,   -98,
     -98,   -98,   -98,   -98,   -98,   -98,   -98,   -98,   -98,   -98,
     -98,    89,   -98,   -98,    90,    91,    92,   -98,   -98,    93,
      94,   -98,   -98,    95,   -98,    96,   -98,   -98,    83,   -98,
     249,    81,  -174,   376,   377,    84,   303,   161,  -174,   244,
      85,    62,    63,    86,    65,   373,    66,    67,    87,   323,
     305,   320,  -174,  -174,  -174,  -174,     0,   268,   297,   326,
       0,  -174,  -174,  -174,  -174,  -174,  -174,    88,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    89,     0,     0,    90,    91,    92,     0,
     174,    93,    94,  -174,   175,    95,     0,    96,     0,     0,
      27,  -174,   176,     0,     0,     0,     0,     0,     0,     0,
      87,     0,     0,     0,   177,     0,   178,   179,    83,     0,
       0,     0,     0,   180,     0,    84,   181,   182,   183,   123,
      85,    62,    63,    86,    65,     0,    66,    67,    87,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      92,     0,     0,     0,     0,   184,     0,    88,     0,    80,
       0,     0,     0,   -62,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    89,     0,     0,    90,    91,    92,     0,
       0,    93,    94,   165,     0,    95,     0,    96,     0,   -98,
      84,     0,     0,    27,   166,    85,    62,    63,    86,    65,
      83,    66,    67,    87,     0,     0,     0,    84,     0,     0,
       0,     0,    85,    62,    63,    86,    65,    83,    66,    67,
      87,     0,    88,     0,    84,     0,     0,     0,     0,    85,
      62,    63,    86,    65,     0,    66,    67,    87,    89,    88,
       0,    90,    91,    92,     0,     0,    93,    94,     0,   -98,
      95,   -43,    96,     0,     0,    89,    88,     0,    90,    91,
      92,     0,     0,    93,    94,     0,   -98,    95,   -98,    96,
       0,     0,    89,     0,     0,    90,    91,    92,     0,     0,
      93,    94,   -98,    83,    95,   -98,    96,     0,     0,     0,
      84,     0,     0,    27,     0,    85,    62,    63,    86,    65,
      83,    66,    67,    87,     0,     0,     0,    84,     0,     0,
       0,     0,    85,    62,    63,    86,    65,    83,    66,    67,
      87,     0,    88,     0,    84,     0,     0,     0,     0,    85,
      62,    63,    86,    65,     0,    66,    67,    87,    89,    88,
       0,    90,    91,    92,     0,     0,    93,    94,     0,     0,
      95,     0,    96,     0,     0,    89,    88,     0,    90,    91,
      92,     0,     0,    93,    94,     0,     0,    95,   -98,    96,
       0,     0,    89,     0,     0,    90,    91,    92,   174,     0,
      93,    94,   175,     0,    95,     0,    96,     0,    27,     0,
     176,     0,     0,     0,     0,     0,     0,     0,    87,     0,
       0,     0,   177,     0,   178,   179,    83,     0,     0,     0,
       0,   180,     0,    84,   181,   182,   183,   123,   159,    62,
      63,    86,    65,     0,    66,    67,    87,   304,    62,    63,
      86,    65,     0,    66,    67,   137,   138,     0,    92,     0,
       0,   143,     0,   184,     0,   160,     0,   329,   150,   151,
     152,   153,   154,   155,   160,     0,     0,     0,     0,     0,
     156,     0,     0,   133,     0,    68,    92,     0,     0,    93,
       0,     0,     0,     0,    68,    96,     0,     0,    93,     0,
       0,     0,     0,   134,    96,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,     0,     0,
       0,   134,   156,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,     0,     0,     0,     0,
     156,   337,   134,     0,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,     0,     0,   215,
     134,   156,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,     0,     0,     0,     0,   332,   134,   156,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,     0,   356,     0,     0,     0,   134,   156,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   134,
       0,   135,   136,   137,   138,   156,     0,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   134,     0,  -229,  -229,   137,   138,   156,     0,
     141,   142,   143,     0,     0,     0,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   134,     0,   135,   136,   137,
     138,   156,     0,   141,   142,   143,     0,     0,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,     0,     0,
       0,   137,   138,     0,   156,   141,   142,   143,     0,     0,
       0,     0,   148,   149,   150,   151,   152,   153,   154,   155,
       0,     0,     0,     0,     0,     0,   156
};

static const yytype_int16 yycheck[] =
{
      58,    91,    92,    89,    90,    29,   117,   218,    94,    95,
     127,    51,    16,     1,    52,    14,    97,    13,    14,   277,
      43,    59,    60,    61,    62,    63,    64,   117,    53,    14,
      15,    16,    17,    71,    19,    20,   205,     0,   300,    14,
     302,    16,   302,    13,    40,    68,     3,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
      95,   329,   334,    72,   334,    60,     6,     7,   247,     9,
      68,    13,     1,    23,    24,    60,     5,    13,    18,    21,
      68,   116,    11,    64,    13,    73,   358,   183,   358,    66,
      71,     4,    21,    43,   185,   363,    25,    67,    27,    28,
      10,    11,    72,   117,    40,    34,    35,    36,    37,    38,
      39,    40,    64,    16,    64,    13,    14,    66,    16,    71,
      65,    71,   343,    11,     1,    11,    12,    66,     5,    35,
      36,    66,    61,    66,    11,     6,    13,    66,     0,   173,
      67,    70,    23,    24,    21,    74,    64,     7,    25,    43,
      27,    28,    43,     9,    68,    70,   277,    34,    13,   286,
      37,    38,    39,    40,    72,   261,    14,    68,    11,    14,
     205,    16,    17,    68,    19,    20,    43,   277,    16,   300,
      72,   302,    43,   279,    61,    14,    15,    16,    17,    66,
      19,    20,    68,    70,    67,    52,    68,    74,    72,    69,
     300,    69,   302,    16,    61,    62,    63,    64,   329,    13,
      68,    16,   247,   334,    71,    60,   251,    68,    67,    69,
      67,   256,    29,    30,    31,    32,    33,    66,   324,   329,
      13,    60,   359,    68,   334,    68,    14,   358,   288,    66,
      66,    66,   363,    74,   340,   313,   337,   338,    69,    41,
     341,    67,    72,    16,    66,    66,    13,    22,   358,    69,
      69,    69,    26,   363,   364,   299,     1,    66,    71,    22,
       5,    73,    73,     8,    70,    73,    11,    74,    13,    14,
      15,    16,    17,   351,    19,    20,    21,    22,    73,    14,
      25,    26,    27,    28,    69,    42,    11,    36,    48,    34,
      35,    36,    37,    38,    39,    40,    35,    42,   251,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,     1,    74,
     163,    55,     5,   364,   364,     8,   263,   109,    11,   155,
      13,    14,    15,    16,    17,   362,    19,    20,    21,   290,
     265,   286,    25,    26,    27,    28,    -1,   182,   252,   293,
      -1,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    -1,    59,    60,    61,    -1,
       1,    64,    65,    66,     5,    68,    -1,    70,    -1,    -1,
      11,    74,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      21,    -1,    -1,    -1,    25,    -1,    27,    28,     1,    -1,
      -1,    -1,    -1,    34,    -1,     8,    37,    38,    39,    40,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    -1,    -1,    66,    -1,    40,    -1,    70,
      -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    -1,    59,    60,    61,    -1,
      -1,    64,    65,     1,    -1,    68,    -1,    70,    -1,    72,
       8,    -1,    -1,    11,    12,    13,    14,    15,    16,    17,
       1,    19,    20,    21,    -1,    -1,    -1,     8,    -1,    -1,
      -1,    -1,    13,    14,    15,    16,    17,     1,    19,    20,
      21,    -1,    40,    -1,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    17,    -1,    19,    20,    21,    56,    40,
      -1,    59,    60,    61,    -1,    -1,    64,    65,    -1,    67,
      68,    69,    70,    -1,    -1,    56,    40,    -1,    59,    60,
      61,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
      -1,    -1,    56,    -1,    -1,    59,    60,    61,    -1,    -1,
      64,    65,    66,     1,    68,    69,    70,    -1,    -1,    -1,
       8,    -1,    -1,    11,    -1,    13,    14,    15,    16,    17,
       1,    19,    20,    21,    -1,    -1,    -1,     8,    -1,    -1,
      -1,    -1,    13,    14,    15,    16,    17,     1,    19,    20,
      21,    -1,    40,    -1,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    17,    -1,    19,    20,    21,    56,    40,
      -1,    59,    60,    61,    -1,    -1,    64,    65,    -1,    -1,
      68,    -1,    70,    -1,    -1,    56,    40,    -1,    59,    60,
      61,    -1,    -1,    64,    65,    -1,    -1,    68,    69,    70,
      -1,    -1,    56,    -1,    -1,    59,    60,    61,     1,    -1,
      64,    65,     5,    -1,    68,    -1,    70,    -1,    11,    -1,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,
      -1,    -1,    25,    -1,    27,    28,     1,    -1,    -1,    -1,
      -1,    34,    -1,     8,    37,    38,    39,    40,    13,    14,
      15,    16,    17,    -1,    19,    20,    21,    13,    14,    15,
      16,    17,    -1,    19,    20,    46,    47,    -1,    61,    -1,
      -1,    52,    -1,    66,    -1,    40,    -1,    70,    59,    60,
      61,    62,    63,    64,    40,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    22,    -1,    60,    61,    -1,    -1,    64,
      -1,    -1,    -1,    -1,    60,    70,    -1,    -1,    64,    -1,
      -1,    -1,    -1,    42,    70,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    42,    71,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    72,    42,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,    69,
      42,    71,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    -1,    -1,    -1,    69,    42,    71,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    -1,    -1,    -1,    42,    71,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    42,
      -1,    44,    45,    46,    47,    71,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    42,    -1,    44,    45,    46,    47,    71,    -1,
      50,    51,    52,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    42,    -1,    44,    45,    46,
      47,    71,    -1,    50,    51,    52,    -1,    -1,    -1,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    46,    47,    -1,    71,    50,    51,    52,    -1,    -1,
      -1,    -1,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    76,    77,     0,     3,    81,    13,    82,    66,     4,
      83,    84,    16,     9,    18,    78,    85,    86,    97,   108,
     109,    66,    87,    88,    89,    65,    79,    11,    98,   191,
      66,    66,    66,     6,   103,   104,    67,    13,    14,    16,
      92,    80,   110,   111,    64,   192,    92,   105,     7,   103,
      88,    43,    80,    85,    95,    96,    14,    72,    43,   121,
      68,    96,    14,    15,    16,    17,    19,    20,    60,    91,
     193,   194,   195,   196,   197,    10,   191,    13,    21,    94,
      70,   112,    72,     1,     8,    13,    16,    21,    40,    56,
      59,    60,    61,    64,    65,    68,    70,    93,   122,   124,
     129,   130,   131,   133,   134,   139,   145,   184,   187,   190,
     197,    13,   106,   107,    14,    90,    68,   113,    11,    68,
     131,   131,    13,    40,    93,    93,   140,   141,   131,   131,
     191,   135,   121,    22,    42,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    71,    16,    68,    13,
      40,   129,    43,    67,    69,     1,    12,   100,   101,   102,
     122,   188,   189,   191,     1,     5,    13,    25,    27,    28,
      34,    37,    38,    39,    66,    93,    98,   112,   114,   115,
     117,   119,   128,   146,   147,   148,   155,   156,   157,   159,
     171,   176,   181,   184,    68,   185,    72,    16,    17,    19,
      20,   142,   143,   144,   195,    69,    69,    16,   136,   137,
     138,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   123,   124,   131,    13,   186,    16,   107,
      69,    67,    67,    92,    66,     1,    68,   162,   168,   169,
      73,    68,   132,   172,    13,    68,    14,   150,   150,   131,
     149,    23,    24,   118,   121,   127,    66,   182,    66,    64,
      71,    74,   175,    66,    69,   100,    67,    72,    41,   175,
      67,   125,    72,   100,    16,    99,   102,   189,   166,   191,
     160,   131,   177,   132,    13,   133,    66,    66,    29,    30,
      31,    32,    33,   120,    98,   181,   131,    13,    69,   183,
     143,    16,   197,   138,    22,   126,   183,   163,    92,    70,
     114,   174,    69,   174,   173,    69,   122,    72,   116,   131,
      66,   167,   170,   181,   178,   174,   158,   121,   121,   131,
     121,    73,   175,    26,   179,    70,    66,   122,   180,    35,
      36,   151,   152,   153,   164,    69,   174,    14,    16,   154,
     195,    73,    74,   151,   181,    93,   117,   128,   161,    22,
      73,   165,    14,    69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    77,    78,    76,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    84,    84,    84,    84,    85,    85,
      86,    87,    87,    89,    90,    88,    91,    91,    92,    92,
      92,    93,    93,    94,    94,    95,    95,    96,    97,    98,
      98,    99,    99,   100,   100,   101,   101,   102,   102,   103,
     103,   105,   104,   106,   107,   107,   107,   109,   108,   111,
     110,   112,   113,   113,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     115,   116,   115,   115,   115,   118,   117,   119,   120,   120,
     120,   120,   120,   121,   122,   122,   123,   123,   124,   125,
     124,   126,   126,   127,   127,   128,   129,   129,   129,   130,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   132,   132,   133,   133,   133,   133,   133,   133,   135,
     134,   136,   136,   137,   137,   138,   138,   140,   139,   141,
     139,   142,   142,   143,   143,   144,   144,   144,   144,   145,
     145,   146,   147,   148,   149,   149,   150,   150,   151,   151,
     152,   153,   153,   154,   154,   154,   155,   156,   158,   157,
     160,   159,   161,   161,   163,   164,   165,   162,   166,   167,
     166,   168,   168,   168,   170,   169,   172,   173,   171,   174,
     174,   175,   177,   178,   176,   179,   180,   179,   181,   182,
     181,   183,   185,   184,   186,   184,   187,   187,   187,   100,
     188,   188,   188,   189,   190,   191,   191,   192,   192,   193,
     193,   194,   195,   195,   196,   197,   197,   197,   197
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     0,
       3,     0,     2,     3,     0,     3,     3,     3,     0,     1,
       2,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     6,     2,     3,
       2,     0,     1,     0,     1,     1,     3,     2,     1,     0,
       2,     0,     5,     3,     0,     1,     3,     0,     4,     0,
       3,     4,     0,     3,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     0,     5,     5,     1,     0,     4,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     0,     0,
       5,     0,     2,     1,     1,     2,     1,     1,     1,     4,
       1,     2,     3,     4,     2,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     0,
       4,     0,     1,     1,     3,     3,     3,     0,     3,     0,
       4,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     2,     0,     1,     0,     1,     0,     2,
       2,     3,     2,     1,     1,     3,     2,     2,     0,     8,
       0,     4,     1,     1,     0,     0,     0,    10,     0,     0,
       4,     1,     1,     1,     0,     7,     0,     0,     5,     1,
       3,     1,     0,     0,     6,     0,     0,     3,     0,     0,
       3,     1,     0,     5,     0,     5,     2,     1,     1,     1,
       1,     3,     1,     1,     3,     1,     2,     2,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1
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
#line 267 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2170 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 273 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2178 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 283 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2186 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 286 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2195 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 294 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2204 "parser.c"
    break;

  case 18: /* scope: %empty  */
#line 304 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2210 "parser.c"
    break;

  case 19: /* scope: T_STATIC  */
#line 305 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2216 "parser.c"
    break;

  case 23: /* $@3: %empty  */
#line 315 "Parser.y"
                { _bDefiningConst = 1; }
#line 2222 "parser.c"
    break;

  case 24: /* $@4: %empty  */
#line 315 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2228 "parser.c"
    break;

  case 25: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 316 "Parser.y"
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
#line 2244 "parser.c"
    break;

  case 26: /* constant_literal: simple_literal  */
#line 329 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2252 "parser.c"
    break;

  case 27: /* constant_literal: T_STRING  */
#line 332 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2261 "parser.c"
    break;

  case 28: /* variable_identifier: T_IDENT  */
#line 338 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2271 "parser.c"
    break;

  case 29: /* variable_identifier: T_NUM  */
#line 344 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2282 "parser.c"
    break;

  case 30: /* variable_identifier: T_STRING  */
#line 351 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2291 "parser.c"
    break;

  case 31: /* variable_reference: T_VARIABLE  */
#line 357 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2297 "parser.c"
    break;

  case 32: /* variable_reference: T_IDENT  */
#line 358 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2309 "parser.c"
    break;

  case 33: /* macro_declaration: T_IDENT  */
#line 368 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2317 "parser.c"
    break;

  case 34: /* macro_declaration: T_FUNC  */
#line 371 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2327 "parser.c"
    break;

  case 35: /* macro_type: scope T_VOID  */
#line 377 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2340 "parser.c"
    break;

  case 36: /* macro_type: scope type_name  */
#line 386 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2353 "parser.c"
    break;

  case 37: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 396 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2363 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier assignment_expression  */
#line 404 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2375 "parser.c"
    break;

  case 40: /* var_decl: type_name variable_identifier  */
#line 410 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2386 "parser.c"
    break;

  case 42: /* optional_description: T_STRING  */
#line 419 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2395 "parser.c"
    break;

  case 47: /* parameter_declaration: type_name variable_identifier  */
#line 433 "Parser.y"
                                                      {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[-1].ident.type, yyvsp[0].ident.s);
				} else {
					sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
					bytecode_defineVariable(_currentBytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER, yyvsp[-1].ident.type,_nparam, _localVariableIndex++);
				}
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2410 "parser.c"
    break;

  case 48: /* parameter_declaration: T_ELLIPSIS  */
#line 442 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2420 "parser.c"
    break;

  case 51: /* $@5: %empty  */
#line 451 "Parser.y"
                         {
				if (strcmp(yyvsp[0].ident.s, "@Method") == 0) {
					_currentNativeMethodAnnotation = hashmap_create(3, 0 ,0);
				}
				free(yyvsp[0].ident.s);
			}
#line 2431 "parser.c"
    break;

  case 53: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 458 "Parser.y"
                                            {
				if (_currentNativeMethodAnnotation) {
					hashmap_put(_currentNativeMethodAnnotation, (intptr_t)yyvsp[-2].ident.s, (intptr_t)yyvsp[0].ident.s);
				} else {
					free(yyvsp[-2].ident.s);
					free(yyvsp[0].ident.s);
				}
			}
#line 2444 "parser.c"
    break;

  case 57: /* $@6: %empty  */
#line 471 "Parser.y"
                          {
					_bInNativeDefinition = 1;
				}
#line 2452 "parser.c"
    break;

  case 58: /* native_macro_definition: $@6 annotations T_NATIVE macrostart  */
#line 474 "Parser.y"
                                                                {
					if (!_currentNativeMethodAnnotation) {
						_currentNativeMethodAnnotation = hashmap_create(3, 0, 0);
					}
					char* pszLib = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"lib");
					char* pszMacroCName = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"name");
					char* pszSignature = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"signature");
					char* pszArgs = stringbuf_getString(_nativeMethodArgs);
					if (!pszSignature) {
						pszSignature = stringbuf_getString(_nativeMethodSignature);
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
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszArgs});
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
							C_0FUNC, (GENERIC_DATA){ FUNC_RegisterNative}, 6);
					// register right away as well, so we can use it.
					function_registerNativeFunction(pszMacroCName, yyvsp[0].ident.s, pszLib, pszSignature, _yyCurrentComment, pszArgs);
					free(yyvsp[0].ident.s);
					hashmap_destroyWithKeysAndValues(_currentNativeMethodAnnotation);
					stringbuf_destroy(_nativeMethodArgs);
					stringbuf_destroy(_nativeMethodSignature);
					_nativeMethodArgs = 0;
					_nativeMethodSignature = 0;
				}
#line 2492 "parser.c"
    break;

  case 59: /* $@7: %empty  */
#line 510 "Parser.y"
                  { init_macroDefinition(); }
#line 2498 "parser.c"
    break;

  case 60: /* macro_definition: $@7 macrostart block  */
#line 511 "Parser.y"
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
#line 2526 "parser.c"
    break;

  case 65: /* statement: call_expression ';'  */
#line 541 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2532 "parser.c"
    break;

  case 66: /* statement: assignment ';'  */
#line 542 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2538 "parser.c"
    break;

  case 67: /* statement: if_expression  */
#line 543 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2544 "parser.c"
    break;

  case 68: /* statement: while  */
#line 544 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2550 "parser.c"
    break;

  case 69: /* statement: for_loop_expression  */
#line 545 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2556 "parser.c"
    break;

  case 70: /* statement: switch_expression  */
#line 546 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2562 "parser.c"
    break;

  case 71: /* statement: break  */
#line 547 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2568 "parser.c"
    break;

  case 72: /* statement: continue  */
#line 548 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2574 "parser.c"
    break;

  case 73: /* statement: return_expression  */
#line 549 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2580 "parser.c"
    break;

  case 74: /* statement: goto  */
#line 550 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2586 "parser.c"
    break;

  case 75: /* statement: block  */
#line 551 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2592 "parser.c"
    break;

  case 76: /* statement: increment_expression  */
#line 552 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2598 "parser.c"
    break;

  case 77: /* statement: label  */
#line 553 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2604 "parser.c"
    break;

  case 78: /* statement: var_decl  */
#line 554 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2612 "parser.c"
    break;

  case 79: /* statement: error ';'  */
#line 557 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2622 "parser.c"
    break;

  case 80: /* assignment: variable_reference assignment_expression  */
#line 563 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2630 "parser.c"
    break;

  case 81: /* $@8: %empty  */
#line 567 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2638 "parser.c"
    break;

  case 82: /* assignment: assignment_target '.' T_IDENT $@8 assignment_expression  */
#line 569 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2646 "parser.c"
    break;

  case 83: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 573 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2654 "parser.c"
    break;

  case 85: /* $@9: %empty  */
#line 579 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2662 "parser.c"
    break;

  case 86: /* shorthand_assignment: variable_reference $@9 shorthand_assignment_operator simple_expression  */
#line 581 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2671 "parser.c"
    break;

  case 87: /* assignment_target: variable_reference  */
#line 586 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2679 "parser.c"
    break;

  case 88: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 591 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2685 "parser.c"
    break;

  case 89: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 592 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2691 "parser.c"
    break;

  case 90: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 593 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2697 "parser.c"
    break;

  case 91: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 594 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2703 "parser.c"
    break;

  case 92: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 595 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2709 "parser.c"
    break;

  case 93: /* assignment_expression: T_ASSIGN simple_expression  */
#line 597 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2715 "parser.c"
    break;

  case 94: /* simple_expression: binary_expression  */
#line 599 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2721 "parser.c"
    break;

  case 99: /* $@10: %empty  */
#line 607 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2727 "parser.c"
    break;

  case 102: /* range_increment: T_DOTDOT binary_expression  */
#line 610 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2733 "parser.c"
    break;

  case 103: /* minusminus_plusplus: T_MINUSMINUS  */
#line 613 "Parser.y"
                                     { yyval.num = -1; }
#line 2739 "parser.c"
    break;

  case 104: /* minusminus_plusplus: T_PLUSPLUS  */
#line 614 "Parser.y"
                                     { yyval.num = 1; }
#line 2745 "parser.c"
    break;

  case 105: /* increment_expression: variable_reference minusminus_plusplus  */
#line 617 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2753 "parser.c"
    break;

  case 106: /* s_bterm: value  */
#line 621 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2759 "parser.c"
    break;

  case 109: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 625 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2767 "parser.c"
    break;

  case 110: /* binary_expression: s_bterm  */
#line 630 "Parser.y"
                                { yyval.ident.type = yyvsp[0].ident.type;}
#line 2773 "parser.c"
    break;

  case 111: /* binary_expression: type_cast s_bterm  */
#line 631 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2782 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '.' T_IDENT  */
#line 636 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2791 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 640 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2797 "parser.c"
    break;

  case 114: /* binary_expression: '!' binary_expression  */
#line 641 "Parser.y"
                                                { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2803 "parser.c"
    break;

  case 115: /* binary_expression: '~' binary_expression  */
#line 642 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2809 "parser.c"
    break;

  case 116: /* binary_expression: '+' binary_expression  */
#line 643 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2815 "parser.c"
    break;

  case 117: /* binary_expression: '-' variable_reference  */
#line 644 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2821 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression '~' binary_expression  */
#line 645 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2827 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 646 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2833 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression '&' binary_expression  */
#line 647 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2839 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression '|' binary_expression  */
#line 648 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2845 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '+' binary_expression  */
#line 649 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2851 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression '^' binary_expression  */
#line 650 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2857 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression '-' binary_expression  */
#line 651 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2863 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression '*' binary_expression  */
#line 652 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2869 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression '/' binary_expression  */
#line 653 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2875 "parser.c"
    break;

  case 127: /* binary_expression: binary_expression '%' binary_expression  */
#line 654 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2881 "parser.c"
    break;

  case 128: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 655 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2887 "parser.c"
    break;

  case 129: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 656 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2893 "parser.c"
    break;

  case 130: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 657 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2899 "parser.c"
    break;

  case 131: /* binary_expression: binary_expression T_AND binary_expression  */
#line 658 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2905 "parser.c"
    break;

  case 132: /* binary_expression: binary_expression T_OR binary_expression  */
#line 659 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2911 "parser.c"
    break;

  case 133: /* binary_expression: binary_expression '<' binary_expression  */
#line 660 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2917 "parser.c"
    break;

  case 134: /* binary_expression: binary_expression '>' binary_expression  */
#line 661 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2923 "parser.c"
    break;

  case 135: /* binary_expression: binary_expression T_LE binary_expression  */
#line 662 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2929 "parser.c"
    break;

  case 136: /* binary_expression: binary_expression T_GE binary_expression  */
#line 663 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2935 "parser.c"
    break;

  case 137: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 664 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2941 "parser.c"
    break;

  case 138: /* binary_expression: binary_expression T_NE binary_expression  */
#line 665 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2947 "parser.c"
    break;

  case 139: /* binary_expression: '(' binary_expression ')'  */
#line 666 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2953 "parser.c"
    break;

  case 140: /* binary_expression: variable_reference assignment_expression  */
#line 667 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2961 "parser.c"
    break;

  case 142: /* condition: '(' binary_expression  */
#line 674 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2969 "parser.c"
    break;

  case 143: /* value: T_VARIABLE  */
#line 678 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2978 "parser.c"
    break;

  case 144: /* value: string  */
#line 682 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2988 "parser.c"
    break;

  case 145: /* value: simple_literal  */
#line 687 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2994 "parser.c"
    break;

  case 146: /* value: map_literal  */
#line 688 "Parser.y"
                                      {
				yyval.ident.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 3005 "parser.c"
    break;

  case 147: /* value: array_literal  */
#line 694 "Parser.y"
                                        {
				yyval.ident.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 3016 "parser.c"
    break;

  case 148: /* value: T_IDENT  */
#line 700 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 3026 "parser.c"
    break;

  case 149: /* $@11: %empty  */
#line 706 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
				// Hack - avoid push instruction being generated for map literal elements.
				_bDefiningConst = 1;
			}
#line 3036 "parser.c"
    break;

  case 150: /* map_literal: '{' $@11 optional_map_associates closing_brace  */
#line 710 "Parser.y"
                                                                {
				_bDefiningConst = 0;
			}
#line 3044 "parser.c"
    break;

  case 155: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 724 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 3053 "parser.c"
    break;

  case 156: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 728 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 3062 "parser.c"
    break;

  case 157: /* $@12: %empty  */
#line 733 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 3070 "parser.c"
    break;

  case 159: /* $@13: %empty  */
#line 737 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 3078 "parser.c"
    break;

  case 163: /* array_element: simple_array_element  */
#line 746 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 3086 "parser.c"
    break;

  case 164: /* array_element: T_STRING  */
#line 750 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 3094 "parser.c"
    break;

  case 165: /* simple_array_element: integer_literal  */
#line 755 "Parser.y"
                                        { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].v.data.longValue; }
#line 3100 "parser.c"
    break;

  case 166: /* simple_array_element: T_CHARACTER  */
#line 756 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 3106 "parser.c"
    break;

  case 167: /* simple_array_element: T_TRUE  */
#line 757 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 3112 "parser.c"
    break;

  case 168: /* simple_array_element: T_FALSE  */
#line 758 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 3118 "parser.c"
    break;

  case 169: /* string: T_STRING  */
#line 760 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 3124 "parser.c"
    break;

  case 170: /* string: string T_STRING  */
#line 761 "Parser.y"
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
#line 3139 "parser.c"
    break;

  case 171: /* break: T_BREAK opt_num ';'  */
#line 773 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3145 "parser.c"
    break;

  case 172: /* continue: T_CONTINUE opt_num ';'  */
#line 776 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3151 "parser.c"
    break;

  case 173: /* return_expression: T_RETURN optional_bterm  */
#line 779 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3159 "parser.c"
    break;

  case 176: /* opt_num: %empty  */
#line 786 "Parser.y"
                {	yyval.num = 1;	}
#line 3165 "parser.c"
    break;

  case 177: /* opt_num: T_NUM  */
#line 787 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3171 "parser.c"
    break;

  case 182: /* case_selector: T_DEFAULT ':'  */
#line 796 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3179 "parser.c"
    break;

  case 183: /* case_condition: integer_literal  */
#line 801 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3187 "parser.c"
    break;

  case 184: /* case_condition: T_STRING  */
#line 804 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3195 "parser.c"
    break;

  case 185: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 807 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3203 "parser.c"
    break;

  case 186: /* label: T_IDENT ':'  */
#line 811 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3212 "parser.c"
    break;

  case 187: /* goto: T_GOTO T_IDENT  */
#line 816 "Parser.y"
                               {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3221 "parser.c"
    break;

  case 188: /* $@14: %empty  */
#line 823 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3231 "parser.c"
    break;

  case 189: /* switch_expression: T_SWITCH '(' value ')' $@14 '{' case_clauses '}'  */
#line 829 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3244 "parser.c"
    break;

  case 190: /* $@15: %empty  */
#line 839 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3253 "parser.c"
    break;

  case 191: /* for_loop_expression: T_FOR for_clause $@15 local_block  */
#line 842 "Parser.y"
                                      {
				parser_flushBuffer(_breaklevel);
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3266 "parser.c"
    break;

  case 194: /* $@16: %empty  */
#line 854 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3274 "parser.c"
    break;

  case 195: /* $@17: %empty  */
#line 856 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3282 "parser.c"
    break;

  case 196: /* $@18: %empty  */
#line 858 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3291 "parser.c"
    break;

  case 199: /* $@19: %empty  */
#line 864 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				free(yyvsp[0].ident.s);
			}
#line 3300 "parser.c"
    break;

  case 203: /* for_clause: error  */
#line 871 "Parser.y"
                                {
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3309 "parser.c"
    break;

  case 204: /* $@20: %empty  */
#line 876 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 3323 "parser.c"
    break;

  case 205: /* in_clause: '(' type_name variable_identifier $@20 ':' simple_expression ')'  */
#line 884 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 3334 "parser.c"
    break;

  case 206: /* $@21: %empty  */
#line 891 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3342 "parser.c"
    break;

  case 207: /* $@22: %empty  */
#line 893 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3351 "parser.c"
    break;

  case 208: /* while: T_WHILE $@21 condition $@22 local_block  */
#line 896 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3363 "parser.c"
    break;

  case 212: /* $@23: %empty  */
#line 909 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3372 "parser.c"
    break;

  case 213: /* $@24: %empty  */
#line 912 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3380 "parser.c"
    break;

  case 214: /* if_expression: T_IF condition $@23 local_block $@24 else_clause  */
#line 914 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3389 "parser.c"
    break;

  case 215: /* else_clause: %empty  */
#line 919 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3398 "parser.c"
    break;

  case 216: /* $@25: %empty  */
#line 923 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3408 "parser.c"
    break;

  case 218: /* stmntlist: %empty  */
#line 930 "Parser.y"
                { yyval.needsPop = 0; }
#line 3414 "parser.c"
    break;

  case 219: /* $@26: %empty  */
#line 931 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3424 "parser.c"
    break;

  case 222: /* @27: %empty  */
#line 939 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3436 "parser.c"
    break;

  case 223: /* call_expression: T_FUNC '(' @27 parameter_list closing_paren  */
#line 945 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3446 "parser.c"
    break;

  case 224: /* @28: %empty  */
#line 950 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3457 "parser.c"
    break;

  case 225: /* call_expression: function_id_or_pointer '(' @28 parameter_list closing_paren  */
#line 955 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3471 "parser.c"
    break;

  case 226: /* function_id_or_pointer: '*' variable_reference  */
#line 966 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3485 "parser.c"
    break;

  case 227: /* function_id_or_pointer: T_IDENT  */
#line 976 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3494 "parser.c"
    break;

  case 228: /* function_id_or_pointer: error  */
#line 980 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3504 "parser.c"
    break;

  case 232: /* nonempty_parameters: error  */
#line 990 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3512 "parser.c"
    break;

  case 233: /* parameter: simple_expression  */
#line 994 "Parser.y"
                                  { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3518 "parser.c"
    break;

  case 234: /* type_cast: '(' type_name ')'  */
#line 996 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3527 "parser.c"
    break;

  case 235: /* type_name: T_TYPE_IDENTIFIER  */
#line 1001 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3535 "parser.c"
    break;

  case 236: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1004 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3544 "parser.c"
    break;

  case 238: /* array_size: '[' T_NUM ']'  */
#line 1010 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3552 "parser.c"
    break;

  case 239: /* boolean_literal: T_TRUE  */
#line 1014 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3561 "parser.c"
    break;

  case 240: /* boolean_literal: T_FALSE  */
#line 1017 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3570 "parser.c"
    break;

  case 241: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1022 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3579 "parser.c"
    break;

  case 242: /* integer_literal: T_NUM  */
#line 1027 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3588 "parser.c"
    break;

  case 243: /* integer_literal: '-' T_NUM  */
#line 1031 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3598 "parser.c"
    break;

  case 244: /* character_literal: T_CHARACTER  */
#line 1038 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3607 "parser.c"
    break;

  case 245: /* simple_literal: integer_literal  */
#line 1043 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3620 "parser.c"
    break;

  case 246: /* simple_literal: float_literal  */
#line 1051 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3633 "parser.c"
    break;

  case 247: /* simple_literal: character_literal  */
#line 1059 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3646 "parser.c"
    break;

  case 248: /* simple_literal: boolean_literal  */
#line 1067 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3659 "parser.c"
    break;


#line 3663 "parser.c"

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

#line 1075 "Parser.y"


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

