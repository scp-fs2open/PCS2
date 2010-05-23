#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"
#include"pcs2.h"

//control for a path vert
class path_vert_ctrl :
	public editor<pcs_pvert>
{
protected:
	float_ctrl*radius;
	vector_ctrl*pos;

public:
	
	path_vert_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor(parent,x,y,w,h, orient, Title)
	{
		//add controls
		add_control(radius =new float_ctrl(this,0,0,60,40,"Radius"),0,wxEXPAND );
		add_control(pos =new vector_ctrl(this,0,0,60,40,"Position"),0,wxEXPAND ); 
	};

	virtual ~path_vert_ctrl(void){};

	void set_value(const pcs_pvert&t){
		radius->set_value(t.radius);
		pos->set_value(t.pos);
	}

	//return's the control's value
	pcs_pvert get_value(){
		pcs_pvert t;
		t.radius = radius->get_value();
		t.pos = pos->get_value();
		return t;
	}
	
};

//control for an array of verts
class point_array_ctrl :
	public type_array_ctrl<pcs_pvert, path_vert_ctrl>
{
public:
	point_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl(parent,x,y,w,h,subtitle, "", wxVERTICAL, wxEXPAND, ARRAY_ITEM)
	{
	}

	virtual~point_array_ctrl(){}
};

void get_subsystem_list(std::vector<std::string>&);

//control for a path
class path_ctrl :
	public editor<pcs_path>
{
protected:
	string_ctrl*name;
	string_ctrl*parent_str; //grrr...wxWindow*parent
	point_array_ctrl*verts;

public:
	
	path_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor(parent,x,y,w,h, orient, Title)
	{
		//add controls
		add_control(name =new string_ctrl(this,0,0,60,40,"Name"),0,wxEXPAND );

		std::vector<std::string> op;
		get_subsystem_list(op);

		add_control(parent_str =new suggest_ctrl<std::string, string_ctrl>(this,0,0,60,40,"Parent",op),0,wxEXPAND ); 

		add_control(verts =new point_array_ctrl(this,0,0,60,150,"Verts", ""),0,wxEXPAND ); 
	};

	virtual ~path_ctrl(void){};

	void set_value(const pcs_path&t){
		name->set_value(t.name);
		parent_str->set_value(t.parent);
		verts->set_value(t.verts);
	}

	//return's the control's value
	pcs_path get_value(){
		pcs_path t;
		t.name = name->get_value();
		t.parent = parent_str->get_value();
		t.verts = verts->get_value();
		return t;
	}

	void set_indexes(std::vector<int> i){
		if(i[0]>-1 && i[0]<(int)verts->get_size())
			verts->set_index(i[0]);
	}
	int get_index(){
		return verts->get_index();
	}
	void set_index(int idx){
		verts->set_index(idx);
	}
	
};



//control for an array of paths
class path_array_ctrl :
	public type_array_ctrl<pcs_path, path_ctrl>
{
public:
	path_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl(parent,x,y,w,h,subtitle, "", wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}

	virtual~path_array_ctrl(){}

	DECLARE_EVENT_TABLE();
	void on_new(wxCommandEvent& event){

		event.Skip();
		//we are just adding functionality, not replaceing it

		if(event.GetId() != GetId())
			return;
		//only proccess our own new events

		//find the last path and figure out a good name for it
		std::string str;

		if(event.GetInt()<0 || event.GetInt()>(int)array.size()){
			return;
			//invalid index, bail
		}else if(array.size()<2){
			//first one
			str = "$Path01";
		}else{
			wxString s = array[event.GetInt()-1].name.c_str();
			if(s.Left(4) == "$bay" || s.Left(4) == "$Bay")
				str = "$Bay";
			else
				str = "$Path";

			long l;
			(s.Right(2)).ToLong(&l);
			if(l<10)
				str = wxString::Format("%s0%i",str.c_str(), int(l+1));
			else 
				str = wxString::Format("%s%i",str.c_str(), int(l+1));
		}
		array[event.GetInt()].name = str;
		ctrl->set_value(array[index]);
	}
};


