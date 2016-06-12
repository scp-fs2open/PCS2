//****************************************************************************
//	main_panel.cpp
//	POF Constructor Suite
//****************************************************************************

/*
 * $Logfile: /main_panel.cpp $
 * $Revision: 1.32 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: Main_Panel.cpp,v $
 * Revision 1.32  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.31  2008/11/10 23:25:19  kazan
 * more progress in transitioning
 *
 * Revision 1.30  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.29  2008/05/15 22:15:56  kazan
 * fix a crash
 *
 * Revision 1.28  2008/05/15 20:20:04  kazan
 * *** empty log message ***
 *
 * Revision 1.27  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.26  2008/05/15 17:43:58  kazan
 * *** empty log message ***
 *
 * Revision 1.25  2008/02/11 13:56:23  kazan
 * tweaks
 *
 * Revision 1.24  2008/02/09 23:01:40  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.23  2008/02/09 22:32:58  kazan
 * bye bye kaz_vector
 *
 * Revision 1.22  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.21  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.20  2008/02/03 21:45:23  kazan
 * Call main_panel::SignalModelChange("", true) to reset the view without changing the currently loaded model.
 * New clears the model then uses the above
 * Save calls the above at the end to refresh the view
 * Leaving the Preferences dialog does as well
 *
 * Revision 1.19  2008/01/14 11:46:32  bobboau
 * you forgot to disable VBO creation
 *
 * Revision 1.18  2008/01/12 18:02:23  bobboau
 * revamped the renderer to draw with VBOs current implementation assumes the user's hardware suports them, which is not a good assumption, the old code is still present and will be used as a fall back for when VBOs don't work, but I haven't implemented the code to do the falling back yet, but it's trivial.
 *
 * also did a few other minor things.
 *
 * Revision 1.17  2008/01/11 06:24:09  bobboau
 * fixing a spelling error, and modifyed the suggest box graphic a little
 *
 * Revision 1.16  2008/01/10 18:28:09  bobboau
 * *** empty log message ***
 *
 * Revision 1.15  2007/07/24 22:50:23  bobboau
 * added undo/redo functionality
 *
 * Revision 1.14  2007/07/18 17:57:47  bobboau
 * made a plaform dependent fix for DevIL crashing when loading the nameplate.dds file, we could include some sort of error mesage or log it when this happens, from testing this fix will still cause the debugger to break on exception, so we will still be able to catch this, but to the user the file will simply fail to load.
 * this is platform dependent and will need Special care for all suported platforms, currently it defaults to simple exception handeling when not in WIN32, but this will not usualy catch a divide by zero error.
 *
 * I also made some modifications to the renderer to make it behave more like FSO, it renders children first, and uses alpha blending, it can lead to some ugly results, but they will be just as ugly in FSO so it's good they get to see it.
 *
 * Revision 1.13  2007/07/15 17:35:16  bobboau
 * fixed STUPIDEST BUG EVER
 *
 * Revision 1.12  2007/07/13 11:31:04  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.11  2007/07/12 11:37:14  bobboau
 * added shine mapping suport.
 *
 * Revision 1.10  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.9  2007/07/11 19:14:47  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.8  2007/07/11 14:09:33  kazan
 * fix solid-view rendering bug, renable app modal progress dialogs during load
 *
 * Revision 1.7  2007/07/01 11:02:53  bobboau
 * modified the way things are drawn and implemented a few minor features
 *
 * Revision 1.6  2007/06/30 04:14:51  bobboau
 * commented out INSG tree item, untill a proper editor can be made for it.
 *
 * Revision 1.5  2007/06/29 21:00:26  bobboau
 * added shield chunk "editor", doesn't actualy let you edit anything, but will display shield poly count and activate the rendering of shileds, which I also added.
 *
 * Revision 1.4  2007/06/29 20:15:42  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.3  2007/06/28 19:03:10  kazan
 * add log headers
 *
 *
 */

#include"main_panel.h"
#include"chunk_editors.h"
#include"pcs2_filethread.h"
#include"VPReader.h"
#include<wx/mimetype.h>
#include<wx/process.h>
#include "AsyncProgress.h"
#include"selected.xpm"
#include"unselected.xpm"

#include"top_level_selected.xpm"
#include"top_level_unselected.xpm"

#include"selected_open.xpm"
#include"unselected_open.xpm"

#include"top_level_selected_open.xpm"
#include"top_level_unselected_open.xpm"

#include"dragable_selected.xpm"
#include"dragable_unselected.xpm"

#include"dragable_selected_open.xpm"
#include"dragable_unselected_open.xpm"

#include "matrix3d.h"

#include<wx/filename.h>
#include <wx/progdlg.h>
#include <wx/imaglist.h>
#include <wx/notebook.h>

#include <map>
#include <algorithm>

#define TREE_SEL 0
#define TREE_UNSEL 1
#define TREE_NOSEL 2
#define TREE_NOUNSEL 3
#define TREE_SEL_OP 4
#define TREE_UNSEL_OP 5
#define TREE_NOSEL_OP 6
#define TREE_NOUNSEL_OP 7
#define TREE_DR_SEL 8
#define TREE_DR_UNSEL 9
#define TREE_DR_SEL_OP 10
#define TREE_DR_UNSEL_OP 11

#if (wxMAJOR_VERSION >= 3)
#define wxOPEN wxFD_OPEN
#define wxFILE_MUST_EXIST wxFD_FILE_MUST_EXIST
#define wxGA_PROGRESSBAR 0
#endif

