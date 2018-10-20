#include "stdafx.h"
#include "WindowsProject1.h"
#include <Windows.h>

#define MAX_LOADSTRING 100
#define FIGURE_WIDTH 200
#define FIGURE_HEIGHT 100

HINSTANCE hInst;   
WCHAR szTitle[MAX_LOADSTRING];     
WCHAR szWindowClass[MAX_LOADSTRING];   

const int ID_TIMER = 1;

BOOL isMooving = false;
BOOL figureFallsDown = true;
BOOL isImgLoaded = false;
struct figureInfo
{
	int x;
	int y;
	int width;
	int height;
	int delta;
} ellipseInfo;
INT distance;
INT track = 0;
HBITMAP hBitmap;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

VOID SetFigureParams(int x, int y, int w, int h, int d);
VOID DrawFigure(HWND hWnd);
VOID MoovingWithTheKeyboard(HWND hWnd, WPARAM wParam);
VOID MoovingWithTheWeel(WPARAM wParam);
VOID FigureFalling(HWND hWnd);
BOOL MouseOnFigure(POINT pt);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		BITMAP bm;
		case WM_CREATE: 
		{
			RECT wndRect;
			GetClientRect(hWnd, &wndRect);

			hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			GetObject(hBitmap, sizeof(bm), &bm); 
			if (hBitmap == NULL)
				MessageBox(hWnd, L"Could not load IDB_BITMAP1!", L"Error", MB_OK | MB_ICONEXCLAMATION);

			SetFigureParams(wndRect.right / 2, wndRect.bottom / 2, FIGURE_WIDTH, FIGURE_HEIGHT, 10);
		}
		case WM_PAINT:
        {
			DrawFigure(hWnd);     
        }
        break;
		case WM_LBUTTONDOWN: 
		{
			POINT ptMousePos = { LOWORD(lParam), HIWORD(lParam) };
			if (MouseOnFigure(ptMousePos))
				isMooving = true;
			InvalidateRgn(hWnd, NULL, TRUE); 
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (isMooving) {
				ellipseInfo.x = LOWORD(lParam);
				ellipseInfo.y = HIWORD(lParam);
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
		case WM_LBUTTONUP:
		{
			isMooving = false;
		}
		break;
		case WM_KEYDOWN:
		{
			MoovingWithTheKeyboard(hWnd, wParam);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			MoovingWithTheWeel(wParam);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_TIMER:
		{
			FigureFalling(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_DESTROY:
			DeleteObject(hBitmap);
			PostQuitMessage(0);
        break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
    return 0;
}

VOID SetFigureParams(int x, int y, int w, int h, int d)
{
	ellipseInfo.x = x;
	ellipseInfo.y = y;
	ellipseInfo.width = w;
	ellipseInfo.height = h;
	ellipseInfo.delta = d;
}

VOID DrawFigure(HWND hWnd)
{
	HBITMAP hPrevBmp;
	PAINTSTRUCT ps;
	HDC hWndDc;
	HDC hMemDc;
	BITMAP bmp;
	RECT wndRect;

	GetClientRect(hWnd, &wndRect);
	hWndDc = BeginPaint(hWnd, &ps);
	
	if (isImgLoaded) {
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		hMemDc = CreateCompatibleDC(hWndDc);
		hPrevBmp = (HBITMAP)SelectObject(hMemDc, hBitmap);
		ellipseInfo.width = bmp.bmWidth;
		ellipseInfo.height = bmp.bmHeight;

		HBRUSH brush = CreateSolidBrush(RGB(120, 110, 120));
		SelectObject(hWndDc, brush);
		Rectangle(hWndDc, 0, 0, wndRect.right, wndRect.bottom);
		DeleteObject(brush);

		BitBlt(hWndDc, ellipseInfo.x - ellipseInfo.width / 2, ellipseInfo.y - ellipseInfo.height / 2, bmp.bmWidth, bmp.bmHeight,
			hMemDc, 0, 0, SRCAND);
		SelectObject(hMemDc, hBitmap);

		DeleteDC(hMemDc);
		DeleteObject(hPrevBmp);
	}
	else {
		ellipseInfo.width = FIGURE_WIDTH;
		ellipseInfo.height = FIGURE_HEIGHT;
		HBRUSH brush = CreateSolidBrush(RGB(0, 29, 1));
		SelectObject(hWndDc, brush);
		Ellipse(hWndDc, ellipseInfo.x - ellipseInfo.width / 2, ellipseInfo.y - ellipseInfo.height / 2, ellipseInfo.x + ellipseInfo.width / 2, ellipseInfo.y + ellipseInfo.height / 2);
		DeleteObject(brush);
	}
	EndPaint(hWnd, &ps);
}

BOOL MouseOnFigure(POINT pt)
{
	if ((pt.x > ellipseInfo.x - ellipseInfo.width / 2) & (pt.x < ellipseInfo.x + ellipseInfo.width / 2)
		& (pt.y > ellipseInfo.y - ellipseInfo.height / 2) & (pt.y < ellipseInfo.y + ellipseInfo.height / 2)) {
		return true;
	}
	return false;
}

VOID MoovingWithTheKeyboard(HWND hWnd, WPARAM wParam) {
	switch (LOWORD(wParam)) {
		case VK_LEFT:
			ellipseInfo.x -= ellipseInfo.delta;
		break;
		case VK_UP:
			ellipseInfo.y -= ellipseInfo.delta;
		break;
		case VK_RIGHT:
			ellipseInfo.x += ellipseInfo.delta;
		break;
		case VK_DOWN:
			ellipseInfo.y += ellipseInfo.delta;
		break;
		case VK_SPACE:
			RECT wndRect;
			GetClientRect(hWnd, &wndRect);
			distance =  wndRect.bottom - 1 - ellipseInfo.y - ellipseInfo.height / 2;
			SetTimer(hWnd, ID_TIMER, 50, NULL);
		break;
		case VK_RETURN:
			isImgLoaded = !isImgLoaded;
		break;
	}
}

VOID MoovingWithTheWeel(WPARAM wParam) {
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	int sign = (zDelta > 0) ? 1 : -1;
	if (LOWORD(wParam) != MK_SHIFT) {
		ellipseInfo.y -= sign * ellipseInfo.delta;
	}
	else {
		ellipseInfo.x -= sign * ellipseInfo.delta;
	}
}

VOID FigureFalling(HWND hWnd) {
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	if (track < distance) {
		if (figureFallsDown) {
			ellipseInfo.y += ellipseInfo.delta;
		}
		else {
			ellipseInfo.y -= ellipseInfo.delta;
		}
		track += ellipseInfo.delta;
	}
	else {
		if (figureFallsDown) {
			figureFallsDown = false;
			distance /= 2;
		}
		else
			figureFallsDown = true;
		track = 0;
	}
	if (distance < ellipseInfo.delta) {
		KillTimer(hWnd, ID_TIMER);
		figureFallsDown = true;
	}
}