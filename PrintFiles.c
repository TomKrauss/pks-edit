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
#include "themes.h"
#include "funcdef.h"

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

typedef struct tagPRINT_SCOPE {
	FTABLE	*fp;
	WINFO	*wp;
	LINE 	*lp,*lplast;
	PRINTRANGE printRange;
	int		firstColumn,lastColumn,nlines;
} PRINT_SCOPE;

extern char 		*ft_visibleName(FTABLE *fp);

static HWND 		hwndPreview;
static PRINT_SCOPE	_currentPrintScope;

/*------------------------------------------------------------
 * print_getPrinterDC()
 */
EXPORT HDC print_getPrinterDC(void)
{
	char szPrinter[256];
	DWORD dwPrinterNameSize;
	HDC hdc;

	dwPrinterNameSize = sizeof szPrinter;
	if (GetDefaultPrinter(szPrinter, &dwPrinterNameSize) &&
		(hdc = CreateDC(NULL, szPrinter, NULL, NULL)) != 0) {
	    return hdc;
	}
	error_showErrorById(IDS_MSGNOPRINTER);
	return 0;
}

/* 
 * Paint the various decoration types for headers and footers
 */
static void print_decoration(HDC hdc, PAGE_DECORATION_TYPE pdType, RECT* pRect, int nDelta, COLORREF cColor) {
	if (pdType == PDT_NONE) {
		return;
	}
	if (pdType == PDT_FILL_RECT) {
		HBRUSH hBrush = CreateSolidBrush(cColor);
		FillRect(hdc, pRect, hBrush);
		DeleteObject(hBrush);
	}
	else {
		HPEN grayPen = CreatePen(PS_SOLID, nDelta, cColor);
		HPEN hpenOld = SelectObject(hdc, grayPen);
		if (pdType == PDT_LINE) {
			int y = (pRect->bottom + pRect->top) / 2;
			MoveTo(hdc, pRect->left, y);
			LineTo(hdc, pRect->right, y);
		}
		else {
			Rectangle(hdc, pRect->left, pRect->top, pRect->right, pRect->bottom);
		}
		SelectObject(hdc, hpenOld);
		DeleteObject(grayPen);

	}

}
/*------------------------------------------------------------
 * print_drawHeaderDecoration()
 * Draw the decoration for the header / and optionally footer printed.
 */
