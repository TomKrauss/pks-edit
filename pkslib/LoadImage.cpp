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
#include <Winhttp.h>

static IStream* CreateStreamOnResource(LPCCH lpName) {
    // initialize return value

    IStream* ipStream = NULL;
    WCHAR wstr[1024];

    // find the resource

    MultiByteToWideChar(CP_UTF8, 0, lpName, -1, wstr, sizeof wstr / sizeof (wstr[0]));
    if (SHCreateStreamOnFileEx(wstr, STGM_FAILIFTHERE, 0, FALSE, NULL, &ipStream) != S_OK) {
        return NULL;
    }

    return ipStream;
}

static char* http_loadData(BOOL https, char* pszHost, char* pszPath, UINT* pLoaded) {
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer = 0;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;
    WCHAR whost[1024];
    WCHAR wpath[1024];

    *pLoaded = 0;
    MultiByteToWideChar(CP_UTF8, 0, pszHost, -1, whost, sizeof whost / sizeof(whost[0]));
    MultiByteToWideChar(CP_UTF8, 0, pszPath, -1, wpath, sizeof wpath / sizeof(wpath[0]));

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"PKS-Edit Image/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        return 0;
    }
    // Specify an HTTP server.
    hConnect = WinHttpConnect(hSession, whost, https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);

    // Create an HTTP request handle.
    if (hConnect) {
        hRequest = WinHttpOpenRequest(hConnect, L"GET", wpath,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);
    }

    // Send a request.
    if (hRequest) {
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0, WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);
    }


    // End the request.
    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    // Keep checking for data until there is nothing left.

    DWORD nTotal = 10000;
    pszOutBuffer = (LPSTR)calloc(nTotal, 1);
    DWORD nOffset = 0;

    if (bResults && pszOutBuffer != NULL) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                free(pszOutBuffer);
                pszOutBuffer = 0;
                break;
            }
            if (nOffset + dwSize > nTotal) {
                nTotal = nOffset + dwSize + 10000;
                LPSTR pszNew = (LPSTR)realloc(pszOutBuffer, nTotal);
                if (pszNew == 0) {
                    free(pszOutBuffer);
                    pszOutBuffer = 0;
                    break;
                }
                else {
                    pszOutBuffer = pszNew;
                }
            }
            if (!WinHttpReadData(hRequest, (LPVOID)(pszOutBuffer + nOffset), dwSize, &dwDownloaded)) {
                free(pszOutBuffer);
                pszOutBuffer = 0;
                break;
            }
            nOffset += dwDownloaded;
        } while (dwSize > 0);
    }
    *pLoaded = nOffset;
    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    return pszOutBuffer;
}

/*
 * Utility method for loading a file from a URL specification (e.g. https://www.microsoft.com/myFile.txt) and returning
 * a pointer to the loaded data and the loaded size in pSize. It is the callers responsibility to free the data allocated.
 */
extern "C" __declspec(dllexport) char* http_loadDataFromUrl(char* pszUrl, UINT* pSize) {
    PARSEDURLA ppu = { 0 };
    ppu.cbSize = sizeof(ppu);
    HRESULT hr = ParseURLA(pszUrl, &ppu);
    if (SUCCEEDED(hr) && (ppu.nScheme == URL_SCHEME_HTTP || ppu.nScheme == URL_SCHEME_HTTPS)) {
        char host[1024];
        strncpy_s(host, ppu.pszSuffix + 2, ppu.cchSuffix - 2);
        char* pszPath = strstr(host, "/");
        if (pszPath != 0 && pszPath > host) {
            *pszPath++ = 0;
            return http_loadData(ppu.nScheme == URL_SCHEME_HTTPS, host, pszPath, pSize);
        }
    }
    return 0;
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
    HBITMAP hbmpImage = NULL;
    GUID guid = CLSID_WICPngDecoder;

    // load the PNG image data into a stream

    char* pszExt = strrchr(pszName, '.');
    // Not supported.
    if (pszExt == NULL) {
        return NULL;
    }
    if (pszExt != NULL) {
        pszExt++;
        if (_stricmp(pszExt, "gif") == 0) {
           guid = CLSID_WICGifDecoder;
        } else if (_stricmp(pszExt, "jpg") == 0 || _stricmp(pszExt, "jpeg") == 0) {
           guid = CLSID_WICJpegDecoder;
        } else if (_stricmp(pszExt, "tif") == 0) {
           guid = CLSID_WICTiffDecoder;
        } else if (_stricmp(pszExt, "png") == 0) {
           guid = CLSID_WICPngDecoder;
        } else if (_stricmp(pszExt, "webp") == 0) {
           guid = CLSID_WICWebpDecoder;
        } else {
           // Not supported.
           return NULL;
        }
    }
    UINT cbSize;
    char* pszLoaded = http_loadDataFromUrl(pszName, &cbSize);
    IStream* ipImageStream = pszLoaded == NULL ? CreateStreamOnResource(pszName) : SHCreateMemStream((const BYTE*)pszLoaded, cbSize);
    if (ipImageStream == NULL)
        return NULL;

    // load the bitmap with WIC

    IWICBitmapSource* ipBitmap = LoadBitmapFromStream(ipImageStream, guid);
    if (ipBitmap == NULL)
        goto ReleaseStream;

    // create a HBITMAP containing the image

    hbmpImage = CreateHBITMAP(ipBitmap);
    ipBitmap->Release();

ReleaseStream:
    ipImageStream->Release();
    return hbmpImage;
}