/*
 * PublicMacroFunctions.c
 *
 * Project: PKS Edit for Windows
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
#include "grammar.h"
#include "pksedit.h"
#include "dial2.h"
#include "resource.h"
#include "pksrc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "actionbindings.h"
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
#include "editoperations.h"
#include "mainframe.h"
#include "dpisupport.h"
#include "fileselector.h"

/*
 * Answer TRUE if a replacement had been performed before.
 */
extern BOOL find_replacementHadBeenPerformed();

extern int 		align_text(char *pszSearch, RANGE_TYPE scope, char filler, ALIGN_FLAGS flags);
extern int 		EdExecute(long flags, LPSTR cmdline, LPSTR newdir, LPSTR errfile);
extern int 		clp_getdata(void);
extern long long EdCharInsert(WINFO* wp, int c);
extern int 		undo_lastModification(FTABLE *fp);
extern int 		compiler_compileCurrentDocument(void);
extern int		doctypes_addDocumentTypesToListView(HWND hwnd, const void* pSelected);

extern long		_multiplier;

static RANGE_TYPE _scope = RNG_BLOCK;
static DOCUMENT_TYPE* _selectedDocType;

static void dlg_initializeWindowTitle(HWND hDlg, DIALOG_ITEM_DESCRIPTOR* pDescriptor) {
	char* pszString = dlg_getResourceString((WORD)pDescriptor->did_data);
	if (pszString) {
		SetWindowText(hDlg, pszString);
	}
}

static void dlg_initializeLabel(HWND hDlg, DIALOG_ITEM_DESCRIPTOR* pDescriptor) {
	SetDlgItemText(hDlg, pDescriptor->did_controlNumber, pDescriptor->did_data);
}

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
void EdAlert(char *s)
{
	if (s)
		error_displayAlertDialog(s);
}

/*--------------------------------------------------------------------------
 * EdCursorLeft()
 */
long long EdCursorLeft(WINFO* wp, CARET_MOVEMENT_OPTTION mtype)
{
	return caret_moveLeftRight(wp, DIR_LEFT, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
long long EdCursorRight(WINFO* wp, CARET_MOVEMENT_OPTTION mtype)
{
	return caret_moveLeftRight(wp, DIR_RIGHT, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
long long EdCursorUp(WINFO* wp, CARET_MOVEMENT_OPTTION mtype)
{
	return caret_moveUpOrDown(wp, DIR_BACKWARD,mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
long long EdCursorDown(WINFO* wp, CARET_MOVEMENT_OPTTION mtype)
{
	return caret_moveUpOrDown(wp, DIR_FORWARD,mtype);
}

/*--------------------------------------------------------------------------
 * EdParaGotoBegin()
 */
int EdParaGotoBegin(WINFO* wp, int dir)
{
	return caret_advanceParagraphFromCurrentLine(wp, dir,1);
}

/*--------------------------------------------------------------------------
 * EdParaGotoEnd()
 */
int EdParaGotoEnd(WINFO* wp, int dir)
{
	return caret_advanceParagraphFromCurrentLine(wp, dir,0);
}

/*--------------------------------------------------------------------------
 * EdUndo()
 */
int EdUndo(WINFO* wp) {
	if (!wp) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	return undo_lastModification(fp);
}

/*--------------------------------------------------------------------------
 * EdUndo()
 */
int EdRedo(WINFO* wp) {
	if (!wp) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	return undo_redoLastModification(fp);
}

/*--------------------------------------------------------------------------
 * EdBlockCopy()
 */
int EdBlockCopy(WINFO* wp)
{	 return EdBlockCopyOrMove(wp, 0);	}

/*--------------------------------------------------------------------------
 * EdBlockMove()
 */
int EdBlockMove(WINFO* wp)
{	return EdBlockCopyOrMove(wp,1);	}

/*--------------------------------------------------------------------------
 * bl_hideSelectionInCurrentWindow()
 */
int bl_hideSelectionInCurrentWindow(WINFO* wp)	 {
	return bl_hideSelection(wp, 1);
}

/*--------------------------------------------------------------------------
 * DialogCharInput()
 */
int DialogCharInput(int promptfield, unsigned char c)
{ 	static unsigned char _c;
	static ITEMS	_i   =  	{ C_PUSH_CHARACTER_LITERAL, &_c 		};
	static PARAMS	_bgc = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_WINTITLE,		0,			0, .did_initialize = dlg_initializeWindowTitle},
		{IDD_POSITIONTCURS,	0,			0},
		{IDD_RAWCHAR,		sizeof _c,	&_c},
		{0}
	};

	_d->did_data = (void*)(LPARAM)promptfield;
	_c = c;
	if (!win_callDialog(DLGBOXC,&_bgc,_d, NULL))
		return -1L;
	return _c;
}

/*--------------------------------------------------------------------------
 * cpyout()
 */
static void cpyout(char *d,char *s, int len, int nMax)
{
	if (len >= nMax)
		len = nMax-1;

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

	cpyout(sbuf,search,slen,sizeof sbuf);
	cpyout(rbuf,replace,dlen, sizeof rbuf);

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
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_OPT1,		FORM_SHOW,	&opt},
		{IDD_OPT2,		FORM_INIT,	&opt},
		{IDD_OPT3,		FORM_REDRAW,	&opt},
		{0}
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

	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RO1,		sizeof _customerMessage, _customerMessage, .did_initialize = dlg_initializeLabel},
		{IDD_STRING1,	sizeof _architecture, _architecture},
		{IDD_STRING2,	sizeof _versionInfo, _versionInfo},
		{0}
	};
	sprintf(_versionInfo, "%s, %s", _pksVersion, __DATE__);
	return DoDialog(DLGABOUT, dlg_standardDialogProcedure, _d, NULL);
}

/*--------------------------------------------------------------------------
 * dialogGetNumber()
 */
static long dialogGetNumber(int nDialog, int nInitialNumber)
{	static long    num;
	static ITEMS	_i   =  	{ C_PUSH_LONG_LITERAL,  (unsigned char * ) &num };
	static PARAMS	_np  = 	{ 1, P_MAYOPEN, _i	  };
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_LONG1,	sizeof num,		&num},
		{0}
	};

	num = nInitialNumber;
	if (!win_callDialog(nDialog,&_np,_d, NULL))
		return -1L;
	return num;
}

