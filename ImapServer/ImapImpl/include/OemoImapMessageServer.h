
#ifndef OEMO_PROTOCOL_MESSAGE_SERVER_H_
#define OEMO_PROTOCOL_MESSAGE_SERVER_H_

#include "ace/Event_Handler.h"
#include "ace/NT_Service.h"
#include "ace/Singleton.h"
#include "ace/Registry.h"
#include "ace/Mutex.h"
#include "ace/Trace.h"
#include <string>
#include <map>

typedef std::map<std::string,std::string> RegParams;

class OemoMessageServerService : public ACE_NT_Service
{
	public:

	  OemoMessageServerService (void);

	  virtual void handle_control (DWORD control_code);
	  // We override <handle_control> because it handles stop requests
	  // privately.

	  virtual int  handle_exception (ACE_HANDLE h);
	  // We override <handle_exception> so a 'stop' control code can pop
	  // the reactor off of its wait.

	  virtual int svc (void);
	  // This is a virtual method inherited from ACE_NT_Service.

	  virtual int handle_timeout (const ACE_Time_Value& tv,
								  const void *arg = 0);
	  // Where the real work is done:

	private:

	  typedef ACE_NT_Service inherited;

	protected:

		 //attributes
		 RegParams m_regParams;
		

	private:

	  int stop_;
	public:
		
		bool ReadParams();
		inline RegParams& Params() { return ( m_regParams ); }	

};

// Define a singleton class as a way to insure that there's only one
// Service instance in the program, and to protect against access from
// multiple threads.  The first reference to it at runtime creates it,
// and the ACE_Object_Manager deletes it at run-down.

typedef ACE_Singleton<OemoMessageServerService, ACE_Mutex> SERVICE;

#endif//OEMO_PROTOCOL_MESSAGE_SERVER_H_
