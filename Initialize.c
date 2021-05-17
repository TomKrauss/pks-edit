/*
 * Initialize.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: Initializations
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */
 
#include <windows.h> 
#include <direct.h>
#include "lineoperations.h"
#include "pathname.h"
#include "editorconfiguration.h"

#undef DELIVER

extern int EdStat(char *s);
extern LPSTR 	strmaxcpy(LPSTR pszDest, LPSTR pszSource, int nMax);
extern void  	stepselectcompiler(char *pszName);
extern int   	proj_init(void);
extern void 	c2asc(char *start, char *source, unsigned char hi, unsigned char lo);
extern char *	cryptXXXX(char *dest, char *source, 
	 			void (*f)(char *start, char *s, unsigned char hi, unsigned char lo));

char	*_datadir;			/* Environment Editor-Data			*/

char _homedir[EDMAXPATHLEN];

char _sysdir[EDMAXPATHLEN +8]  = "****TKR.";
char _serial[10]      = "100000";
char _cryptserial[32] = "?";
char _kunde[30]       = "free version (by us a beer)";
char _cryptkunde[80]  = "X";

/*---------------------------------*/
/* checkkey()					*/
/*---------------------------------*/
static int checkkey(char *serial, char *key)
{	char buf[128];

	cryptXXXX(buf,serial,c2asc);
	if (lstrcmp(buf,key)) {
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * Getenv()
 */
static void Getenv(char *name,char *d,int maxlen)
{	extern char *getenv(char *s);
	char *env;

	if ((env = getenv(name)) != 0) {
		strmaxcpy(d, env, maxlen);
	} else {
		GetPksProfileString("environment",name,d,maxlen);
	}

}

char* mystrrchr(char s[], int c) {
	int tempLen = strlen(s);
	while(--tempLen >= 0) {
		if (s[tempLen] == c) {
			return &s[tempLen+1];
		}
	}
	return NULL;
}

/*
 * Check whether the passed path is a valid PKS_SYS directory containing in particular the pksedit.ini file. 
 */
static BOOL _checkPksSys(char* pathName) {
	char initFileName[1024];
	strdcpy(initFileName, _homedir, "pksedit.ini");
	return EdStat(initFileName) == 0;
}

/*--------------------------------------------------------------------------
 * InitEnv()
 */
EXPORT BOOL InitEnv(void ) 
{
	char 	compiler[32];
	char	datadir[EDMAXPATHLEN];
	char *	pks_sys = "PKS_SYS";
	char *  tempFound;
	int     tempLen;

#if defined(DELIVER)
	if (!checkkey(_serial,_cryptserial) ||
	    !checkkey(_kunde,_cryptkunde)) {
		alert("Bitte erwerben oder importieren Sie eine Lizenz für PKS-EDIT");
		return FALSE;
	}
# endif
	_datadir = _sysdir+8;
	tempLen = GetModuleFileName(NULL, _homedir, EDMAXPATHLEN);
	_homedir[tempLen] = 0;
	GetProfileString("PksEdit", pks_sys, "", _datadir, EDMAXPATHLEN);
	if (!*_datadir || !_checkPksSys(_datadir)) {
		tempFound = mystrrchr(_homedir, '\\');
		if (tempFound != NULL && (tempFound - _homedir) > 1) {
			tempFound[-1] = 0;
			strcat(_homedir, "\\");
			strcat(_homedir, pks_sys);
			if (_checkPksSys(_homedir)) {
				strcpy(_datadir, _homedir);
			}
		}
		else {
			_homedir[0] = '\0';
		}
	}
	if (_homedir[0] == 0) {
		getcwd(_homedir,sizeof _homedir);
	}
	Getenv(pks_sys, datadir,sizeof(_datadir));
	if (*datadir) {
		lstrcpy(_datadir, datadir);
	}
	compiler[0] = 0;
	Getenv("PKS_COMPILER", compiler, sizeof(compiler));
	if (compiler[0]) {
		stepselectcompiler(compiler);
	}
	Getenv("PKS_INCLUDE_PATH", GetConfiguration()->includePath, member_size(EDITOR_CONFIGURATION, includePath));
	Getenv("PKS_TMP", GetConfiguration()->pksEditTempPath, member_size(EDITOR_CONFIGURATION, pksEditTempPath));

	strdcpy(_homedir,_homedir,"");
	return TRUE;
}

/*--------------------------------------------------------------------------
 * ReadConfigFiles()
 */
EXPORT void ReadConfigFiles(void)
{
	prof_getstdopt();
	mac_read((char *)0);
	InitAllDocumentTypes();
	tags_init();
	proj_init();
	compiler_init();
	PrinterDefaults(0);
	fixsets((char *) 0);
}

