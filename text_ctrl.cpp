#include "pcs2_CIDs.h"
#include"text_ctrl.h"

BEGIN_EVENT_TABLE(text_ctrl, wxTextCtrl)
	EVT_KILL_FOCUS(do_focus)
END_EVENT_TABLE()

extern const wxEventType EDIT_DONE;
void text_ctrl::do_focus(wxFocusEvent&ev){
	wxCommandEvent event(EDIT_DONE, GetId());
	GetEventHandler()->ProcessEvent(event);
}
