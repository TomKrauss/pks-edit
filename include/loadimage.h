/*
 * loadimage.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: loading inages.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LOADIMAGE_H

#include <Windows.h>

typedef struct tagIMAGE_LOAD_ASYNC {
    /*
     * Invoked after an image had been loaded.
     */
    void (*ila_complete)(HWND hwnd, void* param, HBITMAP bitmap);
    /*
     * 1st param to pass to ila_complete.
     */
    void* ila_completionParam;
    /*
     * The affected window - also passed into the completion function. If that is NULL, 
     * we are currently printing or are loading the images for some other reasons and
     * should not post loading images onto a separate thread.
     */
    HWND ila_hwnd;
} IMAGE_LOAD_ASYNC;

/*
 * Loads an image with a given name into a HBITMAP trying various formats for loading the image.
 * If the image format is not supported return 0.
 */
#ifdef __cplusplus
extern "C" 
#endif
__declspec(dllexport) HBITMAP loadimage_load(char* pszName, IMAGE_LOAD_ASYNC pAsyncCallback);

#define LOADIMAGE_H
#endif