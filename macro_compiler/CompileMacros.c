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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lineoperations.h"
#include "pkscc.h"
#include "stringutil.h"

int  yyparse(void);
void yyinit(jmp_buf *errb, char *sourcefile, LINE *lps,LINE *lpe);
int  yyfinish(void);
int  opennofsel(char *fn, long line, void *wsp);
char *TmpDir(void);
void alert(char * fmt, ...);
void ShowError(char * fmt, va_list ap);

extern int		_macedited;

/*--------------------------------------------------------------------------
 * CurrentRulerContext(()
 */
int CurrentRulerContext(void)
{	FTABLE *fp;

	if ((fp = _currfile) != 0)
		return fp->documentDescriptor->id;
	return DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
}

/*--------------------------------------------------------------------------
 * ActiveRulerContext()
 */
int ActiveRulerContext(int ctx)
{
	if (ctx != DEFAULT_DOCUMENT_DESCRIPTOR_CTX && ctx != CurrentRulerContext())
		return 0;
	
	return 1;
}

/*--------------------------------------------------------------------------
 * PrintListHeader()
 */
void PrintListHeader(FILE *fp, char *itemname)
{	FTABLE *ftp;

	ftp = _currfile;

	fprintf(fp,"\nList of active %s for file \"%s\"\n\n",itemname,
		ftp ? basename(ftp->fname) : "(no file)");
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

/*--------------------------------------------------------------------------
 * prpastelist()
 */
static void prpastelist(FILE *fp, PASTELIST *pp)
{
	while(pp) {
		fprintf(fp,"'%c'        ˙ ",(pp->id & 0xFF));
		printpaste(fp,&pp->pbuf);
		pp = pp->next;
	}
}

/*--------------------------------------------------------------------------
 * printesclist()
 */
void printesclist(FILE *fp, void *p)
{	PASTELIST **pp = (PASTELIST**)p;
	int ctx;

	prpastelist(fp,pp[0]);
	if ((ctx = CurrentRulerContext()) != DEFAULT_DOCUMENT_DESCRIPTOR_CTX) {
		prpastelist(fp,pp[ctx]);
	}
}

/*
 * createtmp()
 */
FILE *createtmp(char *dest, char *filename)
{	FILE *fp;

	strdcpy(dest, TmpDir(), filename);

	if ((fp = fopen(dest,"w")) == 0) {
		Alert(/*STR*/"kann %s nicht erzeugen", dest);
		return 0;
	}
	return fp;
}

/*---------------------------------*/
/* displinwin()				*/
/*---------------------------------*/
int displinwin(char *fn, long (*callback)(FILE *fp))
{	char tmpfn[128];
	FILE *fp;

	if ((fp = createtmp(tmpfn,fn)) == 0)
		return 0;

	if ((*callback)(fp) != 0) {
		fflush(fp);
		fclose(fp);
		if (ActivateWindowOfFileNamed(tmpfn)) {
			EdFileAbandon(1);
		}
		else {
			opennofsel(tmpfn,-1L, (void *)0);
		}
	} else {
		fclose(fp);
	}

	unlink(tmpfn);

	return 0;
}

/*---------------------------------*/
/* macs_compile()				*/
/*---------------------------------*/
int macs_compile()
{	
	FTABLE *		fp;
	jmp_buf 		errb;

	if ((fp = _currfile) != 0) {
		if (!setjmp(errb)) {
			yyinit(&errb, fp->fname, fp->firstl, fp->lastl->prev);
			yyparse();
			_macedited = 1;
		}
		return yyfinish();
	}

	Alert("Bitte ˆffnen Sie die Datei, die ¸bersetzt werden soll");

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

int macs_execute_string(char *string)
{	
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
		alert("Error in command: %s", string);
		return 0;
	}
	do_macbyname("temp-block");
	mac_delete("temp-block");
	_macedited = saveMacEdited;
	return 1;
}

/*---------------------------------*/
/* Alert()					*/
/*---------------------------------*/
void Alert(char *s, ...)
{	
	va_list 	ap;
	char 	b[256];

	va_start(ap, s);
	vsprintf(b, s, ap);
	va_end(ap);
	alert(b);
}

/*---------------------------------*/
/* protokoll()					*/
/*---------------------------------*/
void protokoll(char *s, ...)
{
	va_list 	ap;
	char 	b[256];

	va_start(ap, s);
	vsprintf(b, s, ap);
	va_end(ap);
	ShowError(b, (void*)0);
}


