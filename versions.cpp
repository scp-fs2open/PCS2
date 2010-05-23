/*
file for holding version information or anyhthing that MUST be recompiled every time
for example anything that uses the __DATE__ or __TIME__ macros
it's sort of hacked into the project's preprocess step to compile this file 
if it's been modified or not. this will ensure the date and time in the title bar is acurate
*/

#include <wx/wx.h>
#include "pcs2.h"

wxString _BUILD_TIME_ = wxString(__DATE__) + " " + wxString(__TIME__);

extern wxString BUILD_TYPE;
extern wxString BUILD_VER;

#ifdef _DEBUG
wxString PCS2_VERSION = "PCS " + BUILD_VER + " " + BUILD_TYPE + " Debug(" + _BUILD_TIME_ + ")";
#else

#ifdef PUBLIC_RELEASE
wxString PCS2_VERSION = "PCS " + BUILD_VER + " " + BUILD_TYPE + " (" + _BUILD_TIME_ + ")";
#else
wxString PCS2_VERSION = "PCS " + BUILD_VER + " " + BUILD_TYPE + " Release(" + _BUILD_TIME_ + ")";
#endif

#endif
