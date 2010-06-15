//****************************************************************************
//	pcs_pmf_cob.cpp
//	POF Constructor Suite 2 File Format: COB Save/Load
//****************************************************************************

/*
 * $Logfile: /pcs_pmf_cob.cpp $
 * $Revision: 1.36 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: pcs_pmf_cob.cpp,v $
 * Revision 1.36  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.35  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.34  2008/05/15 22:49:37  kazan
 * *** empty log message ***
 *
 * Revision 1.33  2008/02/11 17:40:02  kazan
 * Revoking BSPGEN's BSP Caching
 *
 * Revision 1.32  2008/02/11 13:56:08  kazan
 * Possible Fix for Mantis #51
 *
 * Revision 1.31  2008/02/09 22:33:01  kazan
 * bye bye kaz_vector
 *
 * Revision 1.30  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.29  2008/01/19 01:51:59  bobboau
 * fixed small crash bug in COB import related to insignias, Kazan you should still look at the INSG stuff as I'm seeing some probably not right behaviour, see bug 45 for more details.
 *
 * Revision 1.28  2008/01/18 18:39:27  kazan
 * Was finally able to replicate the bounding box inversion - apparently HDR2 bbox's are in openGL coordinate space NOT BSP coordinate Space
 * add support to .cob specifications for "detail" light - a group with a child light named "detail" will be added as a LOD (this allows .cob's written out by PCS2 to be IMMEDIATELY reloadable and have the hierarchy right!)
 *
 * Revision 1.27  2008/01/12 18:02:24  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.26  2007/10/22 23:43:55  kazan
 * Catch a new error condition (thanks for TrashMan for being an airhead), fix crash bug introduced by my replacing POFstring with std::string (thanks to me for being an airhead)
 *
 * Revision 1.25  2007/10/18 15:30:09  kazan
 * fixed a metric ton of memory leaks by finally deprecating COB_String and replacing all instances with std::strings
 *
 * Revision 1.24  2007/07/12 18:25:03  kazan
 * resolve crash in COB save if model has untextured polygons
 *
 * Revision 1.23  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.22  2007/07/06 05:38:55  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.21  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.19  2007/07/01 01:25:47  bobboau
 * removed a function template I had forgoten about and was no longer defined (suprized that didn't cause an error)
 *
 * Revision 1.18  2007/06/30 14:13:13  kazan
 * put back in a little safer version of the contained.size() == 2 optimization
 * potentially fix pof shields
 * add error message for group with no polymodels
 *
 * Revision 1.17  2007/06/30 04:16:31  bobboau
 * added a geometry filter and implemented it into COB loading, it fixes polys with more than 20 verts and more impresively it corects convex polygons.
 *
 * Revision 1.16  2007/06/30 03:02:50  bobboau
 * removed triangulation requirement from shield meshes
 *
 * Revision 1.15  2007/06/26 12:43:34  kazan
 * fix up some more problems with axis conversion - remove code duplication which was making this harder to fix
 *
 * Revision 1.14  2007/06/25 19:39:33  kazan
 * Fix bug related to std::string and operator+= w/ char (not char*)
 * Fix autoturret bugs
 * Fix POF BSP compilation... finally.. confirmed models work in FS2!
 *
 * Revision 1.13  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.12  2007/06/18 01:15:02  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.11  2007/06/14 23:48:10  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.10  2007/06/06 17:35:59  kazan
 * Fix status bar resizing, possibly fix smoothing data bug
 *
 * Revision 1.9  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.8  2007/05/08 02:40:45  kazan
 * bug fixes, error messages
 *
 * Revision 1.7  2007/05/08 00:49:46  kazan
 * fixed some cob bugs, made complete bar work, did some data caching to optimize smoothing calculator
 *
 * Revision 1.6  2007/05/07 17:40:56  kazan
 * changes in progress
 *
 * Revision 1.5  2007/03/25 17:04:15  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.4  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.3  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
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


#pragma warning(disable: 4786)

#include "pcs_file.h"
#include "COBHandler.h"
#include "compat/sleep.h"
#include <cmath>
#include <sstream>
#include "matrix.h"

#ifdef UNIX
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
inline char* _strlwr(char* s) {
	char* start = s;
	while (*s++ = tolower(*s));
	return start;
}
#endif

using namespace std;

vector3d COBtoStandard(const vector3d &cobvec)
{
	//B,C,A->A,B,C
	return MakeVector(cobvec.y, cobvec.z, cobvec.x);
}

vector3d StandardToCOB(const vector3d &stdvec)
{
	//A,B,C->B,C,A
	return MakeVector(stdvec.z, stdvec.x, stdvec.y);
}

struct index_list_cache
{
	vector3d point;
	std::vector<unsigned int> list;
};

bool CheckListcache(std::vector<index_list_cache> &cache, vector3d point, std::vector<unsigned int> &list, int size)
{
	for (unsigned int i = 0; i < (unsigned)size; i++)
	{
		if (cache[i].point == point)
		{
			list=cache[i].list;
			return true;
		}
				
	}

	return false;
}

void AddCache(std::vector<index_list_cache> &cache, vector3d point, std::vector<unsigned int> &list, int &size)
{
	if (unsigned(size) >= cache.size())
	{
		cache.resize(cache.size()*2);
	}
	cache[size].point = point;
	cache[size].list=list;
	size++;
}

bool InArray(std::string needle, std::vector<std::string> haystack)
{
	for (unsigned int i = 0; i < haystack.size(); i++)
	{
		if (needle == haystack[i])
			return true;
	}
	return false;
}

std::vector<unsigned int> GroupPolymodels(COB& cob, int groupID)
{
	std::vector<unsigned int> polymodels;
	for (unsigned int i = 0; i < (unsigned)cob.PolyModel_Count(); i++)
	{
		if (cob.GetPolymodel(i).head.ParentID == cob.GetGroup(groupID).head.ChunkID)
		{
			polymodels.resize(polymodels.size()+1);
			polymodels[polymodels.size()-1]=i;
		}
	}

	return polymodels;
}

std::vector<unsigned int> FindContainingPolygons(pcs_sobj &obj, vector3d &point, int exclude)
{
	std::vector<unsigned int> containers;
	for (unsigned int i = 0; i < obj.polygons.size(); i++)
	{
		if (i == exclude)
			continue; // skip
		for (unsigned int j = 0; j < obj.polygons[i].verts.size(); j++)
		{
			if (obj.polygons[i].verts[j].point == point)
			{
				containers.resize(containers.size()+1);
				containers[containers.size()-1]=i;
			}
		}
	}
	return containers;
}
//****************************************************************************
// Caligari Object Binary (COB) Loader
//****************************************************************************

// 0 on success
// -1 on NO GROUPS
// -2 Shield mesh is not triangluated
// -3 Insig mesh is not triangulated
// -4 Polygon > 20 points
// -5 Geometry group contains no geometry (COB_PolH)
// -6 No LODS (debris, shield, insig, other types of geometry that get culled from the list)
int PCS_Model::LoadFromCOB(std::string filename, AsyncProgress* progress, float scaler, bool useFilter)
{
	unsigned int i, j, k, l, m, MatNum;
	char CStringTemp[256];
	std::string strtmp, strtmp2, strtmp3;
	std::vector<unsigned int> temp_index_list;
	
	
	progress->setMessage("Clearing active model");
	progress->Notify();
	this->Reset(); // clear the model
	
	progress->setMessage("Loading COB");
	progress->Notify();
	COB InputFile;
	InputFile.LoadCOB(filename);

	if (InputFile.Group_Count() < 1)
		return -1;

	progress->setTarget(InputFile.Group_Count()*4+8);
	progress->incrementProgress();

	// ---------------- Shields ---------------
	
	progress->setMessage("Processing Shields");
	progress->Notify();
	pcs_shield_triangle shldtri;
	int ShldIndex = -1;
	vector3d temp_vertex;
	COB_PolH *pmtemp;

	for (i = 0; i < (unsigned)InputFile.Group_Count(); i++)
	{
		// find shield group
		strtmp = COBStringtoAPS(InputFile.GetGroup(i).name.name);
		if (!_strnicmp(strtmp.c_str(), "shield", 6))
		{
			ShldIndex = i;
		}
	}
	
	if (ShldIndex != -1)
	{
		temp_index_list = GroupPolymodels(InputFile, ShldIndex);
		for (i = 0; i < temp_index_list.size(); i++)
		{
			pmtemp = &InputFile.GetPolymodel(temp_index_list[i]);
			for (j = 0; j < (unsigned)pmtemp->num_faces_or_holes; j++)
			{
				/*
				if(pmtemp->fhs[j].num_verts > 3)
					return -2; // error.. shield face with more than 3 corners
					*/
				//I think we can handel this better
				for(int v = 0; v<pmtemp->fhs[j].num_verts-2; v++ ){
					//triangulate the source into the dest

					temp_vertex = pmtemp->verts[pmtemp->fhs[j].verts[0].local_vert];
					shldtri.corners[0] = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);
				
					temp_vertex = pmtemp->verts[pmtemp->fhs[j].verts[v+1].local_vert];
					shldtri.corners[1]  = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);
				
					temp_vertex = pmtemp->verts[pmtemp->fhs[j].verts[v+2].local_vert];
					shldtri.corners[2]  = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);
				
					shldtri.face_normal = MakeUnitVector(FigureNormal(shldtri.corners[0],
						shldtri.corners[1],
						shldtri.corners[2]));
					this->AddShldTri(&shldtri);
				}
			}
		}

		for (i = 0; i < (unsigned)InputFile.Material_Count(); i++)
		{
			if (InputFile.GetMat1(i).head.ParentID == InputFile.GetGroup(ShldIndex).head.ChunkID)
			{
				InputFile.Del_Mat1(i);
				i--;
			}
		}
		InputFile.Del_Grou(ShldIndex);
	}
	progress->incrementWithMessage("Processing Insignia");

	// ------------ Insignia ------------
	int lodnum=0, offset;
	pcs_insig insig;

	for (i = 0; i < (unsigned)InputFile.Group_Count(); i++)
	{
		// find shield group
		
		strtmp = COBStringtoAPS(InputFile.GetGroup(i).name.name);
		if (!_strnicmp(strtmp.c_str(), "InsgLOD", 7)) //"InsgLOD[UU]-[NN]"
		{
			sscanf(strtmp.c_str(), "InsgLOD%d-%u", &lodnum, &j);
		
			temp_index_list = GroupPolymodels(InputFile, ShldIndex);
			
			insig.faces.resize(0);
			insig.offset = COBtoStandard(InputFile.GetGroup(i).local_axis.center);
			insig.offset = insig.offset * scaler;
			for (j = 0; j < temp_index_list.size(); j++)
			{
				pmtemp = &InputFile.GetPolymodel(temp_index_list[j]);
				
				insig.lod = lodnum;
				offset = insig.faces.size();
				insig.faces.resize(offset + pmtemp->num_faces_or_holes);
				for (k = 0; k < (unsigned)pmtemp->num_faces_or_holes; k++)
				{
					if(pmtemp->fhs[k].num_verts > 3)
						return -3; // error.. insig face with more than 3 corners

					MatNum = InputFile.FindMatchingMatl(pmtemp->head.ChunkID, pmtemp->fhs[j].Material_index);

					// Vertex One --------------------------------
					temp_vertex = pmtemp->verts[pmtemp->fhs[k].verts[0].local_vert];
					insig.faces[k+offset].verts[0] = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);
					
					insig.faces[k+offset].u[0] = ((pmtemp->uv_verts[pmtemp->fhs[k].verts[0].uv_vert].u)
										* InputFile.GetMat1(MatNum).U_reps) - InputFile.GetMat1(MatNum).U_offset;
					insig.faces[k+offset].v[0] = ((1-pmtemp->uv_verts[pmtemp->fhs[k].verts[0].uv_vert].v)
										* InputFile.GetMat1(MatNum).V_reps) + InputFile.GetMat1(MatNum).V_offset;

					// Vertex Two --------------------------------
					temp_vertex = pmtemp->verts[pmtemp->fhs[j].verts[1].local_vert];
					insig.faces[k+offset].verts[1] = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);
					
					insig.faces[k+offset].u[1] = ((pmtemp->uv_verts[pmtemp->fhs[k].verts[1].uv_vert].u)
										* InputFile.GetMat1(MatNum).U_reps) - InputFile.GetMat1(MatNum).U_offset;
					insig.faces[k+offset].v[1] = ((1-pmtemp->uv_verts[pmtemp->fhs[k].verts[1].uv_vert].v)
										* InputFile.GetMat1(MatNum).V_reps) + InputFile.GetMat1(MatNum).V_offset;

					// Vertex Three --------------------------------
					temp_vertex = pmtemp->verts[pmtemp->fhs[j].verts[2].local_vert];
					insig.faces[k+offset].verts[0] = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler);

					insig.faces[k+offset].u[2] = ((pmtemp->uv_verts[pmtemp->fhs[k].verts[2].uv_vert].u)
										* InputFile.GetMat1(MatNum).U_reps) - InputFile.GetMat1(MatNum).U_offset;
					insig.faces[k+offset].u[2] = ((1-pmtemp->uv_verts[pmtemp->fhs[k].verts[2].uv_vert].v)
										* InputFile.GetMat1(MatNum).V_reps) + InputFile.GetMat1(MatNum).V_offset;

				}
				this->AddInsignia(&insig);
			}
			for (j = 0; j < (unsigned)InputFile.Material_Count(); j++)
			{
				if (InputFile.GetMat1(j).head.ParentID == InputFile.GetGroup(i).head.ChunkID)
				{
					InputFile.Del_Mat1(j);
					j--;
				}
			}
			InputFile.Del_Grou(i);
		}
	}
	progress->incrementWithMessage("Processing Textures");

	// Load in texture list
	for (i = 0; i < (unsigned)InputFile.Material_Count(); i++)
	{
		strtmp = StripFileName(COBStringtoAPS(InputFile.GetMat1(i).TexMap_Fname));

		if (InArray(strtmp, this->textures) == false)
		{
			this->AddTextures(strtmp);
		}
	}
	progress->incrementWithMessage("Processing Objects");
	
	progress->setTarget(InputFile.Group_Count()*4+8); // realign this - some groups may have been removed

	// -------------------- Objects ------------------
	
	
	// check this again
	if (InputFile.Group_Count() < 1)
		return -1;

	COB_Grou *temp_group;
	float u, v;
	std::vector<vector3d> avgnorms;

	int LodCount = -1, TurretCount = 0;
	for (i = 0; i < (unsigned)InputFile.Group_Count(); i++)
	{
		pcs_sobj tempobj;
		pcs_polygon temp_polygon;

		sprintf(CStringTemp, "Processing Objects (%d/%d) [Typing]", i, InputFile.Group_Count());
		progress->incrementWithMessage(CStringTemp);
		SleepFunc(10); // 10 ms sleep for drawing

		temp_group = &InputFile.GetGroup(i);
		tempobj.parent_sobj = InputFile.FindParentGrou(temp_group->head.ParentID);
		tempobj.name = COBStringtoAPS(temp_group->name.name);
		tempobj.movement_type = 0;
		tempobj.movement_axis = -1;
		tempobj.properties = "";

		// check if it's a LOD level, debris, etc
		if (!_strnicmp(tempobj.name.c_str(), "debris", 6))
			// Debris
		{
			tempobj.parent_sobj = -1;
			this->AddDebris(i);
		}
		else if ((!_strnicmp(tempobj.name.c_str(), "detail", 6)) || tempobj.parent_sobj == -1)
			// Detail
		{
			//the way i do this makes it so detail levels MUST BE IN ORDER
			tempobj.parent_sobj = -1;
			//this is a detail level, put the reference in the header
			this->AddLOD(i);
			LodCount++;
		}

		// check for detail tag light
		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{
			if(!_stricmp(COBStringtoAPS(InputFile.GetLight(j).name.name).c_str(), "detail") && InputFile.GetLight(j).head.ParentID == temp_group->head.ChunkID)
			{
				tempobj.parent_sobj = -1;
				//this is a detail level, put the reference in the header
				this->AddLOD(i);
			}
		}

		strtmp = _strlwr((char*)tempobj.name.c_str());
		if (strstr(strtmp.c_str(), "turret") != NULL && LodCount == 0 ) 
		{ 	
			// ok it's a turret, is it a rotating turret [base/arm]?
			if (strstr(strtmp.c_str(), "base") == NULL &&
				strstr(strtmp.c_str(), "arm") == NULL &&
				strstr(strtmp.c_str(), "barrel") == NULL)
			{	// non rotating
				tempobj.properties = "$special=subsystem\r\n$fov=180\r\n$name=GunTurret\r\n";
			}

			if (strstr(strtmp.c_str(), "base") != NULL)
			{	// rotate base
				tempobj.properties = "$special=subsystem\r\n$fov=180\r\n$name=GunTurret\r\n";
				tempobj.movement_type = 1; //rotate on axis
				tempobj.movement_axis = 0; // x axis
			}

			if (strstr(strtmp.c_str(), "arm") != NULL ||
				strstr(strtmp.c_str(), "barrel") != NULL)
			{	// rotate arms
				tempobj.movement_type = 1; //rotate on axis
				tempobj.movement_axis = 2; // y axis
			}
			TurretCount++;
		}

		tempobj.geometric_center = COBtoStandard(temp_group->local_axis.center);
		tempobj.geometric_center = tempobj.geometric_center * scaler;

		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{	// check for a light definine the center of this object
			strtmp = COBStringtoAPS(InputFile.GetLight(j).name.name);
			
			if (strtmp == std::string("center_") + tempobj.name ||
				strtmp == std::string("c_") + tempobj.name )
			{
				tempobj.geometric_center = COBtoStandard(InputFile.GetLight(j).local_axis.center);
				tempobj.geometric_center = tempobj.geometric_center * scaler;
			}
		}
		// i need the geometric center determined first
		if (!_strnicmp(tempobj.name.c_str(), "debris", 6))
		{ //this is because of the funky axis stuff setup in TS
			tempobj.offset = tempobj.geometric_center;
		}
		else if (tempobj.parent_sobj != -1)
		{
			tempobj.offset = tempobj.geometric_center - subobjects[tempobj.parent_sobj].geometric_center;
		}
		else
		{
			tempobj.offset = MakeVector(0.0, 0.0, 0.0);
		}
		tempobj.bounding_box_max_point = tempobj.bounding_box_min_point = MakeVector(0.0, 0.0, 0.0);

		
		sprintf(CStringTemp, "Processing Objects (%d/%d) [Polygons]", i, InputFile.Group_Count());
		progress->incrementWithMessage(CStringTemp);
		SleepFunc(10); // 10 ms sleep for drawing

		temp_index_list = GroupPolymodels(InputFile, i); 
		if (temp_index_list.size() < 1)
			return -5;
		for (j = 0; j < temp_index_list.size(); j++)
		{
				pmtemp = &InputFile.GetPolymodel(temp_index_list[j]);
				offset = tempobj.polygons.size();
				tempobj.polygons.resize(offset + pmtemp->num_faces_or_holes);
				for (k = 0; k < (unsigned)pmtemp->num_faces_or_holes; k++)
				{
				//	if (pmtemp->fhs[k].num_verts > 20)
				//		return -4; // error, verts > 20
										
					MatNum = InputFile.FindMatchingMatl(pmtemp->head.ChunkID, pmtemp->fhs[k].Material_index);
					if ((pmtemp->fhs[k].flags  != F_HOLE || 
						 pmtemp->fhs[k].flags  != (F_HOLE | F_BACKCULL)))
					{

						if (MatNum != -1)
						{
							tempobj.polygons[offset+k].texture_id = FindTexture(StripFileName(COBStringtoAPS(InputFile.GetMat1(MatNum).TexMap_Fname)));
						}
						else
							tempobj.polygons[offset+k].texture_id = -1;

						tempobj.polygons[offset+k].verts.resize(pmtemp->fhs[k].num_verts);
			

						for (l = 0; l < tempobj.polygons[offset+k].verts.size(); l++)
						{
														
							temp_vertex = pmtemp->verts[pmtemp->fhs[k].verts[l].local_vert];
							tempobj.polygons[offset+k].verts[l].point = TranslateToWorld(temp_vertex, pmtemp->CurPos, pmtemp->local_axis, scaler)-tempobj.geometric_center;
							

							if (tempobj.polygons[offset+k].texture_id != -1)
							{
								u = pmtemp->uv_verts[pmtemp->fhs[k].verts[l].uv_vert].u;
								v = 1-pmtemp->uv_verts[pmtemp->fhs[k].verts[l].uv_vert].v;

								u *= InputFile.GetMat1(MatNum).U_reps;
								v *= InputFile.GetMat1(MatNum).V_reps;

								u -= InputFile.GetMat1(MatNum).U_offset;
								v += InputFile.GetMat1(MatNum).V_offset;
								
								tempobj.polygons[offset+k].verts[l].u = u;
								tempobj.polygons[offset+k].verts[l].v = v;
							}
							else
							{
								tempobj.polygons[offset+k].verts[l].u = 0.;
								tempobj.polygons[offset+k].verts[l].v = 0.;
								tempobj.polygons[offset+k].verts[l].facet_angle = -1;
							}

						}
						tempobj.polygons[offset+k].norm = MakeUnitVector(FigureNormal(tempobj.polygons[offset+k].verts[0].point,
							tempobj.polygons[offset+k].verts[1].point, tempobj.polygons[offset+k].verts[2].point));
						
						for (l = 0; l < tempobj.polygons[offset+k].verts.size() && MatNum != -1; l++)
						{
							// setup faceting stuff - need to go back and smooth later
							tempobj.polygons[offset+k].verts[l].norm = tempobj.polygons[offset+k].norm;
							switch (InputFile.GetMat1(MatNum).facet_type)
							{
								case 'f': // faceted
									tempobj.polygons[offset+k].verts[l].facet_angle = -1;
									break;
								case 'a': //autofacet
									tempobj.polygons[offset+k].verts[l].facet_angle = InputFile.GetMat1(MatNum).autoface_angle;
									break;

								case 's': //smooth
								default:
									tempobj.polygons[offset+k].verts[l].facet_angle = 360; // all angles
									break;
							}
							
						}

					} // Is a face
				} // Faces or Holes loop -k
		} // Polymodel List loop - j

		if (useFilter)
			PCS_Model::filter_geometry(tempobj.polygons);

		
		std::vector<index_list_cache> cache(1000);
		int csize = 0;
		sprintf(CStringTemp, "Processing Objects (%d/%d) [Smoothing/Bounding]", i, InputFile.Group_Count());
		progress->incrementWithMessage(CStringTemp);
		SleepFunc(10); // 10 ms sleep for drawing

		for (j = 0; j < tempobj.polygons.size(); j++)
		{
			for (k = 0; k < tempobj.polygons[j].verts.size(); k++)
			{
				if (tempobj.polygons[j].verts[k].facet_angle != -1)
				{
					if (!CheckListcache(cache, tempobj.polygons[j].verts[k].point, temp_index_list, csize))
					{
						temp_index_list = FindContainingPolygons(tempobj, tempobj.polygons[j].verts[k].point, -1);
						AddCache(cache, tempobj.polygons[j].verts[k].point, temp_index_list, csize);
					}
					avgnorms.resize(100);
					m = 0;
					for (l = 0; l < temp_index_list.size(); l++)
					{
						if (temp_index_list[l] != j)
						{
							u = Angle(tempobj.polygons[j].norm, 
								tempobj.polygons[temp_index_list[l]].norm);
							u /= float(3.1415);
							u *= 180;
							if (u <= tempobj.polygons[j].verts[k].facet_angle)
							{
								if (m >= avgnorms.size())
									avgnorms.resize(avgnorms.size()*2);
								avgnorms[m] = tempobj.polygons[temp_index_list[l]].norm;
								m++;
							}
						}
					}
					avgnorms.resize(m);
					temp_vertex=tempobj.polygons[j].norm;
					for (l = 0; l < avgnorms.size(); l++)
						temp_vertex += avgnorms[l];
					tempobj.polygons[j].verts[k].norm = temp_vertex / float(avgnorms.size() + 1);
				}

				ExpandBoundingBoxes(tempobj.bounding_box_max_point, tempobj.bounding_box_min_point, tempobj.polygons[j].verts[k].point);

			} // tempobj.polygons[j].verts - k

		} // tempobj.polygons - j

		// Set Object Radius
		tempobj.radius = FindObjectRadius(tempobj.bounding_box_max_point, tempobj.bounding_box_min_point, vector3d(0,0,0));

		sprintf(CStringTemp, "Processing Objects (%d/%d) [Adding]", i, InputFile.Group_Count());
		progress->incrementWithMessage(CStringTemp);
		SleepFunc(10); // 10 ms sleep for drawing
		// add object
		this->AddSOBJ(&tempobj);

	} // Group Loop - i

	if (GetLODCount() == 0)
		return -6;

	// we loaded from a COB - we have smoothing data
	has_fullsmoothing_data = true;

	progress->setMessage("Generating Header data");
	progress->Notify();
	// mass center is the geometric center of the parent object of lod0
	header.mass_center = SOBJ(LOD(0)).geometric_center;
	header.min_bounding = header.max_bounding = MakeVector(0.,0.,0.);

	for (i = 0; i < (unsigned)GetSOBJCount(); i++)
	{
		ExpandBoundingBoxes(header.max_bounding, header.min_bounding, SOBJ(0).bounding_box_max_point+header.mass_center);
		ExpandBoundingBoxes(header.max_bounding, header.min_bounding, SOBJ(0).bounding_box_min_point+header.mass_center);
	}
	header.max_radius = FindObjectRadius(header.max_bounding, header.min_bounding, header.mass_center);
	
	//simple volume mass
	temp_vertex = header.max_bounding-header.min_bounding;
	header.mass = Abs(temp_vertex.x * temp_vertex.y * temp_vertex.z);

	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	// Unleash the hounds! i mean.. the autogen code :D

	// ----------- Turret Gen code -----------

	progress->incrementWithMessage("Processing Turrets");
	k = 0; // using J as our running turret count
	for (i = 0; i < (unsigned)GetSOBJCount(); i++)
		// find the highest numbered turret
	{
		strtmp = "";
		if (strstr(subobjects[i].name.c_str(), "turret") != NULL)
		{
			for (j = 6; j < subobjects[i].name.length(); j++)
			{
				if (subobjects[i].name[j] == '1' || subobjects[i].name[j] == '2' || subobjects[i].name[j] == '3' || subobjects[i].name[j] == '4' ||
					subobjects[i].name[j] == '5' || subobjects[i].name[j] == '6' || subobjects[i].name[j] == '7' || subobjects[i].name[j] == '8' ||
					subobjects[i].name[j] == '9' || subobjects[i].name[j] == '0')
				{
					strtmp += subobjects[i].name[j];
				}
				else
					break;
			}
			l = atoi(strtmp.c_str());
			if (l > k)
				k = l;
		}
	}

		// ok add the turrets
	int ParObj, ParObjPhys;
	pcs_turret temp_turret;
	//turrets.resize(k);
	for (i = 0; i < k; i++)
	{
		ParObj = ParObjPhys = -1;
		strtmp = "turret";
		sprintf(CStringTemp, "%02d", (i+1));
		strtmp += CStringTemp;
		// base, arms
		for (j = 0; j < subobjects.size(); j++)
		{
			strtmp2 =  _strlwr((char *) subobjects[j].name.c_str());
			if (strstr(strtmp2.c_str(), strtmp.c_str()) != NULL)
			{
				// determine if it's just a non-rotation or a rotation
				if (strstr(strtmp2.c_str(), "base") == NULL &&
					strstr(strtmp2.c_str(), "arms") == NULL &&
					strstr(strtmp2.c_str(), "barrels") == NULL)
				{	// non rotating
					ParObj = ParObjPhys = j;
				}

				if (strstr(strtmp2.c_str(), "base") != NULL)
				{	// rotate base
					if (ParObj == -1) //it should find the LOD0 one first, so this keeps it from using a lower lod counterpart
						ParObj = j;
				}

				if (strstr(strtmp2.c_str(), "arms") != NULL ||
					strstr(strtmp2.c_str(), "barrels") != NULL)
				{	// rotate arms
					if (ParObjPhys == -1) //it should find the LOD0 one first, so this keeps it from using a lower lod counterpart
						ParObjPhys = j;
				}
			}
			if (ParObj != -1 && ParObjPhys != -1)
				break; //short curcuit when it's found
		}
		
		temp_turret.sobj_par_phys = ParObjPhys;
		temp_turret.sobj_parent = ParObj;
		AddTurret(&temp_turret);
	}

	// now add the firepoints - from lights
	for (i = 0; i < turrets.size(); i++)
	{
		strtmp = "turret";
		sprintf(CStringTemp, "%02d-Fp", (i+1));
		strtmp += CStringTemp;

		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{

			if(!_strnicmp(COBStringtoAPS(InputFile.GetLight(j).name.name).c_str(), 
				strtmp.c_str(), strtmp.length()))
			{
				
				temp_vertex = COBtoStandard(InputFile.GetLight(j).local_axis.center);
				temp_vertex = temp_vertex * scaler;
				temp_vertex = temp_vertex - subobjects[turrets[i].sobj_parent].geometric_center;
				turrets[i].fire_points.resize(turrets[i].fire_points.size()+1);
				turrets[i].fire_points[turrets[i].fire_points.size()-1]=temp_vertex;
			}
		}
	}

	// figure the normals - we do this by finding the average displacement of the firepoints form the center
	// it will fail to assign a normal under the following conditions
	// 1) There are no firepoints
	// 2) The firepoint[s] average out to (0, 0, 0)
	for (i = 0; i < turrets.size(); i++)
	{
		if (turrets[i].fire_points.size() > 0)
		{
			
			//PCinst.TGUN_Get_Bank(i, ParObj, ParObjPhys, TempVect); 
			temp_vertex = MakeVector(0.0,0.0,0.0);
	
			for (j = 0; j < turrets[i].fire_points.size(); j++)
			{
				temp_vertex += turrets[i].fire_points[j];
			}
			turrets[i].turret_normal = MakeUnitVector(temp_vertex / float(turrets[i].fire_points.size()));
		}
	}
	progress->incrementWithMessage("Processing Engines");

	// ----------------------- Engine Points -----------------------
		// ok time to do engine glow points
	k = 0;
	for (i = 0; i < (unsigned)InputFile.Light_Count(); i++)
	// find the highest numbered thurster set
	{
		strtmp = COBStringtoAPS(InputFile.GetLight(i).name.name);
		strtmp2 = "";
		if (!_strnicmp(strtmp.c_str(), "EngGlow", 7))
		{
			for (j = 7; j < strtmp.length(); j++)
			{
				if (strtmp[j] == '1' || strtmp[j] == '2' || strtmp[j] == '3' || strtmp[j] == '4' ||
					strtmp[j] == '5' || strtmp[j] == '6' || strtmp[j] == '7' || strtmp[j] == '8' ||
					strtmp[j] == '9' || strtmp[j] == '0')
				{
					strtmp2 += strtmp[j];
				}
				else
					break;
			}
			l = atoi(strtmp2.c_str());
			if (l > k)
				k = l;
		}
	}
	thrusters.resize(k);

	// ok got the highest numbered set let's make them
	for (i = 0; i < thrusters.size(); i++)
	{
		sprintf(CStringTemp, "$engine_subsystem=$engine%02d", (i+1));
		thrusters[i].properties = CStringTemp;
		
		sprintf(CStringTemp, "EngGlow%02d-", (i+1));
		strtmp = CStringTemp;
		
		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{
			if (!_strnicmp(COBStringtoAPS(InputFile.GetLight(j).name.name).c_str(), 
				strtmp.c_str(), strtmp.length()))
			{
				
				temp_vertex = COBtoStandard(InputFile.GetLight(j).local_axis.center);
				temp_vertex = temp_vertex * scaler;
				thrusters[i].points.resize(thrusters[i].points.size()+1);
				// default Radius of 20.0 and unit vector in the rear direction default asignments
				thrusters[i].points[thrusters[i].points.size()-1] = pcs_thrust_glow(temp_vertex, MakeVector(0.0, 0.0, -1.0), 20.0);
			}
		}
	}

	progress->incrementWithMessage("Processing Special");
	// ------------- Special Points ----------------
	pcs_special temp_spec;
	for (i = 0; i< (unsigned)InputFile.Light_Count(); i++)
	{
		strtmp = COBStringtoAPS(InputFile.GetLight(i).name.name);
		if (!_strnicmp(strtmp.c_str(), "SPCL=", 5))
		{
			strtmp2 = strtmp.substr(5);
			strtmp2 = std::string("$") + strtmp2;

			temp_vertex = COBtoStandard(InputFile.GetLight(i).local_axis.center);
			temp_vertex = temp_vertex * scaler;

			temp_spec.name = strtmp2;
			temp_spec.point = temp_vertex;
			temp_spec.properties = "$special=subsystem";
			temp_spec.radius = 50.0; //default radius of 50.0

			AddSpecial(&temp_spec);
		}
	}

	progress->incrementWithMessage("Processing Paths");
	// ============ auto path ============
	k = 0;
	for (i = 0; i < (unsigned)InputFile.Light_Count(); i++)
	// find the highest numbered thurster set
	{
		strtmp = COBStringtoAPS(InputFile.GetLight(i).name.name);
		strtmp = "";
		if (!_strnicmp(strtmp.c_str(), "Path", 4))
		{
			for (j = 4; j < strtmp.length(); j++)
			{
				if (strtmp[j] == '1' || strtmp[j] == '2' || strtmp[j] == '3' || strtmp[j] == '4' ||
					strtmp[j] == '5' || strtmp[j] == '6' || strtmp[j] == '7' || strtmp[j] == '8' ||
					strtmp[j] == '9' || strtmp[j] == '0')
				{
					strtmp2 += strtmp[j];
				}
				else
					break;
			}
			l = atoi(strtmp2.c_str());
			if (l > k)
				k = l;
		}
	}
	ai_paths.resize(k);

		// ok got the highest numbered set let's make them
	for (i = 0; i < ai_paths.size(); i++)
	{
		sprintf(CStringTemp, "$path%02d", (i+1));
		ai_paths[i].name=CStringTemp;

		sprintf(CStringTemp, "Path%02d-", (i+1));
		strtmp = CStringTemp;
	
		m = 0; // using M as high point
		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{ //gotta go all this funky crap and make it a three-pass algorithm because they have to stay ordered
			strtmp2 = COBStringtoAPS(InputFile.GetLight(j).name.name);
			if (!_strnicmp(strtmp2.c_str(), strtmp.c_str(), strtmp.length()) && !m)
			{
				strtmp3 = "";
				for (k = strtmp.length(); k < strtmp2.length(); k++)
				{
					if (strtmp2[k] == '1' || strtmp2[k] == '2' || strtmp2[k] == '3' || strtmp2[k] == '4' ||
						strtmp2[k] == '5' || strtmp2[k] == '6' || strtmp2[k] == '7' || strtmp2[k] == '8' ||
						strtmp2[k] == '9' || strtmp2[k] == '0')
					{
						strtmp3 += strtmp2[k];
					}
					else
						break;
				}

				l = atoi(strtmp3.c_str());
				if (l > m)
					m = l;
			}
		}

		for (j = 0; j < m; j++)
		{
			ai_paths[i].verts.resize(ai_paths[i].verts.size()+1);
			ai_paths[i].verts[ai_paths[i].verts.size()-1].pos = MakeVector(0.0, 0.0, 0.0);
			ai_paths[i].verts[ai_paths[i].verts.size()-1].radius = (float)pow((float)10.0, (float)j);
		}
	}

	// now to clean up their
	for (i = 0; i < ai_paths.size(); i++)
	{
	
		sprintf(CStringTemp, "Path%02d-%02d-", (i+1), (int)ai_paths[i].verts.size());
		strtmp = CStringTemp;
	
		for (j = 0; j < (unsigned)InputFile.Light_Count(); j++)
		{
			strtmp2 = COBStringtoAPS(InputFile.GetLight(j).name.name);
			strtmp3 = ""; 

			if (!_strnicmp(strtmp2.c_str(), strtmp.c_str(), strtmp.length()))
			{
				for (k = strtmp.length(); k < strtmp2.length(); k++)
				{
					if (strtmp2[k] == '1' || strtmp2[k] == '2' || strtmp2[k] == '3' || strtmp2[k] == '4' ||
						strtmp2[k] == '5' || strtmp2[k] == '6' || strtmp2[k] == '7' || strtmp2[k] == '8' ||
						strtmp2[k] == '9' || strtmp2[k] == '0')
					{
						strtmp3 += strtmp2[k];
					}
					else
						break;
				}

				temp_vertex = COBtoStandard(InputFile.GetLight(j).local_axis.center);
				temp_vertex = temp_vertex * scaler;

				l = atoi(strtmp3.c_str())-1;
				ai_paths[i].verts[l].pos = temp_vertex;
			}
		}
	}

	return 0;
}

