//****************************************************************************
// COBHandler.cpp
// Author: Derek Meek
// Implementation of the COB Handler class 
//****************************************************************************

/*
 * $Logfile: /COBHandler.cpp $
 * $Revision: 1.12 $
 * $Date: 2008/02/09 22:28:08 $
 * $Author: kazan $
 *
 * $Log: COBHandler.cpp,v $
 * Revision 1.12  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.11  2008/01/17 19:08:03  kazan
 * Fix COB saving
 *
 * Revision 1.10  2007/10/18 15:30:09  kazan
 * fixed a metric ton of memory leaks by finally deprecating COB_String and replacing all instances with std::strings
 *
 * Revision 1.9  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.8  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.7  2007/06/18 01:15:01  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.6  2007/03/25 17:04:14  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.5  2007/03/22 13:15:31  kazan
 * MSVC8 compatability checkin, requires wxWidgets 2.8
 *
 * Revision 1.4  2006/04/07 11:57:05  kazan
 * sine _WIN32 !defined checks
 *
 * Revision 1.3  2005/09/19 21:47:12  kazan
 * Linux compat changes
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

#if !defined(_WIN32)
#include "compat/filelength.h"
#include "compat/strncmp.h"
#endif
#include "COBHandler.h"
#include <cstring>
#include <cstdio>//for file_length

#ifdef _WIN32
#include <io.h>  //for file_length
#include <fcntl.h>// for filesize function
#endif

#include <memory.h>
#include <fstream>
#include <boost/scoped_array.hpp>

template<typename T>
void write_to_stream(std::ofstream& out, const T& value) {
	out.write((const char*)&value, sizeof(T));
}

int COB_PolH::GetSize()
{
	int size = sizeof(COB_Matrix) + sizeof(COB_Axis) + 4 + name.name.length();
	size += (verts.size() * sizeof(vector3d) ) + ( uv_verts.size() * sizeof(UV_Vert));
	for (unsigned int i = 0; i < fhs.size(); i++)
	{
		size += 5 + (fhs[i].verts.size() * sizeof(Face_Vert));
	}
	return size;
}

//****************************************************************************************************************
int COB::SaveCOB(std::ofstream &OutFile, bool isScene) //Binary Mode
{
	unsigned int i, j;
	int UChunkID = 1, itemp;
	COB_ChunkHeader end;
	end.ChunkID = 0;
	end.ParentID = 0;
	end.MajVer = 1;
	end.MinVer = 0;
	end.size = 0;
	strncpy(end.type, "END ", 4); 

	COB_Unit Unit;
	Unit.flag = 2; // meters;

	Unit.head.MajVer = 0;
	Unit.head.MinVer = 1;
	Unit.head.size = 2;
	strncpy(Unit.head.type, "Unit", 4);


	OutFile.write((const char *) &header, sizeof(COB_Header));

	/*
	// Scen chunk
	if (isScene)
	{
		COB_Scen scenedata;
		scenedata.head.ChunkID = 1;
		scenedata.head.ParentID = 0;
		scenedata.head.MajVer = 0;
		scenedata.head.MinVer = 1;
		strncpy(scenedata.head.type, "Scen", 4);

		scenedata.name.DupCount = 0;
		scenedata.name.name = APStoCOBString("nameless");

		scenedata.selobject.DupCount = 0;
		scenedata.selobject.name = APStoCOBString("");

		scenedata.head.size = sizeof(COB_ChunkHeader) + 6 + scenedata.name.name.length + scenedata.selobject.name.length;

		OutFile.write((const char *)&scenedata.head, sizeof(COB_ChunkHeader));

		OutFile.write((const char *)&scenedata.name.DupCount, sizeof(short));
		OutFile.write((const char *)&scenedata.name.name.length, sizeof(short));
		OutFile.write((const char *)scenedata.name.name.str, scenedata.name.name.length);

		OutFile.write((const char *)&scenedata.selobject.DupCount, sizeof(short));
		OutFile.write((const char *)&scenedata.selobject.name.length, sizeof(short));
		OutFile.write((const char *)scenedata.selobject.name.str, scenedata.selobject.name.length);
	}
*/
	//Groups first
	
	for (i = 0; i < groups.size(); i++)
	{
		OutFile.write((const char *)&groups[i].head, sizeof(COB_ChunkHeader));
		OutFile.write((const char *)&groups[i].name.DupCount, sizeof(short));
		itemp = groups[i].name.name.length();
		OutFile.write((const char *)&itemp, sizeof(short));
		OutFile.write((const char *)groups[i].name.name.c_str(), itemp);
		OutFile.write((const char *)&groups[i].local_axis, sizeof(COB_Axis));
		OutFile.write((const char *)&groups[i].CurPos, sizeof(COB_Matrix));
	}

	// Lights Second
	for (i = 0; i < lights.size(); i++)
	{
		lights[i].head.size = 4; // COB_Name DupCount and COB_Name::std::string length;
		lights[i].head.size += lights[i].name.name.length();
		lights[i].head.size += (sizeof(COB_Axis) + sizeof(COB_Matrix));
		lights[i].head.size += 4; //type and shadows
		if (lights[i].shadows == 1)
		{
			lights[i].head.size += 2; // shad_type

			if (lights[i].shad_type == 2 || lights[i].shad_type == 4)
			{
				lights[i].head.size += 2; //shad_res
			
			}
			if (lights[i].head.MinVer > 3)
			{
				lights[i].head.size += 2; //shad_sharpness
				
			}

		}
		lights[i].head.size += 22; // RGB, angle, hotspot and falloff

		OutFile.write((const char *)&lights[i].head, sizeof(COB_ChunkHeader));
		OutFile.write((const char *)&lights[i].name.DupCount, sizeof(short));

		itemp = lights[i].name.name.length();
		OutFile.write((const char *)&itemp, sizeof(short));
		OutFile.write((const char *)lights[i].name.name.c_str(), itemp);

		OutFile.write((const char *)&lights[i].local_axis, sizeof(COB_Axis));
		OutFile.write((const char *)&lights[i].CurPos, sizeof(COB_Matrix));
		OutFile.write((const char *)&lights[i].type, sizeof(short));
		OutFile.write((const char *)&lights[i].shadows, sizeof(short));

		if (lights[i].shadows == 1)
		{
			OutFile.write((const char *)&lights[i].shad_type, sizeof(short));

			if (lights[i].shad_type == 2 || lights[i].shad_type == 4)
			{
				OutFile.write((const char *)&lights[i].shad_res, sizeof(short));
				
			}
			if (lights[i].head.MinVer > 3)
			{
				OutFile.write((const char *)&lights[i].shad_sharpness,  sizeof(short));
				
			}

		}

		OutFile.write((const char *)&lights[i].RGB, sizeof(vector3d));	//12
		OutFile.write((const char *)&lights[i].angle, sizeof(float));	//4
		OutFile.write((const char *)&lights[i].hotspot, sizeof(float));	//4
		OutFile.write((const char *)&lights[i].falloff, sizeof(short));	//2

	}

	// Polymodels Third
	for (i = 0; i < polymodels.size(); i++)
	{
		polymodels[i].head.size = 8 + polymodels[i].name.name.length() + sizeof(COB_Axis) + sizeof(COB_Matrix);
		polymodels[i].head.size += (4 + (sizeof(vector3d) * polymodels[i].verts.size()));
		polymodels[i].head.size += (4 + (sizeof(UV_Vert) * polymodels[i].uv_verts.size()));

		for (j = 0; j < polymodels[i].fhs.size(); j++)
		{
			polymodels[i].head.size += 3; //flags and num_verts

			if (polymodels[i].fhs[j].flags != F_HOLE && polymodels[i].fhs[j].flags != (F_HOLE | F_BACKCULL))
			{
				polymodels[i].head.size += 2; //Material_index
			}
			polymodels[i].head.size += (sizeof(Face_Vert) * polymodels[i].fhs[j].verts.size());

		}

		OutFile.write((const char *)&polymodels[i].head, sizeof(COB_ChunkHeader));

		OutFile.write((const char *)&polymodels[i].name.DupCount, sizeof(short));
		
		itemp = polymodels[i].name.name.length();
		OutFile.write((const char *)&itemp, sizeof(short));
		OutFile.write((const char *)polymodels[i].name.name.c_str(), itemp);

		OutFile.write((const char *)&polymodels[i].local_axis, sizeof(COB_Axis));

		OutFile.write((const char *)&polymodels[i].CurPos, sizeof(COB_Matrix));

		write_to_stream(OutFile, (int)polymodels[i].verts.size());
		OutFile.write((const char *)&polymodels[i].verts.front(), sizeof(vector3d) * polymodels[i].verts.size());

		write_to_stream(OutFile, (int)polymodels[i].uv_verts.size());
		OutFile.write((const char *)&polymodels[i].uv_verts.front(), sizeof(UV_Vert) * polymodels[i].uv_verts.size());

		write_to_stream(OutFile, (int)polymodels[i].fhs.size());

		for (j = 0; j < polymodels[i].fhs.size(); j++)
		{
			OutFile.write((const char *)&polymodels[i].fhs[j].flags, sizeof(char));
			write_to_stream(OutFile, (short)polymodels[i].fhs[j].verts.size());

			if (polymodels[i].fhs[j].flags != F_HOLE && polymodels[i].fhs[j].flags != (F_HOLE | F_BACKCULL))
			{
				OutFile.write((const char *)&polymodels[i].fhs[j].Material_index, sizeof(short));
			}
			OutFile.write((const char *)&polymodels[i].fhs[j].verts.front(), sizeof(Face_Vert) * polymodels[i].fhs[j].verts.size());

		}

		//Unit For this
		Unit.head.ChunkID = UChunkID;
		UChunkID++;
		Unit.head.ParentID = polymodels[i].head.ChunkID;

		//OutFile.write((const char *) &Unit, 22); //22 is 20=sizeof(COB_ChunkHeader) + short

	}
	// Textures last

	for (i = 0; i < materials.size(); i++)
	{
		OutFile.write((const char *)&materials[i].head, sizeof(COB_ChunkHeader));
		OutFile.write((const char *)&materials[i].Mat_num, sizeof(short));
		OutFile.write((const char *)&materials[i].shader_type, sizeof(char));
		OutFile.write((const char *)&materials[i].facet_type, sizeof(char));
		OutFile.write((const char *)&materials[i].autoface_angle, sizeof(char));
		OutFile.write((const char *)&materials[i].R, sizeof(float));
		OutFile.write((const char *)&materials[i].G, sizeof(float));
		OutFile.write((const char *)&materials[i].B, sizeof(float));
		OutFile.write((const char *)&materials[i].opacity, sizeof(float));
		OutFile.write((const char *)&materials[i].ambience, sizeof(float));
		OutFile.write((const char *)&materials[i].specular, sizeof(float));
		OutFile.write((const char *)&materials[i].hilight, sizeof(float));
		OutFile.write((const char *)&materials[i].refraction_index, sizeof(float));
		OutFile.write((const char *)materials[i].Ext_id, sizeof(char) * 2);
		OutFile.write((const char *)&materials[i].flags, sizeof(char));

		itemp = materials[i].TexMap_Fname.length();
		OutFile.write((const char *)&itemp, sizeof(short));
		OutFile.write((const char *)materials[i].TexMap_Fname.c_str(), itemp);

		OutFile.write((const char *)&materials[i].U_offset, sizeof(float));
		OutFile.write((const char *)&materials[i].V_offset, sizeof(float));
		OutFile.write((const char *)&materials[i].U_reps, sizeof(float));
		OutFile.write((const char *)&materials[i].V_reps, sizeof(float));
		
	}

	OutFile.write((const char *)&end, sizeof(COB_ChunkHeader));

	return 0;
}