static void print_drawHeaderDecoration(HDC hdc, DEVEXTENTS* dep, PAGE_DECORATION_TYPE pdType, BOOL bHeader, int x1, int x2) {
	/// Should be made configurable somehow.
	if (pdType == PDT_NONE) {
		return;
	}
	int y;
	int y2;

	if (bHeader) {
		y = dep->yHeaderPos;
		y2 = dep->yTop;
	} else {
		y = dep->yBottom;
		y2 = dep->yFooterPos;
	}
	int delta = dep->lineHeight / 4;
	RECT rect;
	rect.left = x1;
	rect.right = x2;
	rect.top = y + delta;
	rect.bottom = y2 - delta;
	if (pdType == PDT_LINE) {
		if (bHeader) {
			rect.top = rect.bottom - 2;
		} else {
			rect.bottom = rect.top + 2;
		}
	}
	COLORREF cColor = RGB(220, 220, 220);
	print_decoration(hdc, pdType, &rect, delta, cColor);
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
	HFONT 		hFont;
	TEXTMETRIC 	tm;

	if (!fsp->fs_name[0]) {
		strcpy(fsp->fs_name, "Consolas");
	}
	if ((hFont = font_createFontHandle(fsp->fs_name, fsp->fs_cheight, fsp->fs_oemmode, FW_NORMAL)) != 0) {
		static HFONT previousFont = NULL;
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
		_currentPrintScope.wp->cwidth  = tm.tmAveCharWidth;
		_currentPrintScope.wp->cheight = tm.tmHeight;
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
 * Returns the number of content lines printed on one page (excluding header and footer).
 */
static int print_ntextlines(DEVEXTENTS* dep) {
	PRTPARAM	*pp = config_getPrintConfiguration();

	int n =(dep->yFooterPos - dep->yHeaderPos) / pp->font.fs_cheight;
	n -= pp->header.pme_lines;
	n -= pp->footer.pme_lines;
	return n;
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
	PRTPARAM 	*pp = config_getPrintConfiguration();
	DWORD	ext;
	int		totalHeight;

	print_resetDeviceMode(hdc);
	ext = win_getWindowExtension(hdc);
	dep->xPage = LOWORD(ext);
	dep->yPage = HIWORD(ext);

	dep->xLMargin = print_calculateRatio(dep->xPage, pp->lmargin, pp->nchars);
	dep->xRMargin = print_calculateRatio(dep->xPage, (pp->nchars-pp->rmargin), pp->nchars);

	totalHeight = dep->yPage;
	dep->lineHeight = totalHeight / pp->pagelen;

	int cFontHeight = dep->lineHeight * pp->lnspace.numerator / pp->lnspace.denominator;
	pp->font.fs_cheight =
		pp->header.pme_font.fs_cheight = 
		pp->footer.pme_font.fs_cheight = cFontHeight;

	pp->font.fs_cwidth =
		pp->header.pme_font.fs_cwidth =
		pp->footer.pme_font.fs_cwidth =
		(dep->xPage / (pp->nchars + pp->lmargin + pp->rmargin)) * pp->nchars / 120;

	dep->yHeaderPos = pp->header.pme_margin * cFontHeight;
	dep->yFooterPos = dep->yPage - pp->footer.pme_margin * cFontHeight;
	if (pp->options & PRTO_HEADERS) {
		dep->yTop = dep->yHeaderPos+pp->header.pme_lines*cFontHeight;
		dep->yBottom = dep->yFooterPos - pp->footer.pme_lines * cFontHeight;
	} else {
		dep->yTop = dep->yHeaderPos;
		dep->yBottom = dep->yFooterPos;
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

	if ((nLen = (int)strlen(szBuff)) == 0)
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
				    long pageno) {
	unsigned char  buf[512];
	int			nParts = 0;

	size_t nLen = strlen(s);
	if (nLen >= sizeof buf) {
		nLen = sizeof buf-1;
	}
	strncpy(buf, s, nLen);
	buf[nLen] = 0;
	nParts = 1;
	// TODO: allow using character ! in headers and footers.
	if ((s = strtok(buf,"!")) != 0) {
		mysprintf(_currentPrintScope.wp,d1,s,pageno);
		if ((s = strtok((char *)0,"!")) != 0) {
			nParts++;
			mysprintf(_currentPrintScope.wp,d2,s,pageno);
			if ((s = strtok((char *)0,"!")) != 0) {
				nParts++;
				mysprintf(_currentPrintScope.wp,d3,s,pageno);
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
	int				align;

	if (!(pp->options & PRTO_HEADERS)) {
		return;
	}
	PAGE_MARGIN_ELEMENT* pElement = bHeader ? &pp->header : &pp->footer;
	if (!pElement->pme_template[0]) {
		return;
	}
	align = pElement->pme_align;
	print_drawHeaderDecoration(hdc, dep, pElement->pme_decoration, bHeader, dep->xLMargin, dep->xRMargin);

	b1[0] = b2[0] = b3[0] = 0;
	numberOfSegments = print_formatheader(b1, b2, b3, fmt, pageno);

	/* swap headings on alternate pages */

	if (numberOfSegments == 1 && align == PRA_CENTER) {
		a1 = align;
	} else {
		if (align != PRA_CENTER && ((pp->options & PRTO_SWAP_HEADER_FOOTER_ALIGNMENT) && EVEN(pageno))) {
			a1 = PRA_LEFT, a2 = PRA_RIGHT;
		}
		else {
			a1 = PRA_RIGHT, a2 = PRA_LEFT;
		}
	}
	print_selectfont(hdc, &pElement->pme_font);
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
static int print_singleLineOfText(HDC hdc, PRINT_LINE *pLine, BOOL printing) {
	PRTPARAM* pParams = config_getPrintConfiguration();
	char 	szBuff[80];
	int		nCount;
	int		max;
	int		xPos = pLine->xPos;
	int		nMaxCharsPerLine;
	int		nActualLine = 0;
	int		nFirstActualLineToPrint = pLine->wrappedLineOffset;
	int		nLinesPrinted = 1;

	int nDCId = SaveDC(hdc);
	pLine->wrappedLineOffset = 0;
	nMaxCharsPerLine = 0;
	if (pParams->options & PRTO_LINE_NUMBERS) {
		wsprintf(szBuff, "%3ld: ", pLine->lineNumber);
		nCount = (int)strlen(szBuff);
		if (printing && nActualLine >= nFirstActualLineToPrint) {
			SetTextColor(hdc, DEFAULT_PRINT_COLOR);
			TextOut(hdc, pLine->xPos, pLine->yPos, szBuff, nCount);
		}
		xPos += LOWORD(win_getTextExtent(hdc, szBuff, nCount));
		nMaxCharsPerLine = -nCount;
	}
	_currentPrintScope.wp->mincol = pLine->firstc;
	max = caret_lineOffset2screen(_currentPrintScope.wp, &(CARET) {.linePointer = pLine->lp, .offset = pLine->lp->len});
	if (max > pLine->lastc) {
		max = pLine->lastc;
	}
	nMaxCharsPerLine += pParams->nchars - pParams->lmargin - pParams->rmargin;
	_currentPrintScope.wp->maxcol = max;
	if (_currentPrintScope.wp->maxcol > nMaxCharsPerLine) {
		_currentPrintScope.wp->maxcol = nMaxCharsPerLine;
	}
	RENDER_CONTEXT rc;
	rc.rc_hdc = hdc;
	rc.rc_wp = _currentPrintScope.wp;
	rc.rc_printing = TRUE;
	rc.rc_theme = theme_getDefault();
	RENDER_LINE_FUNCTION pRender = _currentPrintScope.wp->renderer->r_renderLine;
	if (_currentPrintScope.wp->maxcol > pLine->firstc) {
		if (nActualLine >= nFirstActualLineToPrint) {
			if (printing) {
				pRender(&rc, xPos, pLine->yPos, pLine->lp, pLine->lineNumber);
			}
		}
		while (_currentPrintScope.wp->maxcol < max) {
			if ((pParams->options & PRTO_WRAP_LONG_LINES) == 0) {
				break;
			}
			int delta = max - _currentPrintScope.wp->maxcol;
			if (delta > nMaxCharsPerLine) {
				delta = nMaxCharsPerLine;
			}
			_currentPrintScope.wp->mincol = _currentPrintScope.wp->maxcol+1;
			_currentPrintScope.wp->maxcol = _currentPrintScope.wp->mincol+delta;
			if (pLine->yPos + (2*pLine->charHeight) > pLine->maxYPos) {
				pLine->wrappedLineOffset = nActualLine+1;
				break;
			}
			pLine->yPos += pLine->charHeight;
			nActualLine++;
			if (nActualLine >= nFirstActualLineToPrint) {
				nLinesPrinted++;
				if (printing) {
					ExcludeClipRect(hdc, 0, pLine->yPos, xPos, pLine->yPos + pLine->charHeight);
					pRender(&rc, xPos, pLine->yPos, pLine->lp, pLine->lineNumber);
				}
			}
		}
	}
	pLine->yPos += pLine->charHeight;
	RestoreDC(hdc, nDCId);
	return nLinesPrinted;
}

/**
 * Checks, whether a printrange condition is met. Return 1 if the print range is "active", returns -1,
 * if further processing can be aborted because we are "beyond" the end of the print range.
 */
static int print_isInPrintRange(PRTPARAM *pParams, WINFO *wp, int nCurrentPage, int nCurrentLine) {
	PRINTRANGE* pRange = &_currentPrintScope.printRange;
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
	FTABLE*		fp = _currentPrintScope.fp;
	LINE 		*lplast;
	PRTPARAM	*pp = config_getPrintConfiguration();
	PRINT_LINE	printLineParam;

	lplast = _currentPrintScope.lplast;

	print_getDeviceExtents(hdc, &de);
	print_selectfont(hdc,&pp->font);
	GetTextMetrics(hdc, &textMetrics);
	oldpageno = 0;
	pageno = 1;
	printLineParam.wrappedLineOffset = 0;
	printLineParam.yPos = 0;
	printLineParam.xPos = de.xLMargin;
	printLineParam.lp = _currentPrintScope.lp;
	printLineParam.charHeight = de.lineHeight;
	printLineParam.firstc = _currentPrintScope.firstColumn;
	printLineParam.lastc = _currentPrintScope.lastColumn;
	printLineParam.lineNumber = 1;
	printLineParam.maxYPos = de.yBottom;
	WINFO* wp = WIPOI(fp);
	if ((pp->options & PRTO_SYNTAX_HIGHLIGHT) == 0) {
		_currentPrintScope.wp->dispmode &= ~SHOW_SYNTAX_HIGHLIGHT;
	} else {
		_currentPrintScope.wp->dispmode |= SHOW_SYNTAX_HIGHLIGHT;
	}
	while (printLineParam.lp) {
		if (oldpageno != pageno) {
			if (measureOnly) {
				printing = TRUE;
			} else {
				printing = print_isInPrintRange(pp, wp, pageno, printLineParam.lineNumber);
				if (printing < 0) {
					break;
				}
			}
			produceOutput = printing && !measureOnly;
			if (!PREVIEWING() && produceOutput) {
				wsprintf(message,/*STR*/"%s - %s (Seite %ld)",
						(LPSTR)szAppName,
					  	(LPSTR)string_abbreviateFileName(ft_visibleName(fp)),
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
					pp->header.pme_template, pp, TRUE);
				print_selectfont(hdc,&pp->font);
			}
			printLineParam.yPos = de.yTop;
		}
		if (printLineParam.yPos+de.lineHeight > printLineParam.maxYPos) {
footerprint:
			if (produceOutput) {
				print_headerOrFooter(hdc,&de,de.yFooterPos,pageno,pp->footer.pme_template, pp, FALSE);
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
	PRTPARAM* pParams = config_getPrintConfiguration();
	_currentPrintScope.printRange.prtr_fromPage = _currentPrintScope.printRange.prtr_toPage = newPage;
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
	int			newPage = _currentPrintScope.printRange.prtr_fromPage;
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
			_currentPrintScope.printRange.prtr_fromPage = _currentPrintScope.printRange.prtr_toPage = 1;
			_currentPrintScope.printRange.prtr_type = PRTR_PAGES;
			SetDlgItemInt(hDlg, IDD_INT1, _currentPrintScope.printRange.prtr_fromPage, 0);
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
		case WM_MOUSEWHEEL: {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (zDelta > 0 && newPage < numberOfPreviewPages) {
				newPage++;
			}
			if (zDelta < 0 && newPage > 1) {
				newPage--;
			}
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
			pageRect.right = de.xPage;
			HPEN hPen = CreatePen(0, 1, theme_getDefault()->th_dialogBorder);
			HGDIOBJ original = SelectObject(hdc, hPen);
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SetDCBrushColor(hdc, RGB(255, 255, 255));
			Rectangle(hdc, 0, pageRect.top, pageRect.right, pageRect.bottom);
			DeleteObject(SelectObject(hdc, original));
			print_file(hdc, FALSE);
			RestoreDC(hdc, savedDc);
			return TRUE;
		case WM_DESTROY:
			hwndPreview = 0;
			break;
		default:
			return FALSE;
	}
	if (newPage != _currentPrintScope.printRange.prtr_fromPage) {
		prt_enablePreviewButtons(hDlg, numberOfPreviewPages, newPage);
	}
	return FALSE;
}

/**
 * Save the config file with the printer layout.
 */
static void print_saveSettings() {
	prof_save(FALSE);
}

static void prt_decolboxfill(HWND hwnd, int nItem, void* selValue) {
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT, 0, 0L);
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, FALSE, 0L);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)PDT_NONE);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)PDT_LINE);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)PDT_FRAME);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)PDT_FILL_RECT);
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	PAGE_DECORATION_TYPE pType = (LPARAM) * (PAGE_DECORATION_TYPE*)selValue;
	SendDlgItemMessage(hwnd, nItem, CB_SETCURSEL, pType, 0);
}

