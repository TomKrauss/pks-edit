/*
 * MacroKeyBinding.c
 *
 * editor command handling and key mapping
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
#include "fileselector.h"
#include "edfuncs.h"
#include "funcdef.h"
#include "dial2.h"
#include "iccall.h"
#include "resource.h"
#include "xdialog.h"
#include "errordialogs.h"
#include "stringutil.h"
#include "winutil.h"
#include "mouseutil.h"
#include "pkscc.h"
#include "editorconfiguration.h"
#include "menu.h"
#include "funcdef.h"

extern long		_multiplier;

/*---------------------------------*/
/* key_delbindings()			*/
/* delete all key bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
extern long			rsc_wrmacros(int fd, long offset, char *buf, long maxbytes);
extern char *		rsc_rdmacros(char *param, unsigned char *p, unsigned char *pend);
extern char * 		mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
/*
 * Loads the default action bindings (keys and mouse) for PKS-Edit from the PKS_SYS directory.
 */
extern void key_loadBindings();
extern void 		st_switchtomenumode(BOOL bMenuMode);
extern int			macro_executeSequence(COM_1FUNC* cp, COM_1FUNC* cpmax);
MACROREF			bindings_getKeyBinding(KEYCODE keycode, const char* pszActionContext);

int				_recording;
int				_nmacros = MAXMACRO;
char *			_macroname;
char *			_cmdfuncp;
char *			_cmdmaxp;
char *			_cmdparamp;
unsigned char*  _readparamp;
MACRO *			_macrotab[MAXMACRO];
int				_lastinsertedmac = -1;
int				_macedited;
MACROREF		currentSelectedMacro;
/*
 * Start the recorder.
 */
extern int op_startMacroRecording();


EDBINDS _bindings = {
	_edfunctab,_cmdseqtab,_macrotab,
	&_nfuncs,&_ncmdseq,&_nmacros
};

static FSELINFO 	_seqfsel = {	"","pksedit.mac", "*.mac" };

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
 * macro_reportError()
 */
void macro_reportError(void)
{
	error_showErrorById(IDS_MSGEXECERR);
}

/*------------------------------------------------------------
 * macro_getByIndex()
 * Return a macro by its internal index.
 */
MACRO *macro_getByIndex(int idx)
{
	return _macrotab[idx];
}

/*---------------------------------
 * macro_getInternalIndexByName()
 * Return the internal index of a macro given its name.
 */
int macro_getInternalIndexByName(const char *name)
{	int i;
	MACRO *mp;

	if (name)
	  for (i = 0; i < MAXMACRO; i++) {
		if ((mp = macro_getByIndex(i)) != 0 && 
			strcmp(mp->name,name) == 0)
				return i;
	  }
	return -1;
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
		_macroname = 0;

	if (whichresource & (RSC_MACROS|RSC_SINGLEMACRO)) {
		if (rsc_put(fd,"MACROS","",0,rsc_wrmacros,_linebuf,(long)FBUFSIZE) == 0) {
			error_showErrorById(IDS_MSGNODISKSPACE);
			ret = 0;
			goto done;
		}
	}

done:Fclose(fd);
	_macroname = 0;
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

	if (_macedited) {
		if (warnFlag == 0) {
			string_concatPathAndFilename(fn,_seqfsel.path,_seqfsel.fname);
			if (macro_writeMacroBindingsToFile(RSC_MACROS,fn))
				_macedited = 0;
		} else {
			if (error_displayYesNoConfirmation(IDS_MSGSAVESEQ) == IDYES) {
				macro_readWriteWithFileSelection(1);
			}
		}
	}
	_macedited = 0;
}

/*------------------------------------------------------------
 * macro_deleteByName()
 * Delete the macro (including all its bindings) with the given name. Return
 * 1 if successfully deleted.
 */
