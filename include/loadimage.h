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
     * Invoked after an image had been loaded. Pass ila_completionParam as param, the bitmap loaded
     * and the error which occurred, if the bitmap is NULL and could not be loaded.
     */
    void (*ila_complete)(HWND hwnd, void* param, HBITMAP bitmap, int dwEror);
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

typedef struct tagIMAGE_LOAD_RESULT {
    HBITMAP ilr_bitmap;
    BOOL    ilr_loading : 1;
    BOOL    ilr_notFound : 1;
} IMAGE_LOAD_RESULT;

/*
 * Loads an image with a given name into a HBITMAP trying various formats for loading the image.
 * If the image format is not supported return 0.
 */
#ifdef __cplusplus
extern "C" 
#endif
__declspec(dllexport) IMAGE_LOAD_RESULT loadimage_load(char* pszName, IMAGE_LOAD_ASYNC pAsyncCallback);

/*
 * Load the image from a given file name or from a data buffer with a given size.
 * The pszData buffer must be allocated before and will be released as a side effect of calling this method.
 */
#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport) HBITMAP loadimage_fromFileOrData(char* pszFileName, char* pszData, int cbSize);

#define LOADIMAGE_H
#endif