//cpp for defineing things that were declaired in any of the headers

#include "chunk_editors.h"

DEFINE_EVENT_TYPE(ARRAY_NEW)
DEFINE_EVENT_TYPE(ARRAY_CPY)
DEFINE_EVENT_TYPE(ARRAY_DEL)
DEFINE_EVENT_TYPE(ARRAY_INCREMENT)
DEFINE_EVENT_TYPE(ARRAY_DECREMENT)
DEFINE_EVENT_TYPE(EDIT_DONE)
DEFINE_EVENT_TYPE(DATA_SELECTION_CHANGED)
DEFINE_EVENT_TYPE(BSP_RENDER_CHANGE)
DEFINE_EVENT_TYPE(COMBO_CLICKED)
DEFINE_EVENT_TYPE(SUBOBJECT_UPDATE)
DEFINE_EVENT_TYPE(UNDO_ENABLE)
DEFINE_EVENT_TYPE(REDO_ENABLE)

BEGIN_EVENT_TABLE (path_array_ctrl,resizeable_array_ctrl<pcs_path>)
	EVT_COMMAND(wxID_ANY, ARRAY_NEW, path_array_ctrl::on_new)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE (PATH_ctrl,editor_ctrl<std::vector<pcs_path> >)
	EVT_BUTTON(PATH_AUTO_GEN,PATH_ctrl::on_auto_gen)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE (weapon_hard_point_array_ctrl,hard_point_array_ctrl)
	EVT_BUTTON(WPN_CONVERGENCE,weapon_hard_point_array_ctrl::on_convergence)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE (convergence_calculator,wxDialog)
	EVT_BUTTON(CONVERGE_OK,convergence_calculator::on_ok)
	EVT_BUTTON(CONVERGE_CANCEL,convergence_calculator::on_cancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE (SOBJ_ctrl,editor_ctrl<pcs_sobj>)
	EVT_CHECKBOX(BSP_DRAW,SOBJ_ctrl::on_BSP_render_change)
END_EVENT_TABLE()

//XXX: hack
#ifndef UNIX
BEGIN_EVENT_TABLE (normal_ctrl,vector_ctrl)
	EVT_BUTTON(NORM_NORMALIZE,normal_ctrl::on_normalize)
END_EVENT_TABLE()
#endif


template<> color WPNT_ctrl<GUN>::selected_item = color(255,192,192);
template<> color WPNT_ctrl<GUN>::selected_list = color(128,64,64);
template<> color WPNT_ctrl<GUN>::unselected = color(64,64,64);

template<> color WPNT_ctrl<MISSILE>::selected_item = color(255,192,192);
template<> color WPNT_ctrl<MISSILE>::selected_list = color(128,64,64);
template<> color WPNT_ctrl<MISSILE>::unselected = color(64,64,64);

color TGUN_ctrl::selected_item = color(255,128,255);
color TGUN_ctrl::selected_list = color(128,92,128);
color TGUN_ctrl::unselected = color(64,64,64);

color SPCL_ctrl::selected_item = color(255,255,128);
color SPCL_ctrl::selected_list = color(92,92,64);
color SPCL_ctrl::unselected = color(64,64,32);

color PATH_ctrl::selected_item = color(64,32,32);
color PATH_ctrl::selected_list = color(16,0,0);
color PATH_ctrl::unselected = color(1,0,0);

color GLOW_ctrl::selected_item = color(192,192,255);
color GLOW_ctrl::selected_list = color(64,64,192);
color GLOW_ctrl::unselected = color(64,64,92);

color FUEL_ctrl::selected_item = color(192,192,255);
color FUEL_ctrl::selected_list = color(64,64,192);
color FUEL_ctrl::unselected = color(64,64,92);

color EYE_ctrl::selected_item = color(225,225,255);
color EYE_ctrl::selected_list = color(64,92,64);
color EYE_ctrl::unselected = color(92,64,64);

color DOCK_ctrl::selected_item = color(128,255,128);
color DOCK_ctrl::selected_list = color(128,128,32);
color DOCK_ctrl::unselected = color(48,48,32);

color ACEN_ctrl::selected_item = color(255,128,128);
color ACEN_ctrl::selected_list = color(255,128,128);
color ACEN_ctrl::unselected = color(255,128,128);

color SOBJ_ctrl::selected_item = color(255,255,255);
color TXTR_ctrl::selected_item = color(255,200,200);
color SHLD_ctrl::selected_item = color(64,64,0);

color get_SOBJ_color(){
	return SOBJ_ctrl::selected_item;
}
color get_TXTR_color(){
	return TXTR_ctrl::selected_item;
}
color get_SHLD_color(){
	return SHLD_ctrl::selected_item;
}