//****************************************************************************************************************


int COB::LoadCOB(std::string filename)
{

	int FileSize;
	boost::scoped_array<char> Membuffer;
	char *MemWalker;

	//int fhan;
	//if ((fhan = open(filename.c_str(), O_RDONLY)) == -1)
	//	return -5; //No Such File, or No Permission
	//else
	//{
	//	FileSize = filelength(fhan);
	//	close(fhan);
	//}


	std::ifstream InFile;
	InFile.open(filename.c_str(), std::ios::in | std::ios::binary);
	if (!InFile)
		return -5;
	InFile.seekg(0, std::ios::end);
	FileSize = InFile.tellg();
	InFile.seekg(0, std::ios::beg);

	Membuffer.reset(new char[FileSize]);
	MemWalker = Membuffer.get();
	InFile.read(Membuffer.get(), FileSize);
	InFile.close();

	COB_ChunkHeader CHead;
	COB_Mat1 temp_mat;

	
	memcpy(&header, MemWalker, sizeof(COB_Header));
	MemWalker += sizeof(COB_Header);


	if (_strnicmp(header.signature, "Caligari ", 9))
		return -1; // Signature Fail
	
	if (_strnicmp(header.version, "V00.01", 6))
		return -2; // Version Fail
	
	if (header.mode != 'B')
		return -3; // Mode Fail

	if (_strnicmp(header.edian, "LH", 2))
		return -4; // Endian Fail

	while (1)
	{
		if (MemWalker - Membuffer.get() >= FileSize)
			break; //hit filesize end

		memcpy(&CHead, MemWalker, sizeof(COB_ChunkHeader));
		MemWalker += sizeof(COB_ChunkHeader);

		//cout << CHead.type[0] << CHead.type[1] << CHead.type[2] << CHead.type[3]; //being safe - no null terminator
		//cout << " | " << CHead.MajVer << " | " << CHead.MinVer << " | " << CHead.size << " | ";
		//cout << CHead.ParentID << " | " << CHead.ChunkID << endl;

		if (!_strnicmp(CHead.type, "END ", 4))
			break; // EOF

		else if (!_strnicmp(CHead.type, "Grou", 4))
		{
			Add_Grou(BinParse_Grous(CHead, MemWalker));
			MemWalker += CHead.size;

		}

		else if (!_strnicmp(CHead.type, "Lght", 4))
		{

			Add_Lght(BinParse_Light(CHead, MemWalker));
			MemWalker += CHead.size;
		}

		else if (!_strnicmp(CHead.type, "Mat1", 4))
		{
			if (BinParse_Material(CHead, MemWalker, &temp_mat)) // only want Mat1's that are Textures.. 
														  //it will return false it it's not
			{
				Add_Mat1(temp_mat);
			} //dont need an else condition.. because if &temp_mat isn't a texture type it doesn't allocate any resources
			MemWalker += CHead.size;

		}

		else if (!_strnicmp(CHead.type, "PolH", 4))
		{
			
			Add_PolH(BinParse_Polys(CHead, MemWalker));

			MemWalker += CHead.size;
		}
		else // A Chunk i ignore
		{
			
			MemWalker += CHead.size;
		}
		
		
	}
	
	return 0; // No Error

}

