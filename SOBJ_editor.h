#pragma once

#include <wx/event.h>

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"



//editor for a subobject
class sobj_ctrl :
	public editor<pcs_sobj>
{
	pcs_sobj data;
protected:
//controls
	string_ctrl*name;
	primitive_list_ctrl<int>*movement_type;
	primitive_list_ctrl<int>*movement_axis;
	multi_string_ctrl*properties;
	vector_ctrl*offset;

public:

	sobj_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_sobj>(parent,x,y,w,h, orient, Title)
	{
		std::vector<primitive_list_item<int> > list;

		//add controls
		add_control(name=new string_ctrl(this,0,0,60,40,_("Name")),0,wxEXPAND );

		list.resize(2);

		list[0].title = "No Movement";
		list[0].data = MNONE;

		list[1].title = "Rotate";
		list[1].data = ROTATE;

		add_control(movement_type=new primitive_list_ctrl<int>(this, list,0,0,60,40,_("Movement Type")),0,wxEXPAND );

		list.resize(4);

		list[0].title = "None";
		list[0].data = ANONE;

		list[1].title = "X Axis";
		list[1].data = MV_X;

		list[2].title = "Y Axis";
		list[2].data = MV_Y;

		list[3].title = "Z Axis";
		list[3].data = MV_Z;

		add_control(movement_axis=new primitive_list_ctrl<int>(this, list,0,0,60,40,_("Axis")),0,wxEXPAND );

		add_control(offset=new vector_ctrl(this,0,0,60,40,_("Offset")),0,wxEXPAND );

		std::vector<std::string> op;
		op.push_back("$special=subsystem\n");
		op.push_back("$name=x\n");
		op.push_back("");
		op.push_back("$fov=180\n");
		op.push_back("");
		op.push_back("$rotate=x\n");
		op.push_back("");
		op.push_back("$detail_sphere:1,x\n");
		op.push_back("$detail_box:1\n");
		op.push_back("$box_min: x,x,x\n");
		op.push_back("$box_max: x,x,x\n");
		op.push_back("");
		op.push_back("$triggered:\n");
		op.push_back("");
		op.push_back("$stepped\n");
		op.push_back("$steps=4\n");
		op.push_back("$t_paused=2\n");
		op.push_back("$t_transit=6\n");
		op.push_back("");
		op.push_back("$look_at:0\n");
		op.push_back("$uvec:0,1,0\n");
		op.push_back("$fvec:0,0,1\n");

		add_control(properties=new suggest_ctrl<std::string, multi_string_ctrl>(this,0,0,60,40,_("Properties"),op, SUGGEST_APPEND),1,wxEXPAND );
		
	};

	virtual ~sobj_ctrl(void){};

	//set the control's value
	void set_value(const pcs_sobj&t){
		data=t;
		name->set_value(t.name);
		movement_type->set_value(t.movement_type);
		movement_axis->set_value(t.movement_axis);
		offset->set_value(t.offset);
		properties->set_value(t.properties);
	}

	//return's the control's value
	pcs_sobj get_value(){
		data.name = name->get_value();
		data.movement_type = movement_type->get_value();
		data.movement_axis = movement_axis->get_value();
		data.offset = offset->get_value();
		data.properties = properties->get_value();
		return data;
	}
	void set_pos(vector3d p){
		offset->set_value(data.offset = p);
	}
	
};

