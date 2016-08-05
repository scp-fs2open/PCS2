//****************************************************************************
//	wxGL_PMFCanvas.cpp
//	POF Constructor Suite 2 Renderer
//	
//****************************************************************************

/*
 * $Logfile: /wxGL_PMFCanvas.cpp $
 * $Revision: 1.47 $
 * $Date: 2008/11/13 00:43:42 $
 * $Author: kazan $
 *
 * $Log: wxGL_PMFCanvas.cpp,v $
 * Revision 1.47  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.46  2008/05/15 19:19:24  kazan
 * Resolve #35, #64
 * Maybe Resolve: #68, #61
 *
 * Revision 1.45  2008/05/15 17:43:58  kazan
 * *** empty log message ***
 *
 * Revision 1.44  2008/02/09 22:33:05  kazan
 * bye bye kaz_vector
 *
 * Revision 1.43  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.42  2008/02/07 18:48:50  kazan
 * remove unnecessary trailing else
 *
 * Revision 1.41  2008/02/07 18:45:55  kazan
 * Restore non-VBO omnipoints renderer - used if VBO extension check fails
 *
 * Revision 1.40  2008/01/19 01:55:50  bobboau
 * modified the omnipoint geospheres to use VBOs, the geometry of these things is totaly static, so it makes sence to use them
 *
 * Revision 1.39  2007/07/25 05:43:19  bobboau
 * fixed a crash bug.
 *
 * Revision 1.38  2007/07/25 03:18:30  bobboau
 * fixed bug in right buttom omnipoint movement (was broken aparently, just noticed, guess no one else uses it)
 *
 * Revision 1.37  2007/07/24 22:50:25  bobboau
 * added undo/redo functionality
 *
 * Revision 1.36  2007/07/14 12:11:48  bobboau
 * added a OGL info menu option under optpons to help in debugging problems with people who have out of date OGL drivers.
 *
 * Revision 1.35  2007/07/12 11:37:14  bobboau
 * added shine mapping suport.
 *
 * Revision 1.34  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.33  2007/07/11 19:14:50  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.32  2007/07/10 05:34:47  bobboau
 * added a few preset views to a view menu.
 *
 * Revision 1.31  2007/07/06 05:38:56  bobboau
 * added a grid, fixed a few bugs, mostly related to othodanal view.
 *
 * Revision 1.30  2007/07/01 11:02:54  bobboau
 * modified the way things are drawn and implemented a few minor features
 *
 * Revision 1.29  2007/07/01 08:25:01  bobboau
 * fixed raypicking and moveing for orthographic projection
 *
 * Revision 1.28  2007/06/30 20:09:36  bobboau
 * added color options for wireframe overlays
 *
 * Revision 1.27  2007/06/30 18:58:11  bobboau
 * made some tweaks to the auto-path feature, and added an orthographic projection option
 *
 * Revision 1.26  2007/06/30 01:45:16  bobboau
 * added texture highlighting
 *
 * Revision 1.25  2007/06/29 23:57:54  bobboau
 * removed old toolbar buttons that I didn't think were very useful, added new ones that I thought were (axis locking and movement plane)
 *
 * Revision 1.24  2007/06/29 21:00:28  bobboau
 * added shield chunk "editor", doesn't actualy let you edit anything, but will display shield poly count and activate the rendering of shileds, which I also added.
 *
 * Revision 1.23  2007/06/29 20:15:43  bobboau
 * added copy and delete functionality for subobjects, there are situations were some higherarchies might break this, we should realy consiter holding the subobject data in an actual tree.
 *
 * also added a set/get active chunk function to the gl canvas, and implemented a wireframe overlay for the currently selected subobject. (that only renders when you have a subobject selected in the tree/editor)
 *
 * Revision 1.22  2007/06/28 10:27:05  bobboau
 * fixed crash caused by trying to set focus to windows that no longer exsist (I hope)
 *
 * changed the behaviour of selected subojects back to drawing the LOD of the subobject, rather than the subobject it's self bacause it is very hard to do offset manipulation if you can't see the rest of the model.
 *
 * Revision 1.21  2007/06/26 10:05:06  bobboau
 * added data import, includeing chunk, global, and subobject.
 *
 * Revision 1.20  2007/06/24 10:22:17  bobboau
 * modified the omnipoint 3d movement code, both modes now use a ray pick based delta for motion, so they should move relitive to were your mouse is on screen, needless to say this is slicker than what we had before.
 *
 * Revision 1.19  2007/06/24 06:47:45  bobboau
 * actualy remembered to assign array ids so the menu worked on anything other than the one I was testing on
 *
 * fixed a few minor bugs with the ray picker, and something slightly not so minor that was caused by a CVS conflict that I hadn't noticed.
 *
 * Revision 1.18  2007/06/24 04:44:12  bobboau
 * Implemented ray picking, added menu and kebord shortcuts for add/copy/delete of items and lists, and improved mouse movement of omnipoints.
 *
 * Revision 1.17  2007/06/21 22:57:47  bobboau
 * a number of minor tweaks and bug fixes with the interface mostly dealing with comunication between the tree and the control panel.
 *
 * Revision 1.16  2007/06/18 14:01:53  bobboau
 * added a neat little 'suggestion control' wraper for primitive controls, you push a button it gives you a few good options (for example names of hard coded subsystems)
 * I also added index nameing suport to the array class, but it probly wasn't worth the effort.
 *
 * Revision 1.15  2007/06/17 10:29:17  bobboau
 * made a renderer for omnipoints, don't like it, it's not very fast, hope somone moreknolegable about OGL rerwites it someday.
 *
 * also worked on omnipoint editing, and the data flow related to it.
 *
 * Revision 1.14  2007/06/16 19:40:11  bobboau
 * some small bugfixes, and added light color options.
 *
 * Revision 1.13  2007/06/15 06:00:19  bobboau
 * number of small fixes, mostly related to updateing data in one subsystem when it should be
 *
 * fixed weapon point bug were the editing of one type would result in the eraseing of the other
 *
 * added PINF editor
 *
 * added convergence calculator for weapon points
 *
 * quickened the scroll zoom and made it proportional
 *
 * Revision 1.12  2007/06/09 23:48:56  bobboau
 * made some improvements to the UI of the GL canvas, includeing scroll wheel zooming and omnipoint editing (hold control to enter edit mode). omnipoints still need a rendering method.
 *
 * I also slightly reorganized the GL canvas (moved it's declairation to it's own file)
 *
 * Revision 1.11  2007/06/05 06:29:25  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.10  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.9  2006/04/09 21:04:10  kazan
 * routine checkin
 *
 * Revision 1.8  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.7  2005/09/04 00:36:05  kazan
 * Two Fixes
 *
 * Revision 1.6  2005/08/31 16:59:24  kazan
 * Split control IDs into their own header, disable some context menus, get ready to start putting editing controls in.
 *
 * Revision 1.5  2005/08/30 23:23:25  kazan
 * working dds.. /shock
 *
 * Revision 1.4  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.3  2005/08/29 18:05:43  kazan
 * fixed a crash due to OGL drivers not gracefully handing invalid texture handles, also changed how we resize the window
 *
 * Revision 1.2  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 * Revision 1.1  2005/08/28 02:39:06  kazan
 * Textures load and render!
 *
 *
 *
 */

