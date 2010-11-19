//****************************************************************************
//	pcs2_CIDs.h
//	POF Constructor Suite 2 main header
//	Control IDs
//****************************************************************************

/*
 * $Logfile: /pcs2_CIDs.h $
 * $Revision: 1.36 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs2_CIDs.h,v $
 * Revision 1.36  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.35  2008/11/10 23:25:19  kazan
 * more progress in transitioning
 *
 * Revision 1.34  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.33  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.32  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.31  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.30  2008/01/10 18:28:10  bobboau
 * *** empty log message ***
 *
 * Revision 1.29  2007/10/17 23:17:28  kazan
 * Autosmooth reverse engineer
 *
 * Revision 1.28  2007/07/24 22:50:24  bobboau
 * added undo/redo functionality
 *
 * Revision 1.27  2007/07/14 12:11:48  bobboau
 * added a OGL info menu option under optpons to help in debugging problems with people who have out of date OGL drivers.
 *
 * Revision 1.26  2007/07/13 11:31:05  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.25  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.24  2007/07/11 19:14:48  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.23  2007/07/10 05:34:47  bobboau
 * added a few preset views to a view menu.
 *
 * Revision 1.22  2007/07/06 05:38:55  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.21  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.19  2007/06/30 18:58:11  bobboau
 * made some tweaks to the auto-path feature, and added an orthographic projection option
 *
 * Revision 1.18  2007/06/29 23:57:54  bobboau
 * removed old toolbar buttons that I didn't think were very useful, added new ones that I thought were (axis locking and movement plane)
 *
 * Revision 1.17  2007/06/29 20:15:43  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.16  2007/06/26 10:05:06  bobboau
 * added data import, includeing chunk, global, and subobject.
 *
 * Revision 1.15  2007/06/25 01:42:40  bobboau
 * added a options dialog for selecting editor colors.
 *
 * Revision 1.14  2007/06/24 06:47:45  bobboau
 * actualy remembered to assign array ids so the menu worked on anything other than the one I was testing on
 *
 * fixed a few minor bugs with the ray picker, and something slightly not so minor that was caused by a CVS conflict that I hadn't noticed.
 *
 * Revision 1.13  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.12  2007/06/18 14:01:52  bobboau
 * added a neat little 'suggestion control' wraper for primitive controls, you push a button it gives you a few good options (for example names of hard coded subsystems)
 * I also added index nameing suport to the array class, but it probly wasn't worth the effort.
 *
 * Revision 1.11  2007/06/18 06:56:44  bobboau
 * added a control for normals, is exactly like the vector control exept it has a button on the right for normalizeing the vectore, figured it would be useful, or at leaset fun to implement, also fixed a minor oversight with teh BSP rendering thing I did (forgot to give the flag a defaul value)
 *
 * Revision 1.10  2007/06/18 05:16:39  bobboau
 * added check box for Kazan's BSP rendering, will set PCS_Model::draw_bsp to true or false if BSP rendering should be performed.
 *
 * Revision 1.9  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.8  2007/06/15 06:00:19  bobboau
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
 * Revision 1.7  2007/06/06 04:23:47  bobboau
 * UI cleanup, removed old UI
 *
 * Revision 1.6  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.5  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.4  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.3  2005/09/04 00:26:43  kazan
 * Enabled editor tab switching, added editor tab toolbar, enabled apply/revert for editors - add stubs for all editors
 *
 * Revision 1.2  2005/09/03 02:23:43  kazan
 * Header Editor Panel, Editors toolbar
 *
 * Revision 1.1  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
 *
 *
 *
*/

#if !defined(_pcs2_cids_h_)
#define _pcs2_cids_h_
#include <wx/setup.h>
#include <wx/wx.h>
enum
{
	// ---------------------------- MainPanel/MainWindow/Renderer ---------------------------- 
	// menu/toolbar
	// Stock items:
	// File menu.
	PCS2_MFILE_NEW=wxID_NEW,
	PCS2_MFILE_OPEN=wxID_OPEN,
	PCS2_MFILE_SAVE=wxID_SAVE,
	PCS2_MFILE_SAVEAS=wxID_SAVEAS,
	PCS2_MFILE_CLOSE=wxID_CLOSE,
	PCS2_MFILE_QUIT=wxID_EXIT,

	// Edit menu tabs
	PCS2_EDIT_UNDO=wxID_UNDO,
	PCS2_EDIT_REDO=wxID_REDO,

	// Options menu tabs
	PCS2_MOPT_PREF=wxID_PREFERENCES,

