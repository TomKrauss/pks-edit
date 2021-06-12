/*
 * PublicMacroFunctions.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: functions accessible in user defined macros.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <direct.h>
#include "trace.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "desktopicons.h"
#include "winterf.h"
#include "winfo.h"

#pragma hdrstop

#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "edfuncs.h"
#include "edhist.h"
#include "xdialog.h"
#include "publicapi.h"
#include "fileutil.h"
#include "stringutil.h"
#include "pathname.h"
#include "edexec.h"
#include "errordialogs.h"
#include "documenttypes.h"
#include "findandreplace.h"
#include "desktopicons.h"
#include "winutil.h"
#include "crossreferencelinks.h"
#include "brackets.h"
#include "markpositions.h"

 /*------------------------------------------------------------
  * EdGetActiveWindow()
  */
extern HWND EdGetActiveWindow(int includeicons);
/*
 * Answer TRUE if a replacement had been performed before.
 */
extern BOOL find_replacementHadBeenPerformed();

extern int ww_closeEditChild(HWND hwndChild);
extern HWND ww_winid2hwnd(int winid);
extern int 		macro_executeMacro(MACROREF *mp);
extern int 		AlignText(char *finds, int scope, char filler, int flags);
extern int 		LbGetText(HWND hwnd, int id, void *szBuff);
extern FTABLE *	ww_winid2fp(int winid);
extern int 		EdExecute(long flags, long unused, 
					LPSTR cmdline, LPSTR newdir, LPSTR errfile);
extern int 		clp_getdata(void);
extern int 		doctypes_assignDocumentTypeDescriptor(FTABLE *fp, DOCUMENT_DESCRIPTOR *linp);
extern int 		EdCharInsert(int c);
extern int 		undo_lastModification(FTABLE *fp);
extern int 		mac_compileMacros(void);
extern int		doctypes_addDocumentTypesToListBox(HWND hwnd, int nItem);

extern long		_multiplier;
extern char		_kunde[30];

static int		_scope = RNG_BLOCK;

/*------------------------------------------------------------
 * EdCloseWindow()
 */
int EdCloseWindow(int winid)
{   	HWND hwndChild;

	if (winid < 0)
		hwndChild = EdGetActiveWindow(0);
	else
		hwndChild = ww_winid2hwnd(winid);

	return ww_closeEditChild(hwndChild);
}

/*------------------------------------------------------------
 * EdExit()
 */
