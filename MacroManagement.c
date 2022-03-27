/*
 * MacroManagement.c
 *
 * Execute, edit, display and manage the macros defined in PKS-Edit
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 11/89
 */

#include "alloc.h"
#include <tos.h>
#include "customcontrols.h"
#include <windowsx.h>
#include <string.h>
#include "trace.h"
#include "documentmodel.h"
#include "edierror.h"
#include "pksedit.h"
#include "winfo.h"
#include "winterf.h"
#include "regexp.h"
#include "fileutil.h"
#include "fileselector.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "funcdef.h"
#include "dial2.h"
#include "resource.h"
#include "xdialog.h"
#include "arraylist.h"
#include "errordialogs.h"
#include "stringutil.h"
#include "winutil.h"
#include "editorconfiguration.h"
#include "funcdef.h"
#include "actionbindings.h"

extern long		_multiplier;

extern char *		rsc_rdmacros(char *param, unsigned char *p, unsigned char *pend);
extern char * 		mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
/*
 * Loads the default action bindings (keys and mouse) for PKS-Edit from the PKS_SYS directory.
 */
extern void bindings_loadActionBindings();
extern void st_switchtomenumode(BOOL bMenuMode);
/*
 * Initialize a namespace.
 */
extern int interpreter_initializeNamespace(MACRO* mpNamespace);

static ARRAY_LIST*		_macroTable;
static ARRAY_LIST*		_namespaces;
int						_macrosWereChanged;

static char*	_selectedMacroName;
static MACROREF	currentSelectedMacro;
static FSELINFO _seqfsel = {	"","pksedit.mac", "*.mac" };

static void macro_destroyMacro(MACRO* mp) {
	free(mp->mc_bytecodes);
	free(mp->mc_comment);
	free(mp->mc_name);
	free(mp);
}

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
static int macro_getIndexInTableByName(ARRAY_LIST* pList, const char* name)
{
	if (!pList) {
		return -1;
	}
	int i;
	int nMax = (int)arraylist_size(pList);
	MACRO* mp;

	if (name)
		for (i = 0; i < nMax; i++) {
			if ((mp = (MACRO*)arraylist_get(pList, i)) != 0 &&
				strcmp(MAC_NAME(mp), name) == 0)
				return i;
		}
	return -1;
}


/* 
 * Returns a union of all namespaces and macros.
 * The returned arraylist must be freed by the caller.
 */
ARRAY_LIST* macro_getNamespacesAndMacros() {
	ARRAY_LIST* pResult = arraylist_create(70);
	if (_namespaces) {
		ARRAY_ITERATOR pIter = arraylist_iterator(_namespaces);
		while (pIter.i_buffer < pIter.i_bufferEnd) {
			arraylist_add(pResult, *pIter.i_buffer++);
		}
	}
	if (_macroTable) {
		ARRAY_ITERATOR pIter = arraylist_iterator(_macroTable);
		while (pIter.i_buffer < pIter.i_bufferEnd) {
			arraylist_add(pResult, *pIter.i_buffer++);
		}
	}
	return pResult;
}

static int macro_compileRegularExpression(const char* pszPattern, char* pExpressionBuf, size_t nExpBufSize, RE_PATTERN* pResult) {
	RE_OPTIONS options;
	memset(&options, 0, sizeof options);
	options.expression = (char*)pszPattern;
	options.patternBuf = pExpressionBuf;
	options.eof = 0;
	options.endOfPatternBuf = pExpressionBuf + nExpBufSize;
	options.flags = RE_DOREX | RE_NOADVANCE;
	return regex_compile(&options, pResult);
}


/*
 * List the names of all macros/functions matching a pattern.
 */
ARRAY_LIST* macro_getFunctionNamesMatching(const char* pszPattern, LIST_MACRO_TYPES lTypes) {
	char ebuf[256];
	ARRAY_LIST* pResult = arraylist_create(64);
	RE_PATTERN rePattern;
	RE_MATCH result;
	if (!macro_compileRegularExpression(pszPattern, ebuf, sizeof ebuf, &rePattern)) {
		// Error handling?
		return pResult;
	}
	if ((lTypes & (LMT_GLOBAL_MACROS | LMT_STATIC_MACROS)) && _macroTable) {
		ARRAY_ITERATOR pIter = arraylist_iterator(_macroTable);
		while (pIter.i_buffer < pIter.i_bufferEnd) {
			MACRO* mp = *pIter.i_buffer++;
			if (((mp->mc_scope == MS_GLOBAL && (lTypes & LMT_GLOBAL_MACROS)) || (mp->mc_scope == MS_LOCAL && (lTypes & LMT_STATIC_MACROS)))
					&& regex_match(&rePattern, mp->mc_name, NULL, &result)) {
				arraylist_add(pResult, _strdup(mp->mc_name));
			}
		}
	}
	if (lTypes & LMT_FUNCTION) {
		for (int i = 0; i < _functionTableSize; i++) {
			if (regex_match(&rePattern, _functionTable[i].f_name, NULL, &result)) {
				arraylist_add(pResult, _strdup(_functionTable[i].f_name));
			}
		}
	}
	return pResult;
}
/*
 * Lookup the index of a named macro namespace. If the namespace is not yet defined, define one on the fly. 
 */
