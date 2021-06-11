/*
 * PrintFiles.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: print files
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#include <string.h>
#include <stddef.h>

#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "printing.h"
#include "xdialog.h"
#include "stringutil.h"
#include "editorconfiguration.h"
#include "pathname.h"
#include "winutil.h"
#include "fileselector.h"

#define myoffsetof(typ,offs)	(unsigned char)offsetof(typ, offs)

#define	PREVIEWING()		(hwndPreview != 0)
#define	EVEN(p)			((p & 1) == 0)
#define	WP_FMTLCHAR		'|'

typedef struct tagDEVEXTENTS {
	int			xPage,yPage;
	int			xLMargin,xRMargin;
	int			yHeaderPos,yTop,
				yFooterPos,yBottom;
	int			headerSpace,lnSpace;
} DEVEXTENTS;

typedef struct tagPRINTWHAT {
	FTABLE	*fp;
	WINFO	*wp;
	LINE 	*lp,*lplast;
	int		firstc,lastc,nlines;
} PRINTWHAT;

static PRTPARAM _prtparams = {
	1,-1,0,0,0,60, 		/* startpage,endpage,pageoffs,mode,options,pagelen */
	0,8,0,"Tms Roman",	/* Std Font: oemmode,cheight,cwidth,name */
	0,0,120,			/* lmargin,rmargin, nchars */
	5,1,1,			/* tabsize, lnspace */

	0,0,0,0,			/* distances header, footer */
	0,8,0,"Courier",	/* Header Font: oemmode,cheight,cwidth,name */
	"",					/* header template. A PKS Edit template (see mysprintf) optionally containing multiple segments separated by '!' */
	"%s$f - %D",		/* footer template */
	0,					/* header, footer align */

	0,8,0,"Courier",	/* Footnote Font: oemmode,cheight,cwidth,name */

	1,1,				/* print_singleLineOfText spacing footnotes */
	0,				/* length of fn delimter line */
	0,0,				/* distances to body and footer */
	0,				/* offset of Fnote */
};

extern char 		*ft_visiblename(FTABLE *fp);

static BOOL		_doPrint;
static int 		_printing;
static int 		_previewpage = 1;
static HWND 		hwndPreview;
static PRINTWHAT	_printwhat;
static char 		szPrtDevice[64];

extern int		mysprintf(FTABLE* fp, char* d, char* format, ...);

/*------------------------------------------------------------
 * print_getPrinterDC()
 */
EXPORT HDC print_getPrinterDC(void)
{
	char szPrinter[256];
	char *szDriver,*szOutput;
	HDC hdc;

	GetProfileString("devices", szPrtDevice, "", szPrinter, sizeof szPrinter);
	if ((szDriver = strtok(szPrinter,",")) != 0 &&
	    (szOutput = strtok(NULL     ,",")) != 0 &&
	    (hdc = CreateDC(NULL,szPrtDevice, NULL, NULL)) != 0) {
	    	return hdc;
	}
	error_showErrorById(IDS_MSGNOPRINTER);
	return 0;
}

/*------------------------------------------------------------
 * print_initPrinterDC()
 */
EXPORT void print_initPrinterDC(void)
{
	char *dev,szPrinter[256];

	GetProfileString("windows", "device", ",,,", szPrinter, sizeof szPrinter);
	if ((dev = strtok(szPrinter,",")) != 0)
		lstrcpy(szPrtDevice,dev);
}

/*------------------------------------------------------------
 * EscapeError()
 */
static void EscapeError(short errEscape)
{
	int 	nId;

	if (errEscape >= 0 || (errEscape & SP_NOTREPORTED) == 0)
		return;
	
	switch(errEscape) {
		case SP_ERROR: 		nId = IDS_PRTERR; break;
		case SP_OUTOFDISK:		nId = IDS_PRTERRNOSPCE; break;
		case SP_OUTOFMEMORY:	nId = IDS_PRTERRNOMEM; break;
		case SP_USERABORT:		nId = IDS_PRTERRUSRABRT; break;
		default: 				nId = IDS_PRTERRABRT; break;
	}

	error_showErrorById(nId);
}

