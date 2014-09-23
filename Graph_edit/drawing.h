#pragma once

#include "figures.h"

namespace Drawing
{
	void initializeDcs(HWND &hWnd,
		HDC &mainDc,
		HDC &currentDc,
		HBITMAP &currentBitmap,
		HDC &bufferDc,
		HBITMAP &bufferBitmap,
		HDC& backupDc,
		HBITMAP& backupBitmap);

	void useRubber(HWND &hWnd,
		CustomRubber *rubber,
		int x,
		int y,
		HDC &currentDc,
		HDC &bufferDc,
		draw &drawMode);
}