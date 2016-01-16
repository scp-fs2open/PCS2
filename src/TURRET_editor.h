#pragma once

#include"hardpoint_editor.h"
#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

//control for a single turret
class turret_ctrl :
	public editor<pcs_turret>
{
protected:
	vector_array_ctrl*points;
	normal_ctrl*norm;
	model_list_ctrl*parent_obj;
	model_list_ctrl*physical_obj;
public:
	
	turret_ctrl(wxWindow*parent,  wxString Title, int orient = wxVERTICAL)
	:editor<pcs_turret>(parent, orient, Title)
	{
		//add controls
	//	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		add_control(parent_obj=new model_list_ctrl(this,_("Turret Base")),0,wxEXPAND );
		add_control(physical_obj=new model_list_ctrl(this,_("Turret Arm")),0,wxEXPAND );
		add_control(norm=new normal_ctrl(this,_("Normal")),0,wxEXPAND );
		add_control(points=new vector_array_ctrl(this,_("Points"), _(""), wxVERTICAL, wxEXPAND, ARRAY_ITEM),0,wxEXPAND );
	//	add_sizer(sizer,0,wxEXPAND );
	};

	virtual ~turret_ctrl(void){};

	void set_value(const pcs_turret&t){
		points->set_value(t.fire_points);
		norm->set_value(t.turret_normal);
		parent_obj->set_value(t.sobj_parent);
		physical_obj->set_value(t.sobj_par_phys);
	}

	//return's the control's value
	pcs_turret get_value(){
		pcs_turret t;
		t.fire_points = points->get_value();
		t.turret_normal = norm->get_value();
		t.sobj_parent = parent_obj->get_value();
		t.sobj_par_phys = physical_obj->get_value();
		return t;
	}
	
	void set_indexes(std::vector<int> i){
		if(i[0]>-1 && i[0]<(int)points->get_size())points->set_index(i[0]);
	}

	int get_index(){
		return points->get_index();
	}
	void set_index(int idx){
		points->set_index(idx);
	}

	void transform(const matrix& transform, const vector3d& translation) {
		pcs_turret turret = get_value();
		turret.Transform(transform, translation);
		set_value(turret);
	}
};

//control for an array of turrets
class turret_array_ctrl :
	public type_array_ctrl<pcs_turret, turret_ctrl>
{
public:
	turret_array_ctrl(wxWindow*parent, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_turret, turret_ctrl>(parent, subtitle, _(""), wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}

	virtual~turret_array_ctrl(){}
};

std::string get_turret_name(int idx);

class TGUN_ctrl
	:public editor_ctrl<std::vector<pcs_turret> >
{
	turret_array_ctrl*turrets;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	TGUN_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_turret> >(parent, _("Turrets"))
	{
		//add controls
		add_control(turrets=new turret_array_ctrl(this, _("Turret"), _("")),0,wxEXPAND );
		turrets->set_index_name_function(get_turret_name);
	}

	//do nothing, needed so the base destructor will get called
	virtual~TGUN_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_turret>&t){
		data=t;
		turrets->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_turret> get_value(){
		return turrets->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_turrets());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_turrets(get_value());
	}
	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)turrets->get_size())turrets->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(2);
		ret[0] = turrets->get_index();
		ret[1] = turrets->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		std::vector<pcs_turret> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].fire_points.size());
			for(unsigned int j = 0; j<pnt[i].fire_points.size(); j++){
				o.point[i][j].pos = pnt[i].fire_points[j];
				o.point[i][j].norm = pnt[i].turret_normal;
				o.point[i][j].model = pnt[i].sobj_par_phys != -1 ?
					pnt[i].sobj_par_phys : pnt[i].sobj_parent;
			}
		}

		o.flags = OMNIPOINT_NORM | OMNIPOINT_COMMON_NORMAL;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_turret> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].fire_points.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			if(points.point[i].size()<1)continue;
			temp[i].turret_normal = points.point[i][0].norm;
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].fire_points[j] = points.point[i][j].pos;
			}
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = turrets->get_index(); 
		item = turrets->get_child_control()->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		turrets->set_index(list); 
		turrets->get_child_control()->set_index(item);
	}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		turrets->get_child_control()->transform(transform, translation);
	}
};
