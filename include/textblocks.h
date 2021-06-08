/*
 * TEXTBLOCKS.H
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: operations on textblocks.
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */


#ifndef TEXTBLOCKS_H
#include "lineoperations.h"

extern int _nundo;
extern PASTE* _undobuf;

/*
 * PKS Edit command to write a block to a default file name.
 */
extern int EdBlockWrite(void);

/*
 * PKS Edit command to read a block from a default file name
 */
extern int EdBlockRead(void);

/*------------------------------
 * EdBlockCopyOrMove()
 * PKS Edit command to copy or move a block.
 *----------------------------*/
extern int EdBlockCopyOrMove(BOOL move);

/*--------------------------------------------------------------------------
 * EdPasteString()
 * Paste a String at the current cursor location in the currently open window.
 */
extern int EdPasteString(long dummy1, long dummy2, char* string);

/*----------------------------
 * EdBlockDelete()
 * PKS Edit command to delete the current selection and
 * optionally save the text in the trashcan clipboard of PKS Edit
 *----------------------------*/
extern int EdBlockDelete(int bSaveTrash);

/*---------------------------------*
 * EdBlockWriteToFile()
 * PKS Edit command to write a block to a file with the given name.
 *---------------------------------*/
extern int EdBlockWriteToFile(char* fn);

/*----------------------------
 * EdBlockCut()
 * PKS Edit command to cut a block.
 *----------------------------*/
extern int EdBlockCut(int flg, PASTE* pp);

/*--------------------------------------------------------------------------
 * EdBufferFree()
 * PKS Edit command which releases all block buffers
 */
extern int EdBufferFree(void);

/*--------------------------------------------------------------------------
 * EdGetSelectedText()
 * PKS Edit command which gets the selected text and makes it available
 * to the macro interpreter.
 */
extern void EdGetSelectedText(void);

/*---------------------------------
 * EdSyncSelectionWithCaret()
 * PKS Edit command to set the end of the current selection to the caret position.
 *---------------------------------*/
extern int EdSyncSelectionWithCaret(int flags);

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a paste buffer to a regular string buffer.
 */
extern unsigned char* bl_convertPasteBufferToText(unsigned char* pDestination, unsigned char* pDestinationEnd,
	PASTE* pPasteBuffer);

/*--------------------------------------------------------------------------
 * bl_validateTrashCanName()
 * Validate / generate the name of a "logical" trash can in PKS edit, which may contain
 * data accessible under that name.
 */
void bl_validateTrashCanName(char* pszValid);

/*--------------------------------------------------------------------------
 * bl_free()
 * free a pastebuffer
 */
extern void bl_free(PASTE* buf);

/*--------------------------------------------------------------------------
 * bl_freePasteList()
 */
extern void bl_freePasteList(PASTELIST** header);

/*---------------------------------
 * bl_setSelection()
 * Set the current selection on a given file.
 *---------------------------------*/
extern int bl_setSelection(FTABLE* fp, LINE* lps, int cs, LINE* lpe, int ce);

/*----------------------------
 * bl_insertTextBlockFromFile()
 * Read a text block from a file with the given name
 * and insert it into the current document.
 *----------------------------*/
extern int bl_insertTextBlockFromFile(char* fn);

/*---------------------------------
 * bl_syncSelectionWithCaret()
 * Set the selection end to the passed caret. Use one of the MARK_XXXX flags
 * to modify the request to e.g. change to block selection type etc...
 *---------------------------------*/
extern int bl_syncSelectionWithCaret(FTABLE* fp, CARET* lpCaret, int flags);

/*--------------------------------------------------------------------------
 * blcutbl()
 * cut out a block of text
 * and (opt) delete it
 * pp == 0: only delete block
 */
extern int bl_cutTextWithOptions(PASTE* pp, LINE* lnfirst, LINE* lnlast,
	int cfirst, int clast, int freeflg);

/*---------------------------------
 * bl_defineColumnSelectionFromXY()
 * Given two screen points, determine the start and end column
 * of the current block shape selection.
 *---------------------------------*/
