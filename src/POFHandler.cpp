//****************************************************************************
// POFHandler.cpp
// Author: Derek Meek
// Implentation of the POF Handler Functions
//****************************************************************************

/*
 * $Logfile: /POFHandler.cpp $
 * $Revision: 1.24 $
 * $Date: 2008/11/08 15:13:19 $
 * $Author: kazan $
 *
 * $Log: POFHandler.cpp,v $
 * Revision 1.24  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.23  2008/02/11 13:18:15  kazan
 * Possible fix for Mantis #59
 *
 * Revision 1.22  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.21  2008/02/09 22:32:58  kazan
 * bye bye kaz_vector
 *
 * Revision 1.20  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.19  2007/10/18 15:54:08  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.18  2007/07/13 18:22:20  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.17  2007/07/12 05:36:35  bobboau
 * fixed a crash bug in glowpoint saveing related to memcpying  a class with a string (which has a pointer to it's own memory buffer which it deletes on destruction) in it.
 *
 * Revision 1.16  2007/07/03 02:52:20  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.14  2007/06/29 21:00:26  bobboau
 * added shield chunk "editor", doesn't actualy let you edit anything, but will display shield poly count and activate the rendering of shileds, which I also added.
 *
 * Revision 1.13  2007/06/18 01:15:01  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.12  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.11  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.10  2007/03/22 13:15:33  kazan
 * MSVC8 compatability checkin, requires wxWidgets 2.8
 *
 * Revision 1.9  2006/04/07 11:57:05  kazan
 * sine _WIN32 !defined checks
 *
 * Revision 1.8  2005/10/07 14:08:06  kazan
 * linux compat syncs - i'm getting memory crashes in linux, hopefully it happens in windows too so it's easier to debug
 *
 * Revision 1.7  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.6  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.5  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.4  2005/08/25 15:06:38  kazan
 * various bugfixes and tweaks
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

#if !defined(_WIN32)
#include "compat/filelength.h"
#include "compat/strncmp.h"
#endif

#include <memory.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <boost/scoped_array.hpp>

#include "POFHandler.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Notes: 1/18/2001
// Complete [X]: Need to Finished up FUEL and DOCK structures
// Complete [*]: [most] Array resizing functions need to be updated to the more efficient way i realized while writing the program
// Complete [X]: Need to Finish up Parse_Memory_XXXX functions
// Complete [X]: Need to write file output function

// Notes: 1/26/2001
// Correct a few mistakes in the SavePOF() function

// Notes:11/16/2001
// Moved Notes to top of file

// Notes: 11/17/2001
// Array resizing optimized, found some memory leaks in it too

// Notes: 07/18/2002
// Complete [!]: Search and destroy memory leaks created by destroing (pof)strings without remembering to first delete[] string.str
// Added Parse_Memory_GLOW function and added all of GLOW chunk's functions

// Notes: 10/18/2007
// Replaced all instances of POFstring with std::string

//**********************************************************************************************************************

template<typename T>
void write_to_file(std::ostream& out, const T& value) {
	out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<>
void write_to_file(std::ostream& out, const std::string& value) {
	out.write(value.c_str(), value.size());
}

template<>
void write_to_file(std::ostream& out, const std::vector<char>& value) {
	out.write(value.data(), value.size());
}

void write_to_file(std::ostream& out, const char* value) {
	out.write(value, strlen(value));
}

template<>
void write_to_file(std::ostream& out, const uint64_t& value);
template<>
void write_to_file(std::ostream& out, const int64_t& value);

//**********************************************************************************************************************

void POF::SLDC_SetTree(const std::vector<char>& sldc_tree)
// setting this to empty will empty the tree
{
	shield_collision.tree_data = sldc_tree;
}

void POF::SLDC_SetTree(std::vector<char>&& sldc_tree)
// setting this to empty will empty the tree
{
	shield_collision.tree_data.swap(sldc_tree);
}

//**********************************************************************************************************************
// non member functions
/*std::string APStoString(std::string aps)
{
	std::string temp;

	temp.length = aps.length();
	temp.str = new char[temp.length];

	strncpy(temp.str, aps.c_str(), temp.length);
	return temp;
}

std::string StringToAPS(std::string &str)
{
	char *cstr = new char[str.length + 1];

	memset(cstr, 0, str.length + 1);
	strncpy(cstr, str.str, str.length);

	std::string aps(cstr);
	delete[] cstr;

	return aps;

}*/



//**********************************************************************************************************************
// Misc
void POF::StatsToFile(std::ofstream &outfile)
{
	unsigned int i;
	vector3d vect;

	vect = ACEN_Get_acen();

	// ACEN
	outfile << "ACEN: (" << vect.x << "," << vect.y << "," << vect.z << ")" << std::endl;


	//DOCK
	outfile << "DOCK: " << DOCK_Count_Docks() << " docks." << std::endl;
	
	for (i = 0; i < DOCK_Count_Docks(); i++)
	{
		outfile << "    +Dock[" << i << "]: " << DOCK_Count_Points(i) << " points, " << DOCK_Count_SplinePaths(i) << " spline paths." << std::endl;
	}


	//EYE
	outfile << " EYE: " << EYE_Count_Eyes() << " eyes." << std::endl;


	//FUEL
	outfile << "FUEL: " << FUEL_Count_Thrusters() << " thrusters." << std::endl;

	for (i = 0; i < FUEL_Count_Thrusters(); i++)
	{
		outfile << "    +Fuel[" << i << "]: " << FUEL_Count_Glows(i) << " glow points." << std::endl;
	}

	//GPNT
	outfile << "GPNT: " << GPNT_SlotCount() << " slots." << std::endl;

	for (i = 0; i < GPNT_SlotCount(); i++)
	{
		outfile << "    +Gun[" << i << "]: " << GPNT_PointCount(i) << " firing points." << std::endl;
	}


	//MPNT
	outfile << "MPNT: " << MPNT_SlotCount() << " slots." << std::endl;

	for (i = 0; i < MPNT_SlotCount(); i++)
	{
		outfile << "    +Mis[" << i << "]: " << MPNT_PointCount(i) << " firing points." << std::endl;
	}

	//HDR2
	outfile << "HRD2: " << HDR2_Get_Mass() << " object mass, " << HDR2_Get_SOBJCount() << " SubObjects." << std::endl;

	//INSG
	outfile << "INSG: " << INSG_Count_Insignia() << " insignias." << std::endl;

	//OBJ2
	outfile << "OBJ2: " << OBJ2_Count() << " SubObjects." << std::endl;

	for (i = 0; i < OBJ2_Count(); i++)
	{
		outfile << "    +sobj[" << i << "]:" << OBJ2_BSP_Datasize(i) << " bytes of BSP data." << std::endl;
	}

	//PATH
	outfile << "PATH: " << PATH_Count_Paths() << " paths." << std::endl;

	for (i = 0; i < PATH_Count_Paths(); i++)
	{
		outfile << "    +Path[" << i << "]: " << PATH_Count_Verts(i) << " verts. Parent: " << StringToAPS(paths.paths[i].parent) << ", Name: " << StringToAPS(paths.paths[i].name) << std::endl;
	}

	//PINF

	outfile << "PINF: ";
	
	std::vector<std::string> strings = PINF_Get();
	for (i = 0; i < strings.size(); i++)
	{
		outfile << strings[i].c_str() << ", ";
	}

	outfile << std::endl;

	//SHLD
	outfile << "SHLD: " << SHLD_Count_Faces() << " faces, " << SHLD_Count_Vertices() << " verts." << std::endl;

	//SPCL
	outfile << "SPCL: " << SPCL_Count() << " specials." << std::endl;

	//TGUN
	outfile << "TGUN: " << TGUN_Count_Banks() << " turrets." << std::endl;

	//TMIS
	outfile << "TMIS: " << TMIS_Count_Banks() << " turrets." << std::endl;

	//TXTR
	outfile << "TXTR: " << TXTR_Count_Textures() << " textures." << std::endl;

	for (i = 0; i < TXTR_Count_Textures(); i++)
	{
		outfile << "    +tex[" << i << "]: " << TXTR_GetTextures(i) << std::endl;
	}


}


//**********************************************************************************************************************
// File IO Functions

std::string Parse_BPOFstring(char *&localptr)
{
	std::string retval;
	int len;

	memcpy(&len, localptr, sizeof(int));
	localptr += sizeof(int);

	boost::scoped_array<char> str(new char[len+1]);
	
	memcpy(str.get(), localptr, len);
	localptr += len;

	str[len] = '\0';
	
	retval = str.get();
	return retval;

}

//****************************************************************************************************************

void POF::Parse_Memory_PNT(int mode, char *buffer)
{
	// this is going to be FUN
	char *localptr = buffer;
	GPNT_MPNT *pnt = PNT_Alias(mode);

	unsigned int num_slots;
	memcpy(&num_slots, buffer, 4);
	localptr += 4;

	pnt->slots.resize(num_slots);
	

	for (unsigned int i = 0; i < num_slots; i++)
	{
		unsigned int num_guns;
		memcpy(&num_guns, localptr, 4);
		pnt->slots[i].guns.resize(num_guns);
		localptr += 4; //pointer arithmetic

		for (unsigned int j = 0; j < num_guns; j++)
		{
			memcpy(&pnt->slots[i].guns[j], localptr, sizeof(gun));
			localptr += sizeof(gun);
		}
	}
}

//-----------------------------------------------------------------

void POF::Parse_Memory_T(int mode, char *buffer)
{
	TGUN_TMIS *pnt = T_Alias(mode);

	char *localptr = buffer;

	unsigned int num_banks;
	memcpy(&num_banks, localptr, 4);
	localptr += 4;

	pnt->banks.resize(num_banks);

	for (unsigned int i = 0; i < num_banks; i++)
	{
		//int
		memcpy(&pnt->banks[i].sobj_parent, localptr, 4);
		localptr += 4; //pointer arithmetic

		//int
		memcpy(&pnt->banks[i].sobj_par_phys, localptr, 4);
		localptr += 4; //pointer arithmetic

		//vector (float[3])
		memcpy(&pnt->banks[i].turret_normal, localptr, 12);
		localptr += 12; //pointer arithmetic

		//int
		unsigned int num_firing_points;
		memcpy(&num_firing_points, localptr, 4);
		localptr += 4; //pointer arithmetic

		pnt->banks[i].position.resize(num_firing_points);

		for (unsigned int j = 0; j < num_firing_points; j++)
		{
			//verctor (float[3])
			memcpy(&pnt->banks[i].position[j], localptr, 12);
			localptr += 12;
		}

	}

}

//-----------------------------------------------------------------

