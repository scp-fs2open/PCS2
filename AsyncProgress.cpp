/***************************************************************************
   AsyncProgress.cpp  -  Asynchronus Progress Information passing interface
                             -------------------

    begin                : Nov 10, 2008
    copyright            : (C) 2008 by Derek Meek
    email                : kazan@cs.iastate.edu

	Author(s)			 : Derek Meek
 ***************************************************************************/

#include "AsyncProgress.h"


#if defined(ASYNCPROG_HAVE_WX_WIDGETS)
const wxEventType wxAsyncProgressStartEvtType = wxNewEventType();
const wxEventType wxAsyncProgressEndEvtType = wxNewEventType();
const wxEventType wxAsyncProgressUpdateEvtType = wxNewEventType();
#endif