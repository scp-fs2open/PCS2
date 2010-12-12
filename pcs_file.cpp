//****************************************************************************
//	pcs_file.cpp
//	POF Constructor Suite 2 File Format
//****************************************************************************

/*
 * $Logfile: /pcs_file.cpp $
 * $Revision: 1.72 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs_file.cpp,v $
 * Revision 1.72  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.71  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.70  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.69  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.68  2008/05/15 20:20:04  kazan
 * *** empty log message ***
 *
 * Revision 1.67  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.66  2008/02/12 18:59:54  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.65  2008/02/09 23:01:45  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.64  2008/02/09 22:33:00  kazan
 * bye bye kaz_vector
 *
 * Revision 1.63  2008/02/09 22:31:27  kazan
 * finish removing string
 *
 * Revision 1.62  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.61  2008/01/18 22:47:14  bobboau
 * made some changes to the geometry filter to allow it to handle non-coplanar polys better
 *
 * Revision 1.60  2008/01/14 11:55:13  bobboau
 * syncroniseing with CVS, nothing important, invisible texture don't draw, and a few added utility functions for the string
 *
 * Revision 1.59  2008/01/14 04:25:49  kazan
 * Disable VBOs
 *
 * Revision 1.58  2008/01/13 19:30:24  bobboau
 * update to HTL renderer, wireframe views/highlights
 *
 * Revision 1.57  2008/01/13 04:56:29  bobboau
 * one line bugfix for last commit.
 *
 * Revision 1.56  2008/01/12 18:02:24  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.55  2008/01/11 06:24:09  bobboau
 * fixing a spelling error, and modifyed the suggest box graphic a little
 *
 * Revision 1.54  2008/01/10 18:28:10  bobboau
 * *** empty log message ***
 *
 * Revision 1.53  2007/10/18 15:54:09  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.52  2007/10/18 14:54:02  kazan
 * Fix accuracy problem with BSP depth counter, error BSP_COMPILATION_ERROR flag, reenable infinite recursion catch due to duplicate-polygon-centers still causing infinite recursion
 *
 * Revision 1.51  2007/10/17 23:17:28  kazan
 * Autosmooth reverse engineer
 *
 * Revision 1.50  2007/07/25 05:43:19  bobboau
 * fixed a crash bug.
 *
 * Revision 1.49  2007/07/18 17:57:48  bobboau
 * made a plaform dependent fix for DevIL crashing when loading the nameplate.dds file, we could include some sort of error mesage or log it when this happens, from testing this fix will still cause the debugger to break on exception, so we will still be able to catch this, but to the user the file will simply fail to load.
 * this is platform dependent and will need Special care for all suported platforms, currently it defaults to simple exception handeling when not in WIN32, but this will not usualy catch a divide by zero error.
 *
 * I also made some modifications to the renderer to make it behave more like FSO, it renders children first, and uses alpha blending, it can lead to some ugly results, but they will be just as ugly in FSO so it's good they get to see it.
 *
 * Revision 1.48  2007/07/15 17:35:17  bobboau
 * fixed STUPIDEST BUG EVER
 *
 * Revision 1.47  2007/07/14 12:11:48  bobboau
 * added a OGL info menu option under optpons to help in debugging problems with people who have out of date OGL drivers.
 *
 * Revision 1.46  2007/07/13 11:31:05  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.45  2007/07/12 12:25:14  bobboau
 * fixed very minor potential multi-texture problem.
 *
 * Revision 1.44  2007/07/12 11:37:14  bobboau
 * added shine mapping suport.
 *
 * Revision 1.43  2007/07/12 08:34:21  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.42  2007/07/12 05:36:35  bobboau
 * fixed a crash bug in glowpoint saveing related to memcpying  a class with a string (which has a pointer to it's own memory buffer which it deletes on destruction) in it.
 *
 * Revision 1.41  2007/07/11 19:14:48  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.40  2007/07/11 14:09:37  kazan
 * fix solid-view rendering bug, renable app modal progress dialogs during load
 *
 * Revision 1.39  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.38  2007/07/09 00:07:13  bobboau
 * sence neither I nor Kazan seems to have a solid understanding on how to use branching, I've decided to commit the experimental BSP *within _BOB_BSP_ ifdefs* to the main branch. sence it's mostly a change to one function it makes even more sence to do this IMHO. I also added sevral statistic trackers to be added in the model coments section, I now have the BSP compiler track the maximum number of polygons packed into a single node, the maximum depth of BSP recursion for a model, and compile time (both total and tree generation), in both versions.
 *
 * Revision 1.37  2007/07/08 06:20:36  bobboau
 * fixed a BSP chacheing bug that killed radius and bounding boxes.
 *
 * Revision 1.36  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.34  2007/07/02 20:20:34  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.33  2007/07/01 15:15:11  kazan
 * add special token to paths <appdir> - and new default path <appdir>\textures
 * versions are now PCS2 Beta
 * add filename and version info to title bar
 * enable texture alpha blending
 *
 * Revision 1.32  2007/07/01 11:02:54  bobboau
 * modified the way things are drawn and implemented a few minor features
 *
 * Revision 1.31  2007/06/30 20:09:36  bobboau
 * added color options for wireframe overlays
 *
 * Revision 1.30  2007/06/30 18:58:11  bobboau
 * made some tweaks to the auto-path feature, and added an orthographic projection option
 *
 * Revision 1.29  2007/06/30 06:17:21  bobboau
 * fixed a small flaw in the last commit, forgot an odd case that could cause an infinite recusion.
 *
 * Revision 1.28  2007/06/30 05:43:35  bobboau
 * modified the geometry filter to also fix non-coplanar polygons, this is mostly to ensure good colision detection, but will also make the geometry render a little better.
 *
 * Revision 1.27  2007/06/30 04:16:31  bobboau
 * added a geometry filter and implemented it into COB loading, it fixes polys with more than 20 verts and more impresively it corects concave polygons.
 *
 * Revision 1.26  2007/06/30 01:45:16  bobboau
 * added texture highlighting
 *
 * Revision 1.25  2007/06/29 21:00:27  bobboau
 * added shield chunk "editor", doesn't actualy let you edit anything, but will display shield poly count and activate the rendering of shileds, which I also added.
 *
 * Revision 1.24  2007/06/29 20:15:43  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.23  2007/06/28 10:27:04  bobboau
 * fixed crash caused by trying to set focus to windows that no longer exsist (I hope)
 *
 * changed the behaviour of selected subojects back to drawing the LOD of the subobject, rather than the subobject it's self bacause it is very hard to do offset manipulation if you can't see the rest of the model.
 *
 * Revision 1.22  2007/06/27 21:45:23  kazan
 * fix texture loading and fix rendering selected object only
 *
 * Revision 1.21  2007/06/27 21:13:58  kazan
 * maybe, possibly, we hope i just fixed BSP collision trees
 *
 * Revision 1.20  2007/06/26 10:05:06  bobboau
 * added data import, includeing chunk, global, and subobject.
 *
 * Revision 1.19  2007/06/24 10:22:17  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.18  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.17  2007/06/18 18:43:26  kazan
 * added BSP Boundbox/Sortnorm debug renderer
 * used said renderer to find and solve several BSP compilation bugs
 *
 * Revision 1.16  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.15  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.14  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.13  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.12  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.11  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
 *
 * Revision 1.10  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.9  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.8  2005/08/29 18:05:43  kazan
 * fixed a crash due to OGL drivers not gracefully handing invalid texture handles, also changed how we resize the window
 *
 * Revision 1.7  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.6  2005/08/25 15:06:38  kazan
 * various bugfixes and tweaks
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


#pragma warning(disable: 4786)

#if defined(_WIN32)
	#include <windows.h>
	#include <wx/msw/winundef.h>
#endif

#include "GLee.h"
//#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glaux.h>
#include "pcs_file.h"
#include "pcs_pof_bspfuncs.h"
#include <fstream>
#include <set>
#include <cfloat>
#include "color.h"
#include "omnipoints.h"
#include <wx/msgdlg.h>

unsigned int PCS_Model::BSP_MAX_DEPTH = 0;
unsigned int PCS_Model::BSP_CUR_DEPTH = 0;
unsigned int PCS_Model::BSP_NODE_POLYS = 1;
bool PCS_Model::BSP_COMPILE_ERROR = false;
wxLongLong PCS_Model::BSP_TREE_TIME = 0;

//splits the poly at position I into two seperate polyogns, along the ith and jth vert
bool PCS_Model::split_poly(std::vector<pcs_polygon>&polys, int I, int i, int j){

	if(i>j){
		//for simplicities sake i will be less than j
		int temp = i;
		i=j;
		j=temp;
	}

	if(polys[I].verts.size() < 4 ||
	//can't split a triangle
	i==j ||
	//can't split along one vert
	j-i<2 || (i==0 && (unsigned)j == polys[I].verts.size()-1)){
	//one of them would have fewer than 3 verts: degenerate
		return false;
		wxMessageBox(_("*ERROR*:bad split attempted! \n\nOH NOE NOT THAT!."), _("in an emergency, your seat cusion may be used as a flotation device"));
	}


	polys.push_back(polys[I]);
	//make a copy of the poly
	pcs_polygon&split1 = polys[I];
	pcs_polygon&split2 = polys[polys.size()-1];

	//then remove the uneeded verts from the first and second
	int h;
	//remove the first i verts and everything after j
	for(h = 0; h<i; h++)
		split1.verts.erase(split1.verts.begin());
	split1.verts.resize(j-i+1);

	//remove everything between i and j (non inclusive) from the second
	for(h = i+1; h<j; h++)
		split2.verts.erase(split2.verts.begin()+(i+1));

	//fix normal
	vector3d norm(0,0,0);

	for(h = 0; h<(int)split1.verts.size(); h++)
		norm = norm + CrossProduct(split1.verts[(h+split1.verts.size()-1)%split1.verts.size()].point-split1.verts[h].point, split1.verts[(h+1)%split1.verts.size()].point-split1.verts[h].point);
	split1.norm = MakeUnitVector(norm);// / float(split1.verts.size());

	norm = vector3d(0,0,0);

	for(h = 0; h<(int)split2.verts.size(); h++)
		norm = norm + CrossProduct(split2.verts[(h+split2.verts.size()-1)%split2.verts.size()].point-split2.verts[h].point, split2.verts[(h+1)%split2.verts.size()].point-split2.verts[h].point);
	split2.norm = MakeUnitVector(norm );// / float(split2.verts.size());

	
	return true;
}

//return true if closest point is outside either line segments
bool closest_line_pt(vector3d l1p1, vector3d l1p2, vector3d l2p1, vector3d l2p2, vector3d*closest1 = NULL, vector3d*closest2 = NULL){

	vector3d ln1 = MakeUnitVector(l1p2-l1p1);
	vector3d ln2 = MakeUnitVector(l2p2-l2p1);

	vector3d In = CrossProduct(ln1,ln2);

	vector3d pnA = MakeUnitVector(CrossProduct(ln1,In));
	vector3d pnB = MakeUnitVector(CrossProduct(ln2,In));

	float d;
	
	d = dot(pnB,ln1);
	if(d==0.0f)return true;
	float tA = -dot(pnB,l1p1-l2p1)/d;

	d = dot(pnA,ln2);
	if(d==0.0f)return true;
	float tB = -dot(pnA,l2p1-l1p1)/d;

	if(closest1){
	   (*closest1) = l1p1+ln1*tA;
	}
	if(closest2){
	   (*closest2) = l2p1+ln2*tB;
	}
	if(tA<0.0f || tB<0.0f || Distance(l1p1,l1p2) < tA || Distance(l2p1,l2p2) < tB)
		return true;
	else
		return false;
}

float full_angle(vector3d A, vector3d B, vector3d C, vector3d pln){
	A = MakeUnitVector(A-C);
	B = MakeUnitVector(B-C);

	float ang = acos(dot(A,B));

	if(dot(CrossProduct(B,A),pln) > 0)
		ang = M_PI*2.0f - ang;

	return ang;
}

void interconect_poly_on_verts(std::vector<pcs_polygon>&polys, int i, std::vector<unsigned int>&concaves, vector3d&pnorm){
	
	pcs_polygon&poly = polys[i];
	const unsigned int psize = polys[i].verts.size();

		if(concaves.size() == 1){
			//in this case there is no other concave point
			//so let's split it in half and try that
			//I'm thinking in most cases spliting in half will be the best result

			//but it would be better if I tried every posable split untill I found one 
			//that had at least one good polygon with the largest number of verts in it 
			//less than half the total

			//but this should be good enough

			//probly

			if(PCS_Model::split_poly(polys, i, concaves[0], (concaves[0]+poly.verts.size()/2)%poly.verts.size())){
				PCS_Model::filter_polygon(polys, i);
				//the first split poly will be in the same place as the one we passed
			}
		}else if(concaves.size() > 1){
			//we have more than one concave point, try splitting between them to see if
			//we can find a split that will not cause problems
			unsigned int s;
			unsigned int t;
			for(s = 0; s < concaves.size(); s++){
					vector3d&sth = poly.verts[concaves[s]].point;
					vector3d&sthm1 = poly.verts[(concaves[s]-1+poly.verts.size())%poly.verts.size()].point;
					vector3d&sthp1 = poly.verts[(concaves[s]+1)%poly.verts.size()].point;
				for(t = s+1; t < concaves.size(); t++){
					if((concaves[s]+1)%poly.verts.size() == concaves[t] || (concaves[t]+1)%poly.verts.size() == concaves[s])
						continue;

					vector3d&tth = poly.verts[concaves[t]].point;

					//make sure we don't go into the bad place
					float  ang = full_angle(sthm1, sthp1, sth,pnorm);
					float tang = full_angle(sthm1, tth, sth,pnorm);
					if(tang<=ang)
						continue;
					//if the angle between s and t is between the angle for s and s+1, 
					//then we can dismis this pair right now

					//for every concave pair
					//test to see if the split made by it will cause problems
					//by seeing if there is an intersection with any other line
					unsigned int j;
					for(j = 0; j < poly.verts.size(); j++){
						vector3d test1, test2;
						if(concaves[s] == j || concaves[s] == (j+1)%psize || concaves[t] == j || concaves[t] == (j+1)%psize)
							continue;
						if(!closest_line_pt(poly.verts[j].point, poly.verts[(j+1)%poly.verts.size()].point, sth, tth, &test1, &test2))
						 break;
					}
					if(j == poly.verts.size())break;
				}
				if(t < concaves.size())break;
			}

			//if we found a good pair make the split, 
			if(s < concaves.size()){
				if(PCS_Model::split_poly(polys, i, concaves[s], concaves[t])){
					PCS_Model::filter_polygon(polys, i);
					//the first split poly will be in the same place as the one we passed
				}
			}else{
				//otherwise do an exaustive search for any split that will make a good poly
				//that starts with a concave point
				for(s = 0; s < concaves.size(); s++){
					vector3d&sth = poly.verts[concaves[s]].point;
					vector3d&sthm1 = poly.verts[(concaves[s]-1+poly.verts.size())%poly.verts.size()].point;
					vector3d&sthp1 = poly.verts[(concaves[s]+1)%poly.verts.size()].point;
					unsigned int T;
					for(T = 0; T < poly.verts.size()-1; T++){
						t = (T+concaves[s]+2)%poly.verts.size();

						if((concaves[s]+1)%poly.verts.size() == t || (t+1)%poly.verts.size() == concaves[s])
							continue;

						vector3d&tth = poly.verts[t].point;

						//make sure we don't go into the bad place
						float  ang = full_angle(sthm1, sthp1, sth,pnorm);
						float tang = full_angle(sthm1, tth, sth,pnorm);
						if(tang<=ang)
							continue;
						//if the angle between s and t is between the angle for s and s+1, 
						//then we can dismis this pair right now

						//for every pair
						//test to see if the split made by it will cause problems
						//by seeing if there is an intersection with any other line
						unsigned int j;
						for(j = 0; j < poly.verts.size(); j++){
							if(concaves[s] == j || concaves[s] == (j+1)%psize || t == j || t == (j+1)%psize)
								continue;
							vector3d&jth = poly.verts[j].point;
							//vector3d&jthm1 = poly.verts[(j-1+poly.verts.size())%poly.verts.size()].point;
							vector3d&jthp1 = poly.verts[(j+1)%poly.verts.size()].point;
							if(!closest_line_pt(jth, jthp1, sth, tth))
								break;
						}
						//if it made it to the end then s and t are good
						if(j == poly.verts.size())break;
					}
					//if t didn't make it to the end of the list, then we found a good pair in the t loop
					if(T < poly.verts.size()-1)break;
				}
				if(s < concaves.size()){
					//we found a good split so make it
					if(PCS_Model::split_poly(polys, i, concaves[s], t)){
						PCS_Model::filter_polygon(polys, i);
						//the first split poly will be in the same place as the one we passed
					}
				}else{
					//now we are fairly screwed, as there is no way to fix this
					wxMessageBox(_("*ERROR*:uncorectable geometry encountered! \n\nTruly this is the darkest of hours."), _("Think about the CHILDREN damnit!!!!!!!"));
				}
			}
		}
}

//fixes concave polygons
void PCS_Model::filter_polygon(std::vector<pcs_polygon>&polys, int i){

		if(polys[i].verts.size() < 4)return;
		//no need

		pcs_polygon&poly = polys[i];

		unsigned int psize = polys[i].verts.size();

		vector3d avg(0,0,0);
		for(unsigned int j = 0; j<psize; j++){
			avg += poly.verts[j].point;
		}
		avg = avg/psize;

		std::vector<vector3d> norms(psize);
		vector3d pnorm;
		for(unsigned int j = 0; j<psize; j++){
			norms[j] = CrossProduct(
				poly.verts[(j+1)%poly.verts.size()].point					-poly.verts[j].point, 
				poly.verts[(j-1+poly.verts.size())%poly.verts.size()].point	-poly.verts[j].point);
			if(!no_nan(norms[j])){
				norms[j] = vector3d(0,0,0);
			}
			pnorm += norms[j];
			norms[j] = MakeUnitVector(norms[j]);
		}
		pnorm = MakeUnitVector(pnorm);



		//go through and try to find 2 or more concave points

		std::vector<unsigned int> concaves;

		//make up a list of concave points
		for(unsigned int j = 0; j<psize; j++){
			if(dot(norms[j], pnorm)<=0.0f){
				//this vert is concave
				concaves.push_back(j);
			}
		}

		interconect_poly_on_verts(polys, i, concaves, pnorm);

		//poly should be convex at this point, more or less
		//now lets go for coplanar


		psize = polys[i].verts.size();

		norms.resize(psize);
		//these need to be rebuilt, because if the above function did anything they are totaly invalid
		for(unsigned int j = 0; j<psize; j++){
			norms[j] = CrossProduct(
				polys[i].verts[(j+1)%polys[i].verts.size()].point					-polys[i].verts[j].point, 
				polys[i].verts[(j-1+polys[i].verts.size())%polys[i].verts.size()].point	-polys[i].verts[j].point);
			if(!no_nan(norms[j])){
				norms[j] = vector3d(0,0,0);
			}
			pnorm += norms[j];
			norms[j] = MakeUnitVector(norms[j]);
		}

		pnorm = MakeUnitVector(pnorm);
		std::vector<unsigned int>&nonplanar = concaves;//lets reuse this
		nonplanar.resize(0);

		//make up a list of nonplanar points
		for(unsigned int j = 0; j<psize; j++){
			if(dot(norms[j], pnorm)<=0.999f){
				//this vert is concave
				nonplanar.push_back(j);
			}
		}

		interconect_poly_on_verts(polys, i, nonplanar, pnorm);


		if(polys[i].verts.size() > 20){
			//fix polys with too many verts
			if(split_poly(polys, i, 0, poly.verts.size()/2)){
				filter_polygon(polys, i);
				//the first split poly will be in the same place as the one we passed
			}
			return;
		}
}

void PCS_Model::filter_geometry(std::vector<pcs_polygon>&polys){
	for(unsigned int i = 0; i< polys.size(); i++){
		filter_polygon(polys, i);
	}
}

// PP shortcuts
#define PCS_ADD_TO_VEC(vec, var) unsigned int idx = vec.size(); \
								 vec.resize(idx+1); \
								 if (var) \
									vec[idx] = *var;

//****************************************************************************
// PCS Model File (PMF) Loader
//****************************************************************************

/*
File Version History
100: Initial
101: Added "facet_angle" to polygon
*/
int PCS_Model::LoadFromPMF(std::string filename, AsyncProgress* progress)
//PMF = PCS Model File
{
	this->Reset();
	progress->setTarget(17);
	std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
	if (!in)
		return 1;

	progress->incrementWithMessage("Checking Filesig");
	// -------- header -------- 
	char sig[] = { 'P', 'M', 'F', '1' }, fs[4];
	int ver;

	in.read(fs, 4);
	if (strncmp(fs, sig, 4))
		return 2; // filesig failed
	BFRead(ver, int)

	if (ver < PMF_MIN_VERSION || ver > PMF_MAX_VERSION)
		return 3; // version failed
	// --------  data  -------- 

	progress->incrementWithMessage("Reading Header");

	BFRead(header.max_radius, float)
	BFRead(header.min_bounding, vector3d)
	BFRead(header.max_bounding, vector3d)
	BFReadVector(header.detail_levels)
	BFReadVector(header.debris_pieces)
	BFRead(header.mass, float)
	BFRead(header.mass_center, vector3d)
	in.read((char*)header.MOI, sizeof(float)*9);
	BFReadVector(header.cross_sections)
	BFRead(autocentering, vector3d)

	progress->incrementWithMessage("Reading Textures");

	unsigned int i;
	BFRead(i, int)
	textures.resize(i);
	for (i = 0; i < textures.size(); i++)
		BFReadString(textures[i])

	progress->incrementWithMessage("Reading Subobjects");
	BFReadAdvVector(subobjects)

	progress->incrementWithMessage("Reading Info Strings");
	BFRead(i, int)
	model_info.resize(i);
	for (i = 0; i < model_info.size(); i++)
		BFReadString(model_info[i])

	progress->incrementWithMessage("Reading Eyes");
	BFReadVector(eyes)

	progress->incrementWithMessage("Reading Specials");
	BFReadAdvVector(special)

	progress->incrementWithMessage("Reading Weapons");
	BFReadAdvVector(weapons)

	progress->incrementWithMessage("Reading Turrets");
	BFReadAdvVector(turrets)

	progress->incrementWithMessage("Reading Docking");
	BFReadAdvVector(docking)

	progress->incrementWithMessage("Reading Thrusters");
	BFReadAdvVector(thrusters)

	progress->incrementWithMessage("Reading Shields");
	BFReadVector(shield_mesh)

	progress->incrementWithMessage("Reading Insignia");
	BFReadAdvVector(insignia)

	progress->incrementWithMessage("Reading Paths");
	BFReadAdvVector(ai_paths)

	progress->incrementWithMessage("Reading Glows");
	BFReadAdvVector(light_arrays)
	
	progress->incrementWithMessage("Reading BSP Cache");
	if (ver >= 102)
	{
		BFReadAdvVector(bsp_cache)
		BFRead(can_bsp_cache, bool)
		// XXX: bsp_data will always be junk so never use cached data.
		can_bsp_cache = false;

		// new in ver 102.. but not associated with above
		BFRead(has_fullsmoothing_data, bool)
		
	}

	progress->incrementProgress();

	return 0;
}