extern int bl_defineColumnSelectionFromXY(int x, int y, int nx, int ny);

/*----------------------------
 * bl_hideSelection()
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
extern int bl_hideSelection(int removeLineSelectionFlag);

/*--------------------------------------------------------------------------
 * bl_read()
 */
extern int bl_read(char* fn, PASTE* pb, int rs /* Record Seperator */);

/*----------------------------
 * bl_cutBlockInColumnMode()
 * Cut out a colomn shaped block of text and optionally delete it
 *----------------------------*/
extern int bl_cutBlockInColumnMode(PASTE* pp, LINE* lnfirst, LINE* lnlast, int freeflg);

/*--------------------------------------------------------------------------
 * bl_write()
 */
extern int bl_write(char* fn, PASTE* pb, int mode);

/*--------------------------------------------------------------------------
 * bl_paste()
 * paste a textblock
 */
extern int bl_paste(PASTE* pb, FTABLE* fp, LINE* lpd, int col, int colflg);

/*--------------------------------------------------------------------------
 * bl_hasClipboardBlock()
 * Check, whether the cut text block with the given number exists and can be inserted.
 */
extern BOOL bl_hasClipboardBlock(BOOL isTrashCan, int blockNumber);

/*--------------------------------------------------------------------------
 * bl_join()
 */
extern int bl_join(PASTE* pd, PASTE* p2);

/*--------------------------------------------------------------------------
 * bl_cut()
 */
extern int bl_cut(PASTE* pp, LINE* l1, LINE* l2, int c1, int c2, int freeflg, int colflg);

/*--------------------------------------------------------------------------
* 
 * bl_cut()
 */
extern int bl_cut(PASTE* pp, LINE* l1, LINE* l2, int c1, int c2, int freeflg, int colflg);

/*--------------------------------------*/
/* bl_pastecol()					*/
/* paste a textcol					*/
/*--------------------------------------*/
extern int bl_pastecol(PASTE* pb, FTABLE* fp, LINE* lpd, int col);

/*--------------------------------------------------------------------------
 * bl_delete()
 */
extern int bl_delete(FTABLE* fp, LINE* lnfirst, LINE* lnlast, int cfirst,
	int clast, int blkflg, int saveintrash);


/*--------------------------------------------------------------------------
 * bl_undoIntoUnqBuffer()
 * enqueue next Pastebuffer to undolist
 */
extern int bl_undoIntoUnqBuffer(LINE* lnfirst, LINE* lnlast, int cfirst, int clast, int blockflg);
/* pointer to first and last line to enq	*/
/* first column and last col to enq		*/

/*--------------------------------------------------------------------------
 * bl_addrbyid()
 */
extern 	PASTE* bl_addrbyid(int id, int insert);

/*--------------------------------------------------------------------------
 * bl_undoIntoUnqBuffer()
 * enqueue next Pastebuffer to undolist
 */
extern int bl_undoIntoUnqBuffer(LINE* lnfirst, LINE* lnlast, int cfirst, int clast, int blockflg);

/*--------------------------------------------------------------------------
 * bl_getBlockFromUndoBuffer()
 */
extern PASTE* bl_getBlockFromUndoBuffer(int num);

/*--------------------------------------------------------------------------
 * bl_hasClipboardBlock()
 */
extern int bl_hasClipboardBlock(int ubuf, int id);

/*--------------------------------------------------------------------------
 * bl_append()
 * to append something to the passed paste buffer.
 */
extern int bl_append(PASTE* pb, LINE* lnfirst, LINE* lnlast, int cfirst, int clast);

/*---------------------------------*/
/* bl_pasteBlock()					*/
/*---------------------------------*/
extern int bl_pasteBlock(PASTE* buf, int colflg, int offset, int move);

/*--------------------------------------------------------------------------
 * EdBlockHide()
 * Hide the current selection in the current file.
 */
extern int EdBlockHide(void);

#define TEXTBLOCKS_H
#endif // !TEXTBLOCKS_H