static BOOL dlg_alignmentFlagsExclusive(HWND hwndDialog, int nNotify, LPARAM lParam, 
		DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	dlg_handleRadioButtonGroup(hwndDialog, pDescriptor->did_controlNumber, IDD_3S1, IDD_3S2, 0);
	return FALSE;
}

/*--------------------------------------------------------------------------
 * EdAlignText()
 */
int EdAlignText(void)
{	static int  _alflags;
	static char _alfiller;
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		/* attention!!! FINDS must be placed after find options to
		   make sure correct evalation of options during compile
	      */
		{IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern, &_currentSearchAndReplaceParams.searchPattern},
		{IDD_CHAR,		sizeof _alfiller,	&_alfiller},
		{IDD_RNGE,		RNG_LINE,			&_scope},
		{IDD_REGEXP,	RE_DOREX,			&_currentSearchAndReplaceParams.options},
		{IDD_SHELLJOKER,RE_SHELLWILD,		&_currentSearchAndReplaceParams.options},
		{IDD_IGNORECASE,RE_IGNCASE,			&_currentSearchAndReplaceParams.options},
		{IDD_OPT2,		RE_IGNORE_BINARY,	&_currentSearchAndReplaceParams.options},
		{IDD_3S1,		AL_FIX,				&_alflags, .did_command = dlg_alignmentFlagsExclusive},
		{IDD_3S2,		AL_CPOS,			&_alflags, .did_command = dlg_alignmentFlagsExclusive},
		{IDD_OPT1,		AL_END,				&_alflags},
		{0}
	};
	static ITEMS	_i   =  	{ 
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.searchPattern	 				},
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_alfiller 	},
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_scope	 	},
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_currentSearchAndReplaceParams.options 	},
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_alflags	}
	};
	static PARAMS	_fp = { DIM(_i), P_MAYOPEN, _i	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	bl_getSelectedText(ww_getCurrentEditorWindow(), _currentSearchAndReplaceParams.searchPattern, 1, sizeof _currentSearchAndReplaceParams.searchPattern);
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
	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RNGE,		RNG_LINE ,			&_scope},
		{IDD_RADIO1,	FMT_JUSTIFIED - FMT_LEFT,&alignment},
		{0}
	};
	ITEMS _i = { 
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &alignment     },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_scope	}
	};
	PARAMS _fp = 	{ DIM(_i), P_MAYOPEN, _i   };


	if (!win_callDialog(DLGFORMAT,&_fp,_d, NULL))
		return 0;

	return ft_formatText(ww_getCurrentEditorWindow(), _scope, alignment);
}

/*
 * Try to guess the separator characters used during sort of e.g. CSV fields.
 */
static int sort_guessSeparators(WINFO* wp, char* pszFieldSeparator, int* pFlags) {
	char szText[512];
	int maxLines = 10;
	*pFlags = 0;
	if (!bl_getSelectedText(wp, szText, maxLines, sizeof szText)) {
		return 0;
	}
	int nLine = 0;
	char szTable[128*10];
	memset(szTable, 0, sizeof szTable);
	char* pszRun = szText;
	char* pszCandidates = ",;|#: \t";
	int nQuotes = 0;
	char cPrevious = 0;
	while (*pszRun) {
		char c = *pszRun++;
		if (c == '\n') {
			if (nLine++ > maxLines) {
				break;
			}
			if (nQuotes && (nQuotes % 2) == 0) {
				*pFlags |= SO_CSV_QUOTING;
			}
			cPrevious = 0;
			nQuotes = 0;
			continue;
		} else if (c == '"') {
			nQuotes++;
		}
		if (strchr(pszCandidates, c)) {
			if (cPrevious == '\\') {
				*pFlags |= SO_BACKSLASH_QUOTING;
			} else {
				szTable[nLine * 128 + c]++;
			}
		}
		cPrevious = c;
	}
	char* pszSep = pszFieldSeparator;
	char* pszC = pszCandidates;
	while (*pszC) {
		char c = *pszC++;
		int nCount = szTable[c];
		if (nCount) {
			for (int i = 1; i < nLine; i++) {
				int nNext = szTable[i * 128 + c];
				if (!nNext) {
					nCount = 0;
					break;
				}
				if (nNext != nCount) {
					*pFlags |= SO_SKIPSEPARATORS;
				}
			}
		}
		if (nCount) {
			if (c != '\t' && c != ' ') {
				*pszSep++ = c;
			} else {
				*pFlags |= SO_SKIPSEPARATORS;
				*pszSep++ = ' ';
				*pszSep++ = '\\';
				*pszSep++ = 't';
			}
			break;
		}
	}
	*pszSep = 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * EdSort()
 */
int EdSort(void)
{	static char key[128],fs[32];
	int  flags;
	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RNGE,		RNG_LINE ,		&_scope},
		{IDD_STRING1,	sizeof fs,		&fs},
		{IDD_STRING2,	sizeof key,		&key},
		{IDD_REGEXP,	RE_DOREX,			&_currentSearchAndReplaceParams.options},
		{IDD_SHELLJOKER,RE_SHELLWILD,		&_currentSearchAndReplaceParams.options},
		{IDD_IGNORECASE,RE_IGNCASE,		&_currentSearchAndReplaceParams.options},
		{IDD_FINDS2,	sizeof _currentSearchAndReplaceParams.searchPattern, &_currentSearchAndReplaceParams.searchPattern},
		{IDD_OPT1,		SO_CLUSTERLINES,	&flags},
		{IDD_OPT2,		SO_CSV_QUOTING,&flags},
		{IDD_OPT3,		SO_BACKSLASH_QUOTING,&flags},
		{IDD_OPT4,		SO_SKIPSEPARATORS,		&flags},
		{0}
	};
	ITEMS	_i   = {
		{ C_PUSH_STRING_LITERAL,_currentSearchAndReplaceParams.searchPattern },
		{ C_PUSH_STRING_LITERAL,fs },
		{ C_PUSH_STRING_LITERAL,key },
		{ C_PUSH_INTEGER_LITERAL,	(unsigned char *)&flags },
		{ C_PUSH_INTEGER_LITERAL,	(unsigned char *)&_scope }
	};
	PARAMS	_sp = { DIM(_i), P_MAYOPEN, _i };
	DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_FINDS2,		IDS_TT_REGULAR_EXPRESSION,
		IDD_STRING1,	IDS_TT_SORT_FIELD_SEPARATOR,
		IDD_STRING2,	IDS_TT_SORT_FIELD_OPTIONS,
		0
	};
	WINFO* wp = ww_getCurrentEditorWindow();
	strcpy(fs, ",;");
	if (!*key) {
		strcpy(key, "f1-d");
	}
	sort_guessSeparators(wp, fs, &flags);
	// No default selection criteria
	_currentSearchAndReplaceParams.searchPattern[0] = 0;
	if (!win_callDialog(DLGSORT,&_sp,_d, _tt))
		return 0;

	return ft_sortFile(wp->fp, _scope,fs,key, _currentSearchAndReplaceParams.searchPattern,flags);
}

