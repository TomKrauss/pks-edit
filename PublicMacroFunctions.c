/*
 * PublicMacroFunctions.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: functions accessible in user defined macros.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <CommCtrl.h>
#include <direct.h>
#include <stdio.h>
#include "trace.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "winterf.h"
#include "winfo.h"
#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "resource.h"
#include "edfuncs.h"
#include "history.h"
#include "xdialog.h"
#include "publicapi.h"
#include "fileutil.h"
#include "stringutil.h"
#include "pathname.h"
#include "edexec.h"
#include "errordialogs.h"
#include "documenttypes.h"
#include "findandreplace.h"
#include "winutil.h"
#include "crossreferencelinks.h"
#include "brackets.h"
#include "markpositions.h"
#include "customcontrols.h"
#include "comparefiles.h"
#include "linkedlist.h"

/*
 * Answer TRUE if a replacement had been performed before.
 */
extern BOOL find_replacementHadBeenPerformed();

extern int 		macro_executeMacro(MACROREF *mp);
extern int 		align_text(char *pszSearch, int scope, char filler, int flags);
extern int 		dlg_getListboxText(HWND hwnd, int id, void *szBuff);
extern int 		EdExecute(long flags, long unused, 
					LPSTR cmdline, LPSTR newdir, LPSTR errfile);
extern int 		clp_getdata(void);
extern int 		doctypes_assignDocumentTypeDescriptor(FTABLE *fp, EDIT_CONFIGURATION *linp);
extern int 		EdCharInsert(int c);
extern int 		undo_lastModification(FTABLE *fp);
extern int 		mac_compileMacros(void);
extern int		doctypes_addDocumentTypesToListView(HWND hwnd, const void* pSelected);

extern long		_multiplier;

static int		_scope = RNG_BLOCK;

/*------------------------------------------------------------
 * EdExit()
 */
int EdExit(int rc)
{
	PostMessage(hwndMain, WM_CLOSE, rc, 0L); 
	return 1;
}

/*------------------------------------------------------------
 * EdExitAndSave()
 */
int EdExitAndSave(int rc)
{ 	int ret;
	extern int _ExSave;

	_ExSave = 1;
	ret = EdExit(rc);
	_ExSave = 0;
	return ret;
}

/*------------------------------------------------------------
 * EdAlert()
 */
void EdAlert(long unused1, long unused2, char *s)
{
	if (s)
		error_displayAlertDialog(s);
}

/*--------------------------------------------------------------------------
 * EdCursorLeft()
 */