//****************************************************************************
// PCS Model File (PMF) Saver
//****************************************************************************
	
int PCS_Model::SaveToPMF(std::string filename, AsyncProgress* progress)
// PMF = PCS Model File
{
	progress->setTarget(17);

	std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);
	if (!out)
		return 1;

	progress->incrementWithMessage("Writing Filesig");
	// -------- header -------- 
	char sig[] = { 'P', 'M', 'F', '1' };
	int ver = PMF_VERSION;

	out.write(sig, 4);
	BFWrite(ver, int)

	progress->incrementWithMessage("Writing Header");
	// --------  data  -------- 

	BFWrite(header.max_radius, float)
	BFWrite(header.min_bounding, vector3d)
	BFWrite(header.max_bounding, vector3d)
	BFWriteVector(header.detail_levels)
	BFWriteVector(header.debris_pieces)
	BFWrite(header.mass, float)
	BFWrite(header.mass_center, vector3d)
	out.write((char*)header.MOI, sizeof(float)*9);
	BFWriteVector(header.cross_sections)
	BFWrite(autocentering, vector3d)


	progress->incrementWithMessage("Writing Textures");
	unsigned int i = textures.size();
	BFWrite(i, int)
	for (i = 0; i < textures.size(); i++)
		BFWriteString(textures[i])

	
	progress->incrementWithMessage("Writing Subobjects");
	BFWriteAdvVector(subobjects)
	
	progress->incrementWithMessage("Writing Info Strings");
	i = model_info.size();
	BFWrite(i, int)
	for (i = 0; i < model_info.size(); i++)
		BFWriteString(model_info[i])

	progress->incrementWithMessage("Writing Eyes");
	BFWriteVector(eyes)

	progress->incrementWithMessage("Writing Special");
	BFWriteAdvVector(special)

	progress->incrementWithMessage("Writing Weapons");
	BFWriteAdvVector(weapons)

	progress->incrementWithMessage("Writing Turrets");
	BFWriteAdvVector(turrets)

	progress->incrementWithMessage("Writing Docking");
	BFWriteAdvVector(docking)

	progress->incrementWithMessage("Writing Thrusters");
	BFWriteAdvVector(thrusters)

	progress->incrementWithMessage("Writing Shields");
	BFWriteVector(shield_mesh)

	progress->incrementWithMessage("Writing Insignia");
	BFWriteAdvVector(insignia)

	progress->incrementWithMessage("Writing Paths");
	BFWriteAdvVector(ai_paths)

	progress->incrementWithMessage("Writing Glows");
	BFWriteAdvVector(light_arrays)

	progress->incrementWithMessage("Writing BSP Cache");
	BFWriteAdvVector(bsp_cache)
	BFWrite(can_bsp_cache, bool)

	
	BFWrite(has_fullsmoothing_data, bool)

	progress->incrementProgress();
	return 0;
}