int macro_lookupNamespace(const char* pszNamespaceName) {
	MACRO* mp;
	if (!_namespaces) {
		_namespaces = arraylist_create(13);
		mp = calloc(1, sizeof * mp);
		mp->mc_name = _strdup(MACRO_NAMESPACE_DEFAULT);
		mp->mc_isNamespace = TRUE;
		arraylist_add(_namespaces, mp);
	}
	int idx = macro_getIndexInTableByName(_namespaces, pszNamespaceName);
	if (idx < 0) {
		mp = calloc(1, sizeof * mp);
		mp->mc_name = _strdup(pszNamespaceName);
		mp->mc_isNamespace = TRUE;
		arraylist_add(_namespaces, mp);
		idx = (int) arraylist_size(_namespaces) - 1;
	}
	return idx;
}

/*
 * Define the namespace initializer code.
 */
void macro_defineNamespaceInitializer(int nNamespaceIdx, const char* pBytes, size_t nByteLen) {
	if (!_namespaces || nNamespaceIdx >= arraylist_size(_namespaces)) {
		return;
	}
	MACRO* mp = arraylist_get(_namespaces, nNamespaceIdx);
	mp->mc_bytecodeLength = (unsigned int)nByteLen;
	free(mp->mc_bytecodes);
	mp->mc_bytecodes = malloc(mp->mc_bytecodeLength);
	memmove(mp->mc_bytecodes, pBytes, nByteLen);
	// treat as un-initialized, after the bytecode has changed.
	mp->mc_isInitialized = FALSE;
}

/*------------------------------------------------------------
 * macro_defineNamespace()
 * Define a new namespace object.
 */
int macro_defineNamespace(MACRO_PARAM* mpParam) {
	int    i;

	i = macro_lookupNamespace(mpParam->mp_name);
	macro_defineNamespaceInitializer(i, mpParam->mp_buffer, mpParam->mp_bytecodeLength);
	_macrosWereChanged = 1;
	return i;
}

/*
 * Returns a namespace object given its logical index.
 */
MACRO* macro_getNamespaceByIdx(int idx) {
	if (!_namespaces) {
		return 0;
	}
	return (MACRO*)arraylist_get(_namespaces, idx);
}

/*
 * Convert a menu command to a macroref. It is assumed, that the menu command
 * is an encoded macroref structure.
 */
MACROREF* macro_translateMenuCommand(int nCommand) {
	static MACROREF macroref;

	macroref.typ = nCommand >> 16;
	if (macroref.typ) {
		macroref.index = nCommand & 0xFFFF;
		return &macroref;
	}
	return NULL;
}

/*------------------------------------------------------------
 * macro_getByIndex()
 * Return a macro by its internal index.
 */
MACRO *macro_getByIndex(int idx)
{
	return _macroTable ? arraylist_get(_macroTable, idx) : 0;
}

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
int macro_getInternalIndexByName(const char *name)
{
	return macro_getIndexInTableByName(_macroTable, name);
}

/*---------------------------------
 * macro_hasNamespace()
 * Return true. if the namespace with the given name had been defined.
 */
BOOL macro_hasNamespace(const char* name)
{
	return macro_getIndexInTableByName(_namespaces, name) >= 0;
}

/*------------------------------------------------------------
 * macro_writeMacroBindingsToFile()
 */
#define	RSC_MACROS		0x1
#define	RSC_SINGLEMACRO	0x2
static int macro_writeMacroBindingsToFile(int whichresource, char *name)
{
	int 		ret = 1,fd;

	if ((fd = rsc_create(name,1)) < 0) {
		error_openingFileFailed(name,fd);
		return 0;
	}

	if ((whichresource & RSC_SINGLEMACRO) == 0)
		_selectedMacroName = 0;

	if (whichresource & (RSC_MACROS|RSC_SINGLEMACRO)) {
		if (rsc_put(fd,"MACROS","",0,rsc_wrmacros,_linebuf,(long)FBUFSIZE, _selectedMacroName) == 0) {
			error_showErrorById(IDS_MSGNODISKSPACE);
			ret = 0;
			goto done;
		}
	}

done:Fclose(fd);
	_selectedMacroName = 0;
	return ret;
}

/*------------------------------------------------------------
 * macro_autosaveAllBindings()
 * Save all changed macro bindings and new macros to the corresponding file.
 * If warnflag is specified ask user, whether macros should be saved.
 */
