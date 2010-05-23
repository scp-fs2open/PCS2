//****************************************************************************
// vector3d.cpp
// Author: Derem Meek
// Contents: Implementation of vector functions
//****************************************************************************

/*
 * $Logfile: /vector3d.cpp $
 * $Revision: 1.15 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: vector3d.cpp,v $
 * Revision 1.15  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.14  2008/11/09 21:41:12  kazan
 * Pass By Value => Pass By Reference
 *
 * Revision 1.13  2008/02/12 18:59:57  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.12  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.11  2008/02/09 22:33:05  kazan
 * bye bye kaz_vector
 *
 * Revision 1.10  2008/01/10 18:28:11  bobboau
 * *** empty log message ***
 *
 * Revision 1.9  2007/06/24 10:22:17  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.8  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.7  2007/06/18 15:15:29  kazan
 * Fix #6 "Strange inverted lighting on compiled POFs"
 * Add matrix class and it's dependancies for bobboau's usage
 *
 * Revision 1.6  2007/06/15 06:00:19  bobboau
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
 * Revision 1.5  2007/05/08 02:40:45  kazan
 * bug fixes, error messages
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


#include "vector3d.h"
#include <cmath>
#include <memory.h>
#include <cstdio>
#include <iostream>


//returns the average of all vectors that are within the given angle of the src vector
vector3d average_vectors_if_less_than_angle(int numvectors, float angle, vector3d src, vector3d *vects)
{
	vector3d RetVal = MakeVector(0, 0, 0);
	int nv = 0;

	for (int i = 0; i < numvectors; i++)
	{
		if(Angle(src,vects[i])<angle){
			RetVal += vects[i];
			nv++;
		}
	}

	RetVal = RetVal / float(nv);

	return RetVal;
}


//****************************************************************************************************************


float Magnitude(vector3d &vec)
{
	return (float)sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

//****************************************************************************************************************

vector3d CrossProduct(vector3d &one, vector3d &two)
{
  return MakeVector(float(double(one.y)*double(two.z)-double(two.y)*double(one.z)), 
					float(double(two.x)*double(one.z)-double(one.x)*double(two.z)), 
					float(double(one.x)*double(two.y)-double(two.x)*double(one.y))); 
} 

float dot(vector3d&A, vector3d&B){
	return A.x*B.x+A.y*B.y+A.z*B.z;
}

vector3d FigureNormal(vector3d &one, vector3d &two, vector3d &three)
{
	vector3d ret;
	double v1x, v1y, v1z, v2x, v2y, v2z;

	/*
	C.x = A.y*B.z - A.z*B.y;
	C.y = A.z*B.x - A.x*B.z;
	C.z = A.x*B.y - A.y*B.x;

	I found Cross some information on the net on how to do this.. he crossed some things
	*/

	v1x = double(one.x) - double(two.x);
	v1y = double(one.y) - double(two.y);
  	v1z = double(one.z) - double(two.z);

	v2x = double(three.x) - double(two.x);
	v2y = double(three.y) - double(two.y);
  	v2z = double(three.z) - double(two.z);
	
	ret.x = float(((v1y * v2z) - (v1z * v2y)));
	ret.y = float(((v1z * v2x) - (v1x * v2z)));
	ret.z = float(((v1x * v2y) - (v1y * v2x)));

	return ret;

}
//****************************************************************************************************************


float VectorMagnitude(vector3d &v)
{
	return Magnitude(v);
}

//****************************************************************************************************************


float Angle(vector3d &v1, vector3d &v2)
{
	float Dp1 = VectorMagnitude(v1)*VectorMagnitude(v2); // missing the angle calc
	float Dp2 = (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);

	float Ang = Dp2/Dp1;
	if (Ang > 1.00000f)
		Ang = 1.00000f;
	if (Ang < -1.00000f)
		Ang = -1.00000f;
	Ang = (float) acos(Ang); //inverse cosine ang (cos-1 aka arccos)

	return Ang;
}


//****************************************************************************************************************

vector3d operator+=(vector3d &one, const vector3d &two)
{
	one = one + two;
	return one;
}


//****************************************************************************************************************

vector3d AverageVectors(int numvectors, vector3d *vects)
{
	vector3d RetVal = MakeVector(0, 0, 0);

	for (int i = 0; i < numvectors; i++)
	{
		RetVal += vects[i];
	}

	RetVal = RetVal / float(numvectors);

	return RetVal;
}

//****************************************************************************************************************

vector3d MakeVector(float ax, float ay, float az)
{
	vector3d temp;
	temp.x = ax;
	temp.y = ay;
	temp.z = az;

	return temp;
}

//****************************************************************************************************************

