/*
 * MacroCFunctions.c
 *
 * Provides functions to be invoked from the PKSMacroC language.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author : TOM
 * created: 13.03.1991
 *
 */

#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <corecrt_io.h>
#include "arraylist.h"
#include "documentmodel.h"
#include "mainframe.h"
#include "winfo.h"
#include "pathname.h"
#include "textblocks.h"
#include "findandreplace.h"
#include "editorconfiguration.h"
#include "editoperations.h"
#include "caretmovement.h"
#include "stringutil.h"
#include "edctype.h"
#include "pksmacrocvm.h"
#include "fileutil.h"

static const char* consoleFileName = "console.log";

static const PKS_VALUE NIL = {.pkv_type = VT_NIL};

static int macroc_expectNumberOfArgs(int nExpected, int nArgs, const char* pszFunction) {
	if (nArgs < nExpected) {
		interpreter_raiseError("Invalid number of arguments (%d) for %s. Must pass at least %d arguments.", nArgs, pszFunction, nExpected);
		return 0;
	}
	return 1;
}

/*
 * Opens the console, if not yet open. 
 */
static FTABLE* macroc_openConsole() {
	char szCompleteFileName[EDMAXPATHLEN];

	string_concatPathAndFilename(szCompleteFileName, config_getPKSEditTempPath(), consoleFileName);
	FTABLE* fp = ft_fpbyname(szCompleteFileName);
	if (!fp) {
		fp = ft_openFileWithoutFileselector(szCompleteFileName, 0L, &(FT_OPEN_OPTIONS) {.fo_dockName = DOCK_NAME_RIGHT, .fo_isNewFile = 1});
		if (fp) {
			fp->flags |= F_RDONLY|F_TRANSIENT;
		}
	}
	return fp;
}

/*
 * Print a text to the PKS Edit console.
 */
static long long macroc_printToConsole(int bNewLine, const char* pszText) {
	FTABLE* fp = macroc_openConsole();
	if (!fp) {
		return 0;
	}
	if (!pszText) {
		pszText = "";
	}
	WINFO* wp = WIPOI(fp);
	PASTE pasteBuffer;
	memset(&pasteBuffer, 0, sizeof pasteBuffer);
	strcpy(_linebuf, pszText);
	strcat(_linebuf, bNewLine ? "\n\n" : "\n");
	if (bl_convertTextToPasteBuffer(&pasteBuffer, _linebuf, _linebuf + strlen(_linebuf), '\n', 0, 0)) {
		CARET oldCaret = wp->caret;
		bl_pasteBlock(wp, &pasteBuffer, 0, oldCaret.offset, 0);
		ln_removeFlag(fp->firstl, fp->lastl, LNMODIFIED);
		ft_setFlags(fp, fp->flags & ~F_MODIFIED);
		bl_free(&pasteBuffer);
	}
	return 1;
}

/*
 * Print a text including a trailing newline to the PKS Edit console.
 */
long long macroc_println(const char* pszText) {
	return macroc_printToConsole(1, pszText);
}

/*
 * Print a text to the PKS Edit console.
 */
long long macroc_print(const char* pszText) {
	return macroc_printToConsole(0, pszText);
}

/*
 * Empty the console.
 */
long long macroc_clearConsole() {
	char szCompleteFileName[EDMAXPATHLEN];

	string_concatPathAndFilename(szCompleteFileName, config_getPKSEditTempPath(), consoleFileName);
	FTABLE* fp = ft_fpbyname(szCompleteFileName);
	if (!fp) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);
	LINE* lpLast = fp->lastl->prev;
	caret_placeCursorInCurrentFile(wp, 0, 0);
	bl_delete(wp, fp->firstl, lpLast, 0, lpLast->len, 0, 0);
	ft_setFlags(fp, fp->flags & ~F_MODIFIED);
	wp->caret.linePointer = fp->firstl;
	wp->caret.ln = 0;
	render_repaintAllForFile(fp);
	return 1;
}

/*
 * Convert a text to upper / lower / toggle case.
 */
