/***************************************************************************
   FileList.cpp  -  Cross Platform directory list class
                             -------------------

    begin                : Oct 20, 2003
    copyright            : (C) 2000-2003 by Derek Meek
    email                : kazan@cs.iastate.edu

	Author(s)			 : Derek Meek
 ***************************************************************************/

/*
 * $Logfile: /FileList.cpp $
 * $Revision: 1.19 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: FileList.cpp,v $
 * Revision 1.19  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.18  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.17  2008/09/17 19:34:01  kazan
 * little cleanup
 *
 * Revision 1.16  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.15  2008/02/09 22:32:58  kazan
 * bye bye kaz_vector
 *
 * Revision 1.14  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.13  2007/07/02 20:20:29  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.12  2007/06/28 11:56:39  kazan
 * fix #19
 *
 * Revision 1.11  2007/06/25 13:13:15  kazan
 * had to remove bob's path prefix code from pcs2.ini load - was breaking it
 * moved ilutRenderer(ILUT_OPENGL) to fix 256x256 texture bug (#11)
 * added texture serach logging for debug builds
 *
 * Revision 1.10  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.9  2007/05/07 19:15:11  kazan
 * checkin for doing diffs to find bug
 *
 * Revision 1.8  2007/05/07 17:40:55  kazan
 * changes in progress
 *
 * Revision 1.7  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.6  2005/10/07 14:08:06  kazan
 * linux compat syncs - i'm getting memory crashes in linux, hopefully it happens in windows too so it's easier to debug
 *
 * Revision 1.5  2005/08/31 15:29:55  kazan
 * small behavioral fix for the wildcard search
 *
 * Revision 1.4  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.3  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.2  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.1  2005/08/28 00:54:02  kazan
 * Initial Import from ferrium - switch to kaz_templates
 *
 * Revision 1.14  2005/08/11 01:08:10  kazan
 * Making it compile on linux.. still having SDL problems
 *
 * Revision 1.13  2005/03/01 02:53:12  wmcoolmon
 * General stuff. Got rid of a bunch of signed/unsigned and float errors. Made it possible to start Ferrium (without crashing) with just the init files
 *
 * Revision 1.12  2004/09/06 05:57:44  bobboau
 * added a font system, still needs a little work, font defs are should be loaded from file eventualy
 * could use a stream interface as well (uses the printf syntax currently.
 * create a font by passing a texture, then use the returned index when useing
 * the gr_printf function
 * I saw some memory leaks and tryed to use the MSVC mem traker but that
 * didn't work out so well so I erased all of it (an include at the top of every file)
 * but CVS still sees something diferent in them.
 *
 * Revision 1.11  2004/07/23 20:03:49  kazan
 * Big Commit -- New Libs Required: wxWindows, OpenAL, OggVorbis
 *
 * Revision 1.10  2004/07/20 18:34:31  kazan
 * char *mp(0) was causing problems on MSVC - and the 'int c' was redundant
 *
 * Revision 1.9  2004/07/20 18:20:57  thenti
 * Minor fixes for removing warning messages
 *
 * Revision 1.8  2004/07/20 15:33:10  kazan
 * fixed a problem in the linux implementation of FileList
 *
 * Revision 1.7  2004/07/20 15:10:32  kazan
 * added a makefile for linux and altered stuff so it compiles on gcc under Fedora Core
 *
 * Revision 1.6  2004/07/20 02:30:16  kazan
 * more compat stuff
 *
 * Revision 1.5  2004/07/20 01:22:57  kazan
 * fixed compilation on MSVC that was broken by MingGW/GCC porting
 *
 * Revision 1.4  2004/07/20 01:06:00  thenti
 * Added 'std::' where necessary
 *
 * Revision 1.3  2004/07/17 16:47:46  kazan
 * expression stuff, some minor bug fixes in filelist and elsewhere
 *
 * Revision 1.2  2004/07/14 03:12:08  kazan
 * Wooohooo! Filesystem abstraction _DONE_
 *
 * Revision 1.1  2004/07/05 17:52:27  kazan
 * Filesystem abstraction almost finished
 *
 *
 *
 *
 */

#if defined(_WIN32) // win32
#include <windows.h>
#include <wx/msw/winundef.h>
#pragma warning(disable:4786)

#else // !win32

#include <sys/types.h>
#include <dirent.h>
#include <algorithm>

#endif // win32 check

#include "FileList.h"
#include <cstring>
#include <fstream>




std::string strUpper(const std::string &str)
{
	std::string retval = str;
	for (unsigned int i = 0; i < retval.length(); i++)
		retval[i] = retval[i] & 0xDF; //1101111
	return retval;
	//return _strupr(str.c_str());
}

std::string strLower(const std::string &str)
{
	std::string retval = str;
	for (unsigned int i = 0; i < retval.length(); i++)
		retval[i] = retval[i] | 0x20; //00100000
	return retval;
	//return _strlwr(str.c_str());
}

//****************************************************************************

FileList::FileList()
{
	files.resize(0);
}

//****************************************************************************


