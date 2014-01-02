#include "DAEHandler.h"
#include "pcs_file.h"
#include "BSPHandler.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <limits>
#include <map>
#include <unordered_map>
#include <boost/tr1/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#undef max

using namespace std;

namespace std {
	template<>
	struct hash<std::pair<float, float>> {
		typedef std::pair<float, float> argument_type;
		typedef std::size_t value_type;

		value_type operator()(argument_type const& v) const {
			value_type const h1(std::hash<float>()(v.first));
			value_type const h2(std::hash<float>()(v.second));
			return h1 ^ (h2 << 3);
		}
	};
}


namespace {

	std::vector<std::string> split_string(const std::string& str) {
		std::vector<std::string> result;
		size_t last = 0;
		for (size_t i = 0; i < str.size(); i++) {
			if (isspace(str[i])) {
				result.push_back(str.substr(last, i - last));
				last = i;
			}
		}
		if (last != str.size()) {
			result.push_back(str.substr(last));
		}
		return result;
	}

	pugi::xml_node find_node(pugi::xml_node base, const char* type) {
		string xpath = ".//";
		xpath += type;
		return base.select_single_node(xpath.c_str()).node();
	}

	std::vector<pugi::xml_node> find_nodes(pugi::xml_node base, const char* type) {
		string xpath = ".//";
		xpath += type;
		pugi::xpath_node_set xpath_nodes = base.select_nodes(xpath.c_str());
		std::vector<pugi::xml_node> nodes;
		nodes.reserve(xpath_nodes.size());
		for (const pugi::xpath_node* i = xpath_nodes.begin(); i != xpath_nodes.end(); ++i) {
			nodes.push_back(i->node());
		}
		return nodes;
	}

	pugi::xml_node find_by_id(const char* type, const char* id, const pugi::xml_document& doc) {
		string xpath = "//";
		xpath += type;
		xpath += "[@id='";
		if (id[0] == '#') {
			id++;
		}
		xpath += id;
		xpath += "']";
		return doc.select_single_node(xpath.c_str()).node();
	}

	vector3d relative_to_absolute(vector3d vec, const pcs_sobj* subobj, const std::vector<pcs_sobj*>& subobjs) {
		while (subobj->parent_sobj != -1) {
			vec += subobj->offset;
			subobj = subobjs[subobj->parent_sobj];
		}
		vec += subobj->offset;
		return vec;
	}

	vector3d relative_to_absolute(vector3d vec, const pcs_sobj* subobj, PCS_Model* model) {
		while (subobj->parent_sobj != -1) {
			vec += subobj->offset;
			subobj = &model->SOBJ(subobj->parent_sobj);
		}
		vec += subobj->offset;
		return vec;
	}

}

DAEHandler::DAEHandler(string filename, PCS_Model *model, AsyncProgress *progress, bool mirror_x_axis, bool mirror_y_axis, bool mirror_z_axis) :
	filename(filename),
	model(model) {
	root.load_file(filename.c_str());

	radius = 0;
	scaling_factor = 1.0f;

	pugi::xml_node temp = find_node(root, "up_axis");
	if (temp) {
		if (boost::algorithm::equals(temp.child_value(), "X_UP")) {
			up_axis = 0;			
		} else if (boost::algorithm::equals(temp.child_value(), "Y_UP")) {
			up_axis = 1;
			front = vector3d(0,0,1);
			up = vector3d(0,1,0);
		} else {
			up_axis = 2;
			front = vector3d(0,1,0);
			up = vector3d(0,0,1);
		}
	}
	mirror_x = mirror_x_axis;
	mirror_y = mirror_y_axis;
	mirror_z = mirror_z_axis;

	this->progress = progress;
}

int DAEHandler::populate(void) {
	progress->setTarget(200);
	progress->setMessage("Opening and Reading DAE");
	progress->Notify();
	num_textures = 0;

	pugi::xml_node scene = root.child("COLLADA").child("library_visual_scenes").child("visual_scene");
	vector<int> lods;
	vector<pcs_slot> guns, missiles;
	string name;
	map<string, pugi::xml_node> detail, debris;
	model->SetMass(-1);
	for (pugi::xml_node helper = scene.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			name = get_name(helper);
			progress->incrementWithMessage("Processing " + name);
			if (boost::algorithm::istarts_with(name, "subsystem")) {
				subsystem_handler(helper, true);
			} else if (boost::algorithm::istarts_with(name, "special")) {
				subsystem_handler(helper, false);
			} else if (boost::algorithm::istarts_with(name, "shield")) {
				shield_handler(helper);
			} else if (boost::algorithm::istarts_with(name, "thrusters")) {
				process_thrusters(helper,string(),matrix(),vector3d(0,0,0));
			} else if (boost::algorithm::istarts_with(name, "insigLOD")) {
				process_insignia(helper);
			} else if (boost::algorithm::istarts_with(name, "debris")) {
				debris[name] = helper;
			} else if (boost::algorithm::istarts_with(name, "dockpoint")) {
				process_dockpoint(helper);
			} else if (boost::algorithm::istarts_with(name, "bay")) {
				process_path(helper, "", matrix(), vector3d());
			} else if (boost::algorithm::istarts_with(name, "detail")) {
				detail[name] = helper;
			} else if (boost::algorithm::istarts_with(name, "eyepoint")) {
				unsigned int eye;
				if (name.length() > strlen("eyepoint")) {
					eye = atoi(name.substr(strlen("eyepoint"),2).c_str()) - 1;
					if (eyes.size() <= eye) {
						eyes.resize(eye + 1);
					}
				} else {
					continue;
				}
				eyes[eye] = process_eyepoint(helper);
			} else if (boost::algorithm::istarts_with(name, "gunbank")) {
				unsigned int bank;
				if (name.length() > strlen("gunbank")) {
					bank = atoi(name.substr(strlen("gunbank"),2).c_str()) - 1;
					if (guns.size() <= bank) {
						guns.resize(bank + 1);
					}
				} else {
					continue;
				}
				guns[bank] = process_gunbank(helper, 0);
			} else if (boost::algorithm::istarts_with(name, "missilebank")) {
				unsigned int bank;
				if (name.length() > strlen("missilebank")) {
					bank = atoi(name.substr(strlen("missilebank"),2).c_str()) - 1;
					if (missiles.size() <= bank) {
						missiles.resize(bank + 1);
					}
				} else {
					continue;
				}
				missiles[bank] = process_gunbank(helper, 1);
			} else if (boost::algorithm::istarts_with(name, "mass")) {
				process_mass(helper);
			} else if (boost::algorithm::istarts_with(name, "moi")) {
				process_moment_of_inertia(helper);
			} else if (boost::algorithm::istarts_with(name, "com")) {
				model->SetCenterOfMass(get_translation(helper));
			} else if (boost::algorithm::istarts_with(name, "acen")) {
				model->SetAutoCenter(get_translation(helper));
			}
		}

	}
	for (map<string, pugi::xml_node>::iterator it = detail.begin(); it != detail.end(); ++it) {
		model->AddLOD(model->GetSOBJCount());
		process_subobj(it->second, -1);
	}
	for (map<string, pugi::xml_node>::iterator it = debris.begin(); it != debris.end(); ++it) {
		model->AddDebris(model->GetSOBJCount());
		process_subobj(it->second, -1);
	}
	progress->setTarget(203 + missiles.size() + 3);
	progress->SetProgress(203);

	model->set_eyes(eyes);
	int num_guns = guns.size();
	guns.resize(guns.size() + missiles.size());
	for (unsigned int i = 0; i < missiles.size(); i++) {
		progress->incrementWithMessage("Finalising gun " + boost::lexical_cast<std::string>(i));
		guns[i + num_guns] = missiles[i];
	}
	model->set_weapons(guns);
	// Don't put this before subobject handling...
	progress->incrementWithMessage("Setting radius");
	model->SetMaxRadius(radius);
	progress->incrementWithMessage("Setting bounding box");
	model->SetMinBounding(min_bounding_box);
	model->SetMaxBounding(max_bounding_box);

	// Perpetuating a shonky way of calculating mass...
	progress->incrementWithMessage("Setting mass");
	if (model->GetMass() == -1) {
		model->SetMass((model->GetMaxBounding().x - model->GetMinBounding().x) * (model->GetMaxBounding().y - model->GetMinBounding().y) * (model->GetMaxBounding().z - model->GetMinBounding().z));
	}
	return 0;

}

void DAEHandler::process_subobj(const pugi::xml_node& element, int parent, matrix rotation_matrix) {
	pugi::xml_node geom = element.child("instance_geometry");

	if (!geom) {
		stringstream error;
		error << "instance_geometry not found for subobject " << get_name(element);
		wxMessageBox(wxString(error.str().c_str(), wxConvUTF8));
		return;
	}
	progress->incrementWithMessage("Processing " + get_name(element));

	std::vector<pugi::xml_node> instance_materials = find_nodes(geom, "instance_material");
	std::map<string, string> symbol_to_id;
	for (std::vector<pugi::xml_node>::iterator i = instance_materials.begin(); i < instance_materials.end(); ++i) {
		symbol_to_id.insert(make_pair(i->attribute("symbol").as_string(), i->attribute("target").as_string()));
	}
	pugi::xml_node mesh = find_by_id("geometry", geom.attribute("url").value(), root).child("mesh");
	model->AddSOBJ();
	auto subobj = &model->SOBJ(model->GetSOBJCount() - 1);
	subobj->bounding_box_min_point = vector3d(1e30f,1e30f,1e30f);
	subobj->bounding_box_max_point = vector3d(-1e30f,-1e30f,-1e30f);
	subobj->name = get_name(element).c_str();
	subobj->properties = "";
	subobj->parent_sobj = parent;

	subobj->offset = get_translation(element,rotation_matrix);

	if (subobj->parent_sobj != -1) {
		subobj->geometric_center = relative_to_absolute(subobj->offset, &model->SOBJ(subobj->parent_sobj), model);
	} else {
		subobj->geometric_center = vector3d(0,0,0);
	}

	rotation_matrix = get_rotation(element, rotation_matrix);

	for (pugi::xml_node poly_group = mesh.first_child(); poly_group; poly_group = poly_group.next_sibling()) {
		// Add the polies to the subobj.
		if (boost::algorithm::equals(poly_group.name(), "triangles") || boost::algorithm::equals(poly_group.name(), "polylist")) {
			process_poly_group(poly_group, subobj, rotation_matrix, symbol_to_id);
		}
	}
	pugi::xml_node trans = find_by_id("node", (string(element.attribute("id").value()) + "-trans").c_str(), root);
	if (trans) {
		mesh = trans.child("instance_geometry");
		if (mesh) {
			mesh = find_by_id("geometry", geom.attribute("url").value(), root).child("mesh");
			if (mesh) {
				mesh = mesh.child("mesh");
				if (mesh) {
					for (pugi::xml_node poly_group = mesh.first_child(); poly_group; poly_group = poly_group.next_sibling()) {
						// Add the polies to the subobj.
						if (boost::algorithm::equals(poly_group.name(), "triangles") || boost::algorithm::equals(poly_group.name(), "polylist")) {
							process_poly_group(poly_group, subobj, rotation_matrix, symbol_to_id);
						}
					}
				}
			}
		}
	}

	int current_sobj_id = model->GetSOBJCount() - 1;

	pugi::xml_node sobj_helpers;
	std::map<std::string, pugi::xml_node> subobj_map;
	for (pugi::xml_node subobj_node = element.first_child(); subobj_node; subobj_node = subobj_node.next_sibling()) {
		if (boost::algorithm::equals(subobj_node.name(), "node")) {
			if (!boost::algorithm::istarts_with(get_name(subobj_node), "helper")) {
				if (!strstr(get_name(subobj_node).c_str(),"-trans")) {
					subobj_map[get_name(subobj_node)] = subobj_node;
				}
			} else {
				sobj_helpers = subobj_node;
			}
		} else if (boost::algorithm::equals(subobj_node.name(), "extra")) {
			pugi::xml_node props = find_node(subobj_node, "user_properties");
			if (props) {
				subobj->properties = props.child_value();
			}
		}
	}
	for (std::map<std::string, pugi::xml_node>::const_iterator it = subobj_map.begin(); it != subobj_map.end(); ++it) {
		process_subobj(it->second, current_sobj_id, rotation_matrix);
	}

	if (sobj_helpers) {
		process_sobj_helpers(sobj_helpers, current_sobj_id, parent, rotation_matrix);
	}

}

