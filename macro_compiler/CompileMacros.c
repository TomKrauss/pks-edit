/*
 * CompileMacros.c
 *
 * makro compiler module
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author : TOM
 * created: 13.03.1991
 * 
 */

#include <windows.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>

#include "errordialogs.h"
#include "alloc.h"
#include "regexp.h"
#include "documentmodel.h"
#include "pkscc.h"
#include "stringutil.h"
#include "edfuncs.h"
#include "fileutil.h"
#include "editorconfiguration.h"

int  yyparse(void);
void yyinit(jmp_buf *errb, COMPILER_CONFIGURATION* pConfig, LINE *lps,LINE *lpe);
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

/*
 * file_createTempFile()
 * Create a temp file and return the name of the file created in "dest". The
 * dest character array must contain enough space to hold EDMAXPATHLEN characters.
 */
FILE *file_createTempFile(char *dest, char *filename) {	
	FILE *fp;

	string_concatPathAndFilename(dest, config_getPKSEditTempPath(), filename);
	if (strchr(dest, '.') == NULL) {
		strcat(dest, ".pkc");
	}
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
	char tmpfn[EDMAXPATHLEN];
	FILE *fp;

	if ((fp = file_createTempFile(tmpfn, fn)) == 0) {
		return FALSE;
	}

	if ((*callback)(fp) != 0) {
		fflush(fp);
		fclose(fp);
		if (ft_activateWindowOfFileNamed(tmpfn)) {
			EdFileAbandon();
		}
		else {
			FTABLE* fpTable = ft_openFileWithoutFileselector(tmpfn,-1L, &(FT_OPEN_OPTIONS) { 0, CP_ACP });
			if (fpTable != NULL) {
				fpTable->flags |= F_TRANSIENT;
			}
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
		COMPILER_CONFIGURATION config = {
			macro_insertNewMacro,
			macro_showStatus,
			TRUE,
			fp->fname
		};
		if (!setjmp(errb)) {
			yyinit(&errb, &config, fp->firstl, fp->lastl->prev);
			yyparse();
		}
		return yyfinish();
	}

	error_displayAlertDialog("Bitte öffnen Sie die Datei, die übersetzt werden soll");

	return 0;
}

static void macro_unEscape(char *dst, const char *src, size_t nDestSize) {
	int		bInString;
	char* pszDest = dst + nDestSize - 3;

	bInString = 0;
	while(*src && dst < pszDest) {
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

/*------------------------------------------------------------
 * macro_defineTemporaryMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
static char* _tempMacroName;
static int macro_defineTemporaryMacro(char* name, char* comment, char* macdata, int size) {
	// TODO: when multiple macro functions are compiled as part of the evaluation, we should remove them all.
	free(_tempMacroName);
	_tempMacroName = _strdup(name);
	return macro_insertNewMacro(name, comment, macdata, size);
}

static void macro_noStatus(char* fmt, ...) {
}

/*
 * Checks, whether a code to evaluate needs to be enclosed into a temporary macro
 * function before compiling.
 */
static BOOL macro_needsWrapper(const char* pszCode) {
	RE_PATTERN pattern;
	RE_OPTIONS options;
	RE_MATCH match;
	char patternBuf[400];

	options.flags = RE_DOREX;
	options.patternBuf = patternBuf;
	options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
	options.expression = "^\\s*macro\\s+";
	if (!regex_compile(&options, &pattern)) {
		return TRUE;
	}
	regex_match(&pattern, (char*)pszCode, 0, &match);
	return !match.matches;
}

/**
* Execute a macro given a single line text to execute.
* pszCode ist the string of code to execute. If bUnescape is true, we treat \ and " special
* and assume, the command to execute was passed on the command line (and will be escaped).
* 'pszContext' is the name of the context in which the execution will be performed.
 */
int macro_executeSingleLineMacro(const char *pszCode, BOOL bUnescape, const char* pszContext) {	
	FTABLE 		ft;
	jmp_buf 	errb;
	int			nFail;
	int			saveMacEdited;

	saveMacEdited = _macedited;
	_tempMacroName = NULL;
	memset(&ft, 0, sizeof ft);
	BOOL bNeedsWrapper = macro_needsWrapper(pszCode);
	if (bNeedsWrapper) {
		ln_createAndAdd(&ft, "macro temp-block() {", -1, 0);
	}
	size_t nSize = strlen(pszCode);
	if (bUnescape) {
		char* pszTemp = calloc(1, nSize);
		macro_unEscape(pszTemp, pszCode, nSize);
		ln_createAndAdd(&ft, pszTemp, -1, 0);
		free(pszTemp);
	} else {
		ln_createAndAdd(&ft, (char*)pszCode, -1, 0);
	}
	if (bNeedsWrapper) {
		ln_createAndAdd(&ft, "}", -1, 0);
	}
/* sigh: need an empty line here */
	if (!ln_createAndAdd(&ft, "", -1, 0)) {
		return 0;
	}
	if (!(nFail = setjmp(errb))) {
		COMPILER_CONFIGURATION config = {
			macro_defineTemporaryMacro,
			macro_noStatus,
			FALSE,
			(char*)pszContext
		};
		yyinit(&errb, &config, ft.firstl, NULL);
		yyparse();
	}
	int ret = 1;
	if (!yyfinish() || nFail) {
		error_displayAlertDialog("Error in command: %s", pszCode);
		ret = 0;
	} else {
		macro_executeByName(_tempMacroName);
		macro_deleteByName(_tempMacroName);
	}
	free(_tempMacroName);
	_macedited = saveMacEdited;
	return ret;
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
	error_displayErrorInToastWindow(b);
}


