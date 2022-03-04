/*
 * MacroCFunctions.c
 *
 * Provides functions to be invoked from the PKSMacroC language.
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
#include "documentmodel.h"
#include "mainframe.h"
#include "winfo.h"
#include "pathname.h"
#include "textblocks.h"
#include "editorconfiguration.h"
#include "editoperations.h"
#include "caretmovement.h"
#include "stringutil.h"
#include "edctype.h"

static const char* consoleFileName = "console.log";

/*
 * Opens the console, if not yet open. 
 */
static FTABLE* macroc_openConsole() {
	char szCompleteFileName[EDMAXPATHLEN];

	string_concatPathAndFilename(szCompleteFileName, config_getPKSEditTempPath(), consoleFileName);
	FTABLE* fp = ft_fpbyname(szCompleteFileName);
	if (!fp) {
		fp = ft_openFileWithoutFileselector(szCompleteFileName, 0L, &(FT_OPEN_OPTIONS) {.fo_dockName = DOCK_NAME_RIGHT, .fo_isNewFile = 1});
		if (fp) {
			fp->flags |= F_RDONLY|F_TRANSIENT;
		}
	}
	return fp;
}

/*
 * Print a text to the PKS-Edit console.
 */
static long long macroc_printToConsole(int bNewLine, const char* pszText) {
	FTABLE* fp = macroc_openConsole();
	if (!fp) {
		return 0;
	}
	if (!pszText) {
		pszText = "";
	}
	WINFO* wp = WIPOI(fp);
	PASTE pasteBuffer;
	memset(&pasteBuffer, 0, sizeof pasteBuffer);
	strcpy(_linebuf, pszText);
	strcat(_linebuf, bNewLine ? "\n\n" : "\n");
	if (bl_convertTextToPasteBuffer(&pasteBuffer, _linebuf, _linebuf + strlen(_linebuf), '\n', 0, 0)) {
		CARET oldCaret = wp->caret;
		bl_pasteBlock(wp, &pasteBuffer, 0, oldCaret.offset, 0);
		ln_removeFlag(fp->firstl, fp->lastl, LNMODIFIED);
		ft_setFlags(fp, fp->flags & ~F_MODIFIED);
		bl_free(&pasteBuffer);
	}
	return 1;
}

/*
 * Print a text including a trailing newline to the PKS-Edit console.
 */
long long macroc_println(const char* pszText) {
	return macroc_printToConsole(1, pszText);
}

/*
 * Print a text to the PKS-Edit console.
 */
long long macroc_print(const char* pszText) {
	return macroc_printToConsole(0, pszText);
}

/*
 * Empty the console.
 */
long long macroc_clearConsole() {
	char szCompleteFileName[EDMAXPATHLEN];

	string_concatPathAndFilename(szCompleteFileName, config_getPKSEditTempPath(), consoleFileName);
	FTABLE* fp = ft_fpbyname(szCompleteFileName);
	if (!fp) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);
	LINE* lpLast = fp->lastl->prev;
	caret_placeCursorInCurrentFile(wp, 0, 0);
	bl_delete(wp, fp->firstl, lpLast, 0, lpLast->len, 0, 0);
	ft_setFlags(fp, fp->flags & ~F_MODIFIED);
	wp->caret.linePointer = fp->firstl;
	wp->caret.ln = 0;
	render_repaintAllForFile(fp);
	return 1;
}

/*
 * Convert a text to upper / lower / toggle case.
 */
static char* macroc_convertChars(const char* pszString, CC_OPERATION operation) {
	char c;
	char* pszDest = _linebuf;
	char* pszEnd = &_linebuf[MAXLINELEN-3];
	while ((c = *pszString++) != 0 && pszDest < pszEnd) {
		char c1 = c;
		if (operation == CC_TOGGLE || operation == CC_UPPER) {
			c1 = _l2uset[c];
		}
		if ((c == c1 && operation == CC_TOGGLE) || operation == CC_LOWER) {
			c1 = _u2lset[c];
		}
		*pszDest++ = c1;
	}
	*pszDest = 0;
	return _linebuf;
}

char* macroc_toupper(const char* pszString) {
	return macroc_convertChars(pszString, CC_UPPER);
}


char* macroc_tolower(const char* pszString) {
	return macroc_convertChars(pszString, CC_LOWER);
}

