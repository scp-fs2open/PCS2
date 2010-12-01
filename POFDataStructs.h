//****************************************************************************
// POFDataStructs.h
// Author: Derek Meek
// Much of this file's basic data types have been plagerised from the Descent Network developer pages
// then just translated to C++
//****************************************************************************

/*
 * $Logfile: /POFDataStructs.h $
 * $Revision: 1.12 $
 * $Date: 2008/02/09 23:01:40 $
 * $Author: kazan $
 *
 * $Log: POFDataStructs.h,v $
 * Revision 1.12  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.11  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.10  2007/10/22 23:43:54  kazan
 * Catch a new error condition (thanks for TrashMan for being an airhead), fix crash bug introduced by my replacing POFstring with std::string (thanks to me for being an airhead)
 *
 * Revision 1.9  2007/10/18 15:54:08  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.8  2007/07/13 18:22:20  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.7  2007/06/16 19:40:10  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.6  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.5  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.4  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.3  2005/08/23 22:36:42  kazan
 * s/std::{vector:string}/kaz_{vector:string}
 * renderer motion works
 * location PMF::LoadFromPOF bottleneck
 *
 * Revision 1.2  2005/08/21 01:58:53  kazan
 * Context Menu framework, tree ctrl contructor
 * bug fixes to std::string handling, POF->PMF translator
 * Loaded it's first model!
 *
 * Revision 1.1.1.1  2004/12/31 06:19:42  kazan
 * no message
 *
 *
 *
 */

#include <iostream>


#if !defined(_POF_TYPES_H_)
#define _POF_TYPES_H_
#include "vector3d.h"
#include <string>
#include <boost/shared_array.hpp>
// exists.. C/C++ internal type (int) == 4 bytes, signed
#if !defined(uint)
#define uint unsigned int 
#endif

//exists (short) == 2 bytes, signed

#if !defined(ushort)
#define ushort unsigned short
#endif

//exists (char) == 1 byte, signed
#if !defined (ubyte)
#define ubyte unsigned char
#endif

//exists (float) == 4 bytes, signed
//(vector) == 3 floats, 12 bytes total
//just float[3] ... have to be careful
                     

//(string) == an int specifying length of string and 
//char[length] for the string itself (note: it is NOT null-terminated)

// i understand this at 

/* deprecated
struct POFstring { 
	int length;
	char *str; //non-null terminating when in file
	void operator=(const POFstring &rhs);
	POFstring () { length = 0; str = NULL; }
	POFstring(const POFstring &other) : length(0), str(NULL) { this->operator=(other); }

	// let's protect ourselves -- hopefully this shouldn't break anything!
	~POFstring() { delete[] str; }
};
*/


//****************************************************************************************************

/*  Chunk output priority
1  TXTR
2  HDR2
3  OBJ2
4  SPCL
5  GPNT
6  MPNT
7  TGUN
8  TMIS
9  DOCK
10 FUEL
11 SHLD
12 EYE
13 ACEN
14 INSG
15 PATH
16 GLOW // fs2_open chunk, technically not part of version 2117
17 SLDC // fs2 open chunk, technically not part of version 2117 - added 7/12/2007
18 PINF */


struct Chunk{
	char chunk_id[4];  // see below for available chunk types
	int length;  // after getting this break off into reading the correct header
};

//This starts the file
//char[4] file_id   // must be 'PSPO'
//int version       // Major*100+Minor
 


//****************************************************************************************************
// Needed for HDR2
struct cross_section
{
	float depth, radius;
};

struct muzzle_light
{
	vector3d location;
	int type; //type of light
};

//'OHDR' (FreeSpace 1) and 'HDR2' (FreeSpace 2) - Object header info 
// im assumming version 2116/2117
struct HDR2
{
	float max_radius;           // maximum radius of entire ship
	int obj_flags;              // object flags. Bit 1 = Textures contain tiling
    unsigned int num_subobjects;         // number of subobjects
 
	vector3d min_bounding;       // min bounding box point, "vector"
    vector3d max_bounding;         // max bounding box point, "vector"

    std::vector<int> sobj_detaillevels;		// subobject number for detail level I, 0 being highest
								// this is a OBJ2 segment it points to
    std::vector<int> sobj_debris;            // subobject number for debris piece 
								 // this is a OBJ2 segment it points to

    float mass;                // see notes below
    vector3d mass_center;         // center of mass, "vector"
	float moment_inertia[3][3]; // moment of inertia


   
	std::vector<cross_section> cross_sections; // the depth/radius of each cross section 

