/*
 * gitintegration.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: Accessing GIT information about edited files.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

# ifndef 	GITINTEGRATION_H

typedef enum {
	VCS_NONE,				// File is not under version control
	VCS_CURRENT,			// File is under version control and is "current"
	VCS_NEW,				// File is in repo with a status of NEW
	VCS_MODIFIED,			// File is under version control and is modified.
	VCS_CONFLICTED,			// File is under version control and has conflicts.
	VCS_IGNORED,			// File is ignored by VCS.
	VCS_DELETED				// File is under version control and was deleted.
} VC_STATUS;

/*
 * Info about the version control state of a file.
 */
typedef struct tagVC_INFO VC_INFO;

/*
 * Shutdown the GIT integration.
 */
extern void gi_shutdown();

/*
 * Return the current version control information for a given file name. Note, that the
 * version info object must be released, when not used any more with gi_freeVersionInfo.
 */
extern VC_INFO* gi_getVersionInfo(const char* pszFilename);

/*
 * Free the version info object allocated with gi_getVersionInfo.
 */
extern void gi_freeVersionInfo(VC_INFO* pInfo);

/*
 * Return the current GIT status for a given version info.
 */
extern VC_STATUS gi_getStatus(VC_INFO* pInfo);

/*
 * Print the hash of the current ref of the file loaded into pszBuffer with a maximum length of nLen+1. If nLen is 0
 * GIT_OID_SHA1_HEXSIZE  + 1 characters are used for printing, so pszBuffer must be at least that big.
 */
extern void gi_printHash(VC_INFO* pInfo, char* pszBuffer, int nLen);

#define	GITINTEGRATION_H
# endif

