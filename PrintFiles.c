/*
 * PrintFiles.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: print files
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <windowsx.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

#include <string.h>
#include <stddef.h>

#include "pksedit.h"
#include "dial2.h"
#include "printing.h"
#include "xdialog.h"

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
	1,1,0,0,0,60, 		/* startpage,endpage,pageoffs,mode,options,pagelen */
	0,10,0,"Tms Roman",	/* Std Font: oemmode,cheight,cwidth,name */
	0,0,80,			/* lmargin,rmargin, nchars */
	5,1,1,			/* tabsize, lnspace */

	0,0,0,0,			/* distances header, footer */
	0,10,0,"Courier",	/* Header Font: oemmode,cheight,cwidth,name */
	"",				/* header, footer templates */
	"",
	0,				/* header, footer align */

	0,10,0,"Courier",	/* Footnote Font: oemmode,cheight,cwidth,name */

	1,1,				/* Line spacing footnotes */
	0,				/* length of fn delimter line */
	0,0,				/* distances to body and footer */
	0,				/* offset of Fnote */
};

extern char 		*ft_visiblename(FTABLE *fp);
extern char 		*AbbrevName(char *fn);
extern FTABLE		*ww_stackwi(int num);
extern long 		Atol(char *s);
extern long 		ln_cnt(LINE *lps,LINE *lpe);

static int 		_printing;
static int 		_previewpage = 1;
static HWND 		hwndPreview;
static PRINTWHAT	_printwhat;
static char 		szPrtDevice[64];

/*------------------------------------------------------------
 * GetPrinterDC()
 */
EXPORT HDC GetPrinterDC(void)
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
	ed_error(IDS_MSGNOPRINTER);
	return 0;
}

/*------------------------------------------------------------
 * InitPrinterDC()
 */
EXPORT void InitPrinterDC(void)
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

	ed_error(nId);
}

/*------------------------------------------------------------
 * PrtAbortProc()
 */
BOOL CALLBACK PrtAbortProc(HDC hdcPrn, short nCode)
{
	if (nCode < 0)
		EscapeError(nCode);
	return !ed_abort(FALSE);
}

/*------------------------------------------------------------
 * prt_selectfont()
 */
