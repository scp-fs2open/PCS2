#include "pcs2_CIDs.h"
#include"combo.h"



BEGIN_EVENT_TABLE(combo, wxComboBox)
	EVT_SET_FOCUS(do_focus)
	EVT_COMBOBOX(wxID_ANY,combo::do_selected)
END_EVENT_TABLE()

void combo::do_focus(wxFocusEvent&ev){
	wxCommandEvent event(COMBO_CLICKED, GetId());

	ProcessCommand(event);
	ev.Skip(true);
}

extern const wxEventType EDIT_DONE;

void combo::do_selected(wxCommandEvent&ev){
	ev.Skip();
	wxCommandEvent event(EDIT_DONE, GetId());
	GetEventHandler()->ProcessEvent(event);
}