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
#include "stringutil.h"
#include "pathname.h"
#include "editorconfiguration.h"
#include "caretmovement.h"

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
long long macroc_printToConsole(const char* pszText) {
	FTABLE* fp = macroc_openConsole();
	if (!fp) {
		return 0;
	}
	if (!ln_createAndAddSimple(fp, pszText)) {
		return 0;
	}
	fp->nlines++;
	if (fp->lpReadPointer) {
		render_repaintLineRange(fp, fp->lpReadPointer, fp->lpReadPointer);
	}
	caret_placeCursorMakeVisibleAndSaveLocation(WIPOI(fp), fp->nlines - 1, 0);
	return 1;
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
	LINE* lp;
	while ((lp = fp->firstl) != 0 && lp->next != fp->lastl) {
		ln_delete(fp, lp);
	}
	lp = fp->firstl;
	if (lp) {
		lp->len = 0;
	}
	ft_setFlags(fp, fp->flags & ~F_MODIFIED);
	render_repaintAllForFile(fp);
	return 1;
}
