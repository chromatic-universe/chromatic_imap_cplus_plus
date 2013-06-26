//--------------------------------------------------------------------------------
//						Implementation for Oemo Listener	  
//								  Inter-Tel , Inc.								  
//								?GNU Public License?							  
//						William K. Johnson - initial version  Dec 2002
//							(C) copyright Intertel-Tel , Inc,
//--------------------------------------------------------------------------------


#pragma warning( disable : 4786 )

#undef _WINSOCKAPI_
#undef _WINSOCK2API_
#ifndef OEMO_IMAP_IMPL_H
#include "OemoImapImpl.h"
#endif


using namespace OemoImapProtocolImpl;

OemoAcceptor::OemoAcceptor (int concurrency)
  : concurrency_ (concurrency),
    the_thread_pool_ (private_thread_pool_) ,
	m_instanceData( NULL )
	
{

	//


}

OemoAcceptor::OemoAcceptor (OemoThreadPool &thread_pool)
  : concurrency_ (thread_pool_),
    the_thread_pool_ (thread_pool)
{

	//

}

OemoAcceptor::~OemoAcceptor()
{
  if ( concurrency() == thread_pool_ && thread_pool_is_private () )
  {
    thread_pool ()->close ();
  }
}

int OemoAcceptor::open (const ACE_INET_Addr &addr,
                       ACE_Reactor *reactor,
					   OemoProtocolDataPtr instance_data ,
					   int pool_size )
{

  Data( instance_data );
  
  if ( concurrency() == thread_pool_ && thread_pool_is_private() )
  {
    thread_pool()->start( pool_size );
  }

  return inherited::open ( addr , reactor );
}

int OemoAcceptor::close()
{
  if ( concurrency() == thread_pool_ && thread_pool_is_private () )
  {
    thread_pool ()->stop();
  }

  return inherited::close();
}

