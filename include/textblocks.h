/*
 * textblocks.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: operations on selected text ranges.
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef TEXTBLOCKS_H
#include "documentmodel.h"
#include "winfo.h"

/*
 * PKS Edit command to write a block to a default file name.
 */
extern int EdBlockWrite(WINFO* wp);

/*
 * PKS Edit command to read a block from a default file name
 */
extern int EdBlockRead(WINFO* wp);

/*------------------------------
 * EdBlockCopyOrMove()
 * PKS Edit command to copy or move a block.
 *----------------------------*/
extern int EdBlockCopyOrMove(WINFO* wp, BOOL move);

/*
 * Move the current selection one line up / down depending on 'delta'.
 * If no selection exists, the current line is selected and moved.
 */
extern int bl_moveSelectionUpDown(long delta);

/*----------------------------
 * EdBlockDelete()
 * PKS Edit command to delete the current selection
 *----------------------------*/
extern int EdBlockDelete(WINFO* wp, int bSaveOnClip);

/*---------------------------------*
 * bl_writeToFile()
 * PKS Edit command to write a block to a file with the given name.
 *---------------------------------*/
extern int bl_writeToFile(WINFO* wp, char* fn);

/*----------------------------
 * bl_cutOrCopy()
 * PKS Edit command to cut / copy a block.
 * The flag is a combination of CUT_QUERYID, CUT_USEPP, CUT_APPND.
 *----------------------------*/
extern int bl_cutOrCopy(WINFO* wp, int flg, PASTE* pp);

/*
 * API to copy or cut selected text blocks passing the start and end marketer.
 * The flag is a combination of CUT_QUERYID, CUT_USEPP, CUT_APPND.
 */
extern int bl_cutOrCopyBlock(WINFO* wp, MARK* ms, MARK* me, int flg, PASTE* pp);

/*--------------------------------------------------------------------------
 * bl_getTextForClipboardNamed()
 */
extern char* bl_getTextForClipboardNamed(PASTE* pp, BOOL bDefaultClipboard);

/*--------------------------------------------------------------------------
 * bl_showClipboardList()
 * displays the clipboard list for the purpose of viewing or selecting or adding a named item
 * to the clipboard.
 */
typedef enum { SNCO_CREATE, SNCO_LIST, SNCO_SELECT } SELECT_NAMED_CLIPBOARD_ACTION;
char* bl_showClipboardList(SELECT_NAMED_CLIPBOARD_ACTION nOption);

/*---------------------------------
 * EdSyncSelectionWithCaret()
 * PKS Edit command to set the end of the current selection to the caret position.
 *---------------------------------*/
extern int EdSyncSelectionWithCaret(WINFO* wp, int flags);

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a paste buffer to a regular string buffer.
 */
extern unsigned char* bl_convertPasteBufferToText(
	unsigned char* pDestination, unsigned char* pDestinationEnd, PASTE* pPasteBuffer);

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a string buffer to a paste buffer. Return 1 if successful.
 */
extern int bl_convertTextToPasteBuffer(PASTE* bp, unsigned char* pText, unsigned char* pEnd, char cSeparator1, char cSeparator2, char cCr);

/*--------------------------------------------------------------------------
 * bl_free()
 * free a pastebuffer
 */
extern void bl_free(PASTE* buf);

/*--------------------------------------------------------------------------
 * bl_freePasteList()
 */
extern void bl_destroyPasteList(BOOL bUnlinkBufferFiles);

/*--------------------------------------------------------------------------
 * bl_deleteBufferNamed()
 * Delete a buffer with a given name and return 1 if successful and 0 otherwise.
 */
extern int bl_deleteBufferNamed(char* pszName);

/*---------------------------------
 * bl_setSelection()
 * Set the current selection on a given file.
 *---------------------------------*/
extern int bl_setSelection(WINFO* wp, LINE* lps, int cs, LINE* lpe, int ce);

/*----------------------------
 * bl_insertPasteBufFromFile()
 * Read a text block from a file with the given name
 * and insert it into the current document.
 *----------------------------*/
extern int bl_insertPasteBufFromFile(WINFO* wp, char* fn);

/*---------------------------------
 * bl_syncSelectionWithCaret()
 * Set the selection end to the passed caret. Use one of the MARK_XXXX flags
 * to modify the request to e.g. change to block selection type etc...
 * If pMarkSet != NULL, returns either "MARK_START" or "MARK_END" in pMarkSet depending
 * on whether the start or the end mark had been set.
 *---------------------------------*/
