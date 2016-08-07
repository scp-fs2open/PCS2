#pragma once

#include <unordered_set>
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
	primitive_radio_button_ctrl<int>*movement_axis;
	multi_string_ctrl*properties;
	vector_ctrl*offset;
	default_value_vector_ctrl*bbox_min;
	default_value_vector_ctrl*bbox_max;
	default_value_float_ctrl*max_rad;

public:

	sobj_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_sobj>(parent, orient, Title)
	{
		std::vector<primitive_list_item<int> > list;

		//add controls
		add_control(name=new string_ctrl(this,_("Name")),0,wxEXPAND );

		list.resize(4);

		list[0].title = "None";
		list[0].data = ANONE;

		list[1].title = "X Axis";
		list[1].data = MV_X;

		list[2].title = "Y Axis";
		list[2].data = MV_Y;

		list[3].title = "Z Axis";
		list[3].data = MV_Z;

		add_control(movement_axis=new primitive_radio_button_ctrl<int>(this, list,_("Rotation Axis")),0,wxEXPAND );

		add_control(offset=new vector_ctrl(this,_("Offset")),0,wxEXPAND );

		add_control(bbox_min=new default_value_vector_ctrl(this,_("Bounding Box Min")),0,wxEXPAND );
		add_control(bbox_max=new default_value_vector_ctrl(this,_("Bounding Box Max")),0,wxEXPAND );
		add_control(max_rad = new default_value_float_ctrl(this,_("Max Radius")), 0, wxEXPAND);

		std::vector<std::string> op;
		op.push_back("$special=subsystem\n");
		op.push_back("$special=no_rotate\n");
		op.push_back("$name=x\n");
		op.push_back("$lod0_name=x\n");
		op.push_back("");
		op.push_back("$fov=180\n");
		op.push_back("");
		op.push_back("$rotate=x\n");
		op.push_back("$dumb_rotate:x\n");
		op.push_back("$pbank=0\n");
		op.push_back("");
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
		op.push_back("$fraction_accel=x\n");
		op.push_back("");
		op.push_back("$gun_rotation:\n");
		op.push_back("");
		op.push_back("$uvec:0,1,0\n");
		op.push_back("$fvec:0,0,1\n");
		op.push_back("");
		op.push_back("$no_collisions\n");
		op.push_back("$nocollide_this_only\n");
		op.push_back("$collide_invisible\n");
		add_control(properties=new suggest_ctrl<std::string, multi_string_ctrl>(this,_("Properties"),op, SUGGEST_APPEND),1,wxEXPAND );
		
	};

	virtual ~sobj_ctrl(void){};

	//set the control's value
	void set_value(const pcs_sobj&t){
		data=t;
		name->set_value(t.name);
		movement_axis->set_value(t.movement_axis);
		if (t.movement_type == MNONE) {
			movement_axis->set_value(ANONE);
		}
		offset->set_value(t.offset);
		bbox_min->set_value_with_default(
				data.bounding_box_min_point_overridden ?
				data.bounding_box_min_point_override :
				data.bounding_box_min_point, data.bounding_box_min_point);
		bbox_max->set_value_with_default(
				data.bounding_box_max_point_overridden ?
				data.bounding_box_max_point_override :
				data.bounding_box_max_point, data.bounding_box_max_point);
		max_rad->set_value_with_default(
				data.radius_overridden ?
				data.radius_override :
				data.radius, data.radius);

		properties->set_value(t.properties);
	}

	//return's the control's value
	pcs_sobj get_value(){
		data.name = name->get_value();
		data.movement_axis = movement_axis->get_value();
		data.movement_type = data.movement_axis == ANONE ? MNONE : ROTATE;
		data.offset = offset->get_value();
		data.bounding_box_min_point_override = bbox_min->get_value();
		data.bounding_box_max_point_override = bbox_max->get_value();
		data.bounding_box_min_point = bbox_min->get_default_value();
		data.bounding_box_max_point = bbox_max->get_default_value();
		data.bounding_box_min_point_overridden = bbox_min->is_overridden();
		data.bounding_box_max_point_overridden = bbox_max->is_overridden();
		data.radius_override = max_rad->get_value();
		data.radius = max_rad->get_default_value();
		data.radius_overridden = max_rad->is_overridden();

		data.properties = properties->get_value();
		return data;
	}
	void set_pos(vector3d p){
		offset->set_value(data.offset = p);
	}

	virtual void transform(const matrix& transform, const vector3d& translation, int sobj_num) {
		pcs_sobj sobj = get_value();
		sobj.Transform(get_main_window()->get_model(), sobj_num, transform, translation, false, false);
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
		b->Add(cpy_btn = new wxBitmapButton(this, SOBJ_BUTTON_CPY, wxBitmap(copy_btn),wxDefaultPosition,wxDefaultSize, wxBORDER_NONE));
		b->Add(del_btn = new wxBitmapButton(this, SOBJ_BUTTON_DEL, wxBitmap(delete_btn),wxDefaultPosition,wxDefaultSize, wxBORDER_NONE));
		add_sizer(b);

		cpy_btn->SetToolTip(_("Copy"));
		del_btn->SetToolTip(_("Delete"));

		add_control(sobj=new sobj_ctrl(this, _("")),0,wxEXPAND );

		add_control(bsp_render_box=new wxCheckBox(this, BSP_DRAW, _("Draw BSP Debug Info")));
		bsp_render_box->Disable();
		add_control(sobj_info = new string_disp(this,_("Subobject Info")),0,wxEXPAND );

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
		data = sobj->get_value();
		return data;
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

			update_info(model);
		}
	}

	void update_info(PCS_Model& model) {

		vector3d& min_box = data.bounding_box_min_point_overridden ? data.bounding_box_min_point_override : data.bounding_box_min_point;
		vector3d& max_box = data.bounding_box_max_point_overridden ? data.bounding_box_max_point_override : data.bounding_box_max_point;
		vector3d size = max_box - min_box;
		wxString info(wxString::Format(_("Poly Count:       %li\nChild Poly Count: %li\nTotal:            %li\n"),
			data.polygons.size(),
			model.get_child_subobj_poly_count(sobj_num),
			data.polygons.size() + model.get_child_subobj_poly_count(sobj_num)));
		info += wxString::Format(_("Vertices: %li\n"), count_vertices());
		info += wxString::Format(_("Normals: %li\n"), count_normals());
		info += wxString::Format(_("H: %0.2f, W: %0.2f, D: %0.2f\nParent submodel:  "),
			fabs(size.y), fabs(size.x), fabs(size.z));
		if (data.parent_sobj > -1) {
			info += wxString(model.SOBJ(data.parent_sobj).name.c_str(), wxConvUTF8);
		} else {
			info += _("*NONE*");
		}
		sobj_info->set_value(std::string(info.mb_str()));
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		if(sobj_num >=0 && sobj_num<model.GetSOBJCount()) {
			model.SOBJ(sobj_num)=get_value();
			update_info(model);
		}
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

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		sobj->transform(transform, translation, sobj_num);
		set_data(get_main_window()->get_model());
	}

private:
	size_t count_vertices() {
		std::unordered_set<vector3d> points;
		for (size_t i = 0; i < data.polygons.size(); i++) {
			for (size_t j = 0; j < data.polygons[i].verts.size(); j++) {
				points.insert(data.polygons[i].verts[j].point);
			}
		}
		return points.size();
	}

	size_t count_normals() {
		std::unordered_set<vector3d> vertices;
		for (size_t i = 0; i < data.polygons.size(); i++) {
			for (size_t j = 0; j < data.polygons[i].verts.size(); j++) {
				vertices.insert(data.polygons[i].verts[j].norm);
			}
		}

		return vertices.size();
	}

};
