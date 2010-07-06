#ifndef _INSG_EDITOR_H_
#define _INSG_EDITOR_H_

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

class insignia_face_ctrl :
	public editor<pcs_insig_face>
{
protected:
	pcs_insig_face face;

public:
	
	insignia_face_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig_face>(parent,x,y,w,h, orient, Title)
	{
	};

	virtual ~insignia_face_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig_face&t){
		face = t;
	}

	//return's the control's value
	pcs_insig_face get_value(){
		return face;
	}

};

class insignia_face_array_ctrl
	:public type_array_ctrl<pcs_insig_face, insignia_face_ctrl>
{
public:
	insignia_face_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle)
		:type_array_ctrl<pcs_insig_face, insignia_face_ctrl>(parent,x,y,w,h,Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}
};

class insignia_ctrl :
	public editor<pcs_insig>
{
protected:
	insignia_face_array_ctrl* faces;
	vector_ctrl*offset;
	int_ctrl*lod;

public:
	
	insignia_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig>(parent,x,y,w,h, orient, Title)
	{
		add_control(lod =new int_ctrl(this,0,0,60,40,_("LOD")),0,wxEXPAND );
		add_control(offset =new vector_ctrl(this,0,0,60,40,_("Offset")),0,wxEXPAND );
		add_control(faces=new insignia_face_array_ctrl(this,0,0,60,410,_("Faces"), _("")),0,wxEXPAND );
	};

	virtual ~insignia_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig&t){
		faces->set_value(t.faces);
		offset->set_value(t.offset);
		lod->set_value(t.lod);
	}

	//return's the control's value
	pcs_insig get_value(){
		pcs_insig insignia;
		insignia.faces = faces->get_value();
		insignia.offset = offset->get_value();
		insignia.lod = lod->get_value();
		return insignia;
	}

	void set_indexes(std::vector<int> i){
		faces->set_indexes(i);
	};

	int get_index() {
		return faces->get_index();
	}
};

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
		add_control(insignia=new insignia_array_ctrl(this,0,0,60,410,_("Insignum"), _("")),0,wxEXPAND );
	}

	//do nothing, needed so the base destructor will get called
	virtual~INSG_ctrl(){}

	//set's the control's value
	void set_value(const std::vector<pcs_insig>&t){
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
		ret.resize(2);
		ret[0] = insignia->get_index();
		ret[1] = insignia->get_child_control()->get_index();
		return ret;
	};

	omnipoints get_omnipoints(){
		omnipoints o;
		//std::vector<pcs_insig> insignia = get_value();
		//for(unsigned int i = 0; i<insignia.size(); i++){
		pcs_insig insig = insignia->get_curent_value();
		o.point.reserve(insig.faces.size());
		for (unsigned int j = 0; j < insig.faces.size(); j++) {
			pcs_insig_face face;
			std::vector<omnipoint> points;
			points.resize(3);
			for (unsigned int k = 0; k < 3; k++) {
				points[k].pos = insig.faces[j].verts[k] + insig.offset;
			}
			o.point.push_back(points);
		}
		//}
		o.flags = OMNIPOINT_CLOSED_PATH;
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points_){
		// TODO
	}

	void get_omnipoint_coords(int&list, int&item){
		/*list = 0;
		for (int i = 0; i < insignia->get_index(); i++) {
			list += data[i].faces.size();
		}*/
		list = insignia->get_child_control()->get_index();
		item = -1;//points->get_index();
	}
	void set_omnipoint_coords(int&list, int&item){
		// TODO
	}
};
#endif
