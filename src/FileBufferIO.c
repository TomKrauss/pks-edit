/*
 * FileBufferIO.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: reading and writing file buffers
 *
 * 										created: 10.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include <stdio.h>
#include "alloc.h"
#include "tos.h"
#include "trace.h"
#include "trace.h"
#include "documentmodel.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "linkedlist.h"
#include "pksmacro.h"
#include "winfo.h"

#include "pksedit.h"
#include "editorconfiguration.h"
#include "edexec.h"
#include "encryption.h"
#include "errordialogs.h"
#include "fileutil.h"
#include "documenttypes.h"
#include "mouseutil.h"

/* #define DEMO 1 /* D E M O V E R S I O N */

/*----- EXTERNALS -----------*/

extern void ft_settime(EDTIME* tp);
extern int  dlg_enterPasswordForEncryption(LPSTR password, LPSTR pszFilename, int twice);

int _flushing;

/*----- LOCALS --------------*/

static char _eof[] = "- eof -\n";
static int  _verbose = 1;
static int 	_scratchlen = 0;
static char _crypting;
static unsigned char* _scratchstart;

typedef struct tagMAGIC {
	char m_bytes[10];
	byte m_size;
	long m_codePage;
	BOOL m_crypted;
	BOM_TYPE m_bomType;
	char m_name[12];
} MAGIC;

static MAGIC _magicMarkers[] = {
	{"\0enc\03\07\01\0", 8, CP_ACP, TRUE, BOM_NONE, ""},
	{"\xEF\xBB\xBF", 3, CP_UTF8, FALSE, BOM_UTF8, "UTF8 BOM"},
	{"\xFE\xFF", 2, 1201, FALSE, BOM_UTF16BE, "UTF16 BOM-BE"},				// UTF-16 Big Endian
	{"\xFF\xFE", 2, 1200, FALSE, BOM_UTF16LE, "UTF16 BOM-LE"},				// UTF-16 Little Endian
	{"\x00\x00\xFE\xFF", 4, 12001, FALSE, BOM_UTF32BE, "UTF32 BOM-BE"},		// UTF-32 Big Endian
	{"\xFF\xFE\x00\x00", 4, 12000, FALSE, BOM_UTF32LE, "UTF16 BOM-LE"},		// UTF-32 Little Endian
	0
};

static MAGIC* _cryptMarker = &_magicMarkers[0];

// TODO: get rid of this hack.
char* _linebuf;

/*
 * Returns the name of a magic BOM type encoding. 
 */
char* ft_magicName(BOM_TYPE type) {
	MAGIC* pMagic = _magicMarkers;
	while (pMagic->m_size) {
		if (pMagic->m_bomType == type) {
			return pMagic->m_name;
		}
		pMagic++;
	}
	return NULL;
}

/*
 * Compare the start of a file (bytes in szMagic) with the well-known BOMs used to 
 * mark file encodings. If a BOM is present, return the MAGIC number structure with
 * the details.
 */
static MAGIC* ft_findMagic(char* szMagic, int nBytes) {
	MAGIC* pMagic = _magicMarkers;
	while (pMagic->m_size) {
		if (pMagic->m_size < nBytes && memcmp(szMagic, pMagic->m_bytes, pMagic->m_size) == 0) {
			return pMagic;
		}
		pMagic++;
	}
	return NULL;
}

/*
 * Find the the MAGIC number structure for a file originally read with the BOM described in pInfo
 * and with the codepage described.
 */
static MAGIC* ft_findMagicForCodePageinfo(CODE_PAGE_INFO* pInfo) {
	MAGIC* pMagic = _magicMarkers;
	while (pMagic->m_size) {
		if (pMagic->m_bomType == pInfo->cpi_bomType && pMagic->m_codePage == pInfo->cpi_codepage) {
			return pMagic;
		}
		pMagic++;
	}
	return NULL;
}
/*------------------------------*/
/* ft_initializeReadWriteBuffers()			  */
/*------------------------------*/
BOOL ft_initializeReadWriteBuffers(void)
{
	if ((_linebuf = malloc(LINEBUFSIZE)) == 0) {
		return FALSE;
	}
	_scratchstart = _linebuf+FBUFSIZE;
	return TRUE;
}