static char* macroc_convertChars(const char* pszString, CC_OPERATION operation) {
	char c;
	char* pszDest = _linebuf;
	char* pszEnd = &_linebuf[MAXLINELEN-3];
	while ((c = *pszString++) != 0 && pszDest < pszEnd) {
		char c1 = c;
		if (operation == CC_TOGGLE || operation == CC_UPPER) {
			c1 = _l2uset[c];
		}
		if ((c == c1 && operation == CC_TOGGLE) || operation == CC_LOWER) {
			c1 = _u2lset[c];
		}
		*pszDest++ = c1;
	}
	*pszDest = 0;
	return _linebuf;
}

char* macroc_toupper(const char* pszString) {
	return macroc_convertChars(pszString, CC_UPPER);
}


char* macroc_tolower(const char* pszString) {
	return macroc_convertChars(pszString, CC_LOWER);
}

/*
 * Tokenize a string and split into subtokens delimited by pszCharacters.
 */
ARRAY_LIST * macroc_stringTokenize(const char* pszString, const char* pszCharacters) {
	ARRAY_LIST* pResult = arraylist_create(7);
	if (!pszString) {
		return pResult;
	}
	if (!pszCharacters) {
		pszCharacters = " \t";
	}
	char* pszInput = _strdup(pszString);
	const char* pszNext = strtok(pszInput, pszCharacters);
	while (pszNext) {
		arraylist_add(pResult, _strdup(pszNext));
		pszNext = strtok(0, pszCharacters);
	}
	free(pszInput);
	return pResult;
}

/*
 * Implements the FileOpen() which opens a file for reading or writing.
 */
PKS_VALUE macroc_fileOpen(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (!macroc_expectNumberOfArgs(1, nArgs, "FileOpen")) {
		return NIL;
	}
	const char* pszFilename = memory_accessString(pValues[0]);
	const char* pszAccessMode = nArgs > 1 ? memory_accessString(pValues[1]) : "r";
	FILE* fp = fopen(pszFilename, pszAccessMode);
	if (!fp) {
		return NIL;
	}
	return memory_createHandleObject(pContext, VT_FILE, fp);
}

const char* macroc_accessString(PKS_VALUE v, int idx) {
	const char* pszResult = memory_accessString(v);
	if (!pszResult) {
		interpreter_raiseError("Expecting argument %d to be of type string.", idx);
		return "";
	}
	return pszResult;
}

/*
 * Implements the PathCreateFromSegments() which creates a windows path concatenating multiple path segments.
 */
PKS_VALUE macroc_pathCreateFromSegments(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	if (!macroc_expectNumberOfArgs(2, nArgs, "PathCreateFromSegments")) {
		return NIL;
	}
	int i = 0;
	const char* pszDir = macroc_accessString(pValues[0], 0);
	char* pszResult = malloc(EDMAXPATHLEN);
	if (pszResult == NULL) {
		return memory_createObject(pContext, VT_BOOL, 0, FALSE);
	}
	while (i < nArgs - 1) {
		const char* pszNext = macroc_accessString(pValues[i+1], i+1);
		string_concatPathAndFilename(_linebuf, pszDir, pszNext);
		strcpy(pszResult, _linebuf);
		pszDir = pszResult;
		i++;
	}
	PathCanonicalize(_linebuf, pszResult);
	free(pszResult);
	return memory_createObject(pContext, VT_STRING, 0, _linebuf);
}

/*
 * Implements the FileReadLine() method, which reads and returns one line of text from a given file (FileReadLine(fp)).
 */
PKS_VALUE macroc_fileReadLine(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	PKS_VALUE_TYPE t = VT_FILE;
	if (nArgs < 1 || pValues[0].pkv_type != t) {
		interpreter_raiseError("No file pointer passed to FileReadLine");
		return NIL;
	}
	FILE* fp = memory_handleForValue(pValues[0]);
	if (fp == 0) {
		interpreter_raiseError("0 filepointer used in FileReadLine");
	} else {
		if (fgets(_linebuf, MAXLINELEN, fp)) {
			_linebuf[strcspn(_linebuf, "\r\n")] = 0;
			return memory_createObject(pContext, VT_STRING, 0, _linebuf);
		}
	}
	return NIL;
}

