//****************************************************************************
//	FE_GR_ogl_functable.h
//	Ferrium OpenGL graphics header file
//	
//	OpenGL Function Pointers
// Allows us to use newer OpenGL functions than the system has headers for
//****************************************************************************

/*
 * $Logfile: /compat/FE_GR_ogl_functable.h $
 * $Revision: 1.2 $
 * $Date: 2007/07/12 08:34:24 $
 * $Author: bobboau $
 *
 * $Log: FE_GR_ogl_functable.h,v $
 * Revision 1.2  2007/07/12 08:34:24  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.1  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.1  2007/05/09 02:33:19  kazan
 * salvaged even more code - about all the graphics code that wasn't linked into the old graphics abstraction nightmare.
 * also updated all the CVS log header comments
 *
 * Revision 1.5  2004/08/06 16:40:43  kazan
 * VBO/IBO memory access
 *
 * Revision 1.4  2004/08/04 21:53:39  kazan
 * Generic API and OpenGL specific Vertex Buffer Objects
 *
 * Revision 1.3  2004/07/28 03:30:00  kazan
 * linux sync
 *
 * Revision 1.2  2004/07/28 01:39:09  kazan
 * hm... wtf
 *
 * Revision 1.1  2004/07/25 21:36:27  kazan
 * Direct3D on SDL!  Now the SDL input polling in MainLoop() works on D3D correctly
 *
 *
 *
 *
 *
 */


#if !defined(_FE_OGL_FUNCTABLE_H_)
#define _FE_OGL_FUNCTABLE_H_

#if defined(WIN32)
#include <windows.h>
#pragma warning ( disable : 4786 )
#define GLAPIENTRY __stdcall
#define GetGLFunc wglGetProcAddress
#else
// ----------- linux -----------
#define GLAPIENTRY
#define xglGetProcAddress
#endif

#define USE_SDL
#if defined(USE_SDL)
#include <SDL.h>

#if defined(GetGLFunc)
#undef GetGLFunc
#endif 

#define GetGLFunc SDL_GL_GetProcAddress
#endif // defined(USE_SDL)
//#include "GLee.h"
#include "../GLee.h"

//#include <GL/gl.h>
//#include <GL/glext.h>
#include <GL/glu.h>

// missing GL defines

#define GL_ARRAY_BUFFER_ARB				0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB		0x8893
#define GL_STATIC_DRAW_ARB				0x88E4

#define READ_ONLY_ARB					0x88B8
#define WRITE_ONLY_ARB					0x88B9
#define READ_WRITE_ARB					0x88BA


#define 	GL_TEXTURE0_ARB   0x84C0
#define 	GL_TEXTURE1_ARB   0x84C1
#define 	GL_TEXTURE2_ARB   0x84C2
#define 	GL_TEXTURE3_ARB   0x84C3
#define 	GL_TEXTURE4_ARB   0x84C4
#define 	GL_TEXTURE5_ARB   0x84C5
#define 	GL_TEXTURE6_ARB   0x84C6
#define 	GL_TEXTURE7_ARB   0x84C7
#define 	GL_TEXTURE8_ARB   0x84C8

// structure containing all the pointers
struct op_gl_funcs
{
	void LoadFunctionTable(); // sets the pointers
	bool IsExtensionSupported( const char* szTargetExtension );

	// ************************************* GL extensions ************************************* 
	bool SupportVBO;

	// ************************************* GL functions ************************************* 
	// all GL functions i know off

