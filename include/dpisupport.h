/*
 * dpisupport.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: methods required to handle dpi capabilities of the
 * screen we are running on.
 *
 * 										created: 20.12.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DPISUPPORT_H


 /*
  * Returns the scaling factor in X / the width of one logical pixel in physical pixels.
  */
extern float dpisupport_getScalingFactorX();

/*
 * Returns the configured toolbar icon size in physical pixels.
 */
extern int dpisupport_getTbIconSize(HWND hwnd);

/*
 * Multiplies an arbitrary value by a dpi scaling factor.
 */
extern int dpisupport_getSize(int nBaseValue);

/*
 * Initializes the scaling factor for a given window.
 */
float dpisupport_initScalingFactor(HWND hwnd);

#define DPISUPPORT_H

#endif