//****************************************************************************************************************
std::string Parse_COBBString(char *&localptr)
{
	std::string retval;
	short len;

	memcpy(&len, localptr, sizeof(short));
	localptr += sizeof(short);

	boost::scoped_array<char> str(new char[len+1]);
	
	memcpy(str.get(), localptr, len);
	localptr += len;

	str[len] = '\0';
	
	retval = str.get();
	return retval;

}

//****************************************************************************************************************

COB_Lght COB::BinParse_Light(COB_ChunkHeader Head, char *buffer)
{
	COB_Lght MyLight;
	MyLight.head = Head;
	char *localptr = buffer;

	memcpy(&MyLight.name.DupCount, localptr, sizeof(short));
	localptr += sizeof(short);

	MyLight.name.name = Parse_COBBString(localptr);

	memcpy(&MyLight.local_axis, localptr, sizeof(COB_Axis));
	localptr += sizeof(COB_Axis);

	memcpy(&MyLight.CurPos, localptr, sizeof(COB_Matrix));
	localptr += sizeof(COB_Matrix);

	memcpy(&MyLight.type, localptr, sizeof(short));
	localptr += sizeof(short);

	memcpy(&MyLight.shadows, localptr, sizeof(short));
	localptr += sizeof(short);

	if (MyLight.shadows == 1)
	{
		memcpy(&MyLight.shad_type, localptr, sizeof(short));
		localptr += sizeof(short);

		if (MyLight.shad_type == 2 || MyLight.shad_type == 4)
		{
			memcpy(&MyLight.shad_res, localptr, sizeof(short));
			localptr += sizeof(short);
		}
		else
			MyLight.shad_res = 0;

		if (MyLight.head.MinVer > 3)
		{
			memcpy(&MyLight.shad_sharpness, localptr, sizeof(short));
			localptr += sizeof(short);
		}
		else
			MyLight.shad_sharpness = 0;

	}
	else
	{
		MyLight.shad_res = 0;
		MyLight.shad_sharpness = 0;
		MyLight.shad_type = 0;
	}

	memcpy(&MyLight.RGB, localptr, sizeof(vector3d));
	localptr += sizeof(vector3d);

	memcpy(&MyLight.angle, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&MyLight.hotspot, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&MyLight.falloff, localptr, sizeof(short));
	localptr += sizeof(short);

	return MyLight;
}


