/*
 * pksmacro.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: macro language related definitions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef	_EDFUNCS_H

#include <windows.h>

#ifndef DIM
#define	DIM(tab)		(sizeof(tab)/sizeof(tab[0]))
#endif

#define	RECORDERSPACE	512

#define MACRO_NAMESPACE_DEFAULT "global"

/* "macro reference" types */
#define	CMD_NONE		0
#define	CMD_CMDSEQ		0x2
#define	CMD_MACRO		0x3
#define CMD_NAMESPACE   0x10				// Used inside macro file format to mark name spaces.

typedef unsigned char 	MACROREFTYPE;
typedef unsigned char  	MACROREFIDX;
typedef struct tagMACROREF {
	MACROREFTYPE	typ;
	MACROREFIDX		index;
} MACROREF;

#define MACROREF_TO_INTPTR(m)	(intptr_t)(((unsigned long)m.typ<<16) + (unsigned long)m.index)
#define INTPTR_TO_MACROREF(m)	(MACROREF){.typ = (unsigned char)((unsigned long)m >> 16), .index = (unsigned char)((unsigned long)m&0xFFFF)}

typedef enum { MS_GLOBAL = 0, MS_LOCAL = 1 } MACRO_SCOPE;

typedef struct tagMACRO {
	unsigned char  mc_isNamespace;			// 1 for namespace macros.
	unsigned char  mc_isInitialized;		// book keeping of namespace initialization (had the namespace code be executed yet?).
	MACRO_SCOPE    mc_scope;				// The scope of a macro
	int			   mc_actionFlags;		// The flags defining, when the macro is "enabled"
	unsigned char  mc_namespaceIdx;			// Index of the corresponding name space (or 0 for default namespace).
	unsigned char  mc_returnType;			// The PKS_VALUE_TYPE return type.
	unsigned char* mc_name;					// the macro name
	unsigned char* mc_comment;				// the macro comment
	int			   mc_numberOfLocalVars;			// # of local variables - used to allocate local variable heap.
	unsigned int mc_bytecodeLength;			// total size of the macro byte code in bytes.
	unsigned char* mc_bytecodes;			// the actual bytecode of the macro
} MACRO;

typedef struct tagMACRO_PARAM {
	unsigned char mp_namespaceIdx;
	unsigned char mp_returnType;
	MACRO_SCOPE mp_scope;
	int mp_actionFlags;
	int mp_type;
	char* mp_name;
	char* mp_comment;
	const char* mp_sourceFile;
	int mp_numberOfLocalVariables;
	size_t mp_bytecodeLength;
	void* mp_buffer;
} MACRO_PARAM;

#ifdef HASHMAP_H
typedef struct tagANNOTATION {
	struct tagANNOTATION* a_next;
	const char* a_name;
	HASHMAP* a_values;
} ANNOTATION;
/*
 * An annotation on a macro was detected during compilation. Try to apply to the macro parameter
 * used to later create the macro and record important information.
 */
extern void macro_processAnnotations(MACRO_PARAM* pParam, ANNOTATION* pAnnotation);
#endif

#define	MAC_COMMENTLEN		256

#define	MAC_NAMELEN			32				// no real limitation - only used in the UI.
#define	MAC_NAME(mp)		((mp)->mc_name)
#define	MAC_DATA(mp)		((unsigned char *)mp->mc_bytecodes)
#define	MAC_COMMENT(mp)		((mp)->mc_comment)

typedef struct tagCOMMAND COMMAND;
extern COMMAND		_commandTable[];
extern int			_commandTableSize;

#define	IDM_CMDNAME		1024
#define	IDM_LOWENUMNAME	2512

/*
 *  PARAMETERS (RECORDING DIALOGS) ---------------------------------------
 */
#define	P_MAYOPEN				0x1
#define	P_MAYPRE				0x2

typedef struct des {
	char		cmd_type;
	union {
		unsigned char  *s;
		char			*c;
		double			*d;
		unsigned int	*i;
		long			*l;
	} p;
} ITEMS[];

typedef struct params {
	int		nel;
	int		flags;
	struct	des *el;
} PARAMS;

extern int interpreter_openDialog(PARAMS *p);
extern long long cdecl interpreter_executeFunction(int num, intptr_t* pStack);
extern int interpreter_canExecuteNativeFunction(int num, long long pParam, int warn);
extern int interpreter_canExecuteMacro(int num, int warn);

/*------------------------------------------------------------
 * macro_getByIndex()
 * Return a macro by its internal index.
 */
extern MACRO* macro_getByIndex(int idx);

/*---------------------------------
 * macro_hasNamespace()
 * Return true. if the namespace with the given name had been defined.
 */
