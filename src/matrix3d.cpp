#include "matrix3d.h"
#include <sstream>
#include <cassert>
#include <boost/math/constants/constants.hpp>


matrix::matrix(void) {
	memset(a2d,0,sizeof(a2d));
	for (int i = 0; i < MATRIX_SIZE; i++) {
		a2d[i][i] = 1.0;
	}
}

// Construct a change of basis from e1,e2,e3 to the given vector
matrix::matrix(vector3d basis) {
	basis = MakeUnitVector(basis);
	for (int j = 0; j < MATRIX_SIZE; j++) {
		a2d[0][j] = basis[j];
	}
	vector3d temp (1,0,0);
	temp = temp - dot(temp,basis) / dot(basis,basis) * basis;
	if (temp == vector3d(0,0,0)) {
		temp = vector3d(0,1,0);
		// shouldn't be necessary...
		temp = temp - dot(temp,basis) / dot(basis,basis) * basis;
	}
	temp = MakeUnitVector(temp);
	for (int j = 0; j < MATRIX_SIZE; j++) {
		a2d[1][j] = temp[j];
	}
	temp = MakeUnitVector(CrossProduct(basis,temp));
	for (int j = 0; j < MATRIX_SIZE; j++) {
		a2d[2][j] = temp[j];
	}
}

// Construct a rotation matrix from a given rotation angle
matrix::matrix(float angle) {
	memset(a2d,0,sizeof(a2d));
	if (std::fabs(angle - std::round(angle)) < 10e-5 && ((int)std::round(angle) % 90) == 0) {
		int quarters = ((((int)round(angle)) / 90) + 4) % 4;
		switch(quarters) {
			case 0:
				a2d[1][1] = 1;
				break;
			case 1:
				a2d[2][1] = 1;
				break;
			case 2:
				a2d[1][1] = -1;
				break;
			case 3:
				a2d[2][1] = -1;
				break;
		}
	} else {
		angle = angle * boost::math::constants::pi<float>() / 180;
		a2d[1][1] = cos(angle);
		a2d[2][1] = sin(angle);
	}
	a2d[2][2] = a2d[1][1];
	a2d[1][2] = -a2d[2][1];
	a2d[0][0] = 1.0f;
}


matrix::matrix(std::vector<float> *values) {
	assert(values->size() >= 16);
	int k = 0;
	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < MATRIX_SIZE; j++, k++) {
			a2d[i][j] = (*values)[k];
		}
		k++;
	}
}

std::string matrix::tostring() const {
	std::stringstream logger;
	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < MATRIX_SIZE; j++) {
			logger << a2d[i][j] << " ";
		}
		logger << std::endl;
	}
	return logger.str();
}

vector3d operator *(matrix matrix, vector3d vec) {
	vector3d result;
	for (int i = 0; i < MATRIX_SIZE; i++) {
		result[i] = 0;
		for (int j = 0; j < MATRIX_SIZE; j++) {
			result[i] += matrix.a2d[i][j] * vec[j];
		}
	}
	return result;
}

matrix operator %(matrix one, matrix two) {
	matrix result;
	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < MATRIX_SIZE; j++) {
			result.a2d[i][j] = 0;
			for (int k = 0; k < MATRIX_SIZE; k++) {
				result.a2d[i][j] += one.a2d[i][k] * two.a2d[k][j];
			}
		}
	}
	return result;
}

float matrix::scale() {
	float sum = 0;
	vector3d zero;
	vector3d vec(1, 0, 0);
	vec = *this * vec;
	sum += Distance(zero, vec);
	vec = vector3d(0, 1, 0);
	vec = *this * vec;
	sum += Distance(zero, vec);
	vec = vector3d(0, 0, 1);
	vec = *this * vec;
	sum += Distance(zero, vec);
	return sum / 3;
}

matrix::matrix(float f[3][3]){
	memcpy(a2d,f,sizeof(float)*9);
}
matrix::matrix(vector3d r, vector3d u, vector3d f){
	memcpy(a2d,&r,sizeof(float)*3);
	memcpy(a2d[1],&u,sizeof(float)*3);
	memcpy(a2d[2],&f,sizeof(float)*3);
}
matrix::matrix(const matrix&m){
	memcpy(a2d,&m,sizeof(float)*9);
}
matrix matrix::operator=(const matrix&m){
	memcpy(a2d,&m,sizeof(float)*9);
	return (*this);
}


matrix matrix::operator * (const float&f){
	return matrix(vector3d(a2d[0][0]*f, a2d[0][1]*f, a2d[0][2]*f),
		vector3d(a2d[1][0]*f, a2d[1][1]*f, a2d[1][2]*f),
		vector3d(a2d[2][0]*f, a2d[2][1]*f, a2d[2][2]*f)
		);
}

matrix matrix::operator + (const matrix&m){
	return matrix(vector3d(a2d[0][0]+m.a2d[0][0], a2d[0][1]+m.a2d[0][1], a2d[0][2]+m.a2d[0][2]),
		vector3d(a2d[1][0]+m.a2d[1][0], a2d[1][1]+m.a2d[1][1], a2d[1][2]+m.a2d[1][2]),
		vector3d(a2d[2][0]+m.a2d[2][0], a2d[2][1]+m.a2d[2][1], a2d[2][2]+m.a2d[2][2])
		);
}
matrix matrix::invert(){
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

float matrix::determinant() const {
	float result = 0.0f;
	for (int i = 0; i < 3; i++) {
		result += a2d[0][i] * (a2d[1][(i+1) % 3] * a2d[2][(i+2) % 3] - a2d[1][(i+2) % 3] * a2d[2][(i+1) % 3]);
	}
	return result;
}
