/*
 * PublicMacroFunctions.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: functions accessible in user defined macros.
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"
#include "editorconfiguration.h"

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
#include "stringutil.h"
#include "pathname.h"
#include "edexec.h"
#include "errordialogs.h"

 /*------------------------------------------------------------
  * EdGetActiveWindow()
  */
extern HWND EdGetActiveWindow(int includeicons);
extern HWND ww_winid2hwnd(int winid);
extern HWND ic_add(void* icp, LPSTR szTitle, LPSTR szParams, int x, int y);
extern HWND ic_active(LPSTR szTitle, LPSTR szParams, void** icClass);
extern int 		AbandonFile(FTABLE *fp, DOCUMENT_DESCRIPTOR *linp);
extern int 		mac_runcmd(MACROREF *mp);
extern int 		AlignText(char *finds, int scope, char filler, int flags);
extern long 	ft_size(FTABLE *fp);
extern char *	AbbrevName(char *fn);
extern BOOL CALLBACK DlgMacEditProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);
extern void 	ic_lboxfill(HWND hwnd, int nItem, long selValue);
extern void 	ic_lboxmeasureitem(MEASUREITEMSTRUCT *mp);
extern void		ic_lboxdrawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl);
extern void		ic_lboxselchange(HWND hDlg, WORD nItem, LONG lParam, void* p);
extern int 		LbGetText(HWND hwnd, WORD id, char *szBuff);
extern FTABLE *	ww_winid2fp(int winid);
extern int 		ww_nwi(void);
extern int 		PrintMacs(char *macroname);
extern int 		PrintMice(void);
extern int 		PrintKeys(void);
extern int 		PrintMenus(void);
extern void* CreateDocumentType(void* llp);
extern void 	fkey_visibilitychanged(void);
extern int 		caret_moveLeftRight(int dir, int mtype);
extern int 		EdExecute(long flags, long unused, 
					LPSTR cmdline, LPSTR newdir, LPSTR errfile);
extern int 		clp_getdata(void);
extern int 		AssignDocumentTypeDescriptor(FTABLE *fp, DOCUMENT_DESCRIPTOR *linp);
extern void *		GetPrivateDocumentType(char *);
extern char *		TmpDir(void);
extern void 		mac_switchtodefaulttables(void);
extern int 		linchange(void);
extern void 		SendRedraw(HWND hwnd);
extern int 		cursupdown(int dir, int mtype);
extern int 		cursabsatz(int dir,int start);
extern int 		curspgrph(int dir,int start);
extern int 		EdCharInsert(int c);
extern int 		undo_lastModification(FTABLE *fp);
extern int 		macs_compile(void);

extern char		_finds[500];
extern char		_repls[500];
extern long		_multiplier;
extern char		_kunde[30];
extern int 		_findopt;

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

	return CloseEditChild(hwndChild);
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
		alert(s);
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
	return cursupdown(-1,mtype);
}

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
int EdCursorDown(int mtype)
{
	return cursupdown(1,mtype);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
int EdChapterGotoBegin(int dir)
{
	return cursabsatz(dir,1);
}

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
int EdChapterGotoEnd(int dir)
{
	return cursabsatz(dir,0);
}

/*--------------------------------------------------------------------------
 * EdParaGotoBegin()
 */
int EdParaGotoBegin(int dir)
{
	return curspgrph(dir,1);
}

/*--------------------------------------------------------------------------
 * EdParaGotoEnd()
 */
int EdParaGotoEnd(int dir)
{
	return curspgrph(dir,0);
}

/*--------------------------------------------------------------------------
 * EdUndo()
 */
int EdUndo(void)
{
	if (!ft_CurrentDocument()) {
		return 0;
	}
	return undo_lastModification(ft_CurrentDocument());
}

/*--------------------------------------------------------------------------
 * EdBlockCopy()
 */
int EdBlockCopy(void)
{	 return EdBlockCopy_mv(0);	}

/*--------------------------------------------------------------------------
 * EdBlockMove()
 */
int EdBlockMove(void)
{	return EdBlockCopy_mv(1);	}

/*--------------------------------------------------------------------------
 * EdBlockHide()
 */
int EdBlockHide(void)	
{	return Pastehide(1);		}

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
	if (!CallDialog(DLGBOXC,&_bgc,_d))
		return -1L;
	return _c;
}

/*--------------------------------------------------------------------------
 * TmplateSelchange()
 */