extern BOOL macro_hasNamespace(const char* name);

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
extern int macro_getInternalIndexByName(const char* name);
/*
 * Returns the number of macros defined
 */
extern int macro_getNumberOfMacros();

typedef enum {
	LMT_FUNCTION = 0x1,				// return the names of matching native functions
	LMT_GLOBAL_MACROS = 0x2,		// return the names of matching macros
	LMT_STATIC_MACROS = 0x4			// must be included to also return static macro names
} LIST_MACRO_TYPES;

#ifdef ARRAYLIST_H
/*
 * Returns a union of all namespaces and macros.
 * The returned arraylist must be freed by the caller.
 */
extern ARRAY_LIST* macro_getNamespacesAndMacros();

/*
 * List the names of known macros of a given type (used as flag).
 */
extern ARRAY_LIST* macro_getFunctionNamesMatching(const char* pszPattern, LIST_MACRO_TYPES lTypes);

/*
 * A macro source file requires a namespace to be defined (loaded). If that is not the case
 * load it first relative to the given source file. Returns 0, if the namespace cannot be located.
 */
extern int compiler_requireNamespaceOrFilename(ARRAY_LIST* pSources, const char* pszSourcefile, const char* pszNamespacename);

#endif

/*------------------------------------------------------------
 * macro_defineNamespace()
 * Define a new namespace object.
 */
extern int macro_defineNamespace(MACRO_PARAM* mpParam);

/*
 * Define the namespace initializer code.
 */
extern void macro_defineNamespaceInitializer(int nNamespaceIdx, const char* pBytes, size_t nByteLen);

/*
 * Returns a namespace object given its logical index.
 */
extern MACRO* macro_getNamespaceByIdx(int idx);

/*
 * Returns the label to use in a user interface to start the macro passed.
 * This is by default calculated from the macro name.
 */
extern void macro_getLabelFor(MACRO* mp, char* pszBuf, size_t nBufferSize);

typedef enum { DM_CODE, DM_INSTRUCTIONS} DECOMPILATION_MODE;
extern int 		decompile_saveMacrosAndDisplay(char* macroname, DECOMPILATION_MODE nMode);
extern int 		print_saveMouseBindingsAndDisplay(void);
extern int 		print_saveKeyBindingsAndDisplay(void);
extern int 		print_saveMenuBindingsAndDisplay(void);

/*
 * Decompile a macro with the given mode and return the result of the decompilation
 * in an array list. The array list must be destroyed by the caller using arraylist_destroyStringList.
 * The decompilation with also return the line number containing the instruction pointer passed.
 */
#ifdef ARRAYLIST_H
ARRAY_LIST* deccompile_macroNamed(const char* pszName, DECOMPILATION_MODE nMode, const char* pszInstructionPointer, int* pNLine);
#endif

/*
 * Generate the signature of a macro into the given string buffer. If a pointer to the instructions is passed (not 0),
 * it is assumed that this points to the 1st parameter definition, if that is 0, the function tries to find the parameter
 * definitions in the byte code.
 */
#ifdef STRINGUTIL_H
extern unsigned char* decompile_printMacroSignature(MACRO* mp, STRING_BUF* pBuf, unsigned char* pInstructionPointer);
#endif

/*------------------------------------------------------------
 * macro_autosaveAllBindings()
 * Save all changed macro bindings and new macros to the corresponding file.
 * If warnflag is specified ask user, whether macros should be saved.
 */
extern void macro_autosaveAllBindings(int warnFlag);

/*------------------------------------------------------------
 * macro_deleteByName()
 * Delete the macro (including all its bindings) with the given name. Return
 * 1 if successfully deleted.
 */
extern int macro_deleteByName(char* name);

/*------------------------------------------------------------
 * macro_validateMacroName()
 * Check, whether a mcro name is valid and whether it exists already.
 */
extern int macro_validateMacroName(char* name, int origidx, int bOverride);


/*------------------------------------------------------------
 * macro_readCompiledMacroFile()
 * Read new bindings from a file. If the current bindings are "dirty" they
 * are flushed before.
 */
extern int macro_readCompiledMacroFile(char* fn);

/*------------------------------------------------------------
 * macro_createWithParams()
 * create a new macro with a name, a comment and the actual byte codes
 * to execute.
 */
extern MACRO* macro_createWithParams(MACRO_PARAM* pParam);

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
extern int macro_insertNewMacro(MACRO_PARAM* mpParam);


/*------------------------------------------------------------
 * macro_renameAndChangeComment()
 */
extern void macro_renameAndChangeComment(int nIndex, char* szName, char* szComment);

/*------------------------------------------------------------
 * recorder_toggleRecording()
 * start/stops the macro recorder.
 */
extern int recorder_toggleRecording(void);

