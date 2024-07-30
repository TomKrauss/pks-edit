/*
 * SettingsIO.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: save and restore settings
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
#include <dos.h>
#include <stdio.h>
#include "trace.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "edierror.h"
#include "documenttypes.h"
#include "funcdef.h"

#include "winfo.h"
#include "winterf.h"
#include "stringutil.h"

#pragma hdrstop

#include "fileselector.h"
#include "pksedit.h"
#include "editorconfiguration.h"
#include "findandreplace.h"
#include "fileutil.h"
#include "mouseutil.h"
#include "winutil.h"
#include "themes.h"

/*
 * The absolute path name of the last pkseditini.json file read or written. 
 */
static char _pksEditIniFilename[512];
static FSELINFO _setfselinfo = { ".", DEFAULT_CONFIG_FILE_NAME, "*.json" };

/*------------------------------------------------------------
 * LocatePksEditIni()
 */
static int LocatePksEditIni(void)
{	char *fn;

	if (_pksEditIniFilename[0]) {
		return 1;
	}
	if ((fn = fsel_initPathes(&_setfselinfo)) == 0) {
		return 0;
	}
	GetFullPathName(fn, sizeof _pksEditIniFilename, _pksEditIniFilename, NULL);
	return 1;
}

/*--------------------------------------------------------------------------
 * prof_setinifile()
 */
void prof_setinifile(char *fn)
{
	if (file_exists(fn) == 0) {
		string_getFullPathName(_pksEditIniFilename, fn, sizeof _pksEditIniFilename);
	}
}

/*------------------------------------------------------------
 * prof_getstdopt()
 */
int prof_getstdopt(void) {
	if (!LocatePksEditIni() || !config_read(_pksEditIniFilename)) {
		config_initialize();
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------
 * config_saveConfiguration()
 * Save the configuration. If bInteractive is true, open
 * a file selector to pick the pkseditini.json file to which the config
 * is saved. If bInteractive is false, use the default file name
 * from which the configuration had been loaded.
 */
long long config_saveConfiguration(int bInteractive) {
	int  	fd;
	char *	fn;

	if (!bInteractive) {
		fn = _pksEditIniFilename;

	} else {
		LocatePksEditIni();
		string_splitFilename(_pksEditIniFilename, _setfselinfo.path, _setfselinfo.fname, sizeof _setfselinfo.fname);
		FILE_SELECT_PARAMS params = {
			.fsp_saveAs = TRUE,
			.fsp_optionsAvailable = FALSE
		};
		if ((fn = fsel_selectFileWithOptions(&_setfselinfo, CMD_SAVE_OPTIONS, &params)) == 0) {
			return 0;
		}
	}

	/* create non existing profiles, others will be updated */
	if (file_exists(fn) < 0) {
		if ((fd = file_createFile(fn)) < 0) {
			return 0;
		}
		_lclose(fd);
	}

	mouse_setBusyCursor();
	lstrcpy(_pksEditIniFilename,fn);
	config_save(_pksEditIniFilename);
	mouse_setDefaultCursor();
	return 1;
}