/*--------------------------------------------------------------------------
 * EdKeycodeInsert()
 * insert ascii text for keycode
 */
int EdKeycodeInsert(void)
{	char 	*visible;
	static KEYCODE 	 k;
	static ITEMS		_i   = { C_PUSH_INTEGER_LITERAL,	(unsigned char *)&k };
	static PARAMS		_sp  = { DIM(_i), P_MAYOPEN, _i };
	static DIALOG_ITEM_DESCRIPTOR 	_d[] = {
		{IDD_KEYCODE,	sizeof k,	&k},
		{0}
	};

	if (!win_callDialog(DLGKEYCODE,&_sp,_d, NULL))
		return 0;

	visible = bindings_keycodeToString(k);
	return (int)edit_insertString(ww_getCurrentEditorWindow(), visible);
}

/*--------------------------------------------------------------------------
 * EdSetMultiplier()
 */
extern long _multiplier;
int EdSetMultiplier(void)
{
	if ((_multiplier = dialogGetNumber(DLGMULTI, 2)) > 0L) 
		return 1;
	_multiplier = 1;
	return 0;
}

/* 
 * Returns the last defined multiplier resetting the current multiplier to be 1.
 */
long long EdGetMultiplier() {
	long long ret = _multiplier;
	_multiplier = 1;
	return ret;
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
	if ((ln = dialogGetNumber(DLGGOTOLINE, wp->caret.ln)) > 0L) {
		if (ln <= 0 ||ln > fp->nlines) {
			error_showErrorById(IDS_LINE_NUMBER_DOES_NOT_EXIST);
		} else {
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
static DIALOG_ITEM_DESCRIPTOR _dconvert[] = {
	{IDD_WINTITLE,	.did_data = (void*)IDS_COMPRESSTABS, .did_initialize = dlg_initializeWindowTitle},
	{IDD_RNGE,		RNG_LINE ,		&_scope},
	{0}
};
int EdReplaceTabs(int expand)
{	static ITEMS	 _i   = { C_PUSH_INTEGER_LITERAL,  (unsigned char*)&_scope	};
	static PARAMS	 _tp  = { 1, P_MAYOPEN, _i };

	_dconvert->did_flagOrSize = expand ? IDS_EXPANDTABS : IDS_COMPRESSTABS;

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
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_STRING1,	128, 		0},
		{0}
	};

	lstrcpy(path, config_getPKSEditTempPath());
	_d[0].did_data = path;

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
	HICON hIcon = (HICON)SendMessageW(hwnd, WM_GETICON, ICON_SMALL, 0L);
	if (hIcon) {
		cust_drawListBoxRowWithIcon(hdc, rcp, hIcon, szBuf);
	}
}


static winlist_drawFileInfo(HDC hdc, RECT* rcp, WINFO* wp, int nItem, int nCtl) {
	dlg_drawFileInfo(hdc, rcp, wp->edwin_handle, nItem, FALSE);
}

/*--------------------------------------------------------------------------
 * infoFillParams()
 */
static void infoFillParams(DIALOG_ITEM_DESCRIPTOR *dp, WINFO *wp) {
	static char szCodePage[32];
	FTABLE* fp = wp->fp;
	dp->did_data = string_getBaseFilename(fp->fname);			dp++;

	string_formatDate(dp->did_data, &fp->ti_created); 	dp++;
	string_formatDate(dp->did_data, &fp->ti_modified); 	dp++;
	string_formatDate(dp->did_data, &fp->ti_saved);  	dp++;
	wsprintf(dp->did_data,"%ld", ft_size(fp)); 	dp++;
	wsprintf(dp->did_data, "%ld", fp->nlines); dp++;
	wsprintf(dp->did_data, "%ld", ft_countWords(fp)); dp++;
	dp->did_data = szCodePage; dp++;
	ft_getCodepageName(fp, szCodePage, sizeof szCodePage);
	dp->did_data = (char*) grammar_name(fp->documentDescriptor->grammar);
}

/*--------------------------------------------------------------------------
 * winlist_command()
 */
static DIALOG_ITEM_DESCRIPTOR infoDialListPars[] = {
	{IDD_RO1,			.did_initialize = dlg_initializeLabel},
	{IDD_RO7,			.did_initialize = dlg_initializeLabel},
	{IDD_RO2,			.did_initialize = dlg_initializeLabel},
	{IDD_RO3,			.did_initialize = dlg_initializeLabel},
	{IDD_RO4,			.did_initialize = dlg_initializeLabel},
	{IDD_RO5,			.did_initialize = dlg_initializeLabel},
	{IDD_RO6,			.did_initialize = dlg_initializeLabel},
	{IDD_RO8,			.did_initialize = dlg_initializeLabel},
	{IDD_RO10,			.did_initialize = dlg_initializeLabel},
	{IDD_WINDOWLIST,	0,				0},
	{IDD_WINTITLE,		.did_initialize = dlg_initializeWindowTitle},
	{0}
};
static void winlist_command(HWND hDlg, int nItem,  int nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		dlg_getListBoxSelection(hDlg, nItem, &pUser);
		if (nNotify == LBN_SELCHANGE) {
			infoFillParams(infoDialListPars, (WINFO *)pUser);
			DoDlgInitPars(hDlg, infoDialListPars, 9);
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
	LIST_HANDLER dlist = {
		0, 
		winlist_lboxfill, 
		dlg_getListBoxSelection, 
		cust_measureListBoxRowWithIcon,
		winlist_drawFileInfo,
		winlist_command 
	};
	char	dmod[40],dsaved[40], dcreated[40], nbytes[20],nlines[20], nwords[20];
	int		nRet;

	dlist.li_param = ww_getCurrentEditorWindow();
    infoDialListPars[1].did_data = dmod;  
	infoDialListPars[2].did_data = dcreated;
	infoDialListPars[3].did_data = dsaved;
    infoDialListPars[4].did_data = nbytes;
    infoDialListPars[5].did_data = nlines;
	infoDialListPars[6].did_data = nwords;
	infoDialListPars[9].did_listhandler = &dlist;
	infoDialListPars[10].did_data = (void*)(LPARAM)nTitleId;

	if (dlist.li_param != NULL) {
		infoFillParams(infoDialListPars, (WINFO*) dlist.li_param);
	}
	nRet = DoDialog(DLGINFOFILE, dlg_standardDialogProcedure,infoDialListPars, NULL);
	if (nRet == IDCANCEL) {
		return 0;
	}

	return ww_selectWindow((WINFO*)dlist.li_param);
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

static DIALOG_ITEM_DESCRIPTOR compareDialListPars[] = {
	{IDD_RO1,		.did_initialize = dlg_initializeLabel},
	{IDD_WINDOWLIST },
	{0}
};
static void dlg_openCompareCommand(HWND hDlg, int nItem, int nNotify, void* pUser) {
	switch (nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		dlg_getComboBoxSelectedText(hDlg, nItem, pUser);
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
	LIST_HANDLER dlist = {
		0,
		winlist_lboxfill,
		dlg_getListBoxSelection,
		cust_measureListBoxRowWithIcon,
		winlist_drawFileInfo,
		dlg_openCompareCommand
	};
	int		nRet;

	_compareFile1 = ww_getCurrentEditorWindow();
	if (!_compareFile1) {
		return 0;
	}
	dlist.li_param = _compareFile1->next;
	FTABLE* fp = _compareFile1->fp;
	compareDialListPars[0].did_data = fp->fname;
	compareDialListPars[1].did_listhandler = &dlist;
	nRet = DoDialog(DLGCOMPAREFILES, compare_dialogProcedure, compareDialListPars, NULL);
	WINFO* wp = (WINFO*) dlist.li_param;
	if (nRet == IDD_BUT3) {
		fp = ft_openBackupfile(_compareFile1->fp);
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
static BOOL doctypes_newType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR *pDialog);
static BOOL doctypes_deleteType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog);
static BOOL doctypes_apply(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog);
static BOOL doctypes_changeType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog);
static void doctypes_fillParameters(DIALOG_ITEM_DESCRIPTOR *dp, void *par);
static DIALOG_ITEM_DESCRIPTOR docTypePars[] = 
{
	{IDD_STRING6,		16,						0},
	{IDD_STRING4,		50,						0},
	{IDD_PATH1,		84,							0},
	{IDD_STRING5,		84,						0},
	{IDD_STRING3,		32,						0},
	{IDD_CALLBACK1,		0,						.did_command = doctypes_changeType},
	{IDD_CALLBACK2,		0,						.did_command = doctypes_deleteType},
	{IDD_CALLBACK3,		0,						.did_command = doctypes_apply},
	{IDD_NOINITCALLBACK,	0,					.did_command = doctypes_newType},
	{0}
};

/*------------------------------------------------------------
 * doctypes_fillListbox()
 */
static void doctypes_fillListbox(HWND hwnd, void* selValue) {
	HWND hwndList = GetDlgItem(hwnd, IDC_DOCTYPES);
	doctypes_addDocumentTypesToListView(hwndList, selValue);
}

/*--------------------------------------------------------------------------
 * doctypes_newType()
 */
#define	  NVDOCTYPEPARS						5
static BOOL doctypes_newType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog)
{
	_selectedDocType = doctypes_createDocumentType(_selectedDocType);
	doctypes_fillListbox(hDlg, _selectedDocType);
	doctypes_fillParameters(docTypePars, _selectedDocType);
	dlg_retrieveParameters(hDlg, docTypePars, NVDOCTYPEPARS);
	return FALSE;
}

/*--------------------------------------------------------------------------
 * doctypes_fillParameters()
 */
static void doctypes_fillParameters(DIALOG_ITEM_DESCRIPTOR *dp, void *par)
{
	char *	pszId;
	char *	pszDescription;
	char *	pszMatch;
	char *	pszFname;
	char*	pszGrammar;

	if (!doctypes_getDocumentTypeDescription(par, &pszId, &pszDescription, &pszMatch, & pszFname, & pszGrammar)) {
		return;
	}

	dp->did_data = pszId;					dp++;
	dp->did_data = pszDescription;			dp++;
	dp->did_data = pszFname;				dp++;
	dp->did_data = pszMatch;				dp++;
	dp->did_data = pszGrammar;
}

/*--------------------------------------------------------------------------
 * doctypes_apply()
 */
static BOOL doctypes_apply(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	EDIT_CONFIGURATION* pConfiguration;
	FTABLE* fp;

	if ((fp = ft_getCurrentDocument()) == 0) {
		return FALSE;
	}
	if (!dlg_applyChanges(hDlg, pDescriptor->did_controlNumber, pDialog->dd_items)) {
		return FALSE;
	}
	char* pDocTypeName;
	if ((pConfiguration = doctypes_getDocumentTypeDescriptor(_selectedDocType, &pDocTypeName)) != NULL) {
		doctypes_assignDocumentTypeDescriptor(fp, NULL, pDocTypeName);
		doctypes_documentTypeChanged(TRUE);
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * doctypes_deleteType()
 */
static BOOL doctypes_deleteType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog)
{
	doctypes_deleteDocumentType(_selectedDocType);
	_selectedDocType = 0;
	doctypes_fillListbox(hDlg, _selectedDocType);
	doctypes_fillParameters(docTypePars, _selectedDocType);
	return FALSE;
}

/*--------------------------------------------------------------------------
 * doctypes_changeType()
 */
static BOOL doctypes_changeType(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	dlg_retrieveParameters(hDlg, docTypePars, NVDOCTYPEPARS);
	doctypes_fillListbox(hDlg, (void*)_selectedDocType);
	return FALSE;
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

	if (!doctypes_getDocumentTypeDescription((void*)plvdi->item.lParam, &pszId, &pszDescription, &pszMatch,
		&pszFname, &pszGrammar)) {
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
	lvc.cx = dpisupport_getSize(100);
	lvc.fmt = LVCFMT_LEFT;

	ListView_InsertColumn(hwnd, 0, &lvc);

	lvc.pszText = "Description";
	lvc.cx = dpisupport_getSize(250);
	lvc.iSubItem = 1;
	ListView_InsertColumn(hwnd, 1, &lvc);

	lvc.pszText = "Name pattern";
	lvc.cx = dpisupport_getSize(100);
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
				_selectedDocType = (void*)pActivate->lParam;
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
		doctypes_fillListbox(hwnd, _selectedDocType);

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

	_selectedDocType = pConfig != NULL ? pConfig->documentType : NULL;
	doctypes_fillParameters(docTypePars, (void*)pConfig);
	if (DoDialog(nDlg, doctype_dialogProcedure, docTypePars, NULL) == IDCANCEL) {
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
{	static ITEMS	 _i   = { C_PUSH_INTEGER_LITERAL,  (unsigned char*)&_scope	};
	static PARAMS	 _tp  = { 1, P_MAYOPEN, _i };

	_dconvert->did_flagOrSize = dir < 0 ? IDS_SHIFTLEFT : IDS_SHIFTRIGHT;
	if (!win_callDialog(DLGCONVERT,&_tp,_dconvert, NULL))
		return 0;
		
	return uc_shiftRange(_scope,dir);
}

/*--------------------------------------------------------------------------
 * dlg_configureEditorModes()
 */
static DIALOG_ITEM_DESCRIPTOR* _paramsPerPage[4];

static DIALOG_ITEM_DESCRIPTOR* _getDialogParsForPage(int pageIndex) {
	return _paramsPerPage[pageIndex];
}

static dlg_updateStatusline(EDIT_CONFIGURATION* linp, char* status) {
	STATUS_LINE_SEGMENT* pSegment = linp->statuslineSegments;
	char* pszText = status;
	char* pszStart = pszText;
	while (*pszText) {
		if (pSegment == NULL) {
			break;
		}
		if (*pszText++ == '!') {
			pszText[-1] = 0;
			if (pSegment->sls_text) {
				free(pSegment->sls_text);
				pSegment->sls_text = _strdup(pszStart);
			}
			pszStart = pszText;
			pSegment = pSegment->sls_next;
		}
	}
}

int dlg_configureEditorModes(void) {
	int scrollmin, mindelta, cursafter, flags;
	char 	status[256];
	COLORREF bgcolor = RGB(0,0,0);
	char 	tabDisplayFillCharacter;
	int  	dispmode;
	int	tabsize;
	int	rmargin;
	int 	nl1, nl2, cr;
	char 	backupExtension[4];
	int	opt;
	int	crypt;
	DIALOG_ITEM_DESCRIPTOR _dFileFormat[] = {
		{IDD_STRING1,	sizeof backupExtension,	backupExtension},
		{IDD_INT1,		sizeof nl1,	&nl1},
		{IDD_INT2,		sizeof nl2,	&nl2},
		{IDD_INT3,		sizeof cr,	&cr},
		{IDD_OPT1,		1,			&opt},
		{IDD_OPT2,		2,			&opt},
		{IDD_OPT3,		4,			&opt},
		{IDD_OPT4,		O_CRYPTED,	&crypt},
		{0}
	};
	DIALOG_ITEM_DESCRIPTOR _dDisplayMode[] = {
		{IDD_STRING1,	sizeof status,		status},
		{IDD_CHAR,		sizeof tabDisplayFillCharacter,	&tabDisplayFillCharacter},
		{IDD_INT1,		sizeof tabsize,	&tabsize},
		{IDD_INT2,		sizeof rmargin,	&rmargin},
		{IDD_OPT1,		SHOW_CONTROL_CHARS,		&dispmode},
		{IDD_OPT4,		SHOW_HEX_DISPLAY,			&dispmode},
		{IDD_OPT5,		SHOW_RULER,			&dispmode},
		{IDD_OPT6,		SHOW_WYSIWYG_DISPLAY,		&dispmode},
		{IDD_OPT7,		SHOW_SYNTAX_HIGHLIGHT,&dispmode},
		{IDD_OPT8,		SHOW_HIDE_VSLIDER,	&dispmode},
		{IDD_OPT9,		SHOW_HIDE_HSLIDER,	&dispmode},
		{IDD_OPT10,		SHOW_LINENUMBERS,	&dispmode},
		{IDD_OPT11,		SHOW_CARET_LINE_HIGHLIGHT, &dispmode},
		{0}
	};
	char createActionName[sizeof(((EDIT_CONFIGURATION*)NULL)->saveActionName)];
	char saveActionName[sizeof (((EDIT_CONFIGURATION*)NULL)->saveActionName)];
	char tabfill;
	int  workmode;
	int fileflag;
	DIALOG_ITEM_DESCRIPTOR _dCursorBehavior[] = {
		{IDD_INT3,		sizeof mindelta,	&mindelta},
		{IDD_INT4,		sizeof scrollmin,	&scrollmin},
		{IDD_OPT1,		SC_THUMBTRACK,		&flags},
		{IDD_OPT2,		SC_CURSORCATCH,		&flags},
		{IDD_OPT3,		SHOW_CARET_PRESERVE_COLUMN,&dispmode},
		{IDD_RADIO1,	CP_POSLOW - CP_POSTOP,&cursafter},
		{0}
	};
	DIALOG_ITEM_DESCRIPTOR _dWorkMode[] = {
		{IDD_STRING2,	sizeof createActionName,	createActionName},
		{IDD_STRING3,	sizeof saveActionName,	saveActionName},
		{IDD_CHAR,		sizeof tabfill,&tabfill},
		{IDD_OPT1,		WM_INSERT,	&workmode},
		{IDD_OPT2,		WM_ABBREV,	&workmode},
		{IDD_OPT3,		WM_AUTOINDENT,	&workmode},
		{IDD_OPT4,		WM_AUTOWRAP,	&workmode},
		{IDD_OPT5,		WM_AUTOFORMAT,	&workmode},
		{IDD_OPT6,		WM_OEMMODE,	&workmode},
		{IDD_OPT7,		F_RDONLY,		&fileflag},
		{IDD_OPT8,		WM_SHOWMATCH,	&workmode},
		{IDD_OPT9,		WM_FIX_CAPITALS,&workmode},
		{IDD_OPT10,		WM_DELETE_MULTIPLE_SPACES, &workmode},
		{IDD_OPT11,		F_WATCH_LOGFILE,&fileflag},
		{0}
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
	status[0] = 0;
	STATUS_LINE_SEGMENT* pSegment = linp->statuslineSegments;
	while (pSegment) {
		if (status[0]) {
			strcat(status, "!");
		}
		if (pSegment->sls_text) {
			if (strlen(status) + strlen(pSegment->sls_text) + 2 > sizeof(status)) {
				break;
			}
			strcat(status, pSegment->sls_text);
		}
		pSegment = pSegment->sls_next;
	}
	tabsize = linp->tabsize;
	rmargin = linp->rmargin;
	dispmode = wp->dispmode;
	tabDisplayFillCharacter = linp->tabDisplayFillCharacter;
	linp = fp->documentDescriptor;
	lstrcpy(createActionName, linp->createActionName);
	lstrcpy(saveActionName, linp->saveActionName);
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
	dlg_updateStatusline(linp, status);
	if (tabsize == 0) {
		tabsize = 8;
	}
	linp->dispmode = dispmode;
	linp->tabDisplayFillCharacter = tabDisplayFillCharacter;
	if ((linp->tabsize = tabsize) != 0) {
		ft_forAllViews(fp, ww_tabsChanged, linp);
	}
	linp->rmargin = rmargin;
	lstrcpy(linp->createActionName, createActionName);
	lstrcpy(linp->saveActionName, saveActionName);
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
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.searchPattern },
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.replaceWith },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &ret },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_scope },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_currentSearchAndReplaceParams.options   },
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RNGE,		RNG_ONCE,			&_scope},
		{IDD_REGEXP,		RE_DOREX,			&_currentSearchAndReplaceParams.options},
		{IDD_SHELLJOKER,	RE_SHELLWILD,		&_currentSearchAndReplaceParams.options},
		{IDD_IGNORECASE,	RE_IGNCASE,		&_currentSearchAndReplaceParams.options},
		{IDD_PRESERVE_CASE,RE_PRESERVE_CASE, &_currentSearchAndReplaceParams.options},
		{IDD_FINDS,		sizeof _currentSearchAndReplaceParams.searchPattern, &_currentSearchAndReplaceParams.searchPattern},
		{IDD_REPLS,		sizeof _currentSearchAndReplaceParams.replaceWith, &_currentSearchAndReplaceParams.replaceWith},
		{IDD_OPT1,		RE_CONFIRM_REPLACEMENT,	&_currentSearchAndReplaceParams.options},
		{IDD_OPT2,		RE_CONSIDER_MARKED_LINES,	 &_currentSearchAndReplaceParams.options},
		{IDD_RECORDRET,	sizeof ret,		&ret},
		{0}
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		{IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION},
		{IDD_FINDS,	IDS_TT_REGULAR_EXPRESSION},
		{IDD_PRESERVE_CASE,	IDS_TT_PRESERVE_CASE},
		{0}
	};
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!wp || win_callDialog(DLGREPLACE,&_fp,_d, _tt) == 0) {
		return 0;
	}

	return edit_replaceText(wp, _currentSearchAndReplaceParams.searchPattern,
		_currentSearchAndReplaceParams.replaceWith, _currentSearchAndReplaceParams.options, _scope,ret);
}

