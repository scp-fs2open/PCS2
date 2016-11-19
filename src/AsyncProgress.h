/***************************************************************************
   AsyncProgress.h  -  Asynchronus Progress Information passing interface
                             -------------------

    begin                : Nov 10, 2008
    copyright            : (C) 2008 by Derek Meek
    email                : kazan@cs.iastate.edu

	Author(s)			 : Derek Meek
 ***************************************************************************/

// define this to enable the wxwidgets derived class of AsyncProgress
#define ASYNCPROG_HAVE_WX_WIDGETS


#if !defined(_aysncprogress_h_)
#define _aysncprogress_h_
#include <string>

class AsyncProgress
{
	/*
	This class is designed following a pattern in which the
	interface is specified by non-virtual public functions
	this makes the interrupt immutable by derviced classes,
	it also allows prereq/postreq checks to only have to be implemented once

	the public non-virtual functions call the private virtual functions which
	actually perform the behavior

	Article link <if i ever find it again>: 
	*/
	public:
		// Sets/Gets the completion target number
		void setTarget(unsigned int target) { this->real_setTarget(target); }
		unsigned int getTarget() { return this->real_getTarget(); }

		// Gets the completion target number as a percent
		float getTargetPercent() { return this->real_getTargetPercent(); }

		// Set/Get/Update completion progress number
		void SetProgress(unsigned int progress) { this->real_SetProgress(progress); }
		unsigned int getProgress() { return this->real_getProgress(); }

		// These two functions generate automatic notifications
		// One simply increments the progress, the other sets the message as well
		void incrementWithMessage(std::string msg) { this->real_incrementProgress(); this->setMessage(msg); this->Notify(); }
		void incrementProgress() { this->real_incrementProgress(); this->Notify(); }
		
		// The Interupt signal should be interpreted by the thread that is
		// performing the progress-tracked operation as meaning it should abort
		void Interrupt() { this->real_Interrupt(); } // sets the interrupt flag to true
		bool Interrupted() { return this->real_Interrupted(); } // returns the interrupt flag state

		// The Early Terminated signal should be interpreted by the thread that is monitoring the
		// progress-tracked operation that the operation has terminated prematurely
		void EarlyTerminate() { this->real_EarlyTerminate(); } // sets the terminated flag to true
		bool EarlyTerminated() { return this->real_EarlyTerminated(); } //gets the terminated flag state

		// The Finished signal tells us that the process finished succesfully
		bool Finished() { return this->real_Finished(); }
		void Finish() { this->real_Finish(); } 

		// Progress message
		void setMessage(std::string msg) { this->real_setMessage(msg); }
		std::string getMessage() { return this->real_getMessage(); }

		// Error Codes
		void setError(int errorcode) { this->real_setError(errorcode); }
		int getError() { return this->real_getError(); }

		// Notifies the thread that is monitoring the progress-tracked operation that
		// the target/progress has been changed
		void Notify() { this->real_Notify(); }

		virtual ~AsyncProgress() { }

	private:
		virtual void real_setTarget(unsigned int target)=0;
		virtual unsigned int real_getTarget()=0;
		virtual float real_getTargetPercent()=0;
		virtual void real_SetProgress(unsigned int progress)=0;
		virtual unsigned int real_getProgress()=0;
		virtual void real_incrementProgress()=0;
		virtual void real_Interrupt()=0;
		virtual bool real_Interrupted()=0;
		virtual void real_Notify()=0;
		virtual void real_EarlyTerminate()=0;
		virtual bool real_EarlyTerminated()=0;
		virtual void real_setMessage(std::string &msg)=0;
		virtual std::string real_getMessage()=0; 
		virtual void real_setError(int errorcode)=0;
		virtual int real_getError()=0;
		virtual bool real_Finished()=0;
		virtual void real_Finish()=0;
};


//*********************************************************************************************
// wxWidgets deriviative - sends events to the monitoring event handler
//*********************************************************************************************

#if defined(ASYNCPROG_HAVE_WX_WIDGETS)

#include <wx/wx.h>
#include <wx/setup.h>
#include <wx/event.h>
#include <wx/app.h>


// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// notification events
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
extern const wxEventType wxAsyncProgressStartEvtType;
extern const wxEventType wxAsyncProgressEndEvtType;
extern const wxEventType wxAsyncProgressUpdateEvtType;


class wxAsyncProgressStartEvt : public wxCommandEvent
{
	public:
		wxAsyncProgressStartEvt(int id = 0) : wxCommandEvent(wxAsyncProgressStartEvtType, id) {}
		wxAsyncProgressStartEvt(wxEventType commandType, int id) : wxCommandEvent(commandType, id) {}
		wxAsyncProgressStartEvt(const wxAsyncProgressStartEvt &event) : wxCommandEvent(event) { }
		
		wxEvent* Clone() const { return new wxAsyncProgressStartEvt(*this); }

};

// --------------------------------------------------------------------------------------------

class wxAsyncProgressEndEvt : public wxCommandEvent
{
	public:
		wxAsyncProgressEndEvt(int id = 0) : wxCommandEvent(wxAsyncProgressEndEvtType, id) {}
		wxAsyncProgressEndEvt(wxEventType commandType, int id) : wxCommandEvent(commandType, id) {}
		wxAsyncProgressEndEvt(const wxAsyncProgressEndEvt &event) : wxCommandEvent(event) 
			{ 
				errorcode = event.errorcode;
				early_terminated = event.early_terminated;
				msg = event.msg;
			}
		
		wxEvent* Clone() const { return new wxAsyncProgressEndEvt(*this); }

		void setErrorCode(int ec) { errorcode = ec; }
		void setEarlyTerminated(bool et) { early_terminated = et; }
		void setMessage(std::string str) { msg = str; }

		int getErrorCode() { return errorcode; }
		bool getEarlyTerminated() { return early_terminated; }
		std::string getMessage() { return msg; }

	private:
		int errorcode;
		bool early_terminated;
		std::string msg;

};

// --------------------------------------------------------------------------------------------

class wxAsyncProgressUpdateEvt : public wxCommandEvent
{
	public:
		wxAsyncProgressUpdateEvt(int id = 0) : wxCommandEvent(wxAsyncProgressUpdateEvtType, id) {}
		wxAsyncProgressUpdateEvt(wxEventType commandType, int id) : wxCommandEvent(commandType, id) {}
		wxAsyncProgressUpdateEvt(const wxAsyncProgressUpdateEvt &event) : wxCommandEvent(event) 
		{ 
			percent = event.percent; 
			target = event.target; 
			progress = event.progress; 
			msg = event.msg;
		}
		
		wxEvent* Clone() const { return new wxAsyncProgressUpdateEvt(*this); }

		void setPercent(float p) { percent = p; }
		void setTarget(unsigned int t) { target = t; }
		void setProgress(unsigned int p) { progress = p; }
		void setMessage(std::string str) { msg = str; }

		float getPercent() { return percent; }
		unsigned int getTarget() { return target; }
		unsigned int getProgress() { return progress; }
		std::string getMessage() { return msg; }
		

	private:
		float percent;
		unsigned int target;
		unsigned int progress;
		std::string msg;
};

// --------------------------------------------------------------------------------------------
// macros and typedefs
typedef void (wxEvtHandler::*wxAsyncProgressStartEvtFunction)(wxAsyncProgressStartEvt &);

// macro syntax pulled from http://wiki.wxwidgets.org/Custom_Events
#define wxAsyncProgressStartEvtHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) \
	wxStaticCastEvent(wxAsyncProgressStartEvtFunction, &func)

#define EVT_wxAsyncProgressStartEvt(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressStartEvtType, id, wxID_ANY, wxAsyncProgressStartEvtHandler(fn), (wxObject*)NULL),	

#define EVT_EVT_wxAsyncProgressStartEvt_RANGE(id1, id2, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressStartEvtType, id, id2, wxAsyncProgressStartEvtHandler(fn), (wxObject*)NULL),	

// --------------------------------------------------------------------------------------------
typedef void (wxEvtHandler::*wxAsyncProgressEndEvtFunction)(wxAsyncProgressEndEvt &);

// macro syntax pulled from http://wiki.wxwidgets.org/Custom_Events
#define wxAsyncProgressEndEvtHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) \
	wxStaticCastEvent(wxAsyncProgressEndEvtFunction, &func)