/*
 * Implements the FileWriteLine() method, which writes one line of text to a given file (FileWriteLine(fp, s)).
 */
PKS_VALUE macroc_fileWriteLine(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	BOOL bSuccess = 0;
	if (!macroc_expectNumberOfArgs(2, nArgs, "FileWriteLine")) {
		return NIL;
	}
	PKS_VALUE_TYPE t = VT_FILE;
	if (pValues[0].pkv_type != t || pValues[1].pkv_type != VT_STRING) {
		interpreter_raiseError("No file pointer / no string to write passed to FileWriteLine");
	}
	else {
		FILE* fp = memory_handleForValue(pValues[0]);
		if (fp == 0) {
			interpreter_raiseError("0 filepointer used in FileWriteLine");
		}
		else {
			bSuccess = fputs(memory_accessString(pValues[1]), fp) >= 0;
			fputc('\n', fp);
		}
	}
	return (PKS_VALUE) {
		.pkv_type = VT_BOOL, .pkv_data.booleanValue = bSuccess
	};
}

/*
 * Implements the FileOpen() which opens a file for reading or writing.
 */
PKS_VALUE macroc_fileClose(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	macroc_expectNumberOfArgs(1, nArgs, "FileClose");
	PKS_VALUE_TYPE t = VT_FILE;
	if (pValues[0].pkv_type != t) {
		interpreter_raiseError("No file pointer passed to FileClose");
	}
	else {
		FILE* fp = memory_handleForValue(pValues[0]);
		if (fp) {
			fclose(fp);
			memory_setNestedPointer(pValues[0], 0, (MAKE_TYPED_OBJECT_POINTER(0, 0, 0)));
		}
	}
	return (PKS_VALUE) {
		.pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = 1
	};
}

/*
 * Determine the index of one object in another. Supported combinations: search any type of object in an array
 * or search a substring or a character inside a string.
 */
PKS_VALUE macroc_indexOf(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs) {
	macroc_expectNumberOfArgs(2, nArgs, "IndexOf");
	PKS_VALUE vTarget = pValues[0];
	PKS_VALUE vOf = pValues[1];
	int nIndex = -2;
	if (vTarget.pkv_type == VT_OBJECT_ARRAY) {
		nIndex = memory_indexOf(vTarget, vOf);
	} else if (vTarget.pkv_type == VT_STRING) {
		const char* pszString = memory_accessString(vTarget);
		const char* pszIndex = 0;
		if (vOf.pkv_type == VT_STRING) {
			const char* pszOther = memory_accessString(vOf);
			pszIndex = strstr(pszString, pszOther);
		} else if (vOf.pkv_type == VT_CHAR) {
			pszIndex = strchr(pszString, vOf.pkv_data.uchar);
		}
		nIndex = pszIndex ? (int)(pszIndex - pszString) : -1;
	}
	if (nIndex == -2) {
		interpreter_raiseError("Unsupported argument type for IndexOf");
	}
	return (PKS_VALUE) {.pkv_type = VT_NUMBER, .pkv_data.intValue = nIndex};
}

/*
 * Test a number of flags to check the type of a file.
 * -e - file exists?
 * -d - file is directory?
 * -s - file is a system file?
 * -r - file is readonly ?
 * -a - file is archive?
 */
int macroc_fileTest(const char* pszFile, const char* pszMode) {
	if (!pszFile) {
		return 0;
	}
	if (file_exists(pszFile) < 0) {
		return 0;
	}
	while (pszMode && *pszMode) {
		char c = *pszMode++;
		if (c == '-') {
			continue;
		}
		if (c == 'e') {
			continue;
		}
		int nMode = file_getFileMode(pszFile);
		if (c == 'd' && (nMode & _A_SUBDIR) == 0) {
			return 0;
		}
		if (c == 'r' && (nMode & _A_RDONLY) == 0) {
			return 0;
		}
		if (c == 's' && (nMode & _A_SYSTEM) == 0) {
			return 0;
		}
		if (c == 'h' && (nMode & _A_HIDDEN) == 0) {
			return 0;
		}
		if (c == 'a' && (nMode & _A_ARCH) == 0) {
			return 0;
		}
	}
	return 1;
}

