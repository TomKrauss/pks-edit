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
#include <Windows.h>
#include <tchar.h>
#include <wincodec.h>
#include <Shlwapi.h>
#include <Winhttp.h>
#include <wininet.h>
#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>
#include <d2d1_3.h>

// Number of milli seconds to wait to load an image via HTTP before timing out.
#define HTTP_LOAD_RESPONSE_TIMEOUT_MS         400

typedef struct tagURL {
    BOOL   url_https;
    LPWSTR url_url;
    LPWSTR url_host;
    LPWSTR url_path;
    LPWSTR url_extraInfo;
    LPWSTR url_pathAndParams;
} URL;

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
    HINTERNET rctx_sessionHandle;
    HINTERNET rctx_httpFileHandle;
    HINTERNET rctx_requestHandle;
    BOOL      rctx_requestComplete;
    DWORD     rctx_error;
} REQUEST_CONTEXT;

static void http_closeRequestContext(REQUEST_CONTEXT* pContext) {
    if (pContext->rctx_requestHandle) {
        InternetCloseHandle(pContext->rctx_requestHandle);
    }
    if (pContext->rctx_httpFileHandle) {
        InternetCloseHandle(pContext->rctx_httpFileHandle);
    }
    if (pContext->rctx_sessionHandle) {
        InternetSetStatusCallback(pContext->rctx_sessionHandle, NULL);
        InternetCloseHandle(pContext->rctx_sessionHandle);
    }
}

static void http_statusChanged(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpStatusInfo, DWORD dwStatusInfoLength) {
    INTERNET_ASYNC_RESULT* pResult = (INTERNET_ASYNC_RESULT * )lpStatusInfo;
    REQUEST_CONTEXT* pRequestContext = (REQUEST_CONTEXT*)dwContext;
    if (dwInternetStatus == INTERNET_STATUS_HANDLE_CREATED) {
        pRequestContext->rctx_httpFileHandle = (HINTERNET)pResult->dwResult;
        pRequestContext->rctx_error = pResult->dwError;
    }
    if (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE) {
        pRequestContext->rctx_requestComplete = TRUE;
        pRequestContext->rctx_error = pResult->dwError;
    }
}

static void http_waitFor(REQUEST_CONTEXT* pContext, int dwMilliSeconds) {
    for (int i = 0; i < dwMilliSeconds/100; i++) {
        if (pContext->rctx_requestComplete) {
            break;
        }
        Sleep(100);
    }
}

