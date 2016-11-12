#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

#define ID_EDIT 100
#define ID_BUTTON 101
#define NTFS_MAX_FILE_SIZE 17592185978880

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("100000000"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL | ES_RIGHT, 10, 10, 224, 32, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("byte"), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 234, 10, 32, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("BUTTON"), TEXT("スパースファイル作成"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 10, 50, 256, 32, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BUTTON)
		{
			LARGE_INTEGER li = { 0 };
			{
				const DWORD dwSize = GetWindowTextLength(GetDlgItem(hWnd, ID_EDIT));
				if (dwSize)
				{
					LPTSTR lpszText = (LPTSTR)GlobalAlloc(0, (dwSize + 1) * sizeof(TCHAR));
					if (lpszText)
					{
						GetDlgItemText(hWnd, ID_EDIT, lpszText, dwSize + 1);
						li.QuadPart = (LONGLONG)_wtoll(lpszText);
						GlobalFree(lpszText);
					}
				}
			}
			if (li.QuadPart > NTFS_MAX_FILE_SIZE)
			{
				li.QuadPart = NTFS_MAX_FILE_SIZE;
				TCHAR szText[20];
				wsprintf(szText, TEXT("%I64d"), li.QuadPart);
				SetDlgItemText(hWnd, ID_EDIT, szText);
			}
			if (li.QuadPart > 0)
			{
				const HANDLE hFile = CreateFile(TEXT("SPARSEFILE"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwByte;
					if (DeviceIoControl(hFile, FSCTL_SET_SPARSE, 0, 0, 0, 0, &dwByte, 0))
					{
						SetFilePointerEx(hFile, li, 0, FILE_BEGIN);
					}
					SetEndOfFile(hFile);
					CloseHandle(hFile);
				}
			}
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	RECT rect;
	SetRect(&rect, 0, 0, 276, 92);
	AdjustWindowRect(&rect, WS_CAPTION | WS_SYSMENU, 0);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Create Sparse File"),
		WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
