/*
 * SprintfUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: special sprintf, printing status infos,...
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# include <windows.h>
# include <stdarg.h>
# include "winterf.h"
# include "edierror.h"

# include <dos.h>
# include <string.h>
# include <time.h>
# include "editab.h"
# include "winfo.h"

extern char *basename(char *fn);
extern LPSTR lstrchr(char *s, char c);
extern char *ft_visiblename(FTABLE *fp);

int  _psenabled = 1;

#define MAXSSIZE		256

/*--------------------------------------------------------------------------
 * getfn()
 */
char *getfn(FTABLE *fp)
{
	return basename(ft_visiblename(fp));
}


/*--------------------------------------------------------------------------
 * InitDateformats()
 */
static char sDate[2], sTime[2];
static int  iDate;
void InitDateformats(void)
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
		iDate == 2 ? tp->tm_mday : tp->tm_year);
}

/*--------------------------------------------------------------------------
 * print_date()
 */
void print_date(char *szDbuf, EDTIME *ltime)
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
		return basename(fp->fname);

	case 'b':
		lstrcpy(fname,basename(fp->fname));
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
		if (fp->flags & F_MODIFIED)
			return "Ge�ndert";
blanks:	return "              ";
		break;

	case '*':
		if (fp->flags & F_MODIFIED)
			return "*";
		break;

	case 'r':
		if (fp->lin->workmode & O_RDONLY)
			return "Nur lesen";
		goto blanks;

	}

	return "";
}

/*--------------------------------------------------------------------------
 * CurrentNumVal()
 */
static long CurrentNumVal(FTABLE *fp, char **fmt)
{
	char *	format;

	if (fp == 0) {
		return 0;
	}

	format = *fmt;

	switch(*format) {

	case 'O':	
		return fp->hexoffset + (long)fp->lnoffset;

	case 'C':
		return (long)((unsigned char)fp->currl->lbuf[fp->lnoffset]);

	case 'l':
		return WIPOI(fp)->ln + 1L;

	case 'c':
		return WIPOI(fp)->col + 1L;

	case 'w':
		return WIPOI(fp)->win_id;

	default:
		*fmt -= 2;

	}
	
	return 0L;	
}

/*--------------------------------------------------------------------------
 * mysprintf()
 */
/*
	dollar handling:

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
*/

int mysprintf(FTABLE *fp, char *d, char *format,...)
{
static char *	_digits = "0123456789ABCDEFGHIJ";
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
					val = CurrentNumVal(fp, &format);
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
					if (*format == 't' || *format == 'h') {
						fm_mktagstring(stack,&stack[sizeof stack],
							*format == '?');
						x = stack;
					} else {
						x = CurrentStringVal(fp, &format, fname);
					}
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
						d  = blfill(d,l,f0);
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
						d = blfill(d,l,' ');
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
	return d-buf;
}

/*--------------------------------------------------------------------------
 * EdFormatPrint()
 */
void EdFormatPrint(long dummy1, long dummy2, char *format, char *p)
{
	char buf[1024];

	if (format) {
		mysprintf(_currfile, buf, format, p, 0L);
	} else {
		buf[0] = 0;
	}
	ReturnString(buf);
}