int EdExit(int rc)
{
	PostMessage(hwndFrame, WM_CLOSE, rc, 0L); 
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
	return caret_moveLeftRight(-1, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
int EdCursorRight(int mtype)
{
	return caret_moveLeftRight(1, mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
int EdCursorUp(int mtype)
{
	return caret_moveUpOrDown(-1,mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
int EdCursorDown(int mtype)
{
	return caret_moveUpOrDown(1,mtype);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
int EdChapterGotoBegin(int dir)
{
	return caret_advanceSection(dir,1);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
int EdChapterGotoEnd(int dir)
{
	return caret_advanceSection(dir,0);
}

/*--------------------------------------------------------------------------
 * EdParaGotoBegin()
 */
int EdParaGotoBegin(int dir)
{
	return caret_advanceParagraphFromCurrentLine(dir,1);
}

/*--------------------------------------------------------------------------
 * EdParaGotoEnd()
 */
int EdParaGotoEnd(int dir)
{
	return caret_advanceParagraphFromCurrentLine(dir,0);
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
 * EdBlockHide()
 */
int EdBlockHide(void)	
{	return bl_hideSelection(1);		}

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
 * templateOnSelectionChange()
 */
static char *(*fpGetText)(char *s);
static char *tmplateStringList;
static void templateOnSelectionChange(HWND hDlg, int nItem,  int nNotify, 
	void *p)
{
	char		szText[128];
	char *	szRet;

	if (nNotify != LBN_SELCHANGE) {
		return;
	}
	LbGetText(hDlg, nItem, szText);
	szRet = (*fpGetText)(szText);
	SendDlgItemMessage(hDlg, IDD_RO1, EM_SETREADONLY, (WPARAM) TRUE, 0L);
	SetWindowText(GetDlgItem(hDlg, IDD_RO1), szRet);
	SetWindowText(GetDlgItem(hDlg, IDD_RAWCHAR), szText);
}

/*------------------------------------------------------------
 * templateFillListbox()
 */
static void templateFillListbox(HWND hwnd, int nItem, void* selValue)
{
	char *	pszChars;
	char		szBuf[10];
	char	cCharToSelect;

	pszChars = tmplateStringList;
	szBuf[1] = 0;
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	if (!pszChars) {
		return;
	}
	cCharToSelect = (char)(uintptr_t)selValue;
	if (!cCharToSelect) {
		cCharToSelect = *pszChars;;
	}
	while((szBuf[0] = *pszChars++) != 0) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, 
			(LPARAM)(LPSTR)szBuf);
	}
	szBuf[0] = cCharToSelect;
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, -1, 
		(LPARAM) szBuf);
	templateOnSelectionChange(hwnd, nItem, MAKELONG(0, LBN_SELCHANGE), (void *)0);
}

/*--------------------------------------------------------------------------
 * dlg_displayDialogTemplate()
 */
int dlg_displayDialogTemplate(unsigned char c, 
	char *(*fpTextForTmplate)(char *s), char *s)
{
	static unsigned char text[30] = { 0 };
	static unsigned char _c = 0;
	static ITEMS	_i   = { C_CHAR1PAR, &_c };
	static PARAMS	_bgc = { DIM(_i), P_MAYOPEN, _i };
	static DIALLIST tmplatelist = {
		(long*)text, templateFillListbox, LbGetText, 0, 0, templateOnSelectionChange};
	static DIALPARS _d[] = {
		IDD_POSITIONTCURS,	0,			0,
		IDD_ICONLIST,		0,			&tmplatelist,
		IDD_RAWCHAR,		sizeof _c,	&_c,
		0
	};

	text[0] = c;
	text[1] = 0;
	tmplateStringList = s;
	fpGetText = fpTextForTmplate;
	if (!win_callDialog(DLGSELTMPLATE, &_bgc, _d, NULL)) {
		return 0;
	}
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
 * DlgQueryReplaceProc()
 */
static HWND hwndQueryReplace;
static WPARAM _answer;
static INT_PTR CALLBACK DlgQueryReplaceProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {

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
	return FALSE;
}

/*--------------------------------------------------------------------------
 * dlg_queryReplace()
 * Open a modeless dialog to query for replacing a text.
 */
int dlg_queryReplace(char *search, int slen, char *replace, int dlen)
{	
	char   		sbuf[500],rbuf[500];
	MSG			msg;
	static DLGPROC lpfnProc;
	BOOL			bFirstOpen;

	cpyout(sbuf,search,slen);
	cpyout(rbuf,replace,dlen);

	EnableWindow(hwndFrame,FALSE);
	bFirstOpen = hwndQueryReplace ? FALSE : TRUE;

	win_createModelessDialog(&hwndQueryReplace,MAKEINTRESOURCE(DLGQUERYREPLACE),
					 DlgQueryReplaceProc,&lpfnProc);

	if (!hwndQueryReplace)
		return IDCANCEL;

	if (!bFirstOpen) {
		win_positionWindowRelativeToCaret(hwndQueryReplace);
	}

	render_selectCustomCaret(1);
	SetDlgItemText(hwndQueryReplace,IDD_STRING1,sbuf);
	SetDlgItemText(hwndQueryReplace,IDD_STRING2,rbuf);

	_answer = 0;
	while(_answer == 0 && GetMessage(&msg,0,0,0)) {
		if (!IsDialogMessage(hwndQueryReplace,&msg))
			DispatchMessage(&msg);
	}
	render_selectCustomCaret(0);

	return (int) _answer;
}

/*--------------------------------------------------------------------------
 * dlg_closeQueryReplace()
 */
void dlg_closeQueryReplace(void)
{
	if (hwndQueryReplace) {
		EnableWindow(hwndFrame,TRUE);
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

	if (!DoDialog(DLGRECORDER,DlgStdProc,_d, NULL))
		return 0;
	*o = opt;
	return 1;
}

/*------------------------------------------------------------
 * EdAbout()
 */
int EdAbout(void)
{
#if defined(_WIN64)
	static char _architecture[] = "- 64 Bit Plattform";
#elif
	static char _architecture[] = "- 32 Bit Plattform";
#endif
	static char _versionInfo[] = "Version 1.7.2, 10.6.2021";

	static DIALPARS _d[] = {
		IDD_RO1,		sizeof _kunde,		_kunde,
		IDD_STRING1,	sizeof _architecture, _architecture,
		IDD_STRING2,	sizeof _versionInfo, _versionInfo,
		0
	};

	return DoDialog(DLGABOUT, DlgStdProc, _d, NULL);
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

	if (!win_callDialog(DLGALIGN,&_fp,_d, _tt))
		return 0;
	return AlignText(_currentSearchAndReplaceParams.searchPattern, _scope, _alfiller, _alflags);
}

/*--------------------------------------------------------------------------
 * EdFormatText()
 */
int EdFormatText(void)
{	
	static int _fmtflags;
	static int _fmttype;
	static DIALPARS _d[] = {
		IDD_RNGE,		RNG_LINE ,			&_scope,
		IDD_RADIO1,	FMT_ADJBLOCK-FMT_ADJLEFT,&_fmttype,
		IDD_OPT1,		FMT_WPFORMAT,			&_fmtflags,
		IDD_OPT2,		FMT_INDENT,			&_fmtflags,
		0
	};
	static ITEMS _i = { 
		{ C_INT1PAR, (unsigned char *) &_fmttype     },
		{ C_INT1PAR, (unsigned char *) &_fmtflags	},
		{ C_INT1PAR, (unsigned char *) &_scope	}
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i   };


	if (!win_callDialog(DLGFORMAT,&_fp,_d, NULL))
		return 0;

	return ft_formatText(ft_getCurrentDocument(), _scope,_fmttype,_fmtflags);
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
int EdGotoLine(void)
{	long ln;

	if ((ln = dialogGetNumber(DLGGOTOLINE)) > 0L)
		return caret_placeCursorMakeVisibleAndSaveLocation(ln-1L,0L);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdMarkSet()
 */
static unsigned char _lastmarkc;
int EdMarkSet(void)
{	FTABLE *fp = ft_getCurrentDocument();

	_lastmarkc = DialogCharInput(IDS_MARKSET,_lastmarkc);
	return 
		(mark_set(fp,fp->caret.linePointer,fp->caret.offset,_lastmarkc) != 0);
}

/*--------------------------------------------------------------------------
 * EdMarkGoto()
 */
int EdMarkGoto(void)
{
	long 		x,y;
	FTABLE *		fp;

	fp = ft_getCurrentDocument();
	_lastmarkc = DialogCharInput(IDS_MARKGOTO,_lastmarkc);
	if (mark_goto(fp, _lastmarkc, &x, &y) == 0) {
		error_showErrorById(IDS_MSGMARKUNDEF); 
		return 0;
	} else {
		return caret_placeCursorMakeVisibleAndSaveLocation(x,y);
	}
}

/*--------------------------------------------------------------------------
 * EdFindTag()
 */
int EdFindTag(void)
{	static ITEMS	 _i   = { C_STRING1PAR,  _currentSearchAndReplaceParams.searchPattern 	};
	static PARAMS	 _tp  = { DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = { 
			IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern,	& _currentSearchAndReplaceParams.searchPattern,
			0 
	};

	if (!win_callDialog(DLGFINDTAG,&_tp,_d, NULL))
		return 0;
	return xref_navigateCrossReference(_currentSearchAndReplaceParams.searchPattern);
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
 * dlg_promptString()
 * Prompt for a string to be returned in string2. If the method returns 1, the
 * user has pressed OK.
 */
int dlg_promptString(int strId, char *string, char *string2)
{
	static DIALPARS _d[] = {
		IDD_WINTITLE,	0,			0,
		IDD_RO1,		128,			0,
		IDD_STRING1,	128, 		0,
		0
	};

	_d->dp_size = strId;
	_d[1].dp_data = string;

	if (string2) {
		_d[2].dp_data = string2;
		_d[2].dp_item = IDD_STRING1;
	} else {
		_d[2].dp_item = 0;
	}
	return DoDialog(string2 ? DLGQUERYRENAME : DLGQUERYDELETE,
				 DlgStdProc,_d, NULL);
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

	lstrcpy(path, file_getTempDirectory());
	_d[0].dp_data = path;

	return DoDialog(DLGNEWASPATH, DlgStdProc,_d, NULL) == IDOK;
}

/*------------------------------------------------------------
 * winlist_lboxfill()
 */
static void winlist_lboxfill(HWND hwnd, int nItem, void* selValue)
{
	int		nFile;
	int		nMaxFiles;
	FTABLE *	fp;

	SendDlgItemMessage(hwnd,nItem,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, FALSE, 0L);
	nMaxFiles = ww_getNumberOfOpenWindows();

	for (nFile = 0; nMaxFiles >= 0 && nFile < 10000; nFile++) {
		if ((fp = ww_winid2fp(nFile)) != 0) {
			SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, 
				(LPARAM)fp);
			nMaxFiles--;
		}
	}
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING,-1, (LPARAM)selValue);
}

/*------------------------------------------------------------
 * winlist_lboxdraw()
 */
static void winlist_lboxdrawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	LPSTR	pszName;
	FTABLE *	fp;
	int		x;
	int		y;
	char		cChanged;
	char		szBuf[128];

	if ((fp = (FTABLE*)par) == 0) {
		return;
	}

	x = rcp->left;
	y = rcp->top;

	pszName = string_abbreviateFileName(fp->fname);
	cChanged = fp->flags & F_MODIFIED ? '*' : ' ';
	wsprintf(szBuf,"%-3d: %c %s", nItem+1, cChanged, pszName);
	TextOut(hdc, x, y, szBuf, lstrlen(szBuf));
}

/*--------------------------------------------------------------------------
 * infoFillParams()
 */
static void infoFillParams(DIALPARS *dp, FTABLE *fp)
{
	dp->dp_data = string_getBaseFilename(fp->fname);			dp++;

     string_formatDate(dp->dp_data, &fp->ti_modified); 	dp++;
     string_formatDate(dp->dp_data, &fp->ti_saved);  	dp++;
     wsprintf(dp->dp_data,"%ld", ft_size(fp)); 	dp++;
     wsprintf(dp->dp_data,"%ld", fp->nlines);
}

/*--------------------------------------------------------------------------
 * winlist_command()
 */
static DIALPARS infoDialListPars[] = {
	IDD_RO1,			128,			0,
	IDD_RO2,			128,			0,
	IDD_RO3,			128,			0,
	IDD_RO4,			128,			0,
	IDD_RO5,			128,			0,
	IDD_WINDOWLIST,	0,			0,
	IDD_WINTITLE,		0,			0,
	0
};
static void winlist_command(HWND hDlg, int nItem,  int nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		LbGetText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			infoFillParams(infoDialListPars, *(FTABLE **)pUser);
			DoDlgInitPars(hDlg, infoDialListPars, 5);
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
	static FTABLE *fp;
	static DIALLIST dlist = {
		(long*)&fp, 
		winlist_lboxfill, 
		LbGetText, 
		0, 
		winlist_lboxdrawitem, 
		winlist_command 
	};
	char	dmod[40],dsaved[40],nbytes[20],nlines[20];
	int		nRet;

	fp = ft_getCurrentDocument();

	if (!fp) {
		return 0;
	}
     infoDialListPars[1].dp_data = dmod;  
     infoDialListPars[2].dp_data = dsaved;
     infoDialListPars[3].dp_data = nbytes;
     infoDialListPars[4].dp_data = nlines;
	infoDialListPars[5].dp_data = &dlist;
	infoDialListPars[6].dp_size = nTitleId;

	infoFillParams(infoDialListPars, fp);
	nRet = DoDialog(DLGINFOFILE, DlgStdProc,infoDialListPars, NULL);
	if (nRet == IDCANCEL || fp == 0) {
		return 0;
	}

	return EdSelectWindow(WIPOI(fp)->win_id);
}

/*--------------------------------------------------------------------------
 * EdInfoFiles()
 */
int EdInfoFiles(void)
{
	return showWindowList(IDS_WINDOWLIST);
}

/*--------------------------------------------------------------------------
 * DocTypecommand()
 */
static void docTypeNewType(HWND hDlg);
static void docTypeDeleteType(HWND hDlg);
static void docTypeApply(void);
static void docTypeChangeType(HWND hDlg);
static void docTypeFillParameters(DIALPARS *dp, void *par);
#define	  NVDOCTYPEPARS						5
static DIALPARS docTypePars[] = 
{
	IDD_STRING6,		16,						0,
	IDD_STRING4,		50,						0,
	IDD_PATH1,		84,						0,
	IDD_STRING5,		84,						0,
	IDD_OPT1,			1,						0,
	IDD_WINDOWLIST,	0,						0,
	IDD_CALLBACK1,		0,						docTypeChangeType,
	IDD_CALLBACK2,		0,						docTypeDeleteType,
	IDD_CALLBACK3,		0,						docTypeApply,
	IDD_NOINITCALLBACK,	0,						docTypeNewType,
	0
};

static void doclist_command(HWND hDlg, int nItem, int nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		LbGetText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			docTypeFillParameters(docTypePars, *(FTABLE **)pUser);
			DoDlgInitPars(hDlg, docTypePars, NVDOCTYPEPARS);
		} else {
			PostMessage(hDlg, WM_COMMAND, IDD_CALLBACK3, 0L);
		}
	}
}

/*------------------------------------------------------------
 * docTypeFillListbox()
 */
static void docTypeFillListbox(HWND hwnd, int nItem, void* selValue)
{
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	if (doctypes_addDocumentTypesToListBox(hwnd, nItem)) {
		SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)selValue);
	}
}

/*------------------------------------------------------------
 * ic_measureIconTypeItem()
 */
static void docTypeLboxMeasureitem(MEASUREITEMSTRUCT* mp)
{
	mp->itemHeight = 22;
}


/*------------------------------------------------------------
 * DocTypelboxdraw()
 */
static void docTypeOwnerDrawListboxItem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	char	*	pszId;
	char *	pszDescription;
	SIZE		size;
	SIZE		sizeDescription;
	int		nLen;
	int		nY;
	int		spacing = 3;

	if (!doctypes_getDocumentTypeDescription(par, 
		&pszId, &pszDescription, (char **)0, (char **)0, (int **)0)) {
		return;
	}
	nLen = lstrlen(pszId);
	GetTextExtentPoint(hdc, pszId, nLen, &size);
	GetTextExtentPoint(hdc, pszDescription, lstrlen(pszDescription), &sizeDescription);
	int dTop = nY = (rcp->bottom - rcp->top - size.cy) / 2;
	TextOut(hdc, rcp->left + spacing, rcp->top + dTop, pszId, nLen);
	if (size.cx < 110) {
		HGDIOBJ original = SelectObject(hdc, GetStockObject(DC_PEN));
		nY = (rcp->top + rcp->bottom) / 2;
		SetDCPenColor(hdc, RGB(200,200,200));
		MoveTo(hdc, rcp->left + spacing + size.cx + 5, nY);
		LineTo(hdc, rcp->right-2*spacing-sizeDescription.cx, nY);
		SelectObject(hdc,original);
	}
	TextOut(hdc, rcp->left + rcp->right-spacing - sizeDescription.cx, rcp->top + dTop, pszDescription, lstrlen(pszDescription));
}

/*--------------------------------------------------------------------------
 * docTypeFillParameters()
 */
static void docTypeFillParameters(DIALPARS *dp, void *par)
{
	char *	pszId;
	char *	pszDescription;
	char *	pszMatch;
	char *	pszFname;
	int *	pOwn;

	if (!doctypes_getDocumentTypeDescription(par, &pszId, &pszDescription, &pszMatch, 
		&pszFname, &pOwn)) {
		return;
	}

	dp->dp_data = pszId;					dp++;
	dp->dp_data = pszDescription;				dp++;
	dp->dp_data = pszFname;					dp++;
	dp->dp_data = pszMatch;					dp++;
	dp->dp_data = pOwn;
}

/*--------------------------------------------------------------------------
 * docTypeApply()
 */
void *lastSelectedDocType;
static void docTypeApply(void)
{
	DOCUMENT_DESCRIPTOR *	lp;
	FTABLE *	fp;

	if ((fp = ft_getCurrentDocument()) == 0) {
		return;
	}
	if ((lp = doctypes_getDocumentTypeDescriptor(lastSelectedDocType)) != 0) {
		if (!(fp->flags & F_MODIFIED)) {
			ft_abandonFile(fp, lp);
		} else {
			_free(fp->documentDescriptor);
			doctypes_assignDocumentTypeDescriptor(fp, lp);
			doctypes_documentTypeChanged();
		}
	}
}

/*--------------------------------------------------------------------------
 * docTypeNewType()
 */
static void docTypeNewType(HWND hDlg)
{
	lastSelectedDocType = doctypes_createDocumentType(lastSelectedDocType);
	docTypeFillListbox(hDlg, IDD_WINDOWLIST, lastSelectedDocType);
	docTypeFillParameters(docTypePars, lastSelectedDocType);
	DoDlgRetreivePars(hDlg, docTypePars, NVDOCTYPEPARS);
}

/*--------------------------------------------------------------------------
 * docTypeDeleteType()
 */
static void docTypeDeleteType(HWND hDlg)
{
	doctypes_deleteDocumentType(lastSelectedDocType);
	lastSelectedDocType = 0;
	docTypeFillListbox(hDlg, IDD_WINDOWLIST, lastSelectedDocType);
	docTypeFillParameters(docTypePars, lastSelectedDocType);
}

/*--------------------------------------------------------------------------
 * docTypeChangeType()
 */
static void docTypeChangeType(HWND hDlg)
{
	DoDlgRetreivePars(hDlg, docTypePars, NVDOCTYPEPARS);
	docTypeFillListbox(hDlg, IDD_WINDOWLIST, (void*)lastSelectedDocType);
}

/*--------------------------------------------------------------------------
 * doDocumentTypes()
 */
static int doDocumentTypes(int nDlg)
{
	int		nRet;
	char		linname[128];
	static DIALLIST dlist = {
		(long*)&lastSelectedDocType, 
		docTypeFillListbox, 
		LbGetText, 
		docTypeLboxMeasureitem, 
		docTypeOwnerDrawListboxItem, 
		doclist_command
	};

	linname[0] = 0;

	docTypePars[NVDOCTYPEPARS].dp_data = &dlist;
	lastSelectedDocType = doctypes_getPrivateDocumentType(
		ft_getCurrentDocument() ? ft_getCurrentDocument()->documentDescriptor->modename : "default");

	docTypeFillParameters(docTypePars, (void*)lastSelectedDocType);
	if ((nRet = DoDialog(nDlg, DlgStdProc,docTypePars, NULL)) == IDCANCEL) {
		lastSelectedDocType = 0;
		return 0;
	}

	if (nDlg != DLGDOCTYPES) {
		return TRUE;
	}

	doctypes_saveAllDocumentTypes((void *)0);

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
	int			iRasterCaps;

	hdc = GetDC(hwnd);
	iRasterCaps = GetDeviceCaps(hdc, RASTERCAPS);
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
 * EdDlgDispMode()
 */
int EdDlgDispMode(void)
{
	EDFONT		font;
	static char 	status[64];
	static long 	bgcolor;
	static char 	tabfill;
	static int  	dispmode;
	static int	tabsize;
	static int	rmargin;
	static ITEMS _i = { 
		{ C_STRING1PAR, 	status }, 
		{ C_INT1PAR, 		(unsigned char *) &tabsize },
		{ C_INT1PAR, 		(unsigned char *) &rmargin },
		{ C_CHAR1PAR, 		(unsigned char *) &tabfill },
		{ C_LONG1PAR, 		(unsigned char *) &bgcolor },
		{ C_INT1PAR, 		(unsigned char *) &dispmode }
	};
	static PARAMS	_fp   = { DIM(_i), P_MAYOPEN, _i	};
	static DIALLIST dlist = {
		&bgcolor, color_lboxfill, color_getitem, 0, color_drawitem,
		0, 0, color_showselection };
	static DIALPARS _d[] = {
		IDD_STRING1,	sizeof status,		status,
		IDD_FONTSELECT,0,				0,
		IDD_CHAR,		sizeof tabfill,	&tabfill,
		IDD_FONTSEL2COLOR,	0,			&dlist,
		IDD_INT1,		sizeof tabsize,	&tabsize,
		IDD_INT2,		sizeof rmargin,	&rmargin,
		IDD_OPT1,		SHOWCONTROL,		&dispmode,
		IDD_OPT2,		SHOWATTR,			&dispmode,
		IDD_OPT4,		SHOWHEX,			&dispmode,
		IDD_OPT5,		SHOWRULER,		&dispmode,
		IDD_OPT6,		SHOWOEM,			&dispmode,
		IDD_OPT7,		SHOWFIXEDWI,		&dispmode,
		IDD_OPT8,		SHOWHIDEVSLIDER,	&dispmode,
		IDD_OPT9,		SHOWHIDEHSLIDER,	&dispmode,
		IDD_OPT10,		SHOWLINENUMBERS,	& dispmode,
		IDD_OPT11,		SHOWCARET_LINE_HIGHLIGHT, &dispmode,
		0
	};
	DOCUMENT_DESCRIPTOR *linp;

	if (ft_getCurrentDocument() == 0) {
		return 0;
	}

	linp = ft_getCurrentDocument()->documentDescriptor;
	lstrcpy(status,linp->statusline);
	tabsize = 0;
	rmargin = linp->rmargin;

	dispmode = linp->dispmode;
	tabfill = linp->t1;
	_d[1].dp_data = &font;
	memmove(&font, &linp->fnt, sizeof font);
	bgcolor = linp->fnt_bgcolor;
	if (win_callDialog(DLGDISPMODE,&_fp,_d, NULL) == 0) {
		return 0;
	}
	memmove(&linp->fnt, &font, sizeof font);
	linp->fnt_bgcolor = bgcolor;
	lstrcpy(linp->statusline,status);
	linp->dispmode = dispmode;
	linp->t1 = tabfill;
	if ((linp->tabsize = tabsize) != 0) {
		doctypes_initDocumentTypeDescriptor(linp, linp->tabsize);
		win_sendRedrawToWindow(WIPOI(ft_getCurrentDocument())->ru_handle);
	}
	linp->rmargin = rmargin;
	return doctypes_documentTypeChanged();
}

/*--------------------------------------------------------------------------
 * EdDlgWorkMode()
 */
int EdDlgWorkMode(void)
{
	static char creationMacroName[20];
	static char cm[20];
	static char cclass[64];
	static char tabfill;
	static int  workmode;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, cclass }, 
		{ C_INT1PAR, (unsigned char *) &workmode }
	};
	static PARAMS	_fp   = { DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_STRING1,	sizeof cclass,	cclass,
		IDD_STRING2,	sizeof creationMacroName,	creationMacroName,
		IDD_STRING3,	sizeof cm,	cm,
		IDD_CHAR,		sizeof tabfill,&tabfill,
		IDD_OPT1,		WM_INSERT,	&workmode,
		IDD_OPT2,		WM_ABBREV,	&workmode,
		IDD_OPT3,		WM_AUTOINDENT,	&workmode,
		IDD_OPT4,		WM_AUTOWRAP,	&workmode,
		IDD_OPT5,		WM_AUTOFORMAT,	&workmode,
		IDD_OPT6,		WM_OEMMODE,	&workmode,
		IDD_OPT7,		O_RDONLY,		&workmode,
		IDD_OPT8,		WM_SHOWMATCH,	&workmode,
		IDD_OPT9,		WM_BRINDENT,	&workmode,
		0
	};
	DOCUMENT_DESCRIPTOR *linp;

	if (ft_getCurrentDocument() == 0) {
		return 0;
	}

	linp = ft_getCurrentDocument()->documentDescriptor;
	lstrcpy(cclass, linp->u2lset);
	lstrcpy(creationMacroName, linp->creationMacroName);
	lstrcpy(cm, linp->cm);
	workmode = linp->workmode;
	tabfill = linp->fillc;
	if (win_callDialog(DLGWORKMODE,&_fp,_d, NULL) == 0)
		return 0;
	if (*cclass) {
		lstrcpy(linp->u2lset,cclass);
	}
	lstrcpy(linp->creationMacroName, creationMacroName);
	lstrcpy(linp->cm, cm);
	linp->workmode = workmode;
	linp->fillc = tabfill;
	return doctypes_documentTypeChanged();
}

/*--------------------------------------------------------------------------
 * EdDlgCursTabs()
 */
int EdDlgCursTabs(void)
{	static int scrollmin,mindelta,cursafter,flags;
	static ITEMS	_i   =  	{ 
		{ C_INT1PAR, (unsigned char *) &scrollmin },
		{ C_INT1PAR, (unsigned char *) &mindelta },
		{ C_INT1PAR, (unsigned char *) &cursafter },
		{ C_INT1PAR, (unsigned char *) &flags }
	};
	static PARAMS	_fp   = { DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_INT3,		sizeof mindelta,	&mindelta,
		IDD_INT4,		sizeof scrollmin,	&scrollmin,
		IDD_OPT1,		SC_THUMBTRACK,		&flags,
		IDD_OPT2,		SC_CURSORCATCH,	&flags,
		IDD_RADIO1,	CP_POSLOW-CP_POSTOP,&cursafter,
		0
	};
	DOCUMENT_DESCRIPTOR *linp;

	if (ft_getCurrentDocument() == 0)
		return 0;

	linp = ft_getCurrentDocument()->documentDescriptor;
	flags = linp->scrollflags;
	cursafter = linp->cursaftersearch;
	scrollmin = linp->vscroll+1;
	mindelta = linp->scroll_dy;
	if (win_callDialog(DLGCURSTABS,&_fp,_d, NULL) == 0)
		return 0;
	linp->scrollflags = flags;
	linp->vscroll = scrollmin-1;
	linp->scroll_dy = mindelta;
	linp->cursaftersearch = cursafter;
	return doctypes_documentTypeChanged();
}

/*--------------------------------------------------------------------------
 * sel_ruler()
 */
static DOCUMENT_DESCRIPTOR *_linp;
static void sel_ruler(HWND hDlg, WORD idCtrl)
{
	EndDialog(hDlg,idCtrl);
	doctypes_readWriteDocumentDescriptor(2,_linp);
}

/*--------------------------------------------------------------------------
 * DlgModeVals()
 */
int DlgModeVals(DOCUMENT_DESCRIPTOR *linp)
{	static int 	nl1,nl2,cr;
	static char 	bak[4];
	static int	opt;
	static int	crypt;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR,(unsigned char *) bak },
		{ C_INT1PAR, 	(unsigned char *) &nl1 },
		{ C_INT1PAR, 	(unsigned char *) &nl2 },
		{ C_INT1PAR, 	(unsigned char *) &cr }
	};
	static PARAMS	_fp   = { DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_STRING1,	sizeof bak,	bak,
		IDD_INT1,		sizeof nl1,	&nl1,
		IDD_INT2,		sizeof nl2,	&nl2,
		IDD_INT3,		sizeof cr,	&cr,
		IDD_OPT1,		1,			&opt,
		IDD_OPT2,		2,			&opt,
		IDD_OPT3,		4,			&opt,
		IDD_OPT4,		O_CRYPTED,	&crypt,
		0
	};
	int	ret;

	_linp = linp;
	lstrcpy(bak,linp->bak);
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

	if ((ret = win_callDialog(DLGFILEFORMAT,&_fp,_d, NULL)) == 0 ||
		ret == IDCANCEL) {
		return 0;
	}

	linp->workmode = (linp->workmode & (~O_CRYPTED)) | crypt;
	lstrcpy(linp->bak,bak);
	linp->nl = (opt & 1) ? nl1 : -1;
	linp->nl2 = (opt & 2) ? nl2 : -1;
	linp->cr = (opt & 4) ? cr : -1;

	return 1;
}

int EdDlgModeVals(void)
{
	if (ft_getCurrentDocument() == 0)
		return 0;
	return DlgModeVals(ft_getCurrentDocument()->documentDescriptor);
}

/*--------------------------------------------------------------------------
 * EdReplace()
 */
int EdReplace(void)
{	static int ret;
	static int flags;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern },
		{ C_STRING1PAR, _currentSearchAndReplaceParams.replaceWith },
		{ C_INT1PAR, (unsigned char *) &ret },
		{ C_INT1PAR, (unsigned char *) &_scope },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options   },
		{ C_INT1PAR, (unsigned char *) &flags },
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
		IDD_OPT1,		OREP_INQ,			&flags,
		IDD_OPT2,		OREP_MARKED,		&flags,
		IDD_RECORDRET,	sizeof ret,		&ret,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		IDD_PRESERVE_CASE,	IDS_TT_PRESERVE_CASE,
		0
	};

	if (win_callDialog(DLGREPLACE,&_fp,_d, _tt) == 0) {
		return 0;
	}

	return EdReplaceText(_scope,ret,flags);
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
		IDD_FINDS,	sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (_dir == -1)
		_dir = 0;

	if (!win_callDialog(DLGFIND,&_fp,_d,_tt))
		return 0;

	if (_dir == 0)
		_dir = -1;

	return find_expressionAgainInCurrentFile(_dir);
}

