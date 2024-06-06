/*
 * pksmacrocvm.h
 *
 * PKS-MacroC VM byte codes and execution.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 16.02.2022
 */

#ifndef PKSMACROCVM_H

#include <windows.h>

#define	BIN_ADD			'+'
#define	BIN_SUB			'-'
#define	BIN_MUL			'*'
#define	BIN_DIV			'/'
#define	BIN_MOD			'%'
#define	BIN_POWER		'P'

#define	BIN_SHIFT_LEFT	'L'
#define	BIN_SHIFT_RIGHT	'R'
#define BIN_AT			'['

#define	BIN_OR			'|'
#define	BIN_AND			'&'
#define	BIN_XOR			'^'

#define	BIN_NOT			'~'
#define	BIN_CAST		'C'

#define BIN_RANGE		','

#define IS_UNARY_OPERATOR(op)		(op == BIN_NOT || op == BIN_CAST)

typedef struct tagOBJECT_DATA OBJECT_DATA;

typedef struct tagNATIVE_FUNCTION NATIVE_FUNCTION;

typedef struct tagMACRO MACRO;

typedef union uGENERIC_DATA {
	unsigned char	uchar;
	intptr_t		val;
	unsigned char	booleanValue;
	OBJECT_DATA*	objectPointer;			// for objects managed by the macroC VM object memory this points to the object data.
	char*			string;
	void*			stringList;
	int				intValue;
	long long		longValue;
	double			doubleValue;
	struct tagRANGE {
		int r_start : 24;
		int r_end : 24;
		int r_increment : 16;
	} range;
} GENERIC_DATA;

// Must match the corresponding symbol types in symbols.h
typedef enum {
	VT_NIL = 0,
	VT_BOOLEAN = 1,
	VT_NUMBER = 2,
	VT_STRING = 3,
	VT_FLOAT = 4,
	VT_CHAR = 5,
	VT_RANGE = 6,
	VT_OBJECT_ARRAY = 7,
	VT_MAP = 8,
	VT_AUTO = 9,
	VT_FILE = 10,
	VT_MAP_ENTRY = 11,
	VT_EDITOR_HANDLE = 12							// Editor window handle
} PKS_VALUE_TYPE;

#define PKS_TYPE_FILE		"FILE"
#define PKS_TYPE_MAP_ENTRY	"MAP_ENTRY"
#define PKS_TYPE_EDITOR		"EDITOR"

typedef struct tagPKS_VALUE {
	int				pkv_managed : 1;				// the memory of this object is managed by the MacroVM object memory
	int				pkv_isPointer : 1;				// the actual data of this value is contained in the linked object
	PKS_VALUE_TYPE	pkv_type : 6;					// one of the "basic types" defined by PKS_VALUE_TYPE
	GENERIC_DATA	pkv_data;						// the data of this value.
} PKS_VALUE;

typedef void (*T_FINALIZER)(PKS_VALUE v);
typedef void* (*T_CONVERT_HANDLE)(void* pPointer);

typedef struct tagTYPE_CALLBACKS {
	T_CONVERT_HANDLE tc_handleToMacroMemory;		// Callback to convert an object for being stored in the macro memory.
	T_CONVERT_HANDLE tc_handleFromMacroMemory;		// Callback to convert an object from macro memory to being used in C code.
	T_FINALIZER tc_close;							// Callback method to close an internal resource associated with a value Gets passed the PKS_VALUE
													// containing the actual handle object.
} TYPE_CALLBACKS;

// Note, that this structure has currently identical fields (and better should have) as type PARAMETER_ENUM_VALUE
typedef struct tagTYPE_PROPERTY_DESCRIPTOR {
	const char*		tpd_name;
	const char*		tpd_documentation;
	PKS_VALUE_TYPE	tpd_type;
} TYPE_PROPERTY_DESCRIPTOR;

typedef struct tagPARAMETER_ENUM_VALUE PARAMETER_ENUM_VALUE;

