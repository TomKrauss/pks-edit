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

#define	MENU_TABCHAR	'\010'

extern long		_multiplier;

extern RSCTABLE *	_keytables;
extern RSCTABLE *	_mousetables;
extern RSCTABLE *	_menutables;

extern void win_setEditMenuText(int menunr, char* text);
extern BOOL op_defineOption(long nFlag);
/*---------------------------------*/
/* key_delbindings()			*/
/* delete all key bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
extern void key_delbindings(MACROREFTYPE typ, MACROREFIDX val);
extern void key_unbind(KEYBIND* kp);
extern long			rsc_wrmacros(int fd, long offset, char *buf, long maxbytes);
extern long 		rsc_wrbuf(int fd, long offset, char *buf, long maxbytes);
extern char *		rsc_rdmacros(char *param, unsigned char *p, unsigned char *pend);
extern char * 		mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
extern void 		st_seterrmsg(char *msg);
extern void 		key_overridetable(void);
extern void 		mouse_destroyMouseBindings(void);
extern int 			menu_addMenuMacroItem(char *pszString, int menutype, 
					MACROREFTYPE mactype, MACROREFTYPE macidx);
extern void 		menu_initializeDefinition(char *szMenu);
extern void 		st_switchtomenumode(BOOL bMenuMode);
extern int			macro_executeSequence(COM_1FUNC* cp, COM_1FUNC* cpmax);
/*------------------------------------------------------------
 * bind_mouse()
 * bind a mouse to current key table context
 */
extern 	int bind_mouse(MOUSECODE mousecode, MACROREFTYPE typ, MACROREFIDX idx, int flags, int augment);
extern MACROREF *	menu_getUserDefinedMacro(int nId);
extern int 			macro_canExecuteFunction(int num, int warn);
extern void 		menu_switchMenusToContext(char *pszContext);

int				_recording;
int				_nmacros = MAXMACRO;
char *			_macroname;
char *			_cmdfuncp;
char *			_cmdmaxp;
char *			_cmdparamp;
unsigned char* _readparamp;
MACRO *			_macrotab[MAXMACRO];
int				_lastinsertedmac = -1;
int				_macedited;
int				_fkeysdirty;
MACROREF		currentSelectedMacro;
/*
 * Start the recorder.
 */
extern int op_startMacroRecording();


static FSELINFO 	_seqfsel = {	"","pksedit.mac", "*.mac" };
static int 		_nkeys = MAXMAPKEY;