void POF::Parse_Memory_OBJ2(char *buffer)
{

	OBJ2 temp;
	char *localptr = buffer;

	memcpy(&temp.submodel_number, localptr, sizeof(int));
	localptr += sizeof(int);

	memcpy(&temp.radius, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&temp.submodel_parent, localptr, sizeof(int));
	localptr += sizeof(int);

	memcpy(&temp.offset, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	memcpy(&temp.geometric_center, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	memcpy(&temp.bounding_box_min_point, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	memcpy(&temp.bounding_box_max_point, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	//string copy
	/*memcpy(&temp.submodel_name.length, localptr, sizeof(int));
	localptr += sizeof(int);
	
	if (temp.submodel_name.length)
		temp.submodel_name.str = new char[temp.submodel_name.length];
	else
		temp.submodel_name.str = 0;

	memcpy(temp.submodel_name.str, localptr, temp.submodel_name.length);
	localptr += temp.submodel_name.length;*/

	temp.submodel_name = Parse_BPOFstring(localptr);

	//string copy
	/*memcpy(&temp.properties.length, localptr, sizeof(int));
	localptr += sizeof(int);
	
	if (temp.properties.length)
		temp.properties.str = new char[temp.properties.length];
	else
		temp.properties.str = NULL;

	memcpy(temp.properties.str, localptr, temp.properties.length);
	localptr += temp.properties.length;*/

	temp.properties = Parse_BPOFstring(localptr);

	memcpy(&temp.movement_type, localptr, sizeof(int));
	localptr += sizeof(int);

	memcpy(&temp.movement_axis, localptr, sizeof(int));
	localptr += sizeof(int);

	memcpy(&temp.reserved, localptr, sizeof(int));
	localptr += sizeof(int);

	int size = temp.bsp_data.size();
	memcpy(&size, localptr, sizeof(int));
	localptr += sizeof(int);

	temp.bsp_data.resize(size);

	memcpy(&temp.bsp_data.front(), localptr, temp.bsp_data.size());
	
	//done!
	temp.reserved = 0;
	objects.push_back(temp);
}

//-----------------------------------------------------------------

void POF::Parse_Memory_DOCK(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_docks;
	memcpy(&num_docks, localptr, sizeof(int));
	localptr += sizeof(int);

	docking.points.resize(num_docks);

	for (unsigned int i = 0; i < num_docks; i++)
	{
		/*
		memcpy(&docking.points[i].properties.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (docking.points[i].properties.length)
			docking.points[i].properties.str = new char[docking.points[i].properties.length];
		else
			docking.points[i].properties.str = NULL;

		memcpy(docking.points[i].properties.str, localptr, docking.points[i].properties.length);
		localptr += docking.points[i].properties.length;
		*/
		
		docking.points[i].properties = Parse_BPOFstring(localptr);

		unsigned int num_spline_paths;
		memcpy(&num_spline_paths, localptr, sizeof(int));
		localptr += sizeof(int);

		docking.points[i].path_number.resize(num_spline_paths);

		for (unsigned int j = 0; j < num_spline_paths; j++)
		{
			memcpy(&docking.points[i].path_number[j], localptr, sizeof(int));
			localptr += sizeof(int);
		}

		unsigned int num_points;
		memcpy(&num_points, localptr, sizeof(int));
		localptr += sizeof(int);

		docking.points[i].points.resize(num_points);

		for (unsigned int k = 0; k < num_points; k++)
		{
			memcpy(&docking.points[i].points[k], localptr, sizeof(gun));
			localptr += sizeof(gun);
		}
	}
}

//-----------------------------------------------------------------

void POF::Parse_Memory_GLOW(char *buffer)
{
	//hull_lights
	char *localptr = buffer;

	unsigned int num_glows_arrays;
	memcpy(&num_glows_arrays, localptr, sizeof(int));
	localptr += sizeof(int);

	hull_lights.lights.resize(num_glows_arrays);

	for (unsigned int i = 0; i < num_glows_arrays; i++)
	{
		memcpy(&hull_lights.lights[i].disp_time, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&hull_lights.lights[i].on_time, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&hull_lights.lights[i].off_time, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&hull_lights.lights[i].obj_parent, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&hull_lights.lights[i].LOD, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&hull_lights.lights[i].type, localptr, sizeof(int));
		localptr += sizeof(int);
		
		unsigned int num_Lights;
		memcpy(&num_Lights, localptr, sizeof(int));
		localptr += sizeof(int);

		/*memcpy(&hull_lights.lights[i].properties.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (hull_lights.lights[i].properties.length)
			hull_lights.lights[i].properties.str = new char[hull_lights.lights[i].properties.length];
		else
			hull_lights.lights[i].properties.str = NULL;

		memcpy(hull_lights.lights[i].properties.str, localptr, hull_lights.lights[i].properties.length);
		localptr += hull_lights.lights[i].properties.length;*/

		hull_lights.lights[i].properties = Parse_BPOFstring(localptr);

		hull_lights.lights[i].lights.resize(num_Lights);

		for (unsigned int j = 0; j < num_Lights; j++)
		{
			memcpy(&hull_lights.lights[i].lights[j].point, localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);

			memcpy(&hull_lights.lights[i].lights[j].norm, localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);

			memcpy(&hull_lights.lights[i].lights[j].radius, localptr, sizeof(float));
			localptr += sizeof(float);
		
		}
	}
}

//-----------------------------------------------------------------

void POF::Parse_Memory_FUEL(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_thrusters;
	memcpy(&num_thrusters, localptr, sizeof(int));
	localptr += sizeof(int);

	thrusters.thrusters.resize(num_thrusters);

	for (unsigned int i = 0; i < num_thrusters; i++)
	{
		unsigned int num_glows;
		memcpy(&num_glows, localptr, sizeof(int));
		localptr += sizeof(int);

		//warning: Version check. version >= 2117 only
		if (version >= 2117)
		{
			/*memcpy(&thrusters.thrusters[i].properties.length, localptr, sizeof(int));
			localptr += sizeof(int);

			if (thrusters.thrusters[i].properties.length)
				thrusters.thrusters[i].properties.str = new char[thrusters.thrusters[i].properties.length];
			else
				thrusters.thrusters[i].properties.str = NULL;
			
			memcpy(thrusters.thrusters[i].properties.str, localptr, thrusters.thrusters[i].properties.length);
			localptr += thrusters.thrusters[i].properties.length;*/
			thrusters.thrusters[i].properties = Parse_BPOFstring(localptr);
		}
		//else
		//{
		//	thrusters.thrusters[i].properties.str = NULL;
		//	thrusters.thrusters[i].properties.length = 0;
		//}// End version check block

		thrusters.thrusters[i].points.resize(num_glows);

		for (unsigned int j = 0; j < num_glows; j++)
		{
			memcpy(&thrusters.thrusters[i].points[j], localptr, sizeof(glow_point));
			localptr += sizeof(glow_point);
		}

	}
}

//-----------------------------------------------------------------

void POF::Parse_Memory_SHLD(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_vertices;
	memcpy(&num_vertices, localptr, sizeof(int));
	localptr += sizeof(int);

	shields.vertecies.resize(num_vertices);

	for (unsigned int i = 0; i < num_vertices; i++)
	{
		memcpy(&shields.vertecies[i], localptr, sizeof(vector3d));
		localptr += sizeof(vector3d);
	}

	unsigned int num_faces;
	memcpy(&num_faces, localptr, sizeof(int));
	localptr += sizeof(int);

	shields.shield_faces.resize(num_faces);

	for (unsigned int j = 0; j < num_faces; j++)
	{
		memcpy(&shields.shield_faces[j], localptr, sizeof(shield_face));
		localptr += sizeof(shield_face);
	}


}

//-----------------------------------------------------------------

void POF::Parse_Memory_EYE(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_eye_positions;
	memcpy(&num_eye_positions, localptr, sizeof(int));
	localptr += sizeof(int);
	
	eyes.eye_positions.resize(num_eye_positions);
	
	for (unsigned int i = 0; i < num_eye_positions; i++)
	{
		memcpy(&eyes.eye_positions[i], localptr, sizeof(eye_pos));
		localptr += sizeof(eye_pos);
	}
}

//-----------------------------------------------------------------

void POF::Parse_Memory_ACEN(char *buffer)
{ // really hard.. mwahahah
	memcpy(&autocentering.point, buffer, sizeof(vector3d));
}


void POF::Parse_Memory_INSG(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_insignias;
	memcpy(&num_insignias, localptr, sizeof(int));
	localptr += sizeof(int);

	insignia.insignias.resize(num_insignias);

	for (unsigned int i = 0; i < num_insignias; i++)
	{
		memcpy(&insignia.insignias[i].detail_level, localptr, sizeof(int));
		localptr += sizeof(int);

		unsigned int num_faces;
		memcpy(&num_faces, localptr, sizeof(int));
		localptr += sizeof(int);
		
		unsigned int num_verticies;
		memcpy(&num_verticies, localptr, sizeof(int));
		localptr += sizeof(int);

		insignia.insignias[i].vertex_pos.resize(num_verticies);
		insignia.insignias[i].faces.resize(num_faces);

		for (unsigned int j = 0; j < num_verticies; j++)
		{
			memcpy(&insignia.insignias[i].vertex_pos[j], localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);
		}

		memcpy(&insignia.insignias[i].offset, localptr, sizeof(vector3d));
		localptr += sizeof(vector3d);

		for (unsigned int k = 0; k < num_faces; k++)
		{
			memcpy(&insignia.insignias[i].faces[k], localptr, sizeof(insg_face));
			localptr += sizeof(insg_face);
		}
	

	}

}


void POF::Parse_Memory_PATH(char *buffer)
{
	char *localptr = buffer;

	unsigned int num_paths;
	memcpy(&num_paths, localptr, sizeof(int));
	localptr += sizeof(int);

	paths.paths.resize(num_paths);

	for (unsigned int i = 0; i < num_paths; i++)
	{
		/*memcpy(&paths.paths[i].name.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (paths.paths[i].name.length)
			paths.paths[i].name.str.resize(length);
		else
			paths.paths[i].name.str = NULL;
			
		memcpy(paths.paths[i].name.str, localptr, paths.paths[i].name.length);
		localptr += paths.paths[i].name.length;*/
		paths.paths[i].name = Parse_BPOFstring(localptr);

		/*memcpy(&paths.paths[i].parent.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (paths.paths[i].parent.length)
			paths.paths[i].parent.str.resize(length);
		else
			paths.paths[i].parent.str = NULL;

		memcpy(paths.paths[i].parent.str, localptr, paths.paths[i].parent.length);
		localptr += paths.paths[i].parent.length;*/
		paths.paths[i].parent = Parse_BPOFstring(localptr);

		unsigned int num_verts;
		memcpy(&num_verts, localptr, sizeof(int));
		localptr += sizeof(int);

		paths.paths[i].verts.resize(num_verts);

		for (unsigned int j = 0; j < num_verts; j++)
		{
			memcpy(&paths.paths[i].verts[j].pos, localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);

			memcpy(&paths.paths[i].verts[j].radius, localptr, sizeof(float));
			localptr += sizeof(float);

			unsigned int num_turrets;
			memcpy(&num_turrets, localptr, sizeof(int));
			localptr += sizeof(int);
			
			paths.paths[i].verts[j].sobj_number.resize(num_turrets);

			for (unsigned int k = 0; k < num_turrets; k++)
			{
				memcpy(&paths.paths[i].verts[j].sobj_number[k], localptr, sizeof(int));
				localptr += sizeof(int);
			}
		}


	}
	
}


void POF::Parse_Memory_SLDC(char *buffer)
{
	char *localptr = buffer;

	int size;
	memcpy(&size, localptr, sizeof(unsigned int));
	localptr += sizeof(unsigned int);

	shield_collision.tree_data.resize(size);
	memcpy(&shield_collision.tree_data.front(), localptr, size);
}

void POF::Parse_Memory_PINF (char *buffer, int size)
{
	pofinfo.strings.resize(size);
	if (size) {
		memcpy(&pofinfo.strings.front(), buffer, size);
	}

}

void POF::Parse_Memory_SPCL(char *buffer)
{
	//specials
	char *localptr = buffer;

	unsigned int num_special_points;
	memcpy(&num_special_points, localptr, sizeof(int));
	localptr += sizeof(int);

	specials.special_points.resize(num_special_points);

	for (unsigned int i = 0; i < num_special_points; i++)
	{
		/*memcpy(&specials.special_points[i].name.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (specials.special_points[i].name.length)
			specials.special_points[i].name.str = new char[specials.special_points[i].name.length];
		else
			specials.special_points[i].name.str = NULL;

		memcpy(specials.special_points[i].name.str, localptr, specials.special_points[i].name.length);
		localptr += specials.special_points[i].name.length;*/
		specials.special_points[i].name = Parse_BPOFstring(localptr);

		/*memcpy(&specials.special_points[i].properties.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (specials.special_points[i].properties.length)
			specials.special_points[i].properties.str = new char[specials.special_points[i].properties.length];
		else
			specials.special_points[i].properties.str = NULL;

		memcpy(specials.special_points[i].properties.str, localptr, specials.special_points[i].properties.length);
		localptr += specials.special_points[i].properties.length;*/
		specials.special_points[i].properties = Parse_BPOFstring(localptr);

		memcpy(&specials.special_points[i].point, localptr, sizeof(vector3d));
		localptr += sizeof(vector3d);

		memcpy(&specials.special_points[i].radius, localptr, sizeof(float));
		localptr += sizeof(float);
	}
}



void POF::Parse_Memory_HDR2(char *buffer)
{
	unsigned int i, first = sizeof(float) + (2 * sizeof(int)) + (2 * sizeof(vector3d));
	char *localptr = buffer;
	//header
	//i get to cheat on the first 6 vars because they are all non-dyamic and i can pull chiggy stuff :)
	memcpy(&header, localptr, first);
	localptr += first;

	unsigned int num_detaillevels;
	memcpy(&num_detaillevels, localptr, sizeof(int));
	localptr += sizeof(int);

	header.sobj_detaillevels.resize(num_detaillevels);

	for (i = 0; i < num_detaillevels; i++)
	{
		memcpy(&header.sobj_detaillevels[i], localptr, sizeof(int));
		localptr += sizeof(int);
	}
	
	unsigned int num_debris;
	memcpy(&num_debris, localptr, sizeof(int));
	localptr += sizeof(int);

	header.sobj_debris.resize(num_debris);

	for (i = 0; i < num_debris; i++)
	{
		memcpy(&header.sobj_debris[i], localptr, sizeof(int));
		localptr += sizeof(int);
	}
	
	memcpy(&header.mass, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&header.mass_center, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	memcpy(header.moment_inertia, localptr, sizeof(float) * 9);
	localptr += (sizeof(float) * 9);

	int num_cross_sections;
	memcpy(&num_cross_sections, localptr, sizeof(int));
	localptr += sizeof(int);

	// dunno how this happened.. but i've seen it on atleast one model (created by PCS1 :( ) -- kaz
	if (num_cross_sections == -1)
		num_cross_sections = 0;

	header.cross_sections.resize(num_cross_sections);

	for (i = 0; i < (unsigned int)num_cross_sections; i++)
	{
		memcpy(&header.cross_sections[i], localptr, sizeof(cross_section));
		localptr += sizeof(cross_section);
	}

	unsigned int num_lights;
	memcpy(&num_lights, localptr, sizeof(int));
	localptr += sizeof(int);

	header.lights.resize(num_lights);

	for (i = 0; i < num_lights; i++)
	{
		memcpy(&header.lights[i], localptr, sizeof(muzzle_light));
		localptr += sizeof(muzzle_light);
	}
}


void POF::Parse_Memory_TXTR(char *buffer)
{
	//textures
	char *localptr = buffer;

	unsigned int num_textures;
	memcpy(&num_textures, localptr, sizeof(int));
	localptr += sizeof(int);

	textures.tex_filename.resize(num_textures);

	for (unsigned int i = 0; i < num_textures; i++)
	{
		/*memcpy(&textures.tex_filename[i].length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (textures.tex_filename[i].length)
			textures.tex_filename[i].str = new char[textures.tex_filename[i].length];
		else
			textures.tex_filename[i].str = NULL;

		memcpy(textures.tex_filename[i].str, localptr, textures.tex_filename[i].length);
		localptr += textures.tex_filename[i].length;*/
		textures.tex_filename[i] = Parse_BPOFstring(localptr);
	}
}


//Error Codes
// -1: PSPO signature failed
// -2: Version < 2116
// -3: Contains Unknown Chunk - malformed data?
int POF::LoadPOF(std::ifstream &infile)
{
	char main_buffer[5], secondary_buffer[5];
	boost::scoped_array<char> dynamic_buffer;
	int len;
	std::string sig("PSPO");

	memset(main_buffer, 0, 5);
	memset(secondary_buffer, 0, 5);
	
	infile.read(main_buffer, 4);

	if (std::string(main_buffer) != sig)
	{
		return -1;
	}

	infile.read(main_buffer, 4);
	memcpy(&version, main_buffer, 4);

	if (version < 2116)
	{
		return -2;
	}

	while (1) // enter the read loop;
	{
		memset(main_buffer, 0, 5);
		memset(secondary_buffer, 0, 5);
		
		if(!infile.read(main_buffer, 4))
			break; // EOF

		// get chunk length
		infile.read(secondary_buffer, 4);
		memcpy(&len, secondary_buffer, 4);

		dynamic_buffer.reset(new char[len]);
		infile.read(dynamic_buffer.get(), len);

		if (!_strnicmp(main_buffer, "TXTR", 4))
			Parse_Memory_TXTR(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "HDR2", 4))
			Parse_Memory_HDR2(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "OBJ2", 4))
			Parse_Memory_OBJ2(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "SPCL", 4))
			Parse_Memory_SPCL(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "GPNT", 4))
			Parse_Memory_GPNT(dynamic_buffer.get());
		
		else if (!_strnicmp(main_buffer, "MPNT", 4))
			Parse_Memory_MPNT(dynamic_buffer.get());
		
		else if (!_strnicmp(main_buffer, "TGUN", 4))
			Parse_Memory_TGUN(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "TMIS", 4))
			Parse_Memory_TMIS(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "DOCK", 4))
			Parse_Memory_DOCK(dynamic_buffer.get());
		
		else if (!_strnicmp(main_buffer, "FUEL", 4))
			Parse_Memory_FUEL(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "SHLD", 4))
			Parse_Memory_SHLD(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "EYE ", 4))
			Parse_Memory_EYE(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "ACEN", 4))
			Parse_Memory_ACEN(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "INSG", 4))
			Parse_Memory_INSG(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "PATH", 4))
			Parse_Memory_PATH(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "GLOW", 4))
			Parse_Memory_GLOW(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "SLDC", 4))
			Parse_Memory_SLDC(dynamic_buffer.get());

		else if (!_strnicmp(main_buffer, "PINF ", 4))
			Parse_Memory_PINF (dynamic_buffer.get(), len);

		// if it's not recognized it's simply skipped
	}

	// new version
	version = 2117;

	return 0;
}// must be binary mode


