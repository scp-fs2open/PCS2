//****************************************************************************
//	pcs2_filethread.h
//	POF Constructor Suite 2
//	
//****************************************************************************

/*
 * $Logfile: /pcs2_filethread.h $
 * $Revision: 1.8 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs2_filethread.h,v $
 * Revision 1.8  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.7  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.6  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.5  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.4  2007/10/18 14:18:55  kazan
 * Improve some error handling, including a critical thread errors causing premature termination
 *
 * Revision 1.3  2007/07/12 08:34:21  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.2  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.1  2005/08/30 11:40:37  kazan
 * forgot to add these
 *
 *
 *
 */

#if !defined(_pcs2_filethread_h_)
#define _pcs2_filethread_h_

#include "GLee.h"

#include <wx/thread.h>
#include "pcs_file.h"
#include <wx/glcanvas.h>
#include "AsyncProgress.h"

class wxPCS2SaveThread : public wxThread
{
	private:
		PCS_Model* model;
		AsyncProgress* progress;
		std::string target_file;

	public:
		wxPCS2SaveThread(PCS_Model* ship, std::string filename, AsyncProgress* _progress);

		virtual void *Entry();

		virtual void OnExit()
		{
			delete progress;
		}
};





class wxPCS2OpenThread : public wxThread
{
	private:
		PCS_Model* model;
		AsyncProgress* progress;
		std::string target_file;
		bool owns_progress;

	public:
		wxPCS2OpenThread(PCS_Model* ship, std::string filename, AsyncProgress* _progress, bool _owns_progress = true);

		virtual void *Entry();

		virtual void OnExit()
		{
			if (owns_progress) {
				delete progress;
			}
		}
};



#endif // _pcs2_rthread_h_