vector3d MakeUnitVector(vector3d &vect)
{
	double VMag = sqrt(double(vect.x * vect.x) 
		+ double(vect.y * vect.y) 
		+ double(vect.z * vect.z));

	return vector3d(float(double(vect.x) / VMag), 
					float(double(vect.y) / VMag), 
					float(double(vect.z) / VMag));


}
//****************************************************************************************************************


bool operator>(vector3d &one, vector3d &two)
{
	return ((one.x >= two.x) && (one.y >= two.y) && (one.z >= two.z) && !(one == two));
}

bool operator<(vector3d &one, vector3d &two)
{
	return ((one.x <= two.x) && (one.y <= two.y) && (one.z <= two.z) && !(one == two));
}

bool operator>=(vector3d &one, vector3d &two)
{
	return ((one.x >= two.x) && (one.y >= two.y) && (one.z >= two.z));
}


bool operator<=(vector3d &one, vector3d &two)
{
	return ((one.x <= two.x) && (one.y <= two.y) && (one.z <= two.z));
}

bool operator==(vector3d &one, vector3d &two)
{
	return (fabs(one.x - two.x) < 0.0001 && fabs(one.y - two.y) < 0.0001  && fabs(one.z - two.z) < 0.0001 );
}

bool operator==(const vector3d &one, const vector3d &two)
{
	return (fabs(one.x - two.x) < 0.0001 && fabs(one.y - two.y) < 0.0001  && fabs(one.z - two.z) < 0.0001 );
}

bool operator!=(const vector3d &one, const vector3d &two)
{
	return !(one == two);
}

bool operator!=(vector3d &one, vector3d &two)
{
	return !(one == two);
}

//****************************************************************************************************************


vector3d operator+(const vector3d &one, const vector3d &two)
{
	vector3d RetVal;
	RetVal.x = one.x + two.x;
	RetVal.y = one.y + two.y;
	RetVal.z = one.z + two.z;
	return RetVal;
}


vector3d operator-(const vector3d &one, const vector3d &two)
{
	vector3d Vtemp;
	Vtemp.x = one.x - two.x;
	Vtemp.y = one.y - two.y;
	Vtemp.z = one.z - two.z;

	return Vtemp;
}

//****************************************************************************************************************


float Distance(const vector3d &one, const vector3d &two)
{
	float A = (one.x - two.x);
	float B = (one.y - two.y);
	float C = (one.z - two.z);
	float D = (A*A) + (B*B) + (C*C);

	return float(sqrt(D));
}


//****************************************************************************************************************


std::ostream & operator << ( std::ostream & os, vector3d &vec)
{
	os << vec.x << " " << vec.y << " " << vec.z;
	return os;
}


std::istream & operator >> ( std::istream & in, vector3d &vec)
{
	in >> vec.x;
	in >> vec.y;
	in >> vec.z;

	return in;
}


//****************************************************************************************************************

vector3d operator *(float scalar, vector3d &v)
{
	vector3d Ret;
	Ret.x = scalar * v.x;
	Ret.y = scalar * v.y;
	Ret.z = scalar * v.z;
	return Ret;
}

//diferent parameter order
vector3d operator *(vector3d &v,  float scalar)
{
	vector3d Ret;
	Ret.x = scalar * v.x;
	Ret.y = scalar * v.y;
	Ret.z = scalar * v.z;
	return Ret;
}


vector3d operator /(vector3d &v, float scalar)
{
	vector3d Ret;
	Ret.x = v.x / scalar;
	Ret.y = v.y / scalar;
	Ret.z = v.z / scalar;
	return Ret;
}


float Abs(float n)
{
	if (n < 0)
		n = -n;
	return n;
}

void ExpandBoundingBoxes(vector3d &max, vector3d &min, vector3d &cur)
{
	if (cur.x > max.x)
			max.x = cur.x;
	if (cur.y > max.y)
			max.y = cur.y;
	if (cur.z > max.z)
			max.z = cur.z;

	if (cur.x < min.x)
			min.x = cur.x;
	if (cur.y < min.y)
			min.y = cur.y;
	if (cur.z < min.z)
			min.z = cur.z;

}

float FindObjectRadius(vector3d &max, vector3d &min, vector3d &center)
{
	vector3d temp_vector;
		// Set Radius
	if (abs(max.x-center.x) >
		abs(center.x-min.x))
		temp_vector.x = abs(max.x-center.x);
	else
		temp_vector.x = abs(center.x-min.x);
	
	if (abs(max.y-center.y) >
		abs(center.y-min.y))
		temp_vector.y = abs(max.y-center.y);
	else
		temp_vector.y = abs(center.y-min.y);
	
	if (abs(max.z-center.z) >
		abs(center.z-min.z))
		temp_vector.z = abs(max.z-center.z);
	else
		temp_vector.z = abs(center.z-min.z);
	return Magnitude(temp_vector);
}