bool POF::SavePOF(std::ofstream &outfile) // must be binary mode
{
	unsigned int size, i, j, k, itemp;
	OBJ2 *local_sobj;
	
	// write signature
	write_to_file(outfile, "PSPO");

	// and version [should be 2117]
	write_to_file(outfile, version);

	//1  TXTR ----------------------------------------
	if (textures.tex_filename.size() > 0)
	{
		write_to_file(outfile, "TXTR");
		
		//calculate size
		size = 4 + (textures.tex_filename.size() * 4);
		for (i = 0; i < textures.tex_filename.size(); i++)
		{
			size += textures.tex_filename[i].length();
		}

		write_to_file(outfile, size);

		// write num_textures
		write_to_file(outfile, (int)textures.tex_filename.size());

		for (i = 0; i < textures.tex_filename.size(); i++)
		{
			// write out the length of the string
			itemp = textures.tex_filename[i].length();
			write_to_file(outfile, itemp);

			// write the string
			write_to_file(outfile, textures.tex_filename[i]);
		}

	}

	//2  HDR2 ----------------------------------------
	// struct sig
	write_to_file(outfile, "HDR2");

	//struct size
	size = 104; // static size;
	size += (header.sobj_detaillevels.size() * 4);
	size += (header.sobj_debris.size() * 4);
	size += (header.cross_sections.size() * sizeof(cross_section));
	size += (header.lights.size() * sizeof(muzzle_light));
	
	write_to_file(outfile, size);

	// output data
	write_to_file(outfile, header.max_radius); 

	write_to_file(outfile, header.obj_flags); 

	write_to_file(outfile, header.num_subobjects); 

	write_to_file(outfile, header.min_bounding); 

	write_to_file(outfile, header.max_bounding); 

	//+-+-+-+-+-+-+-+-



	write_to_file(outfile, (int)header.sobj_detaillevels.size()); 

	// crap the sobj_detaillevels array out
	for (i = 0; i < header.sobj_detaillevels.size(); i++)
	{
		write_to_file(outfile, header.sobj_detaillevels[i]); 
	}

	//+-+-+-+-+-+-+-+-

	
	write_to_file(outfile, (int)header.sobj_debris.size()); 

	// crap the sobj_debris array out
	for (i = 0; i < header.sobj_debris.size(); i++)
	{
		write_to_file(outfile, header.sobj_debris[i]); 
	}

	//+-+-+-+-+-+-+-+-


	write_to_file(outfile, header.mass); 

	write_to_file(outfile, header.mass_center); 

	write_to_file(outfile, header.moment_inertia); 

	//+-+-+-+-+-+-+-+-

	write_to_file(outfile, (int)header.cross_sections.size()); 

	// crap the sobj_debris array out
	for (i = 0; i < header.cross_sections.size(); i++)
	{
		write_to_file(outfile, header.cross_sections[i]); 
	}

	//+-+-+-+-+-+-+-+-

	
	write_to_file(outfile, (int)header.lights.size()); 

	// crap the sobj_debris array out
	for (i = 0; i < header.lights.size(); i++)
	{
		write_to_file(outfile, header.lights[i]); 
	}

	//3  OBJ2 ----------------------------------------
	for (i = 0; i < OBJ2_Count(); i++)
	{
		local_sobj = &objects[i];
		// struct sig
		write_to_file(outfile, "OBJ2");
		
		//struct size
		size = 84 + local_sobj->submodel_name.length() + local_sobj->properties.length() + local_sobj->bsp_data.size();
		
		write_to_file(outfile, size);

		write_to_file(outfile, local_sobj->submodel_number); 

		write_to_file(outfile, local_sobj->radius); 

		write_to_file(outfile, local_sobj->submodel_parent); 

		write_to_file(outfile, local_sobj->offset); 

		write_to_file(outfile, local_sobj->geometric_center); 

		write_to_file(outfile, local_sobj->bounding_box_min_point); 

		write_to_file(outfile, local_sobj->bounding_box_max_point); 

		itemp = local_sobj->submodel_name.length();
		write_to_file(outfile, itemp); 

		write_to_file(outfile, local_sobj->submodel_name);

		itemp = local_sobj->properties.length();
		write_to_file(outfile, itemp); 

		write_to_file(outfile, local_sobj->properties);

		write_to_file(outfile, local_sobj->movement_type); 

		write_to_file(outfile, local_sobj->movement_axis); 

		write_to_file(outfile, static_cast<int>(local_sobj->reserved)); 

		write_to_file(outfile, static_cast<int>(local_sobj->bsp_data.size()));

		write_to_file(outfile, local_sobj->bsp_data);
	}

	//4  SPCL ----------------------------------------
	if (SPCL_Count() > 0)
	{
		// struct sig
		write_to_file(outfile, "SPCL");
		
		//struct size
		size = 4;
		for (i = 0; i < SPCL_Count(); i++)
		{
			size += 24 + specials.special_points[i].name.length() + specials.special_points[i].properties.length();
		}
		
		write_to_file(outfile, size);
		
		write_to_file(outfile, (int)specials.special_points.size()); 
	
		for (i = 0; i < SPCL_Count(); i++)
		{
			itemp = specials.special_points[i].name.length();
			write_to_file(outfile, itemp); 
			write_to_file(outfile, specials.special_points[i].name);	

			itemp = specials.special_points[i].properties.length();
			write_to_file(outfile, itemp); 
			write_to_file(outfile, specials.special_points[i].properties);	

			write_to_file(outfile, specials.special_points[i].point); 
			
			write_to_file(outfile, specials.special_points[i].radius); 

		}

	}

	//5  GPNT ----------------------------------------
	if (GPNT_SlotCount() > 0)
	{


		// struct sig
		write_to_file(outfile, "GPNT");
		
		//struct size
		size = 4 + (4 * GPNT_SlotCount());


		for (i = 0; i < GPNT_SlotCount(); i++)
		{
			size += PNT_Alias(0)->slots[i].guns.size() * sizeof(gun);
		}

		write_to_file(outfile, size);

		write_to_file(outfile, (int)PNT_Alias(0)->slots.size()); 

		for (i = 0; i < GPNT_SlotCount(); i++)
		{
			write_to_file(outfile, (int)PNT_Alias(0)->slots[i].guns.size()); 

			for (j = 0; j < PNT_Alias(0)->slots[i].guns.size(); j++)
			{
				write_to_file(outfile, PNT_Alias(0)->slots[i].guns[j].point); 

				write_to_file(outfile, PNT_Alias(0)->slots[i].guns[j].norm); 

			}

		}
	}

	//6  MPNT ----------------------------------------
	if (MPNT_SlotCount() > 0)
	{


		// struct sig
		write_to_file(outfile, "MPNT");
		
		//struct size
		size = 4 + (4 * MPNT_SlotCount());


		for (i = 0; i < MPNT_SlotCount(); i++)
		{
			size += PNT_Alias(1)->slots[i].guns.size() * sizeof(gun);
		}
		
		write_to_file(outfile, size);

		write_to_file(outfile, (int)PNT_Alias(1)->slots.size()); 

		for (i = 0; i < MPNT_SlotCount(); i++)
		{
			write_to_file(outfile, (int)PNT_Alias(1)->slots[i].guns.size()); 

			for (j = 0; j < PNT_Alias(1)->slots[i].guns.size(); j++)
			{
				write_to_file(outfile, PNT_Alias(1)->slots[i].guns[j].point); 

				write_to_file(outfile, PNT_Alias(1)->slots[i].guns[j].norm); 

			}

		}
	}

	//7  TGUN ----------------------------------------
	if (T_Alias(0)->banks.size() > 0)
	{
		// struct sig
		write_to_file(outfile, "TGUN");
		
		//struct size
		size = 4 + (24 * T_Alias(0)->banks.size());


		for (i = 0; i < T_Alias(0)->banks.size(); i++)
		{
			size += (T_Alias(0)->banks[i].position.size() * 12);
		}
		
		write_to_file(outfile, size);

		write_to_file(outfile, (int)T_Alias(0)->banks.size()); 

		for (i = 0; i < T_Alias(0)->banks.size(); i++)
		{

			write_to_file(outfile, T_Alias(0)->banks[i].sobj_parent); 

			write_to_file(outfile, T_Alias(0)->banks[i].sobj_par_phys); 

			write_to_file(outfile, T_Alias(0)->banks[i].turret_normal); 

			write_to_file(outfile, (int)T_Alias(0)->banks[i].position.size()); 

			for (j = 0; j < T_Alias(0)->banks[i].position.size(); j++)
			{
					write_to_file(outfile, T_Alias(0)->banks[i].position[j]); 
			}
		}

	}

	//8  TMIS ----------------------------------------
	if (T_Alias(1)->banks.size() > 0)
	{
		// struct sig
		write_to_file(outfile, "TMIS");
		
		//struct size
		size = 4 + (24 * T_Alias(1)->banks.size());


		for (i = 0; i < T_Alias(1)->banks.size(); i++)
		{
			size += (T_Alias(1)->banks[i].position.size() * 12);
		}
		
		write_to_file(outfile, size);

		write_to_file(outfile, (int)T_Alias(1)->banks.size()); 

		for (i = 0; i < T_Alias(1)->banks.size(); i++)
		{

			write_to_file(outfile, T_Alias(1)->banks[i].sobj_parent); 

			write_to_file(outfile, T_Alias(1)->banks[i].sobj_par_phys); 

			write_to_file(outfile, T_Alias(1)->banks[i].turret_normal); 

			write_to_file(outfile, (int)T_Alias(1)->banks[i].position.size()); 

			for (j = 0; j < T_Alias(1)->banks[i].position.size(); j++)
			{
					write_to_file(outfile, T_Alias(1)->banks[i].position[j]); 
			}
		}

	}

	//9  DOCK ----------------------------------------
	if (docking.points.size() > 0)
	{
		write_to_file(outfile, "DOCK");

		size = 4;

		for (i = 0; i < docking.points.size(); i++)
		{
			size += 4;
			size += docking.points[i].properties.length();
			size += 4;
			size += (4 * docking.points[i].path_number.size());
			size += 4;
			size += (sizeof(gun) * docking.points[i].points.size());
		}


		write_to_file(outfile, size); 

		write_to_file(outfile, (int)docking.points.size()); 

		for (i = 0; i < docking.points.size(); i++)
		{
			itemp = docking.points[i].properties.length();
			write_to_file(outfile, itemp); 
			write_to_file(outfile, docking.points[i].properties);

			write_to_file(outfile, (int)docking.points[i].path_number.size()); 

			for (j = 0; j < docking.points[i].path_number.size(); j++)
			{
				write_to_file(outfile, docking.points[i].path_number[j]); 
			}

			write_to_file(outfile, (int)docking.points[i].points.size()); 

			for (k = 0; k < docking.points[i].points.size(); k++)
			{
				write_to_file(outfile, docking.points[i].points[k].point); 

				write_to_file(outfile, docking.points[i].points[k].norm); 
			}


		}
	}

	//10 FUEL ----------------------------------------
	if (FUEL_Count_Thrusters() > 0)
	{
		write_to_file(outfile, "FUEL");

		size = 4 + (8 * FUEL_Count_Thrusters());

		for (i = 0; i < FUEL_Count_Thrusters(); i++)
		{
			size += (thrusters.thrusters[i].properties.length() + (sizeof(glow_point) * thrusters.thrusters[i].points.size()));
		}

		write_to_file(outfile, size); 

		write_to_file(outfile, (int)thrusters.thrusters.size()); 

		for (i = 0; i < FUEL_Count_Thrusters(); i++)
		{
			write_to_file(outfile, (int)thrusters.thrusters[i].points.size()); 

			itemp = thrusters.thrusters[i].properties.length();
			write_to_file(outfile, itemp); 
			write_to_file(outfile, thrusters.thrusters[i].properties);

			for (j = 0; j < FUEL_Count_Glows(i); j++)
			{
				write_to_file(outfile, thrusters.thrusters[i].points[j]); 

			}

			

		}
	}

	//11 SHLD ----------------------------------------
	if (shields.shield_faces.size() > 0 || shields.vertecies.size() > 0)
	{
		write_to_file(outfile, "SHLD");

		size = 8 + (sizeof(vector3d) * shields.vertecies.size()) + (sizeof(shield_face) * shields.shield_faces.size());

		write_to_file(outfile, size); 

		write_to_file(outfile, (int)shields.vertecies.size()); 

		for (i = 0; i < shields.vertecies.size(); i++)
		{
			write_to_file(outfile, shields.vertecies[i]); 
		}

		write_to_file(outfile, (int)shields.shield_faces.size()); 


		for (i = 0; i < shields.shield_faces.size(); i++)
		{
			write_to_file(outfile, shields.shield_faces[i]); 
		}
	}

	//12  EYE ----------------------------------------
	if (eyes.eye_positions.size() > 0)
	{
		outfile.write("EYE ", 4);

		size = 4 + (sizeof(eye_pos) * eyes.eye_positions.size());

		write_to_file(outfile, size); 

		write_to_file(outfile, (int)eyes.eye_positions.size()); 

		for (i = 0; i < eyes.eye_positions.size(); i++)
		{
			write_to_file(outfile, eyes.eye_positions[i]); 
		}
	}
	//13 ACEN ----------------------------------------
	if (ACEN_IsSet())
	{
		write_to_file(outfile, "ACEN");
		k = 12;
		
		write_to_file(outfile, k); 

		write_to_file(outfile, autocentering.point); 

	}

	//14 INSG ----------------------------------------
	if (insignia.insignias.size() > 0)
	{

		write_to_file(outfile, "INSG");

		size = 4; // num_insignias

		for (i = 0; i < insignia.insignias.size(); i++)
		{
			size += 24; // detail_level 4|4, num_faces 4|8, num_vertices 4|12, offset 12|24
			size += (sizeof(vector3d) * insignia.insignias[i].vertex_pos.size());
			size += (sizeof(insg_face) * insignia.insignias[i].faces.size());
		}

		write_to_file(outfile, size); 

		write_to_file(outfile, (int)insignia.insignias.size()); 

		for (i = 0; i < insignia.insignias.size(); i++)
		{
			write_to_file(outfile, insignia.insignias[i].detail_level); 

			write_to_file(outfile, (int)insignia.insignias[i].faces.size()); 
	
			write_to_file(outfile, (int)insignia.insignias[i].vertex_pos.size()); 
			
			for (j = 0; j < insignia.insignias[i].vertex_pos.size(); j++)
			{
				write_to_file(outfile, insignia.insignias[i].vertex_pos[j]); 
			}

			write_to_file(outfile, insignia.insignias[i].offset); 

			for (j = 0; j < insignia.insignias[i].faces.size(); j++)
			{
				write_to_file(outfile, insignia.insignias[i].faces[j]);
			}
		}
	}

	//15 PATH ----------------------------------------
	if (paths.paths.size() > 0)
	{

		write_to_file(outfile, "PATH");

		size = 4;

		for (i = 0; i < paths.paths.size(); i++)
		{
			size += (paths.paths[i].name.length() + paths.paths[i].parent.length());
			size += 8; //string size indexcies
			size += 4; //num_verts;

			for (j = 0; j < paths.paths[i].verts.size(); j++)
			{
				//vector pos
				size += 12;
				//float radius
				size += 4;
				//int num_sobj_number
				size += 4;
				// the sobj_number array
				size += (4 * paths.paths[i].verts[j].sobj_number.size());
			}


		}


		write_to_file(outfile, size); 
		outfile.flush();

		write_to_file(outfile, (int)paths.paths.size()); 
		outfile.flush();

		for (i = 0; i < paths.paths.size(); i++)
		{
			itemp = paths.paths[i].name.length();
			write_to_file(outfile, itemp); 
			outfile.flush();

			write_to_file(outfile, paths.paths[i].name);
			outfile.flush();

			itemp = paths.paths[i].parent.length();
			write_to_file(outfile, itemp); 
			outfile.flush();

			write_to_file(outfile, paths.paths[i].parent);
			outfile.flush();

			write_to_file(outfile, (int)paths.paths[i].verts.size()); 

			for (j = 0; j < paths.paths[i].verts.size(); j++)
			{
				write_to_file(outfile, paths.paths[i].verts[j].pos); 

				write_to_file(outfile, paths.paths[i].verts[j].radius); 
			
				write_to_file(outfile, (int)paths.paths[i].verts[j].sobj_number.size()); 

				for (k = 0; k < paths.paths[i].verts[j].sobj_number.size(); k++)
				{
					write_to_file(outfile, paths.paths[i].verts[j].sobj_number[k]); 
				}
			
			}


		}

	}
	// 16 GLOW ----------------------------------------
	if (hull_lights.lights.size() != 0)
	{

		//write signature
		write_to_file(outfile, "GLOW");
		k = 4 + (8 * sizeof(int)) * hull_lights.lights.size(); // each group in binary format is 7 ints + strlen + size of their point array
																 // almost forgot to add 1 int for num_glow_arrays
		for ( i  = 0; i < hull_lights.lights.size(); i++)
		{
			k += hull_lights.lights[i].properties.length();
			k += sizeof(HullLightPoint) * hull_lights.lights[i].lights.size();
		}

		// write size
		write_to_file(outfile, k); 

		// write data
		
		write_to_file(outfile, (int)hull_lights.lights.size());
		outfile.flush();


		for (unsigned int i = 0; i < hull_lights.lights.size(); i++)
		{
			write_to_file(outfile, hull_lights.lights[i].disp_time);
			outfile.flush();

			write_to_file(outfile, hull_lights.lights[i].on_time);
			outfile.flush();

			write_to_file(outfile, hull_lights.lights[i].off_time);
			outfile.flush();

			write_to_file(outfile, hull_lights.lights[i].obj_parent);
			outfile.flush();

			write_to_file(outfile, hull_lights.lights[i].LOD);
			outfile.flush();

			write_to_file(outfile, hull_lights.lights[i].type);
			outfile.flush();
			
			write_to_file(outfile, (int)hull_lights.lights[i].lights.size());
			outfile.flush();

			itemp = hull_lights.lights[i].properties.length();
			write_to_file(outfile, itemp);
			outfile.flush();


			write_to_file(outfile, hull_lights.lights[i].properties);
			outfile.flush();


			for (unsigned int j = 0; j < hull_lights.lights[i].lights.size(); j++)
			{
				write_to_file(outfile, hull_lights.lights[i].lights[j].point);
				outfile.flush();

				write_to_file(outfile, hull_lights.lights[i].lights[j].norm);
				outfile.flush();

				write_to_file(outfile, hull_lights.lights[i].lights[j].radius);
				outfile.flush();
			
			}
		}
	}

	//17 SLDC ----------------------------------------
	if (!shield_collision.tree_data.empty())
	{
		write_to_file(outfile, "SLDC");
		int size = shield_collision.tree_data.size();
		k = sizeof(int) + size;

		write_to_file(outfile, k); 
		write_to_file(outfile, size);
		write_to_file(outfile, shield_collision.tree_data);
	}

	//18 PINF ----------------------------------------
	if (!pofinfo.strings.empty())
	{
		//always terminated with a double null
		write_to_file(outfile, "PINF");

		write_to_file(outfile, (int)pofinfo.strings.size()); 
		write_to_file(outfile, pofinfo.strings);
	}
	
	return true;
}