typedef struct tagPKS_TYPE_DESCRIPTOR {
	PKS_VALUE_TYPE	ptd_type;						// the index used internally in macros for this type
	const char*		ptd_name;						// the printed named of this type
	const char*		ptd_documentation;				// The optional documentation for a type. Is always allocated.
	int				ptd_isEnumType : 1;				// whether this is an enum type.
	int				ptd_isHandleType : 1;			// whether this is a "handle" type wrapping a native C pointer (FILE*, WINFO*, ...)
	int				ptd_isValueType : 1;			// whether this is an immutable value type (number, float boolean etc...)
	int				ptd_hasDefaultValue : 1;		// Whether defining a variable of the described type will automatically create an "empty" instance of the type.
	int				ptd_hasDynamicSize : 1;			// whether objects of this type may change in size (arrays for instance).
	int				ptd_objectSize;					// the size of a non-value type object - typically identical to ptd_numberOfProperties, but for some
													// native types additional "internal pointers" are maintained not accessible as properties.
	int				ptd_numberOfProperties;			// the number of properties described by a property descriptor
	union {
		TYPE_PROPERTY_DESCRIPTOR* ptd_properties;
		PARAMETER_ENUM_VALUE* ptd_enumValues;
	} ptd_elements;
	TYPE_CALLBACKS	ptd_callbacks;
} PKS_TYPE_DESCRIPTOR;

#define IS_NIL(v)			v.pkv_type == VT_NIL

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

#define MAX_STACK_SIZE		256

typedef struct tagOBJECT_MEMORY OBJECT_MEMORY;

struct tagEXECUTION_CONTEXT {
	struct tagEXECUTION_CONTEXT* ec_parent;		// the parent context
	PKS_VALUE* ec_stackBottom;			// the bottom of the value stack
	PKS_VALUE* ec_stackFrame;			// the stack bottom of the stack frame for the current macro executed
	PKS_VALUE* ec_stackCurrent;			// the pointer to the current stack offset
	PKS_VALUE* ec_stackMax;				// the top of the stack. Must not be overridden
	PKS_VALUE* ec_localVariables;		// the local variables of a function
	int ec_localVariableCount;			// Number of local variables stored in the localVariables context.
	const char* ec_currentFunction;		// name of the current function/macro being executed.
	void* ec_instructionPointer;		// The current instruction pointer for debugging
};

typedef struct tagEXECUTION_CONTEXT EXECUTION_CONTEXT;

// Compressed value pointers - used to store as well simple values as also pointers to objects

typedef long long TYPED_OBJECT_POINTER;

#define POINTER_MASK ~(0xFFULL << 56)
#define TOP_DATA_POINTER(p)				(OBJECT_DATA*)((long long)p & POINTER_MASK)
#define TOP_IS_POINTER(p)				(((long long)p) >> 62)
#define TOP_TYPE(p)						((((long long)p) >> 56) & 0x3F)
#define MAKE_TYPED_OBJECT_POINTER(bIsPointer, sType, pPointer)	(((long long)bIsPointer<<62) | ((long long)sType << 56) | (((uintptr_t)pPointer) & POINTER_MASK))

extern void decompile_printValue(char* pszBuf, size_t nMaxChars, PKS_VALUE v);

/*
 * Extract the type info from a macro into the passed array of descriptors up to nMaxVars.
 * The infos are inserted according to the position in the heap, where they are maintained.
 * Return the maximum index of variables extracted.
 */
#if defined(_EDFUNCS_H)
extern int decompile_getLocalVariableInfo(MACRO* mp, TYPE_PROPERTY_DESCRIPTOR* pDescriptors, int nMaxVars);
#endif

/*
 * Return the finalizer for a given value type or 0 if not finalizer exists.
 */
extern T_FINALIZER types_getFinalizer(PKS_VALUE_TYPE vType);

/*
 * Returns the "type descriptor" for the named type.
 */
PKS_TYPE_DESCRIPTOR* types_getTypeDescriptor(const char* pszTypeName);

/*
 * Answer true, if the passed type exists.
 */
