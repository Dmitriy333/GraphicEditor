#include "drawing.h"

void Drawing::initializeDcs(HWND &hWnd, 
							HDC &mainDc, 
							HDC &currentDc, 
							HBITMAP &currentBitmap,
							HDC &bufferDc,
							HBITMAP &bufferBitmap)
{
	RECT rect;
	HBRUSH brush;
	HPEN pen;

	mainDc = GetDC(hWnd);
	GetClientRect(hWnd, &rect);

	brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	pen = (HPEN)GetStockObject(BLACK_PEN);

	currentDc = CreateCompatibleDC(mainDc);
	currentBitmap = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);
	bufferDc = CreateCompatibleDC(mainDc);
	bufferBitmap = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);

	DeleteObject(SelectObject(currentDc, currentBitmap));
	DeleteObject(SelectObject(currentDc, (HBRUSH)WHITE_BRUSH));
	PatBlt(currentDc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(bufferDc, bufferBitmap));
	DeleteObject(SelectObject(bufferDc, (HBRUSH)WHITE_BRUSH));
	PatBlt(bufferDc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(currentDc, brush));
	DeleteObject(SelectObject(bufferDc, pen));
	DeleteObject(SelectObject(bufferDc, brush));
}

void Drawing::useRubber(HWND &hWnd,
				CustomRubber *rubber, 
				int x, int y, 
				HDC &currentDc, HDC &bufferDc, 
				draw &drawMode, 
				HBRUSH &oldBrush, 
				INT penWidth, INT rubberWidth, COLORREF color)
{
	RECT rect;
	HPEN pen;

	pen = CreatePen(PS_SOLID, rubberWidth, RGB(255, 255, 255));
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));

	GetClientRect(hWnd, &rect);
	rubber->draw(bufferDc, x, y);
	BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
	drawMode = BUFFER;
	InvalidateRect(hWnd, NULL, FALSE);

	pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));

	rubber->ellipse(currentDc, x, y, rubberWidth);
	drawMode = CURRENT;
	InvalidateRect(hWnd, NULL, FALSE);

	pen = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
	DeleteObject(SelectObject(currentDc, pen));
	DeleteObject(SelectObject(bufferDc, pen));
}