int EdCursorLeft(int mtype)
{
	return caret_moveLeftRight(ww_getCurrentEditorWindow(), -1, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
int EdCursorRight(int mtype)
{
	return caret_moveLeftRight(ww_getCurrentEditorWindow(), 1, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
int EdCursorUp(int mtype)
{
	return caret_moveUpOrDown(ww_getCurrentEditorWindow(), -1,mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
int EdCursorDown(int mtype)
{
	return caret_moveUpOrDown(ww_getCurrentEditorWindow(), 1,mtype);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
int EdChapterGotoBegin(int dir)
{
	return caret_advanceSection(ww_getCurrentEditorWindow(), dir,1);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
int EdChapterGotoEnd(int dir)
{
	return caret_advanceSection(ww_getCurrentEditorWindow(), dir,0);
}

/*--------------------------------------------------------------------------
 * EdParaGotoBegin()
 */
int EdParaGotoBegin(int dir)
{
	return caret_advanceParagraphFromCurrentLine(ww_getCurrentEditorWindow(), dir,1);
}

/*--------------------------------------------------------------------------
 * EdParaGotoEnd()
 */
int EdParaGotoEnd(int dir)
{
	return caret_advanceParagraphFromCurrentLine(ww_getCurrentEditorWindow(), dir,0);
}

/*--------------------------------------------------------------------------
 * EdUndo()
 */
int EdUndo(void) {
	FTABLE* fp = ft_getCurrentDocument();
	if (!fp) {
		return 0;
	}
	return undo_lastModification(fp);
}

/*--------------------------------------------------------------------------
 * EdUndo()
 */
int EdRedo(void) {
	FTABLE* fp = ft_getCurrentDocument();
	if (!fp) {
		return 0;
	}
	return undo_redoLastModification(fp);
}

/*--------------------------------------------------------------------------
 * EdBlockCopy()
 */
int EdBlockCopy(void)
{	 return EdBlockCopyOrMove(0);	}

/*--------------------------------------------------------------------------
 * EdBlockMove()
 */
int EdBlockMove(void)
{	return EdBlockCopyOrMove(1);	}

/*--------------------------------------------------------------------------
 * bl_hideSelectionInCurrentWindow()
 */
int bl_hideSelectionInCurrentWindow(void)	
{	return bl_hideSelection(ww_getCurrentEditorWindow(), 1);		}

/*--------------------------------------------------------------------------
 * EdSetup()
 */
int EdSetup(void)
{ 
	return prof_save(GetConfiguration(), TRUE);
}

/*--------------------------------------------------------------------------
 * DialogCharInput()
 */
int DialogCharInput(int promptfield, unsigned char c)
{ 	static unsigned char _c;
	static ITEMS	_i   =  	{ C_CHAR1PAR, &_c 		};
	static PARAMS	_bgc = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_WINTITLE,		0,			0,
		IDD_POSITIONTCURS,	0,			0,
		IDD_RAWCHAR,		sizeof _c,	&_c,
		0
	};

	_d->dp_size = promptfield;
	_c = c;
	if (!win_callDialog(DLGBOXC,&_bgc,_d, NULL))
		return -1L;
	return _c;
}

/*--------------------------------------------------------------------------
 * cpyout()
 */
static void cpyout(char *d,char *s, int len)
{
	if (len > 40)
		len = 40;

	while(len > 0) {
		if ((*d++ = *s++) == 0) {
			d[-1] = '\\';
			*d++ = '0';
		}
		len--;
	}
	*d = 0;
}

/*------------------------------------------------------------
 * dlg_queryReplaceWindowProc()
 */
static HWND hwndQueryReplace;
static WPARAM _answer;
static INT_PTR CALLBACK dlg_queryReplaceWindowProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {

		case WM_INITDIALOG:
			win_positionWindowRelativeToCaret(hDlg);
			break;

		case WM_COMMAND:
			_answer = wParam;
			return TRUE;

		case WM_CLOSE:
			_answer = IDCANCEL;
			return TRUE;

		case WM_DESTROY:
			hwndQueryReplace = 0;
			return TRUE;
	}
	return dlg_defaultWndProc(hDlg, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * dlg_queryReplace()
 * Open a modeless dialog to query for replacing a text.
 */
int dlg_queryReplace(char *search, int slen, char *replace, int dlen) {	
	char   			sbuf[512],rbuf[512];
	MSG				msg;
	static DLGPROC	lpfnProc;

	cpyout(sbuf,search,slen);
	cpyout(rbuf,replace,dlen);

	EnableWindow(hwndMain,FALSE);

	win_createModelessDialog(&hwndQueryReplace,MAKEINTRESOURCE(DLGQUERYREPLACE),
					 dlg_queryReplaceWindowProc,&lpfnProc);

	if (!hwndQueryReplace)
		return IDCANCEL;

	win_positionWindowRelativeToCaret(hwndQueryReplace);

	SetDlgItemText(hwndQueryReplace,IDD_STRING1,sbuf);
	SetDlgItemText(hwndQueryReplace,IDD_STRING2,rbuf);

	_answer = 0;
	while(_answer == 0 && GetMessage(&msg,0,0,0)) {
		if (!IsDialogMessage(hwndQueryReplace,&msg))
			DispatchMessage(&msg);
	}

	return (int) _answer;
}

/*--------------------------------------------------------------------------
 * dlg_closeQueryReplace()
 */
void dlg_closeQueryReplace(void)
{
	if (hwndQueryReplace) {
		EnableWindow(hwndMain,TRUE);
		win_destroyModelessDialog(&hwndQueryReplace);
	}
}

/*--------------------------------------------------------------------------
 * dlg_displayRecordMacroOptions()
 * Open a window to start recording macro. Returns 1, if the action to record the
 * macro should really start.
 */
int dlg_displayRecordMacroOptions(int *o)
{	static int opt;
	static DIALPARS _d[] = {
		IDD_OPT1,		FORM_SHOW,	&opt,
		IDD_OPT2,		FORM_INIT,	&opt,
		IDD_OPT3,		FORM_REDRAW,	&opt,
		0
	};

	if (!DoDialog(DLGRECORDER,dlg_standardDialogProcedure,_d, NULL))
		return 0;
	*o = opt;
	return 1;
}

/*------------------------------------------------------------
 * EdAbout()
 */
int EdAbout(void)
{
	char _customerMessage[] = "free version (buy us a beer)";

#if defined(_WIN64)
	char _architecture[] = "64 Bit";
#else
	char _architecture[] = "32 Bit";
#endif
	char _versionInfo[128];

	DIALPARS _d[] = {
		IDD_RO1,		sizeof _customerMessage, _customerMessage,
		IDD_STRING1,	sizeof _architecture, _architecture,
		IDD_STRING2,	sizeof _versionInfo, _versionInfo,
		0
	};
	sprintf(_versionInfo, "%s, %s", "2.1.0", __DATE__);
	return DoDialog(DLGABOUT, dlg_standardDialogProcedure, _d, NULL);
}

/*--------------------------------------------------------------------------
 * dialogGetNumber()
 */
static long dialogGetNumber(int nDialog)
{	static long    num;
	static ITEMS	_i   =  	{ C_LONG1PAR,  (unsigned char * ) &num };
	static PARAMS	_np  = 	{ 1, P_MAYOPEN, _i	  };
	static DIALPARS _d[] = {
		IDD_LONG1,	sizeof num,		&num,
		0
	};

	if (!win_callDialog(nDialog,&_np,_d, NULL))
		return -1L;
	return num;
}

/*--------------------------------------------------------------------------
 * EdAlignText()
 */
int EdAlignText(void)
{	static int  _alflags;
	static char _alfiller;
	static DIALPARS _d[] = {
		/* attention!!! FINDS must be placed after find options to
		   make sure correct evalation of options during compile
	      */
		IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		IDD_CHAR,		sizeof _alfiller,	&_alfiller,
		IDD_RNGE,		RNG_LINE,			&_scope,
		IDD_REGEXP,	RE_DOREX,			& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,RE_SHELLWILD,		& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,RE_IGNCASE,		& _currentSearchAndReplaceParams.options,
		IDD_OPT2,		RE_IGNORE_BINARY,& _currentSearchAndReplaceParams.options,
		IDD_3S1,		AL_FIX,			&_alflags,
		IDD_3S2,		AL_CPOS,			&_alflags,
		IDD_OPT1,		AL_END,			&_alflags,
		0
	};
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern	 				},
		{ C_INT1PAR, (unsigned char *) &_alfiller 	},
		{ C_INT1PAR, (unsigned char *) &_scope	 	},
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options 	},
		{ C_INT1PAR, (unsigned char *) &_alflags	}
	};
	static PARAMS	_fp = { DIM(_i), P_MAYOPEN, _i	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	bl_getSelectedText(_currentSearchAndReplaceParams.searchPattern, sizeof _currentSearchAndReplaceParams.searchPattern);
	if (!win_callDialog(DLGALIGN, &_fp, _d, _tt)) {
		return 0;
	}
	return align_text(_currentSearchAndReplaceParams.searchPattern, _scope, _alfiller, _alflags);
}

/*--------------------------------------------------------------------------
 * EdFormatText()
 */
int EdFormatText(void)
{	
	static FORMATTING_ALIGNMENT alignment = FMT_LEFT;
	DIALPARS _d[] = {
		IDD_RNGE,		RNG_LINE ,			&_scope,
		IDD_RADIO1,	FMT_JUSTIFIED-FMT_LEFT,&alignment,
		0
	};
	ITEMS _i = { 
		{ C_INT1PAR, (unsigned char *) &alignment     },
		{ C_INT1PAR, (unsigned char *) &_scope	}
	};
	PARAMS _fp = 	{ DIM(_i), P_MAYOPEN, _i   };


	if (!win_callDialog(DLGFORMAT,&_fp,_d, NULL))
		return 0;

	return ft_formatText(ww_getCurrentEditorWindow(), _scope, alignment);
}

/*--------------------------------------------------------------------------
 * EdSort()
 */
int EdSort(void)
{	static char key[50],fs[32];
	static int  flags;
	static DIALPARS _d[] = {
		IDD_RNGE,		RNG_LINE ,		&_scope,
		IDD_STRING1,	sizeof fs,		&fs,
		IDD_STRING2,	sizeof key,		&key,
		IDD_REGEXP,	RE_DOREX,			& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,RE_SHELLWILD,		& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,RE_IGNCASE,		& _currentSearchAndReplaceParams.options,
		IDD_FINDS2,	sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		IDD_OPT1,		SO_REVERSE,		&flags,
		IDD_OPT2,		SO_CLUSTERLINES,	&flags,
		IDD_OPT3,		SO_SKIPSEPS,		&flags,
		0
	};
	static ITEMS	_i   = {
		{ C_STRING1PAR,_currentSearchAndReplaceParams.searchPattern },
		{ C_STRING1PAR,fs },
		{ C_STRING1PAR,key },
		{ C_INT1PAR,	(unsigned char *)&flags },
		{ C_INT1PAR,	(unsigned char *)&_scope }
	};
	static PARAMS	_sp = { DIM(_i), P_MAYOPEN, _i };
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	bl_getSelectedText(_currentSearchAndReplaceParams.searchPattern, sizeof _currentSearchAndReplaceParams.searchPattern);
	if (!win_callDialog(DLGSORT,&_sp,_d, _tt))
		return 0;

	return ft_sortFile(ft_getCurrentDocument(), _scope,fs,key, _currentSearchAndReplaceParams.searchPattern,flags);
}

/*--------------------------------------------------------------------------
 * EdKeycodeInsert()
 * insert ascii text for keycode
 */
int EdKeycodeInsert(void)
{	char 	*visible;
	static KEYCODE 	 k;
	static ITEMS		_i   = { C_INT1PAR,	(unsigned char *)&k };
	static PARAMS		_sp  = { DIM(_i), P_MAYOPEN, _i };
	static DIALPARS 	_d[] = {
		IDD_KEYCODE,	sizeof k,	&k,
		0
	};

	if (!win_callDialog(DLGKEYCODE,&_sp,_d, NULL))
		return 0;

	visible = code2key(k);
	return EdPasteString(0L, 0L, visible);
}

/*--------------------------------------------------------------------------
 * EdSetMultiplier()
 */
extern long _multiplier;
int EdSetMultiplier(void)
{
	if ((_multiplier = dialogGetNumber(DLGMULTI)) > 0L) 
		return 1;
	_multiplier = 1;
	return 0;
}

/*--------------------------------------------------------------------------
 * EdGotoLine()
 */
int EdGotoLine(void) {	
	long ln;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == NULL) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	if ((ln = dialogGetNumber(DLGGOTOLINE)) > 0L) {
		if (ln > fp->nlines) {
			// TODO:::
		}
		else {
			return caret_placeCursorMakeVisibleAndSaveLocation(wp, ln - 1L, 0L);
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdMarkSet()
 */
static unsigned char _lastmarkc;
int EdMarkSet(void)
{	WINFO *wp = ww_getCurrentEditorWindow();

	_lastmarkc = DialogCharInput(IDS_MARKSET,_lastmarkc);
	return 
		(mark_set(wp,wp->caret.linePointer,wp->caret.offset,_lastmarkc) != 0);
}

/*--------------------------------------------------------------------------
 * EdMarkGoto()
 */
int EdMarkGoto(void)
{
	long 		x,y;
	WINFO* wp = ww_getCurrentEditorWindow();

	_lastmarkc = DialogCharInput(IDS_MARKGOTO,_lastmarkc);
	if (mark_goto(wp, _lastmarkc, &x, &y) == 0) {
		error_showErrorById(IDS_MSGMARKUNDEF); 
		return 0;
	} else {
		return caret_placeCursorMakeVisibleAndSaveLocation(wp, x,y);
	}
}

/*--------------------------------------------------------------------------
 * EdReplaceTabs()
 * expand = 1 : expand TABS to SPACES
 * expand = 0 : comp SPACES to TABS
 */
static DIALPARS _dconvert[] = {
	IDD_WINTITLE,	IDS_COMPRESSTABS,	0,
	IDD_RNGE,		RNG_LINE ,		&_scope,
	0
};
int EdReplaceTabs(int expand)
{	static ITEMS	 _i   = { C_INT1PAR,  (unsigned char*)&_scope	};
	static PARAMS	 _tp  = { 1, P_MAYOPEN, _i };

	_dconvert->dp_size = expand ? IDS_EXPANDTABS : IDS_COMPRESSTABS;

	if (!win_callDialog(DLGCONVERT,&_tp,_dconvert, NULL))
		return 0;

	return find_replaceTabsWithSpaces(_scope,expand);
}

/*--------------------------------------------------------------------------
 * EdPromptAutosavePath()
 * Public PKS Edit command to prompt for an auto-save path.
 */
int EdPromptAutosavePath(char *path)
{
	static DIALPARS _d[] = {
		IDD_STRING1,	128, 		0,
		0
	};

	lstrcpy(path, config_getPKSEditTempPath());
	_d[0].dp_data = path;

	return DoDialog(DLGNEWASPATH, dlg_standardDialogProcedure,_d, NULL) == IDOK;
}

/*------------------------------------------------------------
 * winlist_lboxfill()
 */
static void winlist_lboxfill(HWND hwnd, int nItem, void* selValue) {

	SendDlgItemMessage(hwnd,nItem,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, FALSE, 0L);
	WINFO* wp = ww_getCurrentEditorWindow();
	while (wp) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LPARAM) wp);
		wp = wp->next;
	}
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING,-1, (LPARAM)selValue);
}

/*------------------------------------------------------------
 * dlg_drawFileInfo()
 * Draw the info about a file with a given index. Paints the index (used to select the file),
 * the name of the file a modification marker and the icon according to the file type.
 */
void dlg_drawFileInfo(HDC hdc, RECT *rcp, HWND hwnd, int nItem, BOOL bSelected) {
	char	szBuf[128];
	
	if (hwnd == 0) {
		return;
	}
	GetWindowText(hwnd, szBuf, sizeof szBuf);
	HICON hIcon = (HICON)SendMessage(hwnd, WM_GETICON, 0, 0L);
	cust_drawListBoxRowWithIcon(hdc, rcp, hIcon, szBuf);
}


static winlist_drawFileInfo(HDC hdc, RECT* rcp, WINFO* wp, int nItem, int nCtl) {
	dlg_drawFileInfo(hdc, rcp, wp->edwin_handle, nItem, FALSE);
}

/*--------------------------------------------------------------------------
 * infoFillParams()
 */
static void infoFillParams(DIALPARS *dp, WINFO *wp) {
	static char szCodePage[32];
	FTABLE* fp = wp->fp;
	dp->dp_data = string_getBaseFilename(fp->fname);			dp++;

	string_formatDate(dp->dp_data, &fp->ti_created); 	dp++;
	string_formatDate(dp->dp_data, &fp->ti_modified); 	dp++;
	string_formatDate(dp->dp_data, &fp->ti_saved);  	dp++;
	wsprintf(dp->dp_data,"%ld", ft_size(fp)); 	dp++;
	wsprintf(dp->dp_data, "%ld", fp->nlines); dp++;
	wsprintf(dp->dp_data, "%ld", ft_countWords(fp)); dp++;
	dp->dp_data = szCodePage;
	ft_getCodepageName(fp, szCodePage, sizeof szCodePage);
}

/*--------------------------------------------------------------------------
 * winlist_command()
 */
static DIALPARS infoDialListPars[] = {
	IDD_RO1,			128,			0,
	IDD_RO7,			128,			0,
	IDD_RO2,			128,			0,
	IDD_RO3,			128,			0,
	IDD_RO4,			128,			0,
	IDD_RO5,			128,			0,
	IDD_RO6,			128,			0,
	IDD_RO8,			128,			0,
	IDD_WINDOWLIST,		0,			0,
	IDD_WINTITLE,		0,			0,
	0
};
static void winlist_command(HWND hDlg, int nItem,  int nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		dlg_getListboxText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			infoFillParams(infoDialListPars, *(WINFO **)pUser);
			DoDlgInitPars(hDlg, infoDialListPars, 8);
		} else {
			PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		}
	}
}

/*--------------------------------------------------------------------------
 * showWindowList()
 */
static int showWindowList(int nTitleId)
{
	WINFO *wp;
	DIALLIST dlist = {
		(long long*)&wp, 
		winlist_lboxfill, 
		dlg_getListboxText, 
		cust_measureListBoxRowWithIcon,
		winlist_drawFileInfo,
		winlist_command 
	};
	char	dmod[40],dsaved[40], dcreated[40], nbytes[20],nlines[20], nwords[20];
	int		nRet;

	wp = ww_getCurrentEditorWindow();

	if (!wp) {
		return 0;
	}
     infoDialListPars[1].dp_data = dmod;  
	 infoDialListPars[2].dp_data = dcreated;
	 infoDialListPars[3].dp_data = dsaved;
     infoDialListPars[4].dp_data = nbytes;
     infoDialListPars[5].dp_data = nlines;
	 infoDialListPars[6].dp_data = nwords;
	 infoDialListPars[8].dp_data = &dlist;
	infoDialListPars[9].dp_size = nTitleId;

	infoFillParams(infoDialListPars, wp);
	nRet = DoDialog(DLGINFOFILE, dlg_standardDialogProcedure,infoDialListPars, NULL);
	if (nRet == IDCANCEL || wp == 0) {
		return 0;
	}

	return EdSelectWindow(wp->win_id);
}

/*--------------------------------------------------------------------------
 * EdInfoFiles()
 */
int EdInfoFiles(void)
{
	return showWindowList(IDS_WINDOWLIST);
}

/*--------------------------------------------------------------------------
 * dlg_openCompareDialog()
 */
static WINFO* _compareFile1;
static INT_PTR CALLBACK compare_dialogProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR nRet = dlg_standardDialogProcedure(hDlg, message, wParam, lParam);
	if (message == WM_INITDIALOG) {
		LRESULT idx = SendDlgItemMessage(hDlg, IDD_WINDOWLIST, LB_GETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(hDlg, IDOK), idx != 0);
		EnableWindow(GetDlgItem(hDlg, IDD_BUT3), _compareFile1 && ft_backupFileExists(_compareFile1->fp));
	}
	return nRet;
}

static DIALPARS compareDialListPars[] = {
	IDD_RO1,			128,		0,
	IDD_WINDOWLIST,		0,			0,
	0
};
static void dlg_openCompareCommand(HWND hDlg, int nItem, int nNotify, void* pUser) {
	switch (nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		dlg_getListboxText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			EnableWindow(GetDlgItem(hDlg, IDOK), *(WINFO**)pUser != _compareFile1);
		} else {
			PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		}
	}
}

