// OemoAcceptor.h,v 1.5 1999/09/22 03:13:38 jcej Exp

#ifndef OEMO_ACCEPTOR_H
#define OEMO_ACCEPTOR_H

#pragma warning( disable: 4786 4800 4365 )

#include "ace/Acceptor.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#include "ace/SOCK_Acceptor.h"


#include "OemoThreadPool.h"
#include "thread_pool.h"
#include "OemoProtocolData.h"
#include "OemoImapHandler.h"


namespace OemoProtocolImpl
{
	

	//enumerations
	enum concurrency_t
	{
		single_threaded_,
		thread_per_connection_,
		thread_pool_
	};

	typedef ACE_Acceptor <chromatic_handler, ACE_SOCK_ACCEPTOR> chromatic_acceptor_base;

	//class chromatic_acceptor
	class chromatic_acceptor : public chromatic_acceptor_base
	{
		public:

		  typedef chromatic_acceptor_Base inherited;
  
		  chromatic_acceptor (int concurrency = thread_pool_);
		  chromatic_acceptor ( chromatic_thread_pool &thread_pool);
		  ~chromatic_acceptor (void);

		  int open (const ACE_INET_Addr &addr,
					ACE_Reactor *reactor,
					chromatic_protocol_data_ptr instance_data ,
					int pool_size = chromatic_thread_pool::default_pool_size_ );

		  int close (void);		 

		protected:

		  //attributes
		  int concurrency_;
		  chromatic_thread_pool private_thread_pool_;
		  chromatic_thread_pool &the_thread_pool_;
		  chromatic_protocol_data_ptr m_instanceData;

		public:	

		  //accessors
		  inline int concurrency (void) noexcept { return this->concurrency_; }
		  inline oemo_thread_pool* thread_pool (void) { return &this->the_thread_pool_; }
		  int thread_pool_is_private (void) {return &the_thread_pool_ == &private_thread_pool_; }
		  chromatic_protocol_data_ptr  Data() { return ( m_instanceData ); }

		  //mutators
		  inline void thread_pool( ACE_Thread_Manager* thrd_mgr ) { ACE_Task<ACE_MT_SYNCH> ( thr_mgr ); }
		  inline void Data( OemoProtocolDataPtr data ) { m_instanceData = data; }

	};

}

namespace opi = OemoProtocolImpl;
#endif /* OEMO_ACCEPTOR_H */
