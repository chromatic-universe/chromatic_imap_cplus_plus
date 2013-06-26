/*
 * the_linux_programming_interface.h
 *
 *  Created on: May 28, 2013
 *      Author: wiljoh
 */

#ifndef THE_LINUX_PROGRAMMING_INTERFACE_H_
#define THE_LINUX_PROGRAMMING_INTERFACE_H_



//c++ standard
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
#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <unistd.h>
#include <sys/fsuid.h>
#include <sys/stat.h>

//generic
#include <generic.h>

namespace tlpi
{

			//forward
			class tlpi_authentication_assist;
			class tlpi_credentials;
			class tlpi_process_info;

			//enumerations
			enum class pwd_moniker : unsigned long
			{
				pw_name ,
				pw_passwd ,
				pw_uid ,
				pw_gid ,
				pw_gecos ,
				pw_dir ,
				pw_shell
			};

			enum class spwd_moniker : unsigned long
			{
				spw_name ,
				spw_encrypted_pwd ,
				spw_last_change ,
				spw_min_days ,
				spw_max_days ,
				spw_warn_days ,
				spw_inactive_days ,
				spw_expire_date ,
				spw_reserved
			};

			enum class pwd_stat : unsigned long
			{
				ps_no_login_required ,
				ps_shadow_by_default ,
				ps_length_out_of_range ,
				ps_shadow_default_exists_shadow ,
				ps_shadow_default_no_exists_shadow ,
				ps_no_stat
			};




			//types
			typedef tlpi_authentication_assist tas;
			typedef std::map<std::string , struct passwd> user_auth_dictionary;
			typedef user_auth_dictionary* user_auth_dictionary_ptr;
			typedef tlpi_credentials*	tlpi_credentials_ptr;
			typedef std::tuple<std::string ,
							   std::string ,
							   uid_t ,
							   gid_t ,
							   std::string ,
							   std::string ,
							   std::string>	pwd_record_tuple;
			typedef std::tuple<std::string ,
						       std::string ,
						       long ,
						       long ,
						       long ,
						       long ,
						       long ,
						       long ,
						       unsigned long> spwd_record_tuple;
			typedef std::pair<uid_t , gid_t> process_credentials_ids;

			//exception classes
			class user_passwd_info_error : public std::exception
			{
				public:

						const char* what()  const throw()
						{
							return "error in processing user or group information";
						}
						//todo
			};

			//functors

			//generic

			//abstract
			class tlpi_authentication
			{
				public:

					virtual  bool authenticate( const std::string& user_name ) = 0;
					virtual  bool authenticate( const unsigned long& token ) = 0;

					virtual ~tlpi_authentication();
			};

			class tlpi_authentication_base : virtual public tlpi_authentication
			{

				public:

					virtual struct passwd info_from_name( const std::string& user_name ) = 0;
					virtual void populate_info_dictionary() = 0;
					virtual bool passwd_fields( const std::string& user_name ,
												pwd_record_tuple& prt) = 0;
					virtual bool sdwpasswd_fields( const std::string& user_name ,
												         spwd_record_tuple& prt) = 0;

					virtual ~tlpi_authentication_base();
			};

			class tlpi_credentials_base
			{
				public:

					virtual process_credentials_ids process_real_ids() = 0;
					virtual process_credentials_ids process_effective_ids() = 0;

					virtual ~tlpi_credentials_base();
			};



			//helper classes
			class tlpi_credentials  : public tlpi_credentials_base
			{
				public:

					//ctors
					explicit tlpi_credentials();
					tlpi_credentials( const std::string& user_name );

					//dtor
					virtual ~tlpi_credentials();

					//stream
					friend std::ostream& operator<< ( std::ostream& o, const tlpi_credentials& tcs ) noexcept;

					//no heap allocations , copy and assign can be synthesized

				private:

					//attributes
					std::string m_user_name;

				protected:

					//attributes
					process_credentials_ids m_pci_real;
					process_credentials_ids m_pci_effective;

				public:

					//id helpers
					process_credentials_ids process_real_ids();
					process_credentials_ids process_effective_ids();
					void set_process_uid( uid_t uid );
					void set_process_gid( gid_t gid );
					void set_effective_process_uid( uid_t euid ) ;
					void set_effective_process_gid( gid_t egid );
					void set_real_effective_process_uids( uid_t ruid , uid_t euid );
					void set_real_effective_process_gids( gid_t rgid , gid_t egid );
					void set_file_system_uid( uid_t fsuid );
					void set_file_system_gid( gid_t fsgid ) ;

					//services

					//accesors-inspectors
					inline process_credentials_ids real_process_ids() const noexcept { return m_pci_real; }
					inline process_credentials_ids real_effective_ids() const noexcept { return  m_pci_effective; }
					inline std::string user_name() const noexcept { return m_user_name; }