/*--------------------------------------------------------------------------
 * EdFilesCompare()
 */
int EdFilesCompare(int dir) {
	WINFO* wp;
	DIALLIST dlist = {
		(long long*)&wp,
		winlist_lboxfill,
		dlg_getListboxText,
		cust_measureListBoxRowWithIcon,
		winlist_drawFileInfo,
		dlg_openCompareCommand
	};
	int		nRet;

	_compareFile1 = ww_getCurrentEditorWindow();
	if (!_compareFile1) {
		return 0;
	}
	wp = _compareFile1->next;
	FTABLE* fp = _compareFile1->fp;
	compareDialListPars[0].dp_data = fp->fname;
	compareDialListPars[1].dp_data = &dlist;
	nRet = DoDialog(DLGCOMPAREFILES, compare_dialogProcedure, compareDialListPars, NULL);
	if (nRet == IDD_BUT3) {
		FTABLE* fp = ft_openBackupfile(_compareFile1->fp);
		if (fp == NULL) {
			return 0;
		}
		wp = WIPOI(fp);
	}
	if (nRet == IDCANCEL || wp == 0 || wp == _compareFile1) {
		return 0;
	}
	return compare_files(_compareFile1, wp);
}

/*--------------------------------------------------------------------------
 * DocTypecommand()
 */
