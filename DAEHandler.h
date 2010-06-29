#if LOGGING_DAE
#include <iostream>
#endif
#include <dae.h>
#include <dom/domCOLLADA.h>

#include <map>
#include "pcs_file.h"
#include "vector3d.h"
#include "DAEDataStructs.h"


#ifndef _DAEHandler_H_
#define _DAEHandler_H_

#define SPACE_REPLACEMENT '%'

#define LOGGING_DAE 0

class DAEHandler
{
	private:
		DAE dae;
		daeElement *root;
		std::string filename;
#if LOGGING_DAE
		std::ostream *log;
#endif
		float scaling_factor;
		std::string doc;
		int num_textures;
		//std::map<char*,int> textures;
		std::vector<std::string> textures;
		PCS_Model *model;
		std::vector<pcs_sobj*> subobjs;
		std::vector<pcs_special*> specials;
		std::vector<pcs_dock_point*> docks;
		AsyncProgress *progress;

		int up_axis;
		vector3d front;
		vector3d up;
		bool mirror_x;
		bool mirror_y;
		bool mirror_z;
		std::map<std::string, int> texture_map;

		// processes a position vector, updating:
		// - bounding box
		// - radius
		void process_vector3d(vector3d vec, pcs_sobj *subobj = NULL);


		int find_or_add_texture(std::string name);
		int find_texture_id(std::string name, std::map<std::string, std::string> *texture_mapping);
		void subsystem_handler(daeElement *helper);
		void shield_handler(daeElement *helper);
		void eyepoint_handler(daeElement *helper);
		void process_poly_group(daeElement *element, pcs_sobj *subobj, matrix rotation, std::map<std::string, std::string> *texture_mapping);
		void process_sobj_helpers(daeElement *element,int current_sobj_id, int arm_id, matrix rotation);
		void process_special_helpers(daeElement *element, int idx, matrix rotation);
		void process_properties(daeElement *element,std::string *properties);
		void process_thrusters(daeElement *element,std::string name,matrix rotation, vector3d offset);
		void process_firepoints(daeElement *element,int parent, int arm,matrix rotation);
		void process_glowpoints(daeElement *element,int parent,matrix rotation, vector3d offset);
		void process_glowpoints_properties(pcs_glow_array &glowbank);
		void process_path(daeElement *element,std::string parent,matrix rotation, vector3d offset, int dock = -1);
		void process_insignia(daeElement *element);
		void process_dockpoint(daeElement *element);
		pcs_eye_pos process_eyepoint(daeElement* helper);
		pcs_slot process_gunbank(daeElement *helper, int type);
		void process_moment_of_inertia(daeElement *element);
		vector3d fix_axes(vector3d broken, matrix rotation);
		vector3d get_translation(daeElement *element, matrix rotation = matrix());
		//vector3d get_scale(daeElement *element, matrix rotation = matrix());
		matrix get_rotation(daeElement *element, matrix old = matrix());

	public:
		DAEHandler(std::string filename, PCS_Model *model, AsyncProgress* progress, bool mirror_x_axis, bool mirror_y_axis, bool mirror_z_axis);
		void process_subobj(daeElement* element, int parent = -1, matrix rotation = matrix());
		void process_dock_helpers(daeElement *element);

		~DAEHandler();

		int populate(void);

		vector3d min_bounding_box;
		vector3d max_bounding_box;
		float radius;

};

class DAESaver {
private:
	DAE dae;
	daeElement *root;
	std::string filename;
	PCS_Model *model;
	int num_textures;
	std::vector<daeElement*> subobjs;
	std::vector<daeElement*> specials;
	std::vector<daeElement*> docks;
	daeElement *geoms;
	daeElement *scene;
	daeElement *materials;
	std::vector<pcs_sobj*> model_subobjs;
	int export_helpers;
	int props_as_helpers;
	AsyncProgress *progress;

	void add_refs(daeElement *mesh, std::string name, std::vector<int> refs, std::vector<int> sizes,daeElement *node = NULL, int texture = -1);
	daeElement *find_subsystem(std::string name,vector3d &offset, vector3d &scale);
	daeElement *find_helper(daeElement *element);
	daeElement *find_dockpoint(int idx,vector3d &offset);
	void add_property(daeElement *props, const char* prop);
	void add_header();
	void add_textures();
	void add_geom();
	void get_subobj(int idx,std::string *name=NULL);
	daeElement *get_polygroups(std::vector<std::vector <pcs_polygon*> > polies, std::string name,daeElement *node);
	void add_turret_fps();
	void add_docks();
	void add_thrusters();
	void add_guns();
	void add_eyes();
	void add_subsystems();
	void add_shield();
	void add_paths();
	void add_helper(daeElement *element,std::string properties);
	std::string add_material(int idx,daeElement *node);
	void add_glows();
	void add_insignia();
	void add_moment_of_inertia();
	static void write_transform(daeElement *element, const vector3d& offset, const vector3d& norm, const vector3d& base, float scale=1.0f);

public:

	DAESaver(std::string name, PCS_Model *model, int helpers, int props_as_helpers, AsyncProgress* progress);
	int save(void);

	~DAESaver() {
	}
};

#endif
