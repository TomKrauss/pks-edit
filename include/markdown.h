/*
 * markdown.h
 *
 * Contains definitions for markdown Wysiwyg rendering allowing to support different markdown
 * languages (markdown, AsciiDoc, HTML, etc...).
 *
 * author: Tom
 * created: 2024
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */


#ifndef MARKDOWN_H

typedef struct tagLINK_PARSE_RESULT {
	// The actual URL - image or hyperlink
	char* lpr_LinkUrl;
	// The reference to a reference link type link
	char* lpr_LinkRefUrl;
	// The title to display in particular for an image
	char* lpr_title;
	// for image links - the width
	int   lpr_width;
	// for image links - the height
	int   lpr_height;
	// If the parsed link represents the definition of a reference link this value is TRUE.
	BOOL  lpr_isReferenceLinkDefinition;
	// If the parsed link represents the definition of a footnote....
	BOOL  lpr_isFootnoteDefinition;
} LINK_PARSE_RESULT;

typedef enum LINK_PARSE_STATE {
	LPS_TITLE,
	LPS_WAIT_URL,
	LPS_PARSE_URL
} LINK_PARSE_STATE;

typedef struct tagINPUT_STREAM INPUT_STREAM;
typedef struct tagHTML_PARSER_STATE HTML_PARSER_STATE;

typedef int (*MDR_GET_LIST_LEVEL)(INPUT_STREAM* pStream, char aListChar);
typedef BOOL(*MDR_DETECT_LINK)(INPUT_STREAM* pStream, char c);
typedef BOOL(*MDR_PARSE_LINK)
	(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText, LINK_PARSE_RESULT* pResult, LINK_PARSE_STATE startState);

typedef struct tagMDR_SYNTAX {
	char syn_header;					// Character used to define a header section - i.e. '#' in markdown and '=' in asciidoc
	char* syn_literalMarker;			// String used to start and and a literal section - i.e. '```' in markdown and '----' in asciidoc
	char* syn_literalMarker2;			// Optional alternate literal marker '....' in asciidoc
	MDR_GET_LIST_LEVEL syn_getLevel;	// Function used to determine the level of a list.
	MDR_DETECT_LINK syn_detectLink;		// Function used to detect a link in the input text.
	MDR_PARSE_LINK syn_parseLink;		// Function used to parse a link from the input.
} MDR_SYNTAX;


#define MARKDOWN_H
#endif