/*--------------------------------------------------------------------------
 * EdFindInFileList()
 */
int EdFindInFileList(void)
{	static char pathlist[512];
	static char filenamePattern[50];
	static int once = 1,depth = -1;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _currentSearchAndReplaceParams.searchPattern },
		{ C_STRING1PAR, pathlist }, 
		{ C_INT1PAR, (unsigned char *) &once   },
		{ C_INT1PAR, (unsigned char *) &depth },
		{ C_INT1PAR, (unsigned char *) &_currentSearchAndReplaceParams.options }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_REGEXP,	RE_DOREX,			& _currentSearchAndReplaceParams.options,
		IDD_SHELLJOKER,RE_SHELLWILD,		& _currentSearchAndReplaceParams.options,
		IDD_IGNORECASE,RE_IGNCASE,		& _currentSearchAndReplaceParams.options,
		IDD_OPT1,		1,				&once,
		IDD_INT1,		sizeof depth,	&depth,
		IDD_FILE_PATTERN, sizeof filenamePattern, &filenamePattern, 
		IDD_FINDS2,	sizeof _currentSearchAndReplaceParams.searchPattern,		& _currentSearchAndReplaceParams.searchPattern,
		IDD_PATH1,	sizeof pathlist,	&pathlist,
		0
	};
	static DLG_ITEM_TOOLTIP_MAPPING _tt[] = {
		IDD_REGEXP,	IDS_TT_REGULAR_EXPRESSION,
		0
	};

	if (!pathlist[0] || !filenamePattern[0]) {
		_getcwd(pathlist, sizeof pathlist);
		strcpy(filenamePattern, "*.*");
	}
	if (!win_callDialog(DLGRETREIVE,&_fp,_d, _tt)) {
		return 0;
	}
	return find_matchesInFiles(pathlist,filenamePattern, _currentSearchAndReplaceParams.searchPattern,depth < 0 ? 999 : depth,once);
}

