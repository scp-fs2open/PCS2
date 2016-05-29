#include"glow_points.h"

//control for thruster bank
class thruster_bank_point_ctrl :
	public editor<pcs_thruster>
{
protected:
	string_ctrl*properties;
	glow_point_array_ctrl*points;
public:
	
	thruster_bank_point_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_thruster>(parent, orient, Title)
	{
		//add controls
		std::vector<std::string> op;
		op.push_back("$engine_subsystem=Engine");
		add_control(properties=new suggest_ctrl<std::string, string_ctrl>(this,_("Properties"), op),0,wxEXPAND );
		add_control(points=new glow_point_array_ctrl(this,_("Glow Points")),0,wxEXPAND );
	};

	virtual ~thruster_bank_point_ctrl(void){};

	void set_value(const pcs_thruster&t){
		properties->set_value(t.properties);
		points->set_value(t.points);
	}

	//return's the control's value
	pcs_thruster get_value(){
		pcs_thruster t;
		t.properties = properties->get_value();
		t.points = points->get_value();
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
		pcs_thruster thruster = get_value();
		thruster.Transform(transform, translation);
		set_value(thruster);
	}
};

//control for an array of glowpoints
class thruster_bank_array_ctrl :
	public type_array_ctrl<pcs_thruster, thruster_bank_point_ctrl>
{
public:
	thruster_bank_array_ctrl(wxWindow*parent, wxString Title, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_thruster, thruster_bank_point_ctrl>(parent, Title, _(""), wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}

	virtual~thruster_bank_array_ctrl(){}
};


//the chunk's editor
class FUEL_ctrl
	:public editor_ctrl<std::vector<pcs_thruster> >
{
	thruster_bank_array_ctrl*thrusters;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	FUEL_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_thruster> >(parent, _("Thrusters"))
	{
		//add controls
		add_control(thrusters=new thruster_bank_array_ctrl(this, _("")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~FUEL_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_thruster>&t){
		data=t;
		thrusters->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_thruster> get_value(){
		return thrusters->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_thrusters());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_thrusters(get_value());
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
		std::vector<pcs_thruster> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].points.size());
			for(unsigned int j = 0; j<pnt[i].points.size(); j++){
				o.point[i][j].pos = pnt[i].points[j].pos;
				o.point[i][j].norm = pnt[i].points[j].norm;
				o.point[i][j].rad = pnt[i].points[j].radius;
			}
		}
		o.flags = OMNIPOINT_NORM | OMNIPOINT_RAD;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_thruster> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].points.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].points[j].pos = points.point[i][j].pos;
				temp[i].points[j].norm = points.point[i][j].norm;
				temp[i].points[j].radius = points.point[i][j].rad;
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
