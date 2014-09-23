#include "drawing.h"

void Drawing::initializeDcs(HWND &hWnd,
	HDC &mainDc,
	HDC &currentDc,
	HBITMAP &currentBitmap,
	HDC &bufferDc,
	HBITMAP &bufferBitmap,
	HDC& backupDc,
	HBITMAP& backupBitmap)
{
	RECT rect;
	HPEN pen;
	HBRUSH brush;

	mainDc = GetDC(hWnd);
	GetClientRect(hWnd, &rect);

	pen = (HPEN)GetStockObject(BLACK_PEN);
	brush = (HBRUSH)GetStockObject(NULL_BRUSH);

	currentDc = CreateCompatibleDC(mainDc);
	currentBitmap = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);
	bufferDc = CreateCompatibleDC(mainDc);
	bufferBitmap = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);
	backupDc = CreateCompatibleDC(mainDc);
	backupBitmap = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);

	DeleteObject(SelectObject(currentDc, currentBitmap));
	DeleteObject(SelectObject(currentDc, (HBRUSH)WHITE_BRUSH));
	PatBlt(currentDc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(bufferDc, bufferBitmap));
	DeleteObject(SelectObject(bufferDc, (HBRUSH)WHITE_BRUSH));
	PatBlt(bufferDc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(backupDc, backupBitmap));
	DeleteObject(SelectObject(backupDc, (HBRUSH)WHITE_BRUSH));
	PatBlt(bufferDc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(currentDc, brush));
	DeleteObject(SelectObject(bufferDc, pen));
	DeleteObject(SelectObject(bufferDc, brush));
	DeleteObject(SelectObject(backupDc, pen));
	DeleteObject(SelectObject(backupDc, brush));
}

void Drawing::useRubber(HWND &hWnd,
	CustomRubber *rubber,
	int x, int y,
	HDC &currentDc, HDC &bufferDc,
	draw &drawMode)
{
	RECT rect;
	HPEN pen;

	GetClientRect(hWnd, &rect);

	pen = CreatePen(PS_SOLID, CustomRubber::rubberWidth, CustomRubber::rubberColor);
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));

	rubber->draw(bufferDc, x, y);
	BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);

	pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));

	rubber->ellipse(currentDc, x, y, CustomRubber::rubberWidth);

	pen = CreatePen(PS_SOLID, CustomShape::penWidth, CustomShape::penColor);
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));

	drawMode = CURRENT;
	InvalidateRect(hWnd, NULL, FALSE);
}