void DAEHandler::process_dockpoint(pugi::xml_node& dockpoint_helper) {
	model->AddDocking();
	pcs_dock_point* dockpoint = &model->Dock(model->GetDockingCount() - 1);
	pcs_hardpoint temp;
	for (pugi::xml_node helper = dockpoint_helper.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node") &&
			!boost::algorithm::istarts_with(get_name(helper), "helper")) {
			temp.norm = fix_axes(up, get_rotation(helper));
			temp.point = get_translation(helper);
			dockpoint->dockpoints.push_back(temp);
		}
	}
	process_dock_helpers(dockpoint_helper, dockpoint);
}


void DAEHandler::process_poly_group(pugi::xml_node& element, pcs_sobj* subobj, matrix rotation, const std::map<string, string>& symbol_to_id) {
	vector<int> refs;
	DAEInputs inputs(element);
	int poly_offset = subobj->polygons.size();
	bool triangles = false;

	pugi::xml_node vcount = element.child("vcount");
	if (!vcount) {
		triangles = true;
	}

	string texture = element.attribute("material").value();
	int texture_id = find_texture_id(texture, symbol_to_id);

	pugi::xml_node ref_element = element.child("p");
	if (ref_element) {
		parse_int_array(ref_element.child_value(), &refs);
	}
	int num_polies = element.attribute("count").as_int();

	subobj->polygons.resize(num_polies + poly_offset);
	vector<int> counts;
	if (!triangles) {
		parse_int_array(vcount.child_value(), &counts, num_polies);
	}

	int position = 0;

	// add each polygon
	for (int j = poly_offset; j < num_polies + poly_offset; j++) {
		subobj->polygons[j].verts.resize(triangles ? 3 : counts[j - poly_offset]);

		for (int k = subobj->polygons[j].verts.size() - 1; k >= 0; k--) {
			subobj->polygons[j].verts[k].point = fix_axes(vector3d((inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.x_offset()],(inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.y_offset()], (inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.z_offset()]),rotation);

			process_vector3d(subobj->polygons[j].verts[k].point,subobj);
			subobj->polygons[j].centeroid += subobj->polygons[j].verts[k].point;
			if (inputs.norm.is_valid()) {
				subobj->polygons[j].verts[k].norm = fix_axes(vector3d((inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.x_offset()],(inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.y_offset()],(inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.z_offset()]),rotation);
			} else {
				subobj->polygons[j].verts[k].norm = vector3d(0,0,0);
			}
			subobj->polygons[j].norm += subobj->polygons[j].verts[k].norm;
			if (inputs.uv.is_valid()) {
				subobj->polygons[j].verts[k].u = (inputs.uv.values())[refs[position + inputs.uv_offset] * inputs.uv.stride() + inputs.uv.u_offset()];
				// upside down textures...
				subobj->polygons[j].verts[k].v = 1.0f - (inputs.uv.values())[refs[position + inputs.uv_offset] * inputs.uv.stride() + inputs.uv.v_offset()];
			} else {
				subobj->polygons[j].verts[k].u = 0.0f;
				subobj->polygons[j].verts[k].v = 0.0f;
			}


			// TODO: something here
			subobj->polygons[j].verts[k].facet_angle = 180;
			position += inputs.max_offset;
		}
		subobj->polygons[j].norm = MakeUnitVector(FigureNormal(
					subobj->polygons[j].verts[0].point,
					subobj->polygons[j].verts[1].point,
				   	subobj->polygons[j].verts[2].point));
		subobj->polygons[j].centeroid = subobj->polygons[j].centeroid / subobj->polygons[j].verts.size();
		subobj->polygons[j].texture_id = texture_id;
	}
}

void DAEHandler::process_sobj_helpers(pugi::xml_node& element, int current_sobj_id, int parent_sobj_id, matrix rotation_matrix) {
	vector3d offset = relative_to_absolute(vector3d(0,0,0), &model->SOBJ(current_sobj_id), model);
	for (pugi::xml_node helper = element.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helper), "properties")) {
				process_properties(helper, &model->SOBJ(current_sobj_id).properties);
			}
		}
	}
	for (pugi::xml_node helper = element.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			std::string name = get_name(helper);
			if (boost::algorithm::istarts_with(name, "thrusters")) {
				process_thrusters(helper,model->SOBJ(current_sobj_id).name,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "multifirepoints")) {
				process_firepoints(helper,parent_sobj_id,current_sobj_id,rotation_matrix);
			} else if (boost::algorithm::istarts_with(name, "firepoints")) {
				process_firepoints(helper,current_sobj_id,current_sobj_id,rotation_matrix);
			} else if (boost::algorithm::istarts_with(name, "glowbank")) {
				process_glowpoints(helper,current_sobj_id,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "bay")) {
				process_path(helper,model->SOBJ(current_sobj_id).name,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "path")) {
				process_path(helper, model->SOBJ(current_sobj_id).name, rotation_matrix, offset);
			} else if (boost::algorithm::istarts_with(name, "vec")) {
				process_sobj_vec(helper,rotation_matrix, &model->SOBJ(current_sobj_id).properties);
			} else if (boost::algorithm::istarts_with(name, "rotate-nospeed")) {
				process_sobj_rotate(helper,rotation_matrix, &model->SOBJ(current_sobj_id), false);
			} else if (boost::algorithm::istarts_with(name, "rotate")) {
				process_sobj_rotate(helper,rotation_matrix, &model->SOBJ(current_sobj_id));
			} else if (boost::algorithm::istarts_with(name, "eyepoint")) {
				unsigned int eye;
				if (name.length() > strlen("eyepoint")) {
					eye = atoi(name.substr(strlen("eyepoint"),2).c_str()) - 1;
					if (eyes.size() <= eye) {
						eyes.resize(eye + 1);
					}
					eyes[eye] = process_eyepoint(helper, rotation_matrix, current_sobj_id);
				}
			}
		}
	}
	trim_extra_spaces(model->SOBJ(current_sobj_id).properties);
}

void DAEHandler::process_special_helpers(pugi::xml_node& element, pcs_special* special, matrix rotation) {
	pugi::xml_node helper_parent;
	for (helper_parent = element.first_child(); helper_parent; helper_parent = helper_parent.next_sibling()) {
		if (boost::algorithm::equals(helper_parent.name(), "node") && boost::algorithm::istarts_with(get_name(helper_parent), "helper")) {
			break;
		}
	}
	if (!helper_parent) {
		return;
	}
	
	vector3d offset = special->point;
	for (pugi::xml_node helper = helper_parent.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helper), "properties")) {
				process_properties(helper, &special->properties);
			}
		}
	}
	for (pugi::xml_node helper = helper_parent.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			if (boost::algorithm::istarts_with(get_name(helper), "thrusters")) {
				process_thrusters(helper, special->name, rotation, offset);
			} else if (boost::algorithm::istarts_with(get_name(helper), "bay")) {
				process_path(helper, special->name, rotation, offset);
			} else if (boost::algorithm::istarts_with(get_name(helper), "path")) {
				process_path(helper, special->name, rotation, offset);
			}
		}
	}
}

void DAEHandler::process_dock_helpers(pugi::xml_node& element, pcs_dock_point* dockpoint) {
	pugi::xml_node helper_parent;
	for (helper_parent = element.first_child(); helper_parent; helper_parent = helper_parent.next_sibling()) {
		if (boost::algorithm::equals(helper_parent.name(), "node") && boost::algorithm::istarts_with(get_name(helper_parent), "helper")) {
			break;
		}
	}
	pugi::xml_node props = find_node(element, "user_properties");
	if (props) {
		dockpoint->properties = props.child_value();
	}

	if (!helper_parent) {
		return;
	}
	
	vector3d offset = vector3d(0,0,0);//docks[idx]->dockpoints[0].point;
	for (pugi::xml_node helper = helper_parent.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helper), "properties")) {
				process_properties(helper, &dockpoint->properties);
			}
		}
	}
	for (pugi::xml_node helper = helper_parent.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			if (boost::algorithm::istarts_with(get_name(helper), "path")) {
				process_path(helper, "", matrix(), offset, dockpoint);
			}
		}
	}

}

void DAEHandler::process_properties(pugi::xml_node element, string *properties) {
	if (!element) {
		return;
	}

	pugi::xml_node extra = find_node(element, "user_properties");
	if (extra) {
		*properties = extra.child_value();
		return;
	}
	*properties = "";
	for (pugi::xml_node helper = element.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			*properties += get_name(helper);
			*properties += "\n";
		}
	}
	trim_extra_spaces(*properties);
}

void DAEHandler::process_sobj_vec(pugi::xml_node& element, matrix rotation, std::string* properties) {
	rotation = get_rotation(element, rotation);
	vector3d uvec = MakeUnitVector(fix_axes(up, rotation));
	vector3d fvec = MakeUnitVector(fix_axes(front, rotation));
	if (properties->find("uvec") == string::npos) {
		properties->append("\n$uvec:");
		stringstream output;
		output << -uvec.x << ", " << uvec.y << ", " << uvec.z;
		properties->append(output.str());
	}
	if (properties->find("fvec") == string::npos) {
		properties->append("\n$fvec:");
		stringstream output;
		output << -fvec.x << ", " << fvec.y << ", " << fvec.z;
		properties->append(output.str());
	}
}

void DAEHandler::process_sobj_rotate(pugi::xml_node& element, matrix rotation, pcs_sobj* sobj, bool speed) {
	rotation = get_rotation(element, rotation);
	vector3d rotate = fix_axes(up, rotation);
	float x, y, z, x_abs, y_abs, z_abs;
	x = dot(rotate, vector3d(1,0,0));
	y = dot(rotate, vector3d(0,1,0));
	z = dot(rotate, vector3d(0,0,1));
	x_abs = fabs(x);
	y_abs = fabs(y);
	z_abs = fabs(z);
	float length;
	if (x_abs > y_abs) {
		if (x_abs > z_abs) {
			sobj->movement_axis = MV_X;
			length = x;
		} else {
			sobj->movement_axis = MV_Z;
			length = z;
		}
	} else if (y_abs > z_abs) {
		sobj->movement_axis = MV_Y;
		length = y;
	} else {
		sobj->movement_axis = MV_Z;
		length = z;
	}
	sobj->movement_type = ROTATE;
	if (speed) {
		if (sobj->properties.find("rotate") == string::npos) {
			sobj->properties.append("\n$rotate=");
			stringstream output;
			output << length;
			sobj->properties.append(output.str());
		}
	}


}