	std::vector<muzzle_light> lights;		// the lights
	HDR2() : max_radius(0), obj_flags(0), num_subobjects(0), mass(0) {
		memset(moment_inertia, 0, sizeof(moment_inertia));
	}
};

/*Notes:

  for version<2009, mass is a volume mass 
  for version>=2009, mass is an area mass 
  conversion: area_mass=4.65*(vol_mass^2/3); also scale moment_inertia by vol_mass/area_mass 
  (*) if there is no cross_section data, num_cross_sections is -1 instead of 0, as one would expect. 
*/

//****************************************************************************************************
//'TXTR' - A list of textures used on this ship.
// DNet: The order they appear here is the number that a face uses to reference a particular texture.

struct TXTR
{
  std::vector<std::string> tex_filename;    // texture filename <string> type.. has int/char[]
};

//****************************************************************************************************
//'PINF' - Miscellaneous info about the POF file, command line, etc.
//Dnet: Contains a block of NULL-terminated strings. Just read chunk size bytes and stuff it into a string
struct PINF
{
	std::vector<char> strings; //a buffer that will contains sets of null-terminated strings
};


//****************************************************************************************************
// Needed for PATH
struct path_vert
{
	vector3d pos;
	float radius;
	std::vector<int> sobj_number; //the SOBJ number for each turret
};

struct a_path //lame name because PATH is already taken
{
	std::string name;
	std::string parent;
	std::vector<path_vert> verts; //the actual verts;
};

//'PATH' -  Paths for docking and AI ships to follow
struct PATH
{
	std::vector<a_path> paths; //the paths
};

//****************************************************************************************************
// needed for SPCL
struct special_point
{
	std::string name;
    std::string properties;
	vector3d point;
    float radius;
};


//'SPCL' - Data for special points
struct SPCL
{
	std::vector<special_point> special_points; //the points

};

//****************************************************************************************************

struct shield_face
{                     
	vector3d face_normal;
    int face_vertices[3];    // indecies into vertex list
    int neighbors[3];        // indecies into face list

};

//'SHLD' - Data for the shield mesh
struct SHLD
{
	std::vector<vector3d> vertecies; // the vertex locations
	std::vector<shield_face> shield_faces;
};


//****************************************************************************************************

//'SLDC' Shield Collision Tree chunk for FS2 Open
struct SLDC_node_head //sz = 5
{
     char type; // 0 = SPLIT, 1 = LEAF/polylist
     unsigned int size; // size
};

struct SLDC_node_split // sz = 5 + 12*2 + 4*2 = 37
{
    SLDC_node_head header; // 5/5
    vector3d bound_min; // 12/17
    vector3d bound_max; // 12/29
    unsigned int front_offset; // 4/33
    unsigned int back_offset; // 4/37
};


struct SLDC_node_leaf // sz = 33 + num_polygons*4
{
    SLDC_node_head header; // 5/5
    vector3d bound_min; // 12/17
    vector3d bound_max; // 12/29
    unsigned int num_polygons; // 4/33
    //unsigned int *shld_polys = (int*)(this+21) - length num_polygons*sizeof(int)
};

struct SLDC
{
      unsigned int tree_size;
	  boost::shared_array<char> tree_data; // = new char[tree_size]

	  SLDC() : tree_size(0), tree_data(NULL) {}
};

//****************************************************************************************************

struct  eye_pos
{
	int sobj_number;      // subobject number this eye is attached to
	vector3d sobj_offset;   // offset from subobject
	vector3d normal;
};
//' EYE' - Data for eye points (Where pilot looks in 1st person views). 
// DNet: Note the space in front of EYE. -it is in the signature for a chunk
struct EYE
{
	std::vector<eye_pos> eye_positions;
};

//****************************************************************************************************

struct gun
{
	vector3d point;
	vector3d norm;
};

struct slot
{
	std::vector<gun> guns;                   
  
};

//'GPNT' and 'MPNT' - Gun and Missile firing points
struct GPNT_MPNT
{
	std::vector<slot> slots;
};

/*DNet
Notes: 

A "slot" is what you see in the loadout screen. 
	Primaries have a max of 2 and secondaries of 3 for player-flyable ships. 
"Guns" are the actual number of barrels and hence projectiles you'll get when you press the trigger. 
	There is likely no practical max. 

*/

//****************************************************************************************************

struct Turret_bank
{
	int sobj_parent; // parent subobject (the subobject with which this turret is associated) (*)
    int sobj_par_phys;     // physical parent subobject (the subobject to which the turret rotates) (*)

