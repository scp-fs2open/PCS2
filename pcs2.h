//****************************************************************************
//	pcs2.h
//	POF Constructor Suite 2 main header
//	
//****************************************************************************

/*
 * $Logfile: /pcs2.h $
 * $Revision: 1.56 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: pcs2.h,v $
 * Revision 1.56  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.55  2008/11/10 23:25:19  kazan
 * more progress in transitioning
 *
 * Revision 1.54  2008/02/11 13:56:23  kazan
 * tweaks
 *
 * Revision 1.53  2008/02/09 23:01:45  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.52  2008/02/09 22:33:00  kazan
 * bye bye kaz_vector
 *
 * Revision 1.51  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.50  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.49  2008/02/09 16:03:11  kazan
 * head branch is no longer public final - there is now stable_2_0_fixes to backport patches and generalte 2.0.x builds from
 *
 * Revision 1.48  2008/02/09 15:58:44  kazan
 * 2.0 Final Public Build
 *
 * Revision 1.47  2008/02/03 21:45:23  kazan
 * Call main_panel::SignalModelChange("", true) to reset the view without changing the currently loaded model.
 * New clears the model then uses the above
 * Save calls the above at the end to refresh the view
 * Leaving the Preferences dialog does as well
 *
 * Revision 1.46  2007/10/18 15:54:09  kazan
 * Removed all instances of POFstring in favor of std::strings
 *
 * Revision 1.45  2007/10/17 23:17:28  kazan
 * Autosmooth reverse engineer
 *
 * Revision 1.44  2007/07/24 22:50:24  bobboau
 * added undo/redo functionality
 *
 * Revision 1.43  2007/07/14 12:11:48  bobboau
 * added a OGL info menu option under optpons to help in debugging problems with people who have out of date OGL drivers.
 *
 * Revision 1.42  2007/07/13 11:31:05  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.41  2007/07/12 08:34:20  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.40  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.39  2007/07/11 14:09:37  kazan
 * fix solid-view rendering bug, renable app modal progress dialogs during load
 *
 * Revision 1.38  2007/07/10 05:34:47  bobboau
 * added a few preset views to a view menu.
 *
 * Revision 1.37  2007/07/06 10:20:59  bobboau
 * moved version strings to there own cpp file and tricked the project file into compileing that file every time it is built (thus ensureing the date and time is acurate)  did a few other minor things related to the title.
 *
 * Revision 1.36  2007/07/06 05:38:55  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.35  2007/07/03 02:52:20  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.33  2007/07/01 15:15:11  kazan
 * add special token to paths <appdir> - and new default path <appdir>\textures
 * versions are now PCS2 Beta
 * add filename and version info to title bar
 * enable texture alpha blending
 *
 * Revision 1.32  2007/06/30 18:58:10  bobboau
 * made some tweaks to the auto-path feature, and added an orthographic projection option
 *
 * Revision 1.31  2007/06/29 23:57:54  bobboau
 * removed old toolbar buttons that I didn't think were very useful, added new ones that I thought were (axis locking and movement plane)
 *
 * Revision 1.30  2007/06/26 10:05:06  bobboau
 * added data import, includeing chunk, global, and subobject.
 *
 * Revision 1.29  2007/06/25 04:14:10  bobboau
 * implemented file assosiation, for windows at least, also made a makeshift icon for the exe, as pofs get there icons changed to reflect assosiation.
 * also made sure that the ini file gets saved and loaded only to the same folder as the exe it'self.
 *
 * Revision 1.28  2007/06/24 06:47:45  bobboau
 * actualy remembered to assign array ids so the menu worked on anything other than the one I was testing on
 *
 * fixed a few minor bugs with the ray picker, and something slightly not so minor that was caused by a CVS conflict that I hadn't noticed.
 *
 * Revision 1.27  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.26  2007/06/18 14:01:52  bobboau
 * added a neat little 'suggestion control' wraper for primitive controls, you push a button it gives you a few good options (for example names of hard coded subsystems)
 * I also added index nameing suport to the array class, but it probly wasn't worth the effort.
 *
 * Revision 1.25  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.24  2007/06/15 06:00:18  bobboau
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
 * Revision 1.23  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.22  2007/06/09 23:48:56  bobboau
 * made some improvements to the UI of the GL canvas, includeing scroll wheel zooming and omnipoint editing (hold control to enter edit mode). omnipoints still need a rendering method.
 *
 * I also slightly reorganized the GL canvas (moved it's declairation to it's own file)
 *
 * Revision 1.21  2007/06/06 12:08:23  bobboau
 * polished the UI some, converted a bunch of int_ctrls to int_lists, mostly for selecting a subobject number, added array over/underflow protection/handeling to some stuff.
 *
 * Revision 1.20  2007/06/06 04:23:47  bobboau
 * UI cleanup, removed old UI
 *
 * Revision 1.19  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.18  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.17  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.16  2005/09/04 00:26:43  kazan
 * Enabled editor tab switching, added editor tab toolbar, enabled apply/revert for editors - add stubs for all editors
 *
 * Revision 1.15  2005/09/03 02:23:43  kazan
 * Header Editor Panel, Editors toolbar
 *
 * Revision 1.14  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
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

#if !defined(_pcs2_h_)
#define _pcs2_h_

#include "GLee.h"

#include <wx/setup.h>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/thread.h>

#include <wx/glcanvas.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/progdlg.h>
#include <wx/filename.h>
#include <wx/docview.h>

#include "wxPCSPref.h"
#include "pcs_file.h"
#include "tex_ctrl.h"
#include "main_panel.h"
#include "wxGL_PMFCanvas.h"

#ifdef UNIX
#define CONFIG_FILE _("/.pcs2")
#else
#define CONFIG_FILE _("\\pcs2.ini")
#endif

/*#include "compat/FE_GR_ogl_functable.h"

extern op_gl_funcs GLFunctions;*/