extern int types_existsType(PKS_VALUE_TYPE t);

/*
 * Register one type in the MacroC type registry. If 'nPreferredIndex is greater or equals 0
 * it is used as the internal type index in PKSMacroC, if it is negative, a dynamic type index
 * is created.
 */
extern int types_register(int nPreferredIndex, PKS_TYPE_DESCRIPTOR* pTemplate);

/*
 * Returns true, if a type is a value type.
 */
extern BOOL types_isValueType(PKS_VALUE_TYPE vType);

/*
 * Returns true, if this is a "handle" type wrapping a native C pointer (FILE*, WINFO*, ...)
 */
extern BOOL types_isHandleType(PKS_VALUE_TYPE vType);

/*
 * Returns true, if objects of the given type are automatically created when declaring them.
 */
extern BOOL types_hasDefaultValue(PKS_VALUE_TYPE vType);

/*
 * Returns the name of a given PKSMacroC value type.
 */
extern const char* types_nameFor(PKS_VALUE_TYPE t);

/*
 * Destroy a type descriptor with all allocated dependent objects.
 */
extern void types_destroyDescriptor(PKS_TYPE_DESCRIPTOR* pType);

/*
 * Returns the enum value table for a PKSMacroC enum type plus the respective size. The type is searched,
 * where the enum values start with a given prefix.
 */
extern int types_getEnumDescriptorForEnumPrefix(const char* pszPrefix, PARAMETER_ENUM_VALUE** pValues, int* pCount, PKS_VALUE_TYPE* pType);

/*
 * Returns the name of a property of a structured object given the type index and the property index.
 */
extern const char* types_getPropertyName(PKS_VALUE_TYPE t, int aPropertyIndex);

/*
 * Return the converter for a given value handle or 0 if not converter exists.
 */
extern T_CONVERT_HANDLE types_getConverterToMemory(PKS_VALUE_TYPE vType);
extern T_CONVERT_HANDLE types_getConverterFromMemory(PKS_VALUE_TYPE vType);

/*
 * Pop one value of the stack of our stack machine
 */
extern PKS_VALUE interpreter_popStackValue(EXECUTION_CONTEXT* pContext);

/*
 * An error has occurred during execution of a macro. Display a descriptive error using the passed format and parameters and abort the execution.
 */
extern void interpreter_raiseError(const char* pFormat, ...);

/*
 * Mini memory management function of PKSMacroC, which temporarily allocates a string to
 * be released later.
 */
extern PKS_VALUE interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource);

/*
 * Push one value onto the stack and raise an error, if that fails.
 */
extern int interpreter_pushValueOntoStack(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue);


/*
 Bytecodes of a macro.
 C_LOOP multiplier CMC_{MENU,CMDSEQ} funcnum {par} C_STR1PAR string ...
 all entities padded to ensure int alignment !!

 */

