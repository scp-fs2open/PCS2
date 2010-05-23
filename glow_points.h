#pragma once

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"


//control for glow point
class glow_point_ctrl :
	public editor<pcs_thrust_glow>
{
protected:
	float_ctrl*rad;
	vector_ctrl*pos;
	normal_ctrl*norm;
public:
	
	glow_point_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor(parent,x,y,w,h, orient, Title)
	{
		//add controls
		add_control(rad=new float_ctrl(this,0,0,60,40,"Radius"),0,wxEXPAND );
		add_control(pos=new vector_ctrl(this,0,0,60,40,"position"),0,wxEXPAND );
		add_control(norm=new normal_ctrl(this,0,0,60,40,"Normal"),0,wxEXPAND );
	};

	virtual ~glow_point_ctrl(void){};

	void set_value(const pcs_thrust_glow&t){
		pos->set_value(t.pos);
		norm->set_value(t.norm);
		rad->set_value(t.radius);
	}

	//return's the control's value
	pcs_thrust_glow get_value(){
		pcs_thrust_glow t;
		t.pos = pos->get_value();
		t.norm = norm->get_value();
		t.radius = rad->get_value();
		return t;
	}
	
};

//control for an array of glowpoints
class glow_point_array_ctrl :
	public type_array_ctrl<pcs_thrust_glow, glow_point_ctrl>
{
public:
	glow_point_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxHORIZONTAL)
		:type_array_ctrl(parent,x,y,w,h,Title, "", wxVERTICAL, wxEXPAND, ARRAY_ITEM)
	{
	}

	virtual~glow_point_array_ctrl(){}
};
