/*
 * customcontrols.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: utilities for implementing custom controls in PKS Edit
 *
 * 										created: 01.01.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CUSTOMCONTROLS_H

#include <windows.h>

/*
 * When rows with text and icons are painted withing PKS Edit - this is the height in pixels to reserve for
 * the row.
 */
#define LB_ROW_WITH_ICON_HEIGHT	24

/*--------------------------------------------------------------------------
 * cust_drawShadow()
 * Draw a shadow around a control
 */
extern void cust_drawShadow(HDC hdc, RECT* rcp, int pressed);

/**
 * Paint a custom button control.
 */
extern void cust_paintButton(HDC hdc, RECT* rcp, HWND hwnd, int ww);

/**
 * Return a handle to a small editor font.
 */
extern HFONT cust_getSmallEditorFont(HWND hwnd);

/**
 * Return a handle to a default font used by controls in PKS Edit.
 */
extern HFONT cust_getDefaultEditorFont(void);

/*------------------------------------------------------------
 * cust_drawOutline()
 * Draw an outline rectanle.
 */
extern void cust_drawOutline(HDC hDC, int left, int top, int w, int h);

/*--------------------------------------------------------------------------
 * cust_calculateButtonCharacterHeight()
 * Calculates the height of a custom buttons character.
 */
extern int cust_calculateButtonCharacterHeight(HWND hwnd);

/*--------------------------------------------------------------------------
 * cust_initializeWindowClassDefaults()
 * Can be used to initialize the class defaults of a window class to create.
 */
extern void cust_initializeWindowClassDefaults(WNDCLASS* wcp);

/*--------------------------------------------------------------------------
 * cust_registerControls()
 * Initialize the PKS edit custom controls registering the window classes.
 */
extern int cust_registerControls(void);


/*--------------------------------------------------------------------------
 * cust_drawComboBoxOwnerDraw()
 */
extern int cust_drawComboBoxOwnerDraw(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(),
	void (*ShowSelection)(LPDRAWITEMSTRUCT lp), BOOL bNoDarkMode);

/*
 * Measure a list box row in an owner drawn listbox painting an icon + a text.
 */
extern void cust_measureListBoxRowWithIcon(MEASUREITEMSTRUCT* mp);

/*
 * Draw a list box row in an owner drawn listbox painting an icon + a text.
 */
extern void cust_drawListBoxRowWithIcon(HDC hdc, RECT* rcp, HICON hIcon, char* pszText);

/*
 * Create a toast window and show the passed text.
 */
extern HWND cust_createToastWindow(char* pszText);

/*
 * Select one of the pre-defined postfix icons for a labeled window.
 */
extern void cust_setPostfixIcon(HWND hwndLabeled, int index);

/*
 * Create a window which displays a child window + an associated prefix icon painting a border around the resulting
 * element. Additional Icons are displayed as postfix icons.
 */
extern HWND cust_createLabeledWindow(HWND hwndParent, HICON* hIcons, char* pszCueBanner, HWND hwndChild);

/*
 * A child of a labeled window got / lost the focus. WPARAM is true for focus gain. 
 */
#define LAB_CHILD_FOCUS_CHANGE		WM_USER + 10

/*
 * cust_createToolTooltip()
 * Create a tooltip to be displayed in/by a target window (hwndTool).
 */
extern HWND cust_createToolTooltip(HWND hwndTool);

/*
 * Utility to check, whether a window refers to an edit field.
 */
extern BOOL cust_isEditField(HWND hwnd);

#define CUSTOMCONTROLS_H

#endif // !CUSTOMCONTROLS_H
