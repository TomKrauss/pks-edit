/*
 *
 * DpiSupport.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: handle device and resolution differences.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "dpisupport.h"
#include "editorconfiguration.h"

extern HWND hwndMain;

static float scale;

/*
 * Initializes the scaling factor for a given window.
 */
float dpisupport_initScalingFactor(HWND hwnd) {
	if (scale == 0) {
		float dpi = (float)GetDpiForWindow(hwnd);
		scale = dpi / 96.0f;
	}
	return scale;
}

/*
 * Invoked, when the scaling has probably changed for the main window. Re-initialize the scaling
 * factor and return true if it really has changed.
 */
BOOL dpisupport_scalingChanged(HWND hwnd) {
	float oldscale = scale;
	scale = 0;
	dpisupport_initScalingFactor(hwnd);
	return scale != oldscale;
}

/*
 * Returns the scaling factor in X / the width of one logical pixel in physical pixels.
 */
float dpisupport_getScalingFactorX() {
	if (scale == 0 && hwndMain == 0) {
		// This is actually bad - should not happen.
		return (float)GetSystemMetrics(SM_CXICON) / 32;
	}
	return dpisupport_initScalingFactor(hwndMain);
}

/*
 * Multiplies an arbitrary value by a dpi scaling factor.
 */
int dpisupport_getSize(int nBaseValue) {
	return (int)(dpisupport_getScalingFactorX() * nBaseValue);
}

/*
 * Returns the configured toolbar icon size in pixels.
 */
int dpisupport_getTbIconSize(HWND hwnd) {
	int nSize;
	switch (GetConfiguration()->iconSize) {
	case ICS_MEDIUM: nSize = 24; break;
	case ICS_BIG: nSize = 32; break;
	case ICS_LARGE: nSize = 48; break;
	default: nSize = 16; break;
	}
	nSize += 2; // margins
	return (int)(nSize * dpisupport_initScalingFactor(hwnd));
}