/*--------------------------------------------------------------------------
 * EdFind()
 */
static int _dir = 1;
int EdFind(void)
{	ITEMS	_i   =  	{ 
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.searchPattern },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_dir   },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_scope },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_currentSearchAndReplaceParams.options   }
	};
	PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RADIO1,	1,				&_dir},
		{IDD_REGEXP,	RE_DOREX,			&_currentSearchAndReplaceParams.options},
		{IDD_SHELLJOKER,RE_SHELLWILD,		&_currentSearchAndReplaceParams.options},
		{IDD_IGNORECASE,RE_IGNCASE,		&_currentSearchAndReplaceParams.options},
		{IDD_OPT1,		RE_WRAPSCAN,		&_currentSearchAndReplaceParams.options},
		{IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern,	_currentSearchAndReplaceParams.searchPattern},
		{0}
	};
	DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		IDD_FINDS,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (_dir == -1) {
		_dir = 0;
	}
	WINFO* wp = ww_getCurrentEditorWindow();
	bl_getSelectedText(wp, _currentSearchAndReplaceParams.searchPattern, 1, sizeof _currentSearchAndReplaceParams.searchPattern);
	if (!win_callDialog(DLGFIND, &_fp, _d, _tt)) {
		return 0;
	}

	if (_dir == 0)
		_dir = -1;

	return find_expressionAgainInCurrentFile(wp, _dir);
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

