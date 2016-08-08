//****************************************************************************
//	pcs2.cpp
//	POF Constructor Suite 2 main
//	
//****************************************************************************

/*
 * $Logfile: /pcs2.cpp $
 * $Revision: 1.85 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs2.cpp,v $
 * Revision 1.85  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.84  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.83  2008/11/10 23:25:19  kazan
 * more progress in transitioning
 *
 * Revision 1.82  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.81  2008/02/12 18:59:51  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.80  2008/02/11 13:56:23  kazan
 * tweaks
 *
 * Revision 1.79  2008/02/09 23:01:45  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.78  2008/02/09 22:33:00  kazan
 * bye bye kaz_vector
 *
 * Revision 1.77  2008/02/09 22:31:26  kazan
 * finish removing string
 *
 * Revision 1.76  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.75  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.74  2008/02/09 15:58:44  kazan
 * 2.0 Final Public Build
 *
 * Revision 1.73  2008/02/03 21:45:23  kazan
 * Call main_panel::SignalModelChange("", true) to reset the view without changing the currently loaded model.
 * New clears the model then uses the above
 * Save calls the above at the end to refresh the view
 * Leaving the Preferences dialog does as well
 *
 * Revision 1.72  2008/01/31 17:04:55  kazan
 * bump comp version due to big bugfix
 *
 * Revision 1.71  2008/01/31 17:02:24  kazan
 * Add error checking to BSP tree packing, this will slow packing somewhat, but will prevent crashes.
 * Correct error in calculating tree size.
 *
 * Revision 1.70  2008/01/30 20:59:14  kazan
 * mark as rc2d - so next public build is properly tagged
 *
 * Revision 1.69  2008/01/30 20:50:05  kazan
 * Fix the damn BBox bug, finally!
 * Fixed a small bug where it would cache when it shouldn't
 * Increment compiler version
 *
 * Revision 1.68  2008/01/18 18:39:26  kazan
 * Was finally able to replicate the bounding box inversion - apparently HDR2 bbox's are in openGL coordinate space NOT BSP coordinate Space
 * add support to .cob specifications for "detail" light - a group with a child light named "detail" will be added as a LOD (this allows .cob's written out by PCS2 to be IMMEDIATELY reloadable and have the hierarchy right!)
 *
 * Revision 1.67  2008/01/11 06:24:09  bobboau
 * fixing a spelling error, and modifyed the suggest box graphic a little
 *
 * Revision 1.66  2008/01/06 21:44:21  kazan
 * Update PMF with new bounding boxes upon POF Compile
 * Update PCS2_COMP_VERSION to cause previously miscompiled (incorrect swapped boudning boxes) to be properly recompiled.
 *
 * Revision 1.65  2007/10/18 15:54:08  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.64  2007/10/18 14:54:00  kazan
 * Fix accuracy problem with BSP depth counter, error BSP_COMPILATION_ERROR flag, reenable infinite recursion catch due to duplicate-polygon-centers still causing infinite recursion
 *
 * Revision 1.63  2007/10/17 23:17:28  kazan
 * Autosmooth reverse engineer
 *
 * Revision 1.62  2007/07/24 22:50:23  bobboau
 * added undo/redo functionality
 *
 * Revision 1.61  2007/07/14 12:11:48  bobboau
 * added a OGL info menu option under optpons to help in debugging problems with people who have out of date OGL drivers.
 *
 * Revision 1.60  2007/07/13 18:22:21  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.59  2007/07/13 11:31:05  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.58  2007/07/12 08:34:20  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.57  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.56  2007/07/11 19:14:47  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.55  2007/07/11 14:09:34  kazan
 * fix solid-view rendering bug, renable app modal progress dialogs during load
 *
 * Revision 1.54  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.53  2007/07/10 05:34:47  bobboau
 * added a few preset views to a view menu.
 *
 * Revision 1.52  2007/07/09 00:07:13  bobboau
 * sence neither I nor Kazan seems to have a solid understanding on how to use branching, I've decided to commit the experimental BSP *within _BOB_BSP_ ifdefs* to the main branch. sence it's mostly a change to one function it makes even more sence to do this IMHO. I also added sevral statistic trackers to be added in the model coments section, I now have the BSP compiler track the maximum number of polygons packed into a single node, the maximum depth of BSP recursion for a model, and compile time (both total and tree generation), in both versions.
 *
 * Revision 1.51  2007/07/06 10:20:59  bobboau
 * moved version strings to there own cpp file and tricked the project file into compileing that file every time it is built (thus ensureing the date and time is acurate)  did a few other minor things related to the title.
 *
 * Revision 1.50  2007/07/06 05:38:55  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.49  2007/07/02 20:20:34  kazan
 * texture loader now supports loading [texture] [texture]-glow and [texture]-shine
 *
 * Revision 1.48  2007/07/01 15:15:11  kazan
 * add special token to paths <appdir> - and new default path <appdir>\textures
 * versions are now PCS2 Beta
 * add filename and version info to title bar
 * enable texture alpha blending
 *
 * Revision 1.47  2007/07/01 01:24:10  bobboau
 * I had disabled the splash screen because there was a bug showing up when it was up makeing debuging a tad bit dificult, I now have it ifdefed so it's on only in release builds.
 *
 * Revision 1.46  2007/06/30 18:58:10  bobboau
 * made some tweaks to the auto-path feature, and added an orthographic projection option
 *
 * Revision 1.45  2007/06/29 23:57:54  bobboau
 * removed old toolbar buttons that I didn't think were very useful, added new ones that I thought were (axis locking and movement plane)
 *
 * Revision 1.44  2007/06/26 10:05:06  bobboau
 * added data import, includeing chunk, global, and subobject.
 *
 * Revision 1.43  2007/06/25 20:50:39  kazan
 * Create and Enable splashscreen now that we have load-on-start
 *
 * Revision 1.42  2007/06/25 15:57:03  kazan
 * recommit fixed path stuff for the config
 *
 * Revision 1.41  2007/06/25 13:13:15  kazan
 * had to remove bob's path prefix code from pcs2.ini load - was breaking it
 * moved ilutRenderer(ILUT_OPENGL) to fix 256x256 texture bug (#11)
 * added texture serach logging for debug builds
 *
 * Revision 1.40  2007/06/25 04:14:10  bobboau
 * implemented file assosiation, for windows at least, also made a makeshift icon for the exe, as pofs get there icons changed to reflect assosiation.
 * also made sure that the ini file gets saved and loaded only to the same folder as the exe it'self.
 *
 * Revision 1.39  2007/06/25 01:42:40  bobboau
 * added a options dialog for selecting editor colors.
 *
 * Revision 1.38  2007/06/24 10:22:16  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.37  2007/06/24 06:47:45  bobboau
 * actualy remembered to assign array ids so the menu worked on anything other than the one I was testing on
 *
 * fixed a few minor bugs with the ray picker, and something slightly not so minor that was caused by a CVS conflict that I hadn't noticed.
 *
 * Revision 1.36  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.35  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.34  2007/06/21 22:57:47  bobboau
 * a number of minor tweaks and bug fixes with the interface mostly dealing with comunication between the tree and the control panel.
 *
 * Revision 1.33  2007/06/18 14:26:20  bobboau
 * fixed a small buffer overrun error I had overlooked
 *
 * Revision 1.32  2007/06/18 14:01:51  bobboau
 * added a neat little 'suggestion control' wraper for primitive controls, you push a button it gives you a few good options (for example names of hard coded subsystems)
 * I also added index nameing suport to the array class, but it probly wasn't worth the effort.
 *
 * Revision 1.31  2007/06/18 01:15:02  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.30  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.29  2007/06/15 06:00:18  bobboau
 * number of small fixes, mostly related to updateing data in one subsystem when it should be
 *
 * fixed weapon point bug were the editing of one type would result in the eraseing of the other
 *
 * added PINF editor
 *
 * added convergence calculator for weapon points
 *
 * quickened the scroll zoom and made it proportional
 *
 * Revision 1.28  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.27  2007/06/06 12:08:23  bobboau
 * polished the UI some, converted a bunch of int_ctrls to int_lists, mostly for selecting a subobject number, added array over/underflow protection/handeling to some stuff.
 *
 * Revision 1.26  2007/06/06 04:23:47  bobboau
 * UI cleanup, removed old UI
 *
 * Revision 1.25  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.24  2007/05/08 02:40:45  kazan
 * bug fixes, error messages
 *
 * Revision 1.23  2007/05/07 17:40:55  kazan
 * changes in progress
 *
 * Revision 1.22  2007/03/25 17:04:14  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.21  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.20  2007/03/22 13:15:33  kazan
 * MSVC8 compatability checkin, requires wxWidgets 2.8
 *
 * Revision 1.19  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.18  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.17  2005/09/05 23:00:16  kazan
 * some sleeps to make the threads sync
 *
 * Revision 1.16  2005/09/03 02:23:43  kazan
 * Header Editor Panel, Editors toolbar
 *
 * Revision 1.15  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
 *
 * Revision 1.14  2005/08/30 23:23:25  kazan
 * working dds.. /shock
 *
 * Revision 1.13  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.12  2005/08/29 18:05:43  kazan
 * fixed a crash due to OGL drivers not gracefully handing invalid texture handles, also changed how we resize the window
 *
 * Revision 1.11  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.10  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 * Revision 1.9  2005/08/28 00:33:31  kazan
 * configuration dialog - next part is the texture loader
 *
 * Revision 1.8  2005/08/25 15:06:38  kazan
 * various bugfixes and tweaks
 *
 * Revision 1.7  2005/08/23 22:36:42  kazan
 * s/std::{vector:string}/kaz_{vector:string}
 * renderer motion works
 * location PMF::LoadFromPOF bottleneck
 *
 * Revision 1.6  2005/08/23 01:31:11  kazan
 * disabled loading of BSP::Defpoints normals.. they're still corrupt
 *
 * renderer movement and lighting works
 *
 * Revision 1.5  2005/08/21 23:40:22  kazan
 * More work
 *
 * Revision 1.4  2005/08/21 01:58:53  kazan
 * Context Menu framework, tree ctrl contructor
 * bug fixes to std::string handling, POF->PMF translator
 * Loaded it's first model!
 *
 * Revision 1.3  2005/04/04 15:47:53  kazan
 * gui
 *
 * Revision 1.2  2005/03/27 16:41:23  kazan
 * new gui structure - the tree
 *
 * Revision 1.1.1.1  2004/12/31 06:19:41  kazan
 * no message
 *
 *
 *
 */