class PATH_ctrl
	:public editor_ctrl<std::vector<pcs_path>>
{
	path_array_ctrl*paths;
	wxButton*auto_gen_btn;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	PATH_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_path>>(parent, "Paths")
	{
		//add controls
		add_control(paths=new path_array_ctrl(this,0,0,60,300,"Path", ""),0,wxEXPAND );
		add_control(auto_gen_btn = new wxButton(this, PATH_AUTO_GEN, "Auto-Gen"));
	}

	//do nothing, needed so the base destructor will get called
	virtual~PATH_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_path>&t){
		data=t;
		paths->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_path> get_value(){
		return paths->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_ai_paths());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_ai_paths(get_value());
	}
	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)paths->get_size())
			paths->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(2);
		ret[0] = paths->get_index();
		ret[1] = paths->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		std::vector<pcs_path> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].verts.size());
			for(unsigned int j = 0; j<pnt[i].verts.size(); j++){
				o.point[i][j].pos = pnt[i].verts[j].pos;
				o.point[i][j].rad = pnt[i].verts[j].radius;
			}
		}
		o.flags = OMNIPOINT_RAD|OMNIPOINT_PATH;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_path> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].verts.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].verts[j].pos = points.point[i][j].pos;
				temp[i].verts[j].radius = points.point[i][j].rad;
			}
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = paths->get_index(); 
		item = paths->get_child_control()->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		paths->set_index(list); 
		paths->get_child_control()->set_index(item);
	}

	//make a path for a turret
	pcs_path auto_gen_turret(int turret_idx){
		pcs_path path;
		pcs_turret & turret = get_main_window()->get_model().Turret(turret_idx);
		pcs_sobj & model = get_main_window()->get_model().SOBJ(turret.sobj_parent);

		path.parent = model.name;

		path.verts.resize(2);

		path.verts[0].radius = MIN(model.radius*30.0f, 1000.0f);
		path.verts[1].radius = MIN(model.radius*0.7f,100);

		path.verts[0].pos = model.offset + turret.turret_normal * (path.verts[0].radius*1.2f);
		path.verts[1].pos = model.offset + turret.turret_normal * (model.radius*4.0f);

		return path;
	}

	//make a path for a spcl point
	pcs_path auto_gen_spcl(int spcl_idx){
		pcs_path path;
		pcs_special & spcl = get_main_window()->get_model().Special(spcl_idx);

		path.parent = spcl.name;

		path.verts.resize(2);

		path.verts[0].radius = MIN(spcl.radius*6.0f,1000.0f);
		path.verts[1].radius = MIN(spcl.radius*0.3f,100.0f);

		vector3d n = MakeUnitVector(spcl.point);

		path.verts[0].pos = spcl.point + n * (path.verts[0].radius*1.2f);
		path.verts[1].pos = spcl.point + n * (spcl.radius*0.9f);

		return path;
	}

	//make a path for a dock
	pcs_path auto_gen_dock(int dock_idx){
		pcs_path path;
		pcs_dock_point & dock = get_main_window()->get_model().Dock(dock_idx);

		path.parent = wxString::Format("$dock%0.2d-01", dock_idx+1).c_str();

		path.verts.resize(4);

		path.verts[0].radius = 1000.0f;
		path.verts[1].radius = 100.0f;
		path.verts[2].radius = 10.0f;
		path.verts[3].radius = 1.0f;

		path.verts[0].pos = dock.dockpoints[0].point + dock.dockpoints[0].norm * 500.0f;
		path.verts[1].pos = dock.dockpoints[0].point + dock.dockpoints[0].norm * 100.0f;
		path.verts[2].pos = dock.dockpoints[0].point + dock.dockpoints[0].norm * 15.0f;
		path.verts[3].pos = dock.dockpoints[0].point + dock.dockpoints[0].norm * 2.0f;

		return path;
	}

	DECLARE_EVENT_TABLE();
	void on_auto_gen(wxCommandEvent& event){
		if(get_value().size() > 0){
			if(wxMessageBox("Warning: Path Auto-Generation will erase all exsisting Paths,\ndo you wish to proceed?", "WARNING", wxYES_NO) == wxNO)
				return;
		}

		std::vector<pcs_path> path;
		
		int i;
		for(i = 0; i<get_main_window()->get_model().GetTurretCount(); i++){
			path.push_back(auto_gen_turret(i));
			path[path.size()-1].name = wxString::Format("$Path%0.2d", path.size()).c_str();
		}
		for(i = 0; i<get_main_window()->get_model().GetSpecialCount(); i++){
			path.push_back(auto_gen_spcl(i));
			path[path.size()-1].name = wxString::Format("$Path%0.2d", path.size()).c_str();
		}
		for(i = 0; i<get_main_window()->get_model().GetDockingCount(); i++){
			path.push_back(auto_gen_dock(i));
			path[path.size()-1].name = wxString::Format("$Path%0.2d", path.size()).c_str();
			get_main_window()->get_model().Dock(i).paths.resize(1);
			get_main_window()->get_model().Dock(i).paths[0] = (int)path.size()-1;
		}

		std::vector<pcs_path> old_path = get_value();

		//add all fighter bay paths from the old list
		for(i = 0; i< (int)old_path.size(); i++){
			if(wxString(old_path[i].name.c_str()).Find("Bay") != wxNOT_FOUND ||
				wxString(old_path[i].name.c_str()).Find("bay") != wxNOT_FOUND)
				path.push_back(old_path[i]);
		}

		set_value(path);

	}
};