//****************************************************************************************************************

bool COB::BinParse_Material(COB_ChunkHeader Head, char *buffer, COB_Mat1 *mat)
{
	mat->head = Head;

	char *localptr = buffer;
	

	memcpy(&mat->Mat_num, localptr, sizeof(short));
	localptr += sizeof(short);

	memcpy(&mat->shader_type, localptr, 1);
	localptr++;

	memcpy(&mat->facet_type, localptr, 1);
	localptr++;

	memcpy(&mat->autoface_angle, localptr, 1);
	localptr++;

	memcpy(&mat->R, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->G, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->B, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->opacity, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->ambience, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->specular, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->hilight, localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->refraction_index, localptr, sizeof(float));
	localptr += sizeof(float);


	memcpy(&mat->Ext_id, localptr, 2);
	localptr += 2;

	memcpy(&mat->flags, localptr, 1);
	localptr ++;
	
	if (_strnicmp("t:", mat->Ext_id, 2))
		return false; // Not a valid material;
	
	mat->TexMap_Fname = Parse_COBBString(localptr);

	memcpy(&mat->U_offset , localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->V_offset , localptr, sizeof(float));
	localptr += sizeof(float);

	memcpy(&mat->U_reps , localptr, sizeof(float));
	localptr += sizeof(float);
	
	memcpy(&mat->V_reps , localptr, sizeof(float));
	localptr += sizeof(float);

	return true;
}

