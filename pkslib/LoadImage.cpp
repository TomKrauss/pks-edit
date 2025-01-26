/*
 * LoadImage.cpp
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
#include "..\include\loadimage.h"
#include "..\include\winterf.h"
#include "..\include\alloc.h"
#include "string.h"
#include <Windows.h>
#include <tchar.h>
#include <wincodec.h>
#include <Shlwapi.h>
#include <Winhttp.h>
#include <wininet.h>
#include <stdio.h>
#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>
#include <d2d1_3.h>

// Fake CLSID to denote SVG image decoding
#define CLSID_SvgDecoder CLSID_WICRAWDecoder

typedef struct tagURL {
    BOOL   url_https;
    LPWSTR url_url;
    LPWSTR url_host;
    LPWSTR url_path;
    LPWSTR url_extraInfo;
    LPWSTR url_pathAndParams;
} URL;

static HBITMAP loadimage_fromFileOrData(char* pszFileName, char* pszData, int cbSize);

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

typedef struct tagREQUEST_CONTEXT {
    void      (*rctx_complete)(void* pContext);
    HINTERNET rctx_sessionHandle;
    HINTERNET rctx_httpFileHandle;
    HINTERNET rctx_requestHandle;
    HANDLE    rctx_cleanupEvent;
    CRITICAL_SECTION rctx_criticalSection;
    char*            rctx_data;
    DWORD            rctx_error;
    UINT             rctx_dataSize;
    BOOL             rctx_posted;
    BOOL             rctx_requestComplete;
    IMAGE_LOAD_ASYNC rctx_callback;
} REQUEST_CONTEXT;

static void http_trace(const wchar_t fmt[], ...) {
    va_list 	ap;
    WCHAR      b[256];

    va_start(ap, fmt);
    wvsprintf(b, fmt, ap);
    va_end(ap);
    OutputDebugString(b);
}

static void http_traceError(const wchar_t fmt[], int errorCode) {
    WCHAR   wszMsgBuff[512];  // Buffer for text.

    DWORD   dwChars;  // Number of chars returned.

    // Try to get the message from the system errors.
    dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        0,
        wszMsgBuff,
        512,
        NULL);
    if (dwChars == 0) {
        wsprintf(wszMsgBuff, L"%d", errorCode);
    }
    http_trace(fmt, wszMsgBuff);
}

/*
 * Release the request context, when image loading from an HTTP URL is complete.
 */
static void http_releaseRequestContext(REQUEST_CONTEXT* pContext) {
    if (pContext->rctx_sessionHandle) {
        InternetSetStatusCallback(pContext->rctx_sessionHandle, NULL);
    }
    if (pContext->rctx_requestHandle) {
        InternetCloseHandle(pContext->rctx_requestHandle);
        //
        // Wait for the closing of the handle to complete
        // (waiting for all async operations to complete)
        //
        // This is the only safe way to get rid of the context
        //
        (VOID)WaitForSingleObject(pContext->rctx_cleanupEvent, INFINITE);
        pContext->rctx_requestHandle = NULL;
    }
    HINTERNET handle = pContext->rctx_httpFileHandle;
    if (handle) {
        InternetCloseHandle(handle);
        pContext->rctx_httpFileHandle = NULL;
    } else {
        http_trace(L"HTTP releasing request context %lx - file handle already released?\n", handle);
    }
    if (pContext->rctx_sessionHandle) {
        InternetCloseHandle(pContext->rctx_sessionHandle);
        pContext->rctx_sessionHandle = NULL;
    }
    if (pContext->rctx_data != NULL) {
        free(pContext->rctx_data);
    }
    http_trace(L"HTTP released request context %lx\n", handle);
    free(pContext);
}