//****************************************************************************
// Caligari Object Binary (COB) Saver
//****************************************************************************

void MakeStandardAxis(COB_Axis &local_Axis)
{
	local_Axis.XAxisDirection.y = local_Axis.XAxisDirection.z =
		local_Axis.YAxisDirection.x = local_Axis.YAxisDirection.z =
		local_Axis.ZAxisDirection.x = local_Axis.ZAxisDirection.y = 0;

	local_Axis.XAxisDirection.x = local_Axis.YAxisDirection.y = local_Axis.ZAxisDirection.z = 1;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

COB_PolH MakePoly(std::string name, int ParId, int ChunkID, vector3d Center, COB_Matrix CurPos)
// this doesn't complete make it - just sets it up
{

	COB_PolH poly;

	// head
	poly.head.ChunkID = ChunkID;
	poly.head.ParentID = ParId;
	poly.head.MajVer = 0;
	poly.head.MinVer = 4;
	poly.head.size = 0; // well come back later and fix this
	strncpy(poly.head.type, "PolH", 4);

	// curpos
	poly.CurPos = CurPos;

	// local axis
	MakeStandardAxis(poly.local_axis);
	poly.local_axis.center = StandardToCOB(Center);

	// name
	poly.name.DupCount = 0;
	poly.name.name = APStoCOBString(name);

	return poly;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

COB_Grou MakeGrou(std::string name, vector3d center, COB_Matrix CurPos, int ParId, int ChunkID)
{

	COB_Grou NewGrou;

	// Coordinate Translation
	NewGrou.CurPos = CurPos;

	//name
	NewGrou.name.DupCount = 0;
	NewGrou.name.name = APStoCOBString(name);

	// axis
	MakeStandardAxis(NewGrou.local_axis);
	NewGrou.local_axis.center = StandardToCOB(center);

	// head
	NewGrou.head.MajVer = 0;
	NewGrou.head.MinVer = 1;
	NewGrou.head.size = sizeof(COB_Axis) + sizeof(COB_Matrix) + 4 + NewGrou.name.name.length();
	strncpy(NewGrou.head.type, "Grou", 4);
	NewGrou.head.ChunkID = ChunkID;
	NewGrou.head.ParentID = ParId;

	return NewGrou;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

COB_Lght MakeLight(std::string name, int ParId, int ChunkID, vector3d pos, COB_Matrix CurPos)
{
	COB_Lght light;

	// head
	light.head.size = sizeof(COB_Lght) - 6;
	light.head.ChunkID = ChunkID;
	light.head.ParentID = ParId;
	strncpy(light.head.type, "Lght", 4);
	light.head.MajVer = 0;
	light.head.MinVer = 4;

	//Local Axis
	MakeStandardAxis(light.local_axis);
	light.local_axis.center = StandardToCOB(pos);
	
	//CurPos
	CurPos.RowOne[3] = light.local_axis.center.x;
	CurPos.RowTwo[3] = light.local_axis.center.y;
	CurPos.RowThree[3] = light.local_axis.center.z;
	light.CurPos = CurPos;

	// name
	light.name.DupCount = 0;
	light.name.name = APStoCOBString(name);

	// light data
	light.type = 2;
	light.shadows = 0;
	light.RGB.x = light.RGB.y = light.RGB.z = 0.01f;
	light.angle = 360;
	light.hotspot = 0;
	light.falloff = 0;
	
	return light;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

COB_Mat1 MakeMat1(std::string TexName, int ParId, int ChunkID, int Matnum, unsigned char autoface_angle = 179, std::string extension=".bmp")
{
	COB_Mat1 mat;

	//head
	strncpy(mat.head.type, "Mat1", 4);
	//mat.head.size = sizeof(COB_Mat1); later
	mat.head.ChunkID = ChunkID;
	mat.head.ParentID = ParId;
	mat.head.MajVer = 0;
	mat.head.MinVer = 5;

	// mat data

	mat.Mat_num = Matnum;
	mat.shader_type = 'p';
	mat.facet_type = 's';
	mat.autoface_angle = autoface_angle;
	mat.opacity = mat.R = mat.G = mat.B = float(1.0);
	mat.ambience = float(0.0);
	mat.specular = float(0.0);
	mat.hilight = float(0.0);
	mat.refraction_index = float(1);
	strncpy(mat.Ext_id, "t:", 2);
	mat.flags = 0x02;

	TexName += extension;
	mat.TexMap_Fname = APStoCOBString(TexName);
	mat.U_offset = 0;
	mat.V_offset = 0;
	mat.U_reps = 1;
	mat.V_reps = 1;

	mat.head.size = 58 + mat.TexMap_Fname.length();

	return mat;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int COB_Maybe_MakeMaterial(std::vector<COB_Mat1> &materials, std::string texname, int ParID, unsigned int &ChunkID, float facet_angle, std::string extension=".bmp")
{
	if (facet_angle > 179 || facet_angle < 0) // invalid
		facet_angle = 32; // default

	for (unsigned int i = 0; i < materials.size(); i++)
	{
		if ((texname+extension) == COBStringtoAPS(materials[i].TexMap_Fname) && 
			int(materials[i].autoface_angle) == int(facet_angle))
		{
			return i;
		}
	}

	// not found.. make one
	materials.resize(materials.size()+1);
	materials[materials.size()-1] = MakeMat1(texname, ParID, ChunkID++, materials.size()-1, (unsigned char)int(facet_angle), extension);
	return materials.size()-1;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::SaveToCOB(std::string filename, AsyncProgress* progress, float scaler)
{
	unsigned int i, j, k, l, m, n;
	unsigned int ChunkID = 1000, RootChunk = 0;
	char CStringTemp[256];
	std::string strtmp, strtmp2, strtmp3;
	std::vector<unsigned int> temp_index_list;
	std::vector<vector3d> points;

	std::vector<int> object_grou_chunkids;
	std::vector<int> object_pol_chunkids;
	std::vector<UV_Vert> UV_verts;
	std::vector<COB_Mat1> materials;

	ostringstream outstring;
		
	progress->setMessage("Generating COB/SCN");
	progress->Notify();
	COB OutFile;

	progress->setTarget(8 + this->subobjects.size());
	progress->incrementProgress();

	// --------- setup data structs -----------
	COB_Matrix def_matrix;
	memset((void*)&def_matrix, 0, sizeof(COB_Matrix));
	def_matrix.RowOne[0] = def_matrix.RowTwo[1] = def_matrix.RowThree[2] = 1.0f;

	matrix_4x4 pos_matrix;

	COB_PolH pol_temp;
	COB_Grou grou_temp;
	COB_Lght lght_temp;
	COB_Mat1 mat_temp;

	// ---------------- Shields ---------------
	
	progress->incrementWithMessage("Processing Shields");

	pcs_shield_triangle shldtri;
	int ShldIndex = -1;
	vector3d temp_vertex;

	i = this->LOD(0);
	grou_temp = MakeGrou(this->subobjects[i].name, this->subobjects[i].geometric_center/scaler, def_matrix, 0, 999);
	OutFile.Add_Grou(grou_temp);
	RootChunk = 999;


	if (this->shield_mesh.size() > 0)
	{		
		grou_temp = MakeGrou("shield", vector3d(0.0f, 0.0f, 0.0f), def_matrix, RootChunk, ChunkID++);
		pol_temp = MakePoly("shieldmesh", grou_temp.head.ChunkID, ChunkID++, vector3d(0.0f, 0.0f, 0.0f), def_matrix);
		lght_temp = MakeLight("shieldmesh_light", grou_temp.head.ChunkID, ChunkID++, vector3d(0.0f, 0.0f, 0.0f), def_matrix);
		mat_temp = MakeMat1("shield", pol_temp.head.ChunkID, ChunkID++, 0); 

		// build a unique points list for the shield mesh

		points.resize(this->shield_mesh.size()*3);
		vector3d shldtvect;

		// make points list
		l = 0;
		for (i = 0; i < this->shield_mesh.size(); i++)
		{
			for (j = 0; j < 3; j++)
			{
				bool found = false;
				for (unsigned int k = 0; k < points.size(); k++)
				{
					if (points[k] == this->shield_mesh[i].corners[j])
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					if (l >= points.size())
						points.resize(points.size()*2);
					points[l] = this->shield_mesh[i].corners[j];
					l++;
				}
			}
		}
		points.resize(l);

		pol_temp.num_verticies = l;
		pol_temp.verts = new vector3d[l];
	
		for (i = 0; i < (unsigned)pol_temp.num_verticies; i++)
		{
			pol_temp.verts[i] = StandardToCOB(points[i]);
			pol_temp.verts[i] = pol_temp.verts[i] / scaler;
		}

		pol_temp.num_faces_or_holes = this->shield_mesh.size();
		pol_temp.fhs = new PolH_Face_Hole[pol_temp.num_faces_or_holes];

		for (i = 0; i < (unsigned)pol_temp.num_faces_or_holes; i++)
		{
			pol_temp.fhs[i].flags = 0;
			pol_temp.fhs[i].num_verts = 3;
			pol_temp.fhs[i].Material_index = 0;
			pol_temp.fhs[i].verts = new Face_Vert[3];
			pol_temp.num_uv_verts = 1;

			for (j = 0; j < 3; j++)
			{
				pol_temp.fhs[i].verts[j].local_vert = FindInList(points, this->shield_mesh[i].corners[j]);
				pol_temp.fhs[i].verts[j].uv_vert = 0;
			}

			pol_temp.uv_verts = new UV_Vert[1];
			pol_temp.uv_verts[0].u = pol_temp.uv_verts[0].v = 0.0f;
		}

		pol_temp.head.size = pol_temp.GetSize();
		OutFile.Add_Grou(grou_temp);
		OutFile.Add_Lght(lght_temp);
		OutFile.Add_PolH(pol_temp);
		OutFile.Add_Mat1(mat_temp);

		points.resize(0);


	} // if (this->shield_mesh.size() > 0)

	progress->incrementWithMessage("Processing Insignia");

	// ------------- insignia ---------------

	for (i = 0; i < this->insignia.size(); i++)
	{
		outstring.clear();
		outstring << "InsgLOD" << this->insignia[i].lod << "-" << i; // InsigLOD[UU]-[NN] UU=lod#, NN=unused uniqueID
		strtmp = outstring.str().c_str();

		grou_temp = MakeGrou(strtmp, this->insignia[i].offset/scaler, def_matrix, RootChunk, ChunkID++);
		pol_temp = MakePoly(strtmp + "mesh", grou_temp.head.ChunkID, ChunkID++, this->insignia[i].offset/scaler, def_matrix);
		lght_temp = MakeLight(strtmp + "_light", grou_temp.head.ChunkID, ChunkID++, this->insignia[i].offset/scaler, def_matrix);
		mat_temp = MakeMat1("insignia", pol_temp.head.ChunkID, ChunkID++, 0); 

		points.resize(this->insignia[i].faces.size()*3);

		// make points list
		m = 0;
		for (j = 0; j < this->insignia[i].faces.size(); j++)
		{
			for (k = 0; k < 3; k++)
			{
				bool found = false;
				for (unsigned int l = 0; l < points.size(); l++)
				{
					if (points[l] == this->insignia[i].faces[j].verts[k])
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					if (m >= points.size())
						points.resize(points.size()*2);
					points[m] = this->insignia[i].faces[j].verts[k];
					m++;
				}
			}
		}
		points.resize(m);

		pol_temp.num_verticies = m;
		pol_temp.verts = new vector3d[m];
	
		for (j = 0; j < (unsigned)pol_temp.num_verticies; j++)
		{
			pol_temp.verts[j] = StandardToCOB(points[j]);
			pol_temp.verts[j] = pol_temp.verts[j] / scaler;
		}

		pol_temp.num_faces_or_holes = this->insignia[i].faces.size();
		pol_temp.fhs = new PolH_Face_Hole[pol_temp.num_faces_or_holes];

		// this shortcut only works for meshes we KNOW are triangulated
		pol_temp.num_uv_verts = pol_temp.num_faces_or_holes * 3;
		pol_temp.uv_verts = new UV_Vert[pol_temp.num_uv_verts];
		//UV_verts.resize(pol_temp.num_faces_or_holes * 3); // max bound

		for (j = 0; j < (unsigned)pol_temp.num_faces_or_holes; j++)
		{
			pol_temp.fhs[j].flags = 0;
			pol_temp.fhs[j].num_verts = 3;
			pol_temp.fhs[j].Material_index = 0;
			pol_temp.fhs[j].verts = new Face_Vert[3];
			

			for (k = 0; k < 3; k++)
			{
				pol_temp.fhs[j].verts[k].local_vert = FindInList(points, this->insignia[i].faces[j].verts[k]);
				pol_temp.fhs[j].verts[k].uv_vert = (j*3)+k;
				pol_temp.uv_verts[pol_temp.fhs[j].verts[k].uv_vert].u = this->insignia[i].faces[j].u[k];
				pol_temp.uv_verts[pol_temp.fhs[j].verts[k].uv_vert].v = 1-this->insignia[i].faces[j].v[k];
			}
		}

		pol_temp.head.size = pol_temp.GetSize();
		
		OutFile.Add_Grou(grou_temp);
		OutFile.Add_Lght(lght_temp);
		OutFile.Add_PolH(pol_temp);
		OutFile.Add_Mat1(mat_temp);

		points.resize(0);
	}

	
	progress->incrementWithMessage("Processing Objects");
	// ---------------------- Objects -----------------
	object_grou_chunkids.resize(this->subobjects.size());
	object_pol_chunkids.resize(this->subobjects.size());

	for (i = 0; i < this->subobjects.size(); i++)
	{
		sprintf(CStringTemp, "Processing Object [%02d/%02d]", (i+1), (int)this->subobjects.size());
		progress->incrementWithMessage(CStringTemp);
		if (i == this->LOD(0))
		{
			pol_temp = MakePoly(this->subobjects[i].name, RootChunk, ChunkID++, this->subobjects[i].geometric_center/scaler, def_matrix);
			lght_temp = MakeLight(std::string("center_") + this->subobjects[i].name, RootChunk, ChunkID++, this->subobjects[i].geometric_center/scaler, def_matrix);
			object_grou_chunkids[i] = RootChunk;
		}
		else
		{
			if (this->subobjects[i].parent_sobj != -1)
			{
				n = object_grou_chunkids[this->subobjects[i].parent_sobj];
			}
			else
			{
				n = RootChunk;
				lght_temp = MakeLight("detail", grou_temp.head.ChunkID, ChunkID++, this->subobjects[i].geometric_center/scaler, def_matrix);
				OutFile.Add_Lght(lght_temp);
			}
			grou_temp = MakeGrou(this->subobjects[i].name, this->subobjects[i].geometric_center/scaler, def_matrix, n, ChunkID++);
			pol_temp = MakePoly(this->subobjects[i].name, grou_temp.head.ChunkID, ChunkID++, this->subobjects[i].geometric_center/scaler, def_matrix);
			lght_temp = MakeLight(std::string("center_") + this->subobjects[i].name, grou_temp.head.ChunkID, ChunkID++, this->subobjects[i].geometric_center/scaler, def_matrix);
			object_grou_chunkids[i] = grou_temp.head.ChunkID;
		}
		object_pol_chunkids[i] = pol_temp.head.ChunkID;

		//mat_temp = MakeMat1("insignia", pol_temp.head.ChunkID, ChunkID++, 0); 
		// make points list
		points.resize(this->subobjects[i].polygons.size()*3); // a good guess
		m = 0;
		for (j = 0; j < this->subobjects[i].polygons.size(); j++)
		{
			for (k = 0; k < this->subobjects[i].polygons[j].verts.size(); k++)
			{
				bool found = false;
				for (unsigned int l = 0; l < points.size(); l++)
				{
					if (points[l] == this->subobjects[i].polygons[j].verts[k].point)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					if (m >= points.size())
						points.resize(points.size()*2);
					points[m] = this->subobjects[i].polygons[j].verts[k].point;
					m++;
				}
			}
		}
		points.resize(m);

		// make COB polygon chunk
		UV_verts.resize(0);
		materials.resize(0);

		pol_temp.num_verticies = m;
		pol_temp.verts = new vector3d[m];
	
		for (j = 0; j < (unsigned)pol_temp.num_verticies; j++)
		{
			pol_temp.verts[j] = StandardToCOB(points[j]+this->get_model_offset(i));
			pol_temp.verts[j] = pol_temp.verts[j] / scaler;
		}

		pol_temp.num_faces_or_holes = this->subobjects[i].polygons.size();
		pol_temp.fhs = new PolH_Face_Hole[pol_temp.num_faces_or_holes];

		pol_temp.num_uv_verts = 0;
		UV_verts.resize(points.size()); // a fairly decent guess
		for (j = 0; j < (unsigned)pol_temp.num_faces_or_holes; j++)
		{
			pol_temp.fhs[j].flags = 0;
			pol_temp.fhs[j].num_verts = this->subobjects[i].polygons[j].verts.size();
			pol_temp.fhs[j].verts = new Face_Vert[pol_temp.fhs[j].num_verts];
			

			for (k = 0; k < (unsigned)pol_temp.fhs[j].num_verts; k++)
			{
				pol_temp.fhs[j].verts[k].local_vert = FindInList(points, this->subobjects[i].polygons[j].verts[k].point);
				pol_temp.fhs[j].verts[k].uv_vert = pol_temp.num_uv_verts;

				if ((unsigned)pol_temp.num_uv_verts >= UV_verts.size())
					UV_verts.resize(UV_verts.size()*2);
				UV_verts[pol_temp.num_uv_verts].u = this->subobjects[i].polygons[j].verts[k].u;
				UV_verts[pol_temp.num_uv_verts].v = 1-this->subobjects[i].polygons[j].verts[k].v;
				pol_temp.num_uv_verts++;
			}

			// ok.. let's figure out our material index - we need #1 our texname, #2 our facet_angle (all verts in a face should have the same facet_angle
			if (this->subobjects[i].polygons[j].texture_id != -1)
			{
				pol_temp.fhs[j].Material_index = COB_Maybe_MakeMaterial(materials, this->textures[this->subobjects[i].polygons[j].texture_id], 
					pol_temp.head.ChunkID, ChunkID, this->subobjects[i].polygons[j].verts[0].facet_angle); 
			}
			else
			{
				pol_temp.fhs[j].Material_index = COB_Maybe_MakeMaterial(materials, "", 
					pol_temp.head.ChunkID, ChunkID, this->subobjects[i].polygons[j].verts[0].facet_angle); 
			}
		}
		pol_temp.uv_verts = new UV_Vert[pol_temp.num_uv_verts];
		for (j = 0; j < (unsigned)pol_temp.num_uv_verts; j++)
		{
			pol_temp.uv_verts[j] = UV_verts[j];
		}

		pol_temp.head.size = pol_temp.GetSize();
		
		if (i != this->LOD(0))
			OutFile.Add_Grou(grou_temp);

		OutFile.Add_Lght(lght_temp);
		OutFile.Add_PolH(pol_temp);

		for (j = 0; j < materials.size(); j++)
			OutFile.Add_Mat1(materials[j]);

		points.resize(0);
	} // for (i = 0; i < this->subobjects.size(); i++)

	
	progress->incrementWithMessage("Processing Turrets");
	// ----------- turret autogen data -----------------
	for (i = 0; i < this->turrets.size(); i++)
	{
		for (j = 0; j < this->turrets[i].fire_points.size(); j++)
		{
			sprintf(CStringTemp, "Turret%02d-Fp%02d", (i+1), j);
			vector3d tmp_vector(this->get_model_offset(this->turrets[i].sobj_par_phys)+this->turrets[i].fire_points[j]);
			lght_temp = MakeLight(CStringTemp, object_grou_chunkids[this->turrets[i].sobj_par_phys], 
						ChunkID++, (tmp_vector)/scaler, def_matrix);
			OutFile.Add_Lght(lght_temp);
		}
		
	}

	progress->incrementWithMessage("Processing Engines");
	// thurusters autogen data
	for (i = 0; i < this->thrusters.size(); i++)
	{
		for (j = 0; j < this->thrusters[i].points.size(); j++)
		{
			sprintf(CStringTemp, "EngGlow%02d-%02d", (i+1), j);
			lght_temp = MakeLight(CStringTemp, object_grou_chunkids[this->LOD(0)], 
				ChunkID++, thrusters[i].points[j].pos/scaler, def_matrix);
			OutFile.Add_Lght(lght_temp);
		}
	}



	progress->incrementWithMessage("Processing Special");
	// special autogen data
	for (i = 0; i < this->special.size(); i++)
	{
		sprintf(CStringTemp, "SPCL=%s", this->special[i].name.substr(1).c_str());
		lght_temp = MakeLight(CStringTemp, object_grou_chunkids[this->LOD(0)], 
					ChunkID++, this->special[i].point/scaler, def_matrix);
		OutFile.Add_Lght(lght_temp);

	}

	progress->incrementWithMessage("Processing Paths");
	// path autogen data
	for (i = 0; i < this->ai_paths.size(); i++)
	{
		for (j = 0; j < this->ai_paths[i].verts.size(); j++)
		{
			sprintf(CStringTemp, "Path%02d-%02d-%02d", (i+1), (int)this->ai_paths[i].verts.size(), (j+1));

		lght_temp = MakeLight(CStringTemp, object_grou_chunkids[this->LOD(0)], 
					ChunkID++, this->ai_paths[i].verts[j].pos/scaler, def_matrix);

		OutFile.Add_Lght(lght_temp);
		}
	}

	// ---------- do the actual save -----------
	progress->incrementWithMessage("Saving to COB");
	std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);

	if (OutFile.SaveCOB(out))
		return 1;
	OutFile.Reset();

	return 0;
}