static void doctypes_newType(HWND hDlg);
static void doctypes_deleteType(HWND hDlg);
static void doctypes_apply(void);
static void doctypes_changeType(HWND hDlg);
static void doctypes_fillParameters(DIALPARS *dp, void *par);
static DIALPARS docTypePars[] = 
{
	IDD_STRING6,		16,						0,
	IDD_STRING4,		50,						0,
	IDD_PATH1,		84,						0,
	IDD_STRING5,		84,						0,
	IDD_STRING3,		32,						0,
	IDD_OPT1,			1,						0,
	IDD_CALLBACK1,		0,						doctypes_changeType,
	IDD_CALLBACK2,		0,						doctypes_deleteType,
	IDD_CALLBACK3,		0,						doctypes_apply,
	IDD_NOINITCALLBACK,	0,						doctypes_newType,
	0
};

#define	  NVDOCTYPEPARS						5

/*------------------------------------------------------------
 * doctypes_fillListbox()
 */
static void doctypes_fillListbox(HWND hwnd, void* selValue) {
	HWND hwndList = GetDlgItem(hwnd, IDC_DOCTYPES);
	doctypes_addDocumentTypesToListView(hwndList, selValue);
}

/*--------------------------------------------------------------------------
 * doctypes_fillParameters()
 */
static void doctypes_fillParameters(DIALPARS *dp, void *par)
{
	char *	pszId;
	char *	pszDescription;
	char *	pszMatch;
	char *	pszFname;
	char*	pszGrammar;
	int *	pOwn;

	if (!doctypes_getDocumentTypeDescription(par, &pszId, &pszDescription, &pszMatch, 
& pszFname, & pszGrammar, & pOwn)) {
return;
	}

	dp->dp_data = pszId;					dp++;
	dp->dp_data = pszDescription;			dp++;
	dp->dp_data = pszFname;					dp++;
	dp->dp_data = pszMatch;					dp++;
	dp->dp_data = pszGrammar;				dp++;
	dp->dp_data = pOwn;
}

/*--------------------------------------------------------------------------
 * doctypes_apply()
 */
DOCUMENT_TYPE* lastSelectedDocType;
static void doctypes_apply(void)
{
	EDIT_CONFIGURATION* lp;
	FTABLE* fp;

	if ((fp = ft_getCurrentDocument()) == 0) {
		return;
	}
	if ((lp = doctypes_getDocumentTypeDescriptor(lastSelectedDocType)) != 0) {
		if (!(fp->flags & F_MODIFIED)) {
			ft_abandonFile(fp, lp);
		}
		else {
			doctypes_assignDocumentTypeDescriptor(fp, lp);
			doctypes_documentTypeChanged(TRUE);
		}
	}
}

/*--------------------------------------------------------------------------
 * doctypes_newType()
 */
static void doctypes_newType(HWND hDlg)
{
	lastSelectedDocType = doctypes_createDocumentType(lastSelectedDocType);
	doctypes_fillListbox(hDlg, lastSelectedDocType);
	doctypes_fillParameters(docTypePars, lastSelectedDocType);
	dlg_retrieveParameters(hDlg, docTypePars, NVDOCTYPEPARS);
}

/*--------------------------------------------------------------------------
 * doctypes_deleteType()
 */
static void doctypes_deleteType(HWND hDlg)
{
	doctypes_deleteDocumentType(lastSelectedDocType);
	lastSelectedDocType = 0;
	doctypes_fillListbox(hDlg, lastSelectedDocType);
	doctypes_fillParameters(docTypePars, lastSelectedDocType);
}

/*--------------------------------------------------------------------------
 * doctypes_changeType()
 */
static void doctypes_changeType(HWND hDlg)
{
	dlg_retrieveParameters(hDlg, docTypePars, NVDOCTYPEPARS);
	doctypes_fillListbox(hDlg, (void*)lastSelectedDocType);
}

/*
 * Domain adapter for the list view displaying the document types.
 */