#include "wxGL_PMFCanvas.h"
#include "pcs2_CIDs.h"
#include <wx/image.h>
#include <GL/glu.h>
#include <fstream>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <cfloat>
#include "INSG_editor.h"

#include "geo_sphere.h"

DEFINE_EVENT_TYPE(OMNIPOINT_RAY_PICKED)


//*******************************************************************************
// wxGL_PMFCanvas Implementation
//*******************************************************************************

wxGL_PMFCanvas::wxGL_PMFCanvas(wxWindow* parent, main_panel* main, int id, wxPoint pos, wxSize sz, PCS_Model &ship, int *attriblist)
 : wxGLCanvas(parent, id, pos, sz, 0, _("GLCanvas"), attriblist), 
   	omni_selected_list(-1), omni_selected_item(-1),model(ship),previus_focus(NULL),kShiftdown(false),FreezeRender(true), IsRendering(false), mainpanel(main), gr_debug(NULL),UI_plane(XZ_PLANE),proj_mode(PROJ_PERSP), draw_the_grid(false), position(0,0,0), rotation(0,0,0)
{
	free_axis[0]=true;
	free_axis[1]=true;
	free_axis[2]=true;

	ambient_light[0]=0.1f; ambient_light[1]=0.1f; ambient_light[2]=0.1f; ambient_light[3]=1.0f;
	diffuse_light[0]=0.9f; diffuse_light[1]=0.9f; diffuse_light[2]=0.9f; diffuse_light[3]=1.0f;
}

void wxGL_PMFCanvas::Init() {
	FreezeRender = false;
	this->SetCurrent();
	// === === === Init GL === === === 
	// Initialize OpenGL function table
	vendor = wxString((char*)glGetString(GL_VENDOR), wxConvUTF8);
	renderer = wxString((char*)glGetString(GL_RENDERER), wxConvUTF8);
	version = wxString((char*)glGetString(GL_VERSION), wxConvUTF8);
	/*std::string extensions = (char*)glGetString(GL_EXTENSIONS);
	GLFunctions.LoadFunctionTable();*/

	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glEnable(GL_NORMALIZE);
	
	glClearColor(0, 0, 0, 1.0f);				// Black Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
//	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glEnable(GL_COLOR_SUM);

	Reinit(true);
//	gr_debug = new debug_window(wxGetTopLevelParent(parent), GR_DEBUG_WINDOW, "Graphics Debug Output", wxPoint(1024-200,768-150), wxSize(200,150));
}


