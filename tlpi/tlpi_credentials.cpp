/*
 * tlpi_credentials.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: wiljoh
 */

#include "tlpi.h"


using namespace tlpi;

using std::cout;
using std::endl;
using std::string;
using std::make_pair;
using std::vector;
using std::pair;
using std::cerr;
using std::make_tuple;
using std::map;
using std::system_error;
using std::ostream;


//--------------------------------------------tlpi_credentials----------------------------------------------------
//abstract virtual destructor
//---------------------------------------------------------------------------------------------------------------
tlpi_credentials_base::~tlpi_credentials_base()
{
	//todo
}

//ctors
//---------------------------------------------------------------------------------------------------------------
tlpi_credentials::tlpi_credentials()
{
	//todo
}

//---------------------------------------------------------------------------------------------------------------
tlpi_credentials::tlpi_credentials( const std::string& user_name ) : m_user_name{ user_name }
{
	//todo
}

//dtor
//---------------------------------------------------------------------------------------------------------------
tlpi_credentials::~tlpi_credentials()
{
	//todo
}

//---------------------------------------------------------------------------------------------------------------
process_credentials_ids tlpi_credentials::process_real_ids()
{
	//todo
	process_credentials_ids pci;

	return pci;
}


//---------------------------------------------------------------------------------------------------------------
process_credentials_ids tlpi_credentials::process_effective_ids()
{
	//todo
	process_credentials_ids pci;

	return pci;
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_process_uid( uid_t uid )
{
	if( setuid( uid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_process_gid( gid_t gid )
{

	if( setgid( gid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for group" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_effective_process_uid( id_t euid )
{
	if( seteuid( euid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_effective_process_gid( gid_t egid )
{

	if( seteuid( egid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_real_effective_process_uids( uid_t ruid , uid_t euid )
{
	if( setreuid( ruid , euid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_real_effective_process_gids( gid_t rgid , gid_t egid )
{
	if( setregid( rgid , egid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}
}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_file_system_uid( uid_t fsuid )
{
	if( setfsuid( fsuid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}

}

//---------------------------------------------------------------------------------------------------------------
void tlpi_credentials::set_file_system_gid( gid_t fsgid )
{
	if( setfsgid ( fsgid ) != 0 )
	{
		throw  system_error( std::make_error_code ( std::errc::permission_denied ) ,
							 "not enough privileges for user" );
	}

}




