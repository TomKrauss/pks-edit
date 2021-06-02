/*
 * documenttypes.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: describes the PKS Edit document types
 *
 * 										created: 15.03.91
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#ifndef DOCUMENTTYPES_H
#define DOCUMENTTYPES_H

typedef struct tagDOCUMENT_TYPE DOCUMENT_TYPE;

/*--------------------------------------------------------------------------
 * InitDocumentTypeDescriptor()
 */
extern void InitDocumentTypeDescriptor(DOCUMENT_DESCRIPTOR* pDescriptor, int tabSize);

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
extern DOCUMENT_DESCRIPTOR* CreateDefaultDocumentTypeDescriptor();

/*--------------------------------------------------------------------------
 * ToggleTabStop()
 */
extern void ToggleTabStop(DOCUMENT_DESCRIPTOR* pDescriptor, int column);

/*--------------------------------------------------------------------------
 * CountDocumentTypes()
 */
extern int CountDocumentTypes(void);

extern BOOL GetDocumentTypeDescription(DOCUMENT_TYPE* llp,
	char** ppszId, char** ppszDescription, char** ppszMatch, char** ppszFname,
	int** pOwn);

extern int CreateTempFileForDocumentType(char* linfn, char* tmpfn);

extern int MergeDocumentTypes(char* pszLinealFile, char* pszDocMacFile);

/*--------------------------------------------------------------------------
 * GetFileDocumentType()
 * find the correct document descriptor for a given file
 * 	1. if own document descriptor, try to read  own document descriptor from disc
 * 	2. if common document descriptor, ...
 *	3. if neither, use standard document descriptor
 */
extern BOOL GetFileDocumentType(DOCUMENT_DESCRIPTOR* linp, char* filename);

/*--------------------------------------------------------------------------
 * CreateDocumentType()
 */
extern DOCUMENT_TYPE* CreateDocumentType(DOCUMENT_TYPE* llp);

/*--------------------------------------------------------------------------
 * GetPrivateDocumentType()
 *
 * Return the private document type given the name of the document type.
 */
extern DOCUMENT_TYPE* GetPrivateDocumentType(char* name);

/*--------------------------------------------------------------------------
 * GetDocumentTypeDescriptor()
 */
extern DOCUMENT_DESCRIPTOR* GetDocumentTypeDescriptor(DOCUMENT_TYPE* p);

/**
 * Deletes and de-allocates all known document types.
 */
extern void DeleteAllDocumentTypes();

/*--------------------------------------------------------------------------
 * DeleteDocumentType()
 * Deletes a given document type.
 */
extern void DeleteDocumentType(DOCUMENT_TYPE* llp);

/*--------------------------------------------------------------------------
 * AssignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if documentDescriptor == 0, read document descriptor from disc according to filename pattern
 * match.
 */
extern int  AssignDocumentTypeDescriptor(FTABLE* fp, DOCUMENT_DESCRIPTOR* documentDescriptor);

#endif

