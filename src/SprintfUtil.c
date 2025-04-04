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
#include "edctype.h"
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

char _pksVersion[] = "2.11.0";

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
	sprintf(buf,"%02d%c%02d%c%02d",
		tp->tm_hour,lim,tp->tm_min,lim,tp->tm_sec);
}

/*--------------------------------------------------------------------------
 * agetdate(date)()
 */
static void agetdate(char *buf, struct tm *tp)
{
	char lim = sDate[0];

	if (tp)
	sprintf(buf,"%02d%c%02d%c%02d",
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
		sprintf(szDbuf,"%s   %s",szBdate,szBtime);
	}
}

static void printVCStatus(char* pszDest, VC_INFO* pInfo, char* pszFilename) {
	VC_STATUS status = gi_getStatus(pInfo, pszFilename);
	if (status == VCS_NONE) {
		strcpy(pszDest, " ");
		return;
	}
	gi_printHash(pInfo, pszDest, 12);
	char* pszInfo = "";
	switch (status) {
	case VCS_MODIFIED: pszInfo = "modified"; break;
	case VCS_CURRENT: pszInfo = "current"; break;
	case VCS_CONFLICTED: pszInfo = "conflicts"; break;
	case VCS_DELETED: pszInfo = "deleted"; break;
	case VCS_IGNORED: pszInfo = "ignored"; break;
	case VCS_NEW: pszInfo = "new"; break;
	}
	strcat(pszDest, " - ");
	strcat(pszDest, pszInfo);
}

/*--------------------------------------------------------------------------
 * string_evaluatePrintfReference()
 */