void DAEHandler::process_thrusters(pugi::xml_node& element,string name,matrix rotation_matrix, vector3d offset) {
	offset = get_translation(element, rotation_matrix) + offset;
	rotation_matrix = get_rotation(element,rotation_matrix);
	pcs_thruster thruster;
	pcs_thrust_glow glow;
	if (name.size() > 0 && strlen(name.c_str()) > 0) {
		if (name[0] == '$') {
			thruster.properties = string("$engine_subsystem=") + name;
		} else {
			thruster.properties = string("$engine_subsystem=$") + name;
		}
	}
	for (pugi::xml_node thruster_node = element.first_child(); thruster_node; thruster_node = thruster_node.next_sibling()) {
		if (boost::algorithm::equals(thruster_node.name(), "node")) {
			glow.pos = get_translation(thruster_node, rotation_matrix) + offset;
			glow.norm = MakeUnitVector(fix_axes(front,get_rotation(thruster_node,rotation_matrix)));
			glow.radius = get_rotation(thruster_node, rotation_matrix).scale();
			thruster.points.push_back(glow);
		}
	}
	model->AddThruster(&thruster);
}

void DAEHandler::process_firepoints(pugi::xml_node& element,int parent, int arm,matrix rotation_matrix) {
	pcs_turret turret;
	turret.sobj_parent = parent;
	turret.sobj_par_phys = arm;
	turret.turret_normal = vector3d(0,0,0);

	for (pugi::xml_node turret_node = element.first_child(); turret_node; turret_node = turret_node.next_sibling()) {
		if (boost::algorithm::equals(turret_node.name(), "node")) {
			turret.turret_normal += fix_axes(up,get_rotation(turret_node,rotation_matrix));
			turret.fire_points.push_back(get_translation(turret_node,rotation_matrix));
		}
	}
	if (model->SOBJ(parent).properties.size() == 0) {
		model->SOBJ(parent).properties = "$special=subsystem\n$fov=180\n$name=GunTurret\n";
	} else {
		if (!strstr(model->SOBJ(parent).properties.c_str(), "special")) {
			model->SOBJ(parent).properties += "$special=subsystem\n";
		}
		if (!strstr(model->SOBJ(parent).properties.c_str(), "fov")) {
			model->SOBJ(parent).properties += "$fov=180\n";
		}
	}
	turret.turret_normal = MakeUnitVector(turret.turret_normal);
	model->AddTurret(&turret);
}

void DAEHandler::process_path(pugi::xml_node& element, string parent, matrix rotation_matrix, vector3d offset, pcs_dock_point* dockpoint) {
	pcs_path path;
	path.name = string ("$") + get_name(element).c_str();
	trim_extra_spaces(path.name);
	path.parent = parent;
	std::multimap<std::string, pcs_pvert> ordered_paths;
	pcs_pvert vert;

	for (pugi::xml_node path_node = element.first_child(); path_node; path_node = path_node.next_sibling()) {
		if (boost::algorithm::equals(path_node.name(), "node")) {
			vert.pos = get_translation(path_node, rotation_matrix) + offset;
			vert.radius = get_rotation(path_node, rotation_matrix).scale();
			ordered_paths.insert(make_pair(get_name(path_node), vert));
		}
	}
	for (std::multimap<string, pcs_pvert>::iterator it = ordered_paths.begin(); it != ordered_paths.end(); ++it) {
		path.verts.push_back(it->second);
	}

	if (dockpoint) {
		dockpoint->paths.push_back(model->GetPathCount());
	}

	model->AddPath(&path);
}

void DAEHandler::process_glowpoints(pugi::xml_node& element,int parent,matrix rotation_matrix, vector3d offset) {
	pcs_glow_array glowbank;
	glowbank.obj_parent = parent;
	pcs_thrust_glow glow;
	for (pugi::xml_node glow_node = element.first_child(); glow_node; glow_node = glow_node.next_sibling()) {
		if (boost::algorithm::equals(glow_node.name(), "node")) {
			if (boost::algorithm::istarts_with(get_name(glow_node), "helper")) {
				process_properties(glow_node.child("node"),&glowbank.properties);
			} else {
				glow.pos = get_translation(glow_node, rotation_matrix);
				glow.radius = get_rotation(glow_node, rotation_matrix).scale();
				glow.norm = vector3d(0,0,0);
				glowbank.lights.push_back(glow);
			}
		} else if (boost::algorithm::equals(glow_node.name(), "extra")) {
			pugi::xml_node props = find_node(glow_node, "user_properties");
			if (props) {
				glowbank.properties = props.child_value();
			}
		}
	}
	process_glowpoints_properties(glowbank);
	model->AddLight(&glowbank);
}

void DAEHandler::process_glowpoints_properties(pcs_glow_array &glowbank) {
	std::stringstream stream(glowbank.properties);
	std::string temp;
	stream >> temp;
	while (!stream.eof()) {
		if (temp[0] == '$') {
			glowbank.properties = temp;
		} else {
			size_t offset = temp.find_first_of("=");
			if (offset != string::npos) {
				int value = atoi(temp.substr(offset + 1).c_str());
				if (boost::algorithm::iequals(temp.substr(0, offset), "type")) {
					glowbank.type = value;
				} else if (boost::algorithm::iequals(temp.substr(0, offset), "on")) {
					glowbank.on_time = value;
				} else if (boost::algorithm::iequals(temp.substr(0, offset), "off")) {
					glowbank.off_time = value;
				} else if (boost::algorithm::iequals(temp.substr(0, offset), "displacement")) {
					glowbank.disp_time = value;
				} else if (boost::algorithm::iequals(temp.substr(0, offset), "lod")) {
					glowbank.LOD = value;
				}
			}
		}
		stream >> temp;
	}
}

string strip_texture(string name) {
	std::tr1::regex re("([^/\\\\]+?)(?:\\.[^./\\\\]+)?$");
	return std::tr1::regex_replace(name, re, "$1", boost::format_no_copy | boost::format_first_only);

}

int DAEHandler::find_texture_id(string name, const std::map<std::string, std::string>& symbol_to_id) {
	std::map<std::string, std::string>::const_iterator it = symbol_to_id.find(name);
	if (it != symbol_to_id.end()) {
		name.assign(it->second);
	}
	pugi::xml_node current = find_by_id("material", name.c_str(), root);
	if (!current) {
		return -1;
	}
	current = current.child("instance_effect");
	if (!current) {
		return -1;
	}
	current = find_by_id("effect", current.attribute("url").value(), root);
	if (!current) {
		return -1;
	}
	current = find_node(current, "init_from");
	if (!current) {
		return -1;
	}
	current = find_by_id("image", current.child_value(), root);
	if (!current) {
		return -1;
	}
	current = find_node(current, "init_from");
	if (!current) {
		return -1;
	}
	return find_or_add_texture(string(current.child_value()));
}

int DAEHandler::find_or_add_texture(string name) {
	if (texture_map.find(name) == texture_map.end()) {
		texture_map[name] = num_textures++;
		model->AddTextures(strip_texture(name));
	}
	return texture_map[name];
}

void DAEHandler::subsystem_handler(pugi::xml_node& helper, bool isSubsystem) {
	model->AddSpecial();
	pcs_special* special = &model->Special(model->GetSpecialCount() - 1);
	matrix rot = get_rotation(helper);
	special->name = "$";
	if (isSubsystem) {
		special->name += &get_name(helper).c_str()[strlen("subsystem")];
	} else {
		special->properties.clear();
		special->name += &get_name(helper).c_str()[strlen("special")];
	}
	special->point = get_translation(helper);
	special->radius = rot.scale();
	process_special_helpers(helper, special, rot);
}

void DAEHandler::shield_handler(pugi::xml_node& helper) {
	pugi::xml_node geom = helper.child("instance_geometry");
	if (!geom) {
		return;
	}
	pugi::xml_node mesh = find_by_id("geometry", geom.attribute("url").value(), root).child("mesh");
	bool use_triangles = false;
	pugi::xml_node triangles = mesh.child("triangles");
	if (triangles) {
		use_triangles = true;
		mesh = triangles;
	} else {
		mesh = mesh.child("polylist");
	}
	matrix rotation = get_rotation(helper);
	
	vector<int> refs;

	DAEInputs inputs(mesh);
	if (!inputs.pos.is_valid()) {
		wxMessageBox(wxT("Positions not found for shield"));
		return;
	}
	if (!inputs.norm.is_valid()) {
		wxMessageBox(wxT("Normals not found for shield"));
		return;
	}

	pugi::xml_node ref_element = find_node(mesh, "p");
	if (ref_element) {
		parse_int_array(ref_element.child_value(), &refs);
	}

	int num_polies = mesh.attribute("count").as_int();

	std::vector<int> vcount;
	if (!use_triangles) {
		parse_int_array(mesh.child("vcount").child_value(), &vcount, num_polies);
	}

	int position = 0;
	// add each polygon
	pcs_shield_triangle shield_bit;
	for (int j = 0; j < num_polies; j++) {
		shield_bit.face_normal = vector3d(0,0,0);
		for (int k = 2; k >= 0; k--) {
			shield_bit.corners[k] = fix_axes(vector3d((inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.x_offset()],(inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.y_offset()], (inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.z_offset()]),rotation);
			shield_bit.face_normal += fix_axes(vector3d((inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.x_offset()],(inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.y_offset()],(inputs.norm.values())[refs[position + inputs.norm_offset] * inputs.norm.stride() + inputs.norm.z_offset()]),rotation);

			position += inputs.max_offset;
		}
		// TODO: deal with untriangulated shield meshes properly.
		if (!use_triangles && vcount[j] > 3) {
			position += inputs.max_offset * (vcount[j] - 2);
		}
		shield_bit.face_normal = MakeUnitVector(FigureNormal(
					shield_bit.corners[0],
					shield_bit.corners[1],
					shield_bit.corners[2]));
		shield_bit.face_normal = shield_bit.face_normal / 3;
		model->AddShldTri(&shield_bit);
	}
}

void DAEHandler::process_insignia(pugi::xml_node& element) {
	pcs_insig insignia;
	string name = get_name(element);
	insignia.lod = 0;
	if (name.length() >= 10) {
		name = name.substr(8, 2);
		insignia.lod = atoi(name.c_str()) - 1;
	}
	insignia.offset = get_translation(element);
	pugi::xml_node geom = element.child("instance_geometry");

	pugi::xml_node mesh = find_by_id("geometry", geom.attribute("url").value(), root).child("mesh");
	bool use_triangles = false;
	pugi::xml_node triangles = mesh.child("triangles");
	if (triangles) {
		use_triangles = true;
		mesh = triangles;
	} else {
		mesh = mesh.child("polylist");
	}

	matrix rotation = get_rotation(element);
	
	vector<int> refs;
	DAEInputs inputs(mesh);


	pugi::xml_node ref_element = find_node(mesh, "p");
	if (ref_element) {
		parse_int_array(ref_element.child_value(), &refs);
	}
	if (!inputs.pos.is_valid()) {
		wxMessageBox(wxT("Positions not found for insignia"));
		return;
	}
	if (!inputs.uv.is_valid()) {
		wxMessageBox(wxT("UV not found for insignia"));
		return;
	}

	int num_polies = mesh.attribute("count").as_int();

	std::vector<int> vcount;
	if (!use_triangles) {
		parse_int_array(mesh.child("vcount").child_value(), &vcount, num_polies);
	}


	int position = 0;
	insignia.faces.resize(num_polies);

	// add each polygon
	for (int j = 0; j < num_polies; j++) {
		for (int k = 2; k >= 0; k--) {
			insignia.faces[j].verts[k] = fix_axes(vector3d((inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.x_offset()],(inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.y_offset()], (inputs.pos.values())[refs[position + inputs.pos_offset] * inputs.pos.stride() + inputs.pos.z_offset()]),rotation);
			insignia.faces[j].u[k] = (inputs.uv.values())[refs[position + inputs.uv_offset] * inputs.uv.stride() + inputs.uv.u_offset()];
			insignia.faces[j].v[k] = 1.0f - (inputs.uv.values())[refs[position + inputs.uv_offset] * inputs.uv.stride() + inputs.uv.v_offset()];
			position += inputs.max_offset;
		}
		// TODO: deal with untriangulated insignia properly.
		if (!use_triangles && vcount[j] > 3) {
			position += inputs.max_offset * (vcount[j] - 2);
		}
	}
	model->AddInsignia(&insignia);
}

