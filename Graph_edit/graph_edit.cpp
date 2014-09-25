#include "graph_edit.h"

enum Tools { PEN, LINE, RECTANGLE, ELLIPSE, POLY, TEXT };

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
	HACCEL hAccel = LoadAccelerators(hInstance, (LPCWSTR)IDR_ACCELERATOR1);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateAccelerator(hWnd, hAccel, &msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	static HDC mainDc, paintDc, currentDc = 0, bufferDc = 0, backupDc[BACKUPS];
	static INT backupDepth = -1, restoreCount = 0;
	static draw drawMode;
	static CustomShape* shape = NULL;
	static CustomRubber* rubber = NULL;
	static Tools ToolId = PEN;
	static INT prevX = -1, prevY = -1, startX = -1, startY = -1; //Using for polyline and polygone
	static BOOL isPolyLine; //Use for identification: polyline or polygone
	static HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	static string text;
	static POINT prevCoord;
	HPEN pen;
	HBRUSH brush;
	CHOOSECOLOR cc;
	COLORREF acrCustClr[16];

	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TOOLS_PEN:
			ToolId = PEN;
			break;

		case ID_TOOLS_LINE:
			ToolId = LINE;
			break;

		case ID_TOOLS_RECTANGLE:
			ToolId = RECTANGLE;
			break;

		case ID_TOOLS_ELLIPSE:
			ToolId = ELLIPSE;
			break;

		case ID_TOOLS_POLYGONE:
			isPolyLine = FALSE;
			prevX = -1;
			prevY = -1;
			ToolId = POLY;
			break;

		case ID_TOOLS_POLYLINE:
			isPolyLine = TRUE;
			prevX = -1;
			prevY = -1;
			ToolId = POLY;
			break;

		case ID_FILE_NEW:
			initializeDcs(hWnd, mainDc, currentDc, bufferDc);
			initializeBackup(hWnd, mainDc, backupDc);
			createBackup(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
			restoreCount = 0;
			break;

		case ID_TOOLS_TEXT:
			ToolId = TEXT;
			break;

		case ID_FILE_UNDO:
			drawMode = BACKUP;
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case ID_FILE_RESTORE:
			if (restoreCount > 0)
			{
				drawMode = RESTORE;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;

		case ID_FILE_EXIT:
			exit(0);

		case ID_PEN_COLOR:
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE)
			{
				HPEN pen;
				CustomShape::penColor = cc.rgbResult;
				pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
				DeleteObject(SelectObject(currentDc, pen));
				DeleteObject(SelectObject(bufferDc, pen));
			}
			break;

		case ID_STYLE_SOLID:
			CustomShape::penStyle = PS_SOLID;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_STYLE_DASH:
			CustomShape::penStyle = PS_DASH;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_STYLE_DOT:
			CustomShape::penStyle = PS_DOT;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_STYLE_DASH_DOT:
			CustomShape::penStyle = PS_DASHDOT;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_STYLE_DASH_DOT_DOT:
			CustomShape::penStyle = PS_DASHDOTDOT;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_STYLE_NONE:
			CustomShape::penStyle = PS_NULL;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			break;

		case ID_RUBBER_COLOR:
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE)
			{
				CustomRubber::rubberColor = cc.rgbResult;
			}
			break;

		case ID_BRUSH_COLOR:
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE)
			{
				brush = CreateSolidBrush(cc.rgbResult);
				DeleteObject(SelectObject(currentDc, brush));
				DeleteObject(SelectObject(bufferDc, brush));
			}
			break;

		case ID_BRUSH_NONE:
			brush = (HBRUSH)GetStockObject(NULL_BRUSH);
			DeleteObject(SelectObject(currentDc, brush));
			DeleteObject(SelectObject(bufferDc, brush));
			break;

		}
		break;
	case WM_CREATE:
		initializeDcs(hWnd, mainDc, currentDc, bufferDc);
		initializeBackup(hWnd, mainDc, backupDc);
		createBackup(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
		restoreCount = 0;
		break;

	case WM_LBUTTONDOWN:
		if (ToolId == PEN)
		{
			shape = new CustomPencil((short)LOWORD(lParam), (short)HIWORD(lParam));
			drawMode = BUFFER;
		}
		else
		{
			switch (ToolId)
			{
			case LINE:
				shape = new CustomLine((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case RECTANGLE:
				shape = new CustomRectangle((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case ELLIPSE:
				shape = new CustomEllipse((short)LOWORD(lParam), (short)HIWORD(lParam));
				break;

			case POLY:
				if (prevX == -1 && prevY == -1)
				{
					prevX = (short)LOWORD(lParam);
					prevY = (short)HIWORD(lParam);
					startX = prevX;
					startY = prevY;
				}
				shape = new CustomLine(prevX, prevY);
				break;

			case TEXT:
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
		useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode);
		SetCapture(hWnd);
		break;

	case WM_MOUSEMOVE:
		prevCoord.x = (short)LOWORD(lParam);
		prevCoord.y = (short)HIWORD(lParam);
		GetClientRect(hWnd, &rect);
		BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
		if (wParam & MK_LBUTTON)
		{
			if (shape)
			{
				if (ToolId == PEN)
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
			if (rubber)
			{
				useRubber(hWnd, rubber, (short)LOWORD(lParam), (short)HIWORD(lParam), currentDc, bufferDc, drawMode);
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		if ((ToolId != PEN) && shape != NULL)
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
		createBackup(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
		delete shape;
		shape = NULL;
		break;

	case WM_LBUTTONDBLCLK:
		if (ToolId == 4)
		{
			shape = new CustomLine(prevX, prevY);
			ReleaseCapture();
			GetClientRect(hWnd, &rect);
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
		createBackup(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
		if (rubber)
		{
			delete rubber;
			rubber = NULL;
		}
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
			undo(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
			BitBlt(paintDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			drawMode = CURRENT;
			break;

		case RESTORE:
			restore(hWnd, backupDepth, restoreCount, bufferDc, backupDc);
			BitBlt(paintDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			drawMode = CURRENT;
			break;
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEWHEEL:
		HPEN pen;
		if (wParam & MK_RBUTTON)
		{
			CustomRubber::rubberWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (CustomRubber::rubberWidth < 0)
				CustomRubber::rubberWidth = 0;
			GetClientRect(hWnd, &rect);
			useRubber(hWnd, rubber, prevCoord.x, prevCoord.y, currentDc, bufferDc, drawMode);
		}
		else
		{
			CustomShape::penWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (CustomShape::penWidth < 0)
				CustomShape::penWidth = 0;
			pen = CreatePen(CustomShape::penStyle, CustomShape::penWidth, CustomShape::penColor);
			DeleteObject(SelectObject(currentDc, pen));
			DeleteObject(SelectObject(bufferDc, pen));
			GetClientRect(hWnd, &rect);
			BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			MoveToEx(currentDc, prevCoord.x, prevCoord.y, NULL);
			LineTo(currentDc, prevCoord.x, prevCoord.y);
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
        ReleaseDC(hWnd, currentDc);
		ReleaseDC(hWnd, bufferDc);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

