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
char _includePath[EDMAXPATHLEN]  = "INCLUCDE;INC";
char _pksEditTempPath[EDMAXPATHLEN];

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

int mystrrchr(char s[], int c) {
	int tempLen = strlen(s);
	while(--tempLen >= 0) {
		if (s[tempLen] == c) {
			return &s[tempLen+1];
		}
	}
	return NULL;
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
		alert("PKS-EDIT is not installed correctly");
		return FALSE;
	}
# endif
	_datadir = _sysdir+8;
	tempLen = GetModuleFileName(NULL, _homedir, EDMAXPATHLEN);
	_homedir[tempLen] = 0;
	tempFound = mystrrchr(_homedir, '\\');
	if (tempFound != NULL && (tempFound-_homedir) > 1) {
		tempFound[-1] = 0;
		strcat(_homedir, "\\");
		strcat(_homedir, pks_sys);
		if (EdStat(_homedir) < 0) {
			_homedir[0] = '\0';
		} else {
			tempFound[-1] = 0;
		}
	} else {
		_homedir[0] = '\0';
	}
	if (_homedir[0] == 0) {
		getcwd(_homedir,sizeof _homedir);
	}
	GetProfileString("PksEdit", pks_sys, "", _datadir, EDMAXPATHLEN);
	if (!*_datadir) {
		strdcpy(_datadir, _homedir, "PKS_SYS");
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
	Getenv("PKS_INCLUDE_PATH", _includePath, sizeof(_includePath));
	Getenv("PKS_TMP", _pksEditTempPath, sizeof(_pksEditTempPath));

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

