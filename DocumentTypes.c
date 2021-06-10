/*
 * 
 * DocumentTypes.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS
 *
 * purpose: maintain "Major Mode Configuration Files", the in PKSEDIT
 *		  called RULER Files,
 *		  link configuration files to file name extensions
 *		  tapstop calculating functions
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "alloc.h"
#include "tos.h"
#include "lineoperations.h"
#include "project.h"
#include "fileselector.h"
#include "edierror.h"
#include "stringutil.h"
#include "pksedit.h"
#include "documenttypes.h"
#include "fileutil.h"
#include "editorconfiguration.h"
#include "winfo.h"

 /*
 * Description of one document type in PKS edit.
 */
typedef struct tagDOCUMENT_TYPE {
	/*
	 * Next Document Type - they are maintained in a linked list.
	 */
	struct tagDOCUMENT_TYPE *	ll_next;
	char				ll_name[32];
	// Description for file selector.
	char				ll_description[50];
	int					ll_ctx;
	int					ll_privateDocumentDescriptor;
	char				ll_documentDescriptorName[256];
	char   				ll_match[256];
	DOCUMENT_DESCRIPTOR * ll_documentDescriptor;
} DOCUMENT_TYPE;

extern void 	*prof_llinsert(void *Head, int size, char *group, 
						char *item, char **idata);
extern void 	prof_killsections(LPSTR pszFn, LPSTR pszSection);
extern int 	Mapread(int context, char *target);

extern char *	_datadir;

DOCUMENT_TYPE *doctypes_createDocumentType(DOCUMENT_TYPE *llp);

static DOCUMENT_TYPE *_linl;
static int	_ndoctypes;
FSELINFO 		_linfsel = {	"", "DEFAULT.LIN", "*.LIN" };

#define	LINSPACE			offsetof(DOCUMENT_DESCRIPTOR, ts)

/*--------------------------------------------------------------------------
 * doctypes_calculateTabStop()
 * calculate next Tabstop
 */
int doctypes_calculateTabStop(int col, DOCUMENT_DESCRIPTOR* l) {
	if (col < DIM(l->ts) && l->ts[col])
		return l->ts[col];

	while (++col < MAXLINELEN && !TABTHERE(l, col))
		;
	return col;
}

/*--------------------------------------------------------------------------
 * doctypes_calculateNextTabStop()
 * calculate next tabstop after the given column.
 */
int doctypes_calculateNextTabStop(int col, DOCUMENT_DESCRIPTOR* l) {
	if (col < DIM(l->ts) && l->ts[col]) {
		int i = l->ts[col];
		if (i != col) {
			return i;
		}
	}
	while (++col < MAXLINELEN && !TABTHERE(l, col))
		;
	return col;
}

/*--------------------------------------------------------------------------
 * InitTabStops()
 */
static void InitTabStops(DOCUMENT_DESCRIPTOR* lp)
{
	int i, ts;

	ts = 0;
	i = 0;
	for (;;) {
		while (!TABTHERE(lp, ts) && ts < MAXLINELEN)
			ts++;
		if (ts >= DIM(lp->ts))
			return;
		while (i <= ts) {
			if (i >= DIM(lp->ts))
				return;
			lp->ts[i] = ts;
			i++;
		}
		ts++;
	}
}

/*--------------------------------------------------------------------------
 * doctypes_initDocumentTypeDescriptor()
 */
void doctypes_initDocumentTypeDescriptor(DOCUMENT_DESCRIPTOR* lp, int ts) {
	int i, ind;

	lp->tabsize = ts;
	memset(lp->tbits, 0, sizeof(lp->tbits));

	for (i = 0, ind = ts; i < MAXLINELEN; i++) {
		if (i == ind) {
			TABPLACE(lp, i);
			ind += ts;
		}
	}
	InitTabStops(lp);
}

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
DOCUMENT_DESCRIPTOR* doctypes_createDefaultDocumentTypeDescriptor() {
	DOCUMENT_DESCRIPTOR* pDescriptor = calloc(1, sizeof *pDescriptor);

	pDescriptor->rmargin = 80;
	pDescriptor->tabsize = 8;
	pDescriptor->shiftwidth = 4;
	pDescriptor->dispmode = WM_INSERT;
	pDescriptor->cr = '\r';
	pDescriptor->nl = '\n';
	pDescriptor->t1 = ' ';
	strcpy(pDescriptor->name, "default.lin");
	strcpy(pDescriptor->statusline, "0x%6p$O: 0x%2p$C 0%h$C");
	pDescriptor->fnt.height = 15;
	pDescriptor->fnt.width = 7;
	strcpy(pDescriptor->fnt.name, "consolas");
	doctypes_initDocumentTypeDescriptor(pDescriptor, 8);
	return pDescriptor;
}

