/*
 * PKSCC.H
 *
 * macro compiler, stddefs.
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 13.03.1991
 *									
 */

#ifndef PKSCC_H

#include <stdio.h>

#ifndef DIM
#define DIM(x)		sizeof(x)/sizeof(x[0])
#endif

#define UNUSED(x)	(x) = (x)

typedef int BOOL;

extern char *		macro_loadStringResource(int nId);
extern void 		macro_showStatus(char *s, ...);

/*--------------------------------------------------------------------------
 * macro_printListHeader()
 */
extern void macro_printListHeader(FILE* fp, char* itemname);

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
extern int macro_readWriteWithFileSelection(int wrflag);

/*---------------------------------
 * macro_executeMacroByIndex()
 * Execute a macro given its logical
 * internal macro number.
 */
extern int macro_executeMacroByIndex(int macroindex);

/*
 * print the current mouse bindings to a file and display them to the user.
 */
extern int macro_saveMouseBindingsAndDisplay(void);

/*---------------------------------
 * macro_createFileAndDisplay()
 * Invoke a callback to generate the contents of a file with
 * the given file name. If successful, open the file in PKS Edit.
 */
extern BOOL macro_createFileAndDisplay(char* fn, long (*callback)(FILE* fp));

/*--------------------------------------------------------------------------
 * macro_isInCurrentDocumentContext()
 * Check, whether the passed document context identifier matches the document
 * context of the current open file.
 */
extern int 		macro_isInCurrentDocumentContext(int ctx);

/*--------------------------------------------------------------------------
 * macro_saveMenuBindingsAndDisplay()
 */
extern int macro_saveMenuBindingsAndDisplay(void);

extern int macro_readWriteWithFileSelection(int wrflag);

/*------------------------------------------------------------
 * macro_readWriteWithFileSelection()
 */
extern int macro_readWriteWithFileSelection(int wrflag);

# ifdef _PKSMOD_H
extern EDXPORTS 	*edx;
# endif

#define PKSCC_H
#endif