					//mutators
					inline void user_name( const std::string& un ) { m_user_name = un; }

					//immutable

			};
			inline std::ostream& operator<< ( std::ostream& o, const tlpi_credentials& tcs ) noexcept
			{
				//todo
				return o << "snaps";
		    }

			//helper classes
			class tlpi_process_info
			{
				public:

					//ctor
					explicit tlpi_process_info() : m_pid { ::getpid() } ,
												   m_ptr_stat { new ( struct stat ) }
					{}


					//dtor
					virtual ~tlpi_process_info()
					{}

					//copy
					tlpi_process_info( const tlpi_process_info& tpi )
									:  m_pid{ tpi.m_pid } ,
									   m_ptr_stat{  new struct stat( *tpi.m_ptr_stat ) }
					{}

					//assign
					inline const tlpi_process_info& operator= ( const tlpi_process_info& tpi )
					{
						*m_ptr_stat = *tpi.m_ptr_stat;
						return *this;
					}

					//stream
					friend std::ostream& operator<< ( std::ostream& o, const tlpi_process_info& tpi ) noexcept;

				private:

					//

				protected:

					//attributes
					pid_t  m_pid;
					std::unique_ptr<struct stat> m_ptr_stat;

					//services
					inline void init();


				public:

					//accessors-inspectors
					inline pid_t pid() noexcept { return m_pid; }

					//helpers

			};
			inline std::ostream& operator<< ( std::ostream& o, const tlpi_process_info& tpi ) noexcept
			{
				//todo
				return o << "snaps";
			}

			class tlpi_authentication_assist : public tlpi_authentication_base
			{

				public:

					//ctors
					explicit tlpi_authentication_assist();
					tlpi_authentication_assist( user_auth_dictionary_ptr uad );

					//dtor
					virtual ~tlpi_authentication_assist();

					//copy
					tlpi_authentication_assist( const tlpi_authentication_assist& tas )
									: m_ptr_credentials( new tlpi_credentials( *tas.m_ptr_credentials ) )
					{}

					//assign
					inline const tlpi_authentication_assist& operator= ( const tlpi_authentication_assist& tas )
					{
						*m_ptr_credentials = *tas.m_ptr_credentials;
						return *this;
					}

					//stream
					friend std::ostream& operator<< ( std::ostream& o, const tlpi_authentication_assist& tcs ) noexcept;

				private:

					//attributes
					std::unique_ptr<user_auth_dictionary> m_ptr_uad;
					std::unique_ptr<tlpi_credentials> 	  m_ptr_credentials;

				protected:

					//

				public:

					//user and group helpers
					struct passwd info_from_name( const std::string& user_name );
					void populate_info_dictionary();
					bool passwd_fields( const std::string& user_name ,
										pwd_record_tuple& prt );
					bool sdwpasswd_fields( const std::string& user_name ,
										   spwd_record_tuple& prt );
					bool user_name_from_id( const uid_t uid , std::string& user_name );
					bool user_id_from_name( const std::string& user_name , uid_t& uid );
					bool group_name_from_id( const gid_t gid , std::string& group_name );
					bool group_id_from_name( const std::string& group_name , gid_t& gid );

					//shadow password helpers
					pwd_stat passwd_peek( const std::string& passwd_field , const std::string& user_name );
					bool retrieve_shadow_passwd( const std::string& user_name , std::string& encrypted_pwd );
					bool retrieve_shadow_passwd( const std::string& user_name , struct spwd* pwd );
					bool authenticate_user( const std::string& user_name  );

					//process identifiers helpers
					inline pid_t get_pid()  noexcept { return getpid(); };
					inline pid_t get_ppid() noexcept { return getppid(); };


					//services
					virtual bool authenticate( const std::string& user_name );
					virtual bool authenticate( const unsigned long& token );

					//accessors-inspectors
					inline const user_auth_dictionary_ptr authentication_dictionary() const noexcept { assert( m_ptr_uad != nullptr );
																								 return m_ptr_uad.get(); }
					inline const tlpi_credentials_ptr user_credentials() const noexcept { assert( m_ptr_credentials != nullptr );
																					      return m_ptr_credentials.get(); }

					//mutators
					inline void user_credentials( std::unique_ptr<tlpi_credentials> tc ) { m_ptr_credentials = std::move( tc ); }

					//immutable
					static const unsigned long max_pwd_record_tuple_fields;
					static const std::map<pwd_moniker , unsigned long> pw_fields_map;
					static const std::map<spwd_moniker , unsigned long> spw_fields_map;
					static const unsigned long  pwd_field_size;
			};
			inline std::ostream& operator<< ( std::ostream& o, const tlpi_authentication_assist& tcs ) noexcept
			{
				//todo
				return o << "snaps";
			}

}
#endif /* THE_LINUX_PROGRAMMING_INTERFACE_H_ */