    vector3d turret_normal;
	std::vector<vector3d> position; // firing positions


};

// 'TGUN' and 'TMIS' - Turret Gun and Turret Missile firing points.
struct TGUN_TMIS {
	std::vector<Turret_bank> banks;
};

/* DNet
Notes: 
For multipart turrets, sobj_parent is the "barrel" of a turret, 
		and the firing points will be in this sobj's axial frame.
		 sobj_par_phys is the "base" of a turret, which the barrel will rotate with. 
For single-part turrets, sobj_parent == sobj_parent_phys. 
*/

//****************************************************************************************************
// will be using struct gun for each point
struct dock_point
{
	std::string properties; // notes below

	std::vector<int> path_number;

	std::vector<gun> points;
};


//'DOCK' - Data for docking points
struct DOCK
{
	std::vector<dock_point> points;
};

//DNet: Note: Properties if $name= found, then this is name. If name is cargo then this is a cargo bay. 

//****************************************************************************************************

struct glow_point
{
	vector3d pos;
    vector3d norm;   // used to tell if behind glow
    float radius;
};

struct thruster
{
                      
	//WARNING: version >= 2117 // FreeSpace 2 change
	// FreeSpace 2 also has sone 2116 pof so i have to code this
	std::string properties;
	// End version >= 2117
                      
	std::vector<glow_point> points;
};

//'FUEL' - Data for engine thruster glows
struct FUEL
{
	std::vector<thruster> thrusters;
};

//****************************************************************************************************



//'OBJ2' (FreeSpace 2) - Data for a subobject. 
//DNet: Contains some info and a bunch of vertices and polygons in the form of a BSP tree or Octree depending on how
//DNet:          you look at it. 
struct OBJ2
{
	int submodel_number;  // What submodel number this is.

	// FreeSpace 2 -- start Don't Touch data

	float radius;        // radius of this subobject
	int submodel_parent; // What submodel is this model's parent. Equal to -1 if none.
	vector3d offset;       // Offset to from parent object <- Added 09/10/98

	vector3d geometric_center;
	vector3d bounding_box_min_point;
	vector3d bounding_box_max_point;
	// End Don't Touch data

	// Here is what we let users play with
	std::string submodel_name;
	std::string properties;
	int movement_type;
    int movement_axis;

	// Begin Don't Touch Data
    int reserved;         // must be 0
    unsigned int bsp_data_size;    // number of bytes now following
	boost::shared_array<char> bsp_data; //size=[bsp_data_size]  -- contains actual polygons, etc.
								  // Me: I'll just be blitting this into memory and leaving it alone;
	// End Don't Touch data
	OBJ2() : submodel_number(0), radius(0.0), submodel_parent(-1), movement_type(-1), movement_axis(-1), reserved(0), bsp_data_size(0) {}
	~OBJ2() {}
};

//Note to me: that struct was too easy


//****************************************************************************************************

struct insg_face_point
{
	int vertex_index; // vertex index for this face
    float u_texture_coordinate;
    float v_texture_coordinate;
};

struct insg_face
{
	insg_face_point points[3];
};

struct insig
{

	int detail_level; //ship detail level
	std::vector<vector3d> vertex_pos; // the verticy array

	vector3d offset; //offset of the insignnia in model coords

	std::vector<insg_face> faces; //data for each face
};


//'INSG' - Squad logo/Insignia data chunk (FreeSpace 2 only)
struct INSG
{
	std::vector<insig> insignias;
};


//****************************************************************************************************


//'ACEN' - Auto-Centering info (FreeSpace 2 only)
struct ACEN
{
	vector3d point;
};


//****************************************************************************************************

// Bobboau's GLOW chunk for defining hull running-lights
// This is NOT a canonical chunk and is NOT a version 2117 chunk
// this is an fs2_open chunk

struct HullLightPoint 
{ 
	vector3d point; 
	vector3d norm; 
	float radius; 
}; 

struct HullLights  
{ 
	int disp_time;
	int on_time; 
	int off_time; 
	int obj_parent;  
	int LOD; 
	int type; 
	std::string properties;
	std::vector<HullLightPoint> lights;
	HullLights() : disp_time(0), on_time(0), off_time(0), obj_parent(0), LOD(0), type(0), lights(0) {}
}; 

struct GLOW 
{ 
	std::vector<HullLights> lights; 
}; 

#endif //_POF_TYPES_H_
