#pragma once
#include <wx/wx.h>


//a modified version of a combo box that sends a mesage I need that 
//wxWindows doesn't do by default for some insine reason
class combo:public wxComboBox{
	void do_focus(wxFocusEvent&);
	void do_selected(wxCommandEvent&);

	DECLARE_EVENT_TABLE();
public:
	combo(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0):wxComboBox(parent,id, value, pos, size, n, choices, style){};
};