//****************************************************************************************************************

COB_PolH COB::BinParse_Polys(COB_ChunkHeader Head, char *buffer)
{
	COB_PolH polh;
	char *localptr = buffer;

	polh.head = Head;

	memcpy(&polh.name.DupCount, localptr, sizeof(short));
	localptr += sizeof(short);

	polh.name.name = Parse_COBBString(localptr);

	memcpy(&polh.local_axis, localptr, sizeof(COB_Axis)); // the center component is the absolute location
	localptr += sizeof(COB_Axis);

	memcpy(&polh.CurPos, localptr, sizeof(COB_Matrix));
	localptr += sizeof(COB_Matrix);

	// Start ----------- Poly Verts
	unsigned int num_verticies;
	memcpy(&num_verticies, localptr, sizeof(int));
	localptr += sizeof(int);

	polh.verts.resize(num_verticies);

	// woohoo blit the memory in one op :)
	memcpy(&polh.verts.front(), localptr, (sizeof(vector3d) * num_verticies));
	localptr += (sizeof(vector3d) * num_verticies);
	// End ----------- Poly Verts

	// Start ----------- UV Verts
	unsigned int num_uv_verts;
	memcpy(&num_uv_verts, localptr, sizeof(int));
	localptr += sizeof(int);

	polh.uv_verts.resize(num_uv_verts);

	// woohoo blit the memory in one op :)
	memcpy(&polh.uv_verts.front(), localptr, (sizeof(UV_Vert) * num_uv_verts));
	localptr += (sizeof(UV_Vert) * num_uv_verts);
	// End ----------- UV Verts

	unsigned int num_faces_or_holes;
	memcpy(&num_faces_or_holes, localptr, sizeof(int));
	localptr += sizeof(int);

	polh.fhs.resize(num_faces_or_holes);

	for (unsigned int i = 0; i < num_faces_or_holes; i++)
	{
		memcpy(&polh.fhs[i].flags, localptr, 1);
		localptr++;
		
		unsigned int num_verts;
		memcpy(&num_verts, localptr, sizeof(short));
		localptr += sizeof(short);

		if (polh.fhs[i].flags != F_HOLE && polh.fhs[i].flags != (F_HOLE | F_BACKCULL))
		{
			memcpy(&polh.fhs[i].Material_index, localptr, sizeof(short));
			localptr += sizeof(short);
			
		}
		else polh.fhs[i].Material_index = -1; //give it a value

		polh.fhs[i].verts.resize(num_verts);

		memcpy(&polh.fhs[i].verts.front(), localptr, sizeof(Face_Vert) * num_verts);
		localptr += (sizeof(Face_Vert) * num_verts);
	}
	return polh;
}