void POF::ClearAllData()
{
	version = 2117;
	objects.clear();

	textures.tex_filename.clear();
	header = HDR2();
	specials.special_points.clear();
	gunpoints.slots.clear();
	missilepoints.slots.clear();
	turretguns.banks.clear();
	turretmissiles.banks.clear();
	docking.points.clear();
	thrusters.thrusters.clear();
	shields.vertecies.clear();
	shields.shield_faces.clear();
	eyes.eye_positions.clear();
	autocentering.point = vector3d();
	insignia.insignias.clear();
	paths.paths.clear();
	pofinfo.strings.clear();
	hull_lights.lights.clear();
	shield_collision = SLDC();

}


//**********************************************************************************************************************
// These are the actual workhorse functions that will be aliased by the GPNT_ and MPNT_ functions
// Mode 0 = Gun, 1 = missile


GPNT_MPNT* POF::PNT_Alias(int mode) //hehe
{
	// Here is the code to alias the funtion
	// simple to the point of being evil isn't it :)
	if (mode == 1)
		return &missilepoints;
	return &gunpoints;
}

//------------------------------------------

void POF::PNT_AddSlot				(int mode)
{


	PNT_Alias(mode)->slots.push_back(slot());
}

//------------------------------------------


bool POF::PNT_AddPoint				(int mode, int slot, vector3d point, vector3d norm)
{
	if ((unsigned)slot > PNT_SlotCount(mode))
		return false;

	gun newgun;

	newgun.norm = norm;
	newgun.point = point;
	PNT_Alias(mode)->slots[slot].guns.push_back(newgun);
	return true;
}

