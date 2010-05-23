//****************************************************************************
// POFHandler.h
// Author: Derek Meek
// Comment: The POF File handler header
//****************************************************************************

/*
 * $Logfile: /POFHandler.h $
 * $Revision: 1.14 $
 * $Date: 2008/09/17 19:34:02 $
 * $Author: kazan $
 *
 * $Log: POFHandler.h,v $
 * Revision 1.14  2008/09/17 19:34:02  kazan
 * little cleanup
 *
 * Revision 1.13  2008/02/11 13:18:21  kazan
 * Possible fix for Mantis #59
 *
 * Revision 1.12  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.11  2008/02/09 22:32:58  kazan
 * bye bye kaz_vector
 *
 * Revision 1.10  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.9  2007/10/18 15:54:08  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.8  2007/07/13 18:22:21  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.7  2007/07/03 02:52:20  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.5  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.4  2005/09/19 21:47:12  kazan
 * Linux compat changes
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


#include <fstream>
#include "POFDataStructs.h"
#include <string>

#if !defined(_POF_HANDLER_H_)
#define _POF_HANDLER_H_

//****************************************************************************************************

// Time to start writing the work horse of POF editing


class POF {
	private:
		TXTR textures;
		HDR2 header;

		OBJ2 *objects;
		int numobj2;

		SPCL specials;
		GPNT_MPNT gunpoints;
		GPNT_MPNT missilepoints;
		TGUN_TMIS turretguns;
		TGUN_TMIS turretmissiles;
		DOCK docking;
		FUEL thrusters;
		SHLD shields;
		EYE eyes;
		ACEN autocentering;
		INSG insignia;
		PATH paths;
		GLOW hull_lights; // FS2_OPEN Chunk
		SLDC shield_collision;
		PINF pofinfo;



		// these are for loading up chunk data from files
		// 0 = gun, 1 = missile;
		void Parse_Memory_PNT(int mode, char *buffer);

		void Parse_Memory_T(int mode, char *buffer);

		void Parse_Memory_TXTR(char *buffer);
		void Parse_Memory_HDR2(char *buffer);
		void Parse_Memory_OBJ2(char *buffer);
		void Parse_Memory_SPCL(char *buffer);

		void Parse_Memory_GPNT(char *buffer)
			{ Parse_Memory_PNT(0, buffer); }

		void Parse_Memory_MPNT(char *buffer)
			{ Parse_Memory_PNT(1, buffer); }


		void Parse_Memory_TGUN(char *buffer)
			{ Parse_Memory_T(0, buffer); }

		void Parse_Memory_TMIS(char *buffer)
			{  Parse_Memory_T(1, buffer); }

		void Parse_Memory_DOCK(char *buffer);
		void Parse_Memory_FUEL(char *buffer);
		void Parse_Memory_GLOW(char *buffer);
		void Parse_Memory_SHLD(char *buffer);
		void Parse_Memory_EYE(char *buffer);
		void Parse_Memory_ACEN(char *buffer);
		void Parse_Memory_INSG(char *buffer);
		void Parse_Memory_PATH(char *buffer);
		void Parse_Memory_SLDC(char *buffer);
		void Parse_Memory_PINF (char *buffer, int size);



		// These are the actual workhorse functions that will be aliased by the GPNT_ and MPNT_ functions
		// Mode 0 = Gun, 1 = missile
		void PNT_AddSlot				(int mode);
		unsigned int PNT_SlotCount		(int mode);
		unsigned int PNT_PointCount		(int mode, int slot);
		bool PNT_DelSlot				(int mode, int slot_num);
		bool PNT_DelPoint				(int mode, int slot, int point);
		bool PNT_AddPoint				(int mode, int slot, vector3d point, vector3d norm);
		bool PNT_EditPoint				(int mode, int slot, int point_num, vector3d point, vector3d norm);
		bool PNT_GetPoint				(int mode, int slot, int point_num, vector3d &point, vector3d &norm);


		// These are the actual workhorse functions that will be aliased by the TGUN_ and TMIS_ functions
		// Mode 0 = Gun, 1 = missile
		void T_Add_Bank					(int mode, int sobj_parent, int sobj_par_phys, vector3d normal);
		bool T_Add_FirePoint			(int mode, int bank, vector3d pos);
		bool T_Edit_Bank				(int mode, int bank, int sobj_parent, int sobj_par_phys, vector3d normal);
		bool T_Edit_FirePoint			(int mode, int bank, int point, vector3d pos);
		bool T_Del_FirePoint			(int mode, int bank, int point);
		bool T_Del_Bank					(int mode, int bank);
		unsigned int T_Count_Banks		(int mode);
		unsigned int T_Count_Points		(int mode, int bank);
		bool T_Get_Bank					(int mode, int bank, int &sobj_parent, int &sobj_par_phys, vector3d &normal);
		bool T_Get_FirePoint			(int mode, int bank, int point, vector3d &pos);

		void ClearAllData();
		void Destroy();

		GPNT_MPNT* PNT_Alias(int mode); //hehe
		TGUN_TMIS* T_Alias(int mode); //hehe

	public:
		int version; //2116 or 2117 pof version only
		// Conversion Functions - Currently not implemented
		//bool ConvertFromCOB(COB cob);
		//bool ConvertFrom3DS(3DS 3ds);
		//bool ConvertToCOB(COB cob);
		//bool ConvertTo3DS(3DS 3ds);
		void Reset() // reset the class to it's initial state [changing files]
		{
			version = 2117;
			Destroy();
			ClearAllData();
		}

		// File IO Functions
		int LoadPOF(std::ifstream &infile);  // must be binary mode
		bool SavePOF(std::ofstream &outfile); // must be binary mode

		// Misc
		void StatsToFile(std::ofstream &outfile);

		// Constructor(s) and Destructor
		POF(std::ifstream &infile) // binary mode
		{
			version = 2117;
			ClearAllData();
			LoadPOF(infile);
		}

		POF()
			{ ClearAllData(); version = 2117; }
		//POF(COB cob); These two are for later use - reference to a Caligari Object File Handler
		//POF(3DS 3ds); Reference to a 3D Studio Max file handler
		~POF() // i almost forgot to do this;
			{ Destroy(); }

		// Shield collision tree
		int SLDC_GetSize() { return shield_collision.tree_size; }
		const char* SLDC_GetTree() { return shield_collision.tree_data; }
		void SLDC_SetTree(char* sldc_tree, unsigned int sz); // setting this to NULL will empty the tree

		// Texture Functions
		int TXTR_AddTexture(std::string texname);
		bool TXTR_DelTexture(int texture);
		int TXTR_FindTexture(std::string texname);
		bool TXTR_Edit_Texture(int texture, std::string textname);
		std::string TXTR_GetTextures(int texture);

		unsigned int TXTR_Count_Textures()
			{ return textures.num_textures; }


		// Header Functions
		void HDR2_Set_MaxRadius				(float maximum_radius)
			{ header.max_radius = maximum_radius; }

		void HDR2_Set_ObjectFlags			(int object_flags)
			{ header.obj_flags = object_flags; }

		void HDR2_Set_SOBJCount				(int count)
			{ header.num_subobjects = count;	}

		void HDR2_Set_MinBound				(vector3d min)
			{ header.min_bounding = min; }

		void HDR2_Set_MaxBound				(vector3d max)
			{ header.max_bounding = max; }

		void HDR2_Set_Details				(int num, std::vector<int> SOBJ_nums);
		void HDR2_Set_Debris				(int num, std::vector<int> SOBJ_nums);

		void HDR2_Set_Mass					(float mass)
			{ header.mass = mass; }

		void HDR2_Set_MassCenter			(vector3d center)
			{ header.mass_center = center; }


		void HDR2_Set_MomentInertia			(float inertia[3][3]);
		void HDR2_Set_CrossSections			(int num, std::vector<cross_section> sections);
		void HDR2_Set_Lights				(int num, const std::vector<muzzle_light> &li);

		float HDR2_Get_MaxRadius			()
			{ return header.max_radius; }

		int HDR2_Get_ObjectFlags			()
			{ return header.obj_flags; }

		unsigned int HDR2_Get_SOBJCount		()
			{ return header.num_subobjects; }

		vector3d HDR2_Get_MinBound			()
			{ return header.min_bounding; }

		vector3d HDR2_Get_MaxBound			()
			{ return header.max_bounding; }

		void HDR2_Get_Details				(int &num, std::vector<int> &SOBJ_nums);
		void HDR2_Get_Debris				(int &num, std::vector<int> &SOBJ_nums);

		float HDR2_Get_Mass					()
			{ return header.mass; }

		vector3d HDR2_Get_MassCenter			()
			{ return header.mass_center; }

		void HDR2_Get_MomentInertia			(float inertia[3][3]);
		void HDR2_Get_CrossSections			(int &num, std::vector<cross_section> &sections);
		void HDR2_Get_Lights				(int &num, std::vector<muzzle_light> &li);



		// Object [OBJ2] Stuff
		int  OBJ2_Add						(OBJ2 &obj);
		int  OBJ2_Add_SOBJ					();
		bool OBJ2_Del_SOBJ					(int SOBJNum);

		bool OBJ2_Set_SOBJNum				(int SOBJNum, int num);
		bool OBJ2_Set_Radius				(int SOBJNum, float rad);
		bool OBJ2_Set_Parent				(int SOBJNum, int parent);
		bool OBJ2_Set_Offset				(int SOBJNum, vector3d offset);
		bool OBJ2_Set_GeoCenter				(int SOBJNum, vector3d GeoCent);
		bool OBJ2_Set_BoundingMin			(int SOBJNum, vector3d min);
		bool OBJ2_Set_BoundingMax			(int SOBJNum, vector3d max);
		bool OBJ2_Set_Name					(int SOBJNum, std::string name);
		bool OBJ2_Set_Props					(int SOBJNum, std::string properties);
		bool OBJ2_Set_MoveType				(int SOBJNum, int type);
		bool OBJ2_Set_MoveAxis				(int SOBJNum, int axis);
		bool OBJ2_Set_BSPData				(int SOBJNum, int size, const char *bsp_data);


		unsigned int OBJ2_BSP_Datasize(int SOBJNum);
		unsigned int OBJ2_Count();

		bool OBJ2_Get_SOBJNum				(int SOBJNum, int &num);
		bool OBJ2_Get_Radius				(int SOBJNum, float &rad);
		bool OBJ2_Get_Parent				(int SOBJNum, int &parent);
		bool OBJ2_Get_Offset				(int SOBJNum, vector3d &offset);
		bool OBJ2_Get_GeoCenter				(int SOBJNum, vector3d &GeoCent);
		bool OBJ2_Get_BoundingMin			(int SOBJNum, vector3d &min);
		bool OBJ2_Get_BoundingMax			(int SOBJNum, vector3d &max);
		bool OBJ2_Get_Name					(int SOBJNum, std::string &name);
		bool OBJ2_Get_Props					(int SOBJNum, std::string &properties);
		bool OBJ2_Get_MoveType				(int SOBJNum, int &type);
		bool OBJ2_Get_MoveAxis				(int SOBJNum, int &axis);
		bool OBJ2_Get_BSPData				(int SOBJNum, int &size, char* &bsp_data);
		bool OBJ2_Get_BSPDataPtr			(int SOBJNum, int &size, char* &bsp_data);
		//'SPCL' specials functions
		void SPCL_AddSpecial				(std::string Name, std::string Properties, vector3d point, float radius);
		bool SPCL_DelSpecial				(int num);
		unsigned int SPCL_Count				();
		bool SPCL_Get_Special				(int num, std::string &Name, std::string &Properties, vector3d &point, float &radius);
		bool SPCL_Edit_Special				(int num, std::string Name, std::string Properties, vector3d point, float radius);



		//GPNT and MPNT
		//These functions are just moded aliases for other functions - hiding the underlying implementation
		//These Functions will just call the hidden functions with a mode flag specifing which point type
		// the first argument of the alised functions is the mode - 0 = gun, 1 = missile
		// then followed by the same arguments
		void GPNT_AddSlot()
			{ PNT_AddSlot(0); }

		unsigned int GPNT_SlotCount()
			{ return PNT_SlotCount(0); }

		unsigned int GPNT_PointCount(int slot)
			{ return PNT_PointCount(0, slot); }

		bool GPNT_DelSlot(int slot)
			{ return PNT_DelSlot(0, slot); }

		bool GPNT_DelPoint(int slot, int point)
			{ return PNT_DelPoint(0, slot, point); }

		bool GPNT_AddPoint(int slot, vector3d point, vector3d norm)
			{ return PNT_AddPoint(0, slot, point, norm); }

		bool GPNT_EditPoint(int slot, int point_num, vector3d point, vector3d norm)
			{ return PNT_EditPoint(0, slot, point_num, point, norm); }

		bool GPNT_GetPoint(int slot, int point_num, vector3d &point, vector3d &norm)
			{ return PNT_GetPoint(0, slot, point_num, point, norm); }

		//---------------------------------------
		void MPNT_AddSlot()
			{ PNT_AddSlot(1); }

		unsigned int MPNT_SlotCount()
			{ return PNT_SlotCount(1); }

		unsigned int MPNT_PointCount(int slot)
			{ return PNT_PointCount(1, slot); }

		bool MPNT_DelSlot(int slot)
			{ return PNT_DelSlot(1, slot); }

		bool MPNT_DelPoint(int slot, int point)
			{ return PNT_DelPoint(1, slot, point); }

		bool MPNT_AddPoint(int slot, vector3d point, vector3d norm)
			{ return PNT_AddPoint(1, slot, point, norm); }

		bool MPNT_EditPoint(int slot, int point_num, vector3d point, vector3d norm)
			{ return PNT_EditPoint(1, slot, point_num, point, norm); }

		bool MPNT_GetPoint(int slot, int point_num, vector3d &point, vector3d &norm)
			{ return PNT_GetPoint(1, slot, point_num, point, norm); }

		//TGUN and TMIS
		//These functions are just moded aliases for other functions - hiding the underlying implementation
		//These Functions will just call the hidden functions with a mode flag specifing which point type
		// the first argument of the alised functions is the mode - 0 = gun, 1 = missile
		// then followed by the same arguments

		void TGUN_Add_Bank(int sobj_parent, int sobj_par_phys, vector3d normal)
			{ T_Add_Bank(0, sobj_parent, sobj_par_phys, normal); }

		bool TGUN_Add_FirePoint(int bank, vector3d pos)
			{ return T_Add_FirePoint(0, bank, pos); }

		bool TGUN_Edit_Bank(int bank, int sobj_parent, int sobj_par_phys, vector3d normal)
			{ return T_Edit_Bank(0, bank, sobj_parent, sobj_par_phys, normal); }

		bool TGUN_Edit_FirePoint(int bank, int point, vector3d pos)
			{ return T_Edit_FirePoint(0, bank, point, pos); }

		bool TGUN_Del_FirePoint(int bank, int point)
			{ return T_Del_FirePoint(0, bank, point); }

		bool TGUN_Del_Bank(int bank)
			{ return T_Del_Bank(0, bank); }

		unsigned int TGUN_Count_Banks()
			{ return T_Count_Banks(0); }

		unsigned int TGUN_Count_Points(int bank)
			{ return T_Count_Points(0, bank); }

		bool TGUN_Get_Bank(int bank, int &sobj_parent, int &sobj_par_phys, vector3d &normal)
			{ return T_Get_Bank(0, bank, sobj_parent, sobj_par_phys, normal); }

		bool TGUN_Get_FirePoint(int bank, int point, vector3d &pos)
			{ return T_Get_FirePoint(0, bank, point, pos); }

		//---------------------------------------


		void TMIS_Add_Bank(int sobj_parent, int sobj_par_phys, vector3d normal)
			{ T_Add_Bank(1, sobj_parent, sobj_par_phys, normal); }

		bool TMIS_Add_FirePoint(int bank, vector3d pos)
			{ return T_Add_FirePoint(1, bank, pos); }

		bool TMIS_Edit_Bank(int bank, int sobj_parent, int sobj_par_phys, vector3d normal)
			{ return T_Edit_Bank(1, bank, sobj_parent, sobj_par_phys, normal); }

		bool TMIS_Edit_FirePoint(int bank, int point, vector3d pos)
			{ return T_Edit_FirePoint(1, bank, point, pos); }

		bool TMIS_Del_FirePoint(int bank, int point)
			{ return T_Del_FirePoint(1, bank, point); }

		bool TMIS_Del_Bank(int bank)
			{ return T_Del_Bank(1, bank); }

		unsigned int TMIS_Count_Banks()
			{ return T_Count_Banks(1); }

		unsigned int TMIS_Count_Points(int bank)
			{ return T_Count_Points(1, bank); }

		bool TMIS_Get_Bank(int bank, int &sobj_parent, int &sobj_par_phys, vector3d &normal)
			{ return T_Get_Bank(1, bank, sobj_parent, sobj_par_phys, normal); }

		bool TMIS_Get_FirePoint(int bank, int point, vector3d &pos)
			{ return T_Get_FirePoint(1, bank, point, pos); }


		// DOCK Functions
		void DOCK_Add_Dock				(std::string properties);
		bool DOCK_Add_SplinePath		(int dock, int path);
		bool DOCK_Add_Point				(int dock, vector3d point, vector3d norm);

		unsigned int DOCK_Count_Docks			();
		unsigned int DOCK_Count_SplinePaths		(int dock);
		unsigned int DOCK_Count_Points			(int dock);

		bool DOCK_Get_SplinePath		(int dock, int spline_path_num, int &path);
		bool DOCK_Get_Point				(int dock, int point, vector3d &pnt, vector3d &norm);
		bool DOCK_Get_DockProps			(int dock, std::string &properties);

		bool DOCK_Edit_SplinePath		(int dock, int spline_path_num, int path);
		bool DOCK_Edit_Point			(int dock, int point, vector3d pnt, vector3d norm);
		bool DOCK_Edit_DockProps		(int dock, std::string properties);

		bool DOCK_Del_Dock				(int dock);
		bool DOCK_Del_SplinePath		(int dock, int spline_path_num);
		bool DOCK_Del_Point				(int dock, int point);

		// FUEL Functions

		void FUEL_Add_Thruster			(std::string properties);
		bool FUEL_Add_GlowPoint			(int bank, float radius, vector3d pos, vector3d norm);

		unsigned int FUEL_Count_Thrusters		();
		unsigned int FUEL_Count_Glows			(int thruster);

		bool FUEL_Edit_GlowPoint		(int thruster, int gp, float radius, vector3d pos, vector3d norm);
		bool FUEL_Edit_ThrusterProps	(int thruster, std::string properties);

		bool FUEL_Del_Thruster			(int thrust);
		bool FUEL_Del_GlowPoint			(int thruster, int glowpoint);

		bool FUEL_Get_GlowPoint			(int thruster, int gp, float &radius, vector3d &pos, vector3d &norm);
		bool FUEL_Get_ThrusterProps		(int thruster, std::string &properties);

		// GLOW Functions

		void GLOW_Add_LightGroup		(int disp_time, int on_time, int off_time, int obj_parent, int LOD, int type, std::string properties = "$glow_texture=thrusterglow01");
		bool GLOW_Add_GlowPoint			(int group, float radius, vector3d pos, vector3d norm);

		unsigned int GLOW_Count_LightGroups		()
			{	return hull_lights.num_glows_arrays; }

		unsigned int GLOW_Count_Glows			(int group);

		bool GLOW_Edit_GlowPoint		(int group, int gp, float radius, vector3d pos, vector3d norm);
		bool GLOW_Edit_Group			(int group, int disp_time, int on_time, int off_time, int obj_parent, int LOD, int type, std::string properties);

		bool GLOW_Del_Group				(int group);
		bool GLOW_Del_GlowPoint			(int group, int glowpoint);

		bool GLOW_Get_GlowPoint			(int group, int gp, float &radius, vector3d &pos, vector3d &norm);
		bool GLOW_Get_Group				(int group, int &disp_time, int &on_time, int &off_time, int &obj_parent, int &LOD, int &type, std::string &properties);

		// SHLD Functions

		void SHLD_Add_Vertex			(vector3d vert);
		void SHLD_Add_Face				(vector3d normal, const int fcs[3], const int nbs[3]);
										// fcs = int[3] faces, nbs = int[3] neighbors

		unsigned int SHLD_Count_Vertices			();
		unsigned int SHLD_Count_Faces			();

		bool SHLD_Get_Face				(int face, vector3d &normal, int fcs[3], int nbs[3]);
		bool SHLD_Get_Vertex			(int vertex, vector3d &vert);

		bool SHLD_Edit_Vertex			(int vertex, vector3d &vert);
		bool SHLD_Edit_Face				(int face, vector3d normal, const int fcs[3], const int nbs[3]);

		bool SHLD_Del_Vertex			(int vertex);
		bool SHLD_Del_Face				(int face);

		// EYE Functions
		void EYE_Add_Eye				(int sobj_num, vector3d offset, vector3d normal);
		bool EYE_Del_Eye				(int eye);
		unsigned int  EYE_Count_Eyes				();
		bool EYE_Get_Eye				(int eye, int &sobj_num, vector3d &offset, vector3d &normal);
		bool EYE_Edit_Eye				(int eye, int sobj_num, vector3d offset, vector3d normal);

		// ACEN Functions
		void ACEN_Set_acen				(vector3d point);
		bool ACEN_Del_acen				();
		vector3d ACEN_Get_acen			();
		bool ACEN_IsSet					();


		// INSG Functions

		void INSG_Add_insignia			(int lod, vector3d offset);
		bool INSG_Add_Insig_Vertex		(int insig, vector3d vertex);
		bool INSG_Add_Insig_Face		(int insig, const int vert_indecies[3], const vector3d u_tex_coord, const vector3d v_tex_coord);
		bool INSG_Add_Insig_Face		(int insig, insg_face &InsgFace);

		unsigned int  INSG_Count_Insignia		();
		unsigned int  INSG_Count_Vertecies		(int insig);
		unsigned int  INSG_Count_Faces			(int insig);

		bool INSG_Get_Insignia			(int insig, int &lod, vector3d &offset);
		bool INSG_Get_Insig_Vertex		(int insig, int vert, vector3d &vertex);
		bool INSG_Get_Insig_Face		(int insig, int face, int vert_indecies[3], vector3d &u_tex_coord, vector3d &v_tex_coord);
		int  INST_Find_Vert				(int insig, vector3d vertex);

		bool INSG_Del_Insignia			(int insg);
		bool INSG_Del_Insig_Vertex		(int insig, int vert);
		bool INSG_Del_Insig_Face		(int insig, int face);

		bool INSG_Edit_Insignia			(int insig, int lod, vector3d offset);
		bool INSG_Edit_Insig_Vertex		(int insig, int vert, vector3d vertex);
		bool INSG_Edit_Insig_Face		(int insig, int face, const int vert_indecies[3], const vector3d u_tex_coord, const vector3d v_tex_coord);


		// PATH Functions
		void PATH_Add_Path				(std::string name, std::string parent);
		bool PATH_Add_Vert				(int path, vector3d point, float rad);
		bool PATH_Add_Turret			(int path, int vert, int sobj_number);

		unsigned int  PATH_Count_Paths			();
		unsigned int  PATH_Count_Verts			(int path);
		unsigned int  PATH_Count_Turrets			(int path, int vert);

		bool PATH_Get_Path				(int path, std::string &name, std::string &parents);
		bool PATH_Get_Vert				(int path, int vert, vector3d &point, float &rad);
		bool PATH_Get_Turret			(int path, int vert, int turret, int &sobj_number);

		bool PATH_Del_Path				(int path);
		bool PATH_Del_Vert				(int path, int vert);
		bool PATH_Del_Turret			(int path, int vert, int turret);

		bool PATH_Edit_Path				(int path, std::string name, std::string parent);
		bool PATH_Edit_Vert				(int path, int vert, vector3d point, float rad);
		bool PATH_Edit_Turret			(int path, int vert, int turret, int sobj_number);

		// PINF Functions
		void PINF_Set					(std::string pof_info);
		void PINF_Set					(char *str, int sz);
		bool PINF_Del					();
		std::vector<std::string> PINF_Get();

};

//std::string APStoString(std::string aps);
//std::string StringToAPS(std::string &str);

#define APStoString(x) x
#define StringToAPS(x) x

#endif //_POF_HANDLER_H_