//****************************************************************************************************************

COB_Grou COB::BinParse_Grous(COB_ChunkHeader Head, char *buffer)
{
	COB_Grou group;
	char *localptr = buffer;

	group.head = Head;


	// Read the name
	memcpy(&group.name.DupCount, localptr, sizeof(short));
	localptr += sizeof(short);

	
	group.name.name = Parse_COBBString(localptr);

	//Read The Local Axis

	memcpy(&group.local_axis, localptr, sizeof(COB_Axis)); // the center component is the absolute location
	localptr += sizeof(COB_Axis);

	//Read the CurPos

	memcpy(&group.CurPos, localptr, sizeof(COB_Matrix));
	localptr += sizeof(COB_Matrix);

	return group;
}
//****************************************************************************************************************

void COB::Add_Lght(COB_Lght Lght)
{
	lights.push_back(Lght);
}

bool COB::Del_Lght(unsigned int index)
{
	if (index >= lights.size())
		return false;
	lights.erase(lights.begin() + index);
	return true;
}

//****************************************************************************************************************

void COB::Add_Mat1(COB_Mat1 mat)
{
	materials.push_back(mat);
}

bool COB::Del_Mat1(unsigned int index)
{	
	if (index >= materials.size())
		return false;
	materials.erase(materials.begin() + index);
	return true;
}

//****************************************************************************************************************

void COB::Add_PolH(COB_PolH pol)
{
	polymodels.push_back(pol);
}

bool COB::Del_PolH(unsigned int index)
{
	if (index >= polymodels.size())
		return false;
	polymodels.erase(polymodels.begin() + index);
	return true;
}
//****************************************************************************************************************

void COB::Add_Grou(COB_Grou gru)
{
	groups.push_back(gru);
}


bool COB::Del_Grou(unsigned int index)
{
	if (index >= groups.size())
		return false;
	groups.erase(groups.begin() + index);
	return true;
}

//****************************************************************************************************************
void FindParent_Grou(COB_Grou *groups, int num, int &index, int &type, int SearchId)
{
	index = type = -1;

	for (int i = 0; i < num && SearchId != 0; i++)
	{
		if (groups[i].head.ChunkID == SearchId)
		{
			index = i;
			type = 1;
			break;
		}
	}
}

