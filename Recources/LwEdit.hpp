#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define VERSION "0.5a"

class App : public wxApp {
public:
	virtual bool OnInit();
};

class MainWindow : public wxFrame {
public:
	MainWindow();

private:
	void OnOpen_World(wxCommandEvent& event);
	void OnOpen_Board(wxCommandEvent& event);

	void OnHelp_Help(wxCommandEvent& event);
	void OnHelp_GH(wxCommandEvent& event);
	void OnHelp_LW(wxCommandEvent& event);
	void OnHelp_About(wxCommandEvent& event);

	void OnExit(wxCommandEvent& event);
};

enum {
	ID_FILE_World = 101,
	ID_FILE_Board = 102,
	ID_FILE_EXIT  = 103,

	ID_HELP_HELP  = 201,
	ID_HELP_GH    = 202,
	ID_HELP_LW    = 203,
	ID_HELP_ABOUT = 204
};