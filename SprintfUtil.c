/*
 * SprintfUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
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

#include "winterf.h"
#include "edierror.h"
#include "documentmodel.h"
#include "winfo.h"
#include "pksmacro.h"
#include "stringutil.h"
#include "markpositions.h"
#include "xdialog.h"
#include "pksrc.h"

extern char *ft_visibleName(FTABLE *fp);

int  _psenabled = 1;

#define MAXSSIZE		256

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
*/
int mysprintf(char *d, char *format, SPRINTF_ARGS* pArgs) {
	static char *_digits = "0123456789ABCDEFGHIJ";
	long 	val;
	int     bHexPrefix;
	int		c;
	int		l;
	int		base;
	int		f0;
	char *	buf = d;
	char *	x;
	char *	dend;
	char* formatStart = 0;
	WINFO* wp = pArgs->sa_wp;
	union U_ARG_VALUE* args = pArgs->sa_values;
	char 	stack[512];
	char 	fname[512];

	dend = d + MAXSSIZE;

	while((c = *format++) != 0) {
		if (c != '%') *d++ = c;
		else {
			formatStart = format - 1;
			l  = 0;
			f0 = 0;
			bHexPrefix = 0;
			if (*format == '#') {
				bHexPrefix = 1;
				format++;
			}
			if (*format == '0' || *format == ' ') {
				f0 = *format++;
			}
			while ((c = *format++) >= '0' && c <= '9') {
				l *= 10;
				l += (c - '0');
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
				if (val < 0 && base > 1) {
					*d++ = '-';
					val = -val;
				}
				x = &stack[sizeof(stack) - 1];
				*x = 0;
				if (c == 'c') {
					*--x = (char)val;
					goto cpyout;
				}
				do {
					*--x = _digits[val % base];
					val /= base;
				} while (val);
				if (bHexPrefix) {
					*--x = 'X';
					*--x = '0';
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
cpyout:			if (d + l > dend)
					break;
				if (f0) {
					l -= lstrlen(x);
					if (l > 0) {
						memset(d,f0,l);
						d += l;
					}
					l  = 0;
					while(*x)
						*d++ = *x++;
				} else {
					while(*x) {
						l--;
						*d++ = *x++;
					}
					if (l > 0) {
						d = memset(d,' ', l);
						d += l;
					}
				}
			} else if (c == 'T') {
				struct tm *tm;
				time_t ltime;
				time(&ltime);
				tm = localtime(&ltime);
			 	agettime(stack, tm);
cp2:			x = stack;
				goto cpyout;
			} else if (c == 'D') {
				struct tm *tm;
				time_t ltime;
				time(&ltime);
				tm = localtime(&ltime);
				agetdate(stack, tm);
				goto cp2;
			} else if (sprintf_isFloatFormat(c) || c == '.') {
				double dNumber = args->v_d;
				if (args->v_d) {
					args++;
				}
				format = sprintf_float(fname, formatStart, dNumber);
				l = (int)strlen(fname);
				if (d + l > dend) {
					break;
				}
				strcpy(d, fname);
				d += l;
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
 * Return a PKS EDIT variable to be used e.g. in code templates.
 */
void string_getVariable(WINFO* wp, unsigned char* pVar, unsigned char* pResult) {
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
	if (strcmp("user", pVar) == 0) {
		DWORD nBytes = 128;
		GetUserName(pResult, &nBytes);
		return;
	}
	unsigned char* pFormat = "%s$F";
	if (strcmp("file_name", pVar) == 0) {
		pFormat = "%s$f";
	} else if (strcmp("date", pVar) == 0) {
		pFormat = "%D";
	} else {
		strcpy(pResult, pVar);
		return;
	}
	mysprintf(pResult, pFormat, &(SPRINTF_ARGS){.sa_wp = wp});
}

