#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

class spcl_point_ctrl :
	public editor<pcs_special>
{
protected:
	string_ctrl*name;
	multi_string_ctrl*properties;
	vector_ctrl*point;
    float_ctrl*radius;
public:
	
	spcl_point_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_special>(parent,x,y,w,h, orient, Title)
	{
		//add controls
		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		std::vector<std::string> op;
		op.push_back("$Engine");
		op.push_back("$Weapons");
		op.push_back("$Sensors");
		op.push_back("$Communication");
		op.push_back("$Navigation");
		op.push_back("$Fighterbay");
		op.push_back("$Bridge");
		op.push_back("$Reactor");
		sizer->Add(name=new suggest_ctrl<std::string, string_ctrl>(this,0,0,60,40,_("Name"),op, SUGGEST_REPLACE),1 );
		sizer->Add(20,40);
		sizer->Add(radius=new float_ctrl(this,0,0,60,40,_("Radius")),1 );
		add_sizer(sizer,0,wxEXPAND );
		add_control(properties=new multi_string_ctrl(this,0,0,60,80,_("Properties")),0,wxEXPAND );
		add_control(point=new vector_ctrl(this,0,0,60,40,_("Position")),0,wxEXPAND );
	};

	virtual ~spcl_point_ctrl(void){};

	void set_value(const pcs_special&t){
		point->set_value(t.point);
		name->set_value(t.name);
		properties->set_value(t.properties);
		radius->set_value(t.radius);
	}

	//return's the control's value
	pcs_special get_value(){
		pcs_special t;
		t.name = name->get_value();
		t.radius = radius->get_value();
		t.properties = properties->get_value();
		t.point = point->get_value();
		return t;
	}
	
};

//control for an array of Special points
class spcl_point_array_ctrl :
	public type_array_ctrl<pcs_special, spcl_point_ctrl>
{
public:
	spcl_point_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_special, spcl_point_ctrl>(parent,x,y,w,h,subtitle, _(""), wxVERTICAL, wxEXPAND, ARRAY_ITEM)
	{
	}

	virtual~spcl_point_array_ctrl(){}
};

std::string get_subsystem_name(int idx);

class SPCL_ctrl
	:public editor_ctrl<std::vector<pcs_special> >
{
	spcl_point_array_ctrl*points;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	SPCL_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_special> >(parent, _("Special Points"))
	{
		//add controls
		add_control(points=new spcl_point_array_ctrl(this,0,0,60,230,_("Point"), _("")),0,wxEXPAND );
		points->set_index_name_function(get_subsystem_name);
	}

	//do nothing, needed so the base destructor will get called
	virtual~SPCL_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_special>&t){
		data=t;
		points->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_special> get_value(){
		return points->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_special());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_special(get_value());
	}
	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)points->get_size())points->set_index(coord[0]);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(1);
		ret[0] = points->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		o.point.resize(1);
		std::vector<pcs_special> pnt = get_value();
		o.point[0].resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[0][i].pos = pnt[i].point;
			o.point[0][i].rad = pnt[i].radius;
		}
		o.flags = OMNIPOINT_RAD;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points_){
		std::vector<pcs_special> temp = get_value();
		if(points_.point.size()!=1 || temp.size()!=points_.point[0].size())return;
		for(unsigned int i = 0; i<points_.point[0].size(); i++){
			temp[i].point = points_.point[0][i].pos;
			temp[i].radius = points_.point[0][i].rad;
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = 0; 
		item = points->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		points->set_index(item);
	}
};
