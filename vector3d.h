//****************************************************************************
// vector3d.h
// Derek Meek
// Had to do this because both POF and COB share the vector
//****************************************************************************

/*
 * $Logfile: /vector3d.h $
 * $Revision: 1.19 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: vector3d.h,v $
 * Revision 1.19  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.18  2008/11/09 21:41:12  kazan
 * Pass By Value => Pass By Reference
 *
 * Revision 1.17  2008/09/17 19:34:02  kazan
 * little cleanup
 *
 * Revision 1.16  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.15  2008/02/09 23:01:47  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.14  2008/02/09 22:33:05  kazan
 * bye bye kaz_vector
 *
 * Revision 1.13  2008/01/10 18:28:11  bobboau
 * *** empty log message ***
 *
 * Revision 1.12  2007/07/25 03:18:30  bobboau
 * fixed bug in right buttom omnipoint movement (was broken aparently, just noticed, guess no one else uses it)
 *
 * Revision 1.11  2007/07/06 05:38:56  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.10  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.8  2007/07/01 08:25:01  bobboau
 * fixed raypicking and moveing for orthographic projection
 *
 * Revision 1.7  2007/06/24 10:22:17  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.6  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.5  2007/06/15 06:00:19  bobboau
 * number of small fixes, mostly related to updateing data in one subsystem when it should be
 *
 * fixed weapon point bug were the editing of one type would result in the eraseing of the other
 *
 * added PINF editor
 *
 * added convergence calculator for weapon points
 *
 * quickened the scroll zoom and made it proportional
 *
 * Revision 1.4  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.3  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.2  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.1.1.1  2004/12/31 06:19:42  kazan
 * no message
 *
 *
 *
 */

#include <ios>
#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_
#include <string>
#include <vector>
#include <cstring>

// COB and POF have this in common
struct vector3d {
	float x, y, z;

	vector3d() : x(0.0), y(0.0), z(0.0) {}
	vector3d(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
	/*vector3d(double ax, double ay, double az) 
		{ 
			x = float(ax); 
			y = float(ay); 
			az = float(az); 
		}*/
	//vector3d(vector3d &v) : x(v.x), y(v.y), z(v.z) {}
	//operator=(vector3d &v) { x=v.x; y=v.x; z=v.z; }
	float&operator[](int i){
		if(i%3 == 0)return x;
		if(i%3 == 1)return y;
		if(i%3 == 2)return z;
		static float error;
		return error;//should not happen
	}
	float operator[](int i) const {
		if(i%3 == 0)return x;
		if(i%3 == 1)return y;
		if(i%3 == 2)return z;
		static float error;
		return error;//should not happen
	}
};

//return true if non NAN
inline bool no_nan(float f){
	return f>0.0f || f<=0.0f;
}
//return true if none of the values are NAN
inline bool no_nan(vector3d&vec){
	return no_nan(vec.x) && no_nan(vec.y) && no_nan(vec.z);
}

float VectorMagnitude(vector3d &v);

std::ostream & operator << ( std::ostream & os, vector3d &vec);
std::istream & operator >> ( std::istream & in, vector3d &vec);
bool operator==(vector3d &one, vector3d &two);
bool operator==(const vector3d &one, const vector3d &two);
bool operator!=(const vector3d &one, const vector3d &two);
bool operator!=(vector3d &one, vector3d &two);

vector3d operator-(const vector3d &one, const vector3d &two);
vector3d operator+(const vector3d &one, const vector3d &two);
vector3d& operator+=(vector3d &one, const vector3d &two);
vector3d& operator-=(vector3d &one, const vector3d &two);

vector3d MakeUnitVector(const vector3d &vect);
vector3d SafeMakeUnitVector(const vector3d &vect);

vector3d operator *(float scalar, const vector3d &v); 
vector3d operator *(const vector3d &v, float scalar); 
vector3d operator /(const vector3d &v, float scalar);
float Distance(const vector3d &one, const vector3d &two);

bool operator>(const vector3d &one, const vector3d &two);
bool operator<(const vector3d &one, const vector3d &two);

bool operator>=(const vector3d &one, const vector3d &two);
bool operator<=(const vector3d &one, const vector3d &two);

vector3d MakeVector(float ax, float ay, float az);
float Abs(float n);

vector3d AverageVectors(int numvectors, vector3d *vects);
float Angle(vector3d &v1, vector3d &v2);

vector3d CrossProduct(const vector3d &one, const vector3d &two);
float dot(const vector3d& A, const vector3d& B);
float Magnitude(const vector3d &vec);
inline bool null_vec(vector3d &a){return a.x == 0.0f && a.y == 0.0f && a.z == 0.0f;}

vector3d average_vectors_if_less_than_angle(int numvectors, float angle, vector3d src, vector3d *vects);

vector3d FigureNormal(vector3d &one, vector3d &two, vector3d &three);
float FindObjectRadius(const vector3d &max, const vector3d &min, const vector3d &center);
void ExpandBoundingBoxes(vector3d &max, vector3d &min, const vector3d &cur);

//returns distance of p from the line defined as starting at lp and
//going in the direction of ln, is negitive if the point is behind
//the start of the line
float point_line_distance(const vector3d& p, const vector3d& lp, vector3d& ln);

//gives the point were the
//success is true unless the line is paralel to the plane,
//in this case the return value is {0,0,0}
vector3d plane_line_intersect(vector3d plane_pt, vector3d plane_norm, vector3d line_pt, vector3d line_norm, bool*success=NULL);

//returns the closest point on line 2 to line 1
vector3d closest_point_between_lines(vector3d p1, vector3d v1, vector3d p2, vector3d v2);

//returns the closes point on he line tp p
vector3d closest_point_on_line(vector3d p, vector3d lp, vector3d ln);

//given a point on the poly plane, find if it is in the poly
//stolen from FS2 source, which was in turn stolen from
//From Graphics Gems I, "An efficient Ray-Polygon intersection", p390
//returns non zero on sucsess
int point_face(vector3d *checkp, std::vector<vector3d> verts, vector3d * norm1);


//there is probly a better place for this, but as of now it's only used in the header
namespace bobboau {

struct matrix{
	matrix(){
		memset(a2d,0,sizeof(float)*9);
		a2d[0][0] = 1;
		a2d[1][1] = 1;
		a2d[2][2] = 1;
	}
	matrix(float f[3][3]){
		memcpy(a2d,f,sizeof(float)*9);
	}
	matrix(vector3d r, vector3d u, vector3d f){
		memcpy(a2d,&r,sizeof(float)*3);
		memcpy(a2d[1],&u,sizeof(float)*3);
		memcpy(a2d[2],&f,sizeof(float)*3);
	}
	matrix(const matrix&m){
		memcpy(a2d,&m,sizeof(float)*9);
	}
	matrix operator=(const matrix&m){
		memcpy(a2d,&m,sizeof(float)*9);
		return (*this);
	}