typedef enum {
	// Flow control
	C_STOP  = 0,    			// eof sequence
	C_0FUNC = 0x1,  			// Function # (char)
	C_1FUNC = 0x2,  			// Function # (char) + 1 int Param
	C_MACRO = 0x3,  			// macro "macroname"
	C_MACRO_REF = 0x4,  		// variable reference to a macro to invoke / function pointer
	C_MACRO_REF_LOCAL = 0x5,  	// local variable reference to a macro to invoke / function pointer

	C_GOTO = 0x6,  				// (conditionally) goto offset

								// Operations
	C_LOGICAL_OPERATION = 0x7,	// Test: binary logical operation between stack[0] and stack[1]
	C_BINOP = 0x8,  			// binary operation between stack[0] and stack[1]
	C_ASSIGN = 0x9,  			// assign: a = stackval
	C_ASSIGN_LOCAL_VAR = 0xA,  	// assign: localVars[i] = stackval
	C_ASSIGN_SLOT = 0xB,  		// assign: a.x = stackval or a[x] = stackval
	C_SPREAD = 0xC,				// Spread operator for spreading an array into a sequence of values

	// Push objects onto the stack
	C_PUSH_CHARACTER_LITERAL = 0x10,		// Push character literal. 1 Ascii character follows
	C_PUSH_SMALL_INT_LITERAL = 0x11,		// Push an integer literal in a compacted way (0-255).
	C_PUSH_STRING_LITERAL = 0x12, 			// Push string literal, 1 string Asciistring\0 follows {pad}
	C_PUSH_INTEGER_LITERAL = 0x13,			// Push Integer literal, pad, 1 int Parameter follows
	C_PUSH_LONG_LITERAL  = 0x14, 			// Push long literal, pad, 1 long Parameter follows
	C_PUSH_FLOAT_LITERAL = 0x15, 			// Push floating point literal
	C_PUSH_BOOLEAN_LITERAL   = 0x16,		// Push boolean literal 1 Ascii character follows
	C_PUSH_VARIABLE = 0x18,					// push named var (global var) onto stack
	C_PUSH_LOCAL_VARIABLE = 0x19,			// push local var onto stack
	C_FORM_START = 0x1A, 					// formular with parameters ...
	C_PUSH_NEW_INSTANCE = 0x1C,				// create a new instance of an object.

	// Define parameters and variables
	C_DEFINE_PARAMETER = 0x20,				// create symbol with type and value
	C_DEFINE_LOCAL_VARIABLE = 0x21,			// define a local variable with type and value. Local variables are maintained by index on the heap
											// rather than in the symbol table
	C_DEFINE_VARIABLE = 0x22,				// define a (global) variable with type and value, maintained and accessed by name from the symbol table.

	// Stack manipulation
	C_SET_STACKFRAME = 0x2A,				// start a new stack frame in an invoked method (after parameter have been retrieved)
	C_POP_STACK = 0x2B						// pop one element of the stack. Marks the end of a statement.
} MACROC_INSTRUCTION_OP_CODE;


#define	C_IS1PAR(typ)			 (typ & 0x10)
#define	C_ISCMD(typ)			 (typ >= C_0FUNC && typ <= C_MACRO_REF_LOCAL)
#define C_IS_PUSH_OPCODE(opCode) (opCode >= C_PUSH_CHARACTER_LITERAL && opCode <= C_PUSH_NEW_INSTANCE)

#define	C_NONE			0xFF

/*
 * Option to be passed to a MacroC function, which will in interactive mode open a dialog.
 * Defines, what to do with the dialog, when the corresponding function is executed as part
 * of runnning a MacroC macro.
 */
typedef enum {
	// form should be opened
	FORM_SHOW = 0x40,
	// form " and be prefilled
	FORM_INIT = 0x1,
	// force redraw
	FORM_REDRAW= 0x2
} FORM_PARAMETER_OPTION;

/*
 * interpreter_getParameterSize()
 * typ: the bytecode
 * s: pointer to the bytecode buffer past(!) the opcode (instructionPointer+1)
 */
extern int interpreter_getParameterSize(unsigned char typ, const char* s);

/*
 * Generic coercion function to coerce PKS Edit values.
 */
extern PKS_VALUE interpreter_coerce(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue, PKS_VALUE_TYPE tTargetType);

typedef struct tagCOM_1FUNC {
	unsigned char	typ;			// Used with bytecde C_1FUNC - carries one explicit param to pass
	unsigned char	funcnum;		// index into editor function table _functionTable
	int				func_args;		// Number of arguments actually passed.
	long			p;				// optional parameter to pass
} COM_1FUNC;

typedef struct tagCOM_0FUNC {
	unsigned char typ;				// Used with bytecde C_0FUNC - all params are located on the stack
	unsigned char funcnum;			// index in function table
	int			  func_nargs;		// Number of arguments actually passed.
} COM_0FUNC;

typedef struct tagCOM_MAC {
	unsigned char typ;				// Used with bytecdes C_MACRO, C_MACRO_REF
	unsigned char heapIndex;		// for C_MACRO_REF, if referring to local variable
	int			  func_args;		// Number of arguments actually passed.
	unsigned char name[1];			// 0-term. string padded to even #
} COM_MAC;