#include "pcs2.h"
#include "pcs2_CIDs.h"
#include <wx/image.h>
#include <GL/glu.h>
#include <fstream>
#include <algorithm>
#include <wx/wfstream.h>
#include "pcs2_filethread.h"
#include <wx/progdlg.h>
#include <wx/splash.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "main_panel.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
// globals are lazy, but just about the only tenable manner
std::vector<file_context> model_files;
int current_model_file;

wxString PCS2_COMP_VERSION = _("PCS 2.0 Compiler Version 2 Final");

wxString PCS2_SUPPORTED_FORMATS = _("All Supported Formats|*.cob;*.pmf;*.pof;*.scn;*.dae|Parallax Object Files (.pof)|*.pof|PCS2 Model File (.pmf)|*.pmf|Caligari Object Binary (.cob)|*.cob|Caligari Scene (.scn)|*.scn|Collada (.dae)|*.dae");
//op_gl_funcs GLFunctions;

//this is just iceing, 
//and if there is a bug before it closes it's in the way
#if defined(_DEBUG)
#define NO_SPLASHSCREEN
#endif

#if (wxMAJOR_VERSION >= 3)
#define wxOPEN wxFD_OPEN
#define wxFILE_MUST_EXIST wxFD_FILE_MUST_EXIST
#define wxSAVE wxFD_SAVE
#define wxOVERWRITE_PROMPT wxFD_OVERWRITE_PROMPT
#endif

//*******************************************************************************
// XPM Resources
//*******************************************************************************

namespace xpms
{

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"

#include "undo.xpm"
#include "redo.xpm"

#include "XZ_plane.xpm"
#include "XY_plane.xpm"
#include "YZ_plane.xpm"

#include "lock_X.xpm"
#include "lock_Y.xpm"
#include "lock_Z.xpm"

#include "proj_persp.xpm"
#include "proj_ortho.xpm"

#include "grid_btn.xpm"

#include "solid.xpm"
#include "textured.xpm"
#include "wireframe.xpm"
#include "placeholder.xpm"
#include "pcsico.xpm"
#ifndef NO_SPLASHSCREEN
#include "splash.xpm"
#endif
};

// #WARNING# Make sure to deallocate this!
wxConfig *config;

//didn't want to do this, but the recursive include problems were to
//much a pain in the ass for such a small amount of code, so they
//are going here now.

void get_model_list(std::vector<std::string>&l){
	get_main_window()->get_model_list(l);
}

void get_path_list(std::vector<std::string>&l){
	get_main_window()->get_path_list(l);
}

void get_subsystem_list(std::vector<std::string>&l){
	get_main_window()->get_subsystem_list(l);
}

	//used by the array control
std::string get_texture_name(int idx){
	if(idx < 0 || idx >= get_main_window()->get_model().GetTexturesCount())
		return "";
	return get_main_window()->get_model().Texture(idx);
}

std::string get_subsystem_name(int idx){
	if(idx < 0 || idx >= get_main_window()->get_model().GetSpecialCount())
		return "";
	return get_main_window()->get_model().Special(idx).name;
}

