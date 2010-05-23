#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

class PINF_ctrl
	:public editor_ctrl<std::vector<std::string> >
{
	multi_string_array_ctrl*coments;

public:
	PINF_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<std::string> >(parent, "PINF")
	{
		//add controls
		add_control(coments=new multi_string_array_ctrl(this,0,0,120,575,"Model Comments", "",wxVERTICAL,wxEXPAND),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~PINF_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<std::string>&t){
		data=t;
		coments->set_value(t);
	}

	//return's the control's value
	std::vector<std::string> get_value(){
		return coments->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_model_info());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_model_info(get_value());
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
};
