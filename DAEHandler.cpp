#include "DAEHandler.h"
#include "pcs_file.h"
#include "BSPHandler.h"
#include <sstream>
#include <iomanip>
#include <limits>
#include <map>
#include <boost/tr1/regex.hpp>
#include <boost/algorithm/string.hpp>

#define VECTOR_GROWTH_FACTOR 4
#define VECTOR_INITIAL_SIZE 100
#undef max

using namespace std;

DAEHandler::DAEHandler(string filename, PCS_Model *model, AsyncProgress *progress, bool mirror_x_axis, bool mirror_y_axis, bool mirror_z_axis) {
	this->filename = filename;
	root = dae.open(this->filename);
#if LOGGING_DAE
	filename.resize(filename.size() - 4);
	filename += ".log";
	log.reset(new std::ofstream(filename.c_str()));
	*log << root->getDocument()->getDocumentURI()->getPath() << endl;
#endif
	radius = 0;
	scaling_factor = 1.0f;
	doc = root->getDocumentURI()->getAuthority();
	doc += root->getDocumentURI()->getPath();
	this->model = model;
	daeElement *temp = root->getDescendant("up_axis");
	if (temp != NULL) {
		if (boost::algorithm::equals(temp->getCharData(), "X_UP")) {
			up_axis = 0;
			
		} else if (boost::algorithm::equals(temp->getCharData(), "Y_UP")) {
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
	daeURI uri(dae);
	string temp;
	progress->setTarget(200);
	progress->setMessage("Opening and Reading DAE");
	progress->Notify();
	num_textures = 0;

	daeTArray< daeSmartRef<daeElement> > helpers = root->getDescendant("visual_scene")->getChildren();
	vector<int> lods;
	vector<pcs_slot> guns, missiles;
	string name;
	map<string, int> detail, debris;
	model->SetMass(-1);
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
#if LOGGING_DAE
		*log << "Found " << helpers[i]->getTypeName();
		if (helpers[i]->getID()) {
			*log << ":" << helpers[get_name(i]);
		}
		*log << endl;
#endif
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			name = get_name(helpers[i]);
			progress->incrementWithMessage("Processing " + name);
			if (boost::algorithm::istarts_with(name, "subsystem")) {
				subsystem_handler(helpers[i], true);
			} else if (boost::algorithm::istarts_with(name, "special")) {
				subsystem_handler(helpers[i], false);
			} else if (boost::algorithm::istarts_with(name, "shield")) {
				shield_handler(helpers[i]);
			} else if (boost::algorithm::istarts_with(name, "thrusters")) {
				process_thrusters(helpers[i],string(),matrix(),vector3d(0,0,0));
			} else if (boost::algorithm::istarts_with(name, "insigLOD")) {
				process_insignia(helpers[i]);
			} else if (boost::algorithm::istarts_with(name, "debris")) {
				debris[name] = i;
			} else if (boost::algorithm::istarts_with(name, "dockpoint")) {
				process_dockpoint(helpers[i]);
			} else if (boost::algorithm::istarts_with(name, "bay")) {
				process_path(helpers[i], "", matrix(), vector3d());
			} else if (boost::algorithm::istarts_with(name, "detail")) {
				detail[name] = i;
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
				eyes[eye] = process_eyepoint(helpers[i]);
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
				guns[bank] = process_gunbank(helpers[i], 0);
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
				missiles[bank] = process_gunbank(helpers[i], 1);
			} else if (boost::algorithm::istarts_with(name, "mass")) {
				process_mass(helpers[i]);
			} else if (boost::algorithm::istarts_with(name, "moi")) {
				process_moment_of_inertia(helpers[i]);
			} else if (boost::algorithm::istarts_with(name, "com")) {
				model->SetCenterOfMass(get_translation(helpers[i]));
			} else if (boost::algorithm::istarts_with(name, "acen")) {
				model->SetAutoCenter(get_translation(helpers[i]));
			}
		}

	}
	for (map<string, int>::iterator it = detail.begin(); it != detail.end(); ++it) {
		model->AddLOD(subobjs.size());
		process_subobj(helpers[it->second],-1);
	}
	for (map<string, int>::iterator it = debris.begin(); it != debris.end(); ++it) {
		model->AddDebris(subobjs.size());
		process_subobj(helpers[it->second], -1);
	}
	progress->setTarget(203 + subobjs.size() + specials.size() + docks.size() + missiles.size() + 3);
	progress->SetProgress(203);

	vector<pcs_sobj> final_subobjs(subobjs.size());
	for (unsigned int i = 0; i < subobjs.size(); i++) {
		progress->incrementWithMessage("Finalising " + subobjs[i]->name);
		final_subobjs[i] = *subobjs[i];
	}
	model->set_subobjects(final_subobjs);
	for (unsigned int i = 0; i < specials.size(); i++) {
		progress->incrementWithMessage("Finalising " + specials[i]->name);
		model->AddSpecial(specials[i].get());
	}
	for (unsigned int i = 0; i < docks.size(); i++) {
		progress->incrementWithMessage("Finalising " + docks[i]->properties);
		model->AddDocking(docks[i].get());
	}
	model->set_eyes(eyes);
	int num_guns = guns.size();
	guns.resize(guns.size() + missiles.size());
	for (unsigned int i = 0; i < missiles.size(); i++) {
		progress->incrementWithMessage("Finalising gun " + i);
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


void DAEHandler::process_subobj(daeElement* element, int parent, matrix rotation_matrix) {
	// maybe getDescendent instead
	// but other elements along the way...
	daeElement *geom = element->getChild("instance_geometry");
	map<string,string> texture_mapping;

	if (geom == NULL) {
		stringstream gah;
		gah << "instance_geometry not found for subobject " << get_name(element);
		wxMessageBox(wxString(gah.str().c_str(), wxConvUTF8));
		return;
	}
	add_texture_mappings(geom, &texture_mapping);
	progress->incrementWithMessage("Processing " + get_name(element));

	string temp = doc;
	daeURI uri(dae);
	temp += geom->getAttribute("url").c_str();
#if LOGGING_DAE
	*log << temp << endl;
#endif
	uri.setURI(temp.c_str());

	daeElement* mesh = uri.getElement()->getChild("mesh");
	boost::shared_ptr<pcs_sobj> subobj(new pcs_sobj());
	subobj->bounding_box_min_point = vector3d(1e30f,1e30f,1e30f);
	subobj->bounding_box_max_point = vector3d(-1e30f,-1e30f,-1e30f);
	subobj->name = get_name(element).c_str();
	subobj->properties = "";
	subobj->parent_sobj = parent;
	//*log << "Processing " << subobj->name.c_str() << endl;

	subobj->offset = get_translation(element,rotation_matrix);

	// Hope this is right
	if (subobj->parent_sobj != -1) {
		subobj->geometric_center = relative_to_absolute(subobj->offset,subobjs[subobj->parent_sobj],subobjs);
	} else {
		subobj->geometric_center = vector3d(0,0,0);
	}

	rotation_matrix = get_rotation(element,rotation_matrix);

	daeTArray< daeSmartRef<daeElement> > poly_groups = mesh->getChildren();
	for (unsigned int i = 0; i < poly_groups.getCount(); i++) {
		// Add the polies to the subobj.
		if (boost::algorithm::equals(poly_groups[i]->getTypeName(), "triangles") || boost::algorithm::equals(poly_groups[i]->getTypeName(), "polylist")) {
			process_poly_group(poly_groups[i],subobj,rotation_matrix, &texture_mapping);
		}
	}
	uri.setURI((doc + "#" + element->getAttribute("id").c_str() + "-trans").c_str());
	daeElement *trans = uri.getElement();
	if (trans != NULL) {
		mesh = trans->getChild("instance_geometry");
		if (mesh != NULL) {
			add_texture_mappings(mesh, &texture_mapping);
			temp = doc;
			temp += mesh->getAttribute("url").c_str();
#if LOGGING_DAE
			*log << temp << endl;
#endif
			uri.setURI(temp.c_str());
			mesh = uri.getElement();
			if (mesh != NULL) {
				mesh = mesh->getChild("mesh");
				if (mesh != NULL) {
					poly_groups = mesh->getChildren();
					for (unsigned int i = 0; i < poly_groups.getCount(); i++) {
						// Add the polies to the subobj.
						if (boost::algorithm::equals(poly_groups[i]->getTypeName(), "triangles") || boost::algorithm::equals(poly_groups[i]->getTypeName(), "polylist")) {
							process_poly_group(poly_groups[i],subobj,rotation_matrix, &texture_mapping);
						}
					}
				}
			}
		}
	}

	this->subobjs.push_back(subobj);
	int current_sobj_id = this->subobjs.size() - 1;

	daeTArray< daeSmartRef<daeElement> > subobjs = element->getChildren();
	daeElement *sobj_helpers = NULL;
	std::map<std::string, unsigned int> subobj_map;
	for (unsigned int i = 0; i < subobjs.getCount(); i++) {
		if (boost::algorithm::equals(subobjs[i]->getTypeName(), "node")) {
			if (!boost::algorithm::istarts_with(get_name(subobjs[i]), "helper")) {
				if (!strstr(get_name(subobjs[i]).c_str(),"-trans")) {
					subobj_map[get_name(subobjs[i])] = i;
					//process_subobj(subobjs[i],current_sobj_id,rotation_matrix);
				}
			} else {
				sobj_helpers = subobjs[i];
			}
		} else if (boost::algorithm::equals(subobjs[i]->getTypeName(), "extra")) {
			daeElement *props = subobjs[i]->getDescendant("user_properties");
			if (props) {
				subobj->properties = props->getCharData().c_str();
			}
		}
	}
	for (std::map<std::string, unsigned int>::const_iterator it = subobj_map.begin(); it != subobj_map.end(); ++it) {
		process_subobj(subobjs[it->second],current_sobj_id,rotation_matrix);
	}

	if (sobj_helpers) {
		process_sobj_helpers(sobj_helpers,current_sobj_id,parent,rotation_matrix);
	}

}

void DAEHandler::process_dockpoint(daeElement *helper) {
	boost::shared_ptr<pcs_dock_point> dockpoint(new pcs_dock_point());
	pcs_hardpoint temp;
	daeTArray< daeSmartRef<daeElement> > helpers = helper->getChildren();
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node") && !boost::algorithm::istarts_with(get_name(helpers[i]), "helper")) {
			temp.norm = fix_axes(up,get_rotation(helpers[i]));
			temp.point = get_translation(helpers[i]);
			dockpoint->dockpoints.push_back(temp);
		}
	}
	docks.push_back(dockpoint);
	process_dock_helpers(helper);

}


