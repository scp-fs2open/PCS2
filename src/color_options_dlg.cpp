#include "color_options_dlg.h"
#include <wx/colordlg.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>
#include <wx/colordlg.h>
#include <wx/stdpaths.h>
#include "chunk_editors.h"

//some global nastyness

wxString chunk_titles[n_editor_colors] = {_("Gun Points"), _("Missile Points"), _("Thruster Points"), _("Glow Points"), _("Special Points"), _("Turrets"), _("Paths"), _("Eye Points"), _("Dock Points"),_("ACEN Point"),_("Wireframe Overlays")};
wxString chunk_paths[n_editor_colors] = {_("GPNT/"), _("MPNT/"), _("FUEL/"), _("GLOW/"), _("SPCL/"), _("TGUN/"), _("PATH/"), _("EYE/"), _("DOCK/"),_("ACEN/"),_("OVERLAY/")};
color_options default_colors[n_editor_colors];

//default colors, stuffed into an array
bool defaults_built = false;
void build_defaults(){
	int i = 0;
	default_colors[i++] = color_options(WPNT_ctrl<GUN>::unselected, WPNT_ctrl<GUN>::selected_list, WPNT_ctrl<GUN>::selected_item);
	default_colors[i++] = color_options(WPNT_ctrl<MISSILE>::unselected, WPNT_ctrl<MISSILE>::selected_list, WPNT_ctrl<MISSILE>::selected_item);
	default_colors[i++] = color_options(FUEL_ctrl::unselected, FUEL_ctrl::selected_list, FUEL_ctrl::selected_item);

	default_colors[i++] = color_options(GLOW_ctrl::unselected, GLOW_ctrl::selected_list, GLOW_ctrl::selected_item);
	default_colors[i++] = color_options(SPCL_ctrl::unselected, SPCL_ctrl::selected_list, SPCL_ctrl::selected_item);
	default_colors[i++] = color_options(TGUN_ctrl::unselected, TGUN_ctrl::selected_list, TGUN_ctrl::selected_item);

	default_colors[i++] = color_options(PATH_ctrl::unselected, PATH_ctrl::selected_list, PATH_ctrl::selected_item);
	default_colors[i++] = color_options(EYE_ctrl::unselected, EYE_ctrl::selected_list, EYE_ctrl::selected_item);
	default_colors[i++] = color_options(DOCK_ctrl::unselected, DOCK_ctrl::selected_list, DOCK_ctrl::selected_item);

	default_colors[i++] = color_options(ACEN_ctrl::unselected, ACEN_ctrl::selected_list, ACEN_ctrl::selected_item);

	default_colors[i++] = color_options(SOBJ_ctrl::selected_item, TXTR_ctrl::selected_item, SHLD_ctrl::selected_item);

	defaults_built = true;
}

//set's the three colors to what is int the registry for the passed chunk number
void set_editor_color(color&u, color&l, color&i, int chunk){

	wxConfigBase *con = wxConfigBase::Get();

		con->SetPath(_T("/gr_options/editor_colors/"));
		con->SetPath(chunk_paths[chunk]);

		long r,g,b;

		con->Read(_("unselected/R"), &r, default_colors[chunk].unsel.r);
		con->Read(_("unselected/G"), &g, default_colors[chunk].unsel.g);
		con->Read(_("unselected/B"), &b, default_colors[chunk].unsel.g);
		u = color(r,g,b);

		con->Read(_("selected/list/R"), &r, default_colors[chunk].sel_l.r);
		con->Read(_("selected/list/G"), &g, default_colors[chunk].sel_l.g);
		con->Read(_("selected/list/B"), &b, default_colors[chunk].sel_l.b);
		l = color(r,g,b);

		con->Read(_("selected/item/R"), &r, default_colors[chunk].sel_i.r);
		con->Read(_("selected/item/G"), &g, default_colors[chunk].sel_i.g);
		con->Read(_("selected/item/B"), &b, default_colors[chunk].sel_i.b);

		i = color(r,g,b);
}

