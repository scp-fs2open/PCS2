//****************************************************************************
//	pcs_pmf_pof.cpp
//	POF Constructor Suite 2 File Format: Load/Save function to POF
//****************************************************************************

/*
 * $Logfile: /pcs_pmf_pof.cpp $
 * $Revision: 1.57 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: pcs_pmf_pof.cpp,v $
 * Revision 1.57  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.56  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.55  2008/11/09 21:41:12  kazan
 * Pass By Value => Pass By Reference
 *
 * Revision 1.54  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.53  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.52  2008/02/11 17:40:03  kazan
 * Revoking BSPGEN's BSP Caching
 *
 * Revision 1.51  2008/02/11 13:18:23  kazan
 * Possible fix for Mantis #59
 *
 * Revision 1.50  2008/02/09 23:01:46  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.49  2008/02/09 22:33:01  kazan
 * bye bye kaz_vector
 *
 * Revision 1.48  2008/02/09 22:31:27  kazan
 * finish removing string
 *
 * Revision 1.47  2008/01/31 17:02:49  kazan
 * disable debug code
 *
 * Revision 1.46  2008/01/31 17:02:24  kazan
 * Add error checking to BSP tree packing, this will slow packing somewhat, but will prevent crashes.
 * Correct error in calculating tree size.
 *
 * Revision 1.45  2008/01/30 21:05:25  kazan
 * Fix cosmetic bug displaying bbox'es
 *
 * Revision 1.44  2008/01/30 20:50:06  kazan
 * Fix the damn BBox bug, finally!
 * Fixed a small bug where it would cache when it shouldn't
 * Increment compiler version
 *
 * Revision 1.43  2008/01/18 18:39:27  kazan
 * Was finally able to replicate the bounding box inversion - apparently HDR2 bbox's are in openGL coordinate space NOT BSP coordinate Space
 * add support to .cob specifications for "detail" light - a group with a child light named "detail" will be added as a LOD (this allows .cob's written out by PCS2 to be IMMEDIATELY reloadable and have the hierarchy right!)
 *
 * Revision 1.42  2008/01/14 01:40:10  kazan
 * wow.. i'm a retard
 *
 * Revision 1.41  2008/01/14 01:13:30  kazan
 * Fix memory leak in POF->PMF code
 * Revoke PCS1.x recognition for BSP caching
 * Update BSP compile info on partial recompile
 *
 * Revision 1.40  2008/01/12 18:02:24  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.39  2008/01/06 21:44:22  kazan
 * Update PMF with new bounding boxes upon POF Compile
 * Update PCS2_COMP_VERSION to cause previously miscompiled (incorrect swapped boudning boxes) to be properly recompiled.
 *
 * Revision 1.38  2007/10/22 23:43:55  kazan
 * Catch a new error condition (thanks for TrashMan for being an airhead), fix crash bug introduced by my replacing POFstring with std::string (thanks to me for being an airhead)
 *
 * Revision 1.37  2007/10/18 15:54:09  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.36  2007/10/18 14:54:02  kazan
 * Fix accuracy problem with BSP depth counter, error BSP_COMPILATION_ERROR flag, reenable infinite recursion catch due to duplicate-polygon-centers still causing infinite recursion
 *
 * Revision 1.35  2007/07/16 06:26:55  kazan
 * generally you shouldn't try to generate an SLDC without a shield
 *
 * Revision 1.34  2007/07/13 18:22:21  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.33  2007/07/11 19:14:48  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.32  2007/07/09 00:07:13  bobboau
 * sence neither I nor Kazan seems to have a solid understanding on how to use branching, I've decided to commit the experimental BSP *within _BOB_BSP_ ifdefs* to the main branch. sence it's mostly a change to one function it makes even more sence to do this IMHO. I also added sevral statistic trackers to be added in the model coments section, I now have the BSP compiler track the maximum number of polygons packed into a single node, the maximum depth of BSP recursion for a model, and compile time (both total and tree generation), in both versions.
 *
 * Revision 1.31  2007/07/08 06:20:36  bobboau
 * fixed a BSP chacheing bug that killed radius and bounding boxes.
 *
 * Revision 1.30  2007/07/06 05:38:55  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.29  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.27  2007/07/02 23:40:09  kazan
 * this didn't fix the problem, but it's still better than the old code
 *
 * Revision 1.26  2007/07/01 15:15:11  kazan
 * add special token to paths <appdir> - and new default path <appdir>\textures
 * versions are now PCS2 Beta
 * add filename and version info to title bar
 * enable texture alpha blending
 *
 * Revision 1.25  2007/07/01 01:27:13  bobboau
 * had the pof exporter print the name of the submodel it was working on as to identify which subobject had a problem in the event of an infinite recursion.
 *
 * Revision 1.24  2007/06/30 14:13:13  kazan
 * put back in a little safer version of the contained.size() == 2 optimization
 * potentially fix pof shields
 * add error message for group with no polymodels
 *
 * Revision 1.23  2007/06/28 18:52:36  kazan
 * more main_panel garbage... and probably fix shield export bug i hope
 *
 * Revision 1.22  2007/06/27 21:13:58  kazan
 * maybe, possibly, we hope i just fixed BSP collision trees
 *
 * Revision 1.21  2007/06/27 00:01:15  kazan
 * trying to squash bugs
 *
 * Revision 1.20  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.19  2007/06/18 18:43:26  kazan
 * added BSP Boundbox/Sortnorm debug renderer
 * used said renderer to find and solve several BSP compilation bugs
 *
 * Revision 1.18  2007/06/18 01:15:02  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.17  2007/06/14 23:48:10  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.16  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.15  2007/03/25 17:04:15  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.14  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.13  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.12  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.11  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.10  2005/08/29 02:26:42  kazan
 * don't triangulate, and fall back to point-average when the centroid function panics
 *
 * Revision 1.9  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.8  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.7  2005/08/25 15:06:38  kazan
 * various bugfixes and tweaks
 *
 * Revision 1.6  2005/08/24 00:28:36  kazan
 * holy mother of optimizations - walk the bsp tree instead of parse it's chunks apart
 *
 * Revision 1.5  2005/08/23 22:36:42  kazan
 * s/std::{vector:string}/kaz_{vector:string}
 * renderer motion works
 * location PMF::LoadFromPOF bottleneck
 *
 * Revision 1.4  2005/08/23 01:31:11  kazan
 * disabled loading of BSP::Defpoints normals.. they're still corrupt
 *
 * renderer movement and lighting works
 *
 * Revision 1.3  2005/08/21 23:40:22  kazan
 * More work
 *
 * Revision 1.2  2005/08/21 01:58:53  kazan
 * Context Menu framework, tree ctrl contructor
 * bug fixes to std::string handling, POF->PMF translator
 * Loaded it's first model!
 *
 * Revision 1.1.1.1  2004/12/31 06:19:41  kazan
 * no message
 *
 *
 *
 */

