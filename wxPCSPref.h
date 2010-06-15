//****************************************************************************
//	wxPCSPref.h
//	Preferences Dialog for PCS2
//	
//****************************************************************************

/*
 * $Logfile: /wxPCSPref.h $
 * $Revision: 1.8 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: wxPCSPref.h,v $
 * Revision 1.8  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.7  2008/05/15 19:19:25  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.6  2007/07/13 11:31:06  bobboau
 * added auto-extraction and external loading of textures in VPs,  and a user defined temp directory.
 *
 * Revision 1.5  2007/06/25 01:42:41  bobboau
 * added a options dialog for selecting editor colors.
 *
 * Revision 1.4  2007/06/16 19:40:12  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.3  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.2  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
 *
 * Revision 1.1  2005/08/28 00:35:06  kazan
 * new files for the config dialog and toolbar
 *
 *
 *
 */

#if !defined(_wxPCSPref_h_)
#define _wxPCSPref_h_

#include <wx/setup.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/textctrl.h>

class PCS_Preferences : public wxDialog
{
	protected:
		DECLARE_EVENT_TABLE()

	private:
		wxListBox *texture_paths;
		wxStaticText *tpath_txt, *cob_scale_text, *dae_flip_axes;
		wxButton *tadd, *tdele, *tbrowse, *col_ops, *ok, *cancel;
		wxTextCtrl *tpath, *cob_scale;
		wxCheckBox *geo_filter, *vbos;

		// Collada options
		wxCheckBox *helpers, *props_as_helpers, *mirror_x, *mirror_y, *mirror_z;

		wxButton *def_col, *amb_col;

		int curEdit;
	public:
		PCS_Preferences(wxWindow *parent);

		void onAdd(wxCommandEvent &event);
		void onEdit(wxCommandEvent &event);
		void onDelete(wxCommandEvent &event);
		void onSelection(wxCommandEvent &event);
		void onBrowse(wxCommandEvent &event);

		virtual void OnOK(wxCommandEvent &event);

		virtual void on_color(wxCommandEvent &event);

		virtual void on_color_op(wxCommandEvent &event);
};	



#endif //_wxPCSPref_h_