//ShellExecute(NULL, _T("open"), <filename that you want to open>, NULL, NULL, SW_SHOWNORMAL);
main_panel::main_panel(wxFrame* parent)
:wxPanel(parent,-1,wxPoint(0,0)),dragged_id(NULL),control_panel(NULL),pstatus(parent->GetStatusBar()),myParent(parent),pgauge(NULL),rebuilding_tree(false),internal_selection(false), threaded_prog_bar_readers(0), should_delete_threaded_prog_bar(false)
{
	gprog = NULL;
	
	threaded_prog_bar = NULL;
	UseThreadedProgBar = true;
	//+++++++++start status bar stuff
	int x = parent->GetSize().x;
	// size the statusbar fields
	int widths[3];
	isDestroyed = false;
	// 10%, 95%, 5%
	widths[0] = int(float(x)*0.15f);
	widths[1] = int(float(x)*0.70f);
	widths[2] = int(float(x)*0.15f);
	pstatus->SetFieldsCount(3, widths);

	pgauge = new wxGauge(pstatus, -1, 100,
								wxPoint(widths[0]+5, 3), 
								wxSize(widths[1], 17),
								wxGA_PROGRESSBAR | wxGA_SMOOTH);

	//+++++++++end status bar stuff


	wxGridSizer*sizer = new wxGridSizer(1,1,0,0);

	//the main spliter, holding the tree on the left and the second spliter on the right
	split1 = new wxSplitterWindow(this,-1,wxPoint(0,0),wxDefaultSize,wxSP_LIVE_UPDATE);	
	split1->SetSashSize(3);

	//the tree
	navigation_panel = new wxTreeCtrl(split1,-1,wxDefaultPosition,wxDefaultSize,wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT);
	wxImageList*IL = new wxImageList(16, 16, true, 2);
//	wxImageList*IL = new wxImageList();

	wxBitmap unsel(unselected);
	unsel.SetMask(new wxMask(unsel,*wxGREEN));

	wxBitmap sel(selected);
	sel.SetMask(new wxMask(sel,*wxGREEN));

	wxBitmap nosel(top_level_selected);
	nosel.SetMask(new wxMask(nosel,*wxGREEN));

	wxBitmap noselunsel(top_level_unselected);
	noselunsel.SetMask(new wxMask(noselunsel,*wxGREEN));

	wxBitmap unsel_op(unselected_open);
	unsel_op.SetMask(new wxMask(unsel_op,*wxGREEN));

	wxBitmap sel_op(selected_open);
	sel_op.SetMask(new wxMask(sel_op,*wxGREEN));

	wxBitmap nosel_op(top_level_selected_open);
	nosel_op.SetMask(new wxMask(nosel_op,*wxGREEN));

	wxBitmap noselunsel_op(top_level_unselected_open);
	noselunsel_op.SetMask(new wxMask(noselunsel_op,*wxGREEN));

	wxBitmap dr_unsel(dragable_unselected);
	dr_unsel.SetMask(new wxMask(dr_unsel,*wxGREEN));

	wxBitmap dr_sel(dragable_selected);
	dr_sel.SetMask(new wxMask(dr_sel,*wxGREEN));

	wxBitmap dr_unsel_op(dragable_unselected_open);
	dr_unsel_op.SetMask(new wxMask(dr_unsel_op,*wxGREEN));

	wxBitmap dr_sel_op(dragable_selected_open);
	dr_sel_op.SetMask(new wxMask(dr_sel_op,*wxGREEN));


	IL->Add(sel);
	IL->Add(unsel);
	IL->Add(nosel);
	IL->Add(noselunsel);
	IL->Add(sel_op);
	IL->Add(unsel_op);
	IL->Add(nosel_op);
	IL->Add(noselunsel_op);
	IL->Add(dr_sel);
	IL->Add(dr_unsel);
	IL->Add(dr_sel_op);
	IL->Add(dr_unsel_op);
	navigation_panel->AssignImageList(IL);


		//the second spliter, the gl canvas on the left the control panel on the right
		split2 = new wxSplitterWindow(split1,-1,wxPoint(0,0),wxDefaultSize,wxSP_LIVE_UPDATE);
		split2->SetSashSize(3);
		split2->SetMinimumPaneSize(30);

		//the glcanvas
		int attriblist[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, WX_GL_LEVEL, 0, 0 };
		glcanvas = new wxGL_PMFCanvas(split2, this, PCS2_CTRL_GLCAN,wxPoint(0,0), wxSize(10,10), model, attriblist);

		//the control panel
		control_pane = new wxPanel(split2);

			//sizer for the buttons on top of the control panel
			wxBoxSizer*button_sizer = new wxBoxSizer(wxHORIZONTAL);

			//the buttons themselves
			button_sizer->Add(load_chunk_btn = new wxButton(control_pane, MAIN_PANEL_LOAD_CHUNK_BTN, _("Load")),1.0);
			//button_sizer->Add(save_chunk_btn = new wxButton(control_pane, MAIN_PANEL_SAVE_CHUNK_BTN, _("Save")),1.0);
			button_sizer->Add(transform_chunk_btn = new wxButton(control_pane, MAIN_PANEL_TRANSFORM_CHUNK_BTN, _("Transform")),1.0);
		
			//sizer that holds the buttons and the actual control panel
			control_sizer = new wxBoxSizer(wxVERTICAL);
			control_sizer->Add(button_sizer,0,wxEXPAND);

			control_panel = NULL;
			control_sizer->Add(control_panel = new model_editor_ctrl<vector3d, ACEN_ctrl>					(control_pane, model, ACEN),1.0,wxEXPAND);
			glcanvas->set_active_chunk(ACEN);

//			control_sizer->Add(control_panel = new model_editor_ctrl<header_data, header_ctrl>				(control_pane, model, HDR2),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<vector3d, ACEN_ctrl>					(control_pane, model, ACEN),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<std::string>, TXTR_ctrl>		(control_pane, model, TXTR),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_slot>, WPNT_ctrl<GUN> >	(control_pane, model, GPNT),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_dock_point>, DOCK_ctrl>	(control_pane, model, DOCK),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_special>, SPCL_ctrl>	(control_pane, model, SPCL),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_eye_pos>, EYE_ctrl>		(control_pane, model, EYE),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_thruster>, FUEL_ctrl>	(control_pane, model, FUEL),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_turret>, TGUN_ctrl>		(control_pane, model, TGUN),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_glow_array>, GLOW_ctrl>	(control_pane, model, GLOW),1.0,wxEXPAND);
//			control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_path>, PATH_ctrl>		(control_pane, model, PATH),1.0,wxEXPAND);
	//		control_sizer->Add(control_panel = new model_editor_ctrl<pcs_sobj, SOBJ_ctrl>					(control_pane, model, SOBJ, -1),1.0,wxEXPAND);

			control_panel->EnableScrolling(false,true);
			control_panel->FitInside();
			control_panel->SetScrollRate(0,10);

		control_pane->SetSizer(control_sizer);
//		control_panel->SetBackgroundColour(*wxRED);

		split2->SplitVertically(glcanvas, control_pane,-30);
		split2->SetSashGravity(1.0f);


	split1->SetMinimumPaneSize(30);

	split1->SplitVertically(navigation_panel, split2,30);
	split1->SetSashGravity(0.0f);

//	wxSize S = split1->GetWindow1()->GetMinSize();
	
	//make sure this sizer takes up the whole panel
	sizer->Add(split1,1.0f,wxEXPAND);
	SetSizer(sizer);


	sizer->Layout();
	split2->SetSashPosition(-200);
	split1->SetSashPosition(150);



	//setup the tree
	tree_id.ROOT_ID = navigation_panel->AddRoot(_("model"));

	std::vector<int> path;

	tree_id.HDR2_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Header"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(HDR2, path));
	navigation_panel->SetItemImage(tree_id.HDR2_ID, TREE_NOSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.HDR2_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	path.resize(1);
	path[0] = -1;

	tree_id.SOBJ_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("SubObjects"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(SOBJ, path));
	navigation_panel->SetItemImage(tree_id.SOBJ_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.SOBJ_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);
		
	path.resize(2);
	path[1] = -1;

	tree_id.TXTR_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Textures"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(TXTR, path));
	tree_id.WEAP_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Weapon Points"), TREE_NOUNSEL, TREE_NOSEL);
		tree_id.GPNT_ID = navigation_panel->AppendItem(tree_id.WEAP_ID,_("Gun Points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(GPNT, path));
		tree_id.MPNT_ID = navigation_panel->AppendItem(tree_id.WEAP_ID,_("Missle Points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(MPNT, path));
	tree_id.DOCK_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Docking points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(DOCK, path));
	tree_id.FUEL_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Thrusters"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(FUEL, path));
	tree_id.GLOW_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Glow Points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(GLOW, path));
	tree_id.SPCL_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Special Points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(SPCL, path));
	tree_id.TGUN_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Turrets"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(TGUN, path));
	tree_id.PATH_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Paths"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(PATH, path));
	tree_id.EYE_ID  = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Eye Points"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(EYE, path));
	tree_id.INSG_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Insignia"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(INSG, path));

	path.resize(0);

	tree_id.SHLD_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Shield"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(SHLD, path));
	tree_id.ACEN_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Auto-centering"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(ACEN, path));
	tree_id.PINF_ID = navigation_panel->AppendItem(navigation_panel->GetRootItem(),_("Model Comments"), TREE_NOUNSEL, TREE_NOSEL, new tree_node_id(PINF, path));

	navigation_panel->SetItemImage(tree_id.TXTR_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.TXTR_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.WEAP_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.WEAP_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.GPNT_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.GPNT_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.MPNT_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.MPNT_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.DOCK_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.DOCK_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.FUEL_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.FUEL_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.GLOW_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.GLOW_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.SPCL_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.SPCL_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.TGUN_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.TGUN_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.PATH_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.PATH_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.EYE_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.EYE_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.INSG_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.INSG_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.SHLD_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.SHLD_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.ACEN_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.ACEN_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.PINF_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.PINF_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	navigation_panel->SetItemImage(tree_id.SHLD_ID, TREE_NOUNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(tree_id.SHLD_ID, TREE_NOSEL_OP, wxTreeItemIcon_SelectedExpanded);

	drwTimer = new wxTimer(this, PCS2_TIMER_GLDRAW);
	drwTimer->Start(33);

}

DEFINE_EVENT_TYPE(wxEVT_REBUILD_TREE)

BEGIN_EVENT_TABLE(main_panel, wxPanel)

	EVT_wxAsyncProgressStartEvt(OPEN_FILE_PROGRESS_MESSAGER, main_panel::open_progbar_start)
	EVT_wxAsyncProgressUpdateEvt(OPEN_FILE_PROGRESS_MESSAGER, main_panel::open_progbar_update)
	EVT_wxAsyncProgressEndEvt(OPEN_FILE_PROGRESS_MESSAGER, main_panel::open_progbar_end)

	EVT_wxAsyncProgressStartEvt(LOAD_TEXTURE_PROGRESS_MESSAGER, main_panel::texture_progbar_start)
	EVT_wxAsyncProgressUpdateEvt(LOAD_TEXTURE_PROGRESS_MESSAGER, main_panel::texture_progbar_update)
	EVT_wxAsyncProgressEndEvt(LOAD_TEXTURE_PROGRESS_MESSAGER, main_panel::texture_progbar_end)

	EVT_wxAsyncProgressStartEvt(OPEN_IMPORT_PROGRESS_MESSAGER, main_panel::import_progbar_start)
	EVT_wxAsyncProgressUpdateEvt(OPEN_IMPORT_PROGRESS_MESSAGER, main_panel::import_progbar_update)
	EVT_wxAsyncProgressEndEvt(OPEN_IMPORT_PROGRESS_MESSAGER, main_panel::import_progbar_end)


	EVT_BUTTON(MAIN_PANEL_LOAD_CHUNK_BTN,main_panel::on_load_chunk)
	EVT_BUTTON(MAIN_PANEL_SAVE_CHUNK_BTN,main_panel::on_save_chunk)
	EVT_BUTTON(MAIN_PANEL_TRANSFORM_CHUNK_BTN,main_panel::on_transform_chunk)

	EVT_BUTTON(SOBJ_BUTTON_CPY,main_panel::on_cpy_sobj)
	EVT_BUTTON(SOBJ_BUTTON_DEL,main_panel::on_del_sobj)

	EVT_SIZE(main_panel::OnSize)
	EVT_PAINT(main_panel::OnPaint)

    EVT_TREE_BEGIN_DRAG(wxID_ANY,	main_panel::tree_begin_drag)
    EVT_TREE_BEGIN_RDRAG(wxID_ANY,	main_panel::tree_begin_drag)
    EVT_TREE_END_DRAG(wxID_ANY,		main_panel::tree_end_drag)

	EVT_TREE_SEL_CHANGED(wxID_ANY,main_panel::on_tree_selection)
	EVT_COMMAND  (wxID_ANY, wxEVT_REBUILD_TREE, main_panel::on_rebuild_tree)

	EVT_COMMAND(wxID_ANY, ARRAY_NEW, main_panel::on_update_tree)
	EVT_COMMAND(wxID_ANY, ARRAY_CPY, main_panel::on_update_tree)
	EVT_COMMAND(wxID_ANY, ARRAY_DEL, main_panel::on_update_tree)
	EVT_COMMAND(wxID_ANY, EDIT_DONE, main_panel::on_update_data)

	EVT_COMMAND(wxID_ANY, DATA_SELECTION_CHANGED, main_panel::on_data_selection_change)


	EVT_COMMAND(wxID_ANY, BSP_RENDER_CHANGE, main_panel::on_BSP_render_change)

	EVT_TEXT(wxID_ANY, main_panel::on_data_selection_change)

	EVT_COMMAND(wxID_ANY, OMNIPOINT_RAY_PICKED, main_panel::on_omnipoint_ray_picked)

	EVT_COMMAND(wxID_ANY, SUBOBJECT_UPDATE, main_panel::on_update_subobject)
	
	EVT_BUTTON(TXTR_RELOAD,main_panel::on_texture_reload)
	EVT_BUTTON(TXTR_EXT_OPEN,main_panel::on_texture_external_open)

	EVT_BUTTON(MOI_CALC_btn,main_panel::on_moi_recalc)
END_EVENT_TABLE()

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//copy all decendents of subobject number copied_sobj
//to model with parent_new being copied_sobj's place in model
void copy_child_subobjects(PCS_Model&model, int parent_new, int copied_sobj){
	for(int i = 0; i<model.GetSOBJCount(); i++){
		if(model.SOBJ(i).parent_sobj == copied_sobj){
			int this_obj = model.GetSOBJCount();

			model.AddSOBJ(&model.SOBJ(i));

			model.SOBJ(this_obj).parent_sobj = parent_new;
			//set the copied object's parent to the new position

			copy_child_subobjects(model, this_obj, i);
		}
	}
}

void main_panel::on_cpy_sobj(wxCommandEvent &event){

	bool child_copy = false;
	for( int i = 0; i<model.GetSOBJCount(); i++){
		if(model.SOBJ(i).parent_sobj == model.get_active_model()){
			if(wxMessageBox(_("This subobject has children, would you like to copy them as well?"), _("Subobject Copy"), wxYES_NO) == wxYES)
				child_copy = true;
			break;
		}
	}

		std::vector<int> path;

		path.resize(1);

		//the number we started out with
		//will be the index of the (first) new subobject

	int new_obj = path[0] = model.GetSOBJCount();
	model.AddSOBJ(&model.SOBJ(model.get_active_model()));

	if(child_copy){
		copy_child_subobjects(model, new_obj, model.get_active_model());
	}

	//turret
	if(wxString(model.SOBJ(model.get_active_model()).properties.c_str(), wxConvUTF8).Contains(_("$fov=")) && 
		wxMessageBox(_("The subobject you have copied seems like it might be a turret, \nwould you like to copy the pertenent turret data if it exsists?"), _("Subobject Copy"), wxYES_NO) == wxYES)
	{
		//turrets first, then paths
		for( int i = 0; i<model.GetTurretCount(); i++){
			if(model.Turret(i).sobj_parent == model.get_active_model()){
				model.AddTurret(&model.Turret(i));
				pcs_turret&active_turret = model.Turret(model.GetTurretCount()-1);
				if(active_turret.sobj_parent == active_turret.sobj_par_phys){
					active_turret.sobj_par_phys = active_turret.sobj_parent = new_obj;
				}else{
					active_turret.sobj_parent = new_obj;

					//I guess we just need to find the last object with that name, it isn't bulletproff but there is no other way realy
					//it'll almost always by the first one we check but there could be wonky higherarchies (someone adds a radar dish as a child of a turret's base for example)
					std::string name = model.SOBJ(active_turret.sobj_par_phys).name;
					for(int idx = model.GetSOBJCount()-1; i>-1; i--){
						if(model.SOBJ(idx).name == name){
							active_turret.sobj_par_phys = idx;
							break;
						}
					}
				}

			}
		}
	}

		control_panel->set_item(path);
		control_panel->set_data(model);
		model.set_active_model(path[0]);

	on_update_tree(event);
}

//delete all decendents of subobject number del_sobj
//returns the first (position in the array) deleted object
int delete_child_subobjects(PCS_Model&model, int del_sobj){
	int ret = INT_MAX;
	for(unsigned int i = 0; (int)i<model.GetSOBJCount(); i++){
		if(model.SOBJ(i).parent_sobj == del_sobj){
			int youngest = delete_child_subobjects(model, i);
			if(youngest < (int)i){
				i=-1;
				if(ret > youngest)ret = youngest;
				continue;
				//something before us was deleted, start over, our index is no longer valid
			}
			if(ret > (int)i)ret = i;
			model.DelSOBJ(i);
		}
	}
	return ret;
}

void main_panel::on_del_sobj(wxCommandEvent &event){
	if(model.get_active_model() < 0 || model.get_active_model() >= (int)model.get_subobjects().size())
		return;//no valis subobject selected
	if(wxMessageBox(wxString::Format(_("Are your *REALLY SURE* you want to delete the object %s?\nbecause it will be gone if you do."), model.SOBJ(model.get_active_model()).name.c_str()), _("Subobject Delete"), wxYES_NO) == wxNO)
		return;

	bool child_delete = false;

	std::vector<int> path;

	path.resize(1);
	path[0] = model.SOBJ(model.get_active_model()).parent_sobj;

	for( int i = 0; i<model.GetSOBJCount(); i++){
		if(model.SOBJ(i).parent_sobj == model.get_active_model()){
			if(wxMessageBox(_("This subobject has children, would you like to kill them as well?"), _("Subobject Delete"), wxYES_NO) == wxYES)
				child_delete = true;
			break;
		}
	}

	if(child_delete){
		delete_child_subobjects(model, model.get_active_model());
	}

		//turret
	if(wxString(model.SOBJ(model.get_active_model()).properties.c_str(), wxConvUTF8).Contains(_("$fov=")) && 
		wxMessageBox(_("The subobject you have deleted seems like it might be a turret, \nwould you like to delete the pertenent turret data if it exsists?"), _("Subobject Delete"), wxYES_NO) == wxYES)
	{
		for( int i = 0; i<model.GetTurretCount(); i++){
			if(model.Turret(i).sobj_parent == model.get_active_model()){
				model.DelTurret(i);
			}
		}
	}

	model.DelSOBJ(model.get_active_model());
		
	path[0] = model.get_active_model();

	control_panel->set_item(path);
	control_panel->set_data(model);

	on_update_tree(event);
}

void remap_texture_ids(pcs_sobj& sobj, std::vector<std::string>& textures, const std::vector<std::string>& other_textures, std::map<std::string, int>& texture_map, std::vector<int>& texture_id_map) {
	for (std::vector<pcs_polygon>::iterator it = sobj.polygons.begin(); it < sobj.polygons.end(); ++it) {
		if (it->texture_id != -1) {
			if (texture_id_map[it->texture_id] == -1) {
				if (texture_map.find(other_textures[it->texture_id]) == texture_map.end()) {
					texture_map[other_textures[it->texture_id]] = textures.size();
					textures.push_back(other_textures[it->texture_id]);
				}
				texture_id_map[it->texture_id] = texture_map[other_textures[it->texture_id]];
			}
			it->texture_id = texture_id_map[it->texture_id];
		}
	}
}

void copy_subobjects(PCS_Model& destination, PCS_Model& source, int sobj, int parent, bool children, bool turret, bool path, std::vector<std::string>& textures, const std::vector<std::string>& other_textures, std::map<std::string, int>& texture_map, std::vector<int>& texture_id_map) {
	remap_texture_ids(source.SOBJ(sobj), textures, other_textures, texture_map, texture_id_map);
	int dest_id = destination.GetSOBJCount();
	destination.AddSOBJ(&source.SOBJ(sobj));
	destination.SOBJ(dest_id).parent_sobj = parent;
	if (path) {
		for(int i = 0; i<source.GetPathCount(); i++){
			if(wxString(source.Path(i).parent.c_str(), wxConvUTF8).Contains(wxString(source.SOBJ(sobj).name.c_str(), wxConvUTF8))){
				destination.AddPath(&source.Path(i));
			}
		}
	}
	std::map<int, int> old_children_to_new;
	if (children) {
		for (int i = 0; i < source.GetSOBJCount(); i++) {
			if (source.SOBJ(i).parent_sobj == sobj) {
				old_children_to_new[i] = destination.GetSOBJCount();
				copy_subobjects(destination, source, i, dest_id, children, turret, path, textures, other_textures, texture_map, texture_id_map);
			}
		}
	}
	if (turret) {
		for(int i = 0; i<source.GetTurretCount(); i++){
			if(source.Turret(i).sobj_parent == sobj){
				int dest_turret_id = destination.GetTurretCount();;
				destination.AddTurret(&source.Turret(i));
				destination.Turret(dest_turret_id).sobj_parent = dest_id;

				// Try guessing the right barrels subobject id to use.
				if (source.Turret(i).sobj_par_phys != source.Turret(i).sobj_parent && old_children_to_new.find(source.Turret(i).sobj_par_phys) != old_children_to_new.end()) {
					destination.Turret(dest_turret_id).sobj_par_phys = old_children_to_new[source.Turret(i).sobj_par_phys];
				} else {
					destination.Turret(dest_turret_id).sobj_par_phys = dest_id;
				}
			}
		}
	}
}

void main_panel::on_transform_chunk(wxCommandEvent &event){
	transform_dialog dialog(NULL, control_panel);
	if (dialog.ShowModal() == wxID_OK) {
		control_panel->transform(dialog.get_transform(), dialog.get_translation());
	}

}

void main_panel::on_load_chunk(wxCommandEvent &event){

	wxFileDialog fdlg(NULL, _("Select Import File"), _(""), _(""), PCS2_SUPPORTED_FORMATS, wxOPEN | wxFILE_MUST_EXIST);
	if (fdlg.ShowModal() != wxID_OK)
		return;

	PCS_Model import_model;

	if(!LoadImportModel(std::string(fdlg.GetPath().mb_str()), &import_model))
		return;

	if(control_panel->chunk_type == SOBJ){
		//import of subobjects adds new subobjects rather than replaces exsisting ones


		int i;

		//figure out which subobjects the user wants

		//get all the available subobjects
		wxArrayString sobj_names;

		sobj_names.Add(_("Metadata"));
		sobj_names.Add(_("All Subobjects"));

		for( i = 0; i<import_model.GetSOBJCount(); i++){
			sobj_names.Add(wxString(import_model.SOBJ(i).name.c_str(), wxConvUTF8));
		}

		int imported_sobj = wxGetSingleChoiceIndex( _("Select Subobject for Importation"), _("Subobject Import"), sobj_names, this);


		
		//user canceled
		if(imported_sobj <0)
			return;

		if (imported_sobj == 0) {
			import_sobj_metadata(import_model);
		} else {
			imported_sobj--;
			std::vector<int> path;

			path.resize(1);

			//the number we started out with
			//will be the index of the (first) new subobject
			path[0] = model.GetSOBJCount();

			glcanvas->FreezeRender = true;
			std::map<std::string, int> texture_map;
			std::vector<std::string> textures = model.get_textures();
			std::vector<std::string> other_textures = import_model.get_textures();
			std::vector<int> texture_id_map(other_textures.size(), -1);
			for (std::vector<std::string>::iterator it = textures.begin(); it < textures.end(); ++it) {
				texture_map[*it] = (int)(it - textures.begin());
			}
			bool turret_import = false;
			bool path_import = false;
			//turret/path import
			if(wxMessageBox(_("Import associated turrets?"), _("Subobject Import"), wxYES_NO) == wxYES)
			{
				turret_import = true;
			}

			//path import
			if(wxMessageBox(_("Import associated paths?"), _("Subobject Import"), wxYES_NO) == wxYES)
			{
				path_import = true;
			}
			if(imported_sobj){
				imported_sobj--;
				bool child_import = false;
				for( i = 0; i<import_model.GetSOBJCount(); i++){
					if(import_model.SOBJ(i).parent_sobj == imported_sobj){
						if(wxMessageBox(_("This subobject has children, would you like to import them as well?"), _("Subobject Import"), wxYES_NO) == wxYES)
							child_import = true;
						break;
					}
				}
				copy_subobjects(model, import_model, imported_sobj, -1, child_import, turret_import, path_import, textures, other_textures, texture_map, texture_id_map);
			}else{
				for( i = 0; i<import_model.GetSOBJCount(); i++){
					if (import_model.SOBJ(i).parent_sobj == -1) {
						copy_subobjects(model, import_model, i, -1, turret_import, path_import, true, textures, other_textures, texture_map, texture_id_map);
					}
				}
			}

			model.set_textures(textures);
			glcanvas->reload_textures();
			glcanvas->FreezeRender = false;
			control_panel->set_item(path);
			control_panel->set_data(model);
		}
	} else if (control_panel->chunk_type == TGUN) {
		import_turrets(import_model);
		control_panel->set_data(model);
	}else{
		control_panel->set_data(import_model);
		//set the data in the control to the data in the import model
		control_panel->apply_data(model);
		//apply the data in the control (which was from the import model) to our model
	}
	on_update_tree(event);
}

void main_panel::import_sobj_metadata(PCS_Model& import_model) {
	typedef std::map<std::string, int> NameIdMap;
	NameIdMap local_map;
	for (int i = 0; i < model.GetSOBJCount(); i++) {
		local_map[model.SOBJ(i).name] = i;
	}
	for (int i = 0; i < import_model.GetSOBJCount(); i++) {
		pcs_sobj& sobj = import_model.SOBJ(i);
		NameIdMap::iterator it = local_map.find(sobj.name);
		if (it != local_map.end()) {
			pcs_sobj& local_sobj = model.SOBJ(it->second);
			local_sobj.properties = sobj.properties;
			local_sobj.movement_type = sobj.movement_type;
			local_sobj.movement_axis = sobj.movement_axis;
		}
	}
}

void main_panel::import_turrets(PCS_Model& import_model) {
	typedef std::map<std::string, int> NameIdMap;
	NameIdMap local_map;
	for (int i = 0; i < model.GetSOBJCount(); i++) {
		local_map[model.SOBJ(i).name] = i;
	}
	std::vector<pcs_turret> turrets;
	for (int i = 0; i < import_model.GetTurretCount(); i++) {
		pcs_turret& import_turret = import_model.Turret(i);
		if (import_turret.sobj_parent > 0 &&
			import_turret.sobj_parent < import_model.GetSOBJCount() &&
			import_turret.sobj_par_phys > 0 &&
			import_turret.sobj_par_phys < import_model.GetSOBJCount()) {
			int local_parent = -1;
			int local_phys_parent = -1;
			NameIdMap::iterator jt;
			if ((jt = local_map.find(import_model.SOBJ(import_turret.sobj_parent).name)) !=
			   	local_map.end()) {
				local_parent = jt->second;
			}
			if ((jt =
					local_map.find(import_model.SOBJ(import_turret.sobj_par_phys).name)) !=
				local_map.end()) {
				local_phys_parent = jt->second;
			}
			if (local_phys_parent != -1 && local_parent != -1) {
				pcs_turret turret(import_turret);
				turret.sobj_parent = local_parent;
				turret.sobj_par_phys = local_phys_parent;
				turrets.push_back(turret);
			}
		}
	}
	model.set_turrets(turrets);
}

void main_panel::on_save_chunk(wxCommandEvent &event){
//	control_panel->save_chunk();
}


void main_panel::on_BSP_render_change(wxCommandEvent &event){
	model.draw_bsp = event.GetInt()!=0;
	glcanvas->Render(); // force update (render) on change
}

void main_panel::OnSize(wxSizeEvent& event)
{
	if (isDestroyed) return; // for some reason we're crashing without this - Kaz

	int x = event.GetSize().GetWidth();


	// size the statusbar fields
	int widths[3];
	// 10%, 95%, 5%
	widths[0] = int(float(x)*0.15f);
	widths[1] = int(float(x)*0.70f);
	widths[2] = int(float(x)*0.15f);
	pstatus->SetFieldsCount(3, widths);

	pgauge->SetSize(widths[0]+5, 3, widths[1]-2, pstatus->GetSize().GetHeight()-3);

	event.Skip();
}

//shamelessly stolen from kaz's main panel
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


bool main_panel::Destroy()
{
	isDestroyed=true;
	model.Reset();
	glcanvas->FreezeRender = true;
	while (glcanvas->IsRendering);
	glcanvas->Destroy();

	//dtree->Destroy();
	return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool main_panel::LoadImportModel(std::string filename, PCS_Model*import_model) 
{
	AsyncProgress* prog_messenger(new wxAsyncProgress(this, OPEN_IMPORT_PROGRESS_MESSAGER));

	wxPCS2OpenThread *thread = new wxPCS2OpenThread(import_model, filename, prog_messenger, false);

	thread->Create();
	thread->Run();

	// unfortunately for this one we have to block - all the calling functions expect this function to block

	while (!prog_messenger->Finished())
	{	
		wxMicroSleep(250);
		wxSafeYield(this->GetParent(),true);
	}

	bool result = !prog_messenger->EarlyTerminated();
	delete prog_messenger;
	return result;
	/*
	bool *IsRunning = new bool; *IsRunning = true;
	int *comp_target = new int; *comp_target = 0;
	int *comp_current = new int; *comp_current = 0;
	int *error_code = new int; *error_code = 0;
	char *comp_msg = new char[128];
	memset(comp_msg, 0, 128);
	int retval;

	int last_cur = 0;
	int comp;
	wxString msg = "Starting Load";

	wxProgressDialog *prog = new wxProgressDialog("Opening File", msg); 
	import_model->Reset();
	wxPCS2OpenThread *thread = new wxPCS2OpenThread(this, import_model, comp_target, comp_current, IsRunning, error_code, filename, comp_msg);
	thread->Create();
	thread->Run();

	while (*IsRunning)
	{
		if (comp_target != 0)
		{
			if (last_cur != *comp_current)
			{
				last_cur = *comp_current;
				msg = comp_msg;
				comp = int((float(*comp_current)/float(*comp_target)*100.0));
				if (comp > 100)
					comp = 100;
				prog->Update(comp, msg);
			}
		}
		wxMicroSleep(250);
		wxSafeYield(this->GetParent(),true);
		
	}	
	delete prog;

	retval = *error_code;
	delete IsRunning;
	delete comp_target;
	delete comp_current;
	delete error_code;
	delete[] comp_msg;

	return retval==0;*/
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::global_import(std::string filename){
	PCS_Model import_model;

	if(!LoadImportModel(filename, &import_model))
		return;

	model.set_ai_paths(import_model.get_ai_paths());
	model.set_docking(import_model.get_docking());
	model.set_eyes(import_model.get_eyes());
	model.set_glow_points(import_model.get_glow_points());
	model.set_insignia(import_model.get_insignia());
	model.set_shield_mesh(import_model.get_shield_mesh());
	model.set_special(import_model.get_special());
	model.set_thrusters(import_model.get_thrusters());
	model.set_weapons(import_model.get_weapons());
	import_turrets(import_model);
	import_sobj_metadata(import_model);

	wxCommandEvent event;
	on_update_tree(event);

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::ImportGeometry(std::string filename)
{
	PCS_Model import_model;

	if(!LoadImportModel(filename, &import_model))
		return;

	// merge texture list
	std::vector<std::string> txts = model.get_textures();
	
	for (int i = 0; i < import_model.GetTexturesCount(); i++)
	{
		if (!in_array<std::string>(txts, import_model.Texture(i)))
		{
			model.AddTextures(import_model.Texture(i));
		}
	}

	std::map<int,int> txtmap;
	for (int i = 0; i < import_model.GetTexturesCount(); i++)
	{
		txtmap[i] = find<std::string>(txts, import_model.Texture(i));
	}

	pcs_sobj tSobj;

	std::map<int,int> sobjIDs;

	for (int i = 0; i < import_model.GetSOBJCount(); i++)
	{
		tSobj = import_model.SOBJ(i);
		if (tSobj.parent_sobj != -1)
			tSobj.parent_sobj = sobjIDs[tSobj.parent_sobj];
		//tSobj.name = std::string("imported_") + tSobj.name;

		// remap textures -- does this need to be done in the vertex buffer as well? or do we just destroy and recreate it?
		for (size_t j = 0; j < tSobj.polygons.size(); j++)
		{
			tSobj.polygons[j].texture_id = txtmap[tSobj.polygons[j].texture_id];
		}

		model.AddSOBJ(&tSobj);
		sobjIDs[i] = model.GetSOBJCount()-1;
	}



	// reset the view, load textures, etc
	SignalModelChange("");
	
	wxCommandEvent event;
	on_update_tree(event);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::open_progbar_start(wxAsyncProgressStartEvt &event)
{
	glcanvas->FreezeRender = true;
	if (UseThreadedProgBar)
	{
		threaded_prog_bar_readers++;
		if (threaded_prog_bar == NULL) {
			threaded_prog_bar = new wxProgressDialog(_("Opening File"), _("Starting File Load")); 
		} else {
			threaded_prog_bar->Update(0, _("Starting File Load"));
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

void main_panel::open_progbar_update(wxAsyncProgressUpdateEvt &event)
{
	int percent = std::min(std::max((int)event.getPercent(), 0), 100);
	pstatus->SetStatusText(wxString(event.getMessage().c_str(), wxConvUTF8), 0);
	pgauge->SetValue(percent);
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

void main_panel::open_progbar_end(wxAsyncProgressEndEvt &event)
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
	pstatus->SetStatusText(_("Idle"), 0);
	pgauge->SetValue(0);

	if (event.getEarlyTerminated())
	{
		wxMessageDialog errormsg((wxWindow*)this, 
			wxString(event.getMessage().c_str(), wxConvUTF8), 
			_("Error Opening File"), wxOK | wxICON_HAND);
		errormsg.ShowModal();
	
	}

	model.Rcall_Wireframe(false);
	model.Rcall_Textureless(false);
	model.set_active_model(
			(model.GetLODCount() > 0 && model.LOD(0) >- 1) ? model.LOD(0) : 0
			);//set to the main LOD if posable

	int itemp;
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->SetPath(_T("/gr_options/"));
	pConfig->Read(_("use_vertex_buffer_objects"), &itemp, 0); // default to off
	model.init_vertex_buffers(itemp != 0);

	//reset the editor, would be nice if I could keep the old one, 
	//but there could be hidden data from the old model, so to be safe
	//were just going to start over.
	control_sizer->Detach(control_panel);
	control_panel->Destroy();
	control_panel = NULL;
	control_sizer->Add(control_panel = new model_editor_ctrl<header_data, header_ctrl>					(control_pane, model, HDR2),1.0,wxEXPAND);
	control_panel->set_data(model);
	control_sizer->Layout();
	control_panel->EnableScrolling(false,true);
	control_panel->FitInside();
	control_panel->SetScrollRate(0,10);
	glcanvas->set_active_chunk(HDR2);



	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);


	rebuild_tree();
	control_panel->reset_undo();
	control_panel->push_undo();


	glcanvas->FreezeRender = false;

	// start texture load
	glcanvas->Reinit(true);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::texture_progbar_end(wxAsyncProgressEndEvt &event)
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
	pstatus->SetStatusText(_("Idle"), 0);
	pgauge->SetValue(0);

	if (event.getEarlyTerminated())
	{
		wxMessageDialog errormsg((wxWindow*)this, 
			wxString(event.getMessage().c_str(), wxConvUTF8),
			_("Error Loading Textures"), wxOK | wxICON_HAND);
		errormsg.ShowModal();
	
	}
	
	glcanvas->FreezeRender = false;
	glcanvas->Render();
}



//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::SignalModelChange(std::string filename, bool skipProgdlg) 
{
	UseThreadedProgBar = !skipProgdlg;
	wxSetWorkingDirectory(wxString(filename.c_str(), wxConvUTF8).BeforeLast('\\'));
	glcanvas->FreezeRender = true;
	//while (glcanvas->IsRendering);

	AsyncProgress* prog_messenger;

	if (filename != "")
	{
		prog_messenger = new wxAsyncProgress(this, OPEN_FILE_PROGRESS_MESSAGER);
		model.Reset();
		wxPCS2OpenThread *thread = new wxPCS2OpenThread(&model, filename, prog_messenger);

		thread->Create();
		thread->Run();

	}
	else
	{
		// we're just triggering a texture reload
		glcanvas->FreezeRender = false;
		glcanvas->Reinit(false);
	}

	
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::on_texture_reload(wxCommandEvent& event){
	control_panel->apply_data(model);
	glcanvas->reload_textures();
	pgauge->SetValue(0);
	glcanvas->Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//small class used to reload textures when the editor program closes
/*
class texture_open_process : public wxProcess{
	wxWindow * Parent;
public:
	texture_open_process(wxWindow * parent):wxProcess(parent->GetEventHandler()),Parent(parent){}
	void OnTerminate(int pid, int status){
		Parent->GetEventHandler()->ProcessEvent(wxCommandEvent(TXTR_RELOAD, -1));
		delete this;//wow you don't see that every day
	}
};
*/

void main_panel::open_texture_externaly(wxString filename){
	if(filename == _(""))
		return;

	wxString file(filename);
	if(filename.Contains(_(".vp:"))){
		//if the texture is in a VP we need to extract it to the temp directory
		//and open it from there
		wxString vp_file = filename.BeforeLast(':');
		file = filename.AfterLast(':');

		filename = wxFileName::CreateTempFileName(_(""));
		VolitionPackfileReader::ReadFromVP(std::string(vp_file.mb_str()), std::string(file.mb_str()), std::string(filename.mb_str()));
	}

	wxFileType*file_type = wxTheMimeTypesManager->GetFileTypeFromExtension(file.AfterLast('.'));
	wxString open_cmd = file_type->GetOpenCommand(filename);
	wxExecute(open_cmd, wxEXEC_ASYNC);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void main_panel::on_texture_external_open(wxCommandEvent& event){
	if(control_panel->chunk_type != TXTR)return;
	wxString filename(glcanvas->get_texture_filename(control_panel->get_item()[0]).c_str(), wxConvUTF8);
	open_texture_externaly(filename);
	filename = wxString(glcanvas->get_texture_filename(control_panel->get_item()[0], TC_GLOW).c_str(), wxConvUTF8);
	open_texture_externaly(filename);
	filename = wxString(glcanvas->get_texture_filename(control_panel->get_item()[0], TC_SHINEMAP).c_str(), wxConvUTF8);
	open_texture_externaly(filename);

	//if the user has set there temp directory to an active mod directory
	//then lets see if we can load the textures in question
	glcanvas->reload_textures();
	pgauge->SetValue(0);
}

void main_panel::on_moi_recalc(wxCommandEvent& event){
		if(wxMessageBox(_("Are you sure you want PCS2 to try to recalculate the MOI?\nit\'s probably not very good at it."), _("MOI Recalc"), wxYES_NO) == wxNO)
			return;
		vector3d min = model.GetMinBounding();
		vector3d max = model.GetMaxBounding();
		model.SetMOI(model.moi_recalculate(log(abs(max.x-min.x)+10)+10, log(abs(max.z-min.z)+10)+10).a2d);

		control_panel->set_data(model);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void main_panel::on_update_data(wxCommandEvent& event){
	if(!control_panel)return;
	if(!control_panel->ready())return;
	if(!glcanvas)return;

	control_panel->apply_data(model);
	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);
	glcanvas->Render();
	rebuild_tree();
	control_panel->push_undo();
}

void main_panel::on_data_selection_change(wxCommandEvent& event){
	if(!control_panel)return;
	if(!control_panel->ready())return;
	if(!glcanvas)return;
	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);
	glcanvas->Render();
	//make sure we try to set it to the proper item
//	update_tree_selection(control_panel->chunk_type, control_panel->get_item());
}

void main_panel::on_update_tree(wxCommandEvent& event){
	on_update_data(event);
	//make sure we try to set it to the proper item
	//update_tree_selection(control_panel->chunk_type, control_panel->get_item());
}

void main_panel::on_update_subobject(wxCommandEvent& event){
	if(!control_panel)return;
	if(!control_panel->ready())return;
	if(!glcanvas)return;

	control_panel->apply_data(model);
	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);
	glcanvas->Render();
}

void main_panel::Revert_data(wxCommandEvent &event){
	if(control_panel)control_panel->set_data(model);
};
void main_panel::Apply_data(wxCommandEvent &event){
	if(control_panel)control_panel->apply_data(model);
	rebuild_tree();
};

void main_panel::undo(){
	control_panel->push_undo();
	//make sure the undo buffer is up to date

	control_panel->undo();
	control_panel->apply_data(model);
	rebuild_tree();
	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);
	glcanvas->Render();
}
void main_panel::redo(){
	control_panel->redo();
	control_panel->apply_data(model);
	rebuild_tree();
	glcanvas->set_omnipoints(control_panel->get_omnipoints());
	int row, item;
	control_panel->get_omnipoint_coords(row, item);
	glcanvas->set_selected_omni_points(row, item);
	glcanvas->Render();
}

void main_panel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	glcanvas->Render();
}

inline void kill_kids(wxTreeCtrl*tree, wxTreeItemId&id){
	wxTreeItemIdValue i;
	while(tree->GetFirstChild(id, i).IsOk())
		tree->Delete(tree->GetFirstChild(id, i));
}

//a semi-recursive fix for situations were a model is a child 
//to a model who's node has not been made yet
void main_panel::parents_first(int i, std::vector<wxTreeItemId>&model_nodes){
	if(model_nodes[i])
		return;

	std::vector<int> path;
	path.resize(1);
	path[0] = i;
	wxTreeItemId parent = tree_id.SOBJ_ID;
	if(model.SOBJ(i).parent_sobj != -1){
			if(i < model.SOBJ(i).parent_sobj){
				parents_first(model.SOBJ(i).parent_sobj, model_nodes);
			}
			parent = model_nodes[model.SOBJ(i).parent_sobj];
	}
	wxString label(model.SOBJ(i).name.c_str(), wxConvUTF8);
	label += _(" (");
	label += itoa(i);
	label += _(")");
	model_nodes[i] = navigation_panel->AppendItem(parent, label, TREE_UNSEL, TREE_SEL, new tree_node_id(SOBJ, path, true));
	navigation_panel->SetItemImage(model_nodes[i], TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
	navigation_panel->SetItemImage(model_nodes[i], TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);
}

void main_panel::rebuild_tree(){

	rebuilding_tree = true;
	//reset the tree to it's empty state
	kill_kids(navigation_panel, tree_id.HDR2_ID);
	kill_kids(navigation_panel, tree_id.SOBJ_ID);
	kill_kids(navigation_panel, tree_id.TXTR_ID);
	kill_kids(navigation_panel, tree_id.GPNT_ID);
	kill_kids(navigation_panel, tree_id.MPNT_ID);
	kill_kids(navigation_panel, tree_id.DOCK_ID);
	kill_kids(navigation_panel, tree_id.FUEL_ID);
	kill_kids(navigation_panel, tree_id.GLOW_ID);
	kill_kids(navigation_panel, tree_id.SPCL_ID);
	kill_kids(navigation_panel, tree_id.TGUN_ID);
	kill_kids(navigation_panel, tree_id.PATH_ID);
	kill_kids(navigation_panel, tree_id.EYE_ID);
	kill_kids(navigation_panel, tree_id.INSG_ID);
	kill_kids(navigation_panel, tree_id.SHLD_ID);
	kill_kids(navigation_panel, tree_id.ACEN_ID);
	kill_kids(navigation_panel, tree_id.PINF_ID);
	kill_kids(navigation_panel, tree_id.SHLD_ID);

	//build it
	int i;
	std::vector<int> path;
	//common variables

	std::vector<wxTreeItemId> model_nodes;
	model_nodes.resize(model.GetSOBJCount());

	path.resize(1);

	for(i=0; i<model.GetSOBJCount(); i++){
		if(model_nodes[i])
			continue;

		path[0] = i;
		wxTreeItemId parent = tree_id.SOBJ_ID;
		if(model.SOBJ(i).parent_sobj != -1){
			if(i < model.SOBJ(i).parent_sobj){
				parents_first(model.SOBJ(i).parent_sobj, model_nodes);
			}
			parent = model_nodes[model.SOBJ(i).parent_sobj];
		}
		wxString label(model.SOBJ(i).name.c_str(), wxConvUTF8);
		label += _(" (");
		label += itoa(i);
		label += _(")");
		model_nodes[i] = navigation_panel->AppendItem(parent, label, TREE_UNSEL, TREE_SEL, new tree_node_id(SOBJ, path, true));
		navigation_panel->SetItemImage(model_nodes[i], TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(model_nodes[i], TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);
	}

	for(i=0; i<model.GetTexturesCount(); i++){
		path[0] = i;
		navigation_panel->AppendItem(tree_id.TXTR_ID, wxString(model.Texture(i).c_str(), wxConvUTF8), TREE_UNSEL, TREE_SEL, new tree_node_id(TXTR, path));
	}


	for(i=0; i<model.GetSpecialCount(); i++){
		path[0] = i;
		wxString str(model.Special(i).name.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Special Point %i"), i+1);
		navigation_panel->AppendItem(tree_id.SPCL_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(SPCL, path));
	}

	for(i=0; i<model.GetEyeCount(); i++){
		path[0] = i;
		wxString str = _("");
		if(model.Eye(i).sobj_number > -1 && model.Eye(i).sobj_number < model.GetSOBJCount())str = wxString(model.SOBJ(model.Eye(i).sobj_number).name.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Eye %i"), i+1);
		navigation_panel->AppendItem(tree_id.EYE_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(EYE, path));
	}

	path.resize(2);

	for(i=0; i<model.GetInsigniaCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.INSG_ID, wxString::Format(_("Insignia %d"), i+1), TREE_UNSEL, TREE_SEL, new tree_node_id(INSG, path));
		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);
		for (unsigned int j = 0; j < model.get_insignia()[i].faces.size(); j++) {
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString::Format(_("Face %d"), j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(INSG, path));
		}
	}

	int g = 0;//gun
	int m = 0;//mis

	for(i=0; i<model.GetWeaponCount(); i++){
		wxTreeItemId parent;
		wxString w_type_str;
		CHUNK ctype;
		if(model.Weapon(i).type == GUN){
			path[0] = g++;
			path[1] = -1;
			w_type_str = _("Gun Point ");
			parent = navigation_panel->AppendItem(tree_id.GPNT_ID, wxString().Format(_("Gun Bank %i"),g), TREE_UNSEL, TREE_SEL, new tree_node_id(GPNT, path));
			ctype = GPNT;
		}else{
			path[0] = m++;
			path[1] = -1;
			w_type_str = _("Missile Point ");
			parent = navigation_panel->AppendItem(tree_id.MPNT_ID, wxString().Format(_("Missile Bank %i"),m), TREE_UNSEL, TREE_SEL, new tree_node_id(MPNT, path));
			ctype = MPNT;
		}

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Weapon(i).muzzles.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("%s%i"), w_type_str.c_str(), j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(ctype, path, true));
		}
	}


	for(i=0; i<model.GetDockingCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxString str(model.Dock(i).properties.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Dock %i"), i+1);
		else str.Replace(_("$name="),_(""));
		str.Trim(true);
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.DOCK_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(DOCK, path));

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Dock(i).dockpoints.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("Docking Point %i"),j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(DOCK, path, true));
		}
	}

	for(i=0; i<model.GetThrusterCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxString str(model.Thruster(i).properties.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Thruster %i"), i+1);
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.FUEL_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(FUEL, path));

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Thruster(i).points.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("Thruster Point %i"),j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(FUEL, path, true));
		}
	}

	for(i=0; i<model.GetLightCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxString str(model.Light(i).properties.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Glow Bank %i"), i+1);
		else str.Replace(_("$glow_texture="),_(""));
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.GLOW_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(GLOW, path));

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Light(i).lights.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("Glow Point %i"),j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(GLOW, path, true));
		}
	}

	for(i=0; i<model.GetTurretCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxString str = _("");
		if(model.Turret(i).sobj_parent > -1 && model.Turret(i).sobj_parent < model.GetSOBJCount())str = wxString(model.SOBJ(model.Turret(i).sobj_parent).name.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Turret %i"), i+1);
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.TGUN_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(TGUN, path));

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Turret(i).fire_points.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("Fire Point %i"),j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(TGUN, path, true));
		}
	}

	for(i=0; i<model.GetPathCount(); i++){
		path[0] = i;
		path[1] = -1;
		wxString str(model.Path(i).name.c_str(), wxConvUTF8);
		if(str == _(""))str = wxString().Format(_("Path %i"), i+1);
		wxTreeItemId parent = navigation_panel->AppendItem(tree_id.PATH_ID, str, TREE_UNSEL, TREE_SEL, new tree_node_id(PATH, path));

		navigation_panel->SetItemImage(parent, TREE_UNSEL_OP, wxTreeItemIcon_Expanded);
		navigation_panel->SetItemImage(parent, TREE_SEL_OP, wxTreeItemIcon_SelectedExpanded);

		for(unsigned int j = 0; j<model.Path(i).verts.size(); j++){
			path[1] = j;
			navigation_panel->AppendItem(parent, wxString().Format(_("Vertex %i"),j+1), TREE_UNSEL, TREE_SEL, new tree_node_id(PATH, path, true));
		}
	}

	rebuilding_tree = false;

	update_tree_selection(control_panel->chunk_type, control_panel->get_item());

	navigation_panel->Update();
}

bool main_panel::update_tree_selection_sub(CHUNK type, const std::vector<int>&path, wxTreeItemId id, wxTreeItemIdValue c){
	for(
	id = navigation_panel->GetFirstChild(id, c);
	id.IsOk();
	id = navigation_panel->GetNextSibling(id)){
		tree_node_id*ID = ((tree_node_id*)navigation_panel->GetItemData(id));
		if(ID && 
			//if the type is the same, or if the node type is weapon, 
			//but the one we are looking for is gun or missle
			(ID->type == type || (ID->type == WEAP && (type == GPNT || type == MPNT))) 
			&& ID->path == path){
			navigation_panel->SelectItem(id);
			return true;
		}else{
			if(update_tree_selection_sub(type, path, id, c))
				return true;
		}
	}
	return false;
}

void main_panel::update_tree_selection(CHUNK type, const std::vector<int>&path){

	if(navigation_panel->GetSelection().IsOk() && navigation_panel->GetItemData(navigation_panel->GetSelection())){
 		tree_node_id id = *((tree_node_id*)navigation_panel->GetItemData(navigation_panel->GetSelection()));
		//don't change anything if it's perfict
		if(id.type == type && id.path==path){
			return;
		}
	}

	wxTreeItemIdValue c = NULL;

	internal_selection = true;

	update_tree_selection_sub(type, path, tree_id.ROOT_ID, c);
	
	internal_selection = false;
}

void main_panel::on_tree_selection(wxTreeEvent& event){

	if(rebuilding_tree || internal_selection)return;

//	if(control_panel)control_panel->apply_data(model);
	tree_node_id*ID = ((tree_node_id*)navigation_panel->GetItemData(event.GetItem()));

	if(!ID)
		return;

	tree_node_id id = *ID;
	//we need to copy the data cause the next line will likely kill ID

	control_panel->apply_data(model);
	//make sure we update data before it gets lost

	int mod_n = -1;
	if(id.type == SOBJ && id.path[0] >-1){
		mod_n = id.path[0];
	}else{
		if(model.GetLODCount()>0 && model.LOD(0)){
			mod_n = model.LOD(0);
		}else{
			mod_n = 0;
		}
	}
	if(id.type != control_panel->chunk_type && control_sizer->Detach(control_panel) && control_panel->Destroy()){
		control_panel = NULL;
		switch(id.type){
			case HDR2:
					control_sizer->Add(control_panel = new model_editor_ctrl<header_data, header_ctrl>					(control_pane, model, HDR2),1.0,wxEXPAND);
				break;
			case SOBJ:
				control_sizer->Add(control_panel = new model_editor_ctrl<pcs_sobj, SOBJ_ctrl>							(control_pane, model, SOBJ, mod_n),1.0,wxEXPAND);
				break;
			case TXTR:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<std::string>, TXTR_ctrl>			(control_pane, model, TXTR),1.0,wxEXPAND);
				break;
			case WEAP:
			case GPNT:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_slot>, WPNT_ctrl<GUN> >		(control_pane, model, GPNT),1.0,wxEXPAND);
				break;
			case MPNT:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_slot>, WPNT_ctrl<MISSILE> >	(control_pane, model, MPNT),1.0,wxEXPAND);
				break;
			case DOCK:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_dock_point>, DOCK_ctrl>		(control_pane, model, DOCK),1.0,wxEXPAND);
				break;
			case SPCL:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_special>, SPCL_ctrl>		(control_pane, model, SPCL),1.0,wxEXPAND);
				break;
			case EYE:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_eye_pos>, EYE_ctrl>			(control_pane, model, EYE),1.0,wxEXPAND);
				break;
			case FUEL:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_thruster>, FUEL_ctrl>		(control_pane, model, FUEL),1.0,wxEXPAND);
				break;
			case TGUN:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_turret>, TGUN_ctrl>			(control_pane, model, TGUN),1.0,wxEXPAND);
				break;
			case GLOW:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_glow_array>, GLOW_ctrl>		(control_pane, model, GLOW),1.0,wxEXPAND);
				break;
			case PATH:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_path>, PATH_ctrl>			(control_pane, model, PATH),1.0,wxEXPAND);
				break;
			case ACEN:
					control_sizer->Add(control_panel = new model_editor_ctrl<vector3d, ACEN_ctrl>						(control_pane, model, ACEN),1.0,wxEXPAND);
				break;
			case PINF:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<std::string>, PINF_ctrl>						(control_pane, model, PINF),1.0,wxEXPAND);
				break;
			case SHLD:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_shield_triangle>, SHLD_ctrl>						(control_pane, model, SHLD),1.0,wxEXPAND);
				break;
			case INSG:
					control_sizer->Add(control_panel = new model_editor_ctrl<std::vector<pcs_insig>, INSG_ctrl>						(control_pane, model, INSG),1.0,wxEXPAND);
				break;
			default:
					control_sizer->Add(control_panel = new model_editor_ctrl<header_data, header_ctrl>					(control_pane, model, HDR2),1.0,wxEXPAND);
				break;
		}
	}
	control_panel->set_item(id.path);

	if(id.type == SOBJ){
		control_panel->set_data(model);
		model.set_active_model(mod_n);
	}