	void (GLAPIENTRY *glAccum)(GLenum op, GLfloat value);
	void (GLAPIENTRY *glActiveTextureARB)(GLenum texture);
	void (GLAPIENTRY *glAlphaFunc)(GLenum func, GLclampf ref);
	GLboolean (GLAPIENTRY *glAreTexturesResident)(GLsizei n, const GLuint *textures, GLboolean *residences);
	void (GLAPIENTRY *glArrayElement)(GLint i);
	void (GLAPIENTRY *glBegin)(GLenum mode);
	void (GLAPIENTRY *glBindTexture)(GLenum target, GLuint texture);
	void (GLAPIENTRY *glBitmap)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
	void (GLAPIENTRY *glBlendColor)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GLAPIENTRY *glBlendEquation)( GLenum mode);
	void (GLAPIENTRY *glBlendFunc)( GLenum sfactor, GLenum dfactor);
	void (GLAPIENTRY *glCallList)( GLuint list);
	void (GLAPIENTRY *glCallLists)( GLsizei n, GLenum type, const GLvoid *lists);
	void (GLAPIENTRY *glClear)( GLbitfield mask);
	void (GLAPIENTRY *glClearAccum)( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GLAPIENTRY *glClearColor)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GLAPIENTRY *glClearDepth)( GLclampd depth);
	void (GLAPIENTRY *glClearIndex)( GLfloat c);
	void (GLAPIENTRY *glClearStencil)( GLint s);
	void (GLAPIENTRY *glClientActiveTextureARB)( GLenum texture);
	void (GLAPIENTRY *glClipPlane)( GLenum plane, const GLdouble *equation);

	// glColor variants
	void (GLAPIENTRY *glColor3b) ( GLbyte red, GLbyte green, GLbyte blue);
	void (GLAPIENTRY *glColor3d) ( GLdouble red, GLdouble green, GLdouble blue);
	void (GLAPIENTRY *glColor3f) ( GLfloat red, GLfloat green, GLfloat blue);
	void (GLAPIENTRY *glColor3i) ( GLint red, GLint green, GLint blue);
	void (GLAPIENTRY *glColor3s) ( GLshort red, GLshort green, GLshort blue);
	void (GLAPIENTRY *glColor3ub) ( GLubyte red, GLubyte green, GLubyte blue);
	void (GLAPIENTRY *glColor3ui) ( GLuint red, GLuint green, GLuint blue);
	void (GLAPIENTRY *glColor3us) ( GLushort red, GLushort green, GLushort blue);
	void (GLAPIENTRY *glColor4b) ( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
	void (GLAPIENTRY *glColor4d) ( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
	void (GLAPIENTRY *glColor4f) ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GLAPIENTRY *glColor4i) ( GLint red, GLint green, GLint blue, GLint alpha);
	void (GLAPIENTRY *glColor4s) ( GLshort red, GLshort green, GLshort blue, GLshort alpha);
	void (GLAPIENTRY *glColor4ub) ( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
	void (GLAPIENTRY *glColor4ui) ( GLuint red, GLuint green, GLuint blue, GLuint alpha);
	void (GLAPIENTRY *glColor4us) ( GLushort red, GLushort green, GLushort blue, GLushort alpha);
	void (GLAPIENTRY *glColor3bv) ( const GLdouble *v);
	void (GLAPIENTRY *glColor3fv) ( const GLfloat *v);
	void (GLAPIENTRY *glColor3iv) ( const GLint *v);
	void (GLAPIENTRY *glColor3sv) ( const GLshort *v);
	void (GLAPIENTRY *glColor3ubv) ( const GLubyte *v);
	void (GLAPIENTRY *glColor3uiv) ( const GLuint *v);
	void (GLAPIENTRY *glColor3usv) ( const GLushort *v);
	void (GLAPIENTRY *glColor4bv) ( const GLbyte *v);
	void (GLAPIENTRY *glColor4dv) ( const GLdouble *v);
	void (GLAPIENTRY *glColor4fv) ( const GLfloat *v);
	void (GLAPIENTRY *glColor4iv) ( const GLint *v);
	void (GLAPIENTRY *glColor4sv) ( const GLshort *v);
	void (GLAPIENTRY *glColor4ubv) ( const GLubyte *v);
	void (GLAPIENTRY *glColor4uiv) ( const GLuint *v);
	void (GLAPIENTRY *glColor4usv) ( const GLushort *v);

	// ---------------------------

	void (GLAPIENTRY *glColorMask) ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GLAPIENTRY *glColorMaterial) ( GLenum face, GLenum mode);
	void (GLAPIENTRY *glColorPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GLAPIENTRY *glColorSubTable) ( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
	void (GLAPIENTRY *glColorTable) ( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
	void (GLAPIENTRY *glColorTableParameterfv) ( GLenum target, GLenum pname, const GLfloat *params);
	void (GLAPIENTRY *glColorTableParameteriv) ( GLenum target, GLenum pname, const GLint *params);
	void (GLAPIENTRY *glConvolutionFilter1D) ( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
	void (GLAPIENTRY *glConvolutionFilter2D) ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
	void (GLAPIENTRY *glConvolutionParameterf) ( GLenum target, GLenum pname, GLfloat params);
	void (GLAPIENTRY *glConvolutionParameteri) ( GLenum target, GLenum pname, GLint params);
	void (GLAPIENTRY *glCopyColorSubTable) ( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
	void (GLAPIENTRY *glCopyColorTable) ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
	void (GLAPIENTRY *glCopyConvolutionFilter1D) ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
	void (GLAPIENTRY *glCopyConvolutionFilter2D) ( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GLAPIENTRY *glCopyPixels) ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
	void (GLAPIENTRY *glCopyTexImage1D) ( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
	void (GLAPIENTRY *glCopyTexImage2D) ( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GLAPIENTRY *glCopyTexSubImage1D) ( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
	void (GLAPIENTRY *glCopyTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GLAPIENTRY *glCopyTexSubImage3D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GLAPIENTRY *glCullFace) ( GLenum mode);
	void (GLAPIENTRY *glDeleteLists) ( GLuint list, GLsizei range);
	void (GLAPIENTRY *glDeleteTextures) ( GLsizei n, const GLuint *textures);
	void (GLAPIENTRY *glDepthFunc) ( GLenum func);
	void (GLAPIENTRY *glDepthMask) ( GLboolean flag);
	void (GLAPIENTRY *glDepthRange) ( GLclampd zNear, GLclampd zFar);
	void (GLAPIENTRY *glDisable) ( GLenum cap);
	void (GLAPIENTRY *glDrawArrays) ( GLenum mode, GLint first, GLsizei count);
	void (GLAPIENTRY *glDrawBuffer) ( GLenum mode);
	void (GLAPIENTRY *glDrawElements) ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	void (GLAPIENTRY *glDrawPixels) ( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glDrawRangeElements) ( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
	void (GLAPIENTRY *glEdgeFlag) ( GLboolean flag);

	void (GLAPIENTRY *glEdgeFlagPointer) ( GLsizei stride, const GLboolean *pointer);
	void (GLAPIENTRY *glEnable) ( GLenum cap);
	void (GLAPIENTRY *glEnableClientState) ( GLenum array);
	void (GLAPIENTRY *glEnd)();
	void (GLAPIENTRY *glEvalCoord1d) ( GLdouble u);
	void (GLAPIENTRY *glEvalCoord1f) ( GLfloat u);
	void (GLAPIENTRY *glEvalCoord2d) ( GLdouble u, GLdouble v);
	void (GLAPIENTRY *glEvalCoord2f) ( GLfloat u, GLfloat v);
	void (GLAPIENTRY *glEvalMesh1) ( GLenum mode, GLint i1, GLint i2);
	void (GLAPIENTRY *glEvalPoint1) ( GLint i);
	void (GLAPIENTRY *glEvalPoint2) ( GLint i, GLint j);
	void (GLAPIENTRY *glFeedbackBuffer) ( GLsizei size, GLenum type, GLfloat *buffer);
	void (GLAPIENTRY *glFinish) ( );
	void (GLAPIENTRY *glFlush) ( );
	void (GLAPIENTRY *glFogf) ( GLenum pname, GLfloat param);
	void (GLAPIENTRY *glFogi) ( GLenum pname, GLint param);
	void (GLAPIENTRY *glFrontFace) ( GLenum mode);
	void (GLAPIENTRY *glFrustum) ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	GLuint (GLAPIENTRY *glGenLists) ( GLsizei range);
	void (GLAPIENTRY *glGenTextures) ( GLsizei n, GLuint *textures);
	void (GLAPIENTRY *glGetBooleanv) ( GLenum pname, GLboolean *params);
	void (GLAPIENTRY *glGetDoublev) ( GLenum pname, GLdouble *params);
	void (GLAPIENTRY *glGetFloatv) ( GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetIntegerv) ( GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetClipPlane) ( GLenum plane, GLdouble *equation);
	void (GLAPIENTRY *glGetColorTable) ( GLenum target, GLenum format, GLenum type, GLvoid *table);
	void (GLAPIENTRY *glGetColorTableParameterfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetColorTableParameteriv) ( GLenum target, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetConvolutionFilter) ( GLenum target, GLenum format, GLenum type, GLvoid *image);
	void (GLAPIENTRY *glGetConvolutionParameterfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetConvolutionParameteriv) ( GLenum target, GLenum pname, GLint *params);
	GLenum (GLAPIENTRY *glGetError) ( );
	void (GLAPIENTRY *glGetHistogram) ( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
	void (GLAPIENTRY *glGetHistogramParameterfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetHistogramParameteriv) ( GLenum target, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetLightfv) ( GLenum light, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetLightiv) ( GLenum light, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetMapdv) ( GLenum target, GLenum query, GLdouble *v);
	void (GLAPIENTRY *glGetMapfv) ( GLenum target, GLenum query, GLfloat *v);
	void (GLAPIENTRY *glGetMapiv) ( GLenum target, GLenum query, GLint *v);
	void (GLAPIENTRY *glGetMaterialfv) ( GLenum face, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetMaterialiv) ( GLenum face, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetMinmax) ( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
	void (GLAPIENTRY *glGetMinmaxParameterfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetMinmaxParameteriv) ( GLenum target, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetPixelMapfv) ( GLenum map, GLfloat *values);
	void (GLAPIENTRY *glGetPixelMapuiv) ( GLenum map, GLuint *values);
	void (GLAPIENTRY *glGetPixelMapusv) ( GLenum map, GLushort *values);
	void (GLAPIENTRY *glGetPointerv) ( GLenum pname, GLvoid* *params);
	void (GLAPIENTRY *glGetPolygonStipple) ( GLubyte *mask);
	void (GLAPIENTRY *glGetSeparableFilter) ( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
	const GLubyte* (GLAPIENTRY *glGetString)( GLenum name);
	void (GLAPIENTRY *glGetTexImage) ( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
	void (GLAPIENTRY *glGetTexEnvfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetTexEnviv) ( GLenum target, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetTexGendv) ( GLenum coord, GLenum pname, GLdouble *params);
	void (GLAPIENTRY *glGetTexGenfv) ( GLenum coord, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetTexGeniv) ( GLenum coord, GLenum pname, GLint *params);

	void (GLAPIENTRY *glGetTexLevelParameterfv) ( GLenum target, GLint level, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetTexLevelParameteriv) ( GLenum target, GLint level, GLenum pname, GLint *params);
	void (GLAPIENTRY *glGetTexParameterfv) ( GLenum target, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glGetTexParameteriv) ( GLenum target, GLenum pname, GLint *params);
	void (GLAPIENTRY *glHint) ( GLenum target, GLenum mode);
	void (GLAPIENTRY *glHistogram) ( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);

	//glIndex variants
	void (GLAPIENTRY *glIndexd) ( GLdouble c);
	void (GLAPIENTRY *glIndexf) ( GLfloat c);
	void (GLAPIENTRY *glIndexi) ( GLint c);
	void (GLAPIENTRY *glIndexs) ( GLshort c);
	void (GLAPIENTRY *glIndexub) ( GLubyte c);
	void (GLAPIENTRY *glIndexdv) ( const GLdouble *c);
	void (GLAPIENTRY *glIndexfv) ( const GLfloat *c);
	void (GLAPIENTRY *glIndexiv) ( const GLint *c);
	void (GLAPIENTRY *glIndexsv) ( const GLshort *c);
	void (GLAPIENTRY *glIndexubv) ( const GLubyte *c);
	//-------------------

	void (GLAPIENTRY *glIndexMask) ( GLuint mask);
	void (GLAPIENTRY *glIndexPointer) ( GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GLAPIENTRY *glInitNames) ( );
	void (GLAPIENTRY *glInterleavedArrays) ( GLenum format, GLsizei stride, const GLvoid *pointer);
	GLboolean (GLAPIENTRY *glIsEnabled) ( GLenum cap);
	GLboolean (GLAPIENTRY *glIsList) ( GLuint list);
	GLboolean (GLAPIENTRY *glIsTexture) ( GLuint texture);
	void (GLAPIENTRY *glLightf) ( GLenum light, GLenum pname, GLfloat param);
	void (GLAPIENTRY *glLightfv) ( GLenum light, GLenum pname, GLfloat *params);
	void (GLAPIENTRY *glLighti) ( GLenum light, GLenum pname, GLint param);
	void (GLAPIENTRY *glLightModelf) ( GLenum pname, GLfloat param);
	void (GLAPIENTRY *glLightModeli) ( GLenum pname, GLint param);
	void (GLAPIENTRY *glLineStipple) ( GLint factor, GLushort pattern);
	void (GLAPIENTRY *glLineWidth) ( GLfloat width);
	void (GLAPIENTRY *glListBase) ( GLuint base);
	void (GLAPIENTRY *glLoadIdentity) ( );
	void (GLAPIENTRY *glLoadMatrixd) ( const GLdouble *m);
	void (GLAPIENTRY *glLoadMatrixf) ( const GLfloat *m);
	void (GLAPIENTRY *glLoadName) ( GLuint name);
	void (GLAPIENTRY *glLogicOp) ( GLenum opcode);
	void (GLAPIENTRY *glMap1d) ( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
	void (GLAPIENTRY *glMap1f) ( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
	void (GLAPIENTRY *glMap2d) ( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
	void (GLAPIENTRY *glMap2f) ( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
	void (GLAPIENTRY *glMapGrid1d) ( GLint un, GLdouble u1, GLdouble u2);
	void (GLAPIENTRY *glMapGrid1f) ( GLint un, GLfloat u1, GLfloat u2);
	void (GLAPIENTRY *glMapGrid2d) ( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
	void (GLAPIENTRY *glMapGrid2f) ( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
	void (GLAPIENTRY *glMaterialf) ( GLenum face, GLenum pname, GLfloat param);
	void (GLAPIENTRY *glMateriali) ( GLenum face, GLenum pname, GLint param);
	void (GLAPIENTRY *glMaterialfv) ( GLenum face, GLenum pname, const GLfloat *params);
	void (GLAPIENTRY *glMaterialiv) ( GLenum face, GLenum pname, const GLint *params);
	void (GLAPIENTRY *glMatrixMode) ( GLenum mode);
	void (GLAPIENTRY *glMinmax) ( GLenum target, GLenum internalformat, GLboolean sink);
	void (GLAPIENTRY *glMultMatrixd) ( const GLdouble *m);
	void (GLAPIENTRY *glMultMatrixf) ( const GLfloat *m);

	// glMultiTexCoordinate variants
	void (GLAPIENTRY *glMultiTexCoord1dARB) ( GLenum target, GLdouble s);
	void (GLAPIENTRY *glMultiTexCoord1fARB) ( GLenum target, GLfloat s);
	void (GLAPIENTRY *glMultiTexCoord1iARB) ( GLenum target, GLint s);
	void (GLAPIENTRY *glMultiTexCoord1sARB) ( GLenum target, GLshort s);
	void (GLAPIENTRY *glMultiTexCoord2dARB) ( GLenum target, GLdouble s, GLdouble t);
	void (GLAPIENTRY *glMultiTexCoord2fARB) ( GLenum target, GLfloat s, GLfloat t);
	void (GLAPIENTRY *glMultiTexCoord2iARB) ( GLenum target, GLint s, GLint t);
	void (GLAPIENTRY *glMultiTexCoord2sARB) ( GLenum target, GLshort s, GLshort t);
	void (GLAPIENTRY *glMultiTexCoord3dARB) ( GLenum target, GLdouble s, GLdouble t, GLdouble r);
	void (GLAPIENTRY *glMultiTexCoord3fARB) ( GLenum target, GLfloat s, GLfloat t, GLfloat r);
	void (GLAPIENTRY *glMultiTexCoord3iARB) ( GLenum target, GLint s, GLint t, GLint r);
	void (GLAPIENTRY *glMultiTexCoord3sARB) ( GLenum target, GLshort s, GLshort t, GLshort r);
	void (GLAPIENTRY *glMultiTexCoord4dARB) ( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	void (GLAPIENTRY *glMultiTexCoord4fARB) ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	void (GLAPIENTRY *glMultiTexCoord4iARB) ( GLenum target, GLint s, GLint t, GLint r, GLint q);
	void (GLAPIENTRY *glMultiTexCoord4sARB) ( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
	void (GLAPIENTRY *glMultiTexCoord1dvARB) ( GLenum target, const GLdouble *v);
	void (GLAPIENTRY *glMultiTexCoord1fvARB) ( GLenum target, const GLfloat *v);
	void (GLAPIENTRY *glMultiTexCoord1ivARB) ( GLenum target, const GLint *v);
	void (GLAPIENTRY *glMultiTexCoord1svARB) ( GLenum target, const GLshort *v);
	void (GLAPIENTRY *glMultiTexCoord2dvARB) ( GLenum target, const GLdouble *v);
	void (GLAPIENTRY *glMultiTexCoord2fvARB) ( GLenum target, const GLfloat *v);
	void (GLAPIENTRY *glMultiTexCoord2ivARB) ( GLenum target, const GLint *v);
	void (GLAPIENTRY *glMultiTexCoord2svARB) ( GLenum target, const GLshort *v);
	void (GLAPIENTRY *glMultiTexCoord3dvARB) ( GLenum target, const GLdouble *v);
	void (GLAPIENTRY *glMultiTexCoord3fvARB) ( GLenum target, const GLfloat *v);
	void (GLAPIENTRY *glMultiTexCoord3ivARB) ( GLenum target, const GLint *v);
	void (GLAPIENTRY *glMultiTexCoord3svARB) ( GLenum target, const GLshort *v);
	void (GLAPIENTRY *glMultiTexCoord4dvARB) ( GLenum target, const GLdouble *v);
	void (GLAPIENTRY *glMultiTexCoord4fvARB) ( GLenum target, const GLfloat *v);
	void (GLAPIENTRY *glMultiTexCoord4ivARB) ( GLenum target, const GLint *v);
	void (GLAPIENTRY *glMultiTexCoord4svARB) ( GLenum target, const GLshort *v);
	//------------------------

	void (GLAPIENTRY *glNewList) ( GLuint list, GLenum mode);
	void (GLAPIENTRY *glEndList) ( );

	//glNormal variants
	void (GLAPIENTRY *glNormal3b) ( GLbyte nx, GLbyte ny, GLbyte nz);
	void (GLAPIENTRY *glNormal3d) ( GLdouble nx, GLdouble ny, GLdouble nz);
	void (GLAPIENTRY *glNormal3f) ( GLfloat nx, GLfloat ny, GLfloat nz);
	void (GLAPIENTRY *glNormal3i) ( GLint nx, GLint ny, GLint nz);
	void (GLAPIENTRY *glNormal3s) ( GLshort nx, GLshort ny, GLshort nz);
	void (GLAPIENTRY *glNormal3bv) ( const GLbyte *v);
	void (GLAPIENTRY *glNormal3dv) ( const GLdouble *v);
	void (GLAPIENTRY *glNormal3fv) ( const GLfloat *v);
	void (GLAPIENTRY *glNormal3iv) ( const GLint *v);
	void (GLAPIENTRY *glNormal3sv) ( const GLshort *v);
	//---------------------

	void (GLAPIENTRY *glNormalPointer) ( GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GLAPIENTRY *glOrtho) ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	void (GLAPIENTRY *glPassThrough) ( GLfloat token);
	void (GLAPIENTRY *glPixelMapfv) ( GLenum map, GLint mapsize, const GLfloat *values);
	void (GLAPIENTRY *glPixelMapuiv) ( GLenum map, GLint mapsize, const GLuint *values);
	void (GLAPIENTRY *glPixelMapusv) ( GLenum map, GLint mapsize, const GLushort *values);
	void (GLAPIENTRY *glPixelStoref) ( GLenum pname, GLfloat param);
	void (GLAPIENTRY *glPixelStorei) ( GLenum pname, GLint param);
	void (GLAPIENTRY *glPixelTransferf) ( GLenum pname, GLfloat param);
	void (GLAPIENTRY *glPixelTransferi) ( GLenum pname, GLint param);
	void (GLAPIENTRY *glPixelZoom) ( GLfloat xfactor, GLfloat yfactor);
	void (GLAPIENTRY *glPointSize) ( GLfloat size);
	void (GLAPIENTRY *glPolygonMode) ( GLenum face, GLenum mode);
	void (GLAPIENTRY *glPolygonOffset) ( GLfloat factor, GLfloat units);
	void (GLAPIENTRY *glPolygonStipple) ( const GLubyte *mask);
	void (GLAPIENTRY *glPopMatrix)();
	void (GLAPIENTRY *glPrioritizeTextures) ( GLsizei n, const GLuint *textures, const GLclampf *priorities);
	void (GLAPIENTRY *glPushAttrib) ( GLbitfield mask);
	void (GLAPIENTRY *glPushClientAttrib) ( GLbitfield mask);
	void (GLAPIENTRY *glPushMatrix) ( );
	void (GLAPIENTRY *glPushName) ( GLuint name);

	// glRasterPos variants
	void (GLAPIENTRY *glRasterPos2d) ( GLdouble x, GLdouble y);
	void (GLAPIENTRY *glRasterPos2f) ( GLfloat x, GLfloat y);
	void (GLAPIENTRY *glRasterPos2i) ( GLint x, GLint y);
	void (GLAPIENTRY *glRasterPos2s) ( GLshort x, GLshort y);
	void (GLAPIENTRY *glRasterPos3d) ( GLdouble x, GLdouble y, GLdouble z);
	void (GLAPIENTRY *glRasterPos3f) ( GLfloat x, GLfloat y, GLfloat z);
	void (GLAPIENTRY *glRasterPos3i) ( GLint x, GLint y, GLint z);
	void (GLAPIENTRY *glRasterPos3s) ( GLshort x, GLshort y, GLshort z);
	void (GLAPIENTRY *glRasterPos4d) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	void (GLAPIENTRY *glRasterPos4f) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GLAPIENTRY *glRasterPos4i) ( GLint x, GLint y, GLint z, GLint w);
	void (GLAPIENTRY *glRasterPos4s) ( GLshort x, GLshort y, GLshort z, GLshort w);
	void (GLAPIENTRY *glRasterPos2dv) ( const GLdouble *v);
	void (GLAPIENTRY *glRasterPos2fv) ( const GLfloat *v);
	void (GLAPIENTRY *glRasterPos2iv) ( const GLint *v);
	void (GLAPIENTRY *glRasterPos2sv) ( const GLshort *v);
	void (GLAPIENTRY *glRasterPos3dv) ( const GLdouble *v);
	void (GLAPIENTRY *glRasterPos3fv) ( const GLfloat *v);
	void (GLAPIENTRY *glRasterPos3iv) ( const GLint *v);
	void (GLAPIENTRY *glRasterPos3sv) ( const GLshort *v);
	void (GLAPIENTRY *glRasterPos4dv) ( const GLdouble *v);
	void (GLAPIENTRY *glRasterPos4fv) ( const GLfloat *v);
	void (GLAPIENTRY *glRasterPos4iv) ( const GLint *v);
	void (GLAPIENTRY *glRasterPos4sv) ( const GLshort *v);
	//------------------------

	void (GLAPIENTRY *glReadBuffer) ( GLenum mode);
	void (GLAPIENTRY *glReadPixels) ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

	//glRect variants
	void (GLAPIENTRY *glRectd) ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
	void (GLAPIENTRY *glRectf) ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	void (GLAPIENTRY *glRecti) ( GLint x1, GLint y1, GLint x2, GLint y2);
	void (GLAPIENTRY *glRects) ( GLshort x1, GLshort y1, GLshort x2, GLshort y2);
	void (GLAPIENTRY *glRectdv) ( const GLdouble *v1, const GLdouble *v2);
	void (GLAPIENTRY *glRectfv) ( const GLfloat *v1, const GLfloat *v2);
	void (GLAPIENTRY *glRectiv) ( const GLint *v1, const GLint *v2);
	void (GLAPIENTRY *glRectsv) ( const GLshort *v1, const GLshort *v2);
	//--------

	GLint (GLAPIENTRY *glRenderMode) ( GLenum mode);
	void (GLAPIENTRY *glResetHistogram) ( GLenum target);
	void (GLAPIENTRY *glResetMinmax) ( GLenum target);
	void (GLAPIENTRY *glRotated) ( GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
	void (GLAPIENTRY *glRotatef) ( GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	void (GLAPIENTRY *glScaled) ( GLdouble x, GLdouble y, GLdouble z);
	void (GLAPIENTRY *glScalef) ( GLfloat x, GLfloat y, GLfloat z);
	void (GLAPIENTRY *glScissor) ( GLint x, GLint y, GLsizei width, GLsizei height );
	void (GLAPIENTRY *glSelectBuffer) ( GLsizei size, GLuint *buffer);
	void (GLAPIENTRY *glSeparableFilter2D) ( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
	void (GLAPIENTRY *glShadeModel) ( GLenum mode);
	void (GLAPIENTRY *glStencilFunc) ( GLenum func, GLint ref, GLuint mask);
	void (GLAPIENTRY *glStencilMask) ( GLuint mask);
	void (GLAPIENTRY *glStencilOp) ( GLenum fail, GLenum zfail, GLenum zpass);

	// glTextCoord variants
	void (GLAPIENTRY *glTexCoord1d) ( GLdouble s);
	void (GLAPIENTRY *glTexCoord1f) ( GLfloat s);
	void (GLAPIENTRY *glTexCoord1i) ( GLint s);
	void (GLAPIENTRY *glTexCoord1s) ( GLshort s);
	void (GLAPIENTRY *glTexCoord2d) ( GLdouble s, GLdouble t);
	void (GLAPIENTRY *glTexCoord2f) ( GLfloat s, GLfloat t);
	void (GLAPIENTRY *glTexCoord2i) ( GLint s, GLint t);
	void (GLAPIENTRY *glTexCoord2s) ( GLshort s, GLshort t);
	void (GLAPIENTRY *glTexCoord3d) ( GLdouble s, GLdouble t, GLdouble r);
	void (GLAPIENTRY *glTexCoord3f) ( GLfloat s, GLfloat t, GLfloat r);
	void (GLAPIENTRY *glTexCoord3i) ( GLint s, GLint t, GLint r);
	void (GLAPIENTRY *glTexCoord3s) ( GLshort s, GLshort t, GLshort r);
	void (GLAPIENTRY *glTexCoord4d) ( GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	void (GLAPIENTRY *glTexCoord4f) ( GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	void (GLAPIENTRY *glTexCoord4i) ( GLint s, GLint t, GLint r, GLint q);
	void (GLAPIENTRY *glTexCoord4s) ( GLshort s, GLshort t, GLshort r, GLshort q);
	void (GLAPIENTRY *glTexCoord1dv) ( const GLdouble *v);
	void (GLAPIENTRY *glTexCoord1fv) ( const GLfloat *v);
	void (GLAPIENTRY *glTexCoord1iv) ( const GLint *v);
	void (GLAPIENTRY *glTexCoord1sv) ( const GLshort *v);
	void (GLAPIENTRY *glTexCoord2dv) ( const GLdouble *v);
	void (GLAPIENTRY *glTexCoord2fv) ( const GLfloat *v);
	void (GLAPIENTRY *glTexCoord2iv) ( const GLint *v);
	void (GLAPIENTRY *glTexCoord2sv) ( const GLshort *v);
	void (GLAPIENTRY *glTexCoord3dv) ( const GLdouble *v);
	void (GLAPIENTRY *glTexCoord3fv) ( const GLfloat *v);
	void (GLAPIENTRY *glTexCoord3iv) ( const GLint *v);
	void (GLAPIENTRY *glTexCoord3sv) ( const GLshort *v);
	void (GLAPIENTRY *glTexCoord4dv) ( const GLdouble *v);
	void (GLAPIENTRY *glTexCoord4fv) ( const GLfloat *v);
	void (GLAPIENTRY *glTexCoord4iv) ( const GLint *v);
	void (GLAPIENTRY *glTexCoord4sv) ( const GLshort *v);
	//-------------------------

	void (GLAPIENTRY *glTexCoordPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GLAPIENTRY *glTexEnvf) ( GLenum target, GLenum pname, GLfloat param);
	void (GLAPIENTRY *glTexEnvi) ( GLenum target, GLenum pname, GLint param);
	void (GLAPIENTRY *glTexEnvfv) ( GLenum target, GLenum pname, const GLfloat *params);
	void (GLAPIENTRY *glTexEnviv) ( GLenum target, GLenum pname, const GLint *params);
	void (GLAPIENTRY *glTexGend) ( GLenum coord, GLenum pname, GLdouble param);
	void (GLAPIENTRY *glTexGenf) ( GLenum coord, GLenum pname, GLfloat param);
	void (GLAPIENTRY *glTexGeni) ( GLenum coord, GLenum pname, GLint param);
	void (GLAPIENTRY *glTexGendv) ( GLenum coord, GLenum pname, const GLdouble *params);
	void (GLAPIENTRY *glTexGenfv) ( GLenum coord, GLenum pname, const GLfloat *params);
	void (GLAPIENTRY *glTexGeniv) ( GLenum coord, GLenum pname, const GLint *params);
	void (GLAPIENTRY *glTexImage1D) ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTexImage2D) ( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTexImage3D) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTexParameterf) ( GLenum target, GLenum pname, GLfloat param);
	void (GLAPIENTRY *glTexParameteri) ( GLenum target, GLenum pname, GLint param);
	void (GLAPIENTRY *glTexParameterfv) ( GLenum target, GLenum pname, const GLfloat *params);
	void (GLAPIENTRY *glTexParameteriv) ( GLenum target, GLenum pname, const GLint *params);
	void (GLAPIENTRY *glTexSubImage1D) ( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTexSubImage3D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
	void (GLAPIENTRY *glTranslated) ( GLdouble x, GLdouble y, GLdouble z);
	void (GLAPIENTRY *glTranslatef) ( GLfloat x, GLfloat y, GLfloat z);

	//glVertex variants
	void (GLAPIENTRY *glVertex2d) ( GLdouble x, GLdouble y);
	void (GLAPIENTRY *glVertex2f) ( GLfloat x, GLfloat y);
	void (GLAPIENTRY *glVertex2i) ( GLint x, GLint y);
	void (GLAPIENTRY *glVertex2s) ( GLshort x, GLshort y);
	void (GLAPIENTRY *glVertex3d) ( GLdouble x, GLdouble y, GLdouble z);
	void (GLAPIENTRY *glVertex3f) ( GLfloat x, GLfloat y, GLfloat z);
	void (GLAPIENTRY *glVertex3i) ( GLint x, GLint y, GLint z);
	void (GLAPIENTRY *glVertex3s) ( GLshort x, GLshort y, GLshort z);
	void (GLAPIENTRY *glVertex4d) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	void (GLAPIENTRY *glVertex4f) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GLAPIENTRY *glVertex4i) ( GLint x, GLint y, GLint z, GLint w);
	void (GLAPIENTRY *glVertex4s) ( GLshort x, GLshort y, GLshort z, GLshort w);
	void (GLAPIENTRY *glVertex2dv) ( const GLdouble *v);
	void (GLAPIENTRY *glVertex2fv) ( const GLfloat *v);
	void (GLAPIENTRY *glVertex2iv) ( const GLint *v);
	void (GLAPIENTRY *glVertex2sv) ( const GLshort *v);
	void (GLAPIENTRY *glVertex3dv) ( const GLdouble *v);
	void (GLAPIENTRY *glVertex3fv) ( const GLfloat *v);
	void (GLAPIENTRY *glVertex3iv) ( const GLint *v);
	void (GLAPIENTRY *glVertex3sv) ( const GLshort *v);
	void (GLAPIENTRY *glVertex4dv) ( const GLdouble *v);
	void (GLAPIENTRY *glVertex4fv) ( const GLfloat *v);
	void (GLAPIENTRY *glVertex4iv) ( const GLint *v);
	void (GLAPIENTRY *glVertex4sv) ( const GLshort *v);
	//-----------------------

	void (GLAPIENTRY *glVertexPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GLAPIENTRY *glViewport) ( GLint x, GLint y, GLsizei width, GLsizei height);

	void (GLAPIENTRY *glDisableClientState)(GLenum array);

	// ---- Vertex Buffer Objects ----
	void (GLAPIENTRY *glBindBufferARB) (GLenum target, GLuint buffer);
	void (GLAPIENTRY *glDeleteBuffersARB ) (GLsizei n, const GLuint *buffers);
	void (GLAPIENTRY *glGenBuffersARB) (GLsizei n, GLuint *buffers);
	void (GLAPIENTRY *glBufferDataARB) (GLenum target, int size, const GLvoid *data, GLenum usage);
	void* (GLAPIENTRY *glMapBuffersARB) (GLenum target, GLenum access);
	bool (GLAPIENTRY *glUnmapBufferARB) (GLenum target); 

	// *************************************  GLU Functions ************************************* 
	// only the ones used thus far are here
	// don't have a GetProcAddress for GLU yet
	//void (GLAPIENTRY *gluPerspective)(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

};


#endif //_FE_OGL_FUNCTABLE_H_
