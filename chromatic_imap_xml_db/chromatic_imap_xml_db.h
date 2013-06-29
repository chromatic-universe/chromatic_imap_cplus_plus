//chromatic_imap_xml_db.h
//created on: Jun 28, 2013
//author: William K. Johnson
//c++ wrapper facade for sedna xml db lib

#ifndef CHROMATIC_IMAP_XML_DB_H_
#define CHROMATIC_IMAP_XML_DB_H_

#include <tuple>
#include <stdexcept>
#include <exception>
#include <system_error>
#include <new>
#include <ios>
#include <future>
#include <typeinfo>
#include <iostream>
#include <utility>
#include <string>
#include <memory>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <thread>

//c runtime std namepsace
#include <cstdio>
#include <cerrno>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <climits>
#include <cassert>
//c runtime
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//adaptive communication environment
#include <ace/Task.h>
#include <ace/Atomic_Op.h>
#include <ace/Acceptor.h>
#include <ace/Svc_Handler.h>
#include <ace/Date_Time.h>
#include <ace/Mutex.h>
#include <ace/OS.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Trace.h>
#include <ace/Timer_Queue_T.h>

//sedna
#include <libsedna.h>

//hand rolled
//generic
#include <generic.h>
//utils
#include "chromatic_utils.h"


namespace chromatic_imap_xml_db
{
		//forward

		//enumerations
		enum class chromatic_imap_mgr_xml_level : unsigned long
		{
			imxlCritical = 0 ,
			imxlogic ,
			imxlInformation ,
			imxlDebug ,
			imxlNoError
		};


		//types
		typedef chromatic_imap_mgr_xml_level cimxl;

		//exceptions

		//functors

		//predicates

		//generic

		//helpers
		class sedna_connection
		{
			public:

				//ctors
				explicit sedna_connection();
				sedna_connection( const std::string& addr ,
								  const std::string& database ,
								  const std::string& user ,
								  const std::string& password ,
								  const int sa = SEDNA_AUTOCOMMIT_OFF );



				//dtor
				virtual ~sedna_connection();

				//stream
				friend std::ostream& operator<<( std::ostream& ostr, const sedna_connection& sc );


			protected:

				//attributes
				std::string m_addr;
				std::string m_database;
				std::string m_user;
				std::string m_password;

			private:

				//no copy
				sedna_connection( const sedna_connection& sc );
				//no assign
				const sedna_connection& operator= ( const sedna_connection& sc );

				//attributes
				std::unique_ptr<struct SednaConnection> m_ptr_sedna;

			protected:

				//attributes				s
				int m_se_attr;
				bool m_b_connected;
				int m_last_return_val;

			public:

				//acceptors-inspectors
				inline std::string addr() noexcept { return m_addr; }
				inline std::string database() noexcept { return m_database; }
				inline std::string user() noexcept { return m_user; }
				inline std::string password() noexcept { return m_password; }
				inline int server_attr() noexcept{ return m_se_attr; }
				inline bool is_connected() noexcept { return m_b_connected; }
				inline int last_return_val() { return m_last_return_val; }

				//mutators

				//services
				virtual void init();
				virtual void fini();
				virtual void reset();


				//immutable

		};
		inline std::ostream& operator<<( std::ostream& ostr, const sedna_connection& sc ) { return ostr << "snaps";	} // todo


		//services

		//immutable





}
namespace cixdb = chromatic_imap_xml_db;

#endif //CHROMATIC_IMAP_XML_DB_H_