std::string get_turret_name(int idx){
	if(idx < 0 || idx >= get_main_window()->get_model().GetTurretCount())
		return "";

	int parent = get_main_window()->get_model().Turret(idx).sobj_parent;

	if(parent < 0 || parent >= get_main_window()->get_model().GetSOBJCount())
		return "";
	return get_main_window()->get_model().SOBJ(parent).name;
}


//*******************************************************************************
// PCS2_App Implementation
//*******************************************************************************
void set_editor_colors();

bool PCS2_App::OnInit()
{
	/*
	//argv
	wxMessageBox(wxString::Format("%d arguments", argc), "argc");
	for(int i = 0; i<argc; i++){
		wxMessageBox(argv[i], "argv");
	}
	*/

	myframe = NULL;

	// Iinitialize OpenIL
	ilInit();
	iluInit();

	//this causes DevIL to load TGAs and some others as expected
	ilEnable(IL_ORIGIN_SET); 
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT); 

	//wxConfigBase::Set();
	// get all image handlers we need
	wxInitAllImageHandlers();

#if !defined(NO_SPLASHSCREEN)
	// show the splash screen

	wxBitmap bitmap(xpms::SplashPixmap);

	wxSplashScreen *splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN, 0, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR);
#endif

	// Setup the Config stuff
	wxFile cfg_file; //("pcs2.ini", wxFile::read);
	wxString cfgFilename = wxStandardPaths::Get().GetUserConfigDir() + CONFIG_FILE;

	if (!cfg_file.Exists(cfgFilename))
	{
		// Try falling back to old pcs2.ini location first.
		wxFileName oldCfgFilename = wxStandardPaths::Get().GetExecutablePath();
		oldCfgFilename.SetFullName(CONFIG_FILE);
		if (cfg_file.Exists(oldCfgFilename.GetFullPath()))
		{
			cfgFilename = oldCfgFilename.GetFullPath();
		}
	}

	if (!cfg_file.Exists(cfgFilename))
	{
		
		// Create the config file if it doesn't exist
		wxFileOutputStream *ocf = new wxFileOutputStream(cfgFilename);
		delete ocf;
		
		
		wxFileInputStream config_file_new(cfgFilename);
		wxFileConfig *fConfig = new wxFileConfig(config_file_new);
		wxConfigBase::Set(fConfig);

		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetRecordDefaults(true);


		// "try" reading the config in a manner that will set the defaults
		
		//config = new wxConfig("pcs2");

		wxString str;
		pConfig->SetPath(_T("/tpaths/"));
		pConfig->Read(_("numpaths"), 3l);
	#if defined(WIN32)
		pConfig->Read(_("path0"), &str, _(".\\"));
		pConfig->Read(_("path1"), &str, _("<appdir>\\textures\\"));
		pConfig->Read(_("path2"), &str, _("c:\\games\\freespace2\\"));
	#else
		pConfig->Read(_("path0"), &str, _("./"));
		pConfig->Read(_("path1"), &str, _("~/.pcs2/textures/"));
		pConfig->Read(_("path2"), &str, _("fs2/"));
	#endif

		pConfig->SetPath(_T("/convoptions/"));
		double d;
		int itemp;
		pConfig->Read(_("cobscale"), &d, 20.00);
		pConfig->Read(_("use_geometry_filter"), &itemp, 1); // default to on

		pConfig->SetPath(_T("/gr_options/"));
		pConfig->Read(_("use_vertex_buffer_objects"), &itemp, 0); // default to off
		pConfig->Flush();

		wxFileOutputStream cfg_out(cfgFilename);
		((wxFileConfig*)pConfig)->Save(cfg_out);
		
		pConfig->SetRecordDefaults(false);
	}
	else
	{
		wxFileInputStream config_file(cfgFilename);
		wxFileConfig *fConfig = new wxFileConfig(config_file);
		wxConfigBase::Set(fConfig);
	}
	
	set_editor_colors();//init editor colors

	// actually start the app now


	myframe = new PCS2_MainWindow();

	if(argc > 1)
		myframe->load_file(argv[1], true);
	myframe->Show(TRUE);	
	myframe->startRender();
	SetTopWindow(myframe);

#if !defined(NO_SPLASHSCREEN)
	wxMilliSleep(500);
	if (splash)
		delete splash;
#endif

	return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


int PCS2_App::OnExit()
{
	ilShutDown();
	delete wxConfigBase::Set((wxConfigBase *) NULL);
	//delete config;

	return 0;
}

IMPLEMENT_APP(PCS2_App)

//*******************************************************************************
// PCS2_MainWindow Implementation
//*******************************************************************************


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PCS2_MainWindow::PCS2_MainWindow() : wxFrame(NULL, -1, _("POF Constructor Suite 2"), wxDefaultPosition, wxSize(600,400)),mypanel(NULL), threaded_prog_bar(NULL), threaded_prog_bar_readers(0), should_delete_threaded_prog_bar(false)
{
	wxGridSizer*sizer = new wxGridSizer(1,1,0,0);
	wxBitmap pcsicon((xpms::pcsico));
	wxColour maskcol(0x00, 0x80, 0x80);
	pcsicon.SetMask(new wxMask(pcsicon, maskcol)); //#008080
	wxIcon AppIco;
	AppIco.CopyFromBitmap(pcsicon);
	
	SetIcon(AppIco);
	InitMenu();
//	mypanel = new PCS2_MainPanel((wxFrame*) this, wxSize(600,400), mystatus);
	mypanel = new main_panel(this);
	sizer->Add(mypanel,1.0f,wxEXPAND);
	SetSizer(sizer);
//	central = new wxPanel(this);
	update_color_options();


	putTitle(_(""));
}

