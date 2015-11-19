//****************************************************************************
//	tex_ctrl.cpp
//****************************************************************************

/*
 * $Logfile: /tex_ctrl.cpp $
 * $Revision: 1.38 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: tex_ctrl.cpp,v $
 * Revision 1.38  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.37  2008/05/15 17:43:58  kazan
 * *** empty log message ***
 *
 * Revision 1.36  2008/02/09 23:01:47  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.35  2008/02/09 22:33:05  kazan
 * bye bye kaz_vector
 *
 * Revision 1.34  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.33  2008/02/04 19:17:29  kazan
 * Fix VP Search resuming
 *
 * Revision 1.32  2008/02/04 18:02:44  kazan
 * Dumbest typo that latest forever found while fixing the dumbest bug ever - VolitionPackfileReader::FindFileWild was case sensative.. causing PCS2 not to load textures in VPs when there was a case mismatch
 *
 * Revision 1.31  2008/01/11 06:24:09  bobboau
 * fixing a spelling error, and modifyed the suggest box graphic a little
 *
 * Revision 1.30  2007/07/18 21:14:11  kazan
 * fix a fair of minus "oops"es
 *
 * Revision 1.29  2007/07/18 17:57:48  bobboau
 * made a plaform dependent fix for DevIL crashing when loading the nameplate.dds file, we could include some sort of error mesage or log it when this happens, from testing this fix will still cause the debugger to break on exception, so we will still be able to catch this, but to the user the file will simply fail to load.
 * this is platform dependent and will need Special care for all suported platforms, currently it defaults to simple exception handeling when not in WIN32, but this will not usualy catch a divide by zero error.
 *
 * I also made some modifications to the renderer to make it behave more like FSO, it renders children first, and uses alpha blending, it can lead to some ugly results, but they will be just as ugly in FSO so it's good they get to see it.
 *
 * Revision 1.28  2007/07/11 19:14:50  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.27  2007/07/02 20:20:37  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.26  2007/07/01 15:15:11  kazan
 * add special token to paths <appdir> - and new default path <appdir>\textures
 * versions are now PCS2 Beta
 * add filename and version info to title bar
 * enable texture alpha blending
 *
 * Revision 1.25  2007/06/28 11:56:40  kazan
 * fix #19
 *
 * Revision 1.24  2007/06/27 21:45:23  kazan
 * fix texture loading and fix rendering selected object only
 *
 * Revision 1.23  2007/06/27 00:01:15  kazan
 * trying to squash bugs
 *
 * Revision 1.22  2007/06/25 13:13:15  kazan
 * had to remove bob's path prefix code from pcs2.ini load - was breaking it
 * moved ilutRenderer(ILUT_OPENGL) to fix 256x256 texture bug (#11)
 * added texture serach logging for debug builds
 *
 * Revision 1.21  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.20  2007/06/15 16:53:52  kazan
 * Have to remove main_panel.cpp from CVS and then restore it - cvs version is corrupted and now marked binary
 *
 * Revision 1.19  2007/06/14 23:48:10  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.18  2007/06/05 06:29:25  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.17  2007/05/08 00:49:46  kazan
 * fixed some cob bugs, made complete bar work, did some data caching to optimize smoothing calculator
 *
 * Revision 1.16  2007/05/07 19:15:11  kazan
 * checkin for doing diffs to find bug
 *
 * Revision 1.15  2007/05/07 17:40:56  kazan
 * changes in progress
 *
 * Revision 1.14  2007/03/25 17:13:50  kazan
 * more signed/unsigned cleanup
 *
 * Revision 1.13  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.12  2007/03/22 13:15:33  kazan
 * MSVC8 compatability checkin, requires wxWidgets 2.8
 *
 * Revision 1.11  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.10  2005/10/07 14:08:06  kazan
 * linux compat syncs - i'm getting memory crashes in linux, hopefully it happens in windows too so it's easier to debug
 *
 * Revision 1.9  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.8  2005/08/31 15:29:55  kazan
 * small behavioral fix for the wildcard search
 *
 * Revision 1.7  2005/08/31 03:47:40  kazan
 * fixed bugs in vpreader, implemented wildcard directory and VP searches, implemented BSP caching, got DDS to work (mostly via a custom compile of DevIL]
 *
 * Revision 1.6  2005/08/30 23:23:25  kazan
 * working dds.. /shock
 *
 * Revision 1.5  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.4  2005/08/29 18:05:43  kazan
 * fixed a crash due to OGL drivers not gracefully handing invalid texture handles, also changed how we resize the window
 *
 * Revision 1.3  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.2  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.1  2005/08/28 01:48:02  kazan
 * Initial Import from pcs 1.x - translated to pmf from pof
 *
 *
 *
 */