int macro_deleteByName(char *name)
{	int idx;

	if ((idx = macro_getInternalIndexByName(name)) >= 0) {
		macro_deleteKeyBindingsForMacroRef((MACROREF) { .typ = CMD_MACRO, .index = idx });
		destroy(&_macrotab[idx]);
		_macedited = 1;
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

/*--------------------------------------------------------------------------
 * macro_selectDefaultBindings()
 * Select the default key- / menu- / mouse bindings for PKS Edit.
 */
void macro_selectDefaultBindings(void) {
	const char *pszMode;

	if (ft_getCurrentDocument()) {
		pszMode = ft_getCurrentDocument()->documentDescriptor->actionContext;
	} else {
		pszMode = DEFAULT_ACTION_CONTEXT;
	}
	menu_selectActionContext(pszMode);
}

/*------------------------------------------------------------
 * macro_readBindingsFromFile()
 * Read new bindings from a file. If the current bindings are "dirty" they
 * are flushed before.
 */
int macro_readBindingsFromFile(char *fn) {
	int 	 wasdirty = _macedited;
	RSCFILE	*rp;

	if (fn == 0) {
		if ((fn = fsel_initPathes(&_seqfsel)) == 0)
			return 0;
	}

	if ((rp = rsc_open(fn,RSC_O_READ)) == 0) {
		return 0;
	}

	key_loadBindings();
	macro_autosaveAllBindings(1);

	/* load macros */
	if (!rsc_load(rp, "MACROS", "", rsc_rdmacros)) {
		error_showErrorById(IDS_MSGBADMACFORMAT);
		rsc_close(rp);
		return 0;
	}

	rsc_close(rp);

	macro_selectDefaultBindings();

	_macedited = wasdirty;
	return 1;
}

/*------------------------------------------------------------
 * macro_createWithParams()
 * create a new macro with a name, a comment and the actual byte codes
 * to execute.
 */
MACRO *macro_createWithParams(char* szName, char* szComment, char* bData, int size)
{
	int   nLen,nNamelen,nCommentlen;
	MACRO *mp;
	
	if (!szComment)
		szComment = "";

	nNamelen = (int)strlen(szName) + 1;
	nCommentlen = (int)strlen(szComment) + 1;

	nLen = nNamelen+nCommentlen;
	if (nLen & 1) {
		nLen++;
	}

	if ((mp = malloc(MAC_SIZE(nLen+size))) == 0)
		return (MACRO*)0;

	mp->dstart = (unsigned char)(nLen+sizeof(*mp));

	mp->namelen = nNamelen;
	mp->size = size;

	lstrcpy(mp->name,szName);
	lstrcpy(MAC_COMMENT(mp),szComment);
	memmove(MAC_DATA(mp),bData,size);
	return mp;
}

/*------------------------------------------------------------
 * macro_renameAndChangeComment()
 */
void macro_renameAndChangeComment(int nIndex, char* szName, char* szComment)
{
	MACRO *mp,*mpold;

	mpold = _macrotab[nIndex];
	if ((mp = macro_createWithParams(szName,szComment,MAC_DATA(mpold),mpold->size)) == 0)
		return;
	_macrotab[nIndex] = mp;
	free(mpold);
}

/*
 * Destroy all allocated macros.
 */
void macro_destroy() {
	for (int i = 0; i < DIM(_macrotab); i++) {
		free(_macrotab[i]);
	}
}

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
int macro_insertNewMacro(char *name, char *comment, char *macdata, int size)
{	MACRO  *mp;
	int    i;

	if ((i = macro_getInternalIndexByName(name)) >= 0) {
		destroy(&_macrotab[i]);
	} else {
		for (i = 0; ; i++) {
			if (i >= DIM(_macrotab)) {
				error_showErrorById(IDS_MSGTOOMANYMACROS);
				return 0;
			}
			if (_macrotab[i] == 0) {
				break;
			}
		}
	}

	if ((mp = macro_createWithParams(name,comment,macdata,size)) == 0)
		return -1;

	_macrotab[i] = mp;
	_macedited = 1;
	return i;
}

/*------------------------------------------------------------
 * macro_toggleRecordMaco()
 * start/stops the macro recorder.
 */
static int _recsiz,_lastrecsiz=-1;
int macro_toggleRecordMaco(void)
{	int     size;
	static KEYCODE scan = K_DELETED;
	char    buf[100];
	static  int _ndef;

	if (!_recording) {		/* STOP */
		if (_cmdfuncp && (size = (int)(_cmdparamp-_recorder)) > 0) {
			wsprintf(buf,"NewMacro%d",++_ndef);
			if (!macro_getIndexForKeycode(&scan,buf,-1)) {
				return 0;
			}
			if (!scan) {
				scan = K_DELETED;
			}
			if ((_lastinsertedmac = macro_insertNewMacro(buf,"",_recorder,size)) >= 0) {
				macro_bindKey(scan, (MACROREF) { .index = _lastinsertedmac, .typ = CMD_MACRO }, NULL);
			}
		}
	} else {						/* START */
		_cmdparamp =  _recorder;	/* maybe &_recorder[recno]	*/
		_cmdmaxp	 =  &_recorder[RECORDERSPACE-12];
	}
	return 1;
}

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
int macro_readWriteWithFileSelection(int wrflag) {	
	char *fn;

	FILE_SELECT_PARAMS params;
	params.fsp_saveAs = wrflag;
	params.fsp_optionsAvailable = FALSE;
	if ((fn = fsel_selectFileWithOptions(&_seqfsel,(wrflag) ? CMD_WRITE_MACROS : CMD_READ_MACROS, &params)) == 0) {
		return 0;
	}

	if (wrflag) {
		if (macro_writeMacroBindingsToFile(RSC_MACROS,fn)) {
			_macedited = 0;
			return 1;
		}
		return 0;
	}

	return macro_readBindingsFromFile(fn);
}

/*------------------------------------------------------------
 * EdMacroRecord()
 */
int EdMacroRecord(void)
{
	return op_startMacroRecording();
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
			return macro_executeMacro(mp);
		}
	}
	if (menunum >= IDM_HISTORY && menunum <= IDM_HISTORY + 30) {
		return EdEditFile(OPEN_NOFN|OPEN_HISTORY | (menunum - IDM_HISTORY) << 12, 0);
	}
	return 0;
}

