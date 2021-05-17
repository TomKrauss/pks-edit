/*
 * WalkFileTree.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: file tree walk
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <malloc.h>
#include <string.h>
#include "edtypes.h"
#include "ftw.h"

extern void strdcpy(char *dest, char *source, char *fname);
extern int match(char *string,char *pat);

int __ftw(char *path,int depth);

/* to be reset before every search - indicates minimum 1 file found */
int _ftwflg;

static char *_searchpattern;
static int  _mode;
static int  (*_func)();

/*--------------------------------------------------------------------------
 * __ftw()
 * uses #include ftw.h
 */
int _ftw(
	char *path,				/* Root where the file tree walk begins		*/
	FTWFUNC func,			/* execute for every file found, until func returns != 0 or ... */
	int depth,				/* the depth of the maximum folder recursion level is reached*/
	char *pattern, 			/* pattern for file name matching */
	int mode				/* file attributes of the files searched */
)
{	int ret;

	_searchpattern = pattern;
	_mode = mode;
	_func = func;
	ret = __ftw(path,depth);
	return ret;
}

/*--------------------------------------------------------------------------
 * __ftw()
 */
static int __ftw(char *path,int depth)
{	struct _finddata_t  *	pdta;
	char 				*	target;
	intptr_t				fhandle;
	register int			i;

	if (--depth < 0) {
		return 0;
	}
	i = 0;
	pdta = _alloc(sizeof *pdta);
	target = _alloc(strlen(path) + 256);
	strdcpy(target,path,"*");
	if ((fhandle = _findfirst(target, pdta)) >= 0) {
		do {
			if (ProgressMonitorCancel(0)) {
				_findclose(fhandle);
				_free(pdta);
				_free(target);
				return 1;
			}

			if (strcmp(pdta->name, ".") == 0 || strcmp(pdta->name, "..") == 0) {
				continue;
			}
			if (pdta->attrib == 0 || 
			   (pdta->attrib & _mode) == pdta->attrib) {
				if ((pdta->attrib & _A_SUBDIR) || 
					 match(pdta->name,_searchpattern)) {
					_ftwflg++;
					strdcpy(target,path,pdta->name);
					i = (*_func)(target,pdta);
					if (i) {
						_free(pdta);
						_free(target);
						_findclose(fhandle);
						return i;
					}
				}
			}

			if (depth > 0 && (pdta->attrib & _A_SUBDIR) != 0) {
				strdcpy(target,path,pdta->name);
				i = __ftw(target,depth);
				if (i != 0) {
					_free(pdta);
					_free(target);
					return i;
				}
			}

		} while (!_findnext(fhandle, pdta));
	}

	_free(pdta);
	_findclose(fhandle);
	_free(target);
	return i;
}

/*--------------------------------------------------------------------------
 * ftwalk()
 */
int ftwalk(char *path,FTWFUNC func,int depth,int fmode)
{	int 	ret; 
	char 	newPath[1024];
	char	fname[256];

	_ftwflg = 0;
	sfsplit(path,newPath,fname);
	ret = _ftw(newPath,func,depth,fname,fmode);
	return ((_ftwflg) ? ret : FTW_NOFILE); 
}




