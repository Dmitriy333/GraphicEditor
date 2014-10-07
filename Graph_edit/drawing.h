#pragma once

#include "figures.h"

static RECT rect;
static BOOL initRect = TRUE;

namespace Drawing
{
	void initializeDcs(HWND &hWnd,
		HDC &mainDc,
		HDC &currentDc,
		HDC &bufferDc
		);

	void useRubber(HWND &hWnd,
		CustomRubber *rubber,
		int x,
		int y,
		HDC &currentDc,
		HDC &bufferDc,
		draw &drawMode);

	void initializeBackup(HWND &hWnd,
		HDC &mainDC,
		HDC (&backupDc)[BACKUPS]);

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