// only uncomment the following for actual releases
//#define PUBLIC_RELEASE

extern wxString PCS2_VERSION;
extern wxString PCS2_COMP_VERSION;
extern wxString _BUILD_TIME_;
extern wxString PCS2_SUPPORTED_FORMATS;
// some shortcuts
#define ADD_TOOL(id, bmp, tooltip, help) \
    toolBar->AddTool(id, bmp, wxNullBitmap, FALSE, -1, -1, (wxObject *)0, tooltip, help)

#define ADD_TOGGLE_TOOL(id, bmp, tooltip, help) \
    toolBar->AddTool(id, bmp, wxNullBitmap, wxITEM_CHECK, -1, -1, (wxObject *)0, tooltip, help)

#define ADD_RADIO_TOOL(id, label, bmp, tooltip, help) \
    toolBar->AddRadioTool(id, label, bmp, wxNullBitmap, tooltip, help, NULL) 

#define MIN(a, b) ((a<b)?a:b)

#define MAX(a, b) ((a>b)?a:b)

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct file_context
// this is how we handle having multiple files open - multiple contexts
{
	std::string filename;
	std::string path;
	PCS_Model mdata;
};

// globals are lazy, but just about the only tenable manner
extern std::vector<file_context> model_files;
extern int current_model_file;
extern wxConfig *config;
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class PCS2_MainWindow;

class PCS2_App : public wxApp
{
	PCS2_MainWindow *myframe;
	//makeing it the derived class allows us to use any public functions 
	//of the derived class, and has no drawback that I can see.

	public:
		// -------------- Initialization Function --------------
		virtual bool OnInit();
		virtual int OnExit();

		PCS2_MainWindow*get_main_window(){return myframe;};

};

DECLARE_APP(PCS2_App)

