#define 	NASCII				256

#if defined (ASCII_VERSION)
# include <sys\stat.h>
# include	<string.h>
# include <io.h>
# include	<stdio.h>
#define	creat				_creat
#define	write				_write
#define	close				_close
#define	read					_read
#define	open					_open
#define	READ					0
#else
#include	<windows.h>
#include	<string.h>
#define	creat				_lcreat
#define	write				_lwrite
#define	close				_lclose
#define	read					_lread
#define	open					_lopen
#define	lseek				_llseek
#define	strcpy				lstrcpy
#define	strlen				lstrlen
#define	strcmp				lstrcmp
# endif

# define	E_FILENOTFOUND			-1
# define	E_KEYNOTFOUND			-2
# define	E_WRITEANDCLOSE		-3
# define	SUCCESS				0

# define	FBUFSIZE				4096

typedef struct tagF {
	int		fdSrc;
	int		fdDst;
	int		errno;
	int		offset;
	int		maxbuf;
	long		srcfileoffset;
	long		dstfilewritten;
	char		buf1[FBUFSIZE];
	char		buf2[FBUFSIZE];
} Ffile, *F;

static Ffile theFile;

extern void c2asc(char *start, char *source, unsigned char hi, unsigned char lo);
extern char *cryptXXXX(char *dest, char *source, void (*f)());

/*--------------------------------------------------------------------------
 * Ffclose()
 */
static int Ffclose(F fp)
{
	int	nread;

	if (fp->maxbuf > 0 && (int)write(fp->fdDst, fp->buf1, fp->maxbuf) < 0) {
		fp->errno = 1;
		return 0;
	}
	fp->dstfilewritten += fp->maxbuf;
	while((nread = (int)read(fp->fdSrc, fp->buf1, 2 * FBUFSIZE)) > 0) {
		(int)write(fp->fdDst, fp->buf1, nread);
		fp->dstfilewritten += nread;
	}
	
	if (lseek(fp->fdDst, 0, 2) != fp->dstfilewritten) {
		fp->errno = 2;
		return 0;
	}

	close(fp->fdDst);
	close(fp->fdSrc);
	return 1;
}

/*--------------------------------------------------------------------------
 * Ffillbuf()
 */
static int Ffillbuf(F fp)
{
	int		nflush;
	int		nread;

	nflush = fp->maxbuf > FBUFSIZE ? FBUFSIZE : fp->maxbuf;
	if (nflush > 0) {
		if ((int)write(fp->fdDst, fp->buf1, nflush) < 0) {
			fp->errno = 3;
			return 0;
		}
		fp->dstfilewritten += nflush;
		if (fp->maxbuf > nflush) {
			memcpy(fp->buf1, &fp->buf1[nflush], fp->maxbuf - nflush);
		}
		fp->maxbuf -= nflush;
		fp->offset -= nflush;
	}
	nread = (int)read(fp->fdSrc, &fp->buf1[fp->maxbuf],
		2 * FBUFSIZE - fp->maxbuf);
	if (nread < 0) {
		fp->errno = 4;
		return 0;
	}
	fp->maxbuf += nread;
	return 1;	
}

/*--------------------------------------------------------------------------
 * Fadvance()
 */
# define	Fadvance(f,n)	(f->offset += n, \
	f->offset > f->maxbuf ? Ffillbuf(f) : 1)

/*--------------------------------------------------------------------------
 * Fpeekc()
 */
# define	Fpeekc(f)		f->buf1[f->offset]

/*--------------------------------------------------------------------------
 * Fpokec()
 */
# define	Fpokec(f,c)	Fpeekc(f) = c

/*--------------------------------------------------------------------------
 * Ffopen()
 */
static F Ffopen(char *src, char *dst)
{
	F 	fp;

	fp = &theFile;

	if ((fp->fdSrc = open(src, READ)) < 0) {
		return 0;
	}
	
	if ((fp->fdDst = creat(dst, 0)) < 0) {
		return 0;
	}
	
	fp->offset = 0;
	fp->srcfileoffset = 0;
	fp->dstfilewritten = 0;
	fp->maxbuf = 0;
	fp->errno = 0;

	return fp;
}

/*--------------------------------------------------------------------------
 * mkboyertab()
 */
static void boyertable(unsigned char *bt,unsigned char *pattern,int plen)
{
	int 			i;
	unsigned char 	c;

	for (i = 0; i < NASCII; i++) {
		bt[i] = plen;
	}

	for (i = 0; i < plen; i++) {
		c = pattern[plen-i-1];
		if (bt[c] == plen)
			bt[c] = i;
	}
}

/*--------------------------------------------------------------------------
 * ematch()
 */
