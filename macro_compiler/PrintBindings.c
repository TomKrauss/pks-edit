/*
 * PrintBindings.c
 *
 * print key and mouse mappings
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 13.03.1991
 */

#define	PASTELIST		void

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "alloc.h"
#include "winterf.h"
#include "edfuncs.h"
#include "pksmod.h"
#include "edfuncs.h"

#include "pkscc.h"
#include "funcdef.h"
#include "helpitem.h"

#include "resource.h"
#include "stringutil.h"

#if defined(WIN32)
#define	_Windows
#endif

#define COL_KEYCODE		20
#define COL_FUNCTION	30

extern	MACRO *	macro_getByIndex(int i);
extern	char *	code2key(KEYCODE code);
extern 	char *	mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);

void macro_printListHeader(FILE *fp, char *itemname);

/*
 * print_modifier()
 */
char *print_modifier(char *s, KEYCODE code)
{
	*s = 0;
	if (code & K_ALTERNATE) {
		strcpy(s,"Alt+"); s += 4;
	}
	if (code & K_HAS_SELECTION) {
		strcpy(s, "Selected+"); s += 9;
	}
	if (code & K_CONTROL) {
		strcpy(s,"Ctrl+"); s += 5;
	}
	if (code & K_SHIFT) {
		strcpy(s,"Shift+"); s += 6;
	}
	return s;
}

static char *print_buttonEvent(char *b, int button, int shift, int nclicks)
{
	char			*s;

	s = print_modifier(b,(KEYCODE)(shift<<8));
	if (button & MBUT_L) {
		strcpy(s,"MLeft+");
		s += 5;
	}
	if (button & MBUT_R) {
		strcpy(s,"MRight+");
		s += 6;
	}
	if (button & MBUT_M) {
		strcpy(s,"MMiddle+");
		s += 7;
	}
	sprintf(s,"(%d)",nclicks);
	return b;
}

/*--------------------------------------------------------------------------
 * print_parseKeycode()
 * sorry for this: PKEDIT has the following functions also implemented. 
 * but: making Keys more readable than in menu enmtries in dumb GEM menus 
 *      we has to have a local _scantab
 */
#if defined (_Windows)

extern char *_scantab;

KEYCODE print_parseKeycode(const unsigned char *k, int control)
{
	unsigned char *	t;
	const unsigned char *	K;
	int  			code;

	if (!k[1]) {
		if ((*k >= '0' && *k <= '9') ||
		    (*k >= 'A' && *k <= 'Z')) {
			return *k | control;
		}
	}

	if (k[0] == '\\') {
		code  = (int)(k[1] - '0') * 100;
		code += (int)(k[2] - '0') * 10;
		code += (int)(k[3] - '0');
		return code | control;
	}

	K = k;
	for (t = _scantab; *t; ) {
		code = *t++;
		for (k = K; *k == *t; k++,t++)
			if (*t == 0) {
				return code | control;
			}
		while(*t++)
			;
	}
	return K_INVALID;
}

