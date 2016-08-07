#pragma once
#include "pcs2_CIDs.h"
#include "editor.h"
#include <string>

#include"new_btn.xpm"
#include"copy_btn.xpm"
#include"delete_btn.xpm"




inline wxString itoa(int i){return wxString().Format(_("%d"),i);}

//a static sized array, note it only hides resizeing functionality
//you can still resize programaticly by setting a diferent sized array
template<class type>
class array_ctrl :
	public editor<std::vector<type> >
{
protected:
	int control_orient;
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

		if((this->array.size())>0){
			if(index<0)index=0;
			if(index>=(int)(this->array.size()))index=(int)(this->array.size())-1;
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

	virtual void enable(){this->Enable();};
	virtual void disable(){this->Disable();};


	//event handeling!
	DECLARE_EVENT_TABLE();

	//when the item in the item in the combo box changes
public:
	void on_selection_change(wxCommandEvent& event){
		set_index(array_list->GetCurrentSelection());
	//	GetEventHandler()->ProcessEvent(wxCommandEvent(DATA_SELECTION_CHANGED, GetId()));
		event.Skip();
		rebuild_array_list();
	}

protected:
	void on_select(wxCommandEvent& event){
		rebuild_array_list();
		event.Skip();
	}

	array_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL, int ID = -1,bool a=false)//bool to diferentiate
		:editor<std::vector<type> >(parent, orient, Title, ID),index(-1), get_index_name(NULL)
	{
	}

	//when something has happened to the array this is a convenient way to handel it
	//though not the most effecent solution I doubt handleing this diferently
	//would be worth the effort
	void rebuild_array_list(){
		//clear off the exsisting selections
		array_list->Clear();
		//repopulate
		for(unsigned int i = 0; i<(this->array.size());i++){
			if(get_index_name)
				array_list->Append(wxString((*get_index_name)(i).c_str(), wxConvUTF8));
			else
				array_list->Append(wxString().Format(_("%d"),i+1));
		}
		//update the size box
		array_size_box->ChangeValue(wxString().Format(_("%ld"),(this->array.size())));
		//make sure the cuurent item is selected in the combo box
		array_list->Select(index);
	}

public:
	void set_index_name_function(std::string(*Get_index_name)(int idx)){
		get_index_name = Get_index_name;
	}


	//set's the control's current index to the one passed
	void set_index(int idx){

		if(idx > 0 && static_cast<size_t>(idx) > this->array.size()) {
			idx = -1;//if we are out of bounds set to nothing
		}

		//if we were told to set it to nothing
		if(idx<0){
			//disable
			disable();
			index=idx;
		}else{
			//otherwise enable
			enable();
			//copy what is in the control into the array
			if(index>-1 && index<(int)(this->array.size()))
				array[index]=get_curent_value();
			index=idx;
			//and make sure the controls have the right value
			set_curent_value(array[index]);
		}
		//set the combo box to the proper item
		array_list->Select(idx);

		wxCommandEvent commandEvent(DATA_SELECTION_CHANGED, this->GetId());
		this->GetEventHandler()->ProcessEvent(commandEvent);

	};

	//accessor
	int get_index(){return index;}

	size_t get_size(){return (this->array.size());}

	array_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL, int ID = -1)
		:editor<std::vector<type> >(parent, orient, Title, ID),index(-1),control_orient(orient)
	{
		//the sizer for the array control, not the data of the array, that is handeled by derived classes
		box = new wxStaticBoxSizer(wxHORIZONTAL, this,"Select");
		//the combo box
		box->Add(array_size_box = new wxTextCtrl(this,-1,"0",wxDefaultPosition,wxSize(32,-1),wxTE_READONLY|wxTE_CENTRE));
		box->Add(array_list = new wxComboBox(this,ARRAY_SELECT,"",wxDefaultPosition,wxSize(64,-1)),1);
	}
	virtual std::vector<type> get_value(){if((int)(this->array.size()) > index && index>-1)array[index] = get_curent_value(); return array;}
	virtual void set_value(const std::vector<type>&i){set_array(i);}

	//get's/sets the currently selected item
	virtual type get_curent_value()=0;
	virtual void set_curent_value(const type&i)=0;

	void increment_selection(wxCommandEvent& event){
		if((this->array.size())<1 || index < 0)return;

		if(index + 1 >= (int)(this->array.size()))
			set_index(0);
		else
			set_index(index+1);
	}

	void decrement_selection(wxCommandEvent& event){
		if((this->array.size())<1 || index < 0)return;

		if(index - 1 < 0)
			set_index((this->array.size())-1);
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
	bool user_resizeable;

public:
	//constructor, DOES _NOT_ take an array, they must be set after the 
	//control is is constructed or you get virtual function problems
		resizeable_array_ctrl(wxWindow*parent, wxString Title, int orient = wxVERTICAL, int ID = -1, bool user_resizeable_in=true)
		:array_ctrl<type>(parent, Title, orient,ID,true), user_resizeable(user_resizeable_in)
	{
		//the sizer for the array control, not the data of the array, that is handeled by derived classes
		this->box = new wxStaticBoxSizer(wxVERTICAL, this,_("Select"));
		//the combo box
		this->array_list = new combo(this,ARRAY_SELECT,_(""),wxDefaultPosition,wxSize(64,30));
		this->box->Add(this->array_list, 0, wxEXPAND);
		//sizer that holds the size display and the new/copy/delete buttons
		wxBoxSizer*b=new wxBoxSizer(wxHORIZONTAL);
		this->array_size_box = new wxTextCtrl(this,-1,_("0"),wxDefaultPosition,wxSize(32, -1),wxTE_READONLY|wxTE_CENTRE);
		b->Add(this->array_size_box, 0, wxALIGN_CENTER_VERTICAL);
		if (user_resizeable_in) {
			new_btn = new wxBitmapButton(this, ARRAY_BUTTON_NEW, wxBitmap(_new_btn),wxDefaultPosition,wxDefaultSize, wxBORDER_NONE);
			cpy_btn = new wxBitmapButton(this, ARRAY_BUTTON_COPY, wxBitmap(copy_btn),wxDefaultPosition,wxDefaultSize, wxBORDER_NONE);
			del_btn = new wxBitmapButton(this, ARRAY_BUTTON_DELETE, wxBitmap(delete_btn),wxDefaultPosition,wxDefaultSize, wxBORDER_NONE);
			new_btn->SetToolTip(_("New"));
			cpy_btn->SetToolTip(_("Copy"));
			del_btn->SetToolTip(_("Delete"));
			//add the buttons'n stuff to the proper sizer
			b->Add(new_btn, 0, wxALIGN_CENTER_VERTICAL);
			b->Add(cpy_btn, 0, wxALIGN_CENTER_VERTICAL);
			b->Add(del_btn, 0, wxALIGN_CENTER_VERTICAL);
		}
		this->box->Add(b);
		//add all this to the editor
		this->add_sizer(this->box);
		disable();//untill someone set's an array
	}
public:
	virtual ~resizeable_array_ctrl(void){};

	//disable's everything exept the new buton
	void disable(){
		wxWindowList&children = this->GetChildren();
		for(wxWindowListNode*child = children.GetFirst();child;child=child->GetNext()){
			child->GetData()->Disable();
		}
		if (user_resizeable) {
			new_btn->Enable();
		}
	}

	//enable's everything
	void enable(){
		wxWindowList&children = this->GetChildren();
		for(wxWindowListNode*child = children.GetFirst();child;child=child->GetNext()){
			child->GetData()->Enable();
		}
	}

	//new button
	//add a new item to the end of the array, 
	//let the item's default constructor initalize it
	virtual void on_new_clicked(wxCommandEvent&){
		this->array.resize((this->array.size())+1);
		if((this->array.size())==1){
			//if there was nothing in the array, then the control was 
			//probably disabled exapt for the new button, so enable everything
			this->index = -1;
			enable();
		}

		//select the new item
		this->set_index((int)(this->array.size())-1);
		//fix the combo box
		this->rebuild_array_list();
//		rebuild_tree();

		wxCommandEvent event(ARRAY_NEW, this->GetId());
		event.SetInt((int)(this->array.size())-1);
		this->GetEventHandler()->ProcessEvent(event);

	}

	//copy the current item to the end of the array
	virtual void on_copy_clicked(wxCommandEvent&e){
		if((this->array.size())<1 || this->index<0){
			on_new_clicked(e);
			return;
		}
		this->array[this->index]=this->get_curent_value();
		this->array.push_back(type(this->array[this->index]));
		this->set_index((int)(this->array.size())-1);
	//	set_curent_value(array[index]);
		this->rebuild_array_list();
//		rebuild_tree();

		wxCommandEvent event(ARRAY_CPY, this->GetId());
		event.SetInt((int)(this->array.size())-1);
		this->GetEventHandler()->ProcessEvent(event);
		event.SetEventType(ARRAY_NEW);
		this->GetEventHandler()->ProcessEvent(event);
	}

	//get rid of the currentlly selected item
	virtual void on_delete_clicked(wxCommandEvent&){
		if((this->array.size())<1)
			return;
		wxCommandEvent event(ARRAY_DEL, this->GetId());
		event.SetInt(this->index);

		if(this->index>-1 && this->index<(int)(this->array.size())){
			//check to see if there is a current item to delete
			this->array.erase(this->array.begin()+this->index);
			//get rid of the current item

			if(this->index>=(int)(this->array.size()) && (this->array.size())){
				//if that was the end of the array move to the new end
				//unless the array is now empty
				this->set_index((int)(this->array.size())-1);
			}
			if((this->array.size())<=0){
				//if the array is empty disable everything 
				//exept the new button
				disable();
				//nothing is in the list so nothing can be selected
				this->index=-1;
			}else{
				//set the control's value to the new item
				this->set_curent_value(this->array[this->index]);
			}
		}
		this->rebuild_array_list();

		this->GetEventHandler()->ProcessEvent(event);
	}
	//event handeling!
	DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE_TEMPLATE1 (resizeable_array_ctrl,array_ctrl<type>,type)
	EVT_COMBOBOX(ARRAY_SELECT,array_ctrl<type>::on_selection_change)
	EVT_COMMAND(ARRAY_SELECT, EDIT_DONE, array_ctrl<type>::on_selection_change)
	EVT_COMMAND(wxID_ANY, ARRAY_INCREMENT, array_ctrl<type>::increment_selection)
	EVT_COMMAND(wxID_ANY, ARRAY_DECREMENT, array_ctrl<type>::decrement_selection)
	//you'd think it'd inherit that
	EVT_BUTTON(ARRAY_BUTTON_NEW,resizeable_array_ctrl<type>::on_new_clicked)
	EVT_BUTTON(ARRAY_BUTTON_COPY,resizeable_array_ctrl<type>::on_copy_clicked)
	EVT_BUTTON(ARRAY_BUTTON_DELETE,resizeable_array_ctrl<type>::on_delete_clicked)
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
	type_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags=0, int ID = -1, bool user_resizeable_in =true)
		:resizeable_array_ctrl<type>(parent, Title, orient, ID, user_resizeable_in)
	{
		this->add_control(ctrl=new type_control(this,subTitle),1,flags);
	}

	virtual type get_curent_value(){this->array[this->index] = ctrl->get_value(); return this->array[this->index];}
	virtual void set_curent_value(const type&i){this->array[this->index] = i; ctrl->set_value(this->array[this->index]);}

	type_control*get_child_control(){return ctrl;}

	void set_indexes(std::vector<int> i){
		if(i[0]>-1 && i[0]<(int)this->get_size())
			this->set_index(i[0]);

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
	type_static_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags=0)
		:resizeable_array_ctrl<type>(parent, Title,orient)
	{
		add_control(ctrl=new type_control(this,subTitle),1,flags);
	}

	virtual type get_curent_value(){this->array[this->index] = ctrl->get_value(); return this->array[this->index];}
	virtual void set_curent_value(const type&i){this->array[this->index] = i; ctrl->set_value(this->array[this->index]);}
};

