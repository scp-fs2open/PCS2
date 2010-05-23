//****************************************************************************
//	pcs_pmf_ase.cpp
//	POF Constructor Suite 2 File Format: Load/Save function to ASE
//****************************************************************************

/*
 * $Logfile: /pcs_pmf_ase.cpp $
 * $Revision: 1.3 $
 * $Date: 2008/11/10 19:04:08 $
 * $Author: kazan $
 *
 * $Log: pcs_pmf_ase.cpp,v $
 * Revision 1.3  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.2  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.1  2008/05/27 22:12:51  kazan
 * *** empty log message ***
 *
 *
 *
 */

#include "pcs_file.h"
#include "ase_file.h"

extern std::string StripFileName(std::string fullname); // from COBHandler
//****************************************************************************
// Loader
//****************************************************************************
/*

int PCS_Model::LoadFromASE(std::string filename, int &comp_target, int &comp_current, char *comp_msg_str, float scaler)
{
	ase::ase_file InFile;
	unsigned int i,j,k;
	//float fTemp;
	comp_target = 2;
	comp_current = 0;
	sprintf(comp_msg_str, "Loading ASE File into Memory.");
	try {
		InFile.LoadFromFile(filename);
	}
	catch (ase::ase_exception e)
	{
		strcpy(comp_msg_str, e.what());
		return 1;
	}
	sprintf(comp_msg_str, "Writing Geometry");
	comp_target = 2 + InFile.getObjectCount();
	
	for (i = 0; i < InFile.getObjectCount(); i++)
	{
		sprintf(comp_msg_str, "Converting GeomObject (%02d/%02d)", (i+1), InFile.getObjectCount());
		comp_current++;
		pcs_sobj sobj_temp;

		std::vector<ase::ase_compat_face> faces = InFile.getObject(i).geometry.getCompatFaces();
		for (j = 0; j < faces.size(); j++)
		{
			ase::material& mat = InFile.resolveMaterialReference(InFile.getObject(i).material_ref, InFile.getObject(i).geometry.face_list[j].submaterial_id);
			pcs_polygon poly;
			poly.norm = faces[j].normal;
			poly.texture_id = maybeAddTexture(StripFileName(InFile.getTextureName(InFile.getObject(i).material_ref, InFile.getObject(i).geometry.face_list[j].submaterial_id)));

			for (k = 0; k < 3; k++)
			{
				pcs_vertex vert;
				vert.point = faces[j].verts[k].point;
				vert.u = faces[j].verts[k].u * mat.diffuse_map.u_tiling;
				vert.v = faces[j].verts[k].v * mat.diffuse_map.v_tiling;
				vert.norm = faces[j].normal;
				poly.verts.push_back(vert);
			}
			sobj_temp.polygons.push_back(poly);
		}
		sobj_temp.parent_sobj = InFile.getObjectParent(i);
		sobj_temp.name = InFile.getObject(i).name;
		sobj_temp.geometric_center = ase::ASE_Translate_Vector(InFile.getObject(i).tm.tm_pos);
		
		if (sobj_temp.parent_sobj != -1)
			sobj_temp.offset = sobj_temp.geometric_center - ase::ASE_Translate_Vector(InFile.getObject(sobj_temp.parent_sobj).tm.tm_pos);

		sobj_temp.GenerateBBoxes();
		sobj_temp.GenerateRadius();

		subobjects.push_back(sobj_temp);

		if (sobj_temp.parent_sobj == -1)
			AddLOD(GetSOBJCount()-1);
	}
	comp_current++;
	return 0;
}

//****************************************************************************
// Saver
//****************************************************************************


int PCS_Model::SaveToASE(std::string filename, int &comp_target, int &comp_current, char *comp_msg_str, float scaler)
{
	return 1;
}
*/