void macro_autosaveAllBindings(int warnFlag)
{
	char fn[EDMAXPATHLEN];

	if (_macrosWereChanged) {
		if (warnFlag == 0) {
			string_concatPathAndFilename(fn,_seqfsel.path,_seqfsel.fname);
			if (macro_writeMacroBindingsToFile(RSC_MACROS,fn))
				_macrosWereChanged = 0;
		} else {
			if (error_displayYesNoConfirmation(IDS_MSGSAVESEQ) == IDYES) {
				macro_readWriteWithFileSelection(1);
			}
		}
	}
	_macrosWereChanged = 0;
}

/*------------------------------------------------------------
 * macro_deleteByName()
 * Delete the macro (including all its bindings) with the given name. Return
 * 1 if successfully deleted.
 */
int macro_deleteByName(char *name)
{	int idx;

	if ((idx = macro_getInternalIndexByName(name)) >= 0) {
		bindings_deleteKeyBindingsForMacroRef((MACROREF) { .typ = CMD_MACRO, .index = idx });
		MACRO* mp = macro_getByIndex(idx);
		macro_destroyMacro(mp);
		arraylist_removeAt(_macroTable, idx);
		_macrosWereChanged = 1;
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * macro_validateMacroName()
 * Check, whether a mcro name is valid and whether it exists already.
 */
int macro_validateMacroName(char *name, int origidx, int bOverride)
{	int index;

	if (name[0] == 0)
		return 0;
	if (((index = macro_getInternalIndexByName(name)) >= 0 && index != origidx)) {
		if (origidx < 0 && !bOverride) {
			error_showErrorById(IDS_MSGMACDOUBLEDEF);
			return 0;
		} else {
			macro_deleteByName(name);
		}
	}
	return 1;
}

/*------------------------------------------------------------
 * macro_readCompiledMacroFile()
 * Read a compiled macro file.
 */
int macro_readCompiledMacroFile(char *fn) {
	int 	 wasdirty = _macrosWereChanged;
	RSCFILE	*rp;

	if (fn == 0) {
		fn = fsel_initPathes(&_seqfsel);
	}

	bindings_loadActionBindings();
	macro_autosaveAllBindings(1);

	if (fn && (rp = rsc_open(fn,RSC_O_READ)) != 0) {
		/* load macros */
		if (!rsc_load(rp, "MACROS", "", rsc_rdmacros)) {
			error_showErrorById(IDS_MSGBADMACFORMAT);
			rsc_close(rp);
			return 0;
		}

		rsc_close(rp);
		MACRO* mpNamespace = macro_getNamespaceByIdx(0);
		if (mpNamespace && !mpNamespace->mc_isInitialized) {
			if (!interpreter_initializeNamespace(mpNamespace)) {
				return 0;
			}
		}
	}

	_macrosWereChanged = wasdirty;
	return 1;
}

/*------------------------------------------------------------
 * macro_createWithParams()
 * create a new macro with a name, a comment and the actual byte codes
 * to execute.
 */
MACRO *macro_createWithParams(MACRO_PARAM *pParam) {
	MACRO *mp;
	
	char* szComment = pParam->mp_comment;
	if (!szComment)
		szComment = "";

	char* szName = pParam->mp_name;
	if (szName == 0) {
		szName = "#anonymous";
	}
	mp = calloc(1, sizeof * mp);
	mp->mc_comment = _strdup(szComment);
	mp->mc_name= _strdup(szName);
	mp->mc_scope = pParam->mp_scope;
	mp->mc_namespaceIdx = pParam->mp_namespaceIdx;
	mp->mc_numberOfLocalVars = pParam->mp_numberOfLocalVariables;
	mp->mc_bytecodeLength = (unsigned int)pParam->mp_bytecodeLength;
	mp->mc_bytecodes = malloc(mp->mc_bytecodeLength);
	memmove(mp->mc_bytecodes,pParam->mp_buffer, mp->mc_bytecodeLength);
	return mp;
}

/*------------------------------------------------------------
 * macro_renameAndChangeComment()
 */
void macro_renameAndChangeComment(int nIndex, char* szName, char* szComment)
{
	MACRO *mpold = macro_getByIndex(nIndex);

	if (mpold) {
		free(mpold->mc_comment);
		free(mpold->mc_name);
		mpold->mc_comment = _strdup(szComment);
		mpold->mc_name = _strdup(szName);
	}
}

static void macro_destroyTable(ARRAY_LIST** pList) {
	if (*pList) {
		int nMax = (int)arraylist_size(*pList);
		for (int i = 0; i < nMax; i++) {
			MACRO* mp = arraylist_get(*pList, i);
			if (mp) {
				macro_destroyMacro(mp);
			}
		}
		arraylist_destroy(*pList);
		*pList = 0;
	}
}

/*
 * Destroy all allocated macros.
 */
void macro_destroy() {
	macro_destroyTable(&_macroTable);
	macro_destroyTable(&_namespaces);
}

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
int macro_insertNewMacro(MACRO_PARAM *mpParam)
{	MACRO  *mp;
	int    i;

	if (!_macroTable) {
		_macroTable = arraylist_create(32);
	}
	if ((i = macro_getInternalIndexByName(mpParam->mp_name)) >= 0) {
		macro_destroyMacro(arraylist_get(_macroTable, i));
		arraylist_removeAt(_macroTable, i);
	}

	if ((mp = macro_createWithParams(mpParam)) == 0)
		return -1;

	i = 1;
	arraylist_add(_macroTable, mp);
	_macrosWereChanged = 1;
	return i;
}

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
long long macro_readWriteWithFileSelection(int wrflag) {
	char *fn;

	FILE_SELECT_PARAMS params;
	params.fsp_saveAs = wrflag;
	params.fsp_optionsAvailable = FALSE;
	if ((fn = fsel_selectFileWithOptions(&_seqfsel,(wrflag) ? CMD_WRITE_MACROS : CMD_READ_MACROS, &params)) == 0) {
		return 0;
	}

	if (wrflag) {
		if (macro_writeMacroBindingsToFile(RSC_MACROS,fn)) {
			_macrosWereChanged = 0;
			return 1;
		}
		return 0;
	}

	return macro_readCompiledMacroFile(fn);
}

/*--------------------------------------------------------------------------
 * macro_translateToOriginalMenuIndex()
 * try to find an internal command/standard menu binding, which 
 * allows us to display an appropriate help for synthetic menus
 */
int macro_translateToOriginalMenuIndex(int wParam) {
	if (wParam < IDM_USERDEF0) {
		return wParam;
	}
	if (wParam >> 16) {
		return wParam;
	}
	return wParam;
}

/*---------------------------------
 * macro_onMenuAction()
 * Invoke the macro bound to a menu index.
 *---------------------------------*/
int macro_onMenuAction(WINFO* wp, int menunum, POINT* aPositionClicked) {
	MACROREF *	mp;

	mp = macro_translateMenuCommand(menunum);
	if (mp != 0) {
		if (aPositionClicked) {
			return macro_executeWithPosition(wp, mp, *aPositionClicked);
		} else {
			return (int)macro_executeMacro(mp);
		}
	}
	if (menunum >= IDM_HISTORY && menunum <= IDM_HISTORY + 30) {
		return (int)EdEditFile(OPEN_NOFN|OPEN_HISTORY | (menunum - IDM_HISTORY) << 12, 0);
	}
	return 0;
}

/*------------------------------------------------------------
 * macro_onCharacterInserted()
 */
int macro_onCharacterInserted(WORD c) {
	intptr_t stack[8];
	stack[0] = c;
	return (int) interpreter_executeFunction(FUNC_EdCharInsert, stack);
}

static int macro_executeWithProgress(MACROREF* mp) {
	progress_startMonitor(IDS_ABRTMACRO, 1000);
	int ret = (int)macro_executeMacro(mp);
	progress_closeMonitor(0);
	return ret;
}

/*---------------------------------*/
/* macro_executeByName()				*/
/*---------------------------------*/
int macro_executeByName(char *name) {
	if (!name || !name[0]) {
		return 0;
	}
	MACROREF macref;
	int i;
	if (name[0] == '@') {
		macref.typ = CMD_CMDSEQ;
		i = macro_getCmdIndexByName(name + 1);
	} else {
		macref.typ = CMD_MACRO;
		i = macro_getInternalIndexByName(name);
	}
	if (i < 0) {
		error_showErrorById(IDS_MSGUNKNOWNMAC,name);
		return 0;
	}
	macref.index = i;
	return macro_executeWithProgress(&macref);
}

/*
 * Returns the keyboard binding text for a given internal command.
 */
char* macro_getKeyText(const char* pszActionContext, int nCmd) {
	KEYCODE	k;
	if ((k = bindings_findBoundKey(NULL, (MACROREF) { .typ = CMD_CMDSEQ, .index = nCmd })) != K_DELETED) {
		return bindings_keycodeToString(k);
	}
	return NULL;
}

/*------------------------------------------------------------
 * macro_getSelectedMacro()
 */
static LONG macro_getSelectedMacro(HWND hwnd)
{
	LONG		nData;
	LRESULT		item;

	if ((item = SendDlgItemMessage(hwnd, IDD_MACROLIST, LB_GETCURSEL, 0, 0)) < 0)
		return -1;
	SendDlgItemMessage(hwnd, IDD_MACROLIST, LB_GETTEXT, (WPARAM)item, (LPARAM)&nData);
	return nData;
}

/*------------------------------------------------------------
 * macro_getSelectedKey()
 */
static KEYCODE macro_getSelectedKey(HWND hwnd) {
	KEYCODE		nKey = 0;
	LRESULT		item;

	if ((item = SendDlgItemMessage(hwnd, IDD_LISTBOX2, LB_GETCURSEL, 0, 0)) < 0) {
		return K_DELETED;
	}
	SendDlgItemMessage(hwnd, IDD_LISTBOX2, LB_GETTEXT, (WPARAM)item, (LPARAM)&nKey);
	return nKey;
}

/*------------------------------------------------------------
 * macro_enableButton()
 */
static void macro_enableButton(HWND hwnd, WORD nItem, BOOL nWhich)
{
	HWND 	hwndButton;

	hwndButton = GetDlgItem(hwnd,nItem);
	EnableWindow(hwndButton,nWhich);
}

/*------------------------------------------------------------
 * macro_initializeListBox()
 */
static HWND macro_initializeListBox(HWND hwnd, WORD nItem, LRESULT *nCurr)
{
	HWND hwndList;

	hwndList = GetDlgItem(hwnd,nItem);
	*nCurr = SendMessage(hwndList,LB_GETCURSEL,0,0L);
	if (*nCurr < 0) {
		*nCurr = 0;
	}
	SendMessage(hwndList,LB_RESETCONTENT,0,0L);
	SendMessage(hwndList,WM_SETREDRAW,FALSE,0L);
	return hwndList;
}

/*------------------------------------------------------------
 * macro_listEndFilling()
 */
static void macro_listEndFilling(HWND hwndList, WPARAM nCurr)
{
	SendMessage(hwndList, WM_SETREDRAW,TRUE,0L);
	SendMessage(hwndList, LB_SETCURSEL, nCurr, 0L);
	win_sendRedrawToWindow(hwndList);
}

/*--------------------------------------------------------------------------
 * macro_selectByValue()
 */
static BOOL macro_selectByValue(HWND hwnd, LONG lValue)
{
	int		i;
	HWND 	hwndList;
	LRESULT	nItems;
	LONG	nData = 0;

	hwndList = GetDlgItem(hwnd, IDD_MACROLIST);
	nItems = SendMessage(hwndList, LB_GETCOUNT, 0, 0);

	for (i = 0; i < nItems; i++) {
		SendMessage(hwndList, LB_GETTEXT, (WPARAM)i, (LPARAM)&nData);
		if (nData == lValue) {
			SendMessage(hwndList, LB_SETCURSEL, i, 0);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Returns the number of macros defined
 */
int macro_getNumberOfMacros() {
	return _macroTable == 0 ? 0 : (int)arraylist_size(_macroTable);
}
/*------------------------------------------------------------
 * macro_updateMacroList()
 */
static showInternals = 0;
static void macro_updateMacroList(HWND hwnd)
{
	HWND 	hwndList;
	int  	i,state;
	LRESULT nCurr;
	int nMax = macro_getNumberOfMacros();
	hwndList = macro_initializeListBox(hwnd, IDD_MACROLIST, &nCurr);

	state = showInternals = IsDlgButtonChecked(hwnd, IDD_OPT1);

	for (i = 0; i < nMax; i++) {
		MACRO* mp = macro_getByIndex(i);
		if (mp != 0 && (state || mp->mc_scope == MS_GLOBAL)) {
			SendMessage(hwndList,LB_ADDSTRING,0,MAKELONG(CMD_MACRO,i));
		}
	}

	if (state) {
		for (i = 0; i < _commandTableSize; i++)
			SendMessage(hwndList,LB_ADDSTRING,0,MAKELONG(CMD_CMDSEQ,i));
	}

	macro_listEndFilling(hwndList,nCurr);
}

/*------------------------------------------------------------
 * command_getTooltipAndLabel()
 * Returns the label and tooltip for a given command described by command.
 * The result is copied into the space passed with szTooltip.
 * If szLabel is not NULL, it will contain a longer text used in a tooltip or a help context.
 */
char *command_getTooltipAndLabel(MACROREF command, char* szTooltip, char* szLabel) {
	char *s;

	if (szLabel) {
		szLabel[0] = 0;
	}
	int nIndex = command.index;
	switch(command.typ) {
	case CMD_MACRO: {
			MACRO* mp = macro_getByIndex(command.index);
			if (mp == 0) {
				return "";
			}
			strncpy(szTooltip, MAC_COMMENT(mp), MAC_COMMENTLEN);
			szTooltip[MAC_COMMENTLEN-1] = 0;
			return szTooltip;
		}
		default:
			s = NULL;
			if (nIndex >= 0 && nIndex < _commandTableSize) {
				s = dlg_getResourceString(_commandTable[nIndex].c_index+IDS_BASE_COMMAND);
			}
			if (s == NULL) {
				return "";
			}
	}

	char* pszComment = s;
	strcpy(szTooltip, s);
	if ((s = strchr(s,';')) != 0) {
		if (szLabel) {
			lstrcpy(szLabel, s + 1);
		}
		size_t nLen = s - pszComment;
		strncpy(szTooltip, pszComment, nLen);
		szTooltip[nLen] = 0;
	}

	return szTooltip;
}

/*
 * Returns a tool tip for a given menu id. 
 */
char *macro_getMenuTooltTip(int dMenuId) {
	static char szBuf[256];
	MACROREF *	mp;

	if (dMenuId != -1 && (mp = macro_translateMenuCommand(dMenuId)) != 0) {
		command_getTooltipAndLabel(*mp, szBuf, NULL);
	} else {
		szBuf[0] = 0;
	}
	return szBuf;
}

/*--------------------------------------------------------------------------
 * macro_showHelpForMenu()
 * Display help for a given menu id
 */
void macro_showHelpForMenu(int dMenuId)
{
	char			szBuf[256];
	MACROREF *	mp;

	st_switchtomenumode(dMenuId != -1);
	if (dMenuId != -1 && (mp = macro_translateMenuCommand(dMenuId)) != 0) {
		command_getTooltipAndLabel(*mp, szBuf, NULL);
		st_setStatusLineMessage(*szBuf ? szBuf : (char *)0);
	} else {
		st_setStatusLineMessage((char *)0);
	}
}

/*------------------------------------------------------------
 * macro_updateCommentAndName()
 */
static void macro_updateCommentAndName(HWND hwnd)
{
	LONG		nSelected;
	MACROREFIDX	nIndex;
	MACROREFTYPE type;
	BOOL		editable;
	char		szName[64],szComment[MAC_COMMENTLEN],szK[128];

	nSelected = macro_getSelectedMacro(hwnd);
	nIndex = (MACROREFIDX)HIWORD(nSelected);
	type = (MACROREFTYPE)LOWORD(nSelected);
	DlgInitString(hwnd, IDD_STRING1, 
				mac_name(szName,nIndex,type), MAC_NAMELEN);
	DlgInitString(hwnd, IDD_STRING2, 
		command_getTooltipAndLabel((MACROREF) {.typ = type, .index = nIndex}, szComment, szK), MAC_COMMENTLEN);

	SendDlgItemMessage(hwnd, IDD_STRING1, EM_SETMODIFY, FALSE, 0);
	SendDlgItemMessage(hwnd, IDD_STRING2, EM_SETMODIFY, FALSE, 0);

	editable = (LOWORD(nSelected) == CMD_MACRO) ? TRUE : FALSE;

	SendDlgItemMessage(hwnd, IDD_STRING1, EM_SETREADONLY, !editable, 0);
	SendDlgItemMessage(hwnd, IDD_STRING2, EM_SETREADONLY, !editable, 0);

	/* macro_enableButton(hwnd,IDD_MACSTART,editable); */
	macro_enableButton(hwnd,IDD_MACRENAME,editable);
	macro_enableButton(hwnd,IDD_MACDELETE,editable);
	macro_enableButton(hwnd,IDD_MACEDIT,editable);
	macro_enableButton(hwnd, IDD_MACPRINTINSTRUCTIONS, editable);
}

static int _nBoundKeys;
static int _nSelectedMacroIndex;
static int macro_addKeyCodeToList(KEY_BINDING* kp, void* pParam) {
	if (MAKELONG(kp->macref.typ, kp->macref.index) == _nSelectedMacroIndex) {
		_nBoundKeys++;
		SendMessage((HWND)pParam, LB_ADDSTRING, 0, (LPARAM)kp->keycode);
	}
	return 1;
}

/*------------------------------------------------------------
 * macro_newMacroSelected()
 */
static void macro_newMacroSelected(HWND hwnd)
{
	HWND 		hwndList;
	LRESULT		nCurr;

	macro_updateCommentAndName(hwnd);
	_nSelectedMacroIndex = macro_getSelectedMacro(hwnd);
	_nBoundKeys = 0;
	hwndList = macro_initializeListBox(hwnd,IDD_LISTBOX2,&nCurr);
	bindings_forAllKeyBindingsDo(NULL, macro_addKeyCodeToList, hwndList);
	macro_listEndFilling(hwndList,0);
	macro_enableButton(hwnd,IDD_MACDELKEY,_nBoundKeys ? TRUE : FALSE);
}

/*------------------------------------------------------------
 * macro_fillKeyTables()
 */
static void macro_fillKeyTables(HWND hwnd)
{
	HWND 		hwndList;
	LRESULT		nCurr;

	hwndList = macro_initializeListBox(hwnd,IDD_LISTBOX,&nCurr);
	macro_listEndFilling(hwndList,0);
}

/*------------------------------------------------------------
 * macro_updateSelectedMacro()
 */
static void macro_updateSelectedMacro(HWND hwnd, LONG nSelected)
{
	macro_updateMacroList(hwnd);
	if (nSelected) {
		SendDlgItemMessage(hwnd, IDD_MACROLIST,
			LB_SELECTSTRING, -1, nSelected);
	}
	macro_newMacroSelected(hwnd);
}

/*------------------------------------------------------------
 * mac_lboxdraw()
 */
static void macro_ownerDrawListboxItem(HDC hdc, RECT *rcp, void* par, int nItem, 
					    int nID)
{
	char	szBuf[128];

	if (nID == IDD_LISTBOX2) {
		lstrcpy(szBuf,bindings_keycodeToString((KEYCODE) par));
	} else {
		mac_name(szBuf, (MACROREFIDX)HIWORD(par), (MACROREFTYPE)LOWORD(par));
	}
	TextOut(hdc, rcp->left, rcp->top, szBuf, (int)strlen(szBuf));
}

/*------------------------------------------------------------
 * macro_getCurrentKeycode()
 */
static KEYCODE macro_getCurrentKeycode(void)
{
	static KEYCODE k;
	static DIALPARS _d[] = { IDD_KEYCODE,	sizeof k,	&k, 0 };

	k = K_DELETED;
	if (DoDialog(DLGKEYCODE, dlg_standardDialogProcedure,_d, NULL) == IDCANCEL ||
		k == 0) {
		return K_DELETED;
	}
	return k;
}
	
/*--------------------------------------------------------------------------
 * macro_charItemNextSelected()
 */
static int macro_charItemNextSelected(HWND hwndList, unsigned char ucKey)
{
	LRESULT		nItems;
	LRESULT		nOrigPos;
	LRESULT		lValue;
	unsigned char	szBuf[128];
	unsigned char 	ucCmp;
	LRESULT			nPos;

	if (ucKey >= 'A' && ucKey <= 'Z') {
		ucKey += ('a' - 'A');
	} else if (ucKey >= 'a' && ucKey <= 'z') {
		;
	} else {
		return -2;
	}

	nItems = SendMessage(hwndList, LB_GETCOUNT, 0, 0L);
	nPos = SendMessage(hwndList, LB_GETCARETINDEX, 0, 0L);
	nOrigPos = nPos;

	do {

		if (++nPos >= nItems) {
			nPos = 0;
		}
		lValue = SendMessage(hwndList, LB_GETITEMDATA, nPos, 0);
		mac_name(szBuf, (MACROREFIDX)HIWORD(lValue), (MACROREFTYPE)LOWORD(lValue));

		ucCmp = (szBuf[0] == '@') ? szBuf[1] : szBuf[0];
		if (ucCmp >= 'A' && ucCmp <= 'Z') {
			ucCmp += ('a' - 'A');
		}
		if (ucCmp == ucKey) {
			break;
		}

	} while(nPos != nOrigPos);
	
	return (int)nPos;
}

/*---------------------------------*/
/* macro_getKeyBinding()				*/
/*---------------------------------*/
MACROREF* macro_getKeyBinding(WPARAM key) {
	KEYCODE keycode = (KEYCODE)bindings_addModifierKeysToKeycode((KEYCODE)key);
	static MACROREF macroref;
	WINFO* wp = ww_getCurrentEditorWindow();
	const char* pszActionContext = NULL;
	macroref.index = 0;
	macroref.typ = 0;
	if (wp) {
		pszActionContext = wp->actionContext;
		if (ww_hasSelection(wp)) {
			macroref = bindings_getKeyBinding(keycode | K_HAS_SELECTION, pszActionContext);
		}
	}
	if (macroref.index == 0 && macroref.typ == 0) {
		macroref = bindings_getKeyBinding(keycode, pszActionContext);
	}
	return macroref.index == 0 && macroref.typ == 0 ? NULL : &macroref;;
}

/*------------------------------------------------------------
 * DlgMacEditProc()
 */
static INT_PTR CALLBACK DlgMacEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	char 			szComment[MAC_COMMENTLEN];
	char			szName[128];
	char			szN2[64];
	WORD			nNotify;
	int				nId;
	int				nKey;
	HWND			hwndListBox;
	KEYCODE			keycode;
	LONG			nSelected;
	COMPAREITEMSTRUCT *	cp;

	switch(message) {
		case WM_INITDIALOG:
			win_moveWindowToDefaultPosition(hwnd);
			nSelected = MAKELONG(currentSelectedMacro.typ, 
				currentSelectedMacro.index);
			CheckDlgButton(hwnd,IDD_OPT1,showInternals);
			macro_fillKeyTables(hwnd);
			macro_updateSelectedMacro(hwnd, nSelected);
			break;

		case WM_VKEYTOITEM:
			return -1;

		case WM_CHARTOITEM:
			hwndListBox = GET_WM_CHARTOITEM_HWND(wParam, lParam);
			nKey = GET_WM_CHARTOITEM_CHAR(wParam, lParam); 
			return macro_charItemNextSelected(hwndListBox, nKey);

		case WM_DRAWITEM:
			return cust_drawComboBoxOwnerDraw((DRAWITEMSTRUCT*)lParam,
				macro_ownerDrawListboxItem, 0, FALSE);

		case WM_COMPAREITEM:
			cp = (COMPAREITEMSTRUCT*)lParam;
			return lstrcmp(
				mac_name(szName, (MACROREFIDX)HIWORD(cp->itemData1), (MACROREFTYPE)LOWORD(cp->itemData1)),
				mac_name(szN2, (MACROREFIDX)HIWORD(cp->itemData2), (MACROREFTYPE)LOWORD(cp->itemData2)));

		case WM_COMMAND: 
			nSelected = macro_getSelectedMacro(hwnd);
			GetDlgItemText(hwnd,IDD_STRING1,szName,sizeof szName);
			_selectedMacroName = (LOWORD(nSelected) == CMD_MACRO) ?
				MAC_NAME(macro_getByIndex(HIWORD(nSelected))) : 0;
			nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
			switch (nId = GET_WM_COMMAND_ID(wParam, lParam)) {

			case IDD_LISTBOX:
				if (nNotify == LBN_SELCHANGE) {
					if (dlg_getListboxText(hwnd,IDD_LISTBOX,szName) > 0) {
						macro_newMacroSelected(hwnd);
					}
				}
				break;

			case IDD_MACROLIST:
				if (nNotify == LBN_DBLCLK) {
					SendMessage(hwnd,WM_COMMAND,IDD_MACSTART,0L);
				} else if (nNotify == LBN_SELCHANGE) {
					macro_newMacroSelected(hwnd);
				}
				break;

			case IDD_MACADDKEY:
				if ((keycode = macro_getCurrentKeycode()) == K_DELETED) {
					break;
				}
				bindings_bindKey(keycode, (MACROREF) { .index = (MACROREFIDX)HIWORD(nSelected), .typ = (MACROREFTYPE)LOWORD(nSelected) }, NULL);
				goto upd;

			case IDD_MACDELKEY:
				keycode = macro_getSelectedKey(hwnd);
				bindings_deleteKeyBinding(keycode,NULL);
				goto upd;

			case IDD_MACDELETE:
				macro_deleteByName(szName);

			case IDD_OPT1:
				macro_updateSelectedMacro(hwnd, 0);
				break;

			case IDD_MACRENAME:
				if (nSelected >= 0) {
					GetDlgItemText(hwnd,IDD_STRING2,szComment,
								sizeof szComment);
					macro_renameAndChangeComment(HIWORD(nSelected),szName,szComment);
upd: 				_macrosWereChanged = 1;
					macro_newMacroSelected(hwnd);
				}
				return TRUE;

			case IDD_MACFINDKEY: {
				if ((keycode = macro_getCurrentKeycode()) == K_DELETED) {
					break;
				}
				MACROREF macroref;
				macroref = bindings_getKeyBinding(keycode, 0);
				if (macroref.typ == 0) {
					error_showErrorById(IDS_MSGKEYNOTBOUND);
				}
				else {
					int nIndex = macroref.index;
					int nType = macroref.typ;
					macro_selectByValue(hwnd, MAKELONG(nType, nIndex));
					macro_newMacroSelected(hwnd);
				}
				return TRUE;
			}

			case IDD_STRING1:
			case IDD_STRING2:
				if (nNotify != EN_KILLFOCUS) {
					break;
				}
				if (nSelected && 
				    SendDlgItemMessage(hwnd, nId, EM_GETMODIFY, 0, 0) &&
					error_displayYesNoConfirmation(IDS_MSGUPDATEMACNAME) == IDYES) {
					SendMessage(hwnd, WM_COMMAND, IDD_MACRENAME, 0L);
				}
				break;

			case IDCANCEL:
			case IDOK:
			case IDD_MACSTART:
			case IDD_MACEDIT:
			case IDD_MACPRINTINSTRUCTIONS:
				if (nSelected != -1) {
					currentSelectedMacro.typ = (MACROREFTYPE)LOWORD(nSelected);
					currentSelectedMacro.index = (MACROREFIDX)HIWORD(nSelected);
				}
				EndDialog(hwnd, wParam);
				return TRUE;
			}
			break;
	}
	return dlg_standardDialogProcedure(hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * EdMacrosEdit()
 */
int EdMacrosEdit(void)
{
	int 				ret;
	extern 	MACROREF	currentSelectedMacro;

	ret = DoDialog(DLGMACROS, DlgMacEditProc, 0, NULL);

	if (ret == IDD_MACSTART) {
		long m = _multiplier;
		long long  nResult = 0;

		_multiplier = 1;
		while (m-- > 0) {
			nResult = macro_executeWithProgress(&currentSelectedMacro);
		}
		return (int)nResult;
	}

	if (_selectedMacroName) {
		if (ret == IDD_MACEDIT || ret == IDD_MACPRINTINSTRUCTIONS) {
			return decompile_saveMacrosAndDisplay(_selectedMacroName, ret == IDD_MACEDIT ? DM_CODE : DM_INSTRUCTIONS);
		}
	}
	return 0;
}