/*	else
	{
		model.set_active_model(mod_n);
	}*/
	//aparently people want selected object to remain selected

	//the model editor only handels one model at a time, so it needs to be updated

	control_sizer->Layout();
	
	control_panel->EnableScrolling(false,true);
	control_panel->FitInside();
	control_panel->SetScrollRate(0,10);

	glcanvas->set_active_chunk(id.type);

	wxCommandEvent commandEvent(DATA_SELECTION_CHANGED, GetId());
	GetEventHandler()->ProcessEvent(commandEvent);

	control_panel->fix_buttons();

	//put the inital state of the chunck into the undo buffer
	control_panel->push_undo();
}

void main_panel::tree_begin_drag(wxTreeEvent& event){
	tree_node_id*id = ((tree_node_id*)navigation_panel->GetItemData(event.GetItem()));

	if(id->dragable)event.Allow();
	dragged_id = id;
	if(event.GetEventType() == wxEVT_COMMAND_TREE_BEGIN_RDRAG)dragged_id->right = true;
	else dragged_id->right = false;
}

int find_weapon(std::vector<pcs_slot> wep, int a, int type){
	int l = -1;
	bool any = false;
	unsigned int i;
	for(i = 0; i<wep.size(); i++){
		if(wep[i].type == type){
			l++;
			any = true;
			if(l >= a)break;
		}
	}
	return any?i:-1;
}