	// Without stock items:
	PCS2_MFILE_IMPORT=wxID_HIGHEST,

	
	// open file tabs
	PCS2_MFLIST_ONE,
	PCS2_MFLIST_TWO,
	PCS2_MFLIST_THREE,
	PCS2_MFLIST_FOUR,
	PCS2_MFLIST_FIVE,
	PCS2_MFLIST_SIX,
	PCS2_MFLIST_SEVEN,
	PCS2_MFLIST_EIGHT,
	PCS2_MFLIST_NINE,
	PCS2_MFLIST_TEN,

	//view menu
	PCS2_MVIEW_TOP,
	PCS2_MVIEW_BOTTOM,
	PCS2_MVIEW_LEFT,
	PCS2_MVIEW_RIGHT,
	PCS2_MVIEW_FRONT,
	PCS2_MVIEW_BACK,
	PCS2_MVIEW_RESET,

	// Edit menu tabs
	PCS2_EDIT_GEOIMPORT,

	// Options menu tabs
	PCS2_MOPT_GL_VERS,

	//Data menu
	PCS2_MDATA_ADD_LIST,
	PCS2_MDATA_ADD_ITEM,
	PCS2_MDATA_COPY_LIST,
	PCS2_MDATA_COPY_ITEM,
	PCS2_MDATA_DELETE_LIST,
	PCS2_MDATA_DELETE_ITEM,
	PCS2_MDATA_UP_LIST,
	PCS2_MDATA_DOWN_LIST,
	PCS2_MDATA_UP_ITEM,
	PCS2_MDATA_DOWN_ITEM,
	PCS2_MDATA_PURGEBSPCACHE,
	PCS2_MDATA_SMOOTHINGCALC,

	// editor tabs
	PCS2_TBCTRL_MLEFT,
	PCS2_TBCTRL_MRIGHT,
	PCS2_TBCTRL_MUP,
	PCS2_TBCTRL_MDOWN,
	PCS2_TBCTRL_ZIN,
	PCS2_TBCTRL_ZOUT,

	
	PCS2_TBCTRL_RLeft,
	PCS2_TBCTRL_RRight,
	PCS2_TBCTRL_Rup,
	PCS2_TBCTRL_Rdown,
	PCS2_TBCTRL_RCLeft,
	PCS2_TBCTRL_RCRight,

	PCS2_TBCTRL_XZ_PLANE,
	PCS2_TBCTRL_XY_PLANE,
	PCS2_TBCTRL_YZ_PLANE,

	PCS2_TBCTRL_PROJ_ORTHO,
	PCS2_TBCTRL_PROJ_PERSP,

	PCS2_TBCTRL_LOCK_X,
	PCS2_TBCTRL_LOCK_Y,
	PCS2_TBCTRL_LOCK_Z,

	PCS2_TBCTRL_GRID,

	PCS2_TBCTRL_WIREFRAME,
	PCS2_TBCTRL_SOLID,
	PCS2_TBCTRL_TEXTURED,

	// controls
	PCS2_CTRL_TREE,
	PCS2_CTRL_STATUS,
	PCS2_CTRL_GLCAN,


/*	PCS2_CTRL_TXTRL,

	// editor windows
	PCS2_EDITOR_GEO,
*/
		
	// timers
	PCS2_TIMER_GLDRAW,

	//graphics debug window
	GR_DEBUG_WINDOW,

	// -------------------------------- Array ctrl -------------------------------- 

	ARRAY_SELECT,
	ARRAY_BUTTON_NEW,
	ARRAY_BUTTON_COPY,
	ARRAY_BUTTON_DELETE,
	ARRAY_LIST,//this is the window ID of high level array controls, 
	//that is, an array control which represents and array of arrays
	ARRAY_ITEM,//this identifies a low level array control,
	//that is, an array of data

	// -------------------------------- MainPanel -------------------------------- 

	MAIN_PANEL_LOAD_CHUNK_BTN,
	MAIN_PANEL_SAVE_CHUNK_BTN,
	MAIN_PANEL_TRANSFORM_CHUNK_BTN,

	// -------------------------------- Progress Dialogs -------------------------------- 
	OPEN_FILE_PROGRESS_MESSAGER,
	SAVE_FILE_PROGRESS_MESSAGER,
	LOAD_TEXTURE_PROGRESS_MESSAGER,
	OPEN_IMPORT_PROGRESS_MESSAGER,

	// -------------------------------- misc editor stuff -------------------------------- 

	PATH_AUTO_GEN,
	WPN_CONVERGENCE,
	INSG_PROJECT,

	//convergence calculator
	CHECK_X,
	CHECK_Y,
	CHECK_Z,
	CHECK_THIS_ONLY,
	CONVERGE_POINT,
	CONVERGE_OK,
	CONVERGE_CANCEL,