/*
 * Describes an editor command.
 */
typedef struct tagCOMMAND {
	int		  c_index;				// the command index used internally.
	COM_1FUNC c_functionDef;		// the actual functionto execute including flags.
	char*	  c_name;				// name of the command used in macros
} COMMAND;

extern COMMAND	_commandTable[];

typedef struct tagCOM_ASSIGN {
	unsigned char 	typ;			// Used with bytecde C_ASSIGN - assign current stack top to a variable or C_ASSIGN_SLOT with slot being stack top and value being next to top.
	unsigned char	name[1];		// variable name of variable assigned
} COM_ASSIGN;

typedef struct tagCOM_DEFINE_SYMBOL {
	unsigned char 	typ;			// Used with bytecdes C_DEFINE_PARAMETER or C_DEFINE_VARIABLE
	unsigned char 	vartype;		// variable type
	unsigned char	heapIndex;		// For non-global vars - the offset into the heap / the slot in the heap.
	unsigned short	value;			// for array type variables the size of the array for parameters the index of the parameter or index for C_ASSIGN_SLOT
	unsigned short 	size;			// size of total structure
	unsigned char	name[1];		// variable name
} COM_DEFINE_SYMBOL;

typedef enum {
	BRA_ALWAYS = 0,					// Branch always independent of a condition / the value on the stack.
	BRA_IF_FALSE = 1,				// Pop the top value off the stack, If that is false -> perform a branch
	BRA_CASE = 2,					// Used to evaluate case-labels of a switch expression. Will compare two stack top values and if they
									// match, perform a branch
	BRA_TOS_IF_FALSE = 3,			// Evaluate top of the stack (peek) and branch if false. Used in && expression for short circuit evaluations.
	BRA_TOS_IF_TRUE = 4				// Evaluate top of the stack (peek) and branch if true. Used in || expression for short circuit evaluations.
} BRANCH_TYPE;

typedef struct tagCOM_GOTO {
	unsigned char typ;				// Used with bytecde C_GOTO*/
	unsigned char branchType;		// BRA_ALWAYS, BRA_IF_FALSE, BRA_CASE, BRA_TOS_IF_FALSE, BRA_TOS_IF_TRUE */
	int		    offset;
} COM_GOTO;

typedef struct tagCOM_BINOP {
	unsigned char typ;				// C_BINOP
	unsigned char op;				// see above BIN_ADD etc...
	PKS_VALUE_TYPE targetType;		// for cast operators this is the target type.
} COM_BINOP;

typedef struct tagCOM_CHAR1 {
	unsigned char typ;				// C_PUSH_CHARACTER_LITERAL, C_PUSH_SMALL_INT_LITERAL, C_PUSH_LOCAL_VAR, C_ASSIGN_LOCAL_VAR
	unsigned char val;
} COM_CHAR1;

typedef struct tagCOM_CHAR1 COM_FURET;

typedef struct tagCOM_STOP {				// C_STOP, C_SPREAD, C_ASSIGN_SLOT - no real param needed
	unsigned char typ;
	unsigned char unused;
} COM_STOP;

typedef struct tagCOM_INT1 {
	unsigned char	typ;			// C_PUSH_INTEGER_LITERAL, C_PUSH_LITERAL int, C_PUSH_NEW_INSTANCE
	unsigned char	c_valueType;
	int				val;
} COM_INT1;

typedef struct tagCOM_FLOAT1 {
	unsigned char typ;				// C_PUSH_FLOAT int
	unsigned char c_valueType;
	double		  val;
} COM_FLOAT1;

typedef struct tagCOM_LONG1 {
	unsigned char	typ;			// C_PUSH_LONG_LITERAL, C_PUSH_FLOAT int
	unsigned char	c_valueType;
	intptr_t  		val;
} COM_LONG1;

typedef struct tagCOM_STRING1 {
	unsigned char typ;				// C_STRING1, C_PUSH_VARIABLE
	unsigned char s[1];				// 0-term. string padded to even #
} COM_STRING1;