extern int bl_syncSelectionWithCaret(WINFO* fp, CARET* lpCaret, int flags, int* pMarkSet);

/*--------------------------------------------------------------------------
 * bl_lookupPasteBuffer()
 * Lookup a paste buffer given an id. If insert is 1, space occupied by a possibly existing paste
 * buffer is destroyed before the paste buffer is returned.
 */
extern PASTE* bl_lookupPasteBuffer(char* pszId, int insert, PASTE_LIST_TYPE tType, PASTELIST** header);

/*
 * Returns true, if the given ID represents the ID of the default (system) clipboard.
 */
extern BOOL bl_isDefaultClipboard(const char* pszId);

/*--------------------------------------------------------------------------
 * blcutbl()
 * cut out a block of text
 * and (opt) delete it
 * pp == 0: only delete block
 */
extern int bl_cutTextWithOptions(WINFO* wp, PASTE* pp, LINE* lnfirst, LINE* lnlast,
	int cfirst, int clast, int freeflg);

/*--------------------------------------------------------------------------
 * bl_readFileIntoPasteBuf()
 * Read the file 'fileName' and convert it to a PASTE buf data structure
 * given the line / record separator 'rs'.
 */
extern int bl_readFileIntoPasteBuf(PASTE* pb, FILE_READ_OPTIONS* pOptions);

/*----------------------------
 * bl_cutBlockInColumnMode()
 * Cut out a column shaped selection of text and optionally delete it
 *----------------------------*/
extern int bl_cutBlockInColumnMode(WINFO* wp, PASTE* pp, LINE* lnfirst, LINE* lnlast, int freeflg);

/*--------------------------------------------------------------------------
 * bl_writePasteBufToFile()
 * Write a paste buffer to the file named 'fn'.
 */
extern int bl_writePasteBufToFile(PASTE* pb, char* fn, int mode);

/*--------------------------------------------------------------------------
 * bl_paste()
 * paste a paste buffer (textblock).
 */
extern int bl_paste(PASTE* pb, WINFO* fp, LINE* lpd, int col, int colflg);

/*--------------------------------------------------------------------------
 * bl_join()
 */
extern int bl_join(PASTE* pd, PASTE* p2);

/*--------------------------------------------------------------------------
 * bl_cut()
 */
extern int bl_cut(WINFO* wp, PASTE* pp, LINE* l1, LINE* l2, int c1, int c2, int freeflg, int colflg);

/*--------------------------------------*/
/* bl_pastecol()					*/
/* paste a textcol					*/
/*--------------------------------------*/
extern int bl_pastecol(PASTE* pb, WINFO* wp, LINE* lpd, int col);

/*--------------------------------------------------------------------------
 * bl_addrbyid()
 */
extern 	PASTE* bl_addrbyid(char* pszId, int insert, PASTE_LIST_TYPE tType);

/*
 * Tries to return the text from the current selection in the passed buffer, assuming a maximum
 * of nCapacity characters to return. One can specify a maximum number of lines being put on
 * the clipboard if not the whole selected text is of interest or pass 0 here to get all.
 */
extern int bl_getSelectedText(WINFO* wp, char* pszBuf, int nMaxLines, size_t nCapacity);

	/*--------------------------------------------------------------------------
 * bl_append()
 * to append something to the passed paste buffer.
 */
extern int bl_append(WINFO* wp, PASTE* pb, LINE* lnfirst, LINE* lnlast, int cfirst, int clast);

/*---------------------------------*/
/* bl_pasteBlock()					*/
/*---------------------------------*/
extern int bl_pasteBlock(WINFO* wp, PASTE* buf, int colflg, int offset, int move);

/*
 * Save all named clipboards to external files. Invoked on exit.
 */
extern void bl_autosavePasteBuffers();

/*
 * Restore all paste buffers from external files.
 */
extern void bl_restorePasteBuffers();

/*--------------------------------------------------------------------------
 * bl_hideSelectionInCurrentWindow()
 * Hide the current selection in the current window.
 */
extern int bl_hideSelectionInCurrentWindow(WINFO* wp);

#define TEXTBLOCKS_H
#endif // !TEXTBLOCKS_H
