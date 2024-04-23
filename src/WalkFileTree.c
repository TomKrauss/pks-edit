/*
 * WalkFileTree.c
 *
 * Project: PKS Edit for Windows
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

#include "alloc.h"
#include <string.h>
#include "edtypes.h"
#include "ftw.h"
#include "stringutil.h"
#include "edierror.h"

/*--------------------------------------------------------------------------
 * _ftw()
 * perform a file tree walk scanning files recursively given a file name pattern to match
 * and executes the given callback on each matching file.
 */
int _ftw(
	const char *path,				// Root where the file tree walk begins
	FTWFUNC func,					// execute for every file found, until func returns != 0 or ... 
	int depth,						// the depth of the maximum folder recursion level is reached
	const char *searchPattern, 		// pattern for file name matching. E.g. something such as "*.c" or "dir\*.c". In the later case the pattern is matched
									// also recursively for sub-directories.
	int mode						// file attributes of the files searched
) {
	struct _finddata_t  *	pdta;
	char 				*	target;
	intptr_t				fhandle;
	int						nFiles = 0;
	int						i;
	const char*				pThisPattern = searchPattern;
	const char*				pNextPattern = searchPattern;

	if (--depth < 0) {
		return 0;
	}
	char* pPatternCopy = _strdup(searchPattern);
	i = 0;
	pdta = malloc(sizeof *pdta);
	target = malloc(strlen(path) + 256);
	string_concatPathAndFilename(target,path,"*");
	char* pToken = strtok(pPatternCopy, "/\\");
	if (strcmp(pToken, searchPattern) != 0) {
		pThisPattern = pToken;
		pNextPattern = pPatternCopy+strlen(pThisPattern)+1;
	}
	if ((fhandle = _findfirst(target, pdta)) >= 0) {
		do {
			if (progress_cancelMonitor(0)) {
				i = 1;
				goto done;
			}

			if (strcmp(pdta->name, ".") == 0 || strcmp(pdta->name, "..") == 0) {
				continue;
			}
			nFiles++;
			if (pdta->attrib == 0 || 
			   (pdta->attrib & mode) == pdta->attrib) {
				if ((pThisPattern == searchPattern && (pdta->attrib & _A_SUBDIR)) || 
					 string_matchFilename(pdta->name,pThisPattern)) {
					string_concatPathAndFilename(target,path,pdta->name);
					i = (*func)(target,pdta);
					if (i) {
						goto done;
					}
				} else if (nFiles % 10 == 9) {
					progress_stepIndicator();
				}
			}

			if (depth > 0 && (pdta->attrib & _A_SUBDIR) != 0 && (pThisPattern == searchPattern || string_matchFilename(pdta->name, pThisPattern))) {
				string_concatPathAndFilename(target,path,pdta->name);
				i = _ftw(target, func, depth, pNextPattern,  mode);
				if (i != 0) {
					goto done;
				}
			}

		} while (!_findnext(fhandle, pdta));
	}
done:
	_findclose(fhandle);
	free(pPatternCopy);
	free(pdta);
	free(target);
	return i;
}




