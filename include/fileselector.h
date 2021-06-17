/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * fileselector.h
 *
 * all operations related to selecting a file.
 *
 * 						created: 07.06.91
 *						Author : Tom
 */

#ifndef FILESELECTOR_H

typedef int BOOL;

typedef struct fselinfo {
	char path[1024];
	char fname[256];
	char search[256];
} FSELINFO;

/*---------------------------------
 * fsel_selectFileWithOptions()
 * Select a file given an info data structure and a resource ID to be used as the title.
 *---------------------------------*/
extern char* fsel_selectFileWithOptions(FSELINFO* fp, int idTitle, BOOL showSaveDialog);
extern char* fsel_initPathes(FSELINFO* fp);
extern char	_fseltarget[];

/*
 * Select a folder using a browse for folder dialog. Return TRUE,
 * if the folder was selected. pResult will contain the resulting folder name.
 */
extern BOOL fsel_selectFolder(char* pTitle, char* pResult);

/*------------------------------------------------------------
 * fsel_selectFile
 * select a file with a file open dialog. If bSaveAs is true, the
 * dialog is opened for the purpose of saving files.
 */
extern int fsel_selectFile(char* szFileNameIn, char* szSuffixes, char* szFileNameOut, BOOL bSaveAs);

/*------------------------------------------------------------
 * fsel_setDialogTitle
 * setting the idTitle for the next call of fsel_selectFile
 */
extern void fsel_setDialogTitle(char* title);

/*--------------------------------------------------------------------------
 * fsel_changeDirectory()
 * Change the current directory. Allow for drive specification and trailing slash.
 */
extern void fsel_changeDirectory(char* pszPath);

/*------------------------------------------------------------
 * fsel_selectFileWithTitle()
 * Select a file given a resource id for the title and return the selected
 * filename in "result" (must be large enough to hold the pathname (EDMAXPATHLEN)).
 */
extern int fsel_selectFileWithTitle(int title, char* result, BOOL bSaveAs);

#define FILESELECTOR_H

#endif // !FILESELECTOR_H

