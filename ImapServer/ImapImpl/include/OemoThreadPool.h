#pragma warning( disable: 4786 4800 4365 )
#ifndef OEMO_THREAD_POOL_H
#define OEMO_THREAD_POOL_H


#include "ace/Task.h"
#include "ace/Atomic_Op.h"
#include "ace/Acceptor.h"

namespace OemoImapProtocolImpl
{
	//class thread_pool
	class OemoThreadPool : public ACE_Task<ACE_MT_SYNCH>
	{
		public:

		  typedef ACE_Task<ACE_MT_SYNCH> inherited;

		  enum size_t
		  {
			default_pool_size_ = 5
		  };
  
		  OemoThreadPool (void);
		  int start (int pool_size = default_pool_size_);
		  virtual int stop (void);
		  int enqueue (ACE_Event_Handler *handler);
		  typedef ACE_Atomic_Op<ACE_Mutex, int> counter_t;

		protected:

		  int svc (void);

		  counter_t active_threads_;

	};	

	//class Counter_Guard
	class Counter_Guard
	{
		public:
		  Counter_Guard ( OemoThreadPool::counter_t &counter)
			: counter_ (counter)
		  {
			++counter_;
		  }

		  ~Counter_Guard (void)
		  {
			--counter_;
		  }

		protected:
		  OemoThreadPool::counter_t &counter_;
	};

	//class Message_Block_Guard
	class Message_Block_Guard
	{
		public:
		  Message_Block_Guard (ACE_Message_Block *&mb)
			: mb_ (mb)
		  {
		  }

		  ~Message_Block_Guard (void)
		  {
			mb_->release ();
		  }

		protected:

		  ACE_Message_Block *&mb_;
	};		


}

#endif //OEMO_THREAD_POOL_H