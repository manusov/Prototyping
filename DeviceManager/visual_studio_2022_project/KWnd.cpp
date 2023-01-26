/* ----------------------------------------------------------------------------------------
Class for GUI window.
---------------------------------------------------------------------------------------- */

#include "KWnd.h"

KWnd::KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
				   LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
				   LPCTSTR menuName, int x, int y, int width, int height,
				   UINT classStyle, DWORD windowStyle, HWND hParent)
{
	char szClassName[] = "KWndClass";

	wc.cbSize        = sizeof(wc);
	wc.style         = classStyle;
	wc.lpfnWndProc   = pWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon		 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = menuName;
	wc.lpszClassName = szClassName;
	wc.hIconSm       = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));

	hWnd = NULL;

	// Register window class, show message box if error.
	if (!RegisterClassEx(&wc)) {
		char msg[100] = "Cannot register class: ";
		strcat_s(msg, szClassName);
		MessageBox(NULL, msg, "Error", MB_OK);
		return;
	}
	
	// Create window, show message box if error.
	hWnd = CreateWindow(szClassName, windowName, windowStyle,
		x, y, width, height, hParent, (HMENU)NULL, hInst, NULL);       
	if (!hWnd) {
		char text[100] = "Cannot create window: ";
		strcat_s(text, windowName);
		MessageBox(NULL, text, "Error", MB_OK);
		return;
	}

	// Show window.
	ShowWindow(hWnd, cmdShow); 
}
