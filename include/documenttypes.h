/*
 * documenttypes.h
 *
 * Project: PKS Edit for Windows
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
 * doctypes_getDocumentTypeDescription()
 * Returns the properties for a document type (primarily for the purpose of displaying
 * it in a UI).
 */
extern BOOL doctypes_getDocumentTypeDescription(DOCUMENT_TYPE* llp,
	char** ppszId, char** ppszDescription, char** ppszMatch, char** ppszFname,
	char** ppszGrammar);

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
extern EDIT_CONFIGURATION* doctypes_getDocumentTypeDescriptor(DOCUMENT_TYPE* p, char** pDocumentTypeName);

/*--------------------------------------------------------------------------
 * doctypes_deleteDocumentType()
 * Deletes a given document type.
 */
extern void doctypes_deleteDocumentType(DOCUMENT_TYPE* llp);

/**
 * Cleanup: delete a document descriptor.
 */
extern BOOL doctypes_destroyDocumentType(DOCUMENT_TYPE* dt);

extern BOOL doctypes_destroyEditConfiguration(EDIT_CONFIGURATION* pConfiguration);

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if editConfiguration is NULL, use a descriptor from disc according to filename pattern match.
 * If a pszDocumentTypeName is passed, use that as the name of document type to use rather than trying to match the document
 * type from the file name.
 */
extern int  doctypes_assignDocumentTypeDescriptor(FTABLE* fp, EDIT_CONFIGURATION* pEditConfiguration, char *pszDocumentTypeName);

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * Try to reassign document type properties if document type properties were assigned
 * before. At this point in time we are able to determine a document type from the first line
 * of the document.
 */
extern int doctypes_reassignDocumentTypeDescriptor(FTABLE* fp);

/*--------------------------------------------------------------------------
 * doctypes_saveAllDocumentTypes()
 * Save all document types - pass the pointer to the "HEAD" of the doctype list.
 */
extern int doctypes_saveAllDocumentTypes(EDIT_CONFIGURATION* pChangedConfiguration, char* pFileName);

/*--------------------------------------------------------------------------
 * doctypes_getSelectableDocumentFileTypes()
 * Returns the list of document file types defined in PKS Edit in the format
 * that it can be passed on to the open file dialog for filtering of file types
 * (e.g. *.*|All Files|*.java|Java Files)
 */
extern void doctypes_getSelectableDocumentFileTypes(char* pszDest, int nMax);

/*--------------------------------------------------------------------------
 * doctypes_documentTypeChanged()
 * One document has changed - apply the changes to the current editor. If 'bSave' is
 * true, write out the document descriptors to the JSON config file.
 */
extern int doctypes_documentTypeChanged(int bSave);

/*--------------------------------------
 * Returns the default document type descriptor for situations,
 * where no document type descriptor was assigned to a file.
 */
extern EDIT_CONFIGURATION* doctypes_getDefaultDocumentTypeDescriptor();

/*
 * Find a grammar given the name of a programming language for which a grammar is found.
 * Several different type of syntax to specify the language name are supported:
 * - if the name starts with "lang-" the prefix is skipped.
 * - upper / lower case of language name is ignored.
 * - for every langugage a list of alternate names can be defined (PASCAL, DELPHI, PAS)
 */
#ifdef GRAMMAR_H
extern GRAMMAR* doctypes_findGrammarForLanguage(const char* pszGrammarName);
#endif

#endif

