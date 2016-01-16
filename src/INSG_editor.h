#ifndef _INSG_EDITOR_H_
#define _INSG_EDITOR_H_

#include"model_editor_ctrl.h"
#include"array_ctrl.h"
#include"primitive_ctrl.h"
#include"pcs_file.h"

#define NUM_VERTS 3

struct pcs_insig_face_vert {
	vector3d vert;
	float u, v;
	pcs_insig_face_vert() : u(0.0f), v(0.0f) {}
	pcs_insig_face_vert(pcs_insig_face face, int idx) {
		if (idx > -1 && idx < NUM_VERTS) {
			vert = face.verts[idx];
			u = face.u[idx];
			v = face.v[idx];
		}
	}

	void store_to_face (pcs_insig_face& face, int idx) {
		if (idx > -1 && idx < NUM_VERTS) {
			face.verts[idx] = vert;
			face.u[idx] = u;
			face.v[idx] = v;
		}
	}
};

class insignia_vert_ctrl :
	public editor<pcs_insig_face_vert>
{
protected:
	vector_ctrl* vert;
	float_ctrl* u;
	float_ctrl* v;

public:
	
	insignia_vert_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig_face_vert>(parent, orient, Title)
	{
		add_control(vert =new vector_ctrl(this,_("Position")),0,wxEXPAND );
		add_control(u =new float_ctrl(this,_("u")),0,wxEXPAND );
		add_control(v =new float_ctrl(this,_("v")),0,wxEXPAND );
	};

	virtual ~insignia_vert_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig_face_vert&t){
		vert->set_value(t.vert);
		u->set_value(t.u);
		v->set_value(t.v);
	}

	//return's the control's value
	pcs_insig_face_vert get_value(){
		pcs_insig_face_vert vertex;
		vertex.vert = vert->get_value();
		vertex.u = u->get_value();
		vertex.v = v->get_value();
		return vertex;
	}

};

class insignia_vert_array_ctrl :
	public type_array_ctrl<pcs_insig_face_vert, insignia_vert_ctrl>
{
	public:
		insignia_vert_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle)
			:type_array_ctrl<pcs_insig_face_vert, insignia_vert_ctrl>(parent, Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST, false)
		{
		}
};

class insignia_face_ctrl :
	public editor<pcs_insig_face>
{
protected:
	insignia_vert_array_ctrl* verts;

public:
	
	insignia_face_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig_face>(parent, orient, Title)
	{
		add_control(verts =new insignia_vert_array_ctrl(this,_("Vertexes"), _("")),0,wxEXPAND );
	};

	virtual ~insignia_face_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig_face&t){
		std::vector<pcs_insig_face_vert> vertexes;
		vertexes.reserve(3);
		for (int i = 0; i < NUM_VERTS; i++) {
			vertexes.push_back(pcs_insig_face_vert(t, i));
		}
		verts->set_value(vertexes);
	}

	//return's the control's value
	pcs_insig_face get_value(){
		pcs_insig_face face;
		const std::vector<pcs_insig_face_vert>& vertexes = verts->get_value();
		for (int i = 0; i < NUM_VERTS; i++) {
			face.verts[i] = vertexes[i].vert;
			face.u[i] = vertexes[i].u;
			face.v[i] = vertexes[i].v;
		}
		return face;
	}

	int get_index() {
		return verts->get_index();
	}

};

class insignia_face_array_ctrl
	:public type_array_ctrl<pcs_insig_face, insignia_face_ctrl>
{
public:
	insignia_face_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle)
		:type_array_ctrl<pcs_insig_face, insignia_face_ctrl>(parent, Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST)
	{
	}
};