static char* http_loadData(URL* pURL, UINT* pLoaded, IMAGE_LOAD_ASYNC* pAsyncCallback) {
    BOOL bAsync = pAsyncCallback->ila_hwnd != NULL;
    REQUEST_CONTEXT requestContext;

    *pLoaded = 0;
    ZeroMemory(&requestContext, sizeof requestContext);

    // Use InternetOpen to obtain a session handle.
    requestContext.rctx_sessionHandle = InternetOpen(L"PKS Edit/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL, INTERNET_FLAG_ASYNC);

    if (!requestContext.rctx_sessionHandle) {
        return 0;
    }

    if (InternetSetStatusCallback(requestContext.rctx_sessionHandle, http_statusChanged) == INTERNET_INVALID_STATUS_CALLBACK) {
        http_closeRequestContext(&requestContext);
        return 0;
    }

    // Open the URL
    WCHAR szURL[512];
    DWORD sizeURL = sizeof szURL / sizeof szURL[0] - 1;
    InternetCanonicalizeUrl(pURL->url_url, szURL, &sizeURL, 0);
    szURL[sizeURL] = 0;
    InternetOpenUrl(requestContext.rctx_sessionHandle, szURL, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE |
        INTERNET_FLAG_NO_CACHE_WRITE, (DWORD_PTR) & requestContext);
    http_waitFor(&requestContext, HTTP_LOAD_RESPONSE_TIMEOUT_MS);

    // Create an HTTP request handle.
    if (requestContext.rctx_httpFileHandle && requestContext.rctx_error == 0) {
        DWORD nBufSize = 8192;
        DWORD size;
        requestContext.rctx_requestComplete = FALSE;
        char* lpvBuffer = (char*)calloc(static_cast<size_t>(nBufSize) + 1, 1);
        int nOffset = 0;
        int nTotalRead = 0;
        int nAvail = nBufSize;
        if (lpvBuffer != NULL) {
            while (InternetReadFile(requestContext.rctx_httpFileHandle, &lpvBuffer[nOffset], nAvail, &size) && size > 0) {
                nTotalRead += size;
                nOffset += size;
                nAvail -= size;
                if (nAvail == 0) {
                    nAvail = 8192;
                    nBufSize += nAvail;
                    char* pNew = (char*)realloc(lpvBuffer, static_cast<size_t>(nBufSize) + 1);
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
                http_closeRequestContext(&requestContext);
                return lpvBuffer;
            }
            free(lpvBuffer);
        }
    }
    http_closeRequestContext(&requestContext);
    return 0;
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
    pURL->url_host = (LPWSTR) calloc(sizeof WCHAR, urlComp.dwHostNameLength + 1);
    StrNCpy(pURL->url_host, urlComp.lpszHostName, urlComp.dwHostNameLength + 1);
    pURL->url_path = (LPWSTR)calloc(sizeof WCHAR, urlComp.dwUrlPathLength + 1);
    StrNCpy(pURL->url_path, urlComp.lpszUrlPath, urlComp.dwUrlPathLength + 1);
    if (urlComp.dwExtraInfoLength > 0) {
        pURL->url_extraInfo = (LPWSTR)calloc(sizeof WCHAR, urlComp.dwExtraInfoLength + 1);
        StrNCpy(pURL->url_extraInfo, urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength + 1);
    }
    pURL->url_pathAndParams = (LPWSTR)calloc(sizeof WCHAR, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength + 1);
    StrNCpy(pURL->url_pathAndParams, urlComp.lpszUrlPath, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength + 1);
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

#define CLSID_SvgDecoder CLSID_WICRAWDecoder

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

static ID2D1Factory* d2d_factory;
static HBITMAP load_bitmapFromSVG(IStream* ipImageStream) {
    HRESULT hr;
    if (!d2d_factory) {
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
        if (FAILED(hr)) {
            return NULL;
        }
    }
    D2D1_RENDER_TARGET_PROPERTIES props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    ID2D1DCRenderTarget* target;
    d2d_factory->CreateDCRenderTarget(&props, &target);
    D2D1_SIZE_F size = { 1920, 1920 };
    // this requires Windows 10 1703
    ID2D1DeviceContext5* dc;
    hr = target->QueryInterface(&dc);
    if (FAILED(hr)) {
        target->Release();
        return 0;
    }
    RECT rc = { 0 };
    // open the SVG as a document
    ID2D1SvgDocument* svg;
    hr = dc->CreateSvgDocument(ipImageStream, size, &svg);
    if (FAILED(hr)) {
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
            rc.right = (LONG)svgWidth;
            rc.bottom = (LONG)svgHeight;
            size.width = svgWidth;
            size.height = svgHeight;
        }
    }

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
    if (hbmp == NULL) {
        return NULL;
    }
    HDC hdc = CreateCompatibleDC(hdcScreen);
    ReleaseDC(NULL, hdcScreen);

    auto hbmpOld = SelectObject(hdc, hbmp);
    if (hbmp != NULL) {
        target->BindDC(hdc, &rc);
        // draw it on the render target
        target->BeginDraw();
        target->Clear();
        dc->DrawSvgDocument(svg);
        hr = target->EndDraw();
    }
    target->Release();
    if (hbmpOld != nullptr) {
        SelectObject(hdc, hbmpOld);
    }
    ReleaseDC(0, hdc);
    DeleteDC(hdc);
    ReleaseDC(NULL, hdcScreen);

    return hbmp;
}

static GUID loadimage_getTypeFromContent(char* pszData, UINT cbSize) {
    GUID guid = { 0 };
    if (cbSize < 5) {
        return guid;
    }
    if (pszData[0] == '<' && (strncmp(pszData, "<svg", 4) == 0 || strncmp(pszData, "<?xml", 5) == 0)) {
        return CLSID_SvgDecoder;
    }
    if (strncmp(pszData, "GIF8", 4) == 0) {
        return CLSID_WICGifDecoder;
    }
    if (pszData[0] == 0xff && pszData[1] == 0xd8) {
        return CLSID_WICJpegDecoder;
    }
    if (pszData[0] == 0xff && pszData[1] == 0xd8) {
        return CLSID_WICTiffDecoder;
    }
    if (pszData[0] == 0x49 && pszData[1] == 0x49 && pszData[2] == 0x2a && pszData[3] == 0x00) {
        return CLSID_WICTiffDecoder;
    }
    if (strncmp(pszData, "RIFF", 4) == 0) {
        return CLSID_WICWebpDecoder;
    }
    if (pszData[0] == 0x89 && pszData[1] == 0x50 && pszData[2] == 0x4e && pszData[3] == 0x47) {
        return CLSID_WICPngDecoder;
    }
    return guid;
}
/*
 * Loads an image with a given name into a HBITMAP trying various formats for loading the image.
 * If the image format is not supported return 0.
 */
extern "C" __declspec(dllexport) HBITMAP loadimage_load(char* pszName, IMAGE_LOAD_ASYNC pAsyncCallback) {
    HBITMAP hbmpImage = NULL;
    char* pszLoaded = NULL;
    GUID guid;
    URL url;
    // load the PNG image data into a stream
    UINT cbSize;
    if (loadimage_parseURL(pszName, &url)) {
        pszLoaded = http_loadData(&url, &cbSize, &pAsyncCallback);
        url_free(&url);
        if (pszLoaded == NULL) {
            return NULL;
        }
        guid = loadimage_getTypeFromContent(pszLoaded, cbSize);
    } else {
        guid = loadimage_getType(pszName);
    }

    if (guid.Data1 == 0) {
        free(pszLoaded);
        return NULL;
    }
    IStream* ipImageStream = pszLoaded == NULL ? CreateStreamOnResource(pszName) : SHCreateMemStream((const BYTE*)pszLoaded, cbSize);
    if (ipImageStream == NULL) {
        free(pszLoaded);
        return NULL;
    }

    if (guid == CLSID_SvgDecoder) {
        hbmpImage = load_bitmapFromSVG(ipImageStream);
    } else {
        // load the bitmap with WIC
        IWICBitmapSource* ipBitmap = LoadBitmapFromStream(ipImageStream, guid);
        if (ipBitmap != NULL) {
            // create a HBITMAP containing the image
            hbmpImage = CreateHBITMAP(ipBitmap);
            ipBitmap->Release();
        }
    }

    ipImageStream->Release();
    return hbmpImage;
}