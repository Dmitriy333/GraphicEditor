#pragma once

#include <windows.h>
#include <tchar.h>
#include <time.h>    
#include <string>
#include "figures.h"
#include "drawing.h"
#include "resource.h"

using namespace Drawing;
using namespace std;

static TCHAR szWindowClass[] = _T("Graphical Editor");

static TCHAR szTitle[] = _T("Graphical Editor");

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);