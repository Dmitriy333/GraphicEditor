#include "drawing.h"

void Drawing::initializeDcs(HWND &hWnd,
	HDC &mainDc,
	HDC &currentDc,
	HBITMAP &currentBitmap,
	HDC &bufferDc,
	HBITMAP &bufferBitmap)
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

void Drawing::initializeBackup(HWND &hWnd,
	HDC &mainDc,
	HDC (&backupDc)[BACKUPS],
	HBITMAP (&backupBitmap)[BACKUPS])
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	for (int i = 0; i < BACKUPS; i++)
	{
		backupDc[i] = CreateCompatibleDC(mainDc);
		backupBitmap[i] = CreateCompatibleBitmap(mainDc, rect.right, rect.bottom);
		DeleteObject(SelectObject(backupDc[i], backupBitmap[i]));
		DeleteObject(SelectObject(backupDc[i], (HBRUSH)WHITE_BRUSH));
		PatBlt(backupDc[i], 0, 0, rect.right, rect.bottom, PATCOPY);
	}
}

void Drawing::createBackup(HWND &hWnd,
	int &backupDepth,
	int &restoreCount,
	HDC &bufferDc,
	HDC (&backupDc)[BACKUPS])
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	restoreCount = 0;

	if (backupDepth < (BACKUPS-1))
	{
		backupDepth++;
		BitBlt(backupDc[backupDepth], 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
	}
	else
	{
		for (int i = 0; i < backupDepth; i++)
		{
			BitBlt(backupDc[i], 0, 0, rect.right, rect.bottom, backupDc[i+1], 0, 0, SRCCOPY);
		}
		BitBlt(backupDc[backupDepth], 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
	}
}

void Drawing::undo(HWND hWnd,
	int &backupDepth,
	int &restoreCount,
	HDC &bufferDc,
	HDC (&backupDc)[BACKUPS])
{
	if (backupDepth > 0)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		backupDepth--;
		BitBlt(bufferDc, 0, 0, rect.right, rect.bottom, backupDc[backupDepth], 0, 0, SRCCOPY);
		restoreCount++;
	}
}

void Drawing::restore(HWND hWnd,
	int &backupDepth,
	int &restoreCount,
	HDC &bufferDc,
	HDC(&backupDc)[BACKUPS])
{
	if (restoreCount > 0)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		backupDepth++;
		BitBlt(bufferDc, 0, 0, rect.right, rect.bottom, backupDc[backupDepth], 0, 0, SRCCOPY);
		restoreCount--;
	}
}