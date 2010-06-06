/***************************************************************************
   FileList.h  -  Cross Platform directory list class
                             -------------------

    begin                : Oct 20, 2003
    copyright            : (C) 2000-2003 by Derek Meek
    email                : kazan@cs.iastate.edu

	Author(s)			 : Derek Meek
 ***************************************************************************/

/*
 * $Logfile: /FileList.h $
 * $Revision: 1.15 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: FileList.h,v $
 * Revision 1.15  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.14  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.13  2008/09/17 19:34:01  kazan
 * little cleanup
 *
 * Revision 1.12  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.11  2008/02/09 22:32:58  kazan
 * bye bye kaz_vector
 *
 * Revision 1.10  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.9  2007/07/02 20:20:33  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.8  2007/06/28 11:56:39  kazan
 * fix #19
 *
 * Revision 1.7  2007/05/07 19:15:11  kazan
 * checkin for doing diffs to find bug
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
 * Revision 1.3  2004/07/20 15:10:32  kazan
 * added a makefile for linux and altered stuff so it compiles on gcc under Fedora Core
 *
 * Revision 1.2  2004/07/14 03:12:08  kazan
 * Wooohooo! Filesystem abstraction _DONE_
 *
 * Revision 1.1  2004/07/05 17:52:27  kazan
 * Filesystem abstraction almost finished
 *
 *
 */


#if !defined(_FILELIST_H_)
#define _FILELIST_H_


#if defined(_WIN32) // win32
#include <windows.h>
#include <wx/msw/winundef.h>
#pragma warning(disable:4786)

#else // !win32

#include <sys/types.h>
#include <dirent.h>

#endif // win32 check

#include <string>
#include <vector>

class FileList
{
	private:
		std::vector<std::string> files;
		std::string listeddir;

	public:
		static int new_search;
		bool inList(const std::string &filename) const;

		FileList();
		FileList(const std::string &dir, const std::string &filter = "*") { GetList(dir, filter); }

		void GetList(const std::string &dir, const std::string &filter = "*");

		void Clear() { files.resize(0); }
		size_t Size() const { return files.size(); }

		const std::string& operator[](size_t index) const { return files[index]; }
		FileList operator+=(const FileList &list2);

		size_t wild_search(std::string search, size_t start=0) const;

		static bool IsCaseSensative()
		{
#if defined(_WIN32)
			return false;
#else
			return true;
#endif
		}

		std::string getDir() const { return listeddir; }



};

// used for resolving case-insensative paths to case-sensative ones
// when appropriate IsCaseSensative() = true
// root can be fed an already-normalized path to resolve from
bool PlatformNormalize(std::string &normpath, const std::string &path, const std::string root = "");

// really only used in the non-windows version, but it's useful
size_t wildcmp(const char *wild, const char *string);

// could be useful
size_t getFilesize(std::string filename);
size_t Expand(std::string str, char c,  std::vector<std::string> &StrArray);

// *Grubmle* apparently stricmp isn't standard
std::string strUpper(const std::string &str);
std::string strLower(const std::string &str);


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// returns whether a value exists in the array
template <class vec_type> bool in_array(const std::vector<vec_type> &vec, const vec_type &val)
{
	for(unsigned int i = 0; i< vec.size(); i++)
	{
		if (vec[i] == val)
		return true;
	}
	return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// returns whether a value exists in the array
template <class vec_type> int find(const std::vector<vec_type> &vec, const vec_type &val)
{
 	for(unsigned int i = 0; i<vec.size(); i++)
	{
		if (vec[i] == val)
 			return i;
	}
	return -1;
}

#endif


