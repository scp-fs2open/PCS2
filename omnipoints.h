
/*file for defineing omnipoints, omnipoints describe the graphicaly 
editable protion of a chunck's data. it's a 2d array of vectors with
position and optionaly radius and normal, and other flags (such as if 
it should be drawen as a path)*/

#pragma once
#include "pcs_file.h"
#include "color.h"


#define OMNIPOINT_RAD					(1 << 0)//use radius
#define OMNIPOINT_NORM					(1 << 1)//use normal
#define OMNIPOINT_PATH					(1 << 2)//draw a line from one point to the next
#define OMNIPOINT_COMMON_NORMAL			(1 << 3)//all points in a list should use the same normal (the first one)
#define OMNIPOINT_DONT_DRAW				(1 << 4)//don't draw them
#define OMNIPOINT_CLOSED_PATH			(1 << 5)//draw a line from one point to the next and from the last to the first
#define OMNIPOINT_ANY_PATH				(OMNIPOINT_PATH | OMNIPOINT_CLOSED_PATH)

struct omnipoint{
	omnipoint():model(-1){};
	vector3d pos;
	vector3d norm;
	float rad;
	int model;	//which model space this is atached to, -1 is global space
};

struct omnipoints{
	omnipoints()
		:flags(OMNIPOINT_DONT_DRAW)
	{}
	std::vector<std::vector<omnipoint> > point;
	color selected_item;
	color selected_list;
	color unselected;
	int flags;
};