#pragma warning(disable:4786)

#include <fstream>
#include "pcs_file.h"
#include "POFHandler.h"
#include "BSPHandler.h"
#include "pcs_pof_bspfuncs.h"

#include "pcs2.h"

//****************************************************************************
// Parallax Object File (POF) Saver
//****************************************************************************

int PCS_Model::SaveToPOF(std::string filename, AsyncProgress* progress)
{
	PCS_Model::BSP_MAX_DEPTH = 0;
	PCS_Model::BSP_NODE_POLYS = 1;
	PCS_Model::BSP_TREE_TIME = 0;
	PCS_Model::BSP_COMPILE_ERROR = false;
	POF poffile;
	unsigned int i,j,k,l;
	progress->setTarget(6 + light_arrays.size() + ai_paths.size() + insignia.size() + shield_mesh.size() + 
					thrusters.size() + docking.size() + turrets.size() + weapons.size() + special.size() +
					eyes.size() + model_info.size() + subobjects.size() + textures.size());
	char cstringtemp[256];


	// Update Progress
	progress->incrementWithMessage("Writing Header Pt1");

	// --------- convert cross sections --------- 
	std::vector<cross_section> sections;
	sections.resize(header.cross_sections.size());

	for (i = 0; i < header.cross_sections.size(); i++)
	{
		sections[i].depth = header.cross_sections[i].depth;
		sections[i].radius = header.cross_sections[i].radius;
	}
	poffile.HDR2_Set_CrossSections(header.cross_sections.size(), sections);

	// Update Progress
	progress->incrementWithMessage("Writing Header Pt2");

	// --------- ACEN --------- 
	poffile.ACEN_Set_acen(POFTranslate(autocentering));

	
	// Update Progress
	progress->incrementWithMessage("Writing Acen");
	// --------- TXTR --------- 

	
	// Update Progress
	progress->incrementWithMessage("Writing Textures");
	for (i = 0; i < textures.size(); i++)
		poffile.TXTR_AddTexture(textures[i]);

	
	// --------- Sub object Consversion ---------

	wxLongLong time = wxGetLocalTimeMillis();
	OBJ2 *obj;
	bool bsp_compiled = false;
	for (i = 0; i < subobjects.size(); i++)
	{
		// Update Progress
		//if (subobjects[i].name == "debris08")
		//	sprintf(cstringtemp, "Submodel %d: %s SENTINAL!", i, subobjects[i].name.c_str());
		//else
			sprintf(cstringtemp, "Submodel %d: %s", i, subobjects[i].name.c_str());
		progress->incrementWithMessage(cstringtemp);
	
		//memset((char *)&obj, 0, sizeof(OBJ2)); this is NO LONGER ALLOWED - obj2 now contains an class w/ vtable
		obj = new OBJ2;
		obj->submodel_number = i;
		if (!PMFObj_to_POFObj2(i, *obj, bsp_compiled))
		{
			delete[] obj->bsp_data;
			return 2; // error occured in bsp splitting!
		}
		poffile.OBJ2_Add(*obj); // takes over object management - including pointers

		delete obj;
	}
	time = wxGetLocalTimeMillis() - time;

	// we succeeded in compiling - let's cache the result
	can_bsp_cache = true;
	bsp_cache.resize(subobjects.size());
	for (i = 0; i < subobjects.size(); i++)
		poffile.OBJ2_Get_BSPData(i, bsp_cache[i].bsp_size, bsp_cache[i].bsp_data);


	// --------- ---------------------- ---------
	

	int idx = GetModelInfoCount();
	char cstrtmp[256];
	wxString strtmp = PCS2_VERSION;
	sprintf(cstrtmp, "PMFSaveToPOF: Compiled on %s with %s\nmax BSP depth was %d\nmost polys in a single node was %d\nTotal Compile time was %ldms, tree generation time was %ldms", std::string(strtmp.mb_str()).c_str(), std::string(PCS2_COMP_VERSION.mb_str()).c_str(), PCS_Model::BSP_MAX_DEPTH,PCS_Model::BSP_NODE_POLYS, time.ToLong(), PCS_Model::BSP_TREE_TIME.ToLong());
	
	bool found = false;
	for (i = 0; i < model_info.size() && !found; i++)
	{
		if (strstr(model_info[i].c_str(), "PMFSaveToPOF") != NULL)
		{
			found = true;
			if (bsp_compiled) // update the string
				model_info[i] = cstrtmp;
		}
	}

	if (!found)
		AddModelInfo(cstrtmp);

	j = 0;
	for (i = 0; i < model_info.size(); i++)
		j += model_info[i].length() + 1;
	
	char *pinf = new char[j];
	memset(pinf, 0, j);
	j = 0;

	for (i = 0; i < model_info.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing String %d", i); 
		progress->incrementWithMessage(cstringtemp);

		strncpy(pinf+j, model_info[i].c_str(), model_info[i].length());
		j+= model_info[i].length() + 1;
	}
	poffile.PINF_Set(pinf, j);
	delete[] pinf;

	if (found)
		model_info.resize(idx); // back down to size

	// ---------  EYE --------- 

	for (i = 0; i < eyes.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Eye %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.EYE_Add_Eye(eyes[i].sobj_number, 
							POFTranslate(eyes[i].sobj_offset), 
							POFTranslate(eyes[i].normal));
	}


	// --------- SPCL --------- 	

	for (i = 0; i < special.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Special %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.SPCL_AddSpecial(special[i].name, special[i].properties, 
									POFTranslate(special[i].point), special[i].radius);
	}

	k = l = 0;
	// --------- weapons (GPNT/MPNT) --------- 
	for (i = 0; i < weapons.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Weapon %d", i);
		progress->incrementWithMessage(cstringtemp);
		if (weapons[i].type == GUN)
		{
			poffile.GPNT_AddSlot();

			for (j = 0; j < weapons[i].muzzles.size(); j++)
			{
				poffile.GPNT_AddPoint(k, POFTranslate(weapons[i].muzzles[j].point),
										 POFTranslate(weapons[i].muzzles[j].norm));
			}
			k++;
		}
		else
		{	poffile.MPNT_AddSlot();

			for (j = 0; j < weapons[i].muzzles.size(); j++)
			{
				poffile.MPNT_AddPoint(l, POFTranslate(weapons[i].muzzles[j].point),
										 POFTranslate(weapons[i].muzzles[j].norm));
			}
			l++;
		}
	}

	// --------- turrets TGUN/TMIS --------- 
	k = l = 0;

	for (i = 0; i < turrets.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Turret %d", i);
		progress->incrementWithMessage(cstringtemp);
		if (turrets[i].type == GUN)
		{
			poffile.TGUN_Add_Bank(turrets[i].sobj_parent, 
								  turrets[i].sobj_par_phys, 
								  POFTranslate(turrets[i].turret_normal));
			for (j = 0; j < turrets[i].fire_points.size(); j++)
			{
				poffile.TGUN_Add_FirePoint(k, POFTranslate(turrets[i].fire_points[j]));
			}
			k++;
		}
		else
		{
			poffile.TMIS_Add_Bank(turrets[i].sobj_parent, 
								  turrets[i].sobj_par_phys, 
								  POFTranslate(turrets[i].turret_normal));
			for (j = 0; j < turrets[i].fire_points.size(); j++)
			{
				poffile.TMIS_Add_FirePoint(l, POFTranslate(turrets[i].fire_points[j]));
			}
			l++;
		}
	}

	// --------- docking --------- 
	for (i = 0; i < docking.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Docking %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.DOCK_Add_Dock(docking[i].properties);

		for (j = 0; j < docking[i].dockpoints.size(); j++)
		{
			poffile.DOCK_Add_Point(i, POFTranslate(docking[i].dockpoints[j].point), 
									  POFTranslate(docking[i].dockpoints[j].norm));
		}

		for (j = 0; j < docking[i].paths.size(); j++)
		{
			poffile.DOCK_Add_SplinePath(i, docking[i].paths[j]);
		}
	}

	// --------- thrusters --------- 
	for (i = 0; i < thrusters.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Thruster %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.FUEL_Add_Thruster(thrusters[i].properties);

		for (j = 0; j < thrusters[i].points.size(); j++)
		{
			poffile.FUEL_Add_GlowPoint(i, thrusters[i].points[j].radius,
										  POFTranslate(thrusters[i].points[j].pos),
										  POFTranslate(thrusters[i].points[j].norm));
		}
	}

	// --------- shield_mesh --------- 
	int fcs[3], nbs[3];
	std::vector<vector3d> points(shield_mesh.size()*3);
	vector3d shldtvect;

	// make points list
	l = 0;
	for (i = 0; i < shield_mesh.size(); i++)
	{
		for (j = 0; j < 3; j++)
		{
			bool found = false;
			for (unsigned int k = 0; k < points.size(); k++)
			{
				if (points[k] == POFTranslate(shield_mesh[i].corners[j]))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				if (l >= points.size())
					points.resize(points.size()*2);
				points[l] = POFTranslate(shield_mesh[i].corners[j]);
				l++;
			}
		}
	}
	points.resize(l);

	// translate shield mesh
	for (i = 0; i < shield_mesh.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Shield Tri %d", i);
		progress->incrementWithMessage(cstringtemp);
		// adds points to list if need, determine fcs[]
		for (j = 0; j < 3; j++)
		{
			shldtvect = POFTranslate(shield_mesh[i].corners[j]);
			fcs[j] = FindInList(points, shldtvect);
		}

		// determine neighbors (nbs[])
		j=0;
		for (k = 0; k < shield_mesh.size() && j < 3; k++)
		{
			if (Neighbor(shield_mesh[i], shield_mesh[k]) && i != k)
			{
				nbs[j] = k;
				j++;
			}
		}
		// add
		poffile.SHLD_Add_Face(POFTranslate(shield_mesh[i].face_normal), fcs, nbs);
	}

	// add points
	// Update Progress
	progress->incrementWithMessage("Writing Shield Points");
	for (i = 0; i < points.size(); i++)
		poffile.SHLD_Add_Vertex(points[i]);

	
	progress->incrementWithMessage("Writing Shield Collision Tree");
	// --------------- generate shield collision tree ----------------
	if (poffile.SHLD_Count_Faces() > 0)
	{
		std::vector<pcs_polygon> shldmesh(poffile.SHLD_Count_Faces());

		// make a pcs_polygon mesh from the shields
		for (i = 0; i < shldmesh.size(); i++)
		{
			shldmesh[i].verts.resize(3);

			poffile.SHLD_Get_Face(i, shldmesh[i].norm, fcs, nbs);

			for (j = 0; j < 3; j++)
			{
				 poffile.SHLD_Get_Vertex(fcs[j], shldmesh[i].verts[j].point);
				 shldmesh[i].verts[j].norm = shldmesh[i].norm;
			}
			
			shldmesh[i].centeroid = PolygonCenter(shldmesh[i]);
		}

		// make the tree
		vector3d smin, smax;
		bsp_tree_node *shld_root = MakeTree(shldmesh, smax, smin);

		// pack the tree
		int sldc_size = CalcSLDCTreeSize(shld_root);
		char *sldc = new char[sldc_size];
		
		PackTreeInSLDC(shld_root, 0, sldc, sldc_size);

		poffile.SLDC_SetTree(sldc, sldc_size); // POFHandler will make it's own copy of the buffer

		// we're done, destroy the tree and buffer
		DestroyTree(shld_root);
		delete[] sldc;
	}

	// --------- insignia --------- 

	vector3d uv, vv;
	float *u = (float *)&uv, *v = (float *)&vv;
	for (i = 0; i < insignia.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Insignia %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.INSG_Add_insignia(insignia[i].lod, POFTranslate(insignia[i].offset));

		for (j = 0; j < insignia[i].faces.size(); j++)
		{
			for (k = 0; k < 3; k++)
			{
				while ((l = poffile.INST_Find_Vert(i, POFTranslate(insignia[i].faces[j].verts[k]))) == (unsigned)-1)
				{
					poffile.INSG_Add_Insig_Vertex(i, POFTranslate(insignia[i].faces[j].verts[k]));
				} 
				fcs[k] = l;
				u[k] = insignia[i].faces[j].u[k];
				v[k] = insignia[i].faces[j].v[k];
			}
			poffile.INSG_Add_Insig_Face(i, fcs, uv, vv);
		}
	}

	// --------- ai_paths --------- 
	for (i = 0; i < ai_paths.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Path %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.PATH_Add_Path(ai_paths[i].name, ai_paths[i].parent);

		for (j = 0; j < ai_paths[i].verts.size(); j++)
		{
			poffile.PATH_Add_Vert(i, POFTranslate(ai_paths[i].verts[j].pos), 
									 ai_paths[i].verts[j].radius);
		}
	}

	// --------- light arrays --------- 
	pcs_glow_array *gla;
	for (i = 0; i < light_arrays.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Writing Glow %d", i);
		progress->incrementWithMessage(cstringtemp);
		gla = &light_arrays[i];
		poffile.GLOW_Add_LightGroup(gla->disp_time, gla->on_time, gla->off_time,
								    gla->obj_parent, gla->LOD, gla->type, gla->properties);
		for (j = 0; j < gla->lights.size(); j++)
		{
			poffile.GLOW_Add_GlowPoint(i, gla->lights[j].radius,
										  POFTranslate(gla->lights[j].pos),
										  POFTranslate(gla->lights[j].norm));
		}
	}

	// --------- HDR2 --------- 
	
	// let's make new BBoxes based on the subobjects
	vector3d minbox, maxbox, tmpmin, tmpmax;
	poffile.OBJ2_Get_BoundingMax(0, maxbox);
	poffile.OBJ2_Get_BoundingMin(0, minbox);

	for (i = 1; i < poffile.OBJ2_Count(); i++)
	{
		poffile.OBJ2_Get_BoundingMax(i, tmpmax);
		poffile.OBJ2_Get_BoundingMin(i, tmpmin);
		ExpandBoundingBoxes(maxbox, minbox, tmpmax);
		ExpandBoundingBoxes(maxbox, minbox, tmpmin);

	}

	for (i = 0; i < poffile.SHLD_Count_Vertices(); i++)
	{
		poffile.SHLD_Get_Vertex(i, tmpmax);
		ExpandBoundingBoxes(maxbox, minbox, tmpmax);
	}

	// update our bounding boxes
	//axis doesn't matter on the bounding boxes - it's all negative/positive values! i'm a faqing moron
	poffile.HDR2_Set_MinBound(minbox);
	poffile.HDR2_Set_MaxBound(maxbox);
	this->header.max_bounding = minbox;
	this->header.min_bounding = maxbox;

	poffile.HDR2_Set_MaxRadius(FindObjectRadius(maxbox, minbox, POFTranslate(header.mass_center)));
	poffile.HDR2_Set_Details(header.detail_levels.size(), header.detail_levels);
	poffile.HDR2_Set_Debris(header.debris_pieces.size(), header.debris_pieces);
	poffile.HDR2_Set_Mass(header.mass);
	poffile.HDR2_Set_MassCenter(POFTranslate(header.mass_center));
	poffile.HDR2_Set_MomentInertia(header.MOI);
	poffile.HDR2_Set_SOBJCount(GetSOBJCount());

	std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);

	if (!poffile.SavePOF(out))
		return 1;

	return 0;
}