EDBINDS _bindings = {
	_edfunctab,_cmdseqtab,_macrotab,_keymaptab,_menutab,_mousetab,
	&_nfuncs,&_ncmdseq,&_nmacros,&_nkeys,&_nmenus,&_nmousebind
};

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
int macro_getInternalIndexByName(char *name)
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
#define	RSC_KEYS			0x4
#define	RSC_MICE			0x8
#define	RSC_MENUS			0x10
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
err:			error_showErrorById(IDS_MSGNODISKSPACE);
			ret = 0;
			goto done;
		}
	}

	if (whichresource & RSC_KEYS) {
		if (rsc_wrtables(fd,"KEYS",_keytables) == 0) {
			goto err;
		}
	}

	if (whichresource & RSC_MICE) {
		if (rsc_wrtables(fd,"MOUSE",_mousetables) == 0) {
			goto err;
		}
	}

	if (whichresource & RSC_MENUS) {
		if (rsc_wrtables(fd,"MENU",_menutables) == 0) {
			goto err;
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
	char fn[512];

	if (_macedited) {
		if (warnFlag == 0) {
			string_concatPathAndFilename(fn,_seqfsel.path,_seqfsel.fname);
			if (macro_writeMacroBindingsToFile(RSC_MACROS|RSC_KEYS|RSC_MICE|RSC_MENUS,fn))
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
		key_delbindings(CMD_MACRO,idx);
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
 * macro_onKeybindingChanged()
 * invoked, when a keybinding has changed. May flag e.g. the function keyboard of PKSEDIT
 * as dirty.
 */
void macro_onKeybindingChanged(KEYCODE key) {
	key &= 0xFF;
	if (key >= VK_F1 && key <= VK_F12) {
		_fkeysdirty = 1;
	}
}

/*------------------------------------------------------------
 * macro_bindOrUnbindKey()
 * index < 0:	delete key-binding
 * index >=0:  add key-binding
 */
int macro_bindOrUnbindKey(KEYCODE key, int index, MACROREFTYPE typ)
{	KEYBIND *kp;

	if (key == K_DELETED)
		return 1;

	if ((kp = keybound(key)) == 0) {
		if (index < 0)
			return 0;
		if ((kp = keygetbind()) == 0) {
			return 0;
		}
		kp->keycode = key;
		kp->macref.typ = CMD_NONE;
	}

	if (index < 0) {
		key_unbind(kp);
	} else {
		macro_onKeybindingChanged(key);
		kp->keycode = key;
		kp->macref.index = index;
		kp->macref.typ = typ;
	}
	return 1;
}

/*------------------------------------------------------------
 * macro_addMultipleKeyBindings()
 */
static char *macro_addMultipleKeyBindings(char *name, KEYBIND *kp, KEYBIND *kplast)
{
	if (!rsc_switchtotable(&_keytables,name)) {
		return 0;
	}

	key_overridetable();

	while (kp < kplast) {
		macro_bindOrUnbindKey(kp->keycode,kp->macref.index,kp->macref.typ);
		kp++;
	}
	return (char *)kplast;
}

/*------------------------------------------------------------
 * macro_addMultipleMouseBindings()
 */
static char *macro_addMultipleMouseBindings(char *name, MOUSEBIND *mp, MOUSEBIND *mplast)
{
	if (!rsc_switchtotable(&_mousetables,name)) {
		return 0;
	}

	mouse_destroyMouseBindings();

	while (mp < mplast) {
		MOUSECODE mcode;
		
		mcode.button = mp->button;
		mcode.nclicks = mp->nclicks;
		mcode.shift = mp->shift;
		bind_mouse(mcode, mp->macref.typ, mp->macref.index, 0, 1);
		mp++;
	}
	return (char *)mplast;
}

/*------------------------------------------------------------
 * macro_addMultipleMenuBindings()
 */
static char *macro_addMultipleMenuBindings(char *name, PUSERMENUBIND mp, 
	PUSERMENUBIND mplast)
{
	menu_initializeDefinition(name);

	while (mp < mplast) {
		if (*mp->szString) {
			if (!menu_addMenuMacroItem(mp->szString, mp->type, 
				mp->macref.typ, mp->macref.index)) {
				return 0;
			}
		}
		mp++;
	}
	return (char *)mplast;
}

/*--------------------------------------------------------------------------
 * macro_selectDefaultBindings()
 * Select the default key- / menu- / mouse bindings for PKS Edit.
 */
void macro_selectDefaultBindings(void)
{
	char	*pszDefault = "default";
	char *pszMode;

	if (ft_getCurrentDocument()) {
		pszMode = ft_getCurrentDocument()->documentDescriptor->name;
	} else {
		pszMode = pszDefault;
	}

	rsc_switchtotable(&_keytables,
		rsc_findtable(_keytables, pszMode) ? pszMode : pszDefault);

	rsc_switchtotable(&_mousetables,
		rsc_findtable(_mousetables, pszMode) ? pszMode : pszDefault);

	menu_switchMenusToContext(pszMode);
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

	macro_autosaveAllBindings(1);

	/* load macros */
	if (!rsc_load(rp, "MACROS", "", rsc_rdmacros)) {
		error_showErrorById(IDS_MSGBADMACFORMAT);
		rsc_close(rp);
		return 0;
	}

	/* load key bindings */
	rsc_load(rp,"KEYS",(char*)0,macro_addMultipleKeyBindings);

	/* load mouse bindings */
	rsc_load(rp,"MOUSE",(char*)0,macro_addMultipleMouseBindings);

	/* load menu bindings */
	rsc_load(rp,"MENU",(char*)0,macro_addMultipleMenuBindings);

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
			wsprintf(buf,"Mac%d",++_ndef);
			if (!macro_getIndexForKeycode(&scan,buf,-1)) {
				return 0;
			}
			if (!scan) {
				scan = K_DELETED;
			}
			if ((_lastinsertedmac = macro_insertNewMacro(buf,"",_recorder,size)) >= 0) {
				macro_bindOrUnbindKey(scan,_lastinsertedmac,CMD_MACRO);
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
	if ((fn = fsel_selectFileWithOptions(&_seqfsel,(wrflag) ? MWRSEQ : MREADSEQ, &params)) == 0) {
		return 0;
	}

	if (wrflag) {
		if (macro_writeMacroBindingsToFile(RSC_MACROS|RSC_KEYS|RSC_MICE|RSC_MENUS,fn)) {
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
 * macro_getMacroIndexForMenu()
 * Return the macro reference given a menu id.
 */
MACROREF *macro_getMacroIndexForMenu(int nId)
{
static MACROREF 	macref;
	MACROREF *	mpUser;
	int			i;

	if (nId >= IDM_USERDEF0) {
		if ((mpUser = menu_getUserDefinedMacro(nId)) != 0) {
			return mpUser;
		}
	} else {
		for (i = 0; i < _nmenus; i++) {
			if (_menutab[i].menunum == nId) {
				macref.typ = CMD_CMDSEQ;
				macref.index = _menutab[i].index;
				return &macref;
			}
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * macro_translateToOriginalMenuIndex()
 * try to find an internal command/standard menu binding, which 
 * allows us to display an appropriate help for synthetic menus
 */
WORD macro_translateToOriginalMenuIndex(WORD wParam)
{
	MACROREF *	mpUser;
	int			i;

	if (wParam < IDM_USERDEF0) {
		return wParam;
	}
	if ((mpUser = menu_getUserDefinedMacro(wParam)) == 0) {
		return wParam;
	}

	if (mpUser->typ == CMD_CMDSEQ) {
		for (i = 0; i < _nmenus; i++) {
			if (_menutab[i].index == mpUser->index) {
				return _menutab[i].menunum;
			}
		}
	}
	return wParam;
}

/*---------------------------------
 * macro_onMenuAction()
 * Invoke the macro bound to a menu index.
 *---------------------------------*/
int macro_onMenuAction(int menunum)
{
	MACROREF *	mp;

	if ((mp = macro_getMacroIndexForMenu(menunum)) != 0) {
		return macro_executeMacro(mp);
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
	return key;
}

/*---------------------------------*/
/* macro_getKeyBinding()				*/
/*---------------------------------*/
void* macro_getKeyBinding(WPARAM key)
{
	KEYCODE keycode = (KEYCODE)macro_addModifierKeys((KEYCODE)key);
	void *pResult = NULL;
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp && ww_hasSelection(wp)) {
		pResult = keybound(keycode | K_HAS_SELECTION);
	}
	if (pResult == NULL) {
		pResult = keybound(keycode);
	}
	return pResult;
}

/*---------------------------------*/
/* macro_executeMacro()				*/
/*---------------------------------*/
int macro_executeMacro(MACROREF *mp)
{
	COM_1FUNC   *cp;
	MENUBIND    *menp;

	switch (mp->typ) {
		case CMD_CMDSEQ:
			cp = &_cmdseqtab[mp->index].c_functionDef;
			return macro_executeSequence(cp,cp+1);
		case CMD_MACRO:
			return macro_executeMacroByIndex(mp->index);
		case CMD_MENU:
			menp = &_menutab[mp->index];
			cp = &_cmdseqtab[menp->index].c_functionDef;
			macro_executeSequence(cp,cp+1);
			break;
		default:
			error_displayAlertDialog("bad command or macro");
	}
	return 0;
}

/*---------------------------------*
 * macro_onKeyPressed()
 * Execute a keybinding and return 1 if successful.
 *---------------------------------*/
int macro_onKeyPressed(void* keybind) {
	KEYBIND     *kp;

	if ((kp = (KEYBIND*)keybind) == (KEYBIND *) 0) 
		return 0;

	return macro_executeMacro(&kp->macref);
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
int macro_executeByName(char *name)
{	int i;

	if (!name || !name[0])
		return 0;

	if ((i = macro_getInternalIndexByName(name)) < 0) {
		error_showErrorById(IDS_MSGUNKNOWNMAC,name);
		return 0;
	}

	return macro_executeMacroByIndex(i);
}

/*------------------------------------------------------------
 * macro_findkey()
 */
static KEYCODE macro_findkey(MACROREFTYPE typ, MACROREFIDX index)
{
	KEYBIND 	*kp;

	for (kp = _keytables->rt_data; kp < (KEYBIND*)_keytables->rt_end; kp++) {
		if (kp->macref.typ == typ && kp->macref.index == index)
			return kp->keycode;
	}
	return K_DELETED;
}

/*
 * Returns the keyboard binding text for a given internal command.
 */
char* macro_getKeyText(int nCmd) {
	KEYCODE	k;
	if ((k = macro_findkey(CMD_CMDSEQ, nCmd)) != K_DELETED) {
		return code2key(k);
	}
	return NULL;
}

/*------------------------------------------------------------
 * macro_assignAcceleratorTextOnMenu()
 * this function sets the menu accelerator text, each time a
 * menu popup is opened
 */
void macro_assignAcceleratorTextOnMenu(HMENU hMenu)
{
	KEYCODE	k;
	int		wCount;
     int		nFuncnum;
	int		wItem;
	int		wID;
	char		string[100];
	char *	d;
	MACROREF *mp;

	wCount = GetMenuItemCount(hMenu);
	for (wItem = 0; wItem < wCount; wItem++) {
		if ((wID = GetMenuItemID(hMenu, wItem)) <= 0) {
			continue;
		}
#if 0
		wState = GetMenuState(hMenu, wItem, MF_BYPOSITION);
		if (HIBYTE(wState) != 0) {
		/* popups */
			continue;
		}
# endif
		if ((mp = macro_getMacroIndexForMenu(wID)) == 0) {
			continue;
		}
		if (GetMenuString(hMenu, wItem, string, sizeof string - 1,
			MF_BYPOSITION) < 0) {
			continue;
		}
		for (d = string; *d; d++) {
			if (*d == 9 || *d == 8) {
				break;
			}
		}
		if (*d) {
			if ((k = macro_findkey(mp->typ, mp->index)) != K_DELETED) {
				strcpy(d+1,code2key(k));
			} else {
				d[1] = 0;
			}
			win_setEditMenuText(wID, string);
		}
		if (mp->typ == CMD_CMDSEQ) {
			if (mp->index >= _ncmdseq) {
				error_displayAlertDialog("bad cmdseq");
				continue;
			}
			nFuncnum = _cmdseqtab[mp->index].c_functionDef.funcnum;
			EnableMenuItem(hMenu, wItem, 
				macro_canExecuteFunction(nFuncnum, 0) ?
			    		MF_BYPOSITION|MF_ENABLED : 
					MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
			if (nFuncnum == FUNC_EdOptionToggle) {
				if (op_defineOption(_cmdseqtab[mp->index].c_functionDef.p)) {
					CheckMenuItem(hMenu, wItem, MF_CHECKED|MF_BYPOSITION);
				}
			}
		}
	}
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
 * macro_getComment()
 * Returns the command for a given macro index and type.
 * The result is copied into the space passed with szBuf.
 */
char *macro_getComment(char* szBuf, char* szB2, int nIndex, int type)
{
	char *s;

	szB2[0] = 0;

	switch(type) {
		case CMD_MACRO: 
			if (_macrotab[nIndex] == 0) {
				return "";
			}
			lstrcpy(szBuf,MAC_COMMENT(_macrotab[nIndex]));
			return szBuf;
		case CMD_MENU:
			nIndex = _menutab[nIndex].index;
			/* drop through */
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
	if ((s = lstrchr(s,';')) != 0) {
		lstrcpy(szB2,s+1);
		size_t nLen = s - pszComment;
		strncpy(szBuf, pszComment, nLen);
		szBuf[nLen] = 0;
	}

	return szBuf;
}

/*
 * Returns a tool tip for a given menu id. 
 */
char *macro_getMenuTooltTip(int dMenuId) {
	static char szBuf[256];
	char szBuf2[32];
	MACROREF *	mp;

	if (dMenuId != -1 && (mp = macro_getMacroIndexForMenu(dMenuId)) != 0) {
		macro_getComment(szBuf, szBuf2, mp->index, mp->typ);
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
	char			szBuf2[32];
	MACROREF *	mp;

	st_switchtomenumode(dMenuId != -1);
	if (dMenuId != -1 && (mp = macro_getMacroIndexForMenu(dMenuId)) != 0) {
		macro_getComment(szBuf, szBuf2, mp->index, mp->typ);
		st_seterrmsg(*szBuf ? szBuf : (char *)0);
	} else {
		st_seterrmsg((char *)0);
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
				macro_getComment(szComment,szK,nIndex,type),MAC_COMMENTLEN);

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

/*------------------------------------------------------------
 * macro_newMacroSelected()
 */
static void macro_newMacroSelected(HWND hwnd)
{
	HWND 		hwndList;
	LONG		nSelected;
	KEYBIND		*kp;
	int			nKeys = 0;
	LRESULT		nCurr;

	macro_updateCommentAndName(hwnd);
	nSelected = macro_getSelectedMacro(hwnd);
	hwndList = macro_initializeListBox(hwnd,IDD_LISTBOX2,&nCurr);
	for (kp = _keytables->rt_data; kp < (KEYBIND*)_keytables->rt_end; kp++) {
		if (MAKELONG(kp->macref.typ,kp->macref.index) == nSelected) {
			nKeys++;
			SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM)kp->keycode);
		}
	}
	macro_listEndFilling(hwndList,0);
	macro_enableButton(hwnd,IDD_MACDELKEY,nKeys ? TRUE : FALSE);
}

/*------------------------------------------------------------
 * macro_fillKeyTables()
 */
static void macro_fillKeyTables(HWND hwnd)
{
	HWND 		hwndList;
	LRESULT		nCurr;
	RSCTABLE	*rp;

	hwndList = macro_initializeListBox(hwnd,IDD_LISTBOX,&nCurr);
	for (rp = _keytables; rp; rp = rp->rt_next) {
		SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM)rp->rt_name);
	}
	macro_listEndFilling(hwndList,0);
	SendMessage(hwndList, LB_SELECTSTRING, -1, (LPARAM)_keytables->rt_name);
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
		lstrcpy(szBuf,code2key((KEYCODE) par));
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
	MACROREFTYPE	nType;
	MACROREFIDX		nIndex;
	KEYCODE			keycode;
	KEYBIND *		kp;
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
						rsc_switchtotable(&_keytables,szName);
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
				if ((kp = keybound(keycode)) != 0 &&
				     error_displayYesNoConfirmation(IDS_MSGDELOLDKEYBIND,
						 mac_name(szName,kp->macref.index,
						 		kp->macref.typ)) == IDNO)
					break;
				macro_bindOrUnbindKey(keycode, (MACROREFIDX)HIWORD(nSelected),
					(MACROREFTYPE)LOWORD(nSelected));
				goto upd;

			case IDD_MACDELKEY:
				keycode = macro_getSelectedKey(hwnd);
				macro_bindOrUnbindKey(keycode,-1,0);
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

			case IDD_MACFINDKEY:
				if ((keycode = macro_getCurrentKeycode()) == K_DELETED) {
					break;
				}
				if ((kp = keybound(keycode)) == 0) {
					error_showErrorById(IDS_MSGKEYNOTBOUND);
				} else {
					nIndex = kp->macref.index;
					nType = kp->macref.typ;
					macro_selectByValue(hwnd, MAKELONG(nType, nIndex));
					macro_newMacroSelected(hwnd);
				}
				return TRUE;

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