//------------------------------------------

unsigned int  POF::PNT_SlotCount				(int mode)
{
	
	return PNT_Alias(mode)->slots.size();
}

//------------------------------------------

unsigned int  POF::PNT_PointCount				(int mode, int slot)
{

	if ((unsigned)slot > PNT_Alias(mode)->slots.size())
		return -1; //-1 means array out of bounds

	return PNT_Alias(mode)->slots[slot].guns.size();

}

//------------------------------------------


bool POF::PNT_DelSlot				(int mode, int slot_num)
{
	if ((unsigned)slot_num > PNT_Alias(mode)->slots.size())
		return false;
	PNT_Alias(mode)->slots.erase(PNT_Alias(mode)->slots.begin() + slot_num);
	return true;

}

bool POF::PNT_DelPoint				(int mode, int slot, int point)
{
	if ((unsigned)slot > PNT_Alias(mode)->slots.size())
		return false;

	if ((unsigned)point > PNT_Alias(mode)->slots[slot].guns.size())
		return false;

	PNT_Alias(mode)->slots[slot].guns.erase(PNT_Alias(mode)->slots[slot].guns.begin() + point);
	return true;

}


//------------------------------------------

bool POF::PNT_EditPoint				(int mode, int slot, int point_num, vector3d point, vector3d norm)
{
	if ((unsigned)slot > PNT_Alias(mode)->slots.size())
		return false;

	if ((unsigned)point_num > PNT_Alias(mode)->slots[slot].guns.size())
		return false;

	PNT_Alias(mode)->slots[slot].guns[point_num].point = point;
	PNT_Alias(mode)->slots[slot].guns[point_num].norm = norm;

	return true;

}

//------------------------------------------

bool POF::PNT_GetPoint				(int mode, int slot, int point_num, vector3d &point, vector3d &norm)
{
	if ((unsigned)slot > PNT_Alias(mode)->slots.size())
		return false;

	if ((unsigned)point_num > PNT_Alias(mode)->slots[slot].guns.size())
		return false;

	point = PNT_Alias(mode)->slots[slot].guns[point_num].point;
	norm = PNT_Alias(mode)->slots[slot].guns[point_num].norm;

	return true;
}


//**********************************************************************************************************************
// These are the actual workhorse functions that will be aliased by the TGUN_ and TMIS_ functions
// Mode 0 = Gun, 1 = missile

TGUN_TMIS* POF::T_Alias(int mode) //hehe
{
	// Here is the code to alias the funtion
	// simple to the point of being evil isn't it :)
	if (mode == 1)
		return &turretmissiles;
	return &turretguns;
}

void POF::T_Add_Bank					(int mode, int sobj_parent, int sobj_par_phys, vector3d normal)
{
	Turret_bank nbank;
	nbank.sobj_par_phys = sobj_par_phys;
	nbank.sobj_parent = sobj_parent;
	nbank.turret_normal = normal;
	T_Alias(mode)->banks.push_back(nbank);
}

//------------------------------------------

bool POF::T_Add_FirePoint			(int mode, int bank, vector3d pos)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;
	T_Alias(mode)->banks[bank].position.push_back(pos);
	return true;
}




//------------------------------------------

bool POF::T_Edit_Bank				(int mode, int bank, int sobj_parent, int sobj_par_phys, vector3d normal)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	T_Alias(mode)->banks[bank].sobj_par_phys = sobj_par_phys;
	T_Alias(mode)->banks[bank].sobj_parent = sobj_parent;
	T_Alias(mode)->banks[bank].turret_normal = normal;

	return true;
}

//------------------------------------------

bool POF::T_Edit_FirePoint			(int mode, int bank, int point, vector3d pos)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	if ((unsigned)point > T_Count_Points(mode, bank))
		return false;

	T_Alias(mode)->banks[bank].position[point] = pos;

	return true;

}

//------------------------------------------

bool POF::T_Del_FirePoint			(int mode, int bank, int point)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	if ((unsigned)point > T_Count_Points(mode, bank))
		return false;
	T_Alias(mode)->banks[bank].position.erase(T_Alias(mode)->banks[bank].position.begin() + point);
	return true;
}

//------------------------------------------

bool POF::T_Del_Bank					(int mode, int bank)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;
	T_Alias(mode)->banks.erase(T_Alias(mode)->banks.begin() + bank);
	return true;
}


//------------------------------------------

unsigned int POF::T_Count_Banks				(int mode)
{
	return T_Alias(mode)->banks.size();
}

//------------------------------------------

unsigned int POF::T_Count_Points				(int mode, int bank)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return -1;
	return T_Alias(mode)->banks[bank].position.size();
}

//------------------------------------------

bool POF::T_Get_Bank				(int mode, int bank, int &sobj_parent, int &sobj_par_phys, vector3d &normal)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	sobj_parent = T_Alias(mode)->banks[bank].sobj_parent;
	sobj_par_phys = T_Alias(mode)->banks[bank].sobj_par_phys;
	normal = T_Alias(mode)->banks[bank].turret_normal;

	return true;
}

//------------------------------------------

bool POF::T_Get_FirePoint			(int mode, int bank, int point, vector3d &pos)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	if ((unsigned)point > T_Count_Points(mode, bank))
		return false;

	pos = T_Alias(mode)->banks[bank].position[point];
	

	return true;
}



//**********************************************************************************************************************
// Texture Functions
int POF::TXTR_AddTexture(std::string texname)
{	
	textures.tex_filename.push_back(APStoString(texname));
	return 0;
}

//----------------------------------------------

bool POF::TXTR_DelTexture(int texture)
{
	textures.tex_filename.erase(textures.tex_filename.begin() + texture);
	return true;

}

//----------------------------------------------

