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
#include "lineoperations.h"
#include "winfo.h"
#include "edfuncs.h"
#include "stringutil.h"
#include "markpositions.h"
#include "xdialog.h"
#include "pksrc.h"

extern char *ft_visiblename(FTABLE *fp);
/**
 * Calculate the byte offset of the current caret in a file.
 */
extern long wi_getCaretByteOffset(WINFO* view);

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
 * CurrentStringVal()
 */
static char *CurrentStringVal(FTABLE *fp, char **fmt, char *fname)
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
		return ft_visiblename(fp);

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
 * CurrentNumVal()
 */
static long CurrentNumVal(WINFO *wp, char **fmt) {
	char *	format;
	if (wp == 0) {
		return 0;
	}

	format = *fmt;

	switch(*format) {

	case 'O':
		return wi_getCaretByteOffset(wp);

	case 'C':
		return (long)((unsigned char)wp->caret.linePointer->lbuf[wp->caret.offset]);

	case 'l':
		return wp->caret.ln + 1L;

	case 'c':
		return wp->caret.col + 1L;

	case 'w':
		return wp->win_id;

	default:
		*fmt -= 2;

	}
	
	return 0L;	
}

/*--------------------------------------------------------------------------
 * mysprintf()
 
	One can append to %x formats the following $ regex_addCharacterToCharacterClass holders to format the corresponding value.
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
int mysprintf(WINFO *wp, char *d, char *format,...) {
	static char *_digits = "0123456789ABCDEFGHIJ";
	long 	val;
	int		c;
	int		l;
	int		base;
	int		f0;
	char *	buf = d;
	char *	x;
	char *	dend;
	va_list	args;
	char 	stack[512];
	char 	fname[512];

	dend = d + MAXSSIZE;
	va_start(args, format);

	while((c = *format++) != 0) {
		if (c != '%') *d++ = c;
		else {

			l  = 0;
			f0 = 0;
			if (*format == '0' || *format == ' ')
				f0 = *format++;

			while ((c = *format++) >= '0' && c <= '9') {
				l *= 10;
				l += (c - '0');
			}
			if (c >= 'b' && c <= 'r') { /* base > 2 !!	*/
				base = c - 'a' + 1;
				if (*format == '$') {
					format++;
					val = CurrentNumVal(wp, &format);
					format++;
				} else {
					val  = va_arg(args,long);
				}
				if (val < 0 && base == 10) {
					*d++ = '-';
					val  = -val;
				}
				x  = &stack[sizeof(stack)-1];
				*x = 0;
				do {
					*--x = _digits[val % base];
					val /= base;
				} while(val);
				goto cpyout;
			} else if (c == 's') {
				if (*format == '$') {
					format++;
					stack[0] = 0;
					x = CurrentStringVal(wp->fp, &format, fname);
					format++;
				} else {
					if (!_psenabled)
						break;
					x = va_arg(args,char *);
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
			} else *d++ = c;
		}
	}
	*d = 0;
	va_end(args);
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
	mysprintf(wp, pResult, pFormat);
}

/*--------------------------------------------------------------------------
 * EdFormatPrint()
 */
void EdFormatPrint(long dummy1, long dummy2, char *format, char *p)
{
	char buf[1024];

	if (format) {
		mysprintf(ww_getCurrentEditorWindow(), buf, format, p, 0L);
	} else {
		buf[0] = 0;
	}
	macro_returnString(buf);
}