/*--------------------------------------------------------------------------
 * macro_translateToOriginalMenuIndex()
 * try to find an internal command/standard menu binding, which
 * allows us to display an appropriate help for synthetic menus
 */
extern int macro_translateToOriginalMenuIndex(int wParam);

/*---------------------------------*/
/* macro_getKeyBinding()				*/
/*---------------------------------*/
extern MACROREF* macro_getKeyBinding(WPARAM key);

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
extern long long macro_executeMacro(MACROREF* mp);

/*------------------------------------------------------------
 * macro_onCharacterInserted()
 */
extern int macro_onCharacterInserted(WORD c);

/*---------------------------------*/
/* macro_executeByName()				*/
/*---------------------------------*/
extern int macro_executeByName(char* name);

/*
 * Convert a menu command to a macroref. It is assumed, that the menu command
 * is an encoded macroref structure.
 */
extern MACROREF* macro_translateMenuCommand(int nCommand);

/*
 * Returns the keyboard binding text for a given internal command.
 */
extern char* macro_getKeyText(const char* pszActionContext, int nCmd);

/*---------------------------------*/
/* recorder_recordOperation()				*/
/*---------------------------------*/
extern int recorder_recordOperation(PARAMS* pp);

#ifdef _WINFO_H
/*
 * Execute the provided command/macro assuming the execution of the action was triggered
 * by the used with some kind of pointing interaction (clicking with the mouse or from a popup menu).
 * Return 1 if successful.
 */
extern int macro_executeWithPosition(WINFO* wp, MACROREF* pRef, POINT pt);

/*---------------------------------
 * macro_onMenuAction()
 * Invoke the macro bound to a menu index.
 *---------------------------------*/
extern int macro_onMenuAction(WINFO* wp, int menunum, POINT* aPositionClicked);

#endif

/*------------------------------------------------------------
 * command_getTooltipAndLabel()
 * Returns the label and tooltip for a given command described by command.
 * The result is copied into the space passed with szTooltip.
 * If szLabel is not NULL, it will contain a longer text used in a tooltip or a help context.
 */
extern char* command_getTooltipAndLabel(MACROREF command, char* szTooltip, char* szLabel);

/*
 * Returns a tool tip for a given menu id.
 */
extern char* macro_getMenuTooltTip(int dMenuId);

/*--------------------------------------------------------------------------
 * macro_showHelpForMenu()
 * Display help for a given menu id
 */
extern void macro_showHelpForMenu(int dMenuId);

/*--------------------------------------------------------------------------
 * macro_getCmdIndexByName()
 */
extern int macro_getCmdIndexByName(const char* name);

/*
 * Returns a macro by a given index or NULL if the index does not
 * lie in the valid index range. Can be used to iterate all macros.
 */
extern char* macro_getCommandByIndex(int nIndex);

/**
* Execute a macro given a single line text to execute.
* pszCode ist the string of code to execute. If bUnescape is true, we treat \ and " special
* and assume, the command to execute was passed on the command line (and will be escaped).
* 'pszContext' is the name of the context in which the execution will be performed.
 */
extern int macro_executeSingleLineMacro(const char* pszCode, BOOL bUnescape, const char* pszContext);

extern void recorder_stopAutoInsertRecording(void** pRecordBufferLow, void** pRecordBufferHigh);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
extern int macro_isParameterStringType(unsigned char typ);

#ifndef DIM
#define DIM(x)		sizeof(x)/sizeof(x[0])
#endif

typedef int BOOL;

extern const char* macro_loadStringResource(int nId);
extern void 	   macro_showStatus(char* s, ...);

/*---------------------------------
 * macro_executeMacroByIndex()
 * Execute a macro given its logical
 * internal macro number.
 */
extern long long macro_executeMacroByIndex(int macroindex);

/*
 * print the current mouse bindings to a file and display them to the user.
 */
extern int print_saveMouseBindingsAndDisplay(void);

/*---------------------------------
 * macro_createFileAndDisplay()
 * Invoke a callback to generate the contents of a file with
 * the given file name. If successful, open the file in PKS Edit.
 */
#ifdef _FILE_DEFINED
extern BOOL macro_createFileAndDisplay(char* fn, long (*callback)(FILE* fp));
#endif

/*--------------------------------------------------------------------------
 * print_saveMenuBindingsAndDisplay()
 */
extern int print_saveMenuBindingsAndDisplay(void);

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
extern long long macro_readWriteWithFileSelection(int wrflag);

/*
 * Return the logical function number (index into functionTable) of the function invoked by the command with the given index.
 */
extern int macro_getFunctionNumberForCommand(int nCommand, long long *llParam);

#define	_EDFUNCS_H
#endif

