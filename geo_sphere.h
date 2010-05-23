#pragma once

#include "vector3d.h"
#include <string>
#include "color.h"
#include "ogl_vertex_buffers.h"


#define PHI 1.6180339887f
#define GS1 1/(1.9021130325903f)
#define GSP PHI/(1.9021130325903f)

class geo_sphere{
	struct stri{
		stri(int a = 0, int b = 0, int c = 0){
			point[0]=a;
			point[1]=b;
			point[2]=c;
		}
		int point[3];
	};
	std::vector<vector3d>gspoints;
	std::vector<stri>gstris;


	int pow(int a, int b){
		int r = 1;
		for(int i = 0; i<b; i++)r*=a;
		return r;
	}
	int n_tri(int res){
		return pow(4,res)*20;
	}
	int n_vert(int res){
		return pow(4,res)*10+2;
	}

	/*
	isohedron points are at
		(0, 1, PHI)
		(1, PHI, 0)
		(PHI, 0, 1)

		for isohedron with segment length 2,
		this would have a radius of ~1.9021130325903
		...I think
	*/

	void make_isohedron();

	//adds the vector to the gspoints if it isn't there, 
	//returns the location were it was added or found
	size_t add_if_not_present(vector3d&vec);

	std::vector<int>gs_sphere_start;
	//for a sphere of resolution R the tris for it start at 
	//gs_sphere_start[R], and there will be R^2*20 of them
	//verts 0 to R^2*10+2 will be used

	//will add points along exsisting edges and make new faces from them
	void subdivide_geosphere(int res);

	void make_geo_spheres(int max_res);

public:
	geo_sphere(int r=0){
		make_geo_spheres(r);
	}

	void draw(const vector3d&pnt, float rad, unsigned int res, color col);
	ogl_vertex_buffer make_vertex_buffer(int res);
};