static char *(*fpGetText)(char *s);
static char *tmplateStringList;
static void TmplateSelchange(HWND hDlg, WORD nItem,  int nNotify, 
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
 * TmplateListboxFill()
 */
static void TmplateListboxFill(HWND hwnd, int nItem, long selValue)
{
	char *	pszChars;
	char		szBuf[10];

	pszChars = tmplateStringList;
	szBuf[1] = 0;
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	if (!pszChars) {
		return;
	}
	if (!selValue) {
		selValue = *pszChars;;
	}
	while((szBuf[0] = *pszChars++) != 0) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, 
			(LPARAM)(LPSTR)szBuf);
	}
	szBuf[0] = selValue;
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, -1, 
		(LPARAM) szBuf);
	TmplateSelchange(hwnd, nItem, MAKELONG(0, LBN_SELCHANGE), (void *)0);
}

/*--------------------------------------------------------------------------
 * DialogTemplate()
 */
int DialogTemplate(unsigned char c, 
	char *(*fpTextForTmplate)(char *s), char *s)
{
	static unsigned char text[30];
	static unsigned char _c;
	static ITEMS	_i   = { C_CHAR1PAR, &_c };
	static PARAMS	_bgc = { DIM(_i), P_MAYOPEN, _i };
	static DIALLIST tmplatelist = {
		text, TmplateListboxFill, LbGetText, 0, 0,
		TmplateSelchange};
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
	if (!CallDialog(DLGSELTMPLATE, &_bgc, _d)) {
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
static WORD _answer;
BOOL CALLBACK DlgQueryReplaceProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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
 * QueryReplace()
 */
int QueryReplace(char *search, int slen, char *replace, int dlen)
{	
	char   		sbuf[500],rbuf[500];
	MSG			msg;
	static FARPROC lpfnProc;
	BOOL			bFirstOpen;

	cpyout(sbuf,search,slen);
	cpyout(rbuf,replace,dlen);

	EnableWindow(hwndFrame,FALSE);
	bFirstOpen = hwndQueryReplace ? FALSE : TRUE;

	CreateModelessDialog(&hwndQueryReplace,MAKEINTRESOURCE(DLGQUERYREPLACE),
					 DlgQueryReplaceProc,&lpfnProc);

	if (!hwndQueryReplace)
		return IDCANCEL;

	if (!bFirstOpen) {
		form_textcursor(hwndQueryReplace);
	}

	OwnTextCursor(1);
	SetDlgItemText(hwndQueryReplace,IDD_STRING1,sbuf);
	SetDlgItemText(hwndQueryReplace,IDD_STRING2,rbuf);

	_answer = 0;
	while(_answer == 0 && GetMessage(&msg,0,0,0)) {
		if (!IsDialogMessage(hwndQueryReplace,&msg))
			DispatchMessage(&msg);
	}
	OwnTextCursor(0);

	return _answer;
}

/*--------------------------------------------------------------------------
 * CloseQueryReplace()
 */
void CloseQueryReplace(void)
{
	if (hwndQueryReplace) {
		EnableWindow(hwndFrame,TRUE);
		DestroyModelessDialog(&hwndQueryReplace);
	}
}

/*--------------------------------------------------------------------------
 * RecordOptions()
 */
int RecordOptions(int *o)
{	static int opt;
	static DIALPARS _d[] = {
		IDD_OPT1,		FORM_SHOW,	&opt,
		IDD_OPT2,		FORM_INIT,	&opt,
		IDD_OPT3,		FORM_REDRAW,	&opt,
		0
	};

	if (!DoDialog(DLGRECORDER,(FARPROC)DlgStdProc,_d))
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
	static char _architecture[] = "- 64 Bit Platform Version";
#elif
	static char _architecture[] = "- 32 Bit Platform Version";
#endif
	static char _versionInfo[] = "Version 1.4, 14.5.2021";

	static DIALPARS _d[] = {
		IDD_RO1,		sizeof _kunde,		_kunde,
		IDD_STRING1,	sizeof _architecture, _architecture,
		IDD_STRING2,	sizeof _versionInfo, _versionInfo,
		0
	};

	return DoDialog(DLGABOUT,(FARPROC)DlgStdProc,_d);
}

/*--------------------------------------------------------------------------
 * DialogGetNum()
 */
long DialogGetNum(int nDialog)
{	static long    num;
	static ITEMS	_i   =  	{ C_LONG1PAR,  (unsigned char * ) &num };
	static PARAMS	_np  = 	{ 1, P_MAYOPEN, _i	  };
	static DIALPARS _d[] = {
		IDD_LONG1,	sizeof num,		&num,
		0
	};

	if (!CallDialog(nDialog,&_np,_d))
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
		IDD_FINDS,	sizeof _finds,		&_finds,
		IDD_CHAR,		sizeof _alfiller,	&_alfiller,
		IDD_RNGE,		RNG_LINE,			&_scope,
		IDD_REGEXP,	RE_DOREX,			&_findopt,
		IDD_SHELLJOKER,RE_SHELLWILD,		&_findopt,
		IDD_IGNORECASE,RE_IGNCASE,		&_findopt,
		IDD_3S1,		AL_FIX,			&_alflags,
		IDD_3S2,		AL_CPOS,			&_alflags,
		IDD_OPT1,		AL_END,			&_alflags,
		0
	};
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _finds	 				}, 
		{ C_INT1PAR, (unsigned char *) &_alfiller 	},
		{ C_INT1PAR, (unsigned char *) &_scope	 	},
		{ C_INT1PAR, (unsigned char *) &_findopt 	},
		{ C_INT1PAR, (unsigned char *) &_alflags	}
	};
	static PARAMS	_fp = { DIM(_i), P_MAYOPEN, _i	};

	if (!CallDialog(DLGALIGN,&_fp,_d))
		return 0;
	return AlignText(_finds, _scope, _alfiller, _alflags);
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


	if (!CallDialog(DLGFORMAT,&_fp,_d))
		return 0;

	return FormatText(_scope,_fmttype,_fmtflags);
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
		IDD_REGEXP,	RE_DOREX,			&_findopt,
		IDD_SHELLJOKER,RE_SHELLWILD,		&_findopt,
		IDD_IGNORECASE,RE_IGNCASE,		&_findopt,
		IDD_FINDS2,	sizeof _finds,		&_finds,
		IDD_OPT1,		SO_REVERSE,		&flags,
		IDD_OPT2,		SO_CLUSTERLINES,	&flags,
		IDD_OPT3,		SO_SKIPSEPS,		&flags,
		0
	};
	static ITEMS	_i   = {
		{ C_STRING1PAR,_finds },
		{ C_STRING1PAR,fs },
		{ C_STRING1PAR,key },
		{ C_INT1PAR,	(unsigned char *)&flags },
		{ C_INT1PAR,	(unsigned char *)&_scope }
	};
	static PARAMS	_sp = { DIM(_i), P_MAYOPEN, _i };

	if (!CallDialog(DLGSORT,&_sp,_d))
		return 0;

	return Sort(_scope,fs,key,_finds,flags);
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

	if (!CallDialog(DLGKEYCODE,&_sp,_d))
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
	if ((_multiplier = DialogGetNum(DLGMULTI)) > 0L) 
		return 1;
	_multiplier = 1;
	return 0;
}

