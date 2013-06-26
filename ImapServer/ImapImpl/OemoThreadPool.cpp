
#pragma warning( disable : 4786 )

#include "OemoThreadPool.h"
#include <sstream>

using namespace OemoImapProtocolImpl;
using std::ostringstream;

OemoThreadPool::OemoThreadPool (void)
  : active_threads_ (0)
{
	//
}

int OemoThreadPool::start (int pool_size)
{
  return this->activate ( THR_NEW_LWP | THR_DETACHED , pool_size );
}

int OemoThreadPool::stop (void)
{
  
  int counter = active_threads_.value ();

 
  while (counter--)
  {
    this->enqueue ( 0L );
  }
  
  while ( active_threads_.value () )
  {
    SleepEx( 250 , FALSE );
  }

  
  return( 0L );
}

int OemoThreadPool::enqueue (ACE_Event_Handler *handler)
{
 
  void *v_data = (void *) handler;
  char *c_data = (char *) v_data;

  ACE_Message_Block *mb;

  
  ACE_NEW_RETURN (mb,
                  ACE_Message_Block (c_data),
                  -1);

  
  if (this->putq (mb) == -1)
  {
  
	  mb->release ();

	  return -1;
  }

  return ( 0L );
}

int OemoThreadPool::svc (void)
{
  

  ACE_Message_Block *mb;  

  if (ACE_Thread_Manager::instance ()->testcancel (ACE_Thread::self ()) != 0)
  {
	  ACE_DEBUG ((LM_DEBUG,
		  "(%t) has been cancelled" ));

	  return ( 0L );
  }  

  Counter_Guard counter_guard (active_threads_);

   while (this->getq (mb) != -1)
  {
     
      Message_Block_Guard message_block_guard (mb);

      
      char *c_data = mb->base ();

      
      if (c_data)
      {
         
          void *v_data = (void *) c_data;

          ACE_Event_Handler *handler = (ACE_Event_Handler *) v_data;

          
          if (handler->handle_input (ACE_INVALID_HANDLE) == -1)
          {
             
              handler->handle_close (handler->get_handle (), 0);
              
          }
      }
      else
	  {       
          return ( 0L );		// Ok, shutdown request
	  }     
  }

  return ( 0L );
}

