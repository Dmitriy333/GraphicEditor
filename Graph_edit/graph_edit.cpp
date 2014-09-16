#include "graph_edit.h"

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

	ShowWindow(hWnd, SW_MAXIMIZE);
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
	static HDC mainDc, paintDc, currentDc = 0, bufferDc = 0, backupDc = 0;
	static HBITMAP currentBitmap, bufferBitmap, backupBitmap;
	static HBRUSH brush;
	static COLORREF penColor;
	static INT penWidth;
	static INT rubberWidth = 10;
	static draw drawMode;
	static CustomShape* shape;
	static CustomRubber* rubber;
	static BOOL isPencil = TRUE;
	static INT ToolId = 0; //Identificator of tool: 0 - pen, 1 - line, 2 - rectangle, 3 - ellipse, 4 - poly, 5 - text
	static INT prevX = -1, prevY = -1, startX = -1, startY = -1; //Using for polyline and polygone
	static BOOL isPolyLine; //Use for identification: polyline or polygone
	static HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	static string text;

	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TOOLS_PEN:
			isPencil = TRUE;
			ToolId = 0;
			break;

		case ID_TOOLS_LINE:
			isPencil = FALSE;
			ToolId = 1;
			break;

		case ID_TOOLS_RECTANGLE:
			isPencil = FALSE;
			ToolId = 2;
			break;

		case ID_TOOLS_ELLIPSE:
			isPencil = FALSE;
			ToolId = 3;
			break;

		case ID_TOOLS_POLYGONE:
			isPencil = FALSE;
			isPolyLine = FALSE;
			prevX = -1;
			prevY = -1;
			ToolId = 4;
			break;

		case ID_TOOLS_POLYLINE:
			isPencil = FALSE;
			isPolyLine = TRUE;
			prevX = -1;
			prevY = -1;
			ToolId = 4;
			break;

		case ID_FILE_NEW:
			initializeDcs(hWnd, mainDc, currentDc, currentBitmap, bufferDc, bufferBitmap, backupDc, backupBitmap);
			break;

		case ID_TOOLS_TEXT:
			isPencil = FALSE;
			ToolId = 5;
			break;

		case ID_FILE_CANCEL:
			drawMode = BACKUP;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;
	case WM_CREATE:
		initializeDcs(hWnd, mainDc, currentDc, currentBitmap, bufferDc, bufferBitmap, backupDc, backupBitmap);
		break;

	case WM_LBUTTONDOWN:
		if (isPencil)
		{
			GetClientRect(hWnd, &rect);
			shape = new CustomPencil((short)LOWORD(lParam), (short)HIWORD(lParam));
			BitBlt(backupDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			shape->draw(bufferDc, (short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = BUFFER;
		}
		else
		{
			switch (ToolId)
			{
			case 1:
				shape = new CustomLine((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case 2:
				shape = new CustomRectangle((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case 3:
				shape = new CustomEllipse((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case 4:
				if (prevX == -1 && prevY == -1)
				{
					prevX = (short)LOWORD(lParam);
					prevY = (short)HIWORD(lParam);
					startX = prevX;
					startY = prevY;
				}
				shape = new CustomLine(prevX, prevY);
				break;

			case 5:
				prevX = (short)LOWORD(lParam);
				prevY = (short)HIWORD(lParam);
				text = ' ';
				break;
			}
			drawMode = CURRENT;
		}
		SetCapture(hWnd);
		break;

	case WM_RBUTTONDOWN:
		GetClientRect(hWnd, &rect);
		rubber = new CustomRubber((short)LOWORD(lParam), (short)HIWORD(lParam));
		BitBlt(backupDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
		useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode, brush, penWidth, rubberWidth, penColor);
		SetCapture(hWnd);
		break;

	case WM_MOUSEMOVE:
		GetClientRect(hWnd, &rect);
		BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
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
			BitBlt(backupDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			shape->draw(bufferDc, (short)LOWORD(lParam), (short)HIWORD(lParam));

			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		delete shape;
		shape = NULL;
		break;

	case WM_LBUTTONDBLCLK:
		if (ToolId == 4)
		{
			shape = new CustomLine(prevX, prevY);
			ReleaseCapture();
			GetClientRect(hWnd, &rect);
			BitBlt(backupDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			if (!isPolyLine)
				shape->draw(bufferDc, startX, startY);
			else
				shape->draw(bufferDc, prevX, prevY);
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
			prevX = -1;
			prevY = -1;
			startX = -1;
			startY = -1;
			delete shape;
			shape = NULL;
		}
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

		case BACKUP:
			BitBlt(bufferDc, 0, 0, rect.right, rect.bottom, backupDc, 0, 0, SRCCOPY);
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
			GetClientRect(hWnd, &rect);
			BitBlt(backupDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
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
			GetClientRect(hWnd, &rect);
			BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			MoveToEx(currentDc, (short)LOWORD(lParam), (short)HIWORD(lParam), NULL);
			LineTo(currentDc, (short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = CURRENT;
			InvalidateRect(hWnd, NULL, FALSE); 
		}
		break;

	case WM_CHAR:
		if (ToolId == 5)
		{
			GetClientRect(hWnd, &rect);
			char c = char(wParam);
			text += c;
			TextOut(bufferDc, prevX, prevY, (LPCWSTR)text.c_str(), strlen(text.c_str()));
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
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

