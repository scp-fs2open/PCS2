#pragma once
#include "pcs2_CIDs.h"
#include "editor.h"
#include <string>

#include"new_btn.xpm"
#include"copy_btn.xpm"
#include"delete_btn.xpm"




inline wxString itoa(int i){return wxString().Format("%d",i);}

//a static sized array, note it only hides resizeing functionality
//you can still resize programaticly by setting a diferent sized array
template<class type>
class array_ctrl :
	public editor<std::vector<type>>
{
protected:

	//a sizer for the control box
	wxStaticBoxSizer*box;

	//combo box that lets you select an item in the array
	combo*array_list;

	//displays the number of item's in the array
	wxTextCtrl*array_size_box;

	//the actual array
	std::vector<type>array;
	//the item in the array that is selected (-1 if nothing is selected)
	int index;

	//get a name for this 
	std::string (*get_index_name)(int idx);

	//set's the current array
	void set_array(const std::vector<type>&n_array){
		array=n_array;//yeah

		if(array.size()>0){
			if(index<0)index=0;
			if(index>=(int)array.size())index=(int)array.size()-1;
			//if there is anything in the array
			
			set_curent_value(array[index]);
			//make sure we change the current item before we accedently set our array to it

			rebuild_array_list();
			//rebuild everything 
			
			//set_index(0);
			//pick the one item we know will allways be in an array with anything in it
			
			enable();
			//allow the control to be used
		}else{
			//if there isn't anything in the array don't edit anything 
			disable();
		}
	}

	virtual void enable(){Enable();};
	virtual void disable(){Disable();};


	//event handeling!
	DECLARE_EVENT_TABLE();

	//when the item in the item in the combo box changes
	void on_selection_change(wxCommandEvent& event){
		set_index(array_list->GetCurrentSelection());
	//	GetEventHandler()->ProcessEvent(wxCommandEvent(DATA_SELECTION_CHANGED, GetId()));
		event.Skip();
		rebuild_array_list();
	}

	void on_select(wxCommandEvent& event){
		rebuild_array_list();
		event.Skip();
	}

	array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL, int ID = -1,bool a=false)//bool to diferentiate
		:editor(parent, x, y, w, h, orient, Title, ID),index(-1), get_index_name(NULL)
	{
	}

	//when something has happened to the array this is a convenient way to handel it
	//though not the most effecent solution I doubt handleing this diferently
	//would be worth the effort
	void rebuild_array_list(){
		//clear off the exsisting selections
		array_list->Clear();
		//repopulate
		for(unsigned int i = 0; i<array.size();i++){
			if(get_index_name)
				array_list->Append((*get_index_name)(i).c_str());
			else
				array_list->Append(wxString().Format("%d",i+1));
		}
		//update the size box
		array_size_box->ChangeValue(wxString().Format("%d",array.size()));
		//make sure the cuurent item is selected in the combo box
		array_list->Select(index);
	}
public:
	void set_index_name_function(std::string(*Get_index_name)(int idx)){
		get_index_name = Get_index_name;
	}


	//set's the control's current index to the one passed
	void set_index(int idx){

		if(idx>(int)array.size())
			idx=-1;//if we are out of bounds set to nothing

		//if we were told to set it to nothing
		if(idx<0){
			//disable
			disable();
			index=idx;
		}else{
			//otherwise enable
			enable();
			//copy what is in the control into the array
			if(index>-1 && index<(int)array.size())
				array[index]=get_curent_value();
			index=idx;
			//and make sure the controls have the right value
			set_curent_value(array[index]);
		}
		//set the combo box to the proper item
		array_list->Select(idx);
		
		GetEventHandler()->ProcessEvent(wxCommandEvent(DATA_SELECTION_CHANGED, GetId()));

	};

	//accessor
	int get_index(){return index;}

	size_t get_size(){return array.size();}

	array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL, int ID = -1)
		:editor(parent, x, y, w, h, orient, Title, ID),index(-1),control_orient(orient)
	{
		//the sizer for the array control, not the data of the array, that is handeled by derived classes
		box = new wxStaticBoxSizer(wxHORIZONTAL, this,"Select");
		//the combo box
		box->Add(array_size_box = new wxTextCtrl(this,-1,"0",wxPoint(0,0),wxSize(16,16),wxTE_READONLY|wxTE_CENTRE));
		box->Add(array_list = new wxComboBox(this,ARRAY_SELECT,"",wxPoint(10,10),wxSize(64,30)),1.0);
	}
	virtual std::vector<type> get_value(){if((int)array.size() > index && index>-1)array[index] = get_curent_value(); return array;}
	virtual void set_value(const std::vector<type>&i){set_array(i);}

	//get's/sets the currently selected item
	virtual type get_curent_value()=0;
	virtual void set_curent_value(const type&i)=0;

	void increment_selection(wxCommandEvent& event){
		if(array.size()<1 || index < 0)return;

		if(index + 1 >= (int)array.size())
			set_index(0);
		else
			set_index(index+1);
	}

	void decrement_selection(wxCommandEvent& event){
		if(array.size()<1 || index < 0)return;

		if(index - 1 < 0)
			set_index(array.size()-1);
		else
			set_index(index-1);
	}
};

