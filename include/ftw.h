/*----------------------------------------------------------*/
/* INCLUDE - File fuer den Filetreewalk und Operationen mit */
/* Gemdos-DTA ...                                           */
/*----------------------------------------------------------*/

#ifndef __FTW__
#define __FTW__

#define FTW_NOFILE  -255      /* kein File bei Filesuche gefunden          */
#define NORMALFILE  0x00
#define WPROTECT    0x01
#define HIDDENFILE	0x02
#define SYSFILE	0x04
#define V_LABEL	0x08
#define SUBDIR      0x10
#define ARCHIV		0x20

extern int _ftwflg;

#if defined(WIN32)
#include <io.h>
typedef struct _finddata_t DTA;
#else
#include <dir.h>
#include <dos.h>
typedef struct ffblk DTA;
#endif

typedef int (* FTWFUNC)(char *s,DTA *p);
int _ftw(char *path,FTWFUNC func,int depth,char *pattern,int mode);
int  ftw(char *path,FTWFUNC func,int depth,char *pattern);
int ftwalk(char *path,FTWFUNC func,int depth,int fmode);

#endif __FTW__