static char *string_evaluatePrintfReference(FTABLE *fp, char **fmt, char *fname)
{
	char *	format;
	char *	ext;
	CPINFOEX cpinfo;

	if (fp == 0) {
		return "";
	}

	format = *fmt;

	switch(*format) {
	case 'P': {
		char* pName = ft_magicName(fp->codepageInfo.cpi_bomType);
		if (pName != NULL && pName[0]) {
			strcpy(fname, pName);
		}
		else {
			GetCPInfoEx(fp->codepageInfo.cpi_codepage, 0, &cpinfo);
			strcpy(fname, cpinfo.CodePageName);
		}
		return fname;
	}

	case 'f':
		return string_getBaseFilename(fp->fname);

	case 'b':
		lstrcpy(fname,string_getBaseFilename(fp->fname));
		if ((ext = strrchr(fname,'.')) != 0) {
			*ext = 0;
		}
		return fname;

	case 'V':
		printVCStatus(fname, fp->vcInfo, fp->fname);
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

	SCREEN_OFFSET_TO_BUFFER_FUNCTION pFunction = wp->renderer->r_screenToBuffer;
	switch(*format) {

	case 'O':
		if (wp && pFunction && pFunction(wp, &pos)) {
			return pos.ibp_byteOffset;
		}
		return 0;

	case 'C': {
		if (wp && pFunction && pFunction(wp, &pos)) {
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

	case 'o':
		return wp ? wp->caret.offset + 1L : 0;

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
static char* string_formatWithPadding(char* pszDestination, const char* pszEnd, char* pszText, int nWidth, int cFiller, int bLeftJustify) {
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
	$*		ge�ndert : "*"
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
 * Convert a string "in-place" to upper or lower case depending on the 'bUpper' flag.
 */
void string_convertUpperLower(char* pszString, BOOL bUpper) {
	char c;
	while ((c = *pszString) != 0) {
		if (bUpper) {
			c = _l2uset[c];
		} else {
			c = _u2lset[c];
		}
		*pszString++ = c;
	}
}

/*
 * Convert a string "in-place" to camel case. "hello world" => "HelloWorld".
 */
void string_convertCamelCase(char* pszString) {
	BOOL bUp = TRUE;
	char c;
	char* pszDest = pszString;
	while ((c = *pszString++) != 0) {
		if (c == ' ' || c == '_' || c == '-') {
			bUp = TRUE;
			continue;
		}
		if (bUp) {
			c = _l2uset[c];
			bUp = FALSE;
		} else {
			c = _u2lset[c];
		}
		*pszDest++ = c;
	}
	*pszDest = 0;
}

/*
 * Return a PKS Edit variable to be used e.g. in code templates or to be used in 
 * PKSMacroC scripts.
 */
void string_getVariable(WINFO* wp, const char* pVar, unsigned char* pResult, size_t nSize) {
	char szVar[100];
	char* pFunction = NULL;
	if (strlen(pVar) < sizeof szVar) {
		strcpy(szVar, pVar);
		pVar = szVar;
		pFunction = strchr(pVar, '.');
		if (pFunction) {
			*pFunction++ = 0;
		}
	}
	time_t rawtime;
	BOOL bTime = FALSE;
	FTABLE* fp = FTPOI(wp);
	if (strcmp(pVar, "file_modified") == 0) {
		rawtime = fp->ti_modified;
		bTime = TRUE;
	}
	if (strcmp(pVar, "file_created") == 0) {
		rawtime = fp->ti_created;
		bTime = TRUE;
	}
	if (strcmp(pVar, "file_saved") == 0) {
		rawtime = fp->ti_saved;
		bTime = TRUE;
	}
	if (strcmp(pVar, "today") == 0) {
		time(&rawtime);
		bTime = TRUE;
	}
	if (bTime) {
		struct tm* tp;
		if ((tp = localtime(&rawtime)) == 0) {
			return;
		}
		if (pFunction) {
			if (strcmp("year2", pFunction) == 0) {
				sprintf(pResult, "%d", (int)(tp->tm_year % 100));
				return;
			}
			if (strcmp("year", pFunction) == 0) {
				sprintf(pResult, "%d", (int)tp->tm_year + 1900);
				return;
			}
			if (strcmp("month", pFunction) == 0) {
				strftime(pResult, nSize, "%m", tp);
				return;
			}
			if (strcmp("month_name", pFunction) == 0) {
				strftime(pResult, nSize, "%B", tp);
				return;
			}
			if (strcmp("month_abbr", pFunction) == 0) {
				strftime(pResult, nSize, "%B", tp);
				return;
			}
			if (strcmp("date", pFunction) == 0) {
				agetdate(pResult, tp);
				return;
			}
			if (strcmp("time", pFunction) == 0) {
				agettime(pResult, tp);
				return;
			}
		}
		strftime(pResult, nSize, "%c", tp);
		return;
	}
	if (strcmp("cwd", pVar) == 0) {
		if (_getcwd(pResult, (int)nSize) == NULL) {
			*pResult = 0;
		}
	} else if (strcmp("pks_tmp", pVar) == 0) {
		strcpy(pResult, GetConfiguration()->pksEditTempPath);
	} else if (_strcmpi(PKS_SYS, pVar) == 0) {
		strcpy(pResult, _pksSysFolder);
	} else if (strcmp("pks_version", pVar) == 0) {
		strcpy(pResult, _pksVersion);
	} else if (strcmp("pks_executable", pVar) == 0) {
		DWORD dLen = GetModuleFileName(NULL, pResult, (DWORD)nSize);
		pResult[dLen] = 0;
	} else if (strcmp("user", pVar) == 0) {
		DWORD nBytes = (DWORD)nSize;
		GetUserName(pResult, &nBytes);
	} else {
		unsigned char* pFormat = "%s$F";
		if (strcmp("file_name", pVar) == 0) {
			pFormat = "%s$f";
		}
		else if (strcmp("file_name_no_suffix", pVar) == 0) {
			pFormat = "%s$b";
		}
		else if (strcmp("full_file_name", pVar) == 0) {
			pFormat = "%s$F";
		} else {
			strcpy(pResult, pVar);
			return;
		}
		mysprintf(pResult, pFormat, &(SPRINTF_ARGS){.sa_wp = wp});
	}
	if (pFunction) {
		if (strcmp(pFunction, "toUpper()") == 0) {
			string_convertUpperLower(pResult, TRUE);
		} else if (strcmp(pFunction, "toLower()") == 0) {
			string_convertUpperLower(pResult, FALSE);
		} else if (strcmp(pFunction, "camelCase()") == 0) {
			string_convertCamelCase(pResult);
		}
	}
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