/*
 * Release the scratch buffer as soon as we are done. 
 */
void ft_destroyCaches() {
	free(_linebuf);
	_linebuf = NULL;
}

/*
 * Determines the name of the code page of the file and writes the codepage name
 * to pszName, but only if there is enough space as described by nMaxNameLen.
 */
void ft_getCodepageName(FTABLE* fp, char* pszName, size_t nMaxNameLen) {
	long nCp = fp->codepageInfo.cpi_codepage;
	if (nCp == -1) {
		if (nMaxNameLen > 5) {
			strcpy(pszName, "auto");
		} else {
			*pszName = 0;
		}
		return;
	}
	CPINFOEX cpInfo;
	GetCPInfoEx(nCp, 0, &cpInfo);
	if (strlen(cpInfo.CodePageName) < nMaxNameLen) {
		strcpy(pszName, cpInfo.CodePageName);
	} else {
		*pszName = 0;
	}
}

/*---------------------------------
 * file_closeFile()
 * close a file handle and report an error if unsuccessful. 
/*---------------------------------*/
EXPORT int file_closeFile(int *fd) {
	int ret = 0;

	if (*fd > 0 && (ret = Fclose(*fd)) < 0) {
		error_displayGenericErrorNumber(-ret);
	}
	*fd = -1;
	return ret;
}

/*--------------------------------------------------------------------------
 * err_noDiskSpace()
 */
void err_noDiskSpace(void) {
	error_showErrorById(IDS_MSGNODISKSPACE);
}

/*--------------------------------------------------------------------------
 * err_writeErrorOcurred()
 */
void err_writeErrorOcurred(void)
{
	error_showErrorById(IDS_MSGWRITEERR);
}

/*--------------------------------------------------------------------------
 * err_openFailed()
 */
void err_openFailed(char *fname)
{
	error_openingFileFailed(fname,0);
}

/*
 * Read the file identified by the file descriptor into a single 0-terminated string and return it.
 * Returns NULL, if the file is too large and malloc fails. It is the responsibility of the caller
 * to free the memory returned.
 */
EXPORT char* file_readFileAsString(int fd) {
	size_t	bufSize = 0;
	size_t	bytesRead;
	size_t  nOffset = 0;
	char* pszBuf = NULL;

	while ((bytesRead = Fread(fd, FBUFSIZE, _linebuf)) > 0) {
		if (pszBuf == NULL) {
			bufSize = bytesRead;
			pszBuf = malloc(bufSize+1);
			if (pszBuf == NULL) {
				return NULL;
			}
		} else {
			bufSize += bytesRead;
			char* pszTemp = realloc(pszBuf, bufSize + 1);
			if (!pszTemp) {
				free(pszBuf);
				return NULL;
			}
			pszBuf = pszTemp;
		}
		memcpy(pszBuf+nOffset, _linebuf, bytesRead);
		nOffset = bytesRead;
	}
	if (pszBuf) {
		pszBuf[bufSize] = 0;
	}
	return pszBuf;
}

/*
 * Try to detect the code page of a document. Currently only UTF-8 detection is supported.
 */
