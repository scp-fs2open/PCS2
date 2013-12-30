#include <memory>
#include "matrix3d.h"
#include <pugixml.hpp>

#ifndef _DAEDataStructs_H_
#define _DAEDataStructs_H_



class DAEInput {
	public:

		DAEInput() {
			valid = false;
		}
		DAEInput(const DAEInput&) = delete;
		DAEInput& operator=(const DAEInput&) = delete;
		DAEInput(DAEInput&& other);
		DAEInput& operator=(DAEInput&& other);

		DAEInput(pugi::xml_node element);
		int x_offset();
		int y_offset();
		int z_offset();
		const std::vector<float> &values();
		int stride();
		bool is_valid();
		int u_offset();
		int v_offset();
	private:
		int x,y,z, strides,u,v;
		std::vector<float> value;
		bool valid;
};

class DAEInputs {
public:
	DAEInputs(pugi::xml_node& element);
	virtual ~DAEInputs() {}
	int max_offset;
	int pos_offset;
	int norm_offset;
	int uv_offset;
	DAEInput pos, norm, uv;
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
