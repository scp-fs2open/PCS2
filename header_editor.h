#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"


//control for a cross section
class cross_section_editor :
	public editor<pcs_crs_sect>
{
protected:
	float_ctrl*radius;
	float_ctrl*depth;

public:
	
	cross_section_editor(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_crs_sect>(parent,x,y,w,h, orient, Title)
	{
		//add controls
		add_control(radius =new float_ctrl(this,0,0,60,40,_("Radius")),0,wxEXPAND );
		add_control(depth =new float_ctrl(this,0,0,60,40,_("Depth")),0,wxEXPAND ); 
	};

	virtual ~cross_section_editor(void){};

	void set_value(const pcs_crs_sect&t){
		radius->set_value(t.radius);
		depth->set_value(t.depth);
	}

	//return's the control's value
	pcs_crs_sect get_value(){
		pcs_crs_sect t;
		t.radius = radius->get_value();
		t.depth = depth->get_value();
		return t;
	}
	
};

//control for an array of cross sections
class cross_section_array_ctrl :
	public type_array_ctrl<pcs_crs_sect, cross_section_editor>
{
public:
	cross_section_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_crs_sect, cross_section_editor>(parent,x,y,w,h,subtitle, _(""), wxHORIZONTAL, wxEXPAND)
	{
	}

	virtual~cross_section_array_ctrl(){}
};



class header_ctrl
	:public editor_ctrl<header_data>
{
	vector_disp*bbox_min;
	vector_disp*bbox_max;
	vector_ctrl*cent_mass;

	float_disp*max_rad;
	float_ctrl*max_rad_override;
	float_ctrl*mass;

	model_array_ctrl*detail_levels;
	model_array_ctrl*debris_pieces;

	cross_section_array_ctrl*cross_sections;

	matrix_ctrl*MOI;
	wxButton*moi_btn;

	string_disp*hinfo;

public:
	header_ctrl(wxWindow*parent)
		:editor_ctrl<header_data>(parent, _("Header"))
	{
		add_control(bbox_min=new vector_disp(this,0,0,60,40,_("Bounding Box Min")),0,wxEXPAND );
		add_control(bbox_max=new vector_disp(this,0,30,60,40,_("Bounding Box Max")),0,wxEXPAND );
		add_control(cent_mass=new vector_ctrl(this,0,60,60,40,_("Center of Mass")),0,wxEXPAND );

		wxBoxSizer*f_sizer = new wxBoxSizer(wxHORIZONTAL);
		max_rad = new float_disp(this,0,90,30,40,_("Max Radius"));
		max_rad_override = new float_ctrl(this,0,90,30,40,_("Radius override"));
		f_sizer->Add(max_rad,1);
		f_sizer->Add(max_rad_override,1);
		add_sizer(f_sizer,0,wxEXPAND);
		add_control(mass = new float_ctrl(this,0,90,60,40,_("Mass")), 0, wxEXPAND);

		add_control(detail_levels=new model_array_ctrl(this,0,120,90,90,_("LOD"), _("Model#")),0,wxEXPAND );
		add_control(debris_pieces=new model_array_ctrl(this,0,210,90,90,_("Debris"), _("Model#")),0,wxEXPAND );

		add_control(cross_sections=new cross_section_array_ctrl(this,0,300,90,100,_(""), _("Cross Sections")),0,wxEXPAND );

		add_control(MOI=new matrix_ctrl(this,0,400,90,90,_("Moment of Inertia")),0,wxEXPAND );
		add_control(moi_btn = new wxButton(this, MOI_CALC_btn, _("Recalculate")));
		 
		add_control(hinfo = new string_disp(this,0,0,60,160,_("Model Info")),0,wxEXPAND );

		recalc_size();
	}
	virtual~header_ctrl(){}

	void set_value(const header_data&t){
		data=t;

		bbox_min->set_value(data.min_bounding);
		bbox_max->set_value(data.max_bounding);
		cent_mass->set_value(data.mass_center);

		max_rad->set_value(data.max_radius);
		max_rad_override->set_value(data.max_radius_override);
		mass->set_value(data.mass);
		
		detail_levels->set_value(data.detail_levels);
		debris_pieces->set_value(data.debris_pieces);

		cross_sections->set_value(data.cross_sections);

		MOI->set_value(data.MOI);
	}
	header_data get_value(){
		data.min_bounding = bbox_min->get_value();
		data.max_bounding = bbox_max->get_value();
		data.mass_center = cent_mass->get_value();

		data.mass = mass->get_value();
		data.max_radius = max_rad->get_value();
		data.max_radius_override = max_rad_override->get_value();
		
		data.detail_levels = detail_levels->get_value();
		data.debris_pieces = debris_pieces->get_value();

		data.cross_sections = cross_sections->get_value();

		bobboau::matrix m = MOI->get_value();
		memcpy(data.MOI, &m, sizeof(float)*9);

		return data;
	};

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_header());
		
		vector3d size = data.max_bounding - data.min_bounding;
		hinfo->set_value(std::string(wxString::Format(
						_("H: %0.2f, W: %0.2f, D: %0.2f"),
						abs(size.y), abs(size.x), abs(size.z) ).mb_str()));

	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_header(get_value());
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

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	void transform(const matrix& transform, const vector3d& translation) {
		get_main_window()->get_model().Transform(transform, translation);
		set_data(get_main_window()->get_model());
	}
};