static int ematch(unsigned char *s1,unsigned char *s2,int len)
{
	while(len-- > 0)
		if (*s1++ != *s2++)
			return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * boyermatch()
 */
static int boyermatch(F fp, unsigned char *bt, unsigned char *pat)
{ 	unsigned char *	found;
	unsigned char 		matchc,c;
	unsigned short 	len;
	unsigned char 		szBuf[100];
	int 				plen;
	int				i;

	plen = strlen(pat);
	plen--;
	Fadvance(fp, plen);
	matchc = pat[plen];
	for (;;) {
		c = Fpeekc(fp);
		if (c == matchc) {
			Fadvance(fp, -plen);
			for (i = 0; i < plen; i++) {
				szBuf[i] = Fpeekc(fp);
				Fadvance(fp, 1);
			}
			if (ematch(szBuf,pat,plen)) {
				Fadvance(fp, -plen);
				return 1;
			}
			if (!Fadvance(fp, 1)) {
				break;
			}
		} else {
			if (!Fadvance(fp, bt[c])) {
				break;
			}
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * Fputtag()
 */
static void Fputtag(F fp, char *string, int nTagSize)
{
	while(*string) {
		Fpokec(fp, *string);
		Fadvance(fp, 1);
		string++;
		nTagSize--;
	}
     Fpokec(fp, 0);
	Fadvance(fp, nTagSize);
}

/*--------------------------------------------------------------------------
 * BrandFile()
 */
#if !defined(PASCAL)
#define PASCAL /**/
#endif
#if !defined(FAR)
#define FAR /**/
#endif
int FAR PASCAL BrandFile(char *fname, char *serial, char *cryptserial, char *customer)
{
	unsigned char	*	key = "****TKR.";
	unsigned char		bt[256];
	unsigned char		tmpspace[200];
	unsigned char 		tmpname[128];
	int				nTagSize;
	F				fp;

	boyertable(bt,key,strlen(key));

	strcpy(tmpname, fname);
	tmpname[strlen(tmpname) - 1] = '$';

	if ((fp = Ffopen(fname, tmpname)) == 0) {
		return E_FILENOTFOUND;
	}

	if (!boyermatch(fp, bt, key)) {
		return E_KEYNOTFOUND;
	}

	Fadvance(fp, 102);

/* poke serial */
	Fputtag(fp, serial, 10);
/* poke cryptserial */
	Fputtag(fp, cryptserial, 32);
/* poke customer */
	Fputtag(fp, customer, 30);
/* poke crypte customer */
	cryptXXXX(tmpspace, customer, c2asc);
	Fputtag(fp, tmpspace, 64);

	if (!Ffclose(fp)) {
		return E_WRITEANDCLOSE;	
	}
	return SUCCESS;
}

#if defined (ASCII_VERSION)
/*---------------------------------*/
/* MAIN						*/
/*---------------------------------*/
int main(int argc,char *argv[])
{
	if (argc > 1) {
		BrandFile(argv[1], "99", "CPC", "GEBIT-SPECIAL");
	}
	return 0;
}
# else

#include	<ctype.h>

char *_cryptend;

void c2asc(char *start, char *source, unsigned char hi, unsigned char lo)
{
	while(!isalnum(hi))
		hi += 51;
	*_cryptend++ = hi;
	if ((lo & 1) || source < start + 2) {
		while(!isalnum(lo))
			lo += 51;
		*_cryptend++ = lo;
	}
	_cryptend[1] = 0;
}

static int magics[] = {	
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
};

/*---------------------------------*/
/* cryptXXXX()					*/
/*---------------------------------*/
char *cryptXXXX(char *dest, char *source,
	 void (*f)(char *start, char *s, unsigned char hi, unsigned char lo))
{	long x;
	int  *p;
	unsigned char hi,lo;
	char *s,*start = source;
	char *end      = source;

	while(end[1])
		end++;
	x 		 = 0;
	_cryptend = dest;

	while(*source) {
		p = magics;
		s = source + 1;
		while(1) {
			if (s > end)
				s = start;
			x += ((long)*p * (long)*s);
			x += *++p;
			if (s == source)
				break;
			s++;
		}
		source++;
		hi = x >> 8;
		lo = x;
		if (f) 
			(*f)(start,source,hi,lo);
		else {
			*dest++ = hi;
			*dest++ = lo;
		}
		x >>= 8;
	}
	if (!f)
		_cryptend = dest;
	*_cryptend = 0;
	return _cryptend;
}


int FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize,
                        LPSTR lpszCmdLine) {
	return (1);
}


BOOL FAR PASCAL CheckSerial(char *serial, char *key)
{	
	char buf[128];

	cryptXXXX(buf,serial,c2asc);
	if (lstrcmp(buf,key)) {
		return 0;
	}
	return 1;
}

# endif

