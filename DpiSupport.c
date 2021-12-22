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
#include "editorconfiguration.h"

/*
 * Returns the scaling factor in X / the width of one logical pixel in physical pixels.
 */
float dpisupport_getScalingFactorX() {
	return 1.0f;
}

/*
 * Returns the configured toolbar icon size in pixels.
 */
int dpisupport_getTbIconSize() {
	int nSize;
	switch (GetConfiguration()->iconSize) {
	case ICS_MEDIUM: nSize = 24; break;
	case ICS_BIG: nSize = 32; break;
	case ICS_LARGE: nSize = 48; break;
	default: nSize = 16; break;
	}
	nSize += 2; // margins
	return (int)(nSize * dpisupport_getScalingFactorX());
}

