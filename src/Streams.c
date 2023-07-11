/*
 * Streams.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: Streams abstraction of an input source for the various parsers inside PKS-Edit.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include "alloc.h"
#include <stdio.h>
#include <string.h>
#include "streams.h"
#include "documentmodel.h"

typedef union union_STREAM_DATA {
	struct tagSTRING_STREAM_DATA {
		const char* is_start;						// Original start of the stream data
		const char* is_source;						// A pointer to the source data.
		const char* is_eof;							// An optional end of data marker.
	} sd_string;
	struct tagLINE_STREAM_DATA {
		LINE* is_currentLine;						// The current line pointer.
		int   is_offset;							// The current line offset - might be longer then lp->len to return the newlines and crs.
	} sd_line;
} STREAM_DATA;

#define STRING_DATA(stream)		((STREAM_DATA*)&stream->is_data)->sd_string
#define LINE_DATA(stream)		((STREAM_DATA*)&stream->is_data)->sd_line

/*
 * Restore a pointer into the stream previously returned with ftell 
 */
static void stringstream_seek(INPUT_STREAM* pStream, STREAM_OFFSET offset) {
	STRING_DATA(pStream).is_source = (const char*) offset.so_p;
}

/*
 * Return a pointer to the current offset to be restored later
 */
static STREAM_OFFSET stringstream_tell(INPUT_STREAM* pStream) {
	return (STREAM_OFFSET) { .so_p = (char*)STRING_DATA(pStream).is_source };
}

/*
 * Return the next character from a string input stream.
 */
static int stringstream_getc(INPUT_STREAM* pStream) {
	if (STRING_DATA(pStream).is_source >= STRING_DATA(pStream).is_eof) {
		return 0;
	}
	char c = *STRING_DATA(pStream).is_source;
	STRING_DATA(pStream).is_source++;
	return c;
}

/*
 * Skip a number of bytes in the input stream.
 */
static void stringstream_skip(INPUT_STREAM* pStream, int nOffset) {
	const char* pPointer = &STRING_DATA(pStream).is_source[nOffset];
	if (pPointer < STRING_DATA(pStream).is_start) {
		return;
	}
	STRING_DATA(pStream).is_source = pPointer > STRING_DATA(pStream).is_eof ? STRING_DATA(pStream).is_eof : pPointer;
}

/*
 * Return the next character from a string input stream without incrementing
 * the pointer.
 */
static int stringstream_peekc(INPUT_STREAM* pStream, int nOffset) {
	const char* pPointer = &STRING_DATA(pStream).is_source[nOffset];
	return pPointer < STRING_DATA(pStream).is_eof ? *pPointer : 0;
}

/*
 * Returns a marker for a simple string input stream.
 */
static int stringstream_inputMark(INPUT_STREAM* pStream, void** p) {
	*p = (void*)STRING_DATA(pStream).is_source;
	return 0;
}

/*
 * strncmp on a String stream.
 */
static int stringstream_strncmp(INPUT_STREAM* pStream, const char* pszString, size_t nLen) {
	const char* pPointer = STRING_DATA(pStream).is_source;
	if (pPointer + nLen > STRING_DATA(pStream).is_eof) {
		return -1;
	}
	return strncmp(pPointer, pszString, nLen);
}

/*
 * Start line positioning of streams.
 */
static STREAM_OFFSET stringstream_positionToLineStart(INPUT_STREAM* pStream, int nDelta) {
	STREAM_OFFSET position = stringstream_tell(pStream);
	const char* pOffset = STRING_DATA(pStream).is_source;
	if (nDelta <= 0) {
		const char* pStart = STRING_DATA(pStream).is_start;
		while (nDelta <= 0 && pOffset > pStart) {
			if (pOffset[-1] == '\n') {
				nDelta++;
			}
			pOffset--;
		}
	}
	else {
		const char* pEnd = STRING_DATA(pStream).is_eof;
		while (nDelta > 0 && pOffset < pEnd) {
			if (*pOffset++ == '\n') {
				nDelta--;
			}
		}
	}
	STRING_DATA(pStream).is_source = pOffset;
	return position;
}

/*
 * Creates a WSYIWYG input stream from a simple string. The input stream must be freed, when done.
 */
INPUT_STREAM* streams_createStringInputStream(const char* pszString) {
	INPUT_STREAM* pResult = calloc(1, sizeof * pResult);
	STRING_DATA(pResult).is_source = pszString;
	STRING_DATA(pResult).is_start = pszString;
	STRING_DATA(pResult).is_eof = pszString + strlen(pszString);
	pResult->is_skip = stringstream_skip;
	pResult->is_getc = stringstream_getc;
	pResult->is_peekc = stringstream_peekc;
	pResult->is_seek = stringstream_seek;
	pResult->is_tell = stringstream_tell;
	pResult->is_inputMark = stringstream_inputMark;
	pResult->is_destroy = free;
	pResult->is_positionToLineStart = stringstream_positionToLineStart;
	pResult->is_strncmp = stringstream_strncmp;
	return pResult;
}

/*
 * Access a character from the linestream.
 */
