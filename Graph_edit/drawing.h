#pragma once

#include "figures.h"

namespace Drawing
{
	void initializeDcs(HWND &hWnd,
		HDC &mainDc,
		HDC &currentDc,
		HBITMAP &currentBitmap,
		HDC &bufferDc,
		HBITMAP &bufferBitmap);

	void useRubber(HWND &hWnd,
		CustomRubber *rubber,
		int x,
		int y,
		HDC &currentDc,
		HDC &bufferDc,
		draw &drawMode);

	void initializeBackup(HWND &hWnd,
		HDC &mainDC,
		HDC (&backupDc)[BACKUPS],
		HBITMAP (&backupBitmap)[BACKUPS]);

	void createBackup(HWND &hWnd,
		int &backupDepth,
		int &restoreCount,
		HDC &bufferDc,
		HDC (&backupDc)[BACKUPS]);

	void undo(HWND hWnd,
		int &backupDepth,
		int &restoreCount,
		HDC &bufferDc,
		HDC (&backupDc)[BACKUPS]);

	void restore(HWND hWnd,
		int &backupDepth,
		int &restoreCount,
		HDC &bufferDc,
		HDC(&backupDc)[BACKUPS]);
}