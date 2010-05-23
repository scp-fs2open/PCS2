/***************************************************************************
                          matrix.cpp  -  matrix mathmatics module
                             -------------------
    begin                : Sun Nov 3 2002
    copyright            : (C) 2002 by Derek Meek
    email                : kazan@cs.iastate.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#if defined(_WIN32)
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "matrix.h"



void m4x4::convert_to_d3d()
{
}

void m4x4::convert_from_d3d()
{

}


vector3d matrix_4x4::operator*(const vector3d &point) const
{
  vector3d retval;
  //float Sanity;
  // a little matrix algebra here.. t
  /*
  [x][R0-0 R0-1 R0-2 0]
  [y][R1-0 R1-1 R2-2 0]
  [z][R2-0 R2-1 R2-2 0]
  [1][R3-0 R3-1 R3-2 1]

  x = x(R0-0) + y(R1-0) + z(R2-0) + (R3-0)
  y = x(R0-1) + y(R1-1) + z(R2-1) + (R3-1)
  z = x(R0-2) + y(R1-2) + z(R2-2) + (R3-2)
  1 = x(R0-3) + y(R3-3) + z(R2-3) + (R3-3)

  */
  //cout << point << endl;
  //cout << "[ " << myMatrix.vals[0][0] << " " << myMatrix.vals[1][0] << " "
  //     << myMatrix.vals[2][0] << " " << myMatrix.vals[3][0] << " ]" << endl;
  retval.x = ((point.x * myMatrix.vals[0][0])
           + (point.y * myMatrix.vals[1][0])
           + (point.z * myMatrix.vals[2][0])
           + myMatrix.vals[3][0]);

  retval.y = ((point.x * myMatrix.vals[0][1])
           + (point.y * myMatrix.vals[1][1])
           + (point.z * myMatrix.vals[2][1])
           + myMatrix.vals[3][1]);

  retval.z = ((point.x * myMatrix.vals[0][2])
           + (point.y * myMatrix.vals[1][2])
           + (point.z * myMatrix.vals[2][2])
           + myMatrix.vals[3][2]);

  // do the identity row for a sanity check
  /*Sanity = (tempvec.x) * myMatrix[0][3])
           + (tempvec.y) * myMatrix[1][3])
           + (tempvec.z) * myMatrix[2][3])
           + myMatrix[3][3];

  if (Sanity != 1)
    cout << "Sanity check failed on vector*matrix multiplication!" << endl;   */
  //cout << retval << endl;
  return retval;
}

vector3d matrix_4x4::multiply_rot(const vector3d &point) const
{
  vector3d retval;
  retval.x = ((point.x * myMatrix.vals[0][0])
           + (point.y * myMatrix.vals[1][0])
           + (point.z * myMatrix.vals[2][0]));

  retval.y = ((point.x * myMatrix.vals[0][1])
           + (point.y * myMatrix.vals[1][1])
           + (point.z * myMatrix.vals[2][1]));

  retval.z = ((point.x * myMatrix.vals[0][2])
           + (point.y * myMatrix.vals[1][2])
           + (point.z * myMatrix.vals[2][2]));

  return retval;
}


//****************************************************************************

m4x4 m4x4::operator*(const m4x4 rhv) const
{
  m4x4 outmatrix;

  /* matrix multiplication general formula
                     n
                    ---
          C(r,c) =  \     A(r,i) B(i,c)
                    /
                    ---
                    i=1

  */

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      outmatrix.vals[i][j] = vals[i][0]*rhv.vals[0][j]
                           + vals[i][1]*rhv.vals[1][j]
                           + vals[i][2]*rhv.vals[2][j]
                           + vals[i][3]*rhv.vals[3][j];
    }
  }

  return outmatrix;
}

matrix_4x4 matrix_4x4::operator*(const matrix_4x4 rhv) const
{
  return matrix_4x4(myMatrix * rhv.MyMatrix());
}

//****************************************************************************

void matrix_4x4::RotateX(float xr)
/*Matrix for a 3D rotation around the x axis of x

1     0       0       0
0    cos(x)  sin(x)   0
0   -sin(x)  cos(x)   0
0      0       0      1
*/
{
  m4x4 rotm;
  rotm.vals[0][0] = rotm.vals[3][3] = 1;
  rotm.vals[1][1] = cos(xr);
  rotm.vals[1][2] = sin(xr);

  rotm.vals[2][1] = -sin(xr);
  rotm.vals[2][2] = cos(xr);

  myMatrix = myMatrix * rotm;
}