//a little useful utility function
inline PCS2_MainWindow* get_main_window(){
	return wxGetApp().get_main_window();
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class main_panel;

class PCS2_MainWindow : public wxFrame
{
	protected:
		DECLARE_EVENT_TABLE()

	private:
		wxStatusBar *mystatus;
		wxToolBar *toolBar;
		wxMenuBar* mymenu;
		wxMenu *m_pFileMenu;
		wxMenu *m_pFileHistoryMenu;
		wxFileHistory file_history;
		
		main_panel *mypanel;
		std::string loaded_file;
//		wxPanel*central; //temporary
				
		wxProgressDialog *threaded_prog_bar;
		int threaded_prog_bar_readers;
		bool should_delete_threaded_prog_bar;
		bool UseThreadedProgBar;

		void InitMenu();

		void update_color_options();
	public:
		PCS2_MainWindow();
		void load_file(wxString filename, bool skipProgdlg=false);

		// progress bar events
		void save_progbar_start(wxAsyncProgressStartEvt &event);
		void save_progbar_update(wxAsyncProgressUpdateEvt &event);
		void save_progbar_end(wxAsyncProgressEndEvt &event);

		// events
		void File_Menu_New(wxCommandEvent &event);
		void File_Menu_Quit(wxCommandEvent &event) { Close(true); }
		void File_Menu_Open(wxCommandEvent &event);
		void File_Menu_Save(wxCommandEvent &event);
		void File_Menu_History_Open(wxCommandEvent &event);
		void on_global_import(wxCommandEvent &event);

		void Options_Menu_Preferences(wxCommandEvent &event);
		void Options_Menu_GL_info(wxCommandEvent &event);

		void Toolbar_Move_Left(wxCommandEvent &event);
		void Toolbar_Move_Right(wxCommandEvent &event);
		void Toolbar_Move_Up(wxCommandEvent &event);
		void Toolbar_Move_Down(wxCommandEvent &event);
		void Toolbar_Move_Zin(wxCommandEvent &event);
		void Toolbar_Move_Zout(wxCommandEvent &event);

		void Toolbar_Rotate_XCC(wxCommandEvent &event);
		void Toolbar_Rotate_XC(wxCommandEvent &event);
		void Toolbar_Rotate_YCC(wxCommandEvent &event);
		void Toolbar_Rotate_YC(wxCommandEvent &event);
		void Toolbar_Rotate_ZCC(wxCommandEvent &event);
		void Toolbar_Rotate_Zc(wxCommandEvent &event);

		void menu_set_view(wxCommandEvent &event);

		void on_undo(wxCommandEvent &event);
		void on_redo(wxCommandEvent &event);
		void on_undo_enable(bool enable = true);
		void on_redo_enable(bool enable = true);
		void on_import(wxCommandEvent &event);

		void Toolbar_plane_changed(wxCommandEvent &event);
		void Toolbar_axis_changed(wxCommandEvent &event);

		void Toolbar_projection_changed(wxCommandEvent &event);

		void Toolbar_grid_changed(wxCommandEvent &event);

		void Toolbar_OnTextured(wxCommandEvent &event);
		void Toolbar_OnWireframe(wxCommandEvent &event);
		void Toolbar_OnSolid(wxCommandEvent &event);
		virtual void OnClose(wxCloseEvent& event);

		void menu_data_new_list(wxCommandEvent &event);
		void menu_data_new_item(wxCommandEvent &event);
		void menu_data_copy_list(wxCommandEvent &event);
		void menu_data_copy_item(wxCommandEvent &event);
		void menu_data_delete_list(wxCommandEvent &event);
		void menu_data_delete_item(wxCommandEvent &event);
		void menu_data_increment_list(wxCommandEvent &event);
		void menu_data_decrement_list(wxCommandEvent &event);
		void menu_data_increment_item(wxCommandEvent &event);
		void menu_data_decrement_item(wxCommandEvent &event);
		void menu_data_purgebspcache(wxCommandEvent &event);
		void menu_data_smoothingcaclc(wxCommandEvent &event);


		//returns a list of all subobject names in order
		PCS_Model&get_model();

		//returns a list of all subobject names in order
		 void get_model_list(std::vector<std::string>&);

		//returns a list of all path names in order
		 void get_path_list(std::vector<std::string>&);

		//returns a list of all subsystem names in order
		 void get_subsystem_list(std::vector<std::string>&);

		 // puts a title "" will result in "PCS2_VERSION (__DATE__ __TIME__)" a filename will result in "Filename - PCS2_VERSION (__DATE__ __TIME__)"
		 void putTitle(wxString filename);

		 void startRender();
};



#endif //_pcs2_h_
