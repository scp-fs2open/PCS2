//****************************************************************************
//	ase_file.h
//	ASCII Scene Exporter File Format
//  This is from 3DS Max, 
//  Maya supports as well (with ActorX plugin)
//  Blender (>2.44) has support
//
// Documented at http://www.unrealwiki.com/wiki/ASE_In_File_Format
//				http://www.solosnake.com/main/ase.htm
//****************************************************************************

/*
 * $Logfile: /ase_file.h $
 * $Revision: 1.11 $
 * $Date: 2008/02/12 18:59:50 $
 * $Author: kazan $
 *
 * $Log: ase_file.h,v $
 * Revision 1.11  2008/02/12 18:59:50  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.10  2008/02/11 13:19:42  kazan
 * Few tweaks to the loader from what i learned by loading the Sol City ASE
 *
 * Revision 1.9  2008/02/10 20:53:22  kazan
 * more ASE work, do some reorganization of the project filters
 *
 * Revision 1.8  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.7  2008/02/10 00:19:38  kazan
 * just read straight from disk instead of doing olympics
 *
 * Revision 1.6  2008/02/09 23:58:33  kazan
 * some work on ASE files
 *
 * Revision 1.5  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.4  2008/02/09 22:32:59  kazan
 * bye bye kaz_vector
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

#if !defined(_ase_file_h_)
#define _ase_file_h_

#include "ase_dstructs.h"
#include <iostream>
#include <string>

namespace ase
{
	class ase_exception : public std::exception
	{
		private:
			std::string msg;

		public:
			ase_exception(std::string amsg) : msg(amsg) {}
			virtual const char* what() const throw()
			{
				return msg.c_str();
			}
			~ase_exception() throw ()  {}
	};

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	class ase_file
	{
		private:
			std::string comment;
			scene myScene;
			std::vector<material> material_list;
			std::vector<geomobject> objects;
			std::vector<helperobject> helper_objects;
			
			int find_matching_curlybrace(int start_offset, const std::string &str);

			// parsing functions
			std::string load_between_braces(std::istream &in, int size);

			void parse_materials(std::istream &in);
		public:
			~ase_file() { reset(); }
			void reset();

			bool LoadFromFile(std::string &filename);
			bool SaveToFile(std::string &filename);
	
			// material functions
			bool validSubmat(int matid, int submatid);
			material& resolveMaterialReference(int matid, int submatid);
			std::string getTextureName(int matid, int submatid);

			size_t getMaterialCount() { return material_list.size(); }
			material& getMaterial(size_t idx) { if (idx > getMaterialCount()) { throw ase_exception("Invalid Material Index"); } return material_list[idx]; }

			// object functions
			size_t getObjectCount() { return objects.size(); }
			geomobject& getObject(size_t idx) { if (idx > getObjectCount()) { throw ase_exception("Invalid Object Index"); } return objects[idx]; }
			int getObjectParent(size_t idx);
		
	};

vector3d ASE_Translate_Vector(vector3d vec);

}; //ase namespace
#endif //_ase_file_h_