typedef struct tagCOM_FORM {
	unsigned char	typ;			// CMD_FORMSTART
	unsigned char	options;		// FORM_SHOW
	int				nfields;		// # of fields in formular
} COM_FORM;

typedef union c_seq {
	COM_1FUNC* fu1;
	COM_0FUNC* fu0;
	COM_MAC* ma;
	COM_GOTO* go;
	COM_FORM* fo;
	COM_STRING1* st1;
	COM_LONG1* lo1;
	COM_INT1* in1;
	COM_CHAR1* ch1;
	unsigned char* sp;
} COM_SEQ;

/*
 * Describes a bytecode buffer for being manipulated or read.
 */
typedef struct BYTECODE_BUFFER {
	unsigned char* bb_start;
	unsigned char* bb_current;
	unsigned char* bb_end;
} BYTECODE_BUFFER;

typedef struct tagMACRO_PARAM MACRO_PARAM;

typedef struct tagCOMPILER_INPUT_STREAM {
	void* cis_pointer;												// parameter for the stream implementation. Maybe for instance of type FILE*
																	// or point to some other structure necessary to handle the input.
	int (*cis_next)(struct tagCOMPILER_INPUT_STREAM* pStream);		// Read one character from the input stream
	void (*cis_close)(struct tagCOMPILER_INPUT_STREAM* pStream);	// Close the input stream
} COMPILER_INPUT_STREAM;

typedef struct tagCOMPILER_CONFIGURATION {
	COMPILER_INPUT_STREAM* cb_stream;
	int (*cb_insertNewMacro)(MACRO_PARAM *pParam);
	void  (*cb_showStatus)(char* s, ...);
	int  cb_numberOfFilesCompiled;									// Book keeping of the number of files compiled.
	int  cb_numberOfErrors;											// Book keeping of the total number of errors during the compilation
	int  cb_numberOfWarnings;										// Book keeping of the total number of warnings during the compilation
	BOOL cb_topLevelFile;											// set to true for "top-level" files to compile, must be false for
																	// files compiled as prerequisites for other files.
	const char* cb_source;											// The name of the source file
	const char* cb_errorFile;										// The name of the file, where the errors and warnings are recorded.
	void* cb_dependencies;											// Dependencies / required files also to load detected during analysis
	int   cb_numberOfDependencies;									// Used as a marked to find out, whether additional dependencies must be loaded first.
	int   cb_postponed;												// Set to TRUE, if compilation was postponed as required files needed to be compiled first.
} COMPILER_CONFIGURATION;

extern COMPILER_CONFIGURATION* _compilerConfiguration;

/*
 * Creates one object memory and optionally copies the provided native
 * data into the object memory. Currently two special cases for input are supported:
 * the value is a string - input is interpreted as zero terminated byte string char*,
 * the value is an arraylist of strings - input is interpreted as an ARRAY_LIST containing
 * strings. nInitialSize is used for creating arrays - otherwise one may safely pass 0.
 */
extern PKS_VALUE memory_createObject(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE sType, int nInitialSize, const void* pInput);

/*
 * Return the logical size of one object - either the number of chars for a string or the number of instance
 * variables or the length of an array. Return 0 for other primitive types.
 */
extern int memory_size(PKS_VALUE v);

/*
 * Set a nested pointer of a value at slot nIndex. Can be used to store "handles" in an object rather
 * than a nested object / value.
 */
extern int memory_setNestedPointer(PKS_VALUE vTarget, int nIndex, TYPED_OBJECT_POINTER vPointer);

/*
 * Access a nested object pointer of a value at slot nIndex.
 */
extern TYPED_OBJECT_POINTER memory_getNestedObjectPointer(PKS_VALUE v, int nIndex);

/*
 * Set a nested object of a value at slot nIndex.
 */
extern int memory_setNestedObject(PKS_VALUE vTarget, int nIndex, PKS_VALUE vElement);


