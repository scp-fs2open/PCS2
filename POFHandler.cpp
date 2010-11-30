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
#include <cstring>
#include <string>

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



void POF::Destroy()
{
	int j;

	while (DOCK_Count_Docks())
		DOCK_Del_Dock(0);

	while (EYE_Count_Eyes())
		EYE_Del_Eye(0);

	while (FUEL_Count_Thrusters())
		FUEL_Del_Thruster(0);

	while (GPNT_SlotCount())
		GPNT_DelSlot(0);

	while (MPNT_SlotCount())
		MPNT_DelSlot(0);

	delete[] header.cross_sections;
	delete[] header.sobj_debris;
	delete[] header.sobj_detaillevels;
	delete[] header.lights;


	while (INSG_Count_Insignia())
		INSG_Del_Insignia(0);


	j = OBJ2_Count();
	if (objects)
		delete[] objects;

	while (PATH_Count_Paths())	
		PATH_Del_Path(0);		

	PINF_Del();
	
	delete[] shields.shield_faces;
	delete[] shields.vertecies;


	while (SPCL_Count())
		SPCL_DelSpecial(0);
	

	while (TGUN_Count_Banks())
		TGUN_Del_Bank(0);


	while (TMIS_Count_Banks())
		TMIS_Del_Bank(0);


	while (TXTR_Count_Textures())
		TXTR_DelTexture(0);


	while (GLOW_Count_LightGroups())
		GLOW_Del_Group(0);

	SLDC_SetTree(NULL, 0); //empty SLDC tree

}
//**********************************************************************************************************************

void POF::SLDC_SetTree(char* sldc_tree, unsigned int sz)
// setting this to NULL will empty the tree
{
	if (shield_collision.tree_data)
		delete[] shield_collision.tree_data;
	shield_collision.tree_data = NULL;
	shield_collision.tree_size = 0;

	if (sldc_tree != NULL && sz != 0)
	{
		shield_collision.tree_data = new char[sz];
		memcpy(shield_collision.tree_data, sldc_tree, sz);
		shield_collision.tree_size = sz;
	}
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

	char *str = new char[len+1];
	
	memcpy(str, localptr, len);
	localptr += len;

	str[len] = '\0';
	
	retval = str;
	delete[] str;
	return retval;

}

//****************************************************************************************************************

void POF::Parse_Memory_PNT(int mode, char *buffer)
{
	// this is going to be FUN
	char *localptr = buffer;
	GPNT_MPNT *pnt = PNT_Alias(mode);

	memcpy(&pnt->num_slots, buffer, 4);
	localptr += 4;

	pnt->slots = new slot[pnt->num_slots];
	

	for (unsigned int i = 0; i < pnt->num_slots; i++)
	{
		memcpy(&pnt->slots[i].num_guns, localptr, 4);

		localptr += 4; //pointer arithmetic
		
		pnt->slots[i].guns = new gun[pnt->slots[i].num_guns];

		for (unsigned int j = 0; j < pnt->slots[i].num_guns; j++)
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

	memcpy(&pnt->num_banks, localptr, 4);
	localptr += 4;

	pnt->banks = new Turret_bank[pnt->num_banks];

	for (unsigned int i = 0; i < pnt->num_banks; i++)
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
		memcpy(&pnt->banks[i].num_firing_points, localptr, 4);
		localptr += 4; //pointer arithmetic

		pnt->banks[i].position = new vector3d[pnt->banks[i].num_firing_points];

		for (unsigned int j = 0; j < pnt->banks[i].num_firing_points; j++)
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

	memcpy(&temp.bsp_data_size, localptr, sizeof(int));
	localptr += sizeof(int);

	temp.bsp_data.reset(new char[temp.bsp_data_size]);

	memcpy(temp.bsp_data.get(), localptr, temp.bsp_data_size);
	
	//done!
	temp.reserved = 0;
	// assign the actual internal apvector of them
	//int Cur = numobj2;

	OBJ2* tempobj = new OBJ2[numobj2+1];
	for (int i = 0; i < numobj2; i++)
	{
		tempobj[i] = objects[i];
	}
	//memcpy(tempobj, objects, sizeof(OBJ2)*numobj2);
	tempobj[numobj2] = temp;

	delete[] objects;
	objects = tempobj;
	numobj2++;

	//objects.resize(Cur + 1);
	//objects[Cur] = temp;
}

//-----------------------------------------------------------------

void POF::Parse_Memory_DOCK(char *buffer)
{
	char *localptr = buffer;

	memcpy(&docking.num_docks, localptr, sizeof(int));
	localptr += sizeof(int);

	docking.points = new dock_point[docking.num_docks];

	for (unsigned int i = 0; i < docking.num_docks; i++)
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

		memcpy(&docking.points[i].num_spline_paths, localptr, sizeof(int));
		localptr += sizeof(int);

		docking.points[i].path_number = new int[docking.points[i].num_spline_paths];

		for (unsigned int j = 0; j < docking.points[i].num_spline_paths; j++)
		{
			memcpy(&docking.points[i].path_number[j], localptr, sizeof(int));
			localptr += sizeof(int);
		}

		memcpy(&docking.points[i].num_points, localptr, sizeof(int));
		localptr += sizeof(int);

		docking.points[i].points = new gun[docking.points[i].num_points];

		for (unsigned int k = 0; k < docking.points[i].num_points; k++)
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

	memcpy(&hull_lights.num_glows_arrays, localptr, sizeof(int));
	localptr += sizeof(int);

	hull_lights.lights = new HullLights[hull_lights.num_glows_arrays];

	for (unsigned int i = 0; i < hull_lights.num_glows_arrays; i++)
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
		
		memcpy(&hull_lights.lights[i].num_Lights, localptr, sizeof(int));
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

		hull_lights.lights[i].lights = new HullLightPoint[hull_lights.lights[i].num_Lights];

		for (unsigned int j = 0; j < hull_lights.lights[i].num_Lights; j++)
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

	memcpy(&thrusters.num_thrusters, localptr, sizeof(int));
	localptr += sizeof(int);

	thrusters.thrusters = new thruster[thrusters.num_thrusters];

	for (unsigned int i = 0; i < thrusters.num_thrusters; i++)
	{
		memcpy(&thrusters.thrusters[i].num_glows, localptr, sizeof(int));
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

		thrusters.thrusters[i].points = new glow_point[thrusters.thrusters[i].num_glows];

		for (unsigned int j = 0; j < thrusters.thrusters[i].num_glows; j++)
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

	memcpy(&shields.num_vertices, localptr, sizeof(int));
	localptr += sizeof(int);

	shields.vertecies = new vector3d[shields.num_vertices];

	for (unsigned int i = 0; i < shields.num_vertices; i++)
	{
		memcpy(&shields.vertecies[i], localptr, sizeof(vector3d));
		localptr += sizeof(vector3d);
	}

	memcpy(&shields.num_faces, localptr, sizeof(int));
	localptr += sizeof(int);

	shields.shield_faces = new shield_face[shields.num_faces];

	for (unsigned int j = 0; j < shields.num_faces; j++)
	{
		memcpy(&shields.shield_faces[j], localptr, sizeof(shield_face));
		localptr += sizeof(shield_face);
	}


}

//-----------------------------------------------------------------

void POF::Parse_Memory_EYE(char *buffer)
{
	char *localptr = buffer;

	memcpy(&eyes.num_eye_positions, localptr, sizeof(int));
	localptr += sizeof(int);
	
	eyes.eye_positions = new eye_pos[eyes.num_eye_positions];
	
	for (unsigned int i = 0; i < eyes.num_eye_positions; i++)
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

	memcpy(&insignia.num_insignias, localptr, sizeof(int));
	localptr += sizeof(int);

	insignia.insignias = new insig[insignia.num_insignias];

	for (unsigned int i = 0; i < insignia.num_insignias; i++)
	{
		memcpy(&insignia.insignias[i].detail_level, localptr, sizeof(int));
		localptr += sizeof(int);

		memcpy(&insignia.insignias[i].num_faces, localptr, sizeof(int));
		localptr += sizeof(int);
		
		memcpy(&insignia.insignias[i].num_verticies, localptr, sizeof(int));
		localptr += sizeof(int);

		insignia.insignias[i].vertex_pos = new vector3d[insignia.insignias[i].num_verticies];
		insignia.insignias[i].faces = new insg_face[insignia.insignias[i].num_faces];

		for (unsigned int j = 0; j < insignia.insignias[i].num_verticies; j++)
		{
			memcpy(&insignia.insignias[i].vertex_pos[j], localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);
		}

		memcpy(&insignia.insignias[i].offset, localptr, sizeof(vector3d));
		localptr += sizeof(vector3d);

		for (unsigned int k = 0; k < insignia.insignias[i].num_faces; k++)
		{
			memcpy(&insignia.insignias[i].faces[k], localptr, sizeof(insg_face));
			localptr += sizeof(insg_face);
		}
	

	}

}


void POF::Parse_Memory_PATH(char *buffer)
{
	char *localptr = buffer;

	memcpy(&paths.num_paths, localptr, sizeof(int));
	localptr += sizeof(int);

	paths.paths = new a_path[paths.num_paths];

	for (unsigned int i = 0; i < paths.num_paths; i++)
	{
		/*memcpy(&paths.paths[i].name.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (paths.paths[i].name.length)
			paths.paths[i].name.str = new char[paths.paths[i].name.length];
		else
			paths.paths[i].name.str = NULL;
			
		memcpy(paths.paths[i].name.str, localptr, paths.paths[i].name.length);
		localptr += paths.paths[i].name.length;*/
		paths.paths[i].name = Parse_BPOFstring(localptr);

		/*memcpy(&paths.paths[i].parent.length, localptr, sizeof(int));
		localptr += sizeof(int);

		if (paths.paths[i].parent.length)
			paths.paths[i].parent.str = new char[paths.paths[i].parent.length];
		else
			paths.paths[i].parent.str = NULL;

		memcpy(paths.paths[i].parent.str, localptr, paths.paths[i].parent.length);
		localptr += paths.paths[i].parent.length;*/
		paths.paths[i].parent = Parse_BPOFstring(localptr);

		memcpy(&paths.paths[i].num_verts, localptr, sizeof(int));
		localptr += sizeof(int);

		paths.paths[i].verts = new path_vert[paths.paths[i].num_verts];

		for (unsigned int j = 0; j < paths.paths[i].num_verts; j++)
		{
			memcpy(&paths.paths[i].verts[j].pos, localptr, sizeof(vector3d));
			localptr += sizeof(vector3d);

			memcpy(&paths.paths[i].verts[j].radius, localptr, sizeof(float));
			localptr += sizeof(float);

			memcpy(&paths.paths[i].verts[j].num_turrets, localptr, sizeof(int));
			localptr += sizeof(int);
			
			paths.paths[i].verts[j].sobj_number = new int[paths.paths[i].verts[j].num_turrets];

			for (unsigned int k = 0; k < paths.paths[i].verts[j].num_turrets; k++)
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

	memcpy(&shield_collision.tree_size, localptr, sizeof(unsigned int));
	localptr += sizeof(unsigned int);

	shield_collision.tree_data = new char[shield_collision.tree_size];
	memcpy(shield_collision.tree_data, localptr, shield_collision.tree_size);
}

void POF::Parse_Memory_PINF (char *buffer, int size)
{
	pofinfo.strings = new char[size];
	memcpy(pofinfo.strings, buffer, size);

}

void POF::Parse_Memory_SPCL(char *buffer)
{
	//specials
	char *localptr = buffer;

	memcpy(&specials.num_special_points, localptr, sizeof(int));
	localptr += sizeof(int);

	specials.special_points = new special_point[specials.num_special_points];

	for (unsigned int i = 0; i < specials.num_special_points; i++)
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
	unsigned int i, first = sizeof(float) + (3 * sizeof(int)) + (2 * sizeof(vector3d));
	char *localptr = buffer;
	//header
	//i get to cheat on the first 6 vars because they are all non-dyamic and i can pull chiggy stuff :)
	memcpy(&header, localptr, first);
	localptr += first;

	header.sobj_detaillevels = new int[header.num_detaillevels];

	for (i = 0; i < header.num_detaillevels; i++)
	{
		memcpy(&header.sobj_detaillevels[i], localptr, sizeof(int));
		localptr += sizeof(int);
	}
	
	memcpy(&header.num_debris, localptr, sizeof(int));
	localptr += sizeof(int);

	header.sobj_debris = new int[header.num_debris];

	for (i = 0; i < header.num_debris; i++)
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

	memcpy(&header.num_cross_sections, localptr, sizeof(int));
	localptr += sizeof(int);

	// dunno how this happened.. but i've seen it on atleast one model (created by PCS1 :( ) -- kaz
	if (header.num_cross_sections == -1)
		header.num_cross_sections = 0;

	if (header.num_cross_sections > 0)
	{
		header.cross_sections = new cross_section[header.num_cross_sections];

		for (i = 0; i < (unsigned int)header.num_cross_sections; i++)
		{
			memcpy(&header.cross_sections[i], localptr, sizeof(cross_section));
			localptr += sizeof(cross_section);
		}
	}else{
		header.cross_sections = NULL;
		header.num_cross_sections = 0;
	}

	memcpy(&header.num_lights, localptr, sizeof(int));
	localptr += sizeof(int);

	header.lights = new muzzle_light[header.num_lights];

	for (i = 0; i < header.num_lights; i++)
	{
		memcpy(&header.lights[i], localptr, sizeof(muzzle_light));
		localptr += sizeof(muzzle_light);
	}
}


void POF::Parse_Memory_TXTR(char *buffer)
{
	//textures
	char *localptr = buffer;

	memcpy(&textures.num_textures, localptr, sizeof(int));
	localptr += sizeof(int);

	textures.tex_filename = new std::string[textures.num_textures];

	for (unsigned int i = 0; i < textures.num_textures; i++)
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
	char main_buffer[5], secondary_buffer[5], *dynamic_buffer;
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

		dynamic_buffer = new char[len];
		infile.read(dynamic_buffer, len);

		if (!_strnicmp(main_buffer, "TXTR", 4))
			Parse_Memory_TXTR(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "HDR2", 4))
			Parse_Memory_HDR2(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "OBJ2", 4))
			Parse_Memory_OBJ2(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "SPCL", 4))
			Parse_Memory_SPCL(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "GPNT", 4))
			Parse_Memory_GPNT(dynamic_buffer);
		
		else if (!_strnicmp(main_buffer, "MPNT", 4))
			Parse_Memory_MPNT(dynamic_buffer);
		
		else if (!_strnicmp(main_buffer, "TGUN", 4))
			Parse_Memory_TGUN(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "TMIS", 4))
			Parse_Memory_TMIS(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "DOCK", 4))
			Parse_Memory_DOCK(dynamic_buffer);
		
		else if (!_strnicmp(main_buffer, "FUEL", 4))
			Parse_Memory_FUEL(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "SHLD", 4))
			Parse_Memory_SHLD(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "EYE ", 4))
			Parse_Memory_EYE(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "ACEN", 4))
			Parse_Memory_ACEN(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "INSG", 4))
			Parse_Memory_INSG(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "PATH", 4))
			Parse_Memory_PATH(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "GLOW", 4))
			Parse_Memory_GLOW(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "SLDC", 4))
			Parse_Memory_SLDC(dynamic_buffer);

		else if (!_strnicmp(main_buffer, "PINF ", 4))
			Parse_Memory_PINF (dynamic_buffer, len);

		// if it's not recognized it's simply skipped

		// clean up that memory
		delete[] dynamic_buffer;
	}

	// new version
	version = 2117;

	return 0;
}// must be binary mode


