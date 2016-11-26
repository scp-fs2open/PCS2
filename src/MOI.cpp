#include "MOI.h"
#include <cmath>
#include <vector>
using namespace std;

void MOI::invert(){
	MOI dest;
	dest.a2d[0][0] = (-a2d[1][2]*a2d[2][1]+a2d[1][1]*a2d[2][2]);
	dest.a2d[0][1] = ( a2d[0][2]*a2d[2][1]-a2d[0][1]*a2d[2][2]);
	dest.a2d[0][2] = (-a2d[0][2]*a2d[1][1]+a2d[0][1]*a2d[1][2]);

	dest.a2d[1][0] = ( a2d[1][2]*a2d[2][0]-a2d[1][0]*a2d[2][2]);
	dest.a2d[1][1] = (-a2d[0][2]*a2d[2][0]+a2d[0][0]*a2d[2][2]);
	dest.a2d[1][2] = ( a2d[0][2]*a2d[1][0]-a2d[0][0]*a2d[1][2]);

	dest.a2d[2][0] = (-a2d[1][1]*a2d[2][0]+a2d[1][0]*a2d[2][1]);
	dest.a2d[2][1] = ( a2d[0][1]*a2d[2][0]-a2d[0][0]*a2d[2][1]);
	dest.a2d[2][2] = (-a2d[0][1]*a2d[1][0]+a2d[0][0]*a2d[1][1]);
	(*this) = dest;
}

MOI calc_cuboid_MOI(vector3d center, double xd, double yd, double zd){
	MOI ret;
	double m = 8.0*xd*yd*zd;
	ret.a2d[0][0] = (8/3*m*xd*yd*zd*((pow(yd,2)+3*pow(center.y,2)+pow(zd,2)+3*pow(center.z,2))));
	ret.a2d[1][1] = (8/3*m*xd*yd*zd*((pow(xd,2)+3*pow(center.x,2)+pow(zd,2)+3*pow(center.z,2))));
	ret.a2d[2][2] = (8/3*m*xd*yd*((pow(xd,2)+3*pow(center.x,2)+pow(yd,2)+3*pow(center.y,2)))*zd);
	ret.a2d[0][1] = 	ret.a2d[1][0] = 8*m*xd*center.x*yd*center.y*zd;
	ret.a2d[0][2] = 	ret.a2d[2][0] = 8*m*xd*center.x*yd*zd*center.z;
	ret.a2d[2][1] = 	ret.a2d[1][2] = 8*m*xd*yd*center.y*zd*center.z;

//	ret.invert();
	
	return ret;
}

double minmax(vector3d tri[3], int coord, bool min){
	double ret = tri[0][coord];
	if(ret > tri[1][coord] && min)ret = tri[1][coord];
	if(ret > tri[2][coord] && min)ret = tri[2][coord];
	return ret;
}

MOI calc_under_tri(vector3d tri[3], int res){

	vector3d norm = CrossProduct(tri[0]-tri[1],tri[2]-tri[1]);
	vector3d pcenter = AverageVectors(3, tri);

	MOI ret;
	memset(&ret, 0,sizeof(MOI));
	if(norm.y == 0.0)return ret;

	/*double xmin = minmax(tri, 0, true);
	double xmax = minmax(tri, 0, false);
	double ymin = minmax(tri, 1, true);
	double ymax = minmax(tri, 1, false);*/
	double zmin = minmax(tri, 2, true);
	double zmax = minmax(tri, 2, false);

	int i;

	vector3d*top = &tri[0];
	for(i=1; i<3; i++){
		if(tri[i].z > top->z)
			top=&tri[i];
	}
	vector3d*bottom = &tri[0];
	for(i=1; i<3; i++){
		if(tri[i].z < bottom->z)
			bottom=&tri[i];
	}
	vector3d*mid = NULL;//not all triangles will have a mid point
	for(i=0; i<3; i++){
		if(tri[i].z > bottom->z && tri[i].z < top->z )
			mid=&tri[i];
	}

	bool midpoint = mid!=NULL;
	if(!midpoint){
		mid = &tri[0];
		for(i=0; i<2; i++){
			if(tri[i] == *bottom || tri[i] == *top )
				mid=&tri[i++];
		}
	}

	/*		  
	for the lower half
			  r*top l*top
	           /      \      r*mid
	   l*mid  /        \     /
		 \   /          \   /
		  \ /            \ /
	   cent*bottom    cent*bottom
	   botom is always the center
	   but mid and top can be either the left or right

	   for the upper half the top will be the cent, 
	   mid or bottom will be left or right
	*/


	for(int z =0; z<res; z++){
		double pz = zmin + (double(z+1)/double(res+1))*(zmax-zmin);
		
		vector3d*l, *r, *c;//left and right verts
		bool lower = true;//lower half of the triangle

		//if this triangle had a midpoint
		if(midpoint){
			//then if we are in an upper half or lower half 
			//is dependent on were we actualy are in the triangle
			if(mid->z < pz)
				lower = false;
		}else{
			//otherwise it just depends on if the defacto mid is 
			//below the  top(and thus equal to the botom)
			if(mid->z < top->z)
				lower = false;
		}//finding which half of the triangle we are in

		if(lower){
			//lower half
			if(top->x>mid->x){
				r=top;
				l=mid;
			}else{
				l=top;
				r=mid;
			}
			c=bottom;
		}else{
			//upper half of the triangle
			if(bottom->x>mid->x){
				r=bottom;
				l=mid;
			}else{
				l=bottom;
				r=mid;
			}
			c=top;
		}//finding center, left and right
		double ml, mr;//left and right slope
		double cl, cr;//left and right intercept

		ml = (l->x-c->x)/(l->z-c->z);
		mr = (c->x-r->x)/(c->z-r->z);

		cl = c->x -ml*c->z;
		cr = c->x -mr*c->z;

		//now we have line formulas m*z+c=x for both lines
		
		double L, R;//left and right x coords of the triangle at pz

		L = ml * pz +cl;
		R = mr * pz +cr;

		vector3d vol_cent; //center of the test volume

		vol_cent.x = float((L+R)/2.0);

		vol_cent.z = float(pz);

		bool s;//if the intersect fails, we will be  in a world of hurt
		vol_cent.y = plane_line_intersect(pcenter, norm, vol_cent, vector3d(0,1,0), &s).y/2.0f;

		if(s){
			ret += calc_cuboid_MOI(vol_cent, R-L, std::fabs(vol_cent.y*2), pz);
		}
	}
	if(norm.y<0)
		ret.negate();
	return ret;
}

MOI calc_geometry_MOI(PCS_Model&model){
	return calc_cuboid_MOI(vector3d(0,0,0), 1, 1, 1);
	
	if(model.GetSOBJCount()<1)return MOI();

	int useable_model = 0;

	if(model.GetLODCount()>0){
		useable_model = model.LOD(0);
	}

	vector<pcs_polygon> &geometry = model.SOBJ(useable_model).polygons;

	MOI ret;
	memset(&ret, 0,sizeof(MOI));
	for(unsigned int i = 0; i<geometry.size(); i++ ){
		vector3d tri[3];
		tri[0] = geometry[i].verts[0].point;
		for(unsigned int j=1; j< geometry[i].verts.size()-1; j++){
			tri[1] = geometry[i].verts[j].point;
			tri[2] = geometry[i].verts[j+1].point;
			ret += calc_under_tri(tri, 10);
		}
	}

	return ret/model.GetMass();
}