void DAEHandler::process_mass(pugi::xml_node element) {
	element = element.child("node");
	if (element) {
		std::stringstream stream(get_name(element));
		float mass;
		stream >> mass;
		model->SetMass(mass);
	}
}

void DAEHandler::process_moment_of_inertia(pugi::xml_node element) {
	element = element.child("node");
	if (element) {
		std::stringstream moi(get_name(element));
		float numbers[3][3];
		char dummy;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				moi >> numbers[i][j];
				moi >> dummy;
			}
		}
		model->SetMOI(numbers);
	}
}

void parse_int_array(const char* chars, std::vector<int> *result, unsigned int count) {
	if (count != (unsigned)-1) {
		result->reserve(count);
	}
	auto values = split_string(chars);
	for (const auto& value : values) {
		result->push_back(stoi(value));
	}
}

vector<float> parse_float_array(const char* chars, unsigned int count) {
	vector<float> result;
	if (count != (unsigned)-1) {
		result.reserve(count);
	}
	auto values = split_string(chars);
	for (const auto& value : values) {
		result.push_back(stof(value));
	}
	return result;
}

string write_int_array(const vector<int>& vec) {
	std::string result;
	for (unsigned int i = 0; i < vec.size(); i++) {
		if (i != 0) {
			result.push_back(' ');
		}		result.append(std::to_string(vec[i]));
	}
	return result;
}

string write_float_array(const vector<float>& vec) {
	std::string result;
	for (unsigned int i = 0; i < vec.size(); i++) {
		if (i != 0) {
			result.push_back(' ');
		}
		result.append(std::to_string(vec[i]));

	}
	return result;
}

vector3d DAEHandler::get_translation(const pugi::xml_node& element, matrix rotation) {
	const pugi::xml_node& position = element.child("translate");
	const pugi::xml_node& matrix = element.child("matrix"); 
	if (position) {
		vector<float> points = parse_float_array(position.child_value(), 3);
		return fix_axes(vector3d(points[0], points[1], points[2]), rotation);
	} else if (matrix) {
		vector<float> points = parse_float_array(matrix.child_value(), 16);
		return fix_axes(vector3d(points[3], points[7], points[11]), rotation);
	}
	return vector3d();
}

void DAEHandler::process_vector3d(vector3d vec, pcs_sobj* subobj) {
	if (subobj != NULL) {
		if (vec.x > subobj->bounding_box_max_point.x) {
			subobj->bounding_box_max_point.x = vec.x;
		}
		if (vec.y > subobj->bounding_box_max_point.y) {
			subobj->bounding_box_max_point.y = vec.y;
		}
		if (vec.z > subobj->bounding_box_max_point.z) {
			subobj->bounding_box_max_point.z = vec.z;
		}
		if (vec.x < subobj->bounding_box_min_point.x) {
			subobj->bounding_box_min_point.x = vec.x;
		}
		if (vec.y < subobj->bounding_box_min_point.y) {
			subobj->bounding_box_min_point.y = vec.y;
		}
		if (vec.z < subobj->bounding_box_min_point.z) {
			subobj->bounding_box_min_point.z = vec.z;
		}
		if (Magnitude(vec) > subobj->radius) {
			subobj->radius = Magnitude(vec);
		}
		vec = relative_to_absolute(vec, subobj, model);
	}
	if (vec.x > max_bounding_box.x) {
		max_bounding_box.x = vec.x;
	}
	if (vec.y > max_bounding_box.y) {
		max_bounding_box.y = vec.y;
	}
	if (vec.z > max_bounding_box.z) {
		max_bounding_box.z = vec.z;
	}
	if (vec.x < min_bounding_box.x) {
		min_bounding_box.x = vec.x;
	}
	if (vec.y < min_bounding_box.y) {
		min_bounding_box.y = vec.y;
	}
	if (vec.z < min_bounding_box.z) {
		min_bounding_box.z = vec.z;
	}
	if (Magnitude(vec) > radius) {
		radius = Magnitude(vec);
	}
}

vector3d absolute_to_relative(vector3d vec, pcs_sobj *subobj,vector<pcs_sobj*> *subobjs) {
	while (subobj->parent_sobj != -1) {
		vec += subobj->offset * -1;
		subobj = subobjs[0][subobj->parent_sobj];
	}
	vec += subobj->offset;
	return vec;
}

DAEInput::DAEInput(DAEInput&& other) : x(other.x), y(other.y), z(other.z), strides(other.strides), u(other.u), v(other.v), value(std::move(other.value)), valid(other.valid) {}

DAEInput& DAEInput::operator=(DAEInput&& other) {
	valid = other.valid;
	if (valid) {
		x = other.x;
		y = other.y;
		z = other.z;
		strides = other.strides;
		u = other.u;
		v = other.v;
		value = std::move(other.value);
	}
	return *this;
}

DAEInput::DAEInput(pugi::xml_node element) {
	valid = false;
	pugi::xml_node next = element.child("input");
	next = element.child("technique_common");
	if (!next) {
		return;
	}
	next = next.child("accessor");
	if (!next) {
		return;
	}
	strides = atoi(next.attribute("stride").value());

	// find coord order
	int found = 0;
	int uvs = 0;
	int i = 0;
	for (pugi::xml_node child = next.first_child(); child; child = child.next_sibling(), i++) {
		switch (get_name(child).c_str()[0]) {
			case 'X':
				x = i;
				found++;
				break;
			case 'Y':
				y = i;
				found++;
				break;
			case 'Z':
				z = i;
				found++;
				break;
			case 'S':
				u = i;
				uvs++;
				break;
			case 'T':
				v = i;
				uvs++;
				break;
			default:
				string error = "Unexpected axis '";
				error += get_name(child).c_str();
				error += "' found";
				//wxMessageBox(error.c_str());
				break;
		}
	}
	if (found != 3 && uvs != 2) {
		wxMessageBox(wxT("Incorrect number of coordinate axes found!"));
		exit(1);
	}
	next = element.find_child_by_attribute("id", &next.attribute("source").value()[1]);
	if (!next) {
		return;
	}
	if (next.attribute("count")) {
		value = parse_float_array(next.child_value(), atoi(next.attribute("count").value()));
	} else {
		value = parse_float_array(next.child_value());
	}
	valid = true;
}

bool DAEInput::is_valid() {
	return valid;
}

int DAEInput::stride() {
	assert(valid);
	return strides;
}

const vector<float>& DAEInput::values() {
	assert(valid);
	return value;
}

int DAEInput::x_offset() {
	assert(valid);
	return x;
}

int DAEInput::y_offset() {
	assert(valid);
	return y;
}

int DAEInput::z_offset() {
	assert(valid);
	return z;
}

int DAEInput::u_offset() {
	assert(valid);
	return u;
}

int DAEInput::v_offset() {
	assert(valid);
	return v;
}

DAEInputs::DAEInputs(pugi::xml_node& element) {
	int vert_offset;
	max_offset = 0;
	int uv_set_id = std::numeric_limits<int>::max();
	pugi::xml_node mesh = element.parent();
	for (pugi::xml_node poly_bit = element.first_child(); poly_bit; poly_bit = poly_bit.next_sibling()) {
		if (boost::algorithm::equals(poly_bit.attribute("semantic").value(), "VERTEX")) {
			vert_offset = atoi(poly_bit.attribute("offset").value());
			max_offset = max(max_offset,vert_offset);
			pugi::xml_node vertices = mesh.child("vertices");
			for (pugi::xml_node vertex = vertices.first_child(); vertex; vertex = vertex.next_sibling()) {
				if (boost::algorithm::equals(vertex.attribute("semantic").value(), "POSITION")) {
					pos_offset = vert_offset;
					pos = DAEInput(mesh.find_child_by_attribute("id", &vertex.attribute("source").value()[1]));
				} else if (boost::algorithm::equals(vertex.attribute("semantic").value(), "NORMAL")) {
					norm_offset = vert_offset;
					norm = DAEInput(mesh.find_child_by_attribute("id", &vertex.attribute("source").value()[1]));
				}
			}
		} else if (boost::algorithm::equals(poly_bit.attribute("semantic").value(), "NORMAL")) {
			norm_offset = atoi(poly_bit.attribute("offset").value());
			max_offset = max(max_offset,norm_offset);
			norm = DAEInput(mesh.find_child_by_attribute("id", &poly_bit.attribute("source").value()[1]));
		} else if (boost::algorithm::equals(poly_bit.attribute("semantic").value(), "TEXCOORD")) {
			int current_offset = atoi(poly_bit.attribute("offset").value());
			max_offset = max(max_offset, current_offset);
			if (poly_bit.attribute("set")) {
				int current_uv_set_id = atoi(poly_bit.attribute("set").value());
				if (current_uv_set_id >= uv_set_id) {
					continue;
				}
				uv_set_id = current_uv_set_id;
			}
			uv_offset = current_offset;
			uv = DAEInput(mesh.find_child_by_attribute("id", &poly_bit.attribute("source").value()[1]));
		} else if (!poly_bit.attribute("offset").empty()) {
			max_offset = max(max_offset,atoi(poly_bit.attribute("offset").value()));
		}
	}
	max_offset++;
}


vector3d DAEHandler::fix_axes(vector3d broken, matrix rotation) {
	broken = rotation * broken;

	if (up_axis == 1) {
		broken = vector3d(-broken.x,-broken.y,broken.z);
	} else if (up_axis == 2){
		broken = vector3d(-broken.x,broken.z,broken.y);
	} else {
		broken = vector3d(broken.y,-broken.x,broken.z);
	}
	if (mirror_x) broken.x *= -1;
	if (mirror_y) broken.y *= -1;
	if (mirror_z) broken.z *= -1;
	return broken;
}

matrix DAEHandler::get_rotation(const pugi::xml_node& element, matrix old) {
	matrix rot;
	matrix base;
	for (pugi::xml_node child = element.first_child(); child; child = child.next_sibling()) {
		if (boost::algorithm::equals(child.name(), "rotate")) {
			auto temp = parse_float_array(child.child_value(), 4);
			rot = matrix(temp[3]);
			base = matrix(vector3d(temp[0], temp[1], temp[2]));
			old = old % base.invert() % rot % base;
		} else if (boost::algorithm::equals(child.name(), "scale")) {
			auto temp = parse_float_array(child.child_value(), 3);
			base = matrix();
			for (int j = 0; j < 3; j++) {
				base.a2d[j][j] = temp[j];
			}
			old = old % base;
		} else if (boost::algorithm::equals(child.name(), "matrix")) {
			auto temp = parse_float_array(child.child_value(), 16);
			rot = matrix(&temp);
			old = old % rot;
		}
	}
	return old;
}