static char* loadimage_loadHttpFile(REQUEST_CONTEXT* pRequestContext, UINT* pLoaded) {
    // Load the data from the HTTP file opened.
    http_trace(L"HTTP load image from http file %lx\n", pRequestContext->rctx_httpFileHandle);
    if (pRequestContext->rctx_httpFileHandle) {
        DWORD nBufSize = 8192;
        DWORD size;
        char* lpvBuffer = static_cast<char*>(calloc(static_cast<size_t>(nBufSize) + 1, 1));
        int nOffset = 0;
        int nTotalRead = 0;
        int nAvail = nBufSize;
        if (lpvBuffer == NULL) {
            http_trace(L"HTTP load image from http file cannot alloc %lx\n", nBufSize);
            return NULL;
        }
        while (TRUE) {
            size = 0;
            BOOL bResult = InternetReadFile(pRequestContext->rctx_httpFileHandle, &lpvBuffer[nOffset], nAvail, &size);
            if (bResult && !size) {
                break;
            }
            if (!bResult) {
                DWORD error = GetLastError();
                http_traceError(L"HTTP load InternetReadFile failed with %s. Retrying...", error);
                if (error == ERROR_INVALID_HANDLE) {
                    nTotalRead = 0;
                    break;
                }
                Sleep(100);
            }
            nTotalRead += size;
            nOffset += size;
            nAvail -= size;
            if (nAvail == 0) {
                nAvail = 8192;
                nBufSize += nAvail;
                char* pNew = static_cast<char*>(realloc(lpvBuffer, static_cast<size_t>(nBufSize) + 1));
                if (pNew == NULL) {
                    nTotalRead = 0;
                    break;
                }
                lpvBuffer = pNew;

            }
        }
        if (nTotalRead > 0) {
            lpvBuffer[nTotalRead] = 0;
            *pLoaded = nTotalRead;
            return lpvBuffer;
        }
        http_trace(L"HTTP InternetReadFile from %lx did not return data.\n", pRequestContext->rctx_httpFileHandle);
        free(lpvBuffer);
    }
    return NULL;
}

static void http_onWmThread(void* p) {
    http_trace(L"HTTP executing callback %lx on WM thread\n", p);
    REQUEST_CONTEXT* pRequestContext = (REQUEST_CONTEXT*)p;
    HBITMAP hbmp = pRequestContext->rctx_error ? NULL : loadimage_fromFileOrData(NULL, pRequestContext->rctx_data, pRequestContext->rctx_dataSize);
    pRequestContext->rctx_callback.ila_complete(pRequestContext->rctx_callback.ila_hwnd, pRequestContext->rctx_callback.ila_completionParam, hbmp, 
        pRequestContext->rctx_error);
    http_trace(L"HTTP freeing request context on WM thread\n");
    http_releaseRequestContext(pRequestContext);
}

static void http_onImageRequestComplete(REQUEST_CONTEXT* pRequestContext) {
    if (pRequestContext->rctx_complete) {
        http_trace(L"HTTP avoid double posting image with context %lx to WM thread\n", pRequestContext);
        return;
    }
    pRequestContext->rctx_data = pRequestContext->rctx_error ? NULL : loadimage_loadHttpFile(pRequestContext, &pRequestContext->rctx_dataSize);
    pRequestContext->rctx_complete = http_onWmThread;
    http_trace(L"HTTP posting image load %lx to WM thread\n", pRequestContext);
    PostMessage(pRequestContext->rctx_callback.ila_hwnd, WM_BACKGROUND_TASK_FINISHED, 0, (LPARAM)pRequestContext);
}

static void http_statusChanged(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInfo, DWORD dwStatusInfoLength) {
    INTERNET_ASYNC_RESULT* pResult = reinterpret_cast<INTERNET_ASYNC_RESULT*>(lpStatusInfo);
    REQUEST_CONTEXT* pRequestContext = reinterpret_cast<REQUEST_CONTEXT*>(dwContext);
    if (dwInternetStatus == INTERNET_STATUS_HANDLE_CREATED) {
        pRequestContext->rctx_httpFileHandle = reinterpret_cast<HINTERNET>(pResult->dwResult);
        pRequestContext->rctx_error = pResult->dwError;
    } else if (dwInternetStatus == INTERNET_STATUS_HANDLE_CLOSING) {
        SetEvent(pRequestContext->rctx_cleanupEvent);
    } else if (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE) {
        if (pRequestContext->rctx_requestComplete) {
            http_trace(L"HTTP Load Status REQUEST complete double call\n");
            return;
        }
        HINTERNET handle = pRequestContext->rctx_httpFileHandle;
        if (handle == NULL) {
            return;
        }
        pRequestContext->rctx_requestComplete = TRUE;
        http_trace(L"HTTP Load Status REQUEST Complete %p\n", handle);
        pRequestContext->rctx_error = pResult->dwError;
        if (pResult->dwError) {
            http_traceError(L"HTTP Load error: %s\n", pResult->dwError);
        }
        CRITICAL_SECTION section = pRequestContext->rctx_criticalSection;
        if (!TryEnterCriticalSection(&section)) {
            http_trace(L"HTTP Load Status REQUEST complete double call\n");
            return;
        }
        http_onImageRequestComplete(pRequestContext);
        LeaveCriticalSection(&section);
        DeleteCriticalSection(&section);
    }
}

