// WindowsProject2.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "WindowsProject2.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

CONST INT numOfRows = 3;
CONST INT numOfColumns = 4;
char slist[numOfRows*numOfColumns][255];

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

VOID ArrayInitialization(HANDLE hFile);
VOID DrawTable(HWND hWnd, RECT wndRect);
VOID DrawLine(HDC hdc, COLORREF color, int x1, int y1, int x2, int y2);
VOID DrawVerticalTableLines(HDC hdc, COLORREF color, INT cellSizeX, INT tableSizeY);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//setlocale(LC_ALL, "Rus");
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;

    // Цикл основного сообщения:
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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

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

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		const TCHAR szFileName[] = "TextSource/text.txt";
		HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile) {
			MessageBox(hWnd, "File not found!", "Error", MB_OK);
		}
		else {
			ArrayInitialization(hFile);
			CloseHandle(hFile);
		}
	}
	break;
    case WM_PAINT:
    {
		RECT wndRect;
		GetClientRect(hWnd, &wndRect);

		DrawTable(hWnd, wndRect);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

VOID ArrayInitialization(HANDLE hFile) {

	const int num = numOfColumns * numOfRows;

	for (int i = 0; i < num; i++)
	{
		char cBufferText[255];
		DWORD dwByte;
		DWORD numOfBytesToRead = rand() % 254;
		ReadFile(hFile, cBufferText, numOfBytesToRead, &dwByte, NULL);
		cBufferText[numOfBytesToRead] = 0;

		strcpy_s(slist[i], sizeof(cBufferText), cBufferText);
	}

}

VOID DrawTable(HWND hWnd, RECT wndRect)
{
	INT indent = 5,
		maxRowHight = 0,
		sizeOfColumn;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	RECT rect, cellForText;
	HBRUSH brush;
	COLORREF colorText = RGB(189, 99, 197),
		colorBack = RGB(30, 30, 30),
		colorLine = RGB(71, 163, 64);

	brush = CreateSolidBrush(colorBack);
	SelectObject(hdc, brush);
	Rectangle(hdc, wndRect.left, wndRect.top, wndRect.right, wndRect.bottom);
	DeleteObject(brush);

	sizeOfColumn = wndRect.right / numOfColumns;

	for (int i = 0; i < numOfRows; i++) {

		rect.top = maxRowHight;

		for (int j = 0; j < numOfColumns; j++) {

			rect.left = sizeOfColumn * j;
			rect.right = wndRect.right / numOfColumns * (j + 1);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, colorText);

			cellForText.top = rect.top + indent;
			cellForText.right = rect.right - indent;
			cellForText.left = rect.left + indent;

			DrawText(hdc, (LPCSTR)slist[numOfColumns * i + j], -1, &cellForText, DT_CALCRECT | DT_WORDBREAK | DT_LEFT | DT_EDITCONTROL);
			rect.right = wndRect.right / numOfColumns * (j + 1);
			rect.bottom = cellForText.bottom + indent;
			
			DrawText(hdc, (LPCSTR)slist[numOfColumns * i + j], -1, &cellForText, DT_WORDBREAK);

			if (rect.bottom > maxRowHight)
				maxRowHight = rect.bottom;							
		}

		DrawLine(hdc, colorLine, 0, maxRowHight, wndRect.right, maxRowHight);
	}

	DrawVerticalTableLines(hdc, colorLine, sizeOfColumn, maxRowHight);

	SetBkMode(hdc, OPAQUE);  // Переустанавливаем режим фона в его состояние по умолчанию. 
	EndPaint(hWnd, &ps);
}

VOID DrawLine(HDC hdc, COLORREF color, int x1, int y1, int x2, int y2)
{
	HPEN pen = CreatePen(PS_INSIDEFRAME, 3, color);
	POINT pt;
	SelectObject(hdc, pen);
	MoveToEx(hdc, x1, y1, &pt);
	LineTo(hdc, x2, y2);
	DeleteObject(pen);
}

VOID DrawVerticalTableLines(HDC hdc, COLORREF color, INT cellSizeX, INT tableSizeY)
{
	for (int i = 1; i < numOfColumns; i++) {
		DrawLine(hdc, color, i * cellSizeX, 0, i * cellSizeX, tableSizeY);
	}
}