static void doctypes_getColumnParameters(NMLVDISPINFO* plvdi) {
	char* pszId;
	char* pszDescription;
	char* pszMatch;
	char* pszFname;
	char* pszGrammar;
	int* pOwn;

	if (!doctypes_getDocumentTypeDescription((void*)plvdi->item.lParam, &pszId, &pszDescription, &pszMatch,
		&pszFname, &pszGrammar, &pOwn)) {
		return;
	}
	switch (plvdi->item.iSubItem)
	{
	case 0:
		plvdi->item.pszText = pszId;
		break;
	case 1:
		plvdi->item.pszText = pszDescription;
		break;
	case 2:
		plvdi->item.pszText = pszMatch;
		break;
	}
}

static void doctype_initListView(HWND hwnd) {
	LVCOLUMN lvc;

	ListView_SetExtendedListViewStyleEx(hwnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "Type";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;

	ListView_InsertColumn(hwnd, 0, &lvc);

	lvc.pszText = "Description";
	lvc.cx = 250;
	lvc.iSubItem = 1;
	ListView_InsertColumn(hwnd, 1, &lvc);

	lvc.pszText = "Name pattern";
	lvc.cx = 100;
	lvc.iSubItem = 2;
	ListView_InsertColumn(hwnd, 2, &lvc);
}

/*
 * Custom dialog procedure for doctype dialog procedure.
 */
static INT_PTR doctype_dialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) {
			LPNMLISTVIEW pActivate = (LPNMLISTVIEW)lParam;
			if (pActivate->uNewState) {
				doctypes_fillParameters(docTypePars, (void*)pActivate->lParam);
				DoDlgInitPars(hwnd, docTypePars, NVDOCTYPEPARS);
			}
		} else if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO) {
			NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
			doctypes_getColumnParameters(plvdi);
		}
		break;
	case WM_INITDIALOG:
		doctype_initListView(GetDlgItem(hwnd, IDC_DOCTYPES));
		doctypes_fillListbox(hwnd, lastSelectedDocType);

		break;
	}
	return dlg_standardDialogProcedure(hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * doDocumentTypes()
 */
static int doDocumentTypes(int nDlg) {
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE* fp = wp ? wp->fp : NULL;
	EDIT_CONFIGURATION* pConfig = fp ? fp->documentDescriptor : NULL;
	lastSelectedDocType = doctypes_getPrivateDocumentType(
		pConfig ? pConfig->name : "default");

	doctypes_fillParameters(docTypePars, (void*)lastSelectedDocType);
	if (DoDialog(nDlg, doctype_dialogProcedure,docTypePars, NULL) == IDCANCEL) {
		lastSelectedDocType = 0;
		return 0;
	}

	if (nDlg != DLGDOCTYPES) {
		return TRUE;
	}
	doctypes_documentTypeChanged(TRUE);
	return TRUE;
}

/*--------------------------------------------------------------------------
 * EdDocTypes()
 */
int EdDocTypes(void)
{
	return doDocumentTypes(DLGDOCTYPES);
}

/*--------------------------------------------------------------------------
 * EdRangeShift()
 */
int EdRangeShift(int dir)
{	static ITEMS	 _i   = { C_INT1PAR,  (unsigned char*)&_scope	};
	static PARAMS	 _tp  = { 1, P_MAYOPEN, _i };

	_dconvert->dp_size = dir < 0 ? IDS_SHIFTLEFT : IDS_SHIFTRIGHT;
	if (!win_callDialog(DLGCONVERT,&_tp,_dconvert, NULL))
		return 0;
		
	return uc_shiftRange(_scope,dir);
}

/*--------------------------------------------------------------------------
 * color_lboxfill()
 */
static void color_lboxfill(HWND hwnd, int nItem, void* selValue)
{
	COLORREF		cColor;
	HDC			hdc;
	int			i;
	int			nPaletteSize = 32;
	int			nSelIndex;

	hdc = GetDC(hwnd);
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT,0,0L);
	for (i = 0, nSelIndex = 0; i < nPaletteSize; i++) {
		cColor = GetNearestColor (hdc, PALETTEINDEX (i));
		if (cColor == (COLORREF)(uintptr_t)selValue) {
			nSelIndex = i;
		}
		SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, i);
	}
	SendDlgItemMessage(hwnd, nItem, CB_SETCURSEL, nSelIndex, 0L);

	ReleaseDC(hwnd, hdc);	
}

/*------------------------------------------------------------
 * color_drawitem()
 */
static void color_drawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	HPEN		hPen;
	HPEN		hOldPen;
	HBRUSH	hBrush;
	HBRUSH	hOldBrush;

	hPen = GetStockObject(BLACK_PEN);
	hOldPen = SelectObject(hdc, hPen);

	hBrush = CreateSolidBrush(PALETTEINDEX((intptr_t) par));
	hOldBrush = SelectObject(hdc, hBrush);
	Rectangle(hdc, rcp->left + 2, rcp->top + 2, rcp->right - 2, rcp->bottom - 2);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);

	DeleteObject(hBrush);
}

/*--------------------------------------------------------------------------
 * color_getitem()
 */
static int color_getitem(HWND hwnd, int id, char *szBuff)
{ 	
	LRESULT	item;
	HDC		hDC;

	item = SendDlgItemMessage(hwnd, id, CB_GETCURSEL, 0, 0);
	if (item == CB_ERR) {
		*(long *)szBuff = 0;
		return CB_ERR;
	}

	hDC = GetDC(hwnd);
	*(long *)szBuff = (long) GetNearestColor (hDC, PALETTEINDEX (item));
	ReleaseDC(hwnd, hDC);	
	return 1;
}

/*--------------------------------------------------------------------------
 * color_showselection()
 */
static void color_showselection(DRAWITEMSTRUCT *dp)
{
	HPEN		hPen;
	HPEN		hOldPen;
	HBRUSH	hBrush;
	HBRUSH	hOldBrush;
	RECT *	rcp;

	rcp = (RECT *)&dp->rcItem;

	hPen = GetStockObject(BLACK_PEN);
	hOldPen = SelectObject(dp->hDC, hPen);

	hBrush = GetStockObject(NULL_BRUSH);
	hOldBrush = SelectObject(dp->hDC, hBrush);

	Rectangle(dp->hDC, rcp->left, rcp->top, rcp->right, rcp->bottom);
	SelectObject(dp->hDC, hOldBrush);
	SelectObject(dp->hDC, hOldPen);

	SelectObject(dp->hDC, hOldPen);
}

/*--------------------------------------------------------------------------
 * dlg_configureEditorModes()
 */
static DIALPARS* _paramsPerPage[4];

static DIALPARS* _getDialogParsForPage(int pageIndex) {
	return _paramsPerPage[pageIndex];
}

