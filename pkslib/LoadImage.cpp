/*
 * LoadImageFormat.cpp
 *
 * Project: PKS Edit for Windows
 *
 * purpose: loading images in various formats.
 *
 * 										created: 05.02.89
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "pch.h"
#include <Windows.h>
#include <tchar.h>
#include <wincodec.h>
#include <Shlwapi.h>

static IStream* CreateStreamOnResource(LPCCH lpName) {
    // initialize return value

    IStream* ipStream = NULL;
    WCHAR wstr[1024];

    // find the resource

    MultiByteToWideChar(CP_UTF8, 0, lpName, -1, wstr, sizeof wstr / 4);
    if (SHCreateStreamOnFileEx(wstr, STGM_FAILIFTHERE, 0, FALSE, NULL, &ipStream) != S_OK)
        return NULL;

    return ipStream;
}

static IWICBitmapSource* LoadBitmapFromStream(IStream* ipImageStream, GUID guid) {
    // initialize return value

    IWICBitmapSource* ipBitmap = NULL;
    UINT nFrameCount = 0;
    IWICBitmapFrameDecode* ipFrame = NULL;

    // load WIC's PNG decoder

    IWICBitmapDecoder* ipDecoder = NULL;
    if (FAILED(CoCreateInstance(guid, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
        return NULL;

    // load the PNG

    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
        goto ReleaseDecoder;

    // check for the presence of the first frame in the bitmap

    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount < 1)
        goto ReleaseDecoder;

    // load the first frame (i.e., the image)

    if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
        goto ReleaseDecoder;

    // convert the image to 32bpp BGRA format with pre-multiplied alpha

    //   (it may not be stored in that format natively in the PNG resource,

    //   but we need this format to create the DIB to use on-screen)

    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();

ReleaseDecoder:
    ipDecoder->Release();
    return ipBitmap;
}

static HBITMAP CreateHBITMAP(IWICBitmapSource* ipBitmap) {
    // initialize return value

    HBITMAP hbmp = NULL;
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    void* pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);

    // get image attributes and check for valid image

    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
        return NULL;

    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    // prepare structure giving bitmap information (negative height indicates a top-down DIB)

    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG)height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    // create a DIB section that can hold the image

    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);

    // extract the image into the HBITMAP

    if (hbmp != NULL && FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(pvImageBits))))
    {
        // couldn't extract image; delete HBITMAP

        DeleteObject(hbmp);
        hbmp = NULL;
    }

    return hbmp;
}

// Loads the an image with a name and a format into a HBITMAP.
extern "C" __declspec(dllexport) HBITMAP loadimage_load(char* pszName) {
    HBITMAP hbmpSplash = NULL;
    GUID guid = CLSID_WICPngDecoder;

    // load the PNG image data into a stream

    char* pszExt = strrchr(pszName, '.');
    if (pszExt != NULL) {
        pszExt++;
        if (_stricmp(pszExt, "gif") == 0) {
            guid = CLSID_WICGifDecoder;
        }
        else if (_stricmp(pszExt, "jpg") == 0 || _stricmp(pszExt, "jpeg") == 0) {
            guid = CLSID_WICJpegDecoder;
        }
    }
    IStream* ipImageStream = CreateStreamOnResource(pszName);
    if (ipImageStream == NULL)
        return NULL;

    // load the bitmap with WIC

    IWICBitmapSource* ipBitmap = LoadBitmapFromStream(ipImageStream, guid);
    if (ipBitmap == NULL)
        goto ReleaseStream;

    // create a HBITMAP containing the image

    hbmpSplash = CreateHBITMAP(ipBitmap);
    ipBitmap->Release();

ReleaseStream:
    ipImageStream->Release();
    return hbmpSplash;
}