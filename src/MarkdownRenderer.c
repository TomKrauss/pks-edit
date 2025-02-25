/*
 * MarkdownRenderer.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: A special renderer for markdown files.
 *
 * 										created: 05.11.21
 * 										last modified: 14.12.21
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <wininet.h>

#include "winfo.h"
#include "pksrc.h"
#include "trace.h"
#include "fileutil.h"
#include "caretmovement.h"
#include "customcontrols.h"
#include "linkedlist.h"
#include "editorfont.h"
#include "stringutil.h"
#include "themes.h"
#include "pathname.h"
#include "winutil.h"
#include "pksmacro.h"
#include "mainframe.h"
#include "edctype.h"
#include "codeanalyzer.h"
#include "jsonparser.h"
#include "hashmap.h"
#include "printing.h"
#include "streams.h"
#include "htmlrendering.h"
#include "dpisupport.h"
#include "grammar.h"
#include "documenttypes.h"

#include "syntaxhighlighting.h"
#include "loadimage.h"
#include "markdown.h"

typedef struct tagRENDER_VIEW_PART RENDER_VIEW_PART;

#define NO_COLOR			-1		// marker for no color defined
#define PARAGRAPH_OFFSET	15		// offset in pixels between paragraph type elements.
#define DEFAULT_LEFT_MARGIN	10		// offset of most elements to the left of the screen.

#define MAX_HTML_VALUE_LEN 200

#define IS_UNORDERED_LIST_START(c)	(c == '-' || c == '*' || c == '+')

extern HINSTANCE		hInst;

static const char _escapedChars[] = "\\`*_{}[]<>()#+-.!|&";

extern EDTEXTSTYLE* highlight_getTextStyleForLexicalState(HIGHLIGHTER* pHighlighter, LEXICAL_STATE state);

typedef struct tagRENDER_FLOW_PARAMS {
	RENDER_VIEW_PART* rfp_part;
	WINFO* rfp_wp;
	HDC rfp_hdc;
	BOOL rfp_skipSpace;
	float rfp_zoomFactor;
	BOOL rfp_measureOnly;
	BOOL rfp_focus;
	BOOL rfp_printing;
	BOOL rfp_isCaret;
	THEME_DATA* rfp_theme;
} RENDER_FLOW_PARAMS;

typedef void (*RENDER_PAINT)(RENDER_FLOW_PARAMS* pParams, RECT* pBounds, RECT* pUsed);

typedef struct tagENTITY_MAPPING {
	char	c;
	char    entity[8];
} ENTITY_MAPPING;

static BOOL asciidoc_detectLink(INPUT_STREAM* pStream, char c);
static int asciidoc_getLevelFromIndent(INPUT_STREAM* pStream, char aListChar);
static BOOL asciidoc_parseLink(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText, LINK_PARSE_RESULT* pResult, LINK_PARSE_STATE startState);
static void asciidoc_detectGrammar(INPUT_STREAM* pStream, int nLiteralMarkerLength, RENDER_VIEW_PART_PARAM* pParam);

static BOOL mdr_detectLink(INPUT_STREAM* pStream, char c);
static int mdr_getLevelFromIndent(INPUT_STREAM* pStream, char aListChar);
static BOOL mdr_parseLink(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText, LINK_PARSE_RESULT* pResult, LINK_PARSE_STATE startState);
static void mdr_detectGrammar(INPUT_STREAM* pStream, int nLiteralMarkerLength, RENDER_VIEW_PART_PARAM* pParam);

static MDR_SYNTAX _markdownSyntax = {
	.syn_header = '#',
	.syn_literalMarker = "```",
	.syn_literalMarker2 = "~~~",
	.syn_rulerCharacter = '-',
	.syn_getLevel = mdr_getLevelFromIndent,
	.syn_detectLink = mdr_detectLink,
	.syn_detectGrammar = mdr_detectGrammar,
	.syn_parseLink = mdr_parseLink
};

static MDR_SYNTAX _asciidocSyntax = {
	.syn_header = '=',
	.syn_literalMarker = "----",
	.syn_literalMarker2 = "....",
	.syn_tableDelimiter = "|===",
	.syn_rulerCharacter = '\'',
	.syn_tableEmptyRowsAllowed = TRUE,
	.syn_getLevel = asciidoc_getLevelFromIndent,
	.syn_detectGrammar = asciidoc_detectGrammar,
	.syn_detectLink = asciidoc_detectLink,
	.syn_parseLink = asciidoc_parseLink
};

static MDR_SYNTAX _htmlSyntax;

static ENTITY_MAPPING _entities[] = {
	{'&', "amp;"},
	{'�', "copy;"},
	{'\'', "apos;"},
	{'<', "lt;"},
	{'>', "gt;"},
	{'@', "#64;"},
	{'/', "#47;"},
	{' ', "nbsp;"},
	{'�', "reg;"},
	{'�', "yen;"}
}; 

typedef struct tagEMOJI_MAPPING {
	WCHAR*	emoji;
	char    *name;
} EMOJI_MAPPING;

static EMOJI_MAPPING _emojis[] = {
	// sorted alphabetically so we can bin search in the list of Emojis.
	{L"\U0001F44D", ":+1:"},
	{L"\U0001F44E", ":-1:"},
	{L"\U00002708", ":airplane:"},
	{L"\U0001F47D", ":alien:"},
	{L"\U0001F620", ":angry:"},
	{L"\U0001F41C", ":ant:"},
	{L"\U0001F388", ":balloon:"},
	{L"\U0001F34C", ":banana:"},
	{L"\U0001F4CA", ":bar_chart:"},
	{L"\U0001F514", ":bell:"},
	{L"\U0001F499", ":blue_heart:"},
	{L"\U0001F60A", ":blush:"},
	{L"\U0001F4D6", ":book:"},
	{L"\U0001F494", ":broken_heart:"},
	{L"\U0001F41B", ":bug:"},
	{L"\U0001F4A1", ":bulb:"},
	{L"\U0001F4C6", ":calendar:"},
	{L"\U0001F36C", ":candy:"},
	{L"\U0001F697", ":car:"},
	{L"\U0001F638", ":cat:"},
	{L"\U0001F4BF", ":cd:"},
	{L"\U0001F4C9", ":chart_with_downwards_trend:"},
	{L"\U0001F4C8", ":chart_with_upwards_trend:"},
	{L"\U0001F36B", ":chocolate_bar:"},
	{L"\U0001F44F", ":clap:"},
	{L"\U00002601", ":cloud:"},
	{L"\U0001F4A5", ":collision:"},
	{L"\U0001F4BB", ":computer:"},
	{L"\U0001F615", ":confused:"},
	{L"\U0001F477", ":construction_worker:"},
	{L"\U0001F46E", ":cop:"},
	{L"\U0001F491", ":couple:"},
	{L"\U0001F451", ":crown:"},
	{L"\U0001F498", ":cupid:"},
	{L"\U0001F4C5", ":date:"},
	{L"\U0001F4AB", ":dizzy:"},
	{L"\U0001F42C", ":dolphin:"},
	{L"\U0001F4C0", ":dvd:"},
	{L"\U0001F440", ":eyes:"},
	{L"\U0001F3ED", ":factory:"},
	{L"\U0001F46A", ":family:"},
	{L"\U0001F525", ":fire:"},
	{L"\U0001F526", ":flashlight:"},
	{L"\U0001F626", ":frowning:"},
	{L"\U0001F382", ":game_die:"},
	{L"\U0001F47B", ":ghost:"},
	{L"\U0001F381", ":gift:"},
	{L"\U0001F49A", ":green_heart:"},
	{L"\U0001F600", ":grinning_face:"},
	{L"\U00002764", ":heart:"},
	{L"\U0001F60D", ":heart_eyes:"},
	{L"\U0001F681", ":helicopter:"},
	{L"\U0000231B", ":hourglass:"},
	{L"\U0001F607", ":innocent:"},
	{L"\U0001F602", ":joy:"},
	{L"\U0001F511", ":key:"},
	{L"\U0001F458", ":kimono:"},
	{L"\U0001F48B", ":kiss:"},
	{L"\U0001F34B", ":lemon:"},
	{L"\U0001F484", ":lipstick:"},
	{L"\U0001F512", ":lock:"},
	{L"\U0001F36D", ":lollipop:"},
	{L"\U0001F4E2", ":loudspeaker:"},
	{L"\U0001F50D", ":mag:"},
	{L"\U0001F50E", ":mag_right:"},
	{L"\U0001F4DD", ":memo:"},
	{L"\U0001F3A4", ":microphone:"},
	{L"\U0001F412", ":monkey:"},
	{L"\U0001F42D", ":mouse:"},
	{L"\U0001F401", ":mouse2:"},
	{L"\U0001F4AA", ":muscle:"},
	{L"\U0001F344", ":mushroom:"},
	{L"\U0001F3B9", ":musical_keyboard:"},
	{L"\U0001F3B5", ":musical_note:"},
	{L"\U0001F611", ":neutral_face:"},
	{L"\U0001F636", ":no_mouth:"},
	{L"\U0001F443", ":nose:"},
	{L"\U0001F44C", ":ok_hand:"},
	{L"\U0001F632", ":open_mouth:"},
	{L"\U0001F4CE", ":paperclip:"},
	{L"\U0001F437", ":pig:"},
	{L"\U0001F447", ":point_down:"},
	{L"\U0001F448", ":point_left:"},
	{L"\U0001F449", ":point_right:"},
	{L"\U0001F446", ":point_up:"},
	{L"\U0001F64F", ":pray:"},
	{L"\U0001F49C", ":purple_heart:"},
	{L"\U00002753", ":question:"},
	{L"\U0001F308", ":rainbow:"},
	{L"\U0001F400", ":rat:"},
	{L"\U0001F48D", ":ring:"},
	{L"\U0001F680", ":rocket:"},
	{L"\U0001F339", ":rose:"},
	{L"\U0001F3C3", ":runner:"},
	{L"\U00002702", ":scissors:"},
	{L"\U0001F4A9", ":shit:"},
	{L"\U0001F45E", ":shoe:"},
	{L"\U0001F480", ":skull:"},
	{L"\U0001F604", ":smile:"},
	{L"\U00002744", ":snowflake:"},
	{L"\U0001F62D", ":sob:"},
	{L"\U00002728", ":sparkles:"},
	{L"\U0001F508", ":speaker:"},
	{L"\U00002B50", ":star:"},
	{L"\U0001F61B", ":stuck_out_tounge:"},
	{L"\U0001F33B", ":sunflower:"},
	{L"\U0001F60E", ":sunglasses:"},
	{L"\U0001F31E", ":sunny:"},
	{L"\U0001F389", ":tada:"},
	{L"\U0001F345", ":tomato:" },
	{L"\U0001F3C6", ":trophy:"},
	{L"\U0001F337", ":tulip:"},
	{L"\U0001F495", ":two_hearts:"},
	{L"\U00002614", ":umbrella:"},
	{L"\U0001F513", ":unlock:"},
	{L"\U0001F44B", ":wave:"},
	{L"\U00002705", ":white_check_mark:"},
	{L"\U0001F609", ":winking_face:"},
	{L"\U0001F61F", ":worried:"},
	{L"\U0001F527", ":wrench:"},
	{L"\U0001F49B", ":yellow_heart:"},
	{L"\U0001F60B", ":yum:" }
};

static char _emojiOffsets[256];

/*
 * Used during code completion.
 */
JSON_ENUM_VALUE* emoji_getSuggestions() {
	JSON_ENUM_VALUE* pResult = calloc(DIM(_emojis) + 1, sizeof (*pResult));
	if (pResult != NULL) {
		for (int i = 0; i < DIM(_emojis); i++) {
			pResult[i].jev_name = _emojis[i].name;
			pResult[i].jev_icon.cai_iconType = CAI_EMOJI;
			memcpy(pResult[i].jev_icon.cai_data.cai_emoji, _emojis[i].emoji, 2 * sizeof(WCHAR));
		}
	}
	return pResult;
}
//
// Describes the bounds of a text run with the following shape.
// 
//             +---------+
//       left1 |         |
// +-----------+ top1    |
// |                     |
// |         right2 +----+  bottom2
// |                |
// +----------------+ bottom
//
typedef struct tagRUN_BOUNDS {
	int		left1;
	int		top1;
	int		right2;
	int		bottom2;
	int		left;
	int		top;
	int		right;
	int		bottom;
} RUN_BOUNDS;

typedef enum { CSU_NONE, CSU_PX, CSU_EM, CSU_PT, CSU_PERCENT } CSS_SIZE_UNIT;

typedef struct tagSIZE_SPECIFICATION {
	float	ss_value;
	CSS_SIZE_UNIT ss_units;
} SIZE_SPECIFICATION;

/*
 * The text style of a "text run"
 */
typedef struct tagFONT_ATTRIBUTES {
	SIZE_SPECIFICATION size;
	int		weight;
	int		indent;
	short	lineBreak : 1;
	short	strikeout : 1;
	short	italic : 1;
	short	underline : 1;
	short	rollover : 1;
	short	focussed : 1;
	short	superscript : 1;
	short	subscript : 1;
	COLORREF bgColor;
	const char* style;
	COLORREF fgColor;
} FONT_ATTRIBUTES;

typedef struct tagMD_IMAGE {
	int					mdi_width;
	int					mdi_height;
	int					mdi_borderWidth;
	COLORREF			mdi_borderColor;
	// TRUE, if this is a cached image
	BOOL				mdi_cached : 1;
	// If the mdi_image is an icon.
	BOOL				mdi_icon : 1;
	HANDLE				mdi_image;
} MD_IMAGE;

typedef enum {
	TA_ALIGN_DEFAULT = 0,
	TA_ALIGN_LEFT = 0x1,
	TA_ALIGN_RIGHT = 0x2,
	TA_ALIGN_CENTER = 0x3
} TEXT_ALIGN;

typedef struct tagFONT_STYLE_DELTA {
	int fsd_logicalStyles;
	int fsd_indent;
	COLORREF fsd_textColor;
	const char* fsd_styleName;
	COLORREF fsd_fillColor;
	COLORREF fsd_strokeColor;
	int fsd_strokeWidth;
	TEXT_ALIGN fsd_textAlign;
	SIZE_SPECIFICATION fsd_fontSize;
	SIZE_SPECIFICATION fsd_width;
} FONT_STYLE_DELTA;


typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	LINE*				tr_lp;					
	int					tr_lineOffset;			// The line offset where the text run starts
	size_t				tr_size;
	RUN_BOUNDS			tr_bounds;
	FONT_ATTRIBUTES		tr_attributes;
	// The hyperlink for navigation.
	char*				tr_link;
	// If the link should be interpreted as a reference style indirect link to a reference style link defined somewhere else in the document
	BOOL				tr_isRefLink;
	char*				tr_title;
	char*				tr_anchor;
	BOOL				tr_isEmoji;
	union {
		MD_IMAGE*		tr_image;
		WCHAR*			tr_emoji;
	} tr_data;
	// For images - the URL of the image
	char*				tr_imageUrl;
	int					tr_selectionStart;
	int					tr_selectionLength;
} TEXT_RUN;

/*
 * Forward decl.: destroy a text run possibly release the link.
 */
static int mdr_destroyRun(TEXT_RUN* pRun);

typedef struct tagTEXT_FLOW {
	char* tf_text;
	TEXT_ALIGN	tf_align;
	TEXT_RUN* tf_runs;
} TEXT_FLOW;

typedef enum { 
	MET_NONE = -1,				// marker element type for HTML parsing
	MET_BLOCK_QUOTE, 
	MET_ORDERED_LIST, 
	MET_UNORDERED_LIST, 
	MET_IMAGE, 
	MET_TASK_LIST,
	MET_EMOJI,
	MET_NORMAL,
	MET_FENCED_CODE_BLOCK,
	MET_HEADER, 
	MET_TABLE,
	MET_PARAGRAPH,
	MET_HORIZONTAL_RULE,
	// TODO: not yet supported
	MET_DEFINITION_LIST,
	// TODO: not yet supported
	MET_FOOTNOTES
} MDR_ELEMENT_TYPE;

#define MAX_TABLE_COLUMNS   256

typedef struct tagRENDER_TABLE_CELL {
	struct tagRENDER_TABLE_CELL* rtc_next;
	BOOL		rtc_isHeader;
	SIZE_SPECIFICATION	rtc_width;			// width of the table cell
	TEXT_FLOW	rtc_flow;
} RENDER_TABLE_CELL;

typedef struct tagRENDER_TABLE_ROW {
	struct tagRENDER_TABLE_ROW* rtr_next;
	RENDER_TABLE_CELL* rtr_cells;
} RENDER_TABLE_ROW;

typedef struct tag_RENDER_TABLE {
	RENDER_TABLE_ROW* rt_rows;
	int rt_columnCount;
	int rt_columnWidths[MAX_TABLE_COLUMNS];
	int rt_totalColumnWidth;
	int rt_borderWidth;						// width of the table border (by default 1)
	COLORREF rt_borderColor;				// Optional color of the border
} RENDER_TABLE;

typedef struct tagMARGINS {
	int	m_top;
	int	m_bottom;
	int	m_left;
	int	m_right;
} MARGINS;

typedef struct tagRENDER_BOX_DECORATION {
	COLORREF rbd_fillColor;				// background fill color or NO_COLOR
	COLORREF rbd_strokeColor;			// box stroke color or NO_COLOR
	int      rbd_strokeWidth;			// width of the stroke pen
	BOOL	 rbd_useThemeFillColor;		// Flag: we use a default theme dependent fill color
	BOOL	 rbd_useThemeStrokeColor;	// Flag: we use a default theme dependent stroke color
} RENDER_BOX_DECORATION;

typedef struct tag_RENDER_VIEW_PART_PARAM {
	long rvp_number;					// for numbered lists and emojis
	int	rvp_level;						// for headers and lists - the level.
	char* rvp_grammar;					// for fenced code blocks - the name of the used grammar (if defined)
} RENDER_VIEW_PART_PARAM;

struct tagRENDER_VIEW_PART {
	struct tagRENDER_VIEW_PART* rvp_next;
	LINE* rvp_lpStart;
	RENDER_PAINT rvp_paint;
	MARGINS rvp_margins;
	RENDER_BOX_DECORATION* rvp_decoration; // optional box decoration for a view
	RECT rvp_bounds;					// Bounds of an element excluding the margins.
	RECT rvp_occupiedBounds;			// Bounds occupied by the element painted - this may be bigger then rvp_bounds for some element types (e.g. fenced code blocks).
	int rvp_height;						// The height in pixels.
	BOOL rvp_layouted;					// TRUE, if the layout bounds of the view part are valid.
	MDR_ELEMENT_TYPE rvp_type;
	RENDER_VIEW_PART_PARAM rvp_param;
	SIZE_SPECIFICATION rvp_width;		// width of the part 
	union {
		TEXT_FLOW rvp_flow;
		RENDER_TABLE* rvp_table;
	} rvp_data;
};

