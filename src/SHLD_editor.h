#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

class SHLD_ctrl
	:public editor_ctrl<std::vector<pcs_shield_triangle> >
{
	string_disp*info;

public:
	static color selected_item;

	SHLD_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_shield_triangle> >(parent, _("Shields"))
	{
		//add controls
		add_control(info=new string_disp(this,_("Shield Info")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~SHLD_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_shield_triangle>&t){
		data=t;
		info->set_value(std::string(wxString::Format(_("Editing of shield data is currently unavailable\nshield has %ld faces"), data.size()).mb_str()));
	}

	//return's the control's value
	std::vector<pcs_shield_triangle> get_value(){
		return data;
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_shield_mesh());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_shield_mesh(get_value());
	}
	
	void set_item(const std::vector<int>&coord){
	}

	std::vector<int> get_item(){
		return std::vector<int>();
	};

	//this datatype won't use omnipoints
	omnipoints get_omnipoints(){return omnipoints();}
	void set_omnipoints(const omnipoints&points){}
	void get_omnipoint_coords(int&list, int&item){list = -1; item = -1;}
	void set_omnipoint_coords(int&list, int&item){}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	void transform(const matrix& transform, const vector3d& translation) {
		std::vector<pcs_shield_triangle> shields = get_value();
		for (std::vector<pcs_shield_triangle>::iterator it = shields.begin(); it < shields.end(); ++it) {
			it->Transform(get_main_window()->get_model(), transform, translation);
		}
		set_value(shields);
	}
};