static char* http_loadData(URL* pURL, UINT* pLoaded, IMAGE_LOAD_ASYNC* pAsyncCallback) {
    BOOL bAsync = pAsyncCallback->ila_hwnd != NULL;
    REQUEST_CONTEXT* pRequestContext = (REQUEST_CONTEXT *) calloc(1, sizeof(REQUEST_CONTEXT));

    if (pRequestContext == NULL) {
        return NULL;
    }
    *pLoaded = 0;
    pRequestContext->rctx_callback = *pAsyncCallback;
    if (!InitializeCriticalSectionAndSpinCount(&pRequestContext->rctx_criticalSection, 10)) {
        free(pRequestContext);
        return NULL;
    }
    pRequestContext->rctx_cleanupEvent = CreateEvent(NULL,  // Sec attrib
        FALSE, // Auto reset
        FALSE, // Initial state unsignalled
        NULL); 
    if (pRequestContext->rctx_cleanupEvent == NULL) {
        DeleteCriticalSection(&pRequestContext->rctx_criticalSection);
        free(pRequestContext);
        return NULL;
    }

    // Use InternetOpen to obtain a session handle.
    pRequestContext->rctx_sessionHandle = InternetOpen(L"PKS Edit/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL, bAsync ? INTERNET_FLAG_ASYNC : 0);

    if (!pRequestContext->rctx_sessionHandle) {
        http_releaseRequestContext(pRequestContext);
        return NULL;
    }

    if (InternetSetStatusCallback(pRequestContext->rctx_sessionHandle, http_statusChanged) == INTERNET_INVALID_STATUS_CALLBACK) {
        http_releaseRequestContext(pRequestContext);
        return 0;
    }

    // Open the URL
    WCHAR szURL[512];
    DWORD sizeURL = sizeof szURL / sizeof szURL[0] - 1;
    InternetCanonicalizeUrl(pURL->url_url, szURL, &sizeURL, 0);
    szURL[sizeURL] = 0;
    HINTERNET hFile;
    hFile = InternetOpenUrl(pRequestContext->rctx_sessionHandle, szURL, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE |
        INTERNET_FLAG_NO_CACHE_WRITE, (DWORD_PTR) pRequestContext);
    if (bAsync) {
        return NULL;
    }
    char* pResult = NULL;
    if (hFile) {
        pRequestContext->rctx_httpFileHandle = hFile;
        pResult = loadimage_loadHttpFile(pRequestContext, pLoaded);
    }
    http_releaseRequestContext(pRequestContext);
    return pResult;
}

static int loadimage_parseURL(char* pszUrl, URL *pURL) {
    URL_COMPONENTS urlComp;
    DWORD dwUrlLen = 0;
    WCHAR pszwUrl[512];

    // Initialize the structures.
    ZeroMemory(pURL, sizeof(*pURL));
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);

    // Set required component lengths to non-zero, 
    // so that they are cracked.
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    MultiByteToWideChar(CP_UTF8, 0, pszUrl, -1, pszwUrl, sizeof pszwUrl  / sizeof(pszwUrl[0]));
    if (!WinHttpCrackUrl(pszwUrl, (DWORD)wcslen(pszwUrl), 0, &urlComp) || (urlComp.nScheme != INTERNET_SCHEME_HTTP && urlComp.nScheme != INTERNET_SCHEME_HTTPS)) {
        return 0;
    }
    pURL->url_url = _wcsdup(pszwUrl);
    pURL->url_https = urlComp.nScheme == INTERNET_SCHEME_HTTPS;
    pURL->url_host = (LPWSTR) calloc(sizeof WCHAR, static_cast<size_t>(urlComp.dwHostNameLength) + 1);
    StrNCpy(pURL->url_host, urlComp.lpszHostName, static_cast<size_t>(urlComp.dwHostNameLength) + 1);
    pURL->url_path = (LPWSTR)calloc(sizeof WCHAR, static_cast<size_t>(urlComp.dwUrlPathLength) + 1);
    StrNCpy(pURL->url_path, urlComp.lpszUrlPath, static_cast<size_t>(urlComp.dwUrlPathLength) + 1);
    if (urlComp.dwExtraInfoLength > 0) {
        pURL->url_extraInfo = (LPWSTR)calloc(sizeof WCHAR, static_cast<size_t>(urlComp.dwExtraInfoLength) + 1);
        StrNCpy(pURL->url_extraInfo, urlComp.lpszExtraInfo, static_cast<size_t>(urlComp.dwExtraInfoLength) + 1);
    }
    pURL->url_pathAndParams = (LPWSTR)calloc(sizeof WCHAR, static_cast<size_t>(urlComp.dwUrlPathLength) + urlComp.dwExtraInfoLength + 1);
    StrNCpy(pURL->url_pathAndParams, urlComp.lpszUrlPath, static_cast<size_t>(urlComp.dwUrlPathLength) + urlComp.dwExtraInfoLength + 1);
    return 1;
}