typedef struct tagMDR_ELEMENT_FORMAT {
	MARGINS mef_margins;
	SIZE_SPECIFICATION mef_fontSize;
	int mef_charWeight;
	const char* mef_style;
	RENDER_BOX_DECORATION* mef_decoration;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8
#define ATTR_HIGHLIGHT	0x10
#define ATTR_LINK		0x20
#define ATTR_LINE_BREAK 0x40
#define ATTR_UNDERLINE	0x80
#define ATTR_TAG_CODE	0x100
#define ATTR_SUPER		0x200
#define ATTR_SUB		0x400
#define ATTR_EMOJI		0x800

/*
 * Draws an emoji with the specified attributes.
 */
extern void paint_emojid2d(HDC hdc, WCHAR* emoji, COLORREF cColor, int fontSize, int x, int y, int* pWidth, int* pHeight);
extern void paint_endEmoji();

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(MARGINS* pMargins, TEXT_FLOW* pFlow, RECT* pBounds, 
	RECT* pPartBounds, RECT* pUsed, int nBlockQuoteLevel, int nAlign, const RENDER_FLOW_PARAMS *pRFP);
static void mdr_updateCaretUI(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight);
static int mdr_getNextLinkRunOffset(RENDER_VIEW_PART* pPart, int nOffset, int nDelta);
/*
 * Visitor function visiting all text runs traversable from a view part and executing a callback function on each list
 * of text runs with a custom parameter. If the callback returns 0, visiting stops.
 */
static int mdr_forRunListDo(RENDER_VIEW_PART* pPart, int (*runCallback)(TEXT_RUN* pRunList, void* pParam), void* pParam);

static MDR_ELEMENT_FORMAT _formatText = {
	0, 0, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static RENDER_BOX_DECORATION _fencedBlock = {
	.rbd_useThemeFillColor = 1,
	.rbd_useThemeStrokeColor = 1,
	.rbd_strokeWidth = 1
};

static MDR_ELEMENT_FORMAT _formatFenced = {
	12, 6, 20, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD, "code", .mef_decoration = &_fencedBlock
};

#define BLOCK_QUOTE_INDENT		25

static MDR_ELEMENT_FORMAT _formatParagraph = {
	.mef_margins = {.m_top = 10, .m_bottom = 10, .m_left = DEFAULT_LEFT_MARGIN, .m_right = 10 },
	.mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote = {
	.mef_margins = {.m_top = 10, .m_bottom = 10, .m_left = DEFAULT_LEFT_MARGIN + BLOCK_QUOTE_INDENT, .m_right = 20 },
	.mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote2 = {
	.mef_margins = {.m_top = 0, .m_bottom = 0, .m_left = DEFAULT_LEFT_MARGIN + 2*BLOCK_QUOTE_INDENT, .m_right = 20 },
	.mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote3 = {
	.mef_margins = {.m_top = 0, .m_bottom = 0, .m_left = DEFAULT_LEFT_MARGIN + 3 * BLOCK_QUOTE_INDENT, .m_right = 20 },
	.mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatTable = {
	10, 10, 10, 10, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel1 = {
	1, 1, 45, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel2 = {
	1, 1, 70, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel3 = {
	1, 1, 95, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatH1 = {
	PARAGRAPH_OFFSET+5, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 2.4f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH2 = {
	PARAGRAPH_OFFSET, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 2.0f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH3 = {
	PARAGRAPH_OFFSET-5, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 1.6f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH4 = {
	PARAGRAPH_OFFSET/2, 3, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 1.5f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH5 = {
	PARAGRAPH_OFFSET/3, 3, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 1.4f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH6 = {
	PARAGRAPH_OFFSET/4, 3, DEFAULT_LEFT_MARGIN, 20, .mef_fontSize.ss_value = 1.2f, .mef_fontSize.ss_units = CSU_PERCENT, FW_BOLD
};

static MARGINS _tableMargins = {
	10,5,8,8
};

/*
 * The type of a defined reference.
 */
typedef enum MD_DEFINED_REFERENCE_TYPE {
	MDDRT_REFERENCE_STYLE_LINK,
	MDDRT_REFERENCE_FOOT_NOTE
} MD_DEFINED_REFERENCE_TYPE;

/*
 * Defines a piece of text one can refer to. Two forms exist:
 * - a reference style link URL definition. mdrs_text is the defined URL.
 * - The definition of footnote. mdrs_text is the text of the footnote.
 */
typedef struct tagMD_REFERENCE_DEFINITION {
	struct tagMD_REFERENCE_DEFINITION* mdrsl_next;
	MD_DEFINED_REFERENCE_TYPE mdrs_type;
	// The name of the reference style link
	char* mdrs_name;
	// The URL defined by the reference (reference style links) or the footnote text.
	char* mdrs_text;
	// An optional title
	char* mdrs_title;
} MD_REFERENCE_DEFINITION;

typedef struct tagMARKDOWN_RENDERER_DATA {
	RENDER_VIEW_PART* md_pElements;	// The render view parts to be rendered
	HWND md_hwndTooltip;
	TEXT_RUN* md_focussedRun;
	RENDER_VIEW_PART* md_focussedPart;
	RENDER_VIEW_PART* md_caretView;
	RENDER_VIEW_PART* md_previousCaretView;
	TEXT_RUN* md_caretRun;
	HASHMAP* md_imageCache;
	MD_REFERENCE_DEFINITION* md_referenceDefinitions;
	int md_caretRunIndex;
	int md_caretPartIndex;
	BOOL md_printing;
	BOOL md_needsSBUdpdate;
	float md_zoomFactor;
	void (*md_parse)(WINFO* wp);
} MARKDOWN_RENDERER_DATA;

/*
 * Checks, whether a point is within the bounds of a text run.
 */
static BOOL runbounds_contains(RUN_BOUNDS* pBounds, POINT pt) {
	if (pt.y >= pBounds->top1 && pt.y < pBounds->top) {
		return pt.x >= pBounds->left1 && pt.x < pBounds->right;
	}
	if (pt.y >= pBounds->bottom && pt.y < pBounds->bottom2) {
		return pt.x >= pBounds->left && pt.x < pBounds->right2;
	}
	return (pt.y >= pBounds->top && pt.y < pBounds->bottom&& pt.x >= pBounds->left && pt.x < pBounds->right);
}

/*
 * Perform CSS size calculations.Given a relative value calculate the size in logical markdown renderer pixels.
 */
static int mdr_calculateSize(SIZE_SPECIFICATION sSize, int nRelativeValue, float fZoomfactor) {
	switch (sSize.ss_units) {
	case CSU_PERCENT:
		return (int)(nRelativeValue * sSize.ss_value);
	case CSU_EM:
		return (int)(sSize.ss_value * 16 * fZoomfactor);
	case CSU_PT:
		return (int)(sSize.ss_value * 16 / 12 * fZoomfactor);
	case CSU_NONE:
		return (int)(nRelativeValue * fZoomfactor);
	default:
		return (int)(sSize.ss_value * fZoomfactor);
	}
}

static LOGFONT _defaultFont = {
	12,					// lfHeight;
	0,					// lfWidth;
	0,					// lfEscapement;
	0,					// lfOrientation;
	FW_NORMAL,			// lfWeight;

	0,					// lfItalic;
	0,					// lfUnderline;
	0,					// lfStrikeOut;

	ANSI_CHARSET,		// lfCharSet;
	OUT_DEVICE_PRECIS,	// lfOutPrecision;
	CLIP_DEFAULT_PRECIS,// lfClipPrecision;
	ANTIALIASED_QUALITY,// lfQuality;
	FF_DONTCARE, 		// lfPitchAndFamily;
};

static void mdr_releaseBitmapHandle(MD_IMAGE* pImage) {
	if (pImage->mdi_image && !pImage->mdi_cached) {
		DeleteObject(pImage->mdi_image);
	}
	pImage->mdi_image = NULL;
	pImage->mdi_cached = FALSE;
}

static void mdr_assignImageUrl(HASHMAP* pImageCache, TEXT_RUN* pRun, char* pszImageUrl) {
	HASH_ENTRY entry;
	if (pImageCache && hashmap_getEntry(pImageCache, (intptr_t)pszImageUrl, &entry)) {
		mdr_releaseBitmapHandle(pRun->tr_data.tr_image);
		pRun->tr_data.tr_image->mdi_image = (HBITMAP)entry.he_value;
		pRun->tr_data.tr_image->mdi_cached = TRUE;
	}
	pRun->tr_imageUrl = pszImageUrl;
}

/*
 * Create a font for the given font-attributes. 
 */
static HFONT mdr_createFont(HDC hdc, const FONT_ATTRIBUTES* pAttrs, float fZoom) {
	LOGFONT _lf;
	if (!_defaultFont.lfFaceName[0]) {
		const char* pszFontface = theme_textStyleFontface(WYSIWYG_FONT_STYLE_NAME, "Verdana");
		strcpy(_defaultFont.lfFaceName, pszFontface);
		_defaultFont.lfHeight = theme_textStyleFontsize(WYSIWYG_FONT_STYLE_NAME, 12);
	}
	memcpy(&_lf, &_defaultFont, sizeof _lf);
	long nHeight;
	nHeight = (int)(mdr_calculateSize(pAttrs->size, _defaultFont.lfHeight, 1.0f) * fZoom);
	if (pAttrs->subscript || pAttrs->superscript) {
		nHeight = nHeight < 0 ? -nHeight : nHeight/2;
	}
	_lf.lfHeight = nHeight;
	if (pAttrs->style) {
		const char* pszFontface = theme_textStyleFontface(pAttrs->style, 0);
		if (pszFontface) {
			strcpy(_lf.lfFaceName, pszFontface);
		}
	}
	if (pAttrs->weight) {
		_lf.lfWeight = pAttrs->weight;
	}
	_lf.lfItalic = (BYTE)pAttrs->italic;
	_lf.lfStrikeOut = (BYTE)pAttrs->strikeout;
	_lf.lfUnderline = pAttrs->rollover && pAttrs->underline;
	return CreateFontIndirect(&_lf);
}

static void mdr_paintRule(const RENDER_FLOW_PARAMS* pParams, int left, int right, int y, int nStrokeWidth) {
	THEME_DATA* pTheme = pParams->rfp_theme;
	HPEN hPen = CreatePen(PS_SOLID, nStrokeWidth, pTheme->th_dialogBorder);
	HDC hdc = pParams->rfp_hdc;
	HPEN hPenOld = SelectObject(hdc, hPen);
	MoveTo(hdc, left, y);
	LineTo(hdc, right, y);
	DeleteObject(SelectObject(hdc, hPenOld));
}

static MARGINS mdr_getScaledMargins(float zoomFactor, const MARGINS* pMargins) {
	MARGINS m = {
		.m_bottom = (int)(pMargins->m_bottom * zoomFactor),
		.m_top = (int)(pMargins->m_top * zoomFactor),
		.m_left = (int)(pMargins->m_left * zoomFactor),
		.m_right = (int)(pMargins->m_right * zoomFactor)
	};
	return m;
}

static void mdr_renderHorizontalRule(const RENDER_FLOW_PARAMS* pParams, const RECT* pBounds, RECT* pUsed) {
	RENDER_VIEW_PART* pPart = pParams->rfp_part;
	MARGINS m = mdr_getScaledMargins(pParams->rfp_zoomFactor, &pPart->rvp_margins);
	pUsed->top = pBounds->top + m.m_top;
	int nLeft = (int)(pParams->rfp_zoomFactor * DEFAULT_LEFT_MARGIN);
	pUsed->left = pBounds->left + nLeft;
	pUsed->bottom = pUsed->top + nLeft + m.m_bottom;
	pUsed->right = pBounds->right - 20;
	if (!pParams->rfp_measureOnly) {
		mdr_paintRule(pParams, pUsed->left, pUsed->right, (pUsed->top + pUsed->bottom - 3) / 2, 3);
	}
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
	pPart->rvp_height = pUsed->bottom - pBounds->top;
	pPart->rvp_layouted = TRUE;
}

static long mdr_codeBackgroundColor(THEME_DATA* pTheme) {
	return theme_textStyleBackground(pTheme, "code", RGB(100, 100, 25));
}

/*
 * Render a markdown table. 
 */
static void mdr_renderTable(const RENDER_FLOW_PARAMS* pParams, const RECT* pBounds, RECT* pUsed) {
	THEME_DATA* pTheme = pParams->rfp_theme;
	int nColumnWidth[MAX_TABLE_COLUMNS];
	memset(nColumnWidth, 0, sizeof nColumnWidth);
	struct tagROW_BORDER {
		int rb_header;
		int rb_y;
	} rowBorders[512];
	int nRowBorderIndex = 0;
	RENDER_VIEW_PART* pPart = pParams->rfp_part;
	RENDER_TABLE* pTable = pPart->rvp_data.rvp_table;
	int nColumns = pTable->rt_columnCount;
	MARGINS m = mdr_getScaledMargins(pParams->rfp_zoomFactor, &pPart->rvp_margins);
	int left = pBounds->left+m.m_left;
	int right = pBounds->right - m.m_right;
	int nWidth = right - left;
	nWidth = mdr_calculateSize(pPart->rvp_width, nWidth, pParams->rfp_zoomFactor);
	if (pPart->rvp_width.ss_units != CSU_NONE) {
		right = left + nWidth;
	}
	int nTotal = pTable->rt_totalColumnWidth;
	if (nTotal == 0) {
		nTotal = 1;
	}
	if (nWidth > nTotal && pPart->rvp_width.ss_units == CSU_NONE) {
		nWidth = (int)(nTotal * pParams->rfp_zoomFactor);
		right = left + nWidth;
	}
	for (int i = 0; i < MAX_TABLE_COLUMNS; i++) {
		if (!pTable->rt_columnWidths[i]) {
			break;
		}
		nColumnWidth[i] = pTable->rt_columnWidths[i] * nWidth / nTotal;
	}
	int y = pBounds->top + m.m_top;
	RENDER_TABLE_ROW* pRow = pTable->rt_rows;
	RECT bounds = { 
		.top = y
	};
	MARGINS mTableMargins = mdr_getScaledMargins(pParams->rfp_zoomFactor, &_tableMargins);
	while (pRow) {
		RENDER_TABLE_CELL* pCell = pRow->rtr_cells;
		RECT usedBounds = { 0 };
		RECT flowBounds;
		int nMaxHeight = 10;
		int nColumn = 0;
		BOOL bHeader = FALSE;
		bounds.left = left;
		while (pCell) {
			bHeader = pCell->rtc_isHeader;
			bounds.right = bounds.left + nColumnWidth[nColumn];
			bounds.bottom = bounds.top + 30000;
			if (pCell->rtc_flow.tf_text) {
				byte align = pCell->rtc_flow.tf_align;
				if (!align && pCell->rtc_isHeader) {
					align = TA_ALIGN_CENTER;
				}
				mdr_renderTextFlow(&mTableMargins, &pCell->rtc_flow, & bounds, &flowBounds, &usedBounds, 0, align, pParams);
			}
			int nHeight = usedBounds.bottom - usedBounds.top;
			if (nHeight > nMaxHeight) {
				nMaxHeight = nHeight;
			}
			nColumn++;
			bounds.left = bounds.right;
			pCell = pCell->rtc_next;
		}
		pRow = pRow->rtr_next;
		bounds.top += nMaxHeight;
		if (pRow) {
			rowBorders[nRowBorderIndex].rb_y = bounds.top + _tableMargins.m_bottom / 2;
			rowBorders[nRowBorderIndex].rb_header = bHeader;
			if (nRowBorderIndex < DIM(rowBorders)-1) {
				nRowBorderIndex++;
			}
		}
	}
	int nTableHeight = bounds.top - y + _tableMargins.m_bottom;
	if (!pParams->rfp_measureOnly && pTable->rt_borderWidth > 0) {
		COLORREF cColor = pTable->rt_borderColor;
		if (cColor == NO_COLOR) {
			cColor = mdr_codeBackgroundColor(pTheme);
		}
		HDC hdc = pParams->rfp_hdc;
		HPEN hPen = CreatePen(PS_SOLID, pTable->rt_borderWidth, cColor);
		HPEN hPenOld = SelectObject(hdc, hPen);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, left, y, right, y + nTableHeight);
		int x = left;
		int nDelta = pTable->rt_borderWidth / 2;
		for (int i = 0; i < nColumns; i++) {
			x += nColumnWidth[i];
			MoveTo(hdc, x, y);
			LineTo(hdc, x, y + nTableHeight - nDelta);
		}
		for (int i = 0; i < nRowBorderIndex; i++) {
			int nLineY = rowBorders[i].rb_y;
			MoveTo(hdc, left, nLineY);
			LineTo(hdc, right, nLineY);
			if (rowBorders[i].rb_header) {
				MoveTo(hdc, left, nLineY + 1);
				LineTo(hdc, right, nLineY + 1);
			}
		}
		DeleteObject(SelectObject(hdc, hPenOld));
	}
	pUsed->top = y;
	pUsed->left = left;
	pUsed->right = right;
	pUsed->bottom = pUsed->top + nTableHeight;
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
	pPart->rvp_height = pUsed->bottom - pBounds->top;
	pPart->rvp_layouted = TRUE;
}


static void mdr_imageLoaded(HWND hwnd, void* pParam, HBITMAP hBmp, int dwError) {
	TEXT_RUN* pRun = (TEXT_RUN*)pParam;
	MD_IMAGE* pImage = pRun->tr_data.tr_image;
	mdr_releaseBitmapHandle(pImage);
	if (hBmp == NULL) {
		if (dwError != ERROR_INTERNET_CONNECTION_RESET) {
			pImage->mdi_image = LoadIcon(hInst, MAKEINTRESOURCE(IDB_BROKEN_IMAGE));
		}
		char szTitle[128];
		sprintf_s(szTitle, sizeof szTitle, "Image load error %d", dwError);
		if (pRun->tr_title) {
			free(pRun->tr_title);
		}
		pRun->tr_title = _strdup(szTitle);
		pImage->mdi_icon = TRUE;
	} else {
		pImage->mdi_image = hBmp;
		pImage->mdi_icon = FALSE;
	}
	WINFO* wp = ww_winfoFromWorkwinHandle(hwnd);
	if (wp != NULL && wp->r_data != NULL) {
		mdr_invalidateViewpartsLayout(wp->r_data);
	}
	InvalidateRect(hwnd, NULL, FALSE);
}

static BOOL mdr_loadAndMeasureImage(HWND hwnd, TEXT_RUN* pRun, int *pWidth, int * pHeight, int *pOrigWidth, int *pOrigHeight) {
	MD_IMAGE* pImage = pRun->tr_data.tr_image;
	char* pszImageName = pRun->tr_imageUrl;
	if (!pImage->mdi_image) {
		char* pszExt = strrchr(pszImageName, '.');
		if (pszExt && _stricmp(pszExt + 1, "bmp") != 0) {
			IMAGE_LOAD_ASYNC async = {
				.ila_complete = mdr_imageLoaded,
				.ila_completionParam = pRun,
				.ila_hwnd = hwnd
			};
			IMAGE_LOAD_RESULT result = loadimage_load((char*)pszImageName, async);
			if (result.ilr_bitmap != NULL) {
				pImage->mdi_image = result.ilr_bitmap;
			} else if (result.ilr_loading) {
				pImage->mdi_image = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LOADING_IMAGE));
				pImage->mdi_icon = TRUE;
			}
		} else {
			pImage->mdi_image = LoadImage(NULL, pszImageName, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		}
		if (pImage->mdi_image == NULL) {
			pImage->mdi_image = LoadIcon(hInst, MAKEINTRESOURCE(IDB_BROKEN_IMAGE));
			pImage->mdi_icon = TRUE;
		}
	}
	if (pImage->mdi_image) {
		HBITMAP hbmp = pImage->mdi_image;
		if (pImage->mdi_icon) {
			ICONINFO info = { 0 };
			info.fIcon = TRUE;
			GetIconInfo(pImage->mdi_image, &info);
			hbmp = info.hbmColor;
			if (hbmp == NULL) {
				return FALSE;
			}
		}
		BITMAP bitmap = { 0 };
		GetObject(hbmp, sizeof(bitmap), &bitmap);
		if (bitmap.bmWidth == 0 || bitmap.bmHeight == 0) {
			return FALSE;
		}
		*pOrigWidth = bitmap.bmWidth;
		*pOrigHeight = bitmap.bmHeight;
		*pWidth = pImage->mdi_width ? pImage->mdi_width :
			(pImage->mdi_height ? bitmap.bmWidth * pImage->mdi_height / bitmap.bmHeight : bitmap.bmWidth);
		*pHeight = pImage->mdi_height ? pImage->mdi_height :
			(pImage->mdi_width ? bitmap.bmHeight * pImage->mdi_width / bitmap.bmWidth : bitmap.bmHeight);
		return TRUE;
	}
	return FALSE;
}

/*
 * Paint an image displayed in a markdown text.
 */
static void mdr_paintImage(HDC hdc, TEXT_RUN* pTR, int x, int y, int nMaxWidth, SIZE* pSize, float zoomFactor, BOOL bMeasureOnly) {
	MD_IMAGE* pImage = pTR->tr_data.tr_image;
	if (!pImage) {
		pSize->cx = 20;
		pSize->cy = 20;
		return;
	}
	int nWidth;
	int nHeight;
	int nOrigWidth;
	int nOrigHeight;
	if (mdr_loadAndMeasureImage(WindowFromDC(hdc), pTR, &nWidth, &nHeight, &nOrigWidth, &nOrigHeight)) {
		HGDIOBJ         oldBitmap;
		HDC             hdcMem;
		if (nWidth > nMaxWidth) {
			nWidth = nMaxWidth;
		}
		nWidth = (int)(nWidth * zoomFactor);
		nHeight = (int)(nHeight * zoomFactor);
		int nBorder = pImage->mdi_borderWidth;
		if (!bMeasureOnly) {
			int nDelta = nBorder / 2;
			if (pImage->mdi_icon) {
				DrawIcon(hdc, x + nDelta, y + nDelta, pImage->mdi_image);
			} else {
				hdcMem = CreateCompatibleDC(hdc);
				oldBitmap = SelectObject(hdcMem, pImage->mdi_image);
				SetStretchBltMode(hdc, COLORONCOLOR);
				TransparentBlt(hdc, x + nDelta, y + nDelta, nWidth, nHeight, hdcMem, 0, 0, nOrigWidth, nOrigHeight, RGB(0,0,0));
				SelectObject(hdcMem, oldBitmap);
				DeleteDC(hdcMem);
			}
			if (nBorder) {
				HPEN hPen = CreatePen(PS_SOLID, nBorder, pImage->mdi_borderColor);
				HBRUSH hBrush = GetStockBrush(NULL_BRUSH);
				HBRUSH hBrushOld = SelectObject(hdc, hBrush);
				HPEN hPenOld = SelectObject(hdc, hPen);
				Rectangle(hdc, x, y, x + nWidth + nBorder, y + nHeight + nBorder);
				SelectObject(hdc, hBrushOld);
				DeleteObject(SelectObject(hdc, hPenOld));
			}
		}
		pSize->cx = nWidth+nBorder;
		pSize->cy = nHeight+nBorder;
	} else {
		char szFilename[512];
		const char* pszFilepart = string_getBaseFilename(pTR->tr_imageUrl);
		sprintf(szFilename, "Cannot load image %s", pszFilepart);
		SIZE size;
		GetTextExtentPoint(hdc, szFilename, (int)strlen(szFilename), &size);
		if (!bMeasureOnly) {
			TextOut(hdc, x, y, szFilename, (int)strlen(szFilename));
		}
		pSize->cy = size.cy;
		pSize->cx = size.cx;
	}
}

/*
 * Paint an emoji.
 */
static void mdr_renderEmoji(HDC hdc, WCHAR* pEmoji, COLORREF cColor, int x, int y, float zoomFactor, SIZE* pSize) {
	paint_emojid2d(hdc, pEmoji, cColor, (int)(12 * zoomFactor), x, y, &pSize->cx, &pSize->cy);
}

/*
 * Paint a checkbox for a task list. 
 */
static void mdr_paintCheckmark(RENDER_FLOW_PARAMS* pParams, int x, int y, float zoomFactor, BOOL bChecked) {
	HDC hdc = pParams->rfp_hdc;
	const THEME_DATA* pTheme = pParams->rfp_theme;
	LOGBRUSH brush = {
		.lbColor = pTheme->th_dialogBorder,
		.lbHatch = 0,
		.lbStyle = PS_SOLID
	};
	HPEN hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 2, &brush, 0, NULL);
	HPEN hPenOld = SelectObject(hdc, hPen);
	int nSize = (int)(13 * zoomFactor);
	MoveTo(hdc, x, y);
	LineTo(hdc, x+nSize, y);
	LineTo(hdc, x + nSize, y+nSize);
	LineTo(hdc, x, y + nSize);
	LineTo(hdc, x, y);
	if (bChecked) {
		HPEN hPen2 = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 4, &brush, 0, NULL);
		HPEN hPenOld2 = SelectObject(hdc, hPen2);
		MoveTo(hdc, x+2, y+(nSize/2));
		LineTo(hdc, x + nSize / 3 + 1, y + nSize - 5);
		LineTo(hdc, x + nSize - 4, y + 4);
		DeleteObject(SelectObject(hdc, hPenOld2));
	}
	DeleteObject(SelectObject(hdc, hPenOld));
}

static void mdr_paintSelection(HDC hdc, int x, int y, const RENDER_FLOW_PARAMS* pRFP, TEXT_RUN* pRun, int nOffs, int nDeltaPainted, int nFit, TEXT_FLOW* pFlow) {
	SIZE startSize;
	SIZE selectionSize;
	int nUnused;
	char* pszText = pFlow->tf_text;

	int nStart = pRun->tr_selectionStart - nDeltaPainted;
	if (nStart < 0 || nStart > nFit) {
		return;
	}
	GetTextExtentExPoint(hdc, &pszText[nOffs], nStart,
		20000, &nUnused, 0, &startSize);
	GetTextExtentExPoint(hdc, &pszText[nOffs + nStart], pRun->tr_selectionLength,
		2000, &nUnused, 0, &selectionSize);
	RECT selectionRect = {
		.top = y,
		.bottom = selectionRect.top + selectionSize.cy,
		.left = x + startSize.cx,
		.right = selectionRect.left + selectionSize.cx
	};
	render_paintSelectionRect(hdc, &selectionRect);
}

static void mdr_getLineExtent(HDC hdc, TEXT_FLOW* pTF, TEXT_RUN* pRun, float fZoom, int nOffsetInText, int nTotalWidthInPixels, int nCurrentFit, int* pWidth) {
	SIZE size;
	int nFit;
	GetTextExtentExPoint(hdc, &pTF->tf_text[nOffsetInText], (int)nCurrentFit, nTotalWidthInPixels, &nFit, 0, &size);
	*pWidth = size.cx;
	nTotalWidthInPixels -= size.cx;
	HFONT hOldFont = NULL;
	while (size.cx < nTotalWidthInPixels) {
		pRun = pRun->tr_next;
		if (!pRun) {
			break;
		}
		if (hOldFont) {
			DeleteObject(SelectObject(hdc, hOldFont));
		}
		HFONT hFont = mdr_createFont(hdc, &pRun->tr_attributes, fZoom);
		hOldFont = SelectObject(hdc, hFont);
		nOffsetInText += nFit;
		GetTextExtentExPoint(hdc, &pTF->tf_text[nOffsetInText], (int)pRun->tr_size, nTotalWidthInPixels, &nFit, 0, &size);
		if (nFit < pRun->tr_size) {
			GetTextExtentExPoint(hdc, &pTF->tf_text[nOffsetInText], nFit, nTotalWidthInPixels, &nFit, 0, &size);
			*pWidth += size.cx;
			break;
		}
		*pWidth += size.cx;
		nTotalWidthInPixels -= size.cx;
	}
	if (hOldFont) {
		DeleteObject(SelectObject(hdc, hOldFont));
	}
}

static void mdr_updateHorizontalPartBounds(RECT* pPartBounds, int x, int x2) {
	if (x < pPartBounds->left) {
		pPartBounds->left = x;
	}
	if (x2 > pPartBounds->right) {
		pPartBounds->right = x2;
	}
}

/*
 * Highlight the section where the caret is.
 */
static void mdr_highlightCaretLine(const RENDER_FLOW_PARAMS* pRFP, RENDER_VIEW_PART* pPart) {
	if (!pRFP->rfp_printing && pPart->rvp_layouted) {
		EdTRACE(log_message(DEBUG_TRACE, "mdr_highlightCaretLine(0x%x)", pPart->rvp_lpStart));
		HBRUSH hBrushCaretLine;
		hBrushCaretLine = CreateSolidBrush(pRFP->rfp_theme->th_caretLineColor);
		FillRect(pRFP->rfp_hdc, &pPart->rvp_bounds, hBrushCaretLine);
		DeleteObject(hBrushCaretLine);
	}
}

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(MARGINS* pMargins, TEXT_FLOW* pFlow, RECT* pBounds, RECT* pPartBounds, 
		RECT* pUsed, int nBlockQuoteLevel, int nAlign, const RENDER_FLOW_PARAMS *pRFP) {
	THEME_DATA* pTheme = pRFP->rfp_theme;
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	int nRight = pBounds->right - pMargins->m_right;
	int nDeltaPainted;
	pUsed->top = pBounds->top;
	pUsed->left = x;
	pUsed->right = nRight;
	pPartBounds->left = x;
	pPartBounds->right = x;
	TEXT_FLOW* pTF = pFlow;
	TEXT_RUN* pTR = pTF->tf_runs;
	int nOffs = 0;
	size_t nLen;
	nLen = pTR->tr_size;
	int nHeight = 0;
	HDC hdc = pRFP->rfp_hdc;
	font_setDefaultTextColors(hdc, pTheme);
	COLORREF cDefault = GetTextColor(hdc);
	HFONT hFont = mdr_createFont(hdc, &pTR->tr_attributes, pRFP->rfp_zoomFactor);
	HFONT hOldFont = SelectObject(hdc, hFont);
	if (pRFP->rfp_skipSpace) {
		while (nLen > 0 && isspace((unsigned char)pTF->tf_text[nOffs])) {
			nOffs++;
			nLen--;
		}
	}
	pPartBounds->top = y;
	BOOL bRunBegin = TRUE;
	BOOL bStartOfLine = TRUE;
	int nDeltaX = 0;
	while (pTR) {
		SIZE size;
		int nFit = 0;
		if (pTR->tr_attributes.style) {
			pTR->tr_attributes.bgColor = theme_textStyleBackground(pTheme, pTR->tr_attributes.style, RGB(100, 100, 25));
		}
		if (pTR->tr_attributes.bgColor == NO_COLOR) {
			SetBkMode(hdc, TRANSPARENT);
		} else {
			SetBkMode(hdc, OPAQUE);
			SetBkColor(hdc, pTR->tr_attributes.bgColor);
		}
		if (bRunBegin) {
			x += pTR->tr_attributes.indent;
		}
		if (pTR->tr_isEmoji) {
			mdr_renderEmoji(hdc, pTR->tr_data.tr_emoji, pTR->tr_attributes.fgColor, x, y, pRFP->rfp_zoomFactor, &size);
			pPartBounds->bottom = y + size.cy;
			mdr_updateHorizontalPartBounds(pPartBounds, x, x + size.cx);
			pTR->tr_bounds.top1 = pTR->tr_bounds.top = y - pPartBounds->top;
			pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = pPartBounds->bottom - pPartBounds->top;
			pTR->tr_bounds.left = pTR->tr_bounds.left1 = x - pPartBounds->left;
			pTR->tr_bounds.right = pTR->tr_bounds.right2 = pPartBounds->right - pPartBounds->left;
		} else if (pTR->tr_data.tr_image) {
			paint_endEmoji();
			mdr_paintImage(hdc, pTR, x, y, nRight-x, &size, pRFP->rfp_zoomFactor, pRFP->rfp_measureOnly);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPartBounds->bottom = y + nHeight;
			mdr_updateHorizontalPartBounds(pPartBounds, x, x + size.cx);
			pTR->tr_bounds.top1 = pTR->tr_bounds.top = y - pPartBounds->top;
			pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = pPartBounds->bottom - pPartBounds->top;
			pTR->tr_bounds.left = pTR->tr_bounds.left1 = x - pPartBounds->left;
			pTR->tr_bounds.right = pTR->tr_bounds.right2 = pPartBounds->right - pPartBounds->left;
		} else {
			char* pText = &pTF->tf_text[nOffs];
			for (int i = 0; i < nLen; i++) {
				if (pText[i] != ' ') {
					paint_endEmoji();
					break;
				}
			}
			int nTotalWidth = nRight - x;
			GetTextExtentExPoint(hdc, pText, (int)nLen, nTotalWidth, &nFit, 0 /* TODO: callback for measuring*/, &size);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPartBounds->bottom = y + nHeight;
			if (nOffs == 0) {
				pPartBounds->left = x;
				pPartBounds->right = nRight;
			} else {
				mdr_updateHorizontalPartBounds(pPartBounds, x, nRight);
			}
			if (bRunBegin) {
				nDeltaPainted = 0;
				pTR->tr_bounds.top1 = pTR->tr_bounds.top = y - pPartBounds->top;
				pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = pTR->tr_bounds.top + nHeight;
				pTR->tr_bounds.left = pTR->tr_bounds.left1 = x - pPartBounds->left;
				pTR->tr_bounds.right = pTR->tr_bounds.right2 = x + size.cx - pPartBounds->left;
			}
			else {
				if (pTR->tr_bounds.top == pTR->tr_bounds.top1) {
					pTR->tr_bounds.top = y - pPartBounds->top;
				}
				if (nFit < nLen) {
					pTR->tr_bounds.bottom = y + nHeight - pPartBounds->top;
				}
				else {
					pTR->tr_bounds.bottom2 = y + nHeight - pPartBounds->top;
				}
				pTR->tr_bounds.left = x - pPartBounds->left;
				pTR->tr_bounds.right2 = x + size.cx - pPartBounds->left;
			}
			if (nFit < nLen) {
				int nFindSpace = 0;
				while (nFit + nFindSpace > 0 && !string_isSpace(pTF->tf_text[nOffs + nFit + nFindSpace])) {
					nFindSpace--;
				}
				if (nFit + nFindSpace > 0) {
					nFit += nFindSpace;
				}
			}
			int nDelta = nHeight > size.cy ? (nHeight - size.cy) / 2 : 0;
			if (bStartOfLine && nAlign != TA_ALIGN_LEFT && nAlign != TA_ALIGN_DEFAULT) {
				int nWidth;
				mdr_getLineExtent(hdc, pTF, pTR, pRFP->rfp_zoomFactor, nOffs, nTotalWidth, nFit, &nWidth);
				if (nAlign == TA_ALIGN_RIGHT) {
					nDeltaX = nTotalWidth - nWidth;
				}
				else if (nAlign == TA_ALIGN_CENTER) {
					nDeltaX = (nTotalWidth - nWidth) / 2;
				}
			}
			SetTextColor(hdc, pTR->tr_attributes.fgColor == NO_COLOR ? cDefault : pTR->tr_attributes.fgColor);
			int nSuperDelta = size.cy / 2;
			if (pTR->tr_attributes.subscript) {
				nDelta += nSuperDelta;
			}
			else if (pTR->tr_attributes.superscript) {
				nDelta -= nSuperDelta;
			}
			if (!pRFP->rfp_measureOnly) {
				TextOut(hdc, x + nDeltaX, y + nDelta, &pTF->tf_text[nOffs], nFit);
				RECT r = {
					.left = x + nDeltaX,
					.top = y + nDelta
				};
				if (pTR->tr_selectionLength) {
					mdr_paintSelection(hdc, r.left, r.top, pRFP, pTR, nOffs, nDeltaPainted, nFit, pTF);
				}
				if (pTR->tr_attributes.focussed && pRFP->rfp_focus) {
					r.right = r.left + size.cx;
					r.bottom = r.top + size.cy;
					DrawFocusRect(hdc, &r);
				}
				if (nBlockQuoteLevel) {
					int blockQuoteDelta = (int)(5 * pRFP->rfp_zoomFactor);
					int blockQuoteHorizontalDelta = (int)(BLOCK_QUOTE_INDENT * pRFP->rfp_zoomFactor);
					RECT leftRect = {
						.top = y - blockQuoteDelta,
						.bottom = y + nHeight + blockQuoteDelta
					};
					leftRect.left = pBounds->left + (int)(pRFP->rfp_zoomFactor * _formatText.mef_margins.m_left);
					leftRect.right = leftRect.left + blockQuoteDelta;
					for (int i = 0; i < nBlockQuoteLevel; i++) {
						HBRUSH hBrush = CreateSolidBrush(mdr_codeBackgroundColor(pTheme));
						FillRect(hdc, &leftRect, hBrush);
						DeleteObject(hBrush);
						leftRect.left += blockQuoteHorizontalDelta;
						leftRect.right += blockQuoteHorizontalDelta;
					}
				}
			}
		}
		if (!pTR->tr_data.tr_image && nFit < nLen && nFit > 0) {
			nOffs += nFit;
			nDeltaPainted += nFit;
			x = pUsed->left;
			bStartOfLine = TRUE;
			y += nHeight;
			nHeight = 0;
			nLen -= nFit;
			while (nLen > 0 && string_isSpace(pTF->tf_text[nOffs])) {
				nOffs++;
				nLen--;
				nDeltaPainted++;
			}
			bRunBegin = FALSE;
		}
		else {
			if (pTR->tr_attributes.lineBreak) {
				x = pUsed->left;
				bStartOfLine = TRUE;
				y += nHeight;
				nHeight = 0;
			}
			else {
				x += size.cx;
				bStartOfLine = FALSE;
			}
			nOffs += (int)nLen;
			pTR = pTR->tr_next;
			if (!pTR) {
				break;
			}
			bRunBegin = TRUE;
			nLen = pTR->tr_size;
			DeleteObject(SelectObject(hdc, hOldFont));
			hFont = mdr_createFont(hdc, &pTR->tr_attributes, pRFP->rfp_zoomFactor);
			hOldFont = SelectObject(hdc, hFont);
		}
	}
	DeleteObject(SelectObject(hdc, hOldFont));
	pUsed->bottom = y + nHeight + pMargins->m_bottom;
	paint_endEmoji();
}

static void mdr_paintFillDecoration(const RENDER_FLOW_PARAMS* pRFP, const RENDER_VIEW_PART* pPart, RECT* pBounds) {
	HDC hdc = pRFP->rfp_hdc;
	const THEME_DATA* pData = pRFP->rfp_theme;
	MARGINS m = mdr_getScaledMargins(pRFP->rfp_zoomFactor, & pPart->rvp_margins);
	int x = pBounds->left + m.m_left;
	int y = pBounds->top + m.m_top;
	int nRight = pBounds->right - m.m_right;

	RENDER_BOX_DECORATION* pDecoration = pPart->rvp_decoration;
	int nWhitespace = m.m_top / 2;
	RECT r = {
		.top = y - m.m_top + nWhitespace,
		.left = x - (int)(10 * pRFP->rfp_zoomFactor),
		.right = nRight
	};
	pBounds->left = r.left;
	r.bottom = r.top + (pPart->rvp_bounds.bottom - pPart->rvp_bounds.top) + m.m_bottom + m.m_top - nWhitespace;
	int nWidth = pDecoration->rbd_strokeWidth;
	if (!nWidth) {
		nWidth = 1;
	}
	COLORREF cColor = pDecoration->rbd_useThemeStrokeColor ? pData->th_dialogBorder : pDecoration->rbd_strokeColor;
	COLORREF cFillColor = pDecoration->rbd_useThemeFillColor ? mdr_codeBackgroundColor(pRFP->rfp_theme) : pDecoration->rbd_fillColor;
	if (pRFP->rfp_isCaret && (pRFP->rfp_wp->dispmode & SHOW_CARET_LINE_HIGHLIGHT)) {
		cColor = pRFP->rfp_theme->th_caretLineColor;
		nWidth = 3;
	}
	HPEN hPen = cColor == NO_COLOR ? GetStockObject(NULL_PEN) : CreatePen(PS_SOLID, nWidth, cColor);
	HBRUSH hBrush = cFillColor == NO_COLOR ? GetStockBrush(NULL_BRUSH) : CreateSolidBrush(cFillColor);
	HBRUSH hBrushOld = SelectObject(hdc, hBrush);
	HPEN hPenOld = SelectObject(hdc, hPen);
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	if (cFillColor != NO_COLOR) {
		DeleteObject(SelectObject(hdc, hBrushOld));
	}
	if (cColor != NO_COLOR) {
		DeleteObject(SelectObject(hdc, hPenOld));
	}
}
/*
 * Render a "text type block part".
 */
static void mdr_renderMarkdownBlockPart(RENDER_FLOW_PARAMS* pParams, RECT* pBounds, RECT* pUsed) {
	RENDER_VIEW_PART* pPart = pParams->rfp_part;
	HDC hdc = pParams->rfp_hdc;
	MARGINS m = mdr_getScaledMargins(pParams->rfp_zoomFactor, &pPart->rvp_margins);
	int x = pBounds->left + m.m_left;
	int y = pBounds->top + m.m_top;
	RECT renderBounds = *pBounds;
	if (pPart->rvp_width.ss_units != CSU_NONE) {
		int nWidth = mdr_calculateSize(pPart->rvp_width, renderBounds.right - m.m_right - x, pParams->rfp_zoomFactor);
		renderBounds.right = x + nWidth + m.m_right;
	}
	TEXT_FLOW* pFlow = &pPart->rvp_data.rvp_flow;
	if (!pParams->rfp_measureOnly && pFlow->tf_runs) {
		font_setDefaultTextColors(hdc, pParams->rfp_theme);
		HFONT hFont = mdr_createFont(hdc, &pPart->rvp_data.rvp_flow.tf_runs->tr_attributes, pParams->rfp_zoomFactor);
		HFONT hOldFont = SelectObject(hdc, hFont);
		if (pPart->rvp_type == MET_UNORDERED_LIST) {
			TextOutW(hdc, x - (int)(15* pParams->rfp_zoomFactor), y, pPart->rvp_param.rvp_level == 1 ? L"\u25CF" : (pPart->rvp_param.rvp_level == 2 ? L"\u25CB" : L"\u25A0"), 1);
		} else if (pPart->rvp_type == MET_TASK_LIST) {
			mdr_paintCheckmark(pParams, x - (int)(20 * pParams->rfp_zoomFactor), y, pParams->rfp_zoomFactor, pPart->rvp_param.rvp_number == 1);
		}
		else if (pPart->rvp_type == MET_ORDERED_LIST) {
			char szBuf[20];
			sprintf(szBuf, "%ld.", pPart->rvp_param.rvp_number);
			TextOut(hdc, x - (int)(20*pParams->rfp_zoomFactor), y, szBuf, (int)strlen(szBuf));
		}
		if (pPart->rvp_decoration) {
			mdr_paintFillDecoration(pParams, pPart, &renderBounds);
		}
		DeleteFont(SelectObject(hdc, hOldFont));
	}
	int nDCId = SaveDC(hdc);
	if (pFlow->tf_text && pFlow->tf_runs) {
		mdr_renderTextFlow(&m, pFlow, &renderBounds, &pPart->rvp_bounds, pUsed,
				pPart->rvp_type == MET_BLOCK_QUOTE ? pPart->rvp_param.rvp_level : 0, pFlow->tf_align, pParams);
	} else {
		pUsed->bottom = y + 10 + m.m_bottom;
	}
	if (renderBounds.left < pUsed->left) {
		pUsed->left = renderBounds.left;
	}
	if (!pParams->rfp_measureOnly && pPart->rvp_type == MET_HEADER && pPart->rvp_param.rvp_level < 3) {
		int nMargin = (int)(DEFAULT_LEFT_MARGIN * pParams->rfp_zoomFactor);
		mdr_paintRule(pParams, renderBounds.left + nMargin, renderBounds.right - nMargin, pUsed->bottom - 10, 1);
	}
	RestoreDC(hdc, nDCId);
	pPart->rvp_height = pUsed->bottom - renderBounds.top;
	pPart->rvp_layouted = TRUE;
}

/*
 * Check, whether we match the begin or end of a literal marker. If matched, return the length matched (e.g. 3 for ```).
 */
static int mdr_matchFixBlock(INPUT_STREAM* pStream, MDR_SYNTAX* pSyntax) {
	const char* pMarker = pSyntax->syn_literalMarker;
	if (pMarker) {
		size_t nLen = strlen(pMarker);
		if (pStream->is_strncmp(pStream, pMarker, nLen) == 0) {
			return (int)nLen;
		}
	}
	pMarker = pSyntax->syn_literalMarker2;
	if (pMarker) {
		size_t nLen = strlen(pMarker);
		if (pStream->is_strncmp(pStream, pMarker, nLen) == 0) {
			return (int)nLen;
		}
	}
	return 0;
}

static BOOL mdr_isTopLevelOrBreak(INPUT_STREAM* pStream, MDR_SYNTAX* pSyntax, MDR_ELEMENT_TYPE mCurrentType, int nLevel) {
	char c;
	if ((c = pStream->is_peekc(pStream, 0)) == 0) {
		return FALSE;
	}
	if (c == '\n') {
		return TRUE;
	}
	BOOL bRet = TRUE;
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	while (1) {
		if (mdr_matchFixBlock(pStream, pSyntax)) {
			break;
		}
		if (IS_UNORDERED_LIST_START(c) || c == '#') {
			break;
		}
		if (isdigit((unsigned char)c)) {
			while ((c = pStream->is_getc(pStream)) != 0) {
				if (c == '.') {
					bRet = TRUE;
					break;
				}
				if (!isdigit((unsigned char)c)) {
					bRet = FALSE;
					break;
				}
			}
			break;
		}
		if (c == '>') {
			if (mCurrentType != MET_BLOCK_QUOTE) {
				break;
			}
			int nLevelNew = 1;
			while (pStream->is_peekc(pStream, 1) == '>') {
				nLevelNew++;
				pStream->is_skip(pStream, 1);
			}
			bRet = nLevel != nLevelNew;
			break;
		} else if (mCurrentType == MET_BLOCK_QUOTE) {
			break;
		}
		if (!string_isSpace(c)) {
			bRet = mCurrentType == MET_HEADER;
			break;
		}
		c = pStream->is_getc(pStream);
	}
	pStream->is_seek(pStream, offset);
	return bRet;
}

static int mdr_getLevelFromIndent(INPUT_STREAM* pStream, char aListChar) {
	int cCol = 0;
	int nRet = 1;
	STREAM_OFFSET offset = pStream->is_positionToLineStart(pStream, 0);

	for (int i = 0; ; i++) {
		char c = pStream->is_peekc(pStream, i);
		if (c == '\n') {
			break;
		}
		if (c == ' ') {
			cCol++;
		}
		else if (c == '\t') {
			cCol += 2;
		} else {
			nRet = 1 + (cCol / 2);
			break;
		}
	}
	pStream->is_seek(pStream, offset);
	return nRet;
}

static BOOL asciidoc_detectLink(INPUT_STREAM* pStream, char c) {
	if (c != 'x' && c != 'l' && c != 'h') {
		return FALSE;
	}
	return pStream->is_strncmp(pStream, "link:", 5) == 0 || pStream->is_strncmp(pStream, "xref:", 5) == 0 
		|| pStream->is_strncmp(pStream, "https:", 6) == 0
		|| pStream->is_strncmp(pStream, "htts:", 5) == 0;
}

static int asciidoc_getLevelFromIndent(INPUT_STREAM* pStream, char aListChar) {
	int nRet = 1;

	while (TRUE) {
		char c = pStream->is_peekc(pStream, 1);
		if (c == '\n' || c == 0) {
			break;
		}
		if (c == aListChar) {
			pStream->is_skip(pStream, 1);
			nRet++;
		} else {
			break;
		}
	}
	return nRet;
}

/*
 * Skip all space in the input. Return TRUE, if we found at least one space character.
 */
static BOOL mdr_skipSpace(INPUT_STREAM* pStream, int nOffset) {
	BOOL bFound = FALSE;
	char c;
	while ((c = pStream->is_peekc(pStream, nOffset)) == ' ' || c == '\t') {
		bFound = TRUE;
		pStream->is_skip(pStream, 1);
	}
	return bFound;
}

/*
 * Returns the default element style format for a given render element type.
 */
static MDR_ELEMENT_FORMAT* mdr_getFormatFor(MDR_ELEMENT_TYPE mType, int nLevel) {
	switch (mType) {
	case MET_HEADER:
		switch (nLevel) {
		case 1: return &_formatH1; 
		case 2: return &_formatH2; 
		case 3: return &_formatH3; 
		case 4: return &_formatH4; 
		case 5: return &_formatH5; 
		default: return &_formatH6; 
		}
	case MET_PARAGRAPH:
		return &_formatParagraph;
	case MET_FENCED_CODE_BLOCK:
		return &_formatFenced;
	case MET_TASK_LIST:
	case MET_ORDERED_LIST:
	case MET_UNORDERED_LIST: return nLevel== 1 ? &_formatListLevel1 : (nLevel == 2 ? &_formatListLevel2 : &_formatListLevel3); 
	case MET_BLOCK_QUOTE:
		switch (nLevel) {
		case 1: return &_formatBlockQuote; 
		case 2: return &_formatBlockQuote2; 
		default: return &_formatBlockQuote3; 
		}
		break;
	}
	return &_formatText;
}

static void asciidoc_detectGrammar(INPUT_STREAM* pStream, int nLiteralMarkerLength, RENDER_VIEW_PART_PARAM* pParam) {
	STREAM_OFFSET nOffset = pStream->is_tell(pStream);
	pStream->is_positionToLineStart(pStream, -1);
	int i = 0;
	char szName[128] = { 0 };
	char c;
	if ((c = pStream->is_peekc(pStream, i)) == '[') {
		i++;
		while ((c = pStream->is_peekc(pStream, i)) != 0 && i < (sizeof szName - 1) && c != '\n' && c != ']') {
			szName[i-1] = c;
			i++;
		}
		if (i > 1) {
			szName[i] = 0;
			pParam->rvp_grammar = _strdup(szName);
		}
	}
	pStream->is_seek(pStream, nOffset);
	pStream->is_positionToLineStart(pStream, 1);
}

static void mdr_detectGrammar(INPUT_STREAM* pStream, int nLiteralMarkerLength, RENDER_VIEW_PART_PARAM* pParam) {
	pStream->is_skip(pStream, nLiteralMarkerLength);
	int i = 0;
	char szName[128] = { 0 };
	char c;
	while ((c = pStream->is_peekc(pStream, i)) != 0 && i < (sizeof szName - 1) && c != '\n') {
		szName[i] = c;
		i++;
	}

	if (i > 0) {
		szName[i] = 0;
		pParam->rvp_grammar = _strdup(szName);
	}
	pStream->is_positionToLineStart(pStream, 1);
}

/*
 * Parse a top level markup element (list, header, code block etc...) 
 */
static MDR_ELEMENT_TYPE mdr_determineTopLevelElement(INPUT_STREAM* pStream, RENDER_VIEW_PART_PARAM *pParam, MDR_SYNTAX* pSyntax) {
	int nLiteralMarkerLength;
	char c = pStream->is_peekc(pStream, 0);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;

	if (c == pSyntax->syn_header) {
		mType = MET_HEADER;
		pParam->rvp_level = 1;
		BOOL bSpaceFound = FALSE;
		while ((c = pStream->is_peekc(pStream, 1)) == pSyntax->syn_header || c == ' ') {
			if (c == pSyntax->syn_header) {
				if (bSpaceFound) {
					break;
				}
				pParam->rvp_level++;
			} else {
				bSpaceFound = TRUE;
			}
			pStream->is_skip(pStream, 1);
		}
	} else if ((nLiteralMarkerLength = mdr_matchFixBlock(pStream, pSyntax)) != 0) {
		pSyntax->syn_detectGrammar(pStream, nLiteralMarkerLength, pParam);
		mType = MET_FENCED_CODE_BLOCK;
	} else if (IS_UNORDERED_LIST_START(c)) {
		mType = MET_UNORDERED_LIST;
		if (c == pSyntax->syn_rulerCharacter && pStream->is_peekc(pStream, 1) == c && pStream->is_peekc(pStream, 2) == c) {
			pStream->is_positionToLineStart(pStream, 1);
			return MET_HORIZONTAL_RULE;
		}
		if (pSyntax->syn_getLevel) {
			pParam->rvp_level = pSyntax->syn_getLevel(pStream, c);
		}
		BOOL bSpaceFound = mdr_skipSpace(pStream, 1);
		if (bSpaceFound) {
			if (pStream->is_peekc(pStream, 1) == '[' && pStream->is_peekc(pStream, 3) == ']') {
				mType = MET_TASK_LIST;
				char cX = pStream->is_peekc(pStream, 2);
				pParam->rvp_number = cX == 'x' || cX == 'X' ? 1 : 0;
				pStream->is_skip(pStream, 3);
			}
		} else {
			mType = MET_NORMAL;
		}
	} else if (c == '>') {
		pParam->rvp_level = 1;
		mType = MET_BLOCK_QUOTE;
		pStream->is_skip(pStream, 1);
		while (pStream->is_peekc(pStream, 0) == '>') {
			pParam->rvp_level++;
			pStream->is_skip(pStream, 1);
		}
	} else if (isdigit((unsigned char)c)) {
		pParam->rvp_number = (c - '0');
		while ((c = pStream->is_peekc(pStream, 1)) != 0) {
			if (isdigit((unsigned char)c)) {
				pParam->rvp_number = 10 * pParam->rvp_number + (c - '0');
			} else if (c == '.') {
				pStream->is_skip(pStream, 2);
				mType = MET_ORDERED_LIST;
				break;
			}
			pStream->is_skip(pStream, 1);
		}
		pParam->rvp_level = mdr_getLevelFromIndent(pStream, c);
	}
	return mType;
}

static TEXT_RUN* mdr_appendRun(TEXT_RUN** pRuns, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, FONT_STYLE_DELTA *pFSD, LINE* lp, int nLineOffset, char** ppszLink) {
	int mAttrs = pFSD->fsd_logicalStyles;
	if (nSize == 0 && *pRuns && (mAttrs & (ATTR_LINE_BREAK|ATTR_EMOJI)) == 0) {
		return NULL;
	}
	if (!pFormat) {
		pFormat = &_formatText;
	}
	TEXT_RUN* pRun = *pRuns == 0 || (*pRuns)->tr_size || (*pRuns)->tr_isEmoji ? ll_append(pRuns, sizeof(TEXT_RUN)) : *pRuns;
	pRun->tr_size = nSize;
	pRun->tr_lp = lp;
	pRun->tr_lineOffset = nLineOffset;
	pRun->tr_attributes.size = pFormat->mef_fontSize;
	pRun->tr_attributes.weight = pFormat->mef_charWeight;
	THEME_DATA* pTheme = theme_getCurrent();
	if (mAttrs & ATTR_HIGHLIGHT) {
		pRun->tr_attributes.style = "highlight";
	} else if (mAttrs & (ATTR_CODE|ATTR_TAG_CODE)) {
		pRun->tr_attributes.style = "code";
	} else {
		pRun->tr_attributes.style = pFormat->mef_style;
		pRun->tr_attributes.bgColor = NO_COLOR;
	}
	if (mAttrs & ATTR_SUB) {
		pRun->tr_attributes.subscript = 1;
	} else if (mAttrs & ATTR_SUPER) {
		pRun->tr_attributes.superscript = 1;
	}
	if (mAttrs & (ATTR_LINK|ATTR_UNDERLINE)) {
		pRun->tr_attributes.underline = 1;
	}
	if (pFSD->fsd_styleName) {
		pRun->tr_attributes.fgColor = theme_textStyleForeground(pTheme, pFSD->fsd_styleName, NO_COLOR);
		free((char*)pFSD->fsd_styleName);
		pFSD->fsd_styleName = 0;
	} else {
		if (mAttrs & ATTR_LINK) {
			pRun->tr_attributes.fgColor = theme_textStyleForeground(pTheme, "hyperlink", RGB(120, 120, 255));
		}
		else {
			pRun->tr_attributes.fgColor = pFSD->fsd_textColor;
		}
	}
	pRun->tr_attributes.indent = pFSD->fsd_indent;
	if (mAttrs & ATTR_EMPHASIS) {
		pRun->tr_attributes.italic = 1;
	}
	if (mAttrs & ATTR_STRONG) {
		pRun->tr_attributes.weight = FW_BOLD;
	}
	if (mAttrs & ATTR_STRIKE) {
		pRun->tr_attributes.strikeout = 1;
	}
	if (mAttrs & ATTR_LINE_BREAK) {
		pRun->tr_attributes.lineBreak = 1;
	}
	if (pFSD->fsd_fontSize.ss_units != CSU_NONE) {
		pRun->tr_attributes.size = pFSD->fsd_fontSize;
	}
	pFSD->fsd_logicalStyles &= ~(ATTR_LINE_BREAK | ATTR_LINK);
	if (ppszLink && *ppszLink) {
		pRun->tr_link = *ppszLink;
		*ppszLink = 0;
	}
	return pRun;
}

/*
 * Process a link by optionally prepending a Base URL (if it is a relative link). If 'bAuto' is true,
 * automatically extend the protocol portion of the link.
 * 
 * Returns an allocated result to be freed later.
 */
static char* mdr_processUrlWithBase(const char* pszBaseURL, char* pszLink, BOOL bAuto) {
	char szFullURL[EDMAXPATHLEN];
	if (bAuto && strchr(pszLink, '/') == 0) {
		sprintf(szFullURL, "https://%s", pszLink);
		return _strdup(szFullURL);
	}
	if (pszBaseURL && pszLink[0] != '#' && strstr(pszLink, "//") == 0) {
		// Interpret as local file
		char szLink[EDMAXPATHLEN];
		DWORD len = sizeof szLink;
		UrlUnescape(pszLink, szLink, &len, 0);
		if (file_exists(szLink) == 0) {
			return _strdup(szLink);
		}
		string_splitFilename(pszBaseURL, szFullURL, NULL, 0);
		string_concatPathAndFilename(szFullURL, szFullURL, szLink);
		return _strdup(szFullURL);
	}
	if (strstr(pszLink, "github") != 0 && strstr(pszLink, "raw") == 0) {
		strncpy(szFullURL, pszLink, sizeof szFullURL);
		strncat(szFullURL, "?raw=true", sizeof szFullURL);
		pszLink = szFullURL;
	}
	return _strdup(pszLink);
}

/*
 * Parse the link URL - allow for things as included title specs. 
 */
static BOOL mdr_parseLinkUrl(const char* pszBaseURL, char* pszBuf, char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
	BOOL bAuto = FALSE;
	int nStart = 0;
	if (pszBuf[nStart] == '<') {
		bAuto = TRUE;
		nStart++;
	}
	size_t nLen = strlen(pszBuf+nStart);
	if (nLen == 0) {
		return FALSE;
	}
	if (bAuto) {
		if (pszBuf[nLen - 1] == '>') {
			nLen--;
		} else {
			bAuto = FALSE;
		}
	}
	if (nLen == 0) {
		return FALSE;
	}
	char szTitle[512];
	szTitle[0] = 0;
	char* pszTitleStart = strchr(pszBuf, '"');
	if (pszTitleStart) {
		char* pszTitleEnd = strchr(pszTitleStart, '"');
		if (pszTitleEnd) {
			*pszTitleEnd = 0;
			strcpy(szTitle, pszTitleStart + 1);
			while (pszTitleStart > pszBuf && pszTitleStart[-1] == ' ') {
				pszTitleStart--;
			}
			*pszTitleStart = 0;
		}
	}
	if (nWidth && nHeight) {
		pszTitleStart = strchr(pszBuf, '=');
		if (pszTitleStart) {
			if (pszTitleStart > pszBuf && pszTitleStart[-1] == ' ') {
				pszTitleStart[-1] = 0;
			}
			pszTitleStart++;
			char* pszHeight = strchr(pszTitleStart, 'x');
			if (pszHeight) {
				*pszHeight = 0;
				if (pszHeight > pszTitleStart) {
					*nWidth = atol(pszTitleStart);
				}
				*nHeight = atol(pszHeight + 1);
			}
		}
	}
	*pszLink = mdr_processUrlWithBase(pszBaseURL, pszBuf, bAuto);
	if (szTitle[0]) {
		*pszTitle = _strdup(szTitle);
	}
	else {
		*pszTitle = NULL;
	}
	return TRUE;
}

typedef struct tagHTML_PARSER_STATE {
	STRING_BUF* hps_text;								// the current text contents of a block level node.
	int					hps_lastTextOffset;				// end in hps_text of the last text run
	int					hps_blockLevel;					// simple book keeping of open block levels 
	const char* hps_baseUrl;							// The base URL from where the document is loaded.
	MDR_ELEMENT_TYPE	hps_listType;
	int					hps_orderedListListLevel;
	int					hps_orderedListIndices[10];		// Auto numbering of ordered lists
	int					hps_unorderedListLevel;
	LINE* hps_lp;
	int					hps_runOffset;
	HASHMAP* hps_lastAttributes;
	MDR_ELEMENT_FORMAT  hps_blockFormat;
	FONT_STYLE_DELTA	hps_styleTable[20];
	FONT_STYLE_DELTA*	hps_currentStyle;
	MD_REFERENCE_DEFINITION** hps_referenceDefinitions;
	HASHMAP*			hps_imageCache;
	RENDER_VIEW_PART**	hps_head;
	RENDER_VIEW_PART*	hps_part;
	RENDER_TABLE*		hps_table;
	RENDER_TABLE_ROW*	hps_tableRow;
	RENDER_TABLE_CELL*	hps_tableCell;
	MDR_SYNTAX*			hps_syntax;
} HTML_PARSER_STATE;

static void mdr_ensureParagraph(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState);

static BOOL asciidoc_parseLink(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText, LINK_PARSE_RESULT* pResult, LINK_PARSE_STATE startState) {	
	char* szLinkEnd = szLinkText + 255;
	char c = 0;
	int state = LPS_PARSE_URL;
	memset(pResult, 0, sizeof(*pResult));
	pStream->is_skip(pStream,   -1);
	char szUrl[256];
	char* pszUrl = szUrl;
	char* pszUrlEnd = szUrl + sizeof szUrl - 1;
	*pszUrl = 0;
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (state == LPS_PARSE_URL) {
			if (c == ' ' || c == '[') {
				if (c == '[') {
					state = LPS_TITLE;
				}
				else {
					break;
				}
			} else if (pszUrl < pszUrlEnd) {
				*pszUrl++ = c;
				*pszUrl = 0;
			}
		} else if (state == LPS_TITLE) {
			if (c == ']' || c == '\n' || szLinkText >= szLinkEnd) {
				break;
			}
			else {
				*szLinkText++ = c;
				*szLinkText = 0;
			}
		} else {
			break;
		}
	}
	if (strncmp(szUrl, "xref:", 5) == 0) {
		// some dumb handling of cross references...
		char szTotal[512];
		pszUrl = szUrl + 5;
		char* pszModule = strchr(pszUrl, ':');
		if (pszModule) {
			strcpy(szTotal, "../");
			*pszModule++ = 0;
			strcat(szTotal, pszUrl);
			strcat(szTotal, "/pages/");
			strcat(szTotal, pszModule);
		} else {
			strcpy(szTotal, "pages/");
			strcat(szTotal, pszUrl);
		}
		pResult->lpr_LinkUrl = _strdup(szTotal);
	} else if (strncmp(szUrl, "link:", 5) == 0) {
		pResult->lpr_LinkUrl = _strdup(szUrl + 5);
	} else {
		pResult->lpr_LinkUrl = _strdup(szUrl);
	}
	return TRUE;
}

static BOOL mdr_parseLink(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText, LINK_PARSE_RESULT* pResult, LINK_PARSE_STATE startState) {
	char szBuf[256];
	int nUrlPartStart = 0;
	char c = 0;
	int state = startState;
	const char* szLinkEnd = szLinkText + 255;
	memset(pResult, 0, sizeof(*pResult));
	int bRefStyleLink = 0;
	STREAM_OFFSET savedOffset = pStream->is_tell(pStream);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (state == LPS_TITLE) {
			if (c == ']') {
				*szLinkText = 0;
				while (pStream->is_peekc(pStream, 0) == ' ') {
					pStream->is_skip(pStream, 1);
				}
				state = LPS_WAIT_URL;
			}
			else if (szLinkText < szLinkEnd) {
				*szLinkText++ = c;
			}
		}
		else if (state == LPS_WAIT_URL) {
			if (c == '(') {
				// support special syntax: [text](<link>)
				if (pStream->is_peekc(pStream, 0) == '<') {
					pStream->is_skip(pStream, 1);
				}
			}
			else if (c == '[') {
				bRefStyleLink = 1;
				// Reference style link or footnote.
			}
			else if (c == ':') {
				pStream->is_skip(pStream, 1);
				while (pStream->is_peekc(pStream, 0) == ' ') {
					pStream->is_skip(pStream, 1);
				}
				pResult->lpr_isReferenceLinkDefinition = TRUE;
			}
			else {
				pStream->is_seek(pStream, savedOffset);
				return FALSE;
			}
			state = LPS_PARSE_URL;
		}
		else {
			if (bRefStyleLink && c == ']') {
				szBuf[nUrlPartStart] = 0;
				pResult->lpr_LinkRefUrl = _strdup(szBuf);
				return TRUE;
			}
			else if (!bRefStyleLink && !pResult->lpr_isReferenceLinkDefinition && c == ')') {
				szBuf[nUrlPartStart] = 0;
				return mdr_parseLinkUrl(pState->hps_baseUrl, szBuf, &pResult->lpr_LinkUrl, &pResult->lpr_title, &pResult->lpr_width, &pResult->lpr_height);
			}
			if (nUrlPartStart < sizeof szBuf - 1) {
				szBuf[nUrlPartStart++] = c;
			}
		}
	}
	if ((c == '\n' || c == 0) && pResult->lpr_isReferenceLinkDefinition) {
		szBuf[nUrlPartStart] = 0;
		char* pszTitle = strpbrk(szBuf, " (\"'");
		if (pszTitle != 0) {
			*pszTitle = 0;
		}
		return mdr_parseLinkUrl(pState->hps_baseUrl, szBuf, &pResult->lpr_LinkUrl, &pResult->lpr_title, &pResult->lpr_width, &pResult->lpr_height);
	}
	pStream->is_seek(pStream, savedOffset);
	return FALSE;
}

static TEXT_RUN** mdr_getBlockRunsOf(RENDER_VIEW_PART* pPart) {
	return &pPart->rvp_data.rvp_flow.tf_runs;
}

/*
 * Parse a fenced code block and add the runs to the view part.
 */
static void mdr_parsePreformattedCodeBlock(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bIndented, const char* pEndTag) {
	size_t nLastOffset = 0;
	char c;
	LINE* lp = NULL;
	int bSkipEmptyFirst = 0;
	char szTag[32];

	if (pEndTag) {
		sprintf(szTag, "</%s>", pEndTag);
		bSkipEmptyFirst = 1;
	}
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (!pPart) {
		mdr_ensureParagraph(pStream, pState);
		pPart = pState->hps_part;
	}
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		int nOffs = 0;
		if (bIndented) {
			if (c == '\t') {
				nOffs = 1;
			} else {
				if (c != ' ' || pStream->is_strncmp(pStream, "    ", 4) != 0) {
					return;
				}
				nOffs = 4;
			}
		}
		else if (pEndTag == 0 && mdr_matchFixBlock(pStream, pState->hps_syntax)) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		} else if (pEndTag && c == '<' && pStream->is_strncmp(pStream, szTag, strlen(szTag)) == 0) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		}
		if (pPart) {
			pPart->rvp_param.rvp_number++;
		}
		int nRunStart = pStream->is_inputMark(pStream, &lp);
		pStream->is_skip(pStream, nOffs);
		while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
			if (c == '\r') {
				continue;
			}
			if (c == '\t') {
				size_t nCurrent = stringbuf_size(pState->hps_text) - nLastOffset;
				size_t nTab = (nCurrent + 4) / 4 * 4;
				while (nCurrent < nTab) {
					stringbuf_appendChar(pState->hps_text, ' ');
					nCurrent++;
				}
			} else {
				if (pEndTag && c == '<' && pStream->is_strncmp(pStream, szTag+1, strlen(szTag)-1) == 0) {
					pStream->is_positionToLineStart(pStream, 1);
					return;
				}
				stringbuf_appendChar(pState->hps_text, c);
			}

		}
		size_t nSize = stringbuf_size(pState->hps_text) - nLastOffset;
		if (nSize == 0) {
			if (bSkipEmptyFirst) {
				bSkipEmptyFirst = 0;
				continue;
			}
			// do not skip empty lines.
			stringbuf_appendChar(pState->hps_text, ' ');
			nSize++;
		}
		mdr_appendRun(mdr_getBlockRunsOf(pPart), &_formatFenced, nSize,
			&(FONT_STYLE_DELTA){ATTR_LINE_BREAK, .fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nRunStart, 0);
		nLastOffset += nSize;
	}
}

/*
 * Parses the auto-link syntax.
 */
static BOOL mdr_parseAutolinks(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, TEXT_FLOW* pFlow, MDR_ELEMENT_FORMAT* pFormat, FONT_STYLE_DELTA* pFSD,
		size_t nLastOffset) {
	char szTemp[512];
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	BOOL bDot = FALSE;
	char c;
	LINE* lp = NULL;
	int nUrlLen = 0;
	pStream->is_skip(pStream, 1);
	int nLineOffset = pStream->is_inputMark(pStream, &lp);
	while ((c = pStream->is_getc(pStream)) != '>' && c != 0 && c != '\n') {
		if (c == '.') {
			bDot = TRUE;
		}
		if (nUrlLen < sizeof szTemp - 1) {
			szTemp[nUrlLen++] = c;
		}
	}
	szTemp[nUrlLen] = 0;
	if (bDot && pFlow) {
		size_t nSize = stringbuf_size(pState->hps_text) - nLastOffset;
		mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pFSD, lp, nLineOffset, 0);
		char* pszLink;
		char* pszTitle;
		stringbuf_appendString(pState->hps_text, szTemp);
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nUrlLen, &(FONT_STYLE_DELTA){ATTR_LINK, 
			.fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nLineOffset, 0);
		if (pRun == 0) {
			return FALSE;
		}
		int nWidth = 0;
		int nHeight = 0;
		if (!mdr_parseLinkUrl(0, szTemp, &pszLink, &pszTitle, &nWidth, &nHeight)) {
			return FALSE;
		}
		pRun->tr_title = pszTitle;
		if (pRun->tr_data.tr_image) {
			mdr_assignImageUrl(pState->hps_imageCache, pRun, pszLink);
			pRun->tr_data.tr_image->mdi_height = nHeight;
			pRun->tr_data.tr_image->mdi_width = nWidth;
		} else {
			pRun->tr_link = pszLink;
		}
		return TRUE;
	}
	pStream->is_seek(pStream, offset);
	return FALSE;
}

static void mdr_applyFormat(RENDER_VIEW_PART* pPart, MDR_ELEMENT_FORMAT* pFormat) {
	pPart->rvp_margins = pFormat->mef_margins;
	if (pFormat->mef_decoration) {
		if (!pPart->rvp_decoration) {
			pPart->rvp_decoration = calloc(1, sizeof * (pPart->rvp_decoration));
		}
		if (pPart->rvp_decoration) {
			memcpy(pPart->rvp_decoration, pFormat->mef_decoration, sizeof * (pPart->rvp_decoration));
		}
	}
}

static BOOL mdr_isAtWordBorder(INPUT_STREAM* pStream) {
	char c = pStream->is_peekc(pStream, -1);
	if (c == '\n') {
		return TRUE;
	}
	if (!pks_isalnum(c) && c != '\\') {
		return TRUE;
	}
	c = pStream->is_peekc(pStream, 1);
	if (pks_isalnum(c)) {
		return FALSE;
	}
	return TRUE;
}

static BOOL mdr_isIndentedFencedBlock(INPUT_STREAM* pStream, int mType) {
	char cColumn = 0;
	if (mType == MET_ORDERED_LIST || mType == MET_UNORDERED_LIST) {
		return FALSE;
	}
	char c;
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (c == '\t') {
			if (cColumn > 0 && cColumn < 4) {
				// we do not allow the line to start with a number of spaces < 4 and then tab
				// this has to match the algorithm in mdr_parsePreformattedCodeBlock.
				break;
			}
			cColumn = 4;
			continue;
		}
		if (c != ' ') {
			pStream->is_seek(pStream, offset);
			if (IS_UNORDERED_LIST_START(c)) {
				return FALSE;
			}
			return cColumn >= 4;
		}
		cColumn++;
	}
	pStream->is_seek(pStream, offset);
	return FALSE;
}

typedef enum {HTT_INLINE, HTT_BLOCK, HTT_SPECIAL} HTML_TAG_TYPE;

typedef struct tagHTML_TAG_MAPPING HTML_TAG_MAPPING;

static struct tagHTML_TAG_MAPPING {
	const char* tm_tagName;
	HTML_TAG_TYPE tm_tagType;
	MDR_ELEMENT_TYPE tm_blockElement;
	int tm_elementLevel;				// For H1, H2,... and corresponding tag types - the level - otherwise 0.
	int tm_textAttr;
	int tm_textAttrClose;
	int tm_indent;
	int tm_noCloseTag;
	int tm_returnValues;
} _tagMappings[] = {
	{"em", HTT_INLINE, MET_NONE, 0, ATTR_EMPHASIS},
	{"sup", HTT_INLINE, MET_NONE, 0, ATTR_SUPER},
	{"sub", HTT_INLINE, MET_NONE, 0, ATTR_SUB},
	{"a", HTT_INLINE, MET_NONE, 0, ATTR_LINK, .tm_returnValues = 1},
	{"img", HTT_INLINE, MET_IMAGE, .tm_returnValues = 1, .tm_noCloseTag = 1},
	{"b", HTT_INLINE, MET_NONE, 0, ATTR_STRONG},
	{"dt", HTT_INLINE, MET_NONE, 0, 0, ATTR_LINE_BREAK, DEFAULT_LEFT_MARGIN*2},
	{"dd", HTT_INLINE, MET_NONE, 0, ATTR_STRONG, ATTR_LINE_BREAK},
	{"dl", HTT_INLINE, MET_NONE, 0, ATTR_LINE_BREAK},
	{"i", HTT_INLINE, MET_NONE, 0, ATTR_EMPHASIS},
	{"u", HTT_INLINE, MET_NONE, 0, ATTR_UNDERLINE},
	{"del", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"strike", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"s", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"code", HTT_INLINE, MET_NONE, 0, ATTR_TAG_CODE},
	{"span", HTT_INLINE, MET_NONE, 0, 0},
	{"br", HTT_INLINE, MET_NONE, 0, ATTR_LINE_BREAK, .tm_noCloseTag = 1},
	{"p", HTT_BLOCK, MET_PARAGRAPH, 0, ATTR_LINE_BREAK},
	{"script", HTT_BLOCK, MET_FENCED_CODE_BLOCK, 0},
	{"pre", HTT_BLOCK, MET_FENCED_CODE_BLOCK, 0},
	{"table", HTT_BLOCK, MET_TABLE, 0, .tm_returnValues = 1},
	{"hr", HTT_BLOCK, MET_HORIZONTAL_RULE, 0, .tm_noCloseTag = 1},
	{"tr", HTT_SPECIAL, MET_NONE, 0},
	{"td", HTT_SPECIAL, MET_NONE, 0},
	{"th", HTT_SPECIAL, MET_NONE, 0},
	{"h1", HTT_BLOCK, MET_HEADER, 1},
	{"h2", HTT_BLOCK, MET_HEADER, 2},
	{"h3", HTT_BLOCK, MET_HEADER, 3},
	{"h4", HTT_BLOCK, MET_HEADER, 4},
	{"h5", HTT_BLOCK, MET_HEADER, 5},
	{"h6", HTT_BLOCK, MET_HEADER, 6},
	{"ul", HTT_SPECIAL, MET_NONE, 1},
	{"li", HTT_BLOCK, MET_UNORDERED_LIST, 1},
	{"ol", HTT_SPECIAL, MET_NONE, 1},
	{"tt", HTT_INLINE, MET_NONE, 0, ATTR_TAG_CODE}
};

typedef struct tagHTML_TAG {
	HTML_TAG_MAPPING* ht_descriptor;
	BOOL ht_isClose;
	BOOL ht_isOpen;
	HASHMAP* ht_attributes;
} HTML_TAG;

/*
 * Parse a CSS style border specification.
 */
static void mdr_parseBorder(FONT_STYLE_DELTA* pFSD, char* pszBorder) {
	char* pszNext = strtok(pszBorder, " ");
	while (pszNext) {
		if (isdigit((unsigned char)pszNext[0])) {
			// TODO: support size specifications
			pFSD->fsd_strokeWidth = (int)string_convertToLong(pszNext);
		} else if (strcmp("solid", pszNext) != 0) {
			pFSD->fsd_strokeColor = json_convertColor(pszNext);
		}
		pszNext = strtok(0, " ");
	}
}

/*
 * Parse / decode a CSS text-align specification.
 */
static TEXT_ALIGN mdr_parseTextAlignment(const char* pszSpec) {
	if (strcmp("center", pszSpec) == 0) {
		return TA_ALIGN_CENTER;
	}
	if (strcmp("right", pszSpec) == 0) {
		return TA_ALIGN_RIGHT;
	}
	if (strcmp("left", pszSpec) == 0) {
		return TA_ALIGN_LEFT;
	}
	return TA_ALIGN_DEFAULT;
}

/*
 * Parse a CSS size specification.
 */
static SIZE_SPECIFICATION mdr_parseSize(const char* pszText) {
	float fValue = (float)string_convertToLong(pszText);
	CSS_SIZE_UNIT units = CSU_NONE;
	if (_strtolend) {
		if (*_strtolend == '%') {
			fValue /= 100.0f;
			units = CSU_PERCENT;
		}
		else if (_strtolend[0] == 'e' && _strtolend[1] == 'm') {
			units = CSU_EM;
		}
		else if (_strtolend[0] == 'p' && _strtolend[1] == 't') {
			units = CSU_PT;
		}
		else if (_strtolend[0] == 0 || (_strtolend[0] == 'p' && _strtolend[1] == 'x')) {
			units = CSU_PX;
		}
	}
	return (SIZE_SPECIFICATION) { .ss_units = units, .ss_value = fValue };
}

/*
 * Parse a CSS style specification. Currently only color: #xxx / color: red or similar specifications
 * are supported.
 */
static void mdr_parseStyle(FONT_STYLE_DELTA* pFSD, const char* pszStyleSpec) {
	int tState = 0;
	// (A) pszStyleSpec is currently never longer than MAX_HTML_VALUE_LEN
	char szAttribute[MAX_HTML_VALUE_LEN];
	char szAttrValue[MAX_HTML_VALUE_LEN];
	char* pszAttr;
	char* pszValue;
	char c;

	szAttribute[0] = 0;
	while (1) {
		c = *pszStyleSpec++;
		if (tState == 0) {
			if (isalpha((unsigned char)c)) {
				pszAttr = szAttribute;
				tState = 1;
			}
			else {
				if (!c) {
					break;
				}
				continue;
			}
		}
		if (tState == 1) {
			if (!c) {
				break;
			}
			if (c == ':') {
				*pszAttr = 0;
				tState = 2;
				pszValue = szAttrValue;
				continue;
			}
			*pszAttr++ = c;
			continue;
		}
		if (tState == 2) {
			if (c == ';' || c == 0) {
				*pszValue = 0;
				tState = 0;
				if (strcmp("color", szAttribute) == 0) {
					pFSD->fsd_textColor = json_convertColor(szAttrValue);
				} else if (strcmp("background", szAttribute) == 0) {
					pFSD->fsd_fillColor = json_convertColor(szAttrValue);
				} else if (strcmp("width", szAttribute) == 0) {
					pFSD->fsd_width = mdr_parseSize(szAttrValue);
				} else if (strcmp("font-size", szAttribute) == 0) {
					pFSD->fsd_fontSize = mdr_parseSize(szAttrValue);
				} else if (strcmp("border", szAttribute) == 0) {
					mdr_parseBorder(pFSD, szAttrValue);
				} else if (strcmp("text-align", szAttribute) == 0) {
					pFSD->fsd_textAlign = mdr_parseTextAlignment(szAttrValue);
				} else if (strcmp("text-decoration", szAttribute) == 0) {
					if (strcmp("underline", szAttrValue)) {
						pFSD->fsd_logicalStyles |= ATTR_UNDERLINE;
					}
					else if (strcmp("line-through", szAttrValue)) {
						pFSD->fsd_logicalStyles |= ATTR_STRIKE;
					}
				}
				if (!c) {
					break;
				}
			} else if (pszValue != szAttrValue || c != ' ') {
				*pszValue++ = c;
			}
		}
	}
}

/*
 * Parse an HTML tag: <b> or </em> or the like.
 */
typedef enum {
	HPS_INIT = 0,
	HPS_ELEM = 1,
	HPS_BETWEEN_ATTR = 2,
	HPS_ATTR = 3,
	HPS_OPTIONAL_QUOTED_VALUE = 4,
	HPS_WAIT_FOR_VALUE = 5,
	HPS_VALUE = 6
} HTML_PARSE_STATE;
static int mdr_getTag(INPUT_STREAM* pStream, FONT_STYLE_DELTA* pFSD, HTML_TAG* pTag) {
	char szTag[10];
	int bTermQuote = 0;
	char lastC = 0;
	char szAttribute[32];
	static char szValue[MAX_HTML_VALUE_LEN];
	char* pszAttr = 0;
	char* pszValue = 0;
	char* pszDest = szTag;
	size_t nDestSize = sizeof szTag;
	HTML_TAG_MAPPING* pResult = 0;

	HTML_PARSE_STATE nState = HPS_INIT;
	pTag->ht_isClose = 0;
	pTag->ht_isOpen = 0;
	pTag->ht_attributes = 0;
	pTag->ht_descriptor = 0;
	if (pStream->is_peekc(pStream, 0) == '/') {
		pTag->ht_isClose = 1;
		pStream->is_skip(pStream, 1);
	}
	else {
		pTag->ht_isOpen = 1;
	}
	pFSD->fsd_styleName = 0;
	while (TRUE) {
		char c = pStream->is_getc(pStream);
		if (c == 0) {
			break;
		}
		switch (nState) {
		case HPS_INIT:
			if (isalnum((unsigned char)c)) {
				nState = HPS_ELEM;
				*pszDest++ = tolower(c);
			}
			break;
		case HPS_ELEM:
			if (isalnum((unsigned char)c)) {
				if (pszDest < szTag + nDestSize - 1) {
					*pszDest++ = tolower(c);
				}
			}
			else {
				*pszDest = 0;
				for (int nElement = 0; nElement < DIM(_tagMappings); nElement++) {
					if (strcmp(_tagMappings[nElement].tm_tagName, szTag) == 0) {
						pResult = &_tagMappings[nElement];
						break;
					}
				}
				nState = HPS_BETWEEN_ATTR;
			}
			break;
		case HPS_OPTIONAL_QUOTED_VALUE:
			if (c == '"') {
				nState = HPS_VALUE;
				bTermQuote = 1;
			}
			else if (c != ' ') {
				nState = HPS_VALUE;
				*pszValue++ = c;
				bTermQuote = 0;
			}
			break;
		case HPS_WAIT_FOR_VALUE:
		case HPS_BETWEEN_ATTR:
			if (isalnum((unsigned char)c)) {
				pszAttr = szAttribute;
				*pszAttr++ = tolower(c);
				nState = HPS_ATTR;
			}
			break;
		case HPS_ATTR:
			if (isalnum((unsigned char)c)) {
				if (pszAttr < (szAttribute + sizeof szAttribute - 1)) {
					*pszAttr++ = tolower(c);
				}
			} else if (c == '=') {
				*pszAttr = 0;
				bTermQuote = 0;
				nState = HPS_OPTIONAL_QUOTED_VALUE;
				pszValue = szValue;
			} else {
				*pszAttr = 0;
				nState = HPS_BETWEEN_ATTR;
			}
			break;
		case HPS_VALUE:
			if ((bTermQuote && c != '"') || (!bTermQuote && c != ' ' && c != '/' && c != '>')) {
				if (pszValue < (szValue + sizeof szValue - 1)) {
					*pszValue++ = c;
				}
			} else {
				*pszValue = 0;
				if (strcmp("class", szAttribute) == 0) {
					pFSD->fsd_styleName = _strdup(szValue);
				} else if (strcmp("style", szAttribute) == 0) {
					mdr_parseStyle(pFSD, szValue);
				} else if (pResult && pResult->tm_returnValues) {
					if (!pTag->ht_attributes) {
						pTag->ht_attributes = hashmap_create(13, 0, 0);
					}
					hashmap_put(pTag->ht_attributes, _strdup(szAttribute), (intptr_t)_strdup(szValue));
				} 
				nState = HPS_BETWEEN_ATTR;
			}
		}
		if (c == '>') {
			if (pResult) {
				pTag->ht_descriptor = pResult;
				pFSD->fsd_indent = pResult->tm_indent;
				if (lastC == '/' && !pResult->tm_noCloseTag) {
					pTag->ht_isClose = 1;
				}
				return 1;
			}
			return 0;
		} else if (c != ' ') {
			lastC = c;
		}
	}
	return 0;
}

/*
 * Parse an HTML entity spec and convert into a character if successful. 
 */
static int mdr_parseEntity(STRING_BUF* pSB, INPUT_STREAM* pStream) {
	char szBuf[32];
	char* pszDest;
	if (pStream->is_peekc(pStream, 0) == '#' && pStream->is_peekc(pStream, 1) == 'x') {
		pStream->is_skip(pStream, 2);
		pszDest = szBuf;
		char c;
		while ((c = pStream->is_getc(pStream)) != 0 && c != ';' && pszDest < szBuf + sizeof szBuf - 1) {
			*pszDest++ = c;
		}
		*pszDest = 0;
		stringbuf_appendChar(pSB, (char)string_convertToLongBase16(szBuf));
		return 1;
	}
	for (int nE = 0; nE < DIM(_entities); nE++) {
		size_t nLen = strlen(_entities[nE].entity);
		if (pStream->is_strncmp(pStream, _entities[nE].entity, nLen) == 0) {
			stringbuf_appendChar(pSB, _entities[nE].c);
			pStream->is_skip(pStream, (int)nLen);
			return 1;
		}
	}
	return 0;
}

/*
 * Create a new RENDER_TABLE with default values.
 */
static RENDER_TABLE* mdr_newTable(int nColumnCount) {
	RENDER_TABLE* pResult = calloc(1, sizeof * pResult);
	if (pResult == NULL) {
		return NULL;
	}
	pResult->rt_borderWidth = 1;
	pResult->rt_borderColor = NO_COLOR;
	pResult->rt_columnCount = nColumnCount;
	return pResult;
}
/*
 * Destroy a table cell.
 */
static int mdr_destroyTableCell(RENDER_TABLE_CELL* pCell) {
	free(pCell->rtc_flow.tf_text);
	ll_destroy(&pCell->rtc_flow.tf_runs, mdr_destroyRun);
	return 1;
}

/*
 * Destroy a table row.
 */
static int mdr_destroyTableRow(RENDER_TABLE_ROW* pRow) {
	ll_destroy(&pRow->rtr_cells, mdr_destroyTableCell);
	return 1;
}

static void mdr_destroyTable(RENDER_VIEW_PART* pRVP) {
	ll_destroy(&pRVP->rvp_data.rvp_table->rt_rows, mdr_destroyTableRow);
	free(pRVP->rvp_data.rvp_table);
	pRVP->rvp_data.rvp_table = 0;
}

static void mdr_resetFontStyleDelta(FONT_STYLE_DELTA* pFSD) {
	pFSD->fsd_logicalStyles = 0;
	pFSD->fsd_styleName = 0;
	pFSD->fsd_indent = 0;
	pFSD->fsd_textAlign = TA_ALIGN_DEFAULT;
	pFSD->fsd_textColor = NO_COLOR;
	pFSD->fsd_fillColor = NO_COLOR;
	pFSD->fsd_strokeColor = NO_COLOR;
}

static RENDER_VIEW_PART* mdr_newPart(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, MDR_ELEMENT_TYPE mType, int nLevel) {
	RENDER_VIEW_PART* pExisting = pState->hps_part;
	RENDER_VIEW_PART* pPart = (pExisting && pExisting->rvp_data.rvp_flow.tf_text == 0) ? pExisting : ll_append(pState->hps_head, sizeof * pPart);
	if (pPart->rvp_data.rvp_table) {
		mdr_destroyTable(pPart);
	}
	if (pPart->rvp_decoration) {
		free(pPart->rvp_decoration);
		pPart->rvp_decoration = 0;
	}
	switch (mType) {
	case MET_HORIZONTAL_RULE: pPart->rvp_paint = mdr_renderHorizontalRule; break;
	case MET_TABLE: pPart->rvp_paint = mdr_renderTable; break;
	default:
		pPart->rvp_paint = mdr_renderMarkdownBlockPart;
		pPart->rvp_data.rvp_flow.tf_align = pState->hps_currentStyle->fsd_textAlign;
		break;
	}
	pPart->rvp_type = mType;
	pState->hps_blockFormat = *mdr_getFormatFor(mType, nLevel);
	pPart->rvp_param.rvp_level = nLevel;
	mdr_applyFormat(pPart, &pState->hps_blockFormat);
	FONT_STYLE_DELTA* pFSD = pState->hps_currentStyle;
	if (pFSD->fsd_strokeColor != NO_COLOR || pFSD->fsd_fillColor != NO_COLOR) {
		RENDER_BOX_DECORATION* pDecoration = calloc(1, sizeof * pDecoration);
		if (pDecoration != NULL) {
			pDecoration->rbd_fillColor = pFSD->fsd_fillColor;
			pDecoration->rbd_strokeColor = pFSD->fsd_strokeColor;
			pDecoration->rbd_strokeWidth = pFSD->fsd_strokeWidth;
		}
		pPart->rvp_decoration = pDecoration;
	}
	mdr_resetFontStyleDelta(pFSD);
	if (mType == MET_TABLE) {
		pPart->rvp_data.rvp_table = mdr_newTable(0);
	}
	pStream->is_inputMark(pStream, &pPart->rvp_lpStart);
	pState->hps_blockLevel = 1;
	return pPart;
}

/*
 * Skip all leading space chars in an input string.
 */
static void mdr_skipLeadingSpace(INPUT_STREAM* pStream) {
	while (isspace((unsigned char) pStream->is_peekc(pStream, 0))) {
		pStream->is_skip(pStream, 1);
	}
}

static BOOL mdr_skipHTMLCommentOrDoctype(INPUT_STREAM* pStream) {
	if (pStream->is_peekc(pStream, 0) == '!') {
		pStream->is_skip(pStream, 3);
		char c;
		while ((c = pStream->is_peekc(pStream, 0)) != 0) {
			if (c == '>') {
				pStream->is_skip(pStream, 1);
				break;
			}
			pStream->is_skip(pStream, 1);
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Ensures, there is a paragraph, when HTML contents is added.
 */
static void mdr_ensureParagraph(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	if (!pState->hps_part) {
		FONT_STYLE_DELTA fsd = *pState->hps_currentStyle;
		pState->hps_part = mdr_newPart(pStream, pState, MET_NORMAL, 0);
		*pState->hps_currentStyle = fsd;
	}
}

static void mdr_finishTableSetup(RENDER_TABLE* pTable) {
	if (!pTable) {
		return;
	}
	RENDER_TABLE_ROW* pRow = pTable->rt_rows;
	int nColumn = 0;
	// stupid simple layout mechanism assuming a ave char width of 8 to calculate
	// required column widths.
	while (pRow) {
		nColumn = 0;
		RENDER_TABLE_CELL* pCell = pRow->rtr_cells;
		while (pCell) {
			if (pCell->rtc_flow.tf_text) {
				int nWidth = 0;
				TEXT_RUN* pRun = pCell->rtc_flow.tf_runs;
				if (pRun && pRun->tr_data.tr_image && !pRun->tr_isEmoji) {
					int nHeight;
					int nOrigWidth;
					int nOrigHeight;
					mdr_loadAndMeasureImage(NULL, pRun, &nWidth, &nHeight, &nOrigWidth, &nOrigHeight);
				} else {
					size_t nLen = strlen(pCell->rtc_flow.tf_text);
					if (nLen > 100) {
						// Force wrapping of long column contents
						nLen = 100;
					}
					nWidth = (int)nLen * 6;
				}
				nWidth += 30 + _tableMargins.m_left + _tableMargins.m_right;
				if (nWidth > pTable->rt_columnWidths[nColumn]) {
					pTable->rt_columnWidths[nColumn] = nWidth;
				}
			} else {
				pTable->rt_columnWidths[nColumn] = 20;
			}
			if (pCell->rtc_flow.tf_runs && pCell->rtc_isHeader) {
				pCell->rtc_flow.tf_runs->tr_attributes.weight = FW_BOLD;
			}
			pCell = pCell->rtc_next;
			nColumn++;
		}
		pRow = pRow->rtr_next;
	}
	if (!pTable->rt_columnCount) {
		pTable->rt_columnCount = nColumn;
	}
	for (nColumn = 0; nColumn < MAX_TABLE_COLUMNS; nColumn++) {
		pTable->rt_totalColumnWidth += pTable->rt_columnWidths[nColumn];
	}
}

/*
 * End a currently open part. For now regular text based parts not containing a text will be discarded.
 */
static int mdr_endPart(HTML_PARSER_STATE* pState) {
	if (!pState->hps_part) {
		return 0;
	}
	if (pState->hps_part->rvp_type == MET_TABLE) {
		mdr_finishTableSetup(pState->hps_table);
		pState->hps_table = 0;
		pState->hps_tableCell = 0;
		pState->hps_tableRow = 0;
	} else if (pState->hps_part->rvp_type != MET_HORIZONTAL_RULE && pState->hps_part->rvp_data.rvp_flow.tf_text == 0 && pState->hps_part->rvp_decoration == 0) {
		mdr_resetFontStyleDelta(pState->hps_currentStyle);
		pState->hps_blockLevel = 0;
		return 0;
	}
	pState->hps_blockLevel = 0;
	pState->hps_part = 0;
	return 1;
}

static int mdr_applyImageAttributes(HTML_PARSER_STATE* pState, HASHMAP* pValues) {
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (!pPart || pPart->rvp_type == MET_TABLE || !pValues) {
		return 0;
	}
	char* pszLink = (char*)hashmap_get(pValues, "src");
	char* pszTitle = (char*)hashmap_get(pValues, "alt");
	if (!pszTitle) {
		pszTitle = (char*)hashmap_get(pValues, "title");
		if (!pszTitle) {
			pszTitle = (char*)hashmap_get(pValues, "name");
		}
	}
	char* pszW = (char*)hashmap_get(pValues, "width");
	char* pszH = (char*)hashmap_get(pValues, "height");
	FONT_STYLE_DELTA* pfsd = pState->hps_currentStyle;
	pfsd->fsd_logicalStyles |= ATTR_LINK;
	char* pszText = " ";
	int nLen = (int)strlen(pszText);
	stringbuf_appendString(pState->hps_text, pszText);
	TEXT_RUN* pRun = mdr_appendRun(mdr_getBlockRunsOf(pPart), &pState->hps_blockFormat, nLen, pfsd, pState->hps_lp, 0, 0);
	if (pszTitle) {
		pRun->tr_title = _strdup(pszTitle);
	}
	MD_IMAGE* pImage = calloc(1, sizeof(MD_IMAGE));
	pRun->tr_data.tr_image = pImage;
	if (pImage != NULL) {
		if (pszW) {
			pImage->mdi_width = (int)string_convertToLong(pszW);
		}
		if (pszH) {
			pImage->mdi_height = (int)string_convertToLong(pszH);
		}
		if (pfsd->fsd_strokeColor != NO_COLOR && pfsd->fsd_strokeWidth > 0) {
			pImage->mdi_borderColor = pfsd->fsd_strokeColor;
			pImage->mdi_borderWidth = pfsd->fsd_strokeWidth;
		}
	}
	pRun->tr_imageUrl = mdr_processUrlWithBase(pState->hps_baseUrl, pszLink, FALSE);
	return nLen;
}

/*
 * Initialize the parser state
 */
static void mdr_initParserState(HTML_PARSER_STATE* pState, RENDER_VIEW_PART** pHead, MD_REFERENCE_DEFINITION** pReferenceDefinitions, 
		HASHMAP* pImageCache, const char* pszBaseUrl) {
	memset(pState, 0, sizeof *pState);
	pState->hps_listType = MET_UNORDERED_LIST;
	pState->hps_text = stringbuf_create(256);
	pState->hps_head = pHead;
	*pReferenceDefinitions = 0;
	pState->hps_referenceDefinitions = pReferenceDefinitions;
	pState->hps_imageCache = pImageCache;
	pState->hps_currentStyle = pState->hps_styleTable;
	pState->hps_baseUrl = pszBaseUrl;
	mdr_resetFontStyleDelta(pState->hps_currentStyle);
}

/*
 * Apply the attributes of the last open tag on a text run.
 */
static void mdr_applyRunAttributes(HTML_PARSER_STATE* pState, TEXT_RUN* pRun) {
	if (pRun && pState->hps_lastAttributes) {
		char* pszLink = (char*)hashmap_get(pState->hps_lastAttributes, "href");
		if (pszLink) {
			pRun->tr_link = mdr_processUrlWithBase(pState->hps_baseUrl, pszLink, FALSE);
		}
		char* pszTitle = (char*)hashmap_get(pState->hps_lastAttributes, "title");
		if (pszTitle) {
			pRun->tr_title = _strdup(pszTitle);
		}
		hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
		pState->hps_lastAttributes = 0;
	}
}

static TEXT_FLOW* mdr_getTextFlowEnsureInit(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	if (!pState) {
		return 0;
	}

	if (!pState->hps_part) {
		mdr_ensureParagraph(pStream, pState);
		if (pState->hps_part == NULL) {
			return 0;
		}
	}
	if (pState->hps_table && !pState->hps_tableCell) {
		return 0;
	}
	return pState->hps_tableCell ? &pState->hps_tableCell->rtc_flow : &pState->hps_part->rvp_data.rvp_flow;

}

static TEXT_FLOW* mdr_endRun(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bForced) {
	size_t nSize = stringbuf_size(pState->hps_text);
	if (!nSize && !bForced) {
		return 0;
	}
	TEXT_FLOW* pFlow = mdr_getTextFlowEnsureInit(pStream, pState);
	int nDelta = (int)(nSize - pState->hps_lastTextOffset);
	if (pFlow && (nDelta || bForced)) {
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, &pState->hps_blockFormat, nDelta, pState->hps_currentStyle, pState->hps_lp, pState->hps_runOffset, 0);
		mdr_applyRunAttributes(pState, pRun);
		pState->hps_lastTextOffset += nDelta;
		pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
	}
	return pFlow;
}

/*
 * Apply a grammar on a fenced code block and add color coding.
 */
static void mdr_applyGrammar(RENDER_VIEW_PART* pPart) {
	if (pPart->rvp_param.rvp_grammar == NULL) {
		return;
	}
	GRAMMAR* pGrammar = doctypes_findGrammarForLanguage(pPart->rvp_param.rvp_grammar);
	if (pGrammar == NULL) {
		return;
	}
	TEXT_FLOW* pFlow = &pPart->rvp_data.rvp_flow;
	if (pFlow->tf_text == NULL) {
		return;
	}
	HIGHLIGHTER* pHighlighter = highlight_createHighlighter(pGrammar);
	TEXT_RUN* pRun = pFlow->tf_runs;
	LEXICAL_ELEMENT elements[MAX_LEXICAL_ELEMENT];
	int lineEndSpanning = 0;
	LEXICAL_STATE startState = INITIAL;
	int nOffset = 0;
	while (pRun != NULL) {
		size_t lLength = pRun->tr_size;
		TEXT_RUN* pNext = pRun->tr_next;
		int nTokens = grammar_parse(pGrammar, elements, startState, &pFlow->tf_text[nOffset], lLength, &lineEndSpanning);
		nOffset += (int)lLength;
		if (nTokens > 0) {
			startState = elements[nTokens - 1].le_state;
		}
		for (int i = 0; i < nTokens && pRun; i++) {
			LEXICAL_ELEMENT state = elements[i];
			EDTEXTSTYLE* pStyle = highlight_getTextStyleForLexicalState(pHighlighter, state.le_state);
			pRun->tr_size = state.le_length;
			if (state.le_length > lLength) {
				lLength = 0;
			} else {
				lLength -= state.le_length;
			}
			if (i < nTokens-1) {
				// Insert new run
				pRun->tr_attributes.lineBreak = 0;
				TEXT_RUN* pNew = calloc(1, sizeof(*pRun));
				if (pNew != NULL) {
					pNew->tr_size = state.le_length;
					pNew->tr_attributes = pRun->tr_attributes;
					pRun->tr_attributes.lineBreak = 0;
					pNew->tr_next = pRun->tr_next;
					pRun->tr_next = pNew;
				}
			} else {
				pRun->tr_attributes.lineBreak = 1;
				pRun->tr_size = state.le_length;
			}
			pRun->tr_attributes.fgColor = pStyle->fgcolor;
			pRun->tr_attributes.bgColor = pStyle->bgcolor;
			pRun = pRun->tr_next;
		}
		pRun = pNext;
	}
	highlight_destroy(pHighlighter);
}

static void mdr_closeTextElement(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bForced) {
	TEXT_FLOW* pFlow = mdr_endRun(pStream, pState, bForced);
	if (!pFlow) {
		return;
	}
	pFlow->tf_text = _strdup(stringbuf_getString(pState->hps_text));
	stringbuf_reset(pState->hps_text);
	pState->hps_lastTextOffset = 0;
	pState->hps_tableCell = 0;
	mdr_applyGrammar(pState->hps_part);
}

static void mdr_finalizeParserState(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	mdr_closeTextElement(pStream, pState, FALSE);
	stringbuf_destroy(pState->hps_text);
	hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
}

static void mdr_onTagSpecialHandling(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	const char* pszTN = pTag->ht_descriptor->tm_tagName;
	if (strcmp("ol", pszTN) == 0) {
		pState->hps_listType = MET_ORDERED_LIST;
		int nLevel = pState->hps_orderedListListLevel;
		pState->hps_orderedListListLevel = pTag->ht_isClose ? nLevel - 1 : nLevel + 1;
		if (!pTag->ht_isClose) {
			pState->hps_orderedListIndices[nLevel] = 1;
		}
		return;
	}
	if (strcmp("ul", pszTN) == 0) {
		pState->hps_listType = MET_UNORDERED_LIST;
		pState->hps_unorderedListLevel = pTag->ht_isClose ? pState->hps_unorderedListLevel - 1 : pState->hps_unorderedListLevel + 1;
		return;
	}
	if (strcmp("tr", pszTN) == 0) {
		if (pTag->ht_isClose) {
			pState->hps_tableRow = 0;
		}
		else if (pState->hps_table) {
			pState->hps_tableRow = (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pState->hps_table->rt_rows, sizeof * pState->hps_tableRow);
		}
		return;
	}
	if (strcmp("td", pszTN) == 0 || strcmp("th", pszTN) == 0) {
		if (pTag->ht_isClose) {
			if (pState->hps_tableCell) {
				mdr_closeTextElement(pStream, pState, TRUE);
			}
			pState->hps_tableCell = 0;
		}
		else if (pState->hps_tableRow) {
			RENDER_TABLE_CELL* pTC = (RENDER_TABLE_CELL*)ll_append((LINKED_LIST**)&pState->hps_tableRow->rtr_cells, sizeof * pState->hps_tableCell);
			pState->hps_tableCell = pTC;
			pTC->rtc_isHeader = pTag->ht_descriptor->tm_tagName[1] == 'h';
			pTC->rtc_flow.tf_align = pState->hps_currentStyle->fsd_textAlign;
		}
		mdr_skipLeadingSpace(pStream);
		return;
	}
}

static int mdr_processOpeningAnchor(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HASHMAP* pAttributes) {
	if (!pAttributes) {
		return 0;
	}
	char* pszAnchor = (char*)hashmap_get(pAttributes, "name");
	if (pszAnchor) {
		TEXT_FLOW* pFlow = mdr_endRun(pStream, pState, TRUE);
		if (pFlow) {
			pFlow->tf_runs->tr_anchor = _strdup(pszAnchor);
		}
	}
	return hashmap_containsKey(pAttributes, "href");
}

static void mdr_onInlineTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	if (pTag->ht_isClose) {
		if (pTag->ht_descriptor->tm_noCloseTag) {
			return;
		}
		pState->hps_currentStyle->fsd_logicalStyles |= pTag->ht_descriptor->tm_textAttrClose;
		mdr_endRun(pStream, pState, FALSE);
		if (pState->hps_currentStyle > pState->hps_styleTable) {
			pState->hps_currentStyle--;
		}
	} else {
		const char* pszTag = pTag->ht_descriptor->tm_tagName;
		if (strcmp("img", pszTag) == 0) {
			pState->hps_lastTextOffset += mdr_applyImageAttributes(pState, pTag->ht_attributes);
			return;
		}
		if (strcmp("a", pszTag) == 0 && !mdr_processOpeningAnchor(pStream, pState, pTag->ht_attributes)) {
			return;
		}
		BOOL bBreak = pTag->ht_descriptor->tm_textAttr & ATTR_LINE_BREAK;
		if (bBreak) {
			pState->hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
			mdr_skipLeadingSpace(pStream);
		}
		mdr_endRun(pStream, pState, bBreak);
		FONT_STYLE_DELTA fsdNext = *pState->hps_currentStyle;
		fsdNext.fsd_logicalStyles |= pTag->ht_descriptor->tm_textAttr;
		BOOL bPop = pTag->ht_descriptor->tm_noCloseTag;
		if (pState->hps_currentStyle < &pState->hps_styleTable[DIM(pState->hps_styleTable) - 1]) {
			pState->hps_currentStyle++;
			*pState->hps_currentStyle = fsdNext;
		} else {
			bPop = FALSE;
		}
		if (bPop) {
			pState->hps_currentStyle--;
		}
	}
}

/*
 * Configure table from defined CSS style and additional table attributes. 
 */
static void mdr_applyTableAttributes(HTML_PARSER_STATE* pState, HASHMAP* pAttributes) {
	RENDER_TABLE* pTable = pState->hps_table;
	if (!pTable) {
		return;
	}
	FONT_STYLE_DELTA* pfsd = pState->hps_currentStyle;
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (pfsd->fsd_strokeWidth) {
		pTable->rt_borderWidth = pfsd->fsd_strokeWidth;
	}
	if (pfsd->fsd_strokeColor != NO_COLOR) {
		pTable->rt_borderColor = pfsd->fsd_strokeColor;
	}
	if (!pAttributes) {
		return;
	}
	char* pszBorder = (char*)hashmap_get(pAttributes, "border");
	if (pszBorder) {
		pTable->rt_borderWidth = (int)string_convertToLong(pszBorder);
	}
	char* pszWidth = (char*)hashmap_get(pAttributes, "width");
	if (pszWidth) {
		pPart->rvp_width = mdr_parseSize(pszWidth);
	}
}

static void mdr_onBlockLevelTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	MDR_ELEMENT_TYPE mType = pTag->ht_descriptor->tm_blockElement;
	if (pTag->ht_isClose && pTag->ht_isOpen) {
		if (!pState->hps_blockLevel) {
			pState->hps_part = mdr_newPart(pStream, pState, mType, pTag->ht_descriptor->tm_elementLevel);
			mdr_endPart(pState);
		}
		return;
	}
	if (pTag->ht_isClose) {
		if (pState->hps_blockLevel > 0) {
			pState->hps_blockLevel--;
		}
		if (pState->hps_blockLevel > 0) {
			return;
		}
	}
	// Save current font-style settings - may be reset across part state handling
	FONT_STYLE_DELTA fsd = *pState->hps_currentStyle;
	if (mType == MET_TABLE && !pState->hps_table && pTag->ht_isOpen) {
		// For tables force a new part start
		pState->hps_blockLevel = 0;
	}
	if (mType == MET_FENCED_CODE_BLOCK && pTag->ht_isOpen) {
		pState->hps_blockLevel = 0;
	}
	// close current block if either it was an "auto-created" block (blocklevel == 0) or it is explicitly closed and the block
	if (pTag->ht_isClose || pState->hps_blockLevel == 0) {
		mdr_closeTextElement(pStream, pState, FALSE);
		if (mType == MET_TABLE || !pState->hps_table) {
			mdr_endPart(pState);
			*pState->hps_currentStyle = fsd;
		}
		if (pTag->ht_isClose) {
			return;
		}
	}
	if (pState->hps_table) {
		return;
	}
	if (pState->hps_blockLevel <= 1) {
		int nLevel = pTag->ht_descriptor->tm_elementLevel;
		if (mType == MET_UNORDERED_LIST) {
			mType = pState->hps_listType;
		}
		if (mType == MET_UNORDERED_LIST) {
			nLevel = pState->hps_unorderedListLevel;
		} else if (mType == MET_ORDERED_LIST) {
			nLevel = pState->hps_orderedListListLevel;
		}
		pState->hps_part = mdr_newPart(pStream, pState, mType, nLevel);
		if (mType == MET_HORIZONTAL_RULE) {
			pState->hps_blockLevel = 0;
		}
		if (mType == MET_ORDERED_LIST) {
			pState->hps_part->rvp_param.rvp_number = pState->hps_orderedListIndices[nLevel-1]++;
		}
		*pState->hps_currentStyle = fsd;
		if (mType != MET_FENCED_CODE_BLOCK) {
			mdr_skipLeadingSpace(pStream);
		}
		if (fsd.fsd_width.ss_units != CSU_NONE) {
			pState->hps_part->rvp_width = fsd.fsd_width;
		}
		if (fsd.fsd_fontSize.ss_units != CSU_NONE) {
			pState->hps_blockFormat.mef_fontSize = fsd.fsd_fontSize;
		}
		if (mType == MET_TABLE) {
			pState->hps_table = pState->hps_part->rvp_data.rvp_table;
			mdr_applyTableAttributes(pState, pTag->ht_attributes);
		} else if (mType == MET_FENCED_CODE_BLOCK) {
			mdr_parsePreformattedCodeBlock(pStream, pState, FALSE, pTag->ht_descriptor->tm_tagName);
			if (stringbuf_size(pState->hps_text)) {
				pState->hps_part->rvp_data.rvp_flow.tf_text = _strdup(stringbuf_getString(pState->hps_text));
				stringbuf_reset(pState->hps_text);
			} else {
				free(pState->hps_part->rvp_decoration);
				pState->hps_part->rvp_decoration = 0;
			}
			pState->hps_lastTextOffset = 0;
			mdr_endPart(pState);
		}
	}
}

static void mdr_onHtmlTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag, FONT_STYLE_DELTA* pFSDDelta) {
	HTML_TAG_TYPE tType = pTag->ht_descriptor->tm_tagType;

	FONT_STYLE_DELTA* pOldFSD = pState->hps_currentStyle;
	FONT_STYLE_DELTA fsdOld = *pOldFSD;
	if (!pTag->ht_isClose) {
		*pState->hps_currentStyle = *pFSDDelta;
	}
	if (tType == HTT_SPECIAL) {
		mdr_onTagSpecialHandling(pStream, pState, pTag);
	} else if (tType == HTT_INLINE) {
		mdr_onInlineTag(pStream, pState, pTag);
	} else {
		mdr_onBlockLevelTag(pStream, pState, pTag);
		if (pFSDDelta->fsd_textColor != NO_COLOR) {
			fsdOld.fsd_textColor = pFSDDelta->fsd_textColor;
		}
		fsdOld.fsd_textAlign = pFSDDelta->fsd_textAlign;
	}
	if (pTag->ht_attributes) {
		hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
		pState->hps_lastAttributes = pTag->ht_attributes;
	}
	if (!pTag->ht_isClose) {
		*pOldFSD = fsdOld;
	}
}

/*
 * Parse a text and convert it into a list of view parts to be rendered later.
 * Note, that the viewparts must be destroyed, when they are not needed any more
 * using mdr_destroyRendererData.
 */
MARKDOWN_RENDERER_DATA* mdr_parseHTML(INPUT_STREAM* pStream, HWND hwndParent, const char* pszBaseURL) {
	RENDER_VIEW_PART* pFirst = 0;
	char c;
	size_t nSize;
	HTML_PARSER_STATE state;
	MD_REFERENCE_DEFINITION* pUnused;

	mdr_initParserState(&state, &pFirst, &pUnused, NULL, pszBaseURL);
	state.hps_syntax = &_htmlSyntax;
	while ((c = pStream->is_getc(pStream)) != 0) {
		if (c == '&') {
			if (mdr_parseEntity(state.hps_text, pStream)) {
				continue;
			}
		} else if (c == '<') {
			if (mdr_skipHTMLCommentOrDoctype(pStream)) {
				continue;
			}
			HTML_TAG tag;
			FONT_STYLE_DELTA fsdNext = *state.hps_currentStyle;
			BOOL bMatch = mdr_getTag(pStream, &fsdNext, &tag);
			if (bMatch) {
				if (!tag.ht_isClose) {
					mdr_endRun(pStream, &state, FALSE);
				}
				mdr_onHtmlTag(pStream, &state, &tag, &fsdNext);
			}
			continue;
		} else if (c == '\n' && pStream->is_peekc(pStream, 0) == '\n' && state.hps_table == 0) {
			// treat empty line as real line break.
			nSize = stringbuf_size(state.hps_text) - state.hps_lastTextOffset;
			state.hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
			if (nSize) {
				mdr_ensureParagraph(pStream, &state);
				TEXT_RUN* pRun = mdr_appendRun(mdr_getBlockRunsOf(state.hps_part), &state.hps_blockFormat, nSize, state.hps_currentStyle,
					state.hps_lp, state.hps_runOffset, 0);
				mdr_applyRunAttributes(&state, pRun);
				state.hps_lastTextOffset += (int)nSize;
			}
			state.hps_runOffset = pStream->is_inputMark(pStream, &state.hps_lp);
			pStream->is_getc(pStream);
			continue;
		}
		if (c != '\n') {
			if (stringbuf_size(state.hps_text) == 0) {
				state.hps_runOffset = pStream->is_inputMark(pStream, &state.hps_lp)-1;
			}
			stringbuf_appendChar(state.hps_text, c);
			if (c == ' ') {
				mdr_skipLeadingSpace(pStream);
			}
		}
	}
	mdr_finalizeParserState(pStream, &state);
	MARKDOWN_RENDERER_DATA* pData = calloc(1, sizeof * pData);
	if (pData != NULL) {
		pData->md_pElements = pFirst;
		if (hwndParent) {
			pData->md_hwndTooltip = cust_createToolTooltip(hwndParent);
		}
	}
	return pData;
}

/*
 * Parses an HTML file and create render view parts for wysiwyg rendering.
 */
static void mdr_parseFileToHTML(WINFO* wp) {
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	MARKDOWN_RENDERER_DATA* pParsed = mdr_parseHTML(pStream, 0, fp->fname);
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	pData->md_pElements = pParsed->md_pElements;
	pData->md_parse = mdr_parseFileToHTML;
	free(pParsed);
	pStream->is_destroy(pStream);
}

static TEXT_RUN* mdr_appendRunState(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, FONT_STYLE_DELTA* pFSD) {
	TEXT_FLOW* pFlow = mdr_getTextFlowEnsureInit(pStream, pState);
	if (!pFlow) {
		return 0;
	}
	int nSize = (int)(stringbuf_size(pState->hps_text) - pState->hps_lastTextOffset);
	TEXT_RUN* pResult = mdr_appendRun(&pFlow->tf_runs, &pState->hps_blockFormat, nSize, pFSD, pState->hps_lp, pState->hps_runOffset, 0);
	pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
	pState->hps_lastTextOffset += (int)nSize;
	return pResult;
}

static void mdr_appendRefLinkDefinition(MD_REFERENCE_DEFINITION** pHead, char* pszLinkText, char* pszUrl, char* pszTitle) {
	MD_REFERENCE_DEFINITION* pLink = ll_append(pHead, sizeof **pHead);
	pLink->mdrs_type = pszLinkText[0] == '^' ? MDDRT_REFERENCE_FOOT_NOTE : MDDRT_REFERENCE_STYLE_LINK;
	if (pszLinkText[0] == '^') {
		pszLinkText++;
	}
	pLink->mdrs_name = _strdup(pszLinkText);
	pLink->mdrs_title = pszTitle;
	pLink->mdrs_text = pszUrl;
}

static WCHAR* mdr_findEmoji(INPUT_STREAM* pStream) {
	if (_emojiOffsets[0] == 0) {
		memset(_emojiOffsets, -1, sizeof _emojiOffsets);
		char c = 0;
		for (int i = 0; i < DIM(_emojis); i++) {
			if (_emojis[i].name[1] != c) {
				c = _emojis[i].name[1];
				_emojiOffsets[c] = i;
			}
		}
	}
	char szDef[5];
	size_t nLen = 0;
	szDef[0] = ':';
	int nOffset = -1;
	// maximum length of emoji name: 128.
	for (int i = 1; i < 128; i++) {
		char c = pStream->is_peekc(pStream, i);
		if (c == 0 || c == ' ') {
			break;
		}
		if (i == 1) {
			nOffset = _emojiOffsets[c];
			if (nOffset < 0) {
				return NULL;
			}
			szDef[1] = c;
		} else {
			if (c == ':') {
				if (_emojis[nOffset].name[i] == ':') {
					nLen = i;
					pStream->is_skip(pStream, (int)nLen);
					return _emojis[nOffset].emoji;
				}
				break;
			}
			while (_emojis[nOffset].name[i] != c) {
				if (++nOffset >= DIM(_emojis)) {
					return NULL;
				}
				if (_emojis[nOffset].name[1] != szDef[1]) {
					return NULL;
				}
			}
		}
	}
	return NULL;
}

/*
 * Detect a link reference in the input - markdown style.
 */
static BOOL mdr_detectLink(INPUT_STREAM* pStream, char c) {
	return (c == '[' || (c == '!' && pStream->is_peekc(pStream, 1) == '['));
}

/*
 * Parse a top-level element to be rendered with a view part possibly containing nested formatting. 
 */
static void mdr_parseFlow(INPUT_STREAM* pStream, HTML_PARSER_STATE*pState) {
	stringbuf_reset(pState->hps_text);
	pState->hps_lastTextOffset = 0;
	int nState = 0;
	RENDER_VIEW_PART_PARAM param = { 0 };

	mdr_resetFontStyleDelta(pState->hps_currentStyle);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	BOOL bEnforceBreak = TRUE;
	while (pStream->is_peekc(pStream, 0)) {
		BOOL bSkipped = FALSE;
		char lastC = 0;
		char cNext;
		char c = 0;
		int nLineOffset = 0;
		pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
		for (; (cNext = pStream->is_peekc(pStream, 0)) != 0 && c != '\n'; pStream->is_skip(pStream, 1)) {
			lastC = c;
			c = cNext;
			if (nLineOffset == 0 && pState->hps_blockLevel == 0 && mdr_isIndentedFencedBlock(pStream, mType)) {
				mType = MET_FENCED_CODE_BLOCK;
				pState->hps_blockFormat = _formatFenced;
				mdr_parsePreformattedCodeBlock(pStream, pState, TRUE, 0);
				pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
				goto outer;
			} else if (nState <= 1 && string_isSpace(c)) {
				continue;
			}
			nLineOffset = 1;
			if (nState == 1 && mType == MET_BLOCK_QUOTE && c == '>' && !bSkipped) {
				if (pStream->is_peekc(pStream, 1) == '\n') {
					pState->hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
				}
				bSkipped = TRUE;
				continue;
			}
			if (nState == 0) {
				if (pState->hps_blockLevel == 0) {
					mType = mdr_determineTopLevelElement(pStream, &param, pState->hps_syntax);
					pState->hps_part = mdr_newPart(pStream, pState, mType, param.rvp_level);
					if (mType == MET_HORIZONTAL_RULE) {
						return;
					}
					if (mType == MET_ORDERED_LIST || mType == MET_TASK_LIST) {
						pState->hps_part->rvp_param.rvp_number = param.rvp_number;
					}
					if (mType == MET_FENCED_CODE_BLOCK) {
						pState->hps_part->rvp_param.rvp_grammar = param.rvp_grammar;
						param.rvp_grammar = 0;
						mdr_parsePreformattedCodeBlock(pStream, pState, FALSE, 0);
						pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
						goto outer;
					}
					if (mType != MET_NORMAL) {
						pState->hps_runOffset++;
						nState = 1;
						continue;
					}
				}
			}
			if (nState < 2) {
				nState = 2;
			}
			bEnforceBreak = FALSE;
			int nToggle = 0;
			BOOL bEscaped = pState->hps_currentStyle->fsd_logicalStyles & ATTR_CODE;
			if (!bEscaped && c == ':') {
				WCHAR* pEmoji = mdr_findEmoji(pStream);
				if (pEmoji != NULL) {
					int nSize = (int)(stringbuf_size(pState->hps_text) - pState->hps_lastTextOffset);
					if (nSize > 0) {
						mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
					}
					pState->hps_currentStyle->fsd_logicalStyles |= ATTR_EMOJI;
					TEXT_RUN* pRun = mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
					pRun->tr_isEmoji = TRUE;
					pRun->tr_data.tr_emoji = pEmoji;
					pState->hps_currentStyle->fsd_logicalStyles &= ~ATTR_EMOJI;
					continue;
				}
			}
			if (c == '\\' && strchr(_escapedChars, pStream->is_peekc(pStream, 1)) != NULL) {
				pStream->is_skip(pStream, 1);
				c = pStream->is_peekc(pStream, 0);
			}
			else if (!bEscaped && c == '=' && pStream->is_peekc(pStream, 1) == c && 
					((pState->hps_currentStyle->fsd_logicalStyles & ATTR_HIGHLIGHT) || pStream->is_peekc(pStream, 2) != ' ')) {
				pStream->is_skip(pStream, 1);
				mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
				pState->hps_currentStyle->fsd_logicalStyles ^= ATTR_HIGHLIGHT;
				continue;
			} else if (c == '`' || (!bEscaped && (c == '*' ||
				// allow for _ only at word borders.
					(c == '_' && mdr_isAtWordBorder(pStream)) ||
				c == '~'))) {
				if (nLineOffset == 1 && mType == MET_BLOCK_QUOTE && c == '*' && pStream->is_peekc(pStream, 1) == ' ') {
					nToggle = ATTR_LINE_BREAK;
				} else if (c == '*' || c == '_') {
					if (pStream->is_peekc(pStream, 1) == c) {
						nToggle = ATTR_STRONG;
						pStream->is_skip(pStream, 1);
						c = c == '*' ? '_' : '*';
						if (pStream->is_peekc(pStream, 1) == c) {
							nToggle = ATTR_EMPHASIS;
							pStream->is_skip(pStream, 1);
						}
					} else {
						nToggle = ATTR_EMPHASIS;
					}
				} else if (c == '~' && pStream->is_peekc(pStream, 1) == c) {
					nToggle = ATTR_STRIKE;
					pStream->is_skip(pStream, 1);
				} else if (c == '`') {
					nToggle = ATTR_CODE;
				}
				if (nToggle) {
					mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
					pState->hps_currentStyle->fsd_logicalStyles ^= nToggle;
					continue;
				}
			} else if (!bEscaped && pState->hps_syntax->syn_detectLink(pStream, c)) {
				BOOL bImage = c == '!';
				BOOL bImageLink = FALSE;
				pStream->is_skip(pStream, bImage ? 2 : 1);
				LINK_PARSE_RESULT result;
				char szLinkText[256];
				// Image with link
				if (c == '[' && pStream->is_peekc(pStream, 0) == '!' && pStream->is_peekc(pStream, 1) == '[') {
					bImage = TRUE;
					bImageLink = TRUE;
					pStream->is_skip(pStream, 1);
				}
				if (pState->hps_syntax->syn_parseLink(pStream, pState, szLinkText, &result, LPS_TITLE)) {
					if (result.lpr_isReferenceLinkDefinition) {
						mdr_appendRefLinkDefinition(pState->hps_referenceDefinitions, szLinkText, result.lpr_LinkUrl, result.lpr_title);
						pStream->is_skip(pStream, -1);
					} else {
						int nAttr = 0;
						mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
						int nTextStart = 0;
						char c2 = szLinkText[0];
						int nTextEnd = (int)strlen(szLinkText);
						if (c2 == szLinkText[nTextEnd - 1]) {
							if (c2 == '`') {
								nAttr = ATTR_CODE;
							}
							else if (c2 == '*' || c2 == '_') {
								if (szLinkText[nTextStart + 1] == c2) {
									nAttr = ATTR_STRONG;
									nTextStart++;
									nTextEnd--;
								}
								else {
									nAttr = ATTR_EMPHASIS;
								}
							}
							if (nAttr) {
								nTextStart++;
								nTextEnd--;
							}
						}
						int nLen = nTextEnd - nTextStart;
						if (nLen == 0) {
							szLinkText[nTextEnd++] = ' ';
							nLen = 1;
						}
						stringbuf_appendStringLength(pState->hps_text, &szLinkText[nTextStart], nLen);
						pState->hps_currentStyle->fsd_logicalStyles |= ATTR_LINK;
						int nLastOffset = pState->hps_lastTextOffset;
						TEXT_RUN* pRun = mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
						if (bImage) {
							pRun->tr_data.tr_image = calloc(1, sizeof(MD_IMAGE));
							if (pRun->tr_data.tr_image != NULL) {
								pRun->tr_data.tr_image->mdi_height = result.lpr_height;
								pRun->tr_data.tr_image->mdi_width = result.lpr_width;
							}
							mdr_assignImageUrl(pState->hps_imageCache, pRun, result.lpr_LinkUrl);
							if (bImageLink) {
								pStream->is_skip(pStream, 1);
								if (pState->hps_syntax->syn_parseLink(pStream, pState, szLinkText, &result, LPS_WAIT_URL)) {
									bImage = FALSE;
								}
								pStream->is_skip(pStream, -1);
							}
						}
						if (bImage && !result.lpr_title) {
							result.lpr_title = _strdup(&stringbuf_getString(pState->hps_text)[nLastOffset]);
						}
						if (!bImage) {
							pRun->tr_isRefLink = result.lpr_LinkRefUrl != 0;
							pRun->tr_link = result.lpr_LinkUrl;
							if (pRun->tr_isRefLink) {
								pRun->tr_link = result.lpr_LinkRefUrl;
							}
						}
						pRun->tr_title = result.lpr_title;
						if (!bImageLink) {
							pStream->is_skip(pStream, -1);
						}
						pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
					}
					continue;
				} else {
					pStream->is_skip(pStream, bImage ? -2 : -1);
				}
			} else if (c == '&') {
				STREAM_OFFSET sp = pStream->is_tell(pStream);
				pStream->is_skip(pStream, 1);
				if (mdr_parseEntity(pState->hps_text, pStream)) {
					pStream->is_skip(pStream, -1);
					continue;
				}
				pStream->is_seek(pStream, sp);
			} else if (!bEscaped && c == '<') {
				HTML_TAG tag;
				STREAM_OFFSET offset = pStream->is_tell(pStream);
				FONT_STYLE_DELTA fsdNext = *pState->hps_currentStyle;
				pStream->is_skip(pStream, 1);
				int bSuccess = mdr_getTag(pStream, &fsdNext, &tag);
				if (bSuccess) {
					if (!tag.ht_isClose) {
						mdr_endRun(pStream, pState, FALSE);
					}
					mdr_onHtmlTag(pStream, pState, &tag, &fsdNext);
					pStream->is_skip(pStream, -1);
					continue;
				}
				pStream->is_seek(pStream, offset);
				if (mdr_parseAutolinks(pStream, pState, mdr_getTextFlowEnsureInit(pStream, pState), &pState->hps_blockFormat, pState->hps_currentStyle, pState->hps_lastTextOffset)) {
					pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
					continue;
				}
			}
			if (c != '\n') {
				if (c == lastC && c == ' ' && pStream->is_peekc(pStream, 1) == '\n') {
					break;
				}
				stringbuf_appendChar(pState->hps_text, c);
			}
		}
		if (c == ' ' && lastC == ' ') {
			pStream->is_skip(pStream, 1);
			bEnforceBreak = TRUE;
		} else if (lastC != ' ') {
			stringbuf_appendChar(pState->hps_text, ' ');
		}
		if (bEnforceBreak) {
			pState->hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
		}
		mdr_appendRunState(pStream, pState, pState->hps_currentStyle);
		if (mdr_isTopLevelOrBreak(pStream, pState->hps_syntax, mType, param.rvp_level)) {
			break;
		}
		nState = 1;
	}
outer:
	if (pState->hps_part) {
		pState->hps_part->rvp_type = mType;
		if (pState->hps_blockFormat.mef_fontSize.ss_units != CSU_NONE) {
			mdr_applyFormat(pState->hps_part, &pState->hps_blockFormat);
		}
	}
	mdr_closeTextElement(pStream, pState, FALSE);
}

static RENDER_TABLE_CELL *mdr_parseTableCell(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, RENDER_TABLE_ROW* pRow, STRING_BUF* pContents, 
		LINE*lp, int nLineOffset, BOOL bHeader) {
	RENDER_TABLE_CELL* pCell = ll_append(&pRow->rtr_cells, sizeof * pCell);
	pCell->rtc_isHeader = bHeader;
	STRING_BUF* sb = stringbuf_create(128);
	char* pszBuf = stringbuf_getString(pContents);
	char* pszEnd = pszBuf + strlen(pszBuf)-1;
	while (pszEnd > pszBuf && pszEnd[-1] == ' ') {
		*--pszEnd = 0;
	}
	INPUT_STREAM* pInnerStream = streams_createStringInputStream(pszBuf);
	pState->hps_tableCell = pCell;
	mdr_parseFlow(pInnerStream, pState);
	pState->hps_tableCell = 0;
	// Hack: this is the attempt to get selections in table cell working
	// If the table cell contains formatted contents - this won't work correctly.
	TEXT_RUN* pRun = pCell->rtc_flow.tf_runs;
	while (pRun) {
		pRun->tr_lp = lp;
		pRun->tr_lineOffset = nLineOffset;
		pRun = pRun->tr_next;
	}
	pStream->is_destroy(pInnerStream);
	stringbuf_destroy(sb);
	return pCell;
}

/*
 * Parse a table row.
 */
static BOOL mdr_parseTableRow(INPUT_STREAM* pStream, RENDER_TABLE* pTable, HTML_PARSER_STATE* pState, BOOL bHeader, const byte* columnAlignments, int* pMaxColumns) {
	long lnOffset;
	int nColumn = 0;
	int nStart = 0;
	RENDER_TABLE_ROW row;
	char c;
	LINE* lp = NULL;
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	const char* pszDelimiter = pState->hps_syntax->syn_tableDelimiter;

	if (pszDelimiter && pStream->is_strncmp(pStream, pszDelimiter, strlen(pszDelimiter)) == 0) {
		pStream->is_skip(pStream, (int) strlen(pszDelimiter));
		return FALSE;
	}
	memset(&row, 0, sizeof row);
	lnOffset = pStream->is_inputMark(pStream, &lp);
	int nMaxColumns = *pMaxColumns;
	STRING_BUF* pBuf = stringbuf_create(128);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n' && (nMaxColumns == -1 || nColumn < nMaxColumns)) {
		if (c == '|') {
			mdr_skipLeadingSpace(pStream);
			if (nStart > 0) {
				RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pStream, pState, &row, pBuf, lp, lnOffset, bHeader);
				if (!pCell) {
					break;
				}
				pCell->rtc_flow.tf_align = columnAlignments[nColumn++];
			}
			stringbuf_reset(pBuf);
			lnOffset = pStream->is_inputMark(pStream, &lp);
			nStart = 1;
		}
		else {
			char cEscaped;
			if (c == '\\' && (cEscaped = pStream->is_peekc(pStream, 0)) == '|') {
				pStream->is_skip(pStream, 1);
				c = cEscaped;
			}
			stringbuf_appendChar(pBuf, c);
			nStart = 1;
		}
	}
	if (nColumn == 0) {
		stringbuf_destroy(pBuf);
		if (c != 0 && nMaxColumns > 0 && pState->hps_syntax->syn_tableEmptyRowsAllowed) {
			pStream->is_positionToLineStart(pStream, 1);
			return TRUE;
		}
		pStream->is_seek(pStream, offset);
		return FALSE;
	}
	if ((nMaxColumns < 0 || nColumn < nMaxColumns) && (c == '|' || stringbuf_size(pBuf) > 0)) {
		RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pStream, pState, &row, pBuf, lp, lnOffset, bHeader);
		if (pCell) {
			pCell->rtc_flow.tf_align = columnAlignments[nColumn++];
		}
	}
	stringbuf_destroy(pBuf); 
	RENDER_TABLE_ROW* pAppended = (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pTable->rt_rows, sizeof row);
	memcpy(pAppended, &row, sizeof row);
	pStream->is_seek(pStream, offset);
	pStream->is_positionToLineStart(pStream, 1);
	if (nMaxColumns < 0) {
		*pMaxColumns = nColumn;
	}
	return TRUE;
}

