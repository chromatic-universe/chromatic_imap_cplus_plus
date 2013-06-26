#include "OemoProtocolThread.h"

using namespace OemoImapProtocolImpl;
using std::ostringstream;
using std::cout;
using std::endl;

unsigned long g_dwFinished = 0;

unsigned long __stdcall oipi::OemoProtocolThreadProc( VoidPtr ptrVoid )
{ 
 
	  ACE_Reactor reactor; 
	  OemoAcceptor peer_acceptor( thread_per_connection_ );
	  unsigned long  wait_time = 1L;  

	  //initialize timer
	  ACE_High_Res_Timer::global_scale_factor();
	  reactor.timer_queue()->gettimeofday( &ACE_High_Res_Timer::gettimeofday_hr );	  

	  OemoProtocolDataPtr ptrData = reinterpret_cast<OemoProtocolDataPtr> ( ptrVoid );	
	  
	  peer_acceptor.thread_pool( ACE_Thread_Manager::instance() );
  
	  if ( peer_acceptor.open ( ACE_INET_Addr ( ptrData->Port() ),
												&reactor , 												   
												ptrData ,
												ptrData->ThreadPoolSize() ) == -1 )
	  {
		  
		ACE_ERROR_RETURN ((LM_ERROR,
						   "%p\n",
						   "open"),
						  -1);
	  }	  

 
	  ACE_DEBUG ((LM_DEBUG,
				  "Oemo Imap Message Server  2007 Overtone Corp. All rights reserved \n( Port:%P | ThreadId:%t )\015\012"));
  
	  cout << "Listening on port " << ptrData->Port() << ":oemo" << endl; 
	  cout << "Oemo Imap Server operating in concurency mode=thread_per_connection......\015\012";  
	  
	  ostringstream ostr;

	  while ( ! g_dwFinished )
	  {	  

		  reactor.handle_events ( ACE_Time_Value( wait_time ) );			
	  }  	  	  
	  

	  ACE_Thread_Manager::instance()->cancel_all(); 
	  ACE_Time_Value timeout( 5 ); 

	  if ( ACE_Thread_Manager::instance()->wait( &timeout , 1 ) == -1 )
	  {
		  ACE_ERROR_RETURN( ( LM_ERROR , "(%t) wait() failed\n") , 1 );
	  }

	  ACE_DEBUG ((LM_DEBUG,
				  "(%P|%t) shutting down server daemon\n"));	   


	return ( 0 );


}

#if !defined(ACE_HAS_GNU_REPO)
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <OemoHandler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <OemoHandler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
#endif /* ACE_HAS_GNU_REPO */



