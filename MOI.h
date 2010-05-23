#pragma once
#include "vector3d.h"
#include "pcs_file.h"

union MOI{
	double a2d[3][3];
	double a1d[9];

	MOI&operator = (const MOI&b){
		for(int i =0; i<9; i++){
			a1d[i] = b.a1d[i];
		}
		return *this;
	}
	void negate(){
		for(int i =0; i<9; i++){
			a1d[i] = -a1d[i];
		}
	}
	void invert();
};



inline MOI operator + (const MOI&a, const MOI&b){
	MOI ret;
	for(int i =0; i<9; i++){
		ret.a1d[i] = a.a1d[i] + b.a1d[i];
	}
	return ret;
}

inline MOI operator - (const MOI&a, const MOI&b){
	MOI ret;
	for(int i =0; i<9; i++){
		ret.a1d[i] = a.a1d[i] - b.a1d[i];
	}
	return ret;
}

inline MOI operator * (const MOI&a, const float b){
	MOI ret;
	for(int i =0; i<9; i++){
		ret.a1d[i] = a.a1d[i] * b;
	}
	return ret;
}

inline MOI operator / (const MOI&a, const float b){
	MOI ret;
	for(int i =0; i<9; i++){
		ret.a1d[i] = a.a1d[i] / b;
	}
	return ret;
}


inline MOI&operator += (MOI&a, const MOI&b){
	return a = a+b;
}

inline MOI&operator -= (MOI&a, const MOI&b){
	return a = a-b;
}

MOI calc_geometry_MOI(PCS_Model&model);