int dlg_configureEditorModes(void) {
	int scrollmin, mindelta, cursafter, flags;
	char 	status[64];
	long 	bgcolor;
	char 	tabDisplayFillCharacter;
	int  	dispmode;
	int	tabsize;
	int	rmargin;
	int 	nl1, nl2, cr;
	char 	backupExtension[4];
	int	opt;
	int	crypt;
	DIALPARS _dFileFormat[] = {
		IDD_STRING1,	sizeof backupExtension,	backupExtension,
		IDD_INT1,		sizeof nl1,	&nl1,
		IDD_INT2,		sizeof nl2,	&nl2,
		IDD_INT3,		sizeof cr,	&cr,
		IDD_OPT1,		1,			&opt,
		IDD_OPT2,		2,			&opt,
		IDD_OPT3,		4,			&opt,
		IDD_OPT4,		O_CRYPTED,	&crypt,
		0
	};
	DIALLIST dlist = {
		(long long*)&bgcolor, color_lboxfill, color_getitem, 0, color_drawitem,
		0, 0, color_showselection };
	DIALPARS _dDisplayMode[] = {
		IDD_STRING1,	sizeof status,		status,
		IDD_CHAR,		sizeof tabDisplayFillCharacter,	&tabDisplayFillCharacter,
		IDD_FONTSEL2COLOR,	0,			&dlist,
		IDD_INT1,		sizeof tabsize,	&tabsize,
		IDD_INT2,		sizeof rmargin,	&rmargin,
		IDD_OPT1,		SHOWCONTROL,		&dispmode,
		IDD_OPT4,		SHOWHEX,			&dispmode,
		IDD_OPT5,		SHOWRULER,			&dispmode,
		IDD_OPT6,		SHOWWYSIWYG,		&dispmode,
		IDD_OPT7,		SHOW_SYNTAX_HIGHLIGHT,& dispmode,
		IDD_OPT8,		SHOWHIDEVSLIDER,	&dispmode,
		IDD_OPT9,		SHOWHIDEHSLIDER,	&dispmode,
		IDD_OPT10,		SHOWLINENUMBERS,	&dispmode,
		IDD_OPT11,		SHOWCARET_LINE_HIGHLIGHT, &dispmode,
		0
	};
	char creationMacroName[20];
	char cm[20];
	char tabfill;
	int  workmode;
	int fileflag;
	DIALPARS _dCursorBehavior[] = {
		IDD_INT3,		sizeof mindelta,	&mindelta,
		IDD_INT4,		sizeof scrollmin,	&scrollmin,
		IDD_OPT1,		SC_THUMBTRACK,		&flags,
		IDD_OPT2,		SC_CURSORCATCH,		&flags,
		IDD_OPT3,		SHOWCARET_PRESERVE_COLUMN,& dispmode,
		IDD_RADIO1,	CP_POSLOW - CP_POSTOP,&cursafter,
		0
	};
	DIALPARS _dWorkMode[] = {
		IDD_STRING2,	sizeof creationMacroName,	creationMacroName,
		IDD_STRING3,	sizeof cm,	cm,
		IDD_CHAR,		sizeof tabfill,&tabfill,
		IDD_OPT1,		WM_INSERT,	&workmode,
		IDD_OPT2,		WM_ABBREV,	&workmode,
		IDD_OPT3,		WM_AUTOINDENT,	&workmode,
		IDD_OPT4,		WM_AUTOWRAP,	&workmode,
		IDD_OPT5,		WM_AUTOFORMAT,	&workmode,
		IDD_OPT6,		WM_OEMMODE,	&workmode,
		IDD_OPT7,		F_RDONLY,		&fileflag,
		IDD_OPT8,		WM_SHOWMATCH,	&workmode,
		IDD_OPT10,		WM_DELETE_MULTIPLE_SPACES, &workmode,
		IDD_OPT11,		F_WATCH_LOGFILE,&fileflag,
		0
	};

	ITEMS _i = {
		{ C_STRING1PAR, 	status }, 
		{ C_INT1PAR, 		(unsigned char *) &tabsize },
		{ C_INT1PAR, 		(unsigned char *) &rmargin },
		{ C_CHAR1PAR, 		(unsigned char *) &tabDisplayFillCharacter },
		{ C_LONG1PAR, 		(unsigned char *) &bgcolor },
		{ C_INT1PAR, 		(unsigned char *) &dispmode }
	};
	PROPSHEETPAGE psp[4];
	PROPSHEETHEADER psh;
	EDIT_CONFIGURATION *linp;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == 0) {
		return 0;
	}
	FTABLE* fp = wp->fp;

	linp = fp->documentDescriptor;
	_paramsPerPage[0] = _dDisplayMode;
	_paramsPerPage[1] = _dWorkMode;
	_paramsPerPage[2] = _dFileFormat;
	_paramsPerPage[3] = _dCursorBehavior;

	lstrcpy(backupExtension, linp->backupExtension);
	opt = 0;
	if ((nl1 = linp->nl) >= 0) {
		opt |= 1;
	}
	if ((nl2 = linp->nl2) >= 0) {
		opt |= 2;
	}
	if ((cr = linp->cr) >= 0) {
		opt |= 4;
	}
	crypt = linp->workmode & O_CRYPTED;

	flags = linp->scrollflags;
	cursafter = linp->cursaftersearch;
	scrollmin = linp->vscroll + 1;
	mindelta = linp->scroll_dy;
	lstrcpy(status,linp->statusline);
	tabsize = linp->tabsize;
	rmargin = linp->rmargin;
	dispmode = wp->dispmode;
	tabDisplayFillCharacter = linp->tabDisplayFillCharacter;
	linp = fp->documentDescriptor;
	lstrcpy(creationMacroName, linp->creationMacroName);
	lstrcpy(cm, linp->closingMacroName);
	workmode = linp->workmode;
	tabfill = linp->expandTabsWith;
	fileflag = fp->flags;

	dlg_setXDialogParams(_getDialogParsForPage, TRUE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[0]);
	psp[0].hInstance = ui_getResourceModule();
	psp[0].pszTemplate = MAKEINTRESOURCE(DLGDISPMODE);
	psp[0].pfnDlgProc = dlg_standardDialogProcedure;

	psp[1].dwSize = sizeof(psp[1]);
	psp[1].hInstance = ui_getResourceModule();
	psp[1].pszTemplate = MAKEINTRESOURCE(DLGWORKMODE);
	psp[1].pfnDlgProc = dlg_standardDialogProcedure;

	psp[2].dwSize = sizeof(psp[2]);
	psp[2].hInstance = ui_getResourceModule();
	psp[2].pszTemplate = MAKEINTRESOURCE(DLGFILEFORMAT);
	psp[2].pfnDlgProc = dlg_standardDialogProcedure;

	psp[3].dwSize = sizeof(psp[3]);
	psp[3].hInstance = ui_getResourceModule();
	psp[3].pszTemplate = MAKEINTRESOURCE(DLGCURSTABS);
	psp[3].pfnDlgProc = dlg_standardDialogProcedure;

	psh.dwSize = sizeof(psh);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_USECALLBACK;
	psh.hwndParent = hwndMain;
	psh.hInstance = ui_getResourceModule();
	psh.pszIcon = 0;
	psh.pszCaption = (LPSTR)"Editing Options...";
	psh.nPages = sizeof(psp) / sizeof(psp[0]);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback = (PFNPROPSHEETCALLBACK)dlg_propertySheetCallback;

	INT_PTR tempRet = PropertySheet(&psh);
	if (tempRet != 1) {
		return 0;
	}
	lstrcpy(linp->statusline,status);
	if (tabsize == 0) {
		tabsize = 8;
	}
	linp->dispmode = dispmode;
	linp->tabDisplayFillCharacter = tabDisplayFillCharacter;
	if ((linp->tabsize = tabsize) != 0) {
		ft_forAllViews(fp, ww_tabsChanged, linp);
	}
	linp->rmargin = rmargin;
	lstrcpy(linp->creationMacroName, creationMacroName);
	lstrcpy(linp->closingMacroName, cm);
	linp->workmode = workmode;
	linp->expandTabsWith = tabfill;
	fp->flags = fileflag;
	linp->workmode = (linp->workmode & (~O_CRYPTED)) | crypt;
	lstrcpy(linp->backupExtension, backupExtension);
	linp->nl = (opt & 1) ? nl1 : -1;
	linp->nl2 = (opt & 2) ? nl2 : -1;
	linp->cr = (opt & 4) ? cr : -1;
	linp->scrollflags = flags;
	linp->vscroll = scrollmin - 1;
	linp->scroll_dy = mindelta;
	linp->cursaftersearch = cursafter;
	return doctypes_documentTypeChanged(TRUE);
}

