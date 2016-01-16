#include"glow_points.h"

//control for glow bank
class glow_bank_point_ctrl :
	public editor<pcs_glow_array>
{
protected:
	string_ctrl*properties;
	glow_point_array_ctrl*points;
	int_ctrl* disp_time;
	int_ctrl* on_time; 
	int_ctrl* off_time; 
	model_list_ctrl* obj_parent;  
	int_ctrl* LOD; 
	int_ctrl* type; 
public:
	
	glow_bank_point_ctrl(wxWindow*parent,  wxString Title, int orient = wxVERTICAL)
	:editor<pcs_glow_array>(parent, orient, Title)
	{
		std::vector<std::string> op;
		op.push_back("$glow_texture=");
		
		//add controls
		add_control(disp_time =new int_ctrl(this,_("Displacement Time")),0,wxEXPAND );
		add_control(on_time =new int_ctrl(this,_("On Time")),0,wxEXPAND );
		add_control(off_time =new int_ctrl(this,_("Off Time")),0,wxEXPAND );
		add_control(obj_parent =new model_list_ctrl(this,_("Parent Subobject")),0,wxEXPAND );
		add_control(LOD =new int_ctrl(this,_("LOD")),0,wxEXPAND );
		add_control(type =new int_ctrl(this,_("Type")),0,wxEXPAND );
		add_control(properties=new suggest_ctrl<std::string, string_ctrl>(this,_("Properties"),op),0,wxEXPAND );
		add_control(points=new glow_point_array_ctrl(this,_("Glow Points")),0,wxEXPAND );
	};

	virtual ~glow_bank_point_ctrl(void){};

	void set_value(const pcs_glow_array&t){
		properties->set_value(t.properties);
		points->set_value(t.lights);
		disp_time->set_value(t.disp_time);
		on_time->set_value(t.on_time); 
		off_time->set_value(t.off_time); 
		obj_parent->set_value(t.obj_parent);  
		LOD->set_value(t.LOD); 
		type->set_value(t.type); 
	}

	//return's the control's value
	pcs_glow_array get_value(){
		pcs_glow_array t;
		t.properties = properties->get_value();
		t.lights = points->get_value();
		t.disp_time = disp_time->get_value();
		t.on_time = on_time->get_value(); 
		t.off_time = off_time->get_value(); 
		t.obj_parent = obj_parent->get_value();  
		t.LOD = LOD->get_value(); 
		t.type = type->get_value(); 
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
		pcs_glow_array glow = get_value();
		glow.Transform(transform, translation);
		set_value(glow);
	}
};

//control for an array of glowpoints
class glow_bank_array_ctrl :
	public type_array_ctrl<pcs_glow_array, glow_bank_point_ctrl>
{
public:
	glow_bank_array_ctrl(wxWindow*parent, wxString Title, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_glow_array, glow_bank_point_ctrl>(parent, Title, _(""), wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}

	virtual~glow_bank_array_ctrl(){}
};


//the chunk's editor
class GLOW_ctrl
	:public editor_ctrl<std::vector<pcs_glow_array> >
{
	glow_bank_array_ctrl*thrusters;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	GLOW_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_glow_array> >(parent, _("Glow Points"))
	{
		//add controls
		add_control(thrusters=new glow_bank_array_ctrl(this, _("")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~GLOW_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_glow_array>&t){
		data=t;
		thrusters->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_glow_array> get_value(){
		return thrusters->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_glow_points());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_glow_points(get_value());
	}
	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)thrusters->get_size())thrusters->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(2);
		ret[0] = thrusters->get_index();
		ret[1] = thrusters->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		std::vector<pcs_glow_array> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].lights.size());
			for(unsigned int j = 0; j<pnt[i].lights.size(); j++){
				o.point[i][j].pos = pnt[i].lights[j].pos;
				o.point[i][j].norm = pnt[i].lights[j].norm;
				o.point[i][j].rad = pnt[i].lights[j].radius;
			}
		}
		o.flags = OMNIPOINT_NORM | OMNIPOINT_RAD;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_glow_array> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].lights.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].lights[j].pos = points.point[i][j].pos;
				temp[i].lights[j].norm = points.point[i][j].norm;
				temp[i].lights[j].radius = points.point[i][j].rad;
			}
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = thrusters->get_index(); 
		item = thrusters->get_child_control()->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		thrusters->set_index(list); 
		thrusters->get_child_control()->set_index(item);
	}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		thrusters->get_child_control()->transform(transform, translation);
	}
};