static void url_free(URL* pURL) {
    free(pURL->url_url);
    free(pURL->url_host);
    free(pURL->url_path);
    free(pURL->url_pathAndParams);
    free(pURL->url_extraInfo);
}

/*
 * Utility method for loading a file from a URL specification (e.g. https://www.microsoft.com/myFile.txt) and returning
 * a pointer to the loaded data and the loaded size in pSize. It is the callers responsibility to free the data allocated.
 */
extern "C" __declspec(dllexport) char* http_loadDataFromUrl(char* pszUrl, UINT* pSize) {
    URL url;
    if (loadimage_parseURL(pszUrl, &url)) {
        IMAGE_LOAD_ASYNC asyncCallback = {0};
        char* ret = http_loadData(&url, pSize, &asyncCallback);
        url_free(&url);
        return ret;
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
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0) {
        return NULL;
    }

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

    if (hbmp != NULL && FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(pvImageBits)))) {
        // couldn't extract image; delete HBITMAP

        DeleteObject(hbmp);
        hbmp = NULL;
    }

    return hbmp;
}

static GUID loadimage_getType(char* pszName) {
    GUID guid = { 0 };

    char* pszExt = strrchr(pszName, '.');
    // Not supported.
    if (pszExt != NULL) {
        pszExt++;
        if (_stricmp(pszExt, "svg") == 0) {
            // Hack - use a fake CLSID for SVG decoding.
            return CLSID_SvgDecoder;
        }
        if (_stricmp(pszExt, "gif") == 0) {
            return CLSID_WICGifDecoder;
        }
        else if (_stricmp(pszExt, "jpg") == 0 || _stricmp(pszExt, "jpeg") == 0) {
            return CLSID_WICJpegDecoder;
        }
        else if (_stricmp(pszExt, "tif") == 0) {
            return CLSID_WICTiffDecoder;
        }
        else if (_stricmp(pszExt, "png") == 0) {
            return CLSID_WICPngDecoder;
        }
        else if (_stricmp(pszExt, "webp") == 0) {
            return CLSID_WICWebpDecoder;
        }
    }
    return guid;
}

ID2D1Factory* create_d2dFactory() {
    static ID2D1Factory* d2d_factory;
    HRESULT hr;
    if (!d2d_factory) {
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
        if (FAILED(hr)) {
            return NULL;
        }
    }
    return d2d_factory;
}

static HBITMAP load_bitmapFromSVG(IStream* ipImageStream) {
    ID2D1Factory* d2d_factory = create_d2dFactory();
    if (d2d_factory == NULL) {
        return NULL;
    }
    D2D1_RENDER_TARGET_PROPERTIES props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    ID2D1DCRenderTarget* target;
    d2d_factory->CreateDCRenderTarget(&props, &target);
    D2D1_SIZE_F size = { 256, 256};
    // this requires Windows 10 1703
    ID2D1DeviceContext5* dc;
    HRESULT hr = target->QueryInterface(&dc);
    if (FAILED(hr)) {
        target->Release();
        return 0;
    }
    RECT rc = { 0 };
    // open the SVG as a document
    ID2D1SvgDocument* svg;
    hr = dc->CreateSvgDocument(ipImageStream, size, &svg);
    if (FAILED(hr)) {
        dc->Release();
        target->Release();
        return 0;
    }
    ID2D1SvgElement* root;
    svg->GetRoot(&root);
    if (root) {
        FLOAT svgWidth = 0;
        FLOAT svgHeight = 0;
        root->GetAttributeValue(L"width", &svgWidth);
        root->GetAttributeValue(L"height", &svgHeight);
        if (svgWidth != 0 && svgHeight != 0) {
            size.width = svgWidth;
            size.height = svgHeight;
        }
        root->Release();
    }
    rc.right = (LONG)size.width;
    rc.bottom = (LONG)size.height;

    HBITMAP hbmp = NULL;
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    void* pvImageBits = NULL;

    // prepare structure giving bitmap information (negative height indicates a top-down DIB)

    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = (LONG)size.width;
    bminfo.bmiHeader.biHeight = (LONG)size.height;
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = GetDC(NULL);
    target->BindDC(hdcScreen, &rc);
    // create a DIB section that can hold the image
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    if (hbmp != NULL) {
        HDC hdc = CreateCompatibleDC(hdcScreen);

        auto hbmpOld = SelectObject(hdc, hbmp);
        if (hbmp != NULL) {
            target->BindDC(hdc, &rc);
            // draw it on the render target
            target->BeginDraw();
            dc->DrawSvgDocument(svg);
            hr = target->EndDraw();
        }
        if (hbmpOld != nullptr) {
            SelectObject(hdc, hbmpOld);
        }
        ReleaseDC(NULL, hdc);
        DeleteDC(hdc);
    }
    svg->Release();
    dc->Release();
    target->Release();
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcScreen);

    return hbmp;
}

