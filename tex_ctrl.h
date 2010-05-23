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

#if !defined(_tex_ctrl_h_)
#define _tex_ctrl_h_
#include <string>
#include "pcs_file.h"
#include "FileList.h"
#include <ostream>
#include "AsyncProgress.h"


#if defined(_WIN32) // win32
#include <windows.h>
#include <wx/msw/winundef.h>
#endif

#include "GLee.h"
//#include <GL/gl.h>

// forward dec
class PCS_Model;

enum Texture_Type { TC_TEXTURE, TC_SHINEMAP, TC_GLOW };

struct tc_multitex
{
	GLuint texture;
	GLuint shine;
	GLuint glow;
	std::string texname;
	std::string shine_name;
	std::string glow_name;

	tc_multitex() : texture(0xFFFFFFFF), shine(0xFFFFFFFF), glow(0xFFFFFFFF) {}
};

//#define _ENABLE_TEXTUREPATH_DEBUG_
class TextureControl
{
	private:
		std::vector<tc_multitex> textures;
		std::vector<std::string> texturenames;


		size_t SearchVPs(const FileList &vp_list, std::string directory, std::string &filename, size_t &size, char* &buffer, std::string &rfname, size_t &curvp, size_t searchpos=0);
		size_t SearchAVP(std::string &vp, std::string &filename, size_t &size, char* &buffer, std::string &rfname, size_t searchpos=0);

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
		GLuint LoadTexture(std::string texname,
										   std::string &rfname, //real file name
										   const std::vector<std::string> &paths, 
										   const std::vector<FileList> &vplists, 
										   const std::vector<FileList> &normal_lists,
										   std::ostream &texture_log);
#else
		GLuint LoadTexture(std::string texname, 
										   std::string &rfname, //real file name
										   const std::vector<std::string> &paths, 
										   const std::vector<FileList> &vplists, 
										   std::vector<FileList> &normal_lists);
#endif

	public:
		
		~TextureControl()
			{ Reset(); }

		void Reset();
		

		void LoadTextures(PCS_Model &pf, std::vector<std::string> &paths, AsyncProgress* prog_msgngr);


		GLuint TextureTranslate(int texnum, Texture_Type type);	

		std::string get_texture_filename(int idx, Texture_Type type = TC_TEXTURE){
			if(idx<0||idx>(int)textures.size())
				return "";

			switch(type){
			case TC_TEXTURE:
				return textures[idx].texname;
			case TC_GLOW:
				return textures[idx].glow_name;
			case TC_SHINEMAP:
				return textures[idx].shine_name;
			default:
				return textures[idx].texname;
			}
		}

		//should this texture be drawn in the solid render?
		bool solid_render(int idx){
			return texturenames[idx] != "invisible";//NO!!
		}
};

GLuint LoadFile(std::string Filename);


#endif //_tex_ctrl_h_

