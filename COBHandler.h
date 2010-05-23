//****************************************************************************
// COBHandler.h
// Author: Derek Meek
// Header for the COB Handler;
//****************************************************************************

/*
 * $Logfile: /COBHandler.h $
 * $Revision: 1.7 $
 * $Date: 2008/02/09 22:32:57 $
 * $Author: kazan $
 *
 * $Log: COBHandler.h,v $
 * Revision 1.7  2008/02/09 22:32:57  kazan
 * bye bye kaz_vector
 *
 * Revision 1.6  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.5  2007/10/18 15:30:09  kazan
 * fixed a metric ton of memory leaks by finally deprecating COB_String and replacing all instances with std::strings
 *
 * Revision 1.4  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.3  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
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

#include <iostream>
#include <fstream>

#include <memory.h>
#include "COBDataStructs.h"
#include "POFHandler.h"
#include "BSPHandler.h"
#include <cstring>

#ifndef _COBHandler_H_
#define _COBHandler_H_

// this is a bitch
struct Norm_Struct
{
	vector3d **Norms;
	int **FacetAngles;
};

// -------

class COB
{
	private:

		//storage variables
		COB_Header header;
		COB_Lght *lights;
		COB_Mat1 *materials;
		COB_PolH *polymodels;
		COB_Grou *groups;

		int numlights, nummats, numpolys, numgrous;

		COB_Lght BinParse_Light				(COB_ChunkHeader Head, char *buffer);
		bool	 BinParse_Material			(COB_ChunkHeader Head, char *buffer, COB_Mat1 *mat);
		COB_PolH BinParse_Polys				(COB_ChunkHeader Head, char *buffer);
		COB_Grou BinParse_Grous				(COB_ChunkHeader Head, char *buffer);

		// no reason for internal conversion functions to be public

	public:
		COB_Header& GetHeader() { return header; }
		COB_Lght& GetLight(int i) { return lights[i]; }
		COB_Mat1& GetMat1(int i) { return materials[i]; }
		COB_PolH& GetPolymodel(int i) { return polymodels[i]; }
		COB_Grou& GetGroup(int i) { return groups[i]; }

		int Light_Count()
			{ return numlights; }

		int Material_Count()
			{ return nummats; }

		int PolyModel_Count()
			{ return numpolys; }

		int Group_Count()
			{ return numgrous; }

		void Add_Lght						(COB_Lght Lght);
		bool Del_Lght						(int index);

		void Add_Mat1						(COB_Mat1 mat);
		bool Del_Mat1						(int index);

		void Add_PolH						(COB_PolH pol);
		bool Del_PolH						(int index);

		void Add_Grou						(COB_Grou gru);
		bool Del_Grou						(int index);



		COB()
		{
			numlights = 0;
			lights = NULL;

			nummats = 0;
			materials = NULL;

			numpolys= 0;
			polymodels = NULL;

			numgrous = 0;
			groups = NULL;

			memset(&header, 0, sizeof(header));
			memset(&header, 0, sizeof(header));
			strncpy(header.signature, "Caligari ", 9);
			strncpy(header.version, "V00.01", 6);
			header.mode = 'B';
			strncpy(header.edian, "LH", 2);
			header.newline = '\n';
		}

		~COB();
		void Reset();
		
		bool IndexGroups(int i, COB_Grou &gr)
		{
			if (i > -1 && i < numgrous)
			{
				gr = groups[i];
				return true;
			}
			return false;
		}

		bool IndexPolymodel(int i, COB_PolH &pl)
		{

			if (i > -1 && i < numpolys)
			{
				pl = polymodels[i];
				return true;
			}
			return false;
		}

		bool IndexMaterial(int i, COB_Mat1 &mt)
		{
			if (i > -1 && i < nummats)
			{
				mt = materials[i];
				return true;
			}
			return false;
		}

		bool IndexLight(int i, COB_Lght &lg)
		{
			if (i > -1 && i < numlights)
			{
				lg = lights[i];
				return true;
			}
			return false;
		}

		int LoadCOB							(std::string filename);
		int SaveCOB							(std::ofstream &OutFile, bool isScene=false); //Binary Mode
		std::ostream&							WriteStats(std::ostream &os);

		// conversion functions

		int AddShields						(POF &PCinst, int index, float scaler, vector3d &MaxBound, vector3d &MinBound);
		int AddInsignia						(POF &PCinst, int index, float scaler);
		//int  ConvertToPOF					(POF &PCinst, float scaler = 20);
		// user should only call the above
		int  FindParentGrou					(int ParentID);
		void CreatePointsList				(int ChunkID, std::vector<vector3d> &PointsList, float scalar);
		int	 FindMatchingMatl				(int ParentID, int MatNum);
		int  AddChildPolys					(int CurGrou, std::vector<vector3d> &PolyVertecies, Norm_Struct &normals, char *norm_counts, POF &PCinst,
														  std::vector<BSP_FlatPoly> &FlatPolys, std::vector<BSP_TmapPoly> &TexturePolys, float scaler);

};

// conversion functions
vector3d TranslateToWorld(vector3d point, COB_Matrix &matrix, COB_Axis &axis, float scaler);
std::string StripFileName(std::string fullname);

int FindPoint(vector3d Point, std::vector<vector3d> &PointsList);


void FindBoundBox(std::vector<vector3d> &points, vector3d &Min, vector3d &Max, BSP_TmapPoly &tpoly);
void FindBoundBox(std::vector<vector3d> &points, vector3d &Min, vector3d &Max, BSP_FlatPoly &fpoly);

// Old PCS 1.x functions... not needed
//int GenerateBSP(vector Min, vector Max, apvector<vector> &points, apvector<BSP_FlatPoly> &FlatPolys, apvector<BSP_TmapPoly> &TexturePolys, char *buffer, int &BufferOffset, vector &MinBound, vector &MaxBound, vector GeoCent, apvector<bool> &TpolyHit);
//void RemoveDuplicateCenters(apvector<BSP_FlatPoly> &FlatPolys, apvector<BSP_TmapPoly> &TexturePolys);
//void Remove(apvector<BSP_TmapPoly> &TexturePolys, int Culled);
//void Remove(apvector<BSP_FlatPoly> &FlatPolys, int Culled);
//bool Neighbor(shield_face &face1, shield_face &face2);
//float XtoY(float x, float y);

//-------------

//std::string APStoCOBString(std::string aps);
//std::string COBStringtoAPS(std::string cstr);

#define APStoCOBString(x) x
#define COBStringtoAPS(x) x

#endif //_COBHandler_H_

