//****************************************************************************
//	ase_dstructs.h
//	Data Structures for ASCII Scene Exporter File Format
//  This is from 3DS Max, 
//  Maya supports as well (with ActorX plugin)
//  Blender (>2.44) has support
//****************************************************************************

/*
 * $Logfile: /ase_dstructs.h $
 * $Revision: 1.11 $
 * $Date: 2008/02/12 18:59:49 $
 * $Author: kazan $
 *
 * $Log: ase_dstructs.h,v $
 * Revision 1.11  2008/02/12 18:59:49  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.10  2008/02/11 13:19:40  kazan
 * Few tweaks to the loader from what i learned by loading the Sol City ASE
 *
 * Revision 1.9  2008/02/10 20:53:21  kazan
 * more ASE work, do some reorganization of the project filters
 *
 * Revision 1.8  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.7  2008/02/10 00:19:38  kazan
 * just read straight from disk instead of doing olympics
 *
 * Revision 1.6  2008/02/09 23:58:33  kazan
 * some work on ASE files
 *
 * Revision 1.5  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.4  2008/02/09 22:32:59  kazan
 * bye bye kaz_vector
 *
 * Revision 1.3  2008/02/09 22:31:26  kazan
 * finish removing string
 *
 * Revision 1.2  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.1  2008/02/09 18:34:06  kazan
 * Stub headers and source files for ASE and OBJ file formats
 *
 *
 *
 */

/*
Notes:
v is inverted again just like .cob (so v=1-v)
*/
#if !defined(_ase_dstructs_h_)
#define _ase_dstructs_h_

#include "vector3d.h"
#include <string>
#include "matrix.h"
#include <map>

namespace ase
{

	struct scene
	{
		std::string filename;
		int first_frame;
		int last_frame;
		int frame_speed;
		int ticks_per_frame;
		vector3d background_rgb;
		vector3d ambient_rgb;

		void read(std::istream &in);
		std::string write();

		scene() : first_frame(0), last_frame(0), frame_speed(30), ticks_per_frame(160) {}
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct texture_map
	{
		std::string name; // Map #x
		std::string mapclass; //Bitmap
		int subno;
		float amount;
		std::string bitmap; // filename
		std::string type; // only seen "Screen"
		float u_offset;
		float v_offset;
		float u_tiling;
		float v_tiling;
		float angle;
		float blur;
		float blur_offset;
		float noise_amt; //typoed "NOUSE" in ASE
		float noise_size;
		int noise_level;
		float noise_phase;
		std::string filter; //Pryamidal

		texture_map() : mapclass("Bitmap"), subno(1), amount(1.0f), type("Screen"), u_offset(0.0f), v_offset(0.0f), u_tiling(1.0f), v_tiling(1.0f), angle(0.0f),
						blur(1.0f), blur_offset(0.0f), noise_amt(1.0f), noise_size(1.0f), noise_level(1), noise_phase(0.0f), filter("Pyramidal") {}
		
		void read(std::istream &in);
		std::string write();
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct material
	{
		std::string name;
		std::string matclass;
		vector3d ambient;
		vector3d diffuse;
		vector3d specular;
		float shine;
		float shine_strength;
		float transparency;
		float wiresize;
		std::string shading;
		float xp_falloff;
		float self_illum;
		std::string falloff;
		std::string xp_type;
		bool twosided;

		texture_map diffuse_map;
		texture_map specular_map;
		texture_map shine_map;
		texture_map bump_map;
		texture_map self_illum_map;

		// material children
		std::vector<material> submats;

		material() : ambient(0.5882f,0.5882f,0.5882f), diffuse(0.5882f,0.5882f,0.5882f), specular(0.9000f,0.9000f,0.9000f),
						shine(0.1f), shine_strength(0.0f), transparency(0.0f), wiresize(1.0f), shading("Blinn"), xp_falloff(0.0f),
						self_illum(0.0f), falloff("Filter"), xp_type("Filter"), twosided(false) {}
		void read(std::istream &in);
		std::string write();
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	/* not really used
	struct material_list
	{
		std::vector<material> materials;
	};*/

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct transform_matrix
	{
		std::string name; //but not really.. always matches parent name
		vector3d inherit_pos;
		vector3d inherit_rot;
		vector3d inherit_scl;
		vector3d row0;
		vector3d row1;
		vector3d row2;
		vector3d row3;
		vector3d tm_pos;
		vector3d tm_rotaxis;
		float tm_rotangle;
		vector3d scale;
		vector3d scale_axis;
		float scale_axis_angle;

		transform_matrix() : tm_rotangle(0.0f), scale(1.0f, 1.0f, 1.0f), scale_axis_angle(0.0f) {}

		void read(std::istream &in);
		std::string write();
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct mesh_face
	{
		int vert0;
		int vert1;
		int vert2;
		int unknownAB; //1
		int unknownBC; //1
		int unknownCA; //0
		std::vector<int> smoothing_groups; //comma-delim list
		int submaterial_id; // WARNING 3DS spits a LOT OF GARBAGE submats in this column - so if (submaterial_id > submaterial_count) don't use a submat

		mesh_face() : vert0(0), vert1(0), vert2(0), unknownAB(1), unknownBC(1), unknownCA(0), submaterial_id(0) {}
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct texture_face
	{
		int a,b,c;

		texture_face() : a(0), b(0), c(0) {}
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	// this is a more portal vertex we can export to
	struct ase_compat_vertex
	{
		vector3d point;
		float u;
		float v;
	};

	struct ase_compat_face
	{
		std::vector<ase_compat_vertex> verts;
		vector3d normal;
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct mesh
	{
		int timevalue;
		std::vector<vector3d> vertex_list;
		std::vector<mesh_face> face_list;
		std::vector<vector3d> texture_vertex_list;
		std::vector<texture_face> texture_face_list;
		std::vector<vector3d> normals;

		mesh() : timevalue(0) {}

		void read(std::istream &in);
		std::string write();

		ase_compat_face fetchCompatface(int facenum);
		std::vector<ase_compat_face> getCompatFaces();

	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct geomobject
	{
		std::string name;
		std::string parent;
		transform_matrix tm;
		mesh geometry;
		int motionblur;
		int cast_shadow;
		int recv_shadow;
		int material_ref;


		geomobject() : motionblur(0), cast_shadow(1), recv_shadow(1), material_ref(0) {}
			
		void read(std::istream &in);
		std::string write();
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	struct helperobject
	{
		std::string name;
		std::string parent;
		std::string helpclass;
		transform_matrix tm;
		vector3d bbox_min;
		vector3d bbox_max;

		void read(std::istream &in);
		std::string write();
	};


}; // namespace ase


#endif //_ase_dstructs_h_
