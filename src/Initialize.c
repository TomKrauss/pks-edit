/*
 * Initialize.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: Initializations
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
#include <direct.h>
#include <Shlobj.h>
#include "pksrc.h"
#include "documentmodel.h"
#include "xdialog.h"
#include "pathname.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "regexp.h"
#include "crossreferencelinks.h"
#include "documenttypes.h"
#include "pksmacro.h"
#include "printing.h"
#include "fileselector.h"
#include "winterf.h"
#include "copyright.h"

#undef DELIVER

extern int file_exists(char *s);

static char _sysdir[EDMAXPATHLEN];
char _pksSysExtensionFolder[EDMAXPATHLEN];
char _pksSysOverrideFolder[EDMAXPATHLEN];
extern char* PKS_SYS;					// The name if the PKS_SYS variable / configuration folder etc...
extern char* _pksSysFolder;				// PKS_SYS directory, where the config files are located.

/*--------------------------------------------------------------------------
 * Getenv()
 */
static void Getenv(char *name,char *d,int maxlen)
{	extern char *getenv(const char *s);
	char *env;

	*d = 0;
	if ((env = getenv(name)) != 0) {
		strmaxcpy(d, env, maxlen);
	}

}

/*
 * Check whether the passed path is a valid PKS_SYS directory containing in particular the PKS Edit configuration file. 
 */
static BOOL _checkPksSys(char* pathName) {
	char initFileName[1024];
	string_concatPathAndFilename(initFileName, pathName, DEFAULT_CONFIG_FILE_NAME);
	return file_exists(initFileName) == 0;
}

/*--------------------------------------------------------------------------
 * init_initializeVariables()
 */
EXPORT BOOL init_initializeVariables(void ) 
{
	char	installationDirectory[EDMAXPATHLEN];
	char	datadir[EDMAXPATHLEN];
	char *	pks_sys = PKS_SYS;
	int     tempLen;
	EDITOR_CONFIGURATION* pConfig = GetConfiguration();

	_pksSysFolder = _sysdir;
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, datadir))) {
		strcpy(_pksSysExtensionFolder, datadir);
		strcpy(_pksSysOverrideFolder, datadir);
		strcat(_pksSysExtensionFolder, "\\.pksedit\\config");
		strcat(_pksSysOverrideFolder, "\\.pksedit\\pks_sys");
	}
	tempLen = GetModuleFileName(NULL, installationDirectory, EDMAXPATHLEN);
	installationDirectory[tempLen] = 0;
	// PKS_SYS environment var first
	Getenv(pks_sys, datadir, sizeof(_sysdir));
	if (*datadir) {
		lstrcpy(_pksSysFolder, datadir);
	} else {
		// current directory - PKS_SYS sub-directory next
		if (_getcwd(_pksSysFolder, EDMAXPATHLEN) == NULL) {
			_pksSysFolder[0] = 0;
		}
		string_concatPathAndFilename(_pksSysFolder, _pksSysFolder, pks_sys);
		if (!_checkPksSys(_pksSysFolder)) {
			// Next: config from win.ini
			GetProfileString("PksEdit", pks_sys, "", _pksSysFolder, EDMAXPATHLEN);
			if (!*_pksSysFolder || !_checkPksSys(_pksSysFolder)) {
				// Finally: PKS_SYS from the path where the executable is located.
				char* tempFound = strrchr(installationDirectory, '\\');
				if (tempFound != NULL && (tempFound - installationDirectory) > 1) {
					tempFound[-1] = 0;
					string_concatPathAndFilename(installationDirectory, installationDirectory, pks_sys);
					if (_checkPksSys(installationDirectory)) {
						strcpy(_pksSysFolder, installationDirectory);
					}
				} else {
					installationDirectory[0] = '\0';
				}
			}
		}
	}
	if (installationDirectory[0] == 0) {
		if (_getcwd(installationDirectory, sizeof installationDirectory) == NULL) {
			installationDirectory[0] = 0;
		}
	}
	string_concatPathAndFilename(installationDirectory,installationDirectory,"");
	if (*_pksSysFolder == 0) {
		while(1) {
			char *s = dlg_getResourceString(IDS_PKS_SYS_NOT_FOUND);
			if (!fsel_selectFolder(hwndMain, s, _pksSysFolder)) {
				return FALSE;
			}
			if (_checkPksSys(_pksSysFolder)) {
				break;
			}
		}
	}
	Getenv("PKS_INCLUDE_PATH", pConfig->includePath, member_size(EDITOR_CONFIGURATION, includePath));
	if (pConfig->includePath[0] == 0) {
		strcpy(pConfig->includePath, "include;inc");
	}
	Getenv("PKS_TMP", pConfig->pksEditTempPath, member_size(EDITOR_CONFIGURATION, pksEditTempPath));
	return TRUE;
}

/*--------------------------------------------------------------------------
 * init_readConfigFiles()
 */
EXPORT void init_readConfigFiles(void)
{
	prof_getstdopt();
	macro_readCompiledMacroFile((char *)0);
	doctypes_initAllDocumentTypes();
	xref_restoreFromConfigFile();
	copyright_loadProfiles();
	regex_compileCharacterClasses((char *) 0);
}

