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

#define	MAXMACRO		64

#ifndef DIM
#define	DIM(tab)		(sizeof(tab)/sizeof(tab[0]))
#endif

#define	RECORDERSPACE	512

/* "macro reference" types */
#define	CMD_NONE		0
#define	CMD_CMDSEQ		0x2
#define	CMD_MACRO		0x3

typedef unsigned char 	MACROREFTYPE;
typedef unsigned char  	MACROREFIDX;
typedef struct tagMACROREF {
	MACROREFTYPE	typ;
	MACROREFIDX		index;
} MACROREF;

#define MACROREF_TO_INTPTR(m)	(intptr_t)(((unsigned long)m.typ<<16) + (unsigned long)m.index)
#define INTPTR_TO_MACROREF(m)	(MACROREF){.typ = (unsigned char)((unsigned long)m >> 16), .index = (unsigned char)((unsigned long)m&0xFFFF)}

typedef struct tagMACRO {
	unsigned char namelen;
	unsigned char dstart;
	unsigned 	    size;
	unsigned char name[2];
} MACRO;

#define	MAC_COMMENTLEN		220
#define	MAC_NAMELEN		32
#define	MAC_NAME(mp)		((mp)->name)
#define	MAC_SIZE(size)		((size) + sizeof(MACRO))
#define	MAC_DATA(mp)		((unsigned char *)mp+mp->dstart)
#define	MAC_COMMENT(mp)	((unsigned char *)mp+sizeof *mp+mp->namelen)

typedef struct tagCOMMAND COMMAND;
extern COMMAND		_commandTable[];
extern MACRO*		_macroTable[];
extern int			_commandTableSize,_macroTableSize;

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

typedef struct tagCOMPILER_CONFIGURATION {
	int (*cb_insertNewMacro)(char* name, char* comment, char* macdata, int size);
	void  (*cb_showStatus)(char* s, ...);
	BOOL cb_openErrorList;
	char* cb_source;
} COMPILER_CONFIGURATION;

extern COMPILER_CONFIGURATION* _compilerConfiguration;

extern int macro_openDialog(PARAMS *p);
extern int cdecl interpreter_executeFunction(int num, intptr_t p1, intptr_t p2, void *s1, void *s2, void *s3);

/*------------------------------------------------------------
 * macro_getByIndex()
 * Return a macro by its internal index.
 */
extern MACRO* macro_getByIndex(int idx);

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
extern int macro_getInternalIndexByName(const char* name);

typedef enum { DM_CODE, DM_INSTRUCTIONS} DECOMPILATION_MODE;
extern int 		decompile_saveMacrosAndDisplay(char* macroname, DECOMPILATION_MODE nMode);
extern int 		print_saveMouseBindingsAndDisplay(void);
extern int 		print_saveKeyBindingsAndDisplay(void);
extern int 		print_saveMenuBindingsAndDisplay(void);

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

/*--------------------------------------------------------------------------
 * macro_selectDefaultBindings()
 * Select the default key- / menu- / mouse bindings for PKS Edit.
 */
extern void macro_selectDefaultBindings(void);

/*------------------------------------------------------------
 * macro_readBindingsFromFile()
 * Read new bindings from a file. If the current bindings are "dirty" they
 * are flushed before.
 */
extern int macro_readBindingsFromFile(char* fn);

/*------------------------------------------------------------
 * macro_createWithParams()
 * create a new macro with a name, a comment and the actual byte codes
 * to execute.
 */
extern MACRO* macro_createWithParams(char* szName, char* szComment, char* bData, int size);

/*------------------------------------------------------------
 * macro_renameAndChangeComment()
 */
extern void macro_renameAndChangeComment(int nIndex, char* szName, char* szComment);

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
extern int macro_insertNewMacro(char* name, char* comment, char* macdata, int size);

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
extern int macro_executeMacro(MACROREF* mp);

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

/**
 * macro_returnString()
 * Return the passed String to the macro interpreter so it can be used for further processing.
 */
extern void macro_returnString(char* string);

extern void recorder_stopAutoInsertRecording(void** pRecordBufferLow, void** pRecordBufferHigh);

/*--------------------------------------------------------------------------
 * macro_isParameterStringType()
 */
extern int macro_isParameterStringType(unsigned char typ);

/*--------------------------------------------------------------------------
 * macro_isParameterFloatType()
 */
extern int macro_isParameterFloatType(unsigned char typ);

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
extern int macro_executeMacroByIndex(int macroindex);

/*
 * print the current mouse bindings to a file and display them to the user.
 */
extern int print_saveMouseBindingsAndDisplay(void);

extern int interpreter_getDollarParameter(intptr_t offset, int* typ, intptr_t* value);

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
extern int macro_readWriteWithFileSelection(int wrflag);

/*
 * Return the logical function number (index into functionTable) of the function invoked by the command with the given index.
 */
extern int macro_getFunctionNumberForCommand(int nCommand, long long *llParam);

#define	_EDFUNCS_H
#endif

