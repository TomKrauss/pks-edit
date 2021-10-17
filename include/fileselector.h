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

typedef struct tagFILE_SELECT_PARAMS {
	char* fsp_title;			// The title to display in the file selector
	char* fsp_inputFile;		// The initial file name (including path) to use
	char* fsp_namePatterns;		// The possible file name patterns for selection (*.*, *.c, ...)
	char* fsp_resultFile;		// The result file name will be copied here
	BOOL  fsp_saveAs;			// true to show a save as dialog
	BOOL  fsp_encryptedAvailable; // Whether the encryption checkbox should be shown.
	BOOL  fsp_encrypted;		// in and out parameter - whether the file should be saved using encryption. Only applicable to save as
} FILE_SELECT_PARAMS;

/*---------------------------------
 * fsel_selectFileWithOptions()
 * Select a file given an info data structure and a resource ID to be used as the title.
 *---------------------------------*/
extern char* fsel_selectFileWithOptions(FSELINFO* fp, int idTitle, FILE_SELECT_PARAMS* pFSP);
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
extern int fsel_selectFile(FILE_SELECT_PARAMS* pFSParams);

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
extern int fsel_selectFileWithTitle(int title, char* result, FILE_SELECT_PARAMS* pFSP);

#define FILESELECTOR_H

#endif // !FILESELECTOR_H

