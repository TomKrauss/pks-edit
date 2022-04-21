/*
 * htmlrendering.h
 *
 * light-weight HTML rendering in windows.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 03.04.1991
 */

#ifndef HTMLRENDERING_H

typedef struct tagRENDER_VIEW_PART RENDER_VIEW_PART;

/*
 * Destroy a list of view parts releasing unneeded memory.
 */
extern void mdr_destroyViewParts(RENDER_VIEW_PART** pHEAD);

/*
 * Parse a text and convert it into a list of view parts to be rendered later.
 * Note, that the viewparts must be destroyed, when they are not needed any more
 * using mdr_destroyViewParts.
 */
extern RENDER_VIEW_PART* mdr_parseHTML(const char* pszText);

/*
 * Render some render view parts as a response to a WM_PAINT message.
 */
extern void mdr_renderViewparts(HWND hwnd, PAINTSTRUCT* ps, RENDER_VIEW_PART* pFirst);

#define HTMLRENDERING_H
#endif 