bool POF::SavePOF(std::ofstream &outfile) // must be binary mode
{
	char temp_buf[4], vector_buf[12], *large_buf;
	unsigned int size, i, j, k, itemp;
	OBJ2 *local_sobj;
	
	// write signature
	outfile.write("PSPO", 4);

	// and version [should be 2117]
	memcpy(temp_buf, &version, 4);
	outfile.write(temp_buf, 4);

	//1  TXTR ----------------------------------------
	if (textures.num_textures > 0)
	{
		outfile.write("TXTR", 4);
		
		//calculate size
		size = 4 + (textures.num_textures * 4);
		for (i = 0; i < textures.num_textures; i++)
		{
			size += textures.tex_filename[i].length();
		}

		memcpy(temp_buf, &size, sizeof(int));
		outfile.write(temp_buf, sizeof(int));

		// write num_textures
		memcpy(temp_buf, &textures.num_textures, sizeof(int));
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < textures.num_textures; i++)
		{
			// write out the length of the string
			itemp = textures.tex_filename[i].length();
			memcpy(temp_buf, &itemp, sizeof(int));
			outfile.write(temp_buf, sizeof(int));

			// write the string
			outfile.write(textures.tex_filename[i].c_str(), textures.tex_filename[i].length());
		}

	}

	//2  HDR2 ----------------------------------------
	// struct sig
	outfile.write("HDR2", 4);

	//struct size
	size = 104; // static size;
	size += (header.num_detaillevels * 4);
	size += (header.num_debris * 4);
	if (header.num_cross_sections >= 0)
		size += (header.num_cross_sections * sizeof(cross_section));
	size += (header.num_lights * sizeof(muzzle_light));
	
	memcpy(temp_buf, &size, 4);
	outfile.write(temp_buf, 4);

	// output data
	memcpy(temp_buf, &header.max_radius, sizeof(float)); 
	outfile.write(temp_buf, sizeof(float));

	memcpy(temp_buf, &header.obj_flags, sizeof(int)); 
	outfile.write(temp_buf, sizeof(int));

	memcpy(temp_buf, &header.num_subobjects, sizeof(float)); 
	outfile.write(temp_buf, sizeof(float));

	memcpy(vector_buf, &header.min_bounding, sizeof(vector3d)); 
	outfile.write(vector_buf, sizeof(vector3d));

	memcpy(vector_buf, &header.max_bounding, sizeof(vector3d)); 
	outfile.write(vector_buf, sizeof(vector3d));

	//+-+-+-+-+-+-+-+-



	memcpy(temp_buf, &header.num_detaillevels, sizeof(int)); 
	outfile.write(temp_buf, sizeof(int));

	// crap the sobj_detaillevels array out
	for (i = 0; i < header.num_detaillevels; i++)
	{
		memcpy(temp_buf, &header.sobj_detaillevels[i], sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));
	}

	//+-+-+-+-+-+-+-+-

	
	memcpy(temp_buf, &header.num_debris, sizeof(int)); 
	outfile.write(temp_buf, sizeof(int));

	// crap the sobj_debris array out
	for (i = 0; i < header.num_debris; i++)
	{
		memcpy(temp_buf, &header.sobj_debris[i], sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));
	}

	//+-+-+-+-+-+-+-+-


	memcpy(temp_buf, &header.mass, sizeof(float)); 
	outfile.write(temp_buf, sizeof(int));

	memcpy(vector_buf, &header.mass_center, sizeof(vector3d)); 
	outfile.write(vector_buf, sizeof(vector3d));

	large_buf = new char[36]; // size of float[3][3]


	memcpy(large_buf, &header.moment_inertia, 36); 
	outfile.write(large_buf, 36);

	delete[] large_buf;

	//+-+-+-+-+-+-+-+-

	memcpy(temp_buf, &header.num_cross_sections, sizeof(int)); 
	outfile.write(temp_buf, sizeof(int));

	large_buf = new char[sizeof(cross_section)];

	// crap the sobj_debris array out
	for (i = 0; (int)i < header.num_cross_sections; i++)
	{
		memcpy(large_buf, &header.cross_sections[i], sizeof(cross_section)); 
		outfile.write(large_buf, sizeof(cross_section));
	}

	delete[] large_buf;

	//+-+-+-+-+-+-+-+-

	
	memcpy(temp_buf, &header.num_lights, sizeof(int)); 
	outfile.write(temp_buf, sizeof(int));

	large_buf = new char[sizeof(muzzle_light)];

	// crap the sobj_debris array out
	for (i = 0; i < header.num_lights; i++)
	{
		memcpy(large_buf, &header.lights[i], sizeof(muzzle_light)); 
		outfile.write(large_buf, sizeof(muzzle_light));
	}

	delete[] large_buf;

	//3  OBJ2 ----------------------------------------
	for (i = 0; i < OBJ2_Count(); i++)
	{
		local_sobj = &objects[i];
		// struct sig
		outfile.write("OBJ2", 4);
		
		//struct size
		size = 84 + local_sobj->submodel_name.length() + local_sobj->properties.length() + local_sobj->bsp_data_size;
		
		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);

		memcpy(temp_buf, &local_sobj->submodel_number, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &local_sobj->radius, sizeof(float)); 
		outfile.write(temp_buf, sizeof(float));

		memcpy(temp_buf, &local_sobj->submodel_parent, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(vector_buf, &local_sobj->offset, sizeof(vector3d)); 
		outfile.write(vector_buf, sizeof(vector3d));

		memcpy(vector_buf, &local_sobj->geometric_center, sizeof(vector3d)); 
		outfile.write(vector_buf, sizeof(vector3d));

		memcpy(vector_buf, &local_sobj->bounding_box_min_point, sizeof(vector3d)); 
		outfile.write(vector_buf, sizeof(vector3d));

		memcpy(vector_buf, &local_sobj->bounding_box_max_point, sizeof(vector3d)); 
		outfile.write(vector_buf, sizeof(vector3d));

		itemp = local_sobj->submodel_name.length();
		memcpy(temp_buf, &itemp, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		outfile.write(local_sobj->submodel_name.c_str(), local_sobj->submodel_name.length());

		itemp = local_sobj->properties.length();
		memcpy(temp_buf, &itemp, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		outfile.write(local_sobj->properties.c_str(), local_sobj->properties.length());

		memcpy(temp_buf, &local_sobj->movement_type, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &local_sobj->movement_axis, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &local_sobj->reserved, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &local_sobj->bsp_data_size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		outfile.write(local_sobj->bsp_data.get(), local_sobj->bsp_data_size);
	}

	//4  SPCL ----------------------------------------
	if (SPCL_Count() > 0)
	{
		// struct sig
		outfile.write("SPCL", 4);
		
		//struct size
		size = 4;
		for (i = 0; i < SPCL_Count(); i++)
		{
			size += 24 + specials.special_points[i].name.length() + specials.special_points[i].properties.length();
		}
		
		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);
		
		memcpy(temp_buf, &specials.num_special_points, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));
	
		for (i = 0; i < SPCL_Count(); i++)
		{
			itemp = specials.special_points[i].name.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));	
			outfile.write(specials.special_points[i].name.c_str(), specials.special_points[i].name.length());	

			itemp = specials.special_points[i].properties.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));	
			outfile.write(specials.special_points[i].properties.c_str(), specials.special_points[i].properties.length());	

			memcpy(vector_buf, &specials.special_points[i].point, sizeof(vector3d)); 
			outfile.write(vector_buf, sizeof(vector3d));
			
			memcpy(temp_buf, &specials.special_points[i].radius, sizeof(float)); 
			outfile.write(temp_buf, sizeof(float));

		}

	}

	//5  GPNT ----------------------------------------
	if (GPNT_SlotCount() > 0)
	{


		// struct sig
		outfile.write("GPNT", 4);
		
		//struct size
		size = 4 + (4 * GPNT_SlotCount());


		for (i = 0; i < GPNT_SlotCount(); i++)
		{
			size += PNT_Alias(0)->slots[i].num_guns * sizeof(gun);
		}

		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);

		memcpy(temp_buf, &PNT_Alias(0)->num_slots, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < GPNT_SlotCount(); i++)
		{
			memcpy(temp_buf, &PNT_Alias(0)->slots[i].num_guns, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < PNT_Alias(0)->slots[i].num_guns; j++)
			{
				memcpy(vector_buf, &PNT_Alias(0)->slots[i].guns[j].point, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));

				memcpy(vector_buf, &PNT_Alias(0)->slots[i].guns[j].norm, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));

			}

		}
	}

	//6  MPNT ----------------------------------------
	if (MPNT_SlotCount() > 0)
	{


		// struct sig
		outfile.write("MPNT", 4);
		
		//struct size
		size = 4 + (4 * MPNT_SlotCount());


		for (i = 0; i < MPNT_SlotCount(); i++)
		{
			size += PNT_Alias(1)->slots[i].num_guns * sizeof(gun);
		}
		
		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);

		memcpy(temp_buf, &PNT_Alias(1)->num_slots, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));		

		for (i = 0; i < MPNT_SlotCount(); i++)
		{
			memcpy(temp_buf, &PNT_Alias(1)->slots[i].num_guns, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < PNT_Alias(1)->slots[i].num_guns; j++)
			{
				memcpy(vector_buf, &PNT_Alias(1)->slots[i].guns[j].point, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));

				memcpy(vector_buf, &PNT_Alias(1)->slots[i].guns[j].norm, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));

			}

		}
	}

	//7  TGUN ----------------------------------------
	if (T_Alias(0)->num_banks > 0)
	{
		// struct sig
		outfile.write("TGUN", 4);
		
		//struct size
		size = 4 + (24 * T_Alias(0)->num_banks);


		for (i = 0; i < T_Alias(0)->num_banks; i++)
		{
			size += (T_Alias(0)->banks[i].num_firing_points * 12);
		}
		
		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);

		memcpy(temp_buf, &T_Alias(0)->num_banks, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < T_Alias(0)->num_banks; i++)
		{

			memcpy(temp_buf, &T_Alias(0)->banks[i].sobj_parent, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			memcpy(temp_buf, &T_Alias(0)->banks[i].sobj_par_phys, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			memcpy(vector_buf, &T_Alias(0)->banks[i].turret_normal, sizeof(vector3d)); 
			outfile.write(vector_buf, sizeof(vector3d));

			memcpy(temp_buf, &T_Alias(0)->banks[i].num_firing_points, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < T_Alias(0)->banks[i].num_firing_points; j++)
			{
					memcpy(vector_buf, &T_Alias(0)->banks[i].position[j], sizeof(vector3d)); 
					outfile.write(vector_buf, sizeof(vector3d));
			}
		}

	}

	//8  TMIS ----------------------------------------
	if (T_Alias(1)->num_banks > 0)
	{
		// struct sig
		outfile.write("TMIS", 4);
		
		//struct size
		size = 4 + (24 * T_Alias(1)->num_banks);


		for (i = 0; i < T_Alias(1)->num_banks; i++)
		{
			size += (T_Alias(1)->banks[i].num_firing_points * 12);
		}
		
		memcpy(temp_buf, &size, 4);
		outfile.write(temp_buf, 4);

		memcpy(temp_buf, &T_Alias(1)->num_banks, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < T_Alias(1)->num_banks; i++)
		{

			memcpy(temp_buf, &T_Alias(1)->banks[i].sobj_parent, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			memcpy(temp_buf, &T_Alias(1)->banks[i].sobj_par_phys, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			memcpy(vector_buf, &T_Alias(1)->banks[i].turret_normal, sizeof(vector3d)); 
			outfile.write(vector_buf, sizeof(vector3d));

			memcpy(temp_buf, &T_Alias(1)->banks[i].num_firing_points, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < T_Alias(1)->banks[i].num_firing_points; j++)
			{
					memcpy(vector_buf, &T_Alias(1)->banks[i].position[j], sizeof(vector3d)); 
					outfile.write(vector_buf, sizeof(vector3d));
			}
		}

	}

	//9  DOCK ----------------------------------------
	if (docking.num_docks > 0)
	{
		outfile.write("DOCK", 4);

		size = 4;

		for (i = 0; i < docking.num_docks; i++)
		{
			size += 4;
			size += docking.points[i].properties.length();
			size += 4;
			size += (4 * docking.points[i].num_spline_paths);
			size += 4;
			size += (sizeof(gun) * docking.points[i].num_points);
		}


		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &docking.num_docks, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < docking.num_docks; i++)
		{
			itemp = docking.points[i].properties.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
			outfile.write(docking.points[i].properties.c_str(), docking.points[i].properties.length());

			memcpy(temp_buf, &docking.points[i].num_spline_paths, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < docking.points[i].num_spline_paths; j++)
			{
				memcpy(temp_buf, &docking.points[i].path_number[j], sizeof(int)); 
				outfile.write(temp_buf, sizeof(int));
			}

			memcpy(temp_buf, &docking.points[i].num_points, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (k = 0; k < docking.points[i].num_points; k++)
			{
				memcpy(vector_buf, &docking.points[i].points[k].point, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));

				memcpy(vector_buf, &docking.points[i].points[k].norm, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));
			}


		}
	}

	//10 FUEL ----------------------------------------
	if (FUEL_Count_Thrusters() > 0)
	{
		large_buf = new char[sizeof(glow_point)];

		outfile.write("FUEL", 4);

		size = 4 + (8 * FUEL_Count_Thrusters());

		for (i = 0; i < FUEL_Count_Thrusters(); i++)
		{
			size += (thrusters.thrusters[i].properties.length() + (sizeof(glow_point) * thrusters.thrusters[i].num_glows));
		}

		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &thrusters.num_thrusters, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < FUEL_Count_Thrusters(); i++)
		{
			memcpy(temp_buf, &thrusters.thrusters[i].num_glows, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			itemp = thrusters.thrusters[i].properties.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
			outfile.write(thrusters.thrusters[i].properties.c_str(), thrusters.thrusters[i].properties.length());

			for (j = 0; j < FUEL_Count_Glows(i); j++)
			{
				memcpy(large_buf, &thrusters.thrusters[i].points[j], sizeof(glow_point)); 
				outfile.write(large_buf, sizeof(glow_point));

			}

			

		}

		delete[] large_buf;
	}

	//11 SHLD ----------------------------------------
	if (shields.num_faces > 0 || shields.num_vertices > 0)
	{
		large_buf = new char[sizeof(shield_face)];

		outfile.write("SHLD", 4);

		size = 8 + (sizeof(vector3d) * shields.num_vertices) + (sizeof(shield_face) * shields.num_faces);

		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &shields.num_vertices, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < shields.num_vertices; i++)
		{
			memcpy(vector_buf, &shields.vertecies[i], sizeof(vector3d)); 
			outfile.write(vector_buf, sizeof(vector3d));
		}

		memcpy(temp_buf, &shields.num_faces, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));


		for (i = 0; i < shields.num_faces; i++)
		{
			memcpy(large_buf, &shields.shield_faces[i], sizeof(shield_face)); 
			outfile.write(large_buf, sizeof(shield_face));
		}

		delete[] large_buf;
	}

	//12  EYE ----------------------------------------
	if (eyes.num_eye_positions > 0)
	{
		large_buf = new char[sizeof(eye_pos)];
		outfile.write("EYE ", 4);

		size = 4 + (sizeof(eye_pos) * eyes.num_eye_positions);

		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &eyes.num_eye_positions, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < eyes.num_eye_positions; i++)
		{
			memcpy(large_buf, &eyes.eye_positions[i], sizeof(eye_pos)); 
			outfile.write(large_buf, sizeof(eye_pos));
		}

		delete[] large_buf;
	}
	//13 ACEN ----------------------------------------
	if (ACEN_IsSet())
	{
		outfile.write("ACEN", 4);
		k = 12;
		
		memcpy(temp_buf, &k, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(vector_buf, &autocentering.point, sizeof(vector3d)); 
		outfile.write(vector_buf, sizeof(vector3d));

	}

	//14 INSG ----------------------------------------
	if (insignia.num_insignias > 0)
	{

		large_buf = new char[sizeof(insg_face)];

		outfile.write("INSG", 4);

		size = 4; // num_insignias

		for (i = 0; i < insignia.num_insignias; i++)
		{
			size += 24; // detail_level 4|4, num_faces 4|8, num_vertices 4|12, offset 12|24
			size += (sizeof(vector3d) * insignia.insignias[i].num_verticies);
			size += (sizeof(insg_face) * insignia.insignias[i].num_faces);
		}

		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		memcpy(temp_buf, &insignia.num_insignias, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		for (i = 0; i < insignia.num_insignias; i++)
		{
			memcpy(temp_buf, &insignia.insignias[i].detail_level, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			memcpy(temp_buf, &insignia.insignias[i].num_faces, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
	
			memcpy(temp_buf, &insignia.insignias[i].num_verticies, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
			
			for (j = 0; j < insignia.insignias[i].num_verticies; j++)
			{
				memcpy(vector_buf, &insignia.insignias[i].vertex_pos[j], sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));
			}

			memcpy(vector_buf, &insignia.insignias[i].offset, sizeof(vector3d)); 
			outfile.write(vector_buf, sizeof(vector3d));

			for (j = 0; j < insignia.insignias[i].num_faces; j++)
			{
				memcpy(large_buf, &insignia.insignias[i].faces[j], sizeof(insg_face));
				outfile.write(large_buf, sizeof(insg_face));
			}
		}
		delete[] large_buf;
	}

	//15 PATH ----------------------------------------
	if (paths.num_paths > 0)
	{

		outfile.write("PATH", 4);

		size = 4;

		for (i = 0; i < paths.num_paths; i++)
		{
			size += (paths.paths[i].name.length() + paths.paths[i].parent.length());
			size += 8; //string size indexcies
			size += 4; //num_verts;

			for (j = 0; j < paths.paths[i].num_verts; j++)
			{
				//vector pos
				size += 12;
				//float radius
				size += 4;
				//int num_turrets
				size += 4;
				// the sobj_number array
				size += (4 * paths.paths[i].verts[j].num_turrets);
			}


		}


		memcpy(temp_buf, &size, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));
		outfile.flush();

		memcpy(temp_buf, &paths.num_paths, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));
		outfile.flush();

		for (i = 0; i < paths.num_paths; i++)
		{
			itemp = paths.paths[i].name.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			outfile.write(paths.paths[i].name.c_str(), paths.paths[i].name.length());
			outfile.flush();

			itemp = paths.paths[i].parent.length();
			memcpy(temp_buf, &itemp, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			outfile.write(paths.paths[i].parent.c_str(), paths.paths[i].parent.length());
			outfile.flush();

			memcpy(temp_buf, &paths.paths[i].num_verts, sizeof(int)); 
			outfile.write(temp_buf, sizeof(int));

			for (j = 0; j < paths.paths[i].num_verts; j++)
			{
				memcpy(vector_buf, &paths.paths[i].verts[j].pos, sizeof(vector3d)); 
				outfile.write(vector_buf, sizeof(vector3d));		

				memcpy(temp_buf, &paths.paths[i].verts[j].radius, sizeof(float)); 
				outfile.write(temp_buf, sizeof(float));		
			
				memcpy(temp_buf, &paths.paths[i].verts[j].num_turrets, sizeof(int)); 
				outfile.write(temp_buf, sizeof(int));		

				for (k = 0; k < paths.paths[i].verts[j].num_turrets; k++)
				{
					memcpy(temp_buf, &paths.paths[i].verts[j].sobj_number[k], sizeof(int)); 
					outfile.write(temp_buf, sizeof(int));	
				}
			
			}


		}

	}
	// 16 GLOW ----------------------------------------
	if (hull_lights.num_glows_arrays != 0)
	{

		//write signature
		outfile.write("GLOW", 4);
		k = 4 + (8 * sizeof(int)) * hull_lights.num_glows_arrays; // each group in binary format is 7 ints + strlen + size of their point array
																 // almost forgot to add 1 int for num_glow_arrays
		for ( i  = 0; i < hull_lights.num_glows_arrays; i++)
		{
			k += hull_lights.lights[i].properties.length();
			k += sizeof(HullLightPoint) * hull_lights.lights[i].num_Lights;
		}

		// write size
		memcpy(temp_buf, &k, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		// write data
		
		memcpy(temp_buf, &hull_lights.num_glows_arrays, sizeof(int));
		outfile.write(temp_buf, sizeof(int));
		outfile.flush();


		for (unsigned int i = 0; i < hull_lights.num_glows_arrays; i++)
		{
			memcpy(temp_buf, &hull_lights.lights[i].disp_time, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			memcpy(temp_buf, &hull_lights.lights[i].on_time, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			memcpy(temp_buf, &hull_lights.lights[i].off_time, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			memcpy(temp_buf, &hull_lights.lights[i].obj_parent, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			memcpy(temp_buf, &hull_lights.lights[i].LOD, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			memcpy(temp_buf, &hull_lights.lights[i].type, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();
			
			memcpy(temp_buf, &hull_lights.lights[i].num_Lights, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();

			itemp = hull_lights.lights[i].properties.length();
			memcpy(temp_buf, &itemp, sizeof(int));
			outfile.write(temp_buf, sizeof(int));
			outfile.flush();


			outfile.write(hull_lights.lights[i].properties.c_str(), hull_lights.lights[i].properties.length());
			outfile.flush();


			for (unsigned int j = 0; j < hull_lights.lights[i].num_Lights; j++)
			{
				memcpy(vector_buf, &hull_lights.lights[i].lights[j].point, sizeof(vector3d));
				outfile.write(vector_buf, sizeof(vector3d));
				outfile.flush();

				memcpy(vector_buf, &hull_lights.lights[i].lights[j].norm, sizeof(vector3d));
				outfile.write(vector_buf, sizeof(vector3d));
				outfile.flush();

				memcpy(temp_buf, &hull_lights.lights[i].lights[j].radius, sizeof(float));
				outfile.write(temp_buf, sizeof(float));
				outfile.flush();
			
			}
		}
	}

	//17 SLDC ----------------------------------------
	if (shield_collision.tree_size != 0)
	{
		outfile.write("SLDC", 4);
		k = sizeof(int) + shield_collision.tree_size;

		memcpy(temp_buf, &k, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		outfile.write((char*)&shield_collision.tree_size, sizeof(int));
		outfile.write(shield_collision.tree_data, shield_collision.tree_size);
	}

	//18 PINF ----------------------------------------
	if (pofinfo.strings != NULL || strlen(pofinfo.strings) > 0)
	{
		//always terminated with a double null
		outfile.write("PINF", 4);
		k = strlen(pofinfo.strings) + 2; //for the double null

		memcpy(temp_buf, &k, sizeof(int)); 
		outfile.write(temp_buf, sizeof(int));

		outfile.write(pofinfo.strings, (k - 2));

		// double null
		k = 0;
		memcpy(temp_buf, &k, sizeof(int)); 
		outfile.write(temp_buf, 2);

	}
	
	return true;
}


void POF::ClearAllData()
{
	version = 2117;
	numobj2 = 0;
	objects = NULL;

	memset(&textures, 0, sizeof(TXTR));
	memset(&header, 0, sizeof(HDR2));
	memset(&specials, 0, sizeof(SPCL));
	memset(&gunpoints, 0, sizeof(GPNT_MPNT));
	memset(&missilepoints, 0, sizeof(GPNT_MPNT));
	memset(&turretguns, 0, sizeof(TGUN_TMIS));
	memset(&turretmissiles, 0, sizeof(TGUN_TMIS));
	memset(&docking, 0, sizeof(DOCK));
	memset(&thrusters, 0, sizeof(FUEL));
	memset(&shields, 0, sizeof(SHLD));
	memset(&eyes, 0, sizeof(EYE));
	memset(&autocentering, 0, sizeof(ACEN));
	memset(&insignia, 0, sizeof(INSG));
	memset(&paths, 0, sizeof(PATH));
	memset(&pofinfo, 0, sizeof(PINF));
	memset(&hull_lights, 0, sizeof(PINF));
	memset(&shield_collision, 0, sizeof(SLDC));

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


	int len = PNT_Alias(mode)->num_slots, i;

	
	slot *temp_slots;
	// adding a blank slot;
	slot t_slot;
	t_slot.guns = NULL;
	t_slot.num_guns = 0;



	if (len > 0)
	{
		temp_slots = new slot[len+1];
		//already has slots.. add a new one
		for (i = 0; i < len; i++)
		{
			temp_slots[i] = PNT_Alias(mode)->slots[i];
		}

		PNT_Alias(mode)->num_slots++;

		delete[] PNT_Alias(mode)->slots;

		PNT_Alias(mode)->slots = temp_slots; /*new slot[PNT_Alias(mode)->num_slots];

		for (i = 0; i < len; i++)
		{
			PNT_Alias(mode)->slots[i] = temp_slots[i];
		}*/

		PNT_Alias(mode)->slots[len] = t_slot;
		//delete[] temp_slots;
	}
	else
	{
		PNT_Alias(mode)->num_slots = 1;
		PNT_Alias(mode)->slots = new slot[1];
		PNT_Alias(mode)->slots[0] = t_slot;
	}


}

//------------------------------------------


bool POF::PNT_AddPoint				(int mode, int slot, vector3d point, vector3d norm)
{
	if ((unsigned)slot > PNT_SlotCount(mode))
		return false;

	int i, len;
	gun *temp_guns;
	gun newgun;

	newgun.norm = norm;
	newgun.point = point;

	len = PNT_PointCount(mode, slot);

	if (len > 0)
	{
		temp_guns = new gun[len+1];

		for (i = 0; i < len; i++)
		{
			temp_guns[i] = PNT_Alias(mode)->slots[slot].guns[i];
		}

		delete[] PNT_Alias(mode)->slots[slot].guns;
		PNT_Alias(mode)->slots[slot].num_guns++;
		PNT_Alias(mode)->slots[slot].guns = temp_guns;/*new gun[PNT_Alias(mode)->slots[slot].num_guns];

		for (i = 0; i < len; i++)
		{
			PNT_Alias(mode)->slots[slot].guns[i] = temp_guns[i];
		}*/
		PNT_Alias(mode)->slots[slot].guns[len] = newgun;
		//delete[] temp_guns;
	}
	else
	{
		PNT_Alias(mode)->slots[slot].num_guns = 1;
		PNT_Alias(mode)->slots[slot].guns = new gun[1];
		PNT_Alias(mode)->slots[slot].guns[0] = newgun;
	}
	
	return true;
}

//------------------------------------------

unsigned int  POF::PNT_SlotCount				(int mode)
{
	
	return PNT_Alias(mode)->num_slots;
}

//------------------------------------------

unsigned int  POF::PNT_PointCount				(int mode, int slot)
{

	if ((unsigned)slot > PNT_Alias(mode)->num_slots)
		return -1; //-1 means array out of bounds

	return PNT_Alias(mode)->slots[slot].num_guns;

}

//------------------------------------------


bool POF::PNT_DelSlot				(int mode, int slot_num)
{
	if ((unsigned)slot_num > PNT_Alias(mode)->num_slots)
		return false;
	
	int len = PNT_Alias(mode)->num_slots;

	slot *temp_slots;

	temp_slots = new slot[len - 1];
	int i;

	for (i = 0; i < len; i++)
	{
		if (i != slot_num)
		{
			if (i > slot_num)
				temp_slots[i-1] = PNT_Alias(mode)->slots[i];
			if (i < slot_num)
				temp_slots[i] = PNT_Alias(mode)->slots[i];
		}
	}

	delete[] PNT_Alias(mode)->slots[slot_num].guns;
	delete[] PNT_Alias(mode)->slots;

	PNT_Alias(mode)->num_slots--;

	PNT_Alias(mode)->slots = temp_slots;/*new slot[PNT_Alias(mode)->num_slots];

	for (i = 0; i < PNT_Alias(mode)->num_slots; i++)
	{
		PNT_Alias(mode)->slots[i] = temp_slots[i];
	}
	delete[] temp_slots;*/
	return true;

}

bool POF::PNT_DelPoint				(int mode, int slot, int point)
{
	if ((unsigned)slot > PNT_Alias(mode)->num_slots)
		return false;

	if ((unsigned)point > PNT_Alias(mode)->slots[slot].num_guns)
		return false;

	int len = PNT_Alias(mode)->slots[slot].num_guns;
	gun *temp_guns = new gun[len - 1];
	int i;

	for (i = 0; i < len; i++)
	{
		if (i != point)
		{
			if (i > point)
				temp_guns[i-1] = PNT_Alias(mode)->slots[slot].guns[i];
			if (i < point)
				temp_guns[i] = PNT_Alias(mode)->slots[slot].guns[i];
		}
	}

	delete[] PNT_Alias(mode)->slots[slot].guns;
	

	PNT_Alias(mode)->slots[slot].num_guns--;

	PNT_Alias(mode)->slots[slot].guns = temp_guns;/*new gun[PNT_Alias(mode)->slots[slot].num_guns];

	for (i = 0; i < PNT_Alias(mode)->slots[slot].num_guns; i++)
	{
		PNT_Alias(mode)->slots[slot].guns[i] = temp_guns[i];
	}
	delete[] temp_guns;*/
	return true;

}


//------------------------------------------

bool POF::PNT_EditPoint				(int mode, int slot, int point_num, vector3d point, vector3d norm)
{
	if ((unsigned)slot > PNT_Alias(mode)->num_slots)
		return false;

	if ((unsigned)point_num > PNT_Alias(mode)->slots[slot].num_guns)
		return false;

	PNT_Alias(mode)->slots[slot].guns[point_num].point = point;
	PNT_Alias(mode)->slots[slot].guns[point_num].norm = norm;

	return true;

}

//------------------------------------------

bool POF::PNT_GetPoint				(int mode, int slot, int point_num, vector3d &point, vector3d &norm)
{
	if ((unsigned)slot > PNT_Alias(mode)->num_slots)
		return false;

	if ((unsigned)point_num > PNT_Alias(mode)->slots[slot].num_guns)
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
	int len = T_Count_Banks(mode);
	int i = 0;
	Turret_bank *tempbanks, nbank;
	nbank.num_firing_points = 0;
	nbank.position = NULL;
	nbank.sobj_par_phys = sobj_par_phys;
	nbank.sobj_parent = sobj_parent;
	nbank.turret_normal = normal;

	if (len > 0)
	{
		tempbanks = new Turret_bank[len+1];
		
		for (i = 0; i < len; i++)
		{
			tempbanks[i] = T_Alias(mode)->banks[i];
		}

		delete[] T_Alias(mode)->banks;
		T_Alias(mode)->num_banks++;
		T_Alias(mode)->banks = tempbanks;/*new Turret_bank[T_Alias(mode)->num_banks];

		for (i = 0; i < len; i++)
		{
			T_Alias(mode)->banks[i] = tempbanks[i];
		}*/
		T_Alias(mode)->banks[len] = nbank;
		//delete[] tempbanks;
	}
	else
	{
		T_Alias(mode)->num_banks = 1;
		T_Alias(mode)->banks = new Turret_bank[1];
		T_Alias(mode)->banks[0] = nbank;
	}
}

//------------------------------------------

bool POF::T_Add_FirePoint			(int mode, int bank, vector3d pos)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;
	int i = 0; 
	int len = T_Count_Points(mode, bank);

	vector3d *temp = new vector3d[len+1];

	for (i = 0; i < len; i++)
		temp[i] = T_Alias(mode)->banks[bank].position[i];
	temp[len] = pos;

	delete[] T_Alias(mode)->banks[bank].position;
	T_Alias(mode)->banks[bank].num_firing_points++;
	T_Alias(mode)->banks[bank].position = temp;
		

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

	int i, len = T_Count_Points(mode, bank);
	vector3d *temp = new vector3d[len -1];


	for (i = 0; i < len; i++)
	{
		if (len != point)
		{
			if (i < point)
				temp[i] = T_Alias(mode)->banks[bank].position[i];
			if (i > point)
				temp[i-1] = T_Alias(mode)->banks[bank].position[i];
		}
	}


	delete[] T_Alias(mode)->banks[bank].position;
	T_Alias(mode)->banks[bank].position = temp;
	T_Alias(mode)->banks[bank].num_firing_points--;
		/*new vector[len -1];


	for (i = 0; i < (len-1); i++)
	{
		T_Alias(mode)->banks[bank].position[i] = temp[i];
	}
	delete[] temp;*/
	return true;
}

//------------------------------------------

bool POF::T_Del_Bank					(int mode, int bank)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return false;

	int i, len = T_Count_Banks(mode);

	Turret_bank *temp = new Turret_bank[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != bank)
		{
			if (i < bank)
				temp[i] = T_Alias(mode)->banks[i];
			if (i > bank)
				temp[i-1] = T_Alias(mode)->banks[i];
		}
	}

	delete[] T_Alias(mode)->banks[bank].position;
	delete[] T_Alias(mode)->banks;
	T_Alias(mode)->num_banks--;
	T_Alias(mode)->banks = temp;/*new Turret_bank[T_Alias(mode)->num_banks];

	for (i = 0; i < (len - 1); i++)
	{
		T_Alias(mode)->banks[i] = temp[i];
	}

	delete[] temp;*/
	return true;
}


//------------------------------------------

unsigned int POF::T_Count_Banks				(int mode)
{
	return T_Alias(mode)->num_banks;
}

//------------------------------------------

unsigned int POF::T_Count_Points				(int mode, int bank)
{
	if ((unsigned)bank > T_Count_Banks(mode))
		return -1;
	return T_Alias(mode)->banks[bank].num_firing_points;
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
	unsigned int length = textures.num_textures;
	std::string stemp = APStoString(texname);
	unsigned int i;
	

	if (textures.num_textures > 0)
	{
		std::string *tempstrings = new std::string[textures.num_textures+1];
	
		for (i = 0; i < length; i++)
			tempstrings[i] = textures.tex_filename[i];

		delete[] textures.tex_filename;

		textures.num_textures++;

		textures.tex_filename = tempstrings;/*new string[textures.num_textures];

		for (i = 0; i < length; i++)
			textures.tex_filename[i] = tempstrings[i];
		delete[] tempstrings;*/

		textures.tex_filename[length] = stemp;
		return (signed)length;
	}
	else
	{
		textures.num_textures = 1;
		textures.tex_filename = new std::string[textures.num_textures];
		textures.tex_filename[0] = stemp;

		return 0;
	}
	
	return -1; //some type of error
}

//----------------------------------------------

bool POF::TXTR_DelTexture(int texture)
{
	int len = textures.num_textures;

	if (texture < 0 || texture > len)
		return false;
	
	int i;
	std::string *texes = new std::string[len - 1];


	for (i = 0; i < len; i++)
	{
		if (i != texture)
		{
			if (i < texture)
				texes[i] = textures.tex_filename[i];
			if (i > texture)
				texes[i-1] = textures.tex_filename[i];
			
		}
	}

	delete[] textures.tex_filename;

	textures.num_textures--;
	textures.tex_filename = texes;/*new string[textures.num_textures];

	for (i = 0; i < textures.num_textures; i++)
	{
		textures.tex_filename[i] = texes[i];
	}
	delete[] texes;*/
	return true;

}

//----------------------------------------------

int POF::TXTR_FindTexture(std::string texname)
{
	//char *temp;

	for (unsigned int i = 0; i < textures.num_textures; i++)
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
	if (texture < 0 || (unsigned)texture > textures.num_textures)
		return false;

	textures.tex_filename[texture] = APStoString(textname);
	return true;
}

//----------------------------------------------

std::string POF::TXTR_GetTextures(int texture)
{
	if (texture < 0 || (unsigned)texture > textures.num_textures)
		return std::string("Error: Invalid Index");
	
	return StringToAPS(textures.tex_filename[texture]);

}




//**********************************************************************************************************************
// Header Functions

void POF::HDR2_Get_Details				(int &num, std::vector<int> &SOBJ_nums)
{
	num = header.num_detaillevels; 
	SOBJ_nums.resize(num);

	for (int i = 0; i < num; i++)
	{
		SOBJ_nums[i] = header.sobj_detaillevels[i];
	}
}


void POF::HDR2_Set_Details				(int num, std::vector<int> SOBJ_nums)
{
	if (header.sobj_detaillevels != NULL)
	{
		delete[] header.sobj_detaillevels;
	}

	header.sobj_detaillevels = new int[num];

	for (int i = 0; i < num; i++)
	{
		header.sobj_detaillevels[i] = SOBJ_nums[i];
	}

	header.num_detaillevels = num;

}

//----------------------------------------------

void POF::HDR2_Get_Debris				(int &num, std::vector<int> &SOBJ_nums)
{
	num = header.num_debris;
	SOBJ_nums.resize(num);

	for (int i = 0; i < num; i++)
	{
		SOBJ_nums[i] = header.sobj_debris[i];

	}
}

void POF::HDR2_Set_Debris				(int num, std::vector<int> SOBJ_nums)
{

	if (header.sobj_debris != NULL)
	{
		delete[] header.sobj_debris;
	}

	header.sobj_debris = new int[num];

	for (int i = 0; i < num; i++)
	{
		header.sobj_debris[i] = SOBJ_nums[i];
	}

	header.num_debris = num;
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
	num = header.num_cross_sections;

	if (num == -1)
		return;

	sections.resize(num);

	for (int i = 0; i < num; i++)
	{
		sections[i] = header.cross_sections[i];
	}
}

void POF::HDR2_Set_CrossSections		(int num, std::vector<cross_section> sections)
{
	if (header.cross_sections != NULL)
	{
		delete[] header.cross_sections;
	}

	header.cross_sections = new cross_section[num];

	for (int i = 0; i < num; i++)
	{
		header.cross_sections[i] = sections[i];
	}

	header.num_cross_sections = num;
}

//----------------------------------------------

void POF::HDR2_Get_Lights				(int &num, std::vector<muzzle_light> &li)
{
	num = header.num_lights;
	li.resize(num);

	for (int i = 0 ; i < num; i++)
	{
		li[i] = header.lights[i];
	}
}

void POF::HDR2_Set_Lights				(int num, const std::vector<muzzle_light> &li)
{
	if (header.lights != NULL)
	{
		delete[] header.lights;
	}

	header.lights = new muzzle_light[num];

	for (int i = 0; i < num; i++)
	{
		header.lights[i] = li[i];
	}

	header.num_lights = num;
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
	int len = OBJ2_Count();
	
	OBJ2 *tempobj = new OBJ2[len+1];

	for(int i = 0; i < len; i++)
		tempobj[i] = objects[i];
	//memset(tempobj, 0, sizeof(OBJ2)*(len+1));
	//memcpy(tempobj, objects, sizeof(OBJ2)*len);

	delete[] objects;

	objects = tempobj;
	numobj2++;

	return len;
}

//----------------------------------------------

bool POF::OBJ2_Del_SOBJ					(int SOBJNum)
{
	if ((unsigned)SOBJNum > OBJ2_Count())
		return false;

	int i, len = OBJ2_Count();
	OBJ2 *temp = new OBJ2[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i < SOBJNum)
			temp[i] = objects[i];
		if (i > SOBJNum)
			temp[i - 1] = objects[i];

	}

	delete[] objects;
	numobj2--;
	objects = temp;

	return true;
}

//----------------------------------------------

unsigned int POF::OBJ2_Count()
{
	return numobj2;
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
	return objects[SOBJNum].bsp_data_size;

}

//----------------------------------------------



bool POF::OBJ2_Set_BSPData				(int SOBJNum, int size, const char *bsp_data)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;


	objects[SOBJNum].bsp_data.reset(new char[size]);

	memcpy(objects[SOBJNum].bsp_data.get(), bsp_data, size);

	return true;


}

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

	bsp_data = objects[SOBJNum].bsp_data.get();

	return true;
}

bool POF::OBJ2_Get_BSPData				(int SOBJNum, int &size, boost::shared_array<char> &bsp_data)
{
	if ((unsigned)SOBJNum > OBJ2_Count() || SOBJNum < 0)
		return false;
	size = OBJ2_BSP_Datasize(SOBJNum);

	if (size == 0)
	{
		bsp_data.reset(NULL);
		return true;
	}

	bsp_data.reset(new char[size]);

	memcpy(bsp_data.get(), objects[SOBJNum].bsp_data.get(), size);

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

	int i, len = SPCL_Count();
	special_point *temp = new special_point[len+1];


	for (i = 0; i < len; i++)		
	{
		temp[i] = specials.special_points[i];
	}
	temp[len] = npoint;

	delete[] specials.special_points;
	specials.num_special_points++;
	specials.special_points = temp;
}

//----------------------------------------------

bool POF::SPCL_DelSpecial				(int num)
{
	
	if ((unsigned)num > SPCL_Count())
		return false;

	int i, len = SPCL_Count();
	special_point *temp = new special_point[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != num)
		{
			if (num > i)
				temp[i] = specials.special_points[i];
			if (num < i)
				temp[i-1] = specials.special_points[i];

		}
	}

	delete[] specials.special_points;
	specials.num_special_points--;
	specials.special_points = temp;/*new special_point[specials.num_special_points];

	for (i = 0; i < (len-1); i++)
	{
		specials.special_points[i] = temp[i];
	}
	delete[] temp;*/
	return true;
}

//----------------------------------------------

unsigned int  POF::SPCL_Count					()
{
	return specials.num_special_points; 
}

//----------------------------------------------

bool POF::SPCL_Get_Special				(int num, std::string &Name, std::string &Properties, vector3d &point, float &radius)
{
	//apstring StringToAPS(string str)

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
	int i, len = DOCK_Count_Docks();
	dock_point npt, *temp = new dock_point[len + 1];

	npt.num_points = 0;
	npt.num_spline_paths = 0;
	npt.path_number = NULL;
	npt.points = NULL;
	npt.properties = APStoString(properties);

	for (i = 0; i < len; i++)
		temp[i] = docking.points[i];
	temp[len] = npt;

	delete[] docking.points;
	docking.points = temp;
	docking.num_docks++;
}



bool POF::DOCK_Add_SplinePath		(int dock, int path)
{
	if ((unsigned)dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;
	int i, len = DOCK_Count_SplinePaths(dock);
	int *temp = new int[len + 1];

	for (i = 0; i < len; i++)
		temp[i] = docking.points[dock].path_number[i];
	temp[len] = path;

	delete[] docking.points[dock].path_number;
	docking.points[dock].path_number = temp;
	docking.points[dock].num_spline_paths++;

	return true;
}



bool POF::DOCK_Add_Point			(int dock, vector3d point, vector3d norm)
{
	if ((unsigned)dock < 0  || (unsigned)dock > DOCK_Count_Docks())
		return false;
	int i, len = DOCK_Count_Points(dock);
	gun *temp = new gun[len + 1], np;

	np.norm = norm;
	np.point = point;

	for (i = 0; i < len; i++)
		temp[i] = docking.points[dock].points[i];
	temp[len] = np;

	delete[] docking.points[dock].points;
	docking.points[dock].points = temp;
	docking.points[dock].num_points++;

	return true;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::DOCK_Count_Docks			()
{
	return docking.num_docks;
}

unsigned int POF::DOCK_Count_SplinePaths		(int dock)
{
	if ((unsigned)dock < 0  || (unsigned)dock > docking.num_docks)
		return -1;
	return docking.points[dock].num_spline_paths;
}

unsigned int POF::DOCK_Count_Points			(int dock)
{
	if ((unsigned)dock < 0  || (unsigned)dock > docking.num_docks)
		return -1;
	return docking.points[dock].num_points;
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
	if ((unsigned)dock < 0  || (unsigned)dock > DOCK_Count_Docks())
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
	int i, len = DOCK_Count_Docks();
	dock_point *temp = new dock_point[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != dock)
		{
			if (i < dock)
				temp[i] = docking.points[i];
			if (i > dock)
				temp[i-1] = docking.points[i];
		}
	}

	delete[] docking.points[dock].path_number;
	delete[] docking.points[dock].points;
	delete[] docking.points;
	docking.points = temp;
	docking.num_docks--;
	return true;

}


bool POF::DOCK_Del_SplinePath		(int dock, int spline_path_num)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks() || (unsigned)spline_path_num > DOCK_Count_SplinePaths(dock))
		return false;
	int i, len = DOCK_Count_SplinePaths(dock);
	int *temp = new int[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != spline_path_num)
		{
			if (i < spline_path_num)
				temp[i] = docking.points[dock].path_number[i];
			if (i > spline_path_num)
				temp[i - 1] = docking.points[dock].path_number[i];

		}
	}

	delete[] docking.points[dock].path_number;
	docking.points[dock].path_number = temp;
	docking.points[dock].num_spline_paths--;

	return true;
}

bool POF::DOCK_Del_Point			(int dock, int point)
{
	if (dock < 0  || (unsigned)dock > DOCK_Count_Docks() || (unsigned)point > DOCK_Count_Points(dock))
		return false;
	int i, len = DOCK_Count_Points(dock);
	gun *temp = new gun[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != point)
		{
			if (i < point)
				temp[i] = docking.points[dock].points[i];
			if (i > point)
				temp[i - 1] = docking.points[dock].points[i];
		}
	}

	delete[] docking.points[dock].points;
	docking.points[dock].points = temp;
	docking.points[dock].num_points--;
	return true;

}

//**********************************************************************************************************************
// FUEL Functions

void POF::FUEL_Add_Thruster			(std::string properties)
{
	int i, len = FUEL_Count_Thrusters();
	thruster *ntrts = new thruster[len + 1], n;
	n.num_glows = 0;
	n.points = NULL;
	n.properties = APStoString(properties);

	for (i = 0; i < len; i++)
		ntrts[i] = thrusters.thrusters[i];
	ntrts[len] = n;

	delete[] thrusters.thrusters;
	thrusters.num_thrusters++;
	thrusters.thrusters = ntrts;
	
}

bool POF::FUEL_Add_GlowPoint		(int bank, float radius, vector3d pos, vector3d norm)
{
	if ((unsigned)bank > FUEL_Count_Thrusters())
		return false;

	int i, len = FUEL_Count_Glows(bank);

	glow_point *nglws = new glow_point[len + 1];

	// new glow
	glow_point ng;
	ng.norm = norm;
	ng.pos = pos;
	ng.radius = radius;
	

	for (i = 0; i < len; i++)
		nglws[i] = thrusters.thrusters[bank].points[i];
	nglws[len] = ng;

	delete[] thrusters.thrusters[bank].points;
	thrusters.thrusters[bank].num_glows++;
	thrusters.thrusters[bank].points = nglws;
	return true;	
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::FUEL_Count_Thrusters		()
{ return thrusters.num_thrusters; }

unsigned int POF::FUEL_Count_Glows			(int thruster)
{ 
	if ((unsigned)thruster > FUEL_Count_Thrusters())
		return -1;
	return thrusters.thrusters[thruster].num_glows;
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
	int i, len = FUEL_Count_Thrusters();
	thruster *ntrts = new thruster[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != thrust)
		{
			if (i < thrust)
				ntrts[i] = thrusters.thrusters[i];
			if (i > thrust)
				ntrts[i - 1] = thrusters.thrusters[i];

		}
	}

	delete[] thrusters.thrusters[thrust].points;
	delete[] thrusters.thrusters;
	thrusters.num_thrusters--;
	thrusters.thrusters = ntrts;
	return true;

}


bool POF::FUEL_Del_GlowPoint			(int thruster, int glowpoint)
{
	if ((unsigned)thruster > FUEL_Count_Thrusters() || (unsigned)glowpoint > FUEL_Count_Glows(thruster))
		return false;
	int i, len = FUEL_Count_Glows(thruster);
	glow_point *nglws = new glow_point[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != glowpoint)
		{
			if (i < glowpoint)
				nglws[i] = thrusters.thrusters[thruster].points[i];
			if (i > glowpoint)
				nglws[i-1] = thrusters.thrusters[thruster].points[i];

		}
	}

	delete[] thrusters.thrusters[thruster].points;
	thrusters.thrusters[thruster].num_glows--;
	thrusters.thrusters[thruster].points = nglws;
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
	int i, len = SHLD_Count_Vertices();
	

	if (len > 0)
	{
		vector3d *temp = new vector3d[len + 1];
		for (i = 0; i < len; i++)
		{
			temp[i] = shields.vertecies[i];
		}

		delete[] shields.vertecies;
		shields.num_vertices++;
		shields.vertecies = temp;/*new vector[shields.num_vertices];

		for (i = 0; i < len; i++)
		{
			shields.vertecies[i] = temp[i];
		}*/
		//delete[] temp;

		shields.vertecies[len] = vert;
	}
	else
	{
		shields.num_vertices = 1;
		shields.vertecies = new vector3d[1];
		shields.vertecies[0] = vert;
	}
}


