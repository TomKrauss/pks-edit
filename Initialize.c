/*
 * Initialize.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
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
#include "lineoperations.h"
#include "pathname.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "regexp.h"
#include "crossreferencelinks.h"
#include "documenttypes.h"
#include "edfuncs.h"
#include "printing.h"

#undef DELIVER

extern int file_exists(char *s);
extern void 	c2asc(char *start, char *source, unsigned char hi, unsigned char lo);
extern char *	cryptXXXX(char *dest, char *source, 
	 			void (*f)(char *start, char *s, unsigned char hi, unsigned char lo));

char	*_pksSysFolder;	// PKS_SYS directory, where the config files are located.
static char _sysdir[EDMAXPATHLEN];

/*--------------------------------------------------------------------------
 * Getenv()
 */
static void Getenv(char *name,char *d,int maxlen)
{	extern char *getenv(const char *s);
	char *env;

	if ((env = getenv(name)) != 0) {

		strmaxcpy(d, env, maxlen);
	} else {
		prof_getPksProfileString("environment",name,d,maxlen);
	}

}

/*
 * Check whether the passed path is a valid PKS_SYS directory containing in particular the pksedit.ini file. 
 */
static BOOL _checkPksSys(char* pathName) {
	char initFileName[1024];
	string_concatPathAndFilename(initFileName, pathName, "pksedit.ini");
	return file_exists(initFileName) == 0;
}

/*--------------------------------------------------------------------------
 * init_initializeVariables()
 */
EXPORT BOOL init_initializeVariables(void ) 
{
	char	homeDirectory[EDMAXPATHLEN];
	char 	compiler[32];
	char	datadir[EDMAXPATHLEN];
	char *	pks_sys = "PKS_SYS";
	char *  tempFound;
	int     tempLen;
	EDITOR_CONFIGURATION* pConfig = GetConfiguration();

	_pksSysFolder = _sysdir;
	tempLen = GetModuleFileName(NULL, homeDirectory, EDMAXPATHLEN);
	homeDirectory[tempLen] = 0;
	// PKS_SYS environment var first
	Getenv(pks_sys, datadir, sizeof(_pksSysFolder));
	if (*datadir) {
		lstrcpy(_pksSysFolder, datadir);
	} else {
		// current directory - PKS_SYS sub-directory next
		_getcwd(_pksSysFolder, EDMAXPATHLEN);
		string_concatPathAndFilename(_pksSysFolder, _pksSysFolder, pks_sys);
		if (!_checkPksSys(_pksSysFolder)) {
			// Next: config from win.ini
			GetProfileString("PksEdit", pks_sys, "", _pksSysFolder, EDMAXPATHLEN);
			if (!*_pksSysFolder || !_checkPksSys(_pksSysFolder)) {
				// Finally: PKS_SYS from the path where the executable is located.
				tempFound = strrchr(homeDirectory, '\\');
				if (tempFound != NULL && (tempFound - homeDirectory) > 1) {
					tempFound[-1] = 0;
					string_concatPathAndFilename(homeDirectory, homeDirectory, pks_sys);
					if (_checkPksSys(homeDirectory)) {
						strcpy(_pksSysFolder, homeDirectory);
					}
				}
				else {
					homeDirectory[0] = '\0';
				}
			}
		}
	}
	if (homeDirectory[0] == 0) {
		_getcwd(homeDirectory,sizeof homeDirectory);
	}
	compiler[0] = 0;
	Getenv("PKS_INCLUDE_PATH", pConfig->includePath, member_size(EDITOR_CONFIGURATION, includePath));
	if (pConfig->includePath[0] == 0) {
		strcpy(pConfig->includePath, "include;inc");
	}
	Getenv("PKS_TMP", pConfig->pksEditTempPath, member_size(EDITOR_CONFIGURATION, pksEditTempPath));
	string_concatPathAndFilename(homeDirectory,homeDirectory,"");
	return TRUE;
}

/*--------------------------------------------------------------------------
 * init_readConfigFiles()
 */
EXPORT void init_readConfigFiles(void)
{
	prof_getstdopt();
	macro_readBindingsFromFile((char *)0);
	doctypes_initAllDocumentTypes();
	xref_restoreFromConfigFile();
	print_readWriteConfigFile(0);
	regex_compileCharacterClasses((char *) 0);
}

