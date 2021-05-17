/*
 * StatusLine.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: statusline window
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>
#include <commctrl.h>
#include "trace.h"
#include "winterf.h"
#include "lineoperations.h"
#include "winfo.h"
#include "edierror.h"
#include "editorconfiguration.h"

#define	MAXSEGMENTS			20

extern int 		mysprintf(FTABLE *fp, char *d, char *fmt, ...);
extern char *	lstrchr(const char *, const char);
extern unsigned char *stralloc(unsigned char *buf);
static char	*	pszStatusMessage;
static BOOL		bSimpleMode;
HWND			hwndStatus;

/*------------------------------------------------------------
 * st_format()
 */
static int st_format(char *dest)
{
	int 	len;
	WINFO *	wp;

	*dest = 0;
	if (!_currfile) {
		return 0;
	} else {
		wp = WIPOI(_currfile);
	}

	len = mysprintf(_currfile, dest,
				 (wp && wp->statusline && (wp->dispmode & SHOWHEX)) ? 
				 wp->statusline : 
				 /*STR*/"Ln %2j$l, Col%2j$c!%s$&");
	return len;
}

/*------------------------------------------------------------
 * st_setparts()
 */
static void st_setparts(char *text, BOOL bUpdateMessageOnly)
{
	INT					pSegments[MAXSEGMENTS];
	LPSTR				pszStrArr[MAXSEGMENTS];
	LPSTR				s;
	LPSTR				pszEnd;
	int					i;
	int					offset;
	int					nSegments;

	if (!hwndStatus) {
		return;
	}
	if (bSimpleMode) {
		nSegments = 0;
	} else {
		for (nSegments = 0, s = text; ;) {
			if (nSegments >= MAXSEGMENTS-2) {
				break;
			}
			if ((pszEnd = lstrchr(s, '!')) != 0) {
				*pszEnd++ = 0;
			}
			pszStrArr[nSegments++] = s;
			if ((s = pszEnd) == 0) {
				break;
			}
		}
	}
	if (nSegments != SendMessage(hwndStatus, SB_GETPARTS, 0, 0)) {
		for (i = 0, offset = 100; i < nSegments; i++) {
			pSegments[i] = offset;
			offset += 100;
		}
		pSegments[nSegments] = -1;
		SendMessage(hwndStatus, SB_SETPARTS, nSegments + 1, (LPARAM)pSegments);
	}
	if (!bUpdateMessageOnly) {
		for (i = 0; i < nSegments; i++) {
			SendMessage(hwndStatus, SB_SETTEXT, i, (LPARAM)pszStrArr[i]);
		}
	}
	SendMessage(hwndStatus, SB_SETTEXT, nSegments | SBT_NOBORDERS, 
			(LPARAM)(pszStatusMessage ? pszStatusMessage : ""));
}


void st_redraw(BOOL bUpdateMessageOnly)
{	static char	szBuf[1024];

	st_format(szBuf);
	st_setparts(szBuf, bUpdateMessageOnly);
}

/*--------------------------------------------------------------------------
 * st_seterrmsg()
 */
void st_seterrmsg(char *msg)
{
	if (pszStatusMessage) {
		_free(pszStatusMessage);
		pszStatusMessage = 0;
	} else if (!msg) {
		return;
	}

	if (msg) {
		pszStatusMessage = stralloc(msg);
	}
	st_redraw(TRUE);
}

/*------------------------------------------------------------
 * status_wh()
 */
void status_wh(WORD *width, WORD *height)
{
	RECT		rect;

	if (hwndStatus == 0 || (GetConfiguration()->layoutoptions & OL_SHOWSTATUS) == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	GetWindowRect(hwndStatus, &rect);
	*width = (WORD)(rect.right - rect.left) + 1;
	*height = (WORD)(rect.bottom - rect.top) + 1;
}

/*--------------------------------------------------------------------------
 * st_init()
 */
void st_init(HWND hwndDaddy)
{
	if (!hwndStatus) {
		InitCommonControls();
		hwndStatus = CreateStatusWindow(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			"", hwndDaddy, (UINT)-1);
	}
}

/*--------------------------------------------------------------------------
 * st_update()
 */
void st_update(void)
{
	st_redraw(FALSE);
}

void st_switchtomenumode(BOOL bMenuMode)
{
	bSimpleMode = bMenuMode;
}
