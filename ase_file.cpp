//****************************************************************************
//	ase_file.cpp
//	ASCII Scene Exporter File Format
//  This is from 3DS Max, 
//  Maya supports as well (with ActorX plugin)
//  Blender (>2.44) has support
//****************************************************************************

/*
 * $Logfile: /ase_file.cpp $
 * $Revision: 1.10 $
 * $Date: 2008/02/12 18:59:50 $
 * $Author: kazan $
 *
 * $Log: ase_file.cpp,v $
 * Revision 1.10  2008/02/12 18:59:50  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.9  2008/02/11 13:19:40  kazan
 * Few tweaks to the loader from what i learned by loading the Sol City ASE
 *
 * Revision 1.8  2008/02/10 20:53:21  kazan
 * more ASE work, do some reorganization of the project filters
 *
 * Revision 1.7  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.6  2008/02/10 00:19:38  kazan
 * just read straight from disk instead of doing olympics
 *
 * Revision 1.5  2008/02/09 23:58:33  kazan
 * some work on ASE files
 *
 * Revision 1.4  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.3  2008/02/09 22:31:26  kazan
 * finish removing string
 *
 * Revision 1.2  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.1  2008/02/09 18:34:07  kazan
 * Stub headers and source files for ASE and OBJ file formats
 *
 *
 *
 */

#include "ase_file.h"
#include <climits>
#include <map>
#include <fstream>
#include <stack>
#include <sstream>

using namespace ase;
using namespace std;


vector3d ase::ASE_Translate_Vector(vector3d vec)
{
	vector3d retval = vec;
	retval.x = -retval.x;
	return retval;
}


//****************************************************************************
// Misc Functions
//****************************************************************************

