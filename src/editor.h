#pragma once
#include <string>
#include <vector>
#include <wx/wx.h>

template<class type>
class editor :
	public wxPanel
{
protected:
	//all controls will be in this
	wxSizer*box;

//adds a control to the editor
	void add_control(wxWindow*ctrl, int proportion = 0,int flag = 0, int border = 0){//, bool streach = true){

		box->Add(ctrl,proportion,flag,border);
		box->Layout();
	}
	void add_sizer(wxSizer*s, int proportion = 0,int flag = 0, int border = 0){
		box->Add(s,proportion,flag,border);
		box->Layout();
	}
	wxString title;

public:
	editor(wxWindow*parent, int orientation, wxString Title, int ID=-1)
		:wxPanel(parent,ID),title(Title)
	{
		if(Title == _("")){
			box = new wxBoxSizer(orientation);
		}else{
			box = new wxStaticBoxSizer(new wxStaticBox(this,-1,Title),orientation);
		}
		SetSizer(box);
		box->Layout();
	}
public:
	virtual~editor(void){};

	virtual type get_value()=0;
	virtual void set_value(const type&value)=0;

	virtual void set_indexes(std::vector<int> i){};
	//virtual function which defaults to doing nothing
	//this function is for setting the indexes of child array controls
	//but if there are no array controls, then nothing should be done
	//which is the default behavior

};


/*
//editorr a strut
class type_ctrl :
	public editor<structtype>
{
protected:
//controls
public:
	
	type_ctrl(wxWindow*parent, int x, int y, int w, int h, wxString Title, int orient = wxVERTICAL)
	:editor(parent,x,y,w,h, orient, Title)
	{
		//add controls
	};

	virtual ~type_ctrl(void){};

	//set the control's value
	void set_value(const structtype&t){
	}

	//return's the control's value
	structtype get_value(){
		structtype t;
		return t;
	}
	
};
*/