#include "FileList.h"
#include "insignia.png.h"
#include "main_panel.h"
#include "pcs2.h"
#include "tex_ctrl.h"
#include "VPReader.h"

#include <fstream>

using namespace std;

//used to load the texture and catch divide by zero errors in WIN32
#ifndef WIN32
#pragma message("*****DevIL exeption handeling not implemented for thsi platform! see the top of tex_ctrl for details*****")
#endif
bool load(ILenum type, const void*buf, int zero){
#ifdef WIN32
__try{
#else
try{
#endif
			return ilLoadL(type, buf, zero) == IL_TRUE;
#ifdef WIN32
}__except(true){
	return false;
}
#else
}catch (int) {
	return false;
}
#endif
}
//same as above exept for files
bool load_image(const char*name){
#ifdef WIN32
__try{
#else
try{
#endif
			return ilLoadImage((const ILstring)name) == IL_TRUE;
#ifdef WIN32
}__except(true){
	return false;
}
#else
}catch (int) {
	return false;
}
#endif
}
// ****************************************************************************************************************
// Texture Control code members
// ****************************************************************************************************************


TextureControl::~TextureControl() {
	Reset();
}

bool TextureControl::solid_render(int idx) {
	return (idx < 0) || (idx >= (int)textures.size()) || (texturenames[idx] != "invisible");//NO!!
}