void main_panel::tree_end_drag(wxTreeEvent& event){


	if(!event.GetItem().IsOk())
		return;

	tree_node_id*id = ((tree_node_id*)navigation_panel->GetItemData(event.GetItem()));
	//id of the node being dropped upon
	
	if(!id || !dragged_id){
		dragged_id = NULL;
		return;
	}
	//if either pointer is bad we can't do anything

	if(id->type != dragged_id->type){
		dragged_id = NULL;
		return;
	}
	//we can only drop items within there own chunk

	if(id->type != SOBJ && id->dragable){
		dragged_id = NULL;
		return;
	}
	//subobjects are the only dragable item that can be dropped upon
	if(dragged_id == id){
		dragged_id = NULL;
		return;
	}
	//you can't drop on your self

	int src;
	int dst;

	switch(id->type){

		case SOBJ:
			//reparent the item being droped to 
			//the item being droped upon
			model.SOBJ(dragged_id->path[0]).parent_sobj = id->path[0];
			break;


		case GPNT:
			//reparent the item being droped to 
			//the item being droped upon
			src = find_weapon(model.get_weapons(), dragged_id->path[0], GUN);
			dst = find_weapon(model.get_weapons(), id->path[0], GUN);
			if(src <0 ||dst<0)break;

			//copy the source to the dest, then erase the origonal
			model.Weapon(dst).muzzles.push_back(model.Weapon(src).muzzles[dragged_id->path[1]]);
			if(!dragged_id->right)model.Weapon(src).muzzles.erase(model.Weapon(src).muzzles.begin()+dragged_id->path[1]);
			break;

		case MPNT:
			//reparent the item being droped to 
			//the item being droped upon
			src = find_weapon(model.get_weapons(), dragged_id->path[0], MISSILE);
			dst = find_weapon(model.get_weapons(), id->path[0], MISSILE);
			if(src <0 ||dst<0)break;

			//copy the source to the dest, then erase the origonal
			model.Weapon(dst).muzzles.push_back(model.Weapon(src).muzzles[dragged_id->path[1]]);
			if(!dragged_id->right)model.Weapon(src).muzzles.erase(model.Weapon(src).muzzles.begin()+dragged_id->path[1]);
			
			break;

		case DOCK:
			//copy the source to the dest, then erase the origonal
			model.Dock(id->path[0]).dockpoints.push_back(model.Dock(dragged_id->path[0]).dockpoints[dragged_id->path[1]]);
			if(!dragged_id->right)model.Dock(dragged_id->path[0]).dockpoints.erase(model.Dock(dragged_id->path[0]).dockpoints.begin()+dragged_id->path[1]);
			break;

		case FUEL:
			//copy the source to the dest, then erase the origonal
			model.Thruster(id->path[0]).points.push_back(model.Thruster(dragged_id->path[0]).points[dragged_id->path[1]]);
			if(!dragged_id->right)model.Thruster(dragged_id->path[0]).points.erase(model.Thruster(dragged_id->path[0]).points.begin()+dragged_id->path[1]);
			break;

		case TGUN:
			//copy the source to the dest, then erase the origonal
			model.Turret(id->path[0]).fire_points.push_back(model.Turret(dragged_id->path[0]).fire_points[dragged_id->path[1]]);
			if(!dragged_id->right)model.Turret(dragged_id->path[0]).fire_points.erase(model.Turret(dragged_id->path[0]).fire_points.begin()+dragged_id->path[1]);
			break;

		case GLOW:
			//copy the source to the dest, then erase the origonal
			model.Light(id->path[0]).lights.push_back(model.Light(dragged_id->path[0]).lights[dragged_id->path[1]]);
			if(!dragged_id->right)model.Light(dragged_id->path[0]).lights.erase(model.Light(dragged_id->path[0]).lights.begin()+dragged_id->path[1]);
			break;

		case PATH:
			//copy the source to the dest, then erase the origonal
			model.Path(id->path[0]).verts.push_back(model.Path(dragged_id->path[0]).verts[dragged_id->path[1]]);
			if(!dragged_id->right)model.Path(dragged_id->path[0]).verts.erase(model.Path(dragged_id->path[0]).verts.begin()+dragged_id->path[1]);
			break;

		case HDR2:
		case TXTR:
		case SPCL:
		case EYE:
		case ACEN:
		case PINF:
		case SHLD:
		default:
			break;
	}

	control_panel->set_data(model);
	//update the editor with any changes we just made
	//so it doesn't override us

	dragged_id = NULL;

	rebuild_tree();
	glcanvas->Render();
}


