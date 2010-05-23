
#include"geo_sphere.h"
#include <GL/glu.h>

void geo_sphere::make_isohedron(){

	gspoints.resize(12);
	gstris.resize(20);

	int i = 0;

	//points
	gspoints[i++] = vector3d(0, GS1, GSP);//0
	gspoints[i++] = vector3d(0, -GS1, GSP);
	gspoints[i++] = vector3d(0, GS1, -GSP);
	gspoints[i++] = vector3d(0, -GS1, -GSP);

	gspoints[i++] = vector3d(GS1, GSP, 0);//4
	gspoints[i++] = vector3d(-GS1, GSP, 0);
	gspoints[i++] = vector3d(GS1, -GSP, 0);
	gspoints[i++] = vector3d(-GS1, -GSP, 0);

	gspoints[i++] = vector3d(GSP, 0, GS1);//8
	gspoints[i++] = vector3d(GSP, 0, -GS1);
	gspoints[i++] = vector3d(-GSP, 0, GS1);
	gspoints[i++] = vector3d(-GSP, 0, -GS1);
	//points done

	i=0;
	//tris
	//yes this was basicly done by hand...
	gstris[i++] = stri(0,1,10);
	gstris[i++] = stri(0,10,5);
	gstris[i++] = stri(0,5,4);
	gstris[i++] = stri(0,4,8);
	gstris[i++] = stri(0,8,1);

	gstris[i++] = stri(1,8,6);
	gstris[i++] = stri(1,6,7);
	gstris[i++] = stri(1,7,10);

	gstris[i++] = stri(11,10,7);
	gstris[i++] = stri(11,7,3);
	gstris[i++] = stri(11,3,2);
	gstris[i++] = stri(11,2,5);
	gstris[i++] = stri(11,5,10);

	gstris[i++] = stri(7,6,3);

	gstris[i++] = stri(5,2,4);

	gstris[i++] = stri(9,4,2);
	gstris[i++] = stri(9,2,3);
	gstris[i++] = stri(9,3,6);
	gstris[i++] = stri(9,6,8);
	gstris[i++] = stri(9,8,4);
	//done tris

}

//adds the vector to the gspoints if it isn't there, 
//returns the location were it was added or found
size_t geo_sphere::add_if_not_present(vector3d&vec){
	for(unsigned int i = 0; i<gspoints.size(); i++){
		if(gspoints[i]==vec)
			return i;
	}
	gspoints.push_back(vec);
	return gspoints.size()-1;
}


//will add points along exsisting edges and make new faces from them
void geo_sphere::subdivide_geosphere(int res){
	int e = n_tri(res);
	if(gs_sphere_start[res]+e>(int)gstris.size())return;

	for(int i = 0; i<e; i++){
		int p1 = gstris[gs_sphere_start[res] + i].point[0];
		int p2 = gstris[gs_sphere_start[res] + i].point[1];
		int p3= gstris[gs_sphere_start[res] + i].point[2];

		vector3d v1 = gspoints[p1];
		vector3d v2 = gspoints[p2];
		vector3d v3 = gspoints[p3];
		
		//adding two vectors makes a scalar multiple of of there average
		//normaliseing them will project them onto the unit sphere
		int p12 = add_if_not_present(MakeUnitVector(v1+v2));
		int p23 = add_if_not_present(MakeUnitVector(v2+v3));
		int p31 = add_if_not_present(MakeUnitVector(v3+v1));

		gstris.push_back(stri(p1,p12,p31));
		gstris.push_back(stri(p2,p23,p12));
		gstris.push_back(stri(p3,p31,p23));
		gstris.push_back(stri(p12,p23,p31));
	}
}

void geo_sphere::make_geo_spheres(int max_res){

	make_isohedron();

	gs_sphere_start.resize(max_res+1);
	gs_sphere_start[0]=0;

	for(int i = 0; i<max_res; i++){
		gs_sphere_start[i+1] = (int)gstris.size();
		subdivide_geosphere(i);
	}
}

void geo_sphere::draw(const vector3d&pnt, float rad, unsigned int res, color col){
	if(gs_sphere_start.size() <= res+1)
	{
		//if we haven't made that resolution yet, do it now
		make_geo_spheres(res);
	}
	int start = gs_sphere_start[res];
	int ntri = n_tri(res);
//	ntri = 319;

	glPushMatrix();

	glTranslatef(pnt.x, pnt.y, pnt.z);
	glScalef(rad, rad, rad);

	glBegin(GL_TRIANGLES);
	glColor4ubv( (GLubyte*)col.col);

	for(int i = 0; i<ntri; i++){
		for(int j = 2; j>-1; j--){
			glNormal3fv((GLfloat *)&gspoints[gstris[start + i].point[j]]);
			glVertex3fv((GLfloat *)&(gspoints[gstris[start + i].point[j]]));
			//sence this is a sphere of unit radius, 
			//the verts also serve as there own normals!
		}
	}
	glColor4ubv( (GLubyte*)color(255,255,255,255).col);

	glEnd();

	glPopMatrix();

}

ogl_vertex_buffer geo_sphere::make_vertex_buffer(int res){
	ogl_vertex_buffer ret;

//	struct OGL_vert{
		vector3d pos;
	//	vector3d norm;
//	};

	int start = gs_sphere_start[res];
	int ntri = n_tri(res);


//	std::vector<OGL_vert> tri(ntri*3);
	std::vector<vector3d> tri(ntri*3);

	for(int i = 0; i<ntri; i++){
		for(unsigned int v = 0; v<3; v++){
			tri[i*3+v] = gspoints[gstris[start+i].point[v]];
		//	tri[i*3+v].pos = gspoints[gstris[start+i].point[v]];
		//	tri[i*3+v].norm = gspoints[gstris[start+i].point[v]];
		}
	}

	ret.n_verts = (int)tri.size();
//	ret.vertex_size = sizeof(OGL_vert);
	ret.vertex_size = sizeof(vector3d);
	ret.buffer = 0;
//	ret.format = GL_N3F_V3F;
	ret.format = GL_V3F;

	if(tri.size() <1)return ret;

//	glSetStrideSize(vertex_buffer[tid].vertex_size);
	glGenBuffersARB(1, &ret.buffer);
	ERROR_CHECK;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ret.buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, tri.size()*sizeof(vector3d),NULL, GL_STATIC_DRAW_ARB);
	float*map = (float *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	ERROR_CHECK;
	memcpy(map, &tri[0], tri.size()*sizeof(vector3d));
 	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
 
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	return ret;
}