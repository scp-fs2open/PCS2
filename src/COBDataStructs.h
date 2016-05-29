//****************************************************************************
// COBDataStructs.h
// Author: Derek Meek
// Data Structures for reading/editing/writing COB files - my translations of Frank Krueger's example along with the official
// documentation from Caligari
//****************************************************************************

/*
 * $Logfile: /COBDataStructs.h $
 * $Revision: 1.6 $
 * $Date: 2008/02/09 23:01:40 $
 * $Author: kazan $
 *
 * $Log: COBDataStructs.h,v $
 * Revision 1.6  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.5  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.4  2007/10/18 15:30:09  kazan
 * fixed a metric ton of memory leaks by finally deprecating COB_String and replacing all instances with std::strings
 *
 * Revision 1.3  2007/07/10 20:48:23  kazan
 * Implement PMF -> COB
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

#ifndef _COBDataStructs_h_
#define _COBDataStructs_h_
#include "vector3d.h"
#include <string>

struct COB_Header
{
	char signature[9]; //"Caligari "
	char version[6];   // V00.01
	char mode; //A for ASCII, B for BINARY - I will accept binary files only
	char edian[2]; //LH = Little Endian, HL = Big Endian: Intel machines and Caligari files are LH
	char reserved[13]; //Nulls
	char newline; //A New Line Char.. i presume with must be the structure terminator
};

struct COB_ChunkHeader
{
	char type[4]; // Chunk Signature
	short MajVer; // Major Version
	short MinVer; // Minor Version
	int ChunkID; //obvious
	int ParentID; //obvious
	int size; // Data size of chunk
};

// deprecated
/*struct COB_String
{
	short length;
	char *str; // no NULL .. just like a POF's string

};*/

struct COB_Name
{
	short DupCount; //Numeric ID of this duplicate ie the X part of "ObjectName,X"
	std::string name; //The ObjectName part of "ObjectName,X"
};

struct COB_Axis
{
	vector3d center;
	vector3d XAxisDirection;
	vector3d YAxisDirection;
	vector3d ZAxisDirection;
};

struct COB_Matrix
{
	float RowOne[4];
	float RowTwo[4];
	float RowThree[4];
	//float RowFour[4]; // Never written to file assumed to be [0,0,0,1] 

};

// Begin Chunks

//Ingored Chunks: 'Audi' 'Axes' 'Came' 'Chan' 'CImg' 'DDiv'  'DefP' 'Dims' 'EyeC' 'Grou' 'Hidn' 'Join' 'Mba1' 'MRec' 'Nail'
// Ignored cont: 'OPar'  'OPDf'  'OPP1' 'PANT' 'Path' 'PhAn' 'PhAs'  'PhWd' 'PrTx' 'ROpt' 'SADf' 'Scen' 'URL ' 'VCol' 'Vews'
// Chunk: 'END ' catches the end of file

// Chunks to Handle: 

//*************************************************************************************************************

struct COB_Lght
{
	COB_ChunkHeader head;
	COB_Name name;
	COB_Axis local_axis;
	COB_Matrix CurPos;
	// Light Data
	short type; //1 - infinite, 2 - local, 4 - spot light
	short shadows; //1 - shadows, 0 - no shadows

	// If shadows == 0
	
		short shad_type; //shadow type: 1 - ray traced, 2 - shadow map, 4 - memory map

			// If shad_type == 2 || shad_type == 4
			short shad_res; // Shadow Map Resolution;
			// if Minor Version > 3
			short shad_sharpness; //1 - 3 low - medium

	vector3d RGB; // it's a Float[3] i may as well use a vector.. this is the normalized RGBA of the light
	float angle; // Angle illuminted by light (degrees)
	float hotspot; //Hot Spot of the light (degrees)
	short falloff; // 0 -constant
			

};

//*************************************************************************************************************

#define TXTR_OVERLAY	0x01	// (bit 0)
#define TXTR_USE		0x02	// (bit 1)

struct COB_Mat1
{
	COB_ChunkHeader head;

	// material data

	short Mat_num; //material number;	|2
	char shader_type; //'f' - flat, 'p' - phong, 'm' - metal |3
	char facet_type; //'f' - facted, 'a' autofacet, 's' smooth |4
	char autoface_angle; //0 to 179 degrees |5
	float R; // Red Part: 0.0 to 1.0 |9
	float G; // Green Part: 0.0 to 1.0 |13
	float B; // Blue Part: 0.0 to 1.0 |17
	float opacity; //0.0 to 1.0 |21
	float ambience; // Ambient Coeffecient; 0.0 to 1.0 |25
	float specular; // << Coef. 0.0-1.0 |29
	float hilight; // << Coef, 0.0-1.0 |33
	float refraction_index; //37

	// Extended data
	char Ext_id[2]; // "e:" for envir //39
					// "t:" for texture - only we use
					// 'b:' for bump maps
	char flags; // Ex data flags //40


	//texture map data - these is what we need

	std::string TexMap_Fname; // filename of texmap // 42 + length
	float U_offset; // 46 + length
	float V_offset; // 50 + length
	float U_reps;	// 54 + length
	float V_reps;	// 58 + length



};


//*************************************************************************************************************
struct UV_Vert
{
	float u;
	float v;
};

struct Face_Vert
{
	int local_vert;
	int uv_vert;
};

#define F_HOLE		0x08	// (bit 3)
#define F_BACKCULL	0x10	// (bit 4)


struct PolH_Face_Hole
{
	char flags;
	
	// F_Hole !set
	short Material_index; //Reference to a Material ('Mat1')
	// cont normal

	std::vector<Face_Vert> verts;	//Face vertices.  Each consists of a long index in 
						//the local Vertex list followed by a long index into the UV Vertex list
};


struct COB_PolH
{
	COB_ChunkHeader head;
	COB_Name name;
	COB_Axis local_axis;
	COB_Matrix CurPos;
	
	//Local Vertex list
	std::vector<vector3d> verts; // guess what!

	//UV Vertex list
	std::vector<UV_Vert> uv_verts;

	//Face list
	std::vector<PolH_Face_Hole> fhs;

	int GetSize();
};


struct COB_Grou
{
	COB_ChunkHeader head;
	COB_Name name;
	COB_Axis local_axis;
	COB_Matrix CurPos;
};


struct COB_Unit
{
	COB_ChunkHeader head;
	short flag;
};

//*************************************************************************************************************

struct COB_Scen
{
	// major 0, minor 1, type 'Scen' - think i can sneak by with a type 1 and save myself time
	COB_ChunkHeader head;
	COB_Name name;
	COB_Name selobject;

	// ver = 2;
	/*
	float background_RGBA[4];
	COB_Name background_bitmap;
	float env_RGB[3];
	COB_Name env_map;
	short fog_enabled; //1=enable, 0=disabled
	float fog_RGB[3];
	float fog_near_clip;
	float fog_far_Clip;*/
};
//*************************************************************************************************************


#endif // _COBDataStructs_h_
