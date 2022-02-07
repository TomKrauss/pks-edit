/*
 * pkscc.h
 *
 * macro compiler, stddefs.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 13.03.1991
 */

#ifndef PKSCC_H

#include <stdio.h>

#ifndef DIM
#define DIM(x)		sizeof(x)/sizeof(x[0])
#endif

#define UNUSED(x)	(x) = (x)

typedef int BOOL;

extern const char *	macro_loadStringResource(int nId);
extern void 		macro_showStatus(char *s, ...);

/*---------------------------------
 * macro_executeMacroByIndex()
 * Execute a macro given its logical
 * internal macro number.
 */
extern int macro_executeMacroByIndex(int macroindex);

/*
 * print the current mouse bindings to a file and display them to the user.
 */
extern int print_saveMouseBindingsAndDisplay(void);

/*---------------------------------
 * macro_createFileAndDisplay()
 * Invoke a callback to generate the contents of a file with
 * the given file name. If successful, open the file in PKS Edit.
 */
extern BOOL macro_createFileAndDisplay(char* fn, long (*callback)(FILE* fp));

/*--------------------------------------------------------------------------
 * print_saveMenuBindingsAndDisplay()
 */
extern int print_saveMenuBindingsAndDisplay(void);

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
extern int macro_readWriteWithFileSelection(int wrflag);

# ifdef _PKSMOD_H
extern EDXPORTS 	*edx;
# endif

#define PKSCC_H
#endif