/*------------------------------------------------------------
 * PrtAbortProc()
 */
static BOOL PrtAbortProc(HDC hdcPrn, int nCode) {
	if (nCode < 0) {
		EscapeError(nCode);
	}
	return !progress_cancelMonitor(FALSE);
}

/*------------------------------------------------------------
 * print_selectfont()
 */
static HFONT print_selectfont(HDC hdc, FONTSPEC *fsp) {
	static HFONT previousFont = NULL;
	HFONT 		hFont;
	EDFONT		font;
	TEXTMETRIC 	tm;

	memset(&font, 0, sizeof font);
	if (!PREVIEWING()) {
		lstrcpy(font.name, fsp->fs_name);
	}
	font.style.weight = FW_NORMAL;
	font.height= fsp->fs_cheight;
	font.width = fsp->fs_cwidth;
	font.charset = fsp->fs_oemmode ? OEM_CHARSET : ANSI_CHARSET;
	if ((hFont = font_createFontWithStyle(&font, NULL)) != 0) {
		SelectObject(hdc, hFont);
		if (previousFont) {
			//
			// Avoid running out of GDI resources and eliminate font handles
			// allocated before and now not being used any more.
			//
			DeleteObject(previousFont);
		}
		previousFont = hFont;
		if (PREVIEWING()) {
			SetBkMode(hdc, TRANSPARENT);
		}
		GetTextMetrics(hdc,&tm);
		_printwhat.wp->cwidth  = tm.tmAveCharWidth;
		_printwhat.wp->cheight = tm.tmHeight/* + tm.tmExternalLeading */;
		return hFont;
	} else {
		return 0;
	}
}

/*------------------------------------------------------------
 * print_calculateRatio()
 */
static int print_calculateRatio(int total, int mul, int div)
{
	if (div == 0)
		div = 1;
	return (total * (long)mul / (long)div);
}

/*------------------------------------------------------------
 * print_ntextlines()
 */
static int print_ntextlines(void)
{
	PRTPARAM	*pp = &_prtparams;

	int n = (pp->options & PRTO_HEADERS) ? 
			(pp->pagelen - 2 - pp->d1 - pp->d2 - pp->d3 - pp->d4) :
			pp->pagelen;
	return n <= 0 ? 2 : n;
}

/*--------------------------------------------------------------------------
 * print_resetDeviceMode()
 */
static void print_resetDeviceMode(HDC hdc)
{
	RECT r;
	int	x;
	int	y;

	if (PREVIEWING()) {
		GetClientRect(GetDlgItem(hwndPreview,IDD_PREVIEW),&r);
		x = r.right;
		y = r.bottom;
	} else {
		x = GetDeviceCaps(hdc, HORZRES);
		y = GetDeviceCaps(hdc, VERTRES);
	}

	SetMapMode(hdc,MM_ANISOTROPIC);
	win_setWindowExtension(hdc, 14400, 14400);
	win_setViewportExtension(hdc,x,y);

}

/*------------------------------------------------------------
 * print_getDeviceExtents()
 */