	//MOI calculator
	MOI_CALC_btn,

	//BSP rendering
	BSP_DRAW,

	//normalize button for normal control
	NORM_NORMALIZE,

	//suggest button for suggest string control
	STRING_SUGGEST,

	//reload textures
	TXTR_RELOAD,
	TXTR_EXT_OPEN,

	//copy/delete selected subobject
	SOBJ_BUTTON_CPY,
	SOBJ_BUTTON_DEL,

	// -------------------------------- wxPCSPref -------------------------------- 


	PCS2_PREF_LBOX,
	PCS2_PREF_TADD,
	PCS2_PREF_TEDIT,
	PCS2_PREF_TDELE,
	PCS2_PREF_TSAVE, // saves changes on one you're editing
	PCS2_PREF_TBROWSE,// open a directory browser
	PCS2_PREF_TCTRL,

	PCS2_PREF_DEF_COL_BTN,
	PCS2_PREF_AMB_COL_BTN,

	PCS2_PREF_COL_OP_BTN,

	PCS2_PREF_CSFTXT, 

	PCS2_PREF_TMPD_TXT, 
	PCS2_PREF_TMPD_BTN, 
	PCS2_PREF_CKBX_VBO,
	PCS2_PREF_CKBX_GFILT,

	// Collada options
	PCS2_PREF_CKBX_HELPERS,
	PCS2_PREF_CKBX_PROPS_AS_HELPERS,
	PCS2_PREF_CKBX_DAE_MIRROR_X,
	PCS2_PREF_CKBX_DAE_MIRROR_Y,
	PCS2_PREF_CKBX_DAE_MIRROR_Z,

	// -------------------------------- color options -------------------------------- 

	COLOP_DLG,

	COLOP_UNSEL,
	COLOP_SEL_L,
	COLOP_SEL_I,

	// -------------------------------- pe_header -------------------------------- 
	PE_HEAD_ACENX,
	PE_HEAD_ACENY,
	PE_HEAD_ACENZ,

	PE_HEAD_MAXX,
	PE_HEAD_MAXY,
	PE_HEAD_MAXZ,
	
	PE_HEAD_MINX,
	PE_HEAD_MINY,
	PE_HEAD_MINZ,
	
	PE_HEAD_MASSX,
	PE_HEAD_MASSY,
	PE_HEAD_MASSZ,

	PE_HEAD_MASS,
	PE_HEAD_RADIUS,

	PE_HEAD_DETL,
	PE_HEAD_DETA,
	PE_HEAD_DETD,

	PE_HEAD_DEBL,
	PE_HEAD_DEBA,
	PE_HEAD_DEBD,

	PE_HEAD_CROL,
	PE_HEAD_CROA,
	PE_HEAD_CROD,

	PE_HEAD_EYEL,
	PE_HEAD_EYEA,
	PE_HEAD_EYED,

	PE_HEAD_MOI00,
	PE_HEAD_MOI01,
	PE_HEAD_MOI02,
	PE_HEAD_MOI10,
	PE_HEAD_MOI11,
	PE_HEAD_MOI12,
	PE_HEAD_MOI20,
	PE_HEAD_MOI21,
	PE_HEAD_MOI22,
};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE(ARRAY_NEW, -1)
DECLARE_LOCAL_EVENT_TYPE(ARRAY_CPY, -1)
DECLARE_LOCAL_EVENT_TYPE(ARRAY_DEL, -1)
DECLARE_LOCAL_EVENT_TYPE(ARRAY_INCREMENT, -1)
DECLARE_LOCAL_EVENT_TYPE(ARRAY_DECREMENT, -1)
DECLARE_LOCAL_EVENT_TYPE(COMBO_CLICKED, -1)
DECLARE_LOCAL_EVENT_TYPE(EDIT_DONE, -1)
DECLARE_LOCAL_EVENT_TYPE(DATA_SELECTION_CHANGED, -1)
DECLARE_LOCAL_EVENT_TYPE(BSP_RENDER_CHANGE, -1)
DECLARE_LOCAL_EVENT_TYPE(OMNIPOINT_RAY_PICKED, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_REBUILD_TREE, -1)
DECLARE_LOCAL_EVENT_TYPE(SUBOBJECT_UPDATE, -1)
DECLARE_LOCAL_EVENT_TYPE(UNDO_ENABLE, -1)
DECLARE_LOCAL_EVENT_TYPE(REDO_ENABLE, -1)
END_DECLARE_EVENT_TYPES()


#endif // _pcs2_cids_h_