/*
 * Try to interpret some lines as a markdown table. If successful update the line pointer to point beyond the
 * table and return TRUE. If no successful, return FALSE;
 */
static BOOL mdr_parseTable(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	byte columnAlignments[MAX_TABLE_COLUMNS];
	memset(columnAlignments, TA_ALIGN_DEFAULT, sizeof columnAlignments);
	BOOL bStartColon = FALSE;
	BOOL bDashSeen = FALSE;
	int nColumn = -1;
	int nAlign = TA_ALIGN_LEFT;
	char c;
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (pPart == 0) {
		// Create a normal part and initialize below (not using factory) as pTable was created already.
		pState->hps_part = pPart = mdr_newPart(pStream, pState, MET_NORMAL, 0);
	}
	const char* pszDelimiter = pState->hps_syntax->syn_tableDelimiter;
	STREAM_OFFSET offsetCurrent;
	if (pszDelimiter) {
		if (pStream->is_strncmp(pStream, pszDelimiter, strlen(pszDelimiter)) == 0) {
			pStream->is_skip(pStream, (int)strlen(pszDelimiter));
			offsetCurrent = pStream->is_positionToLineStart(pStream, 1);
		} else {
			return FALSE;
		}
	} else {
		offsetCurrent = pStream->is_positionToLineStart(pStream, 1);
		while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
			if (nColumn == -1) {
				if (isspace((unsigned char)c)) {
					continue;
				}
				if (c == '|' || c == '-' || c == '=' || c == ':') {
					nColumn = 0;
					if (c == ':') {
						bStartColon = TRUE;
					}
					if (c == '-' || c == '=') {
						nColumn = 1;
						bDashSeen = TRUE;
					}
				}
				else {
					pStream->is_seek(pStream, offsetCurrent);
					return FALSE;
				}
			}
			else {
				if (c == '-' || c == '=') {
					bDashSeen = TRUE;
				}
				else if (bDashSeen && c == ':') {
					nAlign = bStartColon ? TA_ALIGN_CENTER : TA_ALIGN_RIGHT;
				}
				else if (!bDashSeen && c == ':') {
					bStartColon = TRUE;
				}
				else if (bDashSeen && c == '|') {
					columnAlignments[nColumn++] = nAlign;
					bDashSeen = FALSE;
					bStartColon = FALSE;
					nAlign = TA_ALIGN_LEFT;
				}
				else {
					pStream->is_seek(pStream, offsetCurrent);
					return FALSE;
				}
			}
		}
		pStream->is_seek(pStream, offsetCurrent);
		if (nColumn < 0) {
			return FALSE;
		}
	}