/*--------------------------------------------------------------------------
 * EdReplace()
 */
int EdReplace(void)
{	static int ret;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern },
		{ C_STRING1PAR, _currentSearchAndReplaceParams.replaceWith },
		{ C_INT1PAR, (unsigned char *) &ret },
		{ C_INT1PAR, (unsigned char *) &_scope },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options   }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_RNGE,		RNG_ONCE,			&_scope,
		IDD_REGEXP,		RE_DOREX,			& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,	RE_SHELLWILD,		& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,	RE_IGNCASE,		& _currentSearchAndReplaceParams.options,
		IDD_PRESERVE_CASE,RE_PRESERVE_CASE, & _currentSearchAndReplaceParams.options,
		IDD_FINDS,		sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		IDD_REPLS,		sizeof _currentSearchAndReplaceParams.replaceWith,		& _currentSearchAndReplaceParams.replaceWith,
		IDD_OPT1,		RE_CONFIRM_REPLACEMENT,	& _currentSearchAndReplaceParams.options,
		IDD_OPT2,		RE_CONSIDER_MARKED_LINES,	 & _currentSearchAndReplaceParams.options,
		IDD_RECORDRET,	sizeof ret,		&ret,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		IDD_PRESERVE_CASE,	IDS_TT_PRESERVE_CASE,
		0
	};
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!wp || win_callDialog(DLGREPLACE,&_fp,_d, _tt) == 0) {
		return 0;
	}

	return EdReplaceText(wp, _scope,ret);
}

/*--------------------------------------------------------------------------
 * EdFind()
 */
static int _dir = 1;
int EdFind(void)
{	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern },
		{ C_INT1PAR, (unsigned char *) &_dir   },
		{ C_INT1PAR, (unsigned char *) &_scope },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options   }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_RADIO1,	1,				&_dir,
		IDD_REGEXP,	RE_DOREX,			& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,RE_SHELLWILD,		& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,RE_IGNCASE,		& _currentSearchAndReplaceParams.options,
		IDD_OPT1,		O_WRAPSCAN,		& _currentSearchAndReplaceParams.options,
		IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern,	_currentSearchAndReplaceParams.searchPattern,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (_dir == -1) {
		_dir = 0;
	}
	bl_getSelectedText(_currentSearchAndReplaceParams.searchPattern, sizeof _currentSearchAndReplaceParams.searchPattern);
	if (!win_callDialog(DLGFIND, &_fp, _d, _tt)) {
		return 0;
	}

	if (_dir == 0)
		_dir = -1;

	return find_expressionAgainInCurrentFile(_dir);
}

/*
 * Custom window procedure for find in files dialog. 
 */
static INT_PTR find_inFilesDialogProc(HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam) {
	INT_PTR pRes = dlg_standardDialogProcedure(hDlg, wMessage, wParam, lParam);
	BOOL bEnable;
	switch (wMessage) {
	case WM_COMMAND:
		if (!(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDD_OPT4)) {
			break;
		}
		// drop through
	case WM_INITDIALOG:
		bEnable = SendDlgItemMessage(hDlg, IDD_OPT4, BM_GETCHECK, 0, 0) != BST_CHECKED;
		EnableWindow(GetDlgItem(hDlg, IDD_FILE_PATTERN), bEnable);
		EnableWindow(GetDlgItem(hDlg, IDD_PATH1), bEnable);
		EnableWindow(GetDlgItem(hDlg, IDD_INT1), bEnable);
		break;
	}
	return pRes;
}

/*--------------------------------------------------------------------------
 * EdFindInFileList()
 */
int EdFindInFileList(void)
{	ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern },
		{ C_STRING1PAR, _currentSearchAndReplaceParams.pathlist },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.fileScanDepth },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options }
	};
	PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	DIALPARS _d[] = {
		IDD_REGEXP,	RE_DOREX,					& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,RE_SHELLWILD,			& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,RE_IGNCASE,				& _currentSearchAndReplaceParams.options,
		IDD_OPT1,		RE_SEARCH_ONCE,			& _currentSearchAndReplaceParams.options,
		IDD_OPT2,		RE_IGNORE_BINARY,		& _currentSearchAndReplaceParams.options,
		IDD_OPT3,		RE_APPEND_TO_SEARCH_RESULTS,& _currentSearchAndReplaceParams.options,
		IDD_OPT4,		RE_SEARCH_IN_SEARCH_RESULTS,& _currentSearchAndReplaceParams.options,
		IDD_OPT5,		RE_CONFIRM_REPLACEMENT,	& _currentSearchAndReplaceParams.options,
		IDD_PRESERVE_CASE,	RE_PRESERVE_CASE,	& _currentSearchAndReplaceParams.options,
		IDD_INT1,		sizeof _currentSearchAndReplaceParams.fileScanDepth, & _currentSearchAndReplaceParams.fileScanDepth,
		IDD_FILE_PATTERN, sizeof _currentSearchAndReplaceParams.filenamePattern, & _currentSearchAndReplaceParams.filenamePattern,
		IDD_FINDS2,	sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		IDD_REPLS,		sizeof _currentSearchAndReplaceParams.replaceWith,& _currentSearchAndReplaceParams.replaceWith,
		IDD_PATH1,	sizeof _currentSearchAndReplaceParams.pathlist,	& _currentSearchAndReplaceParams.pathlist,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (!_currentSearchAndReplaceParams.filenamePattern[0] && hist_getString(FILE_PATTERNS, 0) == 0) {
		strcpy(_currentSearchAndReplaceParams.filenamePattern, "*.*");
	}
	if (!_currentSearchAndReplaceParams.pathlist[0]) {
		_getcwd(_currentSearchAndReplaceParams.pathlist, sizeof _currentSearchAndReplaceParams.pathlist);
	}
	bl_getSelectedText(_currentSearchAndReplaceParams.searchPattern, sizeof _currentSearchAndReplaceParams.searchPattern);
	int ret = win_callDialogCB(DLGFINDINFILES, &_fp, _d, _tt, find_inFilesDialogProc);
	if (ret == 0) {
		return 0;
	}
	return find_matchesInFiles(&_currentSearchAndReplaceParams, ret == IDD_BUT3 ? FIF_REPLACE : FIF_SEARCH);
}

/*--------------------------------------------------------------------------
 * EdReplaceAgain()
 */
