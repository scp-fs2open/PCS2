
#pragma once
#include <wx/wx.h>
#include "pcs_file.h"
#include "primitive_ctrl.h"
#include "omnipoints.h"
#include "op_stack.h"


/*
based roughly on kaz's pe_base, a base class that edits some aspect of a pof
this class does not have the controls on it directly, rather it serves
as the scrolling window parent of a panel that has them, the base for that panel is
defined below
this control can also have common editor elements added to it
*/

//this cannot be replaced with a template cause we need access 
//to some of the functions and we need a base class with those 
//functions, this is that base class
class model_editor_ctrl_base : public wxScrolledWindow
{
	protected:
		//this is so you can get access to the control panel from outside
			wxPanel *base_panel;
			bool Ready;

	public:
		model_editor_ctrl_base(wxWindow*parent, CHUNK Type)
			:wxScrolledWindow(parent), Ready(false), chunk_type(Type)
		{
		}

		//populates the control with data from the model
		virtual void set_data(PCS_Model &model)=0;

		//applies the data in the control to the model
		virtual void apply_data(PCS_Model &model)=0;

		virtual void set_item(const std::vector<int>&coord)=0;
		virtual std::vector<int> get_item()=0;

		virtual omnipoints get_omnipoints()=0;
		virtual void set_omnipoints(const omnipoints&points)=0;
		virtual void get_omnipoint_coords(int&list, int&item)=0;
		virtual void set_omnipoint_coords(int&list, int&item)=0;

		const CHUNK chunk_type;

		bool ready(){return Ready;}

		virtual void push_undo()=0;
		virtual void undo()=0;
		virtual void redo()=0;
		virtual void fix_buttons()=0;
		virtual void reset_undo()=0;

		virtual wxSizer* get_transform_options(wxWindow* parent)=0;
		virtual void transform(const matrix& transform, const vector3d& translation)=0;
};

//base for the panel with the item specific editors on them

//there are four virtual functions all dealing 
//with setting/getting data


//functions inherited from editor<type>

//void set_value(const type&t)
//		-	this function is responcable for takeing a structure
//			and displaying the contents on screen
//if you did everything right this will be a bunch of
//name_of_control->set_value(t);
//...

//type get_value()
//		-	this function is responcable for returning a structure
//			from the controls
//if you did everything right this will be a bunch of
//type type_instance;
//type_instance.member = name_of_control->get_value();
//...
//return type_insance;

//editor_ctrl specific functions

//void set_data(PCS_Model &model)	
//		-	this function is responsable for getting the data 
//			structure from the passed model
//if you did everything right this will be little more than
//set_value(model.get_the_data_you_want());

//void apply_data(PCS_Model &model)
//		-	this function is responsable for setting the data 
//			from the control into the model
//if you did everything right this will be little more than
//model.set_the_data_you_want(conrol_panel->get_value());


template<class type>
class editor_ctrl
	:public editor<type>
{

protected:
	type data;
	op_stack<type> undo_stack;

public:
	void push_undo(){
		data = this->get_value();
		undo_stack.push(data);
	}
	void undo(){
		undo_stack.undo(data);
		set_value(data);//update the UI
	}
	void redo(){
		undo_stack.redo(data);
		set_value(data);//update the UI
	}
	void fix_buttons(){
		undo_stack.fix_buttons();
	}
	void reset_undo(){
		undo_stack.reset();
	}
	void recalc_size(){
	}

	editor_ctrl(wxWindow*parent, const wxString&Title, int orient = wxVERTICAL)
		:editor<type>(parent,orient, Title)
	{
	}
	virtual ~editor_ctrl(){}

	//populates the control with data from the model
	virtual void set_data(PCS_Model &model)=0;

	//applies the data in the control to the model
	virtual void apply_data(PCS_Model &model)=0;

	virtual void set_item(const std::vector<int>&coord)=0;
	virtual std::vector<int> get_item()=0;

	virtual omnipoints get_omnipoints()=0;
	virtual void set_omnipoints(const omnipoints&points)=0;
	virtual void get_omnipoint_coords(int&list, int&item)=0;
	virtual void set_omnipoint_coords(int&list, int&item)=0;
	virtual wxSizer* get_transform_options(wxWindow* parent)=0;
	virtual void transform(const matrix& transform, const vector3d& translation)=0;

};


//this makes makeing the actual editors easier as is hides a 
//bunch of common init stuff that is type specific
//the fact that the editor control is a template means we can't
//just use that in the base unless the base was a template
//but then we couldn't use the base as a pointer, and we'd lose functionality

//type-				the data type this editor deals with

//editor_type-		the editor you want to use, 
//					must be derived from editor_ctrl<type>

template<class type, class editor_type>
class model_editor_ctrl
	:public model_editor_ctrl_base
{
protected:
	editor_ctrl<type>*conrol_panel;

public:
	void push_undo(){
		conrol_panel->push_undo();
	}
	void undo(){
		conrol_panel->undo();
	}
	void redo(){
		conrol_panel->redo();
	}
	void fix_buttons(){
		conrol_panel->fix_buttons();
	}
	void reset_undo(){
		conrol_panel->reset_undo();
	}

	model_editor_ctrl(wxWindow*parent, PCS_Model &model, CHUNK Type)
		:model_editor_ctrl_base(parent, Type),conrol_panel(NULL)
	{
		base_panel = conrol_panel = new editor_type(this);
		if(conrol_panel){
			conrol_panel->recalc_size();
			conrol_panel->set_data(model);

			wxBoxSizer*sizer = new wxBoxSizer(wxHORIZONTAL);
			SetSizer(sizer);
			sizer->Add(conrol_panel,1.0f);
			sizer->Layout();
		}
		Ready = true;
	}

	//Special for sobj or anything that might need an additional int
	model_editor_ctrl(wxWindow*parent, PCS_Model &model, CHUNK Type, int item)
		:model_editor_ctrl_base(parent, Type),conrol_panel(NULL)
	{
		base_panel = conrol_panel = new editor_type(this, item);
		conrol_panel->recalc_size();
		conrol_panel->set_data(model);

		if(conrol_panel){
			wxBoxSizer*sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(conrol_panel,1.0f);
			SetSizer(sizer);
			sizer->Layout();
		}
		Ready = true;
//		conrol_panel->SetSize(100,500);
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		conrol_panel->set_data(model);
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		conrol_panel->apply_data(model);
	}

	void set_item(const std::vector<int>&coord){
		conrol_panel->set_item(coord);
	}
	virtual std::vector<int> get_item(){
		return conrol_panel->get_item();
	}

	omnipoints get_omnipoints(){
		return conrol_panel->get_omnipoints();
	}

	void set_omnipoints(const omnipoints&points){
		conrol_panel->set_omnipoints(points);
	}
	virtual void get_omnipoint_coords(int&list, int&item){
		conrol_panel->get_omnipoint_coords(list, item);
	}

	virtual void set_omnipoint_coords(int&list, int&item){
		conrol_panel->set_omnipoint_coords(list, item);
	}

	virtual wxSizer* get_transform_options(wxWindow* parent) {
		return conrol_panel->get_transform_options(parent);
	}

	virtual void transform(const matrix& transform, const vector3d& translation) {
		conrol_panel->transform(transform, translation);
		wxCommandEvent event(EDIT_DONE);
		GetEventHandler()->ProcessEvent(event);
		reset_undo();
	}

};