pcs_slot DAEHandler::process_gunbank(pugi::xml_node& element, int type) {
	pcs_slot bank;
	bank.type = type;
	pcs_hardpoint temp;
	matrix rot = get_rotation(element);
	vector3d translation = get_translation(element);
	std::multimap<std::string, pcs_hardpoint> ordered_gunpoints;

	for (pugi::xml_node helper = element.first_child(); helper; helper = helper.next_sibling()) {
		if (boost::algorithm::equals(helper.name(), "node")) {
			temp.norm = fix_axes(up, get_rotation(helper, rot));
			temp.point = get_translation(helper, rot) + translation;
			ordered_gunpoints.insert(make_pair(get_name(helper), temp));
		}
	}
	for (std::multimap<string, pcs_hardpoint>::iterator it = ordered_gunpoints.begin(); it != ordered_gunpoints.end(); ++it) {
		bank.muzzles.push_back(it->second);
	}
	return bank;
}

pcs_eye_pos DAEHandler::process_eyepoint(pugi::xml_node& helper, matrix transform, int subobj_idx) {
	pcs_eye_pos result;
	result.normal = fix_axes(up, get_rotation(helper, transform));
	result.sobj_offset = get_translation(helper, transform);
	result.sobj_number = subobj_idx;
	return result;
}

DAESaver::DAESaver(string name, PCS_Model *model, int helpers, int props_as_helpers, AsyncProgress* progress) {
	this->model = model;
	this->filename = name;
	root = doc.append_child("COLLADA");
	for (int i = 0; i < model->GetSOBJCount(); i++) {
		subobjs.push_back(pugi::xml_node());
		model_subobjs.push_back(&model->SOBJ(i));
	}
	this->export_helpers = helpers;
	this->props_as_helpers = props_as_helpers;
	this->progress = progress;
	
}

int DAESaver::save(void) {
	if (export_helpers) {
		progress->setTarget(subobjs.size() + 18);
	} else {
		progress->setTarget(subobjs.size() + 4);
	}
		
	progress->incrementWithMessage( "Adding header");
	add_header();
	progress->incrementWithMessage( "Adding textures");
	add_textures();
	progress->incrementWithMessage( "Adding geometry");
	add_geom();
	if (export_helpers) {
		progress->incrementWithMessage( "Adding specials");
		add_subsystems();
		progress->incrementWithMessage( "Adding turrets");
		add_turret_fps();
		progress->incrementWithMessage( "Adding dockpoints");
		add_docks();
		progress->incrementWithMessage( "Adding thrusters");
		add_thrusters();
		progress->incrementWithMessage( "Adding gunpoints");
		add_guns();
		progress->incrementWithMessage( "Adding eyepoints");
		add_eyes();
		progress->incrementWithMessage( "Adding shield");
		add_shield();
		progress->incrementWithMessage( "Adding paths");
		add_paths();
		progress->incrementWithMessage( "Adding glowpoints");
		add_glows();
		progress->incrementWithMessage( "Adding insignia");
		add_insignia();
		for (int i = 0; i < model->GetSOBJCount(); i++) {
			add_sobj_helpers(i);
		}
		progress->incrementWithMessage( "Adding mass");
		add_mass();
		progress->incrementWithMessage( "Adding moment of inertia");
		add_moment_of_inertia();
		progress->incrementWithMessage( "Adding center of mass");
		add_center_of_mass();
		progress->incrementWithMessage( "Adding autocentering");
		add_autocentering();
	}

	for (size_t i = 0; i < helpers.size(); i++) {
		if (!helpers[i].first_child()) {
			helpers[i].parent().remove_child(helpers[i]);
		}
	}

	pugi::xml_node scene = root.append_child("scene").append_child("instance_visual_scene");
	scene.append_attribute("url") = "#Scene";

	progress->incrementWithMessage( "Saving DAE");
	doc.save_file(filename.c_str());
	return 0;
}

void DAESaver::add_header() {
	root.append_attribute("version") = "1.4.1";
	root.append_attribute("xmlns") = "http://www.collada.org/2005/11/COLLADASchema";
	pugi::xml_node asset = root.append_child("asset");
	asset.append_child("contributor");
	time_t now = time(NULL);
	struct tm* tm = gmtime(&now);
	char time_string[20];
	strftime(time_string, sizeof(time_string), "%Y-%m-%dT%H:%M:%S", tm);
	pugi::xml_node created = asset.append_child("created");
	pugi::xml_node modified = asset.append_child("modified");
	created.text().set(time_string);
	modified.text().set(time_string);
	pugi::xml_node up = asset.append_child("up_axis");
	up.text().set("Z_UP");
}

void DAESaver::add_textures() {
	materials = root.append_child("library_materials");
	pugi::xml_node images = root.append_child("library_images");
	pugi::xml_node effects = root.append_child("library_effects");
	pugi::xml_node current_image;
	pugi::xml_node current_init;
	pugi::xml_node current_effect;
	pugi::xml_node sampler;
	pugi::xml_node surface;
	pugi::xml_node shader;
	string temp;
	num_textures = model->GetTexturesCount();
	
	for (int i = 0; i < model->GetTexturesCount(); i++) {
		temp = model->Texture(i);
		current_image = images.append_child("image");
		current_init = current_image.append_child("init_from");
		current_init.text().set((temp + ".tga").c_str());
		temp += "-img";
		current_image.append_attribute("id") = temp.c_str();
		current_image.append_attribute("name") = temp.c_str();
		current_effect = effects.append_child("effect");
		temp = model->Texture(i);
		temp += "-fx";
		current_effect.append_attribute("id") = temp.c_str();
		current_effect.append_attribute("name") = temp.c_str();
		current_effect = current_effect.append_child("profile_COMMON");
		temp = model->Texture(i);
		surface = current_effect.append_child("newparam");
		surface.append_attribute("sid") = (string(model->Texture(i)) + "-surface").c_str();
		surface = surface.append_child("surface");
		surface.append_attribute("type") = "2D";
		surface = surface.append_child("init_from");
		surface.text().set((string(model->Texture(i)) + "-img").c_str());
		sampler = current_effect.append_child("newparam");
		sampler.append_attribute("sid") = (string(model->Texture(i)) + "-sampler").c_str();
		sampler = sampler.append_child("sampler2D").append_child("source");
		sampler.text().set((string(model->Texture(i)) + "-surface").c_str());
		shader = current_effect.append_child("technique");
		shader.append_attribute("sid");
		shader = shader.append_child("phong");
		shader = shader.append_child("diffuse");
		shader = shader.append_child("texture");
		shader.append_attribute("texcoord") = "CHANNEL1";
		shader.append_attribute("texture") = (string(model->Texture(i)) + "-sampler").c_str();

		pugi::xml_node mat = materials.append_child("material");
		temp = string(model->Texture(i).c_str()) + "-mat";
		mat.append_attribute("id") = temp.c_str();
		mat.append_attribute("name") = temp.c_str();
		pugi::xml_node effect = mat.append_child("instance_effect");
		effect.append_attribute("url") = (string("#") + model->Texture(i) + "-fx").c_str();
	}
}

void DAESaver::add_geom() {
	geoms = root.append_child("library_geometries");
	scene = root.append_child("library_visual_scenes").append_child("visual_scene");
	scene.append_attribute("id") = "Scene";
	scene.append_attribute("name") = "Scene";
	string temp("detail0");
	for (int i = 0; i < model->GetLODCount(); i++) {
		temp[strlen("detail")] = i + '0';
		get_subobj(model->LOD(i),&temp);
	}
	// better fix debris names too
	temp = "debris01";
	for (int i = 0; i < model->GetDebrisCount(); i++) {
		temp[strlen("debris0")] = ((i + 1) % 10) + '0';
		temp[strlen("debris")] = ((i + 1) / 10) + '0';
		get_subobj(model->Debris(i),&temp);
	}
	for (int i = 0; i < model->GetSOBJCount(); i++) {
		get_subobj(i);
	}
}

void DAESaver::get_subobj(int idx,string *name) {
	if (subobjs[idx]) return;
	pugi::xml_node subobj;
	pcs_sobj& sobj = model->SOBJ(idx);
	if (sobj.parent_sobj == -1) {
		subobj = scene.append_child("node");
	} else {
		if (subobjs[sobj.parent_sobj]) {
			subobj = subobjs[sobj.parent_sobj].append_child("node");
		} else {
			return;
		}
	}
	subobjs[idx] = subobj;
	pugi::xml_node translate = subobj.prepend_child("translate");
	translate.text().set(write_vector3d(sobj.offset).c_str());
	if (name) {
		progress->incrementWithMessage("Adding " + *name);
		subobj.append_attribute("id") = name->c_str();
		subobj.append_attribute("name") = name->c_str();
	} else {
		subobj.append_attribute("id") = sobj.name.c_str();
		subobj.append_attribute("name") = sobj.name.c_str();
		progress->incrementWithMessage("Adding " + sobj.name);
	}
	
	// split up polies by texture...
	vector<vector<pcs_polygon*> > polies;
	polies.resize(num_textures + 1);
	vector<unsigned int> counters;
	counters.resize(num_textures + 1, 0);
	for (unsigned int i = 0; i < sobj.polygons.size(); i++) {
		if (sobj.polygons[i].texture_id < num_textures && sobj.polygons[i].texture_id >= 0) {
			polies[sobj.polygons[i].texture_id].push_back(&(sobj.polygons[i]));
			counters[sobj.polygons[i].texture_id]++;
		} else {
			polies[num_textures].push_back(&(sobj.polygons[i]));
			counters[num_textures]++;
		}
	}
	for (int i = 0; i <= num_textures; i++) {
		if (polies[i].size() != counters[i]) {
			polies[i].resize(counters[i]);
		}
	}
	pugi::xml_node current_group;
	pugi::xml_node temp;
	stringstream group_name;
	temp = subobj.insert_child_after("instance_geometry", translate);
	temp.append_child("bind_material").append_child("technique_common");
	temp.append_attribute("url") = (string("#") + get_name(subobj).c_str() + "-geometry").c_str();
	group_name << get_name(subobj).c_str() << "-geometry";
	current_group = get_polygroups(polies,string(group_name.str().c_str()),subobj);

}

void DAESaver::add_sobj_helpers(int idx) {
	pcs_sobj& sobj = model->SOBJ(idx);
	pugi::xml_node subobj = subobjs[idx];
	pugi::xml_node helper = add_helper(subobj,sobj.properties);
	if (sobj.movement_type == 1 && sobj.movement_axis != ANONE) {
		vector3d direction;
		double length = 1;
		size_t rotate_offset = sobj.properties.find("$rotate=");
		if (rotate_offset != string::npos) {
			sscanf(sobj.properties.c_str() + rotate_offset, "$rotate=%lf", &length);
		}
		switch(sobj.movement_axis) {
			case MV_X:
				direction = vector3d(1,0,0);
				break;
			case MV_Y:
				direction = vector3d(0,1,0);
				break;
			case MV_Z:
				direction = vector3d(0,0,1);
				break;
		}
		pugi::xml_node rotation = helper.append_child("node");
		if (rotate_offset != string::npos) {
			rotation.append_attribute("id") = "rotate";
			rotation.append_attribute("name") = "rotate";
		} else {
			rotation.append_attribute("id") = "rotate-nospeed";
			rotation.append_attribute("name") = "rotate-nospeed";
		}
		write_transform(rotation, vector3d(), direction, vector3d(0,1,0), length);
	}
	size_t uvec_offset, fvec_offset;
	uvec_offset = sobj.properties.find("$uvec");
	fvec_offset = sobj.properties.find("$fvec");
	if (uvec_offset != string::npos && fvec_offset != string::npos) {
		double x = 0;
		double y = 0;
		double z = 0;
		vector3d fvec, uvec;
		sscanf(sobj.properties.c_str() + fvec_offset, "$fvec:%lf,%lf,%lf", &x, &y, &z);
		fvec = vector3d(-x, y, z);
		sscanf(sobj.properties.c_str() + uvec_offset, "$uvec:%lf,%lf,%lf", &x, &y, &z);
		uvec = vector3d(-x, y, z);
		if (!helper) {
			helper = subobj.append_child("node");
			helper.append_attribute("id") = "helper";
			helper.append_attribute("name") = "helper";
		}
		pugi::xml_node vec = helper.append_child("node");
		vec.append_attribute("id") = "vec";
		vec.append_attribute("name") = "vec";
		write_transform_binormal(vec, vector3d(), fvec, uvec, vector3d(0,0,1));
	}
}

