/***************************************************************************
                          matrix.h  -  matrix mathmatics module
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


#if !defined(__matrix__h_)
#define __matrix__h_

#if defined(WIN32)
#include <windows.h>
#include <wx/msw/winundef.h>
#pragma warning ( disable : 4786 )
#endif

#include "vector3d.h"
#include <memory.h>
#include <math.h>
#include <stack>

// as pointer to this _CAN_ be cast to _D3DMATRIX*
// this can be memcpyed over a D3DXMATRIX 
struct m4x4
{
  float vals[4][4];
  m4x4() { memset(vals, 0, sizeof(float)*16); }

  m4x4 operator*(const m4x4 rhv) const;
  void operator=(const m4x4 rhv) { memcpy(vals, &rhv, sizeof(m4x4)); }

  void convert_to_d3d();
  void convert_from_d3d();
};

class matrix_4x4
{
  private:
    m4x4 myMatrix;
	std::stack<m4x4> matrixStack;
  public:
    matrix_4x4()  // sets up the matrix as an identity matrix
      { myMatrix.vals[0][0] = myMatrix.vals[1][1] = 1;
        myMatrix.vals[2][2] = myMatrix.vals[3][3] = 1; }

    matrix_4x4(m4x4 val)
      { memcpy(&myMatrix, &val, sizeof(m4x4)); }


    void LoadIdentity()
      {
        memset(&myMatrix, 0, sizeof(m4x4));
        myMatrix.vals[0][0] = myMatrix.vals[1][1] = 1;
        myMatrix.vals[2][2] = myMatrix.vals[3][3] = 1;
      }
    // vector and matrix arithmetic
    vector3d operator*(const vector3d &point) const;
    vector3d multiply_rot(const vector3d &point) const;
	//same as above exept it has the fourth element as 0 rather than 1
	//thus only takes rotation into account

    // matrix arithmetic
    matrix_4x4 operator+(const matrix_4x4 rhv) const;
    matrix_4x4 operator*(const matrix_4x4 rhv) const;

    // accessors
    const float* operator[](int n) const
      { return myMatrix.vals[n]; }

	float&operator()(int x, int y){
		return myMatrix.vals[x][y];
	}

    const m4x4 MyMatrix() const
      { return myMatrix; }

    //mutliplies the current matrix by a matrix produced by the named operation
    void RotateX(float xr);
    void RotateY(float yr);
    void RotateZ(float zr);
    void Rotate(vector3d rotation);
    void Scale(float sz);
    void Scale(vector3d scale);
    void Translate(vector3d trans);

    // matrix stack
    void PushMatrix()
      { matrixStack.push(myMatrix); }

    void PopMatrix()
      { myMatrix = matrixStack.top(); matrixStack.pop(); }


	vector3d&rvec(){
		return *((vector3d*)myMatrix.vals[0]);
	}
	vector3d&uvec(){
		return *((vector3d*)myMatrix.vals[1]);
	}
	vector3d&fvec(){
		return *((vector3d*)myMatrix.vals[2]);
	}
	vector3d&pos(){
		return *((vector3d*)myMatrix.vals[3]);
	}

	void Invert();

};

#endif