int POF::TXTR_FindTexture(std::string texname)
{
	//char *temp;

	for (unsigned int i = 0; i < textures.tex_filename.size(); i++)
	{
		//temp = new char[textures.tex_filename[i].length + 1];

		//memset(temp, 0, textures.tex_filename[i].length + 1);

		//strncpy(temp, textures.tex_filename[i].str, textures.tex_filename[i].length);

		//if (std::string(temp) == texname)
		//	return i;
		if (texname == textures.tex_filename[i])
			return i;

		//delete[] temp;
	}

	return -1;
}

//----------------------------------------------
bool POF::TXTR_Edit_Texture(int texture, std::string textname)
{
	if (texture < 0 || (unsigned)texture > textures.tex_filename.size())
		return false;

	textures.tex_filename[texture] = APStoString(textname);
	return true;
}

//----------------------------------------------

std::string POF::TXTR_GetTextures(int texture)
{
	if (texture < 0 || (unsigned)texture > textures.tex_filename.size())
		return std::string("Error: Invalid Index");
	
	return StringToAPS(textures.tex_filename[texture]);

}




//**********************************************************************************************************************
// Header Functions

void POF::HDR2_Get_Details				(int &num, std::vector<int> &SOBJ_nums)
{
	num = header.sobj_detaillevels.size();
	SOBJ_nums = header.sobj_detaillevels;
}


void POF::HDR2_Set_Details				(int num, std::vector<int> SOBJ_nums)
{
	header.sobj_detaillevels = SOBJ_nums;
}

//----------------------------------------------

void POF::HDR2_Get_Debris				(int &num, std::vector<int> &SOBJ_nums)
{
	num = header.sobj_debris.size();
	SOBJ_nums = header.sobj_debris;
}

void POF::HDR2_Set_Debris				(int num, std::vector<int> SOBJ_nums)
{
	header.sobj_debris = SOBJ_nums;
}

//----------------------------------------------

void POF::HDR2_Get_MomentInertia			(float inertia[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			inertia[i][j] = header.moment_inertia[i][j];
}

void POF::HDR2_Set_MomentInertia			(float inertia[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			header.moment_inertia[i][j] = inertia[i][j];

}

//----------------------------------------------

void POF::HDR2_Get_CrossSections		(int &num, std::vector<cross_section> &sections)
{
	num = header.cross_sections.size();
	sections = header.cross_sections;
}

void POF::HDR2_Set_CrossSections		(int num, std::vector<cross_section> sections)
{
	header.cross_sections = sections;
}

//----------------------------------------------

void POF::HDR2_Get_Lights				(int &num, std::vector<muzzle_light> &li)
{
	num = header.lights.size();
	li = header.lights;
}

void POF::HDR2_Set_Lights				(int num, const std::vector<muzzle_light> &li)
{
	header.lights = li;
}


//**********************************************************************************************************************
// Object [OBJ2] Stuff
int  POF::OBJ2_Add						(OBJ2 &obj)
{
	int i = OBJ2_Add_SOBJ();
	objects[i] = obj;

	return i;
}


int  POF::OBJ2_Add_SOBJ					()
{
	objects.push_back(OBJ2());
	return objects.size() - 1;
}

//----------------------------------------------

bool POF::OBJ2_Del_SOBJ					(int SOBJNum)
{
	objects.erase(objects.begin() + SOBJNum);
	return true;
}

//----------------------------------------------

unsigned int POF::OBJ2_Count()
{
	return objects.size();
}

//----------------------------------------------

bool POF::OBJ2_Set_SOBJNum				(int SOBJNum, int num)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].submodel_number = num;

	return true;
}

bool POF::OBJ2_Get_SOBJNum				(int SOBJNum, int &num)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	num = objects[SOBJNum].submodel_number;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_Radius				(int SOBJNum, float rad)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].radius = rad;

	return true;
}

bool POF::OBJ2_Get_Radius				(int SOBJNum, float &rad)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	rad = objects[SOBJNum].radius;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_Parent				(int SOBJNum, int parent)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].submodel_parent = parent;

	return true;
}


bool POF::OBJ2_Get_Parent				(int SOBJNum, int &parent)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	parent = objects[SOBJNum].submodel_parent;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_Offset				(int SOBJNum, vector3d offset)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].offset = offset;

	return true;
}


bool POF::OBJ2_Get_Offset				(int SOBJNum, vector3d &offset)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	offset = objects[SOBJNum].offset;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_GeoCenter			(int SOBJNum, vector3d GeoCent)
{
	
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].geometric_center = GeoCent;

	return true;

}

bool POF::OBJ2_Get_GeoCenter			(int SOBJNum, vector3d &GeoCent)
{
	
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	GeoCent = objects[SOBJNum].geometric_center;

	return true;

}

//----------------------------------------------

bool POF::OBJ2_Set_BoundingMin			(int SOBJNum, vector3d min)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].bounding_box_min_point = min;

	return true;
}


bool POF::OBJ2_Get_BoundingMin			(int SOBJNum, vector3d &min)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	min = objects[SOBJNum].bounding_box_min_point;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_BoundingMax			(int SOBJNum, vector3d max)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].bounding_box_max_point = max;

	return true;
}

bool POF::OBJ2_Get_BoundingMax			(int SOBJNum, vector3d &max)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	max = objects[SOBJNum].bounding_box_max_point;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_Name					(int SOBJNum, std::string name)
{

	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].submodel_name = APStoString(name);

	return true;
}

bool POF::OBJ2_Get_Name					(int SOBJNum, std::string &name)
{

	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	name = StringToAPS(objects[SOBJNum].submodel_name);

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_Props					(int SOBJNum, std::string properties)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].properties = APStoString(properties);

	return true;
}

bool POF::OBJ2_Get_Props					(int SOBJNum, std::string &properties)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	properties = StringToAPS(objects[SOBJNum].properties);

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_MoveType				(int SOBJNum, int type)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;
	objects[SOBJNum].movement_type = type;

	return true;
}

bool POF::OBJ2_Get_MoveType				(int SOBJNum, int &type)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	type = objects[SOBJNum].movement_type;

	return true;
}

//----------------------------------------------

bool POF::OBJ2_Set_MoveAxis				(int SOBJNum, int axis)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	objects[SOBJNum].movement_axis = axis;

	return true;
}

bool POF::OBJ2_Get_MoveAxis				(int SOBJNum, int &axis)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	axis = objects[SOBJNum].movement_axis;

	return true;
}

//----------------------------------------------

unsigned int POF::OBJ2_BSP_Datasize(int SOBJNum)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return -1;
	return objects[SOBJNum].bsp_data.size();

}

//----------------------------------------------

bool POF::OBJ2_Get_BSPDataPtr			(int SOBJNum, int &size, char* &bsp_data)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	size = OBJ2_BSP_Datasize(SOBJNum);

	if (size == 0)
	{
		bsp_data = NULL;
		return true;
	}

	bsp_data = &objects[SOBJNum].bsp_data.front();

	return true;
}

bool POF::OBJ2_Get_BSPData				(int SOBJNum, std::vector<char> &bsp_data)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	int size = OBJ2_BSP_Datasize(SOBJNum);

	if (size == 0)
	{
		bsp_data.clear();
		return true;
	}
	bsp_data = objects[SOBJNum].bsp_data;

	return true;
}



//**********************************************************************************************************************
//'SPCL' specials functions

void POF::SPCL_AddSpecial				(std::string Name, std::string Properties, vector3d point, float radius)
{
// string APStoString(apstring aps)
	special_point npoint;
	npoint.name = APStoString(Name);
	npoint.point = point;
	npoint.properties = APStoString(Properties);
	npoint.radius = radius;
	specials.special_points.push_back(npoint);
}

//----------------------------------------------

bool POF::SPCL_DelSpecial				(int num)
{
	if ((unsigned)num > SPCL_Count())
		return false;
	specials.special_points.erase(specials.special_points.begin() + num);
	return true;
}

//----------------------------------------------

unsigned int  POF::SPCL_Count					()
{
	return specials.special_points.size();
}

//----------------------------------------------

bool POF::SPCL_Get_Special				(int num, std::string &Name, std::string &Properties, vector3d &point, float &radius)
{
	if ((unsigned)num > SPCL_Count())
		return false;

	Name = StringToAPS(specials.special_points[num].name);
	Properties = StringToAPS(specials.special_points[num].properties);
	point = specials.special_points[num].point;
	radius = specials.special_points[num].radius;
	return true;
}

//----------------------------------------------

bool POF::SPCL_Edit_Special				(int num, std::string Name, std::string Properties, vector3d point, float radius)
{
	//apstring StringToAPS(string str)

	if ((unsigned)num > SPCL_Count())
		return false;
	
	specials.special_points[num].name = APStoString(Name);
	specials.special_points[num].properties = APStoString(Properties);
	specials.special_points[num].point = point;
	specials.special_points[num].radius = radius;

	return true;
}



//**********************************************************************************************************************
// DOCK Functions

void POF::DOCK_Add_Dock				(std::string properties)
{
	dock_point npt;
	npt.properties = APStoString(properties);
	docking.points.push_back(npt);
}

bool POF::DOCK_Add_SplinePath		(int dock, int path)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;
	docking.points[dock].path_number.push_back(path);
	return true;
}

bool POF::DOCK_Add_Point			(int dock, vector3d point, vector3d norm)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;
	gun np;
	np.norm = norm;
	np.point = point;
	docking.points[dock].points.push_back(np);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::DOCK_Count_Docks			()
{
	return docking.points.size();
}

unsigned int POF::DOCK_Count_SplinePaths		(int dock)
{
	if (dock < 0  || (unsigned)dock > docking.points.size())
		return -1;
	return docking.points[dock].path_number.size();
}

unsigned int POF::DOCK_Count_Points			(int dock)
{
	if (dock < 0  || (unsigned)dock > docking.points.size())
		return -1;
	return docking.points[dock].points.size();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::DOCK_Get_SplinePath		(int dock, int spline_path_num, int &path)
{
	if (dock < 0  || spline_path_num < 0 || (unsigned)dock > DOCK_Count_Docks() || (unsigned)spline_path_num > DOCK_Count_SplinePaths(dock))
		return false;

	path = docking.points[dock].path_number[spline_path_num];
	return true;
}


bool POF::DOCK_Get_Point			(int dock, int point, vector3d &pnt, vector3d &norm)
{
	if (dock < 0  || point < 0 || (unsigned)dock > DOCK_Count_Docks() || (unsigned)point > DOCK_Count_Points(dock))
		return false;

	norm = docking.points[dock].points[point].norm;
	pnt = docking.points[dock].points[point].point;
	return true;
}

bool POF::DOCK_Get_DockProps		(int dock, std::string &properties)
{
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;

	properties = StringToAPS(docking.points[dock].properties);

	return true;
}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::DOCK_Edit_SplinePath		(int dock, int spline_path_num, int path)
{
	if (dock < 0  || spline_path_num < 0 || (unsigned)dock > DOCK_Count_Docks() || (unsigned)spline_path_num > DOCK_Count_SplinePaths(dock))
		return false;

	docking.points[dock].path_number[spline_path_num] = path;
	return true;
}

bool POF::DOCK_Edit_Point			(int dock, int point, vector3d pnt, vector3d norm)
{
	if (dock < 0  || point < 0 || (unsigned)dock > DOCK_Count_Docks() || (unsigned)point > DOCK_Count_Points(dock))
		return false;

	docking.points[dock].points[point].norm = norm;
	docking.points[dock].points[point].point = pnt;
	return true;
}


bool POF::DOCK_Edit_DockProps		(int dock, std::string properties)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;

	docking.points[dock].properties = APStoString(properties);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::DOCK_Del_Dock				(int dock)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;
	docking.points.erase(docking.points.begin() + dock);
	return true;

}

