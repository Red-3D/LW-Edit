#include<string>
#include<stdio.h>
#include<iostream>
#include"Recources/File_reader.hpp"

#include"Recources/resource.hpp"
#include<Windows.h>

//yes, make me look awesome
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
#define setFont(hWnd) SendMessage(hWnd, WM_SETFONT, WPARAM(defaultFont), TRUE);

//imagine having to look through everything else
#define version L"0.2a"

#define size_main_x  500
#define size_main_y  500
#define size_about_x 300
#define size_about_y 175

#define M_Exit 1
#define M_FileMenu_World 201
#define M_FileMenu_Board 202
#define M_Help_help  301
#define M_Help_GH    302
#define M_Help_Lw    303
#define M_Help_About 304
#define M_Help_About_Exit 401

//gonna define them later
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutProcedure(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
void AddControls(HWND);
void PopulateAbout(HWND);
std::wstring File_dialogue(HWND hWnd, LPCWSTR filter);

//need to remember those
HWND hMain;
HMENU hMenu;

//Register windows
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR args, _In_ int ncmdshow) {


	//main window
	WNDCLASSW wmain = {0};
	wmain.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wmain.hCursor = LoadCursor(NULL,IDC_ARROW);
	wmain.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
	wmain.hInstance = hInst;
	wmain.lpszClassName = L"LwEdit";
	wmain.lpfnWndProc = WindowProcedure;
	if (!RegisterClassW(&wmain)) {
		exit(101);
	}
	
	//about window
	WNDCLASSW wabout = { 0 };
	wabout.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wabout.hCursor = LoadCursor(NULL, IDC_ARROW);
	wabout.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
	wabout.hInstance = hInst;
	wabout.lpszClassName = L"about";
	wabout.lpfnWndProc = AboutProcedure;
	if (!RegisterClassW(&wabout)) {
		exit(101);
	}

	//create the main window
	hMain = CreateWindowW(L"LwEdit",L"LwEdit",WS_OVERLAPPEDWINDOW | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)-size_main_x)/2, (GetSystemMetrics(SM_CYSCREEN)-size_main_y)/2,size_main_x,size_main_y,NULL,NULL,NULL,NULL);
	
		//---------- debug console ---------//
		#pragma warning(disable : 4996)		//
		AllocConsole();						//
		FILE* lol;							//
		lol = freopen("conin$", "r",stdin); //
		lol = freopen("conout$","w",stdout);//
		lol = freopen("conout$","w",stderr);//
		//----------------------------------//

	//handle user input
	MSG msg = {0};
	while (GetMessage(&msg,NULL,NULL,NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	FreeConsole();
	return 0;
}

//handle main window interactions
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case M_FileMenu_World:{
			std::wstring file = File_dialogue(hWnd, L"meta.succ\0meta.succ\0All Files\0*.*\0");
			MessageBox(hWnd, file.c_str(), L"World:", MB_OK);
			break;
		}
		case M_FileMenu_Board:{
			std::wstring file = File_dialogue(hWnd, L"data.tung\0data.tung\0All Files\0*.*\0");
			readtung(file);
			break;
		}
		case M_Help_help:
			MessageBox(hWnd, L"", L"Help", MB_OK);
			break;
		case M_Help_GH:
			ShellExecute(NULL, L"open", L"https://github.com/Red-3D/LW-Edit", NULL, NULL, SW_SHOWNORMAL);
			break;
		case M_Help_Lw:
			ShellExecute(NULL, L"open", L"https://logicworld.net/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case M_Help_About:
			CreateWindowW(L"about", L"About LwEdit", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)-size_about_x)/2, (GetSystemMetrics(SM_CYSCREEN)-size_about_y)/2, size_about_x, size_about_y, hWnd, NULL, NULL, NULL);
			EnableWindow(hWnd, false);
			break;
		case M_Exit:{
			int exit = MessageBoxW(hWnd, L"you sure?\nall unsaved changes will be lost", L"about to exit", MB_YESNO | MB_ICONEXCLAMATION);
			if (exit == IDYES) {
				DestroyWindow(hWnd);
			}
			break;
		}
		default:
			break;
		}
		return 0;
	
	case WM_CREATE:
		AddMenus(hWnd);
		AddControls(hWnd);
		return 0;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

//handle about window interactions
LRESULT CALLBACK AboutProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case M_Help_About_Exit:
			EnableWindow(hMain, true);
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_CREATE:
		PopulateAbout(hWnd);
		return 0;

	case WM_CLOSE:
		EnableWindow(hMain, true);
		DestroyWindow(hWnd);
		return 0;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

//adds menus to the main window
void AddMenus(HWND hWnd) {

	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hHelpMenu = CreateMenu();

	//open file menu
	AppendMenu(hFileMenu, MF_STRING, M_FileMenu_World, L"Open World");
	AppendMenu(hFileMenu, MF_STRING, M_FileMenu_Board, L"Open Board");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, M_Exit, L"Exit");

	//help menu
	AppendMenu(hHelpMenu, MF_STRING, M_Help_help, L"Help");
	AppendMenu(hHelpMenu, MF_STRING, M_Help_GH, L"Github");
	AppendMenu(hHelpMenu, MF_STRING, M_Help_Lw, L"Logic World");
	AppendMenu(hHelpMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hHelpMenu, MF_STRING, M_Help_About, L"About");

	//function bar
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Open");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

	SetMenu(hWnd,hMenu);
}

//adds controls to the main window
void AddControls(HWND hWnd) {

	HWND tmp;
	tmp = CreateWindowW(L"Static",L"Hello World",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 100, 100, 50, hWnd, NULL, NULL, NULL);
	setFont(tmp);
	tmp = CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 200, 175, 200, 50, hWnd, NULL, NULL, NULL);
	setFont(tmp);
	tmp = CreateWindowW(L"Button", L"Button", WS_VISIBLE | WS_CHILD, 200, 250, 100, 50, hWnd, NULL, NULL, NULL);
	setFont(tmp);
}

//adds controls to the about window
void PopulateAbout(HWND hWnd) {

	HWND tmp;
	std::wstring message = L"Logic World Edit\n\nMade by: Red_3D\nVersion: ";
	message += version;
	message += L"\n\nLogic World Edit is a external editor for logic worlds boards and worlds.\n";

	tmp = CreateWindowW(L"Static", message.c_str(), WS_VISIBLE | WS_CHILD, 17, 10, 250, 100, hWnd, NULL, NULL, NULL);
	setFont(tmp);
	tmp = CreateWindowW(L"Button", L"ok", WS_VISIBLE | WS_CHILD, (size_about_x-100)/2, size_about_y-60, 100, 20, hWnd,(HMENU) M_Help_About_Exit, NULL, NULL);
	setFont(tmp);
}

//open file dialogue
std::wstring File_dialogue(HWND hWnd, LPCWSTR filter) {

	OPENFILENAME ofn;
	TCHAR szFile[260] = {0};

	//Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileName(&ofn);
	return ofn.lpstrFile;
}