/*
 * Lists all files in a directory given a matching pattern.
 */
ARRAY_LIST* macroc_fileListFiles(const char* pszDir, const char* pszPattern) {
	ARRAY_LIST* pResult = arraylist_create(17);
	if (pszDir) {
		char szBuf[EDMAXPATHLEN];
		if (!pszPattern) {
			pszPattern = "*.*";
		}
		string_concatPathAndFilename(szBuf, pszDir, pszPattern);
		struct _finddata_t dta;
		intptr_t handle;

		if ((handle = _findfirst(szBuf, &dta)) >= 0) {
			arraylist_add(pResult, _strdup(dta.name));
			while (_findnext(handle, &dta) >= 0) {
				arraylist_add(pResult, _strdup(dta.name));
			}
			_findclose(handle);
		}
	}
	return pResult;
}

/*
 * Returns an array with all window handles.
 */
PKS_VALUE edit_getAllEditors(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	PKS_VALUE tResult = memory_createObject(pContext, VT_OBJECT_ARRAY, 5, 0);
	WINFO* wp = ww_getCurrentEditorWindow();
	while (wp) {
		PKS_VALUE vWin = memory_createHandleObject(pContext, VT_EDITOR_HANDLE, wp);
		memory_addObject(pContext, &tResult, vWin);
		wp = wp->next;
	}
	return tResult;
}

/*
 * Replace all lines in the specified range with a list of strings.
 */
PKS_VALUE edit_replaceLines(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	WINFO* wp = ww_getCurrentEditorWindow();
	int nFirst = 0;
	if (nArgs >= 1 && pValues->pkv_type == VT_EDITOR_HANDLE) {
		wp = memory_handleForValue(*pValues);
		nFirst = 1;
	}
	macroc_expectNumberOfArgs(2, nArgs-nFirst, "EditorReplaceLines");
	PKS_VALUE range = pValues[nFirst];
	PKS_VALUE lines = pValues[nFirst+1];
	long long bRet = 0;
	if (range.pkv_type == VT_RANGE && lines.pkv_type == VT_OBJECT_ARRAY) {
		size_t nSize = memory_size(lines);
		ARRAY_LIST* pStrings = arraylist_create(nSize);
		for (int i = 0; i < nSize; i++) {
			PKS_VALUE vElement = memory_getNestedObject(lines, i);
			if (vElement.pkv_type == VT_STRING) {
				arraylist_add(pStrings, (void*)memory_accessString(vElement));
			}
		}
		bRet = edit_replaceSelectedRange(wp, range.pkv_data.range.r_start, range.pkv_data.range.r_end, pStrings);
		arraylist_destroy(pStrings);
	} else {
		interpreter_raiseError("Expecting range and line array argument in EditorReplaceLines");
	}
	return (PKS_VALUE) { .pkv_type = VT_BOOLEAN, .pkv_data.booleanValue = (int)bRet};
}

/*
 * Returns a range of the lines currently selected in the editor passed as the 1st argument or
 * the current editor. If no lines are selected a range with a single line - the line containing
 * the caret is returned.
 */
PKS_VALUE edit_getSelectedLineRange(EXECUTION_CONTEXT* pContext, PKS_VALUE* pValues, int nArgs) {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (nArgs == 1 && pValues->pkv_type == VT_EDITOR_HANDLE) {
		wp = memory_handleForValue(*pValues);
	}
	if (wp == NULL) {
		return NIL;
	}
	long nLow = wp->caret.ln;
	long nHigh = nLow;
	if (wp->blstart && wp->blend) {
		FTABLE* fp = wp->fp;
		nLow = ln_indexOf(fp, wp->blstart->m_linePointer);
		nHigh = ln_indexOf(fp, wp->blend->m_linePointer);
		if (nHigh > nLow && wp->blend->m_column == 0) {
			nHigh--;
		}
	}
	return (PKS_VALUE) { .pkv_type = VT_RANGE, .pkv_data.range.r_start = nLow, .pkv_data.range.r_end = nHigh };
}

/*
 * Returns the length of the specified line in number of characters.
 */