pugi::xml_node DAESaver::get_polygroups(vector <vector <pcs_polygon*> > polies, string name,pugi::xml_node& node) {
	pugi::xml_node result = geoms.append_child("geometry");
	result.append_attribute("id") = name.c_str();
	result.append_attribute("name") = name.c_str();
	pugi::xml_node mesh = result.append_child("mesh");
	vector<float> vert,norm,uv;
	vector<int> ref, sizes;
	unordered_map<vector3d, int> vert_map;
	unordered_map<vector3d, int> norm_map;
	unordered_map<pair<float, float>, int> uv_map;
	unsigned int vert_idx = 0, norm_idx = 0, uv_idx = 0;

	for (unsigned int i = 0; i < polies.size(); i++) {
		if (polies[i].size() > 0) {
			ref.resize(0);
			sizes.resize(0);
			int count = 0;
			sizes.resize(polies[i].size());
			for (unsigned int j = 0; j < polies[i].size(); j++) {
				sizes[j] = polies[i][j]->verts.size();
				for (int k = polies[i][j]->verts.size() - 1; k >= 0; k--) {
					if (vert_map.find(polies[i][j]->verts[k].point) == vert_map.end()) {
						vert_map.insert(make_pair(polies[i][j]->verts[k].point, vert_map.size()));
						vert.push_back(-polies[i][j]->verts[k].point.x);
						vert.push_back(polies[i][j]->verts[k].point.z);
						vert.push_back(polies[i][j]->verts[k].point.y);
						vert_idx += 3;
					}
					if (norm_map.find(polies[i][j]->verts[k].norm) == norm_map.end()) {
						norm_map.insert(make_pair(polies[i][j]->verts[k].norm, norm_map.size()));
						norm.push_back(-polies[i][j]->verts[k].norm.x);
						norm.push_back(polies[i][j]->verts[k].norm.z);
						norm.push_back(polies[i][j]->verts[k].norm.y);
						norm_idx += 3;

					}
					if (uv_map.find(make_pair(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v)) == uv_map.end()) {
						uv_map.insert(make_pair(make_pair(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v), uv_map.size()));
						uv.push_back(polies[i][j]->verts[k].u);
						uv.push_back(1.0f - polies[i][j]->verts[k].v);
						uv_idx += 2;
					}
					count += 3;
				}
			}
			vert.resize(vert_idx);
			norm.resize(norm_idx);
			uv.resize(uv_idx);
			ref.resize(count);
			count = 0;
			for (unsigned int j = 0; j < polies[i].size(); j++) {
				for (int k = polies[i][j]->verts.size() - 1; k >= 0; k--) {
					ref[count] = vert_map[polies[i][j]->verts[k].point];
					ref[count + 1] = norm_map[polies[i][j]->verts[k].norm];
					ref[count + 2] = uv_map[make_pair(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v)];
					count += 3;
				}
			}
			add_refs(mesh,name,ref,sizes,&node,i);
		}
	}
	pugi::xml_node pos = mesh.prepend_child("source");
	pos.append_attribute("id") = (name + "-position").c_str();
	pugi::xml_node pos_float_array = pos.append_child("float_array");
	pos_float_array.append_attribute("id") = (name + "-position-array").c_str();
	pos_float_array.text().set(write_float_array(vert).c_str());
	pos_float_array.append_attribute("count") = std::to_string(vert.size()).c_str();
	add_accessor(pos,name + "-position-array",vert.size()/3);

	pugi::xml_node norms = mesh.insert_child_after("source", pos);
	norms.append_attribute("id") = (name + "-normals").c_str();
	pugi::xml_node norms_float_array = norms.append_child("float_array");
	norms_float_array.append_attribute("id") = (name + "-normals-array").c_str();
	norms_float_array.text().set(write_float_array(norm).c_str());
	norms_float_array.append_attribute("count") = std::to_string(norm.size()).c_str();
	add_accessor(norms,name + "-normals-array",norm.size()/3);

	pugi::xml_node verts = mesh.insert_child_after("vertices", norms);
	verts.append_attribute("id") = (name + "-vertex").c_str();
	pugi::xml_node vert_input = verts.append_child("input");
	vert_input.append_attribute("semantic") =  "POSITION";
	vert_input.append_attribute("source") =  (string("#") + name + "-position").c_str();
	pugi::xml_node uvs = mesh.insert_child_after("source", norms);
	uvs.append_attribute("id") = (name + "-uv").c_str();
	pugi::xml_node uvs_float_array = uvs.append_child("float_array");
	uvs_float_array.append_attribute("id") = (name + "-uv-array").c_str();
	uvs_float_array.text().set(write_float_array(uv).c_str());
	uvs_float_array.append_attribute("count") = std::to_string(uv.size()).c_str();
	add_accessor(uvs,name + "-uv-array",uv.size()/2,true);

	return result;
}

void DAESaver::add_refs(pugi::xml_node& mesh, string name, vector<int> refs, vector<int> sizes, pugi::xml_node* node, int texture) {
	pugi::xml_node polylist;
	bool tri = true;
	for (unsigned int i = 0; i < sizes.size(); i++) {
		if (sizes[i] != 3) {
			tri = false;
			break;
		}
	}
	if (tri) {
		polylist = mesh.append_child("triangles");
	} else {
		polylist = mesh.append_child("polylist");
	}
	if (texture > -1 && texture < num_textures) {
		polylist.append_attribute("material") = add_material(texture,*node).c_str();
	}
	pugi::xml_node vert, norm, uv;
	vert = polylist.append_child("input");
	vert.append_attribute("offset") = "0";
	vert.append_attribute("semantic") = "VERTEX";
	vert.append_attribute("source") =  (string("#") + name + "-vertex").c_str();
	if (texture < num_textures + 1) {
		norm = polylist.append_child("input");
		norm.append_attribute("offset") = "1";
		norm.append_attribute("semantic") = "NORMAL";
		norm.append_attribute("source") =  (string("#") + name + "-normals").c_str();
	}
	if (texture > -1) {
		uv = polylist.append_child("input");
		if (texture < num_textures + 1) {
			uv.append_attribute("offset") = "2";
		} else {
			uv.append_attribute("offset") = "1";
		}
		uv.append_attribute("semantic") = "TEXCOORD";
		uv.append_attribute("source") =  (string("#") + name + "-uv").c_str();
	}
	if (!tri) {
		pugi::xml_node vcount = polylist.append_child("vcount");
		vcount.text().set(write_int_array(sizes).c_str());
	}

	pugi::xml_node p = polylist.append_child("p");
	p.text().set(write_int_array(refs).c_str());
	polylist.append_attribute("count") = std::to_string(sizes.size()).c_str();
}

void DAESaver::add_turret_fps() {
	pugi::xml_node element;
	stringstream name;
	pcs_turret *turret;
	pugi::xml_node helper;
	for (int i = 0; i < model->GetTurretCount(); i++) {
		turret = &model->Turret(i);
		helper = find_helper(subobjs[turret->sobj_par_phys]);
		if (!helper) {
			helper = scene;
		}
		helper = helper.prepend_child("node");
		name.str("");
		if (turret->sobj_parent == turret->sobj_par_phys) {
			name << "firepoints" << i;
		} else {
			name << "multifirepoints" << i;
		}
		helper.append_attribute("id") = name.str().c_str();
		helper.append_attribute("name") = name.str().c_str();
		for (unsigned int j = 0; j < turret->fire_points.size(); j++) {
			name.str("");
			name << model_subobjs[0][turret->sobj_parent].name.c_str() << "-" << setfill ('0') << setw (2) << (j + 1);
			element = helper.append_child("node");
			element.append_attribute("id") = name.str().c_str();
			element.append_attribute("name") = name.str().c_str();
			write_transform(element, turret->fire_points[j], turret->turret_normal, vector3d(0,1,0));
		}
	}
}

void DAESaver::add_docks() {
	pugi::xml_node element;
	pugi::xml_node group;
	stringstream name;
	pcs_dock_point *dockpoint;
	for (int i = 0; i < model->GetDockingCount(); i++) {
		dockpoint = &model->Dock(i);
		name.str("");
		name << "dockpoint" << setfill ('0') << setw (2) << (i+1);
		group = scene.append_child("node");
		group.append_attribute("id") = name.str().c_str();
		group.append_attribute("name") = name.str().c_str();
		docks.push_back(group);

		for (unsigned int j = 0; j < dockpoint->dockpoints.size(); j++) {
			name.str("");
			name << "dockpoint" << setfill ('0') << setw (2) << (i+1) << "-" << setfill ('0') << setw (2) << (j+1);
			element = group.append_child("node");
			element.append_attribute("id") = name.str().c_str();
			element.append_attribute("name") = name.str().c_str();
			write_transform(element,dockpoint->dockpoints[j].point,dockpoint->dockpoints[j].norm,vector3d(0,1,0));
		}
		add_helper(group,dockpoint->properties);
	}
}

void DAESaver::add_thrusters() {
	pugi::xml_node element;
	stringstream name;
	pcs_thruster *thruster;
	pugi::xml_node helper;
	string engine_name;
	vector3d offset,scale_vec;
	for (int i = 0; i < model->GetThrusterCount(); i++) {
		helper = pugi::xml_node();
		thruster = &model->Thruster(i);
		engine_name = thruster->properties;
		if (!engine_name.empty() && boost::algorithm::istarts_with(engine_name, "$engine_subsystem=")) {
			engine_name = engine_name.substr(engine_name.find("="));
			helper = find_subsystem(engine_name,offset,scale_vec);
		}
		if (!helper) {
			helper = scene;
			scale_vec = vector3d(1,1,1);
		}
		helper = helper.append_child("node");
		helper.append_attribute("id") = "thrusters";
		helper.append_attribute("name") = "thrusters";


		for (unsigned int j = 0; j < thruster->points.size(); j++) {
			element = helper.append_child("node");
			element.append_attribute("id") = "thruster";
			element.append_attribute("name") = "thruster";
			write_transform(element, thruster->points[j].pos - offset, MakeUnitVector(thruster->points[j].norm), vector3d(0,0,1), thruster->points[j].radius, scale_vec.x);
		}
	}
}

void DAESaver::add_guns() {
	pugi::xml_node element;
	stringstream name;
	pcs_slot *gunbank;
	pugi::xml_node group;
	int guns = 0;
	int missiles = 0;

	for (int i = 0; i < model->GetWeaponCount(); i++) {
		gunbank = &model->Weapon(i);
		if (gunbank->type == GUN) {
			guns++;
		} else if (gunbank->type == MISSILE) {
			missiles++;
		}
		name.str("");
		if (gunbank->type == GUN) {
			name << "gunbank" << setfill ('0') << setw (2) << (guns);
		} else {
			name << "missilebank" << setfill ('0') << setw (2) << (missiles);
		}
		group = scene.append_child("node");
		group.append_attribute("id") = name.str().c_str();
		group.append_attribute("name") = name.str().c_str();
		for (unsigned int j = 0; j < gunbank->muzzles.size(); j++) {
			name.str("");
			if (gunbank->type == GUN) {
				name << "gunbank" << setfill ('0') << setw (2) << (guns);
			} else {
				name << "missilebank" << setfill ('0') << setw (2) << (missiles);
			}
			name << "-" << setfill ('0') << setw (2) << (j+1);
			element = group.append_child("node");
			element.append_attribute("id") = name.str().c_str();
			element.append_attribute("name") = name.str().c_str();
			write_transform(element,gunbank->muzzles[j].point,gunbank->muzzles[j].norm,vector3d(0,1,0));
		}
	}
}

