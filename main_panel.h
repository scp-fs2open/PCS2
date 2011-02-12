//****************************************************************************
//	main_panel.h
//	POF Constructor Suite
//****************************************************************************

/*
 * $Logfile: /main_panel.h $
 * $Revision: 1.25 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: main_panel.h,v $
 * Revision 1.25  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.24  2008/05/15 17:43:58  kazan
 * *** empty log message ***
 *
 * Revision 1.23  2008/02/09 22:33:00  kazan
 * bye bye kaz_vector
 *
 * Revision 1.22  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.21  2008/02/09 17:54:27  kazan
 * geometry can now be imported into an existing model
 *
 * Revision 1.20  2008/01/10 18:28:10  bobboau
 * *** empty log message ***
 *
 * Revision 1.19  2007/07/24 22:50:23  bobboau
 * added undo/redo functionality
 *
 * Revision 1.18  2007/07/13 11:31:04  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.17  2007/07/11 19:14:47  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.16  2007/07/11 14:09:34  kazan
 * fix solid-view rendering bug, renable app modal progress dialogs during load
 *
 * Revision 1.15  2007/06/29 20:15:43  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.14  2007/06/28 19:03:10  kazan
 * add log headers
 *
 *
 */

#if !defined(_MAIN_PANEL_H_)
#define _MAIN_PANEL_H_

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include "pcs2.h"
#include "pcs2_CIDs.h"
#include "model_editor_ctrl.h"
#include <wx/progdlg.h>
#include <wx/thread.h>
#include <wx/notebook.h>

#include "matrix3d.h"



class wxGL_PMFCanvas;


struct tree_node_id 
	: public wxTreeItemData
{
	CHUNK type;
	std::vector<int> path; 
	//the meaning of this differs per chunk
	//but most chuncks have a tree like structure
	//this is the nodes and child nodes you need to
	//take to get to this item
	tree_node_id(CHUNK Type, std::vector<int> Path, bool d = false, bool r = false)
		:type(Type), path(Path), dragable(d), right(r)
	{}
	bool dragable;
	bool right;
};

struct tree_ids{
	wxTreeItemId ROOT_ID;
	wxTreeItemId HDR2_ID;
	wxTreeItemId SOBJ_ID;
	wxTreeItemId TXTR_ID;
	wxTreeItemId WEAP_ID;
	wxTreeItemId GPNT_ID;
	wxTreeItemId MPNT_ID;
	wxTreeItemId DOCK_ID;
	wxTreeItemId FUEL_ID;
	wxTreeItemId GLOW_ID;
	wxTreeItemId SPCL_ID;
	wxTreeItemId TGUN_ID;
	wxTreeItemId PATH_ID;
	wxTreeItemId EYE_ID;
	wxTreeItemId INSG_ID;
	wxTreeItemId SHLD_ID;
	wxTreeItemId ACEN_ID;
	wxTreeItemId PINF_ID;
};

