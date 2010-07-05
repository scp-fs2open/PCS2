//****************************************************************************
//	pcs_file_dstructs.h
//	POF Constructor Suite 2 File Format data structures
//****************************************************************************

/*
 * $Logfile: /pcs_file_dstructs.h $
 * $Revision: 1.18 $
 * $Date: 2008/02/12 18:59:56 $
 * $Author: kazan $
 *
 * $Log: pcs_file_dstructs.h,v $
 * Revision 1.18  2008/02/12 18:59:56  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.17  2008/02/09 23:01:46  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.16  2008/02/09 22:33:01  kazan
 * bye bye kaz_vector
 *
 * Revision 1.15  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.14  2008/01/13 19:30:24  bobboau
 * update to HTL renderer, wireframe views/highlights
 *
 * Revision 1.13  2008/01/12 18:02:24  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.12  2007/07/24 22:50:24  bobboau
 * added undo/redo functionality
 *
 * Revision 1.11  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.9  2007/06/27 21:13:58  kazan
 * maybe, possibly, we hope i just fixed BSP collision trees
 *
 * Revision 1.8  2007/06/14 23:48:10  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.7  2007/06/06 12:08:23  bobboau
 * polished the UI some, converted a bunch of int_ctrls to int_lists, mostly for selecting a subobject number, added array over/underflow protection/handeling to some stuff.
 *
 * Revision 1.6  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.5  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.4  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.3  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
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

#if !defined(_pcs_file_dstructs_h_)
#define _pcs_file_dstructs_h_

#include "vector3d.h"
#include <string>
#include "ogl_vertex_buffers.h"


#include <iostream>

//****************************************************************************
//shortcuts
//****************************************************************************

#define BFWrite(obj, type) out.write((char *)&obj, sizeof(type));
#define BFRead(obj, type) in.read((char *)&obj, sizeof(type));
#define BFWriteString(string) BF_WriteString(out, string);
#define BFReadString(string) BF_ReadString(in, string);
#define BFWriteVector(vector) BF_WriteVector(out, vector);
#define BFReadVector(vector) BF_ReadVector(in, vector);
#define BFWriteAdvVector(vec) BF_WriteAdvVector(out, vec);
#define BFReadAdvVector(vec) BF_ReadAdvVector(in, vec, ver);

void BF_ReadString(std::istream &in, std::string &retval);
void BF_WriteString(std::ostream &out, std::string &str);


//****************************************************************************
// templates
//****************************************************************************

// this will be useful in pcs_file.cpp, and since it's templated it must be
template <class L1TYPE> void RemoveIndex(std::vector<L1TYPE> &arr, unsigned int idx)
{
	for (unsigned int i = idx; i < arr.size()-1; i++)
		arr[i] = arr[i+1];
	arr.resize(arr.size()-1);
}

// these will be useful in many places
template <class L2TYPE> void BF_WriteVector(std::ostream &out, std::vector<L2TYPE> &arr)
{
	int cnt = arr.size();
	BFWrite(cnt, int)

	for (int i = 0; i < cnt; i++)
		BFWrite(arr[i], L2TYPE)
}

template <class L3TYPE> void BF_ReadVector(std::istream &in, std::vector<L3TYPE> &arr)
{
	int cnt;
	BFRead(cnt, int)
	arr.resize(cnt);

	for (int i = 0; i < cnt; i++)
		BFRead(arr[i], L3TYPE)
}

template <class L4TYPE> void BF_WriteAdvVector(std::ostream &out, std::vector<L4TYPE> &arr)
{
	int cnt = arr.size();
	BFWrite(cnt, int)

	for (int i = 0; i < cnt; i++)
		arr[i].Write(out);
}

template <class L5TYPE> void BF_ReadAdvVector(std::istream &in, std::vector<L5TYPE> &arr, int ver)
{
	int cnt;
	BFRead(cnt, int)
	arr.resize(cnt);

	for (int i = 0; i < cnt; i++)
		arr[i].Read(in, ver);
}

template <class L6TYPE> int FindInList(std::vector<L6TYPE> &haystack, const L6TYPE &needle)
{
	for (unsigned int i = 0; i < haystack.size(); i++)
		if (haystack[i] == needle)
			return i;
	return -1;
}
//****************************************************************************
// data structures
//****************************************************************************

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_vertex
{
	vector3d point;
	vector3d norm;
	float u;
	float v;
	float facet_angle;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_vertex() : u(0.0), v(0.0), facet_angle(-1) {}
	//pcs_vertex(pcs_vertex &other) : point(other.point), norm(other.norm), u(other.u), v(other.v) {}

};

inline bool operator == (const pcs_vertex&t, const pcs_vertex&o){
	return  t.point == o.point&&
		t.norm == o.norm&&
		t.u == o.u&&
		t.v == o.v&&
		t.facet_angle == o.facet_angle;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_polygon
{
	std::vector<pcs_vertex> verts;
	int texture_id;
	vector3d norm;

	// this variable is NOT saved to disk - Kazan 6/27/2007
	vector3d centeroid; 

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_polygon() : texture_id(-1) {}
	//pcs_polygon(pcs_polygon &other) : verts(other.verts), texture_id(other.texture_id), norm(other.norm) {}

};

inline bool operator==(const pcs_polygon&t, const pcs_polygon&o){
	return t.verts == o.verts&&
		t.texture_id == o.texture_id&&
		t.norm == o.norm &&
		t.centeroid == o.centeroid;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
enum { MNONE=0, ROTATE };
enum { ANONE=0, MV_X, MV_Y, MV_Z };

struct pcs_sobj
{
	//int sobj_num is determined by it's place in the vector
	int parent_sobj;

	// some physics useful stuff
	float radius;
	vector3d offset;

	vector3d geometric_center;
	vector3d bounding_box_min_point;
	vector3d bounding_box_max_point;

	// other stuff
	std::string name;
	std::string properties;

	int movement_type;
	int movement_axis;

	std::vector<pcs_polygon> polygons;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_sobj() : parent_sobj(-1), radius(0.0), movement_type(MNONE), movement_axis(ANONE) {}
	void GenerateBBoxes() { 
				for (unsigned int i = 0; i < polygons.size(); i++) 
				{ 
					for (unsigned int j = 0; j < polygons[i].verts.size(); j++) 
					{ 
						ExpandBoundingBoxes(bounding_box_max_point, bounding_box_min_point, polygons[i].verts[j].point);
					} 
				} 
			}
	void GenerateRadius() {
		radius = FindObjectRadius(bounding_box_max_point, bounding_box_min_point, geometric_center);
	}

		/*
OGL vertex buffer stuf!
		*/

	std::vector<ogl_vertex_buffer> vertex_buffer;
	std::vector<ogl_vertex_buffer> line_vertex_buffer;//for outlineing

	void make_vertex_buffer(int tid);//set's up a vertex buffer
	void destroy_vertex_buffer();//frees all the stuff
};