void POF::SHLD_Add_Face				(vector3d normal, const int fcs[3], const int nbs[3])
{
	int i, len = SHLD_Count_Faces();

	shield_face nface;

	nface.face_normal = normal;

	for (i = 0; i < 3; i++)
	{
		nface.face_vertices[i] = fcs[i];
		nface.neighbors[i] = nbs[i];
	}

	if (len > 0)
	{
		shield_face *temp = new shield_face[len+1];
		for (i = 0; i < len; i++)
		{
			temp[i] = shields.shield_faces[i];
		}

		delete[] shields.shield_faces;
		shields.num_faces++;
		shields.shield_faces = temp;/*new shield_face[shields.num_faces];

		for (i = 0; i < len; i++)
		{
			shields.shield_faces[i] = temp[i];
		}
		delete[] temp;*/
		shields.shield_faces[len] = nface;
	}
	else
	{
		shields.num_faces = 1;
		shields.shield_faces = new shield_face[1];
		shields.shield_faces[0] = nface;
	}

}
								// fcs = int[3] faces, nbs = int[3] neighbors
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::SHLD_Count_Vertices		()
{
	return shields.num_vertices;
}

unsigned int POF::SHLD_Count_Faces			()
{
	return shields.num_faces;
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

	int i, len = SHLD_Count_Vertices();
	vector3d *temp = new vector3d[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != vertex)
		{
			if (i < vertex)
				temp[i] = shields.vertecies[i];
			if (i > vertex)
				temp[i-1] = shields.vertecies[i];
		}
	}

	delete[] shields.vertecies;
	
	shields.num_vertices--;

	shields.vertecies = temp;/*new vector[shields.num_vertices];

	for (i = 0; i < (len -1); i++)
	{
		shields.vertecies[i] = temp[i];
	}
	delete[] temp;*/
	return true;
}