//array of floats
class float_array_ctrl
	:public type_array_ctrl<float, float_ctrl>
{
public:
	float_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<float, float_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}

	virtual void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl<float>::on_new_clicked(e);
		set_curent_value(0.0f);
	}
};

//array of ints
class int_array_ctrl
	:public type_array_ctrl<int, int_ctrl>
{
public:
	int_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<int, int_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}

	virtual void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl<int>::on_new_clicked(e);
		set_curent_value(0);
	}
};

//array of ints representing model numbers
class model_array_ctrl
	:public type_array_ctrl<int, model_list_ctrl>
{
public:
	model_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<int, model_list_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}
};

//array of ints representing model numbers
class path_idx_array_ctrl
	:public type_array_ctrl<int, path_list_ctrl>
{
public:
	path_idx_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<int, path_list_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}
};

//array of strings
class string_array_ctrl
	:public type_array_ctrl<std::string, string_ctrl>
{
public:
	string_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<std::string, string_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}

	virtual void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl<std::string>::on_new_clicked(e);
		set_curent_value("");
	}
};

//array of multi-line strings
class multi_string_array_ctrl
	:public type_array_ctrl<std::string, multi_string_ctrl>
{
public:
	multi_string_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxVERTICAL, int flags = 0, int ID = -1)
		:type_array_ctrl<std::string, multi_string_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}

	virtual void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl<std::string>::on_new_clicked(e);
		set_curent_value("");
	}
};

//array of vectors
class vector_array_ctrl
	:public type_array_ctrl<vector3d, vector_ctrl>
{
public:
	vector_array_ctrl(wxWindow*parent, wxString Title, wxString subTitle, int orient = wxHORIZONTAL, int flags = 0, int ID = -1)
		:type_array_ctrl<vector3d, vector_ctrl>(parent, Title, subTitle, orient, flags, ID)
	{
	}

	virtual void on_new_clicked(wxCommandEvent&e){
		resizeable_array_ctrl<vector3d>::on_new_clicked(e);
		set_curent_value(vector3d(0,0,0));
	}
};

