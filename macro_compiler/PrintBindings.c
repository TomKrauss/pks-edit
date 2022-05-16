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
#include "pksmacro.h"
#include "pksmod.h"
#include "actionbindings.h"

#include "funcdef.h"
#include "helpitem.h"

#include "stringutil.h"

#if defined(WIN32)
#define	_Windows
#endif

#define COL_KEYCODE		20
#define COL_FUNCTION	30

extern	MACRO *	macro_getByIndex(int i);
extern 	char *	mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);

static char *print_buttonEvent(char *b, int button, int shift, int nclicks)
{
	char			*s;

	s = bindings_printModifier(b,(KEYCODE)shift);
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

static void print_getNameAndComment(MACROREF macroref, char* comment, char* command) {
	int findex = macroref.index;
	MACRO* macp;

	comment[0] = 0;

	/* prepare for invalid entries */

	switch (macroref.typ) {
	case CMD_MACRO:
		if ((macp = macro_getByIndex(findex)) != 0)
			strcpy(comment, MAC_COMMENT(macp));
		break;
	case CMD_CMDSEQ:
		command_getTooltipAndLabel(macroref, comment, command);
		break;
	default:
		strcpy(comment, "illegal macro type");
		findex = -1;
		break;
	}
	mac_name(command, findex, (MACROREFTYPE)macroref.typ);
}

/*--------------------------------------------------------------------------
 * print_keybinding()
 * print one key binding as an HTML table row
 */
static void print_keybinding(FILE *fp, KEY_BINDING *kp)
{
	char 	comment[MAC_COMMENTLEN],command[128];

	if (kp->keycode == 0 || kp->keycode == K_DELETED)
		return;

	print_getNameAndComment(kp->macref, comment, command);
 	fprintf(fp,"<tr><td><code>%s</code></td><td><em>%s</em></td><td>%s</td></tr>\n",
		bindings_keycodeToString(kp->keycode),command,
		comment ? comment : "");
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
	int	    			n;
	struct tagCOLLECTED_KEYBINDS collected;
	char* szContexts[20];

	fprintf(fp, "<h3>Keyboard Bindings</h3>\n");
	int nContext = bindings_getBindingContexts(szContexts);
	for (int i = 0; i < nContext; i++) {
		char* pszContext = szContexts[i];
		collected.nElements = 0;
		collected.nCapacity = 300;
		collected.table = calloc(collected.nCapacity, sizeof(KEY_BINDING));
		bindings_forAllKeyBindingsDo(pszContext, print_collectKeybindings, &collected);
		kpd = collected.table;
		n = collected.nElements;

		qsort(kpd, n, sizeof * kpd, (int (*)(const void*, const void*))print_compareKeyBindings);
		fprintf(fp, "<h4>Context %s</h4>\n", pszContext);
		fprintf(fp, "<table style=\"width: 80%%\">\n");
		fprintf(fp, "<tr><th>Key Combination</th><th>Binding</th><th>Description</th></tr>\n");

		for (int k = 0; k < n; k++) {
			print_keybinding(fp, &kpd[k]);
		}
		fprintf(fp, "</table>\n");
		free(kpd);

	}

	return 1;
}

/*
 * print_saveKeyBindingsAndDisplay
 */
int print_saveKeyBindingsAndDisplay(void) {
	return macro_createFileAndDisplay("keys.html", print_keyBindingsCallback, 1);
}


/*
 * print_mousebinding()
 * command clicks button shift comment
 */
static void print_mousebinding(FILE *fp, MOUSE_EVENT_BINDING *mp)
{
	char 	command[128],button[64],comment[MAC_COMMENTLEN];

	print_getNameAndComment(mp->macref, comment, command);
	print_buttonEvent(button,mp->button,mp->shift,mp->nclicks);
 	fprintf(fp,"<tr><td><code>%s</code></td><td>%s</td><td>%s</td></tr>\n",
		button, 
		command, mp->msg.bt_text ? mp->msg.bt_text : comment);
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
	int	    	n;
	struct tagCOLLECTED_MOUSEBINDS collected;
	char *szContexts[20];

	int nContext = bindings_getBindingContexts(szContexts);
	fprintf(fp, "<h3>Mouse Bindings</h3>\n");
	for (int i = 0; i < nContext; i++) {
		char* pszContext = szContexts[i];
		collected.nElements = 0;
		collected.nCapacity = 300;
		collected.table = calloc(collected.nCapacity, sizeof(struct tagCOLLECTED_MOUSEBINDS));
		bindings_forAllMouseBindingsDo(pszContext, print_collectMousebindings, &collected);
		mpd = collected.table;
		n = collected.nElements;
		qsort(mpd, n, sizeof * mpd, (int (*)(const void*, const void*))print_compareMousebindings);
		fprintf(fp, "<h4>Context %s</h4>\n", pszContext);
		fprintf(fp, "<table style=\"width: 80%%\">\n");
		fprintf(fp, "<tr><th>Mouse Button Combination</th><th>Binding</th><th>Description</th></tr>\n");

		for (int m = 0; m < n; m++) {
			print_mousebinding(fp, &mpd[m]);
		}
		fprintf(fp, "</table>\n");

		free(mpd);
	}
	return 1;
}

/*
 * print the current mouse bindings to a file and display them to the user.
 */
int print_saveMouseBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("mouse.html", print_mouseBindingCallback,1);
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
	return macro_createFileAndDisplay("menu", print_menuCallback,0);
}

/*--------------------------------------------------------------------------
 * macro_loadStringResource()
 */
const char *macro_loadStringResource(int nIndex) {
	return nIndex >= 0 && nIndex < _functionTableSize ? _functionTable[nIndex].nf_name : NULL;
}
