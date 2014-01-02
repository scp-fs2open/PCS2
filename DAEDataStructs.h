#include <memory>
#include "matrix3d.h"
#include <pugixml.hpp>

#ifndef _DAEDataStructs_H_
#define _DAEDataStructs_H_


class DAEVectorInput {
public:

	DAEVectorInput() {
		valid = false;
	}
	DAEVectorInput(const DAEVectorInput&) = delete;
	DAEVectorInput& operator=(const DAEVectorInput&) = delete;
	DAEVectorInput(DAEVectorInput&& other);
	DAEVectorInput& operator=(DAEVectorInput&& other);

	DAEVectorInput(pugi::xml_node element);
	bool is_valid() const;
	vector3d operator[](size_t index) const;
private:
	int x, y, z, strides;
	std::vector<float> values;
	bool valid;
};

class DAEUvInput {
public:

	DAEUvInput() {
		valid = false;
	}
	DAEUvInput(const DAEUvInput&) = delete;
	DAEUvInput& operator=(const DAEUvInput&) = delete;
	DAEUvInput(DAEUvInput&& other);
	DAEUvInput& operator=(DAEUvInput&& other);

	DAEUvInput(pugi::xml_node element);
	bool is_valid() const;
	std::pair<float, float> operator[](size_t index) const;
private:
	int u, v, strides;
	std::vector<float> values;
	bool valid;
};

class DAEInputs {
public:
	DAEInputs(pugi::xml_node& element);
	virtual ~DAEInputs() {}

	vector3d position(size_t index) const;
	vector3d normal(size_t index) const;
	std::pair<float, float> uv(size_t index) const;

private:
	int max_offset;
	int pos_offset;
	int norm_offset;
	int uv_offset;
	DAEVectorInput pos;
	DAEVectorInput norm;
	DAEUvInput uvs;
	std::vector<int> refs;
};

// parses ints into a vector
void parse_int_array(const char* chars, std::vector<int> *result, unsigned int count = -1);
// parses floats into a vector
std::vector<float> parse_float_array(const char* chars, unsigned int count = -1);
std::string write_int_array(const std::vector<int>& vec);
std::string write_float_array(const std::vector<float>& vec);
std::string write_vector3d(vector3d vec,vector3d scale = vector3d(1,1,1));
vector3d absolute_to_relative(vector3d vec, pcs_sobj *subobj, std::vector<pcs_sobj*> *subobjs);
std::string int_to_string(int i);
void add_accessor(pugi::xml_node element, std::string source, int count, bool uv = false);
void add_param(pugi::xml_node accessor, std::string name);
std::string strip_texture(std::string path);
bool vector3d_comparator(const vector3d& a, const vector3d& b);
bool float_pair_comparator(const std::pair<float,float>& a, const std::pair<float,float>& b);
vector3d radius_to_scale(float radius);
void add_scale(pugi::xml_node element, vector3d scale, vector3d parent_scale = vector3d(1,1,1));
void filter_string(std::string& base, const std::string& property);
void trim_extra_spaces(std::string& s);
std::string get_name(const pugi::xml_node& element);

#endif
