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
#define SETFONT(hWnd) SendMessage(hWnd, WM_SETFONT, WPARAM(defaultFont), TRUE);

//imagine having to look through everything else
#define VERSION L"0.4a"

#define SIZE_MAIN_X  500
#define SIZE_MAIN_Y  500
#define SIZE_ABOUT_X 300
#define SIZE_ABOUT_Y 175

#define M_EXIT 1
#define M_FILE_MENU_WORLD 201
#define M_FILE_MENU_BOARD 202
#define M_HELP_HELP  301
#define M_HELP_GH    302
#define M_HELP_LW    303
#define M_HELP_ABOUT 304
#define M_HELP_ABOUT_EXIT 401

//gonna define them later
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutProcedure(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
void AddControls_World(HWND);
void AddControls_Board(HWND);
void PopulateAbout(HWND);
std::wstring File_dialogue(HWND hWnd, LPCWSTR filter);

//need to remember those
HWND hMain;
HMENU hMenu;

//used to clean the ui later
void CALLBACK Destroy_Children(_In_ HWND hWnd) {
	DestroyWindow(hWnd);
}

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
	hMain = CreateWindowW(L"LwEdit",L"LwEdit",WS_OVERLAPPEDWINDOW | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)-SIZE_MAIN_X)/2, (GetSystemMetrics(SM_CYSCREEN)-SIZE_MAIN_Y)/2,SIZE_MAIN_X,SIZE_MAIN_Y,NULL,NULL,NULL,NULL);
	
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
		case M_FILE_MENU_WORLD:{
			std::wstring path = File_dialogue(hWnd, L"meta.succ\0meta.succ\0All Files\0*.*\0");
			
			if (path != L"") {
				EnumChildWindows(hWnd, (WNDENUMPROC)Destroy_Children, 0);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, 500, 500, SWP_NOMOVE);
				AddControls_World(hWnd);
				MessageBox(hWnd, path.c_str(), L"World:", MB_OK);
			}
			break;
		}
		case M_FILE_MENU_BOARD:{
			std::wstring path = File_dialogue(hWnd, L"data.tung\0data.tung\0All Files\0*.*\0");
			
			if (path != L"") {
				EnumChildWindows(hWnd, (WNDENUMPROC)Destroy_Children, 0);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, 500, 500, SWP_NOMOVE);
				//AddControls_World(hWnd);
				tungfile file = readtung(path);

				std::cout << "\nHeader: ";
				for (int i = 0; i < 16; i++) {
					std::cout << file.header[i];
				}
				std::cout << "\nSaveFormat Version: " << (int)file.SaveFormatVersion;

				std::cout << "\nGame Version: " << file.GameVersion[0] << ' ' << file.GameVersion[1] << ' ' << file.GameVersion[2] << ' ' << file.GameVersion[3];
				std::cout << "\nComponents: " << file.count_components << " Wires:" << file.count_wires;
				std::cout << "\nComponents in ID Map" << file.componentIDs;
				
				for (int i = 0; i < file.componentIDs; i++) {
					std::cout << "\n\nid: " << file.ID_Map[i].id;
					std::cout << "\nlength: " << file.ID_Map[i].length;
					std::cout << "\ntext_id: ";
					for (int j = 0; j < file.ID_Map[i].length; j++) {
						std::cout << file.ID_Map[i].text_id[j];
					}
				}

				for (int i = 0; i < file.count_components; i++) {
					std::cout << "\n\naddress: " << file.components[i].address;
					std::cout << "\nparent address: " << file.components[i].parent_address;
					std::cout << "\nid: " << file.components[i].id << ' ' << file.ID_Map[file.components[i].id].text_id;

					std::cout << "\n\nloc x: " << file.components[i].loc_x;
					std::cout << "\nloc y: " << file.components[i].loc_y;
					std::cout << "\nloc z: " << file.components[i].loc_z;

					std::cout << "\n\nrot r: " << file.components[i].rot_r;
					std::cout << "\nrot i: " << file.components[i].rot_i;
					std::cout << "\nrot j: " << file.components[i].rot_j;
					std::cout << "\nrot k: " << file.components[i].rot_k;
				}

				for (int i = 0; i < file.count_wires; i++) {

					std::cout << "\n\nin/out one: " << std::boolalpha << (bool)file.wires[i].inoutput_one;
					std::cout << "\naddress one: " << file.wires[i].address_one;
					std::cout << "\nindex one: " << (int)file.wires[i].index_one;

					std::cout << "\nin/out two: " << std::boolalpha << (bool)file.wires[i].inoutput_two;
					std::cout << "\naddress two: " << file.wires[i].address_two;
					std::cout << "\nindex two: " << (int)file.wires[i].index_two;

					std::cout << "\nrotation: " << file.wires[i].rotation;
				}

				std::cout << "\n\nfooter: " << file.footer;

			}
			break;
		}
		case M_HELP_HELP:
			MessageBox(hWnd, L"if you require help feel free to:\n - open a help issue on Github\n - send me a dm on Discord [Red_3D#3062]", L"Help", MB_OK);
			break;
		case M_HELP_GH:
			ShellExecute(NULL, L"open", L"https://github.com/Red-3D/LW-Edit", NULL, NULL, SW_SHOWNORMAL);
			break;
		case M_HELP_LW:
			ShellExecute(NULL, L"open", L"https://logicworld.net/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case M_HELP_ABOUT:
			CreateWindowW(L"about", L"About LwEdit", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)-SIZE_ABOUT_X)/2, (GetSystemMetrics(SM_CYSCREEN)-SIZE_ABOUT_Y)/2, SIZE_ABOUT_X, SIZE_ABOUT_Y, hWnd, NULL, NULL, NULL);
			EnableWindow(hWnd, false);
			break;
		case M_EXIT:{
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
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 400, 400, SWP_NOMOVE);
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
		case M_HELP_ABOUT_EXIT:
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
	AppendMenu(hFileMenu, MF_STRING, M_FILE_MENU_WORLD, L"Open World");
	AppendMenu(hFileMenu, MF_STRING, M_FILE_MENU_BOARD, L"Open Board");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, M_EXIT, L"Exit");

	//help menu
	AppendMenu(hHelpMenu, MF_STRING, M_HELP_HELP, L"Help");
	AppendMenu(hHelpMenu, MF_STRING, M_HELP_GH, L"Github");
	AppendMenu(hHelpMenu, MF_STRING, M_HELP_LW, L"Logic World");
	AppendMenu(hHelpMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hHelpMenu, MF_STRING, M_HELP_ABOUT, L"About");

	//function bar
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Open");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

	SetMenu(hWnd,hMenu);
}

void AddControls_World(HWND hWnd) {

	return;
}

void AddControls_Board(HWND hWnd) {

	return;
}

//adds controls to the about window
void PopulateAbout(HWND hWnd) {

	HWND tmp;
	std::wstring message = L"Logic World Edit\n\nMade by: Red_3D\nVersion: ";
	message += VERSION;
	message += L"\n\nLogic World Edit is a external editor for logic worlds boards and worlds.\n";

	tmp = CreateWindowW(L"Static", message.c_str(), WS_VISIBLE | WS_CHILD, 17, 10, 250, 100, hWnd, NULL, NULL, NULL);
	SETFONT(tmp);
	tmp = CreateWindowW(L"Button", L"ok", WS_VISIBLE | WS_CHILD, (SIZE_ABOUT_X-100)/2, SIZE_ABOUT_Y-60, 100, 20, hWnd,(HMENU) M_HELP_ABOUT_EXIT, NULL, NULL);
	SETFONT(tmp);

	return;
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