#pragma once

#include "omnipoints.h"
#include "pcs2.h"
#include "matrix.h"
#include "debug_window.h"
#include <wx/glcanvas.h>
#include <wx/clipbrd.h>

enum movement_plane{XZ_PLANE, XY_PLANE, YZ_PLANE};
enum AXIS {X_AXIS = 0,Y_AXIS = 1,Z_AXIS = 2};
enum projection_mode {PROJ_ORTHO, PROJ_PERSP};

class main_panel;

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class wxGL_PMFCanvas : public wxGLCanvas
{
	wxString vendor;
	wxString renderer;
	wxString version;

	protected:
		DECLARE_EVENT_TABLE()

	private:
		float  ambient_light[4];
		float  diffuse_light[4];

		//omnipoint stuff
		omnipoints omni;
		int omni_selected_list;
		int omni_selected_item;
		CHUNK active_chunk;
		void refresh_omni_points();//when ever omnipoint data 

		//gets changed this gets called to update what is displayed
		bool UseVBOs;
		ogl_vertex_buffer omni_point_buffer;//vertex buffer with geosphere in it

		// editor data
		PCS_Model &model;

		//wxSize glsz;
		wxWindow*previus_focus;
	
		// mouse data
		wxPoint mouse_start;
		bool kShiftdown;	

		// Texture manager
		TextureControl tex_ctrl;
		bool FreezeRender, IsRendering;

		main_panel*mainpanel;

		debug_window*gr_debug;

		void draw_omnipoints();

		movement_plane UI_plane;
		bool free_axis[3];
		projection_mode proj_mode;
		bool draw_the_grid;

		vector3d get_movement_plane_norm(){
			switch(UI_plane){
			case XZ_PLANE:
				return vector3d(0,1,0);
			case XY_PLANE:
				return vector3d(0,0,1);
			case YZ_PLANE:
				return vector3d(1,0,0);
			default:
				return vector3d(0,1,0);
			}
		}
		vector3d filter_movement_delta(vector3d in){
			return vector3d(
				free_axis[X_AXIS]?in.x:0.0f,
				free_axis[Y_AXIS]?in.y:0.0f,
				free_axis[Z_AXIS]?in.z:0.0f
				);
		}

		void draw_grid(vector3d min, vector3d max, vector3d point);

		wxGLContext* m_context;

	public:
		//omnipoint stuff
		void set_omnipoints(const omnipoints&Omni){
			omni = Omni;
			refresh_omni_points();
		};
		const omnipoints&get_omnipoints(){
			return omni;
		};
		void set_selected_omni_points(int row, int item){
			omni_selected_list = row; 
			omni_selected_item = item;
			refresh_omni_points();
		};
		void get_selected_omni_points(int&row, int&item){
			row = omni_selected_list; 
			item = omni_selected_item;
		};

		void set_active_chunk(CHUNK type){active_chunk = type;}
		CHUNK get_active_chunk(){return active_chunk;}

		//pickes an omnipoint from the provided ray
		void ray_pick(vector3d point, vector3d norm);

		//makes a ray from the on screen coords
		void ray_cast(int mx, int my, vector3d&start, vector3d&dir);



		wxGL_PMFCanvas(wxWindow* parent, main_panel* main, int id, wxPoint pos, wxSize sz, PCS_Model &ship, int *attriblist);
	
		void Render();
		virtual void OnMouseEvt(wxMouseEvent& event);
		void on_click(wxMouseEvent& event);
		void Init();
		void Reinit(bool should_reset_view);
		void reset_view();

		void reload_textures();
		std::string get_texture_filename(int idx, Texture_Type type = TC_TEXTURE){
			return tex_ctrl.get_texture_filename(idx, type);
		}

		void OnPaint(wxPaintEvent&event){event.Skip(); Render();}
		

		virtual void OnSize(wxSizeEvent& event);
		virtual bool Destroy();

		// render data
		vector3d position, rotation;

		friend class main_panel;//again? o_0 I thought friends were the enemy

		void projection_matrix(float fovy, float aspect, float zNear, float zFar);

		void set_ambient_light(unsigned char r, unsigned char g, unsigned char b){
			ambient_light[0] = float(r)/255.0f;
			ambient_light[1] = float(g)/255.0f;
			ambient_light[2] = float(b)/255.0f;
		}
		void set_diffuse_light(unsigned char r, unsigned char g, unsigned char b){
			diffuse_light[0] = float(r)/255.0f;
			diffuse_light[1] = float(g)/255.0f;
			diffuse_light[2] = float(b)/255.0f;
		}

		void set_movement_plane(movement_plane pl){UI_plane = pl; Render();};
		void set_axis_lock(AXIS a, bool free){free_axis[a] = free; Render();};

		void set_projection_mode(projection_mode p){proj_mode = p;Render();};
		void set_draw_grid(bool g){draw_the_grid = g; Render();};
		void set_eye_angles(const vector3d&angs){rotation=angs;Render();}
		void set_eye_position(const vector3d&pos){position=pos;Render();}

		void display_OGL_info(){
			wxString info = wxString::Format(_("OpenGL Version is \"%s\"\nVendor is \"%s\"\nRenderer is \"%s\""), version.c_str(), vendor.c_str(), renderer.c_str());
			wxMessageBox(info + _("\n\nThis information has been copied to the clipboard for your convenience"), _("OpenGL Info"));
      if (wxTheClipboard->Open()) {
        wxTextDataObject* obj = new wxTextDataObject(info);
        // takes ownership
        wxTheClipboard->SetData(obj);
        wxTheClipboard->Close();
      }
		}

		vector3d get_eye_pos(){
			matrix_4x4 M;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
			M.Invert();
			return M * vector3d(0,0,0);
		}

		vector3d get_eye_rvec(){
			matrix_4x4 M;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
			M.Invert();
			vector3d v = M.multiply_rot(vector3d(1,0,0));
			return v;
		}

		vector3d get_eye_uvec(){
			matrix_4x4 M;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
			M.Invert();
			vector3d v = M.multiply_rot(vector3d(0,1,0));
			return v;
		}

		vector3d get_eye_fvec(){
			matrix_4x4 M;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
			M.Invert();
			vector3d v = M.multiply_rot(vector3d(0,0,1));
			return v;
		}

		matrix_4x4 get_eye_matrix(){
			matrix_4x4 M, R;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
			M.Invert();
			R.rvec() = M.multiply_rot(vector3d(1,0,0));
			R.uvec() = M.multiply_rot(vector3d(0,1,0));
			R.fvec() = M.multiply_rot(vector3d(0,0,1));
			R.pos() = M*vector3d(0,0,0);
			R(3,3) = 1;
			return R;
		}

		matrix_4x4 get_eye_rotation_matrix(){
			matrix_4x4 M, R;
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)(M[0]));
		//	M.Invert();
			R.rvec() = M.multiply_rot(vector3d(1,0,0));
			R.uvec() = M.multiply_rot(vector3d(0,1,0));
			R.fvec() = M.multiply_rot(vector3d(0,0,1));
			R(3,3) = 1;
			return R;
		}

};