#else
static char *_scantab=
/* 0-14   */  "\0Esc\0\x31\0\x32\0\x33\0\x34\0\x35\0\x36\0\x37\0\x38\0\x39\0\x30\0Beta\0\Quote\0Backspace\0"
/* 15-29  */  "Tab\0Q\0W\0E\0R\0T\0Z\0U\0I\0O\0P\0UmlautUE\0Plus\0Return\0Ctr\0"
/* 30-43  */  "A\0S\0D\0F\0G\0H\0J\0K\0L\0UmlautOE\0UmlautAE\0Hash\0ShL\0Tilde\0"
/* 44-53  */  "Y\0X\0C\0V\0B\0N\0M\0Colon\0Dot\0Minus\0"
/* 54-58  */  "ShR\0\0Alt\0Space\0CLock\0"
/* 59-68  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 69-70  */  "\0\0"
/* 71-83  */  "ClrHome\0Up\0\0NumMinus\0Left\0\0Right\0NumPlus\0\0Down\0\0Insert\0Delete\0"
/* 84-93  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 94-102 */  "\0\0Less\0Undo\0Help\0NumLBracket\0NumRBracket\0NumSlash\0NumStar\0"
/* 103-112*/  "Num7\0Num8\0Num9\0Num4\0Num5\0Num6\0Num1\0Num2\0Num3\0Num0\0"
/* 113-119*/  "NumDot\0Enter\0Left\0Right\0\0\0\0"
/* 120-132*/  "1\0" "2\0" "3\0" "4\0" "5\0" "6\0" "7\0" "8\0" "9\0" "0\0" "Beta\0'";
KEYCODE print_parseKeycode(char *K, int control)
{
	char *	t;
	char *	k;
	int  	code;

	for (code = 114, t = _scantab+1; code >= 0; t++ , code-- ) {
fail:	if (*t) {
			k = K;
			while (*t)
				if (*k++ != *t++) {
					while(*t++);
					if (--code < 0)
						return K_INVALID;
					goto fail;
				}
			if (!*k) {
				code = 115-code;

				/*
				 * [Ctl] Cursorkeys left-right
				 */
				if (control & K_CONTROL) {
					if (code == 75)
						code = 115;
					else if (code == 77)
						code = 116;
				}
	
				/*
				 * [Alt]0-9
				 */
				if (code >= 2 && code <= 13 && 
				    (control & (K_ALTERNATE|K_CONTROL)) == K_ALTERNATE)
					code += 118;
				else

				/*
				 * [Sft]F1-F10
				 */
				if (code >= 59 && code <= 68 && 
				    (control & K_SHIFT))
					code += (84-59);

				return control|code;
			}
		}
	}
	return K_INVALID;
}

/*
 * code2key()
 */
static char *code2key(KEYCODE code)
{
	char *	t;
	char *	s;
	int  	i;
	static char b[64];

	s = c2shift(b,code);

	code &= 0xFF;

	if (code <= 0 || code >= 132) 
		return "[?]";

	for (i = 0, t = _scantab; i < code; t++, i++) {
		if (*t) {
			while (*t) 
				t++;
		}
	}

	strcpy(s,t);

	return b;
}

#endif

/*--------------------------------------------------------------------------
 * print_commandDelimiter()
 */
static char *print_commandDelimiter(char *d,char *command,char delim)
{
	sprintf(d,"%s%c",command,delim);
	return d;
}

/*--------------------------------------------------------------------------
 * print_comment()
 */
static char *print_comment(char *d, const char *comment)
{
	if (!comment)
		return "";
	sprintf(d,"# %s",comment);
	return d;
}

/*--------------------------------------------------------------------------
 * print_keybinding()
 * print one key binding in the following manner
 * KEYCODE | FUNCTION+PARS | REMARK
 */
static void print_keybinding(FILE *fp, KEY_BINDING *kp, char delim)
{
	EDBINDS *	ep = &_bindings;
	char 	comment[100],command[128],n1[100],n2[100];
	int  		findex;
	MACRO	*macp;

	if (kp->keycode == 0 || kp->keycode == K_DELETED)
		return;

	findex = kp->macref.index;
	comment[0] = 0;

	/* prepare for invalid entries */

	switch(kp->macref.typ) {
		case CMD_MACRO:
			if ((macp = ep->macp[findex]) != 0)
				strcpy(comment,MAC_COMMENT(macp)); 
			break;
		case CMD_CMDSEQ:
			command_getTooltipAndLabel(kp->macref, comment, command);
			break;
		default:
			strcpy(comment,"garbagge.."); 
			findex = -1;
			break;
	}
	mac_name(command, findex, (MACROREFTYPE)kp->macref.typ);

 	fprintf(fp,"%-25s= %-25s%s\n",
		code2key(kp->keycode),print_commandDelimiter(n1,command,delim),
		print_comment(n2,comment));
}

/*
 * print_compareKeyBindings()
 *
 * sort KEY_BINDING entries according to scancode:
 *
 */
static int print_compareKeyBindings(KEY_BINDING *kp1, KEY_BINDING *kp2)
{
	if (!kp1->keycode || kp1->keycode == K_DELETED)
		return -1;
	if (!kp2->keycode || kp2->keycode == K_DELETED)
		return 1;
	return kp1->keycode-kp2->keycode;
}

struct tagCOLLECTED_KEYBINDS {
	int nElements;
	int nCapacity;
	KEY_BINDING *table;
};

static int print_collectKeybindings(KEY_BINDING* kp, void* pParam) {
	struct tagCOLLECTED_KEYBINDS *pCollected = (struct tagCOLLECTED_KEYBINDS*)pParam;
	if (pCollected->nElements >= pCollected->nCapacity) {
		return 0;
	}
	pCollected->table[pCollected->nElements++] = *kp;
	return 1;
}