static void print_getDeviceExtents(HDC hdc, DEVEXTENTS *dep) {
	PRTPARAM 	*pp = &_prtparams;
	DWORD	ext;
	int		textheight;

	print_resetDeviceMode(hdc);
	ext = win_getWindowExtension(hdc);
	dep->xPage = LOWORD(ext);
	dep->yPage = HIWORD(ext);

	dep->xLMargin = print_calculateRatio(dep->xPage, pp->lmargin, pp->nchars);
	dep->xRMargin = print_calculateRatio(dep->xPage, (pp->nchars-pp->rmargin), pp->nchars);

	if (pp->options&PRTO_HEADERS) {
		dep->yHeaderPos = print_calculateRatio(dep->yPage,pp->d1,pp->pagelen);
		dep->yTop 	 = print_calculateRatio(dep->yPage,pp->d1+pp->d2+1,pp->pagelen);
		dep->yFooterPos = print_calculateRatio(dep->yPage,(pp->pagelen-pp->d4-1),
						    pp->pagelen);
		dep->yBottom    = print_calculateRatio(dep->yPage,(pp->pagelen-pp->d3-pp->d4-1),
				       	    pp->pagelen);
	} else {
		dep->yHeaderPos = dep->yTop = 0;
		dep->yFooterPos = dep->yBottom = dep->yPage;
	}

	textheight = (dep->yBottom-dep->yTop);
	dep->lnSpace = (textheight) / print_ntextlines();

	pp->font.fs_cheight = 
	pp->htfont.fs_cheight = 
		 dep->lnSpace * pp->lnspace.n / pp-> lnspace.z;

	pp->font.  fs_cwidth =
	pp->htfont.fs_cwidth = 
		(dep->xPage / (pp->nchars + pp->lmargin + pp->rmargin)) * pp->nchars / 120;
}

/*---------------------------------*/
/* print_mkwpheader()				*/
/*---------------------------------*/
static void print_mkwpheader(HDC hdc, int yPos, DEVEXTENTS *dep,
				   char *szBuff, int align)
{
	int  		nLen,xPos,w;

	if ((nLen = lstrlen(szBuff)) == 0)
		return;
	w = LOWORD(win_getTextExtent(hdc,szBuff,nLen));

	switch(align) {
		default:	xPos = dep->xLMargin; break;
		case 1:	xPos = (dep->xPage - w) / 2; break;
		case 2:	xPos = (dep->xRMargin - w); break;
	}
	TextOut(hdc,xPos,yPos,szBuff,nLen);
}

/*---------------------------------*/
/* print_formatheader()				*/
/*---------------------------------*/
static int print_formatheader(unsigned char *d1, unsigned char *d2, 
			  	    unsigned char *d3, unsigned char *s,
				    long pageno)
{
	unsigned char  buf[256];
	int			nParts = 0;

	if (lstrlen(s) >= sizeof buf)
		return 0;
	lstrcpy(buf,s);

	if ((s = strtok(buf,"!")) != 0) {
		mysprintf(_printwhat.fp,d1,s,pageno);
		if ((s = strtok((char *)0,"!")) != 0) {
			nParts++;
			mysprintf(_printwhat.fp,d2,s,pageno);
			if ((s = strtok((char *)0,"!")) != 0) {
				nParts++;
				mysprintf(_printwhat.fp,d3,s,pageno);
			}
		}
	}
	return nParts;
}

/*------------------------------------------------------------
 * print_headerAndFooter()
 */
static void print_headerAndFooter(HDC hdc, DEVEXTENTS *dep, int y, long pageno, 
				char *fmt,int align)
{
	unsigned char 	b1[256],b2[256],b3[256];
	int  			wpflg,a1,a2;
	PRTPARAM		*pp = &_prtparams;

	if (!(pp->options & PRTO_HEADERS))
		return;

	b1[0] = b2[0] = b3[0] = 0;
	wpflg = print_formatheader(b1, b2, b3, fmt, pageno + pp->pageoffs);

	/* swap headings on alternate pages */

	if (!wpflg) {
		switch (align) {
			case PRA_LEFT:
				break;
			case PRA_CENTER:
				lstrcpy(b2,b1); b1[0] = 0; break;
			case PRA_RIGHT:
				lstrcpy(b3,b1); b1[0] = 0; break;
		}
	}

	if (align != PRA_CENTER && 
	    ((pp->options & PRTO_SWAP) && EVEN(pageno))) {
	    	a1 = 0, a2 = 2;
	} else {
		a1 = 2, a2 = 0;
	}
	print_mkwpheader(hdc,y,dep,b1,a1);
	print_mkwpheader(hdc,y,dep,b2,1);
	print_mkwpheader(hdc,y,dep,b3,a2);
}

/*------------------------------------------------------------
 * print_singleLineOfText()
 */