static GUID loadimage_getTypeFromContent(char* pszData, UINT cbSize) {
    GUID guid = { 0 };
    if (cbSize < 5) {
        return guid;
    }
    int i = 0;
    while(i < (int)cbSize) {
        if (pszData[i] != '\n' && pszData[i] != ' ') {
            break;
        }
        i++;
    }
    if (pszData[i] == '<' && (strncmp(pszData+i, "<svg", 4) == 0 || strncmp(pszData+i, "<?xml", 5) == 0)) {
        return CLSID_SvgDecoder;
    }
    if (strncmp(pszData, "GIF8", 4) == 0) {
        return CLSID_WICGifDecoder;
    }
    if ((unsigned char)pszData[0] == 0xff && (unsigned char)pszData[1] == 0xd8) {
        return CLSID_WICJpegDecoder;
    }
    if (pszData[0] == 0x49 && pszData[1] == 0x49 && pszData[2] == 0x2a && pszData[3] == 0x00) {
        return CLSID_WICTiffDecoder;
    }
    if (strncmp(pszData, "BM", 2) == 0) {
        return CLSID_WICBmpDecoder;
    }
    if (pszData[0] == 0 && pszData[1] == 0 && pszData[2] == 1) {
        return CLSID_WICIcoDecoder;
    }
    if (strncmp(pszData, "RIFF", 4) == 0) {
        return CLSID_WICWebpDecoder;
    }
    if ((unsigned char)pszData[0] == 0x89 && pszData[1] == 'P' && pszData[2] == 'N' && pszData[3] == 'G') {
        return CLSID_WICPngDecoder;
    }
    return guid;
}

/*
 * Load the image from a given file name or from a data buffer with a given size.
 * The pszData buffer must be allocated before and will be released as a side effect of calling this method.
 */
HBITMAP loadimage_fromFileOrData(char* pszFileName, char* pszData, int cbSize) {
    HBITMAP hbmpImage = NULL;
    GUID guid = { 0 };
    if (pszData != NULL) {
        guid = loadimage_getTypeFromContent(pszData, cbSize);
    } else if (pszFileName != NULL) {
        guid = loadimage_getType(pszFileName);
    }
    if (guid.Data1 == 0) {
        return NULL;
    }
    IStream* ipImageStream = pszData == NULL ? CreateStreamOnResource(pszFileName) : SHCreateMemStream(reinterpret_cast<BYTE*>(pszData), cbSize);
    if (ipImageStream == NULL) {
        return NULL;
    }

    if (guid == CLSID_SvgDecoder) {
        hbmpImage = load_bitmapFromSVG(ipImageStream);
        http_trace(L"HTTP: Loading SVG image -> %lx\n", hbmpImage);
    }
    else {
        // load the bitmap with WIC
        IWICBitmapSource* ipBitmap = LoadBitmapFromStream(ipImageStream, guid);
        http_trace(L"HTTP: Loaded arbitrary image -> %px\n", ipBitmap);
        if (ipBitmap != NULL) {
            // create a HBITMAP containing the image
            hbmpImage = CreateHBITMAP(ipBitmap);
            ipBitmap->Release();
        }
    }
    ipImageStream->Release();
    return hbmpImage;
}

/*
 * Loads an image with a given name into a HBITMAP trying various formats for loading the image.
 * If the image format is not supported return 0.
 */
extern "C" __declspec(dllexport) IMAGE_LOAD_RESULT loadimage_load(char* pszName, IMAGE_LOAD_ASYNC pAsyncCallback) {
    char* pszLoaded = NULL;
    URL url;
    // load the PNG image data into a stream
    UINT cbSize = 0;
    IMAGE_LOAD_RESULT result = { 0 };

    if (loadimage_parseURL(pszName, &url)) {
        pszLoaded = http_loadData(&url, &cbSize, &pAsyncCallback);
        url_free(&url);
        if (pszLoaded == NULL) {
            result.ilr_loading = TRUE;
            return result;
        }
    }
    result.ilr_bitmap = loadimage_fromFileOrData(pszName, pszLoaded, cbSize);
    if (pszLoaded != NULL) {
        free(pszLoaded);
    }
    if (result.ilr_bitmap == NULL) {
        result.ilr_notFound = TRUE;
    }
    return result;
}