int edit_getLineLen(WINFO* wp, long nLine) {
	FTABLE* fp = wp->fp;
	LINE* lp = ln_goto(fp, nLine);
	return lp ? lp->len : -1;
}

/*
 * Returns the string representing the contents of the specified line.
 */
char* edit_getLineText(WINFO* wp, long nLine) {
	if (wp == NULL) {
		return NULL;
	}
	FTABLE* fp = wp->fp;
	LINE* lp = ln_goto(fp, nLine);
	if (!lp) {
		return 0;
	}
	memcpy(_linebuf, lp->lbuf, (size_t)lp->len + 1);
	_linebuf[lp->len] = 0;
	return _linebuf;
}

/*
 * In the passed text replace spaces with tabs according to the tab stop specification for the given editor.
 */
char* edit_replaceSpacesWithTabs(WINFO* wp, char* pszLine) {
	int t;
	int nLen = ft_compressSpacesToTabs(wp, _linebuf, LINEBUFSIZE, pszLine, strlen(pszLine), &t);
	_linebuf[nLen] = 0;
	return _linebuf;
}

/*
 * In the passed text replace all tabs with spaces according to the tab stop specification for the given editor.
 */
char* edit_replaceTabsWithSpaces(WINFO* wp, char* pszLine) {
	int t = 0;
	int nLen = ft_expandTabsWithSpaces(wp, _linebuf, LINEBUFSIZE, pszLine, strlen(pszLine), &t);
	_linebuf[nLen] = 0;
	return _linebuf;
}

/*--------------------------------------------------------------------------
 * macroc_substituteStringWith()
 * substitute in a string
 */
char* macroc_substituteStringWith(char* string, char* pattern, char* with, long nREFlags, int maxOccurence) {
	char	ebuf[ESIZE];
	char* src;
	char* send;
	int		newlen;
	unsigned char nlchar;
	char		replace[256];
	RE_MATCH	match;
	RE_PATTERN  rePattern;
	REPLACEMENT_PATTERN replacementPattern;

	_linebuf[0] = 0;
	replace[0] = 0;
	RE_PATTERN* pPattern;
	if (ft_getCurrentDocument()) {
		nlchar = ft_getCurrentDocument()->documentDescriptor->nl;
	}
	else {
		nlchar = '\n';
	}
	if (pattern &&
		string &&
		with &&
		(pPattern = find_regexCompile(&rePattern, ebuf, pattern, nREFlags)) &&
		regex_initializeReplaceByExpressionOptions(&(REPLACEMENT_OPTIONS) { with, nREFlags, nlchar, pPattern->nbrackets }, & replacementPattern)) {
		src = string;
		send = _linebuf;

		/* copy default string */
		while ((*send++ = *src++) != 0)
			;
		src = _linebuf;

		while (maxOccurence-- > 0) {
			if (!regex_match(pPattern, src, send, &match)) {
				break;
			}
			if (match.circf && match.loc1 != _linebuf) {
				break;
			}
			if ((newlen = regex_replaceSearchString(&replacementPattern, replace, sizeof replace, &match)) < 0) {
				break;
			}
			memmove(match.loc1 + newlen, match.loc2, (int)(send - match.loc2));
			send += (newlen - (match.loc2 - match.loc1));
			*send = 0;
			memcpy(match.loc1, replace, newlen);
			src = match.loc2;
		}
	}
	return _linebuf;
}

/*--------------------------------------------------------------------------
 * macroc_findPattern()
 * Find a pattern in a string and return the index in the string or -1 if not found.
 */
long long macroc_findPattern(const char* string, char* pattern, int nREFlags) {
	char		ebuf[ESIZE];
	RE_MATCH	match;
	RE_PATTERN  rePattern;
	RE_PATTERN* pPattern;

	if (!pattern ||
		!string ||
		!(pPattern = find_regexCompile(&rePattern, ebuf, pattern, nREFlags))) {
		interpreter_raiseError("Wrong parameters or failure to compile pattern %s", pattern ? pattern : "NO_PATTERN");
		return -1;
	} 
	if (!regex_match(pPattern, string, 0, &match)) {
		return -1;
	}
	return (long long)(match.loc1 - string);
}