static int print_singleLineOfText(HDC hdc, int xPos, int yPos, int charHeight, long lineno,
		 	  LINE *lp, int firstc, int lastc)
{
	char 	szBuff[80];
	int		nCount;
	int		max;
	int		nMaxCharsPerLine;
	int		deltaHeight = charHeight;

	nMaxCharsPerLine = 0;
	if (_prtparams.options & PRTO_LINES) {
		wsprintf(szBuff, "%3ld: ", lineno);
		TextOut(hdc, xPos, yPos, szBuff, nCount = lstrlen(szBuff));
		xPos += LOWORD(win_getTextExtent(hdc, szBuff, nCount));
		nMaxCharsPerLine = -7;
	}

	_printwhat.wp->mincol = firstc;
	max = caret_lineOffset2screen(_printwhat.fp, &(CARET) { lp, lp->len});
	if (max > lastc)
		max = lastc;
	nMaxCharsPerLine += _prtparams.nchars - _prtparams.lmargin - _prtparams.rmargin;
	_printwhat.wp->maxcol = nMaxCharsPerLine;
	_printwhat.wp->maxcol = max;
	if (_printwhat.wp->maxcol > nMaxCharsPerLine) {
		_printwhat.wp->maxcol = nMaxCharsPerLine;
	}
	if (_printwhat.wp->maxcol > firstc) {
		_printwhat.wp->renderFunction(hdc, xPos, yPos, _printwhat.wp, lp);
		while (_printwhat.wp->maxcol < max) {
			int delta = max - _printwhat.wp->maxcol;
			if (delta > nMaxCharsPerLine) {
				delta = nMaxCharsPerLine;
			}
			_printwhat.wp->mincol = _printwhat.wp->maxcol+1;
			_printwhat.wp->maxcol = _printwhat.wp->mincol+delta;
			deltaHeight += charHeight;
			yPos += charHeight;
			_printwhat.wp->renderFunction(hdc, xPos, yPos, _printwhat.wp, lp);
		}
	}
	return deltaHeight;
}

/*------------------------------------------------------------
 * print_drawLine()
 */
static void print_drawLine(HDC hdc, int x1, int x2, int y)
{
	MoveTo(hdc,x1,y);
	LineTo(hdc,x2,y);
}

/*------------------------------------------------------------
 * PrintPage()
 */
static int print_file(HDC hdc)
{	char 		message[128];
	long		oldpageno,lineno,pageno;
	int			yPos,ret = 1;
	DEVEXTENTS	de;
	LINE 		*lp,*lplast;
	PRTPARAM	*pp = &_prtparams;
	int			firstc,lastc;
	RECT		pageRect;

	lp     = _printwhat.lp;
	lplast = _printwhat.lplast;
	firstc = _printwhat.firstc;
	lastc  = _printwhat.lastc;

	print_getDeviceExtents(hdc, &de);
	print_selectfont(hdc,&pp->font);

	oldpageno = 0;
	pageno = lineno = 1;
	yPos = 0;
	if (PREVIEWING()) {
		pageRect.left = 0;
		pageRect.right = de.xPage;
		pageRect.top = 0;
		pageRect.bottom = de.yPage;
		HGDIOBJ original = SelectObject(hdc, GetStockObject(DC_PEN));
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, RGB(255, 255, 255));
		Rectangle(hdc, 0, 0, pageRect.right, pageRect.bottom);
		SelectObject(hdc, original);
	}
	while (lp) {
		if (oldpageno != pageno) {
			if (PREVIEWING()) {
				if (pageno > _previewpage)
					break;
				_printing = (pageno == _previewpage);
			} else {
				_printing = (pageno >= pp->startpage && 
			       		   (pp->endpage <= 0 || pageno <= pp->endpage));
			}
			if (!PREVIEWING() && _printing) {
				wsprintf(message,/*STR*/"%s - %s (Seite %d)",
						(LPSTR)szAppName,
					  	(LPSTR)string_abbreviateFileName(ft_visiblename(ft_getCurrentDocument())),
					  	pageno);
				progress_showMonitorMessage(message); 
			}
			oldpageno = pageno;
		}
		if (yPos == 0) {
			if (_printing) {
				StartPage(hdc);
				print_resetDeviceMode(hdc);
				print_selectfont(hdc,&pp->htfont);
				print_headerAndFooter(hdc, &de, de.yHeaderPos, pageno,
					pp->header, pp->align);
				print_selectfont(hdc,&pp->font);
				if (pp->options & PRTO_ULHEA) {
					print_drawLine(hdc,de.xLMargin,de.xRMargin,
						   (de.yTop+de.yHeaderPos)/2);
				}
			}
			yPos = de.yTop;
		}
		if (yPos+de.lnSpace > de.yBottom) {
footerprint:
			if (_printing) {
				if (pp->options & PRTO_ULHEA) {
					print_drawLine(hdc,de.xLMargin,de.xRMargin,
						    (de.yBottom+de.yFooterPos)/2);
				}
				print_selectfont(hdc,&pp->htfont);
				print_headerAndFooter(hdc,&de,de.yFooterPos,pageno,pp->footer,pp->align);
				EndPage(hdc);
			}
			if (!lp || P_EQ(lp,lplast))
				break;
			yPos = 0;
			pageno++;
		} else {
			if (_printing) {
				yPos += print_singleLineOfText(hdc, de.xLMargin, yPos, de.lnSpace, lineno, lp, firstc, lastc);
			}
			else {
				yPos += de.lnSpace;
			}
			if (P_EQ(lp,lplast) || (lp = lp->next) == 0)
				goto footerprint;
			lineno++;
		}
	}
	font_selectSystemFixedFont(hdc);
	return ret;
}