BEGIN_EVENT_TABLE_TEMPLATE1 (array_ctrl,wxPanel,type)
//	EVT_COMBOBOX(ARRAY_SELECT,array_ctrl::on_selection_change)
	EVT_COMMAND(ARRAY_SELECT, COMBO_CLICKED, array_ctrl::on_select)
	EVT_COMMAND(ARRAY_SELECT, EDIT_DONE, array_ctrl::on_selection_change)
	EVT_COMMAND(wxID_ANY, ARRAY_INCREMENT, array_ctrl::increment_selection)
	EVT_COMMAND(wxID_ANY, ARRAY_DECREMENT, array_ctrl::decrement_selection)
END_EVENT_TABLE()

template<class type>
class resizeable_array_ctrl :
	public array_ctrl<type>
{

	//buttons for manageing the array
	wxBitmapButton*new_btn;
	wxBitmapButton*del_btn;
	wxBitmapButton*cpy_btn;

protected:
	int control_orient;
public:
	//constructor, DOES _NOT_ take an array, they must be set after the 
	//control is is constructed or you get virtual function problems
	resizeable_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL, int ID = -1)
		:array_ctrl(parent, x, y, w, h, Title, orient,ID,true)
	{
		//the sizer for the array control, not the data of the array, that is handeled by derived classes
		box = new wxStaticBoxSizer(wxVERTICAL, this,"Select");
		//the combo box
		array_list = new combo(this,ARRAY_SELECT,"",wxPoint(10,10),wxSize(64,30));
		//sizer that holds the size display and the new/copy/delete buttons
		wxBoxSizer*b=new wxBoxSizer(wxHORIZONTAL);
		array_size_box = new wxTextCtrl(this,-1,"0",wxPoint(0,0),wxSize(16,16),wxTE_READONLY|wxTE_CENTRE);
		new_btn = new wxBitmapButton(this, ARRAY_BUTTON_NEW, wxBitmap(_new_btn),wxPoint(26,0),wxSize(16,16));
		cpy_btn = new wxBitmapButton(this, ARRAY_BUTTON_COPY, wxBitmap(copy_btn),wxPoint(42,0),wxSize(16,16));
		del_btn = new wxBitmapButton(this, ARRAY_BUTTON_DELETE, wxBitmap(delete_btn),wxPoint(58,0),wxSize(16,16));
		new_btn->SetToolTip("New");
		cpy_btn->SetToolTip("Copy");
		del_btn->SetToolTip("Delete");
		//add the buttons'n stuff to the proper sizer
		b->Add(array_size_box);
		b->Add(new_btn);
		b->Add(cpy_btn);
		b->Add(del_btn);
		box->Add(array_list);
		box->Add(b);
		//add all this to the editor
		add_sizer(box);
		disable();//untill someone set's an array
	}
public:
	virtual ~resizeable_array_ctrl(void){};

	//disable's everything exept the new buton
	void disable(){
		wxWindowList&children = GetChildren();
		for(wxWindowListNode*child = children.GetFirst();child;child=child->GetNext()){
			child->GetData()->Disable();
		}
		new_btn->Enable();
	}

	//enable's everything
	void enable(){
		wxWindowList&children = GetChildren();
		for(wxWindowListNode*child = children.GetFirst();child;child=child->GetNext()){
			child->GetData()->Enable();
		}
	}

	//new button
	//add a new item to the end of the array, 
	//let the item's default constructor initalize it
	virtual void on_new_clicked(wxCommandEvent&){
		array.resize(array.size()+1);
		if(array.size()==1){
			//if there was nothing in the array, then the control was 
			//probably disabled exapt for the new button, so enable everything
			index = -1;
			enable();
		}

		//select the new item
		set_index((int)array.size()-1);
		//fix the combo box
		rebuild_array_list();
//		rebuild_tree();

		wxCommandEvent event(ARRAY_NEW, GetId());
		event.SetInt((int)array.size()-1);
		this->GetEventHandler()->ProcessEvent(event);

	}

	//copy the current item to the end of the array
	virtual void on_copy_clicked(wxCommandEvent&e){
		if(array.size()<1 || index<0){
			on_new_clicked(e);
			return;
		}
		array[index]=get_curent_value();
		array.push_back(type(array[index]));
		set_index((int)array.size()-1);
	//	set_curent_value(array[index]);
		rebuild_array_list();
//		rebuild_tree();

		wxCommandEvent event(ARRAY_CPY, GetId());
		event.SetInt((int)array.size()-1);
		this->GetEventHandler()->ProcessEvent(event);
		event.SetEventType(ARRAY_NEW);
		this->GetEventHandler()->ProcessEvent(event);
	}

	//get rid of the currentlly selected item
	virtual void on_delete_clicked(wxCommandEvent&){
		if(array.size()<1)
			return;
		wxCommandEvent event(ARRAY_DEL, GetId());
		event.SetInt(index);

		if(index>-1 && index<(int)array.size()){
			//check to see if there is a current item to delete
			array.erase(array.begin()+index);
			//get rid of the current item

			if(index>=(int)array.size() && array.size()){
				//if that was the end of the array move to the new end
				//unless the array is now empty
				set_index((int)array.size()-1);
			}
			if(array.size()<=0){
				//if the array is empty disable everything 
				//exept the new button
				disable();
				//nothing is in the list so nothing can be selected
				index=-1;
			}else{
				//set the control's value to the new item
				set_curent_value(array[index]);
			}
		}
		rebuild_array_list();

		this->GetEventHandler()->ProcessEvent(event);
	}
	//event handeling!
	DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE_TEMPLATE1 (resizeable_array_ctrl,array_ctrl,type)
	EVT_COMBOBOX(ARRAY_SELECT,array_ctrl::on_selection_change)
	EVT_COMMAND(ARRAY_SELECT, EDIT_DONE, array_ctrl::on_selection_change)
	EVT_COMMAND(wxID_ANY, ARRAY_INCREMENT, array_ctrl::increment_selection)
	EVT_COMMAND(wxID_ANY, ARRAY_DECREMENT, array_ctrl::decrement_selection)
	//you'd think it'd inherit that
	EVT_BUTTON(ARRAY_BUTTON_NEW,resizeable_array_ctrl::on_new_clicked)
	EVT_BUTTON(ARRAY_BUTTON_COPY,resizeable_array_ctrl::on_copy_clicked)
	EVT_BUTTON(ARRAY_BUTTON_DELETE,resizeable_array_ctrl::on_delete_clicked)
