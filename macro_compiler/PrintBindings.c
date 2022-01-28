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
extern 	char *	mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);

void macro_printListHeader(FILE *fp, char *itemname);

static char *print_buttonEvent(char *b, int button, int shift, int nclicks)
{
	char			*s;

	s = macro_printModifier(b,(KEYCODE)(shift<<8));
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
		macro_keycodeToString(kp->keycode),print_commandDelimiter(n1,command,delim),
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
		print_commandDelimiter(b2,command,delim),print_comment(b3,mp->msg.bt_text));
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