void matrix_4x4::RotateY(float yr)
/* Matrix for a 3D rotation around the y axis of y

cos(y)   0   -sin(y)   0
0        1      0      0
sin(y)   0   cos(y)    0
0        0      0      1*/
{
  m4x4 rotm;
  rotm.vals[1][1] = rotm.vals[3][3] = 1;
  rotm.vals[0][0] = cos(yr);
  rotm.vals[0][2] = -sin(yr);

  rotm.vals[2][0] = sin(yr);
  rotm.vals[2][2] = cos(yr);

  myMatrix = myMatrix * rotm;
}

void matrix_4x4::RotateZ(float zr)
/*Matrix for a 3D rotation around the z axis of z
cos(z)  sin(z)  0     0
-sin(z) cos(z)  0     0
0       0       1     0
0       0       0     1

*/
{
  m4x4 rotm;
  rotm.vals[3][3] = rotm.vals[2][2] = 1;
  rotm.vals[0][0] = cos(zr);
  rotm.vals[0][1] = sin(zr);

  rotm.vals[1][0] = -sin(zr);
  rotm.vals[1][1] = cos(zr);

  myMatrix = myMatrix * rotm;
}


void matrix_4x4::Rotate(vector3d rotation)
{
  RotateX(rotation.x);
  RotateY(rotation.y);
  RotateZ(rotation.z);
}

//****************************************************************************

void matrix_4x4::Scale(float sz)
{
  m4x4 transm;
  transm.vals[3][3] = 1;
  transm.vals[0][0] = transm.vals[1][1] = transm.vals[2][2] = sz;
  myMatrix = myMatrix * transm;
}


void matrix_4x4::Scale(vector3d scale)
{
  m4x4 transm;
  transm.vals[0][0] = scale.x;
  transm.vals[1][1] = scale.y;
  transm.vals[2][2] = scale.z;
  transm.vals[3][3] = 1;
  myMatrix = myMatrix * transm;

}

//****************************************************************************

void matrix_4x4::Translate(vector3d trans)
/*
1 0 0 0
0 1 0 0
0 0 1 0
x y z 1
*/
{
  m4x4 transm;
  transm.vals[0][0] = transm.vals[1][1] = transm.vals[2][2] = transm.vals[3][3] = 1;
  transm.vals[3][0] = trans.x;
  transm.vals[3][1] = trans.y;
  transm.vals[3][2] = trans.z;

  myMatrix = myMatrix * transm;
}



