//****************************************************************************
// BSPDataStructs.h
// Author: Derek Meek [using Descent Developer Network reference materials]
// Structures for the BSP Data
//****************************************************************************

/*
 * $Logfile: /BSPDataStructs.h $
 * $Revision: 1.6 $
 * $Date: 2008/02/09 23:01:40 $
 * $Author: kazan $
 *
 * $Log: BSPDataStructs.h,v $
 * Revision 1.6  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.5  2008/02/09 22:32:57  kazan
 * bye bye kaz_vector
 *
 * Revision 1.4  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.3  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
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

#include "vector3d.h"
#include <memory.h>
#include <string>

#if !defined(_BSP_DATA_STRUCTS_H_)
#define _BSP_DATA_STRUCTS_H_

#define byte unsigned char


struct BSP_BlockHeader
{
	int id;
	//0 - EOF, 1 - DEFPOINTS, 2 - FLATPOLY, 3 - IMAPPOLY, 4- SORTNORM, 5- BOUNDBOX
	int size; //number of bytes in this structure instance - dynamic

	//---------------------------------------
	int Read(char *buffer);
	int Write(char *buffer);
	int MySize()
		{ return 8; }
};

//********************************************************************************************************************

struct vertdata
{
	vector3d vertex;
	vector3d *norms;
};

struct BSP_DefPoints // 1 - Defines Verticies
{
	BSP_BlockHeader head;			//8			|8
	int n_verts;					//4			|12
	int n_norms;					//4			|16
	int offset;						//4			|20			|from start of chunk to vertex data
	unsigned char *norm_counts;		//n_norms	|20+n_norms	|n_verts in size;
	// DM: I don't know WTF data size/format the vertex data is in.. so im GUESSING it's vertex point followed by vertex norms
    // DM: Ok after staring at that insanely bad code written by Knudson i finally made sense of it.. and YES im correct :)
	vertdata *vertex_data;			//Equasion	|20+n_norms+Equasion
	//char *vertex_data // at startofmemory+BSP::DefPoints.offset; Each vertex n is a point followed by norm_counts[n] normals. 

	//---------------------------------------
	BSP_DefPoints() { memset(this, 0, sizeof(BSP_DefPoints)); }
	int Read(char *buffer, BSP_BlockHeader hdr);
	int Write(char *buffer);
	int MySize();
	void Destroy();
};

//********************************************************************************************************************

struct Flat_vertex
{
   short vertnum; //these are indecies into the vertex arrays
   short normnum;

	//---------------------------------------
	int Read(char *buffer);
	int Write(char *buffer);
	int MySize()
		{ return 4; }
};

struct BSP_FlatPoly //2 - FLATPOLY - Flat (non-textured) polygon
{                          
	BSP_BlockHeader head;	//8		|8
	vector3d normal;			//12	|20
	vector3d center;			//12	|32
	float radius;			//4		|36
	int nverts;				//4		|40
	byte red;				//1		|41
	byte green;				//1		|42
	byte blue;				//1		|43
	byte pad;				//1		|44
	Flat_vertex *verts;		

  	//---------------------------------------
	int Read(char *buffer, BSP_BlockHeader hdr);
	int Write(char *buffer);
	int MySize()
		{	return 44 + (4 * nverts);  }
	void Destroy()
		{ delete[] verts; }
	float MyRadius(vector3d center, std::vector<vector3d> Verts);
	vector3d MyCenter(std::vector<vector3d> Verts);
};

//********************************************************************************************************************
struct Tmap_vertex
{
	short vertnum; //these are indecies into the vertex arrays
	short normnum;
	float u;
	float v;

   	//---------------------------------------
	int Read(char *buffer);
	int Write(char *buffer);
	int MySize()
		{ return 12; }

};

struct BSP_TmapPoly //3 - TMAPPOLY - Textured polygons
{ 
	BSP_BlockHeader head;	//8		|8		|0
	vector3d normal;		//12	|20		|8
	vector3d center;		//12	|32		|20
	float radius;			//4		|36		|32
	int nverts;				//4		|40		|36
	int tmap_num;			//4		|44		|40
	Tmap_vertex *verts;		//12 * nverts

	//---------------------------------------
	int Read(char *buffer, BSP_BlockHeader hdr);
	int Write(char *buffer);
	int MySize()
		{ return ((12 * nverts) + 44); }
	void Destroy()
		{ delete[] verts; }
	float MyRadius(vector3d center, std::vector<vector3d> Verts);
	vector3d MyCenter(std::vector<vector3d> Verts);

};

//********************************************************************************************************************


struct BSP_SortNorm //4 - SORTNORM - Sortnorms are planes that split the model recursively
{
	BSP_BlockHeader head;			//8		|8
	vector3d plane_normal;			//12	|20
	vector3d plane_point;				//12	|32
	int reserved;					//4		|36 set to 0
	int front_offset;				//4		|40 Only recurse into this if non-zero.
	int back_offset;				//4		|44 Only recurse into this if non-zero.
	int prelist_offset;				//4		|48 Only recurse into this if non-zero.
	int postlist_offset;			//4		|52 Only recurse into this if non-zero.
	int online_offset;				//4		|56 Only recurse into this if non-zero.
	vector3d min_bounding_box_point;	//12	|68 of all polys under here
	vector3d max_bounding_box_point;	//12	|80 of all polys under here 

	//---------------------------------------
	BSP_SortNorm() : reserved(0), front_offset(0), back_offset(0), prelist_offset(0), postlist_offset(0), online_offset(0) {}
	int Read(char *buffer, BSP_BlockHeader hdr);
	int Write(char *buffer);
	int MySize()
		{ return 80; }
};

//********************************************************************************************************************
struct BSP_BoundBox //5 - BOUNDBOX - Bounding boxes are used to speed up lighting and collision calculations.
{
	BSP_BlockHeader head; //8	|8
	vector3d min_point;	  //12	|20
	vector3d max_point;	  //12	|32

	//---------------------------------------
	int Read(char *buffer, BSP_BlockHeader hdr);
	int Write(char *buffer);
	int MySize()
		{ return 32; }
};

#endif // _BSP_DATA_STRUCTS_H_