static prt_decodraw(HDC hdc, RECT* rcp, void *pParam, int nItem, int nCtl) {
	COLORREF cColor = RGB(50, 50, 100);
	PAGE_DECORATION_TYPE pType = (PAGE_DECORATION_TYPE)pParam;
	RECT rect = *rcp;
	InflateRect(&rect, -2, -2);
	print_decoration(hdc, pType, &rect, 2, cColor);
}

/*
 * Measure a list box painting a page header decoration type.
 */
void prt_decomeasure(MEASUREITEMSTRUCT* mp) {
	mp->itemHeight = 20;
}

/*--------------------------------------------------------------------------
 * dlg_getListboxText()
 */
int prt_decoget(HWND hwnd, int id, void* pDecoType) {
	LRESULT nIdx = SendDlgItemMessage(hwnd, id, CB_GETCURSEL, 0, 0);
	if (nIdx == LB_ERR) {
		return 0;
	}
	**((PAGE_DECORATION_TYPE**) pDecoType) = (PAGE_DECORATION_TYPE)nIdx;
	return 1;
}

/*--------------------------------------------------------------------------
 * DlgPrint()
 */
static DIALPARS* _dPrintLayoutParams;
static DIALPARS* _getDialogParsForPage(int page) {
	if (page == 1) {
		return _dPrintLayoutParams;
	}
	return NULL;
}
static HDC DlgPrint(char* title, PRTPARAM *pp, WINFO* wp) {
	PRTPARAM* pParams = config_getPrintConfiguration();
	DIALPARS _dPrintLayout[] = {
		IDD_ICONLIST,	0,	0,
		IDD_ICONLIST2,	0,	0,
		IDD_RADIO1,		PRA_RIGHT - PRA_LEFT,	&pParams->header.pme_align,
		IDD_RADIO4,		PRA_RIGHT - PRA_LEFT,& pParams->footer.pme_align,
		IDD_OPT1,			PRTO_SWAP_HEADER_FOOTER_ALIGNMENT,& pParams->options,
		IDD_OPT2,			PRTO_LINE_NUMBERS,& pParams->options,
		IDD_OPT3,			PRTO_HEADERS,& pParams->options,
		IDD_OPT5,			PRTO_WRAP_LONG_LINES,& pParams->options,
		IDD_OPT6,			PRTO_SYNTAX_HIGHLIGHT,& pParams->options,
		IDD_INT1,			0,& pParams->pagelen,
		IDD_INT2,			0,& pParams->nchars,
		IDD_INT3,			0,& pParams->lmargin,
		IDD_INT4,			0,& pParams->rmargin,
		IDD_INT5,			0,& pParams->header.pme_margin,
		IDD_INT6,			0,& pParams->footer.pme_margin,
		IDD_INT7,			0,& pParams->header.pme_lines,
		IDD_INT8,			0,& pParams->footer.pme_lines,
		IDD_STRING1,		sizeof pParams->header,	pParams->header.pme_template,
		IDD_STRING2,		sizeof pParams->footer,	pParams->footer.pme_template,
		IDD_CALLBACK1,		0,				print_saveSettings,
		IDD_FONTSELECT,		TRUE,			pParams->font.fs_name,
		IDD_FONTSELECT2,	TRUE,			pParams->header.pme_font.fs_name,
		IDD_FONTSELECT3,	TRUE,			pParams->footer.pme_font.fs_name,
		0
	};
	_dPrintLayoutParams = _dPrintLayout;
	PAGE_DECORATION_TYPE* p1 = &pp->header.pme_decoration;
	PAGE_DECORATION_TYPE* p2 = &pp->footer.pme_decoration;
	DIALLIST dlist1 = {
		(long long*)&p1,
		prt_decolboxfill,
		prt_decoget,
		prt_decomeasure,
		prt_decodraw,
		0
	};
	DIALLIST dlist2 = dlist1;
	dlist2.li_param = (long long*) & p2;
	_dPrintLayout[0].dp_data = &dlist1;
	_dPrintLayout[1].dp_data = &dlist2;

	PROPSHEETPAGE psp[2];
	PROPSHEETHEADER psh;

	dlg_setXDialogParams(_getDialogParsForPage, FALSE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[0]);
	psp[0].hInstance = ui_getResourceModule();
	psp[0].pszTemplate = MAKEINTRESOURCE(DLGPRINTERLAYOUT);
	psp[0].pfnDlgProc = dlg_standardDialogProcedure;

	psp[1].dwSize = sizeof(psp[1]);
	psp[1].hInstance = ui_getResourceModule();
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
	prtDlg->hwndOwner = hwndMain;
	prtDlg->lphPropertyPages = pages;
	prtDlg->Flags = PD_ALLPAGES | PD_RETURNDC | PD_PAGENUMS;
	if (wp == NULL || !ww_hasSelection(wp)) {
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
	prtDlg->lpCallback = NULL;
	dlg_disableDarkHandling = TRUE;
	int nRet = PrintDlgEx(prtDlg);
	dlg_disableDarkHandling = FALSE;
	if (prtDlg->hDevNames) {
		GlobalFree(prtDlg->hDevNames);
	}
	if (prtDlg->hDevMode) {
		GlobalFree(prtDlg->hDevMode);
	}
	if (nRet == S_OK) {
		if (prtDlg->dwResultAction != PD_RESULT_CANCEL) {
			_currentPrintScope.printRange.prtr_fromPage = prtDlg->lpPageRanges[0].nFromPage;
			_currentPrintScope.printRange.prtr_toPage = prtDlg->lpPageRanges[0].nToPage;
			_currentPrintScope.printRange.prtr_type = PRTR_PAGES;
			if (prtDlg->Flags & PD_CURRENTPAGE) {
				_currentPrintScope.printRange.prtr_type = PRTR_CURRENT_PAGE;
			} else if (prtDlg->Flags & PD_ALLPAGES) {
				_currentPrintScope.printRange.prtr_type = PRTR_ALL;
			} else if (prtDlg->Flags & PD_SELECTION) {
				_currentPrintScope.printRange.prtr_type = PRTR_SELECTION;
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
	PRTPARAM		*pp = config_getPrintConfiguration();

	memset(&winfo,0,sizeof winfo);
	memset(&_ft,0,sizeof _ft);
	_currentPrintScope.firstColumn = 0;
	_currentPrintScope.lastColumn = 1024;
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
		if (!fsel_selectFileWithTitle(CMD_PRINT_FILE, fp->fname, FALSE)) {
			return 0;
		}
		FILE_READ_OPTIONS fro;
		memset(&fro, 0, sizeof fro);
		fro.fro_fileName = fp->fname;
		fro.fro_useDefaultDocDescriptor = 0;
		if (ft_readfileWithOptions(fp, &fro) == 0)
	 		return 0;
		bFileRead = TRUE;
		ft_connectViewWithFT(fp, &winfo);
		ww_applyDisplayProperties(&winfo);
		wp = &winfo;
	}

	if (what == PRT_CURRWI || what == PRT_FILE) {
		_currentPrintScope.lp = fp->firstl;
		_currentPrintScope.lplast = fp->lastl->prev;
		if (fp->documentDescriptor) {
			BOOL showLineNumbers = wp->dispmode & SHOWLINENUMBERS;
			if (showLineNumbers) {
				pp->options |= PRTO_LINE_NUMBERS;
			}
			else {
				pp->options &= ~PRTO_LINE_NUMBERS;
			}
		}
	}

	if ((_currentPrintScope.fp = fp) == 0) {
		goto byebye;
	}
	_currentPrintScope.nlines = 
		ln_cnt(_currentPrintScope.lp,_currentPrintScope.lplast);
	if (what == PRT_CURRBLK) {
		wsprintf(message, "%s-selection", string_getBaseFilename(ft_visibleName(fp)));
	} else {
		char* pBasename = string_getBaseFilename(ft_visibleName(fp));
		strcpy(message, pBasename);
	}

	if (wp) {
		memmove(&winfo, wp, sizeof winfo);
	}
	_currentPrintScope.wp = &winfo;
	hdcPrn = DlgPrint(message, pp, wp);
	if (hdcPrn) {
		if (_currentPrintScope.printRange.prtr_type == PRTR_SELECTION ||what == PRT_CURRBLK) {
			if (wp == NULL || !ww_checkSelectionWithError(wp)) {
				goto byebye;
			}
			if (ww_isColumnSelectionMode(wp)) {
				_currentPrintScope.firstColumn = wp->blcol1;
				_currentPrintScope.lastColumn = wp->blcol2;
			}
			_currentPrintScope.lp = wp->blstart->m_linePointer;
			_currentPrintScope.lplast = wp->blend->m_linePointer;
		}
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof docinfo);
		docinfo.cbSize = sizeof docinfo;
		docinfo.lpszDocName = message;
		docinfo.lpszDatatype = "Text";
		progress_startMonitor(IDS_ABRTPRINT);
		SetAbortProc(hdcPrn, (ABORTPROC)PrtAbortProc);
		if ((PREVIEWING() || (errEscape = StartDoc(hdcPrn, &docinfo)) > 0) &&
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


