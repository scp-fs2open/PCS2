#pragma once

#include"hardpoint_editor.h"

//editor for a dock point
class dock_pnt_ctrl :
	public editor<pcs_dock_point>
{
protected:
//controls
	string_ctrl*properties;
	path_idx_array_ctrl*paths;
	hard_point_array_ctrl*dockpoints;

public:
	
	dock_pnt_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor(parent,x,y,w,h, orient, Title)
	{
		//add controls
		std::vector<std::string> op;
		op.push_back("$name=Docking Point");
		add_control(properties=new suggest_ctrl<std::string, string_ctrl>(this,0,0,60,40,"Properties",op),0,wxEXPAND );
		add_control(paths=new path_idx_array_ctrl(this,0,0,60,110,"Paths", "", wxVERTICAL),0,wxEXPAND );
		add_control(dockpoints=new hard_point_array_ctrl(this,0,0,60,170,"Points"),0,wxEXPAND );
	};

	virtual ~dock_pnt_ctrl(void){};

	//set the control's value
	void set_value(const pcs_dock_point&t){
		properties->set_value(t.properties);
		paths->set_value(t.paths);
		dockpoints->set_value(t.dockpoints);
	}

	//return's the control's value
	pcs_dock_point get_value(){
		pcs_dock_point t;
		t.properties = properties->get_value();
		t.paths = paths->get_value();
		t.dockpoints = dockpoints->get_value();
		return t;
	}
	void set_indexes(std::vector<int> i){
		if(i[0]>-1 && i[0]<(int)dockpoints->get_size())dockpoints->set_index(i[0]);
	}
	int get_index(){
		return dockpoints->get_index();
	}
	void set_index(int idx){
		dockpoints->set_index(idx);
	}
	
};

//array of dock points
class dock_pnt_array_ctrl
	:public type_array_ctrl<pcs_dock_point, dock_pnt_ctrl>
{
public:
	dock_pnt_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}
};


class DOCK_ctrl
	:public editor_ctrl<std::vector<pcs_dock_point>>
{
	dock_pnt_array_ctrl*docking;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	DOCK_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_dock_point>>(parent, "Docking Points")
	{
		//add controls
		add_control(docking=new dock_pnt_array_ctrl(this,0,0,60,410,"Point", ""),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~DOCK_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_dock_point>&t){
		data=t;
		docking->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_dock_point> get_value(){
		return docking->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_docking());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_docking(get_value());
	}

	void set_item(const std::vector<int>&coord){
		docking->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(2);
		ret[0] = docking->get_index();
		ret[1] = docking->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		std::vector<pcs_dock_point> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].dockpoints.size());
			for(unsigned int j = 0; j<pnt[i].dockpoints.size(); j++){
				o.point[i][j].pos = pnt[i].dockpoints[j].point;
				o.point[i][j].norm = pnt[i].dockpoints[j].norm;
			}
		}
		o.flags = OMNIPOINT_NORM;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_dock_point> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].dockpoints.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].dockpoints[j].point = points.point[i][j].pos;
				temp[i].dockpoints[j].norm = points.point[i][j].norm;
			}
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = docking->get_index(); 
		item = docking->get_child_control()->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		docking->set_index(list); 
		docking->get_child_control()->set_index(item);
	}
};
