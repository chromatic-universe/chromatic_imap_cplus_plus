/*
 * tlpi_authentication_assist.cpp
 *
 *  Created on: Jun 10, 2013
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


//static initialization
const unsigned long tlpi_authentication_assist::max_pwd_record_tuple_fields { 7L };
const std::map<pwd_moniker , unsigned long> tlpi_authentication_assist::pw_fields_map{    { pwd_moniker::pw_name , 0L } ,
																						  { pwd_moniker::pw_passwd , 1L } ,
																						  { pwd_moniker::pw_uid , 2L } ,
																						  { pwd_moniker::pw_gid , 3L } ,
																						  { pwd_moniker::pw_gecos , 4L } ,
																						  { pwd_moniker::pw_dir , 5L } ,
																						  { pwd_moniker::pw_shell , 6L } };
const std::map<spwd_moniker , unsigned long> tlpi_authentication_assist::spw_fields_map{  { spwd_moniker::spw_name , 0L } ,
																						  { spwd_moniker::spw_encrypted_pwd , 1L } ,
																						  { spwd_moniker::spw_last_change, 2L } ,
																						  { spwd_moniker::spw_min_days , 3L } ,
																						  { spwd_moniker::spw_max_days , 4L } ,
																						  { spwd_moniker::spw_warn_days , 5L } ,
																						  { spwd_moniker::spw_inactive_days , 6L } ,
																						  { spwd_moniker::spw_expire_date , 7L } ,
																						  { spwd_moniker::spw_reserved , 8L } };
const unsigned long  tlpi_authentication_assist::pwd_field_size { 13L };

//-----------------------------------------tlpi_authentication---------------------------------------------------
//abstract virtual destructor
//---------------------------------------------------------------------------------------------------------------
tlpi_authentication::~tlpi_authentication()
{
	//
}

//-----------------------------------------tlpi_authentication_base--------------------------------------------
//abstract virtual destructor
//---------------------------------------------------------------------------------------------------------------
tlpi_authentication_base::~tlpi_authentication_base()
{
	//
}

//ctors
//---------------------------------------------------------------------------------------------------------------
tlpi_authentication_assist::tlpi_authentication_assist() : m_ptr_uad( new user_auth_dictionary ) ,
														   m_ptr_credentials( new tlpi_credentials() )
{
		try
		{
			populate_info_dictionary();
		}
		catch( user_passwd_info_error& e )
		{
			cerr << e.what() << "\n";
			throw;
		}
}


//---------------------------------------------------------------------------------------------------------------
tlpi_authentication_assist::tlpi_authentication_assist( user_auth_dictionary_ptr uad )
													  : m_ptr_uad( uad ) ,
													    m_ptr_credentials( new tlpi_credentials() )
{
		try
		{
			populate_info_dictionary();
		}
		catch( user_passwd_info_error& e )
		{
			cerr << e.what() << "\n";
			throw;
		}
}

//---------------------------------------------------------------------------------------------------------------
tlpi_authentication_assist::~tlpi_authentication_assist()
{
	//
}

//user and group helpers
//---------------------------------------------------------------------------------------------------------------
struct passwd tlpi_authentication_assist::info_from_name( const std::string& user_name )
{
		assert( m_ptr_uad != nullptr );

		struct passwd  ps;
		std::memset( (void*) &ps , sizeof( ps ) , 0 );
		user_auth_dictionary_ptr uad = m_ptr_uad.get();

		auto idx = uad->find( user_name );
		if( idx != uad->end() )
		{
			ps = uad->at( user_name );
		}
		else
		{
			throw user_passwd_info_error();
		}

		return ps;

}

//---------------------------------------------------------------------------------------------------------------
void tlpi_authentication_assist::populate_info_dictionary()
{
	assert( m_ptr_uad != nullptr );

	struct passwd *pwd;

	m_ptr_uad->clear();
	while ( ( pwd = getpwent() ) != NULL )
	{
		pair<string , struct passwd>  pr( pwd->pw_name , *pwd );
		m_ptr_uad->insert( pr );
	}

	endpwent();

}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::passwd_fields( const std::string& user_name ,
											    pwd_record_tuple& prt )
{
	bool b_ret( true );

	try
	{
		struct passwd pw = info_from_name( user_name );
		prt = make_tuple( pw.pw_name ,
						  pw.pw_passwd ,
						  pw.pw_uid ,
						  pw.pw_gid ,
						  pw.pw_gecos ,
						  pw.pw_dir ,
						  pw.pw_shell );
	}
	catch( user_passwd_info_error& e )
	{
		cerr << e.what() << "\n";

		b_ret = false;
	}

	return b_ret;
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::sdwpasswd_fields( const std::string& user_name ,
							 	 	 	 	 	   spwd_record_tuple& prt )
{
	return ( true );
	//todo
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::user_name_from_id( const uid_t uid , string& user_name )
{
	struct passwd *pwd;

	pwd = getpwuid( uid );
	if( pwd != nullptr )
	{
		user_name = pwd->pw_name;
	}

	return ( user_name.empty() == false ) ? true : false;
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::user_id_from_name( const string& user_name , uid_t& uid )
{
	struct passwd* pwd = nullptr;

	if  ( user_name.empty() == false )
	{
		pwd = getpwnam( user_name.c_str() );
		if ( pwd != nullptr )
		{
			uid = pwd->pw_uid;
		}
	}

	return ( pwd != nullptr ) ? true : false;

}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::group_name_from_id( const gid_t gid , string& group_name )
{
	struct group* grp = nullptr;

	if ( group_name.empty() == false )
	{
		grp = getgrgid( gid );
		if( grp != nullptr )
		{
			group_name = grp->gr_name;
		}
	}

	return ( group_name.empty() == false ) ? true : false;

}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::group_id_from_name( const string& group_name , gid_t& gid )
{
	struct group* grp = nullptr;

	if( group_name.empty() == false )
	{
		grp = getgrnam( group_name.c_str( ) );
		if ( grp != nullptr )
		{
			gid = grp->gr_gid;
		}
	}

	return ( grp != nullptr ) ? true : false;

}

//---------------------------------------------------------------------------------------------------------------
pwd_stat tlpi_authentication_assist::passwd_peek( const std::string& passwd_field , const std::string& user_name )
{
	//if it is not known if shadow passwords are enabled , we try to infer it
	//and let the caller figure out what they want to do
	pwd_stat ps;

	ps = pwd_stat::ps_no_login_required;
	while ( ps != pwd_stat::ps_no_stat )
	{
		switch( ps )
		{

			//if passwd_field is empty , no password required
			case pwd_stat::ps_no_login_required:
			{
			  if( passwd_field.empty() == true ) { break; } else { ps = pwd_stat::ps_length_out_of_range; continue; }
			}
			//if value is > pwd_field_size , can't be encrypted password
			case pwd_stat::ps_length_out_of_range:
			{
			  if( passwd_field.size() > pwd_field_size ) { break; } else { ps = pwd_stat::ps_shadow_by_default; continue; }
			}
			//if value is < than pwd_field_size , assume it's a placeholder
			case pwd_stat::ps_shadow_by_default :
			{
			  if(  passwd_field.size() < pwd_field_size ) { break; } else { ps = pwd_stat::ps_no_stat; continue;}
			}
			case pwd_stat::ps_no_stat:

				break;
		}
		//if shadow_by_default , we check for shadow password
		//todo
	}

	return ps;
}

//shadow password helpers
//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::retrieve_shadow_passwd( const std::string& user_name , std::string& encrypted_pwd )
{
	bool b_ret( true );

	try
	{
		struct spwd* pwd = nullptr;

		pwd = getspnam( user_name.c_str() );
		if( pwd != nullptr )
		{
			encrypted_pwd =  pwd->sp_pwdp;
		}
		else if( pwd == nullptr && errno == EACCES )
		{
			throw std::system_error( EACCES , std::system_category() );
		}
		else
		{
			b_ret = false;
		}
	}
	catch( system_error& error )
	{
		cout << "error: " << error.code() << " - " << error.what() << '\n';
		throw;
	}

	return b_ret;

}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::retrieve_shadow_passwd( const std::string& user_name , struct spwd* pwd )
{

	bool b_ret( true );
	memset( (void*)pwd , sizeof( struct spwd ) , 0L );

	try
	{
		pwd = getspnam( user_name.c_str() );
		if( pwd == nullptr )
		{
			b_ret = false;
		}
		else if( pwd == nullptr && errno == EACCES )
		{
			throw std::system_error( EACCES , std::system_category() );
		}
	}
	catch( system_error& error )
	{
		cout << "error: " << error.code() << " - " << error.what() << '\n';
		throw;
	}

	return b_ret;
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::authenticate_user( const std::string& user_name  )
{
	//todo
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::authenticate( const std::string& user_name )
{

	//todo
	return true;
}

//---------------------------------------------------------------------------------------------------------------
bool tlpi_authentication_assist::authenticate( const unsigned long& token )
{
	//todo
	return true;
}