/*
 * print_keyBindingsCallback()
 */
static int print_keyBindingsCallback(FILE *fp)
{
	KEY_BINDING *		kpd;
	int	    		i,n;
	struct tagCOLLECTED_KEYBINDS collected;

	char* pszContext = "default";
	collected.nElements = 0;
	collected.nCapacity = 300;
	collected.table = calloc(collected.nCapacity, sizeof (KEY_BINDING));
	key_bindingsDo(pszContext, print_collectKeybindings, &collected);
	kpd = collected.table;
	n = collected.nElements;

	qsort(kpd,n,sizeof *kpd,(int (*)(const void*, const void*))print_compareKeyBindings);
	fprintf(fp, "\n\n//----------------------------------------\n");
	fprintf(fp, "// This file contains the PKS Edit keyboard bindings\n");
	fprintf(fp, "// To change the bindings\n");
	fprintf(fp, "//   - add the keyboard shortcut (if in doubt use the insert shortcut function from the menu)\n");
	fprintf(fp, "//   - add the command to bind to (code completion available)\n");
	fprintf(fp, "//   - press F10 to compile this file\n");
	fprintf(fp, "//   - save the new keyboard bindings using Save Macros...\n");
	fprintf(fp,"//----------------------------------------\n");
	fprintf(fp,"\n\noverride KEYS %s	# total %d bindings\n\n", pszContext, n);

	for (i = 0; i < n; i++) {
		print_keybinding(fp, &kpd[i], i < n-1 ? ',' : ';');
	}

	free(kpd);
	return 1;
}

/*
 * print_saveKeyBindingsAndDisplay
 */
int print_saveKeyBindingsAndDisplay(void) {
	return macro_createFileAndDisplay("keys", print_keyBindingsCallback);
}


/*
 * print_mousebinding()
 * command clicks button shift comment
 */
static void print_mousebinding(FILE *fp, MOUSE_EVENT_BINDING *mp, char delim)
{
	char 	command[128],button[30],b2[128],b3[128];

	mac_name(command,mp->macref.index,mp->macref.typ);
	print_buttonEvent(button,mp->button,mp->shift,mp->nclicks);
 	fprintf(fp,"%-25s= %c %-25s%s\n",
		button, ' ',
		print_commandDelimiter(b2,command,delim),print_comment(b3,mp->msg));
}

/*
 * print_compareMousebindings()
 *
 * sort MOUSE_EVENT_BINDING entries according to Button nclicks shift
 *
 */
static int print_compareMousebindings(MOUSE_EVENT_BINDING *mp1, MOUSE_EVENT_BINDING *mp2)
{	int d;

	if ((d = mp1->button - mp2->button) == 0 &&
	    (d = mp1->nclicks - mp2->nclicks) == 0) {
		d = mp1->shift - mp2->shift;
	}
	return d;
}

struct tagCOLLECTED_MOUSEBINDS {
	int nElements;
	int nCapacity;
	MOUSE_EVENT_BINDING* table;
};

static int print_collectMousebindings(MOUSE_EVENT_BINDING* mp, void* pParam) {
	struct tagCOLLECTED_MOUSEBINDS* pCollected = (struct tagCOLLECTED_MOUSEBINDS*)pParam;
	if (pCollected->nElements >= pCollected->nCapacity) {
		return 0;
	}
	pCollected->table[pCollected->nElements++] = *mp;
	return 1;
}

/*
 * print_mouseBindingCallback
 */
