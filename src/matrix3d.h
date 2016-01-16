#include <ios>
#include <vector>
#ifndef _MATRIX3D_H_
#define _MATRIX3D_H_
#include "vector3d.h"
#include <sstream>

#define MATRIX_SIZE 3

std::string write_vector3d(vector3d vec,vector3d scale);

struct matrix {
	float a2d[MATRIX_SIZE][MATRIX_SIZE];

	matrix();

	// Construct a change of basis from e1,e2,e3 to the given vector
	matrix(vector3d basis);

	// Construct a rotation matrix from a given rotation angle
	matrix(float angle);

	matrix(std::vector<float> *matrix);

	matrix inverse();

	std::string tostring() const;

	float scale();

	matrix(float f[3][3]);

	matrix(vector3d r, vector3d u, vector3d f);

	matrix(const matrix&m);

	matrix operator=(const matrix&m);



	matrix operator * (const float&f);

	matrix operator + (const matrix&m);

	matrix invert();

	float determinant() const;

};

vector3d operator *(matrix matrix, vector3d vec);
matrix operator %(matrix one, matrix two);

#endif /* _MATRIX3D_H_ */