class insignia_generator_ctrl :
	public editor<pcs_insig_generator>
{
protected:
	vector_ctrl*pos;
	vector_ctrl*forward;
	vector_ctrl*up;
	float_ctrl*radius;
	float_ctrl*distance;
	float_ctrl*merge_eps;
	int_ctrl*subdivision;

public:
	
	insignia_generator_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig_generator>(parent,orient, Title)
	{
		add_control(pos =new vector_ctrl(this,_("Position")),0,wxEXPAND );
		add_control(forward =new vector_ctrl(this,_("Projection vector")),0,wxEXPAND );
		add_control(up =new vector_ctrl(this,_("Up vector")),0,wxEXPAND );
		add_control(radius =new float_ctrl(this,_("Length")),0,wxEXPAND );
		add_control(distance =new float_ctrl(this,_("Back-off distance")),0,wxEXPAND );
		add_control(subdivision =new int_ctrl(this,_("Subdivision")),0,wxEXPAND );
		add_control(merge_eps =new float_ctrl(this,_("Polygon merge threshold")),0,wxEXPAND );
	};

	virtual ~insignia_generator_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig_generator&t){
		pos->set_value(t.pos);
		forward->set_value(t.forward);
		up->set_value(t.up);
		radius->set_value(t.radius);
		distance->set_value(t.distance);
		subdivision->set_value(t.subdivision);
		merge_eps->set_value(t.merge_eps);
		
	}

	//return's the control's value
	pcs_insig_generator get_value(){
		pcs_insig_generator generator;
		generator.pos = pos->get_value();
		generator.forward = forward->get_value();
		generator.up = up->get_value();
		generator.radius = radius->get_value();
		generator.distance = distance->get_value();
		generator.subdivision = subdivision->get_value();
		generator.merge_eps = merge_eps->get_value();
		return generator;
	}
};


class insignia_ctrl :
	public editor<pcs_insig>
{
protected:
	insignia_face_array_ctrl* faces;
	vector_ctrl*offset;
	int_ctrl*lod;

	insignia_generator_ctrl* generator;
	wxButton*project_btn;

public:
	
	insignia_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL)
	:editor<pcs_insig>(parent, orient, Title)
	{
		add_control(lod =new int_ctrl(this,_("LOD")),0,wxEXPAND );
		add_control(offset =new vector_ctrl(this,_("Offset")),0,wxEXPAND );
		add_control(faces=new insignia_face_array_ctrl(this, _("Faces"), _("")),0,wxEXPAND );
		add_control(generator =new insignia_generator_ctrl(this,_("Projection")),0,wxEXPAND );
		add_control(project_btn = new wxButton(this, INSG_PROJECT, _("Project")));
	};

	virtual ~insignia_ctrl(void){};

	//set the control's value
	void set_value(const pcs_insig&t){
		faces->set_value(t.faces);
		offset->set_value(t.offset);
		lod->set_value(t.lod);
		generator->set_value(t.generator);
		
	}

	//return's the control's value
	pcs_insig get_value(){
		pcs_insig insignia;
		insignia.faces = faces->get_value();
		insignia.offset = offset->get_value();
		insignia.lod = lod->get_value();
		insignia.generator = generator->get_value();
		if (insignia.faces.size() == 0) {
			generator->Enable();
			project_btn->Enable();
		} else {
			generator->Disable();
			project_btn->Disable();
		}
		return insignia;
	}

	void set_indexes(std::vector<int> i){
		faces->set_indexes(i);
	};

	int get_index() {
		return faces->get_index();
	}

	std::vector<int> get_indexes() {
		std::vector<int> result;
		result.resize(2);
		result[0] = faces->get_index();
		result[1] = faces->get_child_control()->get_index();
		return result;
	}

	void transform(const matrix& transform, const vector3d& translation) {
		pcs_insig insignia = get_value();
		insignia.Transform(transform, translation);
		set_value(insignia);
	}

	DECLARE_EVENT_TABLE();
	void on_project(wxCommandEvent& event){
		PCS_Model& model = get_main_window()->get_model();
		pcs_insig insignia;
		insignia.lod = lod->get_value();
		insignia.generator = generator->get_value();
		int sobj = model.LOD(insignia.lod);
		insignia.generator.pos -= model.SOBJ(sobj).offset;
		if (insignia.Generate(model.SOBJ(sobj).polygons, insignia.generator.merge_eps)) {
			faces->set_value(insignia.faces);
			offset->set_value(insignia.offset + model.SOBJ(sobj).offset);
			lod->set_value(insignia.lod);
			generator->Disable();
			project_btn->Disable();
			wxCommandEvent event(EDIT_DONE);
			GetEventHandler()->ProcessEvent(event);
		}
		insignia.generator.pos += model.SOBJ(sobj).offset;
	}
};