int EdReplaceAgain(void) {
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp && find_replacementHadBeenPerformed())
		return EdReplaceText(wp, RNG_ONCE,REP_REPLACE); 
	error_showErrorById(IDS_MSGNOREPLACESTRING);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdFindAgain()
 */
int EdFindAgain(int dir)
{	
	if (!_currentSearchAndReplaceParams.searchPattern[0]) {
		error_showErrorById(IDS_MSGNOSEARCHSTRING);
		return 0;
	}
	return find_expressionAgainInCurrentFile((dir) ? (_dir = dir) : _dir);
}

/*--------------------------------------------------------------------------
 * macro_getIndexForKeycode()
 * Return the internall index for a given macro keycode and name.
 */
int macro_getIndexForKeycode(KEYCODE *scan,char *name,int oldidx)
{	static DIALPARS  _d[] = {
		IDD_STRING1,	MAC_NAMELEN,		_currentSearchAndReplaceParams.searchPattern,
		IDD_KEYCODE,	sizeof(KEYCODE),	0,
		0
	};

	_d[0].dp_data = name;
	_d[1].dp_data = scan;
	do {
		if (DoDialog(DLGMACNAME, dlg_standardDialogProcedure,_d, NULL) == IDCANCEL)
			return 0;
	} while (!macro_validateMacroName(name,oldidx, FALSE));
	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharControlInsert()
 */
int EdCharControlInsert(void)
{ 	static char c;
	static ITEMS	 _i   = { C_CHAR1PAR,  (unsigned char*)&c	};
	static PARAMS	 _p  = { 1, P_MAYOPEN, _i };
	static DIALPARS _d[] = { 
		IDD_POSITIONTCURS,	0,			0,
		IDD_CSEL,			sizeof(c),	&c,
		0
	};

   if (!win_callDialog(DLGCONTROLINS,&_p,_d, NULL)) {
		return 0;
	}
	return EdCharInsert(0x100|(int)(unsigned char)c);
}

/*--------------------------------------------------------------------------
 * EdPromptForCharacter()
 * Prompt for a character. The prompt message is defined via resource ID passed
 * as a parameter.
 */
int EdPromptForCharacter(int ids_num) 
{	static unsigned char c;

	c = DialogCharInput(ids_num,c);
	return c;
}

/*--------------------------------------------------------------------------
 * EdCompileMacros()
 */
int EdCompileMacros(int bShowList)
{
	if (!bShowList || showWindowList(IDS_SELECTCOMPILEWINDOW)) {
		return mac_compileMacros();
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdListBindings()
 */
int EdListBindings(long lWhich)
{
	if (lWhich & LIST_MACROS) {
		if (macro_saveMacrosAndDisplay((char*)0) == 0) {
			return 0;
		}
	}

	if (lWhich & LIST_KEYS) {
		if (macro_saveKeyBindingsAndDisplay() == 0) {
			return 0;
		}
	}


	if (lWhich & LIST_MICE) {
		if (macro_saveMouseBindingsAndDisplay() == 0) {
			return 0;
		}
	}

	if (lWhich & LIST_MENUS) {
		if (macro_saveMenuBindingsAndDisplay() == 0) {
			return 0;
		}
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * EdCommandExecute()
 */
int EdCommandExecute(void)
{
	static char	cmd[256];
	static char	dir[256];
	static char	errlist[64];
	static int  	opt;
	static int	redir;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, 	cmd }, 
		{ C_STRING1PAR, 	dir }, 
		{ C_STRING1PAR, 	errlist },
		{ C_INT1PAR, (unsigned char *) &opt }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_PATH1,	sizeof cmd,		cmd,
		IDD_STRING1,	sizeof dir,		dir,
		IDD_STRING2,	sizeof errlist,	errlist,
		IDD_OPT1,		EX_SYMBOL,		&opt,
		IDD_OPT2,		EX_WAIT,			&opt,
		IDD_OPT3,		EX_CD,			&opt,
		IDD_RADIO1,		4,				&redir,
		0
	};

	bl_getSelectedText(cmd, sizeof cmd);
	if (!win_callDialog(DLGEXEC,&_fp,_d, NULL)) {
		return 0;
	}

	return EdExecute((long)(opt | (EX_RDNONE<<redir)), 
		(long)0, cmd, dir, errlist);
}

/*--------------------------------------------------------------------------
 * inputPassWord()
 */
static int inputPassWord(LPSTR pszPW, LPSTR pszFilename, BOOL bSave) {
	char szMsg[256];
	DIALPARS _d[] = {
		IDD_STRING1,	20,		0,
		IDD_RO1,		sizeof szMsg,		szMsg,
		0
	};

	pszPW[0] = 0;
	_d[0].dp_data = pszPW;
	LPSTR pszFormat = dlg_getResourceString(bSave ? IDS_ENTER_PASSWORD_TO_ENCRYPT : IDS_ENTER_PASSWORD_TO_DECRYPT);
	sprintf(szMsg, pszFormat, string_abbreviateFileName(pszFilename));
	return DoDialog(DLGCRYPT, dlg_standardDialogProcedure, _d, NULL) != IDCANCEL;
}

/*--------------------------------------------------------------------------
 * dlg_enterPasswordForEncryption()
 */
int dlg_enterPasswordForEncryption(LPSTR password, LPSTR pszFilename, int bSave) {
	char		pw1[128];

	while(1) {
		if (!inputPassWord(password, pszFilename, bSave)) {
			return 0;
		}
		if (!bSave) {
			break;
		}
		inputPassWord(pw1, pszFilename, bSave);
		if (lstrcmp(pw1, password)) {
			error_showErrorById(IDS_MSGDIFFERENTPW);
		} else {
			break;
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdIsDefined()
 */
#define	QUERY_CLIPBOARDAVAIL		0
#define	QUERY_BLKMARKSTART			1
#define	QUERY_BLKMARKEND			2
#define	QUERY_BLKMARKED			3
#define	QUERY_CURRENTFILE			4
#define	QUERY_OPTIONS				5
#define	QUERY_LAYOUTOPTIONS			6
#define	QUERY_WORKMODE				7
#define	QUERY_DISPLAYMODE			8
#define 	QUERY_FILEMODIFIED			9
#define 	QUERY_BLOCKXTNDMODE			10
int EdIsDefined(long what)
{
	if (what == QUERY_CLIPBOARDAVAIL) {
		return clp_getdata();
	}

	if (what == QUERY_OPTIONS) {
		return GetConfiguration()->options;
	}

	if (what == QUERY_LAYOUTOPTIONS) {
		return GetConfiguration()->layoutoptions;
	}

	WINFO* wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}

	switch(what) {

	case QUERY_BLKMARKSTART:
		return wp->blstart ? 1 : 0;

	case QUERY_BLKMARKED:
		if (!wp->blstart) {
			return 0;
		}
		/* drop through */

	case QUERY_BLKMARKEND:
		return wp->blend ? 1 : 0;

	case QUERY_WORKMODE:
		return wp->workmode;

	case QUERY_DISPLAYMODE:
		return wp->dispmode;

	case QUERY_CURRENTFILE:
		return 1;

	case QUERY_FILEMODIFIED:
		return ft_isFileModified(wp->fp) ? 1 : 0;
	
	case QUERY_BLOCKXTNDMODE:
		return wp->bXtndBlock;
	}
	return 0;
}

