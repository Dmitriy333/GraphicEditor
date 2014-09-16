#include "graph_edit.h"
#include "resource.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
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
		800, 600,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	//Buttons for choose tools and functions
	//Pen
	//HWND btnPen = CreateWindow(L"button", L"Pen", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 80, 20, hWnd, (HMENU)10000, hInstance, NULL);
	//Line
	//HWND btnLine = CreateWindow(L"button", L"Line", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 30, 80, 20, hWnd, (HMENU)10001, hInstance, NULL);
	//Polygone
	//HWND btnPolyGone = CreateWindow(L"button", L"PolyGone", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 90, 80, 20, hWnd, (HMENU)10004, hInstance, NULL);
	//Polyline
	//HWND btnPolyLine = CreateWindow(L"button", L"PolyLine", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 110, 80, 20, hWnd, (HMENU)10005, hInstance, NULL);
	//Rectangle
	//HWND btnRectangle = CreateWindow(L"button", L"Rectangle", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 50, 80, 20, hWnd, (HMENU)10002, hInstance, NULL);
	//Ellipse
	//HWND btnEllipse = CreateWindow(L"button", L"Ellipse", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 80, 20, hWnd, (HMENU)10003, hInstance, NULL);

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
	static INT rubberWidth = 10;
	static draw drawMode;
	static CustomShape* shape;
	static CustomRubber* rubber;
	static BOOL isPencil = TRUE;
	static INT ToolId = 0; //Identificator of tool: 0 - pen, 1 - line, 2 - rectangle, 3 - ellipse, 4 - polyline, 5 - polygone
	static INT prevX = -1, prevY = -1, startX = -1, startY = -1; //Using for polyline and polygone
	static BOOL isPolyLine; //Use for identification: polyline or polygone

	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 40001:
		{
			isPencil = TRUE;
			ToolId = 0;
			break;
		}
		case 40002:
		{
			isPencil = FALSE;
			ToolId = 1;
			break;
		}
		case 40005:
		{
			isPencil = FALSE;
			ToolId = 2;
			break;
		}
		case 40007:
		{
			isPencil = FALSE;
			ToolId = 3;
			break;
		}
		case 40004:
		{
			isPencil = FALSE;
			isPolyLine = FALSE;
			prevX = -1;
			prevY = -1;
			ToolId = 4;
			break;
		}
		case 40003:
		{
			isPencil = FALSE;
			isPolyLine = TRUE;
			prevX = -1;
			prevY = -1;
			ToolId = 5;
			break;
		}
		}
		break;
	case WM_CREATE:
		initializeDcs(hWnd, mainDc, currentDc, currentBitmap, bufferDc, bufferBitmap);
		break;

	case WM_LBUTTONDOWN:
		if (isPencil)
		{
			shape = new CustomPencil((short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = BUFFER;
		}
		else
		{
			switch (ToolId)
			{
			case 1:
			{
				shape = new CustomLine((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;
			}
			case 2:
			{
				shape = new CustomRectangle((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;
			}
			case 3:
			{
				shape = new CustomEllipse((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;
			}
			case 4: //Polygone
			{
				if (prevX == -1 && prevY == -1)
				{
					prevX = (int)LOWORD(lParam);
					prevY = (int)HIWORD(lParam);
					startX = prevX;
					startY = prevY;
				}
				shape = new CustomLine(prevX, prevY);
				break;
			}
			case 5: //Polyline
			{
				if (prevX == -1 && prevY == -1)
				{
					prevX = (int)LOWORD(lParam);
					prevY = (int)HIWORD(lParam);
					startX = prevX;
					startY = prevY;
				}
				shape = new CustomLine(prevX, prevY);
				break;
			}
			}
			drawMode = CURRENT;
		}
		SetCapture(hWnd);
		break;

	case WM_RBUTTONDOWN:
		rubber = new CustomRubber((short)LOWORD(lParam), (short)HIWORD(lParam));
		useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, rubberWidth, penColor);
		SetCapture(hWnd);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			if (shape)
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
					drawMode = CURRENT;
				}
			}
		}
		else if (wParam & MK_RBUTTON)
		{
			useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, rubberWidth, penColor);
		}
		InvalidateRect(hWnd, NULL, FALSE);

		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		if (!isPencil && shape != NULL)
		{
			if (prevX != -1 && prevY != -1)
			{
				prevX = (int)LOWORD(lParam);
				prevY = (int)HIWORD(lParam);
			}
			GetClientRect(hWnd, &rect);
			shape->draw(bufferDc, (short)LOWORD(lParam), (short)HIWORD(lParam));

			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		delete shape;
		shape = NULL;
		break;
	case WM_LBUTTONDBLCLK:
		shape = new CustomLine(prevX, prevY);
		ReleaseCapture();
		if (!isPencil)
		{
			GetClientRect(hWnd, &rect);
			if (!isPolyLine)
				shape->draw(bufferDc, startX, startY);
			else
				shape->draw(bufferDc, prevX, prevY);
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		if (ToolId == 4 || ToolId == 5)
		{
			prevX = -1;
			prevY = -1;
			startX = -1;
			startY = -1;
		}
		delete shape;
		shape = NULL;
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
		if (wParam & MK_RBUTTON)
		{
			rubberWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (penWidth < 0)
				penWidth = 0;
			useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, rubberWidth, penColor);
		}
		else
		{
			penWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (penWidth < 0)
				penWidth = 0;
			pen = CreatePen(PS_SOLID, penWidth, penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
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