class main_panel
	: public wxPanel
{
		/*
		split2 will be the right child of split1
		the left child (A) will be a panel for
		model navigation (the tree), the left 
		child of split2 (B) will be the GLcanvas
		and the right child (C) will be the control panel

		+---+-----------+<-split1
		|	|-------+---+
		|	|		| 	|
		| A	|		|	|
		|	|	B	|	|
		|	|		| C	|
		|	|		| 	|
		|	|-------|---+<-split2
		+---+-----------+
		*/

	wxSplitterWindow*split1;
	wxSplitterWindow*split2;

	wxTreeCtrl*navigation_panel;	//(A)
	tree_ids tree_id;
	tree_node_id*dragged_id;

	wxGL_PMFCanvas *glcanvas;	//(B)

	model_editor_ctrl_base*control_panel;		//(C)
	wxBoxSizer*control_sizer;
	wxPanel*control_pane;//holds the control panel and the buttons
	wxButton*load_chunk_btn;	//button that appliys changes to the model
	wxButton*save_chunk_btn; //button that discards changes and set's the control to what is in the model
	wxButton*transform_chunk_btn;

	// editor data
	PCS_Model model;

	//external controls, generaly owned by the main frame
		wxStatusBar *pstatus; // our parents status bar, we manage the size of it
							  // along with adding a child control
		wxWindow *myParent;
		wxGauge *pgauge; //status bar progress gauge
		wxTimer *drwTimer;
		bool isDestroyed; // needed to avoid a crash condition -- kaz

		void ImportGeometry(std::string filename);
		void SignalModelChange(std::string filename, bool skipProgdlg=false); 
		bool LoadImportModel(std::string filename, PCS_Model*import_model);

		void global_import(std::string filename); 
		void import_turrets(PCS_Model& import_model); 

		//when a new model is loaded this will 
		//erase the exsisting tree and make a new one
		void rebuild_tree();
		void parents_first(int i, std::vector<wxTreeItemId>&model_nodes);
		bool rebuilding_tree;
		bool internal_selection;//used to shortcurcit the on select event 
		//when I'm setting a selection to what the editor want's it to be

		wxProgressDialog *threaded_prog_bar;
		int threaded_prog_bar_readers;
		bool should_delete_threaded_prog_bar;

		bool UseThreadedProgBar;

	
public:
	void undo();
	void redo();

	virtual bool Destroy();

	void startRender();

	void SetCurOp(std::string op) { if(pstatus)pstatus->SetStatusText(wxString(op.c_str(), wxConvUTF8), 0); }
	void SetCurProg(int perc) { if(pgauge)pgauge->SetValue(perc); } 

	main_panel(wxFrame* parent);
	//virtual ~main_panel(void){};

	void Revert_data(wxCommandEvent &event);
	void Apply_data(wxCommandEvent &event);

	virtual void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

	void on_tree_selection(wxTreeEvent& event);
	void on_rebuild_tree(wxCommandEvent& event){rebuild_tree();};

	void update_tree_selection(CHUNK type, const std::vector<int>&path);
	bool update_tree_selection_sub(CHUNK type, const std::vector<int>&path, wxTreeItemId id, wxTreeItemIdValue c);

 	void on_update_data(wxCommandEvent& event);
	void on_update_subobject(wxCommandEvent& event);
	void on_update_tree(wxCommandEvent& event);

	void open_texture_externaly(wxString filename);
	void on_data_selection_change(wxCommandEvent& event);

	void tree_begin_drag(wxTreeEvent& event);
	void tree_end_drag(wxTreeEvent& event);

	void on_load_chunk(wxCommandEvent &event);
	void on_save_chunk(wxCommandEvent &event);
	void on_transform_chunk(wxCommandEvent &event);

	void on_cpy_sobj(wxCommandEvent &event);
	void on_del_sobj(wxCommandEvent &event);


	void on_BSP_render_change(wxCommandEvent &event);

	void on_omnipoint_ray_picked(wxCommandEvent &event);

	void on_texture_reload(wxCommandEvent& event);
	void on_texture_external_open(wxCommandEvent& event);

	void on_moi_recalc(wxCommandEvent& event);

	// progress bar events
	void open_progbar_start(wxAsyncProgressStartEvt &event);
	void open_progbar_update(wxAsyncProgressUpdateEvt &event);
	void open_progbar_end(wxAsyncProgressEndEvt &event);

	void texture_progbar_start(wxAsyncProgressStartEvt &event) { open_progbar_start(event); }
	void texture_progbar_update(wxAsyncProgressUpdateEvt &event) { open_progbar_update(event); }
	void texture_progbar_end(wxAsyncProgressEndEvt &event);
	
	void import_progbar_start(wxAsyncProgressStartEvt &event) { open_progbar_start(event); }
	void import_progbar_update(wxAsyncProgressUpdateEvt &event) { open_progbar_update(event); }
	void import_progbar_end(wxAsyncProgressEndEvt &event) { texture_progbar_end(event); }


	friend class PCS2_MainWindow;//ick! :O
	friend class wxGL_PMFCanvas;//why the hell not... meh...

	// so texture control can manipulate the dialog
	
	wxProgressDialog *gprog;

	
	DECLARE_EVENT_TABLE();

	class transform_dialog : public wxDialog
	{
		class transform_method : public wxPanel {
			public:
				transform_method(wxWindow* parent, int id=wxID_ANY) : wxPanel(parent, id) {}
				virtual matrix get_transform() const=0;
				virtual vector3d get_translation() const=0;
				virtual wxString name() const=0;
		};

		class matrix_transform : public transform_method {
			private:
				wxTextCtrl* matrix_entry[16];

			public:
				matrix_transform(wxWindow* parent, int id=wxID_ANY) : transform_method(parent, id) {
					wxGridSizer* sizer = new wxGridSizer(4);
					for (int i = 0; i < 16; i++) {
						matrix_entry[i] = new wxTextCtrl(this, -1, ((i % 4) == (i / 4)) ? _("1") : _("0"), wxDefaultPosition, wxSize(60, 20));
						if (i >= 12) {
							matrix_entry[i]->SetEditable(false);
						}
						sizer->Add(matrix_entry[i]);
					}
					SetSizerAndFit(sizer);
				}

				virtual matrix get_transform() const;
				virtual vector3d get_translation() const;
				virtual wxString name() const {
					return _("Matrix");
				}

		};

		class scale_transform : public transform_method {
			private:
				vector3d_radio_button_ctrl* axis;
				float_ctrl* scale;

			public:
				scale_transform(wxWindow* parent, int id=wxID_ANY) : transform_method(parent, id) {
					std::vector<primitive_list_item<vector3d> > values;
					values.resize(5);
					values[0].title = "All axes";
					values[0].data = vector3d();
					values[1].title = "X axis";
					values[1].data = vector3d(1, 0, 0);
					values[2].title = "Y axis";
					values[2].data = vector3d(0, 1, 0);
					values[3].title = "Z axis";
					values[3].data = vector3d(0, 0, 1);
					values[4].title = "Custom:";
					values[4].data = vector3d();
					wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
					sizer->Add(axis=new vector3d_radio_button_ctrl(this, values, 0, 0, 60, -1, _("Axis")), 0, wxEXPAND);
					sizer->Add(scale=new float_ctrl(this,0,0,60,40,_("Scale (negative values flip)")), 0, wxEXPAND);
					scale->set_value(1.0f);
					SetSizerAndFit(sizer);
				}

				virtual matrix get_transform() const;
				virtual vector3d get_translation() const;
				virtual wxString name() const {
					return _("Scale");
				}

		};

		class translate_transform : public transform_method {
			private:
				vector_ctrl* translation;

			public:
				translate_transform(wxWindow* parent, int id=wxID_ANY) : transform_method(parent, id) {
					wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
					sizer->Add(translation=new vector_ctrl(this,0,0,60,40,_("Translation")), 0, wxEXPAND);
					SetSizerAndFit(sizer);
				}

				virtual matrix get_transform() const;
				virtual vector3d get_translation() const;
				virtual wxString name() const {
					return _("Translate");
				}
		};

		class rotate_transform : public transform_method {
			private:
				vector3d_radio_button_ctrl* axis;
				float_ctrl* angle;

			public:
				rotate_transform(wxWindow* parent, int id=wxID_ANY) : transform_method(parent, id) {
					std::vector<primitive_list_item<vector3d> > values;
					values.resize(4);
					values[0].title = "X axis";
					values[0].data = vector3d(1, 0, 0);
					values[1].title = "Y axis";
					values[1].data = vector3d(0, 1, 0);
					values[2].title = "Z axis";
					values[2].data = vector3d(0, 0, 1);
					values[3].title = "Custom:";
					values[3].data = vector3d();
					wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
					sizer->Add(axis=new vector3d_radio_button_ctrl(this, values, 0, 0, 60, -1, _("Axis")), 0, wxEXPAND);
					sizer->Add(angle=new float_ctrl(this,0,0,60,40,_("Angle (degrees)")), 0, wxEXPAND);
					angle->set_value(0.0f);
					SetSizerAndFit(sizer);
				}
				virtual matrix get_transform() const;
				virtual vector3d get_translation() const;
				virtual wxString name() const {
					return _("Rotate");
				}
		};

		std::vector<transform_method*> pages;
		wxNotebook* tabs;

		public:
		transform_dialog(wxWindow* parent, model_editor_ctrl_base* control);
		matrix get_transform() const;
		vector3d get_translation() const;
		virtual ~transform_dialog(void){};
	};
};

#endif //_MAIN_PANEL_H_
