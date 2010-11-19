#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"


//control for a single slot
class hard_point_ctrl :
	public editor<pcs_hardpoint>
{
protected:
	vector_ctrl*point;
	normal_ctrl*norm;
public:
	
	hard_point_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_hardpoint>(parent,x,y,w,h, orient, Title)
	{
		//add controls
		add_control(point=new vector_ctrl(this,0,0,60,40,_("Point")),0,wxEXPAND );
		add_control(norm=new normal_ctrl(this,0,0,60,40,_("Normal")),0,wxEXPAND );
	};

	virtual ~hard_point_ctrl(void){};

	void set_value(const pcs_hardpoint&t){
		point->set_value(t.point);
		norm->set_value(t.norm);
	}

	//return's the control's value
	pcs_hardpoint get_value(){
		pcs_hardpoint t;
		t.point = point->get_value();
		t.norm = norm->get_value();
		return t;
	}
	
};

//control for an array of hardpoints
class hard_point_array_ctrl :
	public type_array_ctrl<pcs_hardpoint, hard_point_ctrl>
{
public:
	hard_point_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxHORIZONTAL)
		:type_array_ctrl<pcs_hardpoint, hard_point_ctrl>(parent,x,y,w,h,Title, _(""), wxVERTICAL, wxEXPAND, ARRAY_ITEM)
	{

	}

	virtual~hard_point_array_ctrl(){}


	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
	}
};
