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
	DeleteObject(pData->ird_handle);
	free(pData);
	p->r_data = NULL;
}

static int image_supportsMode(int mode) {
	return FALSE;
}

/*
 * Returns the renderer data for the given window.
 */
static IMAGE_RENDERER_DATA* image_dataFromWindow(HWND hwnd) {
	WINFO* wp = ww_winfoFromWorkwinHandle(hwnd);
	return wp ? wp->r_data : 0;
}


/*
 * Custom window procedure used in markdown renderers.
 */
static LRESULT image_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	IMAGE_RENDERER_DATA* pData;

	switch (message) {
	case WM_SHOWWINDOW:
	case WM_SIZE:
		return 0;
	case WM_PAINT: {
		if ((pData = image_dataFromWindow(hwnd)) != 0) {
		}
		return FALSE;
	}
	}
	return render_defaultWindowProc(hwnd, message, wParam, lParam);
}

static RENDERER _imageRenderer = {
	.r_create = image_allocData,
	.r_destroy = image_freeData,
	.r_supportsMode = image_supportsMode,
	.r_wndProc = image_windowProc,
};

/*
 * Returns a hex renderer.
 */
RENDERER* image_getRenderer() {
	return &_imageRenderer;
}


