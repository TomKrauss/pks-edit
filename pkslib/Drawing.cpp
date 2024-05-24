/*
 * Drawing.cpp
 *
 * Project: PKS Edit for Windows
 *
 * purpose: perform some GDI operations for nicer rendering.
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
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

typedef struct tagCHAR_WITH_STYLE {
	wchar_t symbol;
	int regular;
} CHAR_WITH_STYLE;

static void paint_initGdiPlus() {
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR           gdiplusToken;

	if (gdiplusToken == 0) {
		// Initialize GDI.
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}
}

static Gdiplus::PrivateFontCollection* awesomeFontCollection;

extern "C" __declspec(dllexport) BOOL paint_loadFontAwesome(void* pFontData, DWORD len) {
	paint_initGdiPlus();
	static PrivateFontCollection myColl;
	auto sRet = myColl.AddMemoryFont(pFontData, len);
	awesomeFontCollection = &myColl;
	return sRet == Ok;
}

/*
 * Draws a text in an antialiased way. 
 */
static void paint_awesomeIcons(HDC hdc, const CHAR_WITH_STYLE* pszText, int nLen, COLORREF cColor, int x, int y, int nIconSize) {
	paint_initGdiPlus();
	Gdiplus::Graphics  graphics(hdc);
	SolidBrush  brush(Color(255, GetRValue(cColor), GetGValue(cColor), GetBValue(cColor)));
	PointF pointF((float)x, (float)y);
	FontFamily families[2];
	int found = (*awesomeFontCollection).GetFamilyCount();
	WCHAR familyName[LF_FACESIZE] = { 0 };
	WCHAR familyName2[LF_FACESIZE] = { 0 };
	// Currently the free awesome font is not correctly handled as a font family, but the two font files are loaded
	// as two font families.
	if (found > 0) {
		(*awesomeFontCollection).GetFamilies(found, families, &found);
		families[found-1].GetFamilyName(familyName);
		families[0].GetFamilyName(familyName2);
	}
	Gdiplus::Font myFontSolid(familyName, (float)(nIconSize - 2), FontStyleRegular, UnitPixel, awesomeFontCollection);
	Gdiplus::Font myFontRegular(familyName2, (float)(nIconSize - 2), FontStyleBold, UnitPixel, awesomeFontCollection);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	int delta = nIconSize / 8;
	x -= delta;
	for (int i = 0; i < nLen; i++) {
		pointF.X = x + (float)nIconSize * i;

		// Work around for the fact, that the FA Icons are not quadratic: clip excessively wide characters
		Region region(Rect((int)pointF.X, (int)pointF.Y, (int)pointF.Y+nIconSize+delta, (int)pointF.Y + nIconSize));
		HRGN hRegion = region.GetHRGN(&graphics);
		graphics.SetClip(hRegion);
		CHAR_WITH_STYLE cs = pszText[i];
		auto s = graphics.DrawString(&cs.symbol, 1, cs.regular ? &myFontRegular : &myFontSolid, pointF, &brush);
		DeleteObject(hRegion);
		if (s != Ok) {
			return;
		}
	}
}

/*
 * Draws a rounded rectangle in an antialiased way.
 */
extern "C" __declspec(dllexport) void paint_roundedRect(HDC hdc, COLORREF cColor, float penWidth, int x, int y, int width, int height, int radius) {
	paint_initGdiPlus();
	Gdiplus::Graphics  graphics(hdc);
	Pen  pen(Color(255, GetRValue(cColor), GetGValue(cColor), GetBValue(cColor)), penWidth);
	GraphicsPath gp;

	width -= (int)(penWidth / 2);
	height -= (int)(penWidth / 2);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	gp.AddArc(x, y, radius, radius, 180, 90);
	gp.AddArc(x + width - radius, y, radius, radius, 270, 90);
	gp.AddArc(x + width - radius, y + height - radius, radius, radius, 0, 90);
	gp.AddArc(x, y + height - radius, radius, radius, 90, 90);
	gp.CloseAllFigures();
	graphics.DrawPath(&pen, &gp);
}

extern "C" __declspec(dllexport) HBITMAP tb_createAwesomeIcons(COLORREF nColorRef, int nSize, CHAR_WITH_STYLE icons[], int nIcons) {
    HDC hdcScreen = GetDC(0);
    HDC hdc = CreateCompatibleDC(hdcScreen);

    int nWidth = nIcons * nSize;
    int nBitcount = 32;
    int size = ((((nWidth * nBitcount) + 31) & ~31) >> 3) * nSize;

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    bmi.bmiHeader.biHeight = -nSize;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = nBitcount;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = size;

    BYTE* pvBits = NULL;
    HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,
        (void**)&pvBits, NULL, 0x0);
    auto hBmpOld = hBmp == nullptr ? nullptr : SelectObject(hdc, hBmp);
    RECT rect;
    rect.top = 0;
    rect.bottom = nSize;
    rect.left = 0;
    rect.right = rect.left + nWidth;
    paint_awesomeIcons(hdc, icons, nIcons, nColorRef, rect.left, 0, nSize);
	if (hBmpOld != nullptr) {
		SelectObject(hdc, hBmpOld);
	}
    ReleaseDC(0, hdc);
    DeleteDC(hdc);
    ReleaseDC(NULL, hdcScreen);
    return hBmp;
}

