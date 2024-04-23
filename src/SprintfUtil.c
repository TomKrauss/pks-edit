/*
 * SprintfUtil.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: special sprintf, printing status infos,...
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <windows.h>
#include <stdarg.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <direct.h> 

#include "winterf.h"
#include "edierror.h"
#include "documentmodel.h"
#include "winfo.h"
#include "pksmacro.h"
#include "stringutil.h"
#include "markpositions.h"
#include "xdialog.h"
#include "pksrc.h"
#include "fileutil.h"
#include "editorconfiguration.h"

extern char *ft_visibleName(FTABLE *fp);
extern char* PKS_SYS;

int  _psenabled = 1;

#define MAXSSIZE		256

char _pksVersion[] = "2.4.0";

static char sDate[2], sTime[2];
static int  iDate;

/*--------------------------------------------------------------------------
 * string_initDateformats()
 * Initialize the PKS Edit date format.
 */
void string_initDateformats(void)
{	static char *cGroup = "intl";

	GetProfileString(cGroup, "sDate", "/", sDate, sizeof sDate);
	GetProfileString(cGroup, "sTime", ":", sTime, sizeof sTime);
	iDate = GetProfileInt(cGroup, "iDate", 0);

#if 0		/* sorry: unsupported.... */
	iDate = GetProfileInt(cGroup, "iTime", 0);
	GetProfileString(cGroup, "s1159", "AM", sAMPM[0], sizeof sAMPM[0]);
	GetProfileString(cGroup, "s2359", "PM", sAMPM[1], sizeof sAMPM[1]);
#endif
}

/*--------------------------------------------------------------------------
 * agettime(date)()
 */
static void agettime(char *buf, struct tm *tp)
{
	char lim = sTime[0];

	if (tp)
	wsprintf(buf,"%02d%c%02d%c%02d",
		tp->tm_hour,lim,tp->tm_min,lim,tp->tm_sec);
}

/*--------------------------------------------------------------------------
 * agetdate(date)()
 */
static void agetdate(char *buf, struct tm *tp)
{
	char lim = sDate[0];

	if (tp)
	wsprintf(buf,"%02d%c%02d%c%02d",
		iDate == 1 ? tp->tm_mday : iDate == 2 ? tp->tm_year : tp->tm_mon,lim,
		iDate == 0 ? tp->tm_mday : tp->tm_mon,lim,
		iDate == 2 ? tp->tm_mday : (tp->tm_year+1900));
}

/*--------------------------------------------------------------------------
 * string_formatDate()
 */
void string_formatDate(char *szDbuf, EDTIME *ltime)
{	char 		szBtime[64];
	char		szBdate[64];
	struct tm *	tp;

	if (*ltime == 0 || (tp = localtime(ltime)) == 0) {
		lstrcpy(szDbuf,"NEVER");
	} else {
		agetdate(szBdate, tp);
		agettime(szBtime, tp);
		wsprintf(szDbuf,"%s   %s",szBdate,szBtime);
	}
}

/*--------------------------------------------------------------------------
 * string_evaluatePrintfReference()
 */
static char *string_evaluatePrintfReference(FTABLE *fp, char **fmt, char *fname)
{
	char *	format;
	char *	ext;

	if (fp == 0) {
		return "";
	}

	format = *fmt;

	switch(*format) {

	case 'f':
		return string_getBaseFilename(fp->fname);

	case 'b':
		lstrcpy(fname,string_getBaseFilename(fp->fname));
		if ((ext = strrchr(fname,'.')) != 0) {
			*ext = 0;
		}
		return fname;

	case 'C':
		agetdate(fname, localtime(&fp->ti_created));
		return fname;

	case 'c':
		agettime(fname, localtime(&fp->ti_created));
		return fname;

	case 'M':
		agetdate(fname, localtime(&fp->ti_modified));
		return fname;

	case 'm':
		agettime(fname, localtime(&fp->ti_modified));
		return fname;

	case 'S':
		agetdate(fname, localtime(&fp->ti_saved));
		return fname;

	case 's':
		agettime(fname, localtime(&fp->ti_saved));
		return fname;

	case 'F':
		return ft_visibleName(fp);

	case '&':
		if (ft_isFileModified(fp))
			return dlg_getResourceString(IDS_FILE_MODIFIED);
blanks:	return "              ";
		break;

	case '*':
		if (ft_isFileModified(fp))
			return "*";
		break;

	case 'r':
		if (ft_isReadonly(fp))
			return dlg_getResourceString(IDS_FILE_READONLY);
		goto blanks;

	}

	return "";
}

/*--------------------------------------------------------------------------
 * sprintf_getValueFromWindow()
 */