/*--------------------------------------------------------------------------
 * DlgPreviewProc()
 */
static INT_PTR CALLBACK DlgPreviewProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	int			newPage = _previewpage;

	switch(message) {
		case WM_NOTIFY: {	
			LPNMHDR		pNmhdr;

			pNmhdr = (LPNMHDR)lParam;
			if (pNmhdr->code == PSN_SETACTIVE) {
				newPage = 1;
			}
			}
			break;
		case WM_INITDIALOG:
			hwndPreview = hDlg;
			win_moveWindowToDefaultPosition(hDlg);
			SetDlgItemInt(hDlg, IDD_INT1, _previewpage, 0);
			break;
		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDCANCEL:
					EndDialog(hDlg,1);
					break;
				case IDD_BUT3:
					newPage++; break;
				case IDD_BUT4:
					newPage--; break;
			}
			break;
		case WM_DRAWITEM:
			hdc = ((LPDRAWITEMSTRUCT)lParam)->hDC;
			int savedDc = SaveDC(hdc);
			print_file(hdc);
			RestoreDC(hdc, savedDc);
			return TRUE;
		case WM_DESTROY:
			hwndPreview = 0;
			break;
		default:
			return FALSE;
	}
	if (newPage != _previewpage) {
		_previewpage = newPage;
		SetDlgItemInt(hDlg, IDD_INT1, _previewpage, 0);
		EnableWindow(GetDlgItem(hDlg, IDD_BUT4), (_previewpage > 1));
		EnableWindow(GetDlgItem(hDlg, IDD_BUT3),
			(_previewpage <= _printwhat.nlines / print_ntextlines()));
		win_sendRedrawToWindow(GetDlgItem(hDlg, IDD_PREVIEW));
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * DlgInstallPrtProc()
 */
typedef VOID (FAR PASCAL *DEVMODEPROC)(HWND, HANDLE, LPSTR, LPSTR);
static INT_PTR CALLBACK DlgInstallPrtProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND			hwndList;
	HANDLE		hLibrary;
	DEVMODEPROC	lpfnDM;
	HWND		parent;
	char			szDevice[64],szDriver[256],
				szDriverName[64],*szOutput,*s;

	parent = GetParent(hDlg);
	switch(message) {
		case WM_NOTIFY:
		{	LPNMHDR		pNmhdr;

		pNmhdr = (LPNMHDR)lParam;
		switch (pNmhdr->code) {
		case PSN_SETACTIVE:
			hwndList = GetDlgItem(hDlg, IDD_INSTPRINTERS);
			SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
			GetProfileString("devices", NULL, "", _linebuf, LINEBUFSIZE);
			s = _linebuf;
			while (*s) {
				SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)s);
				while (*s++)
					;
			}
			SendMessage(hwndList, LB_SELECTSTRING, -1, (LPARAM)szPrtDevice);
			break;
		case PSN_APPLY:
			LbGetText(hDlg, IDD_INSTPRINTERS, szPrtDevice);
			PropSheet_UnChanged(parent, hDlg);
			return TRUE;
		}
		}
		break;
		case WM_INITDIALOG:
		case WM_COMMAND:
			PropSheet_Changed(parent, hDlg);
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDD_INSTINSTALL:
					LbGetText(hDlg,IDD_INSTPRINTERS,szDevice);
					GetProfileString("devices", szDevice,"",szDriver,
								   sizeof szDriver);
					if ((szOutput = strtok(szDriver,",")) != 0) {
						wsprintf(szDriverName,"%s.DRV",szDriver);
						if ((hLibrary = LoadLibrary(szDriverName)) != NULL) {
							lpfnDM = (DEVMODEPROC)GetProcAddress(hLibrary,"DEVICEMODE");
							(*lpfnDM)(hDlg,hLibrary,(LPSTR)szDevice,(LPSTR)szOutput);
							FreeLibrary(hLibrary);
						}
					}
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * print_readWriteConfigFile()
 * Read / write the print configuration of PKS edit depending
 * on the save flag passed as an argument.
 */
