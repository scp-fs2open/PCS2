//****************************************************************************
//	pcs2_filethread.cpp
//	POF Constructor Suite 2
//	
//****************************************************************************

/*
 * $Logfile: /pcs2_filethread.cpp $
 * $Revision: 1.23 $
 * $Date: 2010/05/22 01:54:28 $
 * $Author: kazan $
 *
 * $Log: pcs2_filethread.cpp,v $
 * Revision 1.23  2010/05/22 01:54:28  kazan
 * Merge Collada code from Spicious
 *
 * Revision 1.22  2008/11/13 00:43:42  kazan
 * Just need to debug some issues with the new progress system, otherwise it's good
 *
 * Revision 1.21  2008/11/10 19:04:08  kazan
 * Partial commit of new multithreaded progress message passing system
 *
 * Revision 1.20  2008/05/15 22:49:37  kazan
 * *** empty log message ***
 *
 * Revision 1.19  2008/05/15 22:15:56  kazan
 * fix a crash
 *
 * Revision 1.18  2008/02/12 18:59:53  kazan
 * Disable ASE after more trying to get it working.  Format found to be unreliable.
 *
 * Revision 1.17  2008/02/11 13:56:23  kazan
 * tweaks
 *
 * Revision 1.16  2008/02/10 01:24:21  kazan
 * More work on the ASE loader
 *
 * Revision 1.15  2008/02/09 23:01:45  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.14  2008/02/09 22:31:26  kazan
 * finish removing string
 *
 * Revision 1.13  2008/02/09 22:28:09  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.12  2007/10/22 23:43:55  kazan
 * Catch a new error condition (thanks for TrashMan for being an airhead), fix crash bug introduced by my replacing POFstring with std::string (thanks to me for being an airhead)
 *
 * Revision 1.11  2007/07/12 08:34:20  bobboau
 * fixed our OGL extention problem, FE_GR_ogl_functable is no longer needed, I used GLee
 *
 * implemented glowmapping
 *
 * Revision 1.10  2007/07/10 20:48:24  kazan
 * Implement PMF -> COB
 *
 * Revision 1.9  2007/06/30 14:13:13  kazan
 * put back in a little safer version of the contained.size() == 2 optimization
 * potentially fix pof shields
 * add error message for group with no polymodels
 *
 * Revision 1.8  2007/06/27 00:01:15  kazan
 * trying to squash bugs
 *
 * Revision 1.7  2007/06/14 23:48:09  kazan
 * various tweaks - incremented PMF version to 102
 *
 * Revision 1.6  2007/06/05 06:29:24  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.5  2007/05/08 02:40:45  kazan
 * bug fixes, error messages
 *
 * Revision 1.4  2007/03/25 17:04:14  kazan
 * Add missing function implementations that weren't in COBHandler.cpp (where they should have been) in PCS1
 * Remove some legacy code that was commented out
 * Remove legacy code for FOF files (Ferrium Object File) as this format is no longer planned to exist
 * Remove obnoxious "COB loading not supported" message that was fibbing to users and linkup cob load
 * Make COB loading scaling config setting work
 * remove duplicate free code
 * Make .SCN recognized as a .COB
 *
 * Revision 1.3  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.2  2005/09/05 23:00:16  kazan
 * some sleeps to make the threads sync
 *
 * Revision 1.1  2005/08/30 11:40:37  kazan
 * forgot to add these
 *
 *
 *
 */

#pragma warning(disable:4786)

#include "GLee.h"
#include "pcs2_filethread.h"
#include "pcs2.h"
//#include <GL/gl.h>
#include <GL/glu.h>


wxPCS2SaveThread::wxPCS2SaveThread(PCS_Model* ship, std::string filename, AsyncProgress* _progress) 
	: model(ship), progress(_progress)
{
	target_file = filename;
}



void *wxPCS2SaveThread::Entry()
{

	wxSleep(1);
	std::string selfile = target_file;
	std::string comp = strLower(selfile);
	//(mypanel)->SignalModelChange(fdlg->GetPath().c_str());
	wxMessageDialog *pop = NULL;
	int err;
	if (strstr(comp.c_str(), ".pmf") != NULL)
	{
		err = model->SaveToPMF(selfile, progress);

		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("Could not open file for writing");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
	}
	else if (strstr(comp.c_str(), ".pof") != NULL)
	{
		err = model->SaveToPOF(selfile, progress);

		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("POF Save Failed");
					break;

				case 2:
					progress->setMessage("BSP Compilation Error Occured");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}

	}
	else if (strstr(comp.c_str(), ".cob") != NULL)
	{
		double scale;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/convoptions/"));
		pConfig->Read(_("cobscale"), &scale, 1.00);

		err = model->SaveToCOB(selfile, progress, float(scale));
		
		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("COB Save Failed");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
	}
	/* format found to be unreliable
	else if (strstr(comp.c_str(), ".ase") != NULL)
	{
		double scale;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/convoptions/"));
		pConfig->Read("cobscale", &scale, 1.00);

		err = model->SaveToASE(selfile, *comp_target, *comp_current, comp_msg_str, float(scale));
		comp_current=comp_target;

		
		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("ASE Save Failed");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
	}*/
	else if (strstr(comp.c_str(), ".dae") != NULL)
	{
		// get helper settings from config
		int helpers, props_as_helpers;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/collada_options/"));
		pConfig->Read(_("export_helpers"), &helpers, 1);
		pConfig->Read(_("export_properties_as_helpers"), &props_as_helpers, 0);


		err = model->SaveToDAE(selfile, progress, helpers, props_as_helpers);

		if (err != 0)
		{
			switch (err)
			{
				case 1:
					progress->setMessage("Collada Save Failed");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}

	}
	else
	{
		pop = new wxMessageDialog(NULL, _("You have selected a non-supported file format"), 
				_("Error"), wxOK);
		pop->ShowModal();
	}

	if (pop)
		delete pop;

	progress->Finish();
	return NULL;
}

