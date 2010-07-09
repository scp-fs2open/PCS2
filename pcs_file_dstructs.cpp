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

bool pcs_insig::Generate(const std::vector<pcs_polygon> polygons)
{
	if (!(generator.up != vector3d() && generator.forward != vector3d() && generator.radius > 0.0f)) {
		return false;
	}
	vector3d forward = MakeUnitVector(generator.forward);
	vector3d up = MakeUnitVector(
			generator.up - (dot(generator.up, forward) * forward));
	vector3d right = CrossProduct(forward, up);
	float radius = generator.radius / 2;
	std::vector<vector3d> points;
	points.reserve((generator.subdivision + 1) * (generator.subdivision + 1));
	for (int i = 0; i <= generator.subdivision + 1; i++) {
		for (int j = 0; j <= generator.subdivision + 1; j++) {
			points.push_back(vector3d(i * 2.0f / (generator.subdivision + 1) - 1, j * 2.0f / (generator.subdivision + 1) - 1, 0));
		}
	}
	matrix transform(up, right, forward);
	transform = transform * (1 / radius);

	std::vector<std::vector<vector3d> > polys;
	std::vector<vector3d> transformed;
	std::vector<vector3d> zbuffer(points.size(), vector3d(0.0f, 0.0f, FLT_MAX));
	for (std::vector<pcs_polygon>::const_iterator it = polygons.begin(); it != polygons.end(); ++it) {
		if (dot(it->norm, forward) >= 0) {
			continue;
		}
		transformed.clear();
		for (std::vector<pcs_vertex>::const_iterator jt = it->verts.begin(); jt != it->verts.end(); ++jt) {
			transformed.push_back(transform * (jt->point - generator.pos));
		}
		if (!outside_viewport(transformed)) {
			polys.push_back(transformed);
		}
	}
	for (std::vector<std::vector<vector3d> >::const_iterator it = polys.begin(); it != polys.end(); ++it) {
		for (unsigned int i = 0; i < points.size(); i++) {
			if (inside_polygon(points[i], *it)) {
				vector3d result = interpolate_z(points[i], *it);
				if (result.z < zbuffer[i].z) {
					zbuffer[i] = result;
				}
			}
		}
	}
	std::vector<vector3d> verts;
	transform = transform.invert();
	verts.reserve(points.size());
	vector3d average;
	for (unsigned int i = 0; i < points.size(); i++) {
		verts.push_back(generator.pos + (transform * zbuffer[i]));
		average += verts[i];
	}
	average = average * (1.0/verts.size());
	// Move back by our fudge factor in the projection direction to avoid
	// intersecting the mesh.
	offset = average - (forward * 0.05);
	for (unsigned int i = 0; i < verts.size(); i++) {
		verts[i] = verts[i] - average;
	}

	pcs_insig_face face;
	for (int i = 0; i < generator.subdivision + 1; i++) {
		for (int j = 0; j < generator.subdivision + 1; j++) {
			face.verts[0] = verts[current(i, j, generator.subdivision)];
			face.verts[1] = verts[next(i, j, generator.subdivision)];
			face.verts[2] = verts[below_next(i, j, generator.subdivision)];
			face.u[0] = (points[current(i, j, generator.subdivision)].y + 1) / 2;
			face.u[1] = (points[next(i, j, generator.subdivision)].y + 1) / 2;
			face.u[2] = (points[below_next(i, j, generator.subdivision)].y + 1) / 2;
			face.v[0] = 1 - (points[current(i, j, generator.subdivision)].x + 1) / 2;
			face.v[1] = 1 - (points[next(i, j, generator.subdivision)].x + 1) / 2;
			face.v[2] = 1 - (points[below_next(i, j, generator.subdivision)].x + 1) / 2;
			faces.push_back(face);

			face.verts[0] = verts[current(i, j, generator.subdivision)];
			face.verts[1] = verts[below_next(i, j, generator.subdivision)];
			face.verts[2] = verts[below(i, j, generator.subdivision)];
			face.u[0] = (points[current(i, j, generator.subdivision)].y + 1) / 2;
			face.u[1] = (points[below_next(i, j, generator.subdivision)].y + 1) / 2;
			face.u[2] = (points[below(i, j, generator.subdivision)].y + 1) / 2;
			face.v[0] = 1 - (points[current(i, j, generator.subdivision)].x + 1) / 2;
			face.v[1] = 1 - (points[below_next(i, j, generator.subdivision)].x + 1) / 2;
			face.v[2] = 1 - (points[below(i, j, generator.subdivision)].x + 1) / 2;
			faces.push_back(face);
		}
	}

	return true;
}

vector3d pcs_insig::interpolate_z(const vector3d& v, const std::vector<vector3d>& verts) {
	if (verts.size() < 3) {
		return vector3d();
	}
	matrix transform(verts[0], verts[1], verts[2]);
	for (int i = 0; i < 3; i++) {
		transform.a2d[i][2] = 1.0f;
	}
	transform = transform.invert();
	vector3d z(verts[0].z, verts[1].z, verts[2].z);
	z = transform * z;
	return vector3d(v.x, v.y, dot(z, vector3d(v.x, v.y, 1)));
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

void pmf_bsp_cache::Read(std::istream& in, int ver)
{
	if (ver >= 102)
	{
		BFRead(bsp_size, int)
		if (bsp_size != 0) 
		{
			bsp_data = new char[bsp_size];
			BFRead(bsp_data, bsp_size) 
		}
		BFRead(changed, int)
	}
}
	
void pmf_bsp_cache::Write(std::ostream& out)
{
	BFWrite(bsp_size, int)
	if (bsp_size != 0) {
		BFWrite(bsp_data, bsp_size) }
	BFWrite(changed, bool)
}
