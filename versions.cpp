/*
file for holding version information or anyhthing that MUST be recompiled every time
for example anything that uses the __DATE__ or __TIME__ macros
it's sort of hacked into the project's preprocess step to compile this file 
if it's been modified or not. this will ensure the date and time in the title bar is acurate
*/

#include <wx/wx.h>
#include "pcs2.h"

wxString _BUILD_TIME_ = wxString(__DATE__, wxConvUTF8) + _(" ") + wxString(__TIME__, wxConvUTF8);

extern wxString BUILD_TYPE;
extern wxString BUILD_VER;


#ifndef UNIX
#ifdef _DEBUG
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" Debug(") + _BUILD_TIME_ + _(")");
#else

#ifdef PUBLIC_RELEASE
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" (") + _BUILD_TIME_ + _(")");
#else
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" Release(") + _BUILD_TIME_ + _(")");
#endif

#endif
#else
wxString PCS2_VERSION = wxString::Format(_("PCS %s (%s)"), BUILD_VER.c_str(), _BUILD_TIME_.c_str());
#endif