static int print_mouseBindingCallback(FILE *fp) {
	MOUSE_EVENT_BINDING* mpd;
	int	    	i, n;
	struct tagCOLLECTED_MOUSEBINDS collected;

	char* pszContext = "default";
	collected.nElements = 0;
	collected.nCapacity = 300;
	collected.table = calloc(collected.nCapacity, sizeof(struct tagCOLLECTED_MOUSEBINDS));
	mouse_bindingsDo(pszContext, print_collectMousebindings, &collected);
	mpd = collected.table;
	n = collected.nElements;
	qsort(mpd,n,sizeof *mpd,(int (*)(const void*, const void*))print_compareMousebindings);
	fprintf(fp, "// This file contains the PKS Edit mouse bindings\n");
	fprintf(fp, "// To change the bindings\n");
	fprintf(fp, "//   - add the mouse click specification\n");
	fprintf(fp, "//   - add the command to bind to (code completion available)\n");
	fprintf(fp, "//   - press F10 to compile this file\n");
	fprintf(fp, "//   - save the new keyboard bindings using Save Macros...\n");
	fprintf(fp,"\n\n//----------------------------------------\n");
	fprintf(fp,"\n\noverride MOUSE %s	# total %d bindings\n\n", pszContext, n);

	for (i = 0; i < n; i++) {
		print_mousebinding(fp, &mpd[i], i < n-1 ? ',' : ';');
	}

	free(mpd);
	return 1;
}

/*
 * print the current mouse bindings to a file and display them to the user.
 */
int print_saveMouseBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("mouse", print_mouseBindingCallback);
}

static void print_indent(FILE* fp, int nIndent) {
	for (int i = 0; i < nIndent; i++) {
		fputc('\t', fp);
	}
}

/*--------------------------------------------------------------------------
 * print_subMenu()
 */
static void print_subMenu(FILE *fp, HMENU hMenu, int nIndent) {
	MACROREF *	mp;
	HMENU		hSubMenu;
	UINT		wCount;
	UINT		wState;
	char		szText[128];
	char		command[100];
	int			nItem;
	int			wID;

	wCount = GetMenuItemCount(hMenu);
	for (nItem = 0; nItem < (int)wCount; ) {
		wState = GetMenuState(hMenu, nItem, MF_BYPOSITION);
		if (HIBYTE(wState) &&
			(hSubMenu = GetSubMenu(hMenu, nItem)) != 0) {
			GetMenuString(hMenu, nItem, szText, sizeof szText,
				MF_BYPOSITION);
			print_indent(fp, nIndent);
			fprintf(fp,"{\"label\": \"%.*s\", \"sub-menu\": [\n", (int)(sizeof szText), szText);
			print_subMenu(fp, hSubMenu, nIndent + 1);
			for (int i = 0; i < nIndent; i++) {
				fputc('\t', fp);
			}
			fprintf(fp, "]}");
		} else if (wState & MF_SEPARATOR) {
			print_indent(fp, nIndent);
			fprintf(fp,"{\"separator\": true}");
		} else {
			GetMenuString(hMenu, nItem, szText, sizeof szText,
				MF_BYPOSITION);
			wID = GetMenuItemID(hMenu, nItem);
			if (wID <= 0 || (wID > IDM_HISTORY && wID < IDM_HISTORY + 10)) {
				nItem++;
				continue;
			}
			print_indent(fp, nIndent);
			BOOL bAutoLabel = FALSE;
			if ((mp = macro_translateMenuCommand(wID)) == 0) {
				sprintf(command, "%d", wID);
			} else {
				bAutoLabel = TRUE;
				mac_name(command, mp->index, mp->typ);
			}
			if (wID == IDM_HISTORY) {
				fprintf(fp, "{\"history-menu\": true,");
				fprintf(fp, "\"command\": \"%s\"},", command);
			} else {
				fprintf(fp, "{");
				if (!bAutoLabel) {
					fprintf(fp, "\"label\": \"%.*s\",", (int)(sizeof szText), szText);
				}
				fprintf(fp, "\"command\": \"%s\"}", command);
			}
		}
		if (++nItem < (int)wCount && wID != IDM_HISTORY) {
			fprintf(fp, ",");
		}
		fprintf(fp, "\n");
	}
}

/*--------------------------------------------------------------------------
 * print_menuCallback ()
 */
static int print_menuCallback(FILE *fp)
{
	HMENU 	hMenu;

	fprintf(fp,"\n\n\"menu\": [");
	hMenu = GetMenu(hwndMain);
	print_subMenu(fp, hMenu, 0);
	fprintf(fp,"]\n");
	return 1;
}

/*--------------------------------------------------------------------------
 * print_saveMenuBindingsAndDisplay()
 */
int print_saveMenuBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("menu", print_menuCallback);
}

/*--------------------------------------------------------------------------
 * macro_loadStringResource()
 */
const char *macro_loadStringResource(int nIndex) {
	return nIndex >= 0 && nIndex < _nfuncs ? _edfunctab[nIndex].f_name : NULL;
}