void PCS2_MainWindow::startRender() {
	mypanel->startRender();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::putTitle(wxString filename)
{
	wxString Title;
	loaded_file = filename.mb_str();
	filename = filename.AfterLast('\\');
	if (filename == _(""))
		Title = PCS2_VERSION;
	else
		Title = filename + _(" - ") + PCS2_VERSION;
	SetTitle(Title);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::load_file(wxString filename, bool skipProgdlg){
	if(filename!=_("")){
		//if we were given a file to load, do it
		putTitle(filename);
		mypanel->SignalModelChange(std::string(filename.mb_str()), skipProgdlg);
		toolBar->ToggleTool(PCS2_TBCTRL_TEXTURED, true);

		// Add the file to our file history.
		wxFileName full_path(filename);
		full_path.Normalize();
		file_history.AddFileToHistory(full_path.GetFullPath());
		// Save to our config file in case something horrible happens.
		wxConfigBase::Get()->SetPath(_T("/history/"));
		file_history.Save(*wxConfigBase::Get());
		wxFileOutputStream cfg_out(wxStandardPaths::Get().GetUserConfigDir()+wxString(CONFIG_FILE));
		((wxFileConfig*)wxConfigBase::Get())->Save(cfg_out);
	}
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::File_Menu_New(wxCommandEvent &event)
{
	mypanel->model.Reset();
	mypanel->SignalModelChange("", true); // with ""/true it just resets the UI
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::File_Menu_Open(wxCommandEvent &event)
{
	wxFileDialog *fdlg;
	fdlg = new wxFileDialog(this, _("Select File(s)"), _(""), _(""), PCS2_SUPPORTED_FORMATS, wxOPEN | wxFILE_MUST_EXIST);
	if (fdlg->ShowModal() != wxID_OK)
		return;

	load_file(fdlg->GetPath());
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::File_Menu_History_Open(wxCommandEvent &event)
{
	load_file(file_history.GetHistoryFile(event.GetId() - file_history.GetBaseId()));
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::save_progbar_start(wxAsyncProgressStartEvt &event)
{
	if (UseThreadedProgBar)
	{
		threaded_prog_bar_readers++;
		if (threaded_prog_bar == NULL) {
			threaded_prog_bar = new wxProgressDialog(_("Saving File"), _("Starting File Save")); 
		} else {
			threaded_prog_bar->Update(0, _("Starting File Save"));
		}
		should_delete_threaded_prog_bar = false;
		threaded_prog_bar->SetSize(300,125);
		threaded_prog_bar->ShowModal();
		threaded_prog_bar_readers--;
		if (threaded_prog_bar_readers == 0 && should_delete_threaded_prog_bar) {
			delete threaded_prog_bar;
			threaded_prog_bar = NULL;
			should_delete_threaded_prog_bar = false;
		}
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::save_progbar_update(wxAsyncProgressUpdateEvt &event)
{
#ifdef _WINDEF_
#undef min
#undef max
#endif
	int percent = std::min(std::max((int)event.getPercent(), 0), 100);
	mypanel->pstatus->SetStatusText(wxString(event.getMessage().c_str(), wxConvUTF8), 0);
	mypanel->pgauge->SetValue(percent);
	if (UseThreadedProgBar && threaded_prog_bar != NULL)
	{
		threaded_prog_bar_readers++;
		threaded_prog_bar->Update(percent, wxString(event.getMessage().c_str(), wxConvUTF8));
		threaded_prog_bar_readers--;
		if (threaded_prog_bar_readers == 0 && should_delete_threaded_prog_bar) {
			delete threaded_prog_bar;
			threaded_prog_bar = NULL;
			should_delete_threaded_prog_bar = false;
		}
	}

}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::save_progbar_end(wxAsyncProgressEndEvt &event)
{
	if (threaded_prog_bar != NULL)
	{
		if (threaded_prog_bar_readers == 0) {
			delete threaded_prog_bar;
			threaded_prog_bar = NULL;
		} else {
			should_delete_threaded_prog_bar = true;
			// Can't delete it now so let's hide it at least.
			threaded_prog_bar->Hide();
		}
	}
	mypanel->pstatus->SetStatusText(_("Idle"), 0);
	mypanel->pgauge->SetValue(0);

	if (event.getEarlyTerminated())
	{
		wxMessageDialog errormsg((wxWindow*)this, 
			wxString(event.getMessage().c_str(), wxConvUTF8), 
			_("Error Loading Textures"), wxOK | wxICON_HAND);
		errormsg.ShowModal();
	
	}
	
	if (PCS_Model::BSP_COMPILE_ERROR)
	{
		wxMessageDialog errormsg((wxWindow*)this, _("An error occured compiling the BSP data for one or more submodels"), _("BSP Compilation Error"), wxOK | wxICON_HAND);
		errormsg.ShowModal();
	}

	//mypanel->SignalModelChange("", true); // with ""/true it just resets the UI - we do this because we might have a BSP cache now
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void PCS2_MainWindow::File_Menu_Save(wxCommandEvent &event)
{
	wxFileDialog *fdlg;
	fdlg = new wxFileDialog(this, _("Save File"), _(""), wxString(loaded_file.c_str(), wxConvUTF8), PCS2_SUPPORTED_FORMATS, wxSAVE | wxOVERWRITE_PROMPT);
	if (fdlg->ShowModal() != wxID_OK)
		return;

	UseThreadedProgBar = true;

	AsyncProgress *prog_messenger = new wxAsyncProgress(this, SAVE_FILE_PROGRESS_MESSAGER);
	wxPCS2SaveThread *thread = new wxPCS2SaveThread(&(mypanel)->model, std::string(fdlg->GetPath().mb_str()), prog_messenger);

	thread->Create();
	thread->Run();

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::InitMenu()
{
		// -------------------- Menu Bar --------------------

	mymenu = new wxMenuBar();
	// File menu
	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(PCS2_MFILE_NEW);
	m_pFileMenu->Append(PCS2_MFILE_OPEN);
	m_pFileMenu->Append(PCS2_MFILE_SAVE);
	//m_pFileMenu->Append(PCS2_MFILE_SAVEAS);
	//m_pFileMenu->Append(PCS2_MFILE_CLOSE);
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MFILE_IMPORT, _("Global &Import"), _("Import All Non-Subobject Data From Another Model"));
	m_pFileMenu->AppendSeparator();
	m_pFileHistoryMenu = new wxMenu();
	file_history.UseMenu(m_pFileHistoryMenu);
	wxConfigBase::Get()->SetPath(_T("/history/"));
	file_history.Load(*wxConfigBase::Get());
	m_pFileMenu->AppendSubMenu(m_pFileHistoryMenu, _("&Recent Files"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MFILE_QUIT);

	mymenu->Append(m_pFileMenu, _("&File"));

	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(PCS2_EDIT_UNDO, _("&Undo\tCTRL+z"), _("Undoes the last major edit"));
	m_pFileMenu->Append(PCS2_EDIT_REDO, _("&Redo\tCTRL+y"), _("Undoes the last undo"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_EDIT_GEOIMPORT, _("&Import Geometry\tCTRL+i"), _("Imports Geometry into currently loaded model"));
	mymenu->Append(m_pFileMenu, _("&Edit"));


	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(PCS2_MVIEW_TOP, _("&Top"), _("View from the top of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_BOTTOM, _("&Bottom"), _("View from the bottom of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_LEFT, _("&Left"), _("View from the left of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_RIGHT, _("&Right"), _("View from the right of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_FRONT, _("&Front"), _("View from the front of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_BACK, _("B&ack"), _("View from the back of the model"));
	m_pFileMenu->Append(PCS2_MVIEW_RESET, _("Re&set"), _("Set the view back to it's default position"));
	mymenu->Append(m_pFileMenu, _("&View"));

	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(PCS2_MOPT_PREF, wxEmptyString, _("PCS2 Program Preferences"));
	m_pFileMenu->Append(PCS2_MOPT_GL_VERS, _("&Query OpenGL"), _("Information about your Open GL Version"));
	mymenu->Append(m_pFileMenu, _("&Options"));

	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(PCS2_MDATA_ADD_LIST, _("Add &List\tCTRL+SHIFT+L"), _("Adds a new high level list of data, i.e. a new thruster bank"));
	m_pFileMenu->Append(PCS2_MDATA_ADD_ITEM, _("&Add Item\tCTRL+SHIFT+I"), _("Adds new low level data, i.e. a new thruster point"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MDATA_COPY_LIST, _("Copy List\tCTRL+L"), _("Copies current high level list of data"));
	m_pFileMenu->Append(PCS2_MDATA_COPY_ITEM, _("&Copy Item\tCTRL+I"), _("Copies current low level data"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MDATA_DELETE_LIST, _("Delete List\tCTRL+ALT+SHIFT+L"), _("Copies current high level list of data"));
	m_pFileMenu->Append(PCS2_MDATA_DELETE_ITEM, _("&Delete Item\tCTRL+ALT+SHIFT+I"), _("Copies current low level data"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MDATA_UP_ITEM, _("&Previous Item\tEND"), _("Move selection to the item preceding the currently selected one"));
	m_pFileMenu->Append(PCS2_MDATA_DOWN_ITEM, _("&Next Item\tHOME"), _("Move selection to the item following the currently selected one"));
	m_pFileMenu->Append(PCS2_MDATA_UP_LIST, _("Previous List\tPGDN"), _("Move selection to the list preceding the currently selected one"));
	m_pFileMenu->Append(PCS2_MDATA_DOWN_LIST, _("Next List\tPGUP"), _("Move selection to the list following the currently selected one"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(PCS2_MDATA_PURGEBSPCACHE, _("Purge BSP Cache"), _("Purges the BSP cache on a model if a recompile needs to be forced."));
	m_pFileMenu->Append(PCS2_MDATA_SMOOTHINGCALC, _("Calculate Smoothing Data"), _("Attempts to reverse engineer facet/smooth/autofacet data"));
	mymenu->Append(m_pFileMenu, _("&Data"));

	// the "Model" Menu - for selecting the active model
	/*m_pModelMenu = new wxMenu();
	char *hlp[] = { "File Slot #1", "File Slot #2", "File Slot #3", "File Slot #4",
					"File Slot #5", "File Slot #6", "File Slot #7", "File Slot #8",
					"File Slot #9", "File Slot #10" };

	m_pModelMenu->AppendRadioItem(PCS2_MFLIST_ONE,		"Untitled", hlp[0]);
	current_model_file = 0;

	for (int i = 1; i < 10; i++)
	{
		m_pModelMenu->AppendRadioItem(PCS2_MFLIST_ONE+i,		"No File", hlp[i]);
		m_pModelMenu->Enable(PCS2_MFLIST_ONE+i, false);
	}
	mymenu->Append(m_pModelMenu, "&Model");*/

	SetMenuBar(mymenu);

	// -------------------- Toolbar --------------------
	//#d4d0c8
	wxBitmap tbBitmaps[18];
	wxColour maskcol(0xd4, 0xd0, 0xc8);
	//pcsicon.SetMask(new wxMask(pcsicon, maskcol)); //#008080

    tbBitmaps[0] = wxBitmap(xpms::tbNew);
    tbBitmaps[1] = wxBitmap(xpms::tbOpen);
    tbBitmaps[2] = wxBitmap(xpms::tbSave);

	tbBitmaps[3] =  wxBitmap(xpms::undo);
	tbBitmaps[4] =  wxBitmap(xpms::redo);

	tbBitmaps[5] =  wxBitmap(xpms::XZ_plane);
	tbBitmaps[6] =  wxBitmap(xpms::XY_plane);
	tbBitmaps[7] =  wxBitmap(xpms::YZ_plane);

	tbBitmaps[8] =  wxBitmap(xpms::lock_X);
	tbBitmaps[9] =  wxBitmap(xpms::lock_Y);
	tbBitmaps[10] =  wxBitmap(xpms::lock_Z);

	tbBitmaps[11] = wxBitmap(xpms::proj_persp);
	tbBitmaps[12] = wxBitmap(xpms::proj_ortho);

	tbBitmaps[13] = wxBitmap(xpms::grid_btn);

	tbBitmaps[14] = wxBitmap(xpms::tbTextured);
	tbBitmaps[15] = wxBitmap(xpms::tbSolid);
	tbBitmaps[16] = wxBitmap(xpms::tbWireframe);
	tbBitmaps[17] = wxBitmap(xpms::PHImg);

	for (int i = 0; i < 18; i++)
	{
		tbBitmaps[i].SetMask(new wxMask(tbBitmaps[i], maskcol));
	}


	// create toolbar wxICON
	toolBar = CreateToolBar();
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(23, 21));

	// add controls
    ADD_TOOL(PCS2_MFILE_NEW,	tbBitmaps[0], _("New"),			_("Start a new model"));
    toolBar->AddSeparator();
    ADD_TOOL(PCS2_MFILE_OPEN,	tbBitmaps[1], _("Open"),		_("Open project"));
    ADD_TOOL(PCS2_MFILE_SAVE,	tbBitmaps[2], _("Save"),		_("Save project"));
	toolBar->AddSeparator();
	ADD_TOOL(PCS2_EDIT_GEOIMPORT,	 tbBitmaps[1],_("Import Geometry"),	_("Imports Geometry into currently loaded model"));

	// undo/redo
    toolBar->AddSeparator();
	ADD_TOOL(PCS2_EDIT_UNDO,		tbBitmaps[3],_("Undo"), 	_("undoes the last major edit"));
	ADD_TOOL(PCS2_EDIT_REDO,		 tbBitmaps[4],_("Redo"),	_("undoes the last undo"));
	

	// Movement
    toolBar->AddSeparator();
	ADD_RADIO_TOOL(PCS2_TBCTRL_XZ_PLANE,		_("XZ Plane"),tbBitmaps[5], 	_("Constrain Movement to the XZ Plane"),_(""));
	ADD_RADIO_TOOL(PCS2_TBCTRL_XY_PLANE,		 _("XY Plane"),tbBitmaps[6],	_("Constrain Movement to the XY Plane"),_(""));
	ADD_RADIO_TOOL(PCS2_TBCTRL_YZ_PLANE,		 _("YZ Plane"),tbBitmaps[7],	_("Constrain Movement to the YZ Plane"),_(""));
	
	
	//axis locking
    toolBar->AddSeparator();
	ADD_TOGGLE_TOOL(PCS2_TBCTRL_LOCK_X,		tbBitmaps[8], _("Lock X"),	_("Toggle Movement Along the X Axis"));
	ADD_TOGGLE_TOOL(PCS2_TBCTRL_LOCK_Y,		tbBitmaps[9], _("Lock Y"),	_("Toggle Movement Along the Y Axis"));
	ADD_TOGGLE_TOOL(PCS2_TBCTRL_LOCK_Z,		tbBitmaps[10], _("Lock Z"),	_("Toggle Movement Along the Z Axis"));

	//projection options
    toolBar->AddSeparator();
	ADD_RADIO_TOOL(PCS2_TBCTRL_PROJ_PERSP,		_("Perspective"),	tbBitmaps[11], _("Use Perspective Projection"), _(""));
	ADD_RADIO_TOOL(PCS2_TBCTRL_PROJ_ORTHO,		_("Orthographic"),	tbBitmaps[12], _("Use Orthographic Projection"), _(""));

	//projection options
    toolBar->AddSeparator();
	ADD_TOGGLE_TOOL(PCS2_TBCTRL_GRID,		tbBitmaps[13], _("Grid"),	_("Toggle the Auto-Grid"));
	
	// Textured/Solid/Wireframe
    toolBar->AddSeparator();
	ADD_RADIO_TOOL(PCS2_TBCTRL_TEXTURED,	_("Textured"),		tbBitmaps[14], _("Render the model textured"), _(""));
	ADD_RADIO_TOOL(PCS2_TBCTRL_SOLID,		_("Solid"),		tbBitmaps[15], _("Render the model as a textureless solid"), _(""));
	ADD_RADIO_TOOL(PCS2_TBCTRL_WIREFRAME,	_("Wireframe"),	tbBitmaps[16], _("Render the model as a wireframe"), _(""));

	toolBar->EnableTool(PCS2_TBCTRL_LOCK_Y, false);

	toolBar->EnableTool(PCS2_EDIT_UNDO, false);
	toolBar->EnableTool(PCS2_EDIT_REDO, false);
	mymenu->Enable(PCS2_EDIT_UNDO, false);
	mymenu->Enable(PCS2_EDIT_REDO, false);
	//you have nothing, you have done nothing to it, there is nothing to undo
	

	// make it display/enable
	toolBar->Realize();
	
	// -------------------- Status Bar --------------------
	/*
    CreateStatusBar(1);
    SetStatusText(_("Welcome to VPCS2!"),0);*/

	mystatus = new wxStatusBar(this, PCS2_CTRL_STATUS, wxST_SIZEGRIP);
	SetStatusBar(mystatus);

	// start timers
	/*t1 = new wxTimer(this, VPCS2_CTRL_TIMER1);
	t1->Start(250);*/
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_data_purgebspcache(wxCommandEvent &event)
{
	mypanel->model.PurgeBSPcache();
}

void PCS2_MainWindow::menu_data_smoothingcaclc(wxCommandEvent &event)
{
	int sobjs_comp; // we can add a progress dialog later
	mypanel->model.Calculate_Smoothing_Data(sobjs_comp);
}

void PCS2_MainWindow::get_model_list(std::vector<std::string>&ret){

	ret.resize(mypanel->model.GetSOBJCount());

	for(int i =0; i<mypanel->model.GetSOBJCount(); i++){
		ret[i] = mypanel->model.SOBJ(i).name;
	}
}

void PCS2_MainWindow::get_path_list(std::vector<std::string>&ret){

	ret.resize(mypanel->model.GetPathCount());

	for(int i =0; i<mypanel->model.GetPathCount(); i++){
		ret[i] = mypanel->model.Path(i).name;
	}
}

		//returns a list of all subsystem names in order
void PCS2_MainWindow::get_subsystem_list(std::vector<std::string>&ret){

	for(int i =0; i<mypanel->model.GetSpecialCount(); i++){
		//skip splits
		if(strstr(mypanel->model.Special(i).properties.c_str(), "$special=subsystem")){
			std::string str = mypanel->model.Special(i).name;
			if(str == "")continue;
			if(str[0] == '$')str.erase(str.begin());
				ret.push_back(str);
		}
	}


	//include model subsystems too I supose
	for(int i =0; i<mypanel->model.GetSOBJCount(); i++){
		if(strstr(mypanel->model.SOBJ(i).properties.c_str(), "$special=subsystem"))
			ret.push_back(mypanel->model.SOBJ(i).name);
	}

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_OnTextured(wxCommandEvent &event)
{
	mypanel->model.Rcall_Textureless(false);
	mypanel->model.Rcall_Wireframe(false);
	mypanel->glcanvas->Render();
}


PCS_Model&PCS2_MainWindow::get_model(){return mypanel->model;};

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_OnWireframe(wxCommandEvent &event)
{
	mypanel->model.Rcall_Textureless(false);
	mypanel->model.Rcall_Wireframe(true);
	mypanel->glcanvas->Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_OnSolid(wxCommandEvent &event)
{
	mypanel->model.Rcall_Textureless(true);
	mypanel->model.Rcall_Wireframe(false);
	mypanel->glcanvas->Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::update_color_options(){
	wxConfigBase *pConfig = wxConfigBase::Get();

	long r,g,b;

	pConfig->SetPath(_T("/gr_options/ambient"));
	pConfig->Read(_("R"), &r, 25);
	pConfig->Read(_("G"), &g, 25);
	pConfig->Read(_("B"), &b, 25);
	mypanel->glcanvas->set_ambient_light(r,g,b);

	pConfig->SetPath(_T("/gr_options/diffuse"));
	pConfig->Read(_("R"), &r, 230);
	pConfig->Read(_("G"), &g, 230);
	pConfig->Read(_("B"), &b, 230);
	mypanel->glcanvas->set_diffuse_light(r,g,b);

	wxCommandEvent commandEvent(EDIT_DONE, GetId());
	mypanel->GetEventHandler()->ProcessEvent(commandEvent);

	mypanel->glcanvas->Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Options_Menu_Preferences(wxCommandEvent &event)
{
	PCS_Preferences* dialog = new PCS_Preferences(this);
	dialog->ShowModal();
	delete dialog;

	update_color_options();
	mypanel->SignalModelChange(""); // with ""/true it just resets the UI
}

void PCS2_MainWindow::Options_Menu_GL_info(wxCommandEvent &event){
	mypanel->glcanvas->display_OGL_info();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Left(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.x -= mypanel->model.SOBJ(0).radius * 0.05f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Right(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.x += mypanel->model.SOBJ(0).radius * 0.05f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Up(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.y += mypanel->model.SOBJ(0).radius * 0.05f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Down(wxCommandEvent &event)
{
	
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.y -= mypanel->model.SOBJ(0).radius * 0.05f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Zin(wxCommandEvent &event)
{
	
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.z += mypanel->model.SOBJ(0).radius * 0.1f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Move_Zout(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->position.z -= mypanel->model.SOBJ(0).radius * 0.1f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void PCS2_MainWindow::Toolbar_Rotate_XCC(wxCommandEvent &event)
{	
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.y -= 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Rotate_XC(wxCommandEvent &event)
{

	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.y += 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Rotate_YCC(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.x -= 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Rotate_YC(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.x += 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Rotate_ZCC(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.z += 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::Toolbar_Rotate_Zc(wxCommandEvent &event)
{
	if (mypanel->model.GetSOBJCount())
	{
		mypanel->glcanvas->rotation.z -= 15.0f;
		mypanel->glcanvas->Render();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_set_view(wxCommandEvent &event){
	switch(event.GetId()){
		case PCS2_MVIEW_TOP:
			mypanel->glcanvas->set_eye_angles(vector3d(90,0,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_BOTTOM:
			mypanel->glcanvas->set_eye_angles(vector3d(-90,0,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_LEFT:
			mypanel->glcanvas->set_eye_angles(vector3d(0,-90,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_RIGHT:
			mypanel->glcanvas->set_eye_angles(vector3d(0,90,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_FRONT:
			mypanel->glcanvas->set_eye_angles(vector3d(0,0,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_BACK:
			mypanel->glcanvas->set_eye_angles(vector3d(0,180,0));
			mypanel->glcanvas->set_eye_position(vector3d(0,0,mypanel->model.GetMaxRadius()*-1.5f));
			break;
		case PCS2_MVIEW_RESET:
			mypanel->glcanvas->reset_view();
			mypanel->glcanvas->Render();
			break;
			
		default:
			break;
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//one of the movement plane toggle buttons was changed
void PCS2_MainWindow::Toolbar_plane_changed(wxCommandEvent &event){
	switch(event.GetId()){
		case PCS2_TBCTRL_XZ_PLANE:
			mypanel->glcanvas->set_movement_plane(XZ_PLANE);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_X, true);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Y, false);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Z, true);
			break;
		case PCS2_TBCTRL_XY_PLANE:
			mypanel->glcanvas->set_movement_plane(XY_PLANE);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_X, true);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Y, true);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Z, false);
			break;
		case PCS2_TBCTRL_YZ_PLANE:
			mypanel->glcanvas->set_movement_plane(YZ_PLANE);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_X, false);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Y, true);
			toolBar->EnableTool(PCS2_TBCTRL_LOCK_Z, true);
			break;
		default:
			break;
	}
}

//one of the axis constraint buttons was changed
void PCS2_MainWindow::Toolbar_axis_changed(wxCommandEvent &event){
	switch(event.GetId()){
		case PCS2_TBCTRL_LOCK_X:
			mypanel->glcanvas->set_axis_lock(X_AXIS, !event.IsChecked());
			break;
		case PCS2_TBCTRL_LOCK_Y:
			mypanel->glcanvas->set_axis_lock(Y_AXIS, !event.IsChecked());
			break;
		case PCS2_TBCTRL_LOCK_Z:
			mypanel->glcanvas->set_axis_lock(Z_AXIS, !event.IsChecked());
			break;
		default:
			break;
	}
}

//one of the projection options was changed
void PCS2_MainWindow::Toolbar_grid_changed(wxCommandEvent &event){
	mypanel->glcanvas->set_draw_grid(event.IsChecked());
}

//the grid option was changed
void PCS2_MainWindow::Toolbar_projection_changed(wxCommandEvent &event){
	switch(event.GetId()){
		case PCS2_TBCTRL_PROJ_ORTHO:
			mypanel->glcanvas->set_projection_mode(PROJ_ORTHO);
			break;
		default:
		case PCS2_TBCTRL_PROJ_PERSP:
			mypanel->glcanvas->set_projection_mode(PROJ_PERSP);
			break;
	}
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::OnClose(wxCloseEvent& event)
{
	if(mypanel)mypanel->Destroy();
	this->Destroy();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void PCS2_MainWindow::menu_data_new_list(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_LIST, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_NEW);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_data_new_item(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_ITEM, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_NEW);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_data_copy_list(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_LIST, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_COPY);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::menu_data_copy_item(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_ITEM, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_COPY);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_data_delete_list(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_LIST, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_DELETE);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PCS2_MainWindow::menu_data_delete_item(wxCommandEvent &event)
{
	wxWindow*win = FindWindowById(ARRAY_ITEM, this);
	if(win) {
		wxCommandEvent commandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ARRAY_BUTTON_DELETE);
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::menu_data_increment_list(wxCommandEvent &event){
	wxWindow*win = FindWindowById(ARRAY_LIST, this);
	if(win) {
		wxCommandEvent commandEvent(ARRAY_INCREMENT, GetId());
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::menu_data_decrement_list(wxCommandEvent &event){
	wxWindow*win = FindWindowById(ARRAY_LIST, this);
	if(win) {
		wxCommandEvent commandEvent(ARRAY_DECREMENT, GetId());
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::menu_data_increment_item(wxCommandEvent &event){
	wxWindow*win = FindWindowById(ARRAY_ITEM, this);
	if(win) {
		wxCommandEvent commandEvent(ARRAY_INCREMENT, GetId());
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCS2_MainWindow::menu_data_decrement_item(wxCommandEvent &event){
	wxWindow*win = FindWindowById(ARRAY_ITEM, this);
	if(win) {
		wxCommandEvent commandEvent(ARRAY_DECREMENT, GetId());
		win->GetEventHandler()->ProcessEvent(commandEvent);	
	}
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void PCS2_MainWindow::on_global_import(wxCommandEvent &event){
	wxFileDialog fdlg(NULL, _("Select Import File"), _(""), _(""), _("All Supported Formats|*.cob;*.pmf;*.pof;*.scn;*.dae|Parallax Object Files (.pof)|*.pof|PCS2 Model File (.pmf)|*.pmf|Caligari Object Binary (.cob)|*.cob|Caligari Scene (.scn)|*.scn|Collada (.dae)|*.dae"), 
							wxOPEN | wxFILE_MUST_EXIST);
	if (fdlg.ShowModal() != wxID_OK)
		return;

	mypanel->global_import(std::string(fdlg.GetPath().mb_str()));
}

void PCS2_MainWindow::on_undo(wxCommandEvent &event){
	mypanel->undo();
}


void PCS2_MainWindow::on_redo(wxCommandEvent &event){
	mypanel->redo();
}


void PCS2_MainWindow::on_import(wxCommandEvent &event){
	wxFileDialog *fdlg;
	fdlg = new wxFileDialog(this, _("Select File(s)"), _(""), _(""), PCS2_SUPPORTED_FORMATS, wxOPEN | wxFILE_MUST_EXIST);
	if (fdlg->ShowModal() != wxID_OK)
		return;

	mypanel->ImportGeometry(std::string(fdlg->GetPath().mb_str()));
}

void PCS2_MainWindow::on_redo_enable(bool enable){
	toolBar->EnableTool(PCS2_EDIT_REDO, enable);
	mymenu->Enable(PCS2_EDIT_REDO, enable);
}

void PCS2_MainWindow::on_undo_enable(bool enable){
	toolBar->EnableTool(PCS2_EDIT_UNDO, enable);
	mymenu->Enable(PCS2_EDIT_UNDO, enable);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BEGIN_EVENT_TABLE(PCS2_MainWindow, wxFrame)
	EVT_wxAsyncProgressStartEvt(SAVE_FILE_PROGRESS_MESSAGER, PCS2_MainWindow::save_progbar_start)
	EVT_wxAsyncProgressUpdateEvt(SAVE_FILE_PROGRESS_MESSAGER, PCS2_MainWindow::save_progbar_update)
	EVT_wxAsyncProgressEndEvt(SAVE_FILE_PROGRESS_MESSAGER, PCS2_MainWindow::save_progbar_end)


	EVT_CLOSE(PCS2_MainWindow::OnClose)
	EVT_MENU(PCS2_MFILE_NEW,		PCS2_MainWindow::File_Menu_New)
	EVT_MENU(PCS2_MFILE_QUIT,		PCS2_MainWindow::File_Menu_Quit)
	EVT_MENU(PCS2_MFILE_OPEN,		PCS2_MainWindow::File_Menu_Open)
	EVT_MENU(PCS2_MFILE_SAVE,		PCS2_MainWindow::File_Menu_Save)
	EVT_MENU(PCS2_MFILE_IMPORT,		PCS2_MainWindow::on_global_import)
	EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9,	PCS2_MainWindow::File_Menu_History_Open)

	EVT_MENU(PCS2_TBCTRL_WIREFRAME, PCS2_MainWindow::Toolbar_OnWireframe)
	EVT_MENU(PCS2_TBCTRL_SOLID,		PCS2_MainWindow::Toolbar_OnSolid)
	EVT_MENU(PCS2_TBCTRL_TEXTURED,	PCS2_MainWindow::Toolbar_OnTextured)

	EVT_MENU(PCS2_MOPT_PREF, PCS2_MainWindow::Options_Menu_Preferences)
	EVT_MENU(PCS2_MOPT_GL_VERS, PCS2_MainWindow::Options_Menu_GL_info)

	EVT_MENU(PCS2_TBCTRL_XZ_PLANE, PCS2_MainWindow::Toolbar_plane_changed)
	EVT_MENU(PCS2_TBCTRL_XY_PLANE, PCS2_MainWindow::Toolbar_plane_changed)
	EVT_MENU(PCS2_TBCTRL_YZ_PLANE, PCS2_MainWindow::Toolbar_plane_changed)

	EVT_MENU(PCS2_TBCTRL_LOCK_X, PCS2_MainWindow::Toolbar_axis_changed)
	EVT_MENU(PCS2_TBCTRL_LOCK_Y, PCS2_MainWindow::Toolbar_axis_changed)
	EVT_MENU(PCS2_TBCTRL_LOCK_Z, PCS2_MainWindow::Toolbar_axis_changed)

	EVT_MENU(PCS2_TBCTRL_GRID, PCS2_MainWindow::Toolbar_grid_changed)

	EVT_MENU(PCS2_TBCTRL_PROJ_ORTHO, PCS2_MainWindow::Toolbar_projection_changed)
	EVT_MENU(PCS2_TBCTRL_PROJ_PERSP, PCS2_MainWindow::Toolbar_projection_changed)

	EVT_MENU(PCS2_EDIT_UNDO, PCS2_MainWindow::on_undo)
	EVT_MENU(PCS2_EDIT_REDO, PCS2_MainWindow::on_redo)
	EVT_MENU(PCS2_EDIT_GEOIMPORT, PCS2_MainWindow::on_import)

/*
	EVT_MENU(PCS2_TBCTRL_MLEFT, PCS2_MainWindow::Toolbar_Move_Left)
	EVT_MENU(PCS2_TBCTRL_MRIGHT, PCS2_MainWindow::Toolbar_Move_Right)
	EVT_MENU(PCS2_TBCTRL_MUP, PCS2_MainWindow::Toolbar_Move_Up)
	EVT_MENU(PCS2_TBCTRL_MDOWN, PCS2_MainWindow::Toolbar_Move_Down)
	EVT_MENU(PCS2_TBCTRL_ZIN, PCS2_MainWindow::Toolbar_Move_Zin)
	EVT_MENU(PCS2_TBCTRL_ZOUT, PCS2_MainWindow::Toolbar_Move_Zout)

	EVT_MENU(PCS2_TBCTRL_RLeft, PCS2_MainWindow::Toolbar_Rotate_XCC)
	EVT_MENU(PCS2_TBCTRL_RRight, PCS2_MainWindow::Toolbar_Rotate_XC)
	EVT_MENU(PCS2_TBCTRL_Rup, PCS2_MainWindow::Toolbar_Rotate_YCC)
	EVT_MENU(PCS2_TBCTRL_Rdown, PCS2_MainWindow::Toolbar_Rotate_YC)
	EVT_MENU(PCS2_TBCTRL_RCLeft, PCS2_MainWindow::Toolbar_Rotate_ZCC)
	EVT_MENU(PCS2_TBCTRL_RCRight, PCS2_MainWindow::Toolbar_Rotate_Zc)

	EVT_MENU(PCS2_MDATA_ADD_LIST, PCS2_MainWindow::menu_data_new_list)
	EVT_MENU(PCS2_MDATA_ADD_ITEM, PCS2_MainWindow::menu_data_new_item)
	EVT_MENU(PCS2_MDATA_COPY_LIST, PCS2_MainWindow::menu_data_copy_list)
	EVT_MENU(PCS2_MDATA_COPY_ITEM, PCS2_MainWindow::menu_data_copy_item)
	EVT_MENU(PCS2_MDATA_DELETE_LIST, PCS2_MainWindow::menu_data_delete_list)
	EVT_MENU(PCS2_MDATA_DELETE_ITEM, PCS2_MainWindow::menu_data_delete_item)
*/
	EVT_MENU(PCS2_MDATA_UP_LIST, PCS2_MainWindow::menu_data_increment_list)
	EVT_MENU(PCS2_MDATA_DOWN_LIST, PCS2_MainWindow::menu_data_decrement_list)
	EVT_MENU(PCS2_MDATA_UP_ITEM, PCS2_MainWindow::menu_data_increment_item)
	EVT_MENU(PCS2_MDATA_DOWN_ITEM, PCS2_MainWindow::menu_data_decrement_item)
	EVT_MENU(PCS2_MDATA_PURGEBSPCACHE, PCS2_MainWindow::menu_data_purgebspcache)
	EVT_MENU(PCS2_MDATA_SMOOTHINGCALC, PCS2_MainWindow::menu_data_smoothingcaclc)
	
	EVT_MENU(PCS2_MVIEW_TOP, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_BOTTOM, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_LEFT, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_RIGHT, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_FRONT, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_BACK, PCS2_MainWindow::menu_set_view)
	EVT_MENU(PCS2_MVIEW_RESET, PCS2_MainWindow::menu_set_view)

END_EVENT_TABLE()
