// OemoProtocolMessageServer.cpp
// Overtone Software , Inc. All Rights Reserved 
// Initial Version 18 July 2004 William K. Johnson


#include "ace/Reactor.h"
#include "OemoImapMessageServer.h"
#include "OemoProtocolThread.h"


using namespace OemoImapProtocolImpl;
using std::ostringstream;
using std::string;
using std::cout;
using std::make_pair;

OemoMessageServerService::OemoMessageServerService (void)
{
  // Remember the Reactor instance.
  reactor (ACE_Reactor::instance ());
  
}

// This method is called when the service gets a control request.  It
// handles requests for stop and shutdown by calling terminate ().
// All others get handled by calling up to inherited::handle_control.

void OemoMessageServerService::handle_control (DWORD control_code)
{
  if (control_code == SERVICE_CONTROL_SHUTDOWN
      || control_code == SERVICE_CONTROL_STOP)
    {
      report_status (SERVICE_STOP_PENDING);

      ACE_DEBUG ((LM_INFO,
                  "Service control stop requested\n"));
	  
      stop_ = 1;
      reactor ()->notify (this,
                          ACE_Event_Handler::EXCEPT_MASK);
    }
  else
    inherited::handle_control (control_code);
}

// This is just here to be the target of the notify from above... it
// doesn't do anything except aid on popping the reactor off its wait
// and causing a drop out of handle_events.

int OemoMessageServerService::handle_exception (ACE_HANDLE)
{
  return 0;
}


int OemoMessageServerService::handle_timeout (const ACE_Time_Value &tv,
                         const void *)
{  
  return 0;
}


bool OemoMessageServerService::ReadParams()
{
	   ACE_Trace _( ACE_TEXT( "ReadParams" ) , __LINE__, ACE_TEXT(__FILE__)  );
	   ostringstream ostr;	   



	   int result;
	   ACE_Registry::Naming_Context parent_context;
	   ACE_Registry::Naming_Context application_context;

	   // Connect to predefined entry
	   result = ACE_Predefined_Naming_Contexts::connect ( parent_context,
													     HKEY_LOCAL_MACHINE );

	   ACE_Registry::Name_Component component;	
	   ACE_Registry::Name context;	    

	   component.id_ = ACE_TEXT ("Software"),   context.insert (component);
	   component.id_ = ACE_TEXT ("Overtone Software"),   context.insert (component);
	   component.id_ = ACE_TEXT ("ManageToneEX"),	    context.insert (component);
	   component.id_ = ACE_TEXT ("Parameters"), context.insert (component);
	   component.id_ = ACE_TEXT ("OemoSearchProtocol"), context.insert (component);
	   
	   // Find application context name
	   result = parent_context.resolve_context ( context,
											     application_context );

	   if( result != 0 )
	   {
		   return ( false );
	   }

	  Params().clear();	   	   
	   
	  ACE_Registry::Binding_List list;

      // get the list of all entries
	  result = application_context.list (list);
	  if ( result != 0 )
	  {
		return ( false );
	  }        	 

	  // iterate through all entries
	  for (ACE_Registry::Binding_List::iterator i = list.begin ();
		   i != list.end ();
		   ++i)
		{      
		  ACE_Registry::Binding &binding = *i;
		  

		  if (binding.type () == ACE_Registry::OBJECT)
		  {
			char buffer[256];
			ACE_Registry::Object object ((void *) buffer ,
			       sizeof buffer,
			       REG_SZ );
			
			result = application_context.resolve ( binding.name() , object);
			string str( (char*) object.data() );

			Params().insert( make_pair( binding.name().c_str() , str ) );			

		  }	  
		  
		}  

		return ( true );
	   
}

// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a shutdown
// request is received.

int OemoMessageServerService::svc (void)
{	  

	  unsigned long dwThreadId = 0L;
	  HANDLE handle = NULL;
	  HANDLE thread_handle = NULL;

	  bool bRet = ReadParams();
	  if( ( bRet == false ) || ( Params().empty() ) )
	  {
		  return ( -1 );
	  }

	  // As an NT service, we come in here in a different thread than the
	  // one which created the reactor.  So in order to do anything, we
	  // need to own the reactor. If we are not a service, report_status
	  // will return -1.
	  if (report_status (SERVICE_RUNNING) == 0)
	  {
		reactor ()->owner (ACE_Thread::self ());
	  }

	  stop_ = 0;

	  const auto_ptr<OemoProtocolData>  oemo( new OemoProtocolData( "OemoImapMessageServer" ) );	  
	  
	  OemoImapProtocolImpl::RegParams::iterator iter = Params().find( "DbServer" );
	  if( iter != Params().end() )
	  {
		  oemo->DefaultHost( iter->second );
	  }
	  iter = Params().find( "Port" );
	  if( iter != Params().end() )
	  {
		  oemo->Port( atoi( (char*) iter->second.c_str() ) );
	  }
	  iter = Params().find( "ThreadPoolSize" );
	  if( iter != Params().end() )
	  {
		  oemo->ThreadPoolSize( atoi( (char*) iter->second.c_str() ) );
	  }
	  iter = Params().find( "RemoteStoragePath" );
	  if( iter != Params().end() )
	  {
		  oemo->Dn( iter->second );
	  }	
	  iter = Params().find( "IsysPath" );
	  if( iter != Params().end() )
	  {
		  oemo->IsysPath( iter->second );
	  }	
	  iter = Params().find( "DocumentPath" );
	  if( iter != Params().end() )
	  {
		  oemo->DocumentPath( iter->second );
	  }	 
	  
	  try
	  {		
			if(  handle != INVALID_HANDLE_VALUE )
			{
						thread_handle = CreateThread( NULL ,
													  0 ,
													  (LPTHREAD_START_ROUTINE) OemoProtocolThreadProc ,
													  (void*) oemo.get() ,
													  0 ,
													  &dwThreadId );
			}	  


	  }
	  catch( ... )
	  {
			if( handle != INVALID_HANDLE_VALUE )
			{
				CloseHandle( handle );
			}
			if( thread_handle != INVALID_HANDLE_VALUE )
			{
				CloseHandle( thread_handle );
			}
	  }

	  //process
	  while (!stop_)
	  {
		reactor ()->handle_events ();
	  }	    
	  
	  //exit
	  g_dwFinished = 1; 
	  unsigned long dwWait = 0L;

	  try
	  {		  
		  BOOL b = GetExitCodeThread( thread_handle , &dwWait );
		   if ( b  == TRUE )
		   {
			   if( dwWait == STILL_ACTIVE )
			   {
				dwWait = WaitForSingleObject( thread_handle , 5000 );
			   }
		   }

		  GetExitCodeThread( thread_handle , &dwWait );

		  if( handle != INVALID_HANDLE_VALUE )
		  {
			CloseHandle( handle );
		  }
		  if( thread_handle != INVALID_HANDLE_VALUE )
		  {
			CloseHandle( thread_handle );
		  }
	  }
	  catch( ... )
	  {
		  //
	  }

	  // Cleanly terminate connections, terminate threads.
	  ostringstream ostr;
	  ostr << "Message Server thread exited with code #" << dwWait << "\n";

	  ACE_DEBUG ((LM_DEBUG,
				  (char*) ostr.str().c_str() ));
	  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Singleton<Service, ACE_Mutex>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Singleton<Service, ACE_Mutex>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
