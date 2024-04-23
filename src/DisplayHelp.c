/*
 * DisplayHelp.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: Handle help requests
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "winterf.h"
#include "helpitem.h"
#include "documentmodel.h"
#include "edierror.h"
#include "winfo.h"
#include "stringutil.h"
#include "fileutil.h"
#include "pathname.h"
#include "mainframe.h"

static char szHelpDir[512];

/*--------------------------------------------------------------------------
 * help_open()
 */
static int help_open(char *szFile) {
	if (!szHelpDir[0]) {
		char* pszHelp = file_searchFileInPKSEditLocation("../doc");
		if (!pszHelp) {
			error_showMessage("Cannot find documentation directory");
			return 0;
		}
		strcpy(szHelpDir, pszHelp);
	}
	char szPath[EDMAXPATHLEN];
	string_concatPathAndFilename(szPath, szHelpDir, szFile);
	FTABLE* fp = NULL;
	if (ft_activateWindowOfFileNamed(szPath) || (fp = ft_openFileWithoutFileselector(szPath, 0, &(FT_OPEN_OPTIONS) { DOCK_NAME_RIGHT, -1 })) != NULL) {
		WINFO* wp = fp ? WIPOI(fp) : ww_getCurrentEditorWindow();
		if (wp) {
			ww_changeDisplayMode(wp, wp->dispmode | SHOW_WYSIWYG_DISPLAY);
		}
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdHelpContext()
 */
int EdHelpContext(DWORD nCtx) {
	char* pszFile = "..\\readme.md";

	switch (nCtx) {
	case IDM_HLPOVERVIEW: pszFile = "manual\\pksedit_ui.md"; break;
	case IDM_HLPONKEYS: pszFile = "manual\\editing_files.md"; break;
	case IDM_HLP_MANUAL_TOC: pszFile = "manual\\toc.md"; break;
	case IDM_HLPRELEASENOTES: pszFile = "release_notes.md"; break;
	}
	return help_open(pszFile);
}

