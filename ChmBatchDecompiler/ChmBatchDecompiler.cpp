// ChmBatchDecompiler.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <string>
#include <map>
#include "../../WinUtil/Util/Util.h"

#include "ChmBatchDecompiler.h"

using namespace std;

#define MAX_LOADSTRING 100

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Util.lib")

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

wstring g_wstrInputFolder;
wstring g_wstrOutputFolder;
bool g_fIncludeSubFolder = false;

HWND g_hDlg = NULL;

enum {
    ID_STATIC_1 = 200,
    ID_EDIT_1,
    ID_BUTTON_1,
    ID_CHECKBOX_1,
    ID_STATIC_2,
    ID_EDIT_2,
    ID_BUTTON_2,
    ID_PROGRESS_1,
    ID_LIST_1,
    ID_BUTTON_3,
    ID_STATIC_4,
};

CONTROL_DATA g_controlDatas[] = {
    {NULL, L"Static", L"Input Folder(contains CHM files):", WS_CHILD | WS_VISIBLE, 10, 10, 275, 20, ID_STATIC_1},
    {NULL, L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 10, 40, 195, 20, ID_EDIT_1},
    {NULL, L"Button", L"Browse", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 215, 40, 70, 20, ID_BUTTON_1},
    {NULL, L"Button", L"Include Sub Folder", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, 10, 70, 275, 20, ID_CHECKBOX_1},
    {NULL, L"Static", L"Output Folder:", WS_CHILD | WS_VISIBLE, 10, 110, 275, 20, ID_STATIC_2},
    {NULL, L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 10, 140, 195, 20, ID_EDIT_2},
    {NULL, L"Button", L"Browse", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 215, 140, 70, 20, ID_BUTTON_2},
    {NULL, L"msctls_progress32", NULL, WS_CHILD | WS_VISIBLE, 10, 170, 275, 20, ID_PROGRESS_1},
    {NULL, L"ListBox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL, 10, 200, 275, 100, ID_LIST_1},
    {NULL, L"Button", L"Decompile", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 310, 275, 40, ID_BUTTON_3},
    {NULL, L"Static", L"www.northbright.com", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY, 10, 360, 275, 20, ID_STATIC_4}
};


map<UINT, HWND> g_controlMap;

Util::ChmDecompiler g_chmDecompiler;
Util::Logger g_logger;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void UpdateUI(bool fRunning);
bool OnLog(HWND hWnd, LPCWSTR lpszLog);

void UpdateUI(bool fRunning) {
    EnableWindow(g_controlMap[ID_EDIT_1], fRunning ? FALSE : TRUE);
    EnableWindow(g_controlMap[ID_BUTTON_1], fRunning ? FALSE : TRUE);
    EnableWindow(g_controlMap[ID_CHECKBOX_1], fRunning ? FALSE : TRUE);
    EnableWindow(g_controlMap[ID_EDIT_2], fRunning ? FALSE : TRUE);
    EnableWindow(g_controlMap[ID_BUTTON_2], fRunning ? FALSE : TRUE);
    SetWindowText(g_controlMap[ID_BUTTON_3], fRunning ? L"Stop" : L"Decompile");
    SendMessage(g_controlMap[ID_PROGRESS_1], PBM_SETPOS, (WPARAM)0, 0);
}

bool OnLog(HWND hWnd, LPCWSTR lpszLog) {
    UINT nCount = 0;
    SendMessage(g_controlMap[ID_LIST_1], LB_ADDSTRING, 0, (LPARAM)lpszLog);
    nCount = (UINT)SendMessage(g_controlMap[ID_LIST_1], LB_GETCOUNT, 0, 0);
    SendMessage(g_controlMap[ID_LIST_1], LB_SETCURSEL, (WPARAM)(nCount - 1), 0);

    Util::DBG_MSG(L"%s\r\n", lpszLog);
    return true;
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHM_BATCH_DECOMPILER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHM_BATCH_DECOMPILER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
        if ((!g_hDlg) || (!IsDialogMessage(g_hDlg, &msg))) {  // Add check dialog message to enable tab stop
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHM_BATCH_DECOMPILER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CHM_BATCH_DECOMPILER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, dwStyle,
      CW_USEDEFAULT, 0, 300, 435, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
    case WM_ACTIVATE:  // Add to enable tab stop for window, see "http://stackoverflow.com/questions/5691724/c-winapi-how-to-switch-between-controls-using-tab-key"
        if (!wParam)
            g_hDlg = NULL;
        else
            g_hDlg = hWnd;
        break;

    case WM_CREATE:
        for (size_t i = 0; i < sizeof(g_controlDatas) / sizeof(g_controlDatas[0]); i++) {
            g_controlDatas[i].hWnd = CreateWindow(g_controlDatas[i].lpszClass, g_controlDatas[i].lpszCaption, g_controlDatas[i].dwStyle, g_controlDatas[i].x, g_controlDatas[i].y, g_controlDatas[i].w, g_controlDatas[i].h, hWnd, (HMENU)g_controlDatas[i].nId, NULL, hInst);
            g_controlMap[g_controlDatas[i].nId] = g_controlDatas[i].hWnd;
        }
        SendMessage(g_controlMap[ID_PROGRESS_1], PBM_SETSTEP, (WPARAM)1, 0);

        // Create Logger
        g_logger.Set(hWnd, L"NorthBright", L"ChmBatchDecompiler", OnLog);
        g_logger.DeleteLogFile();

        break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

        case ID_BUTTON_1:
            Util::OpenSelectFolderDialog(hWnd, L"Input Folder:", g_wstrInputFolder);
            SetWindowText(g_controlMap[ID_EDIT_1], g_wstrInputFolder.c_str());
            break;

        case ID_BUTTON_2:
            Util::OpenSelectFolderDialog(hWnd, L"Output Folder:", g_wstrOutputFolder);
            SetWindowText(g_controlMap[ID_EDIT_2], g_wstrOutputFolder.c_str());
            break;

        case ID_CHECKBOX_1:
            break;

        case ID_BUTTON_3:
            if (!g_chmDecompiler.IsRunning()) {
                Util::MyGetWindowText(g_controlMap[ID_EDIT_1], g_wstrInputFolder);
                if (g_wstrInputFolder.empty())
                    break;

                Util::MyGetWindowText(g_controlMap[ID_EDIT_2], g_wstrOutputFolder);
                if (g_wstrOutputFolder.empty())
                    break;

                g_fIncludeSubFolder = (bool)SendMessage(g_controlMap[ID_CHECKBOX_1], BM_GETCHECK, 0, 0);

                g_chmDecompiler.Set(g_wstrInputFolder.c_str(), g_wstrOutputFolder.c_str(), g_fIncludeSubFolder, hWnd, 4, &g_logger);
                g_chmDecompiler.Start();
                UpdateUI(g_chmDecompiler.IsRunning());
            }else {
                g_chmDecompiler.Stop();
                UpdateUI(g_chmDecompiler.IsRunning());
            }

            break;

        case ID_STATIC_4:
            ShellExecute (NULL, _T("open"), L"http://www.northbright.com", NULL, NULL, SW_SHOW);
            break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

    case WM_THREAD_GROUP_PROGRESS_CHANGED:
        SendMessage(g_controlMap[ID_PROGRESS_1], PBM_SETPOS, (WPARAM)lParam, 0);
        if ((int)lParam == 100)
            MessageBox(hWnd, L"Decompiled Successfully!", L"ChmBatchDecompiler", MB_OK);
        break;

    case WM_THREAD_GROUP_EXITED:
        if ((int)SendMessage(g_controlMap[ID_PROGRESS_1], PBM_GETPOS, 0, 0) < 100) {
            MessageBox(hWnd, L"Failed to decompile one or more CHM files.", L"ChmBatchDecompiler", MB_OK);
            g_chmDecompiler.m_pLogger->LaunchLogFile();
        }
        UpdateUI(false);
        break;

    case WM_THREAD_GROUP_STOPPED:
        UpdateUI(false);
        break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