# if 0
	if (lpDef->lbuf[lpDef->len - 1] != '|') {
		columnAlignments[nColumn++] = nAlign;
	}
#endif
	// OK - we have a valid table definition. Now parse header and rows.
	RENDER_TABLE* pTable = mdr_newTable(nColumn);
	if (!mdr_parseTableRow(pStream, pTable, pState, TRUE, columnAlignments, &nColumn)) {
		free(pTable);
		return FALSE;
	}
	pStream->is_seek(pStream, offsetCurrent);
	pStream->is_positionToLineStart(pStream, 2);
	while (mdr_parseTableRow(pStream, pTable, pState, FALSE, columnAlignments, &nColumn)) {
	}
	pPart->rvp_data.rvp_table = pTable;
	pPart->rvp_type = MET_TABLE;
	pPart->rvp_paint = mdr_renderTable;
	mdr_finishTableSetup(pTable);
	mdr_applyFormat(pPart, &_formatTable);
	return TRUE;
}

static void mdr_parseMarkdownFormat(WINFO *wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	char c;
	HTML_PARSER_STATE state;
	int nDelta = 0;
	RENDER_VIEW_PART* pLastPart = 0;

	pData->md_parse = mdr_parseMarkdownFormat;
	mdr_initParserState(&state, &pData->md_pElements, &pData->md_referenceDefinitions, pData->md_imageCache, fp->fname);
	state.hps_syntax = &_markdownSyntax;
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (c != '\n') {
			if (!mdr_parseTable(pStream, &state)) {
				state.hps_blockLevel = 0;
				mdr_parseFlow(pStream, &state);
			}
			pLastPart = pLastPart ? pLastPart->rvp_next : state.hps_part;
			if (pLastPart) {
				pLastPart->rvp_margins.m_top += nDelta;
				nDelta = 0;
			}
			pLastPart = state.hps_part;
			mdr_endPart(&state);
		} else {
			nDelta += PARAGRAPH_OFFSET / 2;
			pStream->is_skip(pStream, 1);
		}
	}
	mdr_finalizeParserState(pStream, &state);
	pStream->is_destroy(pStream);
}

