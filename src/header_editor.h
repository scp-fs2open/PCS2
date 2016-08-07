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
	
	cross_section_editor(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_crs_sect>(parent, orient, Title)
	{
		//add controls
		add_control(radius =new float_ctrl(this,_("Radius")),0,wxEXPAND );
		add_control(depth =new float_ctrl(this,_("Depth")),0,wxEXPAND );
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
	cross_section_array_ctrl(wxWindow*parent,  wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_crs_sect, cross_section_editor>(parent, subtitle, _(""), wxHORIZONTAL, wxEXPAND)
	{
	}

	virtual~cross_section_array_ctrl(){}
};



class header_ctrl
	:public editor_ctrl<header_data>
{
	default_value_vector_ctrl*bbox_min;
	default_value_vector_ctrl*bbox_max;
	vector_ctrl*cent_mass;

	default_value_float_ctrl*max_rad;
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
		add_control(bbox_min=new default_value_vector_ctrl(this,_("Bounding Box Min")),0,wxEXPAND );
		add_control(bbox_max=new default_value_vector_ctrl(this,_("Bounding Box Max")),0,wxEXPAND );
		add_control(cent_mass=new vector_ctrl(this,_("Center of Mass")),0,wxEXPAND );

		add_control(max_rad = new default_value_float_ctrl(this,_("Max Radius")), 0, wxEXPAND);
		add_control(mass = new float_ctrl(this,_("Mass")), 0, wxEXPAND);

		add_control(detail_levels=new model_array_ctrl(this, _("LOD"), _("Model#")),0,wxEXPAND );
		add_control(debris_pieces=new model_array_ctrl(this, _("Debris"), _("Model#")),0,wxEXPAND );

		add_control(cross_sections=new cross_section_array_ctrl(this, _(""), _("Cross Sections")),0,wxEXPAND );

		add_control(MOI=new matrix_ctrl(this,_("Moment of Inertia")),0,wxEXPAND );
		add_control(moi_btn = new wxButton(this, MOI_CALC_btn, _("Recalculate")));
		 
		add_control(hinfo = new string_disp(this,_("Model Info")),0,wxEXPAND );

		recalc_size();
	}
	virtual~header_ctrl(){}

	void set_value(const header_data&t){
		data=t;

		bbox_min->set_value_with_default(
				data.min_bounding_overridden ?
				data.min_bounding_override :
				data.min_bounding, data.min_bounding);
		bbox_max->set_value_with_default(
				data.max_bounding_overridden ?
				data.max_bounding_override :
				data.max_bounding, data.max_bounding);
		cent_mass->set_value(data.mass_center);

		max_rad->set_value_with_default(
				data.max_radius_overridden ?
				data.max_radius_override :
				data.max_radius, data.max_radius);
		mass->set_value(data.mass);
		
		detail_levels->set_value(data.detail_levels);
		debris_pieces->set_value(data.debris_pieces);

		cross_sections->set_value(data.cross_sections);

		MOI->set_value(data.MOI);
	}
	header_data get_value(){
		data.min_bounding_override = bbox_min->get_value();
		data.max_bounding_override = bbox_max->get_value();
		data.min_bounding = bbox_min->get_default_value();
		data.max_bounding = bbox_max->get_default_value();
		data.min_bounding_overridden = bbox_min->is_overridden();
		data.max_bounding_overridden = bbox_max->is_overridden();

		data.mass_center = cent_mass->get_value();

		data.mass = mass->get_value();
		data.max_radius_override = max_rad->get_value();
		data.max_radius = max_rad->get_default_value();
		data.max_radius_overridden = max_rad->is_overridden();
		
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
		recalculate_dimensions(model);
	}

	void recalculate_dimensions(const PCS_Model &model) {
		vector3d& min_box = data.min_bounding_overridden ? data.min_bounding_override : data.min_bounding;
		vector3d& max_box = data.max_bounding_overridden ? data.max_bounding_override : data.max_bounding;
		vector3d size = max_box - min_box;
		hinfo->set_value(std::string(wxString::Format(
						_("H: %0.2f, W: %0.2f, D: %0.2f"),
						fabs(size.y), fabs(size.x), fabs(size.z) ).mb_str()));

	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_header(get_value());
		recalculate_dimensions(model);
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