/* * Try to get rid of some allocated memory not referenced by the stack any more.
 * This method must be invoked with care and not in the middle of an operation
 * allocating more objects not yet referenced or the objects just created might
 * get disposed to early.
 */
void memory_garbaggeCollect(EXECUTION_CONTEXT* pContext);

/*
 * Free the allocated object memory and all objects remaining in the memory.
 */
extern void memory_destroy();

/*
 * Get a string pointer to the actual string for a value.
 */
extern const char* memory_accessString(PKS_VALUE v);

/*
 * Access the nested object of a value at slot nIndex.
 */
extern PKS_VALUE memory_getNestedObject(PKS_VALUE, int nIndex);

/*
 * Set a nested object using a key (must be a string) assuming the target object is a map.
 */
extern int memory_atPutObject(PKS_VALUE vTarget, PKS_VALUE vKey, PKS_VALUE vElement);

/*
 * Access an object by string key in a map.
 */
extern PKS_VALUE memory_atObject(PKS_VALUE vTarget, PKS_VALUE vKey);

/*
 * Add one object to an array type object.
 */
extern int memory_addObject(EXECUTION_CONTEXT* pContext, PKS_VALUE *vObject, PKS_VALUE vElement);

/*
 * Returns an array object with all keys contained in the map type value passed as single argument.
 */
extern PKS_VALUE memory_mapKeys(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs);

/*
 * If the passed value is of type "handle" (file handle, editor handle etc...), it is assumed,
 * that one foreign memory nested object exists, which can be uwrapped using this method. In
 * other words: return the FILE* or WINFO* objects associated with the given value.
 */
void* memory_handleForValue(PKS_VALUE vValue);

/*
 * Creates a "handle" type PKSMacroC object, which wraps a native C pointer to be passed around
 * in macroC code.
 */
extern PKS_VALUE memory_createHandleObject(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE tType, void* p);

/*
 * Returns an array object with all values contained in the map type value passed as single argument.
 */
extern PKS_VALUE memory_mapValues(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs);

/*
 * Returns an array object with all MAP_ENTRY objects in the map type value passed as single argument.
 */
extern PKS_VALUE memory_mapEntries(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs);

/*
 * Return the "index" of one object in a give array type object.
 */
extern int memory_indexOf(PKS_VALUE vArray, PKS_VALUE vOther);

/*
 * Open the debug window.
 */
typedef enum { DA_STEP, DA_ABORT, DA_CONTINUE } DEBUG_ACTION;
extern DEBUG_ACTION debugger_open(EXECUTION_CONTEXT* pContext, char* pszError);

/*
 * Returns the index for a given types property name or -1, if that cannot be determined.
 */
extern int types_indexForProperty(PKS_VALUE_TYPE t, const char* pszPropertyName, PKS_VALUE_TYPE *tProperty);

/*
 * Used for structured data types to define the number of instance variables.
 */
extern int types_getObjectSize(PKS_VALUE_TYPE t);

/*
 * Returns true, if the passed value type is a "structured" type.
 */
extern BOOL types_isStructuredType(PKS_VALUE_TYPE t);

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for a native PKSMacroC function.
 */
extern const char* macrodoc_helpForNativeFunction(const char* pszName, NATIVE_FUNCTION* pFunc);

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for an enum value.
 */
extern const char* macrodoc_helpForEnumValue(const char* pszName, PARAMETER_ENUM_VALUE* pEnumValue);

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for a user defined macro.
 */
extern const char* macrodoc_helpForMacro(const char* pszName, MACRO* pMacro);

/*
 * Returns the pointer to an allocated string containing the documentation for
 * a PKSMacroC type with the given name 'pszType'.
 */
extern const char* macrodoc_helpForType(const char* pszType, void* pszUnused);

/*
 * Returns a pointer to an allocated string containing the contents of the documentation
 * "pointer to" by a macrodoc hyperlink reference (macroref://typename or the like).
 */
extern const char* macrodoc_helpForHyperlink(const char* pszUrl);

#define PKSMACROCVM_H
#endif