/*--------------------------------------------------------------------------
 * EdReplaceAgain()
 */
int EdReplaceAgain(void)
{	extern int _rflg;

	if (find_replacementHadBeenPerformed())
		return EdReplaceText(RNG_ONCE,REP_REPLACE,0); 
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
		if (DoDialog(DLGMACNAME, DlgStdProc,_d, NULL) == IDCANCEL)
			return 0;
	} while (!macro_validateMacroName(name,oldidx));
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
		IDD_RADIO1,	4,				&redir,
		0
	};

	if (!win_callDialog(DLGEXEC,&_fp,_d, NULL)) {
		return 0;
	}

	return EdExecute((long)(opt | (EX_RDNONE<<redir)), 
		(long)0, cmd, dir, errlist);
}

/*--------------------------------------------------------------------------
 * EdConfigureIcons()
 */
static int add_icon(HWND hDlg);
static int del_icon(HWND hDlg);
static int mod_icon(void);
static char _title[32],_pars[64];
static ICONCLASS* _ictype;
static DIALLIST icondlist = {
	(LONG*)&_ictype, ic_fillIconTypeList, LbGetText, 
	ic_measureIconTypeItem, ic_ownerDrawIconType, ic_onIconTypeSelection, 0  };
static DIALPARS iconDialListPars[] = {
	IDD_STRING1,	sizeof _title,		_title,
	IDD_PATH1,		sizeof _pars,		_pars,
	IDD_ICONLIST,	0,				&icondlist,
	IDD_CALLBACK1,	0,				add_icon,
	IDD_CALLBACK2,	0,				del_icon,
	IDD_CALLBACK3,	0,				mod_icon,
	0
};
static int add_icon(HWND hDlg)
{
	char 	*p;
	HWND		hwnd;

	if ((p = string_allocate(_pars)) != 0) {
		hwnd = ic_addIcon((void*)_ictype,_title,p,CW_USEDEFAULT,CW_USEDEFAULT);
		SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)hwnd, (LPARAM)NULL);
		win_sendRedrawToWindow(hwnd);
		ic_enableConfigurationDialogIcons(hDlg, (void *)_ictype);
	}
	return 1;
}