void DAESaver::add_eyes() {
	for (int i = 0; i < model->GetEyeCount(); i++) {
		pcs_eye_pos& eye = model->Eye(i);
		stringstream name;
		name << "eyepoint" << setfill ('0') << setw (2) << (i+1);
		pugi::xml_node element;
		if (eye.sobj_number == -1) {
			element = scene.append_child("node");
		} else {
			element = find_helper(subobjs[eye.sobj_number]).append_child("node");
		}
		element.append_attribute("id") = name.str().c_str();
		element.append_attribute("name") = name.str().c_str();
		write_transform(element,eye.sobj_offset,eye.normal,vector3d(0,1,0));
	}
}

void DAESaver::add_subsystems() {
	pugi::xml_node element;
	pugi::xml_node translate;
	stringstream name;
	pcs_special *subsys;
	vector3d scale_vec;
	for (int i = 0; i < model->GetSpecialCount(); i++) {
		subsys = &model->Special(i);
		name.str("");
		if (subsys->properties.find("$special=subsystem") != string::npos) {
			name << "subsystem";
		} else {
			name << "special";
		}
	    name << (subsys->name.c_str() + 1);
		element = scene.append_child("node");
		element.append_attribute("id") = name.str().c_str();
		element.append_attribute("name") = name.str().c_str();
		translate = element.append_child("translate");
		translate.append_attribute("sid") = "translate";
		translate.text().set(write_vector3d(subsys->point).c_str());
		add_scale(element,radius_to_scale(subsys->radius));
		specials.push_back(element);
		add_helper(element,subsys->properties);
	}
}

void DAESaver::add_shield() {
	if (model->GetShldTriCount() == 0) {
		return;
	}
	unordered_map<vector3d, int> vert_map;
	unordered_map<vector3d, int> norm_map;
	int vert_idx = 0,norm_idx = 0;
	pugi::xml_node element = scene.append_child("node");
	element.append_attribute("id") = "shield";
	element.append_attribute("name") = "shield";
	pugi::xml_node temp = element.append_child("instance_geometry");
	temp.append_attribute("url") = "#shield-geometry";
	pugi::xml_node geom = geoms.append_child("geometry");
	geom.append_attribute("id") = "shield-geometry";
	geom.append_attribute("name") = "shield-geometry";
	vector<float> vert,norm;
	vector<int> ref,size;
	pcs_shield_triangle tri;
	for (int i = 0; i < model->GetShldTriCount(); i++) {
		tri = model->ShldTri(i);
		if (norm_map.find(tri.face_normal) == norm_map.end()) {
			norm_map.insert(make_pair(tri.face_normal, norm_map.size()));
			norm.push_back(-tri.face_normal.x);
			norm.push_back(tri.face_normal.z);
			norm.push_back(tri.face_normal.y);
		}
		norm_idx = norm_map[tri.face_normal];

		for (int j = 2; j >= 0; j--) {
			if (vert_map.find(tri.corners[j]) == vert_map.end()) {
				vert_map.insert(make_pair(tri.corners[j], vert_map.size()));
				vert.push_back(-tri.corners[j].x);
				vert.push_back(tri.corners[j].z);
				vert.push_back(tri.corners[j].y);
			}
			vert_idx = vert_map[tri.corners[j]];
			ref.push_back(vert_idx);
			ref.push_back(norm_idx);
		}
		size.push_back(3);
	}
	pugi::xml_node mesh = geom.append_child("mesh");
	add_refs(mesh,"shield",ref,size);
	pugi::xml_node pos = mesh.prepend_child("source");
	pos.append_attribute("id") = "shield-position";
	pugi::xml_node pos_float_array = pos.append_child("float_array");
	pos_float_array.append_attribute("id") = "shield-position-array";
	pos_float_array.text().set(write_float_array(vert).c_str());
	pos_float_array.append_attribute("count") = std::to_string(vert.size()).c_str();
	add_accessor(pos,"shield-position-array",vert.size()/3);

	pugi::xml_node norms = mesh.insert_child_after("source", pos);
	norms.append_attribute("id") = "shield-normals";
	pugi::xml_node norms_float_array = norms.append_child("float_array");
	norms_float_array.append_attribute("id") = "shield-normals-array";
	norms_float_array.text().set(write_float_array(norm).c_str());
	norms_float_array.append_attribute("count") = std::to_string(norm.size()).c_str();
	add_accessor(norms,"shield-normals-array",norm.size()/3);

	pugi::xml_node verts = mesh.insert_child_after("vertices", norms);
	verts.append_attribute("id") = "shield-vertex";
	pugi::xml_node vert_input = verts.append_child("input");
	vert_input.append_attribute("semantic") =  "POSITION";
	vert_input.append_attribute("source") =  "#shield-position";
}

void DAESaver::add_paths() {
	pcs_path *path;
	pugi::xml_node element;
	pugi::xml_node translate;
	pugi::xml_node helper;
	stringstream name;
	vector3d offset(0,0,0);
	vector3d scale_vec;
	for (int i = 0; i < model->GetPathCount(); i++) {
		path = &model->Path(i);
		helper = find_subsystem(path->parent,offset,scale_vec);
		if (!helper) {
			helper = find_dockpoint(i,offset);
		}
		if (!helper) {
			helper = scene;
		}
		name.str("");
		name << (path->name.c_str() + 1);
		helper = helper.append_child("node");
		helper.append_attribute("id") = name.str().c_str();
		helper.append_attribute("name") = name.str().c_str();
		for (unsigned int j = 0; j < path->verts.size(); j++) {
			name.str("");
			name << (path->name.c_str() + 1) << "-" << setw(2) << setfill('0') << (j+1);
			element = helper.append_child("node");
			element.append_attribute("id") = name.str().c_str();
			element.append_attribute("name") = name.str().c_str();
			translate = element.append_child("translate");
			translate.append_attribute("sid") = "translate";
			translate.text().set(write_vector3d(path->verts[j].pos - offset,scale_vec).c_str());
			add_scale(element,radius_to_scale(path->verts[j].radius), scale_vec);
		}
	}
}

void DAESaver::add_glows() {
	pcs_glow_array glows;
	pcs_thrust_glow glow;
	string name;
	pugi::xml_node element;
	pugi::xml_node glowpoint;
	pugi::xml_node trans;
	
	for (int i = 0; i < model->GetLightCount(); i++) {
		glows = model->Light(i);
		element = find_helper(subobjs[glows.obj_parent]).append_child("node");
		element.append_attribute("id") = "glowbank";
		element.append_attribute("name") = "glowbank";
		std::stringstream fields;
		fields << glows.properties;
		fields << endl << "type=" << glows.type;
		fields << endl << "on=" << glows.on_time;
		fields << endl << "off=" << glows.off_time;
		fields << endl << "displacement=" << glows.disp_time;
		fields << endl << "lod=" << glows.LOD;
		for (unsigned int j = 0; j < glows.lights.size(); j++) {
			glow = glows.lights[j];
			glowpoint = element.append_child("node");
			glowpoint.append_attribute("id") = "glowpoint";
			glowpoint.append_attribute("name") = "glowpoint";
			add_scale(glowpoint,radius_to_scale(glow.radius));
			trans = glowpoint.append_child("translate");
			trans.append_attribute("sid") = "translate";
			trans.text().set(write_vector3d(glow.pos).c_str());
		}
		add_helper(element, fields.str());
	}
}

void DAESaver::add_insignia() {
	unordered_map<vector3d, int> vert_map;
	unordered_map<pair<float, float>, int> uv_map;
	int vert_idx = 0,uv_idx = 0;
	vector<int>counts(model->GetLODCount(), 1);

	pcs_insig insignia;
	stringstream name;
	pugi::xml_node element;
	pugi::xml_node translate;
	pcs_insig_face face;
	for (int i = 0; i < model->GetInsigniaCount(); i++) {
		insignia = model->Insignia(i);
		name.str("");
		name << "insigLOD" << setfill ('0') << setw (2) << insignia.lod + 1 << "-" << setfill ('0') << setw (2) << counts[insignia.lod];
		counts[insignia.lod]++;
		element = scene.append_child("node");
		element.append_attribute("id") =  name.str().c_str();
		element.append_attribute("name") =  name.str().c_str();
		translate = element.append_child("translate");
		translate.text().set(write_vector3d(insignia.offset).c_str());


		pugi::xml_node temp = element.append_child("instance_geometry");
		temp.append_attribute("url") = (string("#") + name.str().c_str() + "-geometry").c_str();
		pugi::xml_node geom = geoms.append_child("geometry");
		geom.append_attribute("id") = (name.str() + "-geometry").c_str();
		geom.append_attribute("name") = (name.str() + "-geometry").c_str();
		vector<float> vert, uv;
		vector<int> ref,size;
		vert_map.clear();
		uv_map.clear();
		for (unsigned int i = 0; i < insignia.faces.size(); i++) {
			face = insignia.faces[i];
			for (int j = 2; j >= 0; j--) {
				if (vert_map.find(face.verts[j]) == vert_map.end()) {
					vert_map.insert(make_pair(face.verts[j], vert_map.size()));
					vert.push_back(-face.verts[j].x);
					vert.push_back(face.verts[j].z);
					vert.push_back(face.verts[j].y);
				}
				vert_idx = vert_map[face.verts[j]];
				if (uv_map.find(make_pair(face.u[j],face.v[j])) == uv_map.end()) {
					uv_map.insert(make_pair(make_pair(face.u[j],face.v[j]), uv_map.size()));
					uv.push_back(face.u[j]);
					uv.push_back(1.0 - face.v[j]);
				}
				uv_idx = uv_map[make_pair(face.u[j],face.v[j])];



				ref.push_back(vert_idx);
				ref.push_back(uv_idx);
			}
			size.push_back(3);
		}
		pugi::xml_node mesh = geom.append_child("mesh");
		add_refs(mesh,name.str().c_str(),ref,size,NULL, num_textures + 1);
		pugi::xml_node pos = mesh.prepend_child("source");
		pos.append_attribute("id") = (name.str() + "-position").c_str();
		pugi::xml_node pos_float_array = pos.append_child("float_array");
		pos_float_array.append_attribute("id") = (name.str() + "-position-array").c_str();
		pos_float_array.text().set(write_float_array(vert).c_str());
		pos_float_array.append_attribute("count") = std::to_string(vert.size()).c_str();
		add_accessor(pos,(name.str() + "-position-array").c_str(),vert.size()/3);

		pugi::xml_node uvs = mesh.insert_child_after("source", pos);
		uvs.append_attribute("id") = (name.str() + "-uv").c_str();
		pugi::xml_node uvs_float_array = uvs.append_child("float_array");
		uvs_float_array.append_attribute("id") = (name.str() + "-uv-array").c_str();
		uvs_float_array.text().set(write_float_array(uv).c_str());
		uvs_float_array.append_attribute("count") = std::to_string(uv.size()).c_str();
		add_accessor(uvs,(name.str() + "-uv-array").c_str(),uv.size()/2,true);

		pugi::xml_node verts = mesh.insert_child_after("vertices", uvs);
		verts.append_attribute("id") = (name.str() + "-vertex").c_str();
		pugi::xml_node vert_input = verts.append_child("input");
		vert_input.append_attribute("semantic") =  "POSITION";
		vert_input.append_attribute("source") =  (string("#") + name.str().c_str() + "-position").c_str();
	}
}