bool POF::SHLD_Del_Face				(int face)
{
	if ((unsigned)face > SHLD_Count_Faces())
		return false;

	int i, len = SHLD_Count_Faces();
	shield_face *temp = new shield_face[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != face)
		{
			if (i < face)
				temp[i] = shields.shield_faces[i];
			if (i > face)
				temp[i-1] = shields.shield_faces[i];
		}
	}

	delete[] shields.shield_faces;
	
	shields.num_faces--;

	shields.shield_faces = temp;/*new shield_face[shields.num_faces];

	for (i = 0; i < (len -1); i++)
	{
		shields.shield_faces[i] = temp[i];
	}
	delete[] temp;*/
	return true;
}

//**********************************************************************************************************************
// EYE Functions
void POF::EYE_Add_Eye				(int sobj_num, vector3d offset, vector3d normal)
{
	eye_pos *eyes_temp;
	int len = eyes.num_eye_positions;
	int i;
	eye_pos eye_temp;

	eye_temp.normal = normal;
	eye_temp.sobj_number = sobj_num;
	eye_temp.sobj_offset = offset;

	if (len > 0)
	{
		eyes_temp = new eye_pos[len+1];
		//has eyes already
		for (i = 0; i < len; i++)
		{
			eyes_temp[i] = eyes.eye_positions[i];
		}

		delete[] eyes.eye_positions;

		eyes.num_eye_positions++;

		eyes.eye_positions = eyes_temp;/*new eye_pos[eyes.num_eye_positions];

		for (i = 0; i < len; i++)
		{
			eyes.eye_positions[i] = eyes_temp[i];
		}*/

		eyes.eye_positions[len] = eye_temp;

		//delete[] eyes_temp;
	}
	else
	{
		eyes.num_eye_positions = 1;
		eyes.eye_positions = new eye_pos[1];
		eyes.eye_positions[0] = eye_temp;
		//no eyes already
	}
	
}

