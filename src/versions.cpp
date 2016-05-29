/*
file for holding version information or anyhthing that MUST be recompiled every time
for example anything that uses the __DATE__ or __TIME__ macros
it's sort of hacked into the project's preprocess step to compile this file 
if it's been modified or not. this will ensure the date and time in the title bar is acurate
*/

#include <wx/wx.h>
#include "pcs2.h"

wxString _BUILD_TIME_ = wxString(__DATE__, wxConvUTF8) + _(" ") + wxString(__TIME__, wxConvUTF8);

#ifdef PUBLIC_RELEASE
wxString BUILD_TYPE = _("Stable");
wxString BUILD_VER = _("2.0");
#else
wxString BUILD_TYPE = _("Alpha");
wxString BUILD_VER = _("2.1");
#endif

#ifdef _DEBUG
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" Debug(") + _BUILD_TIME_ + _(")");
#else

#ifdef PUBLIC_RELEASE
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" (") + _BUILD_TIME_ + _(")");
#else
wxString PCS2_VERSION = _("PCS ") + BUILD_VER + _(" ") + BUILD_TYPE + _(" Release(") + _BUILD_TIME_ + _(")");
#endif

#endif
