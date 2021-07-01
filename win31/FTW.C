/*
 * FTW.C
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: file tree walk
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "edtypes.h"
#include "ftw.h"

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
	char *path,				/* Root fÅr Filebaumsuche			*/
	FTWFUNC func,				/* fÅr jedes gefundene FILE wird		*/
							/* func(fname,fstat) ausgefÅhrt, bis	*/
							/* return(func) != 0, oder			*/
	int depth,				/* depth der Suche erreicht			*/
	char *pattern, 			/* pattern fuer Matching der Filenamen	*/
	int mode					/* mode der Files die gesucht werden	*/
)
{	int ret;

	_searchpattern = pattern;
	_mode		= mode;
	_func		= func;
	ret = __ftw(path,depth);
	return ret;
}

/*--------------------------------------------------------------------------
 * __ftw()
 */
static int __ftw(char *path,int depth)
{	struct ffblk  *pdta;
	char target[84],fn[14],newpath[84];
	register	i = 0;

	if (--depth < 0) {
		return 0;
	}

	pdta = _alloc(sizeof *pdta);

	strdcpy(target,path,"*.*");

	if (!findfirst(target,pdta,0xFF)) {
		do {

			if (ed_abort(0)) {
				_free(pdta);
				return 1;
			}

			if (pdta->ff_name[0] != '.' ) {

				if (pdta->ff_attrib == 0 || 
				   (pdta->ff_attrib & _mode) == pdta->ff_attrib) {
					fncpyin(pdta->ff_name,fn,0);
					if ((pdta->ff_attrib & SUBDIR) || 
					     match(fn,_searchpattern)) {
						_ftwflg++;
						strdcpy(target,path,pdta->ff_name);
						i = (*_func)(target,pdta);
						if (i) {
							_free(pdta);
							return i;
						}
					}
				}

				if (depth > 0 && pdta->ff_attrib & SUBDIR) {
					strdcpy(newpath,path,pdta->ff_name);
					i = __ftw(newpath,depth);
					if (i != 0) {
						_free(pdta);
						return i;
					}
				}

			}
		} while (!findnext(pdta));
	}

	_free(pdta);
	return i;
}

/*--------------------------------------------------------------------------
 * ftwalk()
 */
int ftwalk(
	char *path,				/* Root fÅr Filebaumsuche			*/
	int (*func)(),				/* fÅr jedes gefundene FILE wird		*/
							/* func(fname,fstat) ausgefÅhrt, bis	*/
							/* return(func) != 0, oder			*/
	int depth,				/* depth der Suche erreicht			*/
	int fmode					/* Filemode, siehe ftw.h 			*/
	)
{	int ret; 
	char _path[80],_fname[16];

	_ftwflg = 0;
	sfsplit(path,_path,_fname);
	ret = _ftw(_path,func,depth,_fname,fmode);
	return ((_ftwflg) ? ret : FTW_NOFILE); 
}


