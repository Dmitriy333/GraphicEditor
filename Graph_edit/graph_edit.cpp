#include "graph_edit.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		1920, 1080,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	static HDC mainDc, paintDc, currentDc = 0, bufferDc = 0;
	static HBITMAP currentBitmap, bufferBitmap;
	static HBRUSH brush;
	static COLORREF penColor;
	static INT penWidth;
	static draw drawMode;
	static CustomShape* shape;
	static CustomRubber* rubber;
	static BOOL isPencil = FALSE;

	switch (message)
	{
	case WM_CREATE:
		initializeDcs(hWnd, mainDc, currentDc, currentBitmap, bufferDc, bufferBitmap);
		break;

	case WM_LBUTTONDOWN:
		isPencil = TRUE;
		if (isPencil)
		{
			shape = new CustomPencil((short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = BUFFER;
		}
		else
		{
			shape = new CustomLine((short)LOWORD(lParam), (short)HIWORD(lParam));
			//shape = new CustomRectangle((short)LOWORD(lParam), (short)HIWORD(lParam));
			//shape = new CustomEllipse((short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = CURRENT;
		}
		SetCapture(hWnd);
		break;

	case WM_RBUTTONDOWN:
		rubber = new CustomRubber((short)LOWORD(lParam), (short)HIWORD(lParam));
		useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, penColor);
		SetCapture(hWnd);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			if (isPencil)
			{
				shape->draw(bufferDc, (short)LOWORD(lParam), (short)HIWORD(lParam));
				drawMode = BUFFER;
			}
			else
			{
				GetClientRect(hWnd, &rect);
				BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
				shape->draw(currentDc, (short)LOWORD(lParam), (short)HIWORD(lParam));
				drawMode = BUFFER;
			}
		}
		else if (wParam & MK_RBUTTON)
		{
			useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, penColor);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		if (!isPencil)
		{
			GetClientRect(hWnd, &rect);
			shape->draw(bufferDc, (short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		delete shape;
		break;

	case WM_RBUTTONUP:
		ReleaseCapture();
		GetClientRect(hWnd, &rect);
		BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
		delete rubber;
		break;

	case WM_PAINT:
		paintDc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		switch (drawMode)
		{
		case CURRENT:
			BitBlt(paintDc, 0, 0, rect.right, rect.bottom, currentDc, 0, 0, SRCCOPY);
			break;

		case BUFFER:
			BitBlt(paintDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			break;
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEWHEEL:
		HPEN pen;
		penWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
		if (penWidth < 0)
			penWidth = 0;
		pen = CreatePen(PS_SOLID, penWidth, penColor);
		DeleteObject(SelectObject(currentDc, pen));
		DeleteObject(SelectObject(bufferDc, pen));
		if (wParam & MK_RBUTTON)
		{
			useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, penColor);
		}
		break;

	case WM_DESTROY:
		ReleaseDC(hWnd, mainDc);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