void DAESaver::add_center_of_mass() {
	const vector3d& com = model->GetCenterOfMass();
	if (com == vector3d())
		return;
	pugi::xml_node center_of_mass = scene.append_child("node");
	if (center_of_mass) {
		center_of_mass.append_attribute("id") =  "com";
		center_of_mass.append_attribute("name") =  "com";
		pugi::xml_node translate = center_of_mass.append_child("translate");
		if (translate) {
			translate.append_attribute("sid") = "translate";
			translate.text().set(write_vector3d(com).c_str());
		}
	}
}

void DAESaver::add_autocentering() {
	const vector3d& acen = model->GetAutoCenter();
	if (acen == vector3d())
		return;
	pugi::xml_node autocentering = scene.append_child("node");
	if (autocentering) {
		autocentering.append_attribute("id") =  "acen";
		autocentering.append_attribute("name") =  "acen";
		pugi::xml_node translate = autocentering.append_child("translate");
		if (translate) {
			translate.append_attribute("sid") = "translate";
			translate.text().set(write_vector3d(acen).c_str());
		}
	}
}

void DAESaver::add_mass() {
	pugi::xml_node mass = scene.append_child("node");
	if (mass) {
		mass.append_attribute("id") =  "mass";
		mass.append_attribute("name") =  "mass";
		mass = mass.append_child("node");
		if (mass) {
			stringstream stream;
			stream << model->GetMass();
			mass.append_attribute("id") =  stream.str().c_str();
			mass.append_attribute("name") =  stream.str().c_str();
		}
	}
}

void DAESaver::add_moment_of_inertia() {
	std::vector<float> numbers(9, 0);
	model->GetMOI(numbers);
	std::stringstream stream;
	bool calculated = false;
	for (int i = 0; i < 9; i++) {
		stream << numbers[i] << ",";
		if (numbers[i] != 0) {
			calculated = true;
		}
	}
	// let's not bother saving a heap of zeroes.
	if (calculated) {
		pugi::xml_node moi = scene.append_child("node");
		if (moi) {
			moi.append_attribute("id") =  "moi";
			moi.append_attribute("name") =  "moi";
			moi = moi.append_child("node");
			if (moi) {
				moi.append_attribute("id") =  stream.str().c_str();
				moi.append_attribute("name") =  stream.str().c_str();
			}
		}
	}
}

string DAESaver::add_material(int idx,pugi::xml_node& node) {

	pugi::xml_node instance = find_node(node, "technique_common");
	instance = instance.append_child("instance_material");
	instance.append_attribute("symbol") = (model->Texture(idx) + "-mat").c_str();
	instance.append_attribute("target") = (string("#") + model->Texture(idx) + "-mat").c_str();
	instance = instance.append_child("bind_vertex_input");
	instance.append_attribute("input_semantic") = "TEXCOORD";
	instance.append_attribute("input_set") = "1";
	instance.append_attribute("semantic") = "CHANNEL1";

	return string(model->Texture(idx) + "-mat");
}

void filter_string(std::string& base, const std::string& property) {
	size_t start = base.find(property);
	if (start == string::npos) {
		return;
	}
	size_t end = base.find("\n", start);
	if (end == string::npos) {
		base.erase(start);
	} else {
		base.erase(start, end - start + 1);
	}
}

pugi::xml_node DAESaver::add_helper(pugi::xml_node& element,string properties) {
	if (export_helpers) {
		pugi::xml_node helper = find_helper(element);
		if (properties.size() > 0 && strlen(properties.c_str()) > 0) {
			if (!helper) {
				helper = element.append_child("node");
				helper.append_attribute("id") =  "helper";
				helper.append_attribute("name") =  "helper";
			}
			pugi::xml_node props;
			stringstream temp;

			string result;
			string properti;
			string previous;
			filter_string(properties, "$rotate");
			filter_string(properties, "$uvec");
			filter_string(properties, "$fvec");
			trim_extra_spaces(properties);

			if (props_as_helpers) {
				props = helper.append_child("node");
				props.append_attribute("id") = "properties";
				props.append_attribute("name") = "properties";
				result = properties;
				temp.str(result.c_str());
				while (!temp.eof()) {
					temp >> properti;
					if (boost::algorithm::iequals(properti, previous)) {
						add_property(props,properti.c_str());
					}
					previous = properti;
				}
			} else {
				props = element.append_child("extra").append_child("technique");
				props.append_attribute("profile") = "FCOLLADA";
				props = props.append_child("user_properties");
				props.text().set(properties.c_str());

			}
		}
		return helper;
	}
	return pugi::xml_node();
}


void DAESaver::add_property(pugi::xml_node& props, const char* prop) {
	pugi::xml_node current_prop;
	current_prop = props.append_child("node");
	current_prop.append_attribute("id") = prop;
	current_prop.append_attribute("name") = prop;
}

pugi::xml_node DAESaver::find_subsystem(string name, vector3d &offset, vector3d &scale) {
	pugi::xml_node answer;
	offset = vector3d(0,0,0);
	scale = vector3d(1,1,1);
	for (unsigned int i = 0; i < subobjs.size(); i++) {
		if (boost::algorithm::iequals(model->SOBJ(i).name, name)) {
			offset = relative_to_absolute(vector3d(0,0,0),&model->SOBJ(i),model_subobjs);
			answer = subobjs[i];
			break;
		}
	}
	if (!answer) {
		for (int i = 0; i < model->GetSpecialCount(); i++) {
			if (boost::algorithm::iequals(model->Special(i).name, name)) {
				offset = model->Special(i).point;
				scale = radius_to_scale(model->Special(i).radius);
				answer = specials[i];
				break;
			}
		}
	}
	if (!answer) {
		if (name[0] != '$') {
			name.insert(0, "$");
		} else if (name[0] == '$') {
			name.erase(0,1);
		}
		for (unsigned int i = 0; i < subobjs.size(); i++) {
			if (boost::algorithm::iequals(model->SOBJ(i).name, name)) {
				offset = relative_to_absolute(vector3d(0,0,0),&model->SOBJ(i),model_subobjs);
				answer = subobjs[i];
				break;
			}
		}
		if (!answer) {
			for (int i = 0; i < model->GetSpecialCount(); i++) {
				if (boost::algorithm::iequals(model->Special(i).name, name)) {
					offset = model->Special(i).point;
					scale = radius_to_scale(model->Special(i).radius);
					answer = specials[i];
					break;
				}
			}
		}
	}
	return find_helper(answer);
}

pugi::xml_node DAESaver::find_helper(pugi::xml_node& element) {
	if (!element) return element;
	pugi::xml_node helper = element.find_child_by_attribute("node", "id", "helper");
	if (helper)
		return helper;
	helper = element.append_child("node");
	helpers.push_back(helper);
	helper.append_attribute("id") = "helper";
	helper.append_attribute("name") = "helper";
	return helper;
}

pugi::xml_node DAESaver::find_dockpoint(int idx,vector3d &offset) {
	offset = vector3d(0,0,0);
	for (unsigned int i = 0; i < docks.size(); i++) {
		for (unsigned int j = 0; j < model->Dock(i).paths.size(); j++) {
			if (model->Dock(i).paths[j] == idx) {
				if (model->Dock(i).dockpoints.size() > 0) {
					//offset = model->Dock(i).dockpoints[0].point;
				}
				return find_helper(docks[i]);
			}
		}
	}
	return pugi::xml_node();
}

void add_accessor(pugi::xml_node element, string source, int count, bool uv) {
	pugi::xml_node accessor = element.append_child("technique_common").append_child("accessor");
	accessor.append_attribute("count") = std::to_string(count).c_str();
	accessor.append_attribute("source") =  (string("#") + source).c_str();
	if (uv) {
		accessor.append_attribute("stride") = "2";
		add_param(accessor,"S");
		add_param(accessor,"T");
	} else {
		accessor.append_attribute("stride") = "3";
		add_param(accessor,"X");
		add_param(accessor,"Y");
		add_param(accessor,"Z");
	}
}

void add_param(pugi::xml_node accessor, string name) {
	pugi::xml_node temp = accessor.append_child("param");
	temp.append_attribute("type") = "float";
	temp.append_attribute("name") = name.c_str();
}

string write_vector3d(vector3d vec,vector3d scale) {
	stringstream output;
	output << -vec.x / scale.x << " " << vec.z / scale.z << " " << vec.y / scale.y;
	return string(output.str().c_str());

}

void DAESaver::write_transform(pugi::xml_node& element, const vector3d& offset, const vector3d& norm, const vector3d& base, float scale, float external_scale) {
	write_transform_binormal(element, offset, norm, vector3d(1, 0, 0), base, scale, external_scale);
}

void DAESaver::write_transform_binormal(pugi::xml_node& element, const vector3d& offset, const vector3d& norm, const vector3d& binorm, const vector3d& base, float scale, float external_scale) {
	element = element.append_child("matrix");
	element.append_attribute("sid") = "matrix";
	int order[3] = {0, 2, 1};
	vector3d one(MakeUnitVector(norm)), two(MakeUnitVector(binorm)), three;
	if (fabs(dot(one, two)) > 0.9) {
		two = vector3d(0,1,0);
	}
	two = MakeUnitVector(two - dot(one, two) * one);
	three = CrossProduct(one, two);
	const vector3d *columns[3];
	if (base.x) {
		columns[0] = &one;
		columns[1] = &two;
		columns[2] = &three;
	} else if (base.y) {
		columns[1] = &one;
		columns[0] = &two;
		columns[2] = &three;
	} else {
		columns[2] = &one;
		columns[1] = &two;
		columns[0] = &three;
	}
	stringstream output;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			float value = scale * (*columns[order[j]])[order[i]] / external_scale;
			if (order[i] == 0) {
				value = -value;
			}
			if (order[j] == 0) {
				value = -value;
			}
			output << value;
			output << " ";
		}
		if (order[i] == 0) {
			output << -offset[order[i]] / external_scale;
		} else {
			output << offset[order[i]] / external_scale;
		}
		output << " ";
	}
	output << "0 0 0 1";
	element.text().set(output.str().c_str());
}

vector3d radius_to_scale(float radius) {
	return vector3d(radius,radius,radius);
}

void add_scale(pugi::xml_node element, vector3d scale, vector3d parent_scale) {
	element = element.append_child("scale");
	element.append_attribute("sid") = "scale";
	scale.x = -scale.x;
	element.text().set(write_vector3d(scale, parent_scale).c_str());
}

void trim_extra_spaces(std::string& s) {
	if (s.empty()) {
		return;
	}
	string::iterator it, jt;
	jt = s.begin();
	for (it = jt; it < s.end() && isspace(*it); ++it) ;
	if (it == s.end()) {
		s.clear();
		return;
	}
	*jt = *it;
	++it;
	for (; it < s.end(); ++it) {
		if (!(isspace(*it) && isspace(*jt))) {
			++jt;
			if (*it == '\r' || *it == '\n') {
				*jt = '\n';
			} else if (isspace(*it)) {
				*jt = ' ';
			} else {
				*jt = *it;
			}
		} else if (*it == '\r' || *it == '\n') {
			*jt = '\n';
		}
	}
	if (isspace(*jt)) {
		jt--;
	}
	s.resize(jt - s.begin() + 1);
}

std::string get_name(const pugi::xml_node& element) {
	std::string name = element.attribute("name").as_string();
	if (name.empty()) {
		name = element.attribute("id").as_string();
	}
	return name;
}
