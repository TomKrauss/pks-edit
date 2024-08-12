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

extern ID2D1Factory* create_d2dFactory();

struct DWriteSupport {
    ID2D1Factory* d2d_factory = NULL;
    IDWriteFactory* dwrite_factory = NULL;
    ID2D1DCRenderTarget* renderTarget = NULL;
    IDWriteTextFormat* m_textFormat = NULL;
    ID2D1SolidColorBrush* m_whiteBrush = NULL;
    FLOAT m_fontSize = 0;
    FLOAT m_width = 0;
    FLOAT m_height = 0;
    HDC m_hdc = NULL;
    BOOL m_begin = FALSE;

    DWriteSupport()
    {
    }

    ~DWriteSupport() {
    }

    HRESULT Setup(HDC hdc, float fontSize) {
        HWND hwnd = WindowFromDC(hdc);
        HRESULT hr;
        d2d_factory = create_d2dFactory();
        if (!d2d_factory) {
            return MAKE_HRESULT(1, FACILITY_GRAPHICS, -1);
        }
        if (!renderTarget) {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, 
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            hr = d2d_factory->CreateDCRenderTarget(&props, &renderTarget);
            if (FAILED(hr)) {
                return hr;
            }
        }

        if (m_whiteBrush == NULL) {
            ::D2D1_COLOR_F color;
            color.r = 0.0f;
            color.g = 0.0f;
            color.b = 0.0f;
            color.a = 1.0f;
            hr = renderTarget->CreateSolidColorBrush(color, &m_whiteBrush);
            if (FAILED(hr)) {
                return hr;
            }
        }

        BOOL bHdcChanged = hdc != m_hdc;
        if (bHdcChanged) {
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

        if (bHdcChanged || fontSize != m_fontSize || m_textFormat == NULL) {
            if (m_textFormat != NULL) {
                m_textFormat->Release();
                m_textFormat = NULL;
            }
            hr = dwrite_factory->CreateTextFormat(L"Segoe UI Emoji",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                fontSize,
                L"",
                &m_textFormat);
            if (FAILED(hr)) {
                return hr;
            }
            m_fontSize = fontSize;
        }
        return S_OK;
    }

    void BeginDraw() {
        if (!m_begin && renderTarget) {
            renderTarget->BeginDraw();
            m_begin = TRUE;
        }
    }

    void EndDraw() {
        if (m_begin && renderTarget) {
            renderTarget->EndDraw();
            m_begin = FALSE;
        }
    }

    D2D1_SIZE_F GetSize() {
        return renderTarget->GetSize();
    }

    D2D1_SIZE_F GetTextExtent(IDWriteTextFormat* textFormat, LPCWSTR text, INT count, FLOAT cx, FLOAT cy) {
        IDWriteTextLayout* write_text_layout = NULL;
        dwrite_factory->CreateGdiCompatibleTextLayout(text, count, textFormat, cx, cy, 1, NULL, TRUE, &write_text_layout);
        if (!write_text_layout)
            return { 0, 0 };

        DWRITE_TEXT_METRICS metrics;
        write_text_layout->GetMetrics(&metrics);
        write_text_layout->Release();

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
    if (g_dwrite.Setup(hdc, (FLOAT) fontSize) != S_OK) {
        return;
    }
    // Start rendering
    g_dwrite.BeginDraw();
    // Calculate the client area
    D2D1_RECT_F rect = D2D1::RectF((FLOAT) x, (FLOAT) y, g_dwrite.m_width, g_dwrite.m_height);

    // The text and its length
    auto text = emoji;
    INT count = lstrlenW(text);

    // Draw the text
    D2D1_SIZE_F extent = g_dwrite.GetTextExtent(g_dwrite.m_textFormat, text, count, g_dwrite.m_width, g_dwrite.m_height);
    *pWidth = (int)extent.width;
    *pHeight = (int)extent.height;
    rect.right = rect.left+extent.width+2;
    rect.bottom = rect.top+extent.height+2;
    g_dwrite.DrawText(g_dwrite.m_textFormat, text, count, g_dwrite.m_whiteBrush, rect);
}


extern "C" __declspec(dllexport) void paint_endEmoji() {
    g_dwrite.EndDraw();
}