#define EVT_wxAsyncProgressEndEvt(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressEndEvtType, id, wxID_ANY, wxAsyncProgressEndEvtHandler(fn), (wxObject*)NULL),	

#define EVT_EVT_wxAsyncProgressEndEvt_RANGE(id1, id2, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressEndEvtType, id, id2, wxAsyncProgressEndEvtHandler(fn), (wxObject*)NULL),	

// --------------------------------------------------------------------------------------------
typedef void (wxEvtHandler::*wxAsyncProgressUpdateEvtFunction)(wxAsyncProgressUpdateEvt &);

// macro syntax pulled from http://wiki.wxwidgets.org/Custom_Events
#define wxAsyncProgressUpdateEvtHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) \
	wxStaticCastEvent(wxAsyncProgressUpdateEvtFunction, &func)

#define EVT_wxAsyncProgressUpdateEvt(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressUpdateEvtType, id, wxID_ANY, wxAsyncProgressUpdateEvtHandler(fn), (wxObject*)NULL),	

#define EVT_EVT_wxAsyncProgressUpdateEvt_RANGE(id1, id2, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxAsyncProgressUpdateEvtType, id, id2, wxAsyncProgressUpdateEvtHandler(fn), (wxObject*)NULL),	

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// progress class
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

class wxAsyncProgress : public AsyncProgress
{
	public:
		wxAsyncProgress(wxEvtHandler *listener, int id = 0, bool _doYieldOnNotify = false) 
			: msgTarget(listener), controlID(id), target(0), progress(0), interrupted(false), terminated(false), errorcode(0), finished(false), doYieldOnNotify(_doYieldOnNotify)
			{
				wxAsyncProgressStartEvt start_event(controlID);
				wxPostEvent(msgTarget, start_event);
				//if (doYieldOnNotify) wxGetApp()->Yield();
			}
		~wxAsyncProgress() 
			{
				wxAsyncProgressEndEvt end_event(controlID);
				end_event.setErrorCode(this->getError());
				end_event.setEarlyTerminated(this->EarlyTerminated());
				end_event.setMessage(this->getMessage());
				wxPostEvent(msgTarget, end_event);
				//if (doYieldOnNotify) wxGetApp()->Yield();
			}

	private:
		wxEvtHandler *msgTarget;
		int controlID;
		unsigned int target;
		unsigned int progress;
		bool interrupted;
		bool terminated;
		std::string message;
		int errorcode;
		bool finished;
		bool doYieldOnNotify;

		virtual void real_setTarget(unsigned int target_in) { this->target = target_in; }
		virtual unsigned int real_getTarget() { return this->target; }

		virtual float real_getTargetPercent() { return 100.00f * (float(this->progress) / float(this->target)); }

		virtual void real_SetProgress(unsigned int progress_in) { this->progress = progress_in; }
		virtual unsigned int real_getProgress() { return this->progress; }
		virtual void real_incrementProgress() { this->progress++; }

		virtual void real_Interrupt() { this->interrupted = true; }
		virtual bool real_Interrupted() { return this->interrupted; }

		virtual void real_EarlyTerminate() { this->terminated = true; }
		virtual bool real_EarlyTerminated() { return this->terminated; }

		virtual bool real_Finished() { return this->finished; }
		virtual void real_Finish() { this->finished = true; }

		virtual void real_setMessage(std::string &msg) { this->message = msg; }
		virtual std::string real_getMessage() { return this->message; }
		
		virtual void real_setError(int errorcode_in) { this->errorcode = errorcode_in; }
		virtual int real_getError() { return this->errorcode; }

		virtual void real_Notify()
		{
			wxAsyncProgressUpdateEvt update_event(controlID);
			update_event.setPercent(this->getTargetPercent());
			update_event.setTarget(this->getTarget());
			update_event.setProgress(this->getProgress());
			update_event.setMessage(this->getMessage());
			wxPostEvent(msgTarget, update_event);
			if (doYieldOnNotify) wxApp::GetInstance()->Yield(true);
		}

};

#endif



#endif // !defined(_aysncprogress_h_)