//----------------------------------------------

bool POF::EYE_Del_Eye				(int eye)
{
	int len = eyes.num_eye_positions;

	if (eye > len)
		return false;
	int i;
	eye_pos *temp_eyes = new eye_pos[len - 1];


	for (i = 0; i < len; i++)
	{
		if (i != eye)
		{
			if (i < eye)
				temp_eyes[i] = eyes.eye_positions[i];
			if (i > eye)
				temp_eyes[i - 1] = eyes.eye_positions[i];
		}
	}

	delete[] eyes.eye_positions;
	eyes.num_eye_positions--;
	eyes.eye_positions = temp_eyes;/*new eye_pos[len - 1];


	for (i = 0; i < eyes.num_eye_positions; i++)
	{
		eyes.eye_positions[i] = temp_eyes[i];
	}
	delete[] temp_eyes;*/
	return true;
}


unsigned int POF::EYE_Count_Eyes			()
{
	return eyes.num_eye_positions;
}


bool POF::EYE_Get_Eye				(int eye, int &sobj_num, vector3d &offset, vector3d &normal)
{
	if ((unsigned)eye > eyes.num_eye_positions)
		return false;

	sobj_num = eyes.eye_positions[eye].sobj_number;
	offset = eyes.eye_positions[eye].sobj_offset;
	normal = eyes.eye_positions[eye].normal;

	return true;
}

