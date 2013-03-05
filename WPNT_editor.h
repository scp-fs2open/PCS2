#pragma once

#include"hardpoint_editor.h"

/*
________________________________
|                              |
|   [] Converge In X           |
|   [] Converge In Y           |
|                              |
|    Point to Converge to      |
|    [___________________]     |
|                              |
|       [OK]  [Cancel]         |
|______________________________|
*/

class convergence_calculator :
	public wxDialog
{
	wxCheckBox*check_box[4];
	vector_ctrl*pnt_ctrl;
	wxButton*ok_btn;
	wxButton*cancel_btn;

	int mask;
	vector3d point;

public:
	convergence_calculator()
		:wxDialog(NULL, -1, wxString(_("Convergence Calculator")))
		,mask(0)
	{
		wxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
		main_sizer->SetMinSize(wxSize(200, 150));
		wxSizer* axes_sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Axes to converge"));
		axes_sizer->Add(check_box[0] = new wxCheckBox(this, CHECK_X, _("X")));
		axes_sizer->AddStretchSpacer(1);
		axes_sizer->Add(check_box[1] = new wxCheckBox(this, CHECK_Y, _("Y")));
		axes_sizer->AddStretchSpacer(1);
		axes_sizer->Add(check_box[2] = new wxCheckBox(this, CHECK_Z, _("Z")));
		axes_sizer->AddStretchSpacer(1);
		main_sizer->Add(axes_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
		main_sizer->Add(check_box[3] = new wxCheckBox(this, CHECK_THIS_ONLY, _("This Point Only")), 0, wxALL, 15);
		
		for(int i = 0; i<4; i++){
			check_box[i]->SetValue(i!=3);
		}
		main_sizer->AddStretchSpacer(1);
		main_sizer->Add(pnt_ctrl = new vector_ctrl(this,_("Point to Converge to")), 0, wxEXPAND | wxALL, 10);
		wxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
		button_sizer->Add(ok_btn = new wxButton(this, CONVERGE_OK, _("OK")));
		button_sizer->AddSpacer(20);
		button_sizer->Add(cancel_btn = new wxButton(this, CONVERGE_CANCEL, _("Cancel")));
		main_sizer->Add(button_sizer, 0, wxBOTTOM | wxTOP | wxALIGN_CENTER_HORIZONTAL, 10);
		SetSizerAndFit(main_sizer);
	};
public:
	~convergence_calculator(void){};

	DECLARE_EVENT_TABLE();

	void on_ok(wxCommandEvent& event){
		for(int i = 0; i<3; i++){
			mask |= ((check_box[i]->GetValue()?1:0)<<i);
		}
		point = pnt_ctrl->get_value();

		EndModal(1);
	}

public:
	void on_cancel(wxCommandEvent& event){
		EndModal(0);
	}

	vector3d get_point(){return point;}
	int get_mask(){return mask;}

};


class weapon_hard_point_array_ctrl :
	public hard_point_array_ctrl
{
	wxButton*converge_btn;
public:
	weapon_hard_point_array_ctrl(wxWindow*parent, wxString Title, int orient = wxHORIZONTAL)
		:hard_point_array_ctrl(parent, Title, orient)
	{
		add_control(converge_btn = new wxButton(this, WPN_CONVERGENCE, _("Convergence")));
	}

	DECLARE_EVENT_TABLE();

	void converge_to(int idx, vector3d pnt, int mask = 0xffffffff){
		if(mask & (1<<0))array[idx].norm.x = pnt.x;
		else array[idx].norm.x = array[idx].point.x;
		if(mask & (1<<1))array[idx].norm.y = pnt.y;
		else array[idx].norm.y = array[idx].point.y;
		if(mask & (1<<2))array[idx].norm.z = pnt.z;
		else array[idx].norm.z = array[idx].point.z;
 
		array[idx].norm = MakeUnitVector(array[idx].norm - array[idx].point);
	}

	void on_convergence(wxCommandEvent& event){
		convergence_calculator calc;
		if(!calc.ShowModal())return;

		int mask = calc.get_mask();
		vector3d pnt = calc.get_point();

		if(mask & (1<<3)){
			converge_to(get_index(),pnt,mask);
		}else{
			for(unsigned int i = 0; i<array.size(); i++){
				converge_to(i,pnt,mask);
			}
		}
		set_value(array);//refresh
		wxCommandEvent commandEvent(EDIT_DONE, GetId());
		GetEventHandler()->ProcessEvent(commandEvent);
	}

};

//control for a type of weapon points
class weapon_point_type_ctrl :
	public type_array_ctrl<std::vector<pcs_hardpoint>, weapon_hard_point_array_ctrl>
{
public:
	weapon_point_type_ctrl(wxWindow*parent, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl<std::vector<pcs_hardpoint>, weapon_hard_point_array_ctrl>(parent, subtitle, _("Point"), wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}

	virtual~weapon_point_type_ctrl(){}

};

/*
//control for both types of weapon points
class weapon_point_both_type_ctrl :
	public type_static_array_ctrl<std::vector<std::vector<pcs_slot> >,weapon_point_type_ctrl>
{
public:
	weapon_point_both_type_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subtitle, int orient = wxHORIZONTAL)
		:type_array_ctrl(parent,x,y,w,h,subtitle, "Point", wxVERTICAL, wxEXPAND)
	{
	}

	virtual~weapon_point_both_type_ctrl(){}

	void set_large_value(const std::vector<pcs_slot>&t)
	{
		std::vector<std::vector<pcs_slot> > temp;
		temp.resize(2);
		for(int i = 0; i<t.size();i++){
			temp[t[i].type] = t[i];
		}
		set_value(temp);
	}
	std::vector<pcs_slot>get_large_value()
	{
		std::vector<std::vector<pcs_slot> > temp = get_value();
		std::vector<pcs_slot> ret = temp[0];
		for(int i = 0; i<temp[1].size();i++){
			ret.push_back(temp[1][i]);
		}
		return ret;
	}
};
*/

//the chunk's editor
template<int slot_type>
class WPNT_ctrl
	:public editor_ctrl<std::vector<pcs_slot> >
{
	weapon_point_type_ctrl*wpn;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	WPNT_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_slot> >(parent, _("Weapon Points"))
	{
		//add controls
		add_control(wpn=new weapon_point_type_ctrl(this, _("Gunpoint"),_("Bank")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~WPNT_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_slot>&t){
		data=t;
		std::vector<std::vector<pcs_hardpoint> > temp;
		for(unsigned int i = 0; i<data.size(); i++){
			if(data[i].type == slot_type)
			temp.push_back(data[i].muzzles);
		}
		wpn->set_value(temp);
	}

	//return's the control's value
	std::vector<pcs_slot> get_value(){
		//because we just HAD to make an array of something a seperate class
		//remove all the exsisting points of our type
		for(unsigned int i = 0; i<data.size();){
			if(data[i].type == slot_type){
				data.erase(data.begin()+i);
			}else{
				i++;
			}
		}
		std::vector<std::vector<pcs_hardpoint> > temp = wpn->get_value();
		data.resize(temp.size());
		for(unsigned int i = 0; i<temp.size(); i++){
			data[i].muzzles = temp[i];
			data[i].type = slot_type;
		}
		return data;
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_weapons());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		std::vector<pcs_slot> ret = get_value();
		for(int i = 0; i<model.GetWeaponCount(); i++){
			if(model.get_weapons()[i].type != slot_type){
				ret.push_back(model.get_weapons()[i]);
			}
		}
		model.set_weapons(ret);
	}

	void set_item(const std::vector<int>&coord){
		wpn->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(2);
		ret[0] = wpn->get_index();
		ret[1] = wpn->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		std::vector<pcs_slot> pnt = get_value();
		o.point.resize(pnt.size());
		for( unsigned int i = 0; i<pnt.size(); i++){
			o.point[i].resize(pnt[i].muzzles.size());
			for(unsigned int j = 0; j<pnt[i].muzzles.size(); j++){
				o.point[i][j].pos = pnt[i].muzzles[j].point;
				o.point[i][j].norm = pnt[i].muzzles[j].norm;
			}
		}
		o.flags = OMNIPOINT_NORM;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points){
		std::vector<pcs_slot> temp = get_value();
		if(temp.size()!=points.point.size())return;

		for(unsigned int i = 0; i<points.point.size(); i++){
			if(temp[i].muzzles.size() != points.point[i].size())
				return;//data was corupted, fortunately it was only a copy we were working with
			for(unsigned int j = 0; j<points.point[i].size(); j++){
				temp[i].muzzles[j].point = points.point[i][j].pos;
				temp[i].muzzles[j].norm = points.point[i][j].norm;
			}
		}
		set_value(temp);
	}
	void get_omnipoint_coords(int&list, int&item){
		list = wpn->get_index(); 
		item = wpn->get_child_control()->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		wpn->set_index(list); 
		wpn->get_child_control()->set_index(item);
	}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		std::vector<int> items = get_item();
		if (items[0] != -1) {
			std::vector<pcs_slot> guns = get_value();
			guns[items[0]].Transform(transform, translation);
			set_value(guns);
		}
	}
};

/*
//both chunk's editor
class WPNT_ctrl2
	:public editor_ctrl<std::vector<pcs_slot> >
{
	weapon_point_both_type_ctrl*wpn;

public:
	WPNT_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_slot> >(parent, "Weapon Points")
	{
		//add controls
		add_control(wpn=new weapon_point_both_type_ctrl(this,0,0,90,260,"Type",""),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~WPNT_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_slot>&t){
		data=t;
		std::vector<std::vector<pcs_hardpoint> > temp;
		for(int i = 0; i<data.size(); i++){
			if(data[i].type == slot_type)
			temp.push_back(data[i].muzzles);
		}
		wpn->set_value(temp);
	}

	//return's the control's value
	std::vector<pcs_slot> get_value(){
		//because we just HAD to make an array of something a seperate class
		//remove all the exsisting points of our type
		for(int i = 0; i<data.size();){
			if(data[i].type = slot_type){
				data.remove(i);
			}else{
				i++;
			}
		}
		std::vector<std::vector<pcs_hardpoint> > temp = wpn->get_value();
		data.resize(temp.size());
		for(int i = 0; i<temp.size(); i++){
			data[i].muzzles = temp[i];
			data[i].type = slot_type;
		}
		return data;
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_weapons());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_weapons(get_value());
	}
};*/
