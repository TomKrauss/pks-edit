/*
 * Streams.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: Streams abstraction of an input source for the various parsers inside PKS Edit.
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
#ifndef STREAMS_H
#define STREAMS_H

typedef struct tagSTREAM_OFFSET {
	void* so_p;
	intptr_t so_offset;
} STREAM_OFFSET;

typedef struct tagINPUT_STREAM INPUT_STREAM;

struct tagINPUT_STREAM {
	int (*is_inputMark)(INPUT_STREAM* pStream, void** p);
														// can be used to retrieve a marker on the input, which can be used during rendering
														// to match the rendered contents with the original input.
	int (*is_getc)(INPUT_STREAM* pStream);				// Provides the next character in the input stream and increments the stream pointer. 
														// When the end of the stream is reached 0 is returned.
	void (*is_skip)(INPUT_STREAM* pStream, int nBytes);
														// Skip nBytes in the given input stream. If the end of file is reached, 
														// do not skip beyond end of file. A valid negative skip is only -1.
	STREAM_OFFSET (*is_positionToLineStart)(INPUT_STREAM* pStream, int nDelta);
														// Position the stream to the start of the line. If 'nDelta' is 0, the current line
														// is used if it is negative a previous line, if positive a next line is used.
	STREAM_OFFSET (*is_tell)(INPUT_STREAM* pStream);	// Save an input position for later restore with is_seek
	// Restore an input position previously marked with is_tell
	void (*is_seek)(INPUT_STREAM* pStream, STREAM_OFFSET pos);
														// Provides the next character at offset in the input stream and leaves the stream 
														// pointer as is. When the end of the stream is reached or nOffset exceeds 
														// the end of the stream 0 is returned.
														// offset may be -1 or 0 or positive. Accessing characters at negative offsets may
														// have limitations.
	int (*is_peekc)(INPUT_STREAM* pStream, int nOffset);
	// Compare pszText with the current text at input stream and return 0 if equals.
	int (*is_strncmp)(INPUT_STREAM* pStream, const char* pszText, size_t nMax);
	void (*is_destroy)(INPUT_STREAM* pStream);			// Destroy the input stream
	void* is_data[5];									// Arbitrary stream-dependent data
};

/*
 * Creates an input stream from a simple string. The input stream must be destroyed, when done using INPUT_STREAM.is_destroy()
 */
extern INPUT_STREAM* streams_createStringInputStream(const char* pszString);

/*
 * Creates a input stream from a line input + a given offset (typically 0). The input stream must be freed, when done.
 * Note, that we are assuming that the last line in the list of lines is ignored (in conformance to editor internal
 * document structure).
 */
#ifdef EDITAB_H
INPUT_STREAM* streams_createLineInputStream(LINE* pLine, int nOffset);
#endif

#endif