class SOBJ_ctrl
	:public editor_ctrl<pcs_sobj>
{
	sobj_ctrl*sobj;

	wxCheckBox*bsp_render_box;

	string_disp*sobj_info;

	int sobj_num;

	wxBitmapButton*cpy_btn;
	wxBitmapButton*del_btn;

public:
	static color selected_item;

	SOBJ_ctrl(wxWindow*parent,int Sobj_num)
		:editor_ctrl<pcs_sobj>(parent, _("Subobject")),sobj_num(Sobj_num)
	{
		//add controls
		wxBoxSizer*b=new wxBoxSizer(wxHORIZONTAL);
		b->Add(cpy_btn = new wxBitmapButton(this, SOBJ_BUTTON_CPY, wxBitmap(copy_btn),wxPoint(42,0),wxSize(16,16)));
		b->Add(del_btn = new wxBitmapButton(this, SOBJ_BUTTON_DEL, wxBitmap(delete_btn),wxPoint(58,0),wxSize(16,16)));
		add_sizer(b);

		cpy_btn->SetToolTip(_("Copy"));
		del_btn->SetToolTip(_("Delete"));

		add_control(sobj=new sobj_ctrl(this,0,0,60,440,_("")),0,wxEXPAND );

		add_control(bsp_render_box=new wxCheckBox(this, BSP_DRAW, _("Draw BSP Debug Info"), wxPoint(0,415), wxSize(140,15)));
		bsp_render_box->Disable();
		add_control(sobj_info = new string_disp(this,0,0,60,160,_("Subobject Info")),0,wxEXPAND );

	}

	//do nothing, needed so the base destructor will get called
	virtual~SOBJ_ctrl(){}

	//set's the control's value
	void set_value(const pcs_sobj&t){
		data=t;
		sobj->set_value(t);
	}

	//return's the control's value
	pcs_sobj get_value(){
		return sobj->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		if(sobj_num >=0 && sobj_num<model.GetSOBJCount()){
			set_value(model.SOBJ(sobj_num));
		
			if(model.get_bsp_cache_status())
				bsp_render_box->Enable();
			else
				bsp_render_box->Disable();

			bsp_render_box->SetValue(model.draw_bsp);

			vector3d size = data.bounding_box_max_point - data.bounding_box_min_point;
			sobj_info->set_value(std::string(wxString::Format(
				_("Poly Count:       %i\nChild Poly Count: %i\nTotal:            %i\nBoundingbox Min:  %0.2f, %0.2f, %0.2f\nBoundingbox Max:  %0.2f, %0.2f, %0.2f\nH: %0.2f, W: %0.2f, D: %0.2f\nRadius:           %f\nParent submodel:  %s\n"),
					
				data.polygons.size(), 
				model.get_child_subobj_poly_count(sobj_num), 
				data.polygons.size() + model.get_child_subobj_poly_count(sobj_num), 
				data.bounding_box_min_point.x,data.bounding_box_min_point.y,data.bounding_box_min_point.z, 
				data.bounding_box_max_point.x,data.bounding_box_max_point.y,data.bounding_box_max_point.z, 
				abs(size.y), abs(size.x), abs(size.z),
				data.radius, 
				(
					(data.parent_sobj>-1)?
						wxString(model.SOBJ(data.parent_sobj).name.c_str(), wxConvUTF8).c_str()
						:_("*NONE*")
				)
					).mb_str())
				);
			}
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		if(sobj_num >=0 && sobj_num<model.GetSOBJCount())
		model.SOBJ(sobj_num)=get_value();
	}

	void set_item(const std::vector<int>&coord){
		if(coord[0] >-1)
			sobj_num = coord[0];
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(1);
		ret[0] = sobj_num;
		return ret;
	};
	
	omnipoints get_omnipoints(){
		omnipoints o;
		o.point.resize(1);
		o.point[0].resize(1);
		o.point[0][0].pos = get_value().offset;
		return o;
	}
	void set_omnipoints(const omnipoints&points){
		if(points.point.size() == 1 && points.point[0].size() == 1){
			sobj->set_pos(points.point[0][0].pos);
      wxCommandEvent event(SUBOBJECT_UPDATE, GetId());
			GetEventHandler()->ProcessEvent(event);
		}
	}
	void get_omnipoint_coords(int&list, int&item){
		list = 0; 
		item = 0;
	}
	void set_omnipoint_coords(int&list, int&item){
	}

	DECLARE_EVENT_TABLE();
	void on_BSP_render_change(wxCommandEvent& event){
		wxCommandEvent e(BSP_RENDER_CHANGE, GetId());
		e.SetInt((event.IsChecked())?1:0);
		GetEventHandler()->ProcessEvent(e);
	}

};