bool POF::DOCK_Del_SplinePath		(int dock, int spline_path_num)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks() || (unsigned)spline_path_num > DOCK_Count_SplinePaths(dock))
		return false;
	docking.points[dock].path_number.erase(docking.points[dock].path_number.begin() + spline_path_num);
	return true;
}

bool POF::DOCK_Del_Point			(int dock, int point)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks() || (unsigned)point > DOCK_Count_Points(dock))
		return false;
	docking.points[dock].points.erase(docking.points[dock].points.begin() + point);
	return true;

}

//**********************************************************************************************************************
// FUEL Functions

void POF::FUEL_Add_Thruster			(std::string properties)
{
	thruster n;
	n.properties = APStoString(properties);
	thrusters.thrusters.push_back(n);
}

bool POF::FUEL_Add_GlowPoint		(int bank, float radius, vector3d pos, vector3d norm)
{
	if ((unsigned)bank > FUEL_Count_Thrusters())
		return false;
	// new glow
	glow_point ng;
	ng.norm = norm;
	ng.pos = pos;
	ng.radius = radius;
	thrusters.thrusters[bank].points.push_back(ng);
	return true;	
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::FUEL_Count_Thrusters		()
{ return thrusters.thrusters.size(); }

unsigned int POF::FUEL_Count_Glows			(int thruster)
{ 
	if ((unsigned)thruster > FUEL_Count_Thrusters())
		return -1;
	return thrusters.thrusters[thruster].points.size();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::FUEL_Edit_GlowPoint		(int thruster, int gp, float radius, vector3d pos, vector3d norm)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters() || (unsigned)gp > FUEL_Count_Glows(thruster))
		return false;

	thrusters.thrusters[thruster].points[gp].radius = radius;
	thrusters.thrusters[thruster].points[gp].pos = pos ;
	thrusters.thrusters[thruster].points[gp].norm = norm;

	return true;
}

bool POF::FUEL_Edit_ThrusterProps	(int thruster, std::string properties)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters())
		return false;
	thrusters.thrusters[thruster].properties = APStoString(properties);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


bool POF::FUEL_Del_Thruster			(int thrust)
{
	if ((unsigned)thrust > FUEL_Count_Thrusters())
		return false;
	thrusters.thrusters.erase(thrusters.thrusters.begin() + thrust);
	return true;

}


bool POF::FUEL_Del_GlowPoint			(int thruster, int glowpoint)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters() || (unsigned)glowpoint > FUEL_Count_Glows(thruster))
		return false;
	thrusters.thrusters[thruster].points.erase(thrusters.thrusters[thruster].points.begin() + glowpoint);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::FUEL_Get_GlowPoint			(int thruster, int gp, float &radius, vector3d &pos, vector3d &norm)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters() || (unsigned)gp > FUEL_Count_Glows(thruster))
		return false;

	radius = thrusters.thrusters[thruster].points[gp].radius;
	pos = thrusters.thrusters[thruster].points[gp].pos;
	norm = thrusters.thrusters[thruster].points[gp].norm;

	return true;
}

bool POF::FUEL_Get_ThrusterProps		(int thruster, std::string &properties)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters())
		return false;
	properties = StringToAPS(thrusters.thrusters[thruster].properties);

	return true;
}



//**********************************************************************************************************************
// SHLD Functions
void POF::SHLD_Add_Vertex			(vector3d vert)
{
	shields.vertecies.push_back(vert);
}

void POF::SHLD_Add_Face				(vector3d normal, const int fcs[3], const int nbs[3])
{
	shield_face nface;

	nface.face_normal = normal;

	for (int i = 0; i < 3; i++)
	{
		nface.face_vertices[i] = fcs[i];
		nface.neighbors[i] = nbs[i];
	}
	shields.shield_faces.push_back(nface);
}
								// fcs = int[3] faces, nbs = int[3] neighbors
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::SHLD_Count_Vertices		()
{
	return shields.vertecies.size();
}

unsigned int POF::SHLD_Count_Faces			()
{
	return shields.shield_faces.size();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::SHLD_Get_Face				(int face, vector3d &normal, int fcs[3], int nbs[3])
{
	if ((unsigned)face > SHLD_Count_Faces())
		return false;
	normal = shields.shield_faces[face].face_normal;

	for (int i = 0; i < 3; i++)
	{
		fcs[i] = shields.shield_faces[face].face_vertices[i];
		nbs[i] = shields.shield_faces[face].neighbors[i];
	}

	return true;
}

bool POF::SHLD_Get_Vertex			(int vertex, vector3d &vert)
{
	if ((unsigned)vertex > SHLD_Count_Vertices())
		return false;
	vert = shields.vertecies[vertex];

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::SHLD_Edit_Vertex			(int vertex, vector3d &vert)
{
	if ((unsigned)vertex > SHLD_Count_Vertices())
		return false;
	shields.vertecies[vertex] = vert;

	return true;
}


bool POF::SHLD_Edit_Face			(int face, vector3d normal, const int fcs[3], const int nbs[3])
{
	if ((unsigned)face > SHLD_Count_Faces())
		return false;
	shields.shield_faces[face].face_normal = normal;

	for (int i = 0; i < 3; i++)
	{
		shields.shield_faces[face].face_vertices[i] = fcs[i] ;
		shields.shield_faces[face].neighbors[i] = nbs[i];
	}
	
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::SHLD_Del_Vertex			(int vertex)
{
	if ((unsigned)vertex > SHLD_Count_Vertices())
		return false;
	shields.vertecies.erase(shields.vertecies.begin() + vertex);
	return true;
}

bool POF::SHLD_Del_Face				(int face)
{
	if ((unsigned)face > SHLD_Count_Faces())
		return false;
	shields.shield_faces.erase(shields.shield_faces.begin() + face);
	return true;
}

//**********************************************************************************************************************
// EYE Functions
void POF::EYE_Add_Eye				(int sobj_num, vector3d offset, vector3d normal)
{
	eye_pos eye_temp;
	eye_temp.normal = normal;
	eye_temp.sobj_number = sobj_num;
	eye_temp.sobj_offset = offset;
	eyes.eye_positions.push_back(eye_temp);
}

//----------------------------------------------

bool POF::EYE_Del_Eye				(int eye)
{
	if (eye >= (int)eyes.eye_positions.size())
		return false;
	eyes.eye_positions.erase(eyes.eye_positions.begin() + eye);
	return true;
}

unsigned int POF::EYE_Count_Eyes			()
{
	return eyes.eye_positions.size();
}

bool POF::EYE_Get_Eye				(int eye, int &sobj_num, vector3d &offset, vector3d &normal)
{
	if ((unsigned)eye > eyes.eye_positions.size())
		return false;

	sobj_num = eyes.eye_positions[eye].sobj_number;
	offset = eyes.eye_positions[eye].sobj_offset;
	normal = eyes.eye_positions[eye].normal;

	return true;
}

bool POF::EYE_Edit_Eye				(int eye, int sobj_num, vector3d offset, vector3d normal)
{
	if ((unsigned)eye > eyes.eye_positions.size())
		return false;

	eyes.eye_positions[eye].normal = normal;
	eyes.eye_positions[eye].sobj_number = sobj_num;
	eyes.eye_positions[eye].sobj_offset = offset;

	return true;
}

//**********************************************************************************************************************
// ACEN Functions

bool POF::ACEN_IsSet					()
{
	vector3d temp;
	temp.x = 0;
	temp.y = 0;
	temp.z = 0;
	return !(ACEN_Get_acen() == temp);
}


void POF::ACEN_Set_acen				(vector3d point)
{
	autocentering.point = point;
}

bool POF::ACEN_Del_acen				()
{
	

	autocentering.point.x = 0;
	autocentering.point.y = 0;
	autocentering.point.z = 0;

	return true;
}

vector3d POF::ACEN_Get_acen			()
{
	return autocentering.point;
}


//**********************************************************************************************************************
// INSG Functions

void POF::INSG_Add_insignia			(int lod, vector3d offset)
{
	//First one of the add functions written with the more efficient coding
	//Talk about ExecutionTime-- and Efficiency++
	insig ni;
	//the New one
	ni.detail_level = lod;
	ni.offset = offset;
	insignia.insignias.push_back(ni);
}

bool POF::INSG_Add_Insig_Vertex		(int insig, vector3d vertex)
{
	// man look at this new code... joygasm :)
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	insignia.insignias[insig].vertex_pos.push_back(vertex);
	return true;
}

bool POF::INSG_Add_Insig_Face		(int insig, const int vert_indecies[], const vector3d u_tex_coord, const vector3d v_tex_coord)
{
	// i cannot get over this efficiency yet
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	
	insg_face nf;
	//new face;

	float a[3], b[3];
	int i;
	//this [the memcpy's] really stems from me rewriting the code from an error in my binary read/write
	// and i didn't want to change what the user of the class sees for function calls
	memcpy((char *) a, (char *) &u_tex_coord, sizeof(vector3d)); // mesa BITCH :)
	memcpy((char *) b, (char *) &v_tex_coord, sizeof(vector3d));	

	for (i = 0; i < 3; i++)
	{
		nf.points[i].vertex_index = vert_indecies[i];
		nf.points[i].u_texture_coordinate = a[i];
		nf.points[i].v_texture_coordinate = b[i];
	}
	insignia.insignias[insig].faces.push_back(nf);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::INSG_Add_Insig_Face		(int insig, insg_face &InsgFace)
// this is how i should have written it in the first place
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	insignia.insignias[insig].faces.push_back(InsgFace);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::INSG_Count_Insignia		()
{ return insignia.insignias.size(); }

unsigned int POF::INSG_Count_Vertecies		(int insig)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return -1;
	return insignia.insignias[insig].vertex_pos.size();
}

unsigned int POF::INSG_Count_Faces			(int insig)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return -1;
	return insignia.insignias[insig].faces.size();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::INSG_Get_Insignia			(int insig, int &lod, vector3d &offset)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	lod = insignia.insignias[insig].detail_level;
	offset = insignia.insignias[insig].offset;

	return true;
}

int  POF::INST_Find_Vert				(int insig, vector3d vertex)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return -1;	

	for (unsigned int i = 0; i < insignia.insignias[insig].vertex_pos.size(); i++)
	{
		if (insignia.insignias[insig].vertex_pos[i] == vertex)
			return i;
	}
	return -1;	
}

bool POF::INSG_Get_Insig_Vertex		(int insig, int vert, vector3d &vertex)
{
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)vert > INSG_Count_Vertecies(insig))
		return false;

	vertex = insignia.insignias[insig].vertex_pos[vert];
	return true;
}


