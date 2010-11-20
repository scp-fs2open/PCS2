#include "ogl_vertex_buffers.h"
#include "pcs_file.h"
#include "pcs_file_dstructs.h"
#include <GL/glu.h>

//sets up all vertex buffers for this model
void PCS_Model::make_vertex_buffers(bool enabled){
//	return;
	vbos_enabled = enabled;
	if (vbos_enabled) {
		for(unsigned int i = 0; i<subobjects.size(); i++){
			make_vertex_buffer(i);
		}
	}
}

void PCS_Model::make_vertex_buffer(int sobj){
	subobjects[sobj].destroy_vertex_buffer();//clear out any exsisting ones
	subobjects[sobj].vertex_buffer.resize(textures.size());
	subobjects[sobj].line_vertex_buffer.resize(textures.size());
	for(unsigned int j = 0; j <textures.size(); j++){
		subobjects[sobj].make_vertex_buffer(j);
	}
}

struct OGL_vert{
	vector3d pos;
	vector3d norm;
	float u,v;
};

//set's up a specific vertex buffer
void pcs_sobj::make_vertex_buffer(int tid){

	std::vector<OGL_vert> tri;
	std::vector<vector3d> line;
	std::vector<OGL_vert> poly;

	for(unsigned int i = 0; i<polygons.size(); i++){
		if(polygons[i].texture_id != tid)continue;

		OGL_vert vert;

		poly.resize(polygons[i].verts.size());

		for(unsigned int v = 0; v<polygons[i].verts.size(); v++){
			vert.pos = polygons[i].verts[v].point;
			vert.norm = polygons[i].verts[v].norm;
			vert.u = polygons[i].verts[v].u;
			vert.v = polygons[i].verts[v].v;
			poly[v] = vert;
		}
		line.push_back(poly[0].pos);
		line.push_back(poly[1].pos);
		for(unsigned int t = 1; t<polygons[i].verts.size()-1; t++){
			tri.push_back(poly[0]);
			tri.push_back(poly[t]);
			tri.push_back(poly[t+1]);

			line.push_back(poly[t].pos);
			line.push_back(poly[t+1].pos);
		}
		line.push_back(poly[polygons[i].verts.size()-1].pos);
		line.push_back(poly[0].pos);
	}
	vertex_buffer[tid].n_verts = (int)tri.size();
	vertex_buffer[tid].vertex_size = sizeof(OGL_vert);
	vertex_buffer[tid].buffer = 0;
	vertex_buffer[tid].format = GL_T2F_N3F_V3F;

	line_vertex_buffer[tid].n_verts = (int)line.size();
	line_vertex_buffer[tid].vertex_size = sizeof(vector3d);
	line_vertex_buffer[tid].buffer = 0;
	line_vertex_buffer[tid].format = GL_V3F;
	if(tri.size() <1)return;

//	glSetStrideSize(vertex_buffer[tid].vertex_size);
	glGenBuffersARB(1, &vertex_buffer[tid].buffer);
	ERROR_CHECK;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertex_buffer[tid].buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, tri.size()*sizeof(OGL_vert), &tri[0], GL_STATIC_DRAW_ARB);
	//float*map = (float *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	ERROR_CHECK;
	//memcpy(map, &tri[0], tri.size()*sizeof(OGL_vert));
 	//glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
 
	glGenBuffersARB(1, &line_vertex_buffer[tid].buffer);
	ERROR_CHECK;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, line_vertex_buffer[tid].buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, line.size()*sizeof(vector3d), &line[0], GL_STATIC_DRAW_ARB);
	//map = (float *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	ERROR_CHECK;
	//memcpy(map, &line[0], line.size()*sizeof(vector3d));
 	//glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
 
}


void pcs_sobj::destroy_vertex_buffer(){
	for(unsigned int i = 0; i<vertex_buffer.size(); i++){
		if(vertex_buffer[i].buffer)
		{
			glDeleteBuffersARB(1, &vertex_buffer[i].buffer);
			vertex_buffer[i].buffer = 0;
		}
		if(line_vertex_buffer[i].buffer)
		{
			glDeleteBuffersARB(1, &line_vertex_buffer[i].buffer);
			line_vertex_buffer[i].buffer = 0;
		}
	}
}
