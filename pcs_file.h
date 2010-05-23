//****************************************************************************
//	pcs_file.h
//	POF Constructor Suite 2 File Format
//  This is our internal project format
//
//	It's generic, geometry manipulation is easy, etc
//  this also makes it so we have a single "Open" and "Save" menu,
//  extension determining output format and then the appropriate data
//  being generated and written	
//
//  We're only doing one level of indirection (array management)
//  from the internal data, to the class, the GUI has raw access otherwise
//****************************************************************************

/*
 * $Logfile: /pcs_file.h $
 * $Revision: 1.52 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs_file.h,v $
 * Revision 1.52  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.51  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.50  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.49  2008/09/17 19:34:02  kazan
 * little cleanup
 *
 * Revision 1.48  2008/05/15 22:49:37  kazan
 * *** empty log message ***
 *
 * Revision 1.47  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.46  2008/02/12 18:59:55  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.45  2008/02/09 23:01:46  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.44  2008/02/09 22:33:01  kazan
 * bye bye kaz_vector
 *
 * Revision 1.43  2008/02/09 22:31:27  kazan
 * finish removing string
 *
 * Revision 1.42  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.41  2008/01/18 22:47:14  bobboau
 * made some changes to the geometry filter to allow it to handle non-coplanar polys better
 *
 * Revision 1.40  2008/01/14 01:13:30  kazan
 * Fix memory leak in POF->PMF code
 * Revoke PCS1.x recognition for BSP caching
 * Update BSP compile info on partial recompile
 *
 * Revision 1.39  2008/01/12 18:02:24  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.38  2008/01/11 06:24:09  bobboau
 * fixing a spelling error, and modifyed the suggest box graphic a little
 *
 * Revision 1.37  2008/01/10 18:28:10  bobboau
 * *** empty log message ***
 *
 * Revision 1.36  2007/10/18 15:54:09  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.35  2007/10/18 14:54:02  kazan
 * Fix accuracy problem with BSP depth counter, error BSP_COMPILATION_ERROR flag, reenable infinite recursion catch due to duplicate-polygon-centers still causing infinite recursion
 *
 * Revision 1.34  2007/10/17 23:17:28  kazan
 * Autosmooth reverse engineer
 *
 * Revision 1.33  2007/07/25 05:43:19  bobboau
 * fixed a crash bug.
 *
 * Revision 1.32  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.31  2007/07/09 00:07:13  bobboau
 * sence neither I nor Kazan seems to have a solid understanding on how to use branching, I've decided to commit the experimental BSP *within _BOB_BSP_ ifdefs* to the main branch. sence it's mostly a change to one function it makes even more sence to do this IMHO. I also added sevral statistic trackers to be added in the model coments section, I now have the BSP compiler track the maximum number of polygons packed into a single node, the maximum depth of BSP recursion for a model, and compile time (both total and tree generation), in both versions.
 *
 * Revision 1.30  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.28  2007/07/01 11:02:54  bobboau
 * modified the way things are drawn and implemented a few minor features
 *
 * Revision 1.27  2007/06/30 05:43:35  bobboau
 * modified the geometry filter to also fix non-coplanar polygons, this is mostly to ensure good colision detection, but will also make the geometry render a little better.
 *
 * Revision 1.26  2007/06/30 04:16:31  bobboau
 * added a geometry filter and implemented it into COB loading, it fixes polys with more than 20 verts and more impresively it corects convex polygons.
 *
 * Revision 1.25  2007/06/30 01:45:16  bobboau
 * added texture highlighting
 *
 * Revision 1.24  2007/06/29 21:00:28  bobboau
 * added shield chunk "editor", doesn't actualy let you edit anything, but will display shield poly count and activate the rendering of shileds, which I also added.
 *
 * Revision 1.23  2007/06/29 20:15:43  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.22  2007/06/27 21:45:23  kazan
 * fix texture loading and fix rendering selected object only
 *
 * Revision 1.21  2007/06/24 10:22:17  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.20  2007/06/18 14:01:53  bobboau
 * added a neat little 'suggestion control' wraper for primitive controls, you push a button it gives you a few good options (for example names of hard coded subsystems)
 * I also added index nameing suport to the array class, but it probly wasn't worth the effort.
 *
 * Revision 1.19  2007/06/18 06:56:44  bobboau
 * added a control for normals, is exactly like the vector control exept it has a button on the right for normalizeing the vectore, figured it would be useful, or at leaset fun to implement, also fixed a minor oversight with teh BSP rendering thing I did (forgot to give the flag a defaul value)
 *
 * Revision 1.18  2007/06/18 05:16:39  bobboau
 * added check box for Kazan's BSP rendering, will set PCS_Model::draw_bsp to true or false if BSP rendering should be performed.
 *
 * Revision 1.17  2007/06/17 10:29:17  bobboau
 * made a renderer for omnipoints, don't like it, it's not very fast, hope somone moreknolegable about OGL rerwites it someday.
 *
 * also worked on omnipoint editing, and the data flow related to it.
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
 * Revision 1.13  2007/03/25 17:04:14  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.12  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.11  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.10  2005/09/04 00:26:43  kazan
 * Enabled editor tab switching, added editor tab toolbar, enabled apply/revert for editors - add stubs for all editors
 *
 * Revision 1.9  2005/09/03 02:23:43  kazan
 * Header Editor Panel, Editors toolbar
 *
 * Revision 1.8  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.7  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.6  2005/08/28 02:39:06  kazan
 * Textures load and render!
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

#if !defined(_pcs_file_h_)
#define _pcs_file_h_

#include <wx/msgdlg.h>
#include <wx/longlong.h>

#include "vector3d.h"
#include "POFHandler.h"

#include "pcs_file_dstructs.h"
#include <string>
#include <vector>
#include "tex_ctrl.h"
#include <math.h>
#include "AsyncProgress.h"

//ogl error probe
#define ERROR_CHECK {GLenum err = glGetError();if(err != GL_NO_ERROR){wxString fileline(wxString::Format("%s (%d)", _(__FILE__), __LINE__));wxMessageBox(wxString("Warning OGL reported \"") + gluErrorString(err) + "\" at " + fileline + "\n please report this issue", "OpenGL Error");}}


// forward dec
class TextureControl;
class model_editor_ctrl_base;

#define PMF_VERSION 102
#define PMF_MIN_VERSION 100
#define PMF_MAX_VERSION PMF_VERSION

enum CHUNK { ROOT, HDR2, HDR2_SUB_DTL, HDR2_SUB_DEBRIS, ACEN, TXTR, SOBJ, PINF, EYE,
			SPCL, WEAP, GPNT, MPNT, WEAP_SUB, TGUN, TGUN_SUB, 
			DOCK, FUEL, FUEL_SUB, SHLD, INSG, INSG_SUB,
			PATH, PATH_SUB, GLOW, GLOW_SUB };

class PCS_Model 
{
	private:
		
		//********************************************************************
		// Data saved to disk
		//********************************************************************
		// ---- -------------------- ----
		// ----  Model Editing Data  ----
		// ---- -------------------- ----

		header_data header;

		vector3d autocentering; //ACEN

		// ---- -------------------- ----
		std::vector<std::string> textures; //TXTR
		std::vector<pcs_sobj> subobjects; // OBJ2
		std::vector<std::string> model_info; // PINF
		std::vector<pcs_eye_pos> eyes; // ' EYE'
		std::vector<pcs_special> special; // SPCL
		std::vector<pcs_slot> weapons; // GPNT and MPNT
		std::vector<pcs_turret> turrets; // TGUN and TMIS
		std::vector<pcs_dock_point> docking; // DOCK
		std::vector<pcs_thruster> thrusters; // FUEL
		std::vector<pcs_shield_triangle> shield_mesh; // SHLD
		std::vector<pcs_insig> insignia; // INSG
		std::vector<pcs_path> ai_paths; // PATH
		std::vector<pcs_glow_array> light_arrays; // GLOW
		// ---- -------------------- ----
		// ----       BSP Cache      ----
		// ---- -------------------- ----
		// #Caveat: BSP Cache/can_cache saved to PMF files starting with PMF version 102
		std::vector<pmf_bsp_cache> bsp_cache;
		bool can_bsp_cache;

		bool has_fullsmoothing_data;

		//********************************************************************
		// Data NOT saved to disk
		//********************************************************************

		// ---- -------------------- ----
		// ----    Rendering Data    ----
		// ---- -------------------- ----
		int active_submodel;
		int active_texture;
		bool Wireframe;
		bool Textureless;
		bool highlight_active_model;




		// priv funcs
		void RenderGeometryRecursive(int sobj, TextureControl &tc, bool use_vbos);
		void RenderGeometry_vertex_buffers(int sobj, TextureControl &tc);
		vector3d OffsetFromParent(int ObjNum);
		int FindTexture(std::string name);


		bool PMFObj_to_POFObj2(int src_num, OBJ2 &dst, bool &bsp_compiled);

		bobboau::matrix moi_recalculate(float X, float Z, float xw, float zw);
		bobboau::matrix moi_recalculate(float X, float Z, float xw, float zw, float Y, float dy);
		bool moi_colide(std::vector<vector3d>&cpoints, float x, float z);

	public:
		bobboau::matrix moi_recalculate(int Xres, int Yres);

		PCS_Model() : header(), active_submodel(0), Wireframe(false), Textureless(false), can_bsp_cache(false), has_fullsmoothing_data(false), draw_bsp(false)
		{

		}
		~PCS_Model(){
			for(unsigned int i = 0; i<subobjects.size(); i++){
				subobjects[i].destroy_vertex_buffer();
			}
		}

		// Renderer commands
		void Rcall_Wireframe(bool tf) { Wireframe = tf; }
		void Rcall_Textureless(bool tf) { Textureless = tf; }

		bool draw_bsp;
		//

		//statistic trackers
		static wxLongLong BSP_TREE_TIME;
		static unsigned int BSP_MAX_DEPTH;
		static unsigned int BSP_CUR_DEPTH;
		static unsigned int BSP_NODE_POLYS;
		static bool BSP_COMPILE_ERROR;


		void Reset();
		void PurgeBSPcache() { bsp_cache.resize(0); can_bsp_cache = false; }
		// Loaders
		int LoadFromPMF(std::string filename, AsyncProgress* progress); //PMF = PCS Model File
		int LoadFromPOF(std::string filename, AsyncProgress* progress);

		int LoadFromCOB(std::string filename, AsyncProgress* progress, float scaler, bool useFilter);
		int LoadFromDAE(std::string filename, AsyncProgress* progress, bool mirror_x, bool mirror_y, bool mirror_z);


		//geometery manipulation

		//splits the poly at position I into two seperate polyogns, along the ith and jth vert
		static bool split_poly(std::vector<pcs_polygon>&polys, int I, int i, int j);

		//poly fixing

		//fixes a convex polygon within a list
		//also fixes polygons whith more than 20 points
		static void filter_polygon(std::vector<pcs_polygon>&polys, int i);

		//calls the above on all polys in a list
		static void filter_geometry(std::vector<pcs_polygon>&polys);


		// brute-force recovers the smoothing angle numbers for each polygon
		void Calculate_Smoothing_Data(int &sobjs_comp); // will IMMEDIATELY TERMINATE if has_fullsmoothing_data == true

		// Savers
		int SaveToPMF(std::string filename, AsyncProgress* progress); // PMF = PCS Model File
		int SaveToPOF(std::string filename, AsyncProgress* progress);

		int SaveToCOB(std::string filename, AsyncProgress* progress, float scaler);

		// These functions don't actually work, and they're not likely to - just kept here for historical purposes
		// the ASE format was found to be unreliable
		//int SaveToASE(std::string filename, AsyncProgress* progress, float scaler);
		//int LoadFromASE(std::string filename, AsyncProgress* progress, float scaler);
		int SaveToDAE(std::string filename, AsyncProgress* progres, int helpers, int props_as_helpers);


		// Rendering
		void Render(TextureControl &tc, bool use_vbos, bool highlight = false);
		void draw_shields();


		// Accessors
		const header_data&get_header(){return header;}
		void set_header(const header_data&h){header = h;}
		float GetMaxRadius();
		float GetMass();
		void GetMOI(float **tensor); // float[3][3]

		vector3d GetMinBounding();
		vector3d GetMaxBounding();
		vector3d GetCenterOfMass();
		vector3d GetAutoCenter();


		size_t GetModelInfoCount() { return model_info.size(); }
		int GetLODCount();
		int GetDebrisCount();
		int GetCrossSectCount();
		int GetTexturesCount();
		int GetSOBJCount();
		int GetEyeCount();
		int GetSpecialCount();
		int GetWeaponCount();
		int GetTurretCount();
		int GetDockingCount();
		int GetThrusterCount();
		int GetShldTriCount();
		int GetInsigniaCount();
		int GetPathCount();
		int GetLightCount();

		// Referencers (both Acc/Mod)

		int&					LOD			(unsigned int idx);
		int&					Debris		(unsigned int idx);
		pcs_crs_sect&			CrossSect	(unsigned int idx);
		std::string&				Texture		(unsigned int idx);
		pcs_sobj&				SOBJ		(unsigned int idx);
		pcs_eye_pos&			Eye			(unsigned int idx);
		pcs_special&			Special		(unsigned int idx);
		pcs_slot&				Weapon		(unsigned int idx);
		pcs_turret&				Turret		(unsigned int idx);
		pcs_dock_point&			Dock		(unsigned int idx);
		pcs_thruster&			Thruster	(unsigned int idx);
		pcs_shield_triangle&	ShldTri		(unsigned int idx);
		pcs_insig&				Insignia	(unsigned int idx);
		pcs_path&				Path		(unsigned int idx);
		pcs_glow_array&			Light		(unsigned int idx);
		std::string&				ModelInfo	(unsigned int idx);

		// Modifiers
		void SetMaxRadius(float rad);
		void SetMass(float mass);
		void SetMOI(float tensor[3][3]); // float[3][3]

		void SetMinBounding(const vector3d &bnd);
		void SetMaxBounding(const vector3d &bnd);
		void SetCenterOfMass(const vector3d &cnt);
		void SetAutoCenter(const vector3d &cnt);

		void AddModelInfo(std::string info="");
		void SetModelInfo(unsigned int idx, std::string info);

		void SetNumLODs(int num) { header.detail_levels.resize(num); }
		void AddLOD(int sobj=-1); // we can add an emtpy lod, or initialize it upon creation
		void DelLOD(unsigned int idx);

		void SetNumDebris(int num) { header.debris_pieces.resize(num); }
		void AddDebris(int sobj=-1); // we can add an emtpy lod, or initialize it upon creation
		void DelDebris(unsigned int idx);

		void SetNumCrossSects(int num) { header.cross_sections.resize(num); }
		void AddCrossSect(pcs_crs_sect *cs=NULL); // we can add an emtpy lod, or initialize it upon creation
		void DelCrossSect(unsigned int idx);

		int maybeAddTexture(std::string txt);
		void AddTextures(std::string txt="");
		void DelTextures(unsigned int idx);

		void AddSOBJ(pcs_sobj *obj=NULL);
		void DelSOBJ(unsigned int idx);
		void SetObjectChanged(unsigned int idx);

		void AddEye(pcs_eye_pos *eye=NULL);
		void DelEye(unsigned int idx);

		void AddSpecial(pcs_special *spcl=NULL);
		void DelSpecial(unsigned int idx);

		void AddWeapon(pcs_slot *weap=NULL);
		void DelWeapon(unsigned int idx);

		void AddTurret(pcs_turret *trrt=NULL);
		void DelTurret(unsigned int idx);

		void AddDocking(pcs_dock_point *dock=NULL);
		void DelDocking(unsigned int idx);

		void AddThruster(pcs_thruster *thrust=NULL);
		void DelThruster(unsigned int idx);

		void AddShldTri(pcs_shield_triangle *stri=NULL);
		void DelShldTri(unsigned int idx);

		void AddInsignia(pcs_insig *insig=NULL);
		void DelInsignia(unsigned int idx);

		void AddPath(pcs_path *path=NULL);
		void DelPath(unsigned int idx);

		void AddLight(pcs_glow_array *lights=NULL);
		void DelLight(unsigned int idx);

		//interface accessors
			std::vector<std::string> get_textures(){return textures;}
			void set_textures(const std::vector<std::string> &t){textures = t;}

			std::vector<pcs_sobj> get_subobjects(){return subobjects;}
			void set_subobjects(const std::vector<pcs_sobj> &t){subobjects = t;}

			std::vector<std::string> get_model_info(){return model_info;}
			void set_model_info(const std::vector<std::string> &t){model_info = t;}
	
			std::vector<pcs_eye_pos> get_eyes(){return eyes;}
			void set_eyes(const std::vector<pcs_eye_pos> &t){eyes = t;}
	
			std::vector<pcs_special> get_special(){return special;}
			void set_special(const std::vector<pcs_special> &t){special = t;}
		
			std::vector<pcs_slot> get_weapons(){return weapons;}
			void set_weapons(const std::vector<pcs_slot> &t){weapons = t;}
		
			std::vector<pcs_turret> get_turrets(){return turrets;}
			void set_turrets(const std::vector<pcs_turret> &t){turrets = t;}
		
			std::vector<pcs_dock_point> get_docking(){return docking;}
			void set_docking(const std::vector<pcs_dock_point> &t){docking = t;}
		
			std::vector<pcs_thruster> get_thrusters(){return thrusters;}
			void set_thrusters(const std::vector<pcs_thruster> &t){thrusters = t;}
		
			std::vector<pcs_shield_triangle> get_shield_mesh(){return shield_mesh;}
			void set_shield_mesh(const std::vector<pcs_shield_triangle> &t){shield_mesh = t;}
		
			std::vector<pcs_insig> get_insignia(){return insignia;}
			void set_insignia(const std::vector<pcs_insig> &t){insignia = t;}
		
			std::vector<pcs_path> get_ai_paths(){return ai_paths;}
			void set_ai_paths(const std::vector<pcs_path> &t){ai_paths = t;}
	
			std::vector<pcs_glow_array> get_glow_points(){return light_arrays;}
			void set_glow_points(const std::vector<pcs_glow_array> &t){light_arrays = t;}

			void set_active_model(int idx){active_submodel = idx;}
			int get_active_model(){return active_submodel;};

			void set_active_texture(int idx){active_texture = idx;}
			int get_active_texture(){return active_texture;};

			int find_LOD_root(int idx){
				if(idx<0)
					return 0;
				if(subobjects[idx].parent_sobj <0)
					return idx;
				for(unsigned int i = 0; i<header.detail_levels.size(); i++){
					if(idx == header.detail_levels[i])
						return idx;
				}
				return find_LOD_root(subobjects[idx].parent_sobj);
			}

			bool is_debris(int idx){
				if(idx<0)
					return false;
				for(unsigned int i = 0; i<header.debris_pieces.size(); i++){
					if(idx == header.debris_pieces[i])
						return true;
				}
				return false;
			}

			vector3d get_model_offset(int i){
				if(i>=(int)subobjects.size())
					return vector3d(0,0,0);
				if(i <0)
					return autocentering;
				return get_model_offset(subobjects[i].parent_sobj) + subobjects[i].offset; 
			}

			//gets the poly count of all children of subobject idx
			size_t get_child_subobj_poly_count(int idx){
			size_t total = 0;
				//we realy need to make a proper tree
				for(unsigned int i = 0; i<subobjects.size(); i++){
					if(subobjects[i].parent_sobj == idx){
						total += subobjects[i].polygons.size() + get_child_subobj_poly_count(i);
					}
				}
				return total;
			}


			bool get_bsp_cache_status(){return can_bsp_cache;}

			//gets the average size of the model
			float get_avg_dimintion(){
				float d[6];
				d[0] = fabs(header.min_bounding.x);
				d[1] = fabs(header.min_bounding.y);
				d[2] = fabs(header.min_bounding.z);
				d[3] = fabs(header.max_bounding.x);
				d[4] = fabs(header.max_bounding.y);
				d[5] = fabs(header.max_bounding.z);

				float avg = 0;
				for(int i = 0; i<6; i++){
					avg += d[i];
				}
				avg/=6.0f;
				return avg;
			}

	void make_vertex_buffers();//sets up all vertex buffers for this model

};


#endif //_pcs_file_h_