static void mdr_parseAdocFormat(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	char c;
	HTML_PARSER_STATE state;
	int nDelta = 0;
	RENDER_VIEW_PART* pLastPart = 0;

	pData->md_parse = mdr_parseAdocFormat;
	mdr_initParserState(&state, &pData->md_pElements, &pData->md_referenceDefinitions, pData->md_imageCache, fp->fname);
	state.hps_syntax = &_asciidocSyntax;
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (c != '\n') {
			if (!mdr_parseTable(pStream, &state)) {
				state.hps_blockLevel = 0;
				mdr_parseFlow(pStream, &state);
			}
			pLastPart = pLastPart ? pLastPart->rvp_next : state.hps_part;
			if (pLastPart) {
				pLastPart->rvp_margins.m_top += nDelta;
				nDelta = 0;
			}
			pLastPart = state.hps_part;
			mdr_endPart(&state);
		}
		else {
			nDelta += PARAGRAPH_OFFSET / 2;
			pStream->is_skip(pStream, 1);
		}
	}
	mdr_finalizeParserState(pStream, &state);
	pStream->is_destroy(pStream);
}

/*
 * Returns the n-th view part.
 */
static RENDER_VIEW_PART* mdr_getViewPartAt(RENDER_VIEW_PART* pFirstPart, long n) {
	return (RENDER_VIEW_PART * )ll_at((LINKED_LIST*)pFirstPart, n);
}