static HFONT prt_selectfont(HDC hdc, FONTSPEC *fsp)
{
	HFONT 		hFont,hOldfont;
	EDFONT		font;
	TEXTMETRIC 	tm;

	blfill(&font, sizeof font, 0);
	if (!PREVIEWING()) {
		lstrcpy(font.name, fsp->fs_name);
	}
	font.weight = FW_NORMAL;
	font.point = fsp->fs_cheight;
	font.width = fsp->fs_cwidth;
	font.charset = fsp->fs_oemmode ? OEM_CHARSET : ANSI_CHARSET;
	if ((hFont = EdCreateFont(&font)) != 0) {
		hOldfont = SelectObject(hdc, hFont);
		if (hOldfont) {
			DeleteObject(hOldfont);
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
 * Ratio()
 */
static int Ratio(int total, int mul, int div)
{
	if (div == 0)
		div = 1;
	return (total * (long)mul / (long)div);
}

/*------------------------------------------------------------
 * prt_ntextlines()
 */
static int prt_ntextlines(void)
{
	PRTPARAM	*pp = &_prtparams;

	return (pp->options & PRTO_HEADERS) ? 
			(pp->pagelen - 2 - pp->d1 - pp->d2 - pp->d3 - pp->d4) :
			pp->pagelen;
}

/*--------------------------------------------------------------------------
 * ResetDeviceMode()
 */
static void ResetDeviceMode(HDC hdc)
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
	SetWindowExt(hdc,14400,14400);
	SetViewportExt(hdc,x,y);

}

/*------------------------------------------------------------
 * GetDeviceExtents()
 */
static void GetDeviceExtents(HDC hdc, DEVEXTENTS *dep)
{
	PRTPARAM 	*pp = &_prtparams;
	DWORD	ext;
	int		textheight;

	ResetDeviceMode(hdc);
	ext = GetWindowExt(hdc);
	dep->xPage = LOWORD(ext);
	dep->yPage = HIWORD(ext);

	dep->xLMargin = Ratio(dep->xPage, pp->lmargin, pp->nchars);
	dep->xRMargin = Ratio(dep->xPage, (pp->nchars-pp->rmargin), pp->nchars);

	if (pp->options&PRTO_HEADERS) {
		dep->yHeaderPos = Ratio(dep->yPage,pp->d1,pp->pagelen);
		dep->yTop 	 = Ratio(dep->yPage,pp->d1+pp->d2+1,pp->pagelen);
		dep->yFooterPos = Ratio(dep->yPage,(pp->pagelen-pp->d4-1),
						    pp->pagelen);
		dep->yBottom    = Ratio(dep->yPage,(pp->pagelen-pp->d3-pp->d4-1),
				       	    pp->pagelen);
	} else {
		dep->yHeaderPos = dep->yTop = 0;
		dep->yFooterPos = dep->yBottom = dep->yPage;
	}

	textheight = (dep->yBottom-dep->yTop);
	dep->lnSpace = (textheight) / prt_ntextlines();

	pp->font.fs_cheight = 
	pp->htfont.fs_cheight = 
		 dep->lnSpace * pp->lnspace.n / pp-> lnspace.z;

	pp->font.  fs_cwidth =
	pp->htfont.fs_cwidth = 
		(dep->xPage) / pp->nchars;
}

/*---------------------------------*/
/* mkwpheader()				*/
/*---------------------------------*/
static void mkwpheader(HDC hdc, int yPos, DEVEXTENTS *dep,
				   char *szBuff, int align)
{
	int  		nLen,xPos,w;

	if ((nLen = lstrlen(szBuff)) == 0)
		return;
	w = LOWORD(GetTextExtent(hdc,szBuff,nLen));

	switch(align) {
		default:	xPos = dep->xLMargin; break;
		case 1:	xPos = (dep->xPage - w) / 2; break;
		case 2:	xPos = (dep->xRMargin - w); break;
	}
	TextOut(hdc,xPos,yPos,szBuff,nLen);
}

/*---------------------------------*/
/* formatheader()				*/
/*---------------------------------*/
static int formatheader(unsigned char *d1, unsigned char *d2, 
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
 * PrintHF()
 */
static void PrintHF(HDC hdc, DEVEXTENTS *dep, int y, long pageno, 
				char *fmt,int align)
{
	unsigned char 	b1[256],b2[256],b3[256];
	int  			wpflg,a1,a2;
	PRTPARAM		*pp = &_prtparams;

	if (!(pp->options & PRTO_HEADERS))
		return;

	b1[0] = b2[0] = b3[0] = 0;
	wpflg = formatheader(b1, b2, b3, fmt, pageno + pp->pageoffs);

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
	mkwpheader(hdc,y,dep,b1,a1);
	mkwpheader(hdc,y,dep,b2,1);
	mkwpheader(hdc,y,dep,b3,a2);
}

/*------------------------------------------------------------
 * Line()
 */
static void Line(HDC hdc, int xPos, int yPos, long lineno,
		 	  LINE *lp, int firstc, int lastc)
{
	char 	szBuff[80];
	int		nCount;
	int		max;
	int		nMaxCharsPerLine;

	nMaxCharsPerLine = 0;
	if (_prtparams.options & PRTO_LINES) {
		wsprintf(szBuff,"%-5ld: ",lineno);
		TextOut(hdc,xPos,yPos,szBuff,nCount = lstrlen(szBuff));
		xPos += LOWORD(GetTextExtent(hdc,szBuff,nCount));
		nMaxCharsPerLine = -7;
	}

	_printwhat.wp->mincol = firstc;
	max = _cphys2scr(_printwhat.fp,lp->lbuf,lp->len);
	if (max > lastc)
		max = lastc;
	nMaxCharsPerLine = _prtparams.nchars - _prtparams.lmargin - _prtparams.rmargin;
	_printwhat.wp->maxcol = nMaxCharsPerLine;
	if (max > nMaxCharsPerLine) {
		max = nMaxCharsPerLine;
	}
	if ((_printwhat.wp->maxcol = max) > firstc) {
		write_line(hdc, xPos, yPos, _printwhat.wp, lp);
	}
}

/*------------------------------------------------------------
 * DrawLine()
 */
static void DrawLine(HDC hdc, int x1, int x2, int y)
{
	MoveTo(hdc,x1,y);
	LineTo(hdc,x2,y);
}

/*------------------------------------------------------------
 * PrintPage()
 */
static int PrintFile(HDC hdc)
{	char 		message[128];
	long			oldpageno,lineno,pageno;
	int			yPos,ret = 1;
	DEVEXTENTS	de;
	LINE 		*lp,*lplast;
	PRTPARAM		*pp = &_prtparams;
	int			firstc,lastc;

	lp     = _printwhat.lp;
	lplast = _printwhat.lplast;
	firstc = _printwhat.firstc;
	lastc  = _printwhat.lastc;

	GetDeviceExtents(hdc, &de);
	prt_selectfont(hdc,&pp->font);

	oldpageno = 0;
	pageno = lineno = 1;
	yPos = 0;

	while (lp) {
		if (oldpageno != pageno) {
			if (PREVIEWING()) {
				if (pageno > _previewpage)
					break;
				_printing = (pageno == _previewpage);
			} else {
				_printing = (pageno >= pp->startpage && 
			       		   pageno <= pp->endpage);
			}
			if (!PREVIEWING() && _printing) {
				wsprintf(message,/*STR*/"%s - %s (Seite %d)",
						(LPSTR)szAppName,
					  	(LPSTR)AbbrevName(ft_visiblename(_currfile)),
					  	pageno);
				abrt_message(message); 
			}
			oldpageno = pageno;
		}
		if (yPos == 0) {
			if (_printing) {
				StartPage(hdc);
				ResetDeviceMode(hdc);
				prt_selectfont(hdc,&pp->htfont);
				PrintHF(hdc, &de, de.yHeaderPos, pageno,
					pp->header, pp->align);
				prt_selectfont(hdc,&pp->font);
				if (pp->options & PRTO_ULHEA) {
					DrawLine(hdc,de.xLMargin,de.xRMargin,
						   (de.yTop+de.yHeaderPos)/2);
				}
			}
			yPos = de.yTop;
		}
		if (yPos+de.lnSpace > de.yBottom) {
footerprint:
			if (_printing) {
				if (pp->options & PRTO_ULHEA) {
					DrawLine(hdc,de.xLMargin,de.xRMargin,
						    (de.yBottom+de.yFooterPos)/2);
				}
				prt_selectfont(hdc,&pp->htfont);
				PrintHF(hdc,&de,de.yFooterPos,pageno,pp->footer,pp->align);
				EndPage(hdc);
			}
			if (!lp || P_EQ(lp,lplast))
				break;
			yPos = 0;
			pageno++;
		} else {
			if (_printing) {
				Line(hdc,de.xLMargin,yPos,lineno,lp,firstc,lastc);
			}
			yPos += de.lnSpace;
			if (P_EQ(lp,lplast) || (lp = lp->next) == 0)
				goto footerprint;
			lineno++;
		}
	}
	EdUnselectFont(hdc);
	return ret;
}

/*--------------------------------------------------------------------------
 * DlgPreviewProc()
 */
BOOL CALLBACK DlgPreviewProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;

	switch(message) {
		case WM_INITDIALOG:
			hwndPreview = hDlg;
			form_move(hDlg);
			break;
		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDCANCEL:
					EndDialog(hDlg,1);
					break;
				case IDD_BUT3:
					_previewpage++; break;
				case IDD_BUT4:
					_previewpage--; break;
			}
			break;
		case WM_DRAWITEM:
			hdc = ((LPDRAWITEMSTRUCT)lParam)->hDC;
			PrintFile(hdc);
			return TRUE;
		case WM_DESTROY:
			hwndPreview = 0;
			break;
		default:
			return FALSE;
	}
	SetDlgItemInt(hDlg,IDD_INT1,_previewpage,0);
	EnableWindow(GetDlgItem(hDlg,IDD_BUT4),(_previewpage>1));
	EnableWindow(GetDlgItem(hDlg,IDD_BUT3),
		(_previewpage<=_printwhat.nlines/prt_ntextlines()));
	SendRedraw(GetDlgItem(hDlg,IDD_PREVIEW));
	return TRUE;
}

/*--------------------------------------------------------------------------
 * DlgInstallPrtProc()
 */
typedef VOID (FAR PASCAL *DEVMODEPROC)(HWND, HANDLE, LPSTR, LPSTR);
BOOL CALLBACK DlgInstallPrtProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND			hwndList;
	HANDLE		hLibrary;
	DEVMODEPROC	lpfnDM;
	char			szDevice[64],szDriver[256],
				szDriverName[64],*szOutput,*s;

	switch(message) {
		case WM_INITDIALOG:
			hwndList = GetDlgItem(hDlg,IDD_INSTPRINTERS);
			SendMessage(hwndList, LB_RESETCONTENT,0,0L);
			GetProfileString("devices", NULL, "", _linebuf, LINEBUFSIZE);
			s = _linebuf;
			while(*s) {
				SendMessage(hwndList,LB_ADDSTRING,0,(LONG)s);
				while(*s++)
					;
			}
			SendMessage(hwndList, LB_SELECTSTRING, -1, (LONG)szPrtDevice);
			form_move(hDlg);
			break;
		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDOK:
					LbGetText(hDlg,IDD_INSTPRINTERS,szPrtDevice);
				case IDCANCEL:
					EndDialog(hDlg,wParam == IDOK);
					break;
				case IDD_INSTINSTALL:
					LbGetText(hDlg,IDD_INSTPRINTERS,szDevice);
					GetProfileString("devices", szDevice,"",szDriver,
								   sizeof szDriver);
					if ((szOutput = strtok(szDriver,",")) != 0) {
						wsprintf(szDriverName,"%s.DRV",szDriver);
						if ((hLibrary = LoadLibrary(szDriverName)) >= 32) {
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
 * PrinterDefaults()
 */
void PrinterDefaults(int save)
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
		0,	offsetof(PRTPARAM,mode),				"mode",
		0,	offsetof(PRTPARAM,options),			"options",
		0,	offsetof(PRTPARAM,pagelen),			"pagelen",
		1,	offsetof(PRTPARAM,font.fs_name),		"font",
		1,	offsetof(PRTPARAM,htfont.fs_name),		"htfont",
		0,	offsetof(PRTPARAM,lnspace.n),			"space_n",
		0,	offsetof(PRTPARAM,lnspace.z),			"space_z",
		1,	offsetof(PRTPARAM,header),			"header",
		1,	offsetof(PRTPARAM,footer),			"footer",
		0,	offsetof(PRTPARAM,lmargin),			"lmargin",
		0,	offsetof(PRTPARAM,rmargin),			"rmargin",
		0,	offsetof(PRTPARAM,nchars),			"chars",
		0,	offsetof(PRTPARAM,d1),				"dhtop",
		0,	offsetof(PRTPARAM,d2),				"dhbot",
		0,	offsetof(PRTPARAM,d3),				"dftop",
		0,	offsetof(PRTPARAM,d4),				"dfbot",
		0,	offsetof(PRTPARAM,align),			"align"
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
			if (GetPksProfileString(prt,name,szBuff,sizeof szBuff)) {
				if (_pi[i].pr_isstring) {
					lstrcpy((LPSTR)loc,szBuff);
				}
				else {
					*(int*)loc = Atol(szBuff);
				}
			}
		}
	}

	if (!pp->lnspace.z)
		pp->lnspace.z = 1;
}

/*------------------------------------------------------------
 * EdPrint()
 */
int EdPrint(long what, long p1, LPSTR fname)
{
	HDC 			hdcPrn;
	int 			ret = 0,nFunc,errEscape;
	char 		message[128],ch;
	WINFO		winfo;
	FTABLE		*fp,_ft;
	PRTPARAM		*pp = &_prtparams;

	blfill(&winfo,sizeof winfo, 0);
	blfill(&_ft,sizeof _ft, 0);
	_printwhat.firstc = 0;
	_printwhat.lastc = 1024;

	fp = 0;

	if (what == PRT_CURRWI || what == PRT_CURRBLK) {
		if ((fp = ww_stackwi(0)) == 0)
			return 0;
	}

	if (what == PRT_FILE) {
		fp = &_ft;
		if (fname)
			lstrcpy(fp->fname,fname);
		else
			if (!txtfile_select(IDM_PRINTFILE,fp->fname))
				return 0;
		if (Readfile(fp,fp->fname,0) == 0)
	 		return 0;
		WIPOI(fp) = &winfo;
		FTPOI((&winfo)) = fp;
		ww_setwindowflags(fp,&winfo);
	}

	if (what == PRT_CURRWI || what == PRT_FILE) {
		_printwhat.lp = fp->firstl;
		_printwhat.lplast = fp->lastl->prev;
	} else {
		if (!chkblk(fp))
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
	wsprintf(message,"%c %s %c",ch,ft_visiblename(fp),ch);

again:
	memmove(&winfo,WIPOI(fp),sizeof winfo);
	_printwhat.wp = &winfo;
	lstrcpy(winfo.fnt_name,pp->font.fs_name);
	winfo.fnt.point = pp->font.fs_cheight;
	_previewpage = 0;
	switch(nFunc = DlgPrint(
			message,
			&pp->startpage,&pp->endpage,&pp->pageoffs)) {
		case IDD_BUT3:	
			_previewpage = 1; break;		/* Preview.. */
		default: 		goto byebye;
		case IDOK: break;				/* Print.. */
	}

	if (_previewpage > 0) {
		winfo.fnt_name[0] = 0;
		DoDialog(DLGPREVIEW,DlgPreviewProc,(LPSTR)0);	
	} else {
		if ((hdcPrn = GetPrinterDC()) != NULL) {
			DOCINFO		docinfo;

			memset(&docinfo, 0, sizeof docinfo);
			docinfo.cbSize = sizeof docinfo;
			docinfo.lpszDocName = message;
			docinfo.lpszDatatype = "Text";
			abrt_start(IDS_ABRTPRINT);
			SetAbortProc(hdcPrn, (LPSTR)PrtAbortProc);
			if ((PREVIEWING() || (errEscape = StartDoc(hdcPrn, &docinfo))) >= 0 &&
			    (errEscape = PrintFile(hdcPrn)) >= 0) {
				if (!PREVIEWING()) {
					EndDoc(hdcPrn);
				}
			} else {
				EscapeError(errEscape);
			}
		}
		if (hdcPrn) {
			abrt_close(0);
			DeleteDC(hdcPrn);
		}
	
		if (errEscape)
			goto byebye;
	}
	if (nFunc == IDD_BUT3)
		goto again;
	ret = 1;
byebye:
	if (what == PRT_FILE && fp)
		ft_bufdestroy(fp);
	return ret;
}

/*--------------------------------------------------------------------------
 * PrtInstall()
 */
static void PrtInstall(void)
{
	DoDialog(DLGPRTINSTALL,(FARPROC)DlgInstallPrtProc,(LPSTR)0);
}

/*--------------------------------------------------------------------------
 * EdPrinterLayout()
 */
int EdPrinterLayout(void)
{
	static EDFONT	font;
	static EDFONT	htfont;
	static int 	hfalign;
	static int	options;
	static int	nchars;
	static int	lmargin;
	static int	rmargin;
	static int	pagelen;
	static DIALPARS _d[] = {
		IDD_RADIO1,		PRA_RIGHT-PRA_LEFT,	&hfalign,
		IDD_OPT1,			PRTO_SWAP,		&options,
		IDD_OPT2,			PRTO_LINES,		&options,
		IDD_OPT3,			PRTO_HEADERS,		&options,
		IDD_OPT4,			PRTO_ULHEA,		&options,
		IDD_INT1,			0,				&pagelen,
		IDD_INT2,			0,				&nchars,
		IDD_INT3,			0,				&lmargin,
		IDD_INT4,			0,				&rmargin,
		IDD_FONTSELECT,	TRUE,			&font,
		IDD_FONTSELECT2,	TRUE,			&htfont,
		IDD_CALLBACK1,		0,				PrtInstall,
		0
	};

	hfalign = _prtparams.align;
	options = _prtparams.options;
	nchars = _prtparams.nchars;
	pagelen = _prtparams.pagelen;
	lmargin = _prtparams.lmargin;
	rmargin = _prtparams.rmargin;
	lstrcpy(font.name, _prtparams.font.fs_name);
	lstrcpy(htfont.name, _prtparams.htfont.fs_name);

	if (DoDialog(DLGPRINTERLAYOUT, (FARPROC)DlgStdProc, _d) == IDCANCEL) {
		return 0;
	}
	PrinterDefaults(1);
	_prtparams.align = hfalign;
	_prtparams.options = options;
	_prtparams.nchars = nchars;
	_prtparams.pagelen = pagelen;
	_prtparams.lmargin = lmargin;
	_prtparams.rmargin = rmargin;

	lstrcpy(_prtparams.font.fs_name, font.name);
	lstrcpy(_prtparams.htfont.fs_name, htfont.name);

	return TRUE;
}