//****************************************************************************
// Resetter
//****************************************************************************

void PCS_Model::Reset()
{
	//leaveing this here in case other code uses it, it needs to be something
	active_submodel = 0;
	active_texture = -1;
	highlight_active_model = false;
	//Wireframe = false;
	//Textureless = false;
	can_bsp_cache = false;
	has_fullsmoothing_data = false;

	
	header.max_radius = 0;

	autocentering = header.mass_center = header.min_bounding = header.max_bounding = vector3d(0,0,0);

	header.detail_levels.resize(0);
	header.debris_pieces.resize(0);
	header.mass = 0.0;

	memset(header.MOI, 0, sizeof(float) * 9);

	for (size_t i = 0; i < subobjects.size(); i++)
		subobjects[i].destroy_vertex_buffer();

	header.cross_sections.resize(0);
	textures.resize(0);
	subobjects.resize(0);
	model_info.resize(0);
	eyes.resize(0);
	special.resize(0);
	weapons.resize(0);
	turrets.resize(0);
	docking.resize(0);
	thrusters.resize(0);
	shield_mesh.resize(0);
	insignia.resize(0);
	ai_paths.resize(0);
	light_arrays.resize(0);

	bsp_cache.resize(0);

}

//****************************************************************************
// Accessors
//****************************************************************************