void matrix_4x4::Invert(){
	m4x4 temp;

	temp.vals[0][0] = (-myMatrix.vals[1][3]*myMatrix.vals[2][2]*myMatrix.vals[3][1]+myMatrix.vals[1][2]*myMatrix.vals[2][3]*myMatrix.vals[3][1]+myMatrix.vals[1][3]*myMatrix.vals[2][1]*myMatrix.vals[3][2]-myMatrix.vals[1][1]*myMatrix.vals[2][3]*myMatrix.vals[3][2]-myMatrix.vals[1][2]*myMatrix.vals[2][1]*myMatrix.vals[3][3]+myMatrix.vals[1][1]*myMatrix.vals[2][2]*myMatrix.vals[3][3]);
	temp.vals[0][1] = (myMatrix.vals[0][3]*myMatrix.vals[2][2]*myMatrix.vals[3][1]-myMatrix.vals[0][2]*myMatrix.vals[2][3]*myMatrix.vals[3][1]-myMatrix.vals[0][3]*myMatrix.vals[2][1]*myMatrix.vals[3][2]+myMatrix.vals[0][1]*myMatrix.vals[2][3]*myMatrix.vals[3][2]+myMatrix.vals[0][2]*myMatrix.vals[2][1]*myMatrix.vals[3][3]-myMatrix.vals[0][1]*myMatrix.vals[2][2]*myMatrix.vals[3][3]);
	temp.vals[0][2] = (-myMatrix.vals[0][3]*myMatrix.vals[1][2]*myMatrix.vals[3][1]+myMatrix.vals[0][2]*myMatrix.vals[1][3]*myMatrix.vals[3][1]+myMatrix.vals[0][3]*myMatrix.vals[1][1]*myMatrix.vals[3][2]-myMatrix.vals[0][1]*myMatrix.vals[1][3]*myMatrix.vals[3][2]-myMatrix.vals[0][2]*myMatrix.vals[1][1]*myMatrix.vals[3][3]+myMatrix.vals[0][1]*myMatrix.vals[1][2]*myMatrix.vals[3][3]);
	temp.vals[0][3] = (myMatrix.vals[0][3]*myMatrix.vals[1][2]*myMatrix.vals[2][1]-myMatrix.vals[0][2]*myMatrix.vals[1][3]*myMatrix.vals[2][1]-myMatrix.vals[0][3]*myMatrix.vals[1][1]*myMatrix.vals[2][2]+myMatrix.vals[0][1]*myMatrix.vals[1][3]*myMatrix.vals[2][2]+myMatrix.vals[0][2]*myMatrix.vals[1][1]*myMatrix.vals[2][3]-myMatrix.vals[0][1]*myMatrix.vals[1][2]*myMatrix.vals[2][3]);
	
	temp.vals[1][0] = (myMatrix.vals[1][3]*myMatrix.vals[2][2]*myMatrix.vals[3][0]-myMatrix.vals[1][2]*myMatrix.vals[2][3]*myMatrix.vals[3][0]-myMatrix.vals[1][3]*myMatrix.vals[2][0]*myMatrix.vals[3][2]+myMatrix.vals[1][0]*myMatrix.vals[2][3]*myMatrix.vals[3][2]+myMatrix.vals[1][2]*myMatrix.vals[2][0]*myMatrix.vals[3][3]-myMatrix.vals[1][0]*myMatrix.vals[2][2]*myMatrix.vals[3][3]);
	temp.vals[1][1] = (-myMatrix.vals[0][3]*myMatrix.vals[2][2]*myMatrix.vals[3][0]+myMatrix.vals[0][2]*myMatrix.vals[2][3]*myMatrix.vals[3][0]+myMatrix.vals[0][3]*myMatrix.vals[2][0]*myMatrix.vals[3][2]-myMatrix.vals[0][0]*myMatrix.vals[2][3]*myMatrix.vals[3][2]-myMatrix.vals[0][2]*myMatrix.vals[2][0]*myMatrix.vals[3][3]+myMatrix.vals[0][0]*myMatrix.vals[2][2]*myMatrix.vals[3][3]);
	temp.vals[1][2] = (myMatrix.vals[0][3]*myMatrix.vals[1][2]*myMatrix.vals[3][0]-myMatrix.vals[0][2]*myMatrix.vals[1][3]*myMatrix.vals[3][0]-myMatrix.vals[0][3]*myMatrix.vals[1][0]*myMatrix.vals[3][2]+myMatrix.vals[0][0]*myMatrix.vals[1][3]*myMatrix.vals[3][2]+myMatrix.vals[0][2]*myMatrix.vals[1][0]*myMatrix.vals[3][3]-myMatrix.vals[0][0]*myMatrix.vals[1][2]*myMatrix.vals[3][3]);
	temp.vals[1][3] = (-myMatrix.vals[0][3]*myMatrix.vals[1][2]*myMatrix.vals[2][0]+myMatrix.vals[0][2]*myMatrix.vals[1][3]*myMatrix.vals[2][0]+myMatrix.vals[0][3]*myMatrix.vals[1][0]*myMatrix.vals[2][2]-myMatrix.vals[0][0]*myMatrix.vals[1][3]*myMatrix.vals[2][2]-myMatrix.vals[0][2]*myMatrix.vals[1][0]*myMatrix.vals[2][3]+myMatrix.vals[0][0]*myMatrix.vals[1][2]*myMatrix.vals[2][3]);
	
	temp.vals[2][0] = (-myMatrix.vals[1][3]*myMatrix.vals[2][1]*myMatrix.vals[3][0]+myMatrix.vals[1][1]*myMatrix.vals[2][3]*myMatrix.vals[3][0]+myMatrix.vals[1][3]*myMatrix.vals[2][0]*myMatrix.vals[3][1]-myMatrix.vals[1][0]*myMatrix.vals[2][3]*myMatrix.vals[3][1]-myMatrix.vals[1][1]*myMatrix.vals[2][0]*myMatrix.vals[3][3]+myMatrix.vals[1][0]*myMatrix.vals[2][1]*myMatrix.vals[3][3]);
	temp.vals[2][1] = (myMatrix.vals[0][3]*myMatrix.vals[2][1]*myMatrix.vals[3][0]-myMatrix.vals[0][1]*myMatrix.vals[2][3]*myMatrix.vals[3][0]-myMatrix.vals[0][3]*myMatrix.vals[2][0]*myMatrix.vals[3][1]+myMatrix.vals[0][0]*myMatrix.vals[2][3]*myMatrix.vals[3][1]+myMatrix.vals[0][1]*myMatrix.vals[2][0]*myMatrix.vals[3][3]-myMatrix.vals[0][0]*myMatrix.vals[2][1]*myMatrix.vals[3][3]);
	temp.vals[2][2] = (-myMatrix.vals[0][3]*myMatrix.vals[1][1]*myMatrix.vals[3][0]+myMatrix.vals[0][1]*myMatrix.vals[1][3]*myMatrix.vals[3][0]+myMatrix.vals[0][3]*myMatrix.vals[1][0]*myMatrix.vals[3][1]-myMatrix.vals[0][0]*myMatrix.vals[1][3]*myMatrix.vals[3][1]-myMatrix.vals[0][1]*myMatrix.vals[1][0]*myMatrix.vals[3][3]+myMatrix.vals[0][0]*myMatrix.vals[1][1]*myMatrix.vals[3][3]);
	temp.vals[2][3] = (myMatrix.vals[0][3]*myMatrix.vals[1][1]*myMatrix.vals[2][0]-myMatrix.vals[0][1]*myMatrix.vals[1][3]*myMatrix.vals[2][0]-myMatrix.vals[0][3]*myMatrix.vals[1][0]*myMatrix.vals[2][1]+myMatrix.vals[0][0]*myMatrix.vals[1][3]*myMatrix.vals[2][1]+myMatrix.vals[0][1]*myMatrix.vals[1][0]*myMatrix.vals[2][3]-myMatrix.vals[0][0]*myMatrix.vals[1][1]*myMatrix.vals[2][3]);
	
	temp.vals[3][0] = (myMatrix.vals[1][2]*myMatrix.vals[2][1]*myMatrix.vals[3][0]-myMatrix.vals[1][1]*myMatrix.vals[2][2]*myMatrix.vals[3][0]-myMatrix.vals[1][2]*myMatrix.vals[2][0]*myMatrix.vals[3][1]+myMatrix.vals[1][0]*myMatrix.vals[2][2]*myMatrix.vals[3][1]+myMatrix.vals[1][1]*myMatrix.vals[2][0]*myMatrix.vals[3][2]-myMatrix.vals[1][0]*myMatrix.vals[2][1]*myMatrix.vals[3][2]);
	temp.vals[3][1] = (-myMatrix.vals[0][2]*myMatrix.vals[2][1]*myMatrix.vals[3][0]+myMatrix.vals[0][1]*myMatrix.vals[2][2]*myMatrix.vals[3][0]+myMatrix.vals[0][2]*myMatrix.vals[2][0]*myMatrix.vals[3][1]-myMatrix.vals[0][0]*myMatrix.vals[2][2]*myMatrix.vals[3][1]-myMatrix.vals[0][1]*myMatrix.vals[2][0]*myMatrix.vals[3][2]+myMatrix.vals[0][0]*myMatrix.vals[2][1]*myMatrix.vals[3][2]);
	temp.vals[3][2] = (myMatrix.vals[0][2]*myMatrix.vals[1][1]*myMatrix.vals[3][0]-myMatrix.vals[0][1]*myMatrix.vals[1][2]*myMatrix.vals[3][0]-myMatrix.vals[0][2]*myMatrix.vals[1][0]*myMatrix.vals[3][1]+myMatrix.vals[0][0]*myMatrix.vals[1][2]*myMatrix.vals[3][1]+myMatrix.vals[0][1]*myMatrix.vals[1][0]*myMatrix.vals[3][2]-myMatrix.vals[0][0]*myMatrix.vals[1][1]*myMatrix.vals[3][2]);
	temp.vals[3][3] = (-myMatrix.vals[0][2]*myMatrix.vals[1][1]*myMatrix.vals[2][0]+myMatrix.vals[0][1]*myMatrix.vals[1][2]*myMatrix.vals[2][0]+myMatrix.vals[0][2]*myMatrix.vals[1][0]*myMatrix.vals[2][1]-myMatrix.vals[0][0]*myMatrix.vals[1][2]*myMatrix.vals[2][1]-myMatrix.vals[0][1]*myMatrix.vals[1][0]*myMatrix.vals[2][2]+myMatrix.vals[0][0]*myMatrix.vals[1][1]*myMatrix.vals[2][2]);

	myMatrix = temp;
}

