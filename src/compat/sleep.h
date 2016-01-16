#ifndef SLEEP_INCLUDED
#define SLEEP_INCLUDED

#ifdef WIN32 

#include <windows.h>
#include <wx/msw/winundef.h>
#define SleepFunc Sleep

#else
#define SleepFunc sleep


#endif

#endif //SLEEP_INCLUDED
