#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

class ACEN_ctrl
	:public editor_ctrl<vector3d>
{
	vector_ctrl*auto_center;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	ACEN_ctrl(wxWindow*parent)
		:editor_ctrl<vector3d>(parent, _("Auto-Center"))
	{
		//add controls
		add_control(auto_center=new vector_ctrl(this,0,0,60,40,_("Point")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~ACEN_ctrl(){}

	//set's the control's value
	void set_value(const vector3d&t){
		data=t;
		auto_center->set_value(t);
	}

	//return's the control's value
	vector3d get_value(){
		return auto_center->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.GetAutoCenter());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.SetAutoCenter(get_value());
	}
	
	void set_item(const std::vector<int>&coord){
	}

	std::vector<int> get_item(){
		return std::vector<int>();
	};
	omnipoints get_omnipoints(){
		omnipoints o;
		o.point.resize(1);
		o.point[0].resize(1);
		o.point[0][0].pos = get_value();
		o.selected_item = selected_item;
		return o;
	}
	void set_omnipoints(const omnipoints&points){
		if(points.point.size() == 1 && points.point[0].size() == 1){
			set_value(points.point[0][0].pos);
		}
	}
	void get_omnipoint_coords(int&list, int&item){list = 0; item = 0;}
	void set_omnipoint_coords(int&list, int&item){}
};
