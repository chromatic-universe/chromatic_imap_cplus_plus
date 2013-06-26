/*
 * chromatic_acceptor.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: wiljoh
 */


//suppress from adaptive communication environment
#pragma clang diagnostic ignored "-Wself-assign"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"

#include "chromatic_imap.h"

using namespace chromatic_imap_protocol_impl;
using std::ostringstream;

//----------------------------------------------------------------------------------------------------------------------------
 chromatic_acceptor::chromatic_acceptor ( concurrency_t concurrency ) : concurrency_ { concurrency } ,
																        the_thread_pool_ ( private_thread_pool_ )  ,
																		m_instanceData{ nullptr }
 {
	 	 ACE_Trace _( ACE_TEXT( "chromatic_acceptor::chromatic_acceptor ( concurrency_t concurrency )" ) , __LINE__ );
 }

 //----------------------------------------------------------------------------------------------------------------------------
  chromatic_acceptor::chromatic_acceptor ( chromatic_thread_pool &thread_pool ) : concurrency_ { concurrency_t::thread_pool_ },
		    																	  the_thread_pool_ ( thread_pool ) ,
		    																	  m_instanceData{ nullptr }
  {
	  	  ACE_Trace _( ACE_TEXT( "chromatic_acceptor::chromatic_acceptor ( chromatic_thread_pool &thread_pool)" ) , __LINE__ );
  }

  //----------------------------------------------------------------------------------------------------------------------------
   chromatic_acceptor::~chromatic_acceptor ( void )
   {
	   	   ACE_Trace _( ACE_TEXT( "chromatic_acceptor::~chromatic_acceptor ( void )" ) , __LINE__ );

		   if ( concurrency() == concurrency_t::thread_pool_ && thread_pool_is_private () )
		   {
			  thread_pool ()->close ();
		   }
   }

   //----------------------------------------------------------------------------------------------------------------------------
    int chromatic_acceptor::open (	const ACE_INET_Addr &addr,
									ACE_Reactor *reactor,
									chromatic_protocol_data_ptr instance_data ,
									int pool_size )
    {
    	  ACE_Trace _( ACE_TEXT( "int chromatic_acceptor::open" ) , __LINE__ );

    	  Data( instance_data );

    	  if ( concurrency() == concurrency_t::thread_pool_ && thread_pool_is_private() )
    	  {
    	    thread_pool()->start( pool_size );
    	  }

    	  return inherited::open ( addr , reactor );
    }

    //----------------------------------------------------------------------------------------------------------------------------
    int chromatic_acceptor::close ( void )
    {
    	ACE_Trace _( ACE_TEXT( "hromatic_acceptor::close" ) , __LINE__ );

    	if ( concurrency() == concurrency_t::thread_pool_ && thread_pool_is_private () )
    	  {
    	    thread_pool ()->stop();
    	  }

    	  return inherited::close();
    }


