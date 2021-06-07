/*
 * MAKROC.C
 *
 * makro compiler module
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 13.03.1991
 *									
 */

#include <windows.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>

#include "errordialogs.h"
#include "alloc.h"
#include "lineoperations.h"
#include "pkscc.h"
#include "stringutil.h"
#include "edfuncs.h"
#include "fileutil.h"

int  yyparse(void);
void yyinit(jmp_buf *errb, char *sourcefile, LINE *lps,LINE *lpe);
int  yyfinish(void);

extern int		_macedited;

/*--------------------------------------------------------------------------
 * getCurrentDocumentTypeContext(()
 */
static int getCurrentDocumentTypeContext(void)
{	FTABLE *fp;

	if ((fp = ft_getCurrentDocument()) != 0)
		return fp->documentDescriptor->id;
	return DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
}

/*--------------------------------------------------------------------------
 * macro_isInCurrentDocumentContext()
 * Check, whether the passed document context identifier matches the document
 * context of the current open file.
 */
int macro_isInCurrentDocumentContext(int ctx)
{
	if (ctx != DEFAULT_DOCUMENT_DESCRIPTOR_CTX && ctx != getCurrentDocumentTypeContext())
		return 0;
	
	return 1;
}

/*--------------------------------------------------------------------------
 * macro_printListHeader()
 */
void macro_printListHeader(FILE *fp, char *itemname)
{	FTABLE *ftp;

	ftp = ft_getCurrentDocument();

	fprintf(fp,"\nList of active %s for file \"%s\"\n\n",itemname,
		ftp ? string_getBaseFilename(ftp->fname) : "(no file)");
}

/*--------------------------------------------------------------------------
 * printpaste()
 */
int printpaste(FILE *fp, void *p)
{	PASTE *pp = p;
	int   maxlen = 40;
	LINE  *lp;

	for (lp = pp->pln; lp; lp = lp->next) {
		fprintf(fp,"%.*s",min(maxlen,lp->len),lp->lbuf);
		if ((maxlen -= lp->len) <= 0 ||
		    (lp = lp->next) == 0)
			break;
		fprintf(fp,"\\n"); 
	}
	if (lp) 
		fprintf(fp," ...");
	fprintf(fp,"\n");
	return maxlen;
}

/*
 * createtmp()
 */
FILE *createtmp(char *dest, char *filename)
{	FILE *fp;

	string_concatPathAndFilename(dest, file_getTempDirectory(), filename);

	if ((fp = fopen(dest,"w")) == 0) {
		error_displayAlertDialog(/*STR*/"kann %s nicht erzeugen", dest);
		return 0;
	}
	return fp;
}

/*---------------------------------
 * macro_createFileAndDisplay()
 * Invoke a callback to generate the contents of a file with
 * the given file name. If successful, open the file in PKS Edit.
 */
BOOL macro_createFileAndDisplay(char *fn, long (* callback)(FILE *fp)) {
	char tmpfn[128];
	FILE *fp;

	if ((fp = createtmp(tmpfn, fn)) == 0) {
		return FALSE;
	}

	if ((*callback)(fp) != 0) {
		fflush(fp);
		fclose(fp);
		if (ft_activateWindowOfFileNamed(tmpfn)) {
			EdFileAbandon();
		}
		else {
			ft_optionFileWithoutFileselector(tmpfn,-1L, (void *)0);
		}
	} else {
		fclose(fp);
	}

	_unlink(tmpfn);

	return TRUE;
}

/*---------------------------------*/
/* mac_compileMacros()				*/
/*---------------------------------*/
int mac_compileMacros()
{	
	FTABLE *		fp;
	jmp_buf 		errb;

	if ((fp = ft_getCurrentDocument()) != 0) {
		if (!setjmp(errb)) {
			yyinit(&errb, fp->fname, fp->firstl, fp->lastl->prev);
			yyparse();
			_macedited = 1;
		}
		return yyfinish();
	}

	error_displayAlertDialog("Bitte ˆffnen Sie die Datei, die ¸bersetzt werden soll");

	return 0;
}

static void UnEscape(char *dst, char *src) {
	int		bInString;

	bInString = 0;
	while(*src) {
		if (bInString) {
			if (*src == '\\') {
				*dst++ = '\\';
			}
			if (*src == '"') {
				bInString = 0;
			}
		} else {
			if (*src == '"') {
				bInString = 1;
			}
		}
		*dst++ = *src++;
	}
	*dst = 0;
}

/**
* Execute a macro given a single line text to execute.
 */
int macro_executeSingleLineMacro(char *string) {	
	char		chTemp[1024];
	FTABLE 		ft;
	LINE *		lp;
	jmp_buf 	errb;
	int			nFail;
	int			saveMacEdited;

	saveMacEdited = _macedited;
	memset(&ft, 0, sizeof ft);
	ln_createAndAdd(&ft, "macro temp-block() {", -1, 0);
	UnEscape(chTemp, string);
	ln_createAndAdd(&ft, chTemp, -1, 0);
	ln_createAndAdd(&ft, "}", -1, 0);
/* sigh: need an empty line here */
	ln_createAndAdd(&ft, "", -1, 0);
	if (!(nFail = setjmp(errb))) {
		for (lp = ft.firstl; lp && lp->next; lp = lp->next) ;
		yyinit(&errb, "\"DDE Command\"", ft.firstl, lp);
		yyparse();
	}
	if (!yyfinish() || nFail) {
		error_displayAlertDialog("Error in command: %s", string);
		return 0;
	}
	macro_executeByName("temp-block");
	macro_deleteByName("temp-block");
	_macedited = saveMacEdited;
	return 1;
}

/*---------------------------------
 * macro_showStatus()
 * Display a status message while compiling a macro.
 *---------------------------------*/
void macro_showStatus(char *s, ...)
{
	va_list 	ap;
	char 	b[256];

	va_start(ap, s);
	vsprintf(b, s, ap);
	va_end(ap);
	error_displayErrorToast(b, (void*)0);
}