//********************************************************************

wxPCS2OpenThread::wxPCS2OpenThread(PCS_Model* ship, std::string filename, AsyncProgress* _progress, bool _owns_progress) 
	: model(ship), progress(_progress), owns_progress(_owns_progress)
{
	target_file = filename;
}


void *wxPCS2OpenThread::Entry()
{
	wxSleep(1);

	std::string selfile = target_file;
	std::string comp = strLower(selfile);

	wxMessageDialog *pop = NULL;
	int err;

	if (strstr(comp.c_str(), ".pmf") != NULL)
	{
		//model->Reset(); makes an openGL call - no longer thread safe
		err = model->LoadFromPMF(selfile, progress);

		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("Could not open file");
					break;
					
				case 2:
					progress->setMessage("File Signature is not PMF");
					break;
					
				case 3:
					progress->setMessage("Unsupported PMF Version");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
				
	}
	else if (strstr(comp.c_str(), ".pof") != NULL)
	{
		//model->Reset(); makes an openGL call - no longer thread safe
		err = model->LoadFromPOF(selfile, progress);

		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case 1:
					progress->setMessage("Could not open file");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}


	}
	else if (strstr(comp.c_str(), ".cob") != NULL || strstr(comp.c_str(), ".scn") != NULL)
	{
		// get scale setting from config
		double scale;
		int itemp;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/convoptions/"));
		pConfig->Read(_("cobscale"), &scale, 1.00);
		pConfig->Read(_("use_geometry_filter"), &itemp, 1); // default to on

		err = model->LoadFromCOB(selfile, progress, scale, itemp == 1);

		// write an error msg into comp_msg_str
		if (err != 0)
		{
			progress->setError(err);
			progress->EarlyTerminate();
			switch (err)
			{
				case -1:
					progress->setMessage("No groups defined");
					break;

				case -2:
					progress->setMessage("Shield mesh is not triangluated");
					break;

				case -3:
					progress->setMessage("Insig mesh is not triangulated");
					break;

				case -4:
					progress->setMessage("Polygon > 20 points");
					break;

				case -5:
					progress->setMessage("Geometry group contained no polymodels");
					break;

				case -6:
					progress->setMessage("No Detail Geometry Present (Detail Geometry name cannot match \"insig*\", \"shield*\" or \"debris*\").");

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
	}
	
	/* format found to be unreliable
	else if (strstr(comp.c_str(), ".ase") != NULL)
	{
		// get scale setting from config
		double scale;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/convoptions/"));
		pConfig->Read("cobscale", &scale, 1.00);

		// do eeet
		model->Reset();
		err = model->LoadFromASE(selfile, *comp_target, *comp_current, comp_msg_str, float(scale));

		// write an error msg into comp_msg_str
		if (err != 0)
		{
			switch (err)
			{
				case 1:
					comp = comp_msg_str;
					sprintf(comp_msg_str, "ASE Loader threw an exception: %s", comp.c_str());
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}
	}
	*/
	else if (strstr(comp.c_str(), ".dae") != NULL)
	{
		int temp;
		bool x = false, y = false, z = false;
		wxConfigBase *pConfig = wxConfigBase::Get();
		pConfig->SetPath(_T("/collada_options/"));
		pConfig->Read(_("mirror_x"), &temp, 0);
		if (temp) x = true;
		pConfig->Read(_("mirror_y"), &temp, 0);
		if (temp) y = true;
		pConfig->Read(_("mirror_z"), &temp, 0);
		if (temp) z = true;


		//model->Reset(); makes an openGL call - no longer thread safe
		err = model->LoadFromDAE(selfile, progress, x, y, z);

		if (err != 0)
		{
			switch (err)
			{
				case 1:
					progress->setMessage("Could not open file");
					break;

				case 2:
					progress->setMessage("No detail0 found");
					break;

				default:
					progress->setMessage("No error message exists for this error, bitch at kazan!");
					break;
			}
		}


	}
	else
	{
		pop = new wxMessageDialog(NULL, _("You have selected a non-supported file format"), 
				_("Error"), wxOK);
		pop->ShowModal();
	}

	if (pop)
		delete pop;

	progress->Finish();
	return NULL;
}
