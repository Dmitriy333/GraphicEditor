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
	HACCEL hAccel = LoadAccelerators(hInstance, (LPCWSTR)IDR_ACCELERATOR);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateAccelerator(hWnd, hAccel, &msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

// Saving functions
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;  
	
	GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;
	
	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
 
	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8	* pbmi->bmiHeader.biHeight; 
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;              
	BITMAPFILEHEADER hdr;     
	PBITMAPINFOHEADER pbih;     
	LPBYTE lpBits;              
	DWORD dwTotal;            
	DWORD cb;                   
	BYTE *hp;                     
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);
  
	hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	
	hdr.bfType = 0x4d42;
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD); 
	WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL);
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, (NULL));
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);
	CloseHandle(hf);
	GlobalFree((HGLOBAL)lpBits);
}

HBITMAP Create_hBitmap(HDC hDC, int w, int h)
{
	HDC hDCmem;
	HBITMAP hbm, holdBM;
	hDCmem = CreateCompatibleDC(hDC);
	hbm = CreateCompatibleBitmap(hDC, w, h);
	holdBM = (HBITMAP)SelectObject(hDCmem, hbm);
	BitBlt(hDCmem, 0, 0, w, h, hDC, 0, 0, SRCCOPY);
	SelectObject(hDCmem, holdBM);
	DeleteDC(hDCmem);
	DeleteObject(holdBM);
	return hbm;
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
	static INT prevX = -1, prevY = -1, startX = -1, startY = -1;
	static BOOL isPolyLine;
	static HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	static POINT prevCoord;
	static String str;
	HPEN pen;
	HBRUSH brush;
	CHOOSECOLOR cc;
	COLORREF acrCustClr[16];
	OPENFILENAME ofn;
	static TCHAR sfile[MAX_PATH];
	static BOOL isFile = false;
	static DOUBLE zoom = DEFAULT_ZOOM;
	static INT prevZoom = 0;
	static BOOL isScale = false;

	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
			ZeroMemory(&ofn, sizeof(ofn));
			ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);

			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sfile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

			if (GetOpenFileName(&ofn))
			{
				HBITMAP hBitmap;
				BITMAP bm;
				HDC hDC;
				HDC hMemDC;

				hDC = GetDC(hWnd);
				hMemDC = CreateCompatibleDC(hDC);
				hBitmap = (HBITMAP)LoadImage(hInst, sfile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				isFile = true;
				GetObject(hBitmap, sizeof(BITMAP), &bm);
				SelectObject(bufferDc, hBitmap);
				BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, bufferDc, 0, 0, SRCCOPY);
				DeleteDC(hMemDC);
				ReleaseDC(hWnd, hDC);
				DeleteObject(hBitmap);
			}
			break;

		case ID_FILE_SAVE:
			if (isFile)
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				CreateBMPFile(hWnd, sfile, CreateBitmapInfoStruct(hWnd, Create_hBitmap(GetDC(hWnd), rect.right - rect.left, rect.bottom - rect.top)), Create_hBitmap(GetDC(hWnd), rect.right - rect.left, rect.bottom - rect.top), GetDC(hWnd));
				break;
			}
		
		case ID_FILE_SAVEAS:
			ZeroMemory(&ofn, sizeof(ofn));
			ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);

			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sfile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = (LPCWSTR)TEXT("bmp\0*.bmp");
			ofn.lpstrDefExt = (LPCWSTR)TEXT("bmp\0*.bmp");
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

			if (GetSaveFileName(&ofn))
			{
				RECT rect;
				GetClientRect(hWnd, &rect);

				CreateBMPFile(hWnd, ofn.lpstrFile, CreateBitmapInfoStruct(hWnd, Create_hBitmap(GetDC(hWnd), rect.right - rect.left, rect.bottom - rect.top)), Create_hBitmap(GetDC(hWnd), rect.right - rect.left, rect.bottom - rect.top), GetDC(hWnd));
				isFile = true;
			}
			break;

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
				str.clear();
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
		if (isScale)
			break;
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
		else
		{
			GetClientRect(hWnd, &rect);
			BitBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, SRCCOPY);
			MoveToEx(currentDc, prevCoord.x, prevCoord.y, NULL);
			LineTo(currentDc, prevCoord.x, prevCoord.y);
			drawMode = CURRENT;
			InvalidateRect(hWnd, &rect, FALSE);
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
		else if (wParam & MK_CONTROL)
		{
			if (!isScale)
				isScale = true;
			GetClientRect(hWnd, &rect);
			if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				zoom *= DELTA;
			}
			else
			{
				zoom /= DELTA;
			}
			HPEN pen = (HPEN)GetStockObject(NULL_PEN);
			HBRUSH brush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			HANDLE oldPen = SelectObject(currentDc, pen);
			DeleteObject(SelectObject(bufferDc, pen));
			HANDLE oldBrush = SelectObject(currentDc, brush);
			DeleteObject(SelectObject(bufferDc, brush));
			Rectangle(currentDc, 0, 0, rect.right, rect.bottom);
			DeleteObject(SelectObject(currentDc, (HPEN)oldPen));
			DeleteObject(SelectObject(bufferDc, (HPEN)oldPen));
			DeleteObject(SelectObject(currentDc, (HBRUSH)oldBrush));
			DeleteObject(SelectObject(bufferDc, (HBRUSH)oldBrush));

			StretchBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, (int)(rect.right * zoom), (int)(rect.bottom * zoom), SRCCOPY);

			pen = (HPEN)GetStockObject(BLACK_PEN);
			brush = (HBRUSH)GetStockObject(NULL_BRUSH);
			oldPen = SelectObject(currentDc, pen);
			DeleteObject(SelectObject(bufferDc, pen));
			oldBrush = SelectObject(currentDc, brush);
			DeleteObject(SelectObject(bufferDc, brush));
			Rectangle(currentDc, 0, 0, (int)(rect.right / zoom), (int)(rect.bottom / zoom));
			DeleteObject(SelectObject(currentDc, (HPEN)oldPen));
			DeleteObject(SelectObject(bufferDc, (HPEN)oldPen));
			DeleteObject(SelectObject(currentDc, (HBRUSH)oldBrush));
			DeleteObject(SelectObject(bufferDc, (HBRUSH)oldBrush));
			DeleteObject(pen);
			DeleteObject(oldPen);
			DeleteObject(brush);
			DeleteObject(oldBrush);

			drawMode = CURRENT;
			InvalidateRect(hWnd, &rect, false);
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
			
			str += (TCHAR)wParam;
			GetClientRect(hWnd, &rect);
			TextOut(bufferDc, prevX, prevY, str.data(), str.size());
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	case WM_KEYUP:
		if (wParam & VK_CONTROL && isScale)
		{	
			GetClientRect(hWnd, &rect);
			HPEN pen = (HPEN)GetStockObject(NULL_PEN);
			HBRUSH brush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			HANDLE oldPen = SelectObject(currentDc, pen);
			DeleteObject(SelectObject(bufferDc, pen));
			HANDLE oldBrush = SelectObject(currentDc, brush);
			DeleteObject(SelectObject(bufferDc, brush));
			Rectangle(currentDc, 0, 0, rect.right, rect.bottom);
			DeleteObject(SelectObject(currentDc, (HPEN)oldPen));
			DeleteObject(SelectObject(bufferDc, (HPEN)oldPen));
			DeleteObject(SelectObject(currentDc, (HBRUSH)oldBrush));
			DeleteObject(SelectObject(bufferDc, (HBRUSH)oldBrush));
			DeleteObject(pen);
			DeleteObject(oldPen);
			DeleteObject(brush);
			DeleteObject(oldBrush);

			isScale = false;
			StretchBlt(currentDc, 0, 0, rect.right, rect.bottom, bufferDc, 0, 0, (int)(rect.right * zoom), (int)(rect.bottom * zoom), SRCCOPY);
			BitBlt(bufferDc, 0, 0, rect.right, rect.bottom, currentDc, 0, 0, SRCCOPY);
			zoom = DEFAULT_ZOOM;
			drawMode = BUFFER;
			InvalidateRect(hWnd, &rect, false);
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

