/*
 * DrawEmojiD2D.cpp
 *
 * Project: PKS Edit for Windows
 *
 * purpose: perform some draw 2D operations for nicer rendering.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "pch.h"
#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>

struct DWriteSupport {
    ID2D1Factory* d2d_factory = NULL;
    IDWriteFactory* dwrite_factory = NULL;
    ID2D1DCRenderTarget* renderTarget = NULL;
    FLOAT m_width;
    FLOAT m_height;
    HDC m_hdc = NULL;

    DWriteSupport()
    {
    }

    ~DWriteSupport() {
    }

    HRESULT Setup(HDC hdc) {
        HWND hwnd = WindowFromDC(hdc);
        HRESULT hr;
        if (!d2d_factory) {
            hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
            if (FAILED(hr)) {
                return hr;
            }
        }
        if (!renderTarget) {
            if (renderTarget) {
                renderTarget->Release();
                renderTarget = NULL;
            }
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, 
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            hr = d2d_factory->CreateDCRenderTarget(&props, &renderTarget);
            if (FAILED(hr)) {
                return hr;
            }
        }
        if (hdc != m_hdc) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            m_width = (FLOAT)(rc.right - rc.left);
            m_height = (FLOAT)(rc.bottom - rc.top);
            renderTarget->BindDC(hdc, &rc);
            m_hdc = hdc;
        }

        if (!dwrite_factory) {
            hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwrite_factory));
            if (FAILED(hr)) {
                return hr;
            }
        }

        return S_OK;
    }

    D2D1_SIZE_F GetSize() {
        return renderTarget->GetSize();
    }

    D2D1_SIZE_F GetTextExtent(IDWriteTextFormat* textFormat, LPCWSTR text, INT count, FLOAT cx, FLOAT cy) {
        IDWriteTextLayout* write_text_layout = NULL;
        dwrite_factory->CreateGdiCompatibleTextLayout(text, count, textFormat, cx, cy, 96, NULL, TRUE, &write_text_layout);
        if (!write_text_layout)
            return { 0, 0 };

        DWRITE_TEXT_METRICS metrics;
        write_text_layout->GetMetrics(&metrics);

        if (write_text_layout) {
            write_text_layout->Release();
            write_text_layout = NULL;
        }

        return { metrics.width, metrics.height };
    }

    void DrawText(
        IDWriteTextFormat* textFormat,
        LPCWSTR text,
        INT count,
        ID2D1SolidColorBrush* brush,
        D2D1_RECT_F rect) {
        auto flags = (D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        renderTarget->DrawText(text, count, textFormat, rect, brush, flags, DWRITE_MEASURING_MODE_GDI_CLASSIC);
    }

};

static DWriteSupport g_dwrite;

/////////////////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) void paint_emojid2d(HDC hdc, WCHAR* emoji, COLORREF cColor, int fontSize, int x, int y, int* pWidth, int* pHeight) {
    if (g_dwrite.Setup(hdc) != S_OK) {
        return;
    }
    ID2D1SolidColorBrush* whiteBrush = NULL;
    // Start rendering
    g_dwrite.renderTarget->BeginDraw();
    // Calculate the client area
    D2D1_RECT_F rect = D2D1::RectF((FLOAT) x, (FLOAT) y, g_dwrite.m_width, g_dwrite.m_height);

    // Create a text format
    HRESULT hr;
    IDWriteTextFormat* textFormat = NULL;
    hr = g_dwrite.dwrite_factory->CreateTextFormat(L"Segoe UI Emoji",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT) fontSize,
        L"",
        &textFormat);

    ::D2D1_COLOR_F color;
    color.r = 0.0f;
    color.g = 0.0f;
    color.b = 0.0f;
    color.a = 1.0f;
    g_dwrite.renderTarget->CreateSolidColorBrush(color, &whiteBrush);
    if (SUCCEEDED(hr)) {
        // The text and its length
        auto text = emoji;
        INT count = lstrlenW(text);

        // Draw the text
        D2D1_SIZE_F extent = g_dwrite.GetTextExtent(textFormat, text, count, g_dwrite.m_width, g_dwrite.m_height);
        *pWidth = (int)extent.width;
        *pHeight = (int)extent.height;
        rect.right = rect.left+extent.width+2;
        rect.bottom = rect.top+extent.height+2;
        g_dwrite.DrawText(textFormat, text, count, whiteBrush, rect);
    }

    // Release resources
    if (whiteBrush) {
        whiteBrush->Release();
        whiteBrush = NULL;
    }
    if (textFormat) {
        textFormat->Release();
        textFormat = NULL;
    }

    // End rendering
    g_dwrite.renderTarget->EndDraw();
}

