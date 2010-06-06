#ifndef _OP_STACK_H_
#define _OP_STACK_H_

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

#endif /* _OP_STACK_H_ */