void main_panel::on_omnipoint_ray_picked(wxCommandEvent &event){
	int list, item;
	glcanvas->get_selected_omni_points(list, item);
	control_panel->set_omnipoint_coords(list, item);
	update_tree_selection(control_panel->chunk_type, control_panel->get_item());
}

void main_panel::startRender() {
	glcanvas->Init();
}

main_panel::transform_dialog::transform_dialog(wxWindow* parent, model_editor_ctrl_base* control)
	: wxDialog(parent, -1, wxString(_("Enter Transform")))
{
	wxSizer* top_sizer;
	wxSizer* buttons_sizer;
	tabs = new wxNotebook(this, wxID_ANY);
	wxSizerFlags flags;
	flags.Centre();
	top_sizer = new wxBoxSizer(wxVERTICAL);
	buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* options = control->get_transform_options(this);

	pages.push_back(new rotate_transform(tabs));
	pages.push_back(new scale_transform(tabs));
	pages.push_back(new translate_transform(tabs));
	pages.push_back(new matrix_transform(tabs));
	for (size_t i = 0; i < pages.size(); i++) {
		tabs->AddPage(pages[i], pages[i]->name());
	}

	buttons_sizer->Add(new wxButton(this, wxID_OK, _("OK")));
	buttons_sizer->Add(new wxButton(this, wxID_CANCEL, _("Cancel")));
	top_sizer->Add(tabs);
	if (options) {
		top_sizer->Add(options);
	}
	top_sizer->Add(buttons_sizer, flags);
	SetSizerAndFit(top_sizer);
}