void DAEHandler::process_poly_group(daeElement *element, boost::shared_ptr<pcs_sobj> subobj, matrix rotation, map<string, string> *texture_mapping) {

	daeTArray< daeSmartRef<daeElement> > poly_bits = element->getChildren();
	string temp;
	daeURI uri(dae);

	vector<int> refs;
	DAEInputs inputs(element,doc,&dae);
	int poly_offset = subobj->polygons.size();
	bool triangles = false;

	daeElement *vcount = element->getChild("vcount");
	if (vcount == NULL) {
		triangles = true;
	}

	string texture = element->getAttribute("material").c_str();
	int texture_id = find_texture_id(texture, texture_mapping);

	daeElement *ref_element = element->getChild("p");
	if (ref_element != NULL) {
		parse_int_array(ref_element->getCharData().c_str(), &refs);
	}



	int num_polies = atoi(element->getAttribute("count").c_str());

	subobj->polygons.resize(num_polies + poly_offset);
	vector<int> counts;
	if (!triangles) {
		parse_int_array(vcount->getCharData().c_str(), &counts, num_polies);
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

// Go through the helpers and hand off work appropriately...
void DAEHandler::process_sobj_helpers(daeElement *element,int current_sobj_id, int parent_sobj_id, matrix rotation_matrix) {
	vector3d offset = relative_to_absolute(vector3d(0,0,0),subobjs[current_sobj_id],subobjs);
	daeTArray< daeSmartRef<daeElement> > helpers = element->getChildren();
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helpers[i]), "properties")) {
				process_properties(helpers[i],&subobjs[current_sobj_id]->properties);
			}
		}
	}
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			std::string name = get_name(helpers[i]);
			if (boost::algorithm::istarts_with(name, "thrusters")) {
				process_thrusters(helpers[i],subobjs[current_sobj_id]->name,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "multifirepoints")) {
				process_firepoints(helpers[i],parent_sobj_id,current_sobj_id,rotation_matrix);
			} else if (boost::algorithm::istarts_with(name, "firepoints")) {
				process_firepoints(helpers[i],current_sobj_id,current_sobj_id,rotation_matrix);
			} else if (boost::algorithm::istarts_with(name, "glowbank")) {
				process_glowpoints(helpers[i],current_sobj_id,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "bay")) {
				process_path(helpers[i],subobjs[current_sobj_id]->name,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "path")) {
				process_path(helpers[i],subobjs[current_sobj_id]->name,rotation_matrix,offset);
			} else if (boost::algorithm::istarts_with(name, "vec")) {
				process_sobj_vec(helpers[i],rotation_matrix, &subobjs[current_sobj_id]->properties);
			} else if (boost::algorithm::istarts_with(name, "rotate-nospeed")) {
				process_sobj_rotate(helpers[i],rotation_matrix, subobjs[current_sobj_id], false);
			} else if (boost::algorithm::istarts_with(name, "rotate")) {
				process_sobj_rotate(helpers[i],rotation_matrix, subobjs[current_sobj_id]);
			} else if (boost::algorithm::istarts_with(name, "eyepoint")) {
				unsigned int eye;
				if (name.length() > strlen("eyepoint")) {
					eye = atoi(name.substr(strlen("eyepoint"),2).c_str()) - 1;
					if (eyes.size() <= eye) {
						eyes.resize(eye + 1);
					}
					eyes[eye] = process_eyepoint(helpers[i], rotation_matrix, current_sobj_id);
				}
			}
		}
	}
	trim_extra_spaces(subobjs[current_sobj_id]->properties);
}

void DAEHandler::process_special_helpers(daeElement *element, int idx, matrix rotation) {
	daeTArray< daeSmartRef<daeElement> > helpers = element->getChildren();
	daeElement *helper = NULL;
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node") && boost::algorithm::istarts_with(get_name(helpers[i]), "helper")) {
			helper = helpers[i];
			break;
		}
	}
	if (helper == NULL) {
		return;
	}
	
	vector3d offset = specials[idx]->point;
	helpers = helper->getChildren();
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helpers[i]), "properties")) {
				process_properties(helpers[i],&specials[idx]->properties);
			}
		}
	}
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			if (boost::algorithm::istarts_with(get_name(helpers[i]), "thrusters")) {
				process_thrusters(helpers[i],specials[idx]->name,rotation,offset);
			} else if (boost::algorithm::istarts_with(get_name(helpers[i]), "bay")) {
				process_path(helpers[i],specials[idx]->name,rotation,offset);
			} else if (boost::algorithm::istarts_with(get_name(helpers[i]), "path")) {
				process_path(helpers[i],specials[idx]->name,rotation,offset);
			}
		}
	}
}

void DAEHandler::process_dock_helpers(daeElement *element) {
	int idx = docks.size() - 1;
	daeTArray< daeSmartRef<daeElement> > helpers = element->getChildren();
	daeElement *helper = NULL;
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node") && boost::algorithm::istarts_with(get_name(helpers[i]), "helper")) {
			helper = helpers[i];
			break;
		}
	}
	daeElement *props = element->getDescendant("user_properties");
	if (props) {
		docks[idx]->properties = props->getCharData().c_str();
	}

	if (helper == NULL) {
		return;
	}
	
	vector3d offset = vector3d(0,0,0);//docks[idx]->dockpoints[0].point;
	helpers = helper->getChildren();
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			// better check for custom properties first
			if (boost::algorithm::istarts_with(get_name(helpers[i]), "properties")) {
				process_properties(helpers[i],&docks[idx]->properties);
			}
		}
	}
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			if (boost::algorithm::istarts_with(get_name(helpers[i]), "path")) {
				process_path(helpers[i],"",matrix(),offset,idx);
			}
		}
	}

}

void DAEHandler::process_properties(daeElement *element,string *properties) {
	if (element == NULL) return;

	daeTArray< daeSmartRef<daeElement> > helpers = element->getChildren();
	daeElement *extra = element->getDescendant("user_properties");
	if (extra) {
		*properties = extra->getCharData().c_str();
		return;
	}
	*properties = "";
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			*properties += get_name(helpers[i]);
			//properties->replace(SPACE_REPLACEMENT,' ');
			*properties += "\n";
		}
	}
	trim_extra_spaces(*properties);
}

