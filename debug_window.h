#pragma once

#include <wx/wx.h>

class debug_window
	:public wxDialog
{
	wxStaticText*text;

public:
	debug_window(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _("dialogBox"))
		:wxDialog(parent, id, title, pos, size, style, name)
	{
		text = new wxStaticText((wxWindow*)this,-1,_(""));
		Show();
	}

	void set_text(wxString&str){
		if(this)
			text->SetLabel(str);
	}
};