bool POF::EYE_Edit_Eye				(int eye, int sobj_num, vector3d offset, vector3d normal)
{
	if ((unsigned)eye > eyes.num_eye_positions)
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
	ni.faces = NULL;
	ni.num_faces = 0;
	ni.num_verticies = 0;
	ni.vertex_pos = NULL;
	//the replacement array
	unsigned int i;
	insig *ninsigs = new insig[INSG_Count_Insignia() + 1];

	for (i = 0; i < INSG_Count_Insignia(); i++)
	{
		ninsigs[i] = insignia.insignias[i];
	}
	ninsigs[INSG_Count_Insignia()] = ni;

	delete[] insignia.insignias;
	insignia.num_insignias++;
	insignia.insignias = ninsigs;

}

bool POF::INSG_Add_Insig_Vertex		(int insig, vector3d vertex)
{
	// man look at this new code... joygasm :)
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	int i, len = INSG_Count_Vertecies(insig);
	vector3d *nvecs = new vector3d[len + 1];

	for (i = 0; i < len; i++)
		nvecs[i] = insignia.insignias[insig].vertex_pos[i];
	nvecs[len] = vertex;

	delete[] insignia.insignias[insig].vertex_pos;
	insignia.insignias[insig].num_verticies++;
	insignia.insignias[insig].vertex_pos = nvecs;
	
	return true;

}