//set's all editor colors to what the configuration file says it should be
void set_editor_colors(){
	if(!defaults_built)build_defaults();

	int i = 0;

	set_editor_color(WPNT_ctrl<GUN>::unselected, WPNT_ctrl<GUN>::selected_list, WPNT_ctrl<GUN>::selected_item, i++);
	set_editor_color(WPNT_ctrl<MISSILE>::unselected, WPNT_ctrl<MISSILE>::selected_list, WPNT_ctrl<MISSILE>::selected_item, i++);
	set_editor_color(FUEL_ctrl::unselected, FUEL_ctrl::selected_list, FUEL_ctrl::selected_item, i++);

	set_editor_color(GLOW_ctrl::unselected, GLOW_ctrl::selected_list, GLOW_ctrl::selected_item, i++);
	set_editor_color(SPCL_ctrl::unselected, SPCL_ctrl::selected_list, SPCL_ctrl::selected_item, i++);
	set_editor_color(TGUN_ctrl::unselected, TGUN_ctrl::selected_list, TGUN_ctrl::selected_item, i++);

	set_editor_color(PATH_ctrl::unselected, PATH_ctrl::selected_list, PATH_ctrl::selected_item, i++);
	set_editor_color(EYE_ctrl::unselected, EYE_ctrl::selected_list, EYE_ctrl::selected_item, i++);
	set_editor_color(DOCK_ctrl::unselected, DOCK_ctrl::selected_list, DOCK_ctrl::selected_item, i++);

	set_editor_color(ACEN_ctrl::unselected, ACEN_ctrl::selected_list, ACEN_ctrl::selected_item, i++);

	set_editor_color(SOBJ_ctrl::selected_item, TXTR_ctrl::selected_item, SHLD_ctrl::selected_item, i++);
}

////////////////////////////////////end global stuf nastyness///////////////////////////////////////


///////////////////////////control for selecting the color of a chunk////////////////////
chunk_color_ctrl::chunk_color_ctrl(wxWindow*parent, wxString Title, wxString Path, int Flags, wxString title1, wxString title2, wxString title3)
	:editor<color_options>(parent,wxVERTICAL, Title), path(Path)
{
	add_control(unselected_btn = new wxButton(this, COLOP_UNSEL, title1, wxDefaultPosition, wxDefaultSize), 1,wxEXPAND, 5);
	add_control(selected_list = new wxButton(this, COLOP_SEL_L, title2, wxDefaultPosition, wxDefaultSize), 1,wxEXPAND, 5);
	add_control(selected_item = new wxButton(this, COLOP_SEL_I, title3, wxDefaultPosition, wxDefaultSize), 1,wxEXPAND);
};


color_options chunk_color_ctrl::get_value(){
	return color_options(color(unselected_btn->GetBackgroundColour()), color(selected_list->GetBackgroundColour()), color(selected_item->GetBackgroundColour()));
}

void chunk_color_ctrl::set_value(const color_options&O){
	set_btn(unselected_btn, O.unsel);
	set_btn(selected_list, O.sel_l);
	set_btn(selected_item, O.sel_i);
}

void chunk_color_ctrl::set_btn(wxButton*btn, color col){
	btn->SetBackgroundColour(wxColour(col.r, col.g, col.b, col.a));
	btn->SetForegroundColour(wxColour((col.r+128)%255,(col.g+128)%255,(col.b+128)%255));
}

void chunk_color_ctrl::set_btn(wxButton*btn, wxColour col){
	if(!col.IsOk())return;
	btn->SetBackgroundColour(col);
	btn->SetForegroundColour(wxColour((col.Red()+128)%255,(col.Green()+128)%255,(col.Blue()+128)%255));
}

void chunk_color_ctrl::on_color(wxCommandEvent &event){
	set_btn(
		((wxButton*)(event.GetEventObject())),
		wxGetColourFromUser(this, 
			((wxButton*)(event.GetEventObject()))->GetBackgroundColour(), 
			((wxButton*)(event.GetEventObject()))->GetLabel())
		);
}

BEGIN_EVENT_TABLE(chunk_color_ctrl, editor<color_options>)
	EVT_BUTTON(COLOP_UNSEL, chunk_color_ctrl::on_color)
	EVT_BUTTON(COLOP_SEL_L, chunk_color_ctrl::on_color)
	EVT_BUTTON(COLOP_SEL_I, chunk_color_ctrl::on_color)
