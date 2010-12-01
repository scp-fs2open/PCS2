// VPReader.h
// Derek Meek
// For Reading Volition Packfiles associated with freespace 2

/*
 * $Logfile: /VPReader.h $
 * $Revision: 1.11 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: VPReader.h,v $
 * Revision 1.11  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.10  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.9  2008/02/09 22:32:59  kazan
 * bye bye kaz_vector
 *
 * Revision 1.8  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.7  2008/02/04 18:02:44  kazan
 * Dumbest typo that latest forever found while fixing the dumbest bug ever - VolitionPackfileReader::FindFileWild was case sensative.. causing PCS2 not to load textures in VPs when there was a case mismatch
 *
 * Revision 1.6  2007/07/13 11:31:04  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.5  2005/10/07 14:08:06  kazan
 * linux compat syncs - i'm getting memory crashes in linux, hopefully it happens in windows too so it's easier to debug
 *
 * Revision 1.4  2005/08/31 15:29:55  kazan
 * small behavioral fix for the wildcard search
 *
 * Revision 1.3  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.2  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.1  2005/08/28 00:57:41  kazan
 * Initial Import from pcs 1.x - switch to kaz_templates
 *
 *
 *
 */

//#include <iostream.h>
#include <fstream>

#if !defined(_VPREADER_H__)
#define _VPREADER_H__
#include <string>
#include "FileList.h"
#include <memory.h>
#include <boost/scoped_array.hpp>


struct Read_VPHeader
{
	char signature[4];   //"VPVP"
	int version;         //"2"
	int diroffset;       //bytes from beginning of file
	int direntries;      //number of files in directory
};

struct Read_VPDirent
{
 int offset;          //from beginning of file
 int size;
 char filename[32];   //Null-terminated string
 int timestamp;       //The time the file was last modified in seconds since 1.1.1970 0:00
                      // Same as from calling findfirst/findnext file using any C compiler.
};
bool operator<(Read_VPDirent &lhs, Read_VPDirent &rhs);
bool operator>(Read_VPDirent &lhs, Read_VPDirent &rhs);



class VolitionPackfileReader
{
	private:
		// let's use a linked list
		std::vector<Read_VPDirent> Files;
		Read_VPHeader HeaderData;
		std::ifstream VPFile;

		std::string lfname;
		int LoadVPDirectory();
	public:
		VolitionPackfileReader()
			{ memset(&HeaderData, 0, sizeof(Read_VPHeader));  }
		VolitionPackfileReader(std::string filename)
			{ OpenVP(filename); }
		~VolitionPackfileReader()
		{
			if (VPFile.is_open())
				VPFile.close();
			memset(&HeaderData, 0, sizeof(Read_VPHeader));
		}

		bool OpenVP(std::string filename)
		{
			VPFile.open(filename.c_str(), std::ios::binary);
			if (!VPFile)
				return false;

			/* validity checks */

			lfname = filename;
			Files.resize(0);
			LoadVPDirectory();
			VPFile.close();
			return true;
		}

		const Read_VPDirent& GetInfo(int fileno) { return Files[fileno]; }
		
		int FindFile(std::string filename);
		int FindFileWild(std::string pattern, int start=0);

		size_t NumFiles()
			{ return Files.size(); }

		int LoadFile(int fileno, boost::scoped_array<char> &membuffer); // reference to a pointer - Data is ALLOCED AT READ
		
		bool OpenFile(int fileno, std::ifstream &infile);

		int FileSize(int fileno) { return Files[fileno].size; }
		std::string FileName(int fileno);

		void Reset()
		{
			if (VPFile.is_open())
	 			VPFile.close();
			Files.resize(0);
			memset(&HeaderData, 0, sizeof(Read_VPHeader));
		}
		static bool ReadFromVP(std::string vp, std::string filename, std::string destination);
};


#endif