/*--------------------------------------------------------------------------
 * doctypes_toggleTabStop()
 */
void doctypes_toggleTabStop(DOCUMENT_DESCRIPTOR *linp, int col)
{
	if (TABTHERE(linp,col))
		TABCLEAR(linp,col);
	else
		TABPLACE(linp,col);

	InitTabStops(linp);

}

/*--------------------------------------------------------------------------
 * GetRelatedFileName()
 */
static void GetRelatedFileName(char *related_name, char *fname, char *newext)
{	char *ext;

	strcpy(related_name,fname);
	ext = string_getFileExtension(related_name);
	if (ext[-1] != '.')
		*ext++ = '.';
	strcpy(ext,newext);
}

/*--------------------------------------------------------------------------
 * doctypes_getSelectableDocumentFileTypes()
 * Returns the list of document file types defined in PKS Edit in the format
 * that it can be passed on to the open file dialog for filtering of file types
 * (e.g. *.*|All Files|*.java|Java Files)
 */
void doctypes_getSelectableDocumentFileTypes(char* pszDest, int nMax) {
	LPSTR		pszEnd;
	DOCUMENT_TYPE *	llp;
	int			nCopied;

	pszEnd = pszDest + nMax - 2;
	for (nCopied = 0, llp = _linl; llp != 0; llp = llp->ll_next) {
		if (nCopied > 0) {
			*pszDest++ = '|';
		}
		if (llp->ll_match[0]) {
			pszDest = strmaxcpy(pszDest, 
					llp->ll_description[0] ? 
					llp->ll_description : llp->ll_match, 
					(int)(pszEnd - pszDest));
			*pszDest++ = '|';
			pszDest = strmaxcpy(pszDest, llp->ll_match, 
					(int)(pszEnd - pszDest));
			nCopied++;
		}
	}
}

/*--------------------------------------------------------------------------
 * doctypes_readDocumentType()
 */