float PCS_Model::GetMaxRadius()
{
	return header.max_radius;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

float PCS_Model::GetMass()
{
	return header.mass;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::GetMOI(std::vector<float>& tensor)
// float[3][3]
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			tensor[(3 * i) + j] = header.MOI[i][j];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


vector3d PCS_Model::GetMinBounding()
{
	return header.min_bounding;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

vector3d PCS_Model::GetMaxBounding()
{
	return header.max_bounding;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

vector3d PCS_Model::GetCenterOfMass()
{
	return header.mass_center;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

vector3d PCS_Model::GetAutoCenter()
{
	return autocentering;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


std::string& PCS_Model::ModelInfo(unsigned int idx)
{
	return model_info[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-



int PCS_Model::GetLODCount()
{
	return header.detail_levels.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetDebrisCount()
{
	return header.debris_pieces.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetCrossSectCount()
{
	return header.cross_sections.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetTexturesCount()
{
	return textures.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetSOBJCount()
{
	return subobjects.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetEyeCount()
{
	return eyes.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetSpecialCount()
{
	return special.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetWeaponCount()
{
	return weapons.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetTurretCount()
{
	return turrets.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetDockingCount()
{
	return docking.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetThrusterCount()
{
	return thrusters.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetShldTriCount()
{
	return shield_mesh.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetInsigniaCount()
{
	return insignia.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetPathCount()
{
	return ai_paths.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::GetLightCount()
{
	return light_arrays.size();
}


//****************************************************************************
// Referencers (both Acc/Mod)
//****************************************************************************

int&					PCS_Model::LOD			(unsigned int idx)
{
	return header.detail_levels[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int&					PCS_Model::Debris		(unsigned int idx)
{
	return header.debris_pieces[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_crs_sect&			PCS_Model::CrossSect	(unsigned int idx)
{
	return header.cross_sections[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string&			PCS_Model::Texture		(unsigned int idx)
{
	return textures[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_sobj&				PCS_Model::SOBJ		(unsigned int idx)
{
	return subobjects[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_eye_pos&			PCS_Model::Eye			(unsigned int idx)
{
	return eyes[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_special&			PCS_Model::Special		(unsigned int idx)
{
	return special[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_slot&				PCS_Model::Weapon		(unsigned int idx)
{
	return weapons[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_turret&				PCS_Model::Turret		(unsigned int idx)
{
	return turrets[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_dock_point&			PCS_Model::Dock		(unsigned int idx)
{
	return docking[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_thruster&			PCS_Model::Thruster	(unsigned int idx)
{
	return thrusters[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_shield_triangle&	PCS_Model::ShldTri		(unsigned int idx)
{
	return shield_mesh[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_insig&				PCS_Model::Insignia	(unsigned int idx)
{
	return insignia[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_path&				PCS_Model::Path		(unsigned int idx)
{
	return ai_paths[idx];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

pcs_glow_array&			PCS_Model::Light		(unsigned int idx)
{
	return light_arrays[idx];
}


//****************************************************************************
// Modifiers
//****************************************************************************

void PCS_Model::SetMaxRadius(float rad)
{
	header.max_radius = rad;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::SetMass(float mass)
{
	this->header.mass = mass;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::SetMOI(float tensor[3][3])
// float[3][3]
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			header.MOI[i][j] = tensor[i][j];
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::SetMinBounding(const vector3d &bnd)
{
	header.min_bounding = bnd;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::SetMaxBounding(const vector3d &bnd)
{
	header.max_bounding = bnd;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::SetCenterOfMass(const vector3d &cnt)
{
	header.mass_center = cnt;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::SetAutoCenter(const vector3d &cnt)
{
	autocentering = cnt;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::AddModelInfo(std::string info)
{
	/*unsigned int idx = model_info.size();
	model_info.resize(idx+1);
	model_info[idx] = info;*/
	model_info.push_back(info);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::SetModelInfo(unsigned int idx, std::string info)
{
	model_info[idx] = info;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddLOD(int sobj)
{
/*	unsigned int idx = header.detail_levels.size();
	header.detail_levels.resize(idx+1);
	header.detail_levels[idx] = sobj;*/
	header.detail_levels.push_back(sobj);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelLOD(unsigned int idx)
{
	RemoveIndex(header.detail_levels, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::AddDebris(int sobj)
{
/*	unsigned int idx = header.debris_pieces.size();
	header.debris_pieces.resize(idx+1);
	header.debris_pieces[idx] = sobj;*/
	header.debris_pieces.push_back(sobj);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelDebris(unsigned int idx)
{
	RemoveIndex(header.debris_pieces, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddCrossSect(pcs_crs_sect *cs)
{
	PCS_ADD_TO_VEC(header.cross_sections, cs)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelCrossSect(unsigned int idx)
{
	RemoveIndex(header.cross_sections, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::maybeAddTexture(std::string txt)
{
	int index = FindInList<std::string>(textures, txt);
	if (index != -1)
		return index;
	index = textures.size();
	textures.push_back(txt);
	return index;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::AddTextures(std::string txt)
{
	//unsigned int idx = textures.size();
	//textures.resize(idx+1);
	//textures[idx] = txt;
	textures.push_back(txt);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelTextures(unsigned int idx)
{
	RemoveIndex(textures, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddSOBJ(pcs_sobj *obj)
{
	pmf_bsp_cache cache;
	cache.decache();
	bsp_cache.push_back(cache);
	if (obj)
		subobjects.push_back(*obj);
	else
	{
		pcs_sobj empty;
		subobjects.push_back(empty);
	}
	if (vbos_enabled) {
		make_vertex_buffer(subobjects.size() - 1);
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelSOBJ(int idx)
{
	//we have to push down every reference to every subobject after this one
	//and mark each reference to this one as -1 (none, or root) or just erase it, or reparent it

	//start with subobject parents
	unsigned int i;
	for(i = 0; i<subobjects.size(); i++){
		if(subobjects[i].parent_sobj == idx)
			subobjects[i].parent_sobj=subobjects[idx].parent_sobj;//reparent to my parent
		if(subobjects[i].parent_sobj > (int)idx)
			subobjects[i].parent_sobj--;//it's actualy been moved one back, or it will be at the end of this function anyway
	}

	//LODs, if we deleted
	for(i = 0; i<header.detail_levels.size(); i++){
		if(header.detail_levels[i] == idx)
			header.detail_levels.erase(header.detail_levels.begin()+i);//does not exsist anymore
		if(i>=header.detail_levels.size())
			break;//we just deleted the last level
		if(header.detail_levels[i] > (int)idx)
			header.detail_levels[i]--;//it's actualy been moved one back, or it will be at the end of this function anyway
	}
	//debris, if we deleted
	for(i = 0; i<header.debris_pieces.size(); i++){
		if(header.debris_pieces[i] == idx)
			header.debris_pieces.erase(header.debris_pieces.begin()+i);//does not exsist anymore
		if(i>=header.debris_pieces.size())
			break;//we just deleted the last level
		if(header.debris_pieces[i] > (int)idx)
			header.debris_pieces[i]--;//it's actualy been moved one back, or it will be at the end of this function anyway
	}
	for(i = 0; i<turrets.size(); i++){
		if(turrets[i].sobj_par_phys == idx)
			turrets[i].sobj_par_phys = turrets[i].sobj_parent;
		if(turrets[i].sobj_par_phys > (int)idx)
			turrets[i].sobj_par_phys--;

		if(turrets[i].sobj_parent == idx)
			turrets.erase(turrets.begin()+i);//does not exsist anymore
		if(i>=turrets.size())
			break;//we just deleted the end
		if(turrets[i].sobj_parent > (int)idx)
			turrets[i].sobj_parent--;//it's actualy been moved one back, or it will be at the end of this function anyway
	}
	for(i = 0; i<eyes.size(); i++){
		if(eyes[i].sobj_number == idx)
			eyes[i].sobj_number = subobjects[idx].parent_sobj;
		if(i>=eyes.size())
			break;//we just deleted the end
		if(eyes[i].sobj_number > (int)idx)
			eyes[i].sobj_number--;
	}
	for(i = 0; i<light_arrays.size(); i++){
		if(light_arrays[i].obj_parent == idx)
			light_arrays[i].obj_parent = subobjects[idx].parent_sobj;
		if(i>=light_arrays.size())
			break;//we just deleted the end
		if(light_arrays[i].obj_parent > (int)idx)
			light_arrays[i].obj_parent--;
	}

	//finaly...
	if(active_submodel == idx)
		active_submodel = subobjects[idx].parent_sobj;
	if(active_submodel > (int)idx)
		active_submodel--;
	if(active_submodel < 0 && header.detail_levels.size() > 0)
		active_submodel = header.detail_levels[0];

	if (bsp_cache.size() > (unsigned)idx) {
		bsp_cache.erase(bsp_cache.begin() + idx);
	}
	RemoveIndex(subobjects, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		
void PCS_Model::SetObjectChanged(unsigned int idx)
{
	if (idx >= subobjects.size())
		return;

	if (vbos_enabled) {
		make_vertex_buffer(idx);
	}
	if (can_bsp_cache)
		bsp_cache[idx].changed = true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddEye(pcs_eye_pos *eye)
{
	PCS_ADD_TO_VEC(eyes, eye)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelEye(unsigned int idx)
{
	RemoveIndex(eyes, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddSpecial(pcs_special *spcl)
{
	PCS_ADD_TO_VEC(special, spcl)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelSpecial(unsigned int idx)
{
	RemoveIndex(special, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddWeapon(pcs_slot *weap)
{
	PCS_ADD_TO_VEC(weapons, weap)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelWeapon(unsigned int idx)
{
	RemoveIndex(weapons, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddTurret(pcs_turret *trrt)
{
	turrets.push_back(*trrt);
//	PCS_ADD_TO_VEC(turrets, trrt)
	//was getting an invalid allocation size with this for some reason
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelTurret(unsigned int idx)
{
	RemoveIndex(turrets, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::AddDocking(pcs_dock_point *dock)
{
	PCS_ADD_TO_VEC(docking, dock)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelDocking(unsigned int idx)
{
	RemoveIndex(docking, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddThruster(pcs_thruster *thrust)
{
	PCS_ADD_TO_VEC(thrusters, thrust)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelThruster(unsigned int idx)
{
	RemoveIndex(thrusters, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddShldTri(pcs_shield_triangle *stri)
{
	PCS_ADD_TO_VEC(shield_mesh, stri)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelShldTri(unsigned int idx)
{
	RemoveIndex(shield_mesh, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddInsignia(pcs_insig *insig)
{
	PCS_ADD_TO_VEC(insignia, insig)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelInsignia(unsigned int idx)
{
	RemoveIndex(insignia, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddPath(pcs_path *path)
{
	PCS_ADD_TO_VEC(ai_paths, path)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelPath(unsigned int idx)
{
	RemoveIndex(ai_paths, idx);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::AddLight(pcs_glow_array *lights)
{
	PCS_ADD_TO_VEC(light_arrays, lights)
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::DelLight(unsigned int idx)
{
	RemoveIndex(light_arrays, idx);
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::Calculate_Smoothing_Data(int &sobjs_comp)
{
	std::vector<std::vector<int> > covertals; // other polygons that share a vertex with us
	unsigned int i, j, k, l, m, cvc;
	bool tBool;//, afKill;
	vector3d tvect;//, *vects;
	pcs_sobj *sobj;
	//pcs_polygon *polA, *polB;
	if (has_fullsmoothing_data) return;

	sobjs_comp = 0;
	for (i =0; i < this->subobjects.size(); i++) // subobjects
	{
		sobj = &this->subobjects[i];
		for (j = 0; j < sobj->polygons.size(); j++) // outer polygon
		{

			// ---- first.. let's see if we're faceted.. if that's the case we don't have to compile the covertals lists -------
			tBool = true;
			for (k = 0; k < sobj->polygons[j].verts.size() && tBool == true; k++)
			{
				tBool = tBool && (sobj->polygons[j].norm == sobj->polygons[j].verts[k].norm);
			}
			if (tBool) 
			{
				// we're faceted
				for (k = 0; k < sobj->polygons[j].verts.size(); k++)
					sobj->polygons[j].verts[k].facet_angle = 0;
				continue;
			}

			//  ---------- find all polygons that share our verts -------
			covertals.resize(sobj->polygons[j].verts.size());
			for (k = 0; k < sobj->polygons[j].verts.size(); k++) // outer vertex
			{
				cvc=0;
				covertals[k].resize(10); // reasonable start

				for (l = 0; l < sobj->polygons.size(); l++) // inner polygon
				{
					if (l == j)
						continue; 
					for (m = 0; m < sobj->polygons[l].verts.size(); m++) // inner vertex
					{
						if (sobj->polygons[j].verts[k].point == sobj->polygons[l].verts[m].point)
						{
							covertals[k][cvc] = l;
							cvc++;

							if (cvc >= covertals[k].size())
									covertals[k].resize(cvc * 2);
						}
					} // inner vertex
				} // inner polygon

				covertals[k].resize(cvc);
			} // outer vertex

			// ok.. we have all our vertex lists covertals... now let's try to figure out what we are

			// -- trying full smooth --
			tBool = true;
			for (k = 0; k < sobj->polygons[j].verts.size() && tBool == true; k++)
			{
				tvect = sobj->polygons[j].norm;

				for (l = 0; l < covertals[k].size(); l++)
				{
					tvect += sobj->polygons[covertals[k][l]].norm;
				}
				tvect = tvect/(1+covertals[k].size());
				tBool = tBool && (tvect == sobj->polygons[j].verts[k].norm);
			}
			if (tBool) 
			{
				// we're full smooth
				for (k = 0; k < sobj->polygons[j].verts.size(); k++)
					sobj->polygons[j].verts[k].facet_angle = -1;
				continue;
			}

			// give up.. set autofacet to 32 degrees
			
			for (k = 0; k < sobj->polygons[j].verts.size(); k++)
				sobj->polygons[j].verts[k].facet_angle = 32;

			// ----- oh... crap... we're autofacet.... this is going to be UGLY --------
			// infact.. we're NOT going to try this.. it would be ungodly bad and probably never find the right angle to match
			//vector3d average_vectors_if_less_than_angle(int numvectors, float angle, vector3d src, vector3d *vects);
			/*afKill = false;
			for (k = 0; k < 360 && !afKill; k) // progressively increase the angle until we find it
			{
				tBool = true;
				for (l = 0; l < sobj->polygons[j].verts.size(); l++)
				{
					vects = new vector3d[1+covertals[l].size()];
					vects[0] = sobj->polygons[j].norm;

					for (m = 0; m < covertals[l].size(); m++)
					{
						vects[m+1] = sobj->polygons[covertals[k][l]].norm
					}

					delete[] vects;
				}
			} //angle loop
			*/


		} // outer polygon

		sobjs_comp++;
	} // subobjects

	has_fullsmoothing_data = true;
}




//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::draw_shields(){
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_ONE,GL_ONE);
	glDisable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);

	float light_one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float light_zero[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_zero );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_one );
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0f );

	glBegin(GL_TRIANGLES);
	glColor4ubv( (GLubyte*)(get_SHLD_color()*0.25f).col);

	for (unsigned int i = 0; i < shield_mesh.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			glNormal3fv((GLfloat *) &shield_mesh[i].face_normal);
			glVertex3fv((GLfloat *) &shield_mesh[i].corners[j]);
		}
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv( (GLubyte*)get_SHLD_color().col);

	for (unsigned int i = 0; i < shield_mesh.size(); i++)
	{
		glBegin(GL_LINE_STRIP);

		for (unsigned int j = 0; j < 3; j++)
		{
			glVertex3fv((GLfloat *) &shield_mesh[i].corners[j]);
		}

		// line strips need to return to zero point
		glVertex3fv((GLfloat *) &shield_mesh[i].corners[0]);

		glEnd();
	}
	glDepthMask(GL_TRUE);

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::draw_insignia(int lod, const omnipoints& omni){
	if (Wireframe) {
		return;
	}
	glColor4f(1.0, 1.0, 1.0, 1.0); // clear any color

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glDisable(GL_CULL_FACE);
	ERROR_CHECK;
	if (!Textureless) {
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);
		//for completness, set the first stage to multiply the base texture with light
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
	} else {
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	}

	for (unsigned int i = 0; i < insignia.size(); i++)
	{
		pcs_insig& insig = insignia[i];
		if (lod == insig.lod) {
			for (unsigned int j = 0; j < insig.faces.size(); j++) {
				pcs_insig_face& face = insig.faces[j];
				glBegin(GL_POLYGON);
				for (unsigned int k = 0; k < 3; k++)
				{
					vector3d offsetted(face.verts[k] + insig.offset);
					if (!Textureless) {
						glTexCoord2f(face.u[k], face.v[k]);
					}
					glVertex3fv((GLfloat *) &offsetted);
				}
				glEnd();
				ERROR_CHECK;
			}
		}
	}
	if (omni.point.size() == 2 && omni.point[0].size() == 1 && omni.point[1].size() == 4) {
			glBegin(GL_POLYGON);
			if (!Textureless) {
				glTexCoord2f(0.0f, 0.0f);
			}
			glVertex3fv((GLfloat *) &omni.point[1][0]);
			if (!Textureless) {
				glTexCoord2f(1.0f, 0.0f);
			}
			glVertex3fv((GLfloat *) &omni.point[1][1]);
			if (!Textureless) {
				glTexCoord2f(1.0f, 1.0f);
			}
			glVertex3fv((GLfloat *) &omni.point[1][2]);
			if (!Textureless) {
				glTexCoord2f(0.0f, 1.0f);
			}
			glVertex3fv((GLfloat *) &omni.point[1][3]);
			glEnd();
			ERROR_CHECK;
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv( (GLubyte*)get_SHLD_color().col);

	glDepthMask(GL_TRUE);

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void PCS_Model::Render(TextureControl &tc, bool use_vbos, bool highlight)
{
	if (header.detail_levels.size() < 1)
		return; // nothing to do

	int render_root = find_LOD_root(active_submodel);

	if ((unsigned)render_root > subobjects.size() || render_root < 0)
		return; // BAAAAAD active_lod

	glTranslatef(-autocentering.x, -autocentering.y, -autocentering.z);

	highlight_active_model = highlight;


	if(is_debris(render_root)){
		for(unsigned int i = 0; i<header.debris_pieces.size(); i++){
			RenderGeometryRecursive(header.debris_pieces[i], tc, use_vbos);
		}
	}else{
		RenderGeometryRecursive(render_root, tc, use_vbos);
	}

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void PCS_Model::RenderGeometryRecursive(int sobj, TextureControl &tc, bool use_vbos)
{
	bool vertex_buffers = GLEE_ARB_vertex_buffer_object == GL_TRUE; // can we use VBOs?
	//bool vertex_buffers = false; // Vertex buffer code was incompatable and busted context features
								 // editor formats are supposed to be easily editable - VBO is NOT.

	float light_one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float light_zero[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	vector3d trans = subobjects[sobj].offset; //OffsetFromParent(sobj);
	//GLenum err;
	ERROR_CHECK;

	glTranslatef(trans.x, trans.y, trans.z);

	// render children
	unsigned int i;
	std::string name;
	for (i = 0; i < subobjects.size(); i++)
	{
		name = strLower(subobjects[i].name.c_str());
	
		if (subobjects[i].parent_sobj == sobj && 
			strstr(name.c_str(), "-destroyed") == NULL)
			RenderGeometryRecursive(i, tc, use_vbos);
	}
		

	if(vertex_buffers && use_vbos){
		RenderGeometry_vertex_buffers(sobj, tc);
	}else{
		// Prep for rendering the geometry
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_one );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_zero );
	/*
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, light_zero );
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, light_zero );
	*/
		std::vector<pcs_polygon> &polygons = subobjects[sobj].polygons;
		// render geometry
		int tex_id = -1, glow_tex_id = -1, shine_tex_id = -1;
		glColor4f(1.0, 1.0, 1.0, 1.0); // clear any color

		if (!Wireframe)
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.01f);
		}
		else
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
		}

		ERROR_CHECK;
		for (i = 0; i < polygons.size(); i++)
		{
			if (!Wireframe)
			{	
				tex_id = tc.TextureTranslate(polygons[i].texture_id, TC_TEXTURE);
				glow_tex_id = tc.TextureTranslate(polygons[i].texture_id, TC_GLOW);
				if (tex_id != -1 && !Textureless)
				{
					glActiveTexture(GL_TEXTURE0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, tex_id);
					//for completness, set the first stage to multiply the base texture with light
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
					if(glow_tex_id > -1 && GLEE_ARB_multitexture){
						//if we have a glow map add it to the lit first stage
						glActiveTexture(GL_TEXTURE1);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, glow_tex_id);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
					}else{
						//if we don't have a glowmap, just pass it through
						glActiveTexture(GL_TEXTURE1);
						glDisable(GL_TEXTURE_2D);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
					}
				}
				else{
					glActiveTexture(GL_TEXTURE0);
					glDisable(GL_TEXTURE_2D);
					if(GLEE_ARB_multitexture){
						glActiveTexture(GL_TEXTURE1);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glDisable(GL_TEXTURE_2D);
					}
				}
				glBegin(GL_POLYGON);
			}
			else
			{
				glBegin(GL_LINE_STRIP);
			}

			glNormal3fv((GLfloat *) &polygons[i].norm);
			for (unsigned int j = 0; j < polygons[i].verts.size(); j++)
			{
				if (!Wireframe && tex_id != -1 && !Textureless)
				{
					glMultiTexCoord2f(GL_TEXTURE0, polygons[i].verts[j].u, polygons[i].verts[j].v);
					glMultiTexCoord2f(GL_TEXTURE1, polygons[i].verts[j].u, polygons[i].verts[j].v);
				}		
				glNormal3fv((GLfloat *) &polygons[i].verts[j].norm);
				glVertex3fv((GLfloat *) &polygons[i].verts[j].point);
			}

			if (Wireframe)
			{
				// line strips need to return to zero point
				glVertex3fv((GLfloat *) &polygons[i].verts[0].point);
			}

			glEnd();
		}

		if(glow_tex_id > -1 && GLEE_ARB_multitexture){
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		}

	//start specular pass
		ERROR_CHECK;

		if(!Wireframe && !Textureless){
			glDisable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glDepthMask(GL_FALSE);

			glActiveTexture(GL_TEXTURE0);

			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_zero );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_one );
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0f );

			glEnable(GL_TEXTURE_2D);
		//for completness, set the first stage to multiply the base texture with light
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 4.0f);


			for(i = 0; i < polygons.size(); i++)
			{
				shine_tex_id = tc.TextureTranslate(polygons[i].texture_id, TC_SHINEMAP);
				if(shine_tex_id >-1){
					glBindTexture(GL_TEXTURE_2D, shine_tex_id);
					glBegin(GL_POLYGON);
					glNormal3fv((GLfloat *) &polygons[i].norm);
					for (unsigned int j = 0; j < polygons[i].verts.size(); j++)
					{
						if (!Wireframe && tex_id != -1 && !Textureless)
						{
							glMultiTexCoord2f(GL_TEXTURE0, polygons[i].verts[j].u, polygons[i].verts[j].v);
						}		
						glNormal3fv((GLfloat *) &polygons[i].verts[j].norm);
						glVertex3fv((GLfloat *) &polygons[i].verts[j].point);
					}
					glEnd();
				}
			}
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

	//end specular pass
		ERROR_CHECK;

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glColor4ubv( (GLubyte*) get_SOBJ_color().col);

		//highlight active model
		if(highlight_active_model && sobj == active_submodel){
			OpenGL_RenderBox(subobjects[sobj].bounding_box_min_point, subobjects[sobj].bounding_box_max_point);
			for (i = 0; i < polygons.size(); i++)
			{

				glBegin(GL_LINE_STRIP);

				for (unsigned int j = 0; j < polygons[i].verts.size(); j++)
				{
					glVertex3fv((GLfloat *) &polygons[i].verts[j].point);
				}

				// line strips need to return to zero point
				glVertex3fv((GLfloat *) &polygons[i].verts[0].point);

				glEnd();
			}
		}

		glColor4ubv( (GLubyte*) get_TXTR_color().col);
		//highlight active texture
		if(active_texture >-1)
		for (i = 0; i < polygons.size(); i++)
		{
			if(polygons[i].texture_id == active_texture){
				glBegin(GL_LINE_STRIP);

				for (unsigned int j = 0; j < polygons[i].verts.size(); j++)
				{
					glVertex3fv((GLfloat *) &polygons[i].verts[j].point);
				}

				// line strips need to return to zero point
				glVertex3fv((GLfloat *) &polygons[i].verts[0].point);

				glEnd();
			}
		}
	}
	

	// (maybe) render BSP debug
	if (draw_bsp && can_bsp_cache && bsp_cache[sobj].bsp_size != 0)
	{
		/*GLUquadricObj *quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )
		gluQuadricNormals(quadratic, GLU_SMOOTH);
		glColor3f(0.5, 0.5, 1.0);
		gluSphere(quadratic,subobjects[sobj].radius,32,32);
		gluDeleteQuadric(quadratic);				// Delete Quadratic - Free Resources*/
		
		RenderBSP(0, (unsigned char*)bsp_cache[sobj].bsp_data.get(), subobjects[sobj].geometric_center);
	}

	// return to parents position
	glTranslatef(-trans.x, -trans.y, -trans.z);
	glActiveTexture(GL_TEXTURE0);

}

void PCS_Model::RenderGeometry_vertex_buffers(int sobj, TextureControl &tc){
	// Prep for rendering the geometry
	float light_one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float light_zero[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// render geometry
	int tex_id = -1, glow_tex_id = -1, shine_tex_id = -1;
	glColor4f(1.0, 1.0, 1.0, 1.0); // clear any color

		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.01f);

	ERROR_CHECK;
	if(!Wireframe)
	for(unsigned int t = 0; t<textures.size(); t++){
		if(subobjects[sobj].vertex_buffer.size() <1)
			continue;
		if(subobjects[sobj].vertex_buffer[t].n_verts <1)
			continue;

		tex_id = tc.TextureTranslate(t, TC_TEXTURE);
		if(tex_id == -1 && !tc.solid_render(t))
			continue;
		glow_tex_id = tc.TextureTranslate(t, TC_GLOW);
		shine_tex_id = tc.TextureTranslate(t, TC_SHINEMAP);

		glBindBuffer(GL_ARRAY_BUFFER, subobjects[sobj].vertex_buffer[t].buffer);
		glLockArraysEXT( 0, subobjects[sobj].vertex_buffer[t].n_verts);
	ERROR_CHECK;
	//	glInterleavedArrays(subobjects[sobj].vertex_buffer[t].format, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)NULL);

		glClientActiveTextureARB(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)NULL);
		glNormalPointer(GL_FLOAT, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)((vector3d*)NULL + 1));
		glTexCoordPointer(2, GL_FLOAT, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)((vector3d*)NULL + 2));

		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_one );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_zero );
		
		if (tex_id != -1 && !Textureless){

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_id);
			glEnable(GL_TEXTURE_2D);
		//for completness, set the first stage to multiply the base texture with light
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
			//glTexCoordPointer( 2, GL_FLOAT, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)NULL );
			if(glow_tex_id > -1 && GLEE_ARB_multitexture){
				//if we have a glow map add it to the lit first stage
				glClientActiveTextureARB(GL_TEXTURE1);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, subobjects[sobj].vertex_buffer[t].vertex_size, (void*)((vector3d*)NULL + 2));
				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
			//	glBindBuffer(GL_ARRAY_BUFFER_ARB, subobjects[sobj].vertex_buffer[t].buffer);
		
				glBindTexture(GL_TEXTURE_2D, glow_tex_id);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
			}else{
				//if we don't have a glowmap, just pass it through
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			}
		}else{
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			if(GLEE_ARB_multitexture){
				glActiveTexture(GL_TEXTURE1);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
				glDisable(GL_TEXTURE_2D);
			}
		}

	ERROR_CHECK;
		glDrawArrays(GL_TRIANGLES, 0, subobjects[sobj].vertex_buffer[t].n_verts);
		

		if(tex_id != -1 && !Textureless && glow_tex_id > -1 && GLEE_ARB_multitexture){
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		}

		if(tex_id != -1 && !Textureless && shine_tex_id >-1){
			glDisable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glDepthMask(GL_FALSE);

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, shine_tex_id);

			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_zero );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_one );
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0f );

		//for completness, set the first stage to multiply the base texture with light
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 4.0f);


	ERROR_CHECK;
			glDrawArrays(GL_TRIANGLES, 0, subobjects[sobj].vertex_buffer[t].n_verts);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glDisable(GL_TEXTURE_2D);
		}
		glUnlockArraysEXT();
	}
	ERROR_CHECK;

	glDisable(GL_LIGHTING);

	ERROR_CHECK;
	if((highlight_active_model && active_submodel == sobj) || Wireframe){
		if(!highlight_active_model || active_submodel != sobj)
			glColor3ub( (GLubyte)255, (GLubyte)255, (GLubyte)255);
		else
			glColor4ubv( (GLubyte*) get_SOBJ_color().col);
		if (highlight_active_model && active_submodel == sobj)
			OpenGL_RenderBox(subobjects[sobj].bounding_box_min_point, subobjects[sobj].bounding_box_max_point);
		for(unsigned int t = 0; t<textures.size(); t++){
			if (subobjects[sobj].line_vertex_buffer[t].buffer != 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, subobjects[sobj].line_vertex_buffer[t].buffer);
				glLockArraysEXT( 0, subobjects[sobj].line_vertex_buffer[t].n_verts);

				glActiveTexture(GL_TEXTURE0);
				glClientActiveTextureARB(GL_TEXTURE0);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3,GL_FLOAT, subobjects[sobj].line_vertex_buffer[t].vertex_size, (void*)NULL);

				glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_one );
				glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_zero );
				
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			ERROR_CHECK;
				glDrawArrays(GL_LINES, 0, subobjects[sobj].line_vertex_buffer[t].n_verts);
				glUnlockArraysEXT();
			}
		}
	}

	ERROR_CHECK;
	if(active_texture >-1 && subobjects[sobj].line_vertex_buffer[active_texture].buffer != 0){
		glColor4ubv( (GLubyte*) get_TXTR_color().col);
		glBindBuffer(GL_ARRAY_BUFFER, subobjects[sobj].line_vertex_buffer[active_texture].buffer);
		glLockArraysEXT( 0, subobjects[sobj].line_vertex_buffer[active_texture].n_verts);
	ERROR_CHECK;
		glActiveTexture(GL_TEXTURE0);
		glClientActiveTextureARB(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT, subobjects[sobj].line_vertex_buffer[active_texture].vertex_size, (void*)NULL);
	ERROR_CHECK;
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, light_one );
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, light_zero );
	ERROR_CHECK;
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	ERROR_CHECK;
		glDrawArrays(GL_LINES, 0, subobjects[sobj].line_vertex_buffer[active_texture].n_verts);
		glUnlockArraysEXT();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	ERROR_CHECK;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

vector3d PCS_Model::OffsetFromParent(int ObjNum)
{
	vector3d RetVal = MakeVector(0, 0, 0);
	int parnum;

	parnum = subobjects[ObjNum].parent_sobj;
	if (parnum == -1)
		return RetVal;
	else
	{
		RetVal = subobjects[ObjNum].offset;
		return RetVal + OffsetFromParent(parnum);
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PCS_Model::FindTexture(std::string name)
{
	for (unsigned int i = 0; i < textures.size(); i++)
		if (textures[i] == name)
			return i;
	return -1;
}

bobboau::matrix PCS_Model::moi_recalculate(int Xres, int Zres){
	float xw = (header.max_bounding.x - header.min_bounding.x)/(Xres);
	float zw = (header.max_bounding.z - header.min_bounding.z)/(Zres);

	bobboau::matrix ret = bobboau::matrix(vector3d(), vector3d(), vector3d());

	for(int x = 0; x<Xres; x++){
		for(int z = 0; z<Zres; z++){
			ret = ret + (moi_recalculate(header.min_bounding.x + x*xw + xw/2.0f, header.min_bounding.z + z*zw + zw/2.0f, xw, zw));
		}
	}

/*
	ret = moi_recalculate((header.max_bounding.x + header.min_bounding.x)/2
		,(header.max_bounding.z + header.min_bounding.z)/2
		,(header.max_bounding.x - header.min_bounding.x)
		,(header.max_bounding.z - header.min_bounding.z)
		,header.min_bounding.y,header.max_bounding.y-header.min_bounding.y);
	*/	
	ret = ret * header.mass;
//	return ret;
	return ret.invert();
}

bobboau::matrix PCS_Model::moi_recalculate(float X, float Z, float xw, float zw){
	std::vector<vector3d> cpoints;
	if(!moi_colide(cpoints, X, Z))
		return bobboau::matrix(vector3d(), vector3d(), vector3d());//return null
	//moi colide ensures there is an even number of colisions and orders them into pairs
	bobboau::matrix ret = moi_recalculate(X,Z,xw,zw,cpoints[0].y,cpoints[1].y-cpoints[0].y);
	for(unsigned int i = 1; i<cpoints.size()/2; i++){
		ret = ret + moi_recalculate(X,Z,xw,zw,cpoints[i].y,cpoints[i+1].y-cpoints[i].y);
	}
	return ret;
}

bobboau::matrix PCS_Model::moi_recalculate(float X, float Z, float xw, float zw, float Y, float dy){
	bobboau::matrix ret;
	double m = 1.0;//dy*xw*zw/2.0;
	double ftm = 4.0/3.0*m;
	double X2 = X*X;
	double Y2 = Y*Y;
	double Z2 = Z*Z;
	double xw2 = xw*xw;
	double zw2 = zw*zw;
	double dy2 = dy*dy;
	ret.a2d[0][0] = ftm*(3.0*dy*Y + dy2 + zw2 + 3.0*Y2 + 12.0*Z2);
	ret.a2d[1][1] = ftm*(xw2 + zw2 + 12.0*X2 + 12.0*Z2);
	ret.a2d[2][2] = ftm*(3.0*dy*Y + dy2 + xw2 + 12.0*X2 + 3.0*Y2);

	ret.a2d[0][1] = ret.a2d[1][0] = -4.0*m*X*(dy+2.0*Y);
	ret.a2d[0][2] = ret.a2d[2][0] = -16.0*m*X*Z;
	ret.a2d[2][1] = ret.a2d[1][2] = -4.0*m*Z*(dy+2.0*Y);

	return ret;
}

vector3d poly_min(pcs_polygon&poly){
	vector3d ret;
	ret = poly.verts[0].point;
	for(unsigned int i = 1; i<poly.verts.size(); i++){
		vector3d&point = poly.verts[i].point;
		if(point.x < ret.x)ret.x = point.x;
		if(point.y < ret.y)ret.y = point.y;
		if(point.z < ret.z)ret.z = point.z;
	}
	return ret;
}

vector3d poly_max(pcs_polygon&poly){
	vector3d ret;
	ret = poly.verts[0].point;
	for(unsigned int i = 1; i<poly.verts.size(); i++){
		vector3d&point = poly.verts[i].point;
		if(point.x > ret.x)ret.x = point.x;
		if(point.y > ret.y)ret.y = point.y;
		if(point.z > ret.z)ret.z = point.z;
	}
	return ret;
}

int moicpcf(const void*a, const void*b){
	return ((vector3d*)(a))->y < ((vector3d*)(b))->y;
}

int point_face(vector3d *checkp, std::vector<pcs_vertex> verts, vector3d * norm1){
	std::vector<vector3d> v(verts.size());
	for(unsigned int i = 0; i< verts.size(); i++){
		v[i] = verts[i].point;
	}
	return point_face(checkp, v, norm1);
}

bool PCS_Model::moi_colide(std::vector<vector3d>&cpoints, float x, float z){
	pcs_sobj&model = subobjects[header.detail_levels[0]];
	for(unsigned int i = 0; i<model.polygons.size(); i++){
		pcs_polygon&poly = model.polygons[i];
		vector3d min = poly_min(poly);
		vector3d max = poly_max(poly);
		if(x>max.x || x<min.x)continue;
		if(z>max.z || z<min.z)continue;

		bool sucsess;
		vector3d cpoint = plane_line_intersect(poly.verts[0].point, poly.norm, vector3d(x,0.0f,z), vector3d(0.0f,1.0f,0.0f), &sucsess);
		if(!sucsess)continue;

		if(!point_face(&cpoint, poly.verts, &poly.norm))
			continue;

		cpoints.push_back(cpoint);
	}

	if(cpoints.size()%2)
		cpoints.resize(0);

	if(cpoints.size() > 0){
		qsort(&cpoints[0], cpoints.size(), sizeof(vector3d), moicpcf);
		return true;
	}else{
		return false;
	}
}

void PCS_Model::Transform(const matrix& transform, const vector3d& translation) {
	std::set<int> dock_paths;
	header.min_bounding = vector3d(FLT_MAX, FLT_MAX, FLT_MAX);
	header.max_bounding = vector3d(FLT_MIN, FLT_MIN, FLT_MIN);
	header.max_radius = 0.0f;
	for (std::vector<pcs_sobj>::iterator it = subobjects.begin(); it < subobjects.end(); ++it) {
		if (it->parent_sobj == -1) {
			it->Transform(*this, (int)(it - subobjects.begin()), transform, translation, true, true);
		}
	}
	if (header.max_radius == 0.0f) {
		header.min_bounding = vector3d(0.0f, 0.0f, 0.0f);
		header.max_bounding = vector3d(0.0f, 0.0f, 0.0f);
	}
	for (std::vector<pcs_special>::iterator it = special.begin(); it < special.end(); ++it) {
		it->Transform(*this, transform, translation);
	}
	for (std::vector<pcs_slot>::iterator it = weapons.begin(); it < weapons.end(); ++it) {
		it->Transform(transform, translation);
	}
	for (std::vector<pcs_dock_point>::iterator it = docking.begin(); it < docking.end(); ++it) {
		it->Transform(*this, transform, translation);
		for (std::vector<int>::iterator jt = it->paths.begin(); jt < it->paths.end(); ++jt) {
			dock_paths.insert(*jt);
		}
	}
	for (std::vector<pcs_thruster>::iterator it = thrusters.begin(); it < thrusters.end(); ++it) {
		it->Transform(transform, translation);
	}
	for (std::vector<pcs_shield_triangle>::iterator it = shield_mesh.begin(); it < shield_mesh.end(); ++it) {
		it->Transform(*this, transform, translation);
	}
	for (std::vector<pcs_insig>::iterator it = insignia.begin(); it < insignia.end(); ++it) {
		it->Transform(transform, translation);
	}
	for (std::vector<pcs_path>::iterator it = ai_paths.begin(); it < ai_paths.end(); ++it) {
		if (it->parent.empty() && dock_paths.find((int)(it - ai_paths.begin())) != dock_paths.end()) {
			it->Transform(transform, translation);
		}
	}
	header.mass_center = transform * header.mass_center + translation;
	header.mass *= fabs(transform.determinant());
}
