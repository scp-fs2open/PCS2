#pragma once
#include <wx/wx.h>

//a modified version of a combo box that sends a mesage I need that 
//wxWindows doesn't do by default for some insine reason
class text_ctrl:public wxTextCtrl{
	void do_focus(wxFocusEvent&);

	DECLARE_EVENT_TABLE();
public:
	text_ctrl(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0):wxTextCtrl(parent, id, value, pos, size, style){};
};