bool POF::INSG_Add_Insig_Face		(int insig, const int vert_indecies[], const vector3d u_tex_coord, const vector3d v_tex_coord)
{
	// i cannot get over this efficiency yet
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	int len = INSG_Count_Faces(insig);
	
	insg_face *nfaces = new insg_face[len + 1], nf;
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

	for (i = 0 ; i < len; i++)
		nfaces[i] = insignia.insignias[insig].faces[i];
	nfaces[len] = nf;

	delete[] insignia.insignias[insig].faces;
	insignia.insignias[insig].num_faces++;
	insignia.insignias[insig].faces = nfaces;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool POF::INSG_Add_Insig_Face		(int insig, insg_face &InsgFace)
// this is how i should have written it in the first place
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return false;
	int len = INSG_Count_Faces(insig), i;
	
	insg_face *nfaces = new insg_face[len + 1];

	for (i = 0 ; i < len; i++)
		nfaces[i] = insignia.insignias[insig].faces[i];
	nfaces[len] = InsgFace;

	delete[] insignia.insignias[insig].faces;
	insignia.insignias[insig].num_faces++;
	insignia.insignias[insig].faces = nfaces;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::INSG_Count_Insignia		()
{ return insignia.num_insignias; }

unsigned int POF::INSG_Count_Vertecies		(int insig)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return -1;
	return insignia.insignias[insig].num_verticies;
}

unsigned int POF::INSG_Count_Faces			(int insig)
{
	if ((unsigned)insig > INSG_Count_Insignia())
		return -1;
	return insignia.insignias[insig].num_faces;
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

	for (unsigned int i = 0; i < insignia.insignias[insig].num_verticies; i++)
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
	int i, len = INSG_Count_Insignia();

	insig *temp = new insig[len - 1];

	for (i = 0; i < len; i++)
		if (i != insg)
		{
			if (i < insg)
				temp[i] = insignia.insignias[i];
			if (i > insg)
				temp[i-1] = insignia.insignias[i];

		}
	delete[] insignia.insignias[insg].faces;
	delete[] insignia.insignias[insg].vertex_pos;
	delete[] insignia.insignias;
	insignia.num_insignias--;
	insignia.insignias = temp;/*new insig[insignia.num_insignias];

	for (i = 0; i < insignia.num_insignias; i++)
	{
		insignia.insignias[i] = temp[i];
	}

	delete[] temp;*/
	return true;
}

bool POF::INSG_Del_Insig_Vertex		(int insig, int vert)
{

	//rewrote to new way
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)vert > INSG_Count_Vertecies(insig))
		return false;

	int i, len = INSG_Count_Vertecies(insig);
	vector3d *temp = new vector3d[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != vert)
		{
			if (i < vert)
				temp[i] = insignia.insignias[insig].vertex_pos[i];
			if (i > vert)
				temp[i - 1] = insignia.insignias[insig].vertex_pos[i];
		}
	}

	delete[] insignia.insignias[insig].vertex_pos;
	insignia.insignias[insig].num_verticies--;
	insignia.insignias[insig].vertex_pos = temp; // mwahaha, just refereing to the new block
												 // Efficiency++
	return true;
}




