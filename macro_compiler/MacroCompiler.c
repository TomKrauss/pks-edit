/*
 * MacroCompiler.c
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
#include "arraylist.h"
#include "alloc.h"
#include "regexp.h"
#include "documentmodel.h"
#include "stringutil.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "fileutil.h"
#include "editorconfiguration.h"
#include "crossreferencelinks.h"
#include "winfo.h"

typedef struct tagLINE_INPUT {
	LINE* li_firstLine;
	LINE* li_lastLine;
	LINE* li_currentLine;
	char* li_buf;
	char* li_bufEnd;
} LINE_INPUT;

int  yyparse(void);
void yyinit(jmp_buf *errb, COMPILER_CONFIGURATION* pConfig);
int  yyfinish(void);

extern int		_macrosWereChanged;
static int compiler_compileWithParams(COMPILER_CONFIGURATION* pConfig);

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
		// (T) I18N
		error_displayAlertDialog(/*STR*/"Cannot create file %s", dest);
		return 0;
	}
	return fp;
}

static int compile_lineInputNext(COMPILER_INPUT_STREAM* pStream) {
	LINE_INPUT* pLI = pStream->cis_pointer;
	LINE* lp;

	if (pLI->li_currentLine == 0) {
		lp = pLI->li_firstLine;
		pLI->li_currentLine = lp;
		pLI->li_buf = lp->lbuf;
		pLI->li_bufEnd = &lp->lbuf[lp->len];
	}
	if (pLI->li_buf >= pLI->li_bufEnd) {
		if (pLI->li_currentLine != pLI->li_lastLine &&
			(lp = pLI->li_currentLine->next) != 0 &&
			lp->next != 0) {
			pLI->li_currentLine = lp;
			pLI->li_buf = lp->lbuf;
			pLI->li_bufEnd = &lp->lbuf[lp->len];
			return '\n';
		}
		return EOF;
	}
	return *pLI->li_buf++;
}

/*---------------------------------
 * macro_createFileAndDisplay()
 * Invoke a callback to generate the contents of a file with
 * the given file name. If successful, open the file in PKS Edit.
 */
BOOL macro_createFileAndDisplay(char *fn, long (* callback)(FILE *fp), int bWysiwyg) {
	char tmpfn[EDMAXPATHLEN];
	FILE *fp;

	if ((fp = file_createTempFile(tmpfn, fn)) == 0) {
		return FALSE;
	}

	if ((*callback)(fp) != 0) {
		fflush(fp);
		fclose(fp);
		if (ft_activateWindowOfFileNamed(tmpfn)) {
			EdRereadFileFromDisk(ww_getCurrentEditorWindow());
		}
		else {
			FTABLE* fpTable = ft_openFileWithoutFileselector(tmpfn,-1L, &(FT_OPEN_OPTIONS) { 0, CP_ACP });
			if (fpTable != NULL) {
				fpTable->flags |= F_TRANSIENT;
				if (bWysiwyg) {
					ww_changeDisplayMode(WIPOI(fpTable), SHOW_WYSIWYG_DISPLAY);
				}
			}
		}
	} else {
		fclose(fp);
	}

	_unlink(tmpfn);

	return TRUE;
}

static void compile_showSummary(COMPILER_CONFIGURATION* pConfig, int nErr, int nWarn) {
	if (nErr || nWarn) {
		xref_openBuildOutputFile((const char*)pConfig->cb_errorFile, TRUE, -1);
	} else {
		macro_showStatus("Compilation of %d file(s) successful. No errors and no warnings found.", pConfig->cb_numberOfFilesCompiled);
	}
}

static void compiler_closeFilestream(COMPILER_INPUT_STREAM* pStream) {
	if (pStream->cis_pointer) {
		fclose(pStream->cis_pointer);
		pStream->cis_pointer = 0;
	}
}

static int compiler_nextFromFilestream(COMPILER_INPUT_STREAM* pStream) {
	return pStream->cis_pointer ? fgetc(pStream->cis_pointer) : EOF;
}