static int doctypes_readDocumentType(char* fname, DOCUMENT_DESCRIPTOR* lp, int id, int forced)
{
	char 	keyfn[512];
	char* fn;
	int  	fd;

	if ((fn = file_searchFileInPKSEditLocation(fname)) != 0L && (fd = Fopen(fn, 0)) > 0) {
		if (Fread(fd, LINSPACE, lp) != LINSPACE) {
			Fclose(fd);
			return 0;
		}
		Fclose(fd);
		string_concatPathAndFilename(keyfn, _datadir, "MODI.XXX");
		doctypes_createTempFileForDocumentType(fn, keyfn);
	}
	else {
		return 0;
	}

	string_splitFilename(fname, (char*)0, lp->name);
	lp->id = id;
	InitTabStops(lp);

	if (keyfn[0] != 0) {
		Mapread(id, keyfn);
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * LoadDocumentTypeDescriptor()
 */
static DOCUMENT_DESCRIPTOR *LoadDocumentTypeDescriptor(DOCUMENT_TYPE *llp)
{
	DOCUMENT_DESCRIPTOR *lp;

	if ((lp = llp->ll_documentDescriptor) == 0) {
		if ((lp = doctypes_createDefaultDocumentTypeDescriptor()) == 0) {
			return 0;
		}
		llp->ll_documentDescriptor = lp;
		doctypes_readDocumentType(llp->ll_documentDescriptorName, lp, llp->ll_ctx, 0);
		lstrcpy(llp->ll_documentDescriptor->modename, llp->ll_name);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * doctypes_countDocumentTypes()
 */
int doctypes_countDocumentTypes(void)
{
	return ll_size((LINKED_LIST*)_linl);
}

/*--------------------------------------------------------------------------
 * doctypes_addDocumentTypesToListBox()
 */
int doctypes_addDocumentTypesToListBox(HWND hwnd, int nItem)
{
	DOCUMENT_TYPE *		llp;
	int			nCnt;

	for (llp = _linl, nCnt = 0; llp != 0; llp = llp->ll_next) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LPARAM)llp);
		nCnt++;
	}
	return nCnt;
}

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescription()
 */
BOOL doctypes_getDocumentTypeDescription(DOCUMENT_TYPE *llp, 
	char **ppszId,	char **ppszDescription, char **ppszMatch, char **ppszFname, 
	int **pOwn)
{

	if (!llp) {
		llp = _linl;
	}
	if (!llp) {
		doctypes_createDocumentType((DOCUMENT_TYPE *)0);
		llp = _linl;
	}
	if (ppszId) {
		*ppszId = llp->ll_name;
	}
	if (ppszDescription) {
		*ppszDescription = llp->ll_description;
	}
	if (ppszMatch) {
		*ppszMatch = llp->ll_match;
	}	
	if (ppszFname) {
		*ppszFname = llp->ll_documentDescriptorName;
	}
	if (pOwn) {
		*pOwn = &llp->ll_privateDocumentDescriptor;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * doctypes_createTempFileForDocumentType()
 */
int doctypes_createTempFileForDocumentType(char *linfn, char *tmpfn)
{
	int		fd;
	int		fd2;
	long		size;

	if ((fd = Fopen(linfn, OF_READ)) < 0) {
		return 0;
	}

	if ((fd2 = Fcreate(tmpfn, 0)) < 0) {
		tmpfn[0] = 0;
	} else if (Fseek(LINSPACE + 1, fd, 0) > LINSPACE) {
		Fseek(LINSPACE, fd, 0);
		while((size = Fread(fd, FBUFSIZE, _linebuf)) > 0) {
			Fwrite(fd2, size, _linebuf);
		}
	}
	if (fd2 > 0) {
		Fclose(fd2);
	}
	Fclose(fd);
	return tmpfn[0] ? 1 : 0;
}

/*--------------------------------------------------------------------------
 * doctypes_mergeDocumentTypes()
 */
int doctypes_mergeDocumentTypes(char *pszLinealFile, char *pszDocMacFile)
{
	HFILE		fd;
	int			fdDocMac;
	HFILE		fdTmp;
	long		size;
	char		tmpfn[512];

	if ((fd = Fopen(pszLinealFile, OF_READ)) < 0) {
		return 0;
	}

	size = Fread(fd, LINSPACE, _linebuf);
	Fclose(fd);
	if (size != LINSPACE) {
		return 0;
	}

	/* copy document descriptor to tempfile */
	file_getTempFilename(tmpfn, 'K');
	if ((fdTmp = Fcreate(tmpfn, 0)) < 0) {
		return 0;
	}
	Fwrite(fdTmp, LINSPACE, _linebuf);

	if ((fdDocMac = Fopen(pszDocMacFile, OF_READ)) < 0) {
		Fclose(fdTmp);
		return 0;
	}

	/* copy extended document macros to tempfile */
	while((size = Fread(fdDocMac, FBUFSIZE, _linebuf)) > 0) {
		Fwrite(fdTmp, size, _linebuf);
	}

	Fclose(fdDocMac);
	Fclose(fdTmp);

	if ((fdTmp = Fopen(tmpfn, OF_READ)) < 0) {
		return 0;
	}

	if ((fd = Fcreate(pszLinealFile, 0)) < 0) {
		return 0;
	}

	/* copy extended document macros to tempfile */
	while((size = Fread(fdTmp, FBUFSIZE, _linebuf)) > 0) {
		Fwrite(fd, size, _linebuf);
	}

	Fclose(fd);
	Fclose(fdTmp);

	Fdelete(tmpfn);

	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_getFileDocumentType()
 * find the correct document descriptor for a given file
 * 	1. if own document descriptor, try to read  own document descriptor from disc
 * 	2. if common document descriptor, ...
 *	3. if neither, use standard document descriptor
 */
BOOL doctypes_getFileDocumentType(DOCUMENT_DESCRIPTOR *linp, char *filename) {
	char 			fname[1024];
	char			linealname[1024];
	DOCUMENT_TYPE *		llp;
	DOCUMENT_DESCRIPTOR *		lp;
	PROJECTITEM *	pip;

	if ((pip = proj_finditem(filename)) != 0 &&
		pip->pi_doctype != 0) {
		lp = LoadDocumentTypeDescriptor((DOCUMENT_TYPE *)pip->pi_doctype);
	} else {

		string_splitFilename(filename,(char *)0, fname);
		for (llp = _linl, lp = 0; llp != 0 && lp == 0; llp = llp->ll_next) {
			if (string_matchFilename(fname,llp->ll_match)) {
				if (llp->ll_privateDocumentDescriptor) {
					GetRelatedFileName(filename,linealname,"LIN");
					if (doctypes_readDocumentType(linealname,linp,llp->ll_ctx,0) != 0) {
						lstrcpy(linp->modename, llp->ll_name);
						return 1;
					}
				}
				if ((lp = LoadDocumentTypeDescriptor(llp)) == 0) {
					break;
				}
			}
		}
	}
	if (!lp) {
		DOCUMENT_DESCRIPTOR* defaultLin = doctypes_createDefaultDocumentTypeDescriptor();
		memmove(linp, defaultLin, sizeof *defaultLin);
		free(defaultLin);
	} else {
		memmove(linp, lp, sizeof * lp);
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if documentDescriptor == 0, read document descriptor from disc according to filename pattern
 * match
 */
int  doctypes_assignDocumentTypeDescriptor(FTABLE *fp, DOCUMENT_DESCRIPTOR *documentDescriptor)
{
	if ((fp->documentDescriptor  = malloc(sizeof *fp->documentDescriptor)) == 0)
		return 0;

	if (documentDescriptor) {
		memmove(fp->documentDescriptor,documentDescriptor,sizeof *documentDescriptor);
		return 1;
	}

	doctypes_getFileDocumentType(fp->documentDescriptor,fp->fname);
	return 1;
}

/*--------------------------------------------------------------------------
 * linname2id()
 */
#if 0
int linname2id(char *name)
{	
	LINLIST	*llp;

	name = string_getBaseFilename(name);

	if (strcmp(name,"default") == 0)
		return LIN_DEFCTX;

	if ((llp = ll_find(_linl,name)) != (LINLIST *) 0) {
		return llp->ll_ctx;
	}

	return DOCUMENT_DESCRIPTOR_NO_CTX;
}
# endif

/*--------------------------------------------------------------------------
 * doctypes_saveDocumentType()
 * save a document type: name + linealfile + match-extensions
 */
static char *szDocTypes = "doctypes";
static int doctypes_saveDocumentType(DOCUMENT_TYPE *lp)
{
	char		szBuf[1024];

	wsprintf(szBuf,"%s,%s,%d,%s", lp->ll_documentDescriptorName, lp->ll_match, 
		lp->ll_privateDocumentDescriptor, lp->ll_description);
	return prof_savestring(szDocTypes,lp->ll_name,szBuf);
}

/*--------------------------------------------------------------------------
 * doctypes_saveAllDocumentTypes()
 * Save all document types - pass the pointer to the "HEAD" of the doctype list.
 */
void doctypes_saveAllDocumentTypes(DOCUMENT_TYPE *llp)
{
	/*
	 * recursive save of ruler list. called with param == 0 for start of saving
	 */
	if (!llp) {
		prof_killsections((LPSTR)0, szDocTypes);
		doctypes_saveAllDocumentTypes(_linl);
	} else {
		if (llp->ll_next) {
			doctypes_saveAllDocumentTypes(llp->ll_next);
		}
		doctypes_saveDocumentType(llp);
	}
}

/*--------------------------------------------------------------------------
 * doctypes_createDocumentType()
 */
DOCUMENT_TYPE *doctypes_createDocumentType(DOCUMENT_TYPE *llp)
{
	DOCUMENT_TYPE * llpNew;
	int		nLen;

	if ((llpNew = (DOCUMENT_TYPE*)ll_insert((LINKED_LIST**)&_linl, sizeof *llpNew)) == 0) {
		return 0;
	}
	if (llp) {
	/* do not copy link pointer ! */
		memmove(&llpNew->ll_name, &llp->ll_name, 
			sizeof *llpNew - sizeof llpNew->ll_next);
	}
	llpNew->ll_documentDescriptor = 0;
	if ((nLen = lstrlen(llpNew->ll_name)) < sizeof(llpNew->ll_name) - 2) {
		llpNew->ll_name[nLen++] = '*';
		llpNew->ll_name[nLen] = 0;
	}
	return llpNew;
}

/**
 * Cleanup: delete a document descriptor.
 */
static BOOL _DeleteDocumentType(DOCUMENT_TYPE* dt) {
	if (dt->ll_privateDocumentDescriptor && dt->ll_documentDescriptor) {
		free(dt->ll_documentDescriptor);
	}
	return TRUE;
}

/**
 * Deletes and de-allocates all known document types. 
 */
void doctypes_deleteAllDocumentTypes() {
	ll_destroy((LINKED_LIST**)&_linl, _DeleteDocumentType);
}

/*--------------------------------------------------------------------------
 * doctypes_deleteDocumentType()
 * Deletes a given document type.
 */
void doctypes_deleteDocumentType(DOCUMENT_TYPE *llp)
{
	ll_delete((LINKED_LIST**)&_linl, llp);
}

/*--------------------------------------------------------------------------
 * doctypes_getPrivateDocumentType()
 * 
 * Return the private document type given the name of the document type.
 */
DOCUMENT_TYPE* doctypes_getPrivateDocumentType(char *name)
{
	DOCUMENT_TYPE *	llp;

	for (llp = _linl; llp; llp = llp->ll_next) {
		if (lstrcmp(llp->ll_name, name) == 0) {
			return llp;
		}
	}

	return (DOCUMENT_TYPE*)_linl;
}

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescriptor()
 */
DOCUMENT_DESCRIPTOR *doctypes_getDocumentTypeDescriptor(DOCUMENT_TYPE*p)
{
	DOCUMENT_TYPE	*llp;

	if ((llp = (DOCUMENT_TYPE *)p) == 0) {
		return 0;
	}
	return LoadDocumentTypeDescriptor(llp);
}

/*--------------------------------------------------------------------------
 * doctypes_initDocumentType()
 * init a document type
 */
static intptr_t doctypes_initDocumentType(char *docname, long unused)
{
	char	 	*s,*szDesc,*szLinname,*szMatch,*szOwn;
	DOCUMENT_TYPE	*llp;

	if ((llp = prof_llinsert(&_linl,sizeof *llp,szDocTypes,docname,&s)) == 0) {
		return 0;
	}

	/*
	 * if new inserted: generate context id
	 */
	if (llp->ll_ctx == 0)
		llp->ll_ctx = ++_ndoctypes;

	szLinname = strtok(s,",");
	szMatch = strtok((char*)0,",");
	szOwn = strtok((char*)0,",");
	szDesc = strtok((char *)0, ",");

	if (szOwn != 0) {
		/*
		 * if document descriptor name has changed for wellknown document,
		 * force reload of document descriptor
		 */
		if (llp->ll_documentDescriptorName[0] &&
		    strcmp(llp->ll_documentDescriptorName,szLinname)) {
			destroy(&llp->ll_documentDescriptor);
		}
		strcpy(llp->ll_documentDescriptorName,szLinname);
		strcpy(llp->ll_match,szMatch);
		if (szDesc) {
			strmaxcpy(llp->ll_description, szDesc, sizeof llp->ll_description);
		}
		llp->ll_privateDocumentDescriptor = string_convertToLong(szOwn);
	}
	_free(s);
	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_initAllDocumentTypes()
 * init all document types
 */
int doctypes_initAllDocumentTypes(void)
{
	return prof_enum(szDocTypes,doctypes_initDocumentType,0L);
}

/*--------------------------------------------------------------------------
 * doctypes_readWriteDocumentDescriptor()
 * Read or write the passed document descriptor to a file. wrflag & 2 -> documentDescriptor is given as par
 */
int doctypes_readWriteDocumentDescriptor(int wrflag, DOCUMENT_DESCRIPTOR *documentDescriptor) {
	int 		fd;
	DOCUMENT_TYPE 	*llp;
	FTABLE 	*fp = ft_getCurrentDocument();
	char 	*fn;

	if ((wrflag & 2) == 0 && fp) {
		documentDescriptor = fp->documentDescriptor;
	}

	if (!documentDescriptor)
		return 0;

	if (wrflag & 1) {
		strcpy(_linfsel.fname, documentDescriptor->name);
		if (fp) {
			ww_getstate(fp->wp,&documentDescriptor->placement);
		}
	}

	BOOL saveAs = wrflag & 1;
	if ((fn = fsel_selectFileWithOptions(&_linfsel,saveAs ? MMWRITE : MMREAD, saveAs)) == 0) {
		return 0;
	}

	if (saveAs) {
		if ((fd = Fopen(fn, OF_READWRITE)) < 0 && (fd = file_createFile(fn)) < 0) {
			return 0;
		}
	   	string_splitFilename(fn,(char *)0,documentDescriptor->name);
		Fwrite(fd,LINSPACE,documentDescriptor);
		Fclose(fd);

		/*
		 * use as prototype
		 */
		for (llp = _linl; llp; llp = llp->ll_next) {
			if (llp->ll_documentDescriptor && lstrcmp(llp->ll_documentDescriptorName, documentDescriptor->name) == 0) {
				memmove(llp->ll_documentDescriptor,documentDescriptor,sizeof *documentDescriptor);
               }
		}
		return 1;
	}

	if (doctypes_readDocumentType(fn,documentDescriptor,documentDescriptor->id,1)) {
		if ((wrflag & 2) == 0)
			doctypes_documentTypeChanged();
		return 1;
	}

	return 0;
}