//makes a left handed projection matrix that displays 
//+x as right, +y as up and +z and forward
void wxGL_PMFCanvas::projection_matrix(float fovy, float aspect, float zNear, float zFar){

	float m[4][4];
	memset(m,0,sizeof(float)*16);
	float f = 1.0f/tan(fovy);
	m[0][0] = -f/aspect;
	m[1][1] = -f;
	m[2][2] = (zNear+zFar)/(zNear-zFar);
	m[2][3] = -(2.0f*zNear*zFar)/(zNear-zFar);
	m[3][2] = -1.0f;
	
	glMultMatrixf(m[0]);
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::OnSize(wxSizeEvent& event)
{
	
	if (FreezeRender) // this is a no-no if we're froze
		return;

	
	this->SetCurrent();

	// Tell openGL
	glViewport(0, 0, (GLsizei)event.GetSize().GetWidth(), (GLsizei)event.GetSize().GetHeight());
  
	// Select & reset the Projection Matrix

	//the render function does all of this already
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Calculate aspect ratio
	if(event.GetSize().GetHeight()){
		//more division by zero-Bobboau
		float asp = (float)event.GetSize().GetWidth()/(float)event.GetSize().GetHeight();

		if(proj_mode == PROJ_ORTHO){
			glOrtho(asp*position.z+position.x, -asp*position.z+position.x, 1.0f*position.z+position.y, -1.0f*position.z+position.y, 0.0, 50000.0);
		}else{
			gluPerspective( 75.0f, asp, 1.0f, 25000.0f );
		}
	}


	// Select & reset the Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


bool wxGL_PMFCanvas::Destroy()
//void wxGL_PMFCanvas::OnClose(wxCloseEvent& event)
{
	
	FreezeRender = true;
	while (IsRendering);
	tex_ctrl.Reset();
	return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void wxGL_PMFCanvas::Reinit(bool should_reset_view)
{
	previus_focus = NULL;

	if (FreezeRender)// don't do this if we're already froze
		return;

	FreezeRender = true;
	while (IsRendering);
	int itemp;
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->SetPath(_T("/gr_options/"));
	pConfig->Read(_("use_vertex_buffer_objects"), &itemp, 0); // default to off
	UseVBOs = (itemp == 1);
	model.init_vertex_buffers(itemp == 1);
	if (model.GetSOBJCount())
	{
		if (should_reset_view) {
			reset_view();
		}

		reload_textures();
	}

	static geo_sphere sphere(3);
	if (GLEE_ARB_vertex_buffer_object)
		omni_point_buffer = sphere.make_vertex_buffer(3);


	FreezeRender = false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::reload_textures(){
	// load textures 
	std::vector<std::string> paths;
	
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->SetPath(_T("/tpaths/"));
	int num_paths = pConfig->Read(_("numpaths"), 0l);
	wxString strs;
	paths.resize(num_paths);

	for (int i = 0; i < num_paths; i++)
	{
		pConfig->Read(wxString::Format(_("path%d"), i), &strs);
		wxFileName filename(strs);
		filename.Normalize();
		paths[i] = std::string(filename.GetFullPath().mb_str());
	}

	mainpanel->UseThreadedProgBar = false; // wxProgressDialog calls Yield! recursive yield calls = FREEZE
	AsyncProgress* prog_messenger(new wxAsyncProgress(mainpanel, LOAD_TEXTURE_PROGRESS_MESSAGER, true));
	tex_ctrl.LoadTextures(model, paths,  prog_messenger);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::reset_view(){
	position = vector3d(0,0,-(model.GetMaxRadius() * 1.5));
	rotation = vector3d(-33,66,0);
	glViewport(0, 0, (GLsizei)GetSize().GetWidth(), (GLsizei)GetSize().GetHeight());
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void wxGL_PMFCanvas::Render()
{	
	if (FreezeRender)
		return;

	if(!IsShownOnScreen())
		return;

	IsRendering = true;
	this->SetCurrent();

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glClearColor(0, 0, 0, 1.0f);				// Black Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(GetSize().GetHeight()){
		//more division by zero-Bobboau
		float asp = (float)GetSize().GetWidth()/(float)GetSize().GetHeight();

		if(proj_mode == PROJ_ORTHO){
			float z = (position.z+log(abs(position.z)+1.0f))/1.25f;
			glOrtho((asp+position.x/1000.0f)*z, (-asp+position.x/1000.0f)*z, (1.0f+position.y/1000.0f)*z, (-1.0f+position.y/1000.0f)*z, -500.0, 50000.0);
		}else{
			gluPerspective( 75.0f, asp, 1.0f, 250000.0f );
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float radius;

	//position = vector3d(0,0,-(model.SOBJ(0).radius * 2.0));
	//rotation = vector3d(33,-66,0);
	if (model.GetSOBJCount())
	{
		radius = model.GetMaxRadius();
		// --------------------------------------------------------
		glTranslatef(position.x, position.y, position.z);
		// translate by autocentering point
		vector3d acen = model.GetAutoCenter();
		//glRotatef(GlobalRotation.z, 0, 0, 1);
		glTranslatef(acen.x, acen.y, acen.z);
				
		// --------------------------------------------------------
		// Setup Lighting
		glEnable(GL_LIGHTING);	
		GLfloat  ambientLight[4]; ambientLight[0] = ambient_light[0]; ambientLight[1] = ambient_light[1]; ambientLight[2] = ambient_light[2]; ambientLight[3] = ambient_light[3];
		GLfloat  diffuseLight[4]; diffuseLight[0] = diffuse_light[0]; diffuseLight[1] = diffuse_light[1]; diffuseLight[2] = diffuse_light[2]; diffuseLight[3] = diffuse_light[3];
		GLfloat  specular[]={ 1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat lightPos[]={ 0.0f, radius+350.0f, (radius+350.0f), 1.0f};
		//GLfloat  specref[]= { 1.0f, 1.0f, 1.0f, 1.0f};

		// Setup and enable light 0
		glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
		glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
		glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
		glEnable(GL_LIGHT0);

		// Enable color tracking
		//glEnable(GL_COLOR_MATERIAL);


		// Render Geometry
		glRotatef(rotation.x, 1, 0, 0);
		glRotatef(rotation.y, 0, 1, 0);
		glRotatef(rotation.z, 0, 0, 1);

		if(active_chunk == TXTR)
			model.set_active_texture(omni_selected_item);
		else
			model.set_active_texture(-1);
		if (omni.lod > -1 && omni.lod < model.GetLODCount()) {
			model.set_active_model(model.LOD(omni.lod));
		}
	
		model.Render(tex_ctrl, UseVBOs, active_chunk == SOBJ);

		glTranslatef(-acen.x, -acen.y, -acen.z);

		if(active_chunk == SHLD)
			model.draw_shields();

		if(active_chunk == INSG)
			model.draw_insignia(omni.lod, omni);

		glDisable(GL_TEXTURE_2D);

		draw_omnipoints();

		glDisable(GL_LIGHTING);

		if(draw_the_grid){
			vector3d grid_point = acen;
			if(omni_selected_list >-1 && omni_selected_item >-1)//active_chunk == GPNT || active_chunk == MPNT || active_chunk == DOCK || active_chunk == FUEL || active_chunk == GLOW || active_chunk == SPCL || active_chunk == TGUN || active_chunk == EYE || active_chunk == ACEN)
				grid_point = omni.point[omni_selected_list][omni_selected_item].pos+model.get_model_offset(omni.point[omni_selected_list][omni_selected_item].model);
			vector3d POFTranslate(vector3d);
			draw_grid(POFTranslate(model.GetMinBounding())+acen, POFTranslate(model.GetMaxBounding())+acen, grid_point);
		}
	}

/*	
	vector3d V = get_eye_pos();
	vector3d U = get_eye_uvec();
	vector3d R = get_eye_rvec();
	vector3d F = get_eye_fvec();

	wxPoint pt = ScreenToClient(wxGetMousePosition());
	vector3d r = ray_cast(pt.x, pt.y);
*/
//	gr_debug->set_text(wxString::Format("Camera Position:%0.2f, %0.2f, %0.2f\nCamera Rvec:    %0.2f, %0.2f, %0.2f\nCamera Uvec:    %0.2f, %0.2f, %0.2f\nCamera Fvec:    %0.2f, %0.2f, %0.2f\n\n\nMouse Point Ray: %0.2f, %0.2f, %0.2f",V.x,V.y,V.z ,R.x,R.y,R.z ,U.x,U.y,U.z ,F.x,F.y,F.z ,r.x,r.y,r.z));
	


	this->SwapBuffers();
	IsRendering = false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::refresh_omni_points(){
	//todo: add code for updateing omnipoints
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void wxGL_PMFCanvas::draw_omnipoints(){
	static geo_sphere sphere(3);

	if(omni.flags & OMNIPOINT_DONT_DRAW)
		return;

	glEnable(GL_BLEND);
	
	glBlendFunc(GL_ONE,GL_ONE);
	glDisable(GL_CULL_FACE);

	GLfloat  ambientLight[4]={0.25f,0.25f,0.25f,1.0f};
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);

	glDepthMask(GL_FALSE);

	unsigned int i;
	unsigned int j;

	if (GLEE_ARB_vertex_buffer_object)
	{
		glBindBuffer(GL_ARRAY_BUFFER, omni_point_buffer.buffer);
		glLockArraysEXT( 0, omni_point_buffer.n_verts);
		ERROR_CHECK;
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT, omni_point_buffer.vertex_size, (void*)NULL);
		glNormalPointer(GL_FLOAT, omni_point_buffer.vertex_size, (void*)((vector3d*)NULL + 1));
	}

	for(i = 0; i<omni.point.size(); i++){
		for(j = 0; j<omni.point[i].size(); j++){
			color col = omni.unselected;
			if(i == (unsigned)omni_selected_list){
				col = omni.selected_list;
				if(j == (unsigned)omni_selected_item)
					col = omni.selected_item;
			}

			//invalid radius so it's ok if we set it to anyhting
			//and the rest of the code assumes a sane rad
			
			if(!(omni.flags & OMNIPOINT_RAD))
				omni.point[i][j].rad = model.get_avg_dimintion()/61.80f;

			if (GLEE_ARB_vertex_buffer_object)
			{
				glPushMatrix();

				vector3d pnt = omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model);
				glTranslatef(pnt.x, pnt.y, pnt.z);

				if(omni.flags & OMNIPOINT_ANY_PATH){
					//sphere.draw(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model), model.get_avg_dimintion()/30.0f, 2, col*10);
					float r = model.get_avg_dimintion()/30.0f;
					glScalef(r,r,r);
					color col2 = col*10;
					glColor4ubv( (GLubyte*)col2.col);
					glDrawArrays(GL_TRIANGLES, 0, omni_point_buffer.n_verts);
					glScalef(1.0f/r,1.0f/r,1.0f/r);
				}

				glColor4ubv( (GLubyte*)col.col);

				glScalef(omni.point[i][j].rad, omni.point[i][j].rad, omni.point[i][j].rad);
				//sphere.draw(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model), omni.point[i][j].rad, 2, col);
				glDrawArrays(GL_TRIANGLES, 0, omni_point_buffer.n_verts);

				glPopMatrix();
			}
			else
			{
				sphere.draw(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model), omni.point[i][j].rad, 2, col);
				if(omni.flags & OMNIPOINT_ANY_PATH) 	                      
					sphere.draw(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model), model.get_avg_dimintion()/30.0f, 2, col*10);
	 		}



		}
	}

	if (GLEE_ARB_vertex_buffer_object)
	{
		glColor4ubv( (GLubyte*)color(255,255,255,255).col);


		glUnlockArraysEXT();
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		ERROR_CHECK;
	}
	
	GLfloat  AmbientLight[4]={1.0f,1.0f,1.0f,1.0f};
	glLightfv(GL_LIGHT0,GL_AMBIENT,AmbientLight);

	glLineWidth(2.5f);


	if(omni.flags & OMNIPOINT_ANY_PATH)
	for(i = 0; i<omni.point.size(); i++){
		glBegin(GL_LINE_STRIP);
		for(j = 0; j<omni.point[i].size(); j++){
			color col = omni.unselected;
			if(i == (unsigned)omni_selected_list){
				col = omni.selected_item;
			}
			col = col * 4.0f;
			glColor4ubv( (GLubyte*)col.col);
			vector3d v(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model));
			glVertex3fv((GLfloat *)&(v));
		}
		if (omni.flags & OMNIPOINT_CLOSED_PATH) {
			color col = omni.unselected;
			if(i == (unsigned)omni_selected_list){
				col = omni.selected_item;
			}
			col = col * 4.0f;
			glColor4ubv( (GLubyte*)col.col);
			vector3d v(omni.point[i][0].pos+model.get_model_offset(omni.point[i][0].model));
			glVertex3fv((GLfloat *)&(v));
		}
		glEnd();
	}

	glDisable(GL_BLEND);
	glBegin(GL_LINES);

	if(omni.flags & OMNIPOINT_NORM)
	for(i = 0; i<omni.point.size(); i++){
		for(j = 0; j<omni.point[i].size(); j++){
			color col = omni.unselected;
			if(i == (unsigned)omni_selected_list){
				col = omni.selected_list;
				if(j == (unsigned)omni_selected_item)
					col = omni.selected_item;
			}
			glColor4ubv( (GLubyte*)col.col);

			vector3d v(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model));
			glVertex3fv((GLfloat *)&(v));
			if(omni.flags & OMNIPOINT_COMMON_NORMAL) {
				vector3d vec(omni.point[i][0].norm * omni.point[i][0].rad);
				vec = omni.point[i][j].pos + model.get_model_offset(omni.point[i][j].model) + vec * 2.5f;
				glVertex3fv((GLfloat *)&(vec));
			} else {
				vector3d vec(omni.point[i][j].norm * omni.point[i][0].rad);
				vec = omni.point[i][j].pos + model.get_model_offset(omni.point[i][j].model) + vec * 2.5f;
				glVertex3fv((GLfloat *)&(vec));
			}
		}
	}

	glEnd();

	glColor4ubv( (GLubyte*)color(255,255,255,255).col);

	glDepthMask(GL_TRUE);

	glLineWidth(1.05f);

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void wxGL_PMFCanvas::draw_grid(vector3d min, vector3d max, vector3d point){

ERROR_CHECK;
	int u, v, w;
	switch(UI_plane){
	case XZ_PLANE:
		u=0;
		v=2;
		w=1;
		break;
	case XY_PLANE:
		u=0;
		v=1;
		w=2;
		break;
	case YZ_PLANE:
		u=1;
		v=2;
		w=0;
		break;
	default:
		u=0;
		v=2;
		w=1;
	}

	vector3d pnt;
	pnt[w] = point[w];

	glColor4ubv( (GLubyte*)color(255,255,255,255).col);

//	float rad = Distance(get_eye_pos(), point);
	float rad = (max.x-min.x+max.y-min.y+max.z-min.z)/3.0f;

	glLineWidth(1.1f);
	glBegin(GL_LINES);
	float i;

	float scale = pow(10.0f, int(log(abs(rad)))/int(log(10.0f))-2);
	//how far the types of grid lines will be apart

	//make finest lines around the thing your editing
	float u_dir = point[u]/abs(point[u]) * 0.5;
	float v_dir = point[v]/abs(point[v]) * 0.5;
	float pu = int((point[u])/scale+u_dir)*scale;
	float pv = int((point[v])/scale+v_dir)*scale;
	for(i = -10; i<11; i++){

		pnt[u] = scale * i + pu;
		pnt[v] = scale * 10 + pv;
		glVertex3fv((GLfloat *)&pnt);
		pnt[u] = scale * i + pu;
		pnt[v] = scale * -10 + pv;
		glVertex3fv((GLfloat *)&pnt);

		pnt[v] = scale * i + pv;
		pnt[u] = scale * 10 + pu;
		glVertex3fv((GLfloat *)&pnt);
		pnt[v] = scale * i + pv;
		pnt[u] = scale * -10 + pu;
		glVertex3fv((GLfloat *)&pnt);
	}
	
	//make lines withing the bounding box

	scale *= 10.0f;

	glEnd();
ERROR_CHECK;

	glLineWidth(2.1f);

	glBegin(GL_LINES);

	float min_u = int((min[u]-(1.0f))/scale)*scale;
	float max_u = int((max[u]+(1.0f))/scale)*scale;
	float min_v = int((min[v]-(1.0f))/scale)*scale;
	float max_v = int((max[v]+(1.0f))/scale)*scale;

	for(i = min_u; i<=max_u; i+=scale){
		pnt[u] = i;
		pnt[v] = max_v;
		glVertex3fv((GLfloat *)&pnt);
		pnt[u] = i;
		pnt[v] = min_v;
		glVertex3fv((GLfloat *)&pnt);
	}


	for(i = min_v; i<=max_v; i+=scale){
		pnt[v] = i;
		pnt[u] = max_u;
		glVertex3fv((GLfloat *)&pnt);
		pnt[v] = i;
		pnt[u] = min_u;
		glVertex3fv((GLfloat *)&pnt);
	}


	glEnd();

ERROR_CHECK;

	glLineWidth(3.1f);

	glBegin(GL_LINES);


	scale *= 10;
	//make big lines
	for(i = -10; i<11; i++){

		pnt[u] = scale * i;
		pnt[v] = scale * 10;
		glVertex3fv((GLfloat *)&pnt);
		pnt[u] = scale * i;
		pnt[v] = scale * -10;
		glVertex3fv((GLfloat *)&pnt);

		pnt[v] = scale * i;
		pnt[u] = scale * 10;
		glVertex3fv((GLfloat *)&pnt);
		pnt[v] = scale * i;
		pnt[u] = scale * -10;
		glVertex3fv((GLfloat *)&pnt);
	}
	glEnd();
ERROR_CHECK;
	glLineWidth(1.0f);
ERROR_CHECK;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void wxGL_PMFCanvas::ray_pick(vector3d point, vector3d norm){
	int idx1=-1;
	int idx2=-1;
	float max_dist = FLT_MAX;
	for(unsigned int i = 0; i<omni.point.size(); i++){
		for(unsigned int j = 0; j< omni.point[i].size(); j++){

			float d = point_line_distance(omni.point[i][j].pos+model.get_model_offset(omni.point[i][j].model), point, norm);
			float rad = 0.0f;
			if(omni.flags & OMNIPOINT_ANY_PATH)//paths can have rediculusly huge radius's
				rad = log(omni.point[i][j].rad)+1.0f;
			else
				rad = omni.point[i][j].rad;
			if(d < rad && d<max_dist){
				max_dist = d;
				idx1 = i;
				idx2 = j;
			}
		}
	}
	if(idx1 >-1 && idx2 >-1){
		set_selected_omni_points(idx1, idx2);
		wxCommandEvent commandEvent(OMNIPOINT_RAY_PICKED, GetId());
		GetEventHandler()->ProcessEvent(commandEvent);
		Render();
	}
}

void wxGL_PMFCanvas::ray_cast(int mx, int my, vector3d&start, vector3d&dir){
	wxSize size = GetClientSize();

	float x = float(mx);
	float y = float(size.y-my);
	
	double M[16], P[16];
	int v[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	 glGetIntegerv(GL_VIEWPORT, v);

	 double ret[3];
	gluUnProject(x, y, 0.0f, M, P, v, &ret[0], &ret[1], &ret[2]);
	start.x = ret[0];
	start.y = ret[1];
	start.z = ret[2];
	gluUnProject(x, y, 1.0f, M, P, v, &ret[0], &ret[1], &ret[2]);
	dir.x = ret[0];
	dir.y = ret[1];
	dir.z = ret[2];

	dir = MakeUnitVector(dir-start);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::on_click(wxMouseEvent& event){
	vector3d ray_start, ray_dir;
	ray_cast(event.GetX(), event.GetY(), ray_start, ray_dir);
	ray_pick(ray_start, ray_dir);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void wxGL_PMFCanvas::OnMouseEvt(wxMouseEvent& event)
{
	bool update_omnipoints = false;

	if(event.Leaving() || event.ButtonUp()){
		mainpanel->control_panel->push_undo();
	}

	//if(!event.Leaving()){
	//	if(this != FindFocus()){
	//		previus_focus = FindFocus();
	//		SetFocus();
	//	}
	//}else{
	//	if(previus_focus)previus_focus->SetFocus();
	//}

	if (!model.GetSOBJCount())
	{
		event.Skip();
		return;
	}
	
	//glsz
	if (event.GetEventType() == wxEVT_LEFT_DOWN || 
		event.GetEventType() == wxEVT_RIGHT_DOWN )
	{
		mouse_start = wxPoint(event.GetX(), event.GetY());
	}
	else if (event.GetEventType() == wxEVT_MOTION && 
				event.Dragging())
	{
//########################start mouse movement code########################//
		if(event.m_controlDown){
	//------------------------start omnipoint code------------------------//
			//if control is down we move the selected omnipoint
			if(omni.point.size() > 0 && omni_selected_list >-1 && omni_selected_list < (int)omni.point.size() && omni_selected_item >-1 && omni_selected_item < (int)omni.point[omni_selected_list].size()){
				//but only if we have omnipoints and our selected coords are valid

				omnipoint&point = omni.point[omni_selected_list][omni_selected_item];

				double M[16], P[16];
				int v[4];
				glGetDoublev(GL_MODELVIEW_MATRIX, M);
				glGetDoublev(GL_PROJECTION_MATRIX, P);
				glGetIntegerv(GL_VIEWPORT, v);

				
				vector3d start;
				vector3d start_dir;
				vector3d end;
				vector3d end_dir;

				ray_cast(event.GetX(), event.GetY(), end, end_dir);
				ray_cast(mouse_start.x, mouse_start.y, start, start_dir);

				vector3d global_point = point.pos+model.get_model_offset(point.model);
					
					bool s = true, s2 = true;

				vector3d delta(0,0,0);

				if(event.m_leftDown){

					delta = filter_movement_delta(	plane_line_intersect(global_point, get_movement_plane_norm(),end, end_dir, &s)
														-	plane_line_intersect(global_point, get_movement_plane_norm(),start, start_dir, &s2));
					//if left button is down we move along x and y
				}else if(event.m_rightDown){
					//if right is down we move along y
					delta = closest_point_between_lines(global_point, get_movement_plane_norm(), end, end_dir)
										-closest_point_between_lines(global_point, get_movement_plane_norm(), start, start_dir);
				}
				if(s &&s2 && !null_vec(delta)){
					point.pos = point.pos + delta;
					update_omnipoints = true;
				}
			}
	//------------------------end omnipoint code------------------------//
		} else if(event.m_altDown){
			//------------------------start transform code------------------------//
			if(event.m_leftDown){
				//if left button is down we move along x and y
				bool s = true, s2 = true;
				vector3d start;
				vector3d start_dir;
				vector3d end;
				vector3d end_dir;

				ray_cast(event.GetX(), event.GetY(), end, end_dir);
				ray_cast(mouse_start.x, mouse_start.y, start, start_dir);

				// XXX: Should this have a value?
				vector3d global_point = vector3d();
				vector3d delta;

				delta = filter_movement_delta(	plane_line_intersect(global_point, get_movement_plane_norm(),end, end_dir, &s)
						-	plane_line_intersect(global_point, get_movement_plane_norm(),start, start_dir, &s2));
				if(s &&s2 && !null_vec(delta)){
					mainpanel->control_panel->transform(matrix(), delta);
				}
			}else if(event.m_rightDown){
				//if right is down we rotate about z
				float angle = (mouse_start.y - event.GetY());
				matrix transform(get_movement_plane_norm());
				if(angle){
					mainpanel->control_panel->transform(transform.invert() % matrix(angle) % transform, vector3d());
				}
			}
		//------------------------end transform code------------------------//
	}else{
	//++++++++++++++++++++++++start view code++++++++++++++++++++++++//
			if (!event.m_shiftDown)
			{
				//if shift is not down we rotate
				if (event.m_leftDown)
				{
					//we rotate about x and y when the left button is down
					rotation.y -= (mouse_start.x - event.GetX());
					rotation.x -= (mouse_start.y - event.GetY());
				}
				else
				{
					//we rotate about z when the right is down
					rotation.z -= (mouse_start.y - event.GetY());
				}
			}
			else
			{
				//if shift is down them we move the position
				if (event.m_leftDown)
				{
					//we move x and y when the left is down
					position.x -= model.SOBJ(0).radius * 
							(float(mouse_start.x - event.GetX())/100.0) ;
					position.y += model.SOBJ(0).radius *
						(float(mouse_start.y - event.GetY())/100.0);
				}
				else
				{
					//we move along z when the right is down
					position.z -= model.SOBJ(0).radius *
						(float(mouse_start.y - event.GetY())/100.0);
				}
			}
	//++++++++++++++++++++++++end view code++++++++++++++++++++++++//
		}
		mouse_start = wxPoint(event.GetX(), event.GetY());
		Render();
//########################end mouse movement code########################//
	}
	
	if (event.GetEventType() == wxEVT_MOUSEWHEEL){
//************************start scroll code************************//
		if(event.m_controlDown){
	//------------------------start omnipoint code------------------------//
			//if control is down we scale the selected omnipoint
			if(omni.flags & OMNIPOINT_RAD && omni.point.size() > 0 && omni_selected_list >-1 && omni_selected_list < (int)omni.point.size() && omni_selected_item >-1 && omni_selected_item < (int)omni.point[omni_selected_list].size()){
				//if the omnipoint has a radius
				//and the coords are valid
				if(omni.point[omni_selected_list][omni_selected_item].rad < 0.000001)omni.point[omni_selected_list][omni_selected_item].rad = model.get_avg_dimintion()/61.80f;
				if(event.GetWheelRotation()>0)
					omni.point[omni_selected_list][omni_selected_item].rad *= float(event.GetWheelRotation()/event.GetWheelDelta())*1.15;
				else
					omni.point[omni_selected_list][omni_selected_item].rad /= float(-event.GetWheelRotation()/event.GetWheelDelta())*1.15;

				update_omnipoints=true;
			}
	//------------------------end omnipoint code------------------------//
		} else if(event.m_altDown){
			//------------------------start transform code------------------------//
			float scale;
			if(event.GetWheelRotation()>0) {
				scale = float(event.GetWheelRotation()/event.GetWheelDelta())*1.15;
			} else {
				scale = 1 / (float(-event.GetWheelRotation()/event.GetWheelDelta())*1.15);
			}
			matrix transform;
			for (int i = 0; i < 3; i++) {
				transform.a2d[i][i] = scale;
			}
			if(scale != 1.0f){
				mainpanel->control_panel->transform(transform, vector3d());
			}
			//------------------------end transform code------------------------//
		}else{
	//++++++++++++++++++++++++start view code++++++++++++++++++++++++//
			position.z -= float(	event.GetWheelRotation()/event.GetWheelDelta()	) * position.z/50.0f;
	//++++++++++++++++++++++++end view code++++++++++++++++++++++++//
		}
//************************end scroll code************************//
		Render();
	}
	else
		event.Skip();

	if(update_omnipoints){
		mainpanel->control_panel->set_omnipoints(omni);
		set_omnipoints(mainpanel->control_panel->get_omnipoints());
	}
//	Render();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BEGIN_EVENT_TABLE(wxGL_PMFCanvas, wxGLCanvas)
	EVT_PAINT(wxGL_PMFCanvas::OnPaint)
	EVT_SIZE(wxGL_PMFCanvas::OnSize)
	//EVT_CLOSE(wxGL_PMFCanvas::OnClose)
	EVT_MOUSE_EVENTS(wxGL_PMFCanvas::OnMouseEvt)
	EVT_LEFT_DCLICK(wxGL_PMFCanvas::on_click)
END_EVENT_TABLE();