//returns distance of p from the line defined as starting at lp and 
//going in the direction of ln, is negitive if the point is behind 
//the start of the line
float point_line_distance(vector3d&p, vector3d&lp, vector3d&ln)
{

	ln = MakeUnitVector(ln);

	vector3d nearest;
	float comp;

	comp = dot(p-lp, ln);
	nearest = lp+ln*comp;

	if(comp < 0.0f)
		return -1.0f*Distance(nearest, p);
	else
		return Distance(nearest, p);

}


vector3d plane_line_intersect(vector3d plane_pt, vector3d plane_norm, vector3d line_pt, vector3d line_norm, bool*success){
	
	vector3d w = line_pt - plane_pt;
	
	float d = -dot(plane_norm,line_norm);

	if ( d == 0.0f ){
		if(success)*success = false;
		return vector3d(0,0,0);
	}
		if(success)*success = true;

	return line_pt+line_norm*dot(plane_norm,w)/d;
}

//returns the closest point on the given line
vector3d closest_point_on_line(vector3d p, vector3d lp, vector3d ln){
	ln = MakeUnitVector(ln);

	return lp+ln*dot(p-lp, ln);
}


float matrix_determinant_from_vectors(vector3d v1,vector3d v2,vector3d v3)
{
	float ans;
	ans =  v1.x * v2.y * v3.z;
	ans += v2.x * v3.y * v1.z;
	ans += v3.x * v1.y * v2.z;
	ans -= v1.z * v2.y * v3.x;
	ans -= v2.z * v3.y * v1.x;
	ans -= v3.z * v1.y * v2.x;

	return ans;
}

vector3d closest_point_between_lines(vector3d p1, vector3d v1, vector3d p2, vector3d v2)
{
	vector3d cross,delta;
	cross = CrossProduct(v1, v2);
	delta = p2 - p1;

	float denominator = Magnitude(cross);
	denominator*=denominator;
	float num_s, s;

	if (denominator > 1e-10) {
		num_s = matrix_determinant_from_vectors(delta, v2, cross);
		s = num_s / denominator;
		return p1 + v1*s;
	}else{
		return vector3d(0,0,0);
	}

}


#define delta 0.0001f
#define	UNINITIALIZED_VALUE	-1234567.8f
int ij_table[3][2] =        {
							{2,1},          //pos x biggest
							{0,2},          //pos y biggest
							{1,0},          //pos z biggest
						};

int point_face(vector3d *checkp, std::vector<vector3d> verts, vector3d * norm1)
{
	float *norm, *P;
	vector3d t;
	int i0, i1,i2;

	norm = (float *)norm1;

	//project polygon onto plane by finding largest component of normal
	t.x = fabsf(norm[0]); 
	t.y = fabsf(norm[1]); 
	t.z = fabsf(norm[2]);

	if (t.x > t.y) if (t.x > t.z) i0=0; else i0=2;
	else if (t.y > t.z) i0=1; else i0=2;

	if (norm[i0] > 0.0f) {
		i1 = ij_table[i0][0];
		i2 = ij_table[i0][1];
	}
	else {
		i1 = ij_table[i0][1];
		i2 = ij_table[i0][0];
	}

	// Have i0, i1, i2
	P = (float *)checkp;
	
	float u0, u1, u2, v0, v1, v2, alpha = UNINITIALIZED_VALUE;
	double beta;

	int inter=0, i = 2;	

	u0 = P[i1] - verts[0][i1];
	v0 = P[i2] - verts[0][i2];

	do {

		u1 = verts[i-1][i1] - verts[0][i1]; 
		u2 = verts[i  ][i1] - verts[0][i1];

		v1 = verts[i-1][i2] - verts[0][i2];
		v2 = verts[i  ][i2] - verts[0][i2];


		if ( (u1 >-delta) && (u1<delta) )	{
			beta = u0 / u2;
			if ((beta >=0.0f) && (beta<=1.0f))	{
				alpha = float((v0 - beta*v2)/v1);
				inter = ((alpha>=0.0f)&&(alpha+beta<=1.0f));
			}
		} else {

			beta = (v0*u1 - u0*v1) / (v2*u1 - u2*v1);
			if ((beta >=0.0f) && (beta<=1.0f))	{
				alpha = float((u0 - beta*u2)/u1);
				inter = ((alpha>=0.0f)&&(alpha+beta<=1.0f));
			}


		}

	} while ((!inter) && ((unsigned int)(++i) < verts.size()) );
	
	return inter;
}