static BOOL dlg_selectPath(HWND hwnDialog, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	char szBuff[128];
	char* pszTitle = dlg_getTitleResource(hwnDialog, pDescriptor->did_controlNumber, szBuff, sizeof szBuff);
	if (fsel_selectFolder(hwnDialog, pszTitle, _fseltarget)) {
		SetDlgItemText(hwnDialog, IDD_PATH1, _fseltarget);
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * EdFindInFileList()
 */
int EdFindInFileList(void)
{	ITEMS	_i   =  	{ 
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.searchPattern },
		{ C_PUSH_STRING_LITERAL, _currentSearchAndReplaceParams.pathlist },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_currentSearchAndReplaceParams.fileScanDepth },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &_currentSearchAndReplaceParams.options }
	};
	PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_PATH1SEL,	.did_command = dlg_selectPath},
		{IDD_REGEXP,	RE_DOREX,					&_currentSearchAndReplaceParams.options},
		{IDD_SHELLJOKER,RE_SHELLWILD,			&_currentSearchAndReplaceParams.options},
		{IDD_IGNORECASE,RE_IGNCASE,				&_currentSearchAndReplaceParams.options},
		{IDD_OPT1,		RE_SEARCH_ONCE,			&_currentSearchAndReplaceParams.options},
		{IDD_OPT2,		RE_IGNORE_BINARY,		&_currentSearchAndReplaceParams.options},
		{IDD_OPT3,		RE_APPEND_TO_SEARCH_RESULTS,&_currentSearchAndReplaceParams.options},
		{IDD_OPT4,		RE_SEARCH_IN_SEARCH_RESULTS,&_currentSearchAndReplaceParams.options},
		{IDD_OPT5,		RE_CONFIRM_REPLACEMENT,	&_currentSearchAndReplaceParams.options},
		{IDD_PRESERVE_CASE,	RE_PRESERVE_CASE,	&_currentSearchAndReplaceParams.options},
		{IDD_INT1,		sizeof _currentSearchAndReplaceParams.fileScanDepth, &_currentSearchAndReplaceParams.fileScanDepth},
		{IDD_FILE_PATTERN, sizeof _currentSearchAndReplaceParams.filenamePattern, &_currentSearchAndReplaceParams.filenamePattern},
		{IDD_FINDS2,	sizeof _currentSearchAndReplaceParams.searchPattern,		&_currentSearchAndReplaceParams.searchPattern},
		{IDD_REPLS,		sizeof _currentSearchAndReplaceParams.replaceWith,&_currentSearchAndReplaceParams.replaceWith},
		{IDD_PATH1,	sizeof _currentSearchAndReplaceParams.pathlist,	&_currentSearchAndReplaceParams.pathlist},
		{0}
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (!_currentSearchAndReplaceParams.filenamePattern[0] && hist_getString(FILE_PATTERNS, 0) == 0) {
		strcpy(_currentSearchAndReplaceParams.filenamePattern, "*.*");
	}
	bl_getSelectedText(ww_getCurrentEditorWindow(), _currentSearchAndReplaceParams.searchPattern, 1, sizeof _currentSearchAndReplaceParams.searchPattern);
	int ret = win_callDialogCB(DLGFINDINFILES, &_fp, _d, _tt, find_inFilesDialogProc);
	if (ret == 0) {
		return 0;
	}
	return find_matchesInFiles(&_currentSearchAndReplaceParams, ret == IDD_BUT3 ? FIF_REPLACE : FIF_SEARCH);
}