/*---------------------------------
 * macro_addModifierKeys()
 * Add the modifier key bits depending on whether
 * Shift, Control or Alt was pressed together with
 * the key passed as an argument.
 */
KEYCODE macro_addModifierKeys(KEYCODE key)
{
	if ( GetKeyState(VK_SHIFT) < 0)
		key |= K_SHIFT;
	if ( GetKeyState(VK_CONTROL) < 0)
		key |= K_CONTROL;
	if ( GetKeyState(VK_MENU) < 0)
		key |= K_ALTERNATE;
	if (GetKeyState(VK_LWIN) < 0)
		key |= K_WINDOWS;
	return key;
}

/*---------------------------------*/
/* macro_getKeyBinding()				*/
/*---------------------------------*/
MACROREF* macro_getKeyBinding(WPARAM key) {
	KEYCODE keycode = (KEYCODE)macro_addModifierKeys((KEYCODE)key);
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

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
int macro_executeMacro(MACROREF *mp)
{
	COM_1FUNC   *cp;

	switch (mp->typ) {
		case CMD_CMDSEQ:
			cp = &_cmdseqtab[mp->index].c_functionDef;
			return macro_executeSequence(cp,cp+1);
		case CMD_MACRO:
			return macro_executeMacroByIndex(mp->index);
		default:
			error_displayAlertDialog("bad command or macro");
	}
	return 0;
}

/*------------------------------------------------------------
 * macro_onCharacterInserted()
 */
int macro_onCharacterInserted(WORD c)
{
	return 
		macro_executeFunction(FUNC_EdCharInsert,c,0,(void*)0,(void*)0,(void*)0);
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
	return macro_executeMacro(&macref);
}

/*
 * Returns the keyboard binding text for a given internal command.
 */
char* macro_getKeyText(const char* pszActionContext, int nCmd) {
	KEYCODE	k;
	if ((k = macro_findKey(NULL, (MACROREF) { .typ = CMD_CMDSEQ, .index = nCmd })) != K_DELETED) {
		return macro_keycodeToString(k);
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

/*------------------------------------------------------------
 * macro_updateMacroList()
 */
static showInternals = 0;
static void macro_updateMacroList(HWND hwnd)
{
	HWND 	hwndList;
	int  	i,state;
	LRESULT nCurr;

	hwndList = macro_initializeListBox(hwnd, IDD_MACROLIST, &nCurr);

	for (i = 0; i < MAXMACRO; i++) {
		if (_macrotab[i] != 0) {
			SendMessage(hwndList,LB_ADDSTRING,0,MAKELONG(CMD_MACRO,i));
		}
	}

	state = showInternals = IsDlgButtonChecked(hwnd,IDD_OPT1);

	if (state) {
		for (i = 0; i < _ncmdseq; i++)
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
		case CMD_MACRO: 
			if (_macrotab[command.index] == 0) {
				return "";
			}
			lstrcpy(szTooltip,MAC_COMMENT(_macrotab[nIndex]));
			return szTooltip;
		default:
			s = NULL;
			if (nIndex >= 0 && nIndex < _ncmdseq) {
				s = dlg_getResourceString(_cmdseqtab[nIndex].c_index+IDS_BASE_COMMAND);
			}
			if (s == NULL) {
				return "";
			}
	}

	char* pszComment = s;
	strcpy(szTooltip, s);
	if ((s = lstrchr(s,';')) != 0) {
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
	char		szName[64],szComment[256],szK[128];

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
	key_bindingsDo(NULL, macro_addKeyCodeToList, hwndList);
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
		lstrcpy(szBuf,macro_keycodeToString((KEYCODE) par));
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
			_macroname = (LOWORD(nSelected) == CMD_MACRO) ?
				_macrotab[HIWORD(nSelected)]->name : 0;
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
				macro_bindKey(keycode, (MACROREF) { .index = (MACROREFIDX)HIWORD(nSelected), .typ = (MACROREFTYPE)LOWORD(nSelected) }, NULL);
				goto upd;

			case IDD_MACDELKEY:
				keycode = macro_getSelectedKey(hwnd);
				macro_deleteKeyBinding(keycode,NULL);
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
upd: 				_macedited = 1;
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
				if (nSelected) {
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
	extern 	char* _macroname;
	extern 	MACROREF	currentSelectedMacro;

	ret = DoDialog(DLGMACROS, DlgMacEditProc, 0, NULL);
	macro_selectDefaultBindings();

	if (ret == IDD_MACSTART) {
		long m = _multiplier;
		int  nResult = 0;

		_multiplier = 1;
		while (m-- > 0) {
			nResult = macro_executeMacro(&currentSelectedMacro);
		}
		return nResult;
	}

	if (_macroname) {
		if (ret == IDD_MACEDIT) {
			return macro_saveMacrosAndDisplay(_macroname);
		}
	}
	return 0;
}



