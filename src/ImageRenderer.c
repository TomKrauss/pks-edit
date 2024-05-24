/*
 * HexRenderer.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: edit image files.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <windows.h>
#include "winfo.h"
#include "themes.h"
#include "documentmodel.h"
#include "loadimage.h"

typedef struct tagIMAGE_RENDERER_DATA {
	HANDLE ird_handle;		// Handle of the image to paint.
} IMAGE_RENDERER_DATA;


static void* image_allocData(WINFO* wp) {
	return calloc(1, sizeof (IMAGE_RENDERER_DATA));
}

static void image_freeData(WINFO* p) {
	IMAGE_RENDERER_DATA* pData = (IMAGE_RENDERER_DATA *) p->r_data;
	if (pData == NULL) {
		return;
	}
	if (pData->ird_handle) {
		DeleteObject(pData->ird_handle);
	}
	free(pData);
	p->r_data = NULL;
}

static int image_supportsMode(int mode) {
	return (mode & (SHOW_CARET_LINE_HIGHLIGHT | SHOW_CONTROL_CHARS | SHOW_LINENUMBERS | SHOW_CARET_PRESERVE_COLUMN | SHOW_SYNTAX_HIGHLIGHT)) == 0;
}

static HANDLE image_createHandle(FTABLE* fp) {
	long long n = ft_totalBytes(fp);
	if (n == 0 || n > 2500000) {
		return NULL;
	}
	char* p = calloc(1, n);
	if (p == NULL) {
		return NULL;
	}
	LINE* lp = fp->firstl;
	char* pRun = p;
	while (lp && lp != fp->lastl) {
		memcpy(pRun, lp->lbuf, lp->len);
		pRun += lp->len;
		if (LINE_HAS_LINE_END(lp)) {
			if (LINE_HAS_CR(lp)) {
				*pRun++ = '\r';
			}
			*pRun++ = '\n';
		}
		lp = lp->next;
	}

	HANDLE ret = loadimage_fromFileOrData(NULL, p, (int) n);
	free(p);
	return ret;
}

/*--------------------------------------------------------------------------
 * render_paintWindowParams()
 */
static void image_paintWindowParams(WINFO* wp) {
	HBRUSH		hBrushBg;
	HDC 		hdc;
	HWND		hwnd;
	PAINTSTRUCT ps;
	THEME_DATA* pTheme = theme_getCurrent();

	hwnd = wp->ww_handle;
	hdc = BeginPaint(hwnd, &ps);
	hBrushBg = CreateSolidBrush(pTheme->th_defaultBackgroundColor);
	FillRect(hdc, &ps.rcPaint, hBrushBg);
	IMAGE_RENDERER_DATA* pData = (IMAGE_RENDERER_DATA * )wp->r_data;
	if (!pData->ird_handle) {
		pData->ird_handle = image_createHandle(wp->fp);
	}
	if (pData->ird_handle) {
		BITMAP bitmap = { 0 };
		GetObject(pData->ird_handle, sizeof(bitmap), &bitmap);
		if (bitmap.bmWidth != 0 && bitmap.bmHeight != 0) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			HDC hdcMem = CreateCompatibleDC(hdc);
			int nWidth = bitmap.bmWidth;
			int nHeight= bitmap.bmHeight;
			int nPaintWidth = (int)(nWidth * wp->zoomFactor);
			int nPaintHeight = (int)(nHeight * wp->zoomFactor);
			int x = (rect.right - nPaintWidth) / 2;
			if (x < 0) {
				x = 0;
			}
			int y = (rect.bottom - nPaintHeight) / 2;
			if (y < 0) {
				y = 0;
			}
			HANDLE oldBitmap = SelectObject(hdcMem, pData->ird_handle);
			SetStretchBltMode(hdc, COLORONCOLOR);
			TransparentBlt(hdc, x, y, nPaintWidth, nPaintHeight, hdcMem, 0, 0, nWidth, nHeight, RGB(0,0,0));
			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);
		}
	}
	DeleteObject(hBrushBg);
	EndPaint(hwnd, &ps);
}

/*
 * Custom window procedure used in markdown renderers.
 */
static LRESULT image_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_PAINT: {
		WINFO* wp = ww_winfoFromWorkwinHandle(hwnd);
		if (wp != 0) {
			image_paintWindowParams(wp);
		}
		return FALSE;
	}
	}
	return render_defaultWindowProc(hwnd, message, wParam, lParam);
}

static void image_modelChanged(WINFO* wp, MODEL_CHANGE* pMC) {
	IMAGE_RENDERER_DATA* pData = (IMAGE_RENDERER_DATA*)wp->r_data;
	if (pData->ird_handle) {
		DeleteObject(pData->ird_handle);
		pData->ird_handle = NULL;
		InvalidateRect(wp->ww_handle, NULL, FALSE);
	}
}

static RENDERER _imageRenderer = {
	.r_create = image_allocData,
	.r_destroy = image_freeData,
	.r_supportsMode = image_supportsMode,
	.r_modelChanged = image_modelChanged,
	.r_wndProc = image_windowProc,
};

/*
 * Returns a hex renderer.
 */
RENDERER* image_getRenderer() {
	return &_imageRenderer;
}


