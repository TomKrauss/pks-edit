/*
 * customcontrols.h
 *
 * PROJEKT: PKS-EDIT for ATARI - WINDOWS
 *
 * purpose: utilities for implementing custom controls in PKS-EDIT
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
extern HFONT cust_getSmallEditorFont(void);

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
 * cust_drawOwnerSelection()
 * Draw the selection of an owner draw listbox.
 */
extern void cust_drawOwnerSelection(LPDRAWITEMSTRUCT lpdis);

/*--------------------------------------------------------------------------
 * cust_drawComboBoxOwnerDraw()
 */
extern int cust_drawComboBoxOwnerDraw(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(),
	void (*ShowSelection)(LPDRAWITEMSTRUCT lp));

#define CUSTOMCONTROLS_H

#endif // !CUSTOMCONTROLS_H
