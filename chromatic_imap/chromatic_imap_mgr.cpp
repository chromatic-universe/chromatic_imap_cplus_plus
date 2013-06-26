

#include "chromatic_imap.h"

using namespace boost;
using namespace chromatic_imap_protocol_impl;

using std::string;
using std::ostringstream;
using std::make_pair;
using std::pair;
using std::cout;
using std::endl;
using std::ends;
using std::set;
using std::cerr;
using std::vector;
using std::sort;
using std::map;
using std::ends;
using std::find_if;
using std::reverse;
using std::max_element;
using std::min_element;
using std::unique_ptr;
using std::chrono::milliseconds;
using boost::format;
using boost::cmatch;
using boost::regex_match; 
using boost::regex;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::algorithm::to_lower_copy;


//immutable

const string chromatic_imap_mgr::constCryptUnlockCode = "SFCNICOMCrypt_BH2SCUed8SPi";
const string chromatic_imap_mgr::constMimeUnlockCode = "SFCNICOMSMIME_sK7NCgi98C5p";
const string chromatic_imap_mgr::constMailUnlockCode = "SFCNICOMMAILQ_5NklwCi28R6H";
const string chromatic_imap_mgr::crlf = "\015\012";
const string chromatic_imap_mgr::space = " ";
const string chromatic_imap_mgr::asterisk = "*";
const string chromatic_imap_mgr::right_paren = ")";
const string chromatic_imap_mgr::left_paren = "(";
const string chromatic_imap_mgr::left_bracket = "[";
const string chromatic_imap_mgr::right_bracket = "]";
const string chromatic_imap_mgr::left_brace = "{";
const string chromatic_imap_mgr::right_brace = "}";
const string chromatic_imap_mgr::quote = "\"";
const unsigned long chromatic_imap_mgr::one = 1L;
const unsigned long chromatic_imap_mgr::two = 2L;
const unsigned long chromatic_imap_mgr::short_call = 512;

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64( unsigned char c )
{
  return ( isalnum(c) || (c == '+') || (c == '/') );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ctors
chromatic_imap_mgr::chromatic_imap_mgr(   const std::string& dbserver  ,
										  const std::string& proxystore ,
										  chromatic_handler_ptr imap_handler ) :    m_bConnected( false ) ,
																					m_bCloseExpunge( false ) ,
																					m_ptrStream( NULL ) ,
																					m_strDbServer( dbserver ) ,
																					m_ptrMessages( new imap_server_messages ) ,
																					m_strProxyStore( proxystore ) ,
																					m_ptrHandler( imap_handler )
							 
							 
{	
	 ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::chromatic_imap_mgr" ) , __LINE__ );

	 try
	 {
		
		open_server();

		read_params();
		
	 }
	 catch( chromatic_imap_mgr_error& e )
	 {		 
		 ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") , (char*) e.what() ) );		
	 }
	 catch( ... )
	 {
		 ACE_DEBUG ( ( LM_ERROR ,	 "Untyped error imap manager....\n" ) );		
	 }
	

	 m_bConnected = true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//dtor			
chromatic_imap_mgr::~chromatic_imap_mgr()
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::open_server()" ) , __LINE__ );

	deinit();

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//open_server
bool chromatic_imap_mgr::open_server()
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::open_server()" ) , __LINE__ );
	ACE_DEBUG ( ( LM_INFO , "Opening server....\n" ) );
	
	try
	{
		
	  ostringstream ostr;
      ostr    << "Driver={MySQL ODBC 5.1 Driver};"
               << "Server=" << (char*) db_server_string().c_str() << ";"
               << "Port=3306;"
               << "Option=131072;"
               << "Stmt=;"
               << "Database=oemo;"
               << "Uid=overtone;"
               << "Pwd=oemo";
       dsn( ostr.str() );

					 
	   m_db.Connect( (char*) dsn().c_str() ,
					  "overtone" ,
					  "oemo" ,
					  SA_ODBC_Client  );	  
		
		
	  /* m_dynHandle = LoadLibrary( "Oemoimap_strings.dll" );
	   if( m_dynHandle == NULL )
	   {
		    ACE_DEBUG ( ( LM_ERROR , "could not load strings library.........\n" ) );					
	   }
	   m_dynUtilsHandle = LoadLibrary( "OemoImapUtilsDll.dll" );
	   if( m_dynHandle == NULL )
	   {
		    ACE_DEBUG ( ( LM_ERROR , "could not load utils library.........\n" ) );					
	   }

	   for( int i = oemo_imap_fetch_fast; i < oemo_imap_fetch_bodyfull + 1; i++ )
		{
			pair<unsigned long,string> pr( i , _R( i ) );

			m_fetchMap.insert( pr );
		}*/
	}
	catch( chromatic_imap_mgr_error& oe )
	{
		
		//rethrow
		throw chromatic_imap_mgr_error( oe.what() , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}	
	catch(SAException &x)
	{

			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch(SAException &)
			  {
				  throw chromatic_imap_mgr_error( (const char*) x.ErrText() , chromatic_imap_mgr_error_level::lmlCritical ) ;
			  }
			  
			  
	}
	catch( ... )
	{
		//rethrow
		throw chromatic_imap_mgr_error( "Unspecified error in opening server....." , chromatic_imap_mgr_error_level::lmlCritical );
	}
	
	return ( true );

}

bool chromatic_imap_mgr::make_directory(const std::string& newdir )
{
	ACE_Trace _( ACE_TEXT( "make_directory(const std::string& newdir )" ) , __LINE__ );

	bool b_ret( false );

	try
	{
		struct stat st;
		if( stat( newdir.c_str() , &st ) == 0 )
		{
		    if(  ( st.st_mode )  & ( S_IFDIR != 0 ) )
		    {
		    	int dw = mkdir( newdir.c_str() ,S_IRWXU|S_IRWXG|S_IRWXO );
		    	if( dw == 0 ) { b_ret = true; }
		    }
		}
	}
	catch( ... )
	{
		throw chromatic_imap_mgr_error( "Make Directory failed..." , chromatic_imap_mgr_error_level::lmlDebug );
	}

	return b_ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//deinit
void chromatic_imap_mgr::deinit()
{ 
	ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::deinit()" ) , __LINE__ );

	try
	{
		m_db.Disconnect();
	}
	catch( ... )
	{
		//
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::enum_mailbox_list(    chromatic_mailbox_list& mbxs ,
											   const std::string& username ,
											   const std::string& reference ,
											   const std::string& criteria ,
											   bool subscribed )
{
	
		  ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::enum_mailbox_list" ) , __LINE__ );

		  string pattern( reference );
		  pattern += criteria;

		  typedef set<char> regSet;
		  regSet reg;

		  reg.insert( '+' );
		  reg.insert( '-' );
		  reg.insert( '.' );
		  reg.insert( '$' );
		  reg.insert( '(' );
		  reg.insert( ')' );
		  regSet::iterator iter;

		  string regEx( "^" );
		  for ( int i = 0; i < (int) pattern.size(); i++ )
		  {
			if( pattern[i] == '*' )
			{
			   regEx += ".*";
			}
			else if( pattern[i] == '%' )
			{
			   regEx += "[^";
			   regEx += "/";
			   regEx += "]*";
			}
			else if( reg.find( pattern[i] ) != reg.end() )
			{
			  regEx += "\\";
			  regEx += pattern[i];
			}
			else regEx += pattern[i];
		  }
		  regEx += '$';

		  collect_mailboxes_by_db( regEx ,  username , mbxs , subscribed );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool chromatic_imap_mgr::is_subscribed_by_db( const std::string& folder , const std::string& nickname )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::is_subscribed_by_db" ) , __LINE__ );

	long dw = 0L;

	try
	{
			  SACommand o( &m_db );			  

			  o.setCommandText( "call user_is_subscribed_to_folder( :1 , :2 )" );
			  o << (char*) folder.c_str()
			    << (char*) nickname.c_str();

			  o.Execute();

			  while( o.FetchNext() )
			  {
					dw = o.Field( "does_exist" ).asLong();
			  }


	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {
				  cerr << (const char*) x.ErrText() << "\n";

				  return ( false );
			  }
	}

	if ( dw > 0 ) return ( true ); else return ( false );

}
				  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::collect_mailboxes_by_db( const std::string& reg ,
												  const std::string& username ,
												  chromatic_mailbox_list& obl ,
												  bool subscribed )
{

		ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::collect_mailboxes_by_db" ) , __LINE__ );

		long dwIdx = 0L;		
		string atom;
		string subatom;
		string prefix;

		try
		{


				  SACommand o( &m_db );			  

				  o.setCommandText( "call get_mail_folders_by_user_moniker_ex( :1 )" );
				  o << (char*) username.c_str();

				  o.Execute();


				  while( o.FetchNext() )
				  {
					   chromatic_mailbox mbx;

					   prefix = "/" + ucase( username ) + "/";
					   subatom = o.Field( "folder_name" ).asString();
					   string::size_type idx = subatom.find( prefix );
					   if( idx != string::npos )
					   {
						   if( subatom.compare( prefix ) == 0 )
						   {
							   atom = subatom;
						   }
						   else
						   {
							   atom = subatom.substr( idx + prefix.size() );
						   }
						   cmatch what; 
						   regex expr( (char*) reg.c_str() );
						   if( regex_match( (const char*) atom.c_str() , what , expr ) == true ) 
						   { 						   
							  
							  mbx.subscribed( is_subscribed_by_db( subatom , username ) );
							  if( ( mbx.subscribed() == false ) && ( subscribed == true ) ) continue;
							  mbx.name( atom );
							  dwIdx = o.Field( "select_" ).asLong();
							  if( dwIdx == 0 ) mbx.attributes().insert( chromatic_mailbox_attribute::omaNoselect );
							  dwIdx = o.Field( "inferiors" ).asLong();
							  if( dwIdx == 0 ) mbx.attributes().insert( chromatic_mailbox_attribute::omaNoinferiors );
							  dwIdx = o.Field( "marked" ).asLong();
							  if( dwIdx == 0 ) mbx.attributes().insert( chromatic_mailbox_attribute::omaUnmarked ); else mbx.attributes().insert( chromatic_mailbox_attribute::omaMarked ) ;
							  
							  obl.push_back( mbx );
						   } 						   

					   }					   
				  }

				
		}
		catch( chromatic_imap_mgr_error& oe )
		{			
			//rethrow
			throw chromatic_imap_mgr_error( oe.what() , chromatic_imap_mgr_error_level::lmlCritical ) ;
		}	
		catch(SAException &x)
		{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {
				  throw chromatic_imap_mgr_error( (const char*) x.ErrText() , chromatic_imap_mgr_error_level::lmlCritical ) ;
			  }
		
				  
		}
		catch( ... )
		{
			//rethrow
			throw chromatic_imap_mgr_error( "Unspecified error in collecting mailboxes ..." , chromatic_imap_mgr_error_level::lmlCritical );
		}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::fill_message_list_by_db( const std::string& folder )
{	
		
                                    

     string strServer;
	 string str;
     chromatic_imap_message oim;
	 chromatic_mail_message_flags flags;
	 unsigned long dwIdx = 1L;
	 

     try
     {
		      messages()->clear();

              SACommand o( &m_db ); 

              o.setCommandText( "call get_user_mail_server_ex( :1 )" );
              o << (char*) user().c_str();

              o.Execute();


              while( o.FetchNext() )
              {
                   strServer = o.Field( "server" ).asString();
				   strServer = lcase( strServer );
              }
			  oim.server( strServer );

			  o.setCommandText( "call enumerate_messages_by_folder( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();

			  unsigned long dwMax = 0L;

			  while( o.FetchNext() )
              {
                   oim.uid( o.Field( "idx" ).asLong() );
				   if( (unsigned long) oim.uid() > dwMax )
				   {
					   max_uid( oim.uid() );
				   }
				   
				   str = o.Field( "absolute_url_string" ).asString();
				  
				   oim.url( str );
				   oim.file_url( str );
				   str = o.Field( "folder_offset" ).asString();
				   oim.offset( str );
				   oim.size( o.Field( "msg_size" ).asLong() );
				   oim.server( strServer );

		
				   string furl;
				   string encode_string;	  
				  
				  
				   furl = message_tag( oim.url() );
				   string s( base64_encode( (unsigned char const*) furl.c_str() , furl.size() ) );				  
				   
				   oim.url( s );
				   
				   long one = 1L;
				   
				   flags.clear();
				   
				   if( o.Field( "deleted" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfDeleted );
				   if( o.Field( "answered" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfAnswered );
				   if( o.Field( "draft" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfDraft );
				   if( o.Field( "flagged" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfFlagged );
				   if( o.Field( "recent" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfRecent );
				   if( o.Field( "seen" ).asLong() == one ) flags.insert( chromatic_mail_message_flag::omfSeen );

				   oim.flags( flags );

				   ostringstream ostr;
				   ostr << dwIdx;				  

				   pair<string,chromatic_imap_message> pr( ostr.str() , oim );
				   messages()->insert( pr );
				   

				   dwIdx++;

              }

			 

             
      }
      catch( SAException &x )
      {

              try
              {
                   m_db.Rollback();
              }
              catch( SAException & )
              {   
                  return;
              }
			  cerr << (const char*) x.ErrText() << "\n";
      }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::update_mailbox_info_by_db( const string& folder , bool bReadOnly )
                                      

{
     ostringstream ostr;
	 long dwSeen = 0L;
	 vector<chromatic_mail_message_flag> vFlags;
	 vFlags.push_back( chromatic_mail_message_flag::omfAnswered );
	 vFlags.push_back( chromatic_mail_message_flag::omfFlagged );
	 vFlags.push_back( chromatic_mail_message_flag::omfDeleted );
	 vFlags.push_back( chromatic_mail_message_flag::omfSeen );
	 vFlags.push_back( chromatic_mail_message_flag::omfDraft );

	 chromatic_mail_message_flags flags;
	 flags.clear();


     try
     {
    	 	  fill_message_list_by_db( folder );
		      mbx_info().clear();

              SACommand o( &m_db );
              
              o.setCommandText( "call get_message_flags_by_folder_ex( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();

			  string str;

              while( o.FetchNext() )
              {
				  
					for( int i = 0; i < (int) vFlags.size(); i++ )
					{
						flags.insert( vFlags[i] );
					}
					mbx_info().flags( flags );
					flags.erase( chromatic_mail_message_flag::omfRecent );
				    mbx_info().changeable_flags( flags );
				    mbx_info().name( folder );
                    mbx_info().recent( o.Field( "dwRecentCount" ).asLong() );
                    mbx_info().first_unseen( o.Field( "first_unseen" ).asLong() );
					mbx_info().exists( o.Field( "dwTotal" ).asLong() );
					dwSeen = o.Field( "dwSeenCount" ).asLong();
                    mbx_info().unseen(  mbx_info().exists() -  dwSeen );
					ostr << o.Field( "dwUidnext" ).asLong();
				    mbx_info().uid_next( ostr.str() );
					ostr.str( "" );
					ostr << o.Field( "dwValidity" ).asLong();;
                    mbx_info().uid_validity( ostr.str() );
					mbx_info().read_only( bReadOnly );
              }

		
      }
      catch( SAException &x )
      {

              try
              {
                   m_db.Rollback();
              }
              catch( SAException & )
              {
                               
              }
			  cerr <<  (const char*) x.ErrText();   
      }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::update_mailbox_info_by_db( const string& folder , chromatic_mailbox_info& info )
{
     ostringstream ostr;
	 long dwSeen = 0L;
	 vector<chromatic_mail_message_flag> vFlags;
	 vFlags.push_back( chromatic_mail_message_flag::omfAnswered );
	 vFlags.push_back( chromatic_mail_message_flag::omfFlagged );
	 vFlags.push_back( chromatic_mail_message_flag::omfDeleted );
	 vFlags.push_back( chromatic_mail_message_flag::omfSeen );
	 vFlags.push_back( chromatic_mail_message_flag::omfDraft );
	// vFlags.push_back( omfRecent );
	 chromatic_mail_message_flags flags;
	 flags.clear();

     try
     {
		      fill_message_list_by_db( folder );
			  info.clear();

              SACommand o( &m_db );
              
              o.setCommandText( "call get_message_flags_by_folder_ex( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();


              while( o.FetchNext() )
              {
				    for( int i = 0; i < (int) vFlags.size(); i++ )
					{
						flags.insert( vFlags[i] );
					}
					info.flags( flags );
					flags.erase( chromatic_mail_message_flag::omfRecent );
				    info.changeable_flags( flags );
				    info.name( folder );
                    info.recent( o.Field( "dwRecentCount" ).asLong() );
                    info.first_unseen( o.Field( "first_unseen" ).asLong() );
                    info.exists( o.Field( "dwTotal" ).asLong() );
					dwSeen = o.Field( "dwSeenCount" ).asLong();
                    info.unseen(  info.exists() -  dwSeen );
					ostr << o.Field( "dwUidnext" ).asLong();
                    info.uid_next( ostr.str() );
					ostr.str( "" );
					ostr << o.Field( "dwValidity" ).asLong();;
                    info.uid_validity( ostr.str() );
              }
      }
      catch( SAException &x )
      {

              try
              {
                   m_db.Rollback();
              }
              catch( SAException & )
              {
                               
              }
			  cerr <<  (const char*) x.ErrText();   
      }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::check_folder_name( const std::string& folder )
{
    string str( folder );

	if( str[0] != '/' )
	{
		str = "/" +  user() + "/" + folder;
	}
	else
	{
		str = "/" + user() + folder;
	}

	return ( ucase( str ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
chromatic_mailbox_result chromatic_imap_mgr::folder_exists( const std::string& folder )
{
	chromatic_mailbox_result result;

     try
     {


              SACommand o( &m_db );
              long dwExists;

              o.setCommandText( "call subscribe_folder_exists( :1 )" );
              o << (char*)folder.c_str();

              o.Execute();


              while( o.FetchNext() )
              {
                   dwExists  = o.Field( "folder_exists" ).asLong();
              }

              switch ( dwExists )
              {
                case 0 :
                   result = chromatic_mailbox_result::omrNotFound;
                   break;
                case 1 :
                   result = chromatic_mailbox_result::omrSuccess;
                   break;
                case -1 :
                   result = chromatic_mailbox_result::omrAccessDenied;
                   break;
              }
      }
      catch( SAException& )
      {

              try
              {
                   m_db.Rollback();
              }
              catch( SAException & )
              {
              }
              result = chromatic_mailbox_result::omrAccessDenied;
      }

      return ( result );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::fetch_message( unsigned long msg_id , std::ostream& ostr , chromatic_imap_fetch_requests& requests )
{

	string req_str;	
	ostringstream dostr;
	ostringstream fetch_stream;
	chromatic_imap_fetch_request req;
	chromatic_imap_message msg;
	string s;
	chromatic_mail_message_flags flags;
	chromatic_mail_message_flags uflags;
	flags.clear();
	uflags.clear();
	string strPayload;
	bool bPeek ( false );
	


	if( uids() == false )
	{
		dostr << msg_id;
		imap_server_messages::iterator siter = messages()->find( dostr.str() );
		if( siter != messages()->end() )
		{
			msg = siter->second;							
		}
		
	}
	else
	{
		for( imap_server_messages::iterator siter = messages()->begin();
				siter != messages()->end();
					siter++ )
					{
						if( (unsigned long) siter->second.uid() == msg_id )
						{
							msg = siter->second;
							dostr.str( "" );
							dostr << siter->first;							

							break;
						}						
					}
	}
	
	s = proxy_store() + "\\" + msg.offset() + "\\" + msg.url()  +  ".xml";

	flags = msg.flags();
	uflags = msg.flags();

     unique_ptr<chromatic_fetch_strings> cfs( new chromatic_fetch_strings );
     
	//aggregate keywords
	chromatic_imap_fetch_requests::iterator dter = requests.begin();
	if( (*dter).first.compare(  cfs->_R( cifc::chromatic_imap_fetch_fast ) ) == 0 )
	{
		requests.clear();
		req.first = cfs->_R( cifc::chromatic_imap_fetch_flags );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_internaldate );
		requests.push_back( req );
		req.first =cfs->_R( cifc::chromatic_imap_fetch_rfc822size );
		requests.push_back( req );
	}
	else if( (*dter).first.compare( cfs->_R( cifc::chromatic_imap_fetch_all ) ) == 0 )
	{
		requests.clear();
		req.first =cfs->_R( cifc::chromatic_imap_fetch_flags );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_internaldate );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_rfc822size );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_envelope );
		requests.push_back( req );
	}
	else if( (*dter).first.compare( cfs->_R( cifc::chromatic_imap_fetch_full ) ) == 0 )
	{
		requests.clear();
		req.first = cfs->_R( cifc::chromatic_imap_fetch_flags );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_internaldate );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_rfc822size );
		requests.push_back( req );
		req.first = cfs->_R( cifc::chromatic_imap_fetch_envelope );
		requests.push_back( req );
		req.first =cfs->_R( cifc::chromatic_imap_fetch_body );
		requests.push_back( req );
	}
    
	if( uids() == true )
	{
		fetch_stream << "UID" << space << msg.uid() << space;
	}

	for( dter = requests.begin();
			dter != requests.end();
			  dter++ )
			  {  
					
					imap_fetch_strings::iterator iter;
					for ( iter = m_fetchMap.begin();
										iter != m_fetchMap.end();
										  iter++ )
										  {
											  string str( iter->second );
											  string str2(  lcase( dter->first ) );
											  if( iter->second.compare( lcase( dter->first ) ) == 0 )
											  	  break;
										  }
					


					
					if( iter == m_fetchMap.end() )
						throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical ) ;
					
					string ret_param( ucase( iter->second ) );

					switch( (cifc) iter->first )
					{
						case cifc::chromatic_imap_fetch_flags:

							if( bPeek == false )
							{
								if( flags.find( chromatic_mail_message_flag::omfSeen ) == flags.end() ) flags.insert( chromatic_mail_message_flag::omfSeen );
								msg.flags( flags );
							}
							
							fetch_stream << ret_param << space;
							fetch_stream << left_paren << mbx_flags_to_str( msg.flags() ) << right_paren
										 << space;

							break;

						case cifc::chromatic_imap_fetch_internaldate:

							fetch_stream << ret_param << space;
							fetch_stream << quote << internal_date( s ) << quote
										 << space;

							break;

						case cifc::chromatic_imap_fetch_rfc822size:

							fetch_stream << ret_param << space;
							fetch_stream << msg.size()
										 << space;

							break;

						case cifc::chromatic_imap_fetch_envelope:

							fetch_stream << ret_param << space;
							fetch_stream << left_paren << fetch_envelope( s ) << right_paren
										 << space;

							break;

						case cifc::chromatic_imap_fetch_bodystructure:

							fetch_stream << ret_param << space;
							fetch_stream << left_paren << fetch_body_structure( s ) << right_paren
										 << space;

							break;

						case cifc::chromatic_imap_fetch_uid:
							
							if( uids() == false )
							{
								fetch_stream << ret_param << space;
								fetch_stream << msg.uid()
											 << space;

							}
							break;

						case cifc::chromatic_imap_fetch_rfc822:
							{
								fetch_stream << ret_param << space;
								fetch_stream << left_brace << msg.size() + two << right_brace
											 << crlf
											 << fetch_rfc822( s , iter->second )
											 << space;	
										
							
							}
							break;
							
						case cifc::chromatic_imap_fetch_rfc822text:
							{
								string strPayload = fetch_rfc822( s , iter->second ) ;
								fetch_stream << ret_param << " ";
								fetch_stream << left_brace << strPayload.size() + two << right_brace
											 << crlf
											 << strPayload
											 << space;	

								flags = msg.flags();
								flags.insert( chromatic_mail_message_flag::omfSeen );
							}
							
							break;	

						case cifc::chromatic_imap_fetch_bodypeek:
							{
								bPeek = true;						
							}
						case cifc::chromatic_imap_fetch_body:
							{
								
								string strTemp( lcase( dter->second ) );
								string strBody(  ucase( cfs->_R( cifc::chromatic_imap_fetch_body ) ) );
								ostringstream fostr;
																

								if( strTemp.compare( cfs->_R( cifc::chromatic_imap_fetch_header ) ) == 0 )
								{
									strPayload = fetch_header( s , "" , false );
								}
								else if( strTemp.find( cfs->_R( cifc::chromatic_imap_fetch_headerfields ) ) != string::npos )
								{
									strPayload = fetch_header_fields( s , dter->second );
								
								}
								else if( strTemp.find( ".header" ) != string::npos )
								{
									

									string::size_type idx = dter->second.find( ".header" );
									if( idx != string::npos )
									{
										string strPart( dter->second.substr( 0 , idx ) );
										string strAfter( dter->second.substr( 0 , ++idx ) );
										if( strAfter.compare( cfs->_R( cifc::chromatic_imap_fetch_header ) ) == 0 )
										{
											strPayload = fetch_header( s , strPart , true );

										}
										else if( strAfter.find( cfs->_R( cifc::chromatic_imap_fetch_header ) ) != string::npos )
										{
											strPayload = fetch_header_fields( s , dter->second );
											
										}
									}
								}
								else 
								{	
									
									if( dter->second.empty() ) 
									{
										

										fetch_stream << strBody << space;
										fetch_stream << left_paren << fetch_body_structure( s ) << right_paren
													 << space;

										break;
									}									
									

									if( dter->second.compare( "nil" ) == 0 )
									{
											strTemp = "";
									}	
									strPayload = fetch_mime( s , strTemp );
									if( (cifc) iter->first == cifc::chromatic_imap_fetch_bodypeek )
									{											
											flags = msg.flags();
											flags.insert( chromatic_mail_message_flag::omfSeen );
									}										

								}

								if( dter->second.compare( "nil" ) == 0 )
								{
										strTemp = "";
								}					
							
								
								fetch_stream << strBody 
											 << left_bracket << strTemp << right_bracket
											 << space
											 << left_brace << strPayload.size() + two << right_brace
											 << crlf
											 << strPayload
											 << crlf											 
											 << space;	
														

								
							
							}
							break;

						default:

							throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
							

							break;
					}

					 milliseconds dura( 5 );
					 std::this_thread::sleep_for( dura );

			  }
			  
			  string strTemp( fetch_stream.str() );			  
			  strTemp = rtrim( strTemp , " " );	
			  
			  string s1;
			 
			  
			  
			  s1 = fetch_response( dostr.str() , strTemp , ostr );
			  
			  flags.erase( chromatic_mail_message_flag::omfRecent );
			  msg.flags( flags );
			 
			  if( uflags != msg.flags() )
			  {
				set_message_flags_by_db( msg.file_url() , flags );
			  }			 

			  return ( s1 );

			  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::fetch_header( const string& message_path ,  const string& params , bool bIsPart )
{
	 string payload;
	 vtor_string parts;
	 CkString ckstr;
		  

	 try
	 {
		   unique_ptr<CkMime> mime( new CkMime );
		   
		   bool bRet = mime->UnlockComponent( (char*) constMimeUnlockCode.c_str() );
		  
		   if( bRet )
		   {
			   if( bIsPart == false )
			   {			
					mime->LoadXmlFile( (char*) message_path.c_str() );
					mime->GetEntireHead( ckstr );

					payload = ckstr.getString();
			   }
			   else
			   {
				   safe_tokenize_t( parts , (char*) params.c_str() , "." );

				   unique_ptr<CkMime> mime( new CkMime );
				   CkMime* mimeIdx;
				   CkMime* mimeQdx;	

				   mimeIdx = mime.get();
				   for( int i = 0; i < (int) parts.size(); i++ )
				   {
					 mimeQdx = mimeIdx->GetPart( boost::lexical_cast<unsigned long>( parts[i] ) - 1 );
                     mimeIdx = mimeQdx;
				   }
				   mimeQdx->GetEntireHead( ckstr );
				   payload = ckstr.getString();
			   }
                                  
		   }
		   
	   }
	   catch( ... )
	   {
		   throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	   }

	   return ( payload );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::fetch_body_structure( const string& message_path )
{

	string payload;

	/*try
	{
		   unique_ptr<CkEmail>  email( new CkEmail );
		   bool bRet = email->UnlockComponent( (char*) constMailUnlockCode.c_str() );
		   if (!bRet ) 
		   {
			 return( "" );
		   }
		   
		   bRet = email->LoadXml( (char*) message_path.c_str() );
		   unique_ptr<CkString> strMime( new CkString );

		   email->GetMime( *strMime.get() );
			
		   proc_char_ptr_in_char_ptr_out_args_return_dword aProc =
			   (proc_char_ptr_in_char_ptr_out_args_return_dword) GetProcAddress( UtilsHandle() ,
			   (const char*) _R( oemo_imap_utils_fetchbodystructure ).c_str() );

		   if ( NULL != aProc ) 
		   {	
			    			
				unsigned long dw = (aProc) ( strMime->getString() , s , 1000 );
				//not enough memory
				if( dw == 0L )
				{
					delete[] s;
					s = NULL;
					s = new char[dw + 1];
					(aProc) ( strMime->getString() , s , dw );
				}

				payload = s;			

				if( s != NULL )
				{
					delete[] s;
					s = NULL;
				}
		   }
	}
	catch( ... )
	{
		if( s != NULL )
		{
			delete[] s;
			s = NULL;
		}
		throw chromatic_imap_mgr_error( "parse error in fetch" , lmlCritical );
	}*/

	return ( payload );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::fetch_envelope( const std::string& message_path )
{
	char* s = new char[1000];
	string payload;

	try
	{
		   unique_ptr<CkMailMan> mailman( new CkMailMan );
		   unique_ptr<CkEmail>  email( new CkEmail );
				   
		   bool bRet = mailman->UnlockComponent( (char*) constMailUnlockCode.c_str() );
		   if (!bRet ) 
		   {
			 return ( "" );
		   }	

		   bRet = email->LoadXml( (char*) message_path.c_str() );
		   unique_ptr<CkString> strMime( new CkString );

		   email->GetMime( *strMime.get() );
			
		   /*proc_char_ptr_in_char_ptr_out_args_return_dword aProc =
			   (proc_char_ptr_in_char_ptr_out_args_return_dword) GetProcAddress( UtilsHandle() ,
			   (const char*) _R( oemo_imap_utils_fetchenvelope ).c_str() );

		   if ( NULL != aProc ) 
		   {	
			    			
				unsigned long dw = (aProc) ( strMime->getString() , s , 1000 );
				//not enough memory
				if( dw == 0L )
				{
					delete[] s;
					s = NULL;
					s = new char[dw + 1];
					(aProc) ( strMime->getString() , s , dw );
				}

				payload = s;			

				if( s != NULL )
				{
					delete[] s;
					s = NULL;
				}
		   }*/
	}
	catch( ... )
	{
		if( s != NULL )
		{
			delete[] s;
			s = NULL;
		}
		throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	}

	return ( payload );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::fetch_rfc822( const string& message_path , const string& command )
{
	   string payload;
	   CkString ckstr;
	   unique_ptr<chromatic_fetch_strings> cfs( new chromatic_fetch_strings );

       try
	   {
		   unique_ptr<CkMime> mime( new CkMime );
		   bool bRet = mime->UnlockComponent( (char*) constMimeUnlockCode.c_str() );
		   if( bRet )
		   {
			   mime->LoadXmlFile( (char*) message_path.c_str() );
			   if( command.compare( cfs->_R( chromatic_imap_fetch_const::chromatic_imap_fetch_rfc822 ) ) == 0 )
			   {
				   mime->GetMime( ckstr );
				   payload = ckstr.getString();
			   }
			   else if( command.compare( cfs->_R( chromatic_imap_fetch_const::chromatic_imap_fetch_rfc822text ) + ".text" ) == 0 )
			   {
				   mime->GetEntireBody( ckstr );
				   payload = ckstr.getString();
			   }
		   }

	   }
	   catch( ... )
	   {
		   throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	   }

	   return ( payload );

}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::fetch_mime( const std::string& message_path , const std::string& mime_part )
{
	 string payload;
	 vtor_string parts;
	 CkString ckstr;
	 unsigned long one = 1;

	 try
	   {
		   auto_ptr<CkMime> mime( new CkMime );
		   CkMime* mimeIdx;
		   CkMime* mimeQdx;		  
		   
		   bool bRet = mime->UnlockComponent( (char*) constMimeUnlockCode.c_str() );
		  
		   if( bRet )
		   {
			  safe_tokenize_t( parts , (char*) mime_part.c_str() , "." );
			  mime->LoadXmlFile( (char*) message_path.c_str() );             
			  if( mime_part.empty() )
			  {
				mime->GetMime( ckstr );
				payload = ckstr.getString();
			  }
			  else if( parts.size() == one )                                   
			  {			
				  unsigned dwPart = boost::lexical_cast<unsigned long>( parts[0] ) - 1;
				  mimeQdx = mime->GetPart( boost::lexical_cast<unsigned long>( dwPart ) );
				  mimeQdx->GetEntireBody( ckstr );
				  payload = ckstr.getString();
			  }			  
			  else
			  {
				  mimeIdx = mime.get();
				  for( int i = 0; i < (int) parts.size(); i++ )
				  {
					mimeQdx = mimeIdx->GetPart( boost::lexical_cast<unsigned long>( parts[i] ) - 1 );
                    mimeIdx = mimeQdx;
				  }
				  mimeQdx->GetEntireBody( ckstr );
				  payload = ckstr.getString();
			  }
                                  
		   }

	   }
	   catch( ... )
	   {
		   throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	   }

	   return ( payload );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::fetch_header_fields( const string& message_path , const string& params )
{
	ostringstream ostr;	
	CkString ckstr;
	imap_header_fields fields;
	imap_header_set fields_set;

	 try
	 {     
		   
		   parse_header_field_params( params , fields_set );

		   
		   fields = fetch_header_fields_mime( message_path , params );

		   for( imap_header_fields::iterator iter = fields.begin();
					iter != fields.end();
						iter++ )
						{							
							string s( lcase( iter->first ) );
							imap_header_set::iterator dter = fields_set.find( lcase( iter->first ) );
							if( dter != fields_set.end() )
							{
								ostr << iter->first << ": " <<  iter->second << crlf;
							}
						}						
	

	   }
	   catch( ... )
	   {
		   throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	   }

	   return ( ostr.str() );	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
imap_header_fields chromatic_imap_mgr::fetch_header_fields_mime( const string& message_path , const string& params )
{

	 string payload;
	 vtor_string parts;
	 string strHeader;	 
	 vtor_string subparts;
	 imap_header_fields fields;
	 bool bSubPart = false;
	 string str( lcase( params ) );
	 string strToken( "header.fields" );
	 string::size_type idx = str.find( strToken );
	 if( idx != string::npos )
	 {
		 str = str.substr( idx + strToken.size() );
	 }
	 str = ltrim( str , " " );

	 try
	   {
		   
		   unique_ptr<CkMime> mime( new CkMime );
		   CkMime* mimeIdx;
		   CkMime* mimeQdx;			   
		   
		   
		   bool bRet = mime->UnlockComponent( (char*) constMimeUnlockCode.c_str() );		  
		   if( bRet )
		   {
			   //get the xml from storage
               bRet = mime->LoadXmlFile( (char*) message_path.c_str() );
			   //get the header
			   string::size_type idx = str.find( "." );
			   if( idx == string::npos )
			   {
				   bSubPart = false;
			   }
			   else
			   {
					string strParams ( str );
                    safe_tokenize_t( parts , (char*) str.c_str() , "." );
					for(  int i = 0; i < (int) parts.size(); i++ )
					{
						try
						{
							boost::lexical_cast<unsigned long>( parts[i] );
							subparts.push_back( parts[i] );
										   
						}
						catch( bad_lexical_cast & )
						{
						    continue;
						}
					}
				    
					mimeIdx = mime.get();                    

					for( int j = 0; j < (int) parts.size(); j++ )
					{
						mimeQdx = mimeIdx->GetPart( boost::lexical_cast<unsigned long>( parts[j] ) - 1 );
						mimeIdx = mimeQdx;
					}
					
					bSubPart = true;
			   }

			   unsigned long dw = 0L;
			   if( bSubPart == false )
			   {
					dw = mime->get_NumHeaderFields();
					for( int i = 0; i < (int) dw; i++ )
					{
					   CkString name;
					   CkString value;

					   mime->GetHeaderFieldName( i , name );
					   mime->GetHeaderFieldValue( i , value );
					   pair<string,string> pr( (char*) name.getString() , (char*) value.getString() );

					   fields.insert( pr );	
					}
			   }
			   else
			   {
					dw = mimeQdx->get_NumHeaderFields();
					for( int i = 0; i < (int) dw; i++ )
					{
					   CkString name;
					   CkString value;

					   mimeQdx->GetHeaderFieldName( i , name );
					   mimeQdx->GetHeaderFieldValue( i , value );
					   pair<string,string> pr( (char*) name.getString() , (char*) value.getString() );

					   fields.insert( pr );	
					}
						
			   }
		   }
			

	   }
	   catch( ... )
	   {
		   throw chromatic_imap_mgr_error( "parse error in fetch" , chromatic_imap_mgr_error_level::lmlCritical );
	   }

	   return ( fields );



}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::parse_header_field_params( const std::string& source , imap_header_set& fields_set )
{
	vtor_string v;
	unique_ptr<chromatic_message_manip> cma ( new chromatic_message_manip() );
	string str ( cma->extract_parenthetical_atoms( source ) );
	safe_tokenize_t( v , (char*) str.c_str() , " " );
	
	for( vtor_string::iterator iter = v.begin();
			iter != v.end();
				iter++ )
				{
					fields_set.insert( lcase( *iter ) );
				}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::internal_date( const string& filename )
{
	ostringstream ostr;

	/*HANDLE handle;
	WIN32_FIND_DATA data;
	ostringstream ostr; 
	SYSTEMTIME st;
	FILETIME ft;
	string str;
	string strFormat( "%.2d-%s-%.2d %d:%.2d:%.2d %s" );
	vtor_string months;

	months.push_back( "Jan" );
	months.push_back( "Feb" );
	months.push_back( "Mar" );
	months.push_back( "Apr" );
	months.push_back( "May" );
	months.push_back( "Jun" );
	months.push_back( "Jul" );
	months.push_back( "Aug" );
	months.push_back( "Sep" );
	months.push_back( "Oct" );
	months.push_back( "Nov" );
	months.push_back( "Dec" );


	handle = FindFirstFile( (char*) filename.c_str() , &data );
    if ( handle != INVALID_HANDLE_VALUE )
    {
		FindClose( handle );
		if( FileTimeToLocalFileTime( &data.ftLastWriteTime , &ft ) )
		{
			FileTimeToSystemTime( &ft , &st );		
		
	
			str = TimeZoneInfo();
			ostr << format( strFormat ) % st.wDay 
										% months[st.wMonth - 1]
										% st.wYear
										% st.wHour
										% st.wMinute
										% st.wSecond
										% str;		 

		}
		
	}*/

	

	return ( ostr.str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::parse_fetch_request_list( const std::string& request , chromatic_imap_fetch_requests& requests )
{

	char c;
	bool bParams = false;
	bool bNull = false;
	string s;
	string source( request );
	chromatic_imap_fetch_request req;

	source = ltrim( source , "(" );
	source = rtrim( source , ")" );

	string::iterator iter = source.begin();	
	
	
	if( source.size() == 0L ) return;

	c = *iter;

	while( iter != source.end() )
	{
		if( c == ' ' ) 
		{
			if( bParams == true )
			{
				s += c;
			}
			else
			{
				bNull = true;
				req.first = "";
				req.second = "";
				if( ! s.empty() )
				{
					req.first = lcase( extract_quoted_string( s ) );
					requests.push_back( req );					
				}	
				s = "";   
			}
		}
		else if ( c == '[' )
		{
			bParams = true;
			bNull = true;
			req.first = "";
			req.second = "";
			if( ! s.empty() )
			{
				lcase( req.first = extract_quoted_string( s ) );
				requests.push_back( req );				
			}	
			s = "";  
		}
		else if ( c == ']' )
		{		

			if( ( bParams == true ) && ( bNull == true ) && ( ! s.empty() ) )
			{				
				lcase( requests[requests.size() - 1].second = extract_quoted_string( s ) );
			}
			if( s.empty() == true )
				requests[requests.size() - 1].second = "nil";
			s = "";
			bParams = false;
			bNull = true;
			req.first = "";
			req.second = "";
		}
		else
		{
			s += c;
		}
		iter++;
		if( iter != source.end() )
		{
			c = *iter;
		}

	}

	if ( ( bParams == false )  && ( ! s.empty() ) )
	{
		req.first = lcase( s );
		requests.push_back( req );
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::set_message_flags_by_db( const string& url , chromatic_mail_message_flags flags )
{
	 string decoded_url;
	 typedef map<chromatic_mail_message_flag,string> flag_strings;
	 flag_strings m;		 
	 
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfAnswered , "isAnswered" ) );
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfDeleted , "isDeleted" ) );
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfDraft , "isDraft" ) );
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfFlagged , "isFlagged" ) );
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfRecent , "isRecent" ) );
	 m.insert( pair<chromatic_mail_message_flag,string>( chromatic_mail_message_flag::omfSeen , "isSeen" ) );


	 try
     {
			  
              SACommand o( &m_db );              

              o.setCommandText( "call update_message_flags( :1 , :2, :3, :4 , :5 , :6 , :7)" );
              o << (char*) url.c_str(); 
			  for( flag_strings::iterator iter = m.begin();
						iter != m.end();
							iter++ )
							{
								chromatic_mail_message_flags::iterator dter = flags.find( iter->first );
								dter != flags.end() ? o << 1L : o << 0L;
							}		  

              o.Execute();             

      }
      catch( SAException& e )
      {

              try
              {
                   m_db.Rollback();
				   cout << (const char*) e.ErrText() << "\n";
              }
              catch( SAException & )
              {
              }
              throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical );
      }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::time_zone_info()
{
	string str;
	/*char* buffer = new char[short_call];

	try
	{

		proc_char_ptr_args_return_void aProc = 
		   (proc_char_ptr_args_return_void) GetProcAddress( UtilsHandle() , (char*) _R( oemo_imap_utils_proc_timezone ).c_str() ); 
		if ( NULL != aProc ) 
		{				
			(aProc) ( buffer ); 

			str = buffer;

			if( buffer != NULL )
				delete[] buffer;
		}
	}
	catch( ... )
	{
		if( buffer != NULL )
			delete[] buffer;
	}*/

	return ( str );

	
}

//overrides
bool chromatic_imap_mgr::on_login( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string _user;
	string password;
	

	pair<string,string> pr = pair_from_tuple( params );
	if( ( pr.first.empty() ) || ( pr.second.empty() ) )
	{
		return ( false );
	}

	_user = lcase( extract_quoted_string( pr.first ) );
	password = extract_quoted_string( pr.second );

	user( _user );

	try
	{
		SACommand o( &m_db );	
		string str;

		o.setCommandText( "call get_user_domain_name_ex( :1 )" );
		o << (char*) user().c_str();

		o.Execute();

		while( o.FetchNext() )
		{
			str = o.Field( "domain_name" ).asString();
		}
		to_lower_copy( str );
		string strDc( "dc=" );
		string::size_type idx = str.find( "," );
		if( idx != string::npos )
		{
			str = str.substr( 0 , idx );
			idx = str.find( strDc );
			if( idx != string::npos )
			{
				idx += strDc.size();
				str = str.substr( idx );
			}
		}

		user_domain( str );
	}
	catch( ... )
	{	
	}

	return ( true );
}

bool chromatic_imap_mgr::on_logout( const string& params , const string& fmt , std::ostream& ostr )
{

	
	return ( true );
}

bool chromatic_imap_mgr::on_list( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;

	try
	{
		chromatic_mailbox_list mbxs;

		safe_tokenize_t( imp , (char*) params.c_str() , " " );
		if( imp.size() != 2 )
		{
			return ( false );
		}
			
		//special case , requesting hierarchy
		if( ( imp[0].compare( "\"\"" ) == 0 ) && ( imp[1].compare( "\"\"" ) == 0 ) )
		{
			ostr << format( fmt ) % "\\noselect \\unmarked" % "/" % "";

			return ( true );

		}
		
		pair<string,string> pr = pair_from_tuple( params );
		
		
		enum_mailbox_list( mbxs ,
						 user() ,
						 extract_quoted_string( pr.first ) ,
						 extract_quoted_string( pr.second ) );

		

		for(chromatic_mailbox_list::iterator iter = mbxs.begin();
				iter != mbxs.end();
					iter++ )
					{
						string str( mbx_attributes_to_str( (*iter).attributes() ) );
					
						ostr << format( fmt ) % str % "/" % (*iter).name();
					}


	}
	catch( chromatic_imap_mgr_error& oe )
	{			
	   cerr << oe.what() << "\n";
	   return ( false );
	}	
	
	return ( true );
}

bool chromatic_imap_mgr::on_lsub( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	imap_strings imp;

	try
	{
		chromatic_mailbox_list mbxs;

		safe_tokenize_t( imp , (char*) params.c_str() , " " );
		if( imp.size() != 2 )
		{
			return ( false );
		}
		
		pair<string,string> pr = pair_from_tuple( params );
		
		enum_mailbox_list(   mbxs ,
							 user() ,
							 extract_quoted_string( pr.first ) ,
							 extract_quoted_string( pr.second ) ,
							 true );

		

		for(chromatic_mailbox_list::iterator iter = mbxs.begin();
				iter != mbxs.end();
					iter++ )
					{
						string str( mbx_attributes_to_str( (*iter).attributes() ) );
					
						ostr << format( fmt ) % str % "/" % (*iter).name();
					}


	}
	catch( chromatic_imap_mgr_error& oe )
	{			
	   cerr << oe.what() << "\n";
	   return ( false );
	}	
	
	return ( true );
}

bool chromatic_imap_mgr::on_authenticate( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}

bool chromatic_imap_mgr::on_select( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	bool isInbox = false;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;

	
	if( params.empty() )
		return ( false );

	mailbox = extract_quoted_string( params );
	

	//inbox
    subatom = "/" + ucase( user() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
      isInbox = true;   
	}

	atom = check_folder_name( mailbox );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{    
		 //select always succeeds for inbox , even it does not exist
		if( isInbox == false )
		{
			throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
		}
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	chromatic_mailbox_info info;
	ostringstream dostr;

	update_mailbox_info_by_db( atom , false );

	select_response( mbx_info() , dostr , false );

	ostr << format( fmt ) % dostr.str();
	

	return ( true );
}

bool chromatic_imap_mgr::on_create( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;

			
	if( params.empty() )
		return ( false );

	mailbox = extract_quoted_string( params );

	atom = check_folder_name( mailbox );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrSuccess )
	{
		 throw chromatic_imap_mgr_error( "mailbox already exists" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( user() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	try
	{
			 
			  SACommand o( &m_db );	
			  string str( ucase( user() ) );
			  long one = 1;
			  long zero = 0;

			  o.setCommandText( "call insert_into_subscribe_folders( :1 , :2 , :3 , :4 , :5 , :6 , :7 )" );
			  o << (char*) str.c_str();
			  o	<< "/";
			  o << (char*) atom.c_str();
			  o	<< (long) one;
			  o	<< (long) one;
			  o	<< (long) zero;
			  o	<< (long) zero;			  

			  o.Execute();
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {
				  cerr << (const char*) x.ErrText() << "\n";				  
			  }

			  return ( false );
	}

	return ( true );
}

bool chromatic_imap_mgr::on_capability( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	const string str( "IMAP4rev1" );

	ostr << format( fmt ) % str;
	
	return ( true );
}

bool chromatic_imap_mgr::on_delete( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = extract_quoted_string( params );
	

	dAtom = "/" + ucase( user() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = check_folder_name( mailbox );
	}
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( user() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         throw chromatic_imap_mgr_error( "can't delete inbox" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	long dwSelectable = 0L;
	long dwMarked = 0L;
	long dwInferiors = 0L;
	long dwPublic = 0L;

	try
	{
		    SACommand o( &m_db );	

			if( bRoot == true ) 
			{				 			

				  o.setCommandText( "call get_subcribe_folder_attributes( :1 )" );
				  o << (char*) atom.c_str();			 

				  o.Execute();

				  subatom = "/" + ucase( user() ) + "/";

				  
				  while( o.FetchNext() )
				  {

					  dwSelectable = o.Field( "select_" ).asLong();
					  dwMarked = o.Field( "marked" ).asLong();
					  dwInferiors = o.Field( "public" ).asLong();
					  dwPublic = o.Field( "public" ).asLong();				 
                                        
  					  if( dwSelectable == 0 )
					  {
						throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
					  }	

					  long zero = 0;
					  //parent folder selectable , remove messages , mark as no select
					  o.setCommandText( "update_subscribe_folder_attributes( :1 , :2 , :3 , :4 , :5 )" );
					  o << (char*) atom.c_str();
					  o	<< (long) zero;
					  o << dwInferiors;
					  o	<< dwMarked;
					  o	<< dwPublic;

					  o.Execute();	
					  
					  return ( true );

				  }										
			  }

			  o.setCommandText( "call delete_from_subscribe_folders( :1 )" );
			  o << (char*) atom.c_str();			 

              o.Execute();              
					
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {
				  cerr << (const char*) x.ErrText() << "\n";				  
			  }

			  return ( false );
	}

	return ( true );
}

bool chromatic_imap_mgr::on_rename( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string atom;
	string oldmailbox;
	string newmailbox;
	string subatom;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 2 )
	{
		return ( false );
	}
	oldmailbox = extract_quoted_string( imp[0] );
	newmailbox = extract_quoted_string( imp[1] );

	atom = check_folder_name( oldmailbox );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( user() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         //todo move messages
	}

	try
	{
			 
			  SACommand o( &m_db );	
			  string str( ucase( user() ) );

			  o.setCommandText( "call rename_subscribe_folders( :1 , :2 , :3 )" );
			  o << (char*) atom.c_str();
			  o << (char*) newmailbox.c_str();
			  o << (char*) str.c_str();			 	  

			  o.Execute();
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  					 	  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") , (const char*) x.ErrText() ) );	

			  return ( false );
	}

	return ( true );
}

bool chromatic_imap_mgr::on_noop( const std::string& params , const std::string& fmt , std::ostream& ostr )
{

	try
	{
		  SACommand o( &m_db );	

		
		  o.setCommandText( "call has_new_messages_by_folder_ex( :1 , :2 )" );
		  o << (char*) mbx_info().name().c_str();
		  o << (long) messages()->size();

		  o.Execute();

		  long dwCount = 0L;
		  
		  while( o.FetchNext() )
		  {

			  dwCount = o.Field( "msg_count" ).asLong();			  
			  if( dwCount != 0 )
			  {
				  
				  update_mailbox_info_by_db( mbx_info().name() );
				  ostr << "* " << messages()->size() << " exists" << crlf
				       << "* " << mbx_info().recent() << " recent" << crlf;
			  }                                

		  }	       
					
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  		  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") , (const char*) x.ErrText() ) );

			  return ( false );
	}

	return ( true );
}

bool chromatic_imap_mgr::on_subscribe( const std::string& params , const std::string& fmt , std::ostream& ostr )
{

	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = extract_quoted_string( params );
	
	dAtom = "/" + ucase( user() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = check_folder_name( mailbox );
	}
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call insert_subscription( :1 , :2 )" );
			  o << (char*) atom.c_str();
			  o << (char*) user().c_str();

              o.Execute();              
					
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  						  			  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") ,  (const char*) x.ErrText() ) );		

			  return ( false );
	}
	
	return ( true );
}

bool chromatic_imap_mgr::on_unsubscribe( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = extract_quoted_string( params );
	
	dAtom = "/" + ucase( user() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = check_folder_name( mailbox );
	}
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call delete_subscription( :1 , :2 )" );
			  o << (char*) atom.c_str();
			  o << (char*) user().c_str();

              o.Execute();              
					
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  						  			  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") ,  (const char*) x.ErrText() ) );		

			  return ( false );
	}
	
	return ( true );
}

bool chromatic_imap_mgr::on_examine( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string atom;
	string mailbox( params );
	string subatom;
	bool isInbox = false;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;

			
	if( mailbox.empty() )
		return ( false );	

	//inbox
    subatom = "/" + ucase( user() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
      isInbox = true;   
	}

	atom = check_folder_name( mailbox );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{    
		 //select always succeeds for inbox , even it does not exist
		if( isInbox == false )
		{
			throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
		}
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	chromatic_mailbox_info info;
	ostringstream dostr;

	update_mailbox_info_by_db( atom , true );

	select_response( mbx_info() , dostr , true );

	ostr << format( fmt ) % dostr.str();

	return ( true );
}

bool chromatic_imap_mgr::on_check( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}

bool chromatic_imap_mgr::on_search( const string& params , const string& fmt , std::ostream& ostr )
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_mgr::OnSearch" ) , __LINE__ );

	/*imap_strings imp;
	string s( params );
	string atoms;
	string message_set;
	v_int v;
	chromatic_imap_messageManip::v_int ret_v_ints;
	
	
	unsigned long dwMin = 0L;
	unsigned long dwMax = 0L;
	
    
	Uids() == false ? dwMax = messages()->size() : dwMax = MaxUid();
	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		message_set = extract_quoted_string( message_set );
		if( IsMessageSet( message_set , dwMax ) == false )
		{
			message_set = "nil";
			atoms = s;
		}
		else
		{
			atoms = s.substr( ++idx );			
		}
		
		if ( atoms.empty() ) 
		{
			throw chromatic_imap_mgr_error( "search parse error" , lmlCritical ) ;
		}		

		try
		{
			auto_ptr<OemoImapSearchCriteria> oisc ( new OemoImapSearchCriteria( atoms , message_set , Uids() ) );
			auto_ptr<OemoImapIsysSearchResolver> oisr( new OemoImapIsysSearchResolver( this ,
																					   ImapHandler()->Acceptor()->Data()->IsysPath() ,
																					   oisc.get() ,
																					   ImapHandler()->Acceptor()->Data()->DocumentPath() ,
																					   UserDomain() ) );
			

			oisc->DeconstructCriteria( oisr.get() );

			if( Uids() == false )
			{
				ret_v_ints = oisr->UidsToMessageNumbers( oisr->SearchResults() );
			}
			else
			{
				ret_v_ints = oisr->SearchResults();
			}

			ostr << SearchResponse( ret_v_ints );

			
		}
		catch( OemoImapSearchError& e )
		{
			throw chromatic_imap_mgr_error( e.what() , lmlCritical );
		}
		
				
	}
	else
	{
		throw chromatic_imap_mgr_error( "search parse error" , lmlCritical ) ;
	}*/
	
	return ( true );
}

bool chromatic_imap_mgr::on_copy( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string atom;	
	string message_set;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	unique_ptr<chromatic_message_manip> cma ( new chromatic_message_manip() );

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 2 )
	{
		return ( false );
	}
	message_set = extract_quoted_string( imp[0] );
	atom = extract_quoted_string( imp[1] );

	atom = check_folder_name( atom );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}	

	v_int v ( cma->expand_message_sequence( message_set , messages()->size() ) );
	v_int vMessageNums;

	for( imap_server_messages::iterator iter = messages()->begin();
			iter != messages()->end();
				iter++ )
				{
					if( uids() == true )
					{
						vMessageNums.push_back( iter->second.uid() );
					}
					else
					{
						vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) );
					}

				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	v_int vRequestSet = cma->atoms_intersection( v , vMessageNums );

	ostringstream dostr;
	string s;
	
	for( v_int::iterator dter = vRequestSet.begin();
		dter != vRequestSet.end();
			dter++ )
			{
				if( uids() == false )
				{	
					imap_server_messages::iterator siter = messages()->find( boost::lexical_cast<string>( *dter ) );
					if( siter != messages()->end() )
					{
						dostr <<  "'"  << siter->second.uid() << "'" << ",";
					}
				}
				else
				{
					    dostr <<  "'"  << *dter <<  "'"  << ",";
				}
			}
			s = dostr.str();
			s = rtrim( s , "," );	

	try
	{
			 
			  SACommand o( &m_db );	
			  string str( ucase( user() ) );

			  o.setCommandText( "call copy_messages_from_subscribe_folder( :1 , :2 , :3 , :4 )" );
			  o << (char*) mbx_info().name().c_str();
			  o << (char*) user().c_str();
			  o << (char*) dostr.str().c_str();				  
			  
			  o.Execute();
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  					 	  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") , (const char*) x.ErrText() ) );	

			  return ( false );
	}
	return ( true );
}

bool chromatic_imap_mgr::on_fetch( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string s( params );
	string atoms;
	string message_set;
	v_int v;

	unique_ptr<chromatic_message_manip> cma ( new chromatic_message_manip() );
			
	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		atoms = s.substr( ++idx );
		//atoms = ltrim( atoms , " " );
	}

	
	message_set = extract_quoted_string( message_set );
	if( atoms.find( right_bracket ) == string::npos )
	{
		atoms = cma->extract_parenthetical_atoms( atoms  );
	}
	
		
	atoms = extract_quoted_string( atoms );

	if ( ( message_set.empty() ) || ( atoms.empty() ) )
	{
		throw chromatic_imap_mgr_error( "fetch parse error" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	update_mailbox_info_by_db( mbx_info().name() );
	
	unsigned long dwMax = 0L;
	
    
	uids() == false ? dwMax = messages()->size() : dwMax = max_uid();
	v = cma->expand_message_sequence( message_set , dwMax );
	v_int vMessageNums;

	for( imap_server_messages::iterator iter = messages()->begin();
			iter != messages()->end();
				iter++ )
				{
					if( uids() == true ) { vMessageNums.push_back( iter->second.uid() ); }
					else { vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) ); }
				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	v_int vRequestSet = cma->atoms_intersection( v , vMessageNums );

	chromatic_imap_fetch_requests requests;
	parse_fetch_request_list( atoms , requests );

	string str;
	for( v_int::iterator dter = vRequestSet.begin();
			dter != vRequestSet.end();
				dter++ )
				{
					
					ostr << fetch_message( *dter , ostr , requests );
				}											
				
								

	return ( true );
}

bool chromatic_imap_mgr::on_status( const string& params , const string& fmt , std::ostream& ostr )
{
	//todo bad metrics here
	imap_strings imp;
	string atoms( lcase( params ) );
	string folder;
	string mailbox;
	string subatom;
	ostringstream dostr;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	unsigned short one = 1;


	string::size_type idx = atoms.find_first_of( " " );
	if( idx != string::npos )
	{
		subatom = atoms.substr( 0 , idx );
		mailbox = extract_quoted_string( subatom );
	}
	else
	{
		return ( false );
	}
	atoms = atoms.substr( idx );

	idx = atoms.find_first_of( "(" );
	if( idx != string::npos )
	{
		atoms.erase( idx , one );
		string::size_type idx = atoms.find_last_of( ")" );
		if( idx != string::npos ) {	atoms.erase( idx , one ); }	else { return ( false ); }
	}
	else
	{
		return ( false );
	}

	atoms = rtrim( atoms , " " );
	atoms = ltrim( atoms , " " );
	safe_tokenize_t( imp , (char*) atoms.c_str() , " " );
	if( imp.empty() ) return ( false );

	folder = check_folder_name( mailbox );
    result = folder_exists( folder );
	if( result == chromatic_mailbox_result::omrNotFound )
	{
		 throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}

	chromatic_mailbox_info info;
	update_mailbox_info_by_db( folder , info );

	status_response( info , dostr , imp );

	ostr << format( fmt ) % mailbox % dostr.str();

	return ( true );
}

bool chromatic_imap_mgr::on_store( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string s( lcase( params ) );
	string atoms;
	string message_set;
	chromatic_mail_message_flags omf;
	v_int v;
	chromatic_flags_method fm;
	bool bSilent = false;
	omf.clear();

	unique_ptr<chromatic_message_manip> cma ( new chromatic_message_manip() );

	if( s.empty() )
		throw chromatic_imap_mgr_error( "store parse error" , chromatic_imap_mgr_error_level::lmlCritical );

	if( mbx_info().read_only() == true )
	{
		throw chromatic_imap_mgr_error( "Mailbox is read-only" , chromatic_imap_mgr_error_level::lmlCritical );
	}

	if( s.find( "silent"  ) != string::npos )
		bSilent = true;

	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		atoms = s.substr( ++idx );		
	}

	
	message_set = extract_quoted_string( message_set );
	atoms = cma->extract_parenthetical_atoms( atoms  );
	atoms = extract_quoted_string( atoms );
	atoms = lcase( atoms );	

	str_to_mbx_flags( atoms , omf );
	if( s.find( "+flags" ) != string::npos)
	{
		fm = chromatic_flags_method::ofmAdd;
	}
	else if( s.find( "-flags" ) != string::npos )
	{
		fm = chromatic_flags_method::ofmRemove;
	}
	else
	{
		fm = chromatic_flags_method::ofmReplace;
	}	
	

	unsigned long dwMax = 0L;
	uids() == false ? dwMax = messages()->size() : dwMax = max_uid();

	v = cma->expand_message_sequence( message_set , dwMax );
	v_int vMessageNums;

	for( imap_server_messages::iterator iter = messages()->begin();
			iter != messages()->end();
				iter++ )
				{
					if( uids() == true ) { vMessageNums.push_back( iter->second.uid() ); }
					else { vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) ); }
				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	v_int vRequestSet = cma->atoms_intersection( v , vMessageNums );

	for( v_int::iterator dter = vRequestSet.begin();
			dter != vRequestSet.end();
				dter++ )
				{
					store_flags( *dter , omf , fm , ostr , bSilent );
				}			
	

	return ( true );
}

bool chromatic_imap_mgr::on_uid( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}

bool chromatic_imap_mgr::on_close( const string& params , const string& fmt , std::ostream& ostr )
{
	close_expunge( true );

	return ( true );
}

bool chromatic_imap_mgr::on_expunge( const string& params , const string& fmt , std::ostream& ostr )
{	
	
	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call expunge_messages_by_idx( :1 )" );
			  o << (char*) mbx_info().name().c_str();
			 

              o.Execute();   
			  

			  v_int v;
			  for( imap_server_messages::iterator iter = messages()->begin();
						iter != messages()->end();
							iter++ )
							{
								if( iter->second.flags().find( chromatic_mail_message_flag::omfDeleted )
										!= iter->second.flags().end() )
								{
									ostringstream dostr;
									dostr << proxy_store()
										  << "\\"
										  << iter->second.offset()
										  << "\\"
										  << iter->second.url()
										  << ".xml";
									if( remove( (char*)  dostr.str().c_str() ) != 0 )
									{
										throw chromatic_imap_mgr_error( "error in expunge message" ,
																        chromatic_imap_mgr_error_level::lmlCritical ) ;
									}
									v.push_back( boost::lexical_cast<int>( iter->first ) );
								}
							}
			  sort( v.begin() , v.end() );
			  unsigned long dw = 0L;

			  if( close_expunge() == false )
			  {
				  for( v_int::iterator dter = v.begin();
							dter != v.end();
								dter++ )
								{
									ostr << "* " << (*dter) - dw << " expunged" << crlf;
									++dw;
								}


				  update_mailbox_info_by_db( mbx_info().name() );
			  }
			  else
			  {
				  messages()->clear();
				  mbx_info().clear();
				  close_expunge( false );
			  }
					
	}
	catch(SAException &x)
	{
			  try
			  {
				   // on error rollback changes
				   m_db.Rollback();
			  }
			  catch( SAException & )
			  {				  						  			  
			  }
			  ACE_DEBUG ( ( LM_ERROR, ACE_TEXT ("%T (%t): %s.\n") ,  (const char*) x.ErrText() ) );		

			  return ( false );
	}
	return ( true );
}

bool chromatic_imap_mgr::on_append( const string& params , const string& fmt , std::ostream& ostr )
{
	imap_strings imp;
	string atom;	
	string strFlags;
	unsigned short zero = 0;
	unsigned short one = 1;
	unsigned short two = 2;
	string subatom;
	string temp;
	chromatic_mailbox_result result = chromatic_mailbox_result::omrNotFound;
	chromatic_mail_message_flags  flags;

	append_rec().clear();

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 3 )
	{
		return ( false );
	}
     
	temp = extract_quoted_string( imp[zero] );
	append_rec().mbx( temp );
	strFlags = lcase( imp[one] );
	string::size_type idx = strFlags.find_first_of( "(" );
	if( idx != string::npos )
	{
		strFlags.erase( idx , one );
		idx = strFlags.find_last_of( ")" );
		if( idx != string::npos )
		{
			strFlags.erase( idx , one );
		}
		else
		{
			return ( false );
		}
	}
	else
	{
		return ( false );
	}
	str_to_mbx_flags( lcase( strFlags ) , flags );
	append_rec().flags( flags );

	subatom = imp[two];
	idx = subatom.find_first_of( "{" );
	if( idx != string::npos )
	{
		subatom.erase( idx , one );
		idx = subatom.find_last_of( "}" );
		if( idx != string::npos )
		{
			subatom.erase( idx , one );
		}
		else
		{
			return ( false );
		}

		append_rec().size( atoi( (char*) subatom.c_str() ) );
	}
	else
	{
		return ( false );
	}


     
	atom = check_folder_name( append_rec().mbx() );
    result = folder_exists( atom );
	if( result == chromatic_mailbox_result::omrNotFound )
	{    		
		throw chromatic_imap_mgr_error( "mailbox does not exist" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}
	else if( result == chromatic_mailbox_result::omrAccessDenied )
	{
		 throw chromatic_imap_mgr_error( "access denied" , chromatic_imap_mgr_error_level::lmlCritical ) ;
	}	

	return ( true );
}

bool chromatic_imap_mgr::on_starttls( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::lcase( const string& str )
{
	string localstr( str );

	if( localstr.empty() )
	{
		return( "" );
	}

	string::iterator iter = localstr.begin();
	while ( iter != localstr.end() )
	{
		*iter = tolower( *iter );
		iter++;
	}

	return ( localstr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::extract_quoted_string( const std::string& str )
{
	   string s( str );
	   if( s.empty() )
	   {
		   return ( s );
	   }

	   string::size_type idx = s.find_first_of( "\"" );
	   if( idx != string::npos )
	   {
			s.erase( idx , 1 );
			idx = s.find_last_of( "\"" );
			if( idx != string::npos )
			{
				s.erase( idx , 1 );
			}
	   }
	   

	   return ( s );	  

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::ucase( const string& str )
{
	string localstr( str );

	if( localstr.empty() )
	{
		return( "" );
	}

	string::iterator iter = localstr.begin();
	while ( iter != localstr.end() )
	{
		*iter = toupper( *iter );
		iter++;
	}

	return ( localstr );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::store_response( const std::string& msg_id , const std::string& payload , std::ostream& ostr )
{
		ostr << "* " 
			 << msg_id
			 << " FETCH ("
			 << payload
			 << ")"
			 << crlf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::store_flags( unsigned long msg_id , chromatic_mail_message_flags flags ,
							          chromatic_flags_method  method , ostream& ostr , bool bSilent )
{
	string req_str;	
	ostringstream dostr;
	ostringstream fetch_stream;
	chromatic_imap_fetch_request req;
	chromatic_imap_message msg;
	string s;
	chromatic_mail_message_flags local_flags;

	if( uids() == false )
	{
		dostr << msg_id;
		imap_server_messages::iterator siter = messages()->find( dostr.str() );
		if( siter != messages()->end() )
		{
			msg = siter->second;
			local_flags = msg.flags();
		}
		
	}
	else
	{
		for( imap_server_messages::iterator siter = messages()->begin();
				siter != messages()->end();
					siter++ )
					{
						if( (unsigned long) siter->second.uid() == msg_id )
						{
							msg = siter->second;
							local_flags = msg.flags();
							dostr.str( "" );
							dostr << siter->first;							

							break;
						}						
					}
	}
	
	if( method == chromatic_flags_method::ofmReplace )
	{
		local_flags = flags;
	}
	else 
	{
		for( chromatic_mail_message_flags::iterator iter = flags.begin();
				iter != flags.end();
					iter++ )
					{
						if( method == chromatic_flags_method::ofmAdd )local_flags.insert( *iter );
							else if( method == chromatic_flags_method::ofmRemove) local_flags.erase( *iter );
					}
	}

	set_message_flags_by_db( msg.url() , local_flags );
	msg.flags( local_flags );

	string strTemp( mbx_flags_to_str( local_flags ) );

	if( bSilent == false ) store_response( dostr.str() , strTemp , ostr );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::str_to_mbx_flags( const std::string& flag_string , chromatic_mail_message_flags&  ommf )
{
	 
	 if( flag_string.find( "\\Answered" ) != string::npos ) ommf.insert( chromatic_mail_message_flag::omfAnswered );
	 if( flag_string.find( "\\Flagged" )  != string::npos ) ommf.insert( chromatic_mail_message_flag::omfFlagged );
	 if( flag_string.find( "\\Deleted" ) != string::npos ) ommf.insert( chromatic_mail_message_flag::omfDeleted);
	 if( flag_string.find( "\\Seen" ) != string::npos ) ommf.insert( chromatic_mail_message_flag::omfSeen );
	 if( flag_string.find( "\\Draft" ) != string::npos ) ommf.insert( chromatic_mail_message_flag::omfDraft );
	 if( flag_string.find( "\\Recent" ) != string::npos ) ommf.insert( chromatic_mail_message_flag::omfRecent );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::mbx_attributes_to_str( chromatic_mailbox_attributes omba )
{
	ostringstream ostr;
	string str;


	for( chromatic_mailbox_attributes::iterator iter = omba.begin();
			iter != omba.end();
				iter++ )
				{
					if ( (*iter) == chromatic_mailbox_attribute::omaNoinferiors ) ostr << "\\Noinferiors ";
					else if( (*iter) == chromatic_mailbox_attribute::omaNoselect ) ostr << "\\Noselect ";
					else if( (*iter) == chromatic_mailbox_attribute::omaMarked ) ostr << "\\Marked ";
					else if( (*iter) == chromatic_mailbox_attribute::omaUnmarked ) ostr << "\\Unmarked ";
				}

	str = ostr.str();
	string::size_type idx = str.find_last_of( " " );
	if( idx != string::npos ) str.erase( idx );

	return ( str );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string chromatic_imap_mgr::mbx_flags_to_str( chromatic_mail_message_flags  ommf )
{
	ostringstream ostr;
	string str;


	for( chromatic_mail_message_flags::iterator iter = ommf.begin();
			iter != ommf.end();
				iter++ )
				{
					if ( (*iter) == chromatic_mail_message_flag::omfAnswered  ) ostr << "\\Answered ";
					else if( (*iter) == chromatic_mail_message_flag::omfFlagged ) ostr << "\\Flagged ";
					else if( (*iter) == chromatic_mail_message_flag::omfDeleted ) ostr << "\\Deleted ";
					else if( (*iter) == chromatic_mail_message_flag::omfSeen ) ostr << "\\Seen ";
					else if( (*iter) == chromatic_mail_message_flag::omfDraft ) ostr << "\\Draft ";
					else if( (*iter) == chromatic_mail_message_flag::omfRecent ) ostr << "\\Recent ";
				}

	str = ostr.str();
	string::size_type idx = str.find_last_of( " " );
	if( idx != string::npos ) str.erase( idx );

	return ( str );

}

//////////////////////////////////////////////////
//pairfromtuple
pair<string,string> chromatic_imap_mgr::pair_from_tuple( const std::string& tuple )
{
	pair<string,string> pr;
	vtor_string vStr;

	safe_tokenize_t( vStr , (char*) tuple.c_str() , " " );
	if( vStr.size() == 2 )
	{
		pr.first = vStr[0];
		pr.second = vStr[1];
	}

	return ( pr );
}


bool chromatic_imap_mgr::read_params()
{
	  /* ostringstream ostr;

	   int result;
	   ACE_Registry::Naming_Context parent_context;
	   ACE_Registry::Naming_Context application_context;

	   // Connect to predefined entry
	   result = ACE_Predefined_Naming_Contexts::connect ( parent_context,
													     HKEY_LOCAL_MACHINE );

	   ACE_Registry::Name_Component component;	
	   ACE_Registry::Name context;	    

	   component.id_ = ACE_TEXT ("Software"),   context.insert (component);
	   component.id_ = ACE_TEXT ("Overtone Software"),   context.insert (component);
	   component.id_ = ACE_TEXT ("ManageToneEX"),	    context.insert (component);
	   component.id_ = ACE_TEXT ("Parameters"), context.insert (component);
	   component.id_ = ACE_TEXT ("OemoSearchProtocol"), context.insert (component);
	   
	   // Find application context name
	   result = parent_context.resolve_context ( context,
											     application_context );

	   if( result != 0 )
	   {
		   return ( false );
	   }

	  Params().clear();	   	   
	   
	  ACE_Registry::Binding_List list;

      // get the list of all entries
	  result = application_context.list (list);
	  if ( result != 0 )
	  {
		return ( false );
	  }        	 

	  // iterate through all entries
	  for (ACE_Registry::Binding_List::iterator i = list.begin ();
		   i != list.end ();
		   ++i)
		{      
		  ACE_Registry::Binding &binding = *i;
		  

		  if (binding.type () == ACE_Registry::OBJECT)
		  {
			char buffer[256];
			ACE_Registry::Object object ((void *) buffer ,
			       sizeof buffer,
			       REG_SZ );
			
			result = application_context.resolve ( binding.name() , object);
			string str( (char*) object.data() );

			Params().insert( make_pair( binding.name().c_str() , str ) );
		
		  }	  
		  
		}  */

		return ( true );
	   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::select_response( chromatic_mailbox_info info , std::ostream& ostr , bool bReadOnly )
{
	ostringstream dostr;

	dostr << "* " << info.exists() << " EXISTS" << crlf
	      << "* " << info.recent() << " RECENT" << crlf
	      << "* FLAGS (" << mbx_flags_to_str( info.changeable_flags() ) << ")" << crlf
	      << "* OK [UNSEEN " << info.unseen() << "]" << crlf //info.FirstUnseen() << " is first unseen"  << crlf
	      << "* OK [UIDVALIDITY " << info.uid_validity() << "] UIDs valid" << crlf
		  << "* OK [PERMANENTFLAGS (" << mbx_flags_to_str( info.flags() ) << ")]" << crlf;


	ostr << dostr.str();


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_imap_mgr::status_response( chromatic_mailbox_info info , std::ostream& ostr ,  imap_strings vStr )
{

    ostringstream payload;
	string strPayload;

	for( imap_strings::iterator iter = vStr.begin();
			iter != vStr.end();
			  iter++ )
			  {
					string str( lcase( *iter ) );

					if( str.compare( "messages" ) == 0 )
					{
						 payload << "messages " << info.exists() << " ";
					}
					else if( str.compare( "uidnext" ) == 0 )
					{
						 payload << "uidnext " << info.uid_next() << " ";
					}
					else if( str.compare( "recent" ) == 0 )
					{
						 payload << "recent " << info.recent() << " ";
					}
					else if( str.compare( "uidvalidity" ) == 0 )
					{
						 payload << "uidvalidity " << info.uid_validity() << " ";
					}
					else if( str.compare( "unseen" ) == 0 )
					{
						 payload << "unseen "  << info.unseen() << " ";
					}			

			  }
			  strPayload = ucase( payload.str() );
			  strPayload = rtrim( strPayload  , " " );

			  ostr << strPayload;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::fetch_response( const string& msg_id , const string& payload , ostream& ostr )
{	
	ostringstream dostr;
	dostr << "* " 
		 << msg_id
		 << " FETCH ("
		 << payload		
		 << ")"
		 << crlf;

	
	
	return ( dostr.str() );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::search_response( const chromatic_imap_protocol_impl::v_int& v_ints )
{
	ostringstream dostr;
	dostr << chromatic_imap_mgr::asterisk << " SEARCH ";

	for( v_int::const_iterator iter = v_ints.begin();
						iter != v_ints.end();
							iter++ )
	{
		dostr << *iter;		
		dostr << chromatic_imap_mgr::space;
	}
	string str( rtrim( dostr.str() , chromatic_imap_mgr::space ) );
	str += crlf;	

	return ( str );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::http_to_file_url( const string& str )
{

	ostringstream ostr;

	if( str.empty() )
	{
		return( str );
	}

	/*string strPayload;
	string strDbl( "//" );
	string strBackOffice( "file://./backofficestorage" );
	string strDomain;
	GetDomainName( strDomain );
	string strMbx( "MBX" );

	string::size_type idx = str.find_first_of( strDbl );
	if( idx != string::npos )
	{
		if( idx != string::npos )
		{
			idx += strDbl.size();
			idx = str.find( "/" , idx );			
			if( idx != string::npos )
			{
				strPayload = str.substr( idx );				
				idx = strPayload.find( "/" );			
				if( idx != string::npos )
				{
					idx++;
					strPayload = strPayload.substr( idx );
				}
				idx = strPayload.find( "/" );	
				if( idx != string::npos )
				{					
					idx++;
					strPayload = strPayload.substr( idx );
				}
			}
		}
	}

	ostringstream ostr;
	ostr << strBackOffice << "/" << strDomain << "/" << strMbx << "/" << strPayload;*/

	return ( ostr.str() );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::message_tag( const std::string& fileurlstr  )
{
	// CoInitialize( NULL );
	 if( fileurlstr.empty() == true )
	 { 
		 return ( "" );
	 } 
	
	 string strTag( "backofficestorage"  );
	 string strServer;

	 string::size_type idx = fileurlstr.find( strTag );
	 if( idx != string::npos )
	 {
		idx += strTag.size();
		idx = fileurlstr.find( "/" , idx );
		if( idx != string::npos )
		{			
			idx++;
			string::size_type jdx = idx;
			idx++;
			idx = fileurlstr.find( "/" , idx );
			if( idx != string::npos )
			{
				strServer = fileurlstr.substr( jdx , idx - jdx );
			}
		}
	 }

	 string str( file_to_http_url( fileurlstr , lcase( strServer ) ) );
	 
	 string strPerm = str;
	 
	 return ( strPerm );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::file_to_http_url( const std::string& fileurl , const std::string& server )
{
    string str( fileurl );
	string::size_type idx;
	ostringstream ostr;
	bool isMailbox = false;

 	if( fileurl.empty() == true )
	{
		return ( str );
	}

	const string mbx ( "MBX" );
	const string folderpublic( "Public Folders" );
	const string publicprefix( "public" );
	const string exchangeprefix( "exchange" );
	const string httpprefix ( "http://" );
	const string fileprefix ( "file://./backofficestorage" );
	
	idx = fileurl.find( fileprefix );
	if( idx == string::npos )
	{
		//invalid spec
		return ( str );
	}
	
	idx = fileurl.find( mbx );
	if( idx != string::npos )
	{
		//mailbox
		isMailbox = true;
	}


	//http
	ostr << httpprefix;
	//server
	ostr << server;
	
	//check for mailbox
	if( isMailbox == true )
	{
		//truncate		
		str = str.substr( idx );
				
		idx = str.find_first_of( "/" );
		if( idx != string::npos )
		{
			idx++;
			str = str.substr( idx );
		}

		ostr << "/" << exchangeprefix << "/" << str;

	}
	else
	{
		//public
		idx = fileurl.find( folderpublic );
		if( idx == string::npos )
		{
			return ( str );
		}
		else
		{
			//truncate		
			idx++;
			str = str.substr( idx );			

			ostr << "/" << exchangeprefix << "/" << str;
		}
	}

	str = ostr.str();

	return ( str );


}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::rtrim( const string &szToTrim, const string& szTrimChars )
{
    std::string szToReturn = szToTrim;

    int nLastPos = szToReturn.length() - 1;

    if( nLastPos <= 0 )
	{
        return ( "" );
	}

    while( strchr( szTrimChars.c_str(), szToReturn[nLastPos] ) != NULL )
    {
        szToReturn.replace( nLastPos, 1, "" );

        nLastPos = szToReturn.length() - 1;

        if( nLastPos <= 0 )
		{
            return ( "" );
		}
    }

    return szToReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::ltrim( const string &szToTrim, const string& szTrimChars )
{
    std::string szToReturn = szToTrim;

    while( strchr( szTrimChars.c_str(), szToReturn[0] ) != NULL )
	{
		if( szToReturn.length() < 1 )
		{
			return ( "" );
		}

        szToReturn.replace( 0, 1, "" );
	}

    return szToReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_imap_mgr::base64_encode( unsigned char const* bytes_to_encode , unsigned int in_len )
{
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}


string chromatic_imap_mgr::base64_decode( string const& encoded_string )
{
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

