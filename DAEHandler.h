#if LOGGING_DAE
#include <iostream>
#endif
#include <pugixml.hpp>

#include <map>
#include "pcs_file.h"
#include "vector3d.h"
#include "DAEDataStructs.h"


#ifndef _DAEHandler_H_
#define _DAEHandler_H_

#define LOGGING_DAE 0

class DAEHandler
{
	private:
		pugi::xml_document root;
		std::string filename;
#if LOGGING_DAE
		std::ofstream log;
#endif
		float scaling_factor;
		int num_textures;
		std::vector<std::string> textures;
		PCS_Model *model;
		std::vector<boost::shared_ptr<pcs_sobj> > subobjs;
		std::vector<boost::shared_ptr<pcs_special> > specials;
		std::vector<boost::shared_ptr<pcs_dock_point> > docks;
		std::vector<pcs_eye_pos> eyes;
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
		void process_vector3d(vector3d vec, boost::shared_ptr<pcs_sobj> subobj = boost::shared_ptr<pcs_sobj>((pcs_sobj*)NULL));


		int find_or_add_texture(std::string name);
		int find_texture_id(std::string name, const std::map<std::string, std::string>& symbol_to_id);
		void subsystem_handler(pugi::xml_node& helper, bool isSubsystem);
		void shield_handler(pugi::xml_node& helper);
		void eyepoint_handler(pugi::xml_node& helper);
		void process_poly_group(pugi::xml_node& element, boost::shared_ptr<pcs_sobj> subobj, matrix rotation, const std::map<std::string, std::string>& symbol_to_id);
		void process_sobj_helpers(pugi::xml_node& element,int current_sobj_id, int parent_sobj_id, matrix rotation);
		void process_special_helpers(pugi::xml_node& element, int idx, matrix rotation);
		void process_properties(pugi::xml_node element,std::string *properties);
		void process_thrusters(pugi::xml_node& element,std::string name,matrix rotation, vector3d offset);
		void process_firepoints(pugi::xml_node& element,int parent, int arm,matrix rotation);
		void process_glowpoints(pugi::xml_node& element,int parent,matrix rotation, vector3d offset);
		void process_glowpoints_properties(pcs_glow_array &glowbank);
		void process_path(pugi::xml_node& element,std::string parent,matrix rotation, vector3d offset, int dock = -1);
		void process_insignia(pugi::xml_node& element);
		void process_dockpoint(pugi::xml_node& element);
		void process_sobj_vec(pugi::xml_node& element, matrix rotation, std::string* properties);
		void process_sobj_rotate(pugi::xml_node& element, matrix rotation, boost::shared_ptr<pcs_sobj> sobj, bool speed=true);
		pcs_eye_pos process_eyepoint(pugi::xml_node& helper, matrix transform = matrix(), int subobj_idx = -1);
		pcs_slot process_gunbank(pugi::xml_node& helper, int type);
		void process_mass(pugi::xml_node element);
		void process_moment_of_inertia(pugi::xml_node element);
		vector3d fix_axes(vector3d broken, matrix rotation);
		vector3d get_translation(const pugi::xml_node& element, matrix rotation = matrix());
		//vector3d get_scale(pugi::xml_node& element, matrix rotation = matrix());
		matrix get_rotation(const pugi::xml_node& element, matrix old = matrix());

	public:
		DAEHandler(std::string filename, PCS_Model *model, AsyncProgress* progress, bool mirror_x_axis, bool mirror_y_axis, bool mirror_z_axis);
		void process_subobj(const pugi::xml_node& element, int parent = -1, matrix rotation = matrix());
		void process_dock_helpers(pugi::xml_node& element);

		virtual ~DAEHandler() {}

		int populate(void);

		vector3d min_bounding_box;
		vector3d max_bounding_box;
		float radius;

};

class DAESaver {
private:
	pugi::xml_document doc;
	pugi::xml_node root;
	std::string filename;
	PCS_Model *model;
	int num_textures;
	std::vector<pugi::xml_node> subobjs;
	std::vector<pugi::xml_node> specials;
	std::vector<pugi::xml_node> docks;
	std::vector<pugi::xml_node> helpers;
	pugi::xml_node geoms;
	pugi::xml_node scene;
	pugi::xml_node materials;
	std::vector<pcs_sobj*> model_subobjs;
	int export_helpers;
	int props_as_helpers;
	AsyncProgress *progress;

	void add_refs(pugi::xml_node& mesh, std::string name, std::vector<int> refs, std::vector<int> sizes,pugi::xml_node* node = NULL, int texture = -1);
	pugi::xml_node find_subsystem(std::string name,vector3d &offset, vector3d &scale);
	pugi::xml_node find_helper(pugi::xml_node& element);
	pugi::xml_node find_dockpoint(int idx,vector3d &offset);
	void add_property(pugi::xml_node& props, const char* prop);
	void add_header();
	void add_textures();
	void add_geom();
	void get_subobj(int idx,std::string *name=NULL);
	pugi::xml_node get_polygroups(std::vector<std::vector <pcs_polygon*> > polies, std::string name,pugi::xml_node& node);
	void add_turret_fps();
	void add_docks();
	void add_thrusters();
	void add_guns();
	void add_eyes();
	void add_subsystems();
	void add_shield();
	void add_paths();
	pugi::xml_node add_helper(pugi::xml_node& element,std::string properties);
	void add_sobj_helpers(int idx);
	std::string add_material(int idx,pugi::xml_node& node);
	void add_glows();
	void add_insignia();
	void add_center_of_mass();
	void add_autocentering();
	void add_mass();
	void add_moment_of_inertia();
	static void write_transform(pugi::xml_node& element, const vector3d& offset, const vector3d& norm, const vector3d& base, float scale=1.0f, float external_scale=1.0f);
	static void write_transform_binormal(pugi::xml_node& element, const vector3d& offset, const vector3d& norm, const vector3d& binorm, const vector3d& base, float scale=1.0f, float external_scale=1.0f);

public:

	DAESaver(std::string name, PCS_Model *model, int helpers, int props_as_helpers, AsyncProgress* progress);
	int save(void);

	~DAESaver() {
	}
};

#endif