static int file_detectCodepage(const char* pData, int nSize) {
	const unsigned char* str = (unsigned char*)pData;
	const unsigned char* end = str + nSize;
	unsigned int nLength, i;
	wchar_t ch;
	while (str != end) {
		unsigned char byte = *str++;
		if (byte <= 0x7F || str >= end) {
			/* 1 byte sequence: U+0000..U+007F */
			continue;
		}

		if (0xC2 <= byte && byte <= 0xDF) {
			nLength = 1;
		} else if (0xE0 <= byte && byte <= 0xEF) {
			nLength = 2;
		} else if (0xF0 <= byte && byte <= 0xF4) {
			nLength = 3;
		} else {
			continue;
		}

		if (str + nLength >= end) {
			/* truncated string or invalid byte sequence */
			return CP_ACP;
		}

		/* Check continuation bytes: bit 7 should be set, bit 6 should be
		 * unset (b10xxxxxx). */
		for (i = 0; i < nLength; i++) {
			if ((str[i] & 0xC0) != 0x80)
				return CP_ACP;
		}

		if (nLength == 1) {
			return CP_UTF8;
		}
		else if (nLength == 2) {
			/* 3 bytes sequence: U+0800..U+FFFF */
			ch = ((str[0] & 0x0f) << 12) + ((str[1] & 0x3f) << 6) +
				(str[2] & 0x3f);
			/* (0xff & 0x0f) << 12 | (0xff & 0x3f) << 6 | (0xff & 0x3f) = 0xffff,
			   so ch <= 0xffff */
			if (ch < 0x0800)
				return CP_ACP;

			/* surrogates (U+D800-U+DFFF) are invalid in UTF-8:
			   test if (0xD800 <= ch && ch <= 0xDFFF) */
			if ((ch >> 11) == 0x1b)
				return CP_ACP;
			return CP_UTF8;
		}
		else if (nLength == 3) {
			/* 4 bytes sequence: U+10000..U+10FFFF */
			ch = ((str[0] & 0x07) << 18) + ((str[1] & 0x3f) << 12) +
				((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
			if ((ch < 0x10000) || (0x10FFFF < ch))
				return CP_ACP;
			return CP_UTF8;
		}
		str += nLength;
	}
	return CP_ACP;
}

/*
 * Try to find out, whether a file has a BOM defining the code page of it.
 */
static long file_analyzeBom(unsigned char* pBuffer, int nSize, unsigned char** bufferEnd, BOM_TYPE* pType) {
	MAGIC* pMagic = ft_findMagic(pBuffer, nSize);
	if (pMagic) {
		*pType = pMagic->m_bomType;
		*bufferEnd = pBuffer + pMagic->m_size;
		return pMagic->m_codePage;
	}
	return -1;
}

/*---------------------------------
 * ft_readDocumentFromFile()
 * Standard implementation to read a file in PKS Edit given the file descriptor,
 * a callback method to invoked for each line read and an optional parameter (typically, but not neccessarily the filepointer itself) to
 * be parsed as the first argument to the callback.
 *---------------------------------*/
EXPORT int ft_readDocumentFromFile(int fd, CODE_PAGE_INFO *pCodepage, 
		unsigned char * (*lineExtractedCallback)(void *, EDIT_CONFIGURATION*, unsigned char *, unsigned char *), void *par) {
	int 	cflg,got,len;
	long	bufferSize;
	char	*q;
	unsigned char *pend;
	unsigned char *bufferStart;

	len = 0;
	bufferSize = 2*FBUFSIZE;
	bufferStart = &_linebuf[128];
	_scratchlen = 0;
	cflg = _crypting;
	_crypting = 0;
	while ((got = Fread(fd,bufferSize,bufferStart)) > 0) {
		if (cflg) {
			char buf[2] = { 0 };
			int cont = Fread(fd,1,buf);
			if (cont)
				Fseek(-1l,fd,1);
			if ((got = crypt_de(bufferStart, got, cont)) < 0) {
				error_showErrorById(IDS_MSGWRONGPW);
				return 0;
			}
		}
		if (pCodepage->cpi_codepage != CP_ACP) {
			if (pCodepage->cpi_codepage == -1) {
				pCodepage->cpi_codepage = file_analyzeBom(bufferStart, got, &bufferStart, &pCodepage->cpi_bomType);
				if (pCodepage->cpi_codepage == -1) {
					pCodepage->cpi_codepage = file_detectCodepage(bufferStart, got);
				}
			}
			if (pCodepage->cpi_codepage != CP_ACP) {
				wchar_t* pszDest = malloc(got * 2);
				int nConv = MultiByteToWideChar(pCodepage->cpi_codepage, MB_PRECOMPOSED, bufferStart, got, pszDest, got);
				char cDefault = '?';
				BOOL bUnsupported;
				got = WideCharToMultiByte(CP_ACP, 0, pszDest, nConv, bufferStart, nConv, &cDefault, &bUnsupported);
				pCodepage->cpi_hasUnsupportedChars |= bUnsupported;
				free(pszDest);
			}
		}
		pend = &bufferStart[got];

		if ((q = (*lineExtractedCallback)(par,doctypes_getDefaultDocumentTypeDescriptor(),&bufferStart[-len],pend)) == 0)
			return 0;
		if ((len = (int)(pend-q)) != 0) {
			if (len < 128) {
				bufferStart = _linebuf+128;
				bufferSize = 2*FBUFSIZE;
			} else {
				bufferStart = _linebuf+FBUFSIZE;
				bufferSize = FBUFSIZE;
			}
			memmove(&bufferStart[-len],q,len);
		}
	}
	_scratchstart 	= bufferStart;
	_scratchlen	= len;

	if (got < 0) {
		error_showErrorById(IDS_MSGREADERROR); 
		return 0;
	}
	return 1;
}

/*--------------------------------------
 * ft_initializeEncryption()
 * Checks, whether a file to read needs to be decrypted.
 * If not, return 0, return 1 otherwise.
 *--------------------------------------*/
static int ft_initializeEncryption(EDIT_CONFIGURATION *linp, char *pw, char* pszFilename, int twice)
{
	if ((linp->workmode & O_CRYPTED) == 0 ||
	    dlg_enterPasswordForEncryption(pw, pszFilename, twice) == 0 ||
	    crypt_init(DES_NOFINAL,pw) == 0) {
		pw[0] = 0;
		return 0;
	}
	mouse_setBusyCursor();
	return 1;
}

/*
 * Read the header of a file to auto-detect, whether is encrypted and to detect BOMs.
 * BOMs are analyzed only, if the codepage passed is -1 (autodetect charset).
 */
static void ft_handleMagic(int fd, EDIT_CONFIGURATION* documentDescriptor, long* pCodepage) {
	char szMagic[sizeof ((MAGIC*)0)->m_bytes];
	memset(szMagic, 0, sizeof szMagic);
	BOOL bHeaderHandled = FALSE;
	if (Fread(fd, sizeof szMagic, szMagic) >= 8) {
		MAGIC* pMagic = ft_findMagic(szMagic, sizeof szMagic);
		if (pMagic) {
			if ((*pCodepage == -1 || pMagic->m_crypted) && memcmp(szMagic, pMagic->m_bytes, pMagic->m_size) == 0) {
				*pCodepage = pMagic->m_codePage;
				if (pMagic->m_crypted) {
					documentDescriptor->workmode |= O_CRYPTED;
				}
				_llseek(fd, pMagic->m_size, SEEK_SET);
				bHeaderHandled = TRUE;
			}
		}
	}
	if (!bHeaderHandled) {
		_llseek(fd, 0, SEEK_SET);
	}
}

/*--------------------------------------*/
/* ft_readfile()						*/
/*--------------------------------------*/
EXPORT int ft_readfile(FTABLE *fp, EDIT_CONFIGURATION *documentDescriptor, long codepage, long fFileOffset)
{
	HANDLE			fileHandle;
	int				ret;
	int				nl;
	int				fd;
	register	char *	buf;
	char				pw[64];
	unsigned char *(*pExtractFunction)(FTABLE *par, EDIT_CONFIGURATION *linp, unsigned char *p, unsigned char *pend);

	fd = -1;
	fp->longLinesSplit = 0;
	_flushing = 0;
	fp->lpReadPointer = fp->firstl;
	int workmode = documentDescriptor ? documentDescriptor->workmode : 0;
	if (workmode & WM_WATCH_LOGFILE) {
		fp->flags |= F_WATCH_LOGFILE;
	} else {
		fp->flags &= ~F_WATCH_LOGFILE;
	}
	buf = _scratchstart;
	if (_verbose)
		mouse_setBusyCursor();

	pExtractFunction = ln_createMultipleLinesFromBuffer;
	if (documentDescriptor == 0) {
		documentDescriptor = doctypes_getDefaultDocumentTypeDescriptor();
	}

	if ((nl = documentDescriptor->nl) <= 0) {
		// split the contents of the file in fixed blocks (e.g. binary files)
		pExtractFunction = ln_createFromBuffer;
	}

	if (fp->flags & F_NEWFILE) {
nullfile:
		if (!ln_createAndAdd(fp,"",0,(nl < 0) ? LNNOTERM : 0)) {
			goto ret0;
		}
		ret = 1;
		fp->nlines = 1;
	} else {
		file_getFileAttributes(fp->fname, &fp->ti_created, &fp->ti_modified, NULL);
		fileHandle = CreateFile(fp->fname, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
		if (fileHandle == INVALID_HANDLE_VALUE) {
			if ((fd = file_openFile(fp->fname)) < 0) {
				ret = 0;
				goto readerr;
			}
			fp->flags |= F_RDONLY;
		} else {
			fd = (int)(uintptr_t)fileHandle;
		}
		if (codepage == -1) {
			codepage = documentDescriptor->codepage;
		}
		if (fFileOffset) {
			LARGE_INTEGER fileSize;
			if (!GetFileSizeEx(fileHandle, &fileSize) || fileSize.QuadPart < fFileOffset) {
				CloseHandle(fileHandle);
				// File was truncated.
				return 0;
			}
			if (_llseek(fd, fFileOffset, SEEK_SET) != fFileOffset) {
				CloseHandle(fileHandle);
				return 0;
			}
		} else {
			ft_handleMagic(fd, documentDescriptor, &codepage);
		}
		_crypting = FALSE;
		if (documentDescriptor->workmode & O_CRYPTED) {
			_crypting = ft_initializeEncryption(documentDescriptor, pw, fp->fname, 0);
			if (!_crypting) {
				return 0;
			}
		}
		fp->codepageInfo.cpi_codepage = codepage;
		if ((ret = ft_readDocumentFromFile(fd, &fp->codepageInfo, pExtractFunction, fp)) == 0) {
			goto readerr;
		}

		if (fp->nlines == 0 && _scratchlen == 0) {	/* 0-file glitch */
			goto nullfile;
		}
		if (_scratchlen) {					/* unterminated line */
			_flushing = 1;
			buf = _scratchstart;
			if (nl >= 0)
				*buf++ = nl;
			if ((*pExtractFunction)(fp,documentDescriptor, &_scratchstart[-_scratchlen],buf) == 0) {
ret0:			ret = 0;
				goto readerr;
			}
			fp->lpReadPointer->lflg |= LNNOTERM;
		}
	}

	if (!ln_createAndAdd(fp,_eof,sizeof _eof -2,LNNOTERM))
		goto ret0;

	if (fp->codepageInfo.cpi_hasUnsupportedChars) {
		error_showErrorById(IDS_UNSUPPORTED_CHARACTERS, string_getBaseFilename(fp->fname));
	} else if (fp->longLinesSplit > 0 && _verbose) {
		error_showErrorById(IDS_MSGWRAPPEDLINES, string_getBaseFilename(fp->fname));
		ft_setFlags(fp, fp->flags | F_CHANGEMARK);
	}

	fp->lastl = fp->lpReadPointer;
	fp->lpReadPointer = fp->firstl;
	doctypes_reassignDocumentTypeDescriptor(fp);
	fp->vcInfo = gi_getVersionInfo(fp->fname);

readerr:
	if (fd > 0) {
		fp->fileSize = _llseek(fd, 0, SEEK_END);
		if ((GetConfiguration()->options & O_LOCKFILES) == 0) {
			file_closeFile(&fd);
		}
		fp->lockFd = fd;
	}
	if (_verbose) {
		mouse_setDefaultCursor();
	}
	return ret;
}

/*
 * Calculate the name of the backup file to create for file fp. The pszResult
 * string must be big enough to hold EDMAXPATHLEN number of characters.
*/
void ft_getBackupFilename(FTABLE* fp, char* pszResult) {
	char* name = fp->fname;
	char* backupExtension = fp->documentDescriptor->backupExtension;
	if (GetConfiguration()->options & O_CREATE_BACKUP_IN_TEMP_PATH) {
		char* fname = string_getBaseFilename(name);
		string_concatPathAndFilename(pszResult, config_getPKSEditTempPath(), fname);
	}
	else {
		strcpy(pszResult, name);
	}
	if (backupExtension[0]) {
		strcat(pszResult, ".");
		strcat(pszResult, backupExtension);
	}
	else {
		strcat(pszResult, ".bak");
	}
}

/*--------------------------------------*/
/* ft_createBackupFile()					*/
/* create a Backup-File				*/
/*--------------------------------------*/
static void ft_createBackupFile(FTABLE* fp, char* pszBackupName) {	
	char* name = fp->fname;

	if (!CopyFile(name, pszBackupName, FALSE)) {
		// TODO: I18N
		error_displayAlertDialog("Cannot rename %s to create backup file %s, errno: %d", name, pszBackupName, errno);
	}
}

/*--------------------------------------------------------------------------
 * ft_flushBuffer()
 * Flush a resource file buffer writing out the number of specified bytes and
 * moving the ramainder of the unflushed data to the beginning of the buffer.
 * Return -2, if the disk is full, 1 upon success and 0 for other failures.
 */
int ft_flushBuffer(int fd, char* buffer, int size, int rest)
{	
	UINT lWritten;
	if (!size) {
		return 1;
	}
	lWritten = Fwrite(fd, size, buffer);
	if (lWritten != size) {
		if (GetLastError() == ERROR_DISK_FULL) {
			err_noDiskSpace();
			return -2;
		}
		else {
			err_writeErrorOcurred();
		}
	} else {
		if (rest) {
			memmove(buffer, &buffer[size], rest);
		}
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * ft_flushBufferAndCrypt()
 */
static int ft_flushBufferAndCrypt(int fd, int size, int rest, int cont, CODE_PAGE_INFO *pCodepageInfo, char *pw) {
	if (*pw) {
		int news;

		news = crypt_en(_linebuf,size,cont);
		if (cont && news != size) {
			// TODO: I18N and error handling
			error_displayAlertDialog("bad flushbuffer size");
			return 0;
		}
		size = news;
	}
	long codepage = pCodepageInfo->cpi_codepage;
	if (codepage != -1 && codepage != CP_ACP) {
		wchar_t* pwszMultiByteDest = malloc(size * 2);
		if (pwszMultiByteDest == NULL) {
			return 0;
		}
		int nConv = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _linebuf, size, pwszMultiByteDest, size);
		char cDefault = '?';
		int nNeeded = WideCharToMultiByte(codepage, 0, pwszMultiByteDest, nConv, NULL, 0, &cDefault, NULL);
		char* pszDest = malloc(nNeeded);
		if (pszDest == NULL) {
			free(pwszMultiByteDest);
			return 0;
		}
		WideCharToMultiByte(codepage, 0, pwszMultiByteDest, nConv, pszDest, nNeeded, &cDefault, NULL);
		int nRet = ft_flushBuffer(fd, pszDest, nNeeded, 0);
		free(pwszMultiByteDest);
		free(pszDest);
		if (rest) {
			memmove(_linebuf, &_linebuf[size], rest);
		}
		return nRet;

	}
	return ft_flushBuffer(fd, _linebuf, size, rest);
}

/*--------------------------------------------------------------------------
 * ft_getBasenameOf()
 */
static char* ft_getBasenameOf(FTABLE* fp) {
	return string_getBaseFilename(ft_visibleName(fp));
}

/*--------------------------------------*/
/* ft_writefileMode() 					*/
/*--------------------------------------*/
EXPORT int ft_writefileMode(FTABLE *fp, int flags)
{	int			offset;
	int			no;
	int			ret;
	LINE *		lp;
	char		backupFile[EDMAXPATHLEN];
	char		pw[64];
	int 		nl;
	int			cr;
	int			fd = -1;
	int			saveLockFd;
	int			nFlushResult = 0;
	EDIT_CONFIGURATION		*linp = fp->documentDescriptor;

#ifdef DEMO
	return 0;
#else
	BOOL quiet = flags & WFM_QUIET;
	if (!linp) {
		linp = doctypes_getDefaultDocumentTypeDescriptor();
	}

	if ((saveLockFd = fp->lockFd) > 0) {
		file_closeFile(&fp->lockFd);
	}
	lp = fp->firstl;
	if (! (fp->flags & (F_MODIFIED | F_NEWFILE | F_WFORCED | F_APPEND))) 
		return 1;
	nl = linp->nl;
	cr = linp->cr;
	ft_initializeEncryption(linp, pw, fp->fname, 1);

	if (!quiet) {
		char* printedFilename = ft_getBasenameOf(fp);
		error_showMessageInStatusbar((fp->flags & F_NEWFILE) ? IDS_MSGNEWFILE : IDS_MSGSAVEFILE,
				  printedFilename, fp->nlines);
		mouse_setBusyCursor();
	}
	if (!(flags & WFM_AUTOSAVING) &&
		linp->backupExtension[0] &&
	    !(fp-> flags & (F_NEWFILE | F_SAVEAS | F_APPEND | F_ISBACKUPPED))) {
		ft_getBackupFilename(fp, backupFile);
		ft_createBackupFile(fp, backupFile);
		fp->flags |= F_ISBACKUPPED;
	}

	if ( (fp->flags & F_APPEND) == 0 || 
	     ((fd = Fopen(fp->fname,2)) < 0)) {
		int crflags = 0;

		if (fp->flags & F_HIDDEN)
			crflags |= F_HIDDEN;
		fd = Fcreate(fp->fname, crflags);
		if (fd < 0) {
			file_makeFileReadWrite(fp->fname);
			fd = Fcreate(fp->fname, crflags);
		}
		if (fd < 0) {
			error_openingFileFailed(fp->fname,fd);
			ret = 100;
			goto wfail1;
		}
	} else {
		LONG flushSize;
		if ((flushSize = Fseek(0l,fd,2)) < 0L) {
			ret = 100;
			nFlushResult = -2;
			error_displayGenericErrorNumber(-flushSize);
			goto wfail1;
		}
	}
	fp->flags &= ~(F_APPEND|F_NEWFILE);
	offset = 0;
	if (pw[0]) {
		ft_flushBuffer(fd, _cryptMarker->m_bytes, _cryptMarker->m_size, 0);
	} else if (fp->codepageInfo.cpi_bomType != BOM_NONE) {
		MAGIC* pMagic = ft_findMagicForCodePageinfo(&fp->codepageInfo);
		if (pMagic != NULL) {
			ft_flushBuffer(fd, pMagic->m_bytes, pMagic->m_size, 0);
		}
	}

	// don't save last line
	while (lp != fp->lastl) {
		// Make sure we are not overriding end of _linebuf, which has size LINEBUFSIZE
		if ((no = offset+lp->len) < LINEBUFSIZE-2) {
			memmove(&_linebuf[offset],lp->lbuf,lp->len);
			offset = no;
			if (LINE_HAS_LINE_END(lp)) {
				if (cr >= 0 && LINE_HAS_CR(lp)) {
					_linebuf[offset++] = cr;
				}
				if (nl >= 0) {
					_linebuf[offset++] = nl;
				}
			}
			lp = lp->next;
		} else {
			no = (offset > 2*FBUFSIZE) ? 2*FBUFSIZE : offset & (~0xF);
			offset -= no;
			nFlushResult = ft_flushBufferAndCrypt(fd, no, offset, 1, &fp->codepageInfo, pw);
			if (nFlushResult < 1)
				goto wfail;
		}
	}
	nFlushResult = ft_flushBufferAndCrypt(fd, offset, 0, 0, &fp->codepageInfo, pw);
	if (nFlushResult < 1) {
	wfail:	
		ft_setFlags(fp, fp->flags | F_CHANGEMARK);
		ret = 100;
		goto wfail1;
	}
	ret = 0;
	if (!(flags & WFM_AUTOSAVING)) {
		ln_changeFlag(fp, fp->firstl, fp->lastl, LNMODIFIED, LNSAVED, 1);
		ln_changeFlag(fp, fp->firstl, fp->lastl, LNMODIFIED, LNUNDO_AFTER_SAVE, 0);
		ft_forAllViews(fp, render_repaintLineNumbers, NULL);
		ft_setFlags(fp, fp->flags & ~(F_CHANGEMARK | F_WFORCED));
		ft_settime(&fp->ti_saved);
		file_getFileAttributes(fp->fname, &fp->ti_created, &fp->ti_modified, NULL);
		fp->fileSize = _llseek(fd, 0, SEEK_END);
	}

wfail1:
	file_closeFile(&fd);
	if (saveLockFd > 0) {
		fp->lockFd = file_openFile(fp->fname);
	}
	if (!quiet)
		mouse_setDefaultCursor();
	if (ret) {
		if (nFlushResult == -2) {	/* delete Files, if Disk is full */
			Fdelete(fp->fname);
		}
		return 0;
	}
	return 1;
#endif
}

/*
 * Return TRUE if for the passed file a backup file had been created previously. 
 */
EXPORT BOOL ft_backupFileExists(FTABLE* fp) {
	char		backupFile[EDMAXPATHLEN];

	ft_getBackupFilename(fp, backupFile);
	return file_exists(backupFile) == 0;
}

/*---------------------------------*/
/* ft_writeFileWithAlternateName()			*/
/*---------------------------------*/
EXPORT int ft_writeFileWithAlternateName(FTABLE *fp)
{	char dirname[512],savefn[512],fname[512];
	int  ret;

	strcpy(savefn,fp->fname);
	string_splitFilename(savefn,dirname,fname);

	EDIT_CONFIGURATION* pDocumentDescriptor = fp->documentDescriptor;
	if (pDocumentDescriptor && pDocumentDescriptor->saveActionName[0]) {
		macro_executeByName(pDocumentDescriptor->saveActionName);
	}
	ret = ft_writefileMode(fp, 0);
	strcpy(fp->fname,savefn);
	return ret;
}

/*---------------------------------*/
/* ft_writefileAsWithFlags()					*/
/*---------------------------------*/
EXPORT int ft_writefileAsWithFlags(FTABLE *fp, char *fn, int fileFlags, BOOL bVerbose)
{	int ret;

	fp->lockFd = -1;
	int newFlags = (fileFlags | F_MODIFIED);
	if (!(fp->flags & F_APPEND)) {
		newFlags  |= F_SAVEAS;
	}
	ft_setFlags(fp, newFlags);
	ft_setOutputFilename(fp, fn);
	ret = ft_writefileMode(fp, bVerbose ? 0 : WFM_QUIET);
	return ret;
}

/*---------------------------------*/
/* read a file, which is never	*/
/* been redrawn on screen		*/
/* Macrofiles, ...				*/
/*---------------------------------*/
EXPORT int ft_readfileWithOptions(FTABLE *fp, FILE_READ_OPTIONS* pOptions) {
	int ret;

	fp->firstl = 0;
	fp->nlines = 0L;
	fp->nbytes = -1;
	fp->documentDescriptor = 0;
	fp->flags  = F_NORMOPEN;
	fp->lockFd = 0;
	strcpy(fp->fname, pOptions->fro_fileName);

	if (!pOptions->fro_useDefaultDocDescriptor && !doctypes_assignDocumentTypeDescriptor(fp,(EDIT_CONFIGURATION*)0, NULL))
		return 0;

	_verbose = 0;
	ret = ft_readfile(fp, fp->documentDescriptor, pOptions->fro_codePage, pOptions->fro_fileReadOffset);
	_verbose = 1;
	file_closeFile(&fp->lockFd);

	return ret;
}