void print_readWriteConfigFile(int save)
{
	char 		*prt	= "print",*name;
	char			szBuff[256];
	PRTPARAM		*pp = &_prtparams;
	void			*loc;
	int			i;
	static struct profitems {
		unsigned char	pr_isstring;
		unsigned char	pr_offs;
		char			*pr_name;
	} _pi[] = {
		0,	myoffsetof(PRTPARAM,mode),				"mode",
		0,	myoffsetof(PRTPARAM,options),			"options",
		0,	myoffsetof(PRTPARAM,pagelen),			"pagelen",
		1,	myoffsetof(PRTPARAM,font.fs_name),		"font",
		1,	myoffsetof(PRTPARAM,htfont.fs_name),		"htfont",
		0,	myoffsetof(PRTPARAM,lnspace.n),			"space_n",
		0,	myoffsetof(PRTPARAM,lnspace.z),			"space_z",
		1,	myoffsetof(PRTPARAM,header),			"header",
		1,	myoffsetof(PRTPARAM,footer),			"footer",
		0,	myoffsetof(PRTPARAM,lmargin),			"lmargin",
		0,	myoffsetof(PRTPARAM,rmargin),			"rmargin",
		0,	myoffsetof(PRTPARAM,nchars),			"chars",
		0,	myoffsetof(PRTPARAM,d1),				"dhtop",
		0,	myoffsetof(PRTPARAM,d2),				"dhbot",
		0,	myoffsetof(PRTPARAM,d3),				"dftop",
		0,	myoffsetof(PRTPARAM,d4),				"dfbot",
		0,	myoffsetof(PRTPARAM,align),			"align"
	};


	for (i = 0; i < DIM(_pi); i++) {
		loc = ((char*)pp+_pi[i].pr_offs);
		name = _pi[i].pr_name;
		if (save) {
			if (_pi[i].pr_isstring) {
				prof_savestring(prt,name,(LPSTR)loc);
			}
			else {
				prof_savelong(prt,name,(long)(*(int*)loc));
			}
		} else {
			if (prof_getPksProfileString(prt,name,szBuff,sizeof szBuff)) {
				if (_pi[i].pr_isstring) {
					lstrcpy((LPSTR)loc,szBuff);
				}
				else {
					*(int*)loc = string_convertToLong(szBuff);
				}
			}
		}
	}

	if (!pp->lnspace.z)
		pp->lnspace.z = 1;
}

