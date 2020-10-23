#include"Recources/File_reader.hpp"
#include"Recources/LwEdit.hpp"

//implements main
wxIMPLEMENT_APP(App);

//statup
bool App::OnInit() {

	//----------- debug console ----------//
	#pragma warning(disable : 4996)       //
	AllocConsole();                       //
	FILE* lol;                            //
	lol = freopen("conin$", "r", stdin);  //
	lol = freopen("conout$", "w", stdout);//
	lol = freopen("conout$", "w", stderr);//
	//------------------------------------//


	MainWindow *m_frame1 = new MainWindow();
	m_frame1->SetIcon(wxICON(main_icon));
	m_frame1->Show();
	return true;
}

//populate main window
MainWindow::MainWindow() : wxFrame(NULL, wxID_ANY, "LwEdit") {
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_FILE_World, "&Open World");
	menuFile->Append(ID_FILE_Board, "&Open Board");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, "&Exit");

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(ID_HELP_HELP, "&Help");
	menuHelp->Append(ID_HELP_GH, "&Github");
	menuHelp->Append(ID_HELP_LW, "&Logic World");
	menuHelp->AppendSeparator();
	menuHelp->Append(ID_HELP_ABOUT, "&About");

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &MainWindow::OnOpen_World, this, ID_FILE_World);
	Bind(wxEVT_MENU, &MainWindow::OnOpen_Board, this, ID_FILE_Board);

	Bind(wxEVT_MENU, &MainWindow::OnHelp_Help, this, ID_HELP_HELP);
	Bind(wxEVT_MENU, &MainWindow::OnHelp_GH, this, ID_HELP_GH);
	Bind(wxEVT_MENU, &MainWindow::OnHelp_LW, this, ID_HELP_LW);
	Bind(wxEVT_MENU, &MainWindow::OnHelp_About, this, ID_HELP_ABOUT);

	Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);
}

//File events
void MainWindow::OnOpen_World(wxCommandEvent& event) {
	wxFileDialog get_path(this, ("open world"), "", "", "worldinfo.succ|worldinfo.succ", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	get_path.ShowModal();
	wxMessageBox(get_path.GetPath(), "path");
}

void MainWindow::OnOpen_Board(wxCommandEvent& event) {
	wxFileDialog get_path(this, ("open Board"), "", "", "data.tung|*.tung", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	get_path.ShowModal();
	
	//if we got a path
	if (get_path.GetPath().ToStdString() != "") {
		tungfile file = readtung(get_path.GetPath().ToStdWstring());

		switch (file.__Status__)
		{
		case CFR_SUCCESS:
			break;
		case CFR_ERR_WHAT:
			wxMessageBox("CFR_ERROR: what?\n - uh... this should not happen, please report this issue", "error", wxOK | wxICON_ERROR);
			return;
		case CFR_ERR_CANT_OPEN:
			wxMessageBox("CFR_ERROR: cant open file\n - make sure you selected the right file", "error", wxOK | wxICON_ERROR);
			return;
		case CFR_ERR_INVALID_HEADER:
			wxMessageBox("CFR_ERROR: invalid file header\n - the file header doesnt match, make sure you selected the right file", "error", wxOK | wxICON_ERROR);
			return;
		case CFR_ERR_INVALID_VERSION:
			wxMessageBox("CFR_ERROR: invalid version\n - the version of this .tung file is not supported by LwEdit", "error", wxOK | wxICON_ERROR);
			return;
		case CFR_ERR_INVALID_FOOTER:
			wxMessageBox("CFR_ERROR: invalid footer\n - the footer doesnt match, make sure you selected the right file", "error", wxOK | wxICON_ERROR);
			return;
		default:
			wxMessageBox("CFR_ERROR: UNKNOWN\n - unknown error, please report this issue", "error", wxOK | wxICON_ERROR);
			return;
		}

		std::cout << "Status: " << (int)file.__Status__ << '\n';
		std::cout << "Game Version: " << file.GameVersion[0] << ' ' << file.GameVersion[1] << ' ' << file.GameVersion[2] << ' ' << file.GameVersion[3];

		writetung(file, L"D:\\CPP\\C++ Projects\\LW-Edit\\World\\test.tung");
	}
}

//Help events
void MainWindow::OnHelp_Help(wxCommandEvent& event) {
	wxMessageBox("if you require help feel free to:\n - open a help issue on Github\n - send me a dm on Discord [Red_3D#3062]", "Help", wxOK | wxICON_NONE);
}

void MainWindow::OnHelp_GH(wxCommandEvent& event) {
	ShellExecute(NULL, L"open", L"https://github.com/Red-3D/LW-Edit", NULL, NULL, SW_SHOWNORMAL);
}

void MainWindow::OnHelp_LW(wxCommandEvent& event) {
	ShellExecute(NULL, L"open", L"https://logicworld.net/", NULL, NULL, SW_SHOWNORMAL);
}

void MainWindow::OnHelp_About(wxCommandEvent& event) {
	std::string message = "Logic World Edit\n\nMade by: Red_3D\nVersion: ";
	message += VERSION;
	message += "\n\nLogic World Edit is a external editor for Logic worlds boards and worlds.";
	wxMessageBox(message, "About LwEdit", wxOK | wxICON_NONE);
}

//exit
void MainWindow::OnExit(wxCommandEvent& event) {
	Close(true);
}