/*--------------------------------------------------------------------------
 * EdGotoLine()
 */
int EdGotoLine(void)
{	long ln;

	if ((ln = DialogGetNum(DLGGOTOLINE)) > 0L)
		return centernewpos(ln-1L,0L);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdMarkSet()
 */
static unsigned char _lastmarkc;
int EdMarkSet(void)
{	FTABLE *fp = ft_CurrentDocument();

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

	fp = ft_CurrentDocument();
	_lastmarkc = DialogCharInput(IDS_MARKGOTO,_lastmarkc);
	if (mark_goto(fp, _lastmarkc, &x, &y) == 0) {
		ed_error(IDS_MSGMARKUNDEF); 
		return 0;
	} else {
		return centernewpos(x,y);
	}
}

/*--------------------------------------------------------------------------
 * EdFindTag()
 */
int EdFindTag(void)
{	static ITEMS	 _i   = { C_STRING1PAR,  _finds 	};
	static PARAMS	 _tp  = { DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = { 
			IDD_FINDS,	sizeof _finds,	&_finds, 
			0 
	};

	if (!CallDialog(DLGFINDTAG,&_tp,_d))
		return 0;
	return showtag(_finds);
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

	if (!CallDialog(DLGCONVERT,&_tp,_dconvert))
		return 0;

	return ReplaceTabs(_scope,expand);
}

/*--------------------------------------------------------------------------
 * PromptString()
 */
int PromptString(int strId, char *string, char *string2)
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
				 (FARPROC)DlgStdProc,_d);
}

/*--------------------------------------------------------------------------
 * PromptAsPath()
 */
int PromptAsPath(char *path)
{
	static DIALPARS _d[] = {
		IDD_STRING1,	128, 		0,
		0
	};

	lstrcpy(path, TmpDir());
	_d[0].dp_data = path;

	return DoDialog(DLGNEWASPATH, (FARPROC)DlgStdProc,_d) == IDOK;
}

/*------------------------------------------------------------
 * winlist_lboxfill()
 */
