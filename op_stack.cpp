#include "pcs2.h"
#include "op_stack.h"

template<class type>
void op_stack<type>::push(const type&new_state){
	if(operation.size()>0){
		//if we already have this, there is no need to save it again
		if(top > -1 && operation[top] == new_state)
			return;
	}
	top++;
	operation.resize(top+1);
	(operation[top]) = new_state;

	if(get_main_window()){
	//this function always kills redo
		get_main_window()->on_redo_enable(false);
	//this function always enables undo
		if(top>0)get_main_window()->on_undo_enable();
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

template<class type>
bool op_stack<type>::undo(type&state){
	top--;
	//pushed past the begining of the stack
	if(top <= 0){
		//if we are at the bottom of the stack there is nothing to undo so disable it
		if(get_main_window())get_main_window()->on_undo_enable(false);
		if(top<0)return false;
	}
	//this function always enables redo
			if(get_main_window())get_main_window()->on_redo_enable();

	state = (operation[top]);
	return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

template<class type>
bool op_stack<type>::redo(type&state){
	if(top < (int)operation.size()-1){
		top++;
		if(top == (int)operation.size()-1){
			//if we are at the top of the stack there is nothing to redo so disable it
			if(get_main_window())get_main_window()->on_redo_enable(false);
		}
		//this function always enables undo
		if(get_main_window())get_main_window()->on_undo_enable();
		state = (operation[top]);
		return true;
	}
	return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

template<class type>
void op_stack<type>::fix_buttons(){
	if(top > 0){
		if(get_main_window())
			get_main_window()->on_undo_enable();
	}else{
		if(get_main_window())
			get_main_window()->on_undo_enable(false);
	}

	if(top > -1 && top < (int)operation.size()){
		if(get_main_window())
			get_main_window()->on_redo_enable();
	}else{
		if(get_main_window())
			get_main_window()->on_redo_enable(false);
	}

}

// Force the compiler to instantiate the template:
template class op_stack<class std::vector<struct pcs_shield_triangle> >;
template class op_stack<class std::vector<std::string> >;
template class op_stack<struct pcs_sobj>;
template class op_stack<class std::vector<struct pcs_path> >;
template class op_stack<class std::vector<struct pcs_turret> >;
template class op_stack<class std::vector<struct pcs_glow_array> >;
template class op_stack<class std::vector<struct pcs_thruster> >;
template class op_stack<class std::vector<struct pcs_eye_pos> >;
template class op_stack<class std::vector<struct pcs_special> >;
template class op_stack<class std::vector<struct pcs_dock_point> >;
template class op_stack<class std::vector<struct pcs_slot> >;
template class op_stack<struct vector3d>;
template class op_stack<struct header_data>;