inline bool operator == (const pcs_sobj&t, const pcs_sobj&o){
	return 	t.parent_sobj == o.parent_sobj&&
		t.radius == o.radius&&
		t.offset == o.offset&&
		t.geometric_center == o.geometric_center&&
		t.bounding_box_min_point == o.bounding_box_min_point&&
		t.bounding_box_max_point == o.bounding_box_max_point&&
		t.name == o.name&&
		t.properties == o.properties&&
		t.movement_type == o.movement_type&&
		t.movement_axis == o.movement_axis&&
		t.polygons == o.polygons;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_crs_sect
{
	float depth, radius;

	pcs_crs_sect() : depth(0.0), radius(0.0) {}

};

inline bool operator==(const pcs_crs_sect&t, const pcs_crs_sect&o){
	return t.depth == o.depth && t.radius == o.radius;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_eye_pos
{
	int sobj_number;      // subobject number this eye is attached to
	vector3d sobj_offset;   // offset from subobject
	vector3d normal;

	pcs_eye_pos() : sobj_number(-1), normal(0,0,1) {}
};

inline bool operator==(const pcs_eye_pos&t, const pcs_eye_pos&o){
	return t.sobj_number == o.sobj_number && t.sobj_offset == o.sobj_offset && t.normal == o.normal;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_special
{
	std::string name;
	std::string properties;
	vector3d point;
    float radius;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_special() : properties("$special=subsystem"), radius(0.0) {}
};


inline bool operator==(const pcs_special&t, const pcs_special&o){
	return t.properties == o.properties && t.point == o.point && t.radius == o.radius;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// GPNT/MPNT

struct pcs_hardpoint 
{
	vector3d point;
	vector3d norm;
	pcs_hardpoint():norm(0,0,1){}
};
inline bool operator==(const pcs_hardpoint&t, const pcs_hardpoint&o){
	return t.point == o.point && t.norm == o.norm;
}

enum { GUN, MISSILE };

struct pcs_slot 
{
	int type; // GUN/MISSILE
	std::vector<pcs_hardpoint> muzzles;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_slot() : type(GUN) {}
};

inline bool operator==(const pcs_slot&t, const pcs_slot&o){
	return t.type == o.type && t.muzzles == o.muzzles;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_turret
{
	int type; // GUN/MISSILE
	int sobj_parent; // parent subobject (the subobject with which this turret is associated) (*)
    int sobj_par_phys;     // physical parent subobject (the subobject to which the turret rotates) (*)

    vector3d turret_normal;
	std::vector<vector3d> fire_points;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_turret() : type(GUN), sobj_parent(-1), sobj_par_phys(-1) {}
};

inline bool operator==(const pcs_turret&t, const pcs_turret&o){
	return t.type == o.type && 
		t.sobj_parent == o.sobj_parent && 
		t.sobj_par_phys == o.sobj_par_phys && 
		t.turret_normal == o.turret_normal && 
		t.fire_points == o.fire_points;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_dock_point
{
	std::string properties; // notes below
	std::vector<int> paths;
	std::vector<pcs_hardpoint> dockpoints;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
};

inline bool operator==(const pcs_dock_point&t, const pcs_dock_point&o){
	return t.properties == o.properties && 
		t.paths == o.paths && 
		t.dockpoints == o.dockpoints;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_thrust_glow
{	
	vector3d pos;
    vector3d norm;   // used to tell if behind glow
    float radius;

	pcs_thrust_glow() : norm(0,0,-1), radius(0.0) {}
	pcs_thrust_glow(vector3d apos, vector3d anorm, float arad) : pos(apos), norm(anorm), radius(arad) {}
};


inline bool operator==(const pcs_thrust_glow&t, const pcs_thrust_glow&o){
	return t.pos == o.pos && 
		t.norm == o.norm && 
		t.radius == o.radius;
}

struct pcs_thruster
{
	std::vector<pcs_thrust_glow> points;
	std::string properties;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
};

inline bool operator==(const pcs_thruster&t, const pcs_thruster&o){
	return t.points == o.points && 
		t.properties == o.properties;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_shield_triangle
{
	vector3d face_normal;
	vector3d corners[3];
};

inline bool operator==(const pcs_shield_triangle&t, const pcs_shield_triangle&o){
	return t.face_normal == o.face_normal && 
		t.corners[0] == o.corners[0] && 
		t.corners[1] == o.corners[1] && 
		t.corners[2] == o.corners[2];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_insig_face
{
	vector3d verts[3];
	float u[3];
	float v[3];

	pcs_insig_face() { memset(this, 0, sizeof(pcs_insig_face)); }
};

inline bool operator==(const pcs_insig_face&t, const pcs_insig_face&o){
	return t.verts == o.verts &&
		t.u[0] == o.u[0] && t.v[0] == o.u[0] &&
		t.u[1] == o.u[1] && t.v[1] == o.u[1] &&
		t.u[2] == o.u[2] && t.v[2] == o.u[2];
}


struct pcs_insig
{
	int lod;
	vector3d offset; // offset from center of ship
	std::vector<pcs_insig_face> faces;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_insig() : lod(0) {} 
};

inline bool operator==(const pcs_insig&t, const pcs_insig&o){
	return t.lod == o.lod && t.offset == o.offset && t.faces == o.faces;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pcs_pvert
{
	vector3d pos;
	float radius;

	pcs_pvert() : radius(0.0) {}
};

inline bool operator==(const pcs_pvert&t, const pcs_pvert&o){
	return t.pos == o.pos && 
		t.radius == o.radius;
}

struct pcs_path
{
	std::string name;
	std::string parent;
	std::vector<pcs_pvert> verts;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
};

inline bool operator==(const pcs_path&t, const pcs_path&o){
	return t.name == o.name && 
		t.parent == o.parent && 
		t.verts == o.verts;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


struct pcs_glow_array  
{ 
	int disp_time;
	int on_time; 
	int off_time; 
	int obj_parent;  
	int LOD; 
	int type; 
	std::string properties;
	std::vector<pcs_thrust_glow> lights;

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);
	pcs_glow_array() : disp_time(0), on_time(0), off_time(0), obj_parent(0), LOD(0), type(0), lights() {}
}; 

inline bool operator==(const pcs_glow_array&t, const pcs_glow_array&o){
	return t.disp_time == o.disp_time && 
		t.on_time == o.on_time && 
		t.off_time == o.off_time && 
		t.obj_parent == o.obj_parent && 
		t.LOD == o.LOD && 
		t.type == o.type && 
		t.properties == o.properties && 
		t.lights == o.lights;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct header_data{
	header_data()
		:max_radius(0.0f),min_bounding(0,0,0),max_bounding(0,0,0),mass(0.0f),mass_center(0,0,0)
	{
		memset(MOI,0,sizeof(float)*9);
		MOI[0][0]=1.0f;
		MOI[1][1]=1.0f;
		MOI[2][2]=1.0f;
	}
	header_data(float Max_radius, const vector3d&Min_bounding, const vector3d&Max_bounding, float Mass, const vector3d&Mass_center, float moi[3][3])
		:max_radius(Max_radius),min_bounding(Min_bounding),max_bounding(Max_bounding),mass(Mass),mass_center(Mass_center)
	{
		memcpy(MOI,moi,sizeof(float)*9);
	}
	header_data(const header_data&h){
		(*this)=h;
	}
	const header_data&operator=(const header_data&h){
		max_radius = h.max_radius;
		min_bounding = h.min_bounding;
		max_bounding = h.max_bounding;
		detail_levels = h.detail_levels;
		debris_pieces = h.debris_pieces;
		mass = h.mass;
		mass_center = h.mass_center;
		memcpy(MOI,h.MOI,sizeof(float)*9);
		cross_sections = h.cross_sections;
		return *this;
	}
		// ++++ "header" information ++++ //HDR2
		float max_radius;           // maximum radius of entire ship

		vector3d min_bounding;       // min bounding box point
		vector3d max_bounding;         // max bounding box point

		std::vector<int> detail_levels; // indexes into the subobjects vector
										// defining "top level" for each sobj
		std::vector<int> debris_pieces; // same as details, but stores debris instead

		float mass;	// mass of the entire model
		vector3d mass_center;

		float MOI[3][3]; //moment of inertia tensor... don't ask me how to calculate it
						 // bobboau knows that

		std::vector<pcs_crs_sect> cross_sections; // FS2 uses this to split exploding ships
												  // i'd do muzzle_light for FS2 - but it's useless

};
inline bool operator == (const header_data&t, const header_data&h){
	return t.max_radius == h.max_radius &&
		t.min_bounding == h.min_bounding &&
		t.max_bounding == h.max_bounding &&
		t.detail_levels == h.detail_levels &&
		t.debris_pieces == h.debris_pieces &&
		t.mass == h.mass &&
		t.mass_center == h.mass_center &&
		!memcmp(t.MOI,h.MOI,sizeof(float)*9) &&
		t.cross_sections == h.cross_sections;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// bsp caching for pof editing
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct pmf_bsp_cache
{
	int bsp_size;
	char *bsp_data;
	bool changed;

	pmf_bsp_cache() : bsp_size(0), bsp_data(NULL), changed(false) {}

	void decache() // use this when it's been changed
	{
		if (bsp_data)
			delete[] bsp_data;
		bsp_size = 0;
		changed = true;
	}

	void Read(std::istream& in, int ver);
	void Write(std::ostream& out);

	~pmf_bsp_cache() { decache(); }
};


#endif //_pcs_file_dstructs_h_

