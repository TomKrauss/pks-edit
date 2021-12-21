#include "pch.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

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
extern "C" __declspec(dllexport) void paint_textAntialised(HDC hdc, WCHAR* pszText, int nLen, COLORREF cColor, int x, int y) {
	paint_initGdiPlus();
	Gdiplus::Graphics  graphics(hdc);
	SolidBrush  brush(Color(255, GetRValue(cColor), GetGValue(cColor), GetBValue(cColor)));
	PointF      pointF((float)x, (float)y);
	Gdiplus::Font myFont(L"Font Awesome 5 Free Solid", 16, FontStyleRegular, UnitPixel, awesomeFontCollection);

	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	for (int i = 0; i < nLen; i++) {
		pointF.X = x + (float)18 * i - 1;
		auto s = graphics.DrawString(&pszText[i], 1, &myFont, pointF, &brush);
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