static int del_icon(HWND hDlg)
{
	HWND hwnd;

	if ((hwnd = ic_active(_title,_pars,&_ictype)) != 0) {
		ic_closeIconWindow(hwnd);
	}
	DoDlgInitPars(hDlg, iconDialListPars, 3);
	return 1;
}

static int mod_icon(void)
{
	void *p;

	if ((p = string_allocate(_pars)) != 0) {
		ic_changeIcon(_title,p,_ictype);
	}
	return 1;
}

int EdConfigureIcons(void)
{
	static ITEMS	_i   =  	{ 
		{ C_LONG1PAR, (unsigned char *) &_ictype }
	};
	static PARAMS	_fp   = { DIM(_i), P_MAYOPEN, _i	};

	_ictype = 0;
	ic_active(_title, _pars, &_ictype);
	return win_callDialog(DLGCONFICONS, &_fp, iconDialListPars, NULL);
}

/*--------------------------------------------------------------------------
 * inputPassWord()
 */
static void inputPassWord(LPSTR pszPW)
{
	static DIALPARS _d[] = {
		IDD_STRING1,	20,		0,
		0
	};

	pszPW[0] = 0;
	_d[0].dp_data = pszPW;
	DoDialog(DLGCRYPT, DlgStdProc, _d, NULL);
}