	float a2d[3][3];

	matrix operator * (const float&f){
		return matrix(vector3d(a2d[0][0]*f, a2d[0][1]*f, a2d[0][2]*f),
			vector3d(a2d[1][0]*f, a2d[1][1]*f, a2d[1][2]*f),
			vector3d(a2d[2][0]*f, a2d[2][1]*f, a2d[2][2]*f)
			);
	}

	matrix operator + (const matrix&m){
		return matrix(vector3d(a2d[0][0]+m.a2d[0][0], a2d[0][1]+m.a2d[0][1], a2d[0][2]+m.a2d[0][2]),
			vector3d(a2d[1][0]+m.a2d[1][0], a2d[1][1]+m.a2d[1][1], a2d[1][2]+m.a2d[1][2]),
			vector3d(a2d[2][0]+m.a2d[2][0], a2d[2][1]+m.a2d[2][1], a2d[2][2]+m.a2d[2][2])
			);
	}
	matrix invert(){
		matrix ret;
		double d = -a2d[0][2]*a2d[1][1]*a2d[2][0] + a2d[0][1]*a2d[1][2]*a2d[2][0] + a2d[0][2]*a2d[1][0]*a2d[2][1] - a2d[0][0]*a2d[1][2]*a2d[2][1] - a2d[0][1]*a2d[1][0]*a2d[2][2] + a2d[0][0]*a2d[1][1]*a2d[2][2];

		ret.a2d[0][0] = float(double(-a2d[1][2]*a2d[2][1] + a2d[1][1]*a2d[2][2])/d);
		ret.a2d[0][1] = float(double(a2d[0][2]*a2d[2][1] - a2d[0][1]*a2d[2][2])/d);
		ret.a2d[0][2] = float(double(-a2d[0][2]*a2d[1][1] + a2d[0][1]*a2d[1][2])/d);

		ret.a2d[1][0] = float(double(a2d[1][2]*a2d[2][0] - a2d[1][0]*a2d[2][2])/d);
		ret.a2d[1][1] = float(double(-a2d[0][2]*a2d[2][0] + a2d[0][0]*a2d[2][2])/d);
		ret.a2d[1][2] = float(double(a2d[0][2]*a2d[1][0] - a2d[0][0]*a2d[1][2])/d);

		ret.a2d[2][0] = float(double(-a2d[1][1]*a2d[2][0] + a2d[1][0]*a2d[2][1])/d);
		ret.a2d[2][1] = float(double(a2d[0][1]*a2d[2][0] - a2d[0][0]*a2d[2][1])/d);
		ret.a2d[2][2] = float(double(-a2d[0][1]*a2d[1][0] + a2d[0][0]*a2d[1][1])/d);

		return ret;
	}
};

} //namespace bobboau

#endif
