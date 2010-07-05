#ifndef _INSG_EDITOR_H_
#define _INSG_EDITOR_H_

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

//editor for a dock point
class insignia_ctrl :
	public editor<pcs_insig>
{
protected:
	pcs_insig insignia;

public:
	
	insignia_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig>(parent,x,y,w,h, orient, Title)
	{
	};

	virtual ~insignia_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig&t){
		insignia = t;
	}

	//return's the control's value
	pcs_insig get_value(){
		return insignia;
	}
};

//array of dock points
class insignia_array_ctrl
	:public type_array_ctrl<pcs_insig, insignia_ctrl>
{
public:
	insignia_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle)
		:type_array_ctrl<pcs_insig, insignia_ctrl>(parent,x,y,w,h,Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}
};


class INSG_ctrl
	:public editor_ctrl<std::vector<pcs_insig> >
{
	insignia_array_ctrl*insignia;

public:
	static color selected_item;
	static color selected_list;
	static color unselected;

	INSG_ctrl(wxWindow*parent)
		:editor_ctrl<std::vector<pcs_insig> >(parent, _("Insignia"))
	{
		//add controls
		add_control(insignia=new insignia_array_ctrl(this,0,0,60,410,_("Point"), _("")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~INSG_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_insig>&t){
		data=t;
		insignia->set_value(t);
	}

	//return's the control's value
	std::vector<pcs_insig> get_value(){
		return insignia->get_value();
	}

	//populates the control with data from the model
	void set_data(PCS_Model &model){
		set_value(model.get_insignia());
	}

	//applies the data in the control to the model
	void apply_data(PCS_Model &model){
		model.set_insignia(get_value());
	}

	void set_item(const std::vector<int>&coord){
		insignia->set_indexes(coord);
	}
	std::vector<int> get_item(){
		std::vector<int> ret;
		ret.resize(1);
		ret[0] = insignia->get_index();
		printf("%d\n", ret[0]);
		return ret;
	};

	omnipoints get_omnipoints(){
		return omnipoints();
	}

	void set_omnipoints(const omnipoints&points){}
	void get_omnipoint_coords(int&list, int&item){
		list = -1;
		item = -1;
	}
	void set_omnipoint_coords(int&list, int&item){}
};
#endif
