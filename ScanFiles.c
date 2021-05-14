/*
 * ScanFiles.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose:
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <string.h>
#include <malloc.h>

#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "ftw.h"

extern char *	_datadir;
static FTABLE	*_outfile;
static long 	_line;
static int 		_checkonce,_trymatch;

extern char	_expbuf[];
extern int 	prnfl(FTABLE *fp, char *fn, long line, char *remark);
extern char 	*AbbrevName(char *fn);
extern void 	ShowMessage(WORD nId, ...);

/*--------------------------------------------------------------------------
 * present()
 * display info in abort dialog box and update .GRP-file
 */
static long _nfound;
static void present(char *fn)
{
	_nfound++;
	ShowMessage(IDS_MSGNTIMESFOUND, _nfound);
	prnfl(_outfile,fn,_line,"");
}

/*--------------------------------------------------------------------------
 * scanlines()
 * 
 */
static unsigned char *scanlines(char *fn, unsigned char *p, unsigned char *qend)
{
	register char	*	q;
	register char 		nl = '\n';
	char *			stepend;

	q = p;
	while (p < qend) {
		if (*p++ == nl) {
longline_scan:
			stepend = p-2;
			if (*stepend == '\r')
				stepend--;
			if (step(q,_expbuf,stepend)) {
				present(fn);
				if (_checkonce)
					return 0;
			}
			_line++;
			q = p;
		}
	}

	if ((qend - q) >= FBUFSIZE) {

	/* this "line" is too long: simulate break */
		goto longline_scan;

	}

	return q;
}

/*--------------------------------------------------------------------------
 * parsefile()
 * scan for pattern in file fn
 */
static int listtags(char *fn, DTA *stat)
{	
	int 	fd;

#if defined(WIN32)
	if (stat->attrib & _A_SUBDIR)
#else
	if (stat->ff_attrib & SUBDIR)
#endif
		return 0;

	_line = 0L;
	abrt_message(AbbrevName(fn));

	if (!_trymatch) {
		present(fn);
		return(0);
	}

	if ((fd = EdOpen(fn)) < 0)
		return -1;

	readfrags(fd,scanlines,fn);

	closeF(&fd);

	return 0;
}

/*--------------------------------------------------------------------------
 * retreive()
 */
int retreive(char *pathes, char *search, int sdepth, int checkonce)
{
	char *		path;
	char *		pathlist;
	char		stepfile[256];

	_checkonce = checkonce;
	_nfound = 0;
	strdcpy(stepfile, _datadir, "pksedit.grp");

	if (!*search) {
		_trymatch = 0;
	} else {
		if (!exprinit(search))
			return 0;
		_trymatch = 1;
	}

	pathlist = _alloc(300);
	_outfile = _alloc(sizeof *_outfile);
	blfill(_outfile, sizeof *_outfile, 0);
	lstrcpy(pathlist,pathes);
	abrt_start(IDS_ABRTRETREIVE);
	if ((path = strtok(pathlist,",;")) != 0) {
		do {	
			if (ftwalk(path,listtags,sdepth,
					 NORMALFILE|ARCHIV|WPROTECT) == 1) break;
		} while ((path = strtok((char *)0,",;")) != 0);
	}

	Writeandclose(_outfile,stepfile,0);
	abrt_close(0);

	_free(pathlist);
	_free(_outfile);

	return stepnofsel(stepfile,0) , 1;
}


