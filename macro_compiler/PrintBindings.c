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
		comment);
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
 * print_keyBindingsAndDisplay
 */
int print_keyBindingsAndDisplay(void) {
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
int print_mouseBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("mouse.html", print_mouseBindingCallback,1);
}

static void print_indent(FILE* fp, int nIndent) {
	for (int i = 0; i < nIndent; i++) {
		fputc('\t', fp);
	}
}

static void print_menuText(FILE* fp, BOUND_TEXT* pText, MACROREF command) {
	char szTooltip[256];
	char szTemp[256];
	const char* pszText = bindings_getBoundText(pText);
	if (!pszText) {
		command_getTooltipAndLabel(command, szTooltip, szTemp);
		pszText = szTemp;
	}
	fprintf(fp, "<b>%s</b>: ", pszText);
}

static void print_menuDefinition(FILE* fp, MENU_ITEM_DEFINITION* pMid) {
	while (pMid) {
		if (pMid->mid_isSeparator) {
			fprintf(fp, "<li>---</li>\n");
		}
		else {
			fprintf(fp, "<li>");
			print_menuText(fp, &pMid->mid_label, pMid->mid_command);
			if (!pMid->mid_children) {
				char 	comment[MAC_COMMENTLEN], command[128];
				print_getNameAndComment(pMid->mid_command, comment, command);
				fputs(command, fp);
			}
			fprintf(fp, "</li>\n");
			if (pMid->mid_children) {
				fprintf(fp, "<ul>\n");
				print_menuDefinition(fp, pMid->mid_children);
				fprintf(fp, "</ul>\n");
			}
		}
		pMid = pMid->mid_next;
	}
}

/*--------------------------------------------------------------------------
 * print_menuCallback ()
 */
static int print_menuCallback(FILE *fp)
{
	char* szContexts[20];

	int nContext = bindings_getBindingContexts(szContexts);
	fprintf(fp, "<h3>Menu Bindings</h3>\n");
	fprintf(fp, "<ul>\n");
	for (int i = 0; i < nContext; i++) {
		char* pszContext = szContexts[i];
		fprintf(fp, "<h4>Context %s</h4>\n", pszContext);
		MENU_ITEM_DEFINITION* pMid = bindings_getMenuBarFor(pszContext);
		print_menuDefinition(fp, pMid);
	}
	fprintf(fp, "</ul>\n");
	return 1;
}

/*--------------------------------------------------------------------------
 * print_macrocDocumentationCallback()
 */
static int print_macrocDocumentationCallback(FILE* fp)
{
	char szNoDoc[512];
	fprintf(fp, "<h3>PKSMacroC API</h3>\n");
	for (int i = 0; i < _functionTableSize; i++) {
		NATIVE_FUNCTION* fup = &_functionTable[i];
		const char* pszApiDoc = macrodoc_helpForNativeFunction(fup->nf_name, fup);
		if (!pszApiDoc) {
			sprintf(szNoDoc, "<p><b>Synopsis</b>: %s()<br/>No documentation available</p>", fup->nf_name);
			pszApiDoc = szNoDoc;
		}
		fputs(pszApiDoc, fp);
		if (pszApiDoc != szNoDoc) {
			free((char*)pszApiDoc);
		}
		if (i < _functionTableSize - 1) {
			fputs("<hr/>", fp);
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * print_macrocDocumentationAndDisplay()
 */
int print_macrocDocumentationAndDisplay(void)
{
	return macro_createFileAndDisplay("macroc.html", print_macrocDocumentationCallback, 1);
}


/*--------------------------------------------------------------------------
 * print_menuBindingsAndDisplay()
 */
int print_menuBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("menu.html", print_menuCallback, 1);
}

/*--------------------------------------------------------------------------
 * macro_loadStringResource()
 */
const char *macro_loadStringResource(int nIndex) {
	return nIndex >= 0 && nIndex < _functionTableSize ? _functionTable[nIndex].nf_name : NULL;
}