static long sprintf_getValueFromWindow(WINFO *wp, char **fmt) {
	INTERNAL_BUFFER_POS pos;
	char *	format;
	if (wp == 0) {
		return 0;
	}

	format = *fmt;

	switch(*format) {

	case 'O':
		if (wp && wp->renderer->r_screenToBuffer(wp, wp->caret.ln, wp->caret.col, &pos)) {
			return pos.ibp_byteOffset;
		}
		return 0;

	case 'C': {
		if (wp && wp->renderer->r_screenToBuffer(wp, wp->caret.ln, wp->caret.col, &pos)) {
			LINE* lp = pos.ibp_lp;
			int nOffs = pos.ibp_lineOffset;
			if (nOffs >= lp->len) {
				if (nOffs == lp->len && LINE_HAS_CR(lp)) {
					return '\r';
				}
				if (LINE_HAS_LINE_END(lp) && nOffs <= lp->len+1) {
					return '\n';
				}
				return 0;
			}
			return (long)((unsigned char)lp->lbuf[nOffs]);
		}
		return 0;
	}

	case 'l':
		return wp ? wp->caret.ln + 1L : 0;

	case 'c':
		return wp ? wp->caret.col + 1L : 0;

	case 'w':
		return wp ? wp->win_id : 0;

	default:
		*fmt -= 2;

	}
	
	return 0L;
}

static int sprintf_isFloatFormat(char c) {
	return (c == 'a' || c >= 'e' && c <= 'g') || (c == 'A' || c >= 'E' && c <= 'G');
}

/*
 * Delegate float formatting to sprintf.
 */
static char* sprintf_float(char* pszDest, char* fmt, double nNumber) {
	char szFloatFormat[32];
	char* pszD = szFloatFormat;
	while (*fmt && pszD < szFloatFormat + sizeof(szFloatFormat) - 1) {
		char c = *fmt++;
		*pszD++ = c;
		if (sprintf_isFloatFormat(c)) {
			break;
		}
	}
	*pszD = 0;
	sprintf(pszDest, szFloatFormat, nNumber);
	return fmt;
}

/*
 * Format a string into an output buffer 
 */
static char* string_formatWithPadding(char* pszDestination, char* pszEnd, char* pszText, int nWidth, int cFiller, int bLeftJustify) {
	if (pszDestination + nWidth > pszEnd)
		return pszDestination;
	if (pszText) {
		if (!cFiller) {
			cFiller = ' ';
		}
		size_t nLen = strlen(pszText);
		nWidth -= (int)nLen;
		if (bLeftJustify) {
			strcpy(pszDestination, pszText);
			pszDestination += nLen;
			if (nWidth > 0) {
				pszDestination = memset(pszDestination, cFiller, nWidth);
				pszDestination += nWidth;
			}
		} else {
			if (nWidth > 0) {
				memset(pszDestination, cFiller, nWidth);
				pszDestination += nWidth;
			}
			strcpy(pszDestination, pszText);
			pszDestination += nLen;
		}
	}
	return pszDestination;
}

static void string_formatSystemTime(char* pszDest, SYSTEMTIME* pTime, char c) {

	if (c == 'D') {
		char lim = sDate[0];
		sprintf(pszDest, "%02d%c%02d%c%02d",
			iDate == 1 ? pTime->wDay : iDate == 2 ? pTime->wYear : pTime->wMonth, lim,
			iDate == 0 ? pTime->wDay : pTime->wMonth, lim,
			iDate == 2 ? pTime->wDay : pTime->wYear);
	} else {
		char lim = sTime[0];
		sprintf(pszDest, "%02d%c%02d%c%02d",
			pTime->wHour, lim, pTime->wMinute, lim, pTime->wSecond);
		if (c == 't') {
			pszDest += strlen(pszDest);
			sprintf(pszDest, ".%d", pTime->wMilliseconds);
		}
	}
}

