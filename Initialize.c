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

#undef DELIVER

extern int EdStat(char *s);
extern LPSTR 	strmaxcpy(LPSTR pszDest, LPSTR pszSource, int nMax);
extern void  	stepselectcompiler(char *pszName);
extern int   	proj_init(void);
extern void 	c2asc(char *start, char *source, unsigned char hi, unsigned char lo);
extern char *	cryptXXXX(char *dest, char *source, 
	 			void (*f)(char *start, char *s, unsigned char hi, unsigned char lo));

char	*_datadir;			/* Environment Editor-Data			*/

#define MAX_PATH_LEN 1024

char _homedir[MAX_PATH_LEN];

char _sysdir[MAX_PATH_LEN +8]  = "****TKR.";
char _serial[10]      = "100000";
char _cryptserial[32] = "?";
char _kunde[30]       = "Demoversion";
char _cryptkunde[80]  = "X";
char _incpath[MAX_PATH_LEN]    = "INC;C:\\BORLANDC\\INCLUDE";
char _aspath[MAX_PATH_LEN];

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
	char	datadir[MAX_PATH_LEN];
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
	tempLen = GetModuleFileName(NULL, _homedir, MAX_PATH_LEN);
	_homedir[tempLen] = 0;
	tempFound = mystrrchr(_homedir, '\\');
	if (tempFound != NULL && (tempFound-_homedir) > 1) {
		tempFound[-1] = 0;
		strcat(_homedir, "\\pks_sys");
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
	GetProfileString("PksEdit", pks_sys, "", _datadir, MAX_PATH_LEN);
	if (!*_datadir) {
		strdcpy(_datadir, _homedir, "PKS_SYS");
	}
	Getenv(pks_sys, datadir,sizeof(_datadir));
	if (*datadir) {
		lstrcpy(_datadir, datadir);
	}

	compiler[0] = 0;
	Getenv("COMPILER",compiler,sizeof(compiler));
	if (compiler[0]) {
		stepselectcompiler(compiler);
	}
	Getenv("INCPATH", _incpath,sizeof(_incpath));
	Getenv("PKS_TMP", _aspath,sizeof(_aspath));

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
	dt_init();
	tags_init();
	proj_init();
	compiler_init();
	PrinterDefaults(0);
	fixsets((char *) 0);
}