string TextureControl::get_texture_filename(int idx, Texture_Type type = TC_TEXTURE) {
	if (idx<0 || idx>(int)textures.size())
		return "";

	switch (type) {
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

void TextureControl::Reset()
{
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		if (textures[i].texture != 0xFFFFFFFF)
			glDeleteTextures(1, &textures[i].texture);

		if (textures[i].shine != 0xFFFFFFFF)
			glDeleteTextures(1, &textures[i].shine);

		if (textures[i].glow != 0xFFFFFFFF)
			glDeleteTextures(1, &textures[i].glow);
	}

	textures.resize(0);
	texturenames.resize(0);
}


void TextureControl::LoadTextures(PCS_Model &pf, std::vector<std::string> &paths, AsyncProgress* prog_msgngr)
{

	if (textures.size() != 0)
		Reset();
	textures.resize(pf.GetTexturesCount());
	texturenames.resize(pf.GetTexturesCount());

	ilutRenderer(ILUT_OPENGL); // call this here to make sure it's called AFTER openGL init
	//int RetVal;


	// done

	// Load our insignia image.
	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);

	if (load(IL_PNG, insignia_png, sizeof(insignia_png)))
	{
		ILinfo imginfo;
		iluGetImageInfo(&imginfo);
		iluFlipImage();
		float sz = (imginfo.Width > imginfo.Height)?imginfo.Width:imginfo.Height;
		iluScale(sz, sz, imginfo.Depth);
		ilutGLBindTexImage();
	}
	ilDeleteImages(1, &ImageName);

	unsigned int i;
	std::string fname;


	// get file lists;
	//FileList bleh(paths[i], "*");
	std::vector<FileList> vplists;
	std::vector<FileList> normal_lists;

	vplists.resize(paths.size());
	normal_lists.resize(paths.size()*2);
	std::string root;

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
	ofstream texture_log("pcs2_txtsrch_log.txt");
#endif
	// setup file paths/get lists
	for (i = 0; i < paths.size(); i++)
	{
		if (paths[i].length())
		{
#if defined(WIN32)
			if (paths[i][0] == '<')
			{
				if (paths[i].substr(0, 8) == "<appdir>")
				{
					paths[i] = std::string(wxGetCwd().mb_str()) + paths[i].substr(8);
				}
			}
#endif
			normal_lists[i*2].GetList(paths[i], "*");
			
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << paths[i] << " Files" << endl;
#endif

			if (PlatformNormalize(fname, "data\\maps\\", paths[i]))
			{
				//vplists[i+1].GetList(fname, "*.vp");
				normal_lists[(i*2)+1].GetList(fname, "*");	

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << fname << " Files" << endl;
#endif
			}

			vplists[i].GetList(paths[i], "*.vp");

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << paths[i] << " VPs" << endl;
#endif
		}
	}

	prog_msgngr->setTarget((unsigned int)textures.size() * 3);
	std::string strtemp;

	for (i = 0; i < textures.size(); i++)
	{
		texturenames[i] = pf.Texture(i);
		strtemp = "Loading Texture \"";
		strtemp += pf.Texture(i);
		strtemp += "\"";

		prog_msgngr->setMessage(strtemp);
		prog_msgngr->Notify();

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
		textures[i].texture = LoadTexture(texturenames[i], textures[i].texname, paths, vplists, normal_lists, texture_log);

		prog_msgngr->incrementWithMessage(strtemp + "-shine");
		textures[i].shine = LoadTexture(texturenames[i] + "-shine", textures[i].shine_name, paths, vplists, normal_lists, texture_log);

		prog_msgngr->incrementWithMessage(strtemp + "-glow");
		textures[i].glow = LoadTexture(texturenames[i] + "-glow", textures[i].glow_name, paths, vplists, normal_lists, texture_log);
#else
		textures[i].texture = LoadTexture(texturenames[i], textures[i].texname, paths, vplists, normal_lists);
		
		prog_msgngr->incrementWithMessage(strtemp + "-shine");
		textures[i].shine = LoadTexture(texturenames[i] + "-shine", textures[i].shine_name, paths, vplists, normal_lists);

		prog_msgngr->incrementWithMessage(strtemp + "-glow");
		textures[i].glow = LoadTexture(texturenames[i] + "-glow", textures[i].glow_name, paths, vplists, normal_lists);
#endif
		prog_msgngr->incrementProgress();


	}
	//delete texture;

	// list ALL files inside filelists
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
	for (i = 0; i < paths.size(); i++)
	{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
		texture_log << "############## Path #" << i << "##############" << endl;
#endif
		if (paths[i].length())
		{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << "+++Dir: " << normal_lists[i*2].getDir() << endl;
#endif
			for (int j=0; j < normal_lists[i*2].Size(); j++)
			{
				texture_log << normal_lists[i*2].getDir() + normal_lists[i*2][j] << endl;
			}

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << "+++Dir: " << normal_lists[(i*2)+1].getDir() << endl;
#endif
			for (int j=0; j < normal_lists[(i*2)+1].Size(); j++)
			{
				texture_log << normal_lists[(i*2)+1].getDir() + normal_lists[(i*2)+1][j] << endl;
			}

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << "+++VPs: " << vplists[i].getDir() << endl;
#endif
			for (int j=0; j < vplists[i].Size(); j++)
			{
				texture_log << vplists[i].getDir() + vplists[i][j] << endl;
			}
		}
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
		texture_log << endl;
#endif
	}
#endif

	delete prog_msgngr;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//enum Texture_Type { TC_TEXTURE, TC_SHINEMAP, TC_GLOW }
GLuint TextureControl::TextureTranslate(int texnum, Texture_Type type)
{
	if (texnum == -1 || (unsigned)texnum >= texturenames.size()) 
		return 0xFFFFFFFF;

	switch (type)
	{
		case TC_SHINEMAP:
			return textures[texnum].shine;

		case TC_GLOW:
			return textures[texnum].glow;

		default:
			return textures[texnum].texture;
	}

	return 0xFFFFFFFF;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
GLuint TextureControl::LoadTexture(std::string texname,
								   std::string &rfname, //real file name
								   const std::vector<std::string> &paths, 
								   const std::vector<FileList> &vplists, 
								   const std::vector<FileList> &normal_lists,
								   std::ostream &texture_log)
#else
GLuint TextureControl::LoadTexture(std::string texname, 
								   std::string &rfname, //real file name
								   const std::vector<std::string> &paths, 
								   const std::vector<FileList> &vplists, 
								   std::vector<FileList> &normal_lists)
#endif
{
	int texId = -1;

	int num_exts = 7;
	const char *extensions[] = { ".pcx", ".dds", ".jpg", ".png", ".tga", ".gif", ".bmp" };
	ILenum type_codes[] = { IL_PCX, IL_DDS , IL_JPG, IL_PNG, IL_TGA, IL_GIF, IL_BMP };

	float sz;
	bool found;
	std::string tempstr;
	ILinfo imginfo;
	std::string fname, lcase;
	ILenum img_type;
	int filenum;
	size_t curvp;

	size_t size;
	boost::scoped_array<char> buffer;

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
	texture_log << "-------- Searching for texture: " << texname << " --------" << endl;
#endif
	found = false;
	for (unsigned int j = 0; j < paths.size() && !found; j++)
	{
		img_type = IL_TYPE_UNKNOWN;

		// search root then /data/maps

		for (int l = 0; l <= 1 && img_type == IL_TYPE_UNKNOWN; l++)
		{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
			texture_log << "Searching Dir: " << normal_lists[(j*2)+l].getDir() << endl;;
#endif
			filenum = FileList::new_search;
			do {
				filenum = normal_lists[(j*2)+l].wild_search(texname + ".*", filenum);

				if (filenum != -1)
				{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
					texture_log << "File matched: " << (normal_lists[(j*2)+l].getDir() + normal_lists[(j*2)+l][filenum]);
#endif

					lcase = strLower(normal_lists[(j*2)+l][filenum]);
					for (int k = 0; k < num_exts; k++)
					{
						if (strstr(lcase.c_str(), extensions[k]))
						{
							fname = normal_lists[(j*2)+l].getDir() + normal_lists[(j*2)+l][filenum];
							texId = LoadFile(fname);
							if (texId != -1 && texId != 0)
							{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
								texture_log << " (Accepted)";
#endif
								rfname = fname;
								wxString temp(rfname.c_str(), wxConvUTF8);
								temp.Replace(_(".\\"),wxGetCwd()+_("\\"));
								rfname = temp.mb_str();
								img_type = type_codes[k];
								found = true;
							}
						} //if (strstr(lcase.c_str(), extensions[k]))
					} // for (int k = 0; k < num_exts; k++)
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
					texture_log << endl;
#endif

				} //if (filenum != -1)
			} while (img_type == IL_TYPE_UNKNOWN && filenum != -1);
		} //for (int l = 0; l <= 1 && img_type == IL_TYPE_UNKNOWN; l++)


		// search VPs in root
		
		if (img_type == IL_TYPE_UNKNOWN && !found)
		{
			filenum = FileList::new_search;
			img_type = IL_TYPE_UNKNOWN;
			buffer.reset(NULL);
			fname = texname;
			curvp = 0; // curvp is used for resuming search

			do {
				filenum = SearchVPs(vplists[j], paths[j], fname, size, buffer, rfname, curvp, filenum);

				if (filenum != -1)
				{
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
					texture_log << "VP matched: " << fname;
#endif
					lcase = strLower(fname);
					for (int k = 0; k < num_exts; k++)
					{
						if (strstr(lcase.c_str(), extensions[k]))
						{
							img_type = type_codes[k];
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
							texture_log << " (Accepted)";
#endif
							break;
						}
					}
#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
					texture_log << endl;
#endif
				}
				
				if (img_type == IL_TYPE_UNKNOWN && buffer.get())
				{
					rfname = "";
					buffer.reset(NULL);
					fname=texname;
				}

			} while (img_type == IL_TYPE_UNKNOWN && filenum != -1);				
		}


		if (img_type != IL_TYPE_UNKNOWN && !found)
		{
			ILuint ImageName; // The image name to return.
			ilGenImages(1, &ImageName);
			ilBindImage(ImageName);

			

			if (load(img_type, buffer.get(), (int)size))
			{
				iluGetImageInfo(&imginfo);
				iluFlipImage();
				sz = (imginfo.Width > imginfo.Height)?imginfo.Width:imginfo.Height;
				iluScale(sz, sz, imginfo.Depth);
				texId = ilutGLBindTexImage();

				//write file to disk for debug purposes
				//ofstream out(fname.c_str());
				//out.write(buffer, size);
				//out.close();

				if (texId != -1 && texId != 0)
				 found = true;
			}
			ilDeleteImages(1, &ImageName);

			buffer.reset(NULL);
		}
	}

#if defined(_DEBUG) && defined(_ENABLE_TEXTUREPATH_DEBUG_)
	texture_log << endl;
#endif
	return texId;
}
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int TextureControl::SearchVPs(const FileList &vp_list, string directory, string &filename, size_t &size, boost::scoped_array<char> &buffer, string &rfname, size_t &curvp, size_t searchpos)
{	
	//FileList vp_list(directory, "*.vp");
	std::string File;

	
#if defined(_WIN32)
	if (directory[directory.length() - 1] != '\\')
		directory += "\\";
#else
	if (directory[directory.length()-1] != '/')
		directory += "/";
#endif
	int filenum;

	for (size_t i = curvp; i < vp_list.Size(); i++)
	{
		File = directory + vp_list[i];

		VolitionPackfileReader VPR(File);
		filenum = VPR.FindFileWild(filename + ".*", (int)searchpos);
		if (filenum != -1) {
			rfname = File + ":" + VPR.GetInfo((int)fileno).filename;
			filename = VPR.FileName((int)fileno);
			size = VPR.LoadFile((int)fileno, buffer);

			curvp = i;
			return filenum;

		} else {
			searchpos = 0; // reset search offset
		}
	}
	
	return -1;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

GLuint LoadFile(std::string Filename)
{

	//OpenGL_TextureID = ilutGLLoadImage((char *const)Filename.c_str());
	//Status = (OpenGL_TextureID != 0);
	GLuint OpenGL_TextureID = 0xFFFFFFFF;
	ILuint ImgId;										// The Image Name
	ILinfo imginfo;
	float sz;

	ilGenImages(1, &ImgId);								// Generate Image Name
	ilBindImage(ImgId);									// Bind Image Name to Be Current
	if (load_image((char *const)Filename.c_str())) {				// Load the Bitmap and Check for Errors
		iluGetImageInfo(&imginfo);
		iluFlipImage();
		sz = (imginfo.Width > imginfo.Height)?imginfo.Width:imginfo.Height;
		iluScale(sz, sz, imginfo.Depth);

		OpenGL_TextureID = ilutGLBindTexImage();					// Send the Image to OpenGL
	}
	//else
	//f	TexNum = ilGetError();				// just some test shiat
	ilDeleteImages(1, &ImgId);						// Delete the Image Name, As GL Has a Copy
	return OpenGL_TextureID;


}
