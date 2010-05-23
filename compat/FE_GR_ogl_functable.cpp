//****************************************************************************
//	FE_GR_ogl_functable.cpp
//	Ferrium OpenGL graphics implementation file
//	
//	OpenGL Function Pointers
// Allows us to use newer OpenGL functions than the system has headers for
//****************************************************************************

/*
 * $Logfile: /compat/FE_GR_ogl_functable.cpp $
 * $Revision: 1.1 $
 * $Date: 2007/07/11 21:28:45 $
 * $Author: kazan $
 *
 * $Log: FE_GR_ogl_functable.cpp,v $
 * Revision 1.1  2007/07/11 21:28:45  kazan
 * purge bsp cache, openGL abstraction that doesn't function (thanks M$)
 *
 * Revision 1.1  2007/05/09 02:33:19  kazan
 * salvaged even more code - about all the graphics code that wasn't linked into the old graphics abstraction nightmare.
 * also updated all the CVS log header comments
 *
 * Revision 1.6  2004/09/06 05:57:43  bobboau
 * added a font system, still needs a little work, font defs are should be loaded from file eventualy
 * could use a stream interface as well (uses the printf syntax currently.
 * create a font by passing a texture, then use the returned index when useing
 * the gr_printf function
 * I saw some memory leaks and tryed to use the MSVC mem traker but that
 * didn't work out so well so I erased all of it (an include at the top of every file)
 * but CVS still sees something diferent in them.
 *
 * Revision 1.5  2004/08/28 06:06:01  bobboau
 * got texture loading to "work" it's based on the test noise generator function
 * and as a result is extreemly ugly, I plan on re-writeing this soonish, but I figured
 * sence I got it working I should put it up. I think the file system might still have
 * a few bugs to work out, this now gives you a way to test it (I had to place a
 * copy of the texture in each directory hopeing it would find one of them)
 *
 * Revision 1.4  2004/08/21 17:37:38  kazan
 * linux sync
 *
 * Revision 1.3  2004/08/06 16:40:43  kazan
 * VBO/IBO memory access
 *
 * Revision 1.2  2004/08/04 21:53:39  kazan
 * Generic API and OpenGL specific Vertex Buffer Objects
 *
 * Revision 1.1  2004/07/25 21:36:27  kazan
 * Direct3D on SDL!  Now the SDL input polling in MainLoop() works on D3D correctly
 *
 *
 *
 *
 *
 */

#include "FE_GR_ogl_functable.h"
#include <cstring>

// borrowed from NeHe
bool op_gl_funcs::IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = this->glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}


