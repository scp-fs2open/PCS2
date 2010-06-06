#pragma once

#include <wx/wx.h>
#include "editor.h"
#include "color.h"
#include "pcs2_CIDs.h"

#define n_editor_colors 11

struct color_options{
	color unsel, sel_l, sel_i;
	color_options(color u, color l, color i)
		:unsel(u), sel_l(l), sel_i(i)
	{}
	color_options(){};
};

class chunk_color_ctrl :
	public editor<color_options>
{
protected:
		wxButton *unselected_btn, *selected_list, *selected_item;
public:
		wxString path;
	
	chunk_color_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString Path, int Flags=0, wxString title1 = _("Unselected"), wxString title2 = _("Selected List"), wxString title3 = _("Selected Item"));
public:
	virtual ~chunk_color_ctrl(void){};

	color_options get_value();

	void set_value(const color_options&O);

	void set_btn(wxButton*btn, color col);
	void set_btn(wxButton*btn, wxColour col);

	void on_color(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};


class color_options_dlg
	:public wxDialog
{
	chunk_color_ctrl* color_selector[n_editor_colors];

	wxButton *ok_btn, *cancel_btn;
public:
	color_options_dlg(wxWindow* parent);

	void on_ok(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};
