// VPReader.cpp
// Derek Meek
// For Reading Volition Packfiles associated with freespace 2

/*
 * $Logfile: /VPReader.cpp $
 * $Revision: 1.12 $
 * $Date: 2008/02/09 23:01:41 $
 * $Author: kazan $
 *
 * $Log: VPReader.cpp,v $
 * Revision 1.12  2008/02/09 23:01:41  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.11  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.10  2008/02/04 18:02:44  kazan
 * Dumbest typo that latest forever found while fixing the dumbest bug ever - VolitionPackfileReader::FindFileWild was case sensative.. causing PCS2 not to load textures in VPs when there was a case mismatch
 *
 * Revision 1.9  2007/07/13 11:31:04  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.8  2007/06/28 11:56:39  kazan
 * fix #19
 *
 * Revision 1.7  2007/03/22 13:15:33  kazan
 * MSVC8 compatability checkin, requires wxWidgets 2.8
 *
 * Revision 1.6  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.5  2006/04/07 11:57:05  kazan
 * sine _WIN32 !defined checks
 *
 * Revision 1.4  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.3  2005/08/31 15:29:55  kazan
 * small behavioral fix for the wildcard search
 *
 * Revision 1.2  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.1  2005/08/28 00:57:41  kazan
 * Initial Import from pcs 1.x - switch to kaz_templates
 *
 *
 *
 */

#if !defined(_WIN32)
#include "compat/filelength.h"
#include "compat/strncmp.h"
#endif
#include "VPReader.h"
#include <cstring>
#include <fstream>

//***********************************************************************************************************************

std::string VolitionPackfileReader::FileName(int fileno)
{ 
	std::string t = Files[fileno].filename;
	return t; 
}


int VolitionPackfileReader::LoadVPDirectory()
{
	VPFile.seekg(0);
	VPFile.read((char *) &HeaderData, sizeof(Read_VPHeader));

	if (_strnicmp(HeaderData.signature, "VPVP", 4))
		return -1; // bad signature
	
	if (HeaderData.version != 2)
		return -2; // bad version

	VPFile.seekg(0);
	VPFile.seekg(HeaderData.diroffset);
	Read_VPDirent DirentTemp;
	Files.resize(HeaderData.direntries);

	int i;
	for (i = 0; i < HeaderData.direntries; i++)
	{
		VPFile.read((char *) &DirentTemp, sizeof(Read_VPDirent));
		Files[i] = DirentTemp;

		if (VPFile.eof())
			break;
	}
	if (VPFile.eof())
	{
		if (i < HeaderData.direntries)
			HeaderData.direntries = i;
	}
	VPFile.seekg(0);
	return HeaderData.direntries;
}

//***********************************************************************************************************************


int VolitionPackfileReader::FindFileWild(std::string pattern, int start)
{
	
	for (int i = start+1; i < HeaderData.direntries; i++)
	{
#ifdef UNIX
#define _strlwr(s) (s)
#endif
		if (wildcmp(_strlwr((char*)pattern.c_str()), _strlwr(Files[i].filename)))
#ifdef UNIX
#undef _strlwr
#endif
			return i;
	}

	return -1;
}

//***********************************************************************************************************************

int VolitionPackfileReader::FindFile(std::string filename)
{

	for (int i = 0; i < HeaderData.direntries; i++)
	{
		if (!_strnicmp(filename.c_str(), Files[i].filename, 32))
			return i;
	}

	return -1;
}

//***********************************************************************************************************************

int VolitionPackfileReader::LoadFile(int fileno, char* &membuffer)
{
	std::ifstream readfile(lfname.c_str(), std::ios::binary);

	if (!readfile)
		return -2; //cannot open file

	membuffer = NULL;
	if (fileno < 0 || fileno > HeaderData.direntries)
		return -1; // bad index;

	membuffer = new char[Files[fileno].size];
	readfile.seekg(0, std::ios::beg);
	readfile.seekg(Files[fileno].offset, std::ios::beg);

	readfile.read(membuffer, Files[fileno].size);

	readfile.close();
	return Files[fileno].size;
}

//***********************************************************************************************************************

bool VolitionPackfileReader::OpenFile(int fileno, std::ifstream &infile)
{
	infile.open(lfname.c_str(), std::ios::binary);
	infile.seekg(0, std::ios::beg);
	infile.seekg(Files[fileno].offset, std::ios::beg);

	return true;
}

//***********************************************************************************************************************

bool operator<(Read_VPDirent &lhs, Read_VPDirent &rhs)
{
	return lhs.timestamp < rhs.timestamp;
}

//***********************************************************************************************************************

bool operator>(Read_VPDirent &lhs, Read_VPDirent &rhs)
{
	return lhs.timestamp > rhs.timestamp;
}

bool VolitionPackfileReader::ReadFromVP(std::string vp, std::string filename, std::string destination)
{
	using namespace std;
	VolitionPackfileReader VPR(vp);
	int filenum = VPR.FindFile(filename);
	if (filenum != -1)
    {
	    char *membuffer;
		VPR.LoadFile(filenum, membuffer); // loadfile allocates the required size of memory
      
		ofstream outfile(destination.c_str(), ios::out | ios::binary);
	    outfile.write(membuffer, VPR.FileSize(filenum));
	    outfile.close();

		delete[] membuffer;
	    return true;
    }
	return false;
}