bool POF::INSG_Get_Insig_Face		(int insig, int face, int vert_indecies[], vector3d &u_tex_coord, vector3d &v_tex_coord)
{
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)face > INSG_Count_Faces(insig))
		return false;
	int i;
	float a[3], b[3];

	for (i = 0; i < 3; i++)
	{
		vert_indecies[i] =	insignia.insignias[insig].faces[face].points[i].vertex_index;
		a[i] =				insignia.insignias[insig].faces[face].points[i].u_texture_coordinate ;
		b[i] =				insignia.insignias[insig].faces[face].points[i].v_texture_coordinate;
	}
	//this [the memcpy's] really stems from me rewriting the code from an error in my binary read/write
	// and i didn't want to change what the user of the class sees for function calls

	memcpy((char *) &u_tex_coord, (char *) a, sizeof(vector3d)); // mesa BITCH :)
	memcpy((char *) &v_tex_coord,  (char *) b, sizeof(vector3d));	

	return true;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::INSG_Del_Insignia			(int insg)
{
	if ((unsigned)insg > INSG_Count_Insignia())
		return false;
	insignia.insignias.erase(insignia.insignias.begin() + insg);
	return true;
}

bool POF::INSG_Del_Insig_Vertex		(int insig, int vert)
{
	//rewrote to new way
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)vert > INSG_Count_Vertecies(insig))
		return false;
	insignia.insignias[insig].vertex_pos.erase(insignia.insignias[insig].vertex_pos.begin() + vert);
	return true;
}

bool POF::INSG_Del_Insig_Face		(int insig, int face)
{
	// the first function i wrote after realizing a more efficient way of reallocating the
	//	data, the rest will be recoded later
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)face > INSG_Count_Faces(insig))
		return false;
	insignia.insignias[insig].faces.erase(insignia.insignias[insig].faces.begin() + face);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::INSG_Edit_Insignia			(int insig, int lod, vector3d offset)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	insignia.insignias[insig].detail_level = lod;
	insignia.insignias[insig].offset = offset;

	return true;
}

bool POF::INSG_Edit_Insig_Vertex		(int insig, int vert, vector3d vertex)
{
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)vert > INSG_Count_Vertecies(insig))
		return false;

	insignia.insignias[insig].vertex_pos[vert] = vertex;
	return true;
}


bool POF::INSG_Edit_Insig_Face		(int insig, int face, const int vert_indecies[], const vector3d u_tex_coord, const vector3d v_tex_coord)
{
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)face > INSG_Count_Faces(insig))
		return false;

	float a[3], b[3];
	int i;
	//this [the memcpy's] really stems from me rewriting the code from an error in my binary read/write
	// and i didn't want to change what the user of the class sees for function calls
	memcpy((char *) a, (char *) &u_tex_coord, sizeof(vector3d)); // mesa BITCH :)
	memcpy((char *) b, (char *) &v_tex_coord, sizeof(vector3d));	

	for (i = 0; i < 3; i++)
	{
		insignia.insignias[insig].faces[face].points[i].vertex_index = vert_indecies[i];
		insignia.insignias[insig].faces[face].points[i].u_texture_coordinate = a[i];
		insignia.insignias[insig].faces[face].points[i].v_texture_coordinate = b[i];
	}

	return true;
}

//**********************************************************************************************************************
// PATH Functions

void POF::PATH_Add_Path				(std::string name, std::string parent)
{
	a_path newpath;
	newpath.name = APStoString(name);
	newpath.parent = APStoString(parent);
	paths.paths.push_back(newpath);
}

bool POF::PATH_Add_Vert				(int path, vector3d point, float rad)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;
	path_vert newvert;
	newvert.pos = point;
	newvert.radius = rad;
	paths.paths[path].verts.push_back(newvert);
	return true;
}

bool POF::PATH_Add_Turret			(int path, int vert, int sobj_number)
{
	if (path < 0 || vert < 0 || (unsigned)path > PATH_Count_Paths() || (unsigned)path > PATH_Count_Verts(path))
		return false;
	paths.paths[path].verts[vert].sobj_number.push_back(sobj_number);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::PATH_Del_Path				(int path)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;
	paths.paths.erase(paths.paths.begin() + path);
	return true;
}

bool POF::PATH_Del_Vert				(int path, int vert)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return false;
	paths.paths[path].verts.erase(paths.paths[path].verts.begin() + vert);
	return true;
}

bool POF::PATH_Del_Turret			(int path, int vert, int turret)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path) || (unsigned)turret > PATH_Count_Turrets(path, vert))
		return false;
	paths.paths[path].verts[vert].sobj_number.erase(paths.paths[path].verts[vert].sobj_number.begin() + turret);
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::PATH_Count_Paths			()
{
	return paths.paths.size();
}

unsigned int POF::PATH_Count_Verts			(int path)
{
	if ((unsigned)path > PATH_Count_Paths())
		return -1;
	return paths.paths[path].verts.size();
}

unsigned int POF::PATH_Count_Turrets		(int path, int vert)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return -1;

	return paths.paths[path].verts[vert].sobj_number.size();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::PATH_Get_Path				(int path, std::string &name, std::string &parents)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;

	name = StringToAPS(paths.paths[path].name);
	parents = StringToAPS(paths.paths[path].parent);
	return true;
}

bool POF::PATH_Get_Vert				(int path, int vert, vector3d &point, float &rad)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return false;
	
	point = paths.paths[path].verts[vert].pos;
	rad = paths.paths[path].verts[vert].radius;

	return true;
}

bool POF::PATH_Get_Turret			(int path, int vert, int turret, int &sobj_number)
{
	// i LOVE short circuit evaluation :)
	if (path < 0 || vert < 0 || turret < 0 || 
		(unsigned)path > PATH_Count_Paths() || 
		(unsigned)vert > PATH_Count_Verts(path) || 
		(unsigned)turret > PATH_Count_Turrets(path, vert))
		return false;

	sobj_number = paths.paths[path].verts[vert].sobj_number[turret];
	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::PATH_Edit_Path				(int path, std::string name, std::string parent)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;

	paths.paths[path].name = APStoString(name);
	paths.paths[path].parent = APStoString(parent);
	return true;
}

bool POF::PATH_Edit_Vert				(int path, int vert, vector3d point, float rad)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return false;
	
	paths.paths[path].verts[vert].pos = point;
	paths.paths[path].verts[vert].radius = rad;

	return true;
}

bool POF::PATH_Edit_Turret			(int path, int vert, int turret, int sobj_number)
{
	// i LOVE short circuit evaluation :)
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path) || 
		(unsigned)turret > PATH_Count_Turrets(path, vert))
		return false;

	paths.paths[path].verts[vert].sobj_number[turret] = sobj_number;
	return true;
}

//**********************************************************************************************************************

// GLOW Functions

void POF::GLOW_Add_LightGroup		(int disp_time, int on_time, int off_time, int obj_parent, int LOD, int type, std::string properties)
{
	HullLights n;

	n.disp_time = disp_time;
	n.obj_parent = obj_parent;
	n.off_time = off_time;
	n.on_time = on_time;
	n.LOD = LOD;
	n.type = type;
	n.properties = APStoString(properties);
	hull_lights.lights.push_back(n);
}


bool POF::GLOW_Add_GlowPoint		(int group, float radius, vector3d pos, vector3d norm)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;

	HullLightPoint n;
	n.norm = norm;
	n.radius = radius;
	n.point = pos;
	hull_lights.lights[group].lights.push_back(n);
	return true;
}


unsigned int POF::GLOW_Count_Glows			(int group)
{
	if (group < 0 || (unsigned)group > GLOW_Count_LightGroups())
		return -1;
	return hull_lights.lights[group].lights.size();
}



bool POF::GLOW_Edit_GlowPoint		(int group, int gp, float radius, vector3d pos, vector3d norm)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;
	if (gp < 0 || (unsigned)gp > GLOW_Count_Glows(group))
		return false;

	hull_lights.lights[group].lights[gp].radius = radius;
	hull_lights.lights[group].lights[gp].norm = norm;
	hull_lights.lights[group].lights[gp].point = pos;

	return true;
}

bool POF::GLOW_Edit_Group			(int group, int disp_time, int on_time, int off_time, int obj_parent, int LOD, int type, std::string properties)
{
		if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
			return false;
		hull_lights.lights[group].disp_time = disp_time;
		hull_lights.lights[group].on_time = on_time;
		hull_lights.lights[group].off_time = off_time;
		hull_lights.lights[group].obj_parent = obj_parent;
		hull_lights.lights[group].LOD = LOD;
		hull_lights.lights[group].type = type;
		
		hull_lights.lights[group].properties = APStoString(properties);
	
		return true;
}

bool POF::GLOW_Del_Group			(int group)
{
	if ((unsigned)group > GLOW_Count_LightGroups() || group < 0)
		return false;
	hull_lights.lights.erase(hull_lights.lights.begin() + group);
	return true;
}

bool POF::GLOW_Del_GlowPoint		(int group, int glowpoint)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;
	if (glowpoint < 0 || (unsigned)glowpoint > GLOW_Count_Glows(group))
		return false;
	hull_lights.lights[group].lights.erase(hull_lights.lights[group].lights.begin() + group);
	return true;
}

bool POF::GLOW_Get_GlowPoint		(int group, int gp, float &radius, vector3d &pos, vector3d &norm)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;
	if (gp < 0 || (unsigned)gp > GLOW_Count_Glows(group))
		return false;
	//hull_lights.lights[group].lights[gp]
	radius = hull_lights.lights[group].lights[gp].radius;
	pos	= hull_lights.lights[group].lights[gp].point;
	norm = hull_lights.lights[group].lights[gp].norm;
	return true;
}

bool POF::GLOW_Get_Group		(int group, int &disp_time, int &on_time, int &off_time, int &obj_parent, int &LOD, int &type, std::string &properties)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;
	//hull_lights.lights[group]
	disp_time = hull_lights.lights[group].disp_time;
	on_time = hull_lights.lights[group].on_time;
	off_time = hull_lights.lights[group].off_time;
	obj_parent = hull_lights.lights[group].obj_parent;
	LOD = hull_lights.lights[group].LOD;
	type = hull_lights.lights[group].type;
	properties = StringToAPS(hull_lights.lights[group].properties);
	return true;
}

//**********************************************************************************************************************
// PINF Functions


void POF::PINF_Set					(std::string pof_info)
{
	pofinfo.strings.resize(pof_info.length()+1);
	memcpy(&pofinfo.strings.front(), pof_info.c_str(), pof_info.length());
	pofinfo.strings[pof_info.length()] = '\0';
}

void POF::PINF_Set					(char *str, int sz)
{
	pofinfo.strings.resize(sz+1);
	memcpy(&pofinfo.strings.front(), str, sz);
	pofinfo.strings[sz] = '\0';
}

bool POF::PINF_Del()
{
	pofinfo.strings.clear();
	return true;
}

std::vector<std::string> POF::PINF_Get				()
{
	std::vector<std::string> strings;
	if (pofinfo.strings.empty())
		return strings;

	for (size_t pos = 0; pos < pofinfo.strings.size() && pofinfo.strings[pos] != '\0'; pos += strlen(&pofinfo.strings.front()+pos)+1)
	{
			strings.resize(strings.size()+1);
			strings[strings.size()-1] = (char*)(&pofinfo.strings.front()+pos);
	}

	return strings;
}

//**********************************************************************************************************************

/*
void POFstring::operator=(const POFstring &rhs)
{
	length = rhs.length;
	str = new char[rhs.length];
	strncpy(str, rhs.str, length);
}*/