/*--------------------------------------------------------------------------
 * CryptDialog()
 */
int CryptDialog(LPSTR password, int twice)
{
	char		pw1[128];

	while(1) {
		inputPassWord(password);
		if (twice == 0) {
			break;
		}
		inputPassWord(pw1);
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

	if (!ft_getCurrentDocument()) {
		return 0;
	}

	switch(what) {

	case QUERY_BLKMARKSTART:
		return ft_getCurrentDocument()->blstart ? 1 : 0;

	case QUERY_BLKMARKED:
		if (!ft_getCurrentDocument()->blstart) {
			return 0;
		}
		/* drop through */

	case QUERY_BLKMARKEND:
		return ft_getCurrentDocument()->blend ? 1 : 0;

	case QUERY_WORKMODE:
		return ft_getCurrentDocument()->documentDescriptor->workmode;

	case QUERY_DISPLAYMODE:
		return ft_getCurrentDocument()->documentDescriptor->dispmode;

	case QUERY_CURRENTFILE:
		return 1;

	case QUERY_FILEMODIFIED:
		return (ft_getCurrentDocument()->flags & F_MODIFIED) ? 1 : 0;
	
	case QUERY_BLOCKXTNDMODE:
		return WIPOI(ft_getCurrentDocument())->bXtndBlock;
	}
	return 0;
}