/*--------------------------------------------------------------------------
 * DlgPrint()
 */
static EDFONT	font;
static EDFONT	htfont;
static void _triggerPrint(HWND hDlg, int idCtrl) {
	_doPrint = TRUE;
	HWND hwndParent = GetParent(hDlg);
	HWND hwndOK = GetDlgItem(hwndParent, IDOK);
	SendMessage(hwndOK, BM_CLICK, (WPARAM) NULL, (LPARAM)NULL);
}
static DIALPARS _dPrintLayout[] = {
	IDD_INT5,		sizeof(int),		0,
	IDD_INT6,		sizeof(int),		0,
	IDD_INT7,		sizeof(int),		0,
	IDD_CALLBACK1,	0,				_triggerPrint,
	IDD_RADIO1,		PRA_RIGHT - PRA_LEFT,	&_prtparams.align,
	IDD_OPT1,			PRTO_SWAP,& _prtparams.options,
	IDD_OPT2,			PRTO_LINES,& _prtparams.options,
	IDD_OPT3,			PRTO_HEADERS,& _prtparams.options,
	IDD_OPT4,			PRTO_ULHEA,& _prtparams.options,
	IDD_INT1,			0,& _prtparams.pagelen,
	IDD_INT2,			0,& _prtparams.nchars,
	IDD_INT3,			0,& _prtparams.lmargin,
	IDD_INT4,			0,& _prtparams.rmargin,
	IDD_STRING1,		sizeof _prtparams.header,	_prtparams.header,
	IDD_STRING2,		sizeof _prtparams.footer,	_prtparams.footer,
	IDD_FONTSELECT,	TRUE,				_prtparams.font.fs_name,
	IDD_FONTSELECT2,	TRUE,			_prtparams.htfont.fs_name,
	0
};
static DIALPARS* _getDialogParsForPage(int page) {
	if (page == 0) {
		return _dPrintLayout;
	}
	if (page == 2) {
		_previewpage = 1;
	}
	return NULL;
}
static int DlgPrint(char* title, PRTPARAM *pp)
{
	DIALPARS* dp = _dPrintLayout;
	//dp++->dp_data = title;
	dp++->dp_data = &pp->startpage;
	dp++->dp_data = &pp->endpage;
	dp->dp_data = &pp->pageoffs;

	lstrcpy(font.name, _prtparams.font.fs_name);
	lstrcpy(htfont.name, _prtparams.htfont.fs_name);

	PROPSHEETPAGE psp[3];
	PROPSHEETHEADER psh;
	INT_PTR tempRet;

	InitCommonControls();
	SetXDialogParams(_getDialogParsForPage, TRUE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[1]);
	psp[0].hInstance = hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(DLGPRINTERLAYOUT);
	psp[0].pfnDlgProc = DlgStdProc;

	psp[1].dwSize = sizeof(psp[1]);
	psp[1].hInstance = hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(DLGPREVIEW);
	psp[1].pfnDlgProc = DlgPreviewProc;

	psp[2].dwSize = sizeof(psp[1]);
	psp[2].hInstance = hInst;
	psp[2].pszTemplate = MAKEINTRESOURCE(DLGPRTINSTALL);
	psp[2].pfnDlgProc = DlgInstallPrtProc;

	psh.dwSize = sizeof(psh);
	psh.dwFlags = PSH_PROPSHEETPAGE;
	psh.hwndParent = hwndFrame;
	psh.hInstance = hInst;
	psh.pszIcon = 0;
	psh.pszCaption = (LPSTR)"Drucken...";
	psh.nPages = sizeof(psp) / sizeof(psp[0]);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback = NULL;
	_doPrint = FALSE;
	tempRet = PropertySheet(&psh);
	if (tempRet == 1) {
		print_readWriteConfigFile(1);
		lstrcpy(_prtparams.font.fs_name, font.name);
		lstrcpy(_prtparams.htfont.fs_name, htfont.name);
	}
	return (int)tempRet;
}


/*------------------------------------------------------------
 * EdPrint()
 */
int EdPrint(long what, long p1, LPSTR fname)
{
	HDC 			hdcPrn;
	int 			ret = 0;
	int				nFunc = 0;
	int				errEscape;
	char 			message[128],ch;
	WINFO			winfo;
	WINFO*			wp;
	FTABLE			*fp,_ft;
	PRTPARAM		*pp = &_prtparams;

	memset(&winfo,0,sizeof winfo);
	memset(&_ft,0,sizeof _ft);
	_printwhat.firstc = 0;
	_printwhat.lastc = 1024;

	wp = 0;

	if (what == PRT_CURRWI || what == PRT_CURRBLK) {
		if ((wp = ww_getWindowFromStack(0)) == 0) {
			return 0;
		}
		fp = wp->fp;
	}

	if (what == PRT_FILE) {
		fp = &_ft;
		if (fname)
			lstrcpy(fp->fname,fname);
		else
			if (!fsel_selectFileWithTitle(IDM_PRINTFILE,fp->fname, FALSE))
				return 0;
		if (ft_readfileWithOptions(fp,fp->fname,0) == 0)
	 		return 0;
		WIPOI(fp) = &winfo;
		FTPOI((&winfo)) = fp;
		ww_applyDisplayProperties(&winfo);
	}

	if (what == PRT_CURRWI || what == PRT_FILE) {
		_printwhat.lp = fp->firstl;
		_printwhat.lplast = fp->lastl->prev;
		if (fp->documentDescriptor) {
			BOOL showLineNumbers = fp->documentDescriptor->dispmode & SHOWLINENUMBERS;
			if (showLineNumbers) {
				_prtparams.options |= PRTO_LINES;
			}
			else {
				_prtparams.options &= ~PRTO_LINES;
			}
		}
	} else {
		if (!ft_checkSelectionWithError(fp))
			goto byebye;
		if (ww_blkcolomn(fp->wp)) {
			_printwhat.firstc =  fp->blcol1;
			_printwhat.lastc = fp->blcol2;
		}
		_printwhat.lp = fp->blstart->lm;
		_printwhat.lplast = fp->blend->lm;
	}

	if ((_printwhat.fp = fp) == 0) {
		goto byebye;
	}
	_printwhat.nlines = 
		ln_cnt(_printwhat.lp,_printwhat.lplast);
	ch = (what == PRT_CURRBLK) ? '*' : ' ';
	wsprintf(message,"%c %s %c", ch, string_getBaseFilename(ft_visiblename(fp)), ch);

	memmove(&winfo,WIPOI(fp),sizeof winfo);
	_printwhat.wp = &winfo;
	lstrcpy(winfo.fnt_name,pp->font.fs_name);
	winfo.fnt.height = pp->font.fs_cheight;
	_previewpage = 0;
	DlgPrint(message, pp);
	if (_doPrint) {
		_doPrint = FALSE;
		_previewpage = 0;
		if ((hdcPrn = print_getPrinterDC()) != NULL) {
			DOCINFO		docinfo;

			memset(&docinfo, 0, sizeof docinfo);
			docinfo.cbSize = sizeof docinfo;
			docinfo.lpszDocName = message;
			docinfo.lpszDatatype = "Text";
			progress_startMonitor(IDS_ABRTPRINT);
			SetAbortProc(hdcPrn, (ABORTPROC)PrtAbortProc);
			if ((PREVIEWING() || (errEscape = StartDoc(hdcPrn, &docinfo))) >= 0 &&
			    (errEscape = print_file(hdcPrn)) >= 0) {
				if (!PREVIEWING()) {
					EndDoc(hdcPrn);
				}
			} else {
				EscapeError(errEscape);
			}
		}
		if (hdcPrn) {
			progress_closeMonitor(0);
			DeleteDC(hdcPrn);
		}
		if (errEscape)
			goto byebye;
		ret = 1;
	}
byebye:
	if (what == PRT_FILE && fp)
		ft_bufdestroy(fp);
	return ret;
}