END_EVENT_TABLE()


///////////////////////////the dialog///////////////////////////////////

color_options_dlg::color_options_dlg(wxWindow* parent)
	:wxDialog(parent, COLOP_DLG, wxString(_("Color Options")))
{
	if(!defaults_built)build_defaults();

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* controls_sizer = new wxGridSizer(3, 10, 5);

	int i = 0;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;

	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this,  chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;

	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;
	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;

	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i]), 1, wxEXPAND);
	i++;

	controls_sizer->Add(color_selector[i] = new chunk_color_ctrl(this, chunk_titles[i], chunk_paths[i], 0, _("Subobjects"), _("Textures"), _("Shields")), 1, wxEXPAND);

	sizer->Add(controls_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

	wxSizer* buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

	buttons_sizer->AddStretchSpacer(3);
	buttons_sizer->Add(ok_btn = new wxButton(this, wxID_OK, _("OK")));
	buttons_sizer->AddStretchSpacer(2);
	buttons_sizer->Add(cancel_btn = new wxButton(this, wxID_CANCEL, _("Cancel")));
	buttons_sizer->AddStretchSpacer(3);
	sizer->AddSpacer(15);
	sizer->Add(buttons_sizer, 0, wxEXPAND);
	sizer->AddSpacer(5);

	SetSizerAndFit(sizer);

	wxConfigBase *con = wxConfigBase::Get();
		
	for(i = 0; i<n_editor_colors; i++){
		color_options ops;

		con->SetPath(_T("/gr_options/editor_colors/"));

		con->SetPath(color_selector[i]->path.c_str());

		long r,g,b;

		con->Read(_("unselected/R"), &r, default_colors[i].unsel.r);
		con->Read(_("unselected/G"), &g, default_colors[i].unsel.g);
		con->Read(_("unselected/B"), &b, default_colors[i].unsel.g);
		ops.unsel = color(r,g,b);

		con->Read(_("selected/list/R"), &r, default_colors[i].sel_l.r);
		con->Read(_("selected/list/G"), &g, default_colors[i].sel_l.g);
		con->Read(_("selected/list/B"), &b, default_colors[i].sel_l.b);
		ops.sel_l = color(r,g,b);

		con->Read(_("selected/item/R"), &r, default_colors[i].sel_i.r);
		con->Read(_("selected/item/G"), &g, default_colors[i].sel_i.g);
		con->Read(_("selected/item/B"), &b, default_colors[i].sel_i.b);
		ops.sel_i = color(r,g,b);

		color_selector[i]->set_value(ops);
	}

	ShowModal();
}

void color_options_dlg::on_ok(wxCommandEvent &event){
	wxConfigBase *con = wxConfigBase::Get();
		
	for(int i = 0; i<n_editor_colors; i++){
		color_options ops = color_selector[i]->get_value();

		con->SetPath(_T("/gr_options/editor_colors/"));

		con->SetPath(color_selector[i]->path.c_str());

		con->Write(_("unselected/R"), (byte)ops.unsel.r);
		con->Write(_("unselected/G"), (byte)ops.unsel.g);
		con->Write(_("unselected/B"), (byte)ops.unsel.b);

		con->Write(_("selected/list/R"), (byte)ops.sel_l.r);
		con->Write(_("selected/list/G"), (byte)ops.sel_l.g);
		con->Write(_("selected/list/B"), (byte)ops.sel_l.b);

		con->Write(_("selected/item/R"), (byte)ops.sel_i.r);
		con->Write(_("selected/item/G"), (byte)ops.sel_i.g);
		con->Write(_("selected/item/B"), (byte)ops.sel_i.b);
	}
	set_editor_colors();

	// -------- Flush to Disk -------- 
	wxFileOutputStream cfg_out(wxStandardPaths::Get().GetUserConfigDir()+wxString(CONFIG_FILE));
	((wxFileConfig*)con)->Save(cfg_out);

	event.Skip();
}


BEGIN_EVENT_TABLE(color_options_dlg, wxDialog)
	EVT_BUTTON(wxID_OK, color_options_dlg::on_ok)
END_EVENT_TABLE()

