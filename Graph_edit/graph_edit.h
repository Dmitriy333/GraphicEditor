#pragma once

#include <windows.h>
#include <tchar.h>
#include <time.h>    
#include <string>
#include <wingdi.h>
#include <xstring>
#include "figures.h"
#include "drawing.h"
#include "resource.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > String;
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