void FindParent_Lght(COB_Lght *Lights, int num, int &index, int &type, int SearchId)
{
	index = type = -1;

	for (int i = 0; i < num && SearchId != 0; i++)
	{
		if (Lights[i].head.ChunkID == SearchId)
		{
			index = i;
			type = 2;
			break;
		}
	}
}

void FindParent_PolH(COB_PolH *Polhs, int num, int &index, int &type, int SearchId)
{
	index = type = -1;

	for (int i = 0; i < num && SearchId != 0; i++)
	{
		if (Polhs[i].head.ChunkID == SearchId)
		{
			index = i;
			type = 3;
			break;
		}
	}
}


//****************************************************************************************************************
/*
std::string APStoCOBString(std::string aps)
{
	std::string cstr;
	cstr.length = aps.length();
	cstr.str = new char[cstr.length];
	strncpy(cstr.str, aps.c_str(), cstr.length);

	return cstr;
}

std::string COBStringtoAPS(std::string cstr)
{
	std::string aps;
	char *cstemp = new char[cstr.length + 1];
	memset(cstemp, 0, cstr.length + 1);

	strncpy(cstemp, cstr.str, cstr.length);
	aps = cstemp;
	return aps;
}
*/

//****************************************************************************************************************

std::ostream& COB::WriteStats(std::ostream &os)
{
	unsigned int i;

	for (i = 0; i < materials.size(); i++)
	{
		os << "@Texture[" << i << "]: " << COBStringtoAPS(materials[i].TexMap_Fname) << std::endl;
		os << "     +ParentID: " << materials[i].head.ParentID << ", ChunkID: " << materials[i].head.ChunkID << std::endl;
		os << "     +U_reps: " << materials[i].U_reps << ", V_reps: " << materials[i].V_reps << std::endl;

	}

	for (i = 0; i < lights.size(); i++)
	{
		os << "@Light[" << i << "]: " << COBStringtoAPS(lights[i].name.name) << std::endl;
		os << "     +ParentID: " << lights[i].head.ParentID << ", ChunkID: " << lights[i].head.ChunkID << std::endl;
	}

	for (i = 0; i < polymodels.size(); i++)
	{
		os << "@PolyModel[" << i << "]: " << COBStringtoAPS(polymodels[i].name.name) << " #Faces: " << polymodels[i].fhs.size() << std::endl;
		os << "     +ParentID: " << polymodels[i].head.ParentID << ", ChunkID: " << polymodels[i].head.ChunkID << std::endl;
		os << "     +NumVerticies: " << polymodels[i].verts.size() << std::endl;
		os << "     +NumUVVerts: " << polymodels[i].uv_verts.size() << std::endl;

		for (unsigned int j = 0; j < polymodels[i].fhs.size(); j++)
		{
			if (polymodels[i].fhs[j].flags != F_HOLE && polymodels[i].fhs[j].flags != (F_HOLE | F_BACKCULL))
			{
				os << "     $Verts[" << j << "]: Material Index: " << polymodels[i].fhs[j].Material_index << std::endl;
			}

		}
	}

	for (i = 0; i < groups.size(); i++)
	{
		os << "@Group[" << i << "]: " << COBStringtoAPS(groups[i].name.name) << std::endl;
		os << "     +ParentID: " << groups[i].head.ParentID << ", ChunkID: " << groups[i].head.ChunkID << std::endl;

	}

	return os;
}





//****************************************************************************************************************

COB::~COB()
{
	Reset();
}


void COB::Reset()
{
	materials.clear();
	groups.clear();
	lights.clear();
	polymodels.clear();
}

//****************************************************************************************************************
int COB::FindParentGrou(int ParentID)
{

	for (unsigned int i = 0; i < groups.size(); i++)
	{
		if (ParentID == groups[i].head.ChunkID)
			return i;
	}
	return -1;
}

//****************************************************************************************************************