FileList FileList::operator+=(const FileList &list2)
{
	size_t oldsize = files.size();
	files.resize(oldsize + list2.Size());

	for (size_t i = oldsize; i < files.size(); i++)
	{
		files[i] = list2[i-oldsize];
	}

	return *this;
}

//****************************************************************************

bool FileList::inList(const std::string &filename) const
{
	std::string fname = strLower(filename), curfile;
	for (unsigned int i = 0; i < files.size(); i++)
	{
		curfile = strLower(files[i]);
		if (!strcmp(curfile.c_str(), fname.c_str()))
			return true;
	}

	return false;
}

//****************************************************************************


void FileList::GetList(const std::string &dir, const std::string &filter)
{
	unsigned int numFiles = 0;
	files.resize(50); // good starting number


#if defined(_WIN32)
	// ------------------------------
	// Windows Implementation
	// ------------------------------

	WIN32_FIND_DATA FoundFile;
	memset((char*)&FoundFile, 0, sizeof(WIN32_FIND_DATA));
	HANDLE DirHandle;

	std::string SearchPattern = dir + "\\" + filter ;
	DirHandle = FindFirstFile(SearchPattern.c_str(), &FoundFile);

	if(DirHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(FoundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && std::string(FoundFile.cFileName) != "")
			{
				if (numFiles >= files.size())
					files.resize(files.size()*2);

				files[numFiles] = FoundFile.cFileName;
				numFiles++;

			}

		}
		while (FindNextFile(DirHandle, &FoundFile));
		FindClose(DirHandle);
	}


#else
	// ------------------------------
	// Linux Implementation
	// ------------------------------
	DIR* mydir;
	mydir = opendir(dir.c_str());

	dirent *curfile;
	std::string lfilter;
    std::transform(filter.begin(), filter.end(), lfilter.begin(), tolower);
	std::string dname;
	if (mydir)
	{

		while ((curfile = readdir(mydir)) != NULL)
		{
			dname = curfile->d_name;
			std::transform(dname.begin(), dname.end(), dname.begin(), tolower);
			if (wildcmp((char*)lfilter.c_str(), (char*)dname.c_str()))
			{
				if (numFiles >= files.size())
					files.resize(files.size()*2);

				files[numFiles] = curfile->d_name;
				numFiles++;
			}
		}

		closedir(mydir);
	}
#endif
	listeddir = dir;
	files.resize(numFiles);
}



//****************************************************************************
int FileList::new_search = -1;

size_t FileList::wild_search(std::string search, size_t start) const
{
	search = strLower(search);
	std::string temp;
	for (size_t i = start+1; i < files.size(); i++)
	{
		temp = strLower(files[i]);
		if (wildcmp((char*)search.c_str(), (char*)temp.c_str()))
			return i;
	}
	return -1; //invalid
}


//****************************************************************************


// used for resolving case-insensative paths to case-sensative ones
// when appropriate IsCaseSensative() = true
bool PlatformNormalize(std::string &normpath, const std::string &path, const std::string root)
{
	if (!FileList::IsCaseSensative()) // windows simply returns identity
	{
		// should only get hit on windows
		if (root[root.length()-1] != '\\')
		{
			normpath = root + "\\" + path;
		}
		else
		{
			normpath = root + path;
		}
		return true;
	}
	// else it's a *nix system - normalize it to / instead of \  -
	FileList curdir;
	normpath = root;
	// break path down into it's componants

	std::vector<std::string> parts;
	Expand(path, '/',  parts);
	bool found;
	for (size_t i = 0; i < parts.size(); i++)
	{
		curdir.GetList(normpath);
		found = false;

		for (size_t j = 0; j < curdir.Size(); j++)
		{
			if (strLower(std::string(curdir[j])) ==
			    strLower(std::string(parts[i])))
			{
				normpath += std::string("/") + curdir[j];
				found = true;
				break;
			}
		}

		if (!found)
			return false; //yikes

		curdir.Clear();
	}

	return true;
}


//****************************************************************************

size_t getFilesize(std::string filename)
{
  std::ifstream is;
  is.open (filename.c_str(), std::ios::binary );

  // get length of file:
  is.seekg (0, std::ios::end);
  size_t length = is.tellg();
  is.close();

  return length;
}

//****************************************************************************


size_t Expand(std::string str, char c,  std::vector<std::string> &StrArray)
// Breaks str into vector of std::strings cutting it appart at occurences of c
{
        size_t i=0, splits = 0;
        StrArray.resize(1);
        size_t len = str.length();

        StrArray[0] = "";
	if (str[0] == c) //ignore leading c
		i++;
	if (str[len-1] == c) //ignore trailing c
		len--;

        for (; i < len; i++)
        {
                if (str[i] != c)
                        StrArray[splits] = StrArray[splits] + str[i];
                if (str[i] == c)
                {
                        splits++;
                        StrArray.resize(splits + 1);

                }

        }
        return splits + 1;

}

//****************************************************************************
// Shamelessly borrowed from http://www.codeproject.com/string/wildcmp.asp
//****************************************************************************

size_t wildcmp(char *wild, char *string) {
   char *cp = NULL;
   char *mp = NULL;

	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}

	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}