matrix main_panel::transform_dialog::get_transform() const {
	int tab = tabs->GetSelection();
	if (tab != -1) {
		return pages[tab]->get_transform();
	} else {
		return matrix();
	}
}

vector3d main_panel::transform_dialog::get_translation() const {
	int tab = tabs->GetSelection();
	if (tab != -1) {
		return pages[tab]->get_translation();
	} else {
		return vector3d();
	}
}

matrix main_panel::transform_dialog::matrix_transform::get_transform() const {
	std::vector<float> values(16, 0.0f);
	for (int i = 0; i < 16; i++) {
		values[i] = (float)atof(matrix_entry[i]->GetValue().mb_str());
	}
	return matrix(&values);
}

vector3d main_panel::transform_dialog::matrix_transform::get_translation() const {
	vector3d result;
	for (int i = 0; i < 3; i++) {
		result[i] = (float)atof(matrix_entry[(i * 4) + 3]->GetValue().mb_str());
	}
	return result;
}

matrix main_panel::transform_dialog::scale_transform::get_transform() const {
	matrix result;
	if (scale->get_value() == 0) {
		return result;
	}
	vector3d axis_vector = axis->get_value();
	if (null_vec(axis_vector)) {
		for (int i = 0; i < 3; i++) {
			result.a2d[i][i] = scale->get_value();
		}
	} else {
		matrix axis_transform(axis_vector);
		result.a2d[0][0] = scale->get_value();
		result = axis_transform.invert() % result % axis_transform;
	}
	return result;
}

vector3d main_panel::transform_dialog::scale_transform::get_translation() const {
	return vector3d();
}

matrix main_panel::transform_dialog::translate_transform::get_transform() const {
	return matrix();
}

vector3d main_panel::transform_dialog::translate_transform::get_translation() const {
	return translation->get_value();
}

matrix main_panel::transform_dialog::rotate_transform::get_transform() const {
	vector3d direction = axis->get_value();
	// Give up if we don't get a valid looking axis.
	if (Magnitude(direction) < 1e-5) {
		return matrix();
	}
	matrix transform(direction);
	matrix rotation(angle->get_value());
	return transform.invert() % rotation % transform;
}

vector3d main_panel::transform_dialog::rotate_transform::get_translation() const {
	return vector3d();
}