static int compiler_compileDependentFile(COMPILER_CONFIGURATION* pBaseConfig, const char* pszFilename) {
	FILE* fp = fopen(pszFilename, "r");
	if (!fp) {
		pBaseConfig->cb_showStatus("Cannot open input file %s", pszFilename);
		return 0;
	}
	COMPILER_INPUT_STREAM inputStream = {
		.cis_close = compiler_closeFilestream,
		.cis_next = compiler_nextFromFilestream,
		.cis_pointer = fp
	};
	pBaseConfig->cb_source = pszFilename;
	pBaseConfig->cb_topLevelFile = FALSE;
	pBaseConfig->cb_stream = &inputStream;
	return compiler_compileWithParams(pBaseConfig);
}

static int compiler_compileWithParams(COMPILER_CONFIGURATION* pConfig) {
	int nRet;
	jmp_buf errb;
	BOOL bTopLevel = pConfig->cb_topLevelFile;
	if (!setjmp(errb)) {
		yyinit(&errb, pConfig);
		yyparse();
	}
	nRet = yyfinish();
	if (nRet && bTopLevel && pConfig->cb_dependencies) {
		for (int i = 0; i < arraylist_size(pConfig->cb_dependencies); ) {
			const char* pszFile = arraylist_get(pConfig->cb_dependencies, i);
			if (!compiler_compileDependentFile(pConfig, pszFile)) {
				nRet = 0;
				break;
			}
			if (!pConfig->cb_postponed) {
				i++;
			}
		}
	}
	if (bTopLevel && pConfig->cb_dependencies) {
		arraylist_destroyStringList(pConfig->cb_dependencies);
	}
	return nRet;
}