END_EVENT_TABLE()



//a templated array
//not all situations will be suitable to this setup
//so it's not the base
template<class type, class type_control>
class type_array_ctrl
	:public resizeable_array_ctrl<type>
{
protected:
	type_control*ctrl;
public:
	type_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags=0, int ID = -1)
		:resizeable_array_ctrl(parent,x,y,w,h,Title,orient,ID)
	{
		add_control(ctrl=new type_control(this,0,30,90,(subTitle=="")?20:40,subTitle),1,flags);
	}

	virtual type get_curent_value(){array[index] = ctrl->get_value(); return array[index];}
	virtual void set_curent_value(const type&i){array[index] = i; ctrl->set_value(array[index]);}

	type_control*get_child_control(){return ctrl;}

	void set_indexes(std::vector<int> i){
		if(i[0]>-1 && i[0]<(int)get_size())
			set_index(i[0]);

		if(i.size()>1 && i[1]>-1){
			i.erase(i.begin());
			ctrl->set_indexes(i);
		}
	}
};



template<class type, class type_control>
class type_static_array_ctrl
	:public array_ctrl<type>
{
	type_control*ctrl;
public:
	type_static_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags=0)
		:resizeable_array_ctrl(parent,x,y,w,h,Title,orient)
	{
		add_control(ctrl=new type_control(this,0,30,90,(subTitle=="")?20:40,subTitle),1,flags);
	}

	virtual type get_curent_value(){array[index] = ctrl->get_value(); return array[index];}
	virtual void set_curent_value(const type&i){array[index] = i; ctrl->set_value(array[index]);}
};

//array of floats
class float_array_ctrl
	:public type_array_ctrl<float, float_ctrl>
{
public:
	float_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
	}

	void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl::on_new_clicked(e);
		set_curent_value(0.0f);
	}
};

//array of ints
class int_array_ctrl
	:public type_array_ctrl<int, int_ctrl>
{
public:
	int_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
	}

	void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl::on_new_clicked(e);
		set_curent_value(0);
	}
};

//array of ints representing model numbers
class model_array_ctrl
	:public type_array_ctrl<int, model_list_ctrl>
{
public:
	model_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
	}
};

//array of ints representing model numbers
class path_idx_array_ctrl
	:public type_array_ctrl<int, path_list_ctrl>
{
public:
	path_idx_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
	}
};

//array of strings
class string_array_ctrl
	:public type_array_ctrl<std::string, string_ctrl>
{
public:
	string_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
		ctrl->SetSize(w, h-40);
	}

	void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl::on_new_clicked(e);
		set_curent_value("");
	}
};

//array of multi-line strings
class multi_string_array_ctrl
	:public type_array_ctrl<std::string, multi_string_ctrl>
{
public:
	multi_string_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
				ctrl->SetSize(200, h-50);
	}

	void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl::on_new_clicked(e);
		set_curent_value("");
	}
};

//array of vectors
class vector_array_ctrl
	:public type_array_ctrl<vector3d, vector_ctrl>
{
public:
	vector_array_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl(parent,x,y,w,h,Title, subTitle, orient, flags, ID)
	{
	}

	void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl::on_new_clicked(e);
		set_curent_value(vector3d(0,0,0));
	}
};