void winlist_lboxfill(HWND hwnd, int nItem, long selValue)
{
	int		nFile;
	int		nMaxFiles;
	FTABLE *	fp;

	SendDlgItemMessage(hwnd,nItem,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, FALSE, 0L);
	nMaxFiles = ww_nwi();

	for (nFile = 0; nMaxFiles >= 0 && nFile < 10000; nFile++) {
		if ((fp = ww_winid2fp(nFile)) != 0) {
			SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, 
				(LPARAM)fp);
			nMaxFiles--;
		}
	}
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING,-1, selValue);
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

	pszName = AbbrevName(fp->fname);
	cChanged = fp->flags & F_MODIFIED ? '*' : ' ';
	wsprintf(szBuf,"%-3d: %c %s", nItem+1, cChanged, pszName);
	TextOut(hdc, x, y, szBuf, lstrlen(szBuf));
}

/*--------------------------------------------------------------------------
 * InfoFillParams()
 */
static void InfoFillParams(DIALPARS *dp, FTABLE *fp)
{
	dp->dp_data = basename(fp->fname);			dp++;

     print_date(dp->dp_data, &fp->ti_modified); 	dp++;
     print_date(dp->dp_data, &fp->ti_saved);  	dp++;
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
static void winlist_command(HWND hDlg, WORD nItem,  int nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		LbGetText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			InfoFillParams(infoDialListPars, *(FTABLE **)pUser);
			DoDlgInitPars(hDlg, infoDialListPars, 5);
		} else {
			PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		}
	}
}

/*--------------------------------------------------------------------------
 * ShowWindowList()
 */
static int ShowWindowList(int nTitleId)
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

	fp = ft_CurrentDocument();

	if (!fp) {
		return 0;
	}
     infoDialListPars[1].dp_data = dmod;  
     infoDialListPars[2].dp_data = dsaved;
     infoDialListPars[3].dp_data = nbytes;
     infoDialListPars[4].dp_data = nlines;
	infoDialListPars[5].dp_data = &dlist;
	infoDialListPars[6].dp_size = nTitleId;

	InfoFillParams(infoDialListPars, fp);
	nRet = DoDialog(DLGINFOFILE,(FARPROC)DlgStdProc,infoDialListPars);
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
	return ShowWindowList(IDS_WINDOWLIST);
}

/*--------------------------------------------------------------------------
 * DocTypecommand()
 */
static void DocNew(HWND hDlg);
static void DocDelete(HWND hDlg);
static void DocTypeApply(void);
static void DocTypeChange(HWND hDlg);
static void DocTypeFillParams(DIALPARS *dp, void *par);
#define	  NVDOCTYPEPARS						5
static DIALPARS docTypePars[] = 
{
	IDD_STRING6,		16,						0,
	IDD_STRING4,		50,						0,
	IDD_PATH1,		84,						0,
	IDD_STRING5,		84,						0,
	IDD_OPT1,			1,						0,
	IDD_WINDOWLIST,	0,						0,
	IDD_CALLBACK1,		0,						DocTypeChange,
	IDD_CALLBACK2,		0,						DocDelete,
	IDD_CALLBACK3,		0,						DocTypeApply,
	IDD_NOINITCALLBACK,	0,						DocNew,
	0
};

static void doclist_command(HWND hDlg, WORD nItem, WORD nNotify, void *pUser)
{
	switch(nNotify) {
	case LBN_SELCHANGE:
	case LBN_DBLCLK:
		LbGetText(hDlg, nItem, pUser);
		if (nNotify == LBN_SELCHANGE) {
			DocTypeFillParams(docTypePars, *(FTABLE **)pUser);
			DoDlgInitPars(hDlg, docTypePars, NVDOCTYPEPARS);
		} else {
			PostMessage(hDlg, WM_COMMAND, IDD_CALLBACK3, 0L);
		}
	}
}

/*------------------------------------------------------------
 * DocTypelboxfill()
 */
void DocTypelboxfill(HWND hwnd, int nItem, long selValue)
{
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	if (AddDocumentTypesToListBox(hwnd, nItem)) {
		SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)selValue);
	}
}

/*------------------------------------------------------------
 * ic_lboxmeasureitem()
 */
void DocTypelboxmeasureitem(MEASUREITEMSTRUCT* mp)
{
	mp->itemHeight = 22;
}


/*------------------------------------------------------------
 * DocTypelboxdraw()
 */
void DocTypelboxdrawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	char	*	pszId;
	char *	pszDescription;
	SIZE		size;
	SIZE		sizeDescription;
	int		nLen;
	int		nY;
	int		spacing = 3;

	if (!GetDocumentTypeDescription(par, 
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
 * DocTypeFillParams()
 */
static void DocTypeFillParams(DIALPARS *dp, void *par)
{
	char *	pszId;
	char *	pszDescription;
	char *	pszMatch;
	char *	pszFname;
	int *	pOwn;

	if (!GetDocumentTypeDescription(par, &pszId, &pszDescription, &pszMatch, 
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
 * DocTypeApply()
 */
void *lastSelectedDocType;
static void DocTypeApply(void)
{
	DOCUMENT_DESCRIPTOR *	lp;
	FTABLE *	fp;

	if ((fp = ft_CurrentDocument()) == 0) {
		return;
	}
	if ((lp = GetDocumentTypeDescriptor(lastSelectedDocType)) != 0) {
		if (!(fp->flags & F_MODIFIED)) {
			AbandonFile(fp, lp);
		} else {
			_free(fp->documentDescriptor);
			AssignDocumentTypeDescriptor(fp, lp);
			linchange();
		}
	}
}

/*--------------------------------------------------------------------------
 * DocNew()
 */
static void DocNew(HWND hDlg)
{
	lastSelectedDocType = CreateDocumentType(lastSelectedDocType);
	DocTypelboxfill(hDlg, IDD_WINDOWLIST, (long)lastSelectedDocType);
	DocTypeFillParams(docTypePars, (void*)lastSelectedDocType);
	DoDlgRetreivePars(hDlg, docTypePars, NVDOCTYPEPARS);
}

/*--------------------------------------------------------------------------
 * DocDelete()
 */
static void DocDelete(HWND hDlg)
{
	DeleteDocumentType(lastSelectedDocType);
	lastSelectedDocType = 0;
	DocTypelboxfill(hDlg, IDD_WINDOWLIST, (long)lastSelectedDocType);
	DocTypeFillParams(docTypePars, (void*)lastSelectedDocType);
}

/*--------------------------------------------------------------------------
 * DocTypeChange()
 */
static void DocTypeChange(HWND hDlg)
{
	DoDlgRetreivePars(hDlg, docTypePars, NVDOCTYPEPARS);
	DocTypelboxfill(hDlg, IDD_WINDOWLIST, (long)lastSelectedDocType);
}

/*--------------------------------------------------------------------------
 * DoDocumentTypes()
 */
int DoDocumentTypes(int nDlg)
{
	int		nRet;
	char		linname[128];
	static DIALLIST dlist = {
		(long*)&lastSelectedDocType, 
		DocTypelboxfill, 
		LbGetText, 
		DocTypelboxmeasureitem, 
		DocTypelboxdrawitem, 
		doclist_command
	};

	linname[0] = 0;

	docTypePars[NVDOCTYPEPARS].dp_data = &dlist;
	lastSelectedDocType = GetPrivateDocumentType(
		ft_CurrentDocument() ? ft_CurrentDocument()->documentDescriptor->modename : "default");

	DocTypeFillParams(docTypePars, (void*)lastSelectedDocType);
	if ((nRet = DoDialog(nDlg, (FARPROC)DlgStdProc,docTypePars)) == IDCANCEL) {
		lastSelectedDocType = 0;
		return 0;
	}

	if (nDlg != DLGDOCTYPES) {
		return TRUE;
	}

	SaveAllDocumentTypes((void *)0);

	return TRUE;
}

/*--------------------------------------------------------------------------
 * EdDocTypes()
 */
int EdDocTypes(void)
{
	return DoDocumentTypes(DLGDOCTYPES);
}

/*--------------------------------------------------------------------------
 * EdRangeShift()
 */
int EdRangeShift(int dir)
{	static ITEMS	 _i   = { C_INT1PAR,  (unsigned char*)&_scope	};
	static PARAMS	 _tp  = { 1, P_MAYOPEN, _i };

	_dconvert->dp_size = dir < 0 ? IDS_SHIFTLEFT : IDS_SHIFTRIGHT;
	if (!CallDialog(DLGCONVERT,&_tp,_dconvert))
		return 0;
		
	return RangeShift(_scope,dir);
}

/*--------------------------------------------------------------------------
 * color_lboxfill()
 */
static void color_lboxfill(HWND hwnd, int nItem, long selValue)
{
	PLOGPALETTE 	pLogPal;
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
		if (cColor == (COLORREF)selValue) {
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

	hBrush = CreateSolidBrush(PALETTEINDEX((int) par));
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
		0
	};
	DOCUMENT_DESCRIPTOR *linp;

	if (ft_CurrentDocument() == 0) {
		return 0;
	}

	linp = ft_CurrentDocument()->documentDescriptor;
	lstrcpy(status,linp->statusline);
	tabsize = 0;
	rmargin = linp->rmargin;

	dispmode = linp->dispmode;
	tabfill = linp->t1;
	_d[1].dp_data = &font;
	memmove(&font, &linp->fnt, sizeof font);
	bgcolor = linp->fnt_bgcolor;
	if (CallDialog(DLGDISPMODE,&_fp,_d) == 0) {
		return 0;
	}
	memmove(&linp->fnt, &font, sizeof font);
	linp->fnt_bgcolor = bgcolor;
	lstrcpy(linp->statusline,status);
	linp->dispmode = dispmode;
	linp->t1 = tabfill;
	if ((linp->tabsize = tabsize) != 0) {
		InitDocumentTypeDescriptor(linp, linp->tabsize);
		SendRedraw(WIPOI(ft_CurrentDocument())->ru_handle);
	}
	linp->rmargin = rmargin;
	return linchange();
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

	if (ft_CurrentDocument() == 0) {
		return 0;
	}

	linp = ft_CurrentDocument()->documentDescriptor;
	lstrcpy(cclass, linp->u2lset);
	lstrcpy(creationMacroName, linp->creationMacroName);
	lstrcpy(cm, linp->cm);
	workmode = linp->workmode;
	tabfill = linp->fillc;
	if (CallDialog(DLGWORKMODE,&_fp,_d) == 0)
		return 0;
	if (*cclass) {
		lstrcpy(linp->u2lset,cclass);
	}
	lstrcpy(linp->creationMacroName, creationMacroName);
	lstrcpy(linp->cm, cm);
	linp->workmode = workmode;
	linp->fillc = tabfill;
	return linchange();
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

	if (ft_CurrentDocument() == 0)
		return 0;

	linp = ft_CurrentDocument()->documentDescriptor;
	flags = linp->scrollflags;
	cursafter = linp->cursaftersearch;
	scrollmin = linp->vscroll+1;
	mindelta = linp->scroll_dy;
	if (CallDialog(DLGCURSTABS,&_fp,_d) == 0)
		return 0;
	linp->scrollflags = flags;
	linp->vscroll = scrollmin-1;
	linp->scroll_dy = mindelta;
	linp->cursaftersearch = cursafter;
	return linchange();
}

/*--------------------------------------------------------------------------
 * sel_ruler()
 */
static DOCUMENT_DESCRIPTOR *_linp;
static void sel_ruler(HWND hDlg, WORD idCtrl)
{
	EndDialog(hDlg,idCtrl);
	EdLineal(2,_linp);
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

	if ((ret = CallDialog(DLGFILEFORMAT,&_fp,_d)) == 0 ||
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
	if (ft_CurrentDocument() == 0)
		return 0;
	return DlgModeVals(ft_CurrentDocument()->documentDescriptor);
}

/*--------------------------------------------------------------------------
 * ReplaceAction()
 */
int ReplaceAction(int idCtrl)
{
	switch(idCtrl) {
		case IDD_BUT3: return REP_MARK;
		case IDD_BUT4: return REP_COUNT;
		case IDCANCEL: return 0;
		default:       return REP_REPLACE;
	}
}

/*--------------------------------------------------------------------------
 * EdReplace()
 */
int EdReplace(void)
{	static int ret;
	static int flags;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _finds }, 
		{ C_STRING1PAR, _repls }, 
		{ C_INT1PAR, (unsigned char *) &ret },
		{ C_INT1PAR, (unsigned char *) &_scope },
		{ C_INT1PAR, (unsigned char *) &_findopt   },
		{ C_INT1PAR, (unsigned char *) &flags },
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_RNGE,		RNG_ONCE,			&_scope,
		IDD_REGEXP,	RE_DOREX,			&_findopt,
		IDD_SHELLJOKER,RE_SHELLWILD,		&_findopt,
		IDD_IGNORECASE,RE_IGNCASE,		&_findopt,
		IDD_FINDS,	sizeof _finds,		&_finds,
		IDD_REPLS,	sizeof _repls,		&_repls,
		IDD_OPT1,		OREP_INQ,			&flags,
		IDD_OPT2,		OREP_MARKED,		&flags,
		IDD_RECORDRET,	sizeof ret,		&ret,
		0
	};

	if (CallDialog(DLGREPLACE,&_fp,_d) == 0) {
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
		{ C_STRING1PAR, _finds }, 
		{ C_INT1PAR, (unsigned char *) &_dir   },
		{ C_INT1PAR, (unsigned char *) &_scope },
		{ C_INT1PAR, (unsigned char *) &_findopt   }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_RADIO1,	1,				&_dir,
		IDD_REGEXP,	RE_DOREX,			&_findopt,
		IDD_SHELLJOKER,RE_SHELLWILD,		&_findopt,
		IDD_IGNORECASE,RE_IGNCASE,		&_findopt,
		IDD_OPT1,		O_WRAPSCAN,		&_findopt,
		IDD_FINDS,	sizeof _finds,		&_finds,
		0
	};

	if (_dir == -1)
		_dir = 0;

	if (!CallDialog(DLGFIND,&_fp,_d))
		return 0;

	if (_dir == 0)
		_dir = -1;

	return findstr(_dir);
}

/*--------------------------------------------------------------------------
 * EdFindInFileList()
 */
int EdFindInFileList(void)
{	static char pathlist[512];
	static char filenamePattern[50];
	static int once = 1,depth = -1;
	static ITEMS	_i   =  	{ 
		{ C_STRING1PAR, _finds }, 
		{ C_STRING1PAR, pathlist }, 
		{ C_INT1PAR, (unsigned char *) &once   },
		{ C_INT1PAR, (unsigned char *) &depth },
		{ C_INT1PAR, (unsigned char *) &_findopt }
	};
	static PARAMS	_fp = 	{ DIM(_i), P_MAYOPEN, _i	};
	static DIALPARS _d[] = {
		IDD_REGEXP,	RE_DOREX,			&_findopt,
		IDD_SHELLJOKER,RE_SHELLWILD,		&_findopt,
		IDD_IGNORECASE,RE_IGNCASE,		&_findopt,
		IDD_OPT1,		1,				&once,
		IDD_INT1,		sizeof depth,	&depth,
		IDD_FILE_PATTERN, sizeof filenamePattern, &filenamePattern, 
		IDD_FINDS2,	sizeof _finds,		&_finds,
		IDD_PATH1,	sizeof pathlist,	&pathlist,
		0
	};

	if (!pathlist[0] || !filenamePattern[0]) {
		getcwd(pathlist, sizeof pathlist);
		strcpy(filenamePattern, "*.*");
	}
	if (!CallDialog(DLGRETREIVE,&_fp,_d)) {
		return 0;
	}
	return retreive(pathlist,filenamePattern, _finds,depth < 0 ? 999 : depth,once);
}

/*--------------------------------------------------------------------------
 * DlgPrint()
 */
int DlgPrint(char *title, int *ps, int *pe, int *po)
{	static DIALPARS _d[] = {
		IDD_WINTITLE2,	0,				0,
		IDD_INT1,		sizeof(int),		0,
		IDD_INT2,		sizeof(int),		0,
		IDD_INT3,		sizeof(int),		0,
		0
	};
	DIALPARS *dp = _d;

	dp++->dp_data = title;
	dp++->dp_data = ps;
	dp++->dp_data = pe;
	dp->dp_data = po;

	return DoDialog(DLGPRINT,(FARPROC)DlgStdProc,_d);
}

/*--------------------------------------------------------------------------
 * EdReplaceAgain()
 */
int EdReplaceAgain(void)
{	extern int _rflg;

	if (_rflg) 
		return EdReplaceText(RNG_ONCE,REP_REPLACE,0); 
	ed_error(IDS_MSGNOREPLACESTRING);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdFindAgain()
 */
int EdFindAgain(int dir)
{	
	if (!_finds[0]) {
		ed_error(IDS_MSGNOSEARCHSTRING);
		return 0;
	}
	return findstr((dir) ? (_dir = dir) : _dir);
}

/*--------------------------------------------------------------------------
 * mac_getname()
 */
int mac_getname(KEYCODE *scan,char *name,int oldidx)
{	static DIALPARS  _d[] = {
		IDD_STRING1,	MAC_NAMELEN,		_finds,
		IDD_KEYCODE,	sizeof(KEYCODE),	0,
		0
	};

	_d[0].dp_data = name;
	_d[1].dp_data = scan;
	do {
		if (DoDialog(DLGMACNAME,(FARPROC)DlgStdProc,_d) == IDCANCEL)
			return 0;
	} while (!mac_isvalidname(name,oldidx));
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

   if (!CallDialog(DLGCONTROLINS,&_p,_d)) {
		return 0;
	}
	return EdCharInsert(0x100|(int)(unsigned char)c);
}

/*--------------------------------------------------------------------------
 * getuntilc()
 */
int getuntilc(int ids_num) 
{	static unsigned char c;

	c = DialogCharInput(ids_num,c);
	return c;
}

/*--------------------------------------------------------------------------
 * EdMacrosEdit()
 */
int EdMacrosEdit(void)
{	
	int 				ret;
	extern 	char *	_macroname;
	extern 	MACROREF	currentSelectedMacro;

	ret = DoDialog(DLGMACROS,(FARPROC)DlgMacEditProc,0);
	mac_switchtodefaulttables();

	if (ret == IDD_MACSTART) {
		long m = _multiplier;
		int  ret;
	
		_multiplier = 1;
		while(m-- > 0) {
			ret = mac_runcmd(&currentSelectedMacro);
		}
		return ret;
	}

	if (_macroname) {
		if (ret == IDD_MACEDIT) {
			return PrintMacs(_macroname);
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdCompileMacros()
 */
int EdCompileMacros(int bShowList)
{
	if (!bShowList || ShowWindowList(IDS_SELECTCOMPILEWINDOW)) {
		return macs_compile();
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdListBindings()
 */
int EdListBindings(long lWhich)
{
	if (lWhich & LIST_MACROS) {
		if (PrintMacs((char*)0) == 0) {
			return 0;
		}
	}

	if (lWhich & LIST_KEYS) {
		if (PrintKeys() == 0) {
			return 0;
		}
	}


	if (lWhich & LIST_MICE) {
		if (PrintMice() == 0) {
			return 0;
		}
	}

	if (lWhich & LIST_MENUS) {
		if (PrintMenus() == 0) {
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

	if (!CallDialog(DLGEXEC,&_fp,_d)) {
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
static long _ictype;
static DIALLIST icondlist = {
	&_ictype, ic_lboxfill, LbGetText, 
	ic_lboxmeasureitem, ic_lboxdrawitem, ic_lboxselchange, 0  };
static DIALPARS iconDialListPars[] = {
	IDD_STRING1,	sizeof _title,		_title,
	IDD_PATH1,	sizeof _pars,		_pars,
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

	if ((p = stralloc(_pars)) != 0) {
		hwnd = ic_add((void*)_ictype,_title,p,CW_USEDEFAULT,CW_USEDEFAULT);
		SendMessage(hwndClient, WM_MDIACTIVATE, hwnd, 0L);
		SendRedraw(hwnd);
		ic_enablecallbacks(hDlg, (void *)_ictype);
	}
	return 1;
}

static int del_icon(HWND hDlg)
{
	HWND hwnd;

	if ((hwnd = ic_active(_title,_pars,&_ictype)) != 0) {
		EdIconsQuit(hwnd);
	}
	DoDlgInitPars(hDlg, iconDialListPars, 3);
	return 1;
}

static int mod_icon(void)
{
	void *p;

	if ((p = stralloc(_pars)) != 0) {
		ic_mod(_title,p,_ictype);
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
	return CallDialog(DLGCONFICONS, &_fp, iconDialListPars);
}

/*--------------------------------------------------------------------------
 * GetPassWord()
 */
static void GetPassWord(LPSTR pszPW)
{
	static DIALPARS _d[] = {
		IDD_STRING1,	20,		0,
		0
	};

	pszPW[0] = 0;
	_d[0].dp_data = pszPW;
	DoDialog(DLGCRYPT, (FARPROC)DlgStdProc, _d);
}

/*--------------------------------------------------------------------------
 * CryptDialog()
 */
int CryptDialog(LPSTR password, int twice)
{
	char		pw1[128];

	while(1) {
		GetPassWord(password);
		if (twice == 0) {
			break;
		}
		GetPassWord(pw1);
		if (lstrcmp(pw1, password)) {
			ed_error(IDS_MSGDIFFERENTPW);
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

	if (!ft_CurrentDocument()) {
		return 0;
	}

	switch(what) {

	case QUERY_BLKMARKSTART:
		return ft_CurrentDocument()->blstart ? 1 : 0;

	case QUERY_BLKMARKED:
		if (!ft_CurrentDocument()->blstart) {
			return 0;
		}
		/* drop through */

	case QUERY_BLKMARKEND:
		return ft_CurrentDocument()->blend ? 1 : 0;

	case QUERY_WORKMODE:
		return ft_CurrentDocument()->documentDescriptor->workmode;

	case QUERY_DISPLAYMODE:
		return ft_CurrentDocument()->documentDescriptor->dispmode;

	case QUERY_CURRENTFILE:
		return 1;

	case QUERY_FILEMODIFIED:
		return (ft_CurrentDocument()->flags & F_MODIFIED) ? 1 : 0;
	
	case QUERY_BLOCKXTNDMODE:
		return WIPOI(ft_CurrentDocument())->bXtndBlock;
	}
	return 0;
}

