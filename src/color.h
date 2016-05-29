#pragma once
#include "pcs_file.h"
#include <wx/colour.h>

#ifndef byte
#define byte unsigned char
#endif

struct color{
	color(){c=0;};

	color(byte R, byte G, byte B, byte A=255)
	:r(R), g(G), b(B), a(A){};

	color(wxColour C)
	:r(C.Red()), g(C.Green()), b(C.Blue()), a(C.Alpha()){};

	union{
		byte	col[4];
		int		c;
		struct{
		byte	r,g,b,a;
		};
	};
};

inline color operator * (color col, float s){
	color ret;
	int temp;

	ret.r = temp = col.r * s;
	if(temp > 255)ret.r = 255;
	if(temp < 0)ret.r = 0;

	ret.g = temp = col.g * s;
	if(temp > 255)ret.g = 255;
	if(temp < 0)ret.g = 0;

	ret.b = temp = col.b * s;
	if(temp > 255)ret.b = 255;
	if(temp < 0)ret.b = 0;
	
	ret.a = temp = col.a * s;
	if(temp > 255)ret.a = 255;
	if(temp < 0)ret.a = 0;

	return ret;
}

color get_SOBJ_color();
color get_TXTR_color();
color get_SHLD_color();