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
	VCS_DELETED				// File is under version control and was deleted.
} VC_STATUS;

/*
 * Shutdown the GIT integration.
 */
extern void gi_shutdown();

/*
 * Return the current GIT status of a given file name.
 */
extern VC_STATUS gi_getStatus(const char* pszFilename);

#define	GITINTEGRATION_H
# endif