void DAEHandler::process_sobj_vec(daeElement *element, matrix rotation, std::string* properties) {
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

void DAEHandler::process_sobj_rotate(daeElement *element, matrix rotation, boost::shared_ptr<pcs_sobj> sobj, bool speed) {
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

void DAEHandler::process_thrusters(daeElement *element,string name,matrix rotation_matrix, vector3d offset) {
	offset = get_translation(element, rotation_matrix) + offset;
	rotation_matrix = get_rotation(element,rotation_matrix);
	daeTArray< daeSmartRef<daeElement> > thrusters = element->getChildren();
	pcs_thruster thruster;
	pcs_thrust_glow glow;
	if (name.size() > 0 && strlen(name.c_str()) > 0) {
		if (name[0] == '$') {
			thruster.properties = string("$engine_subsystem=") + name;
		} else {
			thruster.properties = string("$engine_subsystem=$") + name;
		}
	}
	for (unsigned int i = 0; i < thrusters.getCount(); i++) {
		if (boost::algorithm::equals(thrusters[i]->getTypeName(), "node")) {
			glow.pos = get_translation(thrusters[i], rotation_matrix) + offset;
			glow.norm = MakeUnitVector(fix_axes(front,get_rotation(thrusters[i],rotation_matrix)));
			glow.radius = get_rotation(thrusters[i], rotation_matrix).scale();
			thruster.points.push_back(glow);
		}
	}
	model->AddThruster(&thruster);
}

void DAEHandler::process_firepoints(daeElement *element,int parent, int arm,matrix rotation_matrix) {
	daeTArray< daeSmartRef<daeElement> > turrets = element->getChildren();
	pcs_turret turret;
	turret.sobj_parent = parent;
	turret.sobj_par_phys = arm;
	turret.turret_normal = vector3d(0,0,0);

	for (unsigned int i = 0; i < turrets.getCount(); i++) {
		if (boost::algorithm::equals(turrets[i]->getTypeName(), "node")) {
			turret.turret_normal += fix_axes(up,get_rotation(turrets[i],rotation_matrix));
			turret.fire_points.push_back(get_translation(turrets[i],rotation_matrix));
		}
	}
	if (subobjs[parent]->properties.size() == 0) {
		subobjs[parent]->properties = "$special=subsystem\n$fov=180\n$name=GunTurret\n";
	} else {
		if (!strstr(subobjs[parent]->properties.c_str(),"special")) {
			subobjs[parent]->properties += "$special=subsystem\n";
		}
		if (!strstr(subobjs[parent]->properties.c_str(),"fov")) {
			subobjs[parent]->properties += "$fov=180\n";
		}
		if (!strstr(subobjs[parent]->properties.c_str(),"name")) {
			//subobjs[parent]->properties += "$name=GunTurret\n";
		}
	}
	turret.turret_normal = MakeUnitVector(turret.turret_normal);
	model->AddTurret(&turret);
}

void DAEHandler::process_path(daeElement *element,string parent,matrix rotation_matrix, vector3d offset, int dock) {
	pcs_path path;
	path.name = string ("$") + get_name(element).c_str();
	trim_extra_spaces(path.name);
#if LOGGING_DAE
	*log << element->getAttribute("id").c_str() << endl;
#endif
	path.parent = parent;
	daeTArray< daeSmartRef<daeElement> > paths = element->getChildren();
	std::multimap<std::string, pcs_pvert> ordered_paths;
	pcs_pvert vert;

	for (unsigned int i = 0; i < paths.getCount(); i++) {
		if (boost::algorithm::equals(paths[i]->getTypeName(), "node")) {
			vert.pos = get_translation(paths[i], rotation_matrix) + offset;
			vert.radius = get_rotation(paths[i], rotation_matrix).scale();
			ordered_paths.insert(std::pair<std::string, pcs_pvert>(get_name(paths[i]), vert));
		}
	}
	for (std::multimap<string, pcs_pvert>::iterator it = ordered_paths.begin(); it != ordered_paths.end(); ++it) {
		path.verts.push_back(it->second);
	}

	if (dock > -1) {
		docks[dock]->paths.push_back(model->GetPathCount());
	}

	model->AddPath(&path);
}

void DAEHandler::process_glowpoints(daeElement *element,int parent,matrix rotation_matrix, vector3d offset) {
	daeTArray< daeSmartRef<daeElement> > glows = element->getChildren();
	pcs_glow_array glowbank;
	glowbank.obj_parent = parent;
	pcs_thrust_glow glow;
	for (unsigned int i = 0; i < glows.getCount(); i++) {
		if (boost::algorithm::equals(glows[i]->getTypeName(), "node")) {
			if (boost::algorithm::istarts_with(get_name(glows[i]), "helper")) {
				process_properties(glows[i]->getChild("node"),&glowbank.properties);
			} else {
				glow.pos = get_translation(glows[i], rotation_matrix);
				glow.radius = get_rotation(glows[i], rotation_matrix).scale();
				glow.norm = vector3d(0,0,0);
				glowbank.lights.push_back(glow);
			}
		} else if (boost::algorithm::equals(glows[i]->getTypeName(), "extra")) {
			daeElement *props = glows[i]->getDescendant("user_properties");
			if (props != NULL) {
				glowbank.properties = props->getCharData().c_str();
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

int DAEHandler::find_texture_id(string name, map<string, string> *texture_mapping) {
	daeURI uri(dae);
	string temp = doc;
	if (texture_mapping == NULL) {
		return -1;
	}
	temp += (*texture_mapping)[name.c_str()].c_str();
	uri.setURI(temp.c_str());
	daeElement *current = uri.getElement();
	if (current == NULL) {
		return -1;
	}
	current = current->getChild("instance_effect");
	if (current == NULL) {
		return -1;
	}
	temp = doc;
	temp += current->getAttribute("url").c_str();
	uri.setURI(temp.c_str());
	current = uri.getElement();
	if (current == NULL) {
		return -1;
	}

	current = current->getDescendant("init_from");
	if (current == NULL) {
		return -1;
	}
	temp = doc;
	temp += "#";
	temp += current->getCharData().c_str();
	uri.setURI(temp.c_str());
	current = uri.getElement();
	if (current == NULL) {
		return -1;
	}
	current = current->getDescendant("init_from");
	if (current == NULL) {
		return -1;
	}
	return find_or_add_texture(string(current->getCharData().c_str()));
}

int DAEHandler::find_or_add_texture(string name) {
	if (texture_map.find(name) == texture_map.end()) {
		texture_map[name] = num_textures++;
		model->AddTextures(strip_texture(name));
	}
	return texture_map[name];
}

void DAEHandler::subsystem_handler(daeElement *helper, bool isSubsystem) {
	boost::shared_ptr<pcs_special> special(new pcs_special);
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
	specials.push_back(special);
	process_special_helpers(helper,specials.size() - 1, rot);

}

void DAEHandler::shield_handler(daeElement *helper) {
	daeElement *geom = helper->getChild("instance_geometry");
	if (geom == NULL) {
		return;
	}

	string temp = doc;
	daeURI uri(dae);
	temp += geom->getAttribute("url").c_str();
#if LOGGING_DAE
	*log << temp << endl;
#endif
	uri.setURI(temp.c_str());

	daeElement* mesh = uri.getElement()->getChild("mesh");
	bool use_triangles = false;
	daeElement* triangles = mesh->getChild("triangles");
	if (triangles) {
		use_triangles = true;
		mesh = triangles;
	} else {
		mesh = mesh->getChild("polylist");
	}

	matrix rotation = get_rotation(helper);
	
	vector<int> refs;

	DAEInputs inputs(mesh,doc,&dae);
	if (!inputs.pos.is_valid()) {
		wxMessageBox(wxT("Positions not found for shield"));
		return;
	}
	if (!inputs.norm.is_valid()) {
		wxMessageBox(wxT("Normals not found for shield"));
		return;
	}

	daeElement *ref_element = mesh->getDescendant("p");
	if (ref_element != NULL) {
		parse_int_array(ref_element->getCharData().c_str(), &refs);
	}

	int num_polies = atoi(mesh->getAttribute("count").c_str());

	std::vector<int> vcount;
	if (!use_triangles) {
		parse_int_array(mesh->getChild("vcount")->getCharData().c_str(), &vcount, num_polies);
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

void DAEHandler::process_insignia(daeElement *element) {
	pcs_insig insignia;
	string name = get_name(element);
	insignia.lod = 0;
	if (name.length() >= 10) {
		name = name.substr(8,2);
		insignia.lod = atoi(name.c_str()) - 1;
	}
	insignia.offset = get_translation(element);
	daeElement *geom = element->getChild("instance_geometry");

	daeURI uri(dae);
	string temp = doc;
	temp += geom->getAttribute("url").c_str();
	//*log << temp << endl;
	uri.setURI(temp.c_str());

	daeElement* mesh = uri.getElement()->getChild("mesh");
	bool use_triangles = false;
	daeElement* triangles = mesh->getChild("triangles");
	if (triangles) {
		use_triangles = true;
		mesh = triangles;
	} else {
		mesh = mesh->getChild("polylist");
	}

	matrix rotation = get_rotation(element);
	
	daeTArray< daeSmartRef<daeElement> > poly_bits = mesh->getChildren();

	vector<int> refs;
	DAEInputs inputs(mesh,doc,&dae);


	daeElement *ref_element = mesh->getDescendant("p");
	if (ref_element != NULL) {
		parse_int_array(ref_element->getCharData().c_str(), &refs);
	}
	if (!inputs.pos.is_valid()) {
		wxMessageBox(wxT("Positions not found for insignia"));
		return;
	}
	if (!inputs.uv.is_valid()) {
		wxMessageBox(wxT("UV not found for insignia"));
		return;
	}

	int num_polies = atoi(mesh->getAttribute("count").c_str());

	std::vector<int> vcount;
	if (!use_triangles) {
		parse_int_array(mesh->getChild("vcount")->getCharData().c_str(), &vcount, num_polies);
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

void DAEHandler::process_mass(daeElement *element) {
	element = element->getChild("node");
	if (element != NULL) {
		std::stringstream stream(get_name(element));
		float mass;
		stream >> mass;
		model->SetMass(mass);
	}
}

void DAEHandler::process_moment_of_inertia(daeElement *element) {
	element = element->getChild("node");
	if (element != NULL) {
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
		result->resize(count);
	} else {
		result->resize(VECTOR_INITIAL_SIZE);
	}
	stringstream in(chars);
	int temp;
	unsigned int i = 0;
	while(!in.eof() && i < count) {
		in >> temp;
		if (i == result->size()) {
			result->resize(result->size() * VECTOR_GROWTH_FACTOR);
		}
		(*result)[i] = temp;
		i++;
	}
	if (i < count && count != (unsigned)-1) {
		stringstream error;
		error << "Found " << i << " items \"" << chars << "\", expected " << count << ". This may break something later...";
		wxMessageBox(wxString(error.str().c_str(), wxConvUTF8));
	}
	if (i != result->size()) {
		result->resize(i);
	}
}

boost::shared_ptr<vector<float> > parse_float_array(const char* chars, unsigned int count) {
	boost::shared_ptr<vector<float> > result;
	if (count != (unsigned)-1) {
		result.reset(new vector<float>(count));
	} else {
		result.reset(new vector<float>(VECTOR_INITIAL_SIZE));
	}
	stringstream in(chars);
	float temp;
	unsigned int i = 0;
	while(!in.eof() && i < count) {
		in >> temp;
		if (i == result->size()) {
			result->resize(result->size() * VECTOR_GROWTH_FACTOR);
		}
		(*result)[i] = temp;
		i++;
	}
	
	if (i < count && count != (unsigned)-1) {
		stringstream error;
		error << "Found " << i << " items \"" << chars << "\", expected " << count << ". This may break something later...";
		wxMessageBox(wxString(error.str().c_str(), wxConvUTF8));
	}
	if (i != result->size()) {
		result->resize(i);
	}
	return result;
}

string write_int_array(vector<int> vec) {
	stringstream result;
	for (unsigned int i = 0; i < vec.size(); i++) {
		result << vec[i] << " ";
	}
	return string(result.str().c_str());
}

string write_float_array(vector<float> vec) {
	stringstream result;
	for (unsigned int i = 0; i < vec.size(); i++) {
		result << vec[i] << " ";
	}
	return string(result.str().c_str());
}


string int_to_string(int i) {
	stringstream temp;
	temp << i;
	return string(temp.str().c_str());
}

vector3d DAEHandler::get_translation(daeElement *element, matrix rotation) {
	daeElement *position = element->getChild("translate");
	daeElement *matrix = element->getChild("matrix"); 
	boost::shared_ptr<vector<float> > points;
	vector3d result;
	if (position != NULL) {
		points = parse_float_array(position->getCharData().c_str(),3);
		result = fix_axes(vector3d((*points)[0],(*points)[1],(*points)[2]),rotation);
	} else if (matrix != NULL) {
		points = parse_float_array(matrix->getCharData().c_str(),16);
		result = fix_axes(vector3d((*points)[3],(*points)[7],(*points)[11]),rotation);
	}
	return result;
}

void DAEHandler::process_vector3d(vector3d vec, boost::shared_ptr<pcs_sobj> subobj) {
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
		vec = relative_to_absolute(vec, subobj, subobjs);
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

DAEInput::DAEInput(daeURI uri) {
	valid = false;
	daeElement *element = uri.getElement();
	string next_uri = uri.getAuthority();
	next_uri += uri.getPath();
	if (element == NULL) {
		return;
	}
	daeElement *next = element->getChild("input");
	if (next != NULL) {

		next_uri += next->getAttribute("source").c_str();
		uri.setURI(next_uri.c_str());
		DAEInput other(uri);
		*this = other;
	} else {
		next = element->getChild("technique_common");
		if (next == NULL) {
			return;
		}
		next = next->getChild("accessor");
		if (next == NULL) {
			return;
		}
		strides = atoi(next->getAttribute("stride").c_str());
		next_uri += next->getAttribute("source").c_str();

		// find coord order
		daeTArray <daeSmartRef <daeElement> > children = next->getChildren();
		int found = 0;
		int uvs = 0;
		for (unsigned int i = 0; i < children.getCount(); i++) {
			switch (get_name(children[i]).c_str()[0]) {
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
					error += get_name(children[i]).c_str();
					error += "' found";
					//wxMessageBox(error.c_str());
					break;
			}
		}
		if (found != 3 && uvs != 2) {
			wxMessageBox(wxT("Incorrect number of coordinate axes found!"));
			exit(1);
		}
		uri.setURI(next_uri.c_str());
		next = uri.getElement();
		if (next == NULL) {
			return;
		}
		if (next->hasAttribute("count")) {
			value = parse_float_array(next->getCharData().c_str(), atoi(next->getAttribute("count").c_str()));
		} else {
			value = parse_float_array(next->getCharData().c_str());
		}
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

const vector<float> &DAEInput::values() {
	assert(valid);
	return *value;
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

DAEInputs::DAEInputs(daeElement *element, string doc, DAE *dae) {
	daeURI uri(*dae);
	string temp;
	int vert_offset;
	max_offset = 0;
	daeTArray< daeSmartRef<daeElement> > poly_bits = element->getChildren();
	int uv_set_id = std::numeric_limits<int>::max();
	for (unsigned int k = 0; k < poly_bits.getCount(); k++) {
		if (boost::algorithm::equals(poly_bits[k]->getAttribute("semantic"), "VERTEX")) {
			vert_offset = atoi(poly_bits[k]->getAttribute("offset").c_str());
			max_offset = max(max_offset,vert_offset);
			temp = doc;
			temp += poly_bits[k]->getAttribute("source").c_str();
			uri.setURI(temp.c_str());
			daeTArray< daeSmartRef<daeElement> > vertices = uri.getElement()->getChildren();
			for (unsigned int i = 0; i < vertices.getCount(); i++) {

				if (boost::algorithm::equals(vertices[i]->getAttribute("semantic"), "POSITION")) {
					pos_offset = vert_offset;
					temp = doc;
					temp += vertices[i]->getAttribute("source").c_str();
					uri.setURI(temp.c_str());
					pos = DAEInput(uri);
				} else if (boost::algorithm::equals(vertices[i]->getAttribute("semantic"), "NORMAL")) {
					norm_offset = vert_offset;
					temp = doc;
					temp += vertices[i]->getAttribute("source").c_str();
					uri.setURI(temp.c_str());
					norm = DAEInput(uri);
				}
			}
		} else if (boost::algorithm::equals(poly_bits[k]->getAttribute("semantic"), "NORMAL")) {
			norm_offset = atoi(poly_bits[k]->getAttribute("offset").c_str());
			max_offset = max(max_offset,norm_offset);
			temp = doc;
			temp += poly_bits[k]->getAttribute("source").c_str();
			uri.setURI(temp.c_str());
			norm = DAEInput(uri);
		} else if (boost::algorithm::equals(poly_bits[k]->getAttribute("semantic"), "TEXCOORD")) {
			int current_offset = atoi(poly_bits[k]->getAttribute("offset").c_str());
			max_offset = max(max_offset, current_offset);
			if (poly_bits[k]->hasAttribute("set")) {
				int current_uv_set_id = atoi(poly_bits[k]->getAttribute("set").c_str());
				if (current_uv_set_id >= uv_set_id) {
					continue;
				}
				uv_set_id = current_uv_set_id;
			}
			uv_offset = current_offset;
			temp = doc;
			temp += poly_bits[k]->getAttribute("source").c_str();
			uri.setURI(temp.c_str());
			uv = DAEInput(uri);
		} else if (!poly_bits[k]->getAttribute("offset").empty()) {
			max_offset = max(max_offset,atoi(poly_bits[k]->getAttribute("offset").c_str()));
		}
	}
	max_offset++;
}


vector3d DAEHandler::fix_axes(vector3d broken, matrix rotation) {
	broken = rotation * broken;

	if (up_axis == 1) {
		broken = vector3d(-broken.x,-broken.y,broken.z);
	} else {
		broken = vector3d(-broken.x,broken.z,broken.y);
	}
	if (mirror_x) broken.x *= -1;
	if (mirror_y) broken.y *= -1;
	if (mirror_z) broken.z *= -1;
	return broken;
}

matrix DAEHandler::get_rotation(daeElement *element, matrix old) {
	matrix rot;
	matrix base;
	daeTArray< daeSmartRef<daeElement> > children = element->getChildren();
	boost::shared_ptr<vector<float> > temp;
	for (unsigned int i = 0; i < children.getCount(); i++) {
		if (boost::algorithm::equals(children[i]->getTypeName(), "rotate")) {
			temp = parse_float_array(children[i]->getCharData().c_str(),4);
			rot = matrix((*temp)[3]);
			base = matrix(vector3d((*temp)[0],(*temp)[1],(*temp)[2]));
			old = base.invert() % rot % base % old;
		} else if (boost::algorithm::equals(children[i]->getTypeName(), "scale")) {
			temp = parse_float_array(children[i]->getCharData().c_str(),3);
			base = matrix();
			for (int j = 0; j < 3; j++) {
				base.a2d[j][j] = (*temp)[j];
			}
			old = base % old;
		} else if (boost::algorithm::equals(children[i]->getTypeName(), "matrix")) {
			temp = parse_float_array(children[i]->getCharData().c_str(),16);
			rot = matrix(temp.get());
			old = old % rot;
		}
	}
	return old;
}

pcs_slot DAEHandler::process_gunbank(daeElement *helper, int type) {
	pcs_slot bank;
	bank.type = type;
	pcs_hardpoint temp;
	matrix rot = get_rotation(helper);
	vector3d translation = get_translation(helper);
	daeTArray< daeSmartRef<daeElement> > helpers = helper->getChildren();
	std::multimap<std::string, pcs_hardpoint> ordered_gunpoints;

	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::equals(helpers[i]->getTypeName(), "node")) {
			temp.norm = fix_axes(up,get_rotation(helpers[i], rot));
			temp.point = get_translation(helpers[i], rot) + translation;
			ordered_gunpoints.insert(std::pair<std::string, pcs_hardpoint>(get_name(helpers[i]), temp));
		}
	}
	for (std::multimap<string, pcs_hardpoint>::iterator it = ordered_gunpoints.begin(); it != ordered_gunpoints.end(); ++it) {
		bank.muzzles.push_back(it->second);
	}
	return bank;
}

pcs_eye_pos DAEHandler::process_eyepoint(daeElement* helper, matrix transform, int subobj_idx) {
	pcs_eye_pos result;
	result.normal = fix_axes(up, get_rotation(helper, transform));
	result.sobj_offset = get_translation(helper, transform);
	result.sobj_number = subobj_idx;
	return result;

}

// Saving stuff from here... maybe
DAESaver::DAESaver(string name, PCS_Model *model, int helpers, int props_as_helpers, AsyncProgress* progress) {
	this->model = model;
	this->filename = name;
	root = dae.add(name.c_str());
	for (int i = 0; i < model->GetSOBJCount(); i++) {
		subobjs.push_back(NULL);
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
		progress->incrementWithMessage( "Adding mass");
		add_mass();
		progress->incrementWithMessage( "Adding moment of inertia");
		add_moment_of_inertia();
		progress->incrementWithMessage( "Adding center of mass");
		add_center_of_mass();
		progress->incrementWithMessage( "Adding autocentering");
		add_autocentering();
	}


	progress->incrementWithMessage( "Saving DAE");
	dae.writeAll();
	return 0;
}

void DAESaver::add_header() {
	daeElement* asset = root->add("asset");
	asset->add("contributor");
	daeElement* created = asset->add("created");
	daeElement* modified = asset->add("modified");
	created->setCharData("2008-04-08T13:07:52");
	modified->setCharData("2008-04-08T13:07:52");
	daeElement *scene = root->add("scene instance_visual_scene");
	scene->setAttribute("url","#Scene");
	daeElement *up = asset->add("up_axis");
	up->setCharData("Z_UP");
}

void DAESaver::add_textures() {
	materials = root->add("library_materials");
	daeElement *images = root->add("library_images");
	daeElement *effects = root->add("library_effects");
	daeElement *current_image;
	daeElement *current_init;
	daeElement *current_effect;
	daeElement *sampler;
	daeElement *surface;
	daeElement *shader;
	string temp;
	num_textures = model->GetTexturesCount();
	
	for (int i = 0; i < model->GetTexturesCount(); i++) {
		temp = model->Texture(i);
		current_image = images->add("image");
		current_init = current_image->add("init_from");
		current_init->setCharData((temp + ".tga").c_str());
		temp += "-img";
		current_image->setAttribute("id",temp.c_str());
		current_image->setAttribute("name",temp.c_str());
		current_effect = effects->add("effect");
		temp = model->Texture(i);
		temp += "-fx";
		current_effect->setAttribute("id",temp.c_str());
		current_effect->setAttribute("name",temp.c_str());
		current_effect = current_effect->add("profile_COMMON");
		temp = model->Texture(i);
		surface = current_effect->add("newparam");
		surface->setAttribute("sid",(string(model->Texture(i)) + "-surface").c_str());
		surface = surface->add("surface");
		surface->setAttribute("type","2D");
		surface = surface->add("init_from");
		surface->setCharData((string(model->Texture(i)) + "-img").c_str());
		sampler = current_effect->add("newparam");
		sampler->setAttribute("sid",(string(model->Texture(i)) + "-sampler").c_str());
		sampler = sampler->add("sampler2D source");
		sampler->setCharData((string(model->Texture(i)) + "-surface").c_str());
		shader = current_effect->add("technique");
		shader = shader->add("phong");
		shader = shader->add("diffuse");
		shader = shader->add("texture");
		shader->setAttribute("texcoord","CHANNEL1");
		shader->setAttribute("texture",(string(model->Texture(i)) + "-sampler").c_str());

		daeElement *mat = materials->add("material");
		temp = string(model->Texture(i).c_str()) + "-mat";
		mat->setAttribute("id",temp.c_str());
		mat->setAttribute("name",temp.c_str());
		daeElement *effect = mat->add("instance_effect");
		effect->setAttribute("url",(string("#") + model->Texture(i) + "-fx").c_str());


		// probably still needs more...or not!
	}
}

void DAESaver::add_geom() {
	geoms = root->add("library_geometries");
	scene = root->add("library_visual_scenes visual_scene");
	scene->setAttribute("id","Scene");
	scene->setAttribute("name","Scene");
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
	if (subobjs[idx] != NULL) return;
	daeElement *subobj;
	pcs_sobj& sobj = model->SOBJ(idx);
	if (sobj.parent_sobj == -1) {
		subobj = scene->add("node");
	} else {
		if (subobjs[sobj.parent_sobj] != NULL) {
			subobj = subobjs[sobj.parent_sobj]->add("node");
		} else {
			return;
		}
	}
	subobjs[idx] = subobj;
	daeElement* helper = add_helper(subobj,sobj.properties);
	daeElement *translate = subobj->add("translate");
	translate->setCharData(write_vector3d(sobj.offset).c_str());
	if (name) {
		progress->incrementWithMessage("Adding " + *name);
		subobj->setAttribute("id",name->c_str());
		subobj->setAttribute("name",name->c_str());
	} else {
		subobj->setAttribute("id",sobj.name.c_str());
		subobj->setAttribute("name",sobj.name.c_str());
		progress->incrementWithMessage("Adding " + sobj.name);
	}
	
	// split up polies by texture...
	vector<vector<pcs_polygon*> > polies;
	vector<daeElement*> poly_groups;
	polies.resize(num_textures + 1);
	vector<unsigned int> counters;
	counters.resize(num_textures + 1, 0);
	for (int i = 0; i <= num_textures; i++) {
		polies[i].resize(VECTOR_INITIAL_SIZE);
	}
	for (unsigned int i = 0; i < sobj.polygons.size(); i++) {
		if (sobj.polygons[i].texture_id < num_textures && sobj.polygons[i].texture_id >= 0) {
			if (counters[sobj.polygons[i].texture_id] >= polies[sobj.polygons[i].texture_id].size()) {
				polies[sobj.polygons[i].texture_id].resize(polies[sobj.polygons[i].texture_id].size() * VECTOR_GROWTH_FACTOR);
			}
			polies[sobj.polygons[i].texture_id][counters[sobj.polygons[i].texture_id]] = &(sobj.polygons[i]);
			counters[sobj.polygons[i].texture_id]++;
		} else {
			if (counters[num_textures] >= polies[num_textures].size()) {
				polies[num_textures].resize(polies[num_textures].size() * VECTOR_GROWTH_FACTOR);
			}
			polies[num_textures][counters[num_textures]] = &(sobj.polygons[i]);
			counters[num_textures]++;
		}
	}
	for (int i = 0; i <= num_textures; i++) {
		if (polies[i].size() != counters[i]) {
			polies[i].resize(counters[i]);
		}
	}
	daeElement *current_group;
	daeElement *temp;
	stringstream group_name;
	temp = subobj->add("instance_geometry");
	temp->add("bind_material")->add("technique_common");
	temp->setAttribute("url",(string("#") + get_name(subobj).c_str() + "-geometry").c_str());
	group_name << get_name(subobj).c_str() << "-geometry";
	current_group = get_polygroups(polies,string(group_name.str().c_str()),subobj);

	if (export_helpers) {
		add_sobj_helpers(subobj, helper, sobj);
	}
}

void DAESaver::add_sobj_helpers(daeElement *subobj, daeElement* helper, const pcs_sobj& sobj) {
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
		if (helper == NULL) {
			helper = subobj->add("node");
			helper->setAttribute("id","helper");
			helper->setAttribute("name","helper");
		}
		daeElement* rotation = helper->add("node");
		if (rotate_offset != string::npos) {
			rotation->setAttribute("id","rotate");
			rotation->setAttribute("name","rotate");
		} else {
			rotation->setAttribute("id","rotate-nospeed");
			rotation->setAttribute("name","rotate-nospeed");
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
		if (helper == NULL) {
			helper = subobj->add("node");
			helper->setAttribute("id","helper");
			helper->setAttribute("name","helper");
		}
		daeElement* vec = helper->add("node");
		vec->setAttribute("id","vec");
		vec->setAttribute("name","vec");
		write_transform_binormal(vec, vector3d(), fvec, uvec, vector3d(0,0,1));
	}
}

daeElement *DAESaver::get_polygroups(vector <vector <pcs_polygon*> > polies, string name,daeElement *node) {
	daeElement *result = geoms->add("geometry");
	result->setAttribute("id",name.c_str());
	result->setAttribute("name",name.c_str());
	daeElement *mesh = result->add("mesh");
	vector<float> vert,norm,uv;
	vector<int> ref, sizes;
	map<vector3d,int,bool(*)(const vector3d&, const vector3d&)> vert_map(vector3d_comparator),norm_map(vector3d_comparator);
	map<pair<float,float>,int,bool(*)(const pair<float,float>&, const pair<float,float>&)> uv_map(float_pair_comparator);
	unsigned int vert_idx = 0, norm_idx = 0, uv_idx = 0;
	vert.resize(VECTOR_INITIAL_SIZE);
	norm.resize(VECTOR_INITIAL_SIZE);
	uv.resize(VECTOR_INITIAL_SIZE);

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
						vert_map.insert(pair<vector3d,int>(polies[i][j]->verts[k].point, vert_map.size()));
						if (vert.size() <= vert_idx + 2) {
							vert.resize(vert.size() * VECTOR_GROWTH_FACTOR);
						}
						vert[vert_idx] = -(polies[i][j]->verts[k].point.x);
						vert[vert_idx + 1] = polies[i][j]->verts[k].point.z;
						vert[vert_idx + 2] = polies[i][j]->verts[k].point.y;
						vert_idx += 3;
					}
					if (norm_map.find(polies[i][j]->verts[k].norm) == norm_map.end()) {
						norm_map.insert(pair<vector3d,int>(polies[i][j]->verts[k].norm, norm_map.size()));
						if (norm.size() <= norm_idx + 2) {
							norm.resize(norm.size() * VECTOR_GROWTH_FACTOR);
						}
						norm[norm_idx] = -(polies[i][j]->verts[k].norm.x);
						norm[norm_idx + 1] = polies[i][j]->verts[k].norm.z;
						norm[norm_idx + 2] = polies[i][j]->verts[k].norm.y;
						norm_idx += 3;

					}
					if (uv_map.find(pair<float,float>(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v)) == uv_map.end()) {
						uv_map.insert(pair<pair<float,float>,int>(pair<float,float>(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v), uv_map.size()));
						if (uv.size() <= uv_idx + 1) {
							uv.resize(uv.size() * VECTOR_GROWTH_FACTOR);
						}
						uv[uv_idx] = polies[i][j]->verts[k].u;
						uv[uv_idx + 1] = 1.0f - polies[i][j]->verts[k].v;
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
					ref[count + 2] = uv_map[pair<float,float>(polies[i][j]->verts[k].u,polies[i][j]->verts[k].v)];
					count += 3;
				}
			}
			add_refs(mesh,name,ref,sizes,node,i);
		}
	}
	daeElement *pos = mesh->add("source");
	pos->setAttribute("id",(name + "-position").c_str());
	daeElement *pos_float_array = pos->add("float_array");
	pos_float_array->setAttribute("id",(name + "-position-array").c_str());
	pos_float_array->setCharData(write_float_array(vert).c_str());
	pos_float_array->setAttribute("count",int_to_string(vert.size()).c_str());
	add_accessor(pos,name + "-position-array",vert.size()/3);

	daeElement *norms = mesh->add("source");
	norms->setAttribute("id",(name + "-normals").c_str());
	daeElement *norms_float_array = norms->add("float_array");
	norms_float_array->setAttribute("id",(name + "-normals-array").c_str());
	norms_float_array->setCharData(write_float_array(norm).c_str());
	norms_float_array->setAttribute("count",int_to_string(norm.size()).c_str());
	add_accessor(norms,name + "-normals-array",norm.size()/3);

	daeElement *verts = mesh->add("vertices");
	verts->setAttribute("id",(name + "-vertex").c_str());
	daeElement *vert_input = verts->add("input");
	vert_input->setAttribute("semantic", "POSITION");
	vert_input->setAttribute("source", (string("#") + name + "-position").c_str());
	daeElement *uvs = mesh->add("source");
	uvs->setAttribute("id",(name + "-uv").c_str());
	daeElement *uvs_float_array = uvs->add("float_array");
	uvs_float_array->setAttribute("id",(name + "-uv-array").c_str());
	uvs_float_array->setCharData(write_float_array(uv).c_str());
	uvs_float_array->setAttribute("count",int_to_string(uv.size()).c_str());
	add_accessor(uvs,name + "-uv-array",uv.size()/2,true);

	return result;
}

void DAESaver::add_refs(daeElement *mesh, string name, vector<int> refs, vector<int> sizes, daeElement *node, int texture) {
	daeElement *polylist;
	bool tri = true;
	for (unsigned int i = 0; i < sizes.size(); i++) {
		if (sizes[i] != 3) {
			tri = false;
			break;
		}
	}
	if (tri) {
		polylist = mesh->add("triangles");
	} else {
		polylist = mesh->add("polylist");
	}
	if (texture > -1 && texture < num_textures) {
		polylist->setAttribute("material",add_material(texture,node).c_str());
	}
	daeElement *vert, *norm, *uv;
	vert = polylist->add("input");
	vert->setAttribute("offset","0");
	vert->setAttribute("semantic","VERTEX");
	vert->setAttribute("source", (string("#") + name + "-vertex").c_str());
	if (texture < num_textures + 1) {
		norm = polylist->add("input");
		norm->setAttribute("offset","1");
		norm->setAttribute("semantic","NORMAL");
		norm->setAttribute("source", (string("#") + name + "-normals").c_str());
	}
	if (texture > -1) {
		uv = polylist->add("input");
		if (texture < num_textures + 1) {
			uv->setAttribute("offset","2");
		} else {
			uv->setAttribute("offset","1");
		}
		uv->setAttribute("semantic","TEXCOORD");
		uv->setAttribute("source", (string("#") + name + "-uv").c_str());
	}
	if (!tri) {
		daeElement *vcount = polylist->add("vcount");
		vcount->setCharData(write_int_array(sizes).c_str());
	}

	daeElement *p = polylist->add("p");
	p->setCharData(write_int_array(refs).c_str());
	polylist->setAttribute("count",int_to_string(sizes.size()).c_str());
}

void DAESaver::add_turret_fps() {
	daeElement *element;
	stringstream name;
	pcs_turret *turret;
	daeElement *helper;
	for (int i = 0; i < model->GetTurretCount(); i++) {
		turret = &model->Turret(i);
		helper = find_helper(subobjs[turret->sobj_par_phys]);
		if (helper == NULL) {
			helper = scene;
		}
		helper = helper->add("node");
		name.str("");
		if (turret->sobj_parent == turret->sobj_par_phys) {
			name << "firepoints" << i;
		} else {
			name << "multifirepoints" << i;
		}
		helper->setAttribute("id",name.str().c_str());
		helper->setAttribute("name",name.str().c_str());
		for (unsigned int j = 0; j < turret->fire_points.size(); j++) {
			name.str("");
			name << model_subobjs[0][turret->sobj_parent].name.c_str() << "-" << setfill ('0') << setw (2) << (j + 1);
			element = helper->add("node");
			element->setAttribute("id",name.str().c_str());
			element->setAttribute("name",name.str().c_str());
			write_transform(element, turret->fire_points[j], turret->turret_normal, vector3d(0,1,0));
		}
	}
}

void DAESaver::add_docks() {
	daeElement *element;
	daeElement *group;
	stringstream name;
	pcs_dock_point *dockpoint;
	for (int i = 0; i < model->GetDockingCount(); i++) {
		dockpoint = &model->Dock(i);
		name.str("");
		name << "dockpoint" << setfill ('0') << setw (2) << (i+1);
		group = scene->add("node");
		group->setAttribute("id",name.str().c_str());
		group->setAttribute("name",name.str().c_str());
		add_helper(group,dockpoint->properties);
		docks.push_back(group);

		for (unsigned int j = 0; j < dockpoint->dockpoints.size(); j++) {
			name.str("");
			name << "dockpoint" << setfill ('0') << setw (2) << (i+1) << "-" << setfill ('0') << setw (2) << (j+1);
			element = group->add("node");
			element->setAttribute("id",name.str().c_str());
			element->setAttribute("name",name.str().c_str());
			write_transform(element,dockpoint->dockpoints[j].point,dockpoint->dockpoints[j].norm,vector3d(0,1,0));
		}
	}
}

void DAESaver::add_thrusters() {
	daeElement *element;
	stringstream name;
	pcs_thruster *thruster;
	daeElement *helper;
	string engine_name;
	vector3d offset,scale_vec;
	for (int i = 0; i < model->GetThrusterCount(); i++) {
		helper = NULL;
		thruster = &model->Thruster(i);
		engine_name = thruster->properties;
		if (!engine_name.empty() && boost::algorithm::istarts_with(engine_name, "$engine_subsystem=")) {
			engine_name = engine_name.substr(engine_name.find("="));
			helper = find_subsystem(engine_name,offset,scale_vec);
		}
		if (helper == NULL) {
			helper = scene;
			scale_vec = vector3d(1,1,1);
		}
		helper = helper->add("node");
		helper->setAttribute("id","thrusters");
		helper->setAttribute("name","thrusters");


		for (unsigned int j = 0; j < thruster->points.size(); j++) {
			element = helper->add("node");
			element->setAttribute("id","thruster");
			element->setAttribute("name","thruster");
			write_transform(element, thruster->points[j].pos - offset, MakeUnitVector(thruster->points[j].norm), vector3d(0,0,1), thruster->points[j].radius, scale_vec.x);
		}
	}
}

void DAESaver::add_guns() {
	daeElement *element;
	stringstream name;
	pcs_slot *gunbank;
	daeElement *group;
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
		group = scene->add("node");
		group->setAttribute("id",name.str().c_str());
		group->setAttribute("name",name.str().c_str());
		for (unsigned int j = 0; j < gunbank->muzzles.size(); j++) {
			name.str("");
			if (gunbank->type == GUN) {
				name << "gunbank" << setfill ('0') << setw (2) << (guns);
			} else {
				name << "missilebank" << setfill ('0') << setw (2) << (missiles);
			}
			name << "-" << setfill ('0') << setw (2) << (j+1);
			element = group->add("node");
			element->setAttribute("id",name.str().c_str());
			element->setAttribute("name",name.str().c_str());
			write_transform(element,gunbank->muzzles[j].point,gunbank->muzzles[j].norm,vector3d(0,1,0));
		}
	}
}

void DAESaver::add_eyes() {
	for (int i = 0; i < model->GetEyeCount(); i++) {
		pcs_eye_pos& eye = model->Eye(i);
		stringstream name;
		name << "eyepoint" << setfill ('0') << setw (2) << (i+1);
		daeElement *element;
		if (eye.sobj_number == -1) {
			element = scene->add("node");
		} else {
			element = find_helper(subobjs[eye.sobj_number])->add("node");
		}
		element->setAttribute("id",name.str().c_str());
		element->setAttribute("name",name.str().c_str());
		write_transform(element,eye.sobj_offset,eye.normal,vector3d(0,1,0));
	}
}

void DAESaver::add_subsystems() {
	daeElement *element;
	daeElement *translate;
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
		element = scene->add("node");
		element->setAttribute("id",name.str().c_str());
		element->setAttribute("name",name.str().c_str());
		translate = element->add("translate");
		translate->setAttribute("sid","translate");
		translate->setCharData(write_vector3d(subsys->point).c_str());
		add_scale(element,radius_to_scale(subsys->radius));
		specials.push_back(element);
		add_helper(element,subsys->properties);
	}
}

void DAESaver::add_shield() {
	if (model->GetShldTriCount() == 0) {
		return;
	}
	map<vector3d,int,bool(*)(const vector3d&, const vector3d&)> vert_map(vector3d_comparator),norm_map(vector3d_comparator);
	int vert_idx = 0,norm_idx = 0;
	daeElement *element = scene->add("node");
	element->setAttribute("id","shield");
	element->setAttribute("name","shield");
	daeElement *temp = element->add("instance_geometry");
	temp->setAttribute("url","#shield-geometry");
	daeElement *geom = geoms->add("geometry");
	geom->setAttribute("id","shield-geometry");
	geom->setAttribute("name","shield-geometry");
	vector<float> vert,norm;
	vector<int> ref,size;
	pcs_shield_triangle tri;
	for (int i = 0; i < model->GetShldTriCount(); i++) {
		tri = model->ShldTri(i);
		if (norm_map.find(tri.face_normal) == norm_map.end()) {
			norm_map.insert(pair<vector3d,int>(tri.face_normal, norm_map.size()));
			norm.push_back(-tri.face_normal.x);
			norm.push_back(tri.face_normal.z);
			norm.push_back(tri.face_normal.y);
		}
		norm_idx = norm_map[tri.face_normal];

		for (int j = 2; j >= 0; j--) {
			if (vert_map.find(tri.corners[j]) == vert_map.end()) {
				vert_map.insert(pair<vector3d,int>(tri.corners[j], vert_map.size()));
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
	daeElement *mesh = geom->add("mesh");
	add_refs(mesh,"shield",ref,size);
	daeElement *pos = mesh->add("source");
	pos->setAttribute("id","shield-position");
	daeElement *pos_float_array = pos->add("float_array");
	pos_float_array->setAttribute("id","shield-position-array");
	pos_float_array->setCharData(write_float_array(vert).c_str());
	pos_float_array->setAttribute("count",int_to_string(vert.size()).c_str());
	add_accessor(pos,"shield-position-array",vert.size()/3);

	daeElement *norms = mesh->add("source");
	norms->setAttribute("id","shield-normals");
	daeElement *norms_float_array = norms->add("float_array");
	norms_float_array->setAttribute("id","shield-normals-array");
	norms_float_array->setCharData(write_float_array(norm).c_str());
	norms_float_array->setAttribute("count",int_to_string(norm.size()).c_str());
	add_accessor(norms,"shield-normals-array",norm.size()/3);

	daeElement *verts = mesh->add("vertices");
	verts->setAttribute("id","shield-vertex");
	daeElement *vert_input = verts->add("input");
	vert_input->setAttribute("semantic", "POSITION");
	vert_input->setAttribute("source", "#shield-position");
}

void DAESaver::add_paths() {
	pcs_path *path;
	daeElement *element;
	daeElement *translate;
	daeElement *helper;
	stringstream name;
	vector3d offset(0,0,0);
	vector3d scale_vec;
	for (int i = 0; i < model->GetPathCount(); i++) {
		path = &model->Path(i);
		helper = find_subsystem(path->parent,offset,scale_vec);
		if (helper == NULL) {
			helper = find_dockpoint(i,offset);
		}
		if (helper == NULL) {
			helper = scene;
		}
		name.str("");
		name << (path->name.c_str() + 1);
		helper = helper->add("node");
		helper->setAttribute("id",name.str().c_str());
		helper->setAttribute("name",name.str().c_str());
		for (unsigned int j = 0; j < path->verts.size(); j++) {
			name.str("");
			name << (path->name.c_str() + 1) << "-" << setw(2) << setfill('0') << (j+1);
			element = helper->add("node");
			element->setAttribute("id",name.str().c_str());
			element->setAttribute("name",name.str().c_str());
			translate = element->add("translate");
			translate->setAttribute("sid","translate");
			translate->setCharData(write_vector3d(path->verts[j].pos - offset,scale_vec).c_str());
			add_scale(element,radius_to_scale(path->verts[j].radius), scale_vec);
		}
	}
}

void DAESaver::add_glows() {
	pcs_glow_array glows;
	pcs_thrust_glow glow;
	string name;
	daeElement *element;
	daeElement *glowpoint;
	daeElement *trans;
	
	for (int i = 0; i < model->GetLightCount(); i++) {
		glows = model->Light(i);
		element = find_helper(subobjs[glows.obj_parent])->add("node");
		element->setAttribute("id","glowbank");
		element->setAttribute("name","glowbank");
		std::stringstream fields;
		fields << glows.properties;
		fields << endl << "type=" << glows.type;
		fields << endl << "on=" << glows.on_time;
		fields << endl << "off=" << glows.off_time;
		fields << endl << "displacement=" << glows.disp_time;
		fields << endl << "lod=" << glows.LOD;
		add_helper(element, fields.str());
		for (unsigned int j = 0; j < glows.lights.size(); j++) {
			glow = glows.lights[j];
			glowpoint = element->add("node");
			glowpoint->setAttribute("id","glowpoint");
			glowpoint->setAttribute("name","glowpoint");
			add_scale(glowpoint,radius_to_scale(glow.radius));
			trans = glowpoint->add("translate");
			trans->setAttribute("sid","translate");
			trans->setCharData(write_vector3d(glow.pos).c_str());
		}
	}
}

void DAESaver::add_insignia() {
	map<vector3d,int,bool(*)(const vector3d&, const vector3d&)> vert_map(vector3d_comparator);
	map<pair<float,float>,int,bool(*)(const pair<float,float>&, const pair<float,float>&)> uv_map(float_pair_comparator);
	int vert_idx = 0,uv_idx = 0;
	vector<int>counts(model->GetLODCount(), 1);

	pcs_insig insignia;
	stringstream name;
	daeElement *element;
	daeElement *translate;
	pcs_insig_face face;
	for (int i = 0; i < model->GetInsigniaCount(); i++) {
		insignia = model->Insignia(i);
		name.str("");
		name << "insigLOD" << setfill ('0') << setw (2) << insignia.lod + 1 << "-" << setfill ('0') << setw (2) << counts[insignia.lod];
		counts[insignia.lod]++;
		element = scene->add("node");
		element->setAttribute("id", name.str().c_str());
		element->setAttribute("name", name.str().c_str());
		translate = element->add("translate");
		translate->setCharData(write_vector3d(insignia.offset).c_str());


		daeElement *temp = element->add("instance_geometry");
		temp->setAttribute("url",(string("#") + name.str().c_str() + "-geometry").c_str());
		daeElement *geom = geoms->add("geometry");
		geom->setAttribute("id",(name.str() + "-geometry").c_str());
		geom->setAttribute("name",(name.str() + "-geometry").c_str());
		vector<float> vert, uv;
		vector<int> ref,size;
		vert_map.clear();
		uv_map.clear();
		for (unsigned int i = 0; i < insignia.faces.size(); i++) {
			face = insignia.faces[i];
			for (int j = 2; j >= 0; j--) {
				if (vert_map.find(face.verts[j]) == vert_map.end()) {
					vert_map.insert(pair<vector3d,int>(face.verts[j], vert_map.size()));
					vert.push_back(-face.verts[j].x);
					vert.push_back(face.verts[j].z);
					vert.push_back(face.verts[j].y);
				}
				vert_idx = vert_map[face.verts[j]];
				if (uv_map.find(pair<float,float>(face.u[j],face.v[j])) == uv_map.end()) {
					uv_map.insert(pair<pair<float,float>,int>(pair<float,float>(face.u[j],face.v[j]), uv_map.size()));
					uv.push_back(face.u[j]);
					uv.push_back(1.0 - face.v[j]);
				}
				uv_idx = uv_map[pair<float,float>(face.u[j],face.v[j])];



				ref.push_back(vert_idx);
				ref.push_back(uv_idx);
			}
			size.push_back(3);
		}
		daeElement *mesh = geom->add("mesh");
		add_refs(mesh,name.str().c_str(),ref,size,NULL, num_textures + 1);
		daeElement *pos = mesh->add("source");
		pos->setAttribute("id",(name.str() + "-position").c_str());
		daeElement *pos_float_array = pos->add("float_array");
		pos_float_array->setAttribute("id",(name.str() + "-position-array").c_str());
		pos_float_array->setCharData(write_float_array(vert).c_str());
		pos_float_array->setAttribute("count",int_to_string(vert.size()).c_str());
		add_accessor(pos,(name.str() + "-position-array").c_str(),vert.size()/3);

		daeElement *uvs = mesh->add("source");
		uvs->setAttribute("id",(name.str() + "-uv").c_str());
		daeElement *uvs_float_array = uvs->add("float_array");
		uvs_float_array->setAttribute("id",(name.str() + "-uv-array").c_str());
		uvs_float_array->setCharData(write_float_array(uv).c_str());
		uvs_float_array->setAttribute("count",int_to_string(uv.size()).c_str());
		add_accessor(uvs,(name.str() + "-uv-array").c_str(),uv.size()/2,true);

		daeElement *verts = mesh->add("vertices");
		verts->setAttribute("id",(name.str() + "-vertex").c_str());
		daeElement *vert_input = verts->add("input");
		vert_input->setAttribute("semantic", "POSITION");
		vert_input->setAttribute("source", (string("#") + name.str().c_str() + "-position").c_str());
	}
}

void DAESaver::add_center_of_mass() {
	const vector3d& com = model->GetCenterOfMass();
	if (com == vector3d())
		return;
	daeElement *center_of_mass = scene->add("node");
	if (center_of_mass != NULL) {
		center_of_mass->setAttribute("id", "com");
		center_of_mass->setAttribute("name", "com");
		daeElement* translate = center_of_mass->add("translate");
		if (translate != NULL) {
			translate->setAttribute("sid","translate");
			translate->setCharData(write_vector3d(com));
		}
	}
}

void DAESaver::add_autocentering() {
	const vector3d& acen = model->GetAutoCenter();
	if (acen == vector3d())
		return;
	daeElement *autocentering = scene->add("node");
	if (autocentering != NULL) {
		autocentering->setAttribute("id", "acen");
		autocentering->setAttribute("name", "acen");
		daeElement* translate = autocentering->add("translate");
		if (translate != NULL) {
			translate->setAttribute("sid","translate");
			translate->setCharData(write_vector3d(acen));
		}
	}
}

void DAESaver::add_mass() {
	daeElement *mass = scene->add("node");
	if (mass != NULL) {
		mass->setAttribute("id", "mass");
		mass->setAttribute("name", "mass");
		mass = mass->add("node");
		if (mass != NULL) {
			stringstream stream;
			stream << model->GetMass();
			mass->setAttribute("id", stream.str().c_str());
			mass->setAttribute("name", stream.str().c_str());
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
		daeElement *moi = scene->add("node");
		if (moi != NULL) {
			moi->setAttribute("id", "moi");
			moi->setAttribute("name", "moi");
			moi = moi->add("node");
			if (moi != NULL) {
				moi->setAttribute("id", stream.str().c_str());
				moi->setAttribute("name", stream.str().c_str());
			}
		}
	}
}

string DAESaver::add_material(int idx,daeElement *node) {

	daeElement *instance = node->getDescendant("technique_common");
	instance = instance->add("instance_material");
	instance->setAttribute("symbol",(model->Texture(idx) + "-mat").c_str());
	instance->setAttribute("target",(string("#") + model->Texture(idx) + "-mat").c_str());
	instance = instance->add("bind_vertex_input");
	instance->setAttribute("input_semantic","TEXCOORD");
	instance->setAttribute("input_set","1");
	instance->setAttribute("semantic","CHANNEL1");

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

daeElement* DAESaver::add_helper(daeElement *element,string properties) {
	if (export_helpers) {
		if (properties.size() > 0 && strlen(properties.c_str()) > 0) {
			daeElement *helper = element->add("node");
			daeElement *props;
			stringstream temp;

			string result;
			string properti;
			string previous;
			helper->setAttribute("id", "helper");
			helper->setAttribute("name", "helper");
			filter_string(properties, "$rotate");
			filter_string(properties, "$uvec");
			filter_string(properties, "$fvec");
			trim_extra_spaces(properties);

			if (props_as_helpers) {
				props = helper->add("node");
				props->setAttribute("id","properties");
				props->setAttribute("name","properties");
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
				props = element->add("extra")->add("technique");
				props->setAttribute("profile","FCOLLADA");
				props = props->add("user_properties");
				props->setCharData(properties.c_str());

			}
			return helper;
		}
	}
	return NULL;
}


void DAESaver::add_property(daeElement *props, const char* prop) {
	daeElement *current_prop;
	current_prop = props->add("node");
	current_prop->setAttribute("id",prop);
	current_prop->setAttribute("name",prop);
}

daeElement *DAESaver::find_subsystem(string name, vector3d &offset, vector3d &scale) {
	daeElement *answer = NULL;
	offset = vector3d(0,0,0);
	scale = vector3d(1,1,1);
	for (unsigned int i = 0; i < subobjs.size(); i++) {
		if (boost::algorithm::iequals(model->SOBJ(i).name, name)) {
			offset = relative_to_absolute(vector3d(0,0,0),&model->SOBJ(i),model_subobjs);
			answer = subobjs[i];
			break;
		}
	}
	if (answer == NULL) {
		for (int i = 0; i < model->GetSpecialCount(); i++) {
			if (boost::algorithm::iequals(model->Special(i).name, name)) {
				offset = model->Special(i).point;
				scale = radius_to_scale(model->Special(i).radius);
				answer = specials[i];
				break;
			}
		}
	}
	if (answer == NULL) {
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
		if (answer == NULL) {
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

daeElement *DAESaver::find_helper(daeElement *element) {
	if (element == NULL) return NULL;
	daeTArray< daeSmartRef<daeElement> > helpers = element->getChildren();
	for (unsigned int i = 0; i < helpers.getCount(); i++) {
		if (boost::algorithm::istarts_with(helpers[i]->getAttribute("id"), "helper")) {
			return helpers[i];
		}
	}
	daeElement *temp = element->add("node");
	//stringstream name;
	//num_helpers++;
	//name << "helper" << num_helpers << endl;
	temp->setAttribute("id","helper");
	temp->setAttribute("name","helper");
	return temp;
}

daeElement *DAESaver::find_dockpoint(int idx,vector3d &offset) {
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
	return NULL;
}

void add_accessor(daeElement *element, string source, int count, bool uv) {
	daeElement *accessor = element->add("technique_common accessor");
	accessor->setAttribute("count",int_to_string(count).c_str());
	accessor->setAttribute("source", (string("#") + source).c_str());
	if (uv) {
		accessor->setAttribute("stride","2");
		add_param(accessor,"S");
		add_param(accessor,"T");
	} else {
		accessor->setAttribute("stride","3");
		add_param(accessor,"X");
		add_param(accessor,"Y");
		add_param(accessor,"Z");
	}
}

void add_param(daeElement *accessor, string name) {
	daeElement *temp = accessor->add("param");
	temp->setAttribute("type","float");
	temp->setAttribute("name",name.c_str());
}

string write_vector3d(vector3d vec,vector3d scale) {
	stringstream output;
	output << -vec.x / scale.x << " " << vec.z / scale.z << " " << vec.y / scale.y;
	return string(output.str().c_str());

}

void DAESaver::write_transform(daeElement *element, const vector3d& offset, const vector3d& norm, const vector3d& base, float scale, float external_scale) {
	write_transform_binormal(element, offset, norm, vector3d(1, 0, 0), base, scale, external_scale);
}

void DAESaver::write_transform_binormal(daeElement *element, const vector3d& offset, const vector3d& norm, const vector3d& binorm, const vector3d& base, float scale, float external_scale) {
	element = element->add("matrix");
	element->setAttribute("sid","matrix");
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
	element->setCharData(output.str().c_str());
}

bool vector3d_comparator(const vector3d& a, const vector3d& b) {
	if (a.x != b.x) {
		return a.x < b.x;
	}
	if (a.y != b.y) {
		return a.y < b.y;
	}
	if (a.z != b.z) {
		return a.z < b.z;
	}
	return false;
}

bool float_pair_comparator(const pair<float,float>& a, const pair<float,float>& b) {
	if (a.first != b.first) {
		return a.first < b.first;
	}
	if (a.second != b.second) {
		return a.second < b.second;
	}
	return false;
}

vector3d radius_to_scale(float radius) {
	return vector3d(radius,radius,radius);
}

void add_scale(daeElement *element, vector3d scale, vector3d parent_scale) {
	element = element->add("scale");
	element->setAttribute("sid","scale");
	scale.x = -scale.x;
	element->setCharData(write_vector3d(scale, parent_scale).c_str());
}

void add_texture_mappings(daeElement *element, map<string, string> *mapping) {
	if (element == NULL) {
		return;
	}
	element = element->getChild("bind_material");
	if (element == NULL) {
		return;
	}
	element = element->getChild("technique_common");
	if (element == NULL) {
		return;
	}
	daeTArray< daeSmartRef<daeElement> > children = element->getChildren();
	for (unsigned int i = 0; i < children.getCount(); i++) {
		if (boost::algorithm::equals(children[i]->getTypeName(), "instance_material") && mapping->find(children[i]->getAttribute("symbol")) == mapping->end()) {
			mapping->insert(pair<string, string>(children[i]->getAttribute("symbol"), children[i]->getAttribute("target")));
		}
	}
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

std::string get_name(daeElement* element) {
	std::string name = element->getAttribute("name");
	if (name.empty()) {
		name = element->getID();
	}
	return name;
}