/*
 * Returns the view part corresponding with the passed line pointer.
 */
static RENDER_VIEW_PART* mdr_getViewPartForLine(RENDER_VIEW_PART* pFirstPart, const LINE* lp, int* pIndex) {
	if (!pFirstPart) {
		return 0;
	}
	RENDER_VIEW_PART* pNext;
	LINE* lpPart = pFirstPart->rvp_lpStart;
	*pIndex = 0;
	while (lpPart) {
		if (lpPart == lp) {
			return pFirstPart;
		}
		lpPart = lpPart->next;
		pNext = pFirstPart->rvp_next;
		while (pNext && (lpPart == pNext->rvp_lpStart || pNext->rvp_lpStart == pFirstPart->rvp_lpStart)) {
			pFirstPart = pNext;
			(*pIndex)++;
			pNext = pNext->rvp_next;
		}
	}
	return 0;
}

static int mdr_supportsMode(EDIT_MODE aMode) {
	int flag = aMode.em_flag;
	if (!aMode.em_displayMode) {
		return FALSE;
	}
	return (flag & (SHOW_LINENUMBERS | SHOW_RULER | SHOW_CONTROL_CHARS | SHOW_SYNTAX_HIGHLIGHT)) == 0;
}

/*
 * Returns the total extent in pixels of the current layout described by the list
 * of view parts. If nUpToPart is greater or equals to 0, we will get the extend
 * up to not including the part with the given index. If -1 is passed we will get
 * the extent of all viewparts.
 */
