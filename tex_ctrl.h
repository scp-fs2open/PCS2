#ifndef _tex_ctrl_h_
#define _tex_ctrl_h_
//****************************************************************************
//	tex_ctrl.h
//****************************************************************************

/*
 * $Logfile: /tex_ctrl.h $
 * $Revision: 1.19 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: tex_ctrl.h,v $
 * Revision 1.19  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.18  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.17  2008/09/17 19:34:02  kazan
 * little cleanup
 *
 * Revision 1.16  2008/02/09 23:01:47  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.15  2008/02/09 22:33:05  kazan
 * bye bye kaz_vector
 *
 * Revision 1.14  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.13  2008/02/04 19:17:29  kazan
 * Fix VP Search resuming
 *
 * Revision 1.12  2008/01/14 11:55:13  bobboau
 * syncroniseing with CVS, nothing important, invisible texture don't draw, and a few added utility functions for the string
 *
 * Revision 1.11  2007/07/13 11:31:05  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.10  2007/07/12 08:34:21  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.9  2007/07/11 19:14:50  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.8  2007/07/02 20:20:37  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.7  2007/06/05 06:29:25  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.6  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.5  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.4  2005/08/31 15:29:56  kazan
 * small behavioral fix for the wildcard search
 *
 * Revision 1.3  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.2  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.1  2005/08/28 01:48:02  kazan
 * Initial Import from pcs 1.x - translated to pmf from pof
 *
 *
 *
 */



#include "AsyncProgress.h"
#include "FileList.h"
#include "pcs_file.h"

#include "GLee.h"

#include <boost/scoped_array.hpp>

#include <wx/dir.h>

#include <ostream>
#include <string>

#if defined(_WIN32) // win32
#include <windows.h>

#include <wx/msw/winundef.h>
#endif

/**
 * @enum Texture_Type
 * @brief The texture render type
 */
enum Texture_Type {
	TC_TEXTURE = 0,     // standard diffuse texture
	TC_SHINEMAP,        // shine map
	TC_GLOW             // glow map
};

/**
 * @class tc_multitex
 * @brief Multitexture material
 */
class tc_multitex
{
public:
	GLuint texture;     //!< GL index of diffuse texture
	GLuint shine;       //!< GL index of shine map
	GLuint glow;        //!< GL index of glow map

	std::string texname;    //!< diffuse name
	std::string shine_name; //!< shine map name
	std::string glow_name;  //!< glow map name

	/// @todo Use macros instead of hardcoded hex
	tc_multitex()
		: texture(0xFFFFFFFF), shine(0xFFFFFFFF), glow(0xFFFFFFFF) {}
};

// Uncomment this define to enable texturepath debugging?
//#define _ENABLE_TEXTUREPATH_DEBUG_

/**
 * @class TextureControl
 * @brief Texture management Daemon
 *
 * @details Is in charge of loading and unloading from file, binding and unbinding in OpenGL
 */
class TextureControl
{
public:
	TextureControl();

	~TextureControl();

	void Reset();

	void LoadTextures(PCS_Model &pf, std::vector<std::string> &paths, AsyncProgress* prog_msgngr);

	GLuint TextureTranslate(int texnum, Texture_Type type);

	/**
	 * @brief Returns the filename of the texture indexed by idx
	 * @param[in] type If specified, gets the filename of the type specified. ex: Given TC_GLOW, return the filename of the glow map
	 */
	std::string get_texture_filename(int idx, Texture_Type type = TC_TEXTURE);

	/**
	 * @brief Returns true if this texture should be drawn in the solid render
	 */
	bool solid_render(int idx);

protected:

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
	/**
	 * @brief Debug version of LoadTexture(). Writes debug messages to texture_log
	 */
	GLuint LoadTexture(std::string texname,
		std::string &rfname, //real file name
		const std::vector<std::string> &paths,
		const std::vector<FileList> &vplists,
		const std::vector<FileList> &normal_lists,
		std::ostream &texture_log);
#endif

	/**
	 * @brief Loads a (single) texture from file, checking both the file system and any available VP's
	 *
	 * @param[in]  texname      Name of the texture to load
	 * @param[out] rfname       (Real) Filename of the texture
	 * @param[in]  paths        Directories to check
	 * @param[in]  vplists      VP File lists to check
	 * @param[in]  normal_lists ?
	 *
	 * @returns The GL index of the loaded (single) texture
	 */
	GLuint LoadTexture(std::string texname,
		std::string &rfname, //real file name
		const std::vector<std::string> &paths,
		const std::vector<FileList> &vplists,
		std::vector<FileList> &normal_lists);

	int SearchVPs(const FileList &vp_list, std::string directory, std::string &filename, size_t &size, boost::scoped_array<char> &buffer, std::string &rfname, size_t &curvp, size_t searchpos = 0);

private:
	std::vector<tc_multitex> textures;
	std::vector<std::string> texturenames;
};

GLuint LoadFile(std::string Filename);


#endif //_tex_ctrl_h_

