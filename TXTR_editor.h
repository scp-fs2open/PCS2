#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

std::string get_texture_name(int idx);

class TXTR_ctrl
	:public editor_ctrl<std::vector<std::string> >
{
	string_array_ctrl*textures;

	wxButton*reload_btn;
	wxButton*open_texture_btn;
public:
	static color selected_item;

	TXTR_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<std::string> >(parent, _("Textures"))
	{
		//add controls
		add_control(textures=new string_array_ctrl(this,0,0,90,90,_(""), _("texture")),0,wxEXPAND );
		textures->set_index_name_function(get_texture_name);
		add_control(reload_btn = new wxButton(this, TXTR_RELOAD, _("Reload Textures")));
		add_control(open_texture_btn = new wxButton(this, TXTR_EXT_OPEN, _("Open Externaly")));
	}

	//do nothing, needed so the base destructor will get called
	virtual~TXTR_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<std::string>&t){
		data=t;
		textures->set_value(t);
	}

	//return's the control's value
	std::vector<std::string> get_value(){
		return textures->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_textures());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_textures(get_value());
	}
	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)textures->get_size())textures->set_index(coord[0]);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(1);
		ret[0] = textures->get_index();
		return ret;
	};
	//this datatype won't use omnipoints
	omnipoints get_omnipoints(){return omnipoints();}
	void set_omnipoints(const omnipoints&points){}
	void get_omnipoint_coords(int&list, int&item){list = -1; item = textures->get_index();}
	void set_omnipoint_coords(int&list, int&item){}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
	}
};