static int linestream_peekc(INPUT_STREAM* pStream, int nDelta) {
	LINE* lp = LINE_DATA(pStream).is_currentLine;
	int nOffset = (int)(LINE_DATA(pStream).is_offset+nDelta);
	if (nOffset == -1) {
		if (lp != 0 && (lp = lp->prev) != 0) {
			BOOL bTerm = !(lp->lflg & LNNOTERM);
			// TODO: special case, that previous line is empty AND unterminated	
			return bTerm ? '\n' : lp->lbuf[lp->len - 1];
		}
		return 0;
	}
	while (lp && lp->next) {
		if (nOffset < lp->len) {
			return lp->lbuf[nOffset];
		}
		BOOL bTerm = !(lp->lflg & LNNOTERM);
		// No CR handling currently
		if (nOffset == lp->len && bTerm) {
			return '\n';
		}
		if (bTerm) {
			nOffset--;
		}
		nOffset -= lp->len;
		lp = lp->next;
	}
	return 0;
}

/*
 * Skip a number of characters from the linestream.
 */
static void linestream_skip(INPUT_STREAM* pStream, int nDelta) {
	LINE* lp = LINE_DATA(pStream).is_currentLine;
	int nOffset = (int)(LINE_DATA(pStream).is_offset + nDelta);
	if (nDelta < 0) {
		if (nOffset >= 0) {
			LINE_DATA(pStream).is_offset = nOffset;
		} else {
			lp = lp->prev;
			if (lp) {
				LINE_DATA(pStream).is_offset = lp->len;
				LINE_DATA(pStream).is_currentLine = lp;
			}
		}
		return;
	}
	while (lp && lp->next) {
		BOOL bTerm = !(lp->lflg & LNNOTERM);
		if (nOffset < lp->len || (nOffset == lp->len && bTerm)) {
			LINE_DATA(pStream).is_offset = nOffset;
			LINE_DATA(pStream).is_currentLine = lp;
			return;
		}
		if (bTerm) {
			nOffset--;
		}
		nOffset -= lp->len;
		lp = lp->next;
	}
	LINE_DATA(pStream).is_currentLine = 0;
}

/*
 * Return a character from the line input stream.
 */
static int linestream_getc(INPUT_STREAM* pStream) {
	char c = linestream_peekc(pStream, 0);
	if (c == 0) {
		return 0;
	}
	LINE* lp = LINE_DATA(pStream).is_currentLine;
	int nInternalOffset = LINE_DATA(pStream).is_offset+1;
	if (nInternalOffset > lp->len) {
		LINE_DATA(pStream).is_currentLine = lp->next;
		LINE_DATA(pStream).is_offset = 0;
	} else {
		LINE_DATA(pStream).is_offset = nInternalOffset;
	}
	return c;
}

/*
 * strncmp on a line input stream.
 */
static int linestream_strncmp(INPUT_STREAM* pStream, const char* pszString, size_t nLen) {
	LINE* lp = LINE_DATA(pStream).is_currentLine;
	int nInternalOffset = LINE_DATA(pStream).is_offset;
	if (lp == 0 || lp->next == NULL || nInternalOffset + nLen > lp->len) {
		return -1;
	}
	const char* pszCompare = &lp->lbuf[nInternalOffset];
	return strncmp(pszCompare, pszString, nLen);
}

/*
 * Return the linestream input marker.
 */
static int linestream_inputMark(INPUT_STREAM* pStream, void** pParam) {
	*pParam = (void*)LINE_DATA(pStream).is_currentLine;
	return LINE_DATA(pStream).is_offset;
}

/*
 * Resets the stream to point to a previously saved offset.
 */
static void linestream_seek(INPUT_STREAM* pStream, STREAM_OFFSET offset) {
	LINE_DATA(pStream).is_currentLine = (LINE*)offset.so_p;
	LINE_DATA(pStream).is_offset = (int)offset.so_offset;
}

/*
 * Returns the current stream offset in a linestream.
 */
static STREAM_OFFSET linestream_tell(INPUT_STREAM* pStream) {
	return (STREAM_OFFSET) {
		.so_p = (void*)LINE_DATA(pStream).is_currentLine,
		.so_offset = LINE_DATA(pStream).is_offset
	};
}

/*
 * Position to a line start.
 */
static STREAM_OFFSET linestream_positionToLineStart(INPUT_STREAM* pStream, int nDelta) {
	STREAM_OFFSET offset = linestream_tell(pStream);
	LINE_DATA(pStream).is_offset = 0;
	LINE* lp = LINE_DATA(pStream).is_currentLine;
	if (nDelta < 0) {
		while (nDelta < 0 && lp) {
			lp = lp->prev;
			nDelta++;
		}
	}
	else if (nDelta > 0) {
		while (nDelta > 0 && lp && lp->next) {
			lp = lp->next;
			nDelta--;
		}
	}
	LINE_DATA(pStream).is_currentLine = lp;
	return offset;
}

/*
 * Creates a input stream from a line input + a given offset (typically 0). The input stream must be freed, when done.
 * Note, that we are assuming that the last line in the list of lines is ignored (in conformance to editor internal
 * document structure).
 */
INPUT_STREAM* streams_createLineInputStream(LINE* pLine, int nOffset) {
	INPUT_STREAM* pResult = calloc(1, sizeof * pResult);
	LINE_DATA(pResult).is_currentLine = pLine;
	LINE_DATA(pResult).is_offset = nOffset;
	pResult->is_skip = linestream_skip;
	pResult->is_getc = linestream_getc;
	pResult->is_peekc = linestream_peekc;
	pResult->is_seek = linestream_seek;
	pResult->is_tell = linestream_tell;
	pResult->is_strncmp = linestream_strncmp;
	pResult->is_inputMark = linestream_inputMark;
	pResult->is_positionToLineStart = linestream_positionToLineStart;
	pResult->is_destroy = free;
	return pResult;
}