/*--------------------------------------------------------------------------
 * mysprintf()
 
	One can append to %x formats the following $ place holders to format the corresponding value.
	One sample would be %s$f - prints the file name of a file in string format.

	$O		fileoffset
	$C		current character
	$l		current line
	$c		current col
	$f		Filename
	$F		full path
	$b		Filename without extension
	$w		Window Handle
	$h		Verweispfad - Hilfe
	$t		Verweispfad
	$*		geändert : "*"
	$r		read only: "

	Special % formats supported:
	%D		current date
	%T		current time
	%t		current time including milliseconds
*/
int mysprintf(char *d, char *format, SPRINTF_ARGS* pArgs) {
	static char *_digits = "0123456789ABCDEFGHIJ";
	long 	val;
	int     bForcePlus;
	int     bHexPrefix;
	int		c;
	int		nWidth;
	int		base;
	int		cFiller;
	int     bLeftJustify;
	char *	buf = d;
	char *	x;
	char *	dend;
	WINFO* wp = pArgs->sa_wp;
	union U_ARG_VALUE* args = pArgs->sa_values;
	char 	stack[512];
	char 	fname[512];

	dend = d + MAXSSIZE;

	while((c = *format++) != 0) {
		if (c != '%') *d++ = c;
		else {
			bForcePlus = 0;
			bLeftJustify = 0;
			char* formatStart = format - 1;
			nWidth  = 0;
			cFiller = 0;
			bHexPrefix = 0;
			if (*format == '-') {
				bLeftJustify = 1;
				format++;
			}
			if (*format == '+') {
				bForcePlus = 1;
				format++;
			}
			if (*format == '#') {
				bHexPrefix = 1;
				format++;
			}
			if (*format == '0' || *format == ' ') {
				cFiller = *format++;
			}
			while ((c = *format++) >= '0' && c <= '9') {
				nWidth *= 10;
				nWidth += (c - '0');
			}
			base = -1;
			if (c == 'd') {
				base = 10;
			}
			else if (c == 'x') {
				base = 16;
			}
			else if (c == 'b') {
				base = 2;
			}
			else if (c == 'o') {
				base = 8;
			}
			else if (c == 'c') {
				base = 1;
			}
			if (base > 0) {
				if (*format == '$') {
					format++;
					val = sprintf_getValueFromWindow(wp, &format);
					format++;
				}
				else {
					val = args->v_l;
					if (val) {
						args++;
					}
				}
				x = &stack[sizeof(stack) - 1];
				*x = 0;
				if (c == 'c') {
					*--x = (char)val;
					goto cpyout;
				}
				BOOL bNegative = 0;
				if (val < 0 && base > 1) {
					val = -val;
					bNegative = 1;
				}
				do {
					*--x = _digits[val % base];
					val /= base;
				} while (val);
				if (bHexPrefix) {
					*--x = 'X';
					*--x = '0';
				}
				if (bNegative) {
					*--x = '-';
				} else if (bForcePlus) {
					*--x = '+';
				}
				goto cpyout;
			} else if (c == 's') {
				if (*format == '$') {
					format++;
					stack[0] = 0;
					x = string_evaluatePrintfReference(wp->fp, &format, fname);
					format++;
				} else {
					if (!_psenabled)
						break;
					x = args->v_s;
					if (x) {
						args++;
					}
				}
cpyout:			d = string_formatWithPadding(d, dend, x, nWidth, cFiller, bLeftJustify);
			} else if (c == 'T' || c == 'D' || c == 't') {
				SYSTEMTIME st;
				GetLocalTime(&st);
				string_formatSystemTime(stack, &st, c);
				x = stack;
				goto cpyout;
			} else if (sprintf_isFloatFormat(c) || c == '.') {
				double dNumber = args->v_d;
				if (args->v_d) {
					args++;
				}
				format = sprintf_float(fname, formatStart, dNumber);
				nWidth = (int)strlen(fname);
				if (d + nWidth > dend) {
					break;
				}
				strcpy(d, fname);
				d += nWidth;
			}
			else {
				*d++ = c;
			}
		}
	}
	*d = 0;
	return (int)(d-buf);
}

/*
 * Return a PKS EDIT variable to be used e.g. in code templates or to be used in 
 * PKSMacroC scripts.
 */
void string_getVariable(WINFO* wp, const char* pVar, unsigned char* pResult, size_t nSize) {
	if (strcmp("year2", pVar) == 0) {
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf(pResult, "%d", (int)(time.wYear % 100));
		return;
	}
	if (strcmp("year", pVar) == 0) {
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf(pResult, "%d", (int)time.wYear);
		return;
	}
	if (strcmp("cwd", pVar) == 0) {
		_getcwd(pResult, (int)nSize);
		return;
	}
	if (strcmp("pks_tmp", pVar) == 0) {
		strcpy(pResult, GetConfiguration()->pksEditTempPath);
		return;
	}
	if (_strcmpi(PKS_SYS, pVar) == 0) {
		strcpy(pResult, _pksSysFolder);
		return;
	}
	if (strcmp("pks_version", pVar) == 0) {
		strcpy(pResult, _pksVersion);
		return;
	}
	if (strcmp("pks_executable", pVar) == 0) {
		DWORD dLen = GetModuleFileName(NULL, pResult, (DWORD)nSize);
		pResult[dLen] = 0;
		return;
	}
	if (strcmp("user", pVar) == 0) {
		DWORD nBytes = (DWORD)nSize;
		GetUserName(pResult, &nBytes);
		return;
	}
	unsigned char* pFormat = "%s$F";
	if (strcmp("file_name", pVar) == 0) {
		pFormat = "%s$f";
	} else if (strcmp("date", pVar) == 0) {
		pFormat = "%D";
	} else if (strcmp("time", pVar) == 0) {
		pFormat = "%T";
	}
	else if (strcmp("time_long", pVar) == 0) {
		pFormat = "%t";
	} else {
		strcpy(pResult, pVar);
		return;
	}
	mysprintf(pResult, pFormat, &(SPRINTF_ARGS){.sa_wp = wp});
}

/*
 * Get a variable and return the value of it in a buffer. The buffer will be valid until
 * the next invocation of this method.
 */
char* string_getVariableWithDefaults(const char* pVar) {
	static char szResult[256];
	string_getVariable(ww_getCurrentEditorWindow(), pVar, szResult, sizeof szResult);
	return szResult;
}