static void mdr_getViewpartsExtend(MARKDOWN_RENDERER_DATA* pData, SIZE* pSize, RENDER_VIEW_PART* pUpToPart) {
	if (!pData) {
		return;
	}
	pSize->cx = 10;
	pSize->cy = 0;
	int nIndex = 0;
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	while (pPart) {
		if (pPart == pUpToPart) {
			break;
		}
		if (pPart->rvp_layouted) {
			// for now - not necessary to calculate the width
			int nWidth = 100;
			if (nWidth > pSize->cy) {
				pSize->cx = nWidth;
			}
			pSize->cy += pPart->rvp_height;
		} else {
			EdTRACE(log_message(DEBUG_INFO, "mdr_getViewPartsExtend - part not layouted."));
		}
		pPart = pPart->rvp_next;
		nIndex++;
	}
}

static int mdr_slSize(HWND hwnd, MARKDOWN_RENDERER_DATA* pData) {
	SIZE size;
	mdr_getViewpartsExtend(pData, &size, NULL);
	if (size.cy == 0) {
		return 0;
	}
	SCROLLINFO info = {
		.nMin = 0,
		.cbSize = sizeof info,
		.fMask = SIF_RANGE | SIF_PAGE | SIF_POS
	};
	GetScrollInfo(hwnd, SB_VERT, &info);
	ShowScrollBar(hwnd, SB_HORZ, FALSE);
	info.nMax = size.cy;
	RECT rect;
	GetClientRect(hwnd, &rect);
	info.nPage = rect.bottom - rect.top;
	SetScrollInfo(hwnd, SB_VERT, &info, 1);
	return 1;
}

/*
 * Invoked, when the size of the window has changed - update scrollbars to display visible
 * area etc...
 */
static int mdr_windowSizeChanged(HWND hwnd, MARKDOWN_RENDERER_DATA* pData) {
	if (!pData) {
		return 0;
	}
	if (!pData->md_needsSBUdpdate) {
		mdr_invalidateViewpartsLayout(pData);
		InvalidateRect(hwnd, 0, FALSE);
		UpdateWindow(hwnd);
		pData->md_needsSBUdpdate = TRUE;
	}
	return TRUE;
}

/*
 * Returns the view part for an y pixel position assuming all view parts are laid
 * out on one page.
 */
static int mdr_getViewpartAtY(RENDER_VIEW_PART* pFirst, int nY) {
	int nIndex = 0;
	while (pFirst) {
		if (pFirst->rvp_layouted) {
			nY -= pFirst->rvp_height;
			if (nY < 0) {
				return nIndex;
			}
		}
		pFirst = pFirst->rvp_next;
		nIndex++;
	}
	return -1;
}

static void mdr_updatePartBounds(RENDER_VIEW_PART* pPart, int nDelta) {
	while (pPart) {
		pPart->rvp_bounds.top += nDelta;
		pPart->rvp_bounds.bottom += nDelta;
		pPart->rvp_occupiedBounds.top += nDelta;
		pPart->rvp_occupiedBounds.bottom += nDelta;
		pPart = pPart->rvp_next;
	}
}
/*
 * The vertical scrollbar was dragged - update the scrollbar and scroll the window contents.
 */
void mdr_scrolled(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, WPARAM wParam, BOOL bScrollChanged) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	SIZE size;
	mdr_getViewpartsExtend(pData, &size, NULL);
	SCROLLINFO info = {
		.cbSize = sizeof info,
		.fMask = SIF_RANGE | SIF_PAGE | SIF_POS
	};
	GetScrollInfo(hwnd, SB_VERT, &info);
	info.nMin = 0;
	info.nPage = rect.bottom;
	info.nMax = size.cy;
	int nOldPos = info.nPos;
	if (bScrollChanged) {
		int nCode = LOWORD(wParam);
		if (nCode == SB_LINEUP) {
			info.nPos--;
		}
		else if (nCode == SB_LINEDOWN) {
			info.nPos++;
		}
		else if (nCode == SB_PAGEDOWN) {
			info.nPos += rect.bottom;
		}
		else if (nCode == SB_PAGEUP) {
			info.nPos -= rect.bottom;
		}
		else if (nCode == SB_TOP) {
			info.nPos = 0;
		}
		else if (nCode == SB_BOTTOM) {
			info.nPos = rect.bottom - size.cy;
		}
		else if (nCode == SB_ENDSCROLL) {
			return;
		}
		else {
			info.nPos = HIWORD(wParam);
		}
		if (info.nPos < 0) {
			info.nPos = 0;
		}
	}
	SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
	if (bScrollChanged) {
		int nDelta = nOldPos - info.nPos;
		RECT r;
		GetClientRect(hwnd, &r);
		ScrollWindow(hwnd, 0, nDelta, &r, (LPRECT)0);
		// Fixup bounds of parts and runs to accomodate for scrolling by a delta
		mdr_updatePartBounds(pData->md_pElements, nDelta);
		UpdateWindow(hwnd);
	}
}

/*
 * Calculate the number of logical lines displayed
 */
static long mdr_calculateMaxLine(WINFO* wp) {
	FTABLE* fp = FTPOI(wp);
	return fp->nlines;
}

static int mdr_adjustScrollBoundsOffset(WINFO* wp, BOOL bMiddleOfScreen) {
	SIZE size;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	SCROLLINFO info = {
		.cbSize = sizeof info,
		.fMask = SIF_POS
	};
	GetScrollInfo(wp->ww_handle, SB_VERT, &info);
	RECT rect;
	GetClientRect(wp->ww_handle, &rect);
	RENDER_VIEW_PART* pCaret = pData->md_caretView;
	mdr_getViewpartsExtend(pData, &size, pCaret);
	int nNewY;
	int nScreenHeight = rect.bottom - rect.top;
	int nUse = pCaret ? pCaret->rvp_height : 10;
	if (nUse > nScreenHeight) {
		nUse = nScreenHeight;
	}
	int nMaxY = info.nPos + nScreenHeight - nUse;
	int s2 = nScreenHeight / 2;
	if (bMiddleOfScreen) {
		nMaxY -= 2;
	}
	if (size.cy < info.nPos) {
		nNewY = size.cy;
	} else if (size.cy > nMaxY) {
		nNewY = bMiddleOfScreen ? size.cy - s2 : size.cy - nScreenHeight + nUse;
		SIZE sizeTotal;
		mdr_getViewpartsExtend(pData, &sizeTotal, NULL);
		if (nNewY + nScreenHeight >= sizeTotal.cy) {
			nNewY = sizeTotal.cy-nScreenHeight;
		}
	} else {
		return 0;
	}
	wp->maxcursln = mdr_getViewpartAtY(pData->md_pElements, nNewY);
	wp->mincursln = mdr_getViewpartAtY(pData->md_pElements, nNewY-nScreenHeight);
	if (pData->md_hwndTooltip) {
		ShowWindow(pData->md_hwndTooltip, SW_HIDE);
	}
	mdr_scrolled(wp->ww_handle, pData, MAKELPARAM(SB_THUMBPOSITION, nNewY), TRUE);
	return 1;
}

static int mdr_adjustScrollBounds(WINFO* wp) {
	return mdr_adjustScrollBoundsOffset(wp, FALSE);
}

/*
 * Invalidate the layout of all view parts starting with 'pData'.
 */
void mdr_invalidateViewpartsLayout(MARKDOWN_RENDERER_DATA* pData) {
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pFirst = pData->md_pElements;
	while (pFirst) {
		// clear out old existing bounds.
		memset(&pFirst->rvp_bounds, 0, sizeof pFirst->rvp_bounds);
		pFirst->rvp_layouted = 0;
		pFirst->rvp_height = 0;
		pFirst = pFirst->rvp_next;
	}
}

/*
 * Visitor function visiting all text runs traversable from a view part and executing a callback function on each list
 * of text runs with a custom parameter. If the callback returns 0, visiting stops.
 */
static int mdr_forRunListDo(RENDER_VIEW_PART* pPart, int (*runCallback)(TEXT_RUN* pRunList, void* pParam), void* pParam) {
	if (pPart->rvp_paint == mdr_renderMarkdownBlockPart) {
		TEXT_RUN* pRun = pPart->rvp_data.rvp_flow.tf_runs;
		return runCallback(pRun, pParam);
	}
	else if (pPart->rvp_paint == mdr_renderTable) {
		RENDER_TABLE_ROW* pRows = pPart->rvp_data.rvp_table->rt_rows;
		while (pRows) {
			RENDER_TABLE_CELL* pCell = pRows->rtr_cells;
			while (pCell) {
				if (!runCallback(pCell->rtc_flow.tf_runs, pParam)) {
					return 0;
				}
				pCell = pCell->rtc_next;
			}
			pRows = pRows->rtr_next;
		}
	}
	return 1;
}

/*
 * Can be used to ensure, that the internal view model is properly layouted and
 * all sizes of the viewparts are correctly calculated.
 */
static void mdr_layout(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	if (pPart == NULL) {
		return;
	}
	float zoomFactor = wp->zoomFactor;
	HWND hwnd = wp->edwin_handle;
	HDC hdc = GetWindowDC(hwnd);
	RENDER_FLOW_PARAMS rfp = {
		.rfp_focus = GetFocus() == hwnd,
		.rfp_hdc = hdc,
		.rfp_measureOnly = TRUE,
		.rfp_theme = theme_getCurrent(),
		.rfp_zoomFactor = zoomFactor
	};
	RECT rect;
	GetClientRect(hwnd, &rect);
	SCROLLINFO info = {
		.cbSize = sizeof info,
		.fMask = SIF_POS,
	};
	GetScrollInfo(wp->ww_handle, SB_VERT, &info);
	rect.top -= info.nPos;
	while (pPart) {
		if (!pPart->rvp_layouted) {
			rfp.rfp_part = pPart;
			rfp.rfp_skipSpace = pPart->rvp_type != MET_FENCED_CODE_BLOCK;
			pPart->rvp_paint(&rfp, &rect, &pPart->rvp_occupiedBounds);
			rect.top += pPart->rvp_height;
		}
		pPart = pPart->rvp_next;
	}
	ReleaseDC(hwnd, hdc);
}

static void mdr_renderAll(HWND hwnd, RENDER_CONTEXT* pRC, MARKDOWN_RENDERER_DATA* pData, RECT* pClip, int nTopY) {
	RECT rect;
	HDC hdc = pRC->rc_hdc;
	if (pRC->rc_printing) {
		// hack: when printing - printable area is passed here.
		rect = *pClip;
	} else {
		GetClientRect(hwnd, &rect);
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	HBRUSH hBrushBg = CreateSolidBrush(pRC->rc_theme->th_defaultBackgroundColor);
	FillRect(hdc, pClip, hBrushBg);
	DeleteObject(hBrushBg);
	rect.top -= nTopY;
	WINFO* wp = pRC->rc_wp;
	float zoomFactor = wp ? wp->zoomFactor : dpisupport_getScalingFactorX();
	RENDER_FLOW_PARAMS rfp = {
		.rfp_focus = GetFocus() == hwnd,
		.rfp_hdc = hdc,
		.rfp_theme = pRC->rc_theme,
		.rfp_zoomFactor = zoomFactor,
		.rfp_wp = wp,
		.rfp_printing = pRC->rc_printing
	};
	if (GetMapMode(hdc) == MM_ANISOTROPIC) {
		LONG ext = win_getWindowExtension(hdc);
		// 60 lines of text with an average of 12 pixels height
		rfp.rfp_zoomFactor = (float)HIWORD(ext) / (60 * 12);
	}
	BOOL bHighlight = wp == NULL ? 0 : wp->dispmode & SHOW_CARET_LINE_HIGHLIGHT;
	RECT occupiedBounds = {
		.top = 0
	};
	for (; pPart; rect.top = occupiedBounds.bottom) {
		int nBottom = rect.top + pPart->rvp_height;
		if (pPart->rvp_layouted && (nBottom < pClip->top || rect.top>pClip->bottom)) {
			occupiedBounds.bottom = nBottom;
		}
		else {
			rfp.rfp_part = pPart;
			rfp.rfp_skipSpace = pPart->rvp_type != MET_FENCED_CODE_BLOCK;
			rfp.rfp_isCaret = bHighlight && pData->md_caretView == pPart;
			if (rfp.rfp_isCaret || (!pPart->rvp_layouted && pPart->rvp_decoration)) {
				// need extra layout before paint.
				rfp.rfp_measureOnly = TRUE;
				pPart->rvp_paint(&rfp, &rect, &pPart->rvp_occupiedBounds);
			}
			rfp.rfp_measureOnly = rect.top > pClip->bottom;
			if (rfp.rfp_isCaret && !rfp.rfp_measureOnly) {
				mdr_highlightCaretLine(&rfp, pPart);
			}
			pPart->rvp_paint(&rfp, &rect, &pPart->rvp_occupiedBounds);
			occupiedBounds = pPart->rvp_occupiedBounds;
		}
		pPart = pPart->rvp_next;
	}
}

/*
 * Render the current window displaying MARKDOWN/HTML wysiwyg contents.
 */
static void mdr_renderPage(RENDER_CONTEXT* pCtx, void (*parsePage)(WINFO* wp), RECT* pClip, HBRUSH hBrushBg, int y) {
	WINFO* wp = pCtx->rc_wp;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData->md_pElements) {
		parsePage(wp);
		int nMDCaretLine;
		pData->md_caretView = mdr_getViewPartForLine(pData->md_pElements, wp->caret.linePointer, &nMDCaretLine);
	} else if (pData->md_zoomFactor != wp->zoomFactor) {
		if (pData->md_zoomFactor != 0.0) {
			mdr_invalidateViewpartsLayout(pData);
		}
		pData->md_zoomFactor = wp->zoomFactor;
	}
	if (!pCtx->rc_printing) {
		if (pData->md_printing) {
			mdr_invalidateViewpartsLayout(pData);
			pData->md_printing = FALSE;
		}
		SCROLLINFO info = {
			.cbSize = sizeof info,
			.fMask = SIF_POS,
		};
		GetScrollInfo(wp->ww_handle, SB_VERT, &info);
		y = info.nPos;
	}
	mdr_renderAll(wp->ww_handle, pCtx, pData, pClip, y);
}

/*
 * Render the current window displaying MARKDOWN wysiwyg contents.
 */
static void mdr_renderMarkdownFormatPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	mdr_renderPage(pCtx, mdr_parseMarkdownFormat, pClip, hBrushBg, y);
}

/*
 * Render the current window displaying HTML wysiwyg contents.
 */
static void mdr_renderHTMLFormatPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	mdr_renderPage(pCtx, mdr_parseFileToHTML, pClip, hBrushBg, y);
}

/*
 * Render the current window displaying AsciiDoc wysiwyg contents.
 */
static void mdr_renderAdocFormatPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	mdr_renderPage(pCtx, mdr_parseAdocFormat, pClip, hBrushBg, y);
}

/*
 * Implements rich text rendering printing.
 */
static PRINT_FRAGMENT_RESULT mdr_printFragment(RENDER_CONTEXT* pRC, PRINT_LINE* pPrintLine, DEVEXTENTS* pExtents) {
	WINFO* wp = pPrintLine->wp;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return PFR_END;
	}
	if (!pData->md_printing) {
		mdr_invalidateViewpartsLayout(pData);
		pData->md_printing = TRUE;
	}
	RECT rect = {
		.left = pExtents->xLMargin,
		.right = pExtents->xRMargin,
		.top = pExtents->yTop,
		.bottom = pExtents->yBottom
	};
	int nYBottom = (int)pPrintLine->yOffset + rect.bottom - rect.top;
	int nBottomIdx = mdr_getViewpartAtY(pData->md_pElements, nYBottom);
	if (nBottomIdx >= 0) {
		RENDER_VIEW_PART* pBottom = mdr_getViewPartAt(pData->md_pElements, nBottomIdx);
		RENDER_VIEW_PART* pPart = pData->md_pElements;
		int nY = 0;
		// Try to avoid clipped lines during Wysiwyg printing.
		// This will currently only work for view parts, which are not very big.
		while (pPart) {
			if (pPart == pBottom) {
				break;
			}
			if (pPart->rvp_next == pBottom) {
				if (nY > nYBottom - 500) {
					rect.bottom = nY- pPrintLine->yOffset+rect.top;
					break;
				}
			}
			nY += pPart->rvp_height;
			pPart = pPart->rvp_next;
		}
	}
	int nOldDC = SaveDC(pRC->rc_hdc);
	HBRUSH hBrush = CreateSolidBrush(pRC->rc_theme->th_defaultBackgroundColor);
	IntersectClipRect(pRC->rc_hdc, rect.left, rect.top, rect.right, rect.bottom);
	wp->renderer->r_renderPage(pRC, &rect, hBrush, pPrintLine->yOffset);
	DeleteObject(hBrush);
	RestoreDC(pRC->rc_hdc, nOldDC);
	int nTotal = (int)pPrintLine->yOffset + (rect.bottom - rect.top);
	pPrintLine->yOffset = nTotal;
	SIZE size;
	mdr_getViewpartsExtend(pData, &size, NULL);
	if (nTotal < size.cy) {
		pPrintLine->pagenumber++;
	}
	pPrintLine->yPos = 0;
	return nTotal >= size.cy ? PFR_END : PFR_END_PAGE;
}
/*
 * Render some render view parts as a response to a WM_PAINT message into the window given with 'hwnd',
 * using the repaint areas and device context defined in 'ps' assuming the screen top is scrolled to
 * logical position 'nTopY' render the linked list of view parts starting with 'pData'.
 */
void mdr_renderMarkdownData(HWND hwnd, PAINTSTRUCT* ps, int nTopY, MARKDOWN_RENDERER_DATA* pData) {
	HDC hdc = ps->hdc;
	RECT* pClip = &ps->rcPaint;
	RENDER_CONTEXT rc = {
		.rc_hdc = hdc,
		.rc_theme = theme_getCurrent(),
		.rc_printing = FALSE
	};
	mdr_renderAll(hwnd, &rc, pData, pClip, nTopY);
}

/*
 * Destroy a text run possibly release the link. 
 */
static int mdr_destroyRun(TEXT_RUN* pRun) {
	free(pRun->tr_link);
	free(pRun->tr_imageUrl);
	free(pRun->tr_title);
	free(pRun->tr_anchor);
	MD_IMAGE* pImg = pRun->tr_data.tr_image;
	if (pImg && !pRun->tr_isEmoji) {
		mdr_releaseBitmapHandle(pImg);
		free(pImg);
	}
	return 1;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	if (pRVP->rvp_type == MET_TABLE) {
		mdr_destroyTable(pRVP);
	} else {
		free(pRVP->rvp_data.rvp_flow.tf_text);
		ll_destroy(&pRVP->rvp_data.rvp_flow.tf_runs, mdr_destroyRun);
	}
	if (pRVP->rvp_param.rvp_grammar != 0) {
		free(pRVP->rvp_param.rvp_grammar);
	}
	free(pRVP->rvp_decoration);
	return 1;
}

static int mdr_destroyReferenceDefinition(MD_REFERENCE_DEFINITION* pLink) {
	free(pLink->mdrs_name);
	free(pLink->mdrs_text);
	free(pLink->mdrs_title);
	return 1;
}

/*
 * Free our cashed images.
 */
static int mdr_destroyUrlAndBmp(intptr_t key, intptr_t val) {
	free((void*) key);
	DeleteObject((HBITMAP)val);
	return 1;
}

/*
 * Destroy a list of view parts releasing unneeded memory.
 */
void mdr_destroyRendererData(MARKDOWN_RENDERER_DATA* pData) {
	if (pData->md_hwndTooltip) {
		DestroyWindow(pData->md_hwndTooltip);
	}
	ll_destroy(&pData->md_pElements, mdr_destroyViewPart);
	ll_destroy(&pData->md_referenceDefinitions, mdr_destroyReferenceDefinition);
	hashmap_destroy(pData->md_imageCache, mdr_destroyUrlAndBmp);
	free(pData);
}

/*
 * Destroy the renderer data. 
 */
static void mdr_destroyData(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		mdr_destroyRendererData(pData);
		wp->r_data = NULL;
	}
}

/*
 * Destroy the cached view parts for now. This should be optimized on a long term run.
 */