bool POF::INSG_Del_Insig_Face		(int insig, int face)
{
	// the first function i wrote after realizing a more efficient way of reallocating the
	//	data, the rest will be recoded later
	if ((unsigned)insig > INSG_Count_Insignia() || (unsigned)face > INSG_Count_Faces(insig))
		return false;
	int i, len = INSG_Count_Faces(insig);
	insg_face *temp = new insg_face[len - 1];

	for (i = 0; i < len; i++)
	{
		if (i != face)
		{
			if (i < face)
				temp[i] = insignia.insignias[insig].faces[i];
			if (i > face)
				temp[i-1] = insignia.insignias[insig].faces[i];
		}
	}

	delete[] insignia.insignias[insig].faces;
	insignia.insignias[insig].num_faces--;;
	insignia.insignias[insig].faces = temp; //Refere to new varable instead of wasting memory and computer processing time
											//To copy and use the temp buffer as a temporary only
											//ExecutionTime-- Efficiency++
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
	int i, len = PATH_Count_Paths();
	a_path newpath;

	newpath.name = APStoString(name);
	newpath.parent = APStoString(parent);
	newpath.num_verts = 0;
	newpath.verts = NULL;

	if (len > 0)
	{
		a_path *temp = new a_path[len+1];

		for (i = 0; i < len; i++)
			temp[i] = paths.paths[i];

		delete[] paths.paths;
		paths.num_paths++;
		paths.paths = temp;/*new a_path[len + 1];

		for (i = 0; i < len; i++)
			paths.paths[i] = temp[i];
		delete[] temp;*/
		paths.paths[len] = newpath;
	}
	else
	{
		paths.num_paths = 1;
		paths.paths = new a_path[1];
		paths.paths[0] = newpath;
	}
	

}



bool POF::PATH_Add_Vert				(int path, vector3d point, float rad)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;
	int i, len = PATH_Count_Verts(path);

	path_vert newvert;
	newvert.num_turrets = 0;
	newvert.pos = point;
	newvert.radius = rad;
	newvert.sobj_number = NULL;


	if (len > 0)
	{
		path_vert *temp = new path_vert[len+1];
		for (i = 0; i < len; i++)
			temp[i] = paths.paths[path].verts[i];

		delete[] paths.paths[path].verts;
		paths.paths[path].num_verts++;
		paths.paths[path].verts = temp;/*new path_vert[len + 1];

		for (i = 0; i < len; i++)
			paths.paths[path].verts[i] = temp[i];*/
		paths.paths[path].verts[len] = newvert;
		//delete[] temp;
	}
	else
	{
		paths.paths[path].num_verts = 1;
		paths.paths[path].verts = new path_vert[1];
		paths.paths[path].verts[0] = newvert;
	}

	return true;
}

bool POF::PATH_Add_Turret			(int path, int vert, int sobj_number)
{
	if (path < 0 || vert < 0 || (unsigned)path > PATH_Count_Paths() || (unsigned)path > PATH_Count_Verts(path))
		return false;

	int i, len = PATH_Count_Turrets(path, vert);
	int *temp = new int[len + 1];

	for (i = 0; i < len; i++)
		temp[i] = paths.paths[path].verts[vert].sobj_number[i];
	temp[len] = sobj_number;

	delete[] paths.paths[path].verts[vert].sobj_number;
	paths.paths[path].verts[vert].num_turrets++;		
	paths.paths[path].verts[vert].sobj_number = temp;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


bool POF::PATH_Del_Path				(int path)
{
	if ((unsigned)path > PATH_Count_Paths())
		return false;

	unsigned int i, len = PATH_Count_Paths();
	a_path *temp = new a_path[len - 1];

	for (i = 0; i < len; i++)
		if (i != (unsigned)path)
		{
			if (i < (unsigned)path)
				temp[i] = paths.paths[i];
			if (i > (unsigned)path)
				temp[i - 1] = paths.paths[i];
		}
	
	for (i = 0; i < PATH_Count_Verts(path); i++)
	{
		delete[] paths.paths[path].verts[i].sobj_number;
	}
	delete[] paths.paths[path].verts;
	delete[] paths.paths;
	paths.num_paths--;
	paths.paths = temp;/*new a_path[paths.num_paths];

	for (i = 0; i < paths.num_paths; i++)
		paths.paths[i] = temp[i];
	delete[] temp;*/
	return true;
}


bool POF::PATH_Del_Vert				(int path, int vert)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return false;

	int i, len = PATH_Count_Verts(path);
	path_vert *temp = new path_vert[len - 1];

	for (i = 0; i < len; i++)
		if (i != vert)
		{
			if (i < vert)
				temp[i] = paths.paths[path].verts[i];
			if (i > vert)
				temp[i - 1] = paths.paths[path].verts[i];
		}

	delete[] paths.paths[path].verts[vert].sobj_number;
	delete[] paths.paths[path].verts;
	paths.paths[path].num_verts--;
	paths.paths[path].verts = temp;/*new path_vert[paths.paths[path].num_verts];

	for (i = 0; i < paths.paths[path].num_verts; i++)
		paths.paths[path].verts[i] = temp[i];
	delete[] temp;*/
	return true;
}



bool POF::PATH_Del_Turret			(int path, int vert, int turret)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path) || (unsigned)turret > PATH_Count_Turrets(path, vert))
		return false;
	int i, len = PATH_Count_Turrets(path, vert);
	int *temp = new int[len - 1];

	for (i = 0; i < len; i++)
		if (i != turret)
		{
			if (i < turret)
				temp[i] = paths.paths[path].verts[vert].sobj_number[i];
			if (i < turret)
				temp[i-1] = paths.paths[path].verts[vert].sobj_number[i];
		}

	delete[] paths.paths[path].verts[vert].sobj_number;
	paths.paths[path].verts[vert].num_turrets--;
	paths.paths[path].verts[vert].sobj_number = temp;/*new int[paths.paths[path].verts[vert].num_turrets];

	for (i = 0; i < paths.paths[path].verts[vert].num_turrets; i++)
		paths.paths[path].verts[vert].sobj_number[i] = temp[i];
	delete[] temp;*/
	return true;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int POF::PATH_Count_Paths			()
{
	return paths.num_paths;
}

unsigned int POF::PATH_Count_Verts			(int path)
{
	if ((unsigned)path > PATH_Count_Paths())
		return -1;
	return paths.paths[path].num_verts;
}

unsigned int POF::PATH_Count_Turrets		(int path, int vert)
{
	if ((unsigned)path > PATH_Count_Paths() || (unsigned)vert > PATH_Count_Verts(path))
		return -1;

	return paths.paths[path].verts[vert].num_turrets;
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
	int len = GLOW_Count_LightGroups();
	HullLights *ntrts = new HullLights[len + 1], n;

	n.num_Lights = 0;
	n.disp_time = disp_time;
	n.obj_parent = obj_parent;
	n.off_time = off_time;
	n.on_time = on_time;
	n.LOD = LOD;
	n.type = type;
	n.properties = APStoString(properties);

	// let's make this even FASTER
//	memcpy(ntrts, hull_lights.lights, sizeof(HullLights) * len);
	//lets not shallow copy the properties string
	for (int i = 0; i < len; i++)
		ntrts[i] = hull_lights.lights[i];
	ntrts[len] = n;

	if(hull_lights.num_glows_arrays)
		delete[] hull_lights.lights;
	hull_lights.num_glows_arrays++;
	hull_lights.lights = ntrts;
}


bool POF::GLOW_Add_GlowPoint		(int group, float radius, vector3d pos, vector3d norm)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;

	int len = GLOW_Count_Glows(group);
	HullLightPoint *nlghts = new HullLightPoint[len + 1], n;
	n.norm = norm;
	n.radius = radius;
	n.point = pos;

	// let's make this even FASTER
	memcpy(nlghts, hull_lights.lights[group].lights, sizeof(HullLightPoint) * len);
	nlghts[len] = n;

	if (hull_lights.lights[group].lights)
		delete[] hull_lights.lights[group].lights;
	hull_lights.lights[group].num_Lights++;
	hull_lights.lights[group].lights = nlghts;

	return true;
}


unsigned int POF::GLOW_Count_Glows			(int group)
{
	if (group < 0 || (unsigned)group > GLOW_Count_LightGroups())
		return -1;
	return hull_lights.lights[group].num_Lights;
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

	int i, len = GLOW_Count_LightGroups();
	HullLights *temp = new HullLights[len - 1];

	for (i = 0; i < len; i++)
		if (i != group)
		{
			if (i < group)
				temp[i] = hull_lights.lights[i];
			if (i > group)
				temp[i - 1] = hull_lights.lights[i];
		}
	
	delete[] hull_lights.lights[group].lights;
	//delete[] hull_lights.lights[group].properties.str;

	delete[] hull_lights.lights;
	hull_lights.lights = temp;
	hull_lights.num_glows_arrays--;

	return true;
}

bool POF::GLOW_Del_GlowPoint		(int group, int glowpoint)
{
	if (group < 0 || (unsigned)group >= GLOW_Count_LightGroups())
		return false;
	if (glowpoint < 0 || (unsigned)glowpoint > GLOW_Count_Glows(group))
		return false;

	int i, len = GLOW_Count_Glows(group);
	HullLightPoint *temp = new HullLightPoint[len - 1];

	for (i = 0; i < len; i++)
		if (i != glowpoint)
		{
			if (i < glowpoint)
				temp[i] = hull_lights.lights[group].lights[i];
			if (i > glowpoint)
				temp[i - 1] = hull_lights.lights[group].lights[i];
		}

	delete[] hull_lights.lights[group].lights;
	hull_lights.lights[group].num_Lights--;
	hull_lights.lights[group].lights = temp;

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

	pofinfo.strings = new char[pof_info.length()+1];
	memcpy(pofinfo.strings, pof_info.c_str(), pof_info.length());
	pofinfo.strings[pof_info.length()] = '\0';
}

void POF::PINF_Set					(char *str, int sz)
{

	pofinfo.strings = new char[sz+1];
	memcpy(pofinfo.strings, str, sz);
	pofinfo.strings[sz] = '\0';
}

bool POF::PINF_Del()
{

	delete[] pofinfo.strings;
	pofinfo.strings = NULL;
	return true;
}

std::vector<std::string> POF::PINF_Get				()
{
	int pos = 0;
	std::vector<std::string> strings;
	if (pofinfo.strings == NULL)
		return strings;

	for (pos = 0; pofinfo.strings[pos] != '\0'; pos += strlen(pofinfo.strings+pos)+1)
	{
			strings.resize(strings.size()+1);
			strings[strings.size()-1] = (char*)(pofinfo.strings+pos);
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