bool ase_file::validSubmat(int matid, int submatid)
{
	if ((unsigned int)matid > material_list.size())
		throw ase_exception("Matid out of range");
	
	return matid != 0 && ((unsigned int)submatid)-1 < material_list[matid].submats.size();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

material& ase_file::resolveMaterialReference(int matid, int submatid)
{
	if ((unsigned int)matid > material_list.size())
		throw ase_exception("Matid out of range");
	material &curmat = material_list[matid];

	if (validSubmat(matid, submatid))
		return curmat.submats[((unsigned int)submatid)-1];

	// either submatid ==0, or it's invalid - return the main material
	return curmat;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string ase_file::getTextureName(int matid, int submatid)
{
	material &mat = resolveMaterialReference(matid, submatid);
	if (submatid > 0)
	{
		if (mat.diffuse_map.bitmap != "")
			return mat.diffuse_map.bitmap;
		// else change to parent material
		mat = resolveMaterialReference(matid, 0);
	}
	return mat.diffuse_map.bitmap;
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int ase_file::getObjectParent(size_t idx)
{
	geomobject& obj = getObject(idx);
	if (obj.parent != "")
		for (size_t i = 0; i < objects.size(); i++)
			if (obj.parent == objects[i].name)
				return i;

	return -1;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int ase_file::find_matching_curlybrace(int start_offset, const std::string &str)
{
	int depth = 0;
	for (unsigned int i = start_offset; i < str.size(); i++)
	{
		if (str[i] == '{')
			depth++;
		if (str[i] == '}')
		{
			if (depth == 0)
				return i;
			depth--;
		}
	}
	return -1; // fail
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void ase_file::reset()
{
	comment = "";
	myScene = scene();
	material_list.resize(0);
	objects.resize(0);
	helper_objects.resize(0);
}


//****************************************************************************
// Loader Macros
//****************************************************************************

#define ASE_In_Expect(in,str,tag) in >> str; if (str != tag) throw ase_exception(string("Expected Value \"") + string(tag) + string("\" Not found, found \"") + str + string("\" instead."));

#define ASE_In_Read(var) in >> var;

#define ASE_In_Var(tag,dst,vname) if (vname == tag) { in >> dst; continue; }
#define ASE_In_Bool(tag,dst,vname) if (vname == tag) { dst = true; continue; }
#define ASE_In_2Var(tag,dst1,dst2,vname) if (vname == tag) { in >> dst1 >> dst2; continue; }
#define ASE_In_2VarArray(tag,dst1,dst2,vname,vec1,vec2) if (vname == tag) { in >> dst1 >> dst2; vec1.push_back(dst1); vec2.push_back(dst2); continue; }

#define ASE_In_QuotedStr(tag,dst,vname) if (vname == tag) { in.ignore(10, '"'); getline(in,dst,'"'); continue; }
#define ASE_In_UnquotedStr(tag,dst,vname) ASE_In_Var(tag,dst,vname)
#define ASE_In_If(tag,vname) if (vname == tag) {
#define ASE_In_Else continue; } else {
#define ASE_In_Endif continue; }

#define ASE_In_BreakIf(tag,vname) if (vname == tag) break;
#define ASE_In_UnexpectedVar(section,vname) throw ase_exception(string("Unrecognized ") + string(section) + string(" Variable: ") + vname);

#define ASE_In_Start_VarProcessing(in,endtag,vname) string vname; while (in >> vname) { ASE_In_BreakIf(endtag,vname) 
#define ASE_In_End_VarProcessing(section,vname) ASE_In_UnexpectedVar(section,vname) }

//****************************************************************************
// Loader
//****************************************************************************


bool ase_file::LoadFromFile(std::string &filename)
{
	ifstream in(filename.c_str(), ios::in | ios::binary);
	if (!in)
		throw ase_exception(string("Could not open file ") + filename.c_str());

	// ----------------------------------------------
	int i = 0;
	string sTemp, sTemp2;

	in >> sTemp;

	if (sTemp == "*3DSMAX_ASCIIEXPORT")
		getline(in, sTemp);
	else
	{
		throw ase_exception(string(filename.c_str()) + " is not an ASE file");
	}

	while (in >> sTemp)
	{
		
		if (sTemp == "*COMMENT")
		{
			in.ignore(INT_MAX, '"');
			getline(in, sTemp, '"');
			this->comment = sTemp;
		}
		else if (sTemp == "*SCENE" || sTemp == "*MATERIAL_LIST" || sTemp == "*GEOMOBJECT" || sTemp == "*HELPEROBJECT")
		{
			ASE_In_Expect(in,sTemp2,"{")
			//iTemp = int(in.tellg())-1;
			//size = brace_pairs[iTemp];
			//sTemp2 = load_between_braces(in, size);
			
			if (sTemp == "*SCENE")
			{
				myScene.read(in);
			}
			else if (sTemp == "*MATERIAL_LIST")
			{
				parse_materials(in);
			}
			else if (sTemp == "*GEOMOBJECT")
			{
				geomobject gmTemp;
				gmTemp.read(in);
				objects.push_back(gmTemp);
			}
			else if (sTemp == "*HELPEROBJECT")
			{
				helperobject hoTemp;
				hoTemp.read(in);
				helper_objects.push_back(hoTemp);
			}
		}
		else
		{
			throw ase_exception(string("Unrecognized ASE Block: ") + sTemp);
		}
	}

	return true;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string ase_file::load_between_braces(std::istream &in, int size)
{
	std::string str;

	if (size > 0)
	{
		char *buffer = new char[size];
		memset(buffer, 0, size);
		in.read(buffer, size-1);
		in.ignore(1); // the other brace

		str = buffer;
		delete[] buffer;
	}

	return str;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void ase_file::parse_materials(std::istream &in)
{
	string sTemp;
	int i, mat_count, itemp;

	ASE_In_Expect(in,sTemp,"*MATERIAL_COUNT");

	in >> mat_count;
		
	for (i = 0; i < mat_count; i++)
	{
		ASE_In_Expect(in,sTemp,"*MATERIAL");
		ASE_In_Read(itemp)
		ASE_In_Expect(in,sTemp,"{");

		material mat_temp;
		mat_temp.read(in);

		material_list.push_back(mat_temp);
	}
	ASE_In_Expect(in,sTemp,"}");

}

//****************************************************************************
// Saver
//****************************************************************************

bool ase_file::SaveToFile(std::string &filename)
{
	throw ase_exception("ASE SaveToFile NOT IMPLEMENTED");
	return false;
}



//****************************************************************************
// DataStruct Functions
//****************************************************************************

void scene::read(std::istream &in)
{
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*SCENE_FILENAME",filename,sTemp)
		ASE_In_Var("*SCENE_FIRSTFRAME",first_frame,sTemp)
		ASE_In_Var("*SCENE_LASTFRAME",last_frame,sTemp)
		ASE_In_Var("*SCENE_FRAMESPEED",frame_speed,sTemp)
		ASE_In_Var("*SCENE_TICKSPERFRAME",ticks_per_frame,sTemp)
		ASE_In_Var("*SCENE_BACKGROUND_STATIC",background_rgb,sTemp)
		ASE_In_Var("*SCENE_AMBIENT_STATIC",ambient_rgb,sTemp)	
	ASE_In_End_VarProcessing("*SCENE",sTemp)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string scene::write()
{
	string retval;

	return retval;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void texture_map::read(std::istream &in)
{
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*MAP_NAME",name,sTemp)
		ASE_In_QuotedStr("*MAP_CLASS",mapclass,sTemp)
		ASE_In_Var("*MAP_SUBNO",subno,sTemp)
		ASE_In_Var("*MAP_AMOUNT",amount,sTemp)
		ASE_In_QuotedStr("*BITMAP",bitmap,sTemp)
		ASE_In_UnquotedStr("*MAP_TYPE",type,sTemp)
		ASE_In_Var("*UVW_U_OFFSET",u_offset,sTemp)
		ASE_In_Var("*UVW_V_OFFSET",v_offset,sTemp)
		ASE_In_Var("*UVW_U_TILING",u_tiling,sTemp)
		ASE_In_Var("*UVW_V_TILING",v_tiling,sTemp)
		ASE_In_Var("*UVW_ANGLE",angle,sTemp)
		ASE_In_Var("*UVW_BLUR",blur,sTemp)
		ASE_In_Var("*UVW_BLUR_OFFSET",blur_offset,sTemp)
		ASE_In_Var("*UVW_NOUSE_AMT",noise_amt,sTemp)
		ASE_In_Var("*UVW_NOISE_SIZE",noise_size,sTemp)
		ASE_In_Var("*UVW_NOISE_LEVEL",noise_level,sTemp)
		ASE_In_Var("*UVW_NOISE_PHASE",noise_phase,sTemp)
		ASE_In_UnquotedStr("*BITMAP_FILTER",filter,sTemp)
	ASE_In_End_VarProcessing("*MAP",sTemp)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string texture_map::write()
{
	string retval;

	return retval;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void material::read(std::istream &in)
{
	int itemp;
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*MATERIAL_NAME",name,sTemp)
		ASE_In_QuotedStr("*MATERIAL_CLASS",matclass,sTemp)
		ASE_In_Var("*MATERIAL_AMBIENT",ambient,sTemp)
		ASE_In_Var("*MATERIAL_DIFFUSE",diffuse,sTemp)
		ASE_In_Var("*MATERIAL_SPECULAR",specular,sTemp)
		ASE_In_Var("*MATERIAL_SHINE",shine,sTemp)
		ASE_In_Var("*MATERIAL_SHINESTRENGTH",shine_strength,sTemp)
		ASE_In_Var("*MATERIAL_TRANSPARENCY",transparency,sTemp)
		ASE_In_Var("*MATERIAL_WIRESIZE",wiresize,sTemp)
		ASE_In_UnquotedStr("*MATERIAL_SHADING",shading,sTemp)
		ASE_In_Var("*MATERIAL_XP_FALLOFF",xp_falloff,sTemp)
		ASE_In_Var("*MATERIAL_SELFILLUM",self_illum,sTemp)
		ASE_In_UnquotedStr("*MATERIAL_FALLOFF",falloff,sTemp)
		ASE_In_UnquotedStr("*MATERIAL_XP_TYPE",xp_type,sTemp)
		ASE_In_Var("*NUMSUBMTLS",itemp,sTemp)
		ASE_In_Bool("*MATERIAL_TWOSIDED",twosided,sTemp)

		ASE_In_If("*MAP_DIFFUSE",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			diffuse_map.read(in);
		ASE_In_Endif

		ASE_In_If("*MAP_SPECULAR",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			specular_map.read(in);
		ASE_In_Endif

		ASE_In_If("*MAP_SHINE",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			shine_map.read(in);
		ASE_In_Endif

		ASE_In_If("*MAP_BUMP",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			bump_map.read(in);
		ASE_In_Endif

		ASE_In_If("*MAP_SELFILLUM",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			shine_map.read(in);
		ASE_In_Endif

		ASE_In_If("*SUBMATERIAL",sTemp)
			ASE_In_Read(itemp)
			ASE_In_Expect(in,sTemp,"{")
			material submat;
			submat.read(in);
			submats.push_back(submat);
		ASE_In_Endif
	
	ASE_In_End_VarProcessing("*MATERIAL",sTemp)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string material::write()
{
	string retval;

	return retval;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void transform_matrix::read(std::istream &in)
{
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*NODE_NAME",name,sTemp)
		ASE_In_Var("*INHERIT_POS",inherit_pos,sTemp)
		ASE_In_Var("*INHERIT_ROT",inherit_rot,sTemp)
		ASE_In_Var("*INHERIT_SCL",inherit_scl,sTemp)
		ASE_In_Var("*TM_ROW0",row1,sTemp)
		ASE_In_Var("*TM_ROW1",row1,sTemp)
		ASE_In_Var("*TM_ROW2",row2,sTemp)
		ASE_In_Var("*TM_ROW3",row3,sTemp)
		ASE_In_Var("*TM_POS",tm_pos,sTemp)
		ASE_In_Var("*TM_ROTAXIS",tm_rotaxis,sTemp)
		ASE_In_Var("*TM_ROTANGLE",tm_rotangle,sTemp)
		ASE_In_Var("*TM_SCALE",scale,sTemp)
		ASE_In_Var("*TM_SCALEAXIS",scale_axis,sTemp)
		ASE_In_Var("*TM_SCALEAXISANG",scale_axis_angle,sTemp)
	ASE_In_End_VarProcessing("*NODE_TM",sTemp)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string transform_matrix::write()
{
	string retval;

	return retval;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void mesh::read(std::istream &in)
{
	int numvertex=0, numfaces=0, numtvert=0, numtface=0, itemp;
	vector3d vTemp;
	vector<int> bitbox; // trash
	mesh_face mftemp;
	std::string strlist;
	texture_face tftemp;

	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_Var("*TIMEVALUE",timevalue,sTemp)
		ASE_In_Var("*MESH_NUMVERTEX",numvertex,sTemp)
		ASE_In_Var("*MESH_NUMFACES",numfaces,sTemp)

		// ------------------------- *MESH_VERTEX_LIST  ------------------------- 
		ASE_In_If("*MESH_VERTEX_LIST",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			ASE_In_Start_VarProcessing(in,"}",sTemp2)
				// *MESH_VERTEX    7	62.5000	62.5000	80.0000
				ASE_In_2VarArray("*MESH_VERTEX",itemp,vTemp,sTemp2,bitbox,vertex_list)
			ASE_In_End_VarProcessing("*MESH_VERTEX_LIST",sTemp2)
		ASE_In_Endif
		// ------------------------- *MESH_VERTEX_LIST  -------------------------

		// ------------------------- *MESH_FACE_LIST  ------------------------- 
		ASE_In_If("*MESH_FACE_LIST",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			ASE_In_Start_VarProcessing(in,"}",sTemp3)
				//*MESH_FACE   11:    A:    4 B:    6 C:    2 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 7 	*MESH_MTLID 2
				ASE_In_If("*MESH_FACE",sTemp3)
					ASE_In_Read(itemp)
					ASE_In_Expect(in,sTemp3,":")

					ASE_In_Expect(in,sTemp3,"A:")
					ASE_In_Read(mftemp.vert0)

					ASE_In_Expect(in,sTemp3,"B:")
					ASE_In_Read(mftemp.vert1)

					ASE_In_Expect(in,sTemp3,"C:")
					ASE_In_Read(mftemp.vert2)

					ASE_In_Expect(in,sTemp3,"AB:")
					ASE_In_Read(mftemp.unknownAB)

					ASE_In_Expect(in,sTemp3,"BC:")
					ASE_In_Read(mftemp.unknownBC)

					ASE_In_Expect(in,sTemp3,"CA:")
					ASE_In_Read(mftemp.unknownCA)

					ASE_In_Expect(in,sTemp3,"*MESH_SMOOTHING")
					ASE_In_Read(strlist)

					// sometimes the list is empty
					if (strlist != "*MESH_MTLID")
					{
						stringstream smooth_in(strlist);

						while (smooth_in >> itemp)
							mftemp.smoothing_groups.push_back(itemp);

						ASE_In_Expect(in,sTemp3,"*MESH_MTLID")
						ASE_In_Read(mftemp.submaterial_id)
					}
					else
					{
						ASE_In_Read(mftemp.submaterial_id)
					}
					face_list.push_back(mftemp);
				ASE_In_Endif
				ASE_In_Var("*MESH_FACE",itemp,sTemp3)		
			ASE_In_End_VarProcessing("*MESH_FACE_LIST",sTemp3)
		ASE_In_Endif
		// ------------------------- *MESH_FACE_LIST  ------------------------- 

		ASE_In_Var("*MESH_NUMTVERTEX",numtvert,sTemp)

		// ------------------------- *MESH_TVERTLIST  ------------------------- 
		ASE_In_If("*MESH_TVERTLIST",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			ASE_In_Start_VarProcessing(in,"}",sTemp4)
				//*MESH_TVERT 11	1.0000	1.0000	0.0000
				ASE_In_2VarArray("*MESH_TVERT",itemp,vTemp,sTemp4,bitbox,texture_vertex_list)
			ASE_In_End_VarProcessing("*MESH_TVERTLIST",sTemp4)
		ASE_In_Endif
		// ------------------------- *MESH_TVERTLIST  -------------------------

		ASE_In_Var("*MESH_NUMTVFACES",numtface,sTemp)

		// ------------------------- *MESH_TFACELIST  ------------------------- 
		ASE_In_If("*MESH_TFACELIST",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			ASE_In_Start_VarProcessing(in,"}",sTemp5)
				// *MESH_TFACE 11	3	2	0
				ASE_In_If("*MESH_TFACE",sTemp5)
					ASE_In_Read(itemp)
					ASE_In_Read(tftemp.a)
					ASE_In_Read(tftemp.b)
					ASE_In_Read(tftemp.c)
					texture_face_list.push_back(tftemp);
				ASE_In_Endif
			ASE_In_End_VarProcessing("*MESH_TFACELIS",sTemp5)
		ASE_In_Endif
		// ------------------------- *MESH_TFACELIST  -------------------------

		// ------------------------- *MESH_NORMALS  ------------------------- 
		ASE_In_If("*MESH_NORMALS",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			ASE_In_Start_VarProcessing(in,"}",sTemp6)
				ASE_In_If("*MESH_FACENORMAL",sTemp6)
					//*MESH_FACENORMAL 0	0.0000	0.0000	-1.0000
					ASE_In_Read(itemp)
					ASE_In_Read(vTemp)
					normals.push_back(vTemp);
				
					// three of
					// *MESH_VERTEXNORMAL 0	0.0000	0.0000	-1.0000
					for (int mni = 0; mni < 3; mni++)
					{
						ASE_In_Read(sTemp6)
						ASE_In_Read(itemp);
						ASE_In_Read(vTemp);
					}

				ASE_In_Endif
			ASE_In_End_VarProcessing("*MESH_NORMALS",sTemp6)
		ASE_In_Endif
		// ------------------------- *MESH_NORMALS  -------------------------

	ASE_In_End_VarProcessing("*MESH",sTemp)
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

ase_compat_face mesh::fetchCompatface(int facenum)
{
	if ((unsigned int)facenum > face_list.size())
		throw ase_exception("Invalid face index");

	ase_compat_face tface;
	tface.verts.resize(3);

	// points
	tface.verts[0].point = ASE_Translate_Vector(vertex_list[face_list[facenum].vert0]);
	tface.verts[1].point = ASE_Translate_Vector(vertex_list[face_list[facenum].vert1]);
	tface.verts[2].point = ASE_Translate_Vector(vertex_list[face_list[facenum].vert2]);

	// facenormal
	tface.normal = ASE_Translate_Vector(normals[facenum]);

	//uv 
	tface.verts[0].u = texture_vertex_list[texture_face_list[facenum].a].x;
	tface.verts[0].v = 1-texture_vertex_list[texture_face_list[facenum].a].y;

	tface.verts[1].u = texture_vertex_list[texture_face_list[facenum].b].x;
	tface.verts[1].v = 1-texture_vertex_list[texture_face_list[facenum].b].y;

	tface.verts[2].u = texture_vertex_list[texture_face_list[facenum].c].x;
	tface.verts[2].v = 1-texture_vertex_list[texture_face_list[facenum].c].y;

	return tface;
}


std::vector<ase_compat_face> mesh::getCompatFaces()
{
	std::vector<ase_compat_face> faces;
	faces.resize(face_list.size());

	for (unsigned int i = 0; i < faces.size(); i++)
		faces[i] = fetchCompatface(i);
	return faces;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string mesh::write()
{
	string retval;

	return retval;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void geomobject::read(std::istream &in)
{
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*NODE_NAME",name,sTemp)
		ASE_In_QuotedStr("*NODE_PARENT",parent,sTemp)

		ASE_In_If("*NODE_TM",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			tm.read(in);
		ASE_In_Endif

		ASE_In_If("*MESH",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			geometry.read(in);
		ASE_In_Endif

		ASE_In_Var("*PROP_MOTIONBLUR",motionblur,sTemp)
		ASE_In_Var("*PROP_CASTSHADOW",cast_shadow,sTemp)
		ASE_In_Var("*PROP_RECVSHADOW",recv_shadow,sTemp)
		ASE_In_Var("*MATERIAL_REF",material_ref,sTemp)
	ASE_In_End_VarProcessing("*NODE_TM",sTemp)

}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string geomobject::write()
{
	string retval;

	return retval;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void helperobject::read(std::istream &in)
{
	ASE_In_Start_VarProcessing(in,"}",sTemp)
		ASE_In_QuotedStr("*NODE_NAME",name,sTemp)
		ASE_In_QuotedStr("*NODE_PARENT",parent,sTemp)
		ASE_In_QuotedStr("*HELPER_CLASS",helpclass,sTemp)

		ASE_In_If("*NODE_TM",sTemp)
			ASE_In_Expect(in,sTemp,"{")
			tm.read(in);
		ASE_In_Endif

		ASE_In_Var("*BOUNDINGBOX_MIN",bbox_min,sTemp)
		ASE_In_Var("*BOUNDINGBOX_MAX",bbox_max,sTemp)
	ASE_In_End_VarProcessing("*NODE_TM",sTemp)

}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::string helperobject::write()
{
	string retval;

	return retval;
}