static void mdr_modelChanged(WINFO* wp, MODEL_CHANGE* pChanged) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData && pData->md_pElements) {
		if (pData->md_imageCache == NULL) {
			pData->md_imageCache = hashmap_create(17, 0, 0);
		}
		RECT updateRect = { 0 };
		if (pChanged->type == LINE_MODIFIED && pChanged->lp) {
			// Try to optimize repaint a little bit if only a line is changed.
			int unused;
			RENDER_VIEW_PART* pModifiedPart = mdr_getViewPartForLine(
				pData->md_caretView != NULL ? pData->md_caretView : pData->md_pElements, pChanged->lp, &unused);
			if (pModifiedPart != NULL) {
				updateRect = pModifiedPart->rvp_occupiedBounds;
			}
		}
		for (RENDER_VIEW_PART* pPart = pData->md_pElements; pPart != 0; pPart = pPart->rvp_next) {
			if (pPart->rvp_type == MET_TABLE) {
				continue;
			}
			for (TEXT_RUN* pRun = pPart->rvp_data.rvp_flow.tf_runs; pRun != 0; pRun = pRun->tr_next) {
				if (pRun->tr_imageUrl != NULL && pRun->tr_data.tr_image && !pRun->tr_isEmoji) {
					HASH_ENTRY entry;
					if (!hashmap_getEntry(pData->md_imageCache, (intptr_t) pRun->tr_imageUrl, &entry)) {
						hashmap_put(pData->md_imageCache, pRun->tr_imageUrl, (intptr_t)pRun->tr_data.tr_image->mdi_image);
						pRun->tr_imageUrl = NULL;
						pRun->tr_data.tr_image->mdi_image = NULL;
						pRun->tr_data.tr_image->mdi_cached = FALSE;
					}
				}
			}
		}
		ll_destroy(&pData->md_pElements, mdr_destroyViewPart);
		ll_destroy(&pData->md_referenceDefinitions, mdr_destroyReferenceDefinition);
		pData->md_pElements = NULL;
		pData->md_caretView = NULL;
		pData->md_focussedPart = NULL;
		pData->md_focussedRun = NULL;
		pData->md_previousCaretView = NULL;
		InvalidateRect(wp->ww_handle, updateRect.right != updateRect.left ? &updateRect : (LPRECT)0, 0);
		// do not: UpdateWindow(wp->ww_handle); at this point the model may not be completely edited - further editing operations
		// may follow - so invalidate only and paint when really done.
	}
}

/*
 * Allocate the markdown renderer data structure.
 */
static void* mdr_allocData(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pResult = calloc(1, sizeof(MARKDOWN_RENDERER_DATA));
	if (pResult == NULL) {
		return NULL;
	}
	pResult->md_hwndTooltip = cust_createToolTooltip(wp->ww_handle);
	return pResult;
}

/*
 * Calculate the "longest" line displayed. We do not support horizontal scrolling.
 * Calculate the longes line to be rather small
 */
static long mdr_calculateLongestLine(WINFO* wp) {
	return 200000;
}

/*
 * Calculate the maximum column for column moves. 
 */
static long mdr_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return 200000;
}

/*
 * Hex offset printing not implemented.
 */
static int mdr_screenOffsetToBuffer(WINFO* wp, INTERNAL_BUFFER_POS* pPosition) {
	// not implemented
	return 0;
}

static void mdr_scroll(WINFO* wp, int dx, int dy) {
	if (!dy) {
		return;
	}
	RedrawWindow(wp->ww_handle, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
}

static int mdr_placeCaret(WINFO* wp, long* ln, long offset, long* col, int updateVirtualOffset, CARET_MOVEMENT_SPEC* xDelta) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	if (!pData->md_pElements && pData->md_parse) {
		pData->md_parse(wp);
		mdr_layout(wp);
		long nMDIndex = 0;
		FTABLE* fp = FTPOI(wp);
		wp->caret.linePointer = ln_goto(fp, *ln);
		pData->md_caretPartIndex = 0;
	}
	int dl = xDelta == NULL ? 0 : xDelta->cms_deltaY;
	RENDER_VIEW_PART* pPart = pData->md_caretView;
	if (dl < 0 && pData->md_caretPartIndex == 0) {
		return 0;
	}
	if (pPart) {
		if (dl == 1 && pPart->rvp_next) {
			dl = ln_cnt(pPart->rvp_lpStart, pPart->rvp_next->rvp_lpStart) - 1;
			if (dl == 0) {
				dl = 1;
			}
			*ln = wp->caret.ln + dl;
		}
		else if (dl == -1 && pData->md_caretPartIndex > 0) {
			RENDER_VIEW_PART* pPartPrevious = mdr_getViewPartAt(pData->md_pElements, pData->md_caretPartIndex - 1);
			dl = ln_cnt(pPartPrevious->rvp_lpStart, pPart->rvp_lpStart) - 1;
			if (dl == 0) {
				dl = 1;
			}
			*ln = wp->caret.ln - dl;
		}
	}
	if (!caret_placeCursorAndValidate(wp, ln, offset, col, updateVirtualOffset, xDelta)) {
		return 0;
	}
	int nMDCaretLine;
	pPart = mdr_getViewPartForLine(pData->md_pElements, wp->caret.linePointer, &nMDCaretLine);
	if (pData->md_caretView != pPart) {
		BOOL bHighlight = (wp->dispmode & SHOW_CARET_LINE_HIGHLIGHT);
		if (bHighlight && pData->md_caretView && pData->md_caretView->rvp_layouted) {
			InvalidateRect(wp->ww_handle, &pData->md_caretView->rvp_occupiedBounds, FALSE);
		}
		pData->md_caretView = pPart;
		if (bHighlight && pData->md_caretView && pData->md_caretView->rvp_layouted) {
			InvalidateRect(wp->ww_handle, &pData->md_caretView->rvp_occupiedBounds, FALSE);
		}
	}
	if (pPart) {
		if (xDelta) {
			if (pData->md_caretPartIndex != nMDCaretLine) {
				pData->md_caretPartIndex = nMDCaretLine;
				pData->md_caretRunIndex = 0;
			}
			else {
				pData->md_caretRunIndex = mdr_getNextLinkRunOffset(pPart, 0, *col);
			}
		}
		else {
			pData->md_caretRunIndex = *col;
		}
		if (pData->md_caretRunIndex < 0) {
			pData->md_caretRunIndex = 0;
		}
	}
	return 1;
}

static RUN_BOUNDS mdr_getRunBounds(RENDER_VIEW_PART* pPart, TEXT_RUN* pRun) {
	int offsetY = pPart->rvp_bounds.top;
	int offsetX = pPart->rvp_bounds.left;
	RUN_BOUNDS rb = pRun->tr_bounds;
	rb.bottom += offsetY;
	rb.bottom2 += offsetY;
	rb.top += offsetY;
	rb.top1 += offsetY;
	rb.left += offsetX;
	rb.left1 += offsetX;
	rb.right += offsetX;
	rb.right2 += offsetX;
	return rb;
}

static BOOL mdr_hitTestTextRuns(TEXT_RUN* pRuns, RENDER_VIEW_PART* pPart, POINT pPoint, long* pCol, TEXT_RUN** pMatchedRun) {
	long nCol = 0;
	while (pRuns) {
		RUN_BOUNDS rb = mdr_getRunBounds(pPart, pRuns);
		if (runbounds_contains(&rb, pPoint)) {
			*pCol = nCol;
			*pMatchedRun = pRuns;
			return TRUE;
		}
		pRuns = pRuns->tr_next;
		nCol++;
	}
	return FALSE;
}

typedef struct tagHIT_TEST_RUNLIST {
	long htr_currentColumn;
	long htr_resultColumn;
	int  htr_cx;
	int  htr_cy;
	RENDER_VIEW_PART* htr_part;
	TEXT_RUN* htr_matchedRun;
} HIT_TEST_RUNLIST;

static int mdr_hitTestRunList(TEXT_RUN* pRuns, HIT_TEST_RUNLIST* pParam) {
	long intraCol;
	pParam->htr_matchedRun = 0;
	if (mdr_hitTestTextRuns(pRuns, pParam->htr_part, (POINT) { pParam->htr_cx, pParam->htr_cy }, & intraCol, & pParam->htr_matchedRun)) {
		pParam->htr_resultColumn = intraCol + pParam->htr_currentColumn;
		return FALSE;
	}
	pParam->htr_currentColumn += ll_size((LINKED_LIST*)pRuns);
	return TRUE;
}

static BOOL mdr_hitTestInternal(MARKDOWN_RENDERER_DATA* pData, int cx, int cy, long* pLine, long* pCol, 
			RENDER_VIEW_PART** pMatchedPart, TEXT_RUN **pMatchedRun) {
	if (!pData) {
		return FALSE;
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	long ln = 0;
	POINT pt = { cx, cy };
	HIT_TEST_RUNLIST param = {
		.htr_cx = cx,
		.htr_cy = cy,
	};
	while (pPart) {
		// TODO: optimize
		if (PtInRect(&pPart->rvp_bounds, pt)) {
			*pLine = ln; 
			param.htr_currentColumn = 0;
			param.htr_matchedRun = 0;
			param.htr_part = pPart;
			mdr_forRunListDo(pPart, mdr_hitTestRunList, &param);
			if (param.htr_matchedRun) {
				*pMatchedRun = param.htr_matchedRun;
				*pMatchedPart = pPart;
				*pLine = ln;
				*pCol = param.htr_resultColumn;
				return TRUE;
			}
			return FALSE;
		}
		pPart = pPart->rvp_next;
		ln++;
	}
	return FALSE;
}

char* mdr_resolveLink(MARKDOWN_RENDERER_DATA* pData, TEXT_RUN* pRun) {
	if (pRun->tr_link == NULL) {
		return NULL;
	}
	if (!pRun->tr_isRefLink) {
		return pRun->tr_link;
	}
	MD_REFERENCE_DEFINITION* pLink = pData->md_referenceDefinitions;
	while (pLink != 0) {
		if (pLink->mdrs_type == MDDRT_REFERENCE_STYLE_LINK && strcmp(pRun->tr_link, pLink->mdrs_name) == 0) {
			return pLink->mdrs_text;
		}
		pLink = pLink->mdrsl_next;
	}
	return NULL;
}

/*
 * Can be used to find out, whether a link was clicked in a rich text renderer component given
 * the renderer data hook and the mouse x and y position.
 */
char* mdr_linkClicked(MARKDOWN_RENDERER_DATA* pData, int cxMouse, int cyMouse) {
	RENDER_VIEW_PART* pPart;
	TEXT_RUN* pRun;
	long ln;
	long col;
	if (mdr_hitTestInternal(pData, cxMouse, cyMouse, &ln, &col, &pPart, &pRun)) {
		return mdr_resolveLink(pData, pRun);
	}
	return 0;
}

static BOOL mdr_hasLink(TEXT_RUN* pRun) {
	return pRun->tr_link != 0;
}

static void mdr_hitTest(WINFO* wp, int cx, int cy, long* pLine, long* pCol) {
	RENDER_VIEW_PART* pMatchP;
	TEXT_RUN* pMatchR;
	mdr_hitTestInternal(wp->r_data, cx, cy, pLine, pCol, &pMatchP, &pMatchR);
}

static void mdr_setRollover(HWND hwnd, RENDER_VIEW_PART* pPart, TEXT_RUN* pRun, BOOL aFlag) {
	if (!pRun || !pPart || pRun->tr_attributes.rollover == aFlag || !mdr_hasLink(pRun)) {
		return;
	}
	pRun->tr_attributes.rollover = aFlag;
	RUN_BOUNDS rb = mdr_getRunBounds(pPart, pRun);
	InvalidateRect(hwnd, (RECT*) &rb, FALSE);
}

static void mdr_setFocussed(HWND hwnd, RENDER_VIEW_PART* pPart, TEXT_RUN* pRun, BOOL aFlag) {
	if (!pRun || !pPart || pRun->tr_attributes.focussed == aFlag || !mdr_hasLink(pRun)) {
		return;
	}
	pRun->tr_attributes.focussed = aFlag;
	RUN_BOUNDS rb = mdr_getRunBounds(pPart, pRun);
	InvalidateRect(hwnd, (RECT*)&rb, FALSE);
	UpdateWindow(hwnd);
}

/*
 * Generic mouse move handler for markdown rendering (supporting rollover effects etc...).
 */
void mdr_mouseMove(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, int x, int y) {
	RENDER_VIEW_PART* pMatchP;
	TEXT_RUN* pMatchR = NULL;
	long line;
	long col;
	BOOL bShow = TRUE;
	if (!mdr_hitTestInternal(pData, x, y, &line, &col, &pMatchP, &pMatchR)) {
		bShow = FALSE;
	}
	if (pMatchR == pData->md_focussedRun) {
		return;
	}
	mdr_setRollover(hwnd, pData->md_focussedPart, pData->md_focussedRun, FALSE);
	pData->md_focussedRun = pMatchR;
	pData->md_focussedPart = pMatchP;
	mdr_setRollover(hwnd, pMatchP, pMatchR, TRUE);
	TTTOOLINFO toolinfo = { 0 };
	toolinfo.cbSize = sizeof(toolinfo);
	toolinfo.hwnd = hwnd;
	toolinfo.hinst = hInst;
	if (pMatchR && pMatchR->tr_title) {
		// Activate the tooltip.
		toolinfo.lpszText = pMatchR->tr_title;
		SendMessage(pData->md_hwndTooltip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&toolinfo);
		RUN_BOUNDS rb = mdr_getRunBounds(pData->md_caretView != NULL ? pData->md_caretView : pMatchP, pMatchR);
		POINT pt = {
			.x = rb.left1,
			.y = y - 20
		};
		ClientToScreen(hwnd, &pt);
		SendMessage(pData->md_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x, pt.y));
	} else {
		bShow = FALSE;
	}
	if (pData->md_hwndTooltip && !SendMessage(pData->md_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)bShow, (LPARAM)&toolinfo)) {
		log_message(DEBUG_ERR, "Activating tooltip failed. Error %ld.", GetLastError());
	}
}

/*
 * Returns the renderer data for the given window. 
 */
static MARKDOWN_RENDERER_DATA* mdr_dataFromWindow(HWND hwnd) {
	WINFO* wp = ww_winfoFromWorkwinHandle(hwnd);
	return wp ? wp->r_data : 0;
}

/*
 * Custom window procedure used in markdown renderers.
 */
static LRESULT mdr_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	MARKDOWN_RENDERER_DATA* pData;

	switch (message) {
	case WM_MOUSEMOVE:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			mdr_mouseMove(hwnd, pData, xPos, yPos);
		}
		break;
	case WM_THEMECHANGED:
		_defaultFont.lfFaceName[0] = 0;
		break;
	case WM_MOUSEWHEEL:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			mdr_scrolled(hwnd, pData, zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, TRUE);
		}
		return 0;
	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0 && pData->md_caretRun && pData->md_caretView) {
			RUN_BOUNDS rb = mdr_getRunBounds(pData->md_caretView, pData->md_caretRun);
			InvalidateRect(hwnd, (RECT*) & rb, 0);
		}
		break;
	case WM_VSCROLL:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			mdr_scrolled(hwnd, pData, wParam, TRUE);
		}
		return 0;
	case WM_SHOWWINDOW:
		if (!wParam) {
			return 0;
		}
	case WM_SIZE:
		if (message == WM_SIZE && !IsWindowVisible(hwnd)) {
			return 0;
		}
		pData = mdr_dataFromWindow(hwnd);
		if (pData != 0) {
			mdr_windowSizeChanged(hwnd, pData);
		}
		return 0;
	}
	LRESULT result = render_defaultWindowProc(hwnd, message, wParam, lParam);
	if (message == WM_PAINT && (pData = mdr_dataFromWindow(hwnd)) != 0 && pData->md_needsSBUdpdate) {
		mdr_slSize(hwnd, pData);
		pData->md_needsSBUdpdate = FALSE;
	}
	return result;
}

typedef struct tag_RUN_OFFSET {
	int ro_inputOffset;
	int ro_currentOffset;
	TEXT_RUN* ro_match;
} RUN_OFFSET;

static int mdr_getRunFromRunsAtOffset(TEXT_RUN* pRuns, RUN_OFFSET* pOffsets) {
	int nCount = ll_size((LINKED_LIST*)pRuns);
	pOffsets->ro_currentOffset += nCount;
	if (pOffsets->ro_currentOffset > pOffsets->ro_inputOffset) {
		pOffsets->ro_match = (TEXT_RUN*)ll_at((LINKED_LIST*)pRuns, pOffsets->ro_inputOffset - pOffsets->ro_currentOffset + nCount);
		return 0;
	}
	return 1;
}

static TEXT_RUN* mdr_getRunAtOffset(RENDER_VIEW_PART* pPart, int nOffset) {
	RUN_OFFSET pParam = (RUN_OFFSET){ .ro_inputOffset = nOffset, .ro_currentOffset = 0, .ro_match = 0 };
	mdr_forRunListDo(pPart, mdr_getRunFromRunsAtOffset, &pParam);
	return pParam.ro_match;
}

static int mdr_findNextLinkRun(TEXT_RUN* pRuns, RUN_OFFSET* pOffsets) {
	while (pRuns) {
		if (mdr_hasLink(pRuns)) {
			pOffsets->ro_match = pRuns;
		}
		if (pOffsets->ro_currentOffset >= pOffsets->ro_inputOffset && pOffsets->ro_match) {
			return 0;
		}
		pOffsets->ro_currentOffset++;
		pRuns = pRuns->tr_next;
	}
	return 1;
}

static int mdr_getNextLinkRunOffset(RENDER_VIEW_PART* pPart, int nStart, int nDelta) {
	RUN_OFFSET pParam = (RUN_OFFSET){ .ro_inputOffset = nStart+nDelta, .ro_currentOffset = 0};
	mdr_forRunListDo(pPart, mdr_findNextLinkRun, &pParam);
	return pParam.ro_match ? pParam.ro_currentOffset : -1;
}

static void mdr_updateCaretUI(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	RENDER_VIEW_PART* pPart = pData->md_caretView;
	*pCX = DEFAULT_LEFT_MARGIN;
	if (pPart) {
		TEXT_RUN* pRun = mdr_getRunAtOffset(pPart, pData->md_caretRunIndex);
		if (pRun) {
			RUN_BOUNDS rb = mdr_getRunBounds(pPart, pRun);
			*pCY = rb.top1;
			*pCX = rb.left1;
			*pHeight = rb.bottom - rb.top1;
		}
		else {
			*pCY = pPart->rvp_bounds.top;
			*pCX = pPart->rvp_bounds.left;
			*pHeight = pPart->rvp_bounds.bottom - pPart->rvp_bounds.top;
		}
		if (pRun != pData->md_caretRun) {
			if (pData->md_caretRun) {
				mdr_setFocussed(wp->ww_handle, pData->md_previousCaretView, pData->md_caretRun, FALSE);
			}
			pData->md_caretRun = pRun;
			if (pRun) {
				mdr_setFocussed(wp->ww_handle, pData->md_caretView, pRun, TRUE);
				pData->md_previousCaretView = pData->md_caretView;
			}
		}
		if (*pHeight <= 2) {
			*pHeight = 3;
		}
	}
	*pWidth = 2;
}

static BOOL mdr_findLink(WINFO* wp, char* pszBuf, size_t nMaxChars, NAVIGATION_INFO_PARSE_RESULT* pResult) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	*pszBuf = 0;
	if (!pData) {
		return FALSE;
	}
	RENDER_VIEW_PART* pPart = pData->md_caretView;
	if (pPart) {
		TEXT_RUN* pRun = mdr_getRunAtOffset(pPart, pData->md_caretRunIndex);
		if (!pRun || !mdr_hasLink(pRun)) {
			return FALSE;
		}
		char* pszLink = mdr_resolveLink(pData, pRun);
		if (pszLink && strlen(pszLink) < nMaxChars) {
			strcpy(pszBuf, pszLink);
			char* pszAnchor = strrchr(pszBuf, '#');
			if (pszAnchor) {
				*pszAnchor++ = 0;
				pResult->ni_anchor = pszAnchor;
			}
			pResult->ni_reference = pszBuf;
			pResult->ni_lineNumber = 0;
			pResult->ni_displayMode = wp->dispmode;	
			pResult->ni_wp = mainframe_getDockName(wp->edwin_handle);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Check, whether a text flow matches an anchor. 
 * Used to navigate to an anchor inside markdown documents.
 */
static int mdr_titleMatchesAnchor(TEXT_FLOW* pTitle, const char*pszTitleAnchor, const char* pszAnchor) {
	if (pszTitleAnchor && _stricmp(pszTitleAnchor, pszAnchor) == 0) {
		return 1;
	}
	if (!pTitle) {
		return 0;
	}
	char szTitleAsAnchor[512];
	char* pszTitle = pTitle->tf_text;
	if (strlen(pszTitle) < sizeof szTitleAsAnchor) {
		char* pszDest = szTitleAsAnchor;
		while (*pszTitle) {
			char c = *pszTitle++;
			if (c == ' ') {
				if (!*pszTitle) {
					break;
				}
				c = '-';
			}
			*pszDest++ = c;
		}
		*pszDest = 0;
		return (_stricmp(pszAnchor, szTitleAsAnchor) == 0);
	}
	return 0;
}

typedef struct tagANCHOR_MATCH {
	const char* am_matchAnchor;
	int         am_offsetIdx;
	int			am_matched;
} ANCHOR_MATCH;

/*
 * Match an anchor agains the anchor added to a run.
 */
static int mdr_matchAnchor(TEXT_RUN* pRun, ANCHOR_MATCH* pMatch) {
	if (pRun && pRun->tr_anchor && mdr_titleMatchesAnchor(0, pRun->tr_anchor, pMatch->am_matchAnchor)) {
		pMatch->am_matched = 1;
		return 0;
	}
	pMatch->am_offsetIdx++;
	return 1;
}

static void mdr_navigateAnchor(WINFO* wp, const char* pszAnchor) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	long nCol = 0;
	long nLine;
	while (pPart) {
		ANCHOR_MATCH match = { .am_matchAnchor = pszAnchor, .am_matched = 0, .am_offsetIdx = 0 };
		mdr_forRunListDo(pPart, mdr_matchAnchor, &match);
		if (match.am_matched) {
			nCol = match.am_offsetIdx;
			goto anchorMatched;
		}
		if (pPart->rvp_type == MET_HEADER) {
			if (mdr_titleMatchesAnchor(&pPart->rvp_data.rvp_flow, 0, pszAnchor)) {
anchorMatched:	
				nLine = ln_indexOf(wp->fp, pPart->rvp_lpStart);
				mdr_placeCaret(wp, &nLine, 0, &nCol, 0, 0);
				wt_curpos(wp, nLine, 0);
				mdr_adjustScrollBoundsOffset(wp, TRUE);
				return;
			}
		}
		pPart = pPart->rvp_next;
	}
}

static int mdr_placeCaretAfterClick(WINFO* wp, long* ln, long* col, int updateVirtualColumn) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* rvp = mdr_getViewPartAt(pData->md_pElements, *ln);
	if (rvp) {
		FTABLE* fp = wp->fp;
		*ln = ln_cnt(fp->firstl, rvp->rvp_lpStart)-1;
	}
	return mdr_placeCaret(wp, ln, *col, col, updateVirtualColumn, 0);
}

static int mdr_repaint(WINFO* wp, int ln1, int ln2, int col1, int col2) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (wp->ww_handle && pData) {
		LINE* lp1 = ln_gotoWP(wp, ln1);
		LINE* lp2 = ln_gotoWP(wp, ln2);
		int dummy;
		RENDER_VIEW_PART* p1 = mdr_getViewPartForLine(pData->md_pElements, lp1, &dummy);
		RENDER_VIEW_PART* p2 = mdr_getViewPartForLine(pData->md_pElements, lp2, &dummy);
		if (p1 != 0 && p1 == p2) {
			render_invalidateRect(wp, &p1->rvp_bounds);
		}
		else {
			render_invalidateRect(wp, NULL);
		}
		return 1;
	}
	return 0;
}


static RENDERER _mdrRenderer = {
	render_singleLineOnDevice,
	.r_renderPage = mdr_renderMarkdownFormatPage,
	.r_placeCaret = mdr_placeCaret,
	mdr_calculateMaxLine,
	mdr_calculateLongestLine,
	mdr_calculateMaxColumn,
	.r_placeCaretAfterClick = mdr_placeCaretAfterClick,
	mdr_screenOffsetToBuffer,
	mdr_allocData,
	mdr_destroyData,
	mdr_scroll,
	mdr_adjustScrollBounds,
	.r_updateCaretUI = mdr_updateCaretUI,
	.r_supportsMode = mdr_supportsMode,
	.r_hitTest = mdr_hitTest,
	FALSE,
	.r_modelChanged = mdr_modelChanged,
	.r_repaint = mdr_repaint,
	.r_context = "wysiwyg-renderer",
	.r_findLink = mdr_findLink,
	.r_navigateAnchor = mdr_navigateAnchor,
	.r_wndProc = mdr_wndProc,
	.r_printFragment = mdr_printFragment
};

static RENDERER _htmlRenderer = {
	.r_renderPage = mdr_renderHTMLFormatPage,
};

static RENDERER _adocRenderer = {
	.r_renderPage = mdr_renderAdocFormatPage,
};

/*
 * Returns a markdown renderer.
 */
RENDERER* mdr_getRenderer() {
	return &_mdrRenderer;
}

/*
 * Returns an HTML wysiwyg renderer.
 */
RENDERER* mdr_getHTMLRenderer() {
	memcpy(&_htmlRenderer, &_mdrRenderer, sizeof _mdrRenderer);
	_htmlRenderer.r_renderPage = mdr_renderHTMLFormatPage;
	_htmlRenderer.r_context = "wysiwyg-renderer";
	return &_htmlRenderer;
}

/*
 * Returns an AsciiDoc wysiwyg renderer.
 */
RENDERER* mdr_getAdocRenderer() {
	memcpy(&_adocRenderer, &_mdrRenderer, sizeof _mdrRenderer);
	_adocRenderer.r_renderPage = mdr_renderAdocFormatPage;
	return &_adocRenderer;
}