//****************************************************************************
// Parallax Object File (POF) Loader
//****************************************************************************

int PCS_Model::LoadFromPOF(std::string filename, AsyncProgress* progress)
{
	this->Reset();
	char cstringtemp[256];
	progress->setMessage("Opening and Reading POF");
	progress->Notify();

	std::ifstream infile(filename.c_str(), std::ios::in | std::ios::binary);
	if (!infile)
		return 1;

	POF poffile(infile);
	progress->setTarget(4 + poffile.SPCL_Count() + poffile.GPNT_SlotCount() + poffile.MPNT_SlotCount() + poffile.TGUN_Count_Banks() + poffile.TMIS_Count_Banks() +
					poffile.DOCK_Count_Docks() + poffile.FUEL_Count_Thrusters() + poffile.INSG_Count_Insignia() + poffile.PATH_Count_Paths() + 
					poffile.GLOW_Count_LightGroups() + poffile.OBJ2_Count());

	// Update Progress
	progress->incrementWithMessage("Getting Header");

	header.max_radius = poffile.HDR2_Get_MaxRadius();
	header.min_bounding = poffile.HDR2_Get_MinBound();
	header.max_bounding = poffile.HDR2_Get_MaxBound();

	unsigned int i, j, k;
	int scratch; // useless variable - for legacy remnant argument
	poffile.HDR2_Get_Details(scratch, header.detail_levels);
	poffile.HDR2_Get_Debris(scratch, header.debris_pieces);

	header.mass = poffile.HDR2_Get_Mass();
	header.mass_center = POFTranslate(poffile.HDR2_Get_MassCenter());
	poffile.HDR2_Get_MomentInertia(header.MOI);

	// --------- convert cross sections --------- 
	std::vector<cross_section> sections;
	poffile.HDR2_Get_CrossSections(scratch, sections);
	
	if (scratch != -1)
	{
		header.cross_sections.resize(scratch);

		for (i = 0; i < header.cross_sections.size(); i++)
		{
			header.cross_sections[i].depth = sections[i].depth;
			header.cross_sections[i].radius = sections[i].radius;
		}
	}
	// --------- ACEN --------- 
	// Update Progress
	progress->incrementWithMessage("Getting ACEN, TXTR, PINF, EYE");
	autocentering = POFTranslate(poffile.ACEN_Get_acen());

	// --------- TXTR --------- 
	textures.resize(poffile.TXTR_Count_Textures());
	std::string tmp_test;
	for (i = 0; i < textures.size(); i++)
	{
		tmp_test = poffile.TXTR_GetTextures(i);
		textures[i] = tmp_test;
	}

	// --------- ---------------------- ---------
	
	model_info = poffile.PINF_Get();

	can_bsp_cache = false;
	for (i = 0; i < model_info.size(); i++)
	{
		if ( //strstr(model_info[i].c_str(), "BSPGEN") || // Volition's Compiler - caching revoked 2008-02-11 by Kazan because cannot gaurantee that tagged models actually game from V's compiler
			//strstr(model_info[i].c_str(), "POF-CS Compiler v1.3.4") ||  // removed PCS1 from recognized cacheable list 2008-01-10 - Kazan
			strstr(model_info[i].c_str(), PCS2_COMP_VERSION.mb_str()))
		{
			can_bsp_cache = true;
			break;
		}
	}


	// ---------  EYE --------- 
	eyes.resize(poffile.EYE_Count_Eyes());

	for (i = 0; i < eyes.size(); i++)
	{
		poffile.EYE_Get_Eye(i, eyes[i].sobj_number, eyes[i].sobj_offset, eyes[i].normal);
		eyes[i].sobj_offset = POFTranslate(eyes[i].sobj_offset);
		eyes[i].normal = POFTranslate(eyes[i].normal);
	}

	// --------- SPCL --------- 
	special.resize(poffile.SPCL_Count());

	for (i = 0; i < special.size(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Special %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.SPCL_Get_Special(i, special[i].name, special[i].properties, 
									special[i].point, special[i].radius);
		special[i].point = POFTranslate(special[i].point);
	}


	// --------- weapons (GPNT/MPNT) --------- 
	weapons.resize(poffile.GPNT_SlotCount() + poffile.MPNT_SlotCount());

	for (i = 0; i < poffile.GPNT_SlotCount(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Gun Point %d", i);
		progress->incrementWithMessage(cstringtemp);
		weapons[i].type = GUN;
		weapons[i].muzzles.resize(poffile.GPNT_PointCount(i));

		for (j = 0; j < poffile.GPNT_PointCount(i); j++)
		{
			poffile.GPNT_GetPoint(i, j, weapons[i].muzzles[j].point,
										weapons[i].muzzles[j].norm);
			weapons[i].muzzles[j].point = POFTranslate(weapons[i].muzzles[j].point);
			weapons[i].muzzles[j].norm = POFTranslate(weapons[i].muzzles[j].norm);
		}
	}

	k = poffile.GPNT_SlotCount();
	for (i = 0; i < poffile.MPNT_SlotCount(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Missile Point %d", i);
		progress->incrementWithMessage(cstringtemp);
		weapons[i+k].type = MISSILE;
		weapons[i+k].muzzles.resize(poffile.MPNT_PointCount(i));

		for (j = 0; j < poffile.MPNT_PointCount(i); j++)
		{
			poffile.MPNT_GetPoint(i, j, weapons[i+k].muzzles[j].point,
										weapons[i+k].muzzles[j].norm);
			weapons[i+k].muzzles[j].point = POFTranslate(weapons[i+k].muzzles[j].point);
			weapons[i+k].muzzles[j].norm = POFTranslate(weapons[i+k].muzzles[j].norm);
		}
	}

	// --------- turrets TGUN/TMIS --------- 
	turrets.resize(poffile.TGUN_Count_Banks() + poffile.TMIS_Count_Banks());

	for (i = 0; i < poffile.TGUN_Count_Banks(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Gun Turret %d", i);
		progress->incrementWithMessage(cstringtemp);
		turrets[i].type = GUN;
		poffile.TGUN_Get_Bank(i, turrets[i].sobj_parent, 
								 turrets[i].sobj_par_phys, 
								 turrets[i].turret_normal);

		turrets[i].turret_normal = POFTranslate(turrets[i].turret_normal);

		turrets[i].fire_points.resize(poffile.TGUN_Count_Points(i));

		for (j = 0; j < poffile.TGUN_Count_Points(i); j++)
		{
			poffile.TGUN_Get_FirePoint(i, j, turrets[i].fire_points[j]);
			turrets[i].fire_points[j] = POFTranslate(turrets[i].fire_points[j]);
		}
	}

	k = poffile.TGUN_Count_Banks();
	for (i = 0; i < poffile.TMIS_Count_Banks(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Missile Turret %d", i);
		progress->incrementWithMessage(cstringtemp);
		turrets[i+k].type = GUN;
		poffile.TMIS_Get_Bank(i, turrets[i+k].sobj_parent, 
								 turrets[i+k].sobj_par_phys, 
								 turrets[i+k].turret_normal);

		turrets[i+k].turret_normal = POFTranslate(turrets[i+k].turret_normal);

		turrets[i+k].fire_points.resize(poffile.TMIS_Count_Points(i));

		for (j = 0; j < poffile.TMIS_Count_Points(i); j++)
		{
			poffile.TMIS_Get_FirePoint(i, j, turrets[i+k].fire_points[j]);
			turrets[i+k].fire_points[j] = POFTranslate(turrets[i+k].fire_points[j]);
		}
	}

	// --------- docking --------- 
	docking.resize(poffile.DOCK_Count_Docks());

	for (i = 0; i < poffile.DOCK_Count_Docks(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Docking Point %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.DOCK_Get_DockProps(i, docking[i].properties);
		
		docking[i].dockpoints.resize(poffile.DOCK_Count_Points(i));

		for (j = 0; j < poffile.DOCK_Count_Points(i); j++)
		{
			poffile.DOCK_Get_Point(i, j, docking[i].dockpoints[j].point, 
										 docking[i].dockpoints[j].norm);
			docking[i].dockpoints[j].point = POFTranslate(docking[i].dockpoints[j].point);
			docking[i].dockpoints[j].norm = POFTranslate(docking[i].dockpoints[j].norm);
			
		}

		docking[i].paths.resize(poffile.DOCK_Count_SplinePaths(i));

		for (j = 0; j < poffile.DOCK_Count_SplinePaths(i); j++)
		{
			poffile.DOCK_Get_SplinePath(i, j, docking[i].paths[j]);
		}
	}

	// --------- thrusters --------- 
	thrusters.resize(poffile.FUEL_Count_Thrusters());

	for (i = 0; i < poffile.FUEL_Count_Thrusters(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Thruster %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.FUEL_Get_ThrusterProps(i, thrusters[i].properties);

		thrusters[i].points.resize(poffile.FUEL_Count_Glows(i));
		for (j = 0; j < poffile.FUEL_Count_Glows(i); j++)
		{
			poffile.FUEL_Get_GlowPoint(i, j, thrusters[i].points[j].radius,
											 thrusters[i].points[j].pos,
											 thrusters[i].points[j].norm);
			thrusters[i].points[j].pos = POFTranslate(thrusters[i].points[j].pos);
			thrusters[i].points[j].norm = POFTranslate(thrusters[i].points[j].norm);
		}
	}

	// --------- shield_mesh --------- 
	// Update Progress
	progress->incrementWithMessage("Getting Shields");
	shield_mesh.resize(poffile.SHLD_Count_Faces());
	int fcs[3], nbs[3];
	for (i = 0; i < shield_mesh.size(); i++)
	{
		poffile.SHLD_Get_Face(i, shield_mesh[i].face_normal, fcs, nbs);

		shield_mesh[i].face_normal = POFTranslate(shield_mesh[i].face_normal);\

		for (j = 0; j < 3; j++)
		{
			poffile.SHLD_Get_Vertex(fcs[j], shield_mesh[i].corners[j]);
			shield_mesh[i].corners[j] = POFTranslate(shield_mesh[i].corners[j]);
		}
	}

	// --------- insignia --------- 
	insignia.resize(poffile.INSG_Count_Insignia());

	vector3d uv, vv;
	float *u = (float *)&uv, *v = (float *)&vv;

	for (i = 0; i < poffile.INSG_Count_Insignia(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Insignia %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.INSG_Get_Insignia(i, insignia[i].lod, insignia[i].offset);

		insignia[i].offset = POFTranslate(insignia[i].offset);

		insignia[i].faces.resize(poffile.INSG_Count_Faces(i));

		for (j = 0; j < poffile.INSG_Count_Faces(i); j++)
		{
			poffile.INSG_Get_Insig_Face(i, j, fcs, uv, vv);

			for (k = 0; k < 3; k++)
			{
				poffile.INSG_Get_Insig_Vertex(i, fcs[k], insignia[i].faces[j].verts[k]);
				insignia[i].faces[j].verts[k] = POFTranslate(insignia[i].faces[j].verts[k]);

				insignia[i].faces[j].u[k] = u[k];
				insignia[i].faces[j].v[k] = v[k];
			}
		}
	}

	// --------- ai_paths --------- 
	ai_paths.resize(poffile.PATH_Count_Paths());
	for (i = 0; i < poffile.PATH_Count_Paths(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Path %d", i);
		progress->incrementWithMessage(cstringtemp);
		poffile.PATH_Get_Path(i, ai_paths[i].name, ai_paths[i].parent);

		ai_paths[i].verts.resize(poffile.PATH_Count_Verts(i));

		for (j = 0; j < poffile.PATH_Count_Verts(i); j++)
		{
			poffile.PATH_Get_Vert(i, j, ai_paths[i].verts[j].pos, 
										ai_paths[i].verts[j].radius);
			ai_paths[i].verts[j].pos = POFTranslate(ai_paths[i].verts[j].pos);
		}
	}

	// --------- light arrays --------- 
	light_arrays.resize(poffile.GLOW_Count_LightGroups());
	pcs_glow_array *gla;

	for (i = 0; i < poffile.GLOW_Count_LightGroups(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Glow Array %d", i);
		progress->incrementWithMessage(cstringtemp);
		gla = &light_arrays[i];
		poffile.GLOW_Get_Group(i, gla->disp_time, gla->on_time, gla->off_time,
								  gla->obj_parent, gla->LOD, gla->type, gla->properties);
		gla->lights.resize(poffile.GLOW_Count_Glows(i));

		for (j = 0; j < poffile.GLOW_Count_Glows(i); j++)
		{
			poffile.GLOW_Get_GlowPoint(i, j, gla->lights[j].radius,
											 gla->lights[j].pos,
											 gla->lights[j].norm);
			gla->lights[j].pos = POFTranslate(gla->lights[j].pos);
			gla->lights[j].norm = POFTranslate(gla->lights[j].norm);
		}
	}

	// --------- Sub object Consversion ---------
	subobjects.resize(poffile.OBJ2_Count());

	if (can_bsp_cache)
		bsp_cache.resize(poffile.OBJ2_Count());
	pcs_sobj *obj;
	int type, bspsz;
	unsigned int used_polygons;
	char *bspdata = NULL;
	BSP bspparser;

	BSP_DefPoints points;

	for (i = 0; i < poffile.OBJ2_Count(); i++)
	{
		// Update Progress
		sprintf(cstringtemp, "Getting Object %d", i);
		progress->incrementWithMessage(cstringtemp);
		obj = &subobjects[i];
		poffile.OBJ2_Get_Parent(i, obj->parent_sobj);
		poffile.OBJ2_Get_Radius(i, obj->radius);

		poffile.OBJ2_Get_Offset(i, obj->offset);
		obj->offset = POFTranslate(obj->offset);

		poffile.OBJ2_Get_GeoCenter(i, obj->geometric_center);
		obj->geometric_center = POFTranslate(obj->geometric_center);

		poffile.OBJ2_Get_BoundingMin(i, obj->bounding_box_min_point);
		//obj->bounding_box_min_point = POFTranslate(obj->bounding_box_min_point); Not Required

		poffile.OBJ2_Get_BoundingMax(i, obj->bounding_box_max_point);
		//obj->bounding_box_max_point = POFTranslate(obj->bounding_box_max_point); Not Required

		poffile.OBJ2_Get_Name(i, obj->name);
		poffile.OBJ2_Get_Props(i, obj->properties);
		poffile.OBJ2_Get_MoveType(i, type); // -1 = none, 1 = rotate
		switch (type)
		{
			case 1:
				obj->movement_type = ROTATE;
				break;
			default:
				obj->movement_type = MNONE;
		}
		poffile.OBJ2_Get_MoveAxis(i, type); // -1 = none, 1 = X, 2 = Z, 3 = Y
		switch (type)
		{
			case 1:
				obj->movement_axis = MV_X;
				break;
			case 2:
				obj->movement_axis = MV_Z;
				break;
			case 3:
				obj->movement_axis = MV_Y;
				break;
			default:
				obj->movement_axis = ANONE;
		}

		// fast method
		poffile.OBJ2_Get_BSPDataPtr(i, bspsz, bspdata);

		//OBJ2_Get_BSPData

		 obj->polygons.resize(100); // good starting size;
		used_polygons = 0;
		BSPTransPMF(0, (unsigned char *) bspdata, points, obj->polygons, used_polygons);
		obj->polygons.resize(used_polygons); // resize to exact size

		if (can_bsp_cache)
			poffile.OBJ2_Get_BSPData(i, bsp_cache[i].bsp_size, bsp_cache[i].bsp_data);

		//if (bspdata)
		//	delete[] bspdata;
		//bspdata = NULL;
	}

	
	return 0;
}

//****************************************************************************
// Parallax Object File (POF) Conversion Auxiliery Functions
//****************************************************************************

bool Neighbor(pcs_shield_triangle &face1, pcs_shield_triangle &face2)
{
	int CommonVerts = 0;

	if (face1.corners[0] == face2.corners[0] ||
		face1.corners[0] == face2.corners[1] ||
		face1.corners[0] == face2.corners[2])
		CommonVerts++;

	if (face1.corners[1] == face2.corners[0] ||
		face1.corners[1] == face2.corners[1] ||
		face1.corners[1] == face2.corners[2])
		CommonVerts++;

	if (face1.corners[2] == face2.corners[0] ||
		face1.corners[2] == face2.corners[1] ||
		face1.corners[2] == face2.corners[2])
		CommonVerts++;

	return (CommonVerts > 1 && CommonVerts < 3); // if CV=3 we're looking at ourself :D
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

bool PCS_Model::PMFObj_to_POFObj2(int src_num, OBJ2 &dst, bool &bsp_compiled)
{

	pcs_sobj &src = subobjects[src_num];

	dst.submodel_parent = src.parent_sobj;
	dst.offset = POFTranslate(src.offset);
	dst.geometric_center = POFTranslate(src.geometric_center);
	dst.submodel_name = APStoString(src.name);
	dst.properties = APStoString(src.properties);

	switch (src.movement_type)
	{
		case ROTATE:
			dst.movement_type = 1;
			break;
		default:
			dst.movement_type = -1;
	}
	switch (src.movement_axis)
	{
		case MV_X:
			dst.movement_axis = 1;
			break;
		case MV_Z:
			dst.movement_axis = 2;
			break;
		case MV_Y:
			dst.movement_axis = 3;
			break;
		default:
			dst.movement_axis = -1;
	}

	dst.reserved = 0;



	if(!can_bsp_cache || bsp_cache[src_num].changed)
	{

		unsigned int i, j, k, l;
		// convert them to POF axis
		std::vector<pcs_polygon> clean_list = src.polygons;
		for (i = 0; i < clean_list.size(); i++)
		{
			clean_list[i].norm = POFTranslate(clean_list[i].norm);
			for (j = 0; j < clean_list[i].verts.size(); j++)
			{
				clean_list[i].verts[j].point = POFTranslate(clean_list[i].verts[j].point);
				clean_list[i].verts[j].norm = POFTranslate(clean_list[i].verts[j].norm);
			}
			clean_list[i].centeroid = PolygonCenter(clean_list[i]);
		}

		// BSP Compilation!
		// assemble points list
		std::vector<bsp_vert> points_list;
		std::vector<vector3d> pnts;
		bsp_vert temp;
		points_list.reserve(clean_list.size());
		for (i = 0; i < clean_list.size(); i++)
		{
			for (j = 0; j < clean_list[i].verts.size(); j++)
			{
				temp.point = clean_list[i].verts[j].point;
				l = FindInList(points_list, temp);
				if (l == (unsigned)-1)
				{
					l = points_list.size();
					points_list.resize(l+1);
					pnts.resize(l+1);
					points_list[l].point = clean_list[i].verts[j].point;
					pnts[l] = points_list[l].point;
				}

				k = FindInList(points_list[l].norms, clean_list[i].verts[j].norm);

				if (k == (unsigned)-1)
				{
					k = points_list[l].norms.size();
					points_list[l].norms.resize(k+1);
					points_list[l].norms[k] = clean_list[i].verts[j].norm;
				}
			}
		}



		// create our defpoints
		BSP_DefPoints points;
		MakeDefPoints(points, points_list);
		vector3d AvgNormal;

		// create tree
		bsp_tree_node* root = MakeTree(clean_list, dst.bounding_box_max_point, dst.bounding_box_min_point);
		dst.radius = FindObjectRadius(dst.bounding_box_max_point, dst.bounding_box_min_point, vector3d(0,0,0));

		// calc tree size 
		dst.bsp_data_size = points.head.size + CalculateTreeSize(root, clean_list) + 8; // extra 8byte padd at the end - an extra BSP::EOF

		// allocate buffer and write the defpoints
		dst.bsp_data= new char[dst.bsp_data_size];
		memset(dst.bsp_data, 0, dst.bsp_data_size);

		if (points.Write(dst.bsp_data) != points.head.size)
			return false; // calculation error

		//std::ofstream bsp_debug("c:\\bsp.txt");
		//DebugPrintTree(root, bsp_debug);

		// pack the tree
		int error_flags = 0;
		PackTreeInBSP(root, points.head.size, dst.bsp_data, clean_list, points_list, pnts, points, dst.geometric_center, dst.bsp_data_size, error_flags);
		
		// we got errors!
		if (error_flags != BSP_NOERRORS)
			return false;

		// erase our tree and other variables
		DestroyTree(root);
		points.Destroy();

		// update the bsp_compiled to be true
		bsp_compiled = true;

		// update the BSP cache with the new result
		if (can_bsp_cache)
		{
			// clear the saved - stale cache
			bsp_cache[src_num].decache();
			bsp_cache[src_num].bsp_size = dst.bsp_data_size;
			bsp_cache[src_num].bsp_data= new char[dst.bsp_data_size];
			memcpy(bsp_cache[src_num].bsp_data, dst.bsp_data, dst.bsp_data_size);
			bsp_cache[src_num].changed = false;
		}


	}
	else // Used cached copy!
	{

		dst.bsp_data_size = bsp_cache[src_num].bsp_size; // extra 8byte padd at the end - an extra BSP::EOF

		// allocate buffer and write the defpoints
		dst.bsp_data= new char[dst.bsp_data_size];
		memcpy(dst.bsp_data, bsp_cache[src_num].bsp_data, dst.bsp_data_size);

		dst.bounding_box_max_point = dst.bounding_box_min_point = src.polygons[0].verts[0].point;

		for(unsigned int i = 0; i<src.polygons.size(); i++){
			for(unsigned int j = 0; j<src.polygons[i].verts.size(); j++){
				ExpandBoundingBoxes(dst.bounding_box_max_point, dst.bounding_box_min_point,  src.polygons[i].verts[j].point);
			}
		}
		dst.bounding_box_max_point = dst.bounding_box_max_point;
		dst.bounding_box_min_point = dst.bounding_box_min_point;

		dst.radius = FindObjectRadius(dst.bounding_box_max_point, dst.bounding_box_min_point, vector3d(0,0,0));
	}
	return true;
}