class insignia_array_ctrl
	:public type_array_ctrl<pcs_insig, insignia_ctrl>
{
public:
	insignia_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle)
		:type_array_ctrl<pcs_insig, insignia_ctrl>(parent, Title, subTitle, wxVERTICAL, wxEXPAND, ARRAY_LIST)
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
		add_control(insignia=new insignia_array_ctrl(this,_(""), _("")),0,wxEXPAND );
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
		if (insignia->get_index() != -1) {
			pcs_insig insig = insignia->get_curent_value();
			o.lod = insig.lod;
			if (insig.faces.size() > 0) {
				o.point.reserve(insig.faces.size());
				for (unsigned int j = 0; j < insig.faces.size(); j++) {
					pcs_insig_face face;
					std::vector<omnipoint> points;
					points.resize(NUM_VERTS);
					for (unsigned int k = 0; k < NUM_VERTS; k++) {
						points[k].pos = insig.faces[j].verts[k] + insig.offset;
						points[k].model = -1;
					}
					o.point.push_back(points);
				}
				o.flags = OMNIPOINT_CLOSED_PATH;
			}else {
				o.point.reserve(1);
				if (insig.faces.size() == 0) {
					pcs_insig_generator& generator = insig.generator;
					if (generator.up != vector3d() && generator.forward != vector3d() && generator.radius > 0.0f) {
						vector3d forward = MakeUnitVector(generator.forward);
						vector3d up = MakeUnitVector(
								generator.up - (dot(generator.up, forward) * forward));
						vector3d right = CrossProduct(forward, up);
						float radius = generator.radius / 2;
						std::vector<omnipoint> points;
						points.resize(1);
						points[0].pos = generator.pos;
						points[0].norm = forward * 10;
						o.point.push_back(points);
						points.resize(4);
						for (int i = 0; i < 4; i++) {
							points[i].norm = forward * 10;
						}
						points[0].pos = generator.pos + (up * radius) - (right * radius);
						points[1].pos = generator.pos + (up * radius) + (right * radius);
						points[2].pos = generator.pos - (up * radius) + (right * radius);
						points[3].pos = generator.pos - (up * radius) - (right * radius);
						o.point.push_back(points);
					}
				}
				o.flags = OMNIPOINT_CLOSED_PATH | OMNIPOINT_NORM;
			}
		}
		o.selected_item = selected_item;
		o.selected_list = selected_list;
		o.unselected = unselected;
		return o;
	}

	void set_omnipoints(const omnipoints&points_){
		if (insignia->get_index() != -1) {
			pcs_insig insig = insignia->get_curent_value();
			if (insig.faces.size() > 0) {
				// TODO
			} else {
				insig.generator.pos = points_.point[0][0].pos;
				insignia->set_curent_value(insig);
			}
		}
	}

	void get_omnipoint_coords(int&list, int&item){
		/*list = 0;
		for (int i = 0; i < insignia->get_index(); i++) {
			list += data[i].faces.size();
		}*/
		if (insignia->get_index() != -1) {
			pcs_insig insig = insignia->get_curent_value();
			if (insig.faces.size() > 0) {
				std::vector<int> indexes = insignia->get_child_control()->get_indexes();
				list = indexes[0];
				item = indexes[1];
			} else {
				list = 0;
				item = 0;
			}
		}
	}
	void set_omnipoint_coords(int&list, int&item){
		// TODO
	}

	wxSizer* get_transform_options(wxWindow* parent) {
		return NULL;
	}

	void transform(const matrix& transform, const vector3d& translation) {
		insignia->get_child_control()->transform(transform, translation);
	}
};
#endif
