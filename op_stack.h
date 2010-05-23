#include "pcs2.h"
#include <string>
#include <wx/wx.h>
//opration stack, used for undo/redo


template<class type>
class op_stack{
	std::vector<type> operation;
	int top;
public:

	op_stack():top(-1){};

	//a new state has been made, this will save it to the top of the stack
	void push(const type&new_state);

	//we want to undo the last state, so remove the top and set state to what 
	//was under it the cutrent top is not erased untill a call to push is made, 
	//so any undone action can be redone
	//return's true unless the stack was empty
	bool undo(type&state);

	//undoes an undo :)
	//put's the last top back on and set's state to it, 
	//return's true if there was anything to redo
	bool redo(type&state);

	//utility function that set's the enabled/disabled 
	//status of the undo/redo buttons/menu items
	void fix_buttons();

	void reset(){
		operation.resize(0);
		top = -1;
		fix_buttons();
	}
};

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

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