std::string StripFileName(std::string fullname)
//takes fullname of a fill including path - ie d:\textures\sometexture.tga
//and gets just sometexture from it
// [cobs store them the first way, pofs the second]
{




	char CString[256]; // 256 should be more than fine for a filename
	int len = fullname.length();
	int i, start, stop = 0;
	bool IsStartSet = false, IsStopSet = false;

	memset(CString, 0, 256);

	if (strchr(fullname.c_str(), '\\') == NULL)
	{
		start = 0;
		IsStartSet = true;
	}
	for (i = (len-1); (!IsStartSet || !IsStopSet) && i > -1; i--) //find the start and stop of the sometexture part
	{
		if (fullname[i] == '.' && !IsStopSet)
		{
			IsStopSet = true;
			stop = i-1;
		}

		if (fullname[i] == '\\' && !IsStartSet)
		{
			IsStartSet = true;
			start = i+1;
		}
	}


	for (i = start; i <= stop; i++) //copy over
		CString[i - start] = fullname[i];

	return std::string(CString); // return new string
}

//****************************************************************************************************************


int COB::FindMatchingMatl(int ParentID, int MatNum)
// finds the material index with the correct matnum and parent ID in the
// COB::materials array
{
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		if (materials[i].head.ParentID == ParentID && materials[i].Mat_num == MatNum)
			return i; // found it :)
	}
	return -1; //not found, probably a non-texture map texture went here, so we ignore this
}

//****************************************************************************************************************
vector3d TranslateToWorld	(vector3d point, COB_Matrix &matrix, COB_Axis &axis, float scaler)
// PreCondition:	point contains a coordinate
//					matrix contains the translation matrix
//					scaler contains the scaling factor for COB to POF conversion
// PostCondition:	returns the Standard OGL world coordinate [translation applied and axis switched]
{	
	vector3d outvector, tempvec = point;
	std::ofstream Points;
	double x, y, z;
	
	// Local Axis to World Axis - i checked this work, if the local axis are not defined (tempvec == point) is true
	// ok these math is correct.. but it causes problems - in other words IT'S NOT NEEDED, here for REFERENCE
	/*tempvec.x = (point.x * axis.XAxisDirection.x) +
				(point.y * axis.YAxisDirection.x) +
				(point.z * axis.ZAxisDirection.x) +
				+ axis.center.x;
	tempvec.y = (point.x * axis.XAxisDirection.y) +
				(point.y * axis.YAxisDirection.y) +
				(point.z * axis.ZAxisDirection.y) +
				+ axis.center.y;
	tempvec.z = (point.x * axis.XAxisDirection.z) +
				(point.y * axis.YAxisDirection.z) +
				(point.z * axis.ZAxisDirection.z) +
				+ axis.center.y;*/

	// a little matrix algebra here.. thank god for PreCalculus
	/*
	[x][R0-0 R0-1 R0-2 R0-3]
	[y][R1-0 R1-1 R2-2 R2-3]
	[z][R2-0 R2-1 R2-2 R2-3]
	[1][0    0    0    1   ]
	*/
	x = (double(tempvec.x) * double(matrix.RowOne[0])) 
		+ (double(tempvec.y) * double(matrix.RowOne[1])) 
		+ (double(tempvec.z) * double(matrix.RowOne[2])) 
		+ double(matrix.RowOne[3]);

	y = (double(tempvec.x) * double(matrix.RowTwo[0])) 
		+ (double(tempvec.y) * double(matrix.RowTwo[1])) 
		+ (double(tempvec.z) * double(matrix.RowTwo[2])) 
		+ double(matrix.RowTwo[3]);

	z = (double(tempvec.x) * double(matrix.RowThree[0])) 
		+ (double(tempvec.y) * double(matrix.RowThree[1])) 
		+ (double(tempvec.z) * double(matrix.RowThree[2])) 
		+ double(matrix.RowThree[3]);

	//translate into FreeSpace 2 world coordinates and scale by scaler
	outvector.x = float(y * double(scaler));
	outvector.y = float(z * double(scaler));
	outvector.z = float(x * double(scaler));

	return outvector;
}



