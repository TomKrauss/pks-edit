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
#include <Windows.h>
#include <tchar.h>
#include <wincodec.h>
#include <Shlwapi.h>
#include <Winhttp.h>
#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>
#include <d2d1_3.h>

typedef struct tagURL {
    BOOL   url_https;
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

static char* http_loadData(URL* pURL, UINT* pLoaded) {
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer = 0;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;

    *pLoaded = 0;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"PKS-Edit Image/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        return 0;
    }
    // Specify an HTTP server.
    hConnect = WinHttpConnect(hSession, pURL->url_host, pURL->url_https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);

    // Create an HTTP request handle.
    if (hConnect) {
        hRequest = WinHttpOpenRequest(hConnect, L"GET", pURL->url_pathAndParams,
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

    if (!bResults) {
        return 0;
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
                    memset(pszOutBuffer + nOffset, 0, nTotal - nOffset);
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
        char* ret = http_loadData(&url, pSize);
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
    if (pszData[0] == 0x49 && pszData[1] == 0x49 && pszData[2] == 0x2a && pszData[2] == 0x2a) {
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
extern "C" __declspec(dllexport) HBITMAP loadimage_load(char* pszName) {
    HBITMAP hbmpImage = NULL;
    char* pszLoaded = NULL;
    GUID guid;
    URL url;
    // load the PNG image data into a stream
    UINT cbSize;
    if (loadimage_parseURL(pszName, &url)) {
        pszLoaded = http_loadData(&url, &cbSize);
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
        if (ipBitmap == NULL)
            goto ReleaseStream;

        // create a HBITMAP containing the image

        hbmpImage = CreateHBITMAP(ipBitmap);
        ipBitmap->Release();
    }

ReleaseStream:
    ipImageStream->Release();
    return hbmpImage;
}