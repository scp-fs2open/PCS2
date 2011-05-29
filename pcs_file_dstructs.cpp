//****************************************************************************
//	pcs_file_dstructs.cpp
//	POF Constructor Suite 2 File Format data structure functions
//****************************************************************************

/*
 * $Logfile: /pcs_file_dstructs.cpp $
 * $Revision: 1.6 $
 * $Date: 2008/02/09 22:28:09 $
 * $Author: kazan $
 *
 * $Log: pcs_file_dstructs.cpp,v $
 * Revision 1.6  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.5  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.4  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.3  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.2  2005/08/23 22:36:42  kazan
 * s/std::{vector:string}/kaz_{vector:string}
 * renderer motion works
 * location PMF::LoadFromPOF bottleneck
 *
 * Revision 1.1.1.1  2004/12/31 06:19:41  kazan
 * no message
 *
 *
 *
 */

#include <cfloat>
#include <set>
#include <boost/algorithm/string.hpp>

#include "pcs_file.h"
#include "pcs_file_dstructs.h"
#include "matrix3d.h"


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void BF_ReadString(std::istream &in, std::string &retval)
{
	int len;

	in.read((char *)&len, sizeof(int));

	char *str = new char[len+1];
	in.read(str,len);
	str[len] = '\0';
	
	retval = str;
	delete[] str;


}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void BF_WriteString(std::ostream &out, std::string &str)
{
	int len = str.length();

	out.write((char *)&len, sizeof(int));
	out.write(str.c_str(), len);
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_vertex::Read(std::istream& in, int ver)
{
	BFRead(point, vector3d)
	BFRead(norm, vector3d)
	BFRead(u, float)
	BFRead(v, float)
	if (ver >= 101) {
		BFRead(facet_angle, float)
	} else { facet_angle = -1; }
}

void pcs_vertex::Write(std::ostream& out)
{
	BFWrite(point, vector3d)
	BFWrite(norm, vector3d)
	BFWrite(u, float)
	BFWrite(v, float)
	BFWrite(facet_angle, float)
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_polygon::Read(std::istream& in, int ver)
{

	if (ver >= 101)
		BFReadAdvVector(verts)
	else
		BFReadVector(verts)
	BFRead(texture_id, int)
	BFRead(norm, vector3d)
}

void pcs_polygon::Write(std::ostream& out)
{

	BFWriteAdvVector(verts)
	BFWrite(texture_id, int)
	BFWrite(norm, vector3d)

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_sobj::Write(std::ostream& out)
{
	//BF_WriteString
	BFWrite(parent_sobj, int)
	BFWrite(radius, float)
	BFWrite(offset, vector3d)
	BFWrite(geometric_center, vector3d)
	BFWrite(bounding_box_min_point, vector3d)
	BFWrite(bounding_box_max_point, vector3d)
	BFWriteString(name);
	BFWriteString(properties);
	BFWrite(movement_type, int)
	BFWrite(movement_axis, int)

	int cnt = polygons.size();
	BFWrite(cnt, int)
	for (int i = 0; i < cnt; i++)
		polygons[i].Write(out);
}

void pcs_sobj::Read(std::istream &in, int ver)
{
	BFRead(parent_sobj, int)
	BFRead(radius, float)
	BFRead(offset, vector3d)
	BFRead(geometric_center, vector3d)
	BFRead(bounding_box_min_point, vector3d)
	BFRead(bounding_box_max_point, vector3d)
	BFReadString(name);
	BFReadString(properties);
	BFRead(movement_type, int)
	BFRead(movement_axis, int)

	int cnt;
	BFRead(cnt, int)
	polygons.resize(cnt);
	for (int i = 0; i < cnt; i++)
		polygons[i].Read(in, ver);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_special::Read(std::istream& in, int ver)
{
	BFReadString(name)
	BFReadString(properties)
	BFRead(point, vector3d)
	BFRead(radius, float)
}

void pcs_special::Write(std::ostream& out)
{
	BFWriteString(name)
	BFWriteString(properties)
	BFWrite(point, vector3d)
	BFWrite(radius, float)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_slot::Read(std::istream& in, int ver)
{
	BFRead(type, int)
	BFReadVector(muzzles)
}

void pcs_slot::Write(std::ostream& out)
{
	BFWrite(type, int)
	BFWriteVector(muzzles)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_turret::Read(std::istream& in, int ver)
{	
	BFRead(type, int)
	BFRead(sobj_parent, int)
	BFRead(sobj_par_phys, int)
	BFRead(turret_normal, vector3d)
	BFReadVector(fire_points)
}

void pcs_turret::Write(std::ostream& out)
{	
	BFWrite(type, int)
	BFWrite(sobj_parent, int)
	BFWrite(sobj_par_phys, int)
	BFWrite(turret_normal, vector3d)
	BFWriteVector(fire_points)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_dock_point::Read(std::istream& in, int ver)
{
	BFReadString(properties)
	BFReadVector(paths)
	BFReadVector(dockpoints)
}

void pcs_dock_point::Write(std::ostream& out)
{
	BFWriteString(properties)
	BFWriteVector(paths)
	BFWriteVector(dockpoints)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_thruster::Read(std::istream& in, int ver)
{
	BFReadVector(points)
	BFReadString(properties)
}

void pcs_thruster::Write(std::ostream& out)
{
	BFWriteVector(points)
	BFWriteString(properties)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_insig::Read(std::istream& in, int ver)
{
	BFRead(lod, int)
	BFRead(offset, vector3d)
	BFReadVector(faces)
}

void pcs_insig::Write(std::ostream& out)
{
	BFWrite(lod, int)
	BFWrite(offset, vector3d)
	BFWriteVector(faces)
}

bool pcs_insig::Generate(const std::vector<pcs_polygon>& polygons, const float epsilon)
{
	if (!(generator.up != vector3d() && generator.forward != vector3d() &&
				generator.radius > 0.0f && generator.subdivision > 0)) {
		return false;
	}
	vector3d forward = MakeUnitVector(generator.forward);
	vector3d up = MakeUnitVector(
			generator.up - (dot(generator.up, forward) * forward));
	vector3d right = CrossProduct(forward, up);
	matrix transform(up, right, forward);
	transform = transform * (2.0f / generator.radius);

	std::vector<std::vector<vector3d> > polys;
	for (std::vector<pcs_polygon>::const_iterator it = polygons.begin(); it != polygons.end(); ++it) {
		if (dot(it->norm, forward) >= 0) {
			continue;
		}
		std::vector<vector3d> transformed;
		for (std::vector<pcs_vertex>::const_iterator jt = it->verts.begin(); jt != it->verts.end(); ++jt) {
			transformed.push_back(transform * (jt->point - generator.pos));
		}
		if (!outside_viewport(transformed)) {
			polys.push_back(transformed);
		}
	}
	std::vector<float> zbuffer(generator.subdivision * generator.subdivision, FLT_MAX);
	std::vector<int> zbuffer_idx(generator.subdivision * generator.subdivision, -1);
	for (unsigned int j = 0; j < polys.size(); j++) {
		for (int i = 0; i < generator.subdivision; i++) {
			for (int k = 0; k < generator.subdivision; k++) {
				vector3d point(i * 2.0f / (generator.subdivision) - 1,
						k * 2.0f / (generator.subdivision) - 1, 0);
				if (inside_polygon(point, polys[j])) {
					float result = interpolate_z(point, polys[j]);
					if (result < zbuffer[i * generator.subdivision + k]) {
						zbuffer[i * generator.subdivision + k] = result;
						zbuffer_idx[i * generator.subdivision + k] = j;
					}
				}
			}
		}
	}
	std::set<unsigned int> included_polys;
	for (unsigned int i = 0; i < zbuffer_idx.size(); i++) {
		if (zbuffer_idx[i] != -1) {
			included_polys.insert(zbuffer_idx[i]);
		}
	}
	transform = transform.invert();
	pcs_insig_face face;
	vector3d min_bounding_box, max_bounding_box;
	bool merged = true;
	while (merged) {
		merged = false;
		for (std::set<unsigned int>::const_iterator it = included_polys.begin(); it != included_polys.end(); ++it) {
			std::set<unsigned int>::const_iterator jt = it;
		   	++jt;
			for (; jt != included_polys.end(); ++jt) {
				if (dot(MakeUnitVector(CrossProduct(polys[*it][1] - polys[*it][0],
									polys[*it][2] - polys[*it][0])),
							MakeUnitVector(CrossProduct(polys[*jt][1] - polys[*jt][0],
									polys[*jt][2] - polys[*jt][0]))) > epsilon) {
					for (size_t k = 0; k < polys[*it].size() && !merged; k++) {
						for (size_t l = 0; l < polys[*jt].size() && !merged; l++) {
							if (polys[*it][(k + 1) % polys[*it].size()] == polys[*jt][l] &&
									polys[*it][k] == polys[*jt][(l + 1) % polys[*jt].size()]) {
								// TODO: test for convexity
								merged = true;
								polys[*it].resize(polys[*it].size() + polys[*jt].size() - 2);
								// Shift vertices after the break out of the way.
								for (size_t i = polys[*it].size() - 1; i > k + 1; i--) {
									polys[*it][i] = polys[*it][i - polys[*jt].size() + 2];
								}
								// Copy vertices from poly being merged, in order.
								int i = k + 1;
								for (size_t j = (l + 2) % polys[*jt].size(); j != l; j = (j + 1) % polys[*jt].size()) {
									polys[*it][i] = polys[*jt][j];
									i++;
								}
								included_polys.erase(jt);
							}
						}
					}
				}
			}
		}
	}
	for (std::set<unsigned int>::const_iterator it = included_polys.begin(); it != included_polys.end(); ++it) {
		std::vector<vector3d> clipped_poly(clip(polys[*it]));
		for (unsigned int i = 0; i < clipped_poly.size() - 2; i++) {
			face.verts[0] = generator.pos + (transform * clipped_poly[i]);
			face.u[0] = (1.0f + clipped_poly[i].y) / 2.0f;
			face.v[0] = (1.0f -clipped_poly[i].x) / 2.0f;
			face.verts[1] = generator.pos + (transform * clipped_poly[i + 1]);
			face.u[1] = (1.0f + clipped_poly[i + 1].y) / 2.0f;
			face.v[1] = (1.0f -clipped_poly[i + 1].x) / 2.0f;
			face.verts[2] = generator.pos + (transform * clipped_poly[clipped_poly.size() - 1]);
			face.u[2] = (1.0f + clipped_poly[clipped_poly.size() - 1].y) / 2.0f;
			face.v[2] = (1.0f -clipped_poly[clipped_poly.size() - 1].x) / 2.0f;
			faces.push_back(face);
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					if (face.verts[j][k] > max_bounding_box[k]) {
						max_bounding_box[k] = face.verts[j][k];
					}
					if (face.verts[j][k] < min_bounding_box[k]) {
						min_bounding_box[k] = face.verts[j][k];
					}
				}
			}
		}
	}
	vector3d center = min_bounding_box + ((max_bounding_box - min_bounding_box) / 2.0f);
	offset = center - forward * (generator.distance);
	for (unsigned int i = 0; i < faces.size(); i++) {
		for (int j = 0; j < 3; j++) {
			faces[i].verts[j] -= center;
		}
	}
	return true;
}

namespace {
	class edge {
		float a, b, c;
		public:
		edge(float a, float b, float c) : a(a), b(b), c(c) {}

		bool inside(const vector3d& v) const {
			return a * v.x + b*v.y + c >= 0;
		}

		vector3d intersection(const vector3d& u, const vector3d& v) const {
			vector3d direction = v - u;
			float t = (a * u.x + b*u.y + c) / (a * (u.x - v.x) + b * (u.y - v.y));
			return u + (t * direction);
		}
	};

	std::vector<vector3d> clip_edge(const std::vector<vector3d> verts, const edge& current_edge) {
		std::vector<vector3d> result;
		unsigned int i;
		int j;
		for (i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
			if (current_edge.inside(verts[i])) {
				if (!current_edge.inside(verts[j])) {
					result.push_back(current_edge.intersection(verts[i], verts[j]));
				}
				result.push_back(verts[i]);
			} else if (current_edge.inside(verts[j])) {
				result.push_back(current_edge.intersection(verts[i], verts[j]));
			}
		}
		return result;
	}

}

std::vector<vector3d> pcs_insig::clip(const std::vector<vector3d>& verts) {
	return clip_edge(clip_edge(clip_edge(clip_edge(verts, edge(1, 0, 1)), edge(-1, 0, 1)), edge(0, 1, 1)), edge(0, -1, 1));

}

float pcs_insig::interpolate_z(const vector3d& v, const std::vector<vector3d>& verts) {
	if (verts.size() < 3) {
		return FLT_MAX;
	}
	matrix transform(verts[0], verts[1], verts[2]);
	for (int i = 0; i < 3; i++) {
		transform.a2d[i][2] = 1.0f;
	}
	transform = transform.invert();
	vector3d z(verts[0].z, verts[1].z, verts[2].z);
	return dot(transform * z, vector3d(v.x, v.y, 1));
}

bool pcs_insig::inside_polygon(const vector3d& v, const std::vector<vector3d>& verts) {
	bool result = false;
	unsigned int i;
	int j;
	for (i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
		if ((verts[i].y > v.y) != (verts[j].y > v.y) &&
				(v.x < (verts[j].x-verts[i].x) * (v.y-verts[i].y) / (verts[j].y-verts[i].y) + verts[i].x)) {
			result = !result;
		}
	}
	return result;
}

bool pcs_insig::outside_viewport(const std::vector<vector3d>& verts) {
	bool abovex(true), abovey(true), belowx(true), belowy(true), belowz(true);
	for (std::vector<vector3d>::const_iterator it = verts.begin(); it != verts.end(); ++it) {
		if (it->x >= -1.0f) {
			belowx = false;
		}
		if (it->x <= 1.0f) {
			abovex = false;
		}
		if (it->y >= -1.0f) {
			belowy = false;
		}
		if (it->y <= 1.0f) {
			abovey = false;
		}
		if (it->z >= 0.0f) {
			belowz = false;
		}
	}
	return abovex || abovey || belowx || belowy || belowz;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_path::Read(std::istream& in, int ver)
{
	BFReadString(name)
	BFReadString(parent)
	BFReadVector(verts)
}

void pcs_path::Write(std::ostream& out)
{
	BFWriteString(name)
	BFWriteString(parent)
	BFWriteVector(verts)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  
void pcs_glow_array::Read(std::istream& in, int ver)
{
	BFRead(disp_time, int)
	BFRead(on_time, int)
	BFRead(off_time, int)
	BFRead(obj_parent, int)
	BFRead(LOD, int)
	BFRead(type, int)
	BFReadString(properties)
	BFReadVector(lights)
}

void pcs_glow_array::Write(std::ostream& out)
{
	BFWrite(disp_time, int)
	BFWrite(on_time, int)
	BFWrite(off_time, int)
	BFWrite(obj_parent, int)
	BFWrite(LOD, int)
	BFWrite(type, int)
	BFWriteString(properties)
	BFWriteVector(lights)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

#include <cstdio>
void pmf_bsp_cache::Read(std::istream& in, int ver)
{
	if (ver >= 102)
	{
		BFRead(bsp_size, int)
		if (bsp_size != 0) 
		{
			bsp_data.reset(new char[bsp_size]);
			// XXX FIXME: Reads sizeof(bsp_size): 4 bytes.
			BFRead(bsp_data, bsp_size) 
		}
		BFRead(changed, bool)
	}
}
	
void pmf_bsp_cache::Write(std::ostream& out)
{
	BFWrite(bsp_size, int)
	if (bsp_size != 0) {
		// XXX FIXME: Writes sizeof(bsp_size): 4 bytes.
		BFWrite(*bsp_data.get(), bsp_size) }
	BFWrite(changed, bool)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void pcs_sobj::Transform(PCS_Model& model, int idx, const matrix& transform, const vector3d& translation, bool transform_pivot, bool fixed_pivot) {
	if (parent_sobj == -1) {
		fixed_pivot = true;
	}
	TransformBefore(model, idx);
	TransformAfter(model, idx, transform, translation, transform_pivot, fixed_pivot);
}

void pcs_sobj::TransformBefore(PCS_Model& model, int idx) {
	vector3d global_offset(-1.0f * model.OffsetFromParent(idx));
	matrix transform;
	// Child subobjects:
	for (int i = 0; i < model.GetSOBJCount(); i++) {
		pcs_sobj& other = model.SOBJ(i);
		if (other.parent_sobj == idx) {
			other.TransformBefore(model, i);
		}
	}
	// Paths:
	for (int i = 0; i < model.GetPathCount(); i++) {
		pcs_path& path = model.Path(i);
		if (!path.parent.empty() && (boost::algorithm::iequals(path.parent, name) || boost::algorithm::iequals(path.parent.substr(1), name) || boost::algorithm::iequals(path.parent, name.substr(1)))) {
			path.Transform(transform, global_offset);
		}
	}
	// Glowpoints:
	for (int i = 0; i < model.GetLightCount(); i++) {
		pcs_glow_array& lights = model.Light(i);
		if (lights.obj_parent == idx) {
			lights.Transform(transform, global_offset);
		}
	}
}

void pcs_sobj::TransformAfter(PCS_Model& model, int idx, const matrix& transform, const vector3d& translation, bool transform_pivot, bool fixed_pivot) {
	// TODO: uvec, fvec
	// TODO: rotation axis...
	vector3d subtranslation = translation;
	if (transform_pivot) {
		offset = transform * offset;
	}
	if (!fixed_pivot) {
		offset += subtranslation;
		subtranslation = vector3d();
	}
	vector3d global_offset(model.OffsetFromParent(idx));
	bool should_reverse = transform.determinant() < 0.0f;
	header_data header = model.get_header();

	if (!polygons.empty()) {
		bounding_box_min_point = vector3d(FLT_MAX, FLT_MAX, FLT_MAX);
		bounding_box_max_point = vector3d(FLT_MIN, FLT_MIN, FLT_MIN);
		radius = 0.0f;
	}
	for (std::vector<pcs_polygon>::iterator it = polygons.begin(); it < polygons.end(); ++it) {
		it->norm = SafeMakeUnitVector(transform * it->norm);
		it->centeroid = transform * it->centeroid + subtranslation;
		for (std::vector<pcs_vertex>::iterator jt = it->verts.begin(); jt < it->verts.end(); ++jt) {
			jt->point = transform * jt->point + subtranslation;
			ExpandBoundingBoxes(bounding_box_max_point, bounding_box_min_point, jt->point);
			float point_radius = Magnitude(jt->point);
			if (point_radius > radius) {
				radius = point_radius;
			}
			// N.B.: Only expands global bounding boxes and radius.
			float point_global_radius = Magnitude(jt->point + global_offset);
			if (point_global_radius > header.max_radius) {
				header.max_radius = point_global_radius;
			}
			ExpandBoundingBoxes(header.max_bounding, header.min_bounding, jt->point + global_offset);
			jt->norm = SafeMakeUnitVector(transform * jt->norm);
		}
		if (should_reverse) {
			std::reverse(it->verts.begin(), it->verts.end());
		}
	}
	model.set_header(header);
	// So OffsetFromParent works correctly.
	model.SOBJ(idx) = *this;
	// Child subobjects:
	for (int i = 0; i < model.GetSOBJCount(); i++) {
		pcs_sobj& other = model.SOBJ(i);
		if (other.parent_sobj == idx) {
			other.TransformAfter(model, i, transform, subtranslation, true, false);
		}
	}
	// Turrets:
	for (int i = 0; i < model.GetTurretCount(); i++) {
		pcs_turret& turret = model.Turret(i);
		if (turret.sobj_par_phys == idx) {
			turret.Transform(transform, subtranslation);
		}
	}
	// Eyepoints:
	for (int i = 0; i < model.GetEyeCount(); i++) {
		pcs_eye_pos& eye = model.Eye(i);
		if (eye.sobj_number == idx) {
			eye.Transform(transform, subtranslation);
		}
	}
	// Paths:
	for (int i = 0; i < model.GetPathCount(); i++) {
		pcs_path& path = model.Path(i);
		if (!path.parent.empty() && (boost::algorithm::iequals(path.parent, name) || boost::algorithm::iequals(path.parent.substr(1), name) || boost::algorithm::iequals(path.parent, name.substr(1)))) {
			path.Transform(transform, subtranslation + global_offset);
		}
	}
	// Glowpoints:
	for (int i = 0; i < model.GetLightCount(); i++) {
		pcs_glow_array& lights = model.Light(i);
		if (lights.obj_parent == idx) {
			lights.Transform(transform, subtranslation + global_offset);
		}
	}
	model.SetObjectChanged(idx);
}

void pcs_turret::Transform(const matrix& transform, const vector3d& translation) {
	turret_normal = SafeMakeUnitVector(transform * turret_normal);
	for (std::vector<vector3d>::iterator it = fire_points.begin(); it < fire_points.end(); ++it) {
		*it = transform * *it + translation;
	}
}

void pcs_eye_pos::Transform(const matrix& transform, const vector3d& translation) {
	sobj_offset = transform * sobj_offset + translation;
	normal = SafeMakeUnitVector(transform * normal);
}

void pcs_glow_array::Transform(const matrix& transform, const vector3d& translation) {
	for (std::vector<pcs_thrust_glow>::iterator it = lights.begin(); it < lights.end(); ++it) {
		it->Transform(transform, translation);
	}
}

void pcs_thruster::Transform(const matrix& transform, const vector3d& translation) {
	for (std::vector<pcs_thrust_glow>::iterator it = points.begin(); it < points.end(); ++it) {
		it->Transform(transform, translation);
	}
}

void pcs_thrust_glow::Transform(const matrix& transform, const vector3d& translation) {
	float norm_before, norm_after;
	pos = transform * pos + translation;
	norm_before = Magnitude(norm);
	norm = transform * norm;
	norm_after = Magnitude(norm);
	norm = SafeMakeUnitVector(norm);
	if (norm_after > 1e-5) {
		radius *= sqrt(fabs(transform.determinant() / norm_after * norm_before));
	} else {
		radius = 0;
	}
}

void pcs_insig::Transform(const matrix& transform, const vector3d& translation) {
	offset = transform * offset + translation;
	for (std::vector<pcs_insig_face>::iterator it = faces.begin(); it < faces.end(); ++it) {
		it->Transform(transform, vector3d());
	}
	generator.Transform(transform, translation);
}

void pcs_insig_generator::Transform(const matrix& transform, const vector3d& translation) {
	pos = transform * pos + translation;
	forward = SafeMakeUnitVector(transform * forward);
	up = SafeMakeUnitVector(transform * up);
}

void pcs_insig_face::Transform(const matrix& transform, const vector3d& translation) {
	for(int i = 0; i < 3; i++) {
		verts[i] = transform * verts[i] + translation;
	}
	if (transform.determinant() < 0.0f) {
		std::reverse(verts, verts + 3);
	}
}

void pcs_path::Transform(const matrix& transform, const vector3d& translation) {
	for (std::vector<pcs_pvert>::iterator it = verts.begin(); it < verts.end(); ++it) {
		it->Transform(transform, translation);
	}
}

void pcs_pvert::Transform(const matrix& transform, const vector3d& translation) {
	pos = transform * pos + translation;
	radius *= pow(fabs(transform.determinant()), 1.0f / 3);
}

void pcs_dock_point::Transform(PCS_Model& model, const matrix& transform, const vector3d& translation) {
	for (std::vector<pcs_hardpoint>::iterator it = dockpoints.begin(); it < dockpoints.end(); ++it) {
		it->Transform(transform, translation);
	}
		for (std::vector<int>::iterator it = paths.begin(); it < paths.end(); ++it) {
			if (*it < model.GetPathCount() && *it >= 0) {
				model.Path(*it).Transform(transform, translation);
			}
		}
}

void pcs_shield_triangle::Transform(PCS_Model& model, const matrix& transform, const vector3d& translation) {
	face_normal = SafeMakeUnitVector(transform * face_normal);
	header_data header = model.get_header();
	for (int i = 0; i < 3; i++) {
		corners[i] = transform * corners[i] + translation;
		ExpandBoundingBoxes(header.max_bounding, header.min_bounding, corners[i]);
	}
	if (transform.determinant() < 0.0f) {
		std::reverse(corners, corners + 3);
	}
	model.set_header(header);
}

void pcs_hardpoint::Transform(const matrix& transform, const vector3d& translation) {
	norm = SafeMakeUnitVector(transform * norm);
	point = transform * point + translation;
}

void pcs_slot::Transform(const matrix& transform, const vector3d& translation) {
	for (std::vector<pcs_hardpoint>::iterator it = muzzles.begin(); it < muzzles.end(); ++it) {
		it->Transform(transform, translation);
	}
}

void pcs_special::Transform(PCS_Model& model, const matrix& transform, const vector3d& translation) {
	point = transform * point + translation;
	radius *= pow(fabs(transform.determinant()), 1.0f / 3);
	for (int i = 0; i < model.GetPathCount(); i++) {
		pcs_path& path = model.Path(i);
		if (!path.parent.empty() && (boost::algorithm::iequals(path.parent, name) || boost::algorithm::iequals(path.parent.substr(1), name) || boost::algorithm::iequals(path.parent, name.substr(1)))) {
			path.Transform(transform, translation);
		}
	}
}
