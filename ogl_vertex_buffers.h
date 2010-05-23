#pragma once
#include "GLee.h"


struct ogl_vertex_buffer
{
	unsigned int n_verts;
	unsigned int vertex_size;
	GLenum format;
	GLuint buffer;
};