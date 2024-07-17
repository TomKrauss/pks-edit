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

typedef struct tagINPUT_STREAM INPUT_STREAM;

typedef struct tagMARKDOWN_RENDERER_DATA MARKDOWN_RENDERER_DATA;

/*
 * Destroy a list of view parts releasing unneeded memory.
 */
extern void mdr_destroyRendererData(MARKDOWN_RENDERER_DATA* pData);

/*
 * Parse a text and convert it into a list of view parts to be rendered later.
 * Note, that the viewparts must be destroyed, when they are not needed any more
 * using mdr_destroyRendererData. If hwndParent is specified also create some internal
 * WINDOWS resources.
 */
extern MARKDOWN_RENDERER_DATA* mdr_parseHTML(INPUT_STREAM* pStream, HWND hwndParent, const char* pszBaseURL);

/*
 * Render some render view parts as a response to a WM_PAINT message.
 */
extern void mdr_renderMarkdownData(HWND hwnd, PAINTSTRUCT* ps, int nTopY, MARKDOWN_RENDERER_DATA* pData);

/*
 * Invalidate the layout of all view parts of a markdown renderer 'pData' structure.
 */
extern void mdr_invalidateViewpartsLayout(MARKDOWN_RENDERER_DATA* pData);

/*
 * Can be used to find out, whether a link was clicked in a rich text renderer component given
 * the renderer data hook and the mouse x and y position. If a link was clicked, the target
 * URL is returned.
 */
extern char* mdr_linkClicked(MARKDOWN_RENDERER_DATA* pData, int cxMouse, int cyMouse);

/*
 * Generic mouse move handler for markdown rendering (supporting rollover effects etc...).
 */
extern void mdr_mouseMove(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, int x, int y);

/*
 * The vertical scrollbar was dragged - update the scrollbar and scroll the window contents.
 */
extern void mdr_scrolled(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, WPARAM wParam, BOOL bScrollChanged);

#define HTMLRENDERING_H
#endif 