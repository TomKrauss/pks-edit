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
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DOCUMENTTYPES_H
#define DOCUMENTTYPES_H

typedef struct tagDOCUMENT_TYPE DOCUMENT_TYPE;

/*--------------------------------------------------------------------------
 * doctypes_initAllDocumentTypes()
 * init all document types
 */
extern int doctypes_initAllDocumentTypes(void);

/**
 * Deletes and de-allocates all known document types.
 */
extern void doctypes_destroyAllDocumentTypes();

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
extern EDIT_CONFIGURATION* doctypes_createDefaultDocumentTypeDescriptor();

/*--------------------------------------------------------------------------
 * doctypes_countDocumentTypes()
 */
extern int doctypes_countDocumentTypes(void);

extern BOOL doctypes_getDocumentTypeDescription(DOCUMENT_TYPE* llp,
	char** ppszId, char** ppszDescription, char** ppszMatch, char** ppszFname,
	int** pOwn);

/*--------------------------------------------------------------------------
 * doctypes_getFileDocumentType()
 * find the correct document descriptor for a given file
 * 	1. if own document descriptor, try to read  own document descriptor from disc
 * 	2. if common document descriptor, ...
 *	3. if neither, use standard document descriptor
 */
extern BOOL doctypes_getFileDocumentType(EDIT_CONFIGURATION* linp, char* filename);

/*--------------------------------------------------------------------------
 * doctypes_createDocumentType()
 */
extern DOCUMENT_TYPE* doctypes_createDocumentType(DOCUMENT_TYPE* llp);

/*--------------------------------------------------------------------------
 * doctypes_getPrivateDocumentType()
 *
 * Return the private document type given the name of the document type.
 */
extern DOCUMENT_TYPE* doctypes_getPrivateDocumentType(char* name);

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescriptor()
 */
extern EDIT_CONFIGURATION* doctypes_getDocumentTypeDescriptor(DOCUMENT_TYPE* p);

/*--------------------------------------------------------------------------
 * doctypes_deleteDocumentType()
 * Deletes a given document type.
 */
extern void doctypes_deleteDocumentType(DOCUMENT_TYPE* llp);

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if documentDescriptor == 0, read document descriptor from disc according to filename pattern
 * match.
 */
extern int  doctypes_assignDocumentTypeDescriptor(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor);

/*--------------------------------------------------------------------------
 * doctypes_saveAllDocumentTypes()
 * Save all document types - pass the pointer to the "HEAD" of the doctype list.
 */
extern int doctypes_saveAllDocumentTypes(char* pFileName);

/*--------------------------------------------------------------------------
 * doctypes_getSelectableDocumentFileTypes()
 * Returns the list of document file types defined in PKS Edit in the format
 * that it can be passed on to the open file dialog for filtering of file types
 * (e.g. *.*|All Files|*.java|Java Files)
 */
extern void doctypes_getSelectableDocumentFileTypes(char* pszDest, int nMax);

/*--------------------------------------------------------------------------
 * doctypes_documentTypeChanged()
 * One document has changed - apply the changes to the current editor.
 */
extern int doctypes_documentTypeChanged(void);

#endif

