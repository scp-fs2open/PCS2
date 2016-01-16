#include"hardpoint_editor.h"

//control for eye
class eye_point_ctrl :
	public editor<pcs_eye_pos>
{
protected:
	model_list_ctrl*sobj_number;
	vector_ctrl*sobj_offset;
	normal_ctrl*norm;
public:
	
	eye_point_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_eye_pos>(parent, orient, Title)
	{
		//add controls
		add_control(sobj_number=new model_list_ctrl(this,_("Subobject Parent")),0,wxEXPAND );
		add_control(sobj_offset=new vector_ctrl(this,_("Offset")),0,wxEXPAND );
		add_control(norm=new normal_ctrl(this,_("Normal")),0,wxEXPAND );
	};

	virtual ~eye_point_ctrl(void){};

	void set_value(const pcs_eye_pos&t){
		sobj_offset->set_value(t.sobj_offset);
		norm->set_value(t.normal);
		sobj_number->set_value(t.sobj_number);
	}

	//return's the control's value
	pcs_eye_pos get_value(){
		pcs_eye_pos t;
		t.sobj_offset = sobj_offset->get_value();
		t.normal = norm->get_value();
		t.sobj_number = sobj_number->get_value();
		return t;
	}
	
	void transform(const matrix& transform, const vector3d& translation) {
		pcs_eye_pos eye = get_value();
		eye.Transform(transform, translation);
		set_value(eye);
	}
};

//control for an array of hardpoints
class eye_point_array_ctrl :
	public type_array_ctrl<pcs_eye_pos, eye_point_ctrl>
{
public:
	eye_point_array_ctrl(wxWindow*parent, wxString Title, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_eye_pos, eye_point_ctrl>(parent,Title, _(""), wxVERTICAL, wxEXPAND, ARRAY_ITEM)
	{
	}

	virtual~eye_point_array_ctrl(){}
};

//the chunk's editor
class EYE_ctrl
	:public editor_ctrl<std::vector<pcs_eye_pos> >
{
	eye_point_array_ctrl*eye;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	EYE_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_eye_pos> >(parent, _("Eye Points"))
	{
		//add controls
		add_control(eye=new eye_point_array_ctrl(this, _("")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~EYE_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_eye_pos>&t){
		data=t;
		eye->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_eye_pos> get_value(){
		return eye->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_eyes());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_eyes(get_value());
	}

	void set_item(const std::vector<int>&coord){
		if(coord[0]>-1 && coord[0]<(int)eye->get_size())eye->set_index(coord[0]);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(1);
		ret[0] = eye->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		o.point.resize(1);
		std::vector<pcs_eye_pos> pnt = get_value();
		o.point[0].resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[0][i].pos = pnt[i].sobj_offset;
			o.point[0][i].norm = pnt[i].normal;
			o.point[0][i].model = pnt[i].sobj_number;
		}
		o.flags = OMNIPOINT_NORM;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_eye_pos> temp = get_value();
		if(points.point.size()!=1 || temp.size()!=points.point[0].size())return;

		for(unsigned int i = 0; i<points.point[0].size(); i++){
			temp[i].sobj_offset = points.point[0][i].pos;
			temp[i].normal = points.point[0][i].norm;
			temp[i].sobj_number = points.point[0][i].model;
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = 0; 
		item = eye->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		eye->set_index(item);
	}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		eye->get_child_control()->transform(transform, translation);
	}
};