void op_gl_funcs::LoadFunctionTable()
 // sets the pointers
{
	
  // ************************************* GL functions ************************************* 
  // all GL functions i know off
	this->glGetString = (const GLubyte*(GLAPIENTRY*)(GLenum))															GetGLFunc("glGetString");

	/* windows debug */
	int errornumber = GetLastError();

  	this->glAccum = (void(GLAPIENTRY*)(GLenum, GLfloat))															GetGLFunc("glAccum");
	this->glActiveTextureARB = (void (GLAPIENTRY*)(GLenum))															GetGLFunc("glActiveTextureARB");
	this->glAlphaFunc = (void (GLAPIENTRY*)(GLenum, GLclampf))														GetGLFunc("glAlphaFunc");
	this->glAreTexturesResident = (GLboolean (GLAPIENTRY*)(GLsizei, const GLuint*, GLboolean*))						GetGLFunc("glAreTexturesResident");
	this->glArrayElement = (void (GLAPIENTRY*)(GLint))																GetGLFunc("glArrayElement");
	this->glBegin = (void (GLAPIENTRY*)(GLenum))																	GetGLFunc("glBegin");
	this->glBindTexture = (void (GLAPIENTRY*)(GLenum, GLuint))														GetGLFunc("glBindTexture");
	this->glBitmap = (void (GLAPIENTRY*)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte*))		GetGLFunc("glBitmap");
	this->glBlendColor = (void (GLAPIENTRY*)(GLclampf, GLclampf, GLclampf, GLclampf))								GetGLFunc("glBlendColor");
	this->glBlendEquation = (void (GLAPIENTRY*)(GLenum))															GetGLFunc("glBlendEquation");
	this->glBlendFunc = (void (GLAPIENTRY*)(GLenum, GLenum))														GetGLFunc("glBlendFunc");
	this->glCallList = (void (GLAPIENTRY*)(GLuint))																	GetGLFunc("glCallList");
	this->glCallLists = (void (GLAPIENTRY*)(GLsizei, GLenum, const GLvoid*))										GetGLFunc("glCallLists");
	this->glClear = (void (GLAPIENTRY*)(GLbitfield))																GetGLFunc("glClear");
	this->glClearAccum = (void (GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))									GetGLFunc("glClearAccum");
	this->glClearColor = (void (GLAPIENTRY*)(GLclampf, GLclampf, GLclampf, GLclampf))								GetGLFunc("glClearColor");
	this->glClearDepth = (void (GLAPIENTRY*)(GLclampd))																GetGLFunc("glClearDepth");
	this->glClearIndex = (void (GLAPIENTRY*)(GLfloat))																GetGLFunc("glClearIndex");
	this->glClearStencil = (void (GLAPIENTRY*)(GLint))																GetGLFunc("glClearStencil");
	this->glClientActiveTextureARB = (void (GLAPIENTRY*)(GLenum))													GetGLFunc("glClientActiveTextureARB");
	this->glClipPlane = (void (GLAPIENTRY*)(GLenum, const GLdouble*))												GetGLFunc("glClipPlane");

  // glColor variants
	this->glColor3b = (void (GLAPIENTRY*) (GLbyte, GLbyte, GLbyte))						GetGLFunc("glColor3b");
	this->glColor3d = (void (GLAPIENTRY*) (GLdouble, GLdouble, GLdouble))				GetGLFunc("glColor3d");
	this->glColor3f = (void (GLAPIENTRY*) (GLfloat, GLfloat, GLfloat))					GetGLFunc("glColor3f");
	this->glColor3i = (void (GLAPIENTRY*) (GLint, GLint, GLint))						GetGLFunc("glColor3i");
	this->glColor3s = (void (GLAPIENTRY*) (GLshort, GLshort, GLshort))					GetGLFunc("glColor3s");
	this->glColor3ub = (void (GLAPIENTRY*) (GLubyte, GLubyte, GLubyte))					GetGLFunc("glColor3ub");
	this->glColor3ui = (void (GLAPIENTRY*) (GLuint, GLuint, GLuint))					GetGLFunc("glColor3ui");
	this->glColor3us = (void (GLAPIENTRY*) (GLushort, GLushort, GLushort))				GetGLFunc("glColor3us");
	this->glColor4b = (void (GLAPIENTRY*) (GLbyte, GLbyte, GLbyte, GLbyte))				GetGLFunc("glColor4b");
	this->glColor4d = (void (GLAPIENTRY*) (GLdouble, GLdouble, GLdouble, GLdouble))		GetGLFunc("glColor4d");
	this->glColor4f = (void (GLAPIENTRY*) (GLfloat, GLfloat, GLfloat, GLfloat))			GetGLFunc("glColor4f");
	this->glColor4i = (void (GLAPIENTRY*) (GLint, GLint, GLint, GLint))					GetGLFunc("glColor4i");
	this->glColor4s = (void (GLAPIENTRY*) (GLshort, GLshort, GLshort, GLshort))			GetGLFunc("glColor4s");
	this->glColor4ub = (void (GLAPIENTRY*) (GLubyte, GLubyte, GLubyte, GLubyte))		GetGLFunc("glColor4ub");
	this->glColor4ui = (void (GLAPIENTRY*) (GLuint, GLuint, GLuint, GLuint))			GetGLFunc("glColor4ui");
	this->glColor4us = (void (GLAPIENTRY*) (GLushort, GLushort, GLushort, GLushort))	GetGLFunc("glColor4us");
	this->glColor3bv = (void (GLAPIENTRY*) (const GLdouble*))							GetGLFunc("glColor3bv");
	this->glColor3fv = (void (GLAPIENTRY*) (const GLfloat*))							GetGLFunc("glColor3fv");
	this->glColor3iv = (void (GLAPIENTRY*) (const GLint*))								GetGLFunc("glColor3iv");
	this->glColor3sv = (void (GLAPIENTRY*) (const GLshort*))							GetGLFunc("glColor3sv");
	this->glColor3ubv = (void (GLAPIENTRY*) (const GLubyte*))							GetGLFunc("glColor3ubv");
	this->glColor3uiv = (void (GLAPIENTRY*) (const GLuint*))							GetGLFunc("glColor3uiv");
	this->glColor3usv = (void (GLAPIENTRY*) (const GLushort*))							GetGLFunc("glColor3usv");
	this->glColor4bv = (void (GLAPIENTRY*) (const GLbyte*))								GetGLFunc("glColor4bv");
	this->glColor4dv = (void (GLAPIENTRY*) (const GLdouble*))							GetGLFunc("glColor4dv");
	this->glColor4fv = (void (GLAPIENTRY*) (const GLfloat*))							GetGLFunc("glColor4fv");
	this->glColor4iv = (void (GLAPIENTRY*) (const GLint*))								GetGLFunc("glColor4iv");
	this->glColor4sv = (void (GLAPIENTRY*) (const GLshort*))							GetGLFunc("glColor4sv");
	this->glColor4ubv = (void (GLAPIENTRY*) (const GLubyte*))							GetGLFunc("glColor4ubv");
	this->glColor4uiv = (void (GLAPIENTRY*) (const GLuint*))							GetGLFunc("glColor4uiv");
	this->glColor4usv = (void (GLAPIENTRY*) (const GLushort*))							GetGLFunc("glColor4usv");

  // ---------------------------
  
	this->glColorMask = (void (GLAPIENTRY*) (GLboolean, GLboolean, GLboolean, GLboolean))									GetGLFunc("glColorMask");
	this->glColorMaterial = (void (GLAPIENTRY*) (GLenum, GLenum))															GetGLFunc("glColorMaterial");
	this->glColorPointer = (void (GLAPIENTRY*) (GLint, GLenum, GLsizei, const GLvoid*))										GetGLFunc("glColorPointer");
	this->glColorSubTable = (void (GLAPIENTRY*) (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))					GetGLFunc("glColorSubTable");
	this->glColorTable = (void (GLAPIENTRY*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid*))						GetGLFunc("glColorTable");
	this->glColorTableParameterfv = (void (GLAPIENTRY*) (GLenum, GLenum, const GLfloat*))									GetGLFunc("glColorTableParameterfv");
	this->glColorTableParameteriv = (void (GLAPIENTRY*) (GLenum, GLenum, const GLint*))										GetGLFunc("glColorTableParameteriv");
	this->glConvolutionFilter1D = (void (GLAPIENTRY*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid*))				GetGLFunc("glConvolutionFilter1D");
	this->glConvolutionFilter2D = (void (GLAPIENTRY*) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))	GetGLFunc("glConvolutionFilter2D");
	this->glConvolutionParameterf = (void (GLAPIENTRY*) (GLenum, GLenum, GLfloat))											GetGLFunc("glConvolutionParameterf");
	this->glConvolutionParameteri = (void (GLAPIENTRY*) (GLenum, GLenum, GLint))											GetGLFunc("glConvolutionParameteri");
	this->glCopyColorSubTable = (void (GLAPIENTRY*) (GLenum, GLsizei, GLint, GLint, GLsizei))								GetGLFunc("glCopyColorSubTable");
	this->glCopyColorTable = (void (GLAPIENTRY*) (GLenum, GLenum, GLint, GLint, GLsizei))									GetGLFunc("glCopyColorTable");
	this->glCopyConvolutionFilter1D = (void (GLAPIENTRY*) (GLenum, GLenum, GLint, GLint, GLsizei))							GetGLFunc("glCopyConvolutionFilter1D");
	this->glCopyConvolutionFilter2D = (void (GLAPIENTRY*) (GLenum, GLenum, GLint, GLint, GLsizei, GLsizei))					GetGLFunc("glCopyConvolutionFilter2D");
	this->glCopyPixels = (void (GLAPIENTRY*) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type))				GetGLFunc("glCopyPixels");
	this->glCopyTexImage1D = (void (GLAPIENTRY*) (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))						GetGLFunc("glCopyTexImage1D");
	this->glCopyTexImage2D = (void (GLAPIENTRY*) (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))			GetGLFunc("glCopyTexImage2D");
	this->glCopyTexSubImage1D = (void (GLAPIENTRY*) (GLenum, GLint, GLint, GLint, GLint, GLsizei))							GetGLFunc("glCopyTexSubImage1D");
	this->glCopyTexSubImage2D = (void (GLAPIENTRY*) (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))			GetGLFunc("glCopyTexSubImage2D");
	this->glCopyTexSubImage3D = (void (GLAPIENTRY*) (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))	GetGLFunc("glCopyTexSubImage3D");
	this->glCullFace = (void (GLAPIENTRY*) (GLenum))																		GetGLFunc("glCullFace");
	this->glDeleteLists = (void (GLAPIENTRY*) (GLuint, GLsizei))															GetGLFunc("glDeleteLists");
	this->glDeleteTextures = (void (GLAPIENTRY*) (GLsizei, const GLuint*))													GetGLFunc("glDeleteTextures");
  
	this->glDepthFunc = (void(GLAPIENTRY*)(GLenum))																GetGLFunc("glDepthFunc");
	this->glDepthMask = (void(GLAPIENTRY*)(GLboolean))															GetGLFunc("glDepthMask");
	this->glDepthRange = (void(GLAPIENTRY*)(GLclampd, GLclampd))												GetGLFunc("glDepthRange");
  this->glDisable = (void(GLAPIENTRY *)(GLenum))                                     GetGLFunc("glDisable");

	this->glDrawArrays = (void(GLAPIENTRY*)(GLenum, GLint, GLsizei))											GetGLFunc("glDrawArrays");
	this->glDrawBuffer = (void(GLAPIENTRY*)(GLenum))															GetGLFunc("glDrawBuffer");
	this->glDrawElements = (void(GLAPIENTRY*)(GLenum, GLsizei, GLenum, const GLvoid*))							GetGLFunc("glDrawElements");
	this->glDrawPixels = (void(GLAPIENTRY*)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))					GetGLFunc("glDrawPixels");
	this->glDrawRangeElements = (void(GLAPIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid*))		GetGLFunc("glDrawRangeElements");
	this->glEdgeFlag = (void(GLAPIENTRY*)(GLboolean))															GetGLFunc("glEdgeFlag");
	this->glEdgeFlagPointer = (void(GLAPIENTRY*)(GLsizei, const GLboolean*))									GetGLFunc("glEdgeFlagPointer");
	this->glEnable = (void(GLAPIENTRY*)(GLenum))																GetGLFunc("glEnable");
	this->glEnableClientState = (void(GLAPIENTRY*)(GLenum))														GetGLFunc("glEnableClientState");
	this->glEnd = (void(GLAPIENTRY*)())																			GetGLFunc("glEnd");
	this->glEvalCoord1d = (void(GLAPIENTRY*)(GLdouble))															GetGLFunc("glEvalCoord1d");
	this->glEvalCoord1f = (void(GLAPIENTRY*)(GLfloat))															GetGLFunc("glEvalCoord1f");
	this->glEvalCoord2d = (void(GLAPIENTRY*)(GLdouble, GLdouble))												GetGLFunc("glEvalCoord2d");
	this->glEvalCoord2f = (void(GLAPIENTRY*)(GLfloat, GLfloat))													GetGLFunc("glEvalCoord2f");
	this->glEvalMesh1 = (void(GLAPIENTRY*)(GLenum, GLint, GLint))												GetGLFunc("glEvalMesh1");
	this->glEvalPoint1 = (void(GLAPIENTRY*)(GLint))																GetGLFunc("glEvalPoint1");
	this->glEvalPoint2 = (void(GLAPIENTRY*)(GLint, GLint))														GetGLFunc("glEvalPoint2");
	this->glFeedbackBuffer = (void(GLAPIENTRY*)(GLsizei, GLenum, GLfloat*))										GetGLFunc("glFeedbackBuffer");
	this->glFinish = (void(GLAPIENTRY*)())																		GetGLFunc("glFinish");
	this->glFlush = (void(GLAPIENTRY*)())																		GetGLFunc("glFlush");
	this->glFogf = (void(GLAPIENTRY*)(GLenum, GLfloat))															GetGLFunc("glFogf");
	this->glFogi = (void(GLAPIENTRY*)(GLenum, GLint))															GetGLFunc("glFogi");
	this->glFrontFace = (void(GLAPIENTRY*)(GLenum))																GetGLFunc("glFrontFace");
	this->glFrustum = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))			GetGLFunc("glFrustum");
	this->glGenLists = (GLuint(GLAPIENTRY*)(GLsizei))															GetGLFunc("glGenLists");
	this->glGenTextures = (void(GLAPIENTRY*)(GLsizei, GLuint*))													GetGLFunc("glGenTextures");
	this->glGetBooleanv = (void(GLAPIENTRY*)(GLenum, GLboolean*))												GetGLFunc("glGetBooleanv");
	this->glGetDoublev = (void(GLAPIENTRY*)(GLenum, GLdouble*))													GetGLFunc("glGetDoublev");
	this->glGetFloatv = (void(GLAPIENTRY*)(GLenum, GLfloat*))													GetGLFunc("glGetFloatv");
	this->glGetIntegerv = (void(GLAPIENTRY*)(GLenum, GLint*))													GetGLFunc("glGetIntegerv");
	this->glGetClipPlane = (void(GLAPIENTRY*)(GLenum, GLdouble*))												GetGLFunc("glGetClipPlane");
	this->glGetColorTable = (void(GLAPIENTRY*)(GLenum, GLenum, GLenum, GLvoid*))								GetGLFunc("glGetColorTable");
	this->glGetColorTableParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))							GetGLFunc("glGetColorTableParameterfv");
	this->glGetColorTableParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))								GetGLFunc("glGetColorTableParameteriv");
	this->glGetConvolutionFilter = (void(GLAPIENTRY*)(GLenum, GLenum, GLenum, GLvoid*))							GetGLFunc("glGetConvolutionFilter");
	this->glGetConvolutionParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))							GetGLFunc("glGetConvolutionParameterfv");
	this->glGetConvolutionParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))								GetGLFunc("glGetConvolutionParameteriv");
	this->glGetError = (GLenum(GLAPIENTRY*)())																	GetGLFunc("glGetError");
	this->glGetHistogram = (void(GLAPIENTRY*)(GLenum, GLboolean, GLenum, GLenum, GLvoid*))						GetGLFunc("glGetHistogram");
	this->glGetHistogramParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))								GetGLFunc("glGetHistogramParameterfv");
	this->glGetHistogramParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))								GetGLFunc("glGetHistogramParameteriv");
	this->glGetLightfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))											GetGLFunc("glGetLightfv");
	this->glGetLightiv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))											GetGLFunc("glGetLightiv");
	this->glGetMapdv = (void(GLAPIENTRY*)(GLenum, GLenum, GLdouble*))											GetGLFunc("glGetMapdv");
	this->glGetMapfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))											GetGLFunc("glGetMapfv");
	this->glGetMapiv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))												GetGLFunc("glGetMapiv");
	this->glGetMaterialfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))										GetGLFunc("glGetMaterialfv");
	this->glGetMaterialiv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))											GetGLFunc("glGetMaterialiv");
	this->glGetMinmax = (void(GLAPIENTRY*)(GLenum, GLboolean, GLenum, GLenum, GLvoid*))							GetGLFunc("glGetMinmax");
	this->glGetMinmaxParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))								GetGLFunc("glGetMinmaxParameterfv");
	this->glGetMinmaxParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))									GetGLFunc("glGetMinmaxParameteriv");
	this->glGetPixelMapfv = (void(GLAPIENTRY*)(GLenum, GLfloat*))												GetGLFunc("glGetPixelMapfv");
	this->glGetPixelMapuiv = (void(GLAPIENTRY*)(GLenum, GLuint*))												GetGLFunc("glGetPixelMapuiv");
	this->glGetPixelMapusv = (void(GLAPIENTRY*)(GLenum, GLushort*))												GetGLFunc("glGetPixelMapusv");
	this->glGetPointerv = (void(GLAPIENTRY*)(GLenum, GLvoid**))													GetGLFunc("glGetPointerv");
	this->glGetPolygonStipple = (void(GLAPIENTRY*)(GLubyte*))													GetGLFunc("glGetPolygonStipple");
	this->glGetSeparableFilter = (void(GLAPIENTRY*)(GLenum, GLenum, GLenum, GLvoid*, GLvoid*, GLvoid*))			GetGLFunc("glGetSeparableFilter");
	this->glGetString = (const GLubyte*(GLAPIENTRY*)(GLenum))													GetGLFunc("glGetString");
	this->glGetTexImage = (void(GLAPIENTRY*)(GLenum, GLint, GLenum, GLenum, GLvoid*))							GetGLFunc("glGetTexImage");
	this->glGetTexEnvfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))											GetGLFunc("glGetTexEnvfv");
	this->glGetTexEnviv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))											GetGLFunc("glGetTexEnviv");
	this->glGetTexGendv = (void(GLAPIENTRY*)(GLenum, GLenum, GLdouble*))										GetGLFunc("glGetTexGendv");
	this->glGetTexGenfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))											GetGLFunc("glGetTexGenfv");
	this->glGetTexGeniv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))											GetGLFunc("glGetTexGeniv");
	this->glGetTexLevelParameterfv = (void(GLAPIENTRY*)(GLenum, GLint, GLenum, GLfloat*))						GetGLFunc("glGetTexLevelParameterfv");
	this->glGetTexLevelParameteriv = (void(GLAPIENTRY*)(GLenum, GLint, GLenum, GLint*))							GetGLFunc("glGetTexLevelParameteriv");
	this->glGetTexParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat*))									GetGLFunc("glGetTexParameterfv");
	this->glGetTexParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, GLint*))										GetGLFunc("glGetTexParameteriv");
	this->glHint = (void(GLAPIENTRY*)(GLenum, GLenum))															GetGLFunc("glHint");
	this->glHistogram = (void(GLAPIENTRY*)(GLenum, GLsizei, GLenum, GLboolean))									GetGLFunc("glHistogram");

  //glIndex variants
	this->glIndexd = (void(GLAPIENTRY*)(GLdouble))			GetGLFunc("glIndexd");
	this->glIndexf = (void(GLAPIENTRY*)(GLfloat))				GetGLFunc("glIndexf");
	this->glIndexi = (void(GLAPIENTRY*)(GLint))				GetGLFunc("glIndexi");
	this->glIndexs = (void(GLAPIENTRY*)(GLshort))				GetGLFunc("glIndexs");
	this->glIndexub = (void(GLAPIENTRY*)(GLubyte))			GetGLFunc("glIndexub");
	this->glIndexdv = (void(GLAPIENTRY*)(const GLdouble*))	GetGLFunc("glIndexdv");
	this->glIndexfv = (void(GLAPIENTRY*)(const GLfloat*))		GetGLFunc("glIndexfv");
	this->glIndexiv = (void(GLAPIENTRY*)(const GLint*))		GetGLFunc("glIndexiv");
	this->glIndexsv = (void(GLAPIENTRY*)(const GLshort*))		GetGLFunc("glIndexsv");
	this->glIndexubv = (void(GLAPIENTRY*)(const GLubyte*))	GetGLFunc("glIndexubv");
  //-------------------

	this->glIndexMask = (void(GLAPIENTRY*)(GLuint))																						GetGLFunc("glIndexMask");
	this->glIndexPointer = (void(GLAPIENTRY*)(GLenum, GLsizei, const GLvoid*))															GetGLFunc("glIndexPointer");
	this->glInitNames = (void(GLAPIENTRY*)())																							GetGLFunc("glInitNames");
	this->glInterleavedArrays = (void(GLAPIENTRY*)(GLenum, GLsizei, const GLvoid*))														GetGLFunc("glInterleavedArrays");
	this->glIsEnabled = (GLboolean(GLAPIENTRY*)(GLenum))																				GetGLFunc("glIsEnabled");
	this->glIsList = (GLboolean(GLAPIENTRY*)(GLuint))																					GetGLFunc("glIsList");
	this->glIsTexture = (GLboolean(GLAPIENTRY*)(GLuint))																				GetGLFunc("glIsTexture");
	this->glLightf = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat))																		GetGLFunc("glLightf");
	this->glLightfv = (void(GLAPIENTRY*)( GLenum , GLenum , GLfloat*))																	GetGLFunc("glLightfv");
	this->glLighti = (void(GLAPIENTRY*)(GLenum, GLenum, GLint))																			GetGLFunc("glLighti");
	this->glLightModelf = (void(GLAPIENTRY*)(GLenum, GLfloat))																			GetGLFunc("glLightModelf");
	this->glLightModeli = (void(GLAPIENTRY*)(GLenum, GLint))																			GetGLFunc("glLightModeli");
	this->glLineStipple = (void(GLAPIENTRY*)(GLint, GLushort))																			GetGLFunc("glLineStipple");
	this->glLineWidth = (void(GLAPIENTRY*)(GLfloat))																					GetGLFunc("glLineWidth");
	this->glListBase = (void(GLAPIENTRY*)(GLuint))																						GetGLFunc("glListBase");
	this->glLoadIdentity = (void(GLAPIENTRY*)())																						GetGLFunc("glLoadIdentity");
	this->glLoadMatrixd = (void(GLAPIENTRY*)(const GLdouble*))																			GetGLFunc("glLoadMatrixd");
	this->glLoadMatrixf = (void(GLAPIENTRY*)(const GLfloat*))																			GetGLFunc("glLoadMatrixf");
	this->glLoadName = (void(GLAPIENTRY*)(GLuint))																						GetGLFunc("glLoadName");
	this->glLogicOp = (void(GLAPIENTRY*)(GLenum))																						GetGLFunc("glLogicOp");
	this->glMap1d = (void(GLAPIENTRY*)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble*))										GetGLFunc("glMap1d");
	this->glMap1f = (void(GLAPIENTRY*)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat*))											GetGLFunc("glMap1f");
	this->glMap2d = (void(GLAPIENTRY*)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble*))	GetGLFunc("glMap2d");
	this->glMap2f = (void(GLAPIENTRY*)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat*))			GetGLFunc("glMap2f");
	this->glMapGrid1d = (void(GLAPIENTRY*)(GLint, GLdouble, GLdouble))																	GetGLFunc("glMapGrid1d");
	this->glMapGrid1f = (void(GLAPIENTRY*)(GLint, GLfloat, GLfloat))																	GetGLFunc("glMapGrid1f");
	this->glMapGrid2d = (void(GLAPIENTRY*)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble))										GetGLFunc("glMapGrid2d");
	this->glMapGrid2f = (void(GLAPIENTRY*)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat))											GetGLFunc("glMapGrid2f");
	this->glMaterialf = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat))																	GetGLFunc("glMaterialf");
	this->glMateriali = (void(GLAPIENTRY*)(GLenum, GLenum, GLint))																		GetGLFunc("glMateriali");
	this->glMaterialfv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLfloat*))															GetGLFunc("glMaterialfv");
	this->glMaterialiv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLint*))																GetGLFunc("glMaterialiv");
	this->glMatrixMode = (void(GLAPIENTRY*)(GLenum))																					GetGLFunc("glMatrixMode");
	this->glMinmax = (void(GLAPIENTRY*)(GLenum, GLenum, GLboolean))																		GetGLFunc("glMinmax");
	this->glMultMatrixd = (void(GLAPIENTRY*)(const GLdouble*))																			GetGLFunc("glMultMatrixd");
	this->glMultMatrixf = (void(GLAPIENTRY*)(const GLfloat*))																			GetGLFunc("glMultMatrixf");

  // glMultiTexCoordinate variants

	this->glMultiTexCoord1dARB = (void(GLAPIENTRY*)(GLenum, GLdouble))									GetGLFunc("glMultiTexCoord1dARB");
	this->glMultiTexCoord1fARB = (void(GLAPIENTRY*)(GLenum, GLfloat))									GetGLFunc("glMultiTexCoord1fARB");
	this->glMultiTexCoord1iARB = (void(GLAPIENTRY*)(GLenum, GLint))										GetGLFunc("glMultiTexCoord1iARB");
	this->glMultiTexCoord1sARB = (void(GLAPIENTRY*)(GLenum, GLshort))									GetGLFunc("glMultiTexCoord1sARB");

	this->glMultiTexCoord2dARB = (void(GLAPIENTRY*)(GLenum, GLdouble, GLdouble))						GetGLFunc("glMultiTexCoord2dARB");
	this->glMultiTexCoord2fARB = (void(GLAPIENTRY*)(GLenum, GLfloat, GLfloat))							GetGLFunc("glMultiTexCoord2fARB");
	this->glMultiTexCoord2iARB = (void(GLAPIENTRY*)(GLenum, GLint, GLint))								GetGLFunc("glMultiTexCoord2iARB");
	this->glMultiTexCoord2sARB = (void(GLAPIENTRY*)(GLenum, GLshort, GLshort))							GetGLFunc("glMultiTexCoord2sARB");
	this->glMultiTexCoord3dARB = (void(GLAPIENTRY*)(GLenum, GLdouble, GLdouble, GLdouble))				GetGLFunc("glMultiTexCoord3dARB");
	this->glMultiTexCoord3fARB = (void(GLAPIENTRY*)(GLenum, GLfloat, GLfloat, GLfloat))					GetGLFunc("glMultiTexCoord3fARB");
	this->glMultiTexCoord3iARB = (void(GLAPIENTRY*)(GLenum, GLint, GLint, GLint))						GetGLFunc("glMultiTexCoord3iARB");
	this->glMultiTexCoord3sARB = (void(GLAPIENTRY*)(GLenum, GLshort, GLshort, GLshort))					GetGLFunc("glMultiTexCoord3sARB");
	this->glMultiTexCoord4dARB = (void(GLAPIENTRY*)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("glMultiTexCoord4dARB");
	this->glMultiTexCoord4fARB = (void(GLAPIENTRY*)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat))		GetGLFunc("glMultiTexCoord4fARB");
	this->glMultiTexCoord4iARB = (void(GLAPIENTRY*)(GLenum, GLint, GLint, GLint, GLint))				GetGLFunc("glMultiTexCoord4iARB");
	this->glMultiTexCoord4sARB = (void(GLAPIENTRY*)(GLenum, GLshort, GLshort, GLshort, GLshort))		GetGLFunc("glMultiTexCoord4sARB");
	this->glMultiTexCoord1dvARB = (void(GLAPIENTRY*)(GLenum, const GLdouble*))							GetGLFunc("glMultiTexCoord1dvARB");
	this->glMultiTexCoord1fvARB = (void(GLAPIENTRY*)(GLenum, const GLfloat*))							GetGLFunc("glMultiTexCoord1fvARB");
	this->glMultiTexCoord1ivARB = (void(GLAPIENTRY*)(GLenum, const GLint*))								GetGLFunc("glMultiTexCoord1ivARB");
	this->glMultiTexCoord1svARB = (void(GLAPIENTRY*)(GLenum, const GLshort*))							GetGLFunc("glMultiTexCoord1svARB");
	this->glMultiTexCoord2dvARB = (void(GLAPIENTRY*)(GLenum, const GLdouble*))							GetGLFunc("glMultiTexCoord2dvARB");
	this->glMultiTexCoord2fvARB = (void(GLAPIENTRY*)(GLenum, const GLfloat*))							GetGLFunc("glMultiTexCoord2fvARB");
	this->glMultiTexCoord2ivARB = (void(GLAPIENTRY*)(GLenum, const GLint*))								GetGLFunc("glMultiTexCoord2ivARB");
	this->glMultiTexCoord2svARB = (void(GLAPIENTRY*)(GLenum, const GLshort*))							GetGLFunc("glMultiTexCoord2svARB");
	this->glMultiTexCoord3dvARB = (void(GLAPIENTRY*)(GLenum, const GLdouble*))							GetGLFunc("glMultiTexCoord3dvARB");
	this->glMultiTexCoord3fvARB = (void(GLAPIENTRY*)(GLenum, const GLfloat*))							GetGLFunc("glMultiTexCoord3fvARB");
	this->glMultiTexCoord3ivARB = (void(GLAPIENTRY*)(GLenum, const GLint*))								GetGLFunc("glMultiTexCoord3ivARB");
	this->glMultiTexCoord3svARB = (void(GLAPIENTRY*)(GLenum, const GLshort*))							GetGLFunc("glMultiTexCoord3svARB");
	this->glMultiTexCoord4dvARB = (void(GLAPIENTRY*)(GLenum, const GLdouble*))							GetGLFunc("glMultiTexCoord4dvARB");
	this->glMultiTexCoord4fvARB = (void(GLAPIENTRY*)(GLenum, const GLfloat*))							GetGLFunc("glMultiTexCoord4fvARB");
	this->glMultiTexCoord4ivARB = (void(GLAPIENTRY*)(GLenum, const GLint*))								GetGLFunc("glMultiTexCoord4ivARB");
	this->glMultiTexCoord4svARB = (void(GLAPIENTRY*)(GLenum, const GLshort*))							GetGLFunc("glMultiTexCoord4svARB");
  //------------------------
  
	this->glNewList = (void(GLAPIENTRY*)(GLuint, GLenum))	GetGLFunc("glNewList");
	this->glEndList = (void(GLAPIENTRY*)())					GetGLFunc("glEndList");

  //glNormal variants
	this->glNormal3b = (void(GLAPIENTRY*)(GLbyte, GLbyte, GLbyte))			GetGLFunc("glNormal3b");
	this->glNormal3d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))	GetGLFunc("glNormal3d");
	this->glNormal3f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))		GetGLFunc("glNormal3f");
	this->glNormal3i = (void(GLAPIENTRY*)(GLint, GLint, GLint))				GetGLFunc("glNormal3i");
	this->glNormal3s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort))		GetGLFunc("glNormal3s");
	this->glNormal3bv = (void(GLAPIENTRY*)(const GLbyte*))					GetGLFunc("glNormal3bv");
	this->glNormal3dv = (void(GLAPIENTRY*)(const GLdouble*))				GetGLFunc("glNormal3dv");
	this->glNormal3fv = (void(GLAPIENTRY*)(const GLfloat*))					GetGLFunc("glNormal3fv");
	this->glNormal3iv = (void(GLAPIENTRY*)(const GLint*))					GetGLFunc("glNormal3iv");
	this->glNormal3sv = (void(GLAPIENTRY*)(const GLshort*))					GetGLFunc("glNormal3sv");
  //---------------------
  
	this->glNormalPointer = (void(GLAPIENTRY*)(GLenum, GLsizei, const GLvoid*))						GetGLFunc("glNormalPointer");
	this->glOrtho = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("glOrtho");
	this->glPassThrough = (void(GLAPIENTRY*)(GLfloat))												GetGLFunc("glPassThrough");
	this->glPixelMapfv = (void(GLAPIENTRY*)(GLenum, GLint, const GLfloat*))							GetGLFunc("glPixelMapfv");
	this->glPixelMapuiv =  (void(GLAPIENTRY*)(GLenum, GLint, const GLuint*))						GetGLFunc("glPixelMapuiv");
	this->glPixelMapusv = (void(GLAPIENTRY*)(GLenum, GLint, const GLushort*))						GetGLFunc("glPixelMapusv");
	this->glPixelStoref = (void(GLAPIENTRY*)(GLenum, GLfloat))										GetGLFunc("glPixelStoref");
	this->glPixelStorei = (void(GLAPIENTRY*)(GLenum, GLint))										GetGLFunc("glPixelStorei");
	this->glPixelTransferf = (void(GLAPIENTRY*)(GLenum, GLfloat))									GetGLFunc("glPixelTransferf");
	this->glPixelTransferi = (void(GLAPIENTRY*)(GLenum, GLint))										GetGLFunc("glPixelTransferi");
	this->glPixelZoom = (void(GLAPIENTRY*)(GLfloat, GLfloat))										GetGLFunc("glPixelZoom");
	this->glPointSize = (void(GLAPIENTRY*)(GLfloat))												GetGLFunc("glPointSize");
	this->glPolygonMode = (void(GLAPIENTRY*)(GLenum, GLenum))										GetGLFunc("glPolygonMode");
	this->glPolygonOffset = (void(GLAPIENTRY*)(GLfloat, GLfloat))									GetGLFunc("glPolygonOffset");
	this->glPolygonStipple = (void(GLAPIENTRY*)(const GLubyte*))									GetGLFunc("glPolygonStipple");
    this->glPopMatrix = (void(GLAPIENTRY*)())														GetGLFunc("glPopMatrix");
	this->glPrioritizeTextures = (void(GLAPIENTRY*)(GLsizei, const GLuint*, const GLclampf*))		GetGLFunc("glPrioritizeTextures");
	this->glPushAttrib = (void(GLAPIENTRY*)(GLbitfield))											GetGLFunc("glPushAttrib");
	this->glPushClientAttrib = (void(GLAPIENTRY*)(GLbitfield))										GetGLFunc("glPushClientAttrib");
	this->glPushMatrix = (void(GLAPIENTRY*)())														GetGLFunc("glPushMatrix");
	this->glPushName = (void(GLAPIENTRY*)(GLuint))													GetGLFunc("glPushName");

  // glRasterPos variants
	this->glRasterPos2d = (void(GLAPIENTRY*)(GLdouble, GLdouble))						GetGLFunc("glRasterPos2d");
	this->glRasterPos2f = (void(GLAPIENTRY*)(GLfloat, GLfloat))							GetGLFunc("glRasterPos2f");
	this->glRasterPos2i = (void(GLAPIENTRY*)(GLint, GLint))								GetGLFunc("glRasterPos2i");
	this->glRasterPos2s = (void(GLAPIENTRY*)(GLshort, GLshort))							GetGLFunc("glRasterPos2s");
	this->glRasterPos3d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))				GetGLFunc("glRasterPos3d");
	this->glRasterPos3f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))				GetGLFunc("glRasterPos3f");
	this->glRasterPos3i = (void(GLAPIENTRY*)(GLint, GLint, GLint))						GetGLFunc("glRasterPos3i");
	this->glRasterPos3s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort))				GetGLFunc("glRasterPos3s");
	this->glRasterPos4d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("glRasterPos4d");
	this->glRasterPos4f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))		GetGLFunc("glRasterPos4f");
	this->glRasterPos4i = (void(GLAPIENTRY*)(GLint, GLint, GLint, GLint))				GetGLFunc("glRasterPos4i");
	this->glRasterPos4s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort, GLshort))		GetGLFunc("glRasterPos4s");
	this->glRasterPos2dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glRasterPos2dv");
	this->glRasterPos2fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glRasterPos2fv");
	this->glRasterPos2iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glRasterPos2iv");
	this->glRasterPos2sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glRasterPos2sv");
	this->glRasterPos3dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glRasterPos3dv");
	this->glRasterPos3fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glRasterPos3fv");
	this->glRasterPos3iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glRasterPos3iv");
	this->glRasterPos3sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glRasterPos3sv");
	this->glRasterPos4dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glRasterPos4dv");
	this->glRasterPos4fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glRasterPos4fv");
	this->glRasterPos4iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glRasterPos4iv");
	this->glRasterPos4sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glRasterPos4sv");
  //------------------------
  
	this->glReadBuffer = (void(GLAPIENTRY*)(GLenum))														GetGLFunc("glReadBuffer");
	this->glReadPixels =   (void(GLAPIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*))	GetGLFunc("glReadPixels");

  //glRect variants
  	this->glRectd = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))		GetGLFunc("glRectd");
	this->glRectf = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))			GetGLFunc("glRectf");
	this->glRecti = (void(GLAPIENTRY*)(GLint, GLint, GLint, GLint))					GetGLFunc("glRecti");
	this->glRects = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort, GLshort))			GetGLFunc("glRects");
	this->glRectdv = (void(GLAPIENTRY*)(const GLdouble*, const GLdouble*))			GetGLFunc("glRectdv");
	this->glRectfv = (void(GLAPIENTRY*)(const GLfloat*, const GLfloat*))			GetGLFunc("glRectfv");
	this->glRectiv = (void(GLAPIENTRY*)(const GLint*, const GLint*))				GetGLFunc("glRectiv");
	this->glRectsv = (void(GLAPIENTRY*)(const GLshort*, const GLshort*))			GetGLFunc("glRectsv");
  //--------

    this->glRenderMode = (GLint(GLAPIENTRY*)(GLenum))																					GetGLFunc("glRenderMode");
	this->glResetHistogram = (void(GLAPIENTRY*)(GLenum))																				GetGLFunc("glResetHistogram");
	this->glResetMinmax = (void(GLAPIENTRY*)(GLenum))																					GetGLFunc("glResetMinmax");
	this->glRotated = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))														GetGLFunc("glRotated");
	this->glRotatef = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))															GetGLFunc("glRotatef");
	this->glScaled = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))																	GetGLFunc("glScaled");
	this->glScalef = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))																		GetGLFunc("glScalef");
	this->glScissor = (void(GLAPIENTRY*)(GLint, GLint, GLsizei, GLsizei))																GetGLFunc("glScissor");
	this->glSelectBuffer = (void(GLAPIENTRY*)(GLsizei, GLuint*))																		GetGLFunc("glSelectBuffer");
	this->glSeparableFilter2D = (void(GLAPIENTRY*)(GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*, const GLvoid*))		GetGLFunc("glSeparableFilter2D");
	this->glShadeModel = (void(GLAPIENTRY*)(GLenum))																					GetGLFunc("glShadeModel");
	this->glStencilFunc = (void(GLAPIENTRY*)(GLenum, GLint, GLuint))																	GetGLFunc("glStencilFunc");
	this->glStencilMask = (void(GLAPIENTRY*)(GLuint))																					GetGLFunc("glStencilMask");
	this->glStencilOp = (void(GLAPIENTRY*)(GLenum, GLenum, GLenum))																		GetGLFunc("glStencilOp");

  // glTextCoord variants
	this->glTexCoord1d = (void(GLAPIENTRY*)(GLdouble))									GetGLFunc("glTexCoord1d");
	this->glTexCoord1f = (void(GLAPIENTRY*)(GLfloat))									GetGLFunc("glTexCoord1f");
	this->glTexCoord1i = (void(GLAPIENTRY*)(GLint))										GetGLFunc("glTexCoord1i");
	this->glTexCoord1s = (void(GLAPIENTRY*)(GLshort))									GetGLFunc("glTexCoord1s");
	this->glTexCoord2d = (void(GLAPIENTRY*)(GLdouble, GLdouble))						GetGLFunc("glTexCoord2d");
	this->glTexCoord2f = (void(GLAPIENTRY*)(GLfloat, GLfloat))							GetGLFunc("glTexCoord2f");
	this->glTexCoord2i = (void(GLAPIENTRY*)(GLint, GLint))								GetGLFunc("glTexCoord2i");
	this->glTexCoord2s = (void(GLAPIENTRY*)(GLshort, GLshort))							GetGLFunc("glTexCoord2s");
	this->glTexCoord3d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))				GetGLFunc("glTexCoord3d");
	this->glTexCoord3f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))					GetGLFunc("glTexCoord3f");
	this->glTexCoord3i = (void(GLAPIENTRY*)(GLint, GLint, GLint))						GetGLFunc("glTexCoord3i");
	this->glTexCoord3s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort))					GetGLFunc("glTexCoord3s");
	this->glTexCoord4d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("glTexCoord4d");
	this->glTexCoord4f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))		GetGLFunc("glTexCoord4f");
	this->glTexCoord4i = (void(GLAPIENTRY*)(GLint, GLint, GLint, GLint))				GetGLFunc("glTexCoord4i");
	this->glTexCoord4s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort, GLshort))		GetGLFunc("glTexCoord4s");
	this->glTexCoord1dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glTexCoord1dv");
	this->glTexCoord1fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glTexCoord1fv");
	this->glTexCoord1iv = (void(GLAPIENTRY*)(const GLint*))								GetGLFunc("glTexCoord1iv");
	this->glTexCoord1sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glTexCoord1sv");
	this->glTexCoord2dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glTexCoord2dv");
	this->glTexCoord2fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glTexCoord2fv");
	this->glTexCoord2iv = (void(GLAPIENTRY*)(const GLint*))								GetGLFunc("glTexCoord2iv");
	this->glTexCoord2sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glTexCoord2sv");
	this->glTexCoord3dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glTexCoord3dv");
	this->glTexCoord3fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glTexCoord3fv");
	this->glTexCoord3iv = (void(GLAPIENTRY*)(const GLint*))								GetGLFunc("glTexCoord3iv");
	this->glTexCoord3sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glTexCoord3sv");
	this->glTexCoord4dv = (void(GLAPIENTRY*)(const GLdouble*))							GetGLFunc("glTexCoord4dv");
	this->glTexCoord4fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glTexCoord4fv");
	this->glTexCoord4iv = (void(GLAPIENTRY*)(const GLint*))								GetGLFunc("glTexCoord4iv");
	this->glTexCoord4sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glTexCoord4sv");
  //-------------------------

	this->glTexCoordPointer = (void(GLAPIENTRY*)(GLint, GLenum, GLsizei , const GLvoid*))														GetGLFunc("glTexCoordPointer");
	this->glTexEnvf = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat))																				GetGLFunc("glTexEnvf");
	this->glTexEnvi = (void(GLAPIENTRY*)(GLenum, GLenum, GLint))																				GetGLFunc("glTexEnvi");
	this->glTexEnvfv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLfloat*))																		GetGLFunc("glTexEnvfv");
	this->glTexEnviv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLint*))																		GetGLFunc("glTexEnviv");
	this->glTexGend = (void(GLAPIENTRY*)(GLenum, GLenum, GLdouble))																				GetGLFunc("glTexGend");
	this->glTexGenf = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat))																				GetGLFunc("glTexGenf");
	this->glTexGeni = (void(GLAPIENTRY*)(GLenum, GLenum, GLint))																				GetGLFunc("glTexGeni");
	this->glTexGendv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLdouble*))																		GetGLFunc("glTexGendv");
	this->glTexGenfv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLfloat*))																		GetGLFunc("glTexGenfv");
	this->glTexGeniv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLint*))																		GetGLFunc("glTexGeniv");
	this->glTexImage1D = (void(GLAPIENTRY*)(GLenum, GLint level, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*))							GetGLFunc("glTexImage1D");
	this->glTexImage2D = (void(GLAPIENTRY*)(GLenum, GLint level, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))				GetGLFunc("glTexImage2D");
	this->glTexImage3D = (void(GLAPIENTRY*)(GLenum, GLint level, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))		GetGLFunc("glTexImage3D");
	this->glTexParameterf = (void(GLAPIENTRY*)(GLenum, GLenum, GLfloat))																		GetGLFunc("glTexParameterf");
	this->glTexParameteri = (void(GLAPIENTRY*)(GLenum, GLenum, GLint))																			GetGLFunc("glTexParameteri");
	this->glTexParameterfv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLfloat*))																GetGLFunc("glTexParameterfv");
	this->glTexParameteriv = (void(GLAPIENTRY*)(GLenum, GLenum, const GLint*))																	GetGLFunc("glTexParameteriv");
	this->glTexSubImage1D = (void(GLAPIENTRY*)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*))									GetGLFunc("glTexSubImage1D");
	this->glTexSubImage2D = (void(GLAPIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))					GetGLFunc("glTexSubImage2D");
	this->glTexSubImage3D = (void(GLAPIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))	GetGLFunc("glTexSubImage3D");
	this->glTranslated = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))																		GetGLFunc("glTranslated");
	this->glTranslatef = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))																			GetGLFunc("glTranslatef");

  //glVertex variants
	this->glVertex2d = (void(GLAPIENTRY*)(GLdouble, GLdouble))						GetGLFunc("glVertex2d");
	this->glVertex2f = (void(GLAPIENTRY*)(GLfloat, GLfloat))						GetGLFunc("glVertex2f");
	this->glVertex2i = (void(GLAPIENTRY*)(GLint, GLint))							GetGLFunc("glVertex2i");
	this->glVertex2s = (void(GLAPIENTRY*)(GLshort, GLshort))						GetGLFunc("glVertex2s");
	this->glVertex3d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble))			GetGLFunc("glVertex3d");
	this->glVertex3f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat))				GetGLFunc("glVertex3f");
	this->glVertex3i = (void(GLAPIENTRY*)(GLint, GLint, GLint))						GetGLFunc("glVertex3i");
	this->glVertex3s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort))				GetGLFunc("glVertex3s");
	this->glVertex4d = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("glVertex4d");
	this->glVertex4f = (void(GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))		GetGLFunc("glVertex4f");
	this->glVertex4i = (void(GLAPIENTRY*)(GLint, GLint, GLint, GLint))				GetGLFunc("glVertex4i");
	this->glVertex4s = (void(GLAPIENTRY*)(GLshort, GLshort, GLshort, GLshort))		GetGLFunc("glVertex4s");
	this->glVertex2dv = (void(GLAPIENTRY*)(const GLdouble*))						GetGLFunc("glVertex2dv");
	this->glVertex2fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glVertex2fv");
	this->glVertex2iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glVertex2iv");
	this->glVertex2sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glVertex2sv");
	this->glVertex3dv = (void(GLAPIENTRY*)(const GLdouble*))						GetGLFunc("glVertex3dv");
	this->glVertex3fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glVertex3fv");
	this->glVertex3iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glVertex3iv");
	this->glVertex3sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glVertex3sv");
	this->glVertex4dv = (void(GLAPIENTRY*)(const GLdouble*))						GetGLFunc("glVertex4dv");
	this->glVertex4fv = (void(GLAPIENTRY*)(const GLfloat*))							GetGLFunc("glVertex4fv");
	this->glVertex4iv = (void(GLAPIENTRY*)(const GLint*))							GetGLFunc("glVertex4iv");
	this->glVertex4sv = (void(GLAPIENTRY*)(const GLshort*))							GetGLFunc("glVertex4sv");
  //-----------------------
 
	this->glVertexPointer = (void(GLAPIENTRY*)(GLint, GLenum, GLsizei, const GLvoid*)) GetGLFunc("glVertexPointer");
	this->glViewport = (void(GLAPIENTRY*)(GLint, GLint, GLsizei, GLsizei)) GetGLFunc("glViewport");

	this->glDisableClientState = (void(GLAPIENTRY*)(GLenum))GetGLFunc("glDisableClientState");


	// ---- Vertex Buffer Objects ----
	this->glBindBufferARB =		(void(GLAPIENTRY*) (GLenum, GLuint))						GetGLFunc("glBindBufferARB");
	this->glDeleteBuffersARB =	(void(GLAPIENTRY*) (GLsizei, const GLuint*))				GetGLFunc("glDeleteBuffersARB");
	this->glGenBuffersARB =		(void(GLAPIENTRY*) (GLsizei, GLuint*))						GetGLFunc("glGenBuffersARB");
	this->glBufferDataARB =		(void(GLAPIENTRY*) (GLenum, int, const GLvoid*, GLenum))	GetGLFunc("glBufferDataARB");
	this->glMapBuffersARB =		(void*(GLAPIENTRY*)(GLenum, GLenum))							GetGLFunc("glMapBuffersARB");
	this->glUnmapBufferARB =	(bool(GLAPIENTRY*)(GLenum))									GetGLFunc("glUnmapBufferARB");
	
	
	
	

  // *************************************  GLU Functions ************************************* 
  // only the ones used thus far are here
  
	//this->gluPerspective = (void(GLAPIENTRY*)(GLdouble, GLdouble, GLdouble, GLdouble))	GetGLFunc("gluPerspective"); 

	
	// ************************************* GL extensions ************************************* 
	SupportVBO = IsExtensionSupported("GL_ARB_vertex_buffer_object");

}