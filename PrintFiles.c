/*
 * PrintFiles.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
 *
 * purpose: print files
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

#define	PREVIEWING()	(hwndPreview != 0)
#define	EVEN(p)			((p & 1) == 0)
#define	WP_FMTLCHAR		'|'

#define DEFAULT_PRINT_COLOR RGB(0,0,0)

typedef struct tagDEVEXTENTS {
	int			xPage,yPage;
	int			xLMargin,xRMargin;
	int			yHeaderPos,yTop,
				yFooterPos,yBottom;
	int			headerSpace,lineHeight;
} DEVEXTENTS;

typedef struct tagPRINTWHAT {
	FTABLE	*fp;
	WINFO	*wp;
	LINE 	*lp,*lplast;
	int		firstColumn,lastColumn,nlines;
} PRINTWHAT;

static PRTPARAM _prtparams = {
	PRTR_PAGES,0,0,		/* Print range*/
	0,0,60, 			/* mode,options,pagelen */
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

static HWND 		hwndPreview;
static PRINTWHAT	_printwhat;
static char 		szPrtDevice[64];

extern int		mysprintf(WINFO* wp, char* d, char* format, ...);

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
 * print_drawLine()
 */
static void print_drawLine(HDC hdc, DEVEXTENTS* dep, int x1, int x2, int y, int y2)
{
	/// Should be made configurable somehow.
	int delta = dep->lineHeight/4;
	HPEN grayPen = CreatePen(PS_SOLID, delta, RGB(220, 220, 220));
	HPEN hpenOld = SelectObject(hdc, grayPen);
	Rectangle(hdc, x1, y+delta, x2, y2-delta);
	SelectObject(hdc, hpenOld);
	DeleteObject(grayPen);
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
	TEXTMETRIC 	tm;

	if ((hFont = font_createFontHandle(fsp->fs_name, fsp->fs_cheight, fsp->fs_oemmode)) != 0) {
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
		_printwhat.wp->cheight = tm.tmHeight;
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
static int print_ntextlines(DEVEXTENTS* dep)
{
	PRTPARAM	*pp = &_prtparams;

	return (dep->yFooterPos - dep->yHeaderPos) / pp->font.fs_cheight;
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
	int		totalHeight;

	print_resetDeviceMode(hdc);
	ext = win_getWindowExtension(hdc);
	dep->xPage = LOWORD(ext);
	dep->yPage = HIWORD(ext);

	dep->xLMargin = print_calculateRatio(dep->xPage, pp->lmargin, pp->nchars);
	dep->xRMargin = print_calculateRatio(dep->xPage, (pp->nchars-pp->rmargin), pp->nchars);

	totalHeight = dep->yPage;
	dep->lineHeight = totalHeight / _prtparams.pagelen;

	int cFontHeight = dep->lineHeight * pp->lnspace.n / pp->lnspace.z;
	pp->font.fs_cheight =
		pp->htfont.fs_cheight = cFontHeight;

	pp->font.fs_cwidth =
		pp->htfont.fs_cwidth =
		(dep->xPage / (pp->nchars + pp->lmargin + pp->rmargin)) * pp->nchars / 120;

	dep->yHeaderPos = pp->marginTop * cFontHeight;
	dep->yFooterPos = dep->yPage - pp->marginBottom * cFontHeight;
	if (pp->options & PRTO_HEADERS) {
		dep->yTop = dep->yHeaderPos+pp->headerSize*cFontHeight;
		dep->yBottom = dep->yFooterPos - pp->footerSize * cFontHeight;
	} else {
		dep->yTop = dep->yHeaderPos;
		dep->yFooterPos = dep->yFooterPos;
	}

}

/*---------------------------------*/
/* print_mkwpheader()				*/
/*---------------------------------*/
static void print_mkwpheader(HDC hdc, int yPos, DEVEXTENTS *dep,
				   char *szBuff, int align)
{
	int  nLen,xPos;
	SIZE size;

	if ((nLen = lstrlen(szBuff)) == 0)
		return;
	GetTextExtentPoint32(hdc, szBuff, nLen, &size);

	switch(align) {
		default:	xPos = dep->xLMargin; break;
		case PRA_CENTER:	xPos = (dep->xPage - size.cx) / 2; break;
		case PRA_RIGHT:		xPos = (dep->xRMargin - size.cx); break;
	}
	SetTextColor(hdc, DEFAULT_PRINT_COLOR);
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
	nParts = 1;
	// TODO: allow using character ! in headers and footers.
	if ((s = strtok(buf,"!")) != 0) {
		mysprintf(_printwhat.wp,d1,s,pageno);
		if ((s = strtok((char *)0,"!")) != 0) {
			nParts++;
			mysprintf(_printwhat.wp,d2,s,pageno);
			if ((s = strtok((char *)0,"!")) != 0) {
				nParts++;
				mysprintf(_printwhat.wp,d3,s,pageno);
			}
		}
	}
	return nParts;
}

/*------------------------------------------------------------
 * print_headerOrFooter()
 */
static void print_headerOrFooter(HDC hdc, DEVEXTENTS *dep, int y, long pageno, 
				char *fmt, PRTPARAM *pp, BOOL bHeader)
{
	unsigned char 	b1[256],b2[256],b3[256];
	int  			numberOfSegments,a1,a2;
	int				align = pp->align;

	if (!(pp->options & PRTO_HEADERS)) {
		return;
	}
	if ((bHeader && !pp->header[0]) || (!bHeader && !pp->footer[0])) {
		return;
	}
	if (pp->options & PRTO_ULHEA) {
		if (bHeader) {
			print_drawLine(hdc, dep, dep->xLMargin, dep->xRMargin,
				dep->yHeaderPos, dep->yTop);
		} else {
			print_drawLine(hdc, dep, dep->xLMargin, dep->xRMargin,
				dep->yBottom, dep->yFooterPos);
		}
	}

	b1[0] = b2[0] = b3[0] = 0;
	numberOfSegments = print_formatheader(b1, b2, b3, fmt, pageno);

	/* swap headings on alternate pages */

	if (numberOfSegments == 1 && align == PRA_CENTER) {
		a1 = align;
	} else {
		if (align != PRA_CENTER && ((pp->options & PRTO_SWAP) && EVEN(pageno))) {
			a1 = PRA_LEFT, a2 = PRA_RIGHT;
		}
		else {
			a1 = PRA_RIGHT, a2 = PRA_LEFT;
		}
	}
	print_selectfont(hdc, &pp->htfont);
	if (bHeader) {
		y = (dep->yHeaderPos + dep->yTop - dep->lineHeight) / 2;
	} else {
		y = (dep->yFooterPos+dep->yBottom - dep->lineHeight)/2;
	}
	print_mkwpheader(hdc,y,dep,b1,a1);
	if (numberOfSegments > 1) {
		print_mkwpheader(hdc, y, dep, b2, PRA_CENTER);
		print_mkwpheader(hdc, y, dep, b3, a2);
	}
}

typedef struct tagPRINT_LINE {
	int wrappedLineOffset;	// normally 0. WHen a line is printed, which is wrapped print_signleLineOfText is called for every "segment line" with this being the
							// offset of the lines printed before.
	LINE* lp;				// current line to print.
	int charHeight;			// character / line height
	int firstc;				// first character index to print (mostly 0)
	int lastc;				// last character index to print (mostly lp->len)
	long lineNumber;		// number of line to print
	int xPos;				// x position, where the printing starts
	int yPos;				// y position, where the printing starts
	int maxYPos;			// the available space in y direction
} PRINT_LINE;

/*------------------------------------------------------------
 * print_singleLineOfText()
 */
static int print_singleLineOfText(HDC hdc, PRINT_LINE *pLine, BOOL printing)
{
	char 	szBuff[80];
	int		nCount;
	int		max;
	int		xPos = pLine->xPos;
	int		nMaxCharsPerLine;
	int		nActualLine = 0;
	int		nFirstActualLineToPrint = pLine->wrappedLineOffset;
	int		nLinesPrinted = 0;

	pLine->wrappedLineOffset = 0;
	nMaxCharsPerLine = 0;
	if (_prtparams.options & PRTO_LINES) {
		wsprintf(szBuff, "%3ld: ", pLine->lineNumber);
		nCount = lstrlen(szBuff);
		if (printing && nActualLine >= nFirstActualLineToPrint) {
			SetTextColor(hdc, DEFAULT_PRINT_COLOR);
			TextOut(hdc, pLine->xPos, pLine->yPos, szBuff, nCount);
		}
		xPos += LOWORD(win_getTextExtent(hdc, szBuff, nCount));
		nMaxCharsPerLine = -nCount;
	}

	_printwhat.wp->mincol = pLine->firstc;
	max = caret_lineOffset2screen(_printwhat.wp, &(CARET) { pLine->lp, pLine->lp->len});
	if (max > pLine->lastc) {
		max = pLine->lastc;
	}
	nMaxCharsPerLine += _prtparams.nchars - _prtparams.lmargin - _prtparams.rmargin;
	_printwhat.wp->maxcol = nMaxCharsPerLine;
	_printwhat.wp->maxcol = max;
	if (_printwhat.wp->maxcol > nMaxCharsPerLine) {
		_printwhat.wp->maxcol = nMaxCharsPerLine;
	}
	if (_printwhat.wp->maxcol > pLine->firstc) {
		if (nActualLine >= nFirstActualLineToPrint) {
			nLinesPrinted++;
			if (printing) {
				_printwhat.wp->renderFunction(hdc, xPos, pLine->yPos, _printwhat.wp, pLine->lp, pLine->lineNumber);
			}
		}
		while (_printwhat.wp->maxcol < max) {
			int delta = max - _printwhat.wp->maxcol;
			if (delta > nMaxCharsPerLine) {
				delta = nMaxCharsPerLine;
			}
			_printwhat.wp->mincol = _printwhat.wp->maxcol+1;
			_printwhat.wp->maxcol = _printwhat.wp->mincol+delta;
			if (pLine->yPos + (2*pLine->charHeight) > pLine->maxYPos) {
				pLine->wrappedLineOffset = nActualLine+1;
				break;
			}
			pLine->yPos += pLine->charHeight;
			nActualLine++;
			if (nActualLine >= nFirstActualLineToPrint) {
				nLinesPrinted++;
				if (printing) {
					_printwhat.wp->renderFunction(hdc, xPos, pLine->yPos, _printwhat.wp, pLine->lp, pLine->lineNumber);
				}
			}
		}
	}
	pLine->yPos += pLine->charHeight;
	return nLinesPrinted++;
}

/**
 * Checks, whether a printrange condition is met. Return 1 if the print range is "active", returns -1,
 * if further processing can be aborted because we are "beyond" the end of the print range.
 */
static int print_isInPrintRange(PRTPARAM *pParams, WINFO *wp, int nCurrentPage, int nCurrentLine) {
	PRINTRANGE* pRange = &pParams->printRange;
	if (pRange->prtr_type == PRTR_ALL) {
		return 1;
	}
	if (pRange->prtr_type == PRTR_PAGES) {
		if (pRange->prtr_toPage > 0 && nCurrentPage > pRange->prtr_toPage) {
			return -1;
		}
		return (nCurrentPage >= pRange->prtr_fromPage && nCurrentPage <= pRange->prtr_toPage);
	}
	if (pRange->prtr_type == PRTR_CURRENT_PAGE) {
		if (nCurrentLine > wp->maxln) {
			return -1;
		}
		return nCurrentLine + pParams->pagelen >= wp->minln && nCurrentLine <= wp->maxln;
	}
	if (pRange->prtr_type == PRTR_SELECTION) {
		// already handled in restricting the range to print.
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * PrintPage()

 * Print the current document / file. Returns the number of lines actually printed.
 */
static int print_file(HDC hdc, BOOL measureOnly)
{	char 		message[128];
	long		oldpageno,pageno;
	long		linesPrinted = 0;
	BOOL		printing = TRUE;
	BOOL		produceOutput;
	DEVEXTENTS	de;
	TEXTMETRIC	textMetrics;
	FTABLE*		fp = _printwhat.fp;
	LINE 		*lplast;
	PRTPARAM	*pp = &_prtparams;
	PRINT_LINE	printLineParam;

	lplast = _printwhat.lplast;

	print_getDeviceExtents(hdc, &de);
	print_selectfont(hdc,&pp->font);
	GetTextMetrics(hdc, &textMetrics);
	oldpageno = 0;
	pageno = 1;
	printLineParam.wrappedLineOffset = 0;
	printLineParam.yPos = 0;
	printLineParam.xPos = de.xLMargin;
	printLineParam.lp = _printwhat.lp;
	printLineParam.charHeight = de.lineHeight;
	printLineParam.firstc = _printwhat.firstColumn;
	printLineParam.lastc = _printwhat.lastColumn;
	printLineParam.lineNumber = 1;
	printLineParam.maxYPos = de.yBottom;
	while (printLineParam.lp) {
		if (oldpageno != pageno) {
			if (measureOnly) {
				printing = TRUE;
			} else {
				printing = print_isInPrintRange(pp, WIPOI(fp), pageno, printLineParam.lineNumber);
				if (printing < 0) {
					break;
				}
			}
			produceOutput = printing && !measureOnly;
			if (!PREVIEWING() && produceOutput) {
				wsprintf(message,/*STR*/"%s - %s (Seite %d)",
						(LPSTR)szAppName,
					  	(LPSTR)string_abbreviateFileName(ft_visiblename(fp)),
					  	pageno);
				progress_showMonitorMessage(message); 
			}
			oldpageno = pageno;
		}
		if (printLineParam.yPos == 0) {
			if (produceOutput) {
				StartPage(hdc);
				print_resetDeviceMode(hdc);
				print_headerOrFooter(hdc, &de, de.yHeaderPos, pageno,
					pp->header, pp, TRUE);
				print_selectfont(hdc,&pp->font);
			}
			printLineParam.yPos = de.yTop;
		}
		if (printLineParam.yPos+de.lineHeight > printLineParam.maxYPos) {
footerprint:
			if (produceOutput) {
				print_headerOrFooter(hdc,&de,de.yFooterPos,pageno,pp->footer, pp, FALSE);
				EndPage(hdc);
			}
			if (!printLineParam.lp || P_EQ(printLineParam.lp,lplast))
				break;
			printLineParam.yPos = 0;
			pageno++;
		} else {
			int nLinesPrinted = print_singleLineOfText(hdc, &printLineParam, produceOutput);
			linesPrinted += nLinesPrinted;
			if (printLineParam.wrappedLineOffset == 0) {
				if (P_EQ(printLineParam.lp, lplast) || (printLineParam.lp = printLineParam.lp->next) == 0)
					goto footerprint;
				printLineParam.lineNumber++;
			}
		}
	}
	return linesPrinted;
}

/*
 * Enable / disable the preview next/previous buttons. 
 */
static prt_enablePreviewButtons(HWND hDlg, int numberOfPreviewPages, int newPage) {
	_prtparams.printRange.prtr_fromPage = _prtparams.printRange.prtr_toPage = newPage;
	SetDlgItemInt(hDlg, IDD_INT1, newPage, 0);
	EnableWindow(GetDlgItem(hDlg, IDD_FIRST_PAGE), newPage > 1);
	EnableWindow(GetDlgItem(hDlg, IDD_BACK_PAGE), newPage > 1);
	int maxPages = numberOfPreviewPages;
	EnableWindow(GetDlgItem(hDlg, IDD_NEXT_PAGE), newPage < maxPages);
	EnableWindow(GetDlgItem(hDlg, IDD_LAST_PAGE), newPage < maxPages);
	win_sendRedrawToWindow(GetDlgItem(hDlg, IDD_PREVIEW));
}

/*--------------------------------------------------------------------------
 * DlgPreviewProc()
 */
static INT_PTR CALLBACK DlgPreviewProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	int			newPage = _prtparams.printRange.prtr_fromPage;
	static int  numberOfPreviewPages;

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
			numberOfPreviewPages = -1;
			_prtparams.printRange.prtr_fromPage = _prtparams.printRange.prtr_toPage = 1;
			_prtparams.printRange.prtr_type = PRTR_PAGES;
			SetDlgItemInt(hDlg, IDD_INT1, _prtparams.printRange.prtr_fromPage, 0);
			break;
		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDCANCEL:
					EndDialog(hDlg,1);
					break;
				case IDD_LAST_PAGE:
					newPage = numberOfPreviewPages; break;
				case IDD_FIRST_PAGE:
					newPage = 1; break;
				case IDD_NEXT_PAGE:
					if (newPage < numberOfPreviewPages) {
						newPage++;
					}
					break;
				case IDD_BACK_PAGE:
					if (newPage > 1) {
						newPage--;
					}
					break;
			}
			break;
		case WM_DRAWITEM:
			hdc = ((LPDRAWITEMSTRUCT)lParam)->hDC;
			int savedDc = SaveDC(hdc);
			RECT pageRect;
			DEVEXTENTS de;
			print_getDeviceExtents(hdc, &de);
			if (numberOfPreviewPages <= 0) {
				int nTotal = print_ntextlines(&de);
				numberOfPreviewPages = (print_file(hdc, TRUE) + nTotal - 1) / nTotal;
				if (numberOfPreviewPages <= 0) {
					numberOfPreviewPages = 1;
				}
				prt_enablePreviewButtons(hDlg, numberOfPreviewPages, newPage);
			}
			pageRect.top = 0;
			pageRect.bottom = de.yPage;
			pageRect.left = de.xLMargin;
			pageRect.right = de.xPage;
			HGDIOBJ original = SelectObject(hdc, GetStockObject(DC_PEN));
			SelectObject(hdc, GetStockObject(BLACK_PEN));
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SetDCBrushColor(hdc, RGB(255, 255, 255));
			Rectangle(hdc, 0, 0, pageRect.right, pageRect.bottom);
			SelectObject(hdc, original);
			print_file(hdc, FALSE);
			RestoreDC(hdc, savedDc);
			return TRUE;
		case WM_DESTROY:
			hwndPreview = 0;
			break;
		default:
			return FALSE;
	}
	if (newPage != _prtparams.printRange.prtr_fromPage) {
		prt_enablePreviewButtons(hDlg, numberOfPreviewPages, newPage);
	}
	return FALSE;
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
		size_t			pr_offs;
		char			*pr_name;
	} _pi[] = {
		1,	offsetof(PRTPARAM,header),			"header",
		1,	offsetof(PRTPARAM,footer),			"footer",
		0,	offsetof(PRTPARAM,mode),				"mode",
		0,	offsetof(PRTPARAM,options),			"options",
		0,	offsetof(PRTPARAM,pagelen),			"pagelen",
		1,	offsetof(PRTPARAM,font.fs_name),		"font",
		1,	offsetof(PRTPARAM,htfont.fs_name),		"htfont",
		0,	offsetof(PRTPARAM,lnspace.n),			"space_n",
		0,	offsetof(PRTPARAM,lnspace.z),			"space_z",
		0,	offsetof(PRTPARAM,lmargin),			"lmargin",
		0,	offsetof(PRTPARAM,rmargin),			"rmargin",
		0,	offsetof(PRTPARAM,nchars),			"chars",
		0,	offsetof(PRTPARAM,marginTop),			"dhtop",
		0,	offsetof(PRTPARAM,marginBottom),		"dhbot",
		0,	offsetof(PRTPARAM,headerSize),		"dftop",
		0,	offsetof(PRTPARAM,footerSize),		"dfbot",
		0,	offsetof(PRTPARAM,align),				"align"
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

/**
 * Save the config file with the printer layout.
 */
static void print_saveSettings() {
	print_readWriteConfigFile(1);
}

/*--------------------------------------------------------------------------
 * DlgPrint()
 */
static DIALPARS _dPrintLayout[] = {
	IDD_RADIO1,		PRA_RIGHT - PRA_LEFT,	&_prtparams.align,
	IDD_OPT1,			PRTO_SWAP,& _prtparams.options,
	IDD_OPT2,			PRTO_LINES,& _prtparams.options,
	IDD_OPT3,			PRTO_HEADERS,& _prtparams.options,
	IDD_OPT4,			PRTO_ULHEA,& _prtparams.options,
	IDD_INT1,			0,& _prtparams.pagelen,
	IDD_INT2,			0,& _prtparams.nchars,
	IDD_INT3,			0,& _prtparams.lmargin,
	IDD_INT4,			0,& _prtparams.rmargin,
	IDD_INT5,			0,& _prtparams.marginTop,
	IDD_INT6,			0,& _prtparams.marginBottom,
	IDD_INT7,			0,& _prtparams.headerSize,
	IDD_INT8,			0,& _prtparams.footerSize,
	IDD_STRING1,		sizeof _prtparams.header,	_prtparams.header,
	IDD_STRING2,		sizeof _prtparams.footer,	_prtparams.footer,
	IDD_CALLBACK1,		0,				print_saveSettings,
	IDD_FONTSELECT,	TRUE,				_prtparams.font.fs_name,
	IDD_FONTSELECT2,	TRUE,			_prtparams.htfont.fs_name,
	0
};
static DIALPARS* _getDialogParsForPage(int page) {
	if (page == 1) {
		return _dPrintLayout;
	}
	return NULL;
}
static HDC DlgPrint(char* title, PRTPARAM *pp, WINFO* wp) {
	DIALPARS* dp = _dPrintLayout;

	PROPSHEETPAGE psp[2];
	PROPSHEETHEADER psh;

	SetXDialogParams(_getDialogParsForPage, FALSE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[0]);
	psp[0].hInstance = hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(DLGPRINTERLAYOUT);
	psp[0].pfnDlgProc = DlgStdProc;

	psp[1].dwSize = sizeof(psp[1]);
	psp[1].hInstance = hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(DLGPREVIEW);
	psp[1].pfnDlgProc = DlgPreviewProc;

	LPPRINTDLGEXA prtDlg = calloc(1, sizeof *prtDlg);
	PRINTPAGERANGE pageRange;
	HPROPSHEETPAGE pPage1 = CreatePropertySheetPage(&psp[0]);
	HPROPSHEETPAGE pPage2 = CreatePropertySheetPage(&psp[1]);
	HPROPSHEETPAGE pages[2];
	pages[0] = pPage1;
	pages[1] = pPage2;
	pageRange.nFromPage = 1;
	pageRange.nToPage = 5;
	prtDlg->lStructSize = sizeof *prtDlg;
	prtDlg->hwndOwner = hwndMDIFrameWindow;
	prtDlg->lphPropertyPages = pages;
	prtDlg->Flags = PD_ALLPAGES | PD_RETURNDC | PD_PAGENUMS;
	if (wp == NULL || !ww_checkSelection(wp)) {
		prtDlg->Flags |= PD_NOSELECTION;
	}
	prtDlg->nPageRanges = 1;
	prtDlg->lpPageRanges = &pageRange;
	pageRange.nFromPage = 1;
	pageRange.nToPage = 999;
	prtDlg->nMinPage = 1;
	prtDlg->nMaxPage = 999;
	prtDlg->nCopies = 1;
	prtDlg->nMaxPageRanges = 1;
	prtDlg->nStartPage = START_PAGE_GENERAL;
	prtDlg->nPropertyPages = 2;
	int nRet = PrintDlgEx(prtDlg);
	if (prtDlg->hDevNames) {
		GlobalFree(prtDlg->hDevNames);
	}
	if (prtDlg->hDevMode) {
		GlobalFree(prtDlg->hDevMode);
	}
	if (nRet == S_OK) {
		if (prtDlg->dwResultAction != PD_RESULT_CANCEL) {
			_prtparams.printRange.prtr_fromPage = prtDlg->lpPageRanges[0].nFromPage;
			_prtparams.printRange.prtr_toPage = prtDlg->lpPageRanges[0].nToPage;
			_prtparams.printRange.prtr_type = PRTR_PAGES;
			if (prtDlg->Flags & PD_CURRENTPAGE) {
				_prtparams.printRange.prtr_type = PRTR_CURRENT_PAGE;
			} else if (prtDlg->Flags & PD_ALLPAGES) {
				_prtparams.printRange.prtr_type = PRTR_ALL;
			} else if (prtDlg->Flags & PD_SELECTION) {
				_prtparams.printRange.prtr_type = PRTR_SELECTION;
			}
		}
		HDC hdc = prtDlg->hDC;
		DWORD dwResult = prtDlg->dwResultAction;
		free(prtDlg);
		return dwResult == PD_RESULT_PRINT ? hdc : NULL;
	}
	free(prtDlg);
	if (nRet == E_INVALIDARG) {
		log_vsprintf("invalid arg");
	}
	if (nRet == E_POINTER) {
		log_vsprintf("invalid pointer");
	}
	if (nRet == E_HANDLE) {
		log_vsprintf("invalid handle");
	}
	return 0;
}


/*------------------------------------------------------------
 * EdPrint()
 */
int EdPrint(long what, long p1, LPSTR fname) {
	BOOL			bFileRead = FALSE;
	HDC 			hdcPrn;
	int 			ret = 0;
	int				nFunc = 0;
	int				errEscape;
	char 			message[128];
	WINFO			winfo;
	WINFO*			wp;
	FTABLE*			fp = NULL;
	FTABLE			_ft;
	PRTPARAM		*pp = &_prtparams;

	memset(&winfo,0,sizeof winfo);
	memset(&_ft,0,sizeof _ft);
	_printwhat.firstColumn = 0;
	_printwhat.lastColumn = 1024;
	wp = 0;
	if (what == PRT_CURRWI || what == PRT_CURRBLK) {
		wp = ww_getCurrentEditorWindow();
		if (wp) {
			fp = wp->fp;
		}
	}

	if (what == PRT_FILE || fp == NULL) {
		fp = &_ft;
		if (fname) {
			lstrcpy(fp->fname, fname);
		}
		if (!fsel_selectFileWithTitle(IDM_PRINTFILE, fp->fname, FALSE)) {
			return 0;
		}
		if (ft_readfileWithOptions(fp,fp->fname,0) == 0)
	 		return 0;
		bFileRead = TRUE;
		ft_connectViewWithFT(fp, &winfo);
		ww_applyDisplayProperties(&winfo);
		wp = &winfo;
	}

	if (what == PRT_CURRWI || what == PRT_FILE) {
		_printwhat.lp = fp->firstl;
		_printwhat.lplast = fp->lastl->prev;
		if (fp->documentDescriptor) {
			BOOL showLineNumbers = wp->dispmode & SHOWLINENUMBERS;
			if (showLineNumbers) {
				_prtparams.options |= PRTO_LINES;
			}
			else {
				_prtparams.options &= ~PRTO_LINES;
			}
		}
	}

	if ((_printwhat.fp = fp) == 0) {
		goto byebye;
	}
	_printwhat.nlines = 
		ln_cnt(_printwhat.lp,_printwhat.lplast);
	if (what == PRT_CURRBLK) {
		wsprintf(message, "%s-selection", string_getBaseFilename(ft_visiblename(fp)));
	} else {
		char* pBasename = string_getBaseFilename(ft_visiblename(fp));
		strcpy(message, pBasename);
	}

	memmove(&winfo, wp, sizeof winfo);
	_printwhat.wp = &winfo;
	hdcPrn = DlgPrint(message, pp, wp);
	if (hdcPrn) {
		if (pp->printRange.prtr_type == PRTR_SELECTION ||what == PRT_CURRBLK) {
			if (wp == NULL || !ww_checkSelectionWithError(wp)) {
				goto byebye;
			}
			if (ww_isColumnSelectionMode(wp)) {
				_printwhat.firstColumn = wp->blcol1;
				_printwhat.lastColumn = wp->blcol2;
			}
			_printwhat.lp = wp->blstart->lm;
			_printwhat.lplast = wp->blend->lm;
		}
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof docinfo);
		docinfo.cbSize = sizeof docinfo;
		docinfo.lpszDocName = message;
		docinfo.lpszDatatype = "Text";
		progress_startMonitor(IDS_ABRTPRINT);
		SetAbortProc(hdcPrn, (ABORTPROC)PrtAbortProc);
		if ((PREVIEWING() || (errEscape = StartDoc(hdcPrn, &docinfo))) >= 0 &&
			(errEscape = print_file(hdcPrn, FALSE)) >= 0) {
			if (!PREVIEWING()) {
				EndDoc(hdcPrn);
			}
		} else {
			EscapeError(errEscape);
		}
		progress_closeMonitor(0);
		DeleteDC(hdcPrn);
		if (errEscape)
			goto byebye;
		ret = 1;
	}
byebye:
	if (bFileRead) {
		ft_bufdestroy(fp);
	}
	return ret;
}