/*--------------------------------------------------------------------------
 * EdReplaceAgain()
 */
int EdReplaceAgain(WINFO* wp) {

	if (wp && find_replacementHadBeenPerformed())
		return edit_replaceText(wp, _currentSearchAndReplaceParams.searchPattern, 
			_currentSearchAndReplaceParams.replaceWith, _currentSearchAndReplaceParams.options, RNG_ONCE,REP_REPLACE);
	error_showErrorById(IDS_MSGNOREPLACESTRING);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdFindAgain()
 */
int EdFindAgain(WINFO *wp, int dir)
{	
	return find_expressionAgainInCurrentFile(wp, (dir) ? (_dir = dir) : _dir);
}

/*--------------------------------------------------------------------------
 * macro_getIndexForKeycode()
 * Return the internall index for a given macro keycode and name.
 */
int macro_getIndexForKeycode(KEYCODE *scan,char *name,int oldidx)
{	static DIALOG_ITEM_DESCRIPTOR  _d[] = {
	{IDD_STRING1,	MAC_NAMELEN,		_currentSearchAndReplaceParams.searchPattern},
	{IDD_KEYCODE,	sizeof(KEYCODE),	0},
	{0}
	};

	_d[0].did_data = name;
	_d[1].did_data = scan;
	do {
		if (DoDialog(DLGMACNAME, dlg_standardDialogProcedure,_d, NULL) == IDCANCEL)
			return 0;
	} while (!macro_validateMacroName(name,oldidx, FALSE));
	return 1;
}

static void dlg_initializeCharacter(HWND hDialog, DIALOG_ITEM_DESCRIPTOR* pDescriptor) {
	SendDlgItemMessage(hDialog, pDescriptor->did_controlNumber, WM_CHARCHANGE, 
		*(unsigned char*)pDescriptor->did_data, 0L);
}

static BOOL dlg_applyCharacter(HWND hDialog, DIALOG_ITEM_DESCRIPTOR* pDescriptor) {
	*(char*)pDescriptor->did_data = (char)(short)(long)
		SendDlgItemMessage(hDialog, pDescriptor->did_controlNumber, CS_QUERYCHAR, 0, 0L);
	return TRUE;
}

static BOOL dlg_displayCharacter(HWND hDialog, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	if (nNotify == CSN_CHARSELECT) {
		LONG c = (LONG)LOWORD(lParam);
		union U_ARG_VALUE values[] = {
			{c},{c},{c},{c},{0}
		};
		char szBuff[128];
		mysprintf(szBuff,
			"DEZ: %03j  OKT: %03i  HEX: 0x%02p  BIN: %08b", &(SPRINTF_ARGS){.sa_values = values});
		SetDlgItemText(hDialog, IDD_CSELT1, szBuff);
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * EdCharControlInsert()
 */
long long EdCharControlInsert(void)
{ 	static char c;
	static ITEMS	 _i   = { C_PUSH_CHARACTER_LITERAL,  (unsigned char*)&c	};
	static PARAMS	 _p  = { 1, P_MAYOPEN, _i };
	static DIALOG_ITEM_DESCRIPTOR _d[] = { 
		{IDD_POSITIONTCURS,	0,			0},
		{IDD_CSEL,			sizeof(c),	&c, .did_initialize = dlg_initializeCharacter, 
			.did_apply = dlg_applyCharacter, .did_command = dlg_displayCharacter},
		{0}
	};

   if (!win_callDialog(DLGCONTROLINS,&_p,_d, NULL)) {
		return 0;
	}
	return EdCharInsert(ww_getCurrentEditorWindow(), 0x100|(int)(unsigned char)c);
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
		return compiler_compileCurrentDocument();
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdListBindings()
 */
int EdListBindings(LIST_BINDING_OPTION lWhich)
{
	if (lWhich & LISTB_MACROS) {
		if (decompile_saveMacrosAndDisplay(0, DM_CODE) == 0) {
			return 0;
		}
	}

	if (lWhich & LISTB_KEYS) {
		if (print_keyBindingsAndDisplay() == 0) {
			return 0;
		}
	}


	if (lWhich & LISTB_MICE) {
		if (print_mouseBindingsAndDisplay() == 0) {
			return 0;
		}
	}

	if (lWhich & LISTB_MENUS) {
		if (print_menuBindingsAndDisplay() == 0) {
			return 0;
		}
	}

	if (lWhich & LISTB_MACROC_API) {
		if (print_macrocDocumentationAndDisplay() == 0) {
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
	static char	cmd[512];
	static char	dir[512];
	static char	errlist[64];
	static int  opt;
	static int	redir;
	static ITEMS	_i   =  	{ 
		{ C_PUSH_STRING_LITERAL, 	cmd }, 
		{ C_PUSH_STRING_LITERAL, 	dir }, 
		{ C_PUSH_STRING_LITERAL, 	errlist },
		{ C_PUSH_INTEGER_LITERAL, (unsigned char *) &opt }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_PATH1SEL,	.did_command = dlg_selectPath},
		{IDD_PATH1,	sizeof cmd,			cmd},
		{IDD_STRING1,	sizeof dir,		dir},
		{IDD_STRING2,	sizeof errlist,	errlist},
		{IDD_OPT1,		EX_SYMBOL,		&opt},
		{IDD_OPT2,		EX_WAIT,		&opt},
		{IDD_OPT3,		EX_CD,			&opt},
		{IDD_RADIO1,		4,			&redir},
		{0}
	};

	if (!win_callDialog(DLGEXEC,&_fp,_d, NULL)) {
		return 0;
	}
	int nExecuteOptions = opt;
	switch (redir) {
	case 1: nExecuteOptions |= EX_RDOUT; break;
	case 2: nExecuteOptions |= EX_RDIN; break;
	case 3: nExecuteOptions |= EX_RDCONV; break;
	}
	return EdExecute((long)nExecuteOptions, cmd, dir, errlist);
}

/*--------------------------------------------------------------------------
 * inputPassWord()
 */
static int inputPassWord(LPSTR pszPW, LPSTR pszFilename, BOOL bSave) {
	char szMsg[256];
	DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_STRING1,	20,		0},
		{IDD_RO1,		0,		szMsg, .did_initialize = dlg_initializeLabel},
		{0}
	};

	pszPW[0] = 0;
	_d[0].did_data = pszPW;
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

	case QUERY_HAS_SELECTION_START:
		return wp->blstart ? 1 : 0;

	case QUERY_HAS_SELECTION:
		if (!wp->blstart) {
			return 0;
		}
		/* drop through */

	case QUERY_HAS_SELECTION_END:
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

/*--------------------------------------------------------------------------
 * EdPromptAssign()
 */
char* EdPromptAssign(char* prompt, char* init) {
	static char buf[128];
	static ITEMS	_i = { C_PUSH_STRING_LITERAL,  (unsigned char*)0 };
	static PARAMS	_np = { 1, P_MAYOPEN, _i };
	static DIALOG_ITEM_DESCRIPTOR _d[] = {
		{IDD_RO1,		.did_initialize = dlg_initializeLabel},
		{IDD_STRING1,	128, 		0},
		{0}
	};

	if (!prompt) {
		prompt = " > ";
	}
	_d[0].did_data = prompt;
	_d[1].did_data = buf;
	_i->p.s = buf;
	if (init) {
		lstrcpy(buf, init);
	}
	else {
		buf[0] = 0;
	}

	if (win_callDialog(DLGPROMPT, &_np, _d, NULL) != IDOK) {
		*buf = 0;
	}

	return buf;
}