/*---------------------------------*/
/* compiler_compileCurrentDocument()				*/
/*---------------------------------*/
int compiler_compileCurrentDocument() {
	FTABLE *		fp;
	if ((fp = ft_getCurrentDocument()) != 0) {
		COMPILER_CONFIGURATION config = {
			.cb_insertNewMacro = macro_insertNewMacro,
			.cb_showStatus = macro_showStatus,
			.cb_topLevelFile = TRUE,
			.cb_source = fp->fname
		};
		LINE_INPUT lineInput = { .li_firstLine = fp->firstl, .li_lastLine = fp->lastl };
		COMPILER_INPUT_STREAM cis = {
			.cis_next = compile_lineInputNext,
			.cis_pointer = &lineInput
		};
		config.cb_stream = &cis;
		int nRet = compiler_compileWithParams(&config);
		compile_showSummary(&config, config.cb_numberOfErrors, config.cb_numberOfWarnings);
		return nRet;
	}

	// TODO: I18N
	error_displayAlertDialog("Please open the file to compile.");

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
static int macro_defineTemporaryMacro(MACRO_PARAM* pParam) {
	// TODO: when multiple macro functions are compiled as part of the evaluation, we should remove them all.
	free(_tempMacroName);
	_tempMacroName = _strdup(pParam->mp_name);
	return macro_insertNewMacro(pParam);
}

static void macro_noStatus(char* fmt, ...) {
}

/*
 * Checks, whether a code to evaluate needs to be enclosed into a temporary macro
 * function before compiling.
 */
static BOOL macro_needsWrapper(const char* pszCode) {
	int state = 0;
	const char* pszId = NULL;

	while (*pszCode) {
		char c = *pszCode++;
		if (isspace(c)) {
			continue;
		}
		if (c == '/') {
			if (*pszCode == '/') {
				while (*pszCode) {
					c = *pszCode++;
					if (c == '\n') {
						break;
					}
				}
			} else if (*pszCode == '*') {
				pszCode++;
				while (*pszCode) {
					c = *pszCode++;
					if (c == '*' && *pszCode == '/') {
						pszCode++;
						break;
					}
				}
			}
			continue;
		} else if (isalpha(c)) {
			pszId = pszCode - 1;
			while (*pszCode) {
				c = *pszCode;
				if (!isalpha(c)) {
					break;
				}
				pszCode++;
			}
			char buf[10];
			size_t nLen = pszCode - pszId;
			if (nLen < sizeof buf) {
				strncpy(buf, pszId, nLen);
				buf[nLen] = 0;
				if (strcmp("void", buf) == 0 || strcmp("int", buf) == 0 || strcmp("string", buf) == 0 || strcmp("bool", buf) == 0) {
					return FALSE;
				}
			}
			return TRUE;
		}
	}
	return TRUE;
}

/*
 * A macro source file requires a namespace (or file name) to be defined (loaded). If that is not the case
 * load it first relative to the given source file.
 */
int compiler_requireNamespaceOrFilename(ARRAY_LIST* pDependentFiles, int nIndex, const char* pszSourcefile, const char* pszRequired) {
	char szNamespacename[128];
	char szFilename[128];
	char szPath[EDMAXPATHLEN];
	char szBuf[EDMAXPATHLEN];
	strmaxcpy(szNamespacename, pszRequired, sizeof szNamespacename);
	char* pszExt = strrchr(szNamespacename, '.');
	if (strcmp(pszSourcefile, pszRequired) == 0) {
		strcpy(szBuf, pszRequired);
	}
	else {
		if (pszExt) {
			strcpy(szFilename, szNamespacename);
			*pszExt++ = 0;
		}
		else {
			sprintf(szFilename, "%s.pkc", pszRequired);
		}
		if (macro_hasNamespace(szNamespacename)) {
			return 1;
		}
		string_splitFilename(pszSourcefile, szPath, 0, 0);
		string_concatPathAndFilename(szBuf, szPath, szFilename);
	}
	if (arraylist_indexOfComparing(pDependentFiles, szBuf, _stricmp) >= 0) {
		return 1;
	}
	if (file_exists(szBuf) < 0) {
		return 0;
	}
	arraylist_insertAt(pDependentFiles, _strdup(szBuf), nIndex);
	return 1;
}

/**
* Execute a macro given a single line text to execute.
* pszCode ist the string of code to execute. If bUnescape is true, we treat \ and " special
* and assume, the command to execute was passed on the command line (and will be escaped).
* 'pszContext' is the name of the context in which the execution will be performed.
 */
int macro_executeSingleLineMacro(const char* pszCode, BOOL bUnescape, const char* pszContext) {
	FTABLE 		ft;
	int			saveMacEdited;

	saveMacEdited = _macrosWereChanged;
	_tempMacroName = NULL;
	memset(&ft, 0, sizeof ft);
	BOOL bNeedsWrapper = macro_needsWrapper(pszCode);
	char* pszHeader = "string temp-block() {";
	if (bNeedsWrapper) {
		ln_createAndAdd(&ft, pszHeader, -1, 0);
	}
	size_t nSize = strlen(pszCode);
	char* pszTemp = calloc(1, nSize + 3);
	if (pszTemp == NULL) {
		return 0;
	}
	if (bUnescape) {
		macro_unEscape(pszTemp, pszCode, nSize);
	} else {
		strcpy(pszTemp, pszCode);
	}
	int nLen = (int)strlen(pszTemp);
	while (--nLen > 0) {
		if (pszTemp[nLen] == ')') {
			pszTemp[nLen+1] = ';';
			break;
		}
		if (!isspace((unsigned char)pszTemp[nLen])) {
			break;
		}
	}
	ln_createAndAdd(&ft, pszTemp, -1, 0);
	if (bNeedsWrapper) {
		ln_createAndAdd(&ft, "}", -1, 0);
	}
/* sigh: need an empty line here */
	if (!ln_createAndAdd(&ft, "", -1, 0)) {
		return 0;
	}
	COMPILER_CONFIGURATION config = {
		.cb_insertNewMacro = macro_defineTemporaryMacro,
		.cb_showStatus = macro_noStatus,
		.cb_topLevelFile = TRUE,
		.cb_source = (char*)pszContext
	};
	LINE_INPUT lineInput = { .li_firstLine = ft.firstl };
	COMPILER_INPUT_STREAM cis = {
		.cis_next = compile_lineInputNext,
		.cis_pointer = &lineInput
	};
	config.cb_stream = &cis;
	int ret = 1;
	if (!compiler_compileWithParams(&config)) {
		if (strlen(pszTemp) > 128) {
			// Shorten command
			strcpy(pszTemp + 128, "...");
		}
		char buffer[256];
		if (bNeedsWrapper) {
			sprintf(buffer, "%s\n%s\n}", pszHeader, pszTemp);
		}
		else {
			strcpy(buffer, pszTemp);
		}
		compile_showSummary(&config, config.cb_numberOfErrors, config.cb_numberOfWarnings);
		error_displayAlertDialog("Failed to compile\n%s", buffer);
		ret = 0;
	} else {
		macro_executeByName(_tempMacroName);
		macro_deleteByName(_tempMacroName);
	}
	free(pszTemp);
	free(_tempMacroName);
	ln_listfree(ft.firstl);
	_macrosWereChanged = saveMacEdited;
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


