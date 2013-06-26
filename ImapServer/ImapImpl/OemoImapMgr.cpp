///////////////////////////////////////////////////////////
//  OemoImapMgr.cpp
//  Implementation of the Class OemoImapMgr
//  Created on:      29-Sept-2007 10:11:28 AM
///////////////////////////////////////////////////////////
#include "ace/Registry.h"
#include "ace/Trace.h"
#ifndef OEMO_IMAP_IMPL_H
#include "OemoImapImpl.h"
#endif



using namespace OemoImapMessageManip;
using namespace OemoImapProtocolImpl;

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
using boost::format;
using boost::cmatch;
using boost::regex_match; 
using boost::regex;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::algorithm::to_lower;

//immutable

const string OemoImapMgr::constCryptUnlockCode = "SFCNICOMCrypt_BH2SCUed8SPi";
const string OemoImapMgr::constMimeUnlockCode = "SFCNICOMSMIME_sK7NCgi98C5p";
const string OemoImapMgr::constMailUnlockCode = "SFCNICOMMAILQ_5NklwCi28R6H";
const string OemoImapMgr::crlf = "\015\012";
const string OemoImapMgr::space = " ";
const string OemoImapMgr::asterisk = "*";
const string OemoImapMgr::right_paren = ")";
const string OemoImapMgr::left_paren = "(";
const string OemoImapMgr::left_bracket = "[";
const string OemoImapMgr::right_bracket = "]";
const string OemoImapMgr::left_brace = "{";
const string OemoImapMgr::right_brace = "}";
const string OemoImapMgr::quote = "\"";
const unsigned long OemoImapMgr::one = 1L;
const unsigned long OemoImapMgr::two = 2L;
const unsigned long OemoImapMgr::short_call = 512;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ctors
OemoImapMgr::OemoImapMgr( const std::string& dbserver  , 
						  const std::string& proxystore ,
						  OemoHandlerPtr imap_handler ) :   m_bConnected( false ) ,
														    m_bCloseExpunge( false ) ,
														    m_ptrStream( NULL ) ,
														    m_strDbServer( dbserver ) ,
														    m_ptrMessages( new ImapServerMessages ) ,
														    m_strProxyStore( proxystore ) ,
														    m_dynHandle( NULL ) ,
														    m_dynUtilsHandle( NULL ) ,
														    m_ptrHandler( imap_handler )
							 
							 
{	
	 ACE_DEBUG ( ( LM_INFO , "Constructiing imap manager....\n" ) );		

	 try
	 {
		CoInitialize( NULL );	
		
		OpenServer();

		ReadParams();
		
	 }
	 catch( OemoImapMgrError& e )
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
OemoImapMgr::~OemoImapMgr()
{

	ACE_DEBUG ( ( LM_INFO , "Destructing imap manager....\n" ) );

	CoUninitialize();

	DeInit();

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//openserver
bool OemoImapMgr::OpenServer()
{
	HRESULT hr = S_OK;	
	bool bRet = false;

	 ACE_DEBUG ( ( LM_INFO , "Opening server....\n" ) );		
	
	try
	{
		
	  ostringstream ostr;
      ostr    << "Driver={MySQL ODBC 5.1 Driver};"
               << "Server=" << (char*) DbServerString().c_str() << ";"
               << "Port=3306;"
               << "Option=131072;"
               << "Stmt=;"
               << "Database=oemo;"
               << "Uid=overtone;"
               << "Pwd=oemo";
       Dsn( ostr.str() );

					 
	   m_db.Connect( (char*) Dsn().c_str() ,
					  "overtone" ,
					  "oemo" ,
					  SA_ODBC_Client  );	  
		
		
	   m_dynHandle = LoadLibrary( "OemoImapStrings.dll" );
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
		}
	}
	catch( _com_error& e )
	{
		*OutputStream() << FormatComError( &e );		
	
		//rethrow
		throw OemoImapMgrError( FormatComError( &e ) , lmlCritical );	
		 
	}
	catch( OemoImapMgrError& oe )
	{
		
		//rethrow
		throw OemoImapMgrError( oe.what() , lmlCritical ) ;
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
				  throw OemoImapMgrError( (const char*) x.ErrText() , lmlCritical ) ;
			  }
			  
			  
	}
	catch( ... )
	{
		//rethrow
		throw OemoImapMgrError( "Unspecified error in opening server....." , lmlCritical );	
	}
	
	return ( true );

}

BOOL OemoImapMgr::MakeDirectory(const std::string& newdir )
{
	try
	{
		if(_access(newdir.c_str(), 0) == -1)
		{
			return CreateDirectory( newdir.c_str(),NULL );
		}
		return true;
	}
	catch( ... )
	{
		throw OemoImapMgrError( "Make Directory failed..." , lmlDebug );
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//formatcomerror
string OemoImapMgr::FormatComError( _com_error* ptrError )
{
	try
	{
		ostringstream ostr;
		ostr << "error#: " << ptrError->Error() << "\n";
		ostr << "error_message: " << (char*) (_bstr_t) ptrError->ErrorMessage() << "\n";
		ostr << "error_source: " << (char*) (_bstr_t) ptrError->Source() << "\n";
		ostr << "error_description: " << (char*) (_bstr_t) ptrError->Description() << "\n";
		return ( ostr.str() );
	}
	catch( ... )
	{
		return "";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//deinit
void OemoImapMgr::DeInit()
{ 
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
void OemoImapMgr::EnumMailboxList( OemoMailBoxList& mbxs ,
								   const std::string& username ,
								   const std::string& reference ,
								   const std::string& criteria ,
								   bool subscribed )
{
	
 

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

  CollectMailboxesByDb( regEx ,  username , mbxs , subscribed );  

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OemoImapMgr::IsSubscribedByDb( const std::string& folder , const std::string& nickname )
{
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
void OemoImapMgr::CollectMailboxesByDb( const std::string& reg , 
									    const std::string& username ,
										OemoMailBoxList& obl ,
										bool subscribed )
{
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
					   OemoMailBox mbx;					   

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
							  
							  mbx.Subscribed( IsSubscribedByDb( subatom , username ) );
							  if( ( mbx.Subscribed() == false ) && ( subscribed == true ) ) continue;
							  mbx.Name( atom );
							  dwIdx = o.Field( "select_" ).asLong();
							  if( dwIdx == 0 ) mbx.Attributes().insert( omaNoselect );
							  dwIdx = o.Field( "inferiors" ).asLong();
							  if( dwIdx == 0 ) mbx.Attributes().insert( omaNoinferiors );
							  dwIdx = o.Field( "marked" ).asLong();
							  if( dwIdx == 0 ) mbx.Attributes().insert( omaUnmarked ); else mbx.Attributes().insert( omaMarked ) ;
							  
							  obl.push_back( mbx );
						   } 						   

					   }					   
				  }

				
		}
		catch( OemoImapMgrError& oe )
		{			
			//rethrow
			throw OemoImapMgrError( oe.what() , lmlCritical ) ;
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
				  throw OemoImapMgrError( (const char*) x.ErrText() , lmlCritical ) ;
			  }
		
				  
		}
		catch( ... )
		{
			//rethrow
			throw OemoImapMgrError( "Unspecified error in collecting mailboxes ..." , lmlCritical );	
		}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::FillMessageListByDb( const std::string& folder )
{	
		
                                    

     string strServer;
	 string str;
     OemoImapMessage oim;
	 OemoMailMessageFlags flags;
	 unsigned long dwIdx = 1L;
	 

     try
     {
		      Messages()->clear();		  

              SACommand o( &m_db ); 

              o.setCommandText( "call get_user_mail_server_ex( :1 )" );
              o << (char*) User().c_str();

              o.Execute();


              while( o.FetchNext() )
              {
                   strServer = o.Field( "server" ).asString();
				   strServer = lcase( strServer );
              }
			  oim.Server( strServer );

			  o.setCommandText( "call enumerate_messages_by_folder( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();

			  unsigned long dwMax = 0L;

			  while( o.FetchNext() )
              {
                   oim.Uid( o.Field( "idx" ).asLong() );
				   if( oim.Uid() > dwMax )
				   {
					   MaxUid( oim.Uid() );
				   }
				   
				   str = o.Field( "absolute_url_string" ).asString();
				  
				   oim.Url( str );
				   oim.FileUrl( str );
				   str = o.Field( "folder_offset" ).asString();
				   oim.Offset( str );
				   oim.Size( o.Field( "msg_size" ).asLong() );
				   oim.Server( strServer );	

		
				   string furl;
				   string encode_string;	  
				  
				  
				   furl = MessageTag( oim.Url() );
				   string s( base64_encode( (unsigned char const*) furl.c_str() , furl.size() ) );				  
				   
				   oim.Url( s );				   
				   
				   long one = 1L;
				   
				   flags.clear();
				   
				   if( o.Field( "deleted" ).asLong() == one ) flags.insert( omfDeleted ); 
				   if( o.Field( "answered" ).asLong() == one ) flags.insert( omfAnswered ); 
				   if( o.Field( "draft" ).asLong() == one ) flags.insert( omfDraft ); 
				   if( o.Field( "flagged" ).asLong() == one ) flags.insert( omfFlagged ); 
				   if( o.Field( "recent" ).asLong() == one ) flags.insert( omfRecent ); 
				   if( o.Field( "seen" ).asLong() == one ) flags.insert( omfSeen );

				   oim.Flags( flags );

				   ostringstream ostr;
				   ostr << dwIdx;				  

				   pair<string,OemoImapMessage> pr( ostr.str() , oim );
				   Messages()->insert( pr );
				   

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
void OemoImapMgr::UpdateMailBoxInfoByDb( const string& folder , bool bReadOnly )
                                      

{
     ostringstream ostr;
	 long dwSeen = 0L;
	 vector<OemoMailMessageFlag> vFlags;
	 vFlags.push_back( omfAnswered );
	 vFlags.push_back( omfFlagged );
	 vFlags.push_back( omfDeleted );
	 vFlags.push_back( omfSeen );
	 vFlags.push_back( omfDraft );
	 //vFlags.push_back( omfRecent );
	 OemoMailMessageFlags flags;
	 flags.clear();


     try
     {
		      FillMessageListByDb( folder );
		      MbxInfo().clear();

              SACommand o( &m_db );
              
              o.setCommandText( "call get_message_flags_by_folder_ex( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();

			  string str;
			  unsigned long foo = o.RowsAffected();
              while( o.FetchNext() )
              {
				  
					for( int i = 0; i < vFlags.size(); i++ )
					{
						flags.insert( vFlags[i] );
					}
					MbxInfo().Flags( flags );
					flags.erase( omfRecent );
				    MbxInfo().ChangeableFlags( flags );					
				    MbxInfo().Name( folder );					
                    MbxInfo().Recent( o.Field( "dwRecentCount" ).asLong() );					
                    MbxInfo().FirstUnseen( o.Field( "first_unseen" ).asLong() );
					MbxInfo().Exists( o.Field( "dwTotal" ).asLong() ); 
					dwSeen = o.Field( "dwSeenCount" ).asLong();
                    MbxInfo().Unseen(  MbxInfo().Exists() -  dwSeen );
					ostr << o.Field( "dwUidnext" ).asLong();
				    MbxInfo().UidNext( ostr.str() );
					ostr.str( "" );
					ostr << o.Field( "dwValidity" ).asLong();;
                    MbxInfo().UidValidity( ostr.str() );					
					MbxInfo().ReadOnly( bReadOnly );
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
void OemoImapMgr::UpdateMailBoxInfoByDb( const string& folder , OemoMailBoxInfo& info )
{
     ostringstream ostr;
	 long dwSeen = 0L;
	 vector<OemoMailMessageFlag> vFlags;
	 vFlags.push_back( omfAnswered );
	 vFlags.push_back( omfFlagged );
	 vFlags.push_back( omfDeleted );
	 vFlags.push_back( omfSeen );
	 vFlags.push_back( omfDraft );
	// vFlags.push_back( omfRecent );
	 OemoMailMessageFlags flags;
	 flags.clear();

     try
     {
		      FillMessageListByDb( folder );
			  info.clear();

              SACommand o( &m_db );
              
              o.setCommandText( "call get_message_flags_by_folder_ex( :1 )" );
              o << (char*) folder.c_str();

              o.Execute();


              while( o.FetchNext() )
              {
				    for( int i = 0; i < vFlags.size(); i++ )
					{
						flags.insert( vFlags[i] );
					}
					info.Flags( flags );
					flags.erase( omfRecent );
				    info.ChangeableFlags( flags );		
				    info.Name( folder );
                    info.Recent( o.Field( "dwRecentCount" ).asLong() );
                    info.FirstUnseen( o.Field( "first_unseen" ).asLong() );
                    info.Exists( o.Field( "dwTotal" ).asLong() );  
					dwSeen = o.Field( "dwSeenCount" ).asLong();
                    info.Unseen(  info.Exists() -  dwSeen );
					ostr << o.Field( "dwUidnext" ).asLong();
                    info.UidNext( ostr.str() );
					ostr.str( "" );
					ostr << o.Field( "dwValidity" ).asLong();;
                    info.UidValidity( ostr.str() );   					
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
std::string OemoImapMgr::CheckFolderName( const std::string& folder )
{
    string str( folder );

	if( str[0] != '/' )
	{
		str = "/" +  User() + "/" + folder;
	}
	else
	{
		str = "/" + User() + folder;
	}

	return ( ucase( str ) );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OemoMailboxResult OemoImapMgr::FolderExists( const std::string& folder )
{
	 OemoMailboxResult result;

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
                   result = omrNotFound;
                   break;
                case 1 :
                   result = omrSuccess;
                   break;
                case -1 :
                   result = omrAccessDenied;
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
              result = omrAccessDenied;
      }

      return ( result );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FetchMessage( unsigned long msg_id , std::ostream& ostr , OemoImapFetchRequests& requests )
{

	string req_str;	
	ostringstream dostr;
	ostringstream fetch_stream;
	OemoImapFetchRequest req;
	OemoImapMessage msg;
	string s;
	OemoMailMessageFlags flags;
	OemoMailMessageFlags uflags;
	flags.clear();
	uflags.clear();
	string strPayload;
	bool bPeek ( false );
	


	if( Uids() == false )
	{
		dostr << msg_id;
		ImapServerMessages::iterator siter = Messages()->find( dostr.str() );
		if( siter != Messages()->end() )
		{
			msg = siter->second;							
		}
		
	}
	else
	{
		for( ImapServerMessages::iterator siter = Messages()->begin();
				siter != Messages()->end();
					siter++ )
					{
						if( siter->second.Uid() == msg_id )
						{
							msg = siter->second;
							dostr.str( "" );
							dostr << siter->first;							

							break;
						}						
					}
	}
	
	s = ProxyStore() + "\\" + msg.Offset() + "\\" + msg.Url()  +  ".xml";		

	flags = msg.Flags();
	uflags = msg.Flags();

     
	//aggregate keywords
	OemoImapFetchRequests::iterator dter = requests.begin();	
	if( (*dter).first.compare( _R( oemo_imap_fetch_fast ) ) == 0 ) 
	{
		requests.clear();
		req.first = _R( oemo_imap_fetch_flags );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_internaldate );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_rfc822size );
		requests.push_back( req );
	}
	else if( (*dter).first.compare( _R( oemo_imap_fetch_all ) ) == 0 )
	{
		requests.clear();
		req.first = _R( oemo_imap_fetch_flags );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_internaldate );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_rfc822size );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_envelope );
		requests.push_back( req );
	}
	else if( (*dter).first.compare( _R( oemo_imap_fetch_full ) ) == 0 )
	{
		requests.clear();
		req.first = _R( oemo_imap_fetch_flags );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_internaldate );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_rfc822size );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_envelope );
		requests.push_back( req );
		req.first = _R( oemo_imap_fetch_body );
		requests.push_back( req );
	}
    
	if( Uids() == true )
	{
		fetch_stream << "UID" << space << msg.Uid() << space;
	}

	for( dter = requests.begin();
			dter != requests.end();
			  dter++ )
			  {  
					
					ImapFetchStrings::iterator iter;
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
						throw OemoImapMgrError( "parse error in fetch" , lmlCritical ) ;				
					
					string ret_param( ucase( iter->second ) );

					switch( iter->first )
					{
						case oemo_imap_fetch_flags:

							if( bPeek == false )
							{
								if( flags.find( omfSeen ) == flags.end() ) flags.insert( omfSeen );
								msg.Flags( flags );
							}
							
							fetch_stream << ret_param << space;
							fetch_stream << left_paren << MbxFlagsToStr( msg.Flags() ) << right_paren
										 << space;

							break;

						case oemo_imap_fetch_internaldate:

							fetch_stream << ret_param << space;
							fetch_stream << quote << InternalDate( s ) << quote
										 << space;

							break;

						case oemo_imap_fetch_rfc822size:

							fetch_stream << ret_param << space;
							fetch_stream << msg.Size()
										 << space;

							break;

						case oemo_imap_fetch_envelope:

							fetch_stream << ret_param << space;
							fetch_stream << left_paren << FetchEnvelope( s ) << right_paren
										 << space;

							break;

						case oemo_imap_fetch_bodystructure:

							fetch_stream << ret_param << space;
							fetch_stream << left_paren << FetchBodyStructure( s ) << right_paren
										 << space;

							break;

						case oemo_imap_fetch_uid:
							
							if( Uids() == false )
							{
								fetch_stream << ret_param << space;
								fetch_stream << msg.Uid()
											 << space;

							}
							break;

						case oemo_imap_fetch_rfc822:
							{
								fetch_stream << ret_param << space;
								fetch_stream << left_brace << msg.Size() + two << right_brace
											 << crlf
											 << FetchRfc822( s , iter->second ) 
											 << space;	
										
							
							}
							break;
							
						case oemo_imap_fetch_rfc822text:
							{
								string strPayload = FetchRfc822( s , iter->second ) ;
								fetch_stream << ret_param << " ";
								fetch_stream << left_brace << strPayload.size() + two << right_brace
											 << crlf
											 << strPayload
											 << space;	

								flags = msg.Flags();
								flags.insert( omfSeen );									
							}
							
							break;	

						case oemo_imap_fetch_bodypeek:		
							{
								bPeek = true;						
							}
						case oemo_imap_fetch_body:						
							{
								
								string strTemp( lcase( dter->second ) );
								string strBody(  ucase( _R( oemo_imap_fetch_body ) ) );
								bool bPart = false;		
								ostringstream fostr;
																

								if( strTemp.compare( _R( oemo_imap_fetch_header ) ) == 0 )	
								{
									strPayload = FetchHeader( s , "" , false );									
								}
								else if( strTemp.find( _R( oemo_imap_fetch_headerfields ) ) != string::npos )
								{
									strPayload = FetchHeaderFields( s , dter->second );									
								
								}
								else if( strTemp.find( ".header" ) != string::npos )
								{
									

									string::size_type idx = dter->second.find( ".header" );
									if( idx != string::npos )
									{
										string strPart( dter->second.substr( 0 , idx ) );
										string strAfter( dter->second.substr( 0 , ++idx ) );
										if( strAfter.compare( _R( oemo_imap_fetch_header ) ) == 0 )	
										{
											strPayload = FetchHeader( s , strPart , true );

										}
										else if( strAfter.find( _R( oemo_imap_fetch_headerfields ) ) != string::npos )
										{
											strPayload = FetchHeaderFields( s , dter->second );
											
										}
									}
								}
								else 
								{	
									
									if( dter->second.empty() ) 
									{
										

										fetch_stream << strBody << space;
										fetch_stream << left_paren << FetchBodyStructure( s ) << right_paren
													 << space;

										break;
									}									
									

									if( dter->second.compare( "nil" ) == 0 )
									{
											strTemp = "";
									}	
									strPayload = FetchMime( s , strTemp );
									if( iter->first == oemo_imap_fetch_bodypeek )
									{											
											flags = msg.Flags();
											flags.insert( omfSeen );										
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

							throw OemoImapMgrError( "parse error in fetch" , lmlCritical );						
							

							break;
					}

					SleepEx( 5 , FALSE );
			  }
			  
			  string strTemp( fetch_stream.str() );			  
			  strTemp = rtrim( strTemp , " " );	
			  
			  string s1;
			 
			  
			  
			  s1 = FetchResponse( dostr.str() , strTemp , ostr );		 
			  
			  flags.erase( omfRecent );	
			  msg.Flags( flags );							
			 
			  if( uflags != msg.Flags() )
			  {
				SetMessageFlagsByDb( msg.FileUrl() , flags );
			  }			 

			  return ( s1 );

			  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FetchHeader( const string& message_path ,  const string& params , bool bIsPart )
{
	 string payload;
	 vtorString parts;
	 CkString ckstr;
		  

	 try
	 {
		   auto_ptr<CkMime> mime( new CkMime );	   
		   
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

				   auto_ptr<CkMime> mime( new CkMime );
				   CkMime* mimeIdx;
				   CkMime* mimeQdx;	

				   mimeIdx = mime.get();
				   for( int i = 0; i < parts.size(); i++ )
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
		   throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	   }

	   return ( payload );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FetchBodyStructure( const string& message_path )
{

	char* s = new char[1000];
	string payload;

	try
	{
		   auto_ptr<CkEmail>  email( new CkEmail );
		   bool bRet = email->UnlockComponent( (char*) constMailUnlockCode.c_str() );
		   if (!bRet ) 
		   {
			 return( "" );
		   }
		   
		   bRet = email->LoadXml( (char*) message_path.c_str() );
		   auto_ptr<CkString> strMime( new CkString );

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
		throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	}

	return ( payload );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::FetchEnvelope( const std::string& message_path )
{
	char* s = new char[1000];
	string payload;

	try
	{
		   auto_ptr<CkEmail>  email( new CkEmail );
				   
		   bool bRet = email->UnlockComponent( (char*) constMailUnlockCode.c_str() );
		   if (!bRet ) 
		   {
			 return ( "" );
		   }	

		   bRet = email->LoadXml( (char*) message_path.c_str() );
		   auto_ptr<CkString> strMime( new CkString );

		   email->GetMime( *strMime.get() );
			
		   proc_char_ptr_in_char_ptr_out_args_return_dword aProc = 
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
		   }
	}
	catch( ... )
	{
		if( s != NULL )
		{
			delete[] s;
			s = NULL;
		}
		throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	}

	return ( payload );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::FetchRfc822( const string& message_path , const string& command )
{
	   string payload;
	   CkString ckstr;

       try
	   {
		   auto_ptr<CkMime> mime( new CkMime );
		   bool bRet = mime->UnlockComponent( (char*) constMimeUnlockCode.c_str() );
		   if( bRet )
		   {
			   mime->LoadXmlFile( (char*) message_path.c_str() );
			   if( command.compare( _R( oemo_imap_fetch_rfc822 ) ) == 0 )
			   {
				   mime->GetMime( ckstr );
				   payload = ckstr.getString();
			   }
			   else if( command.compare( _R( oemo_imap_fetch_rfc822 ) + ".text" ) == 0 )
			   {
				   mime->GetEntireBody( ckstr );
				   payload = ckstr.getString();
			   }
		   }

	   }
	   catch( ... )
	   {
		   throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	   }

	   return ( payload );

}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::FetchMime( const std::string& message_path , const std::string& mime_part )
{
	 string payload;
	 vtorString parts;
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
				  for( int i = 0; i < parts.size(); i++ )
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
		   throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	   }

	   return ( payload );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FetchHeaderFields( const string& message_path , const string& params )
{
	ostringstream ostr;	
	CkString ckstr;
	ImapHeaderFields fields;
	ImapHeaderSet fields_set;

	 try
	 {     
		   
		   ParseHeaderFieldParams( params , fields_set );	   

		   
		   fields = FetchHeaderFieldsMime( message_path , params );		   

		   for( ImapHeaderFields::iterator iter = fields.begin();
					iter != fields.end();
						iter++ )
						{							
							string s( lcase( iter->first ) );
							ImapHeaderSet::iterator dter = fields_set.find( lcase( iter->first ) );
							if( dter != fields_set.end() )
							{
								ostr << iter->first << ": " <<  iter->second << crlf;
							}
						}						
	

	   }
	   catch( ... )
	   {
		   throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	   }

	   return ( ostr.str() );	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImapHeaderFields OemoImapMgr::FetchHeaderFieldsMime( const string& message_path , const string& params )
{

	 string payload;
	 vtorString parts;	 
	 string strHeader;	 
	 vtorString subparts;
	 ImapHeaderFields fields;
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
		   
		   auto_ptr<CkMime> mime( new CkMime );
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
					for(  int i = 0; i < parts.size(); i++ )
					{
						try
						{
							unsigned long dw = boost::lexical_cast<unsigned long>( parts[i] );
							subparts.push_back( parts[i] );
										   
						}
						catch( bad_lexical_cast & )
						{
						    continue;
						}
					}
				    
					mimeIdx = mime.get();                    

					for( int j = 0; j < parts.size(); j++ )
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
					for( int i = 0; i < dw; i++ )
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
					for( int i = 0; i < dw; i++ )
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
		   throw OemoImapMgrError( "parse error in fetch" , lmlCritical );
	   }

	   return ( fields );



}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::ParseHeaderFieldParams( const std::string& source , ImapHeaderSet& fields_set )
{
	vtorString v;
	string str ( ExtractParentheticalAtoms( source ) );
	safe_tokenize_t( v , (char*) str.c_str() , " " );
	
	for( vtorString::iterator iter = v.begin();
			iter != v.end();
				iter++ )
				{
					fields_set.insert( lcase( *iter ) );
				}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::InternalDate( const string& filename )
{
	HANDLE handle;
	WIN32_FIND_DATA data;
	ostringstream ostr; 
	SYSTEMTIME st;
	FILETIME ft;
	string str;
	string strFormat( "%.2d-%s-%.2d %d:%.2d:%.2d %s" );
	vtorString months;

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
		
	} 

	

	return ( ostr.str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::ParseFetchRequestList( const std::string& request , OemoImapFetchRequests& requests )
{

	char c;
	bool bParams = false;
	bool bNull = false;
	string s;
	string source( request );
	OemoImapFetchRequest req;	

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
					req.first = lcase( ExtractQuotedString( s ) );
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
				lcase( req.first = ExtractQuotedString( s ) );
				requests.push_back( req );				
			}	
			s = "";  
		}
		else if ( c == ']' )
		{		

			if( ( bParams == true ) && ( bNull == true ) && ( ! s.empty() ) )
			{				
				lcase( requests[requests.size() - 1].second = ExtractQuotedString( s ) );
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
void OemoImapMgr::SetMessageFlagsByDb( const string& url , OemoMailMessageFlags flags )
{
	 string decoded_url;
	 typedef map<OemoMailMessageFlag,string> flag_strings;
	 flag_strings m;		 
	 
	 m.insert( pair<OemoMailMessageFlag,string>( omfAnswered , "isAnswered" ) );
	 m.insert( pair<OemoMailMessageFlag,string>( omfDeleted , "isDeleted" ) );
	 m.insert( pair<OemoMailMessageFlag,string>( omfDraft , "isDraft" ) );
	 m.insert( pair<OemoMailMessageFlag,string>( omfFlagged , "isFlagged" ) );
	 m.insert( pair<OemoMailMessageFlag,string>( omfRecent , "isRecent" ) );
	 m.insert( pair<OemoMailMessageFlag,string>( omfSeen , "isSeen" ) );


	 try
     {
			  
              SACommand o( &m_db );              

              o.setCommandText( "call update_message_flags( :1 , :2, :3, :4 , :5 , :6 , :7)" );
              o << (char*) url.c_str(); 
			  for( flag_strings::iterator iter = m.begin();
						iter != m.end();
							iter++ )
							{
								OemoMailMessageFlags::iterator dter = flags.find( iter->first );
								dter != flags.end() ? o << 1L : o << 0L;
							}		  

              o.Execute();             

      }
      catch( SAException& e )
      {

              try
              {
                   m_db.Rollback();
				   cout << e.ErrText() << "\n";
              }
              catch( SAException & )
              {
              }
              throw OemoImapMgrError( "access denied" , lmlCritical );
      }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::TimeZoneInfo()
{
	string str;
	char* buffer = new char[short_call];

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
	}

	return ( str );

	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::_R( unsigned long dwIdx )
{
	char buffer[short_call];

	LoadString( StrTblHandle(), dwIdx , buffer , short_call );

	string str( buffer );

	return ( str );

}



//overrides
bool OemoImapMgr::OnLogin( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string user;
	string password;
	

	pair<string,string> pr = PairFromTuple( params );
	if( ( pr.first.empty() ) || ( pr.second.empty() ) )
	{
		return ( false );
	}

	user = lcase( ExtractQuotedString( pr.first ) );
	password = ExtractQuotedString( pr.second );

	User( user );

	try
	{
		SACommand o( &m_db );	
		string str;

		o.setCommandText( "call get_user_domain_name_ex( :1 )" );
		o << (char*) User().c_str();     

		o.Execute();

		while( o.FetchNext() )
		{
			str = o.Field( "domain_name" ).asString();
		}
		to_lower( str );
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

		UserDomain( str );
	}
	catch( ... )
	{	
	}

	return ( true );
}

bool OemoImapMgr::OnLogout( const string& params , const string& fmt , std::ostream& ostr )
{

	
	return ( true );
}

bool OemoImapMgr::OnList( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;

	try
	{
		OemoMailBoxList mbxs;

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
		
		pair<string,string> pr = PairFromTuple( params );	
		
		
		EnumMailboxList( mbxs ,
						 User() ,
						 ExtractQuotedString( pr.first ) ,
						 ExtractQuotedString( pr.second ) );

		

		for( OemoMailBoxList::iterator iter = mbxs.begin();
				iter != mbxs.end();
					iter++ )
					{
						string str( MbxAttributesToStr( (*iter).Attributes() ) );
					
						ostr << format( fmt ) % str % "/" % (*iter).Name();
					}


	}
	catch( OemoImapMgrError& oe )
	{			
	   cerr << oe.what() << "\n";
	   return ( false );
	}	
	
	return ( true );
}

bool OemoImapMgr::OnLsub( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	ImapStrings imp;

	try
	{
		OemoMailBoxList mbxs;

		safe_tokenize_t( imp , (char*) params.c_str() , " " );
		if( imp.size() != 2 )
		{
			return ( false );
		}

		
		pair<string,string> pr = PairFromTuple( params );		
		
		EnumMailboxList( mbxs ,
						 User() ,
						 ExtractQuotedString( pr.first ) ,
						 ExtractQuotedString( pr.second ) ,
						 true );

		

		for( OemoMailBoxList::iterator iter = mbxs.begin();
				iter != mbxs.end();
					iter++ )
					{
						string str( MbxAttributesToStr( (*iter).Attributes() ) );
					
						ostr << format( fmt ) % str % "/" % (*iter).Name();
					}


	}
	catch( OemoImapMgrError& oe )
	{			
	   cerr << oe.what() << "\n";
	   return ( false );
	}	
	
	return ( true );
}

bool OemoImapMgr::OnAuthenticate( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}

bool OemoImapMgr::OnSelect( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	bool isInbox = false;
	OemoMailboxResult result = omrNotFound;

	
	if( params.empty() )
		return ( false );

	mailbox = ExtractQuotedString( params );
	
	long dw = 0L;

	//inbox
    subatom = "/" + ucase( User() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
      isInbox = true;   
	}

	atom = CheckFolderName( mailbox );
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{    
		 //select always succeeds for inbox , even it does not exist
		if( isInbox == false )
		{
			throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
		}
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	OemoMailBoxInfo info;
	ostringstream dostr;

	UpdateMailBoxInfoByDb( atom , false );

	SelectResponse( MbxInfo() , dostr , false );

	ostr << format( fmt ) % dostr.str();
	

	return ( true );
}

bool OemoImapMgr::OnCreate( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	OemoMailboxResult result = omrNotFound;

			
	if( params.empty() )
		return ( false );

	mailbox = ExtractQuotedString( params );
	
	long dw = 0L;

	atom = CheckFolderName( mailbox );
    result = FolderExists( atom );
	if( result == omrSuccess ) 
	{
		 throw OemoImapMgrError( "mailbox already exists" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( User() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	try
	{
			 
			  SACommand o( &m_db );	
			  string str( ucase( User() ) );
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

bool OemoImapMgr::OnCapability( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	const string str( "IMAP4rev1" );

	ostr << format( fmt ) % str;
	
	return ( true );
}

bool OemoImapMgr::OnDelete( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	OemoMailboxResult result = omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = ExtractQuotedString( params );
	
	long dw = 0L;
	dAtom = "/" + ucase( User() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = CheckFolderName( mailbox );
	}
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( User() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         throw OemoImapMgrError( "can't delete inbox" , lmlCritical ) ;
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

				  subatom = "/" + ucase( User() ) + "/";

				  
				  while( o.FetchNext() )
				  {

					  dwSelectable = o.Field( "select_" ).asLong();
					  dwMarked = o.Field( "marked" ).asLong();
					  dwInferiors = o.Field( "public" ).asLong();
					  dwPublic = o.Field( "public" ).asLong();				 
                                        
  					  if( dwSelectable == 0 )
					  {
						throw OemoImapMgrError( "access denied" , lmlCritical ) ;
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

bool OemoImapMgr::OnRename( const std::string& params , const std::string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string atom;
	string oldmailbox;
	string newmailbox;
	string subatom;
	OemoMailboxResult result = omrNotFound;

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 2 )
	{
		return ( false );
	}
	oldmailbox = ExtractQuotedString( imp[0] );
	newmailbox = ExtractQuotedString( imp[1] );
	
	long dw = 0L;

	atom = CheckFolderName( oldmailbox );
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	//inbox
    subatom = "/" + ucase( User() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
         //todo move messages
	}

	try
	{
			 
			  SACommand o( &m_db );	
			  string str( ucase( User() ) );			  

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

bool OemoImapMgr::OnNoop( const std::string& params , const std::string& fmt , std::ostream& ostr )
{

	try
	{
		  SACommand o( &m_db );	

		
		  o.setCommandText( "call has_new_messages_by_folder_ex( :1 , :2 )" );
		  o << (char*) MbxInfo().Name().c_str();
		  o << (long) Messages()->size();

		  o.Execute();

		  long dwCount = 0L;
		  
		  while( o.FetchNext() )
		  {

			  dwCount = o.Field( "msg_count" ).asLong();			  
			  if( dwCount != 0 )
			  {
				  
				  UpdateMailBoxInfoByDb( MbxInfo().Name() );
				  ostr << "* " << Messages()->size() << " exists" << crlf
				       << "* " << MbxInfo().Recent() << " recent" << crlf;
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

bool OemoImapMgr::OnSubscribe( const std::string& params , const std::string& fmt , std::ostream& ostr )
{

	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	OemoMailboxResult result = omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = ExtractQuotedString( params );
	
	long dw = 0L;
	dAtom = "/" + ucase( User() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = CheckFolderName( mailbox );
	}
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call insert_subscription( :1 , :2 )" );
			  o << (char*) atom.c_str();
			  o << (char*) User().c_str();

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

bool OemoImapMgr::OnUnsubscribe( const string& params , const string& fmt , std::ostream& ostr )
{
	
	string atom;
	string mailbox;
	string subatom;
	string dAtom;
	OemoMailboxResult result = omrNotFound;
	bool bRoot = false;

			
	if( params.empty() )
		return ( false );

	mailbox = ExtractQuotedString( params );
	
	long dw = 0L;
	dAtom = "/" + ucase( User() ) + "/";
    if( mailbox.compare( dAtom ) == 0 )
	{
		atom = mailbox;
		bRoot = true;
	}
	else
	{
		atom = CheckFolderName( mailbox );
	}
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call delete_subscription( :1 , :2 )" );
			  o << (char*) atom.c_str();
			  o << (char*) User().c_str();

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

bool OemoImapMgr::OnExamine( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string atom;
	string mailbox( params );
	string subatom;
	bool isInbox = false;
	OemoMailboxResult result = omrNotFound;

			
	if( mailbox.empty() )
		return ( false );	
	
	long dw = 0L;

	//inbox
    subatom = "/" + ucase( User() ) + "/" + "INBOX";
    if ( subatom.compare( atom ) == 0 )
	{
      isInbox = true;   
	}

	atom = CheckFolderName( mailbox );
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{    
		 //select always succeeds for inbox , even it does not exist
		if( isInbox == false )
		{
			throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
		}
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	OemoMailBoxInfo info;
	ostringstream dostr;

	UpdateMailBoxInfoByDb( atom , true );

	SelectResponse( MbxInfo() , dostr , true );

	ostr << format( fmt ) % dostr.str();

	return ( true );
}

bool OemoImapMgr::OnCheck( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}

bool OemoImapMgr::OnSearch( const string& params , const string& fmt , std::ostream& ostr )
{

	ACE_Trace _( ACE_TEXT( "OemoImapMgr::OnSearch" ) , __LINE__ );	

	ImapStrings imp;
	string s( params );
	string atoms;
	string message_set;
	vInt v;
	OemoImapMessageManip::vInt ret_vints;
	
	
	unsigned long dwMin = 0L;
	unsigned long dwMax = 0L;
	
    
	Uids() == false ? dwMax = Messages()->size() : dwMax = MaxUid();
	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		message_set = ExtractQuotedString( message_set );
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
			throw OemoImapMgrError( "search parse error" , lmlCritical ) ;
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
				ret_vints = oisr->UidsToMessageNumbers( oisr->SearchResults() );
			}
			else
			{
				ret_vints = oisr->SearchResults();
			}

			ostr << SearchResponse( ret_vints );

			
		}
		catch( OemoImapSearchError& e )
		{
			throw OemoImapMgrError( e.what() , lmlCritical );
		}
		
				
	}
	else
	{
		throw OemoImapMgrError( "search parse error" , lmlCritical ) ;
	}
	
	return ( true );
}

bool OemoImapMgr::OnCopy( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string atom;	
	string message_set;
	OemoMailboxResult result = omrNotFound;

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 2 )
	{
		return ( false );
	}
	message_set = ExtractQuotedString( imp[0] );
	atom = ExtractQuotedString( imp[1] );
	
	long dw = 0L;

	atom = CheckFolderName( atom );
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}	

	vInt v ( ExpandMessageSequence( message_set , Messages()->size() ) );
	vInt vMessageNums;

	for( ImapServerMessages::iterator iter = Messages()->begin();
			iter != Messages()->end();
				iter++ )
				{
					if( Uids() == true )
					{
						vMessageNums.push_back( iter->second.Uid() );
					}
					else
					{
						vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) );
					}

				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	vInt vRequestSet = AtomsIntersection( v , vMessageNums );

	ostringstream dostr;
	string s;
	
	for( vInt::iterator dter = vRequestSet.begin();
		dter != vRequestSet.end();
			dter++ )
			{
				if( Uids() == false )
				{	
					ImapServerMessages::iterator siter = Messages()->find( boost::lexical_cast<string>( *dter ) );
					if( siter != Messages()->end() )
					{
						dostr <<  "'"  << siter->second.Uid() << "'" << ",";
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
			  string str( ucase( User() ) );			  

			  o.setCommandText( "call copy_messages_from_subscribe_folder( :1 , :2 , :3 , :4 )" );
			  o << (char*) MbxInfo().Name().c_str();
			  o << (char*) User().c_str();
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

bool OemoImapMgr::OnFetch( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string s( params );
	string atoms;
	string message_set;
	vInt v;
	unsigned long dw = 0L;
			
	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		atoms = s.substr( ++idx );
		//atoms = ltrim( atoms , " " );
	}

	
	message_set = ExtractQuotedString( message_set );
	if( atoms.find( right_bracket ) == string::npos )
	{
		atoms = ExtractParentheticalAtoms( atoms  );
	}
	
		
	atoms = ExtractQuotedString( atoms );

	if ( ( message_set.empty() ) || ( atoms.empty() ) )
	{
		throw OemoImapMgrError( "fetch parse error" , lmlCritical ) ;
	}

	UpdateMailBoxInfoByDb( MbxInfo().Name() );
	
	unsigned long dwMin = 0L;
	unsigned long dwMax = 0L;
	
    
	Uids() == false ? dwMax = Messages()->size() : dwMax = MaxUid();
	v = ExpandMessageSequence( message_set , dwMax );
	vInt vMessageNums;

	for( ImapServerMessages::iterator iter = Messages()->begin();
			iter != Messages()->end();
				iter++ )
				{
					if( Uids() == true )
					{
						vMessageNums.push_back( iter->second.Uid() );
					}
					else
					{
						vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) );
					}

				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	vInt vRequestSet = AtomsIntersection( v , vMessageNums );	

	OemoImapFetchRequests requests;
	ParseFetchRequestList( atoms , requests );

	string str;
	for( vInt::iterator dter = vRequestSet.begin();
			dter != vRequestSet.end();
				dter++ )
				{
					
					ostr << FetchMessage( *dter , ostr , requests );					
				}											
				
								

	return ( true );
}

bool OemoImapMgr::OnStatus( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string atoms( lcase( params ) );
	string folder;
	string mailbox;
	string subatom;
	ostringstream dostr;
	OemoMailboxResult result = omrNotFound;
	unsigned short one = 1;

	string::size_type idx = atoms.find_first_of( " " );
	if( idx != string::npos )
	{
		subatom = atoms.substr( 0 , idx );
		mailbox = ExtractQuotedString( subatom );
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
		if( idx != string::npos )
		{
			atoms.erase( idx , one );
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

	atoms = rtrim( atoms , " " );
	atoms = ltrim( atoms , " " );
	safe_tokenize_t( imp , (char*) atoms.c_str() , " " );
	if( imp.empty() ) return ( false );
	
	long dw = 0L;

	folder = CheckFolderName( mailbox );
    result = FolderExists( folder );
	if( result == omrNotFound ) 
	{
		 throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;
	}

	OemoMailBoxInfo info;
	UpdateMailBoxInfoByDb( folder , info );

	StatusResponse( info , dostr , imp );

	ostr << format( fmt ) % mailbox % dostr.str();

	return ( true );
}

bool OemoImapMgr::OnStore( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string s( lcase( params ) );
	string atoms;
	string message_set;
	unsigned long dw = 0L;
	OemoMailMessageFlags omf;
	vInt v;
	OemoFlagsMethod fm;
	bool bSilent = false;
	omf.clear();

	if( s.empty() )
		throw OemoImapMgrError( "store parse error" , lmlCritical );

	if( MbxInfo().ReadOnly() == true )
	{
		throw OemoImapMgrError( "Mailbox is read-only" , lmlCritical );
	}

	if( s.find( "silent"  ) != string::npos )
		bSilent = true;

	string::size_type idx = s.find_first_of( " " );
	if( idx != string::npos )
	{
		message_set = s.substr( 0 , idx );		
		atoms = s.substr( ++idx );		
	}

	
	message_set = ExtractQuotedString( message_set );
	atoms = ExtractParentheticalAtoms( atoms  );
	atoms = ExtractQuotedString( atoms );
	atoms = lcase( atoms );	

	StrToMbxFlags( atoms , omf );	
	if( s.find( "+flags" ) != string::npos)
	{
		fm = ofmAdd;
	}
	else if( s.find( "-flags" ) != string::npos )
	{
		fm = ofmRemove;
	}
	else
	{
		fm = ofmReplace;
	}	
	
	//message set bounds are known , no reason to iterate through the universe
	/*if( ( message_set.find( ":" ) != string::npos ) &&
		( Uids() == true ) )
	{
		string::size_type qdx = message_set.find( ":" );
		string strMin( message_set.substr( 0 , qdx ) );
		string strMax( message_set.substr( qdx ) );
		ostringstream fostr;

		unsigned long dwMin = boost::lexical_cast<unsigned long>( strMin );
		unsigned long dwMax = 0L;
		dwMin > MinUid() ? fostr << dwMin : fostr << MinUid();
		fostr << ":";
		if( strMax.compare( "*" ) != 0 )
		{
			dwMax = boost::lexical_cast<unsigned long>( strMax );
			dwMax < MaxUid() ? fostr << dwMax : fostr << MaxUid();
		}
		else
		{
			fostr << MaxUid();
		}
	
		message_set = fostr.str();
	}*/

	unsigned long dwMax = 0L;
	Uids() == false ? dwMax = Messages()->size() : dwMax = MaxUid();

	v = ExpandMessageSequence( message_set , dwMax );
	vInt vMessageNums;

	for( ImapServerMessages::iterator iter = Messages()->begin();
			iter != Messages()->end();
				iter++ )
				{
					if( Uids() == true )
					{
						vMessageNums.push_back( iter->second.Uid() );
					}
					else
					{
						vMessageNums.push_back( atoi( (char*) iter->first.c_str() ) );
					}

				}

	sort( vMessageNums.begin() , vMessageNums.end() );
	vInt vRequestSet = AtomsIntersection( v , vMessageNums );	

	for( vInt::iterator dter = vRequestSet.begin();
			dter != vRequestSet.end();
				dter++ )
				{
					StoreFlags( *dter , omf , fm , ostr , bSilent );
				}			
	

	return ( true );
}

bool OemoImapMgr::OnUid( const string& params , const string& fmt , std::ostream& ostr )
{
	
	bool bRet = false;

	return ( true );
}

bool OemoImapMgr::OnClose( const string& params , const string& fmt , std::ostream& ostr )	
{
	CloseExpunge( true );

	return ( true );
}

bool OemoImapMgr::OnExpunge( const string& params , const string& fmt , std::ostream& ostr )
{	
	
	try
	{
		      SACommand o( &m_db );	

			  o.setCommandText( "call expunge_messages_by_idx( :1 )" );
			  o << (char*) MbxInfo().Name().c_str();
			 

              o.Execute();   
			  

			  vInt v;
			  for( ImapServerMessages::iterator iter = Messages()->begin();
						iter != Messages()->end();
							iter++ )
							{
								if( iter->second.Flags().find( omfDeleted ) != iter->second.Flags().end() )
								{
									ostringstream dostr;
									dostr << ProxyStore()
										  << "\\"
										  << iter->second.Offset() 
										  << "\\"
										  << iter->second.Url()
										  << ".xml";
									if( ! DeleteFile( (char*)  dostr.str().c_str() ) )
									{
										throw OemoImapMgrError( "error in expunge message" , lmlCritical ) ;
									}
									v.push_back( boost::lexical_cast<int>( iter->first ) );
								}
							}
			  sort( v.begin() , v.end() );
			  unsigned long dw = 0L;

			  if( CloseExpunge() == false )
			  {
				  for( vInt::iterator dter = v.begin();
							dter != v.end();
								dter++ )
								{
									ostr << "* " << (*dter) - dw << " expunged" << crlf;
									++dw;
								}


				  UpdateMailBoxInfoByDb( MbxInfo().Name() );
			  }
			  else
			  {
				  Messages()->clear();
				  MbxInfo().clear();
				  CloseExpunge( false );
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

bool OemoImapMgr::OnAppend( const string& params , const string& fmt , std::ostream& ostr )
{
	ImapStrings imp;
	string atom;	
	string strFlags;
	unsigned short zero = 0;
	unsigned short one = 1;
	unsigned short two = 2;
	string subatom;
	string temp;
	bool isInbox = false;
	OemoMailboxResult result = omrNotFound;
	OemoMailMessageFlags  flags;

	AppendRec().clear();

			
	safe_tokenize_t( imp , (char*) params.c_str() , " " );
	if( imp.size() != 3 )
	{
		return ( false );
	}
     
	temp = ExtractQuotedString( imp[zero] );
	AppendRec().Mbx( temp );
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
	StrToMbxFlags( lcase( strFlags ) , flags );
	AppendRec().Flags( flags );

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

		AppendRec().Size( atoi( (char*) subatom.c_str() ) );
	}
	else
	{
		return ( false );
	}


     
	atom = CheckFolderName( AppendRec().Mbx() );
    result = FolderExists( atom );
	if( result == omrNotFound ) 
	{    		
		throw OemoImapMgrError( "mailbox does not exist" , lmlCritical ) ;
	}
	else if( result == omrAccessDenied )
	{
		 throw OemoImapMgrError( "access denied" , lmlCritical ) ;		 
	}	

	return ( true );
}

bool OemoImapMgr::OnStarttls( const string& params , const string& fmt , std::ostream& ostr )
{
	return ( true );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::lcase( const string& str )
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
std::string OemoImapMgr::ExtractQuotedString( const std::string& str )
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
string OemoImapMgr::ucase( const string& str )
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
void OemoImapMgr::StoreResponse( const std::string& msg_id , const std::string& payload , std::ostream& ostr )
{
		ostr << "* " 
			 << msg_id
			 << " FETCH ("
			 << payload
			 << ")"
			 << crlf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::StoreFlags( unsigned long msg_id , OemoMailMessageFlags flags ,
							  OemoFlagsMethod  method , ostream& ostr , bool bSilent )
{
	string req_str;	
	ostringstream dostr;
	ostringstream fetch_stream;
	OemoImapFetchRequest req;
	OemoImapMessage msg;
	string s;
	OemoMailMessageFlags local_flags;	

	if( Uids() == false )
	{
		dostr << msg_id;
		ImapServerMessages::iterator siter = Messages()->find( dostr.str() );
		if( siter != Messages()->end() )
		{
			msg = siter->second;
			local_flags = msg.Flags();
		}
		
	}
	else
	{
		for( ImapServerMessages::iterator siter = Messages()->begin();
				siter != Messages()->end();
					siter++ )
					{
						if( siter->second.Uid() == msg_id )
						{
							msg = siter->second;
							local_flags = msg.Flags();
							dostr.str( "" );
							dostr << siter->first;							

							break;
						}						
					}
	}
	
	if( method == ofmReplace )
	{
		local_flags = flags;
	}
	else 
	{
		for( OemoMailMessageFlags::iterator iter = flags.begin();
				iter != flags.end();
					iter++ )
					{
						if( method == ofmAdd )local_flags.insert( *iter );
							else if( method == ofmRemove) local_flags.erase( *iter );
					}
	}

	SetMessageFlagsByDb( msg.Url() , local_flags );
	msg.Flags( local_flags );

	string strTemp( MbxFlagsToStr( local_flags ) );		

	if( bSilent == false ) StoreResponse( dostr.str() , strTemp , ostr );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::StrToMbxFlags( const std::string& flag_string , OemoMailMessageFlags&  ommf )
{
	 
	 if( flag_string.find( "\\Answered" ) != string::npos ) ommf.insert( omfAnswered );
	 if( flag_string.find( "\\Flagged" )  != string::npos ) ommf.insert( omfFlagged );
	 if( flag_string.find( "\\Deleted" ) != string::npos ) ommf.insert( omfDeleted);
	 if( flag_string.find( "\\Seen" ) != string::npos ) ommf.insert( omfSeen );
	 if( flag_string.find( "\\Draft" ) != string::npos ) ommf.insert( omfDraft );
	 if( flag_string.find( "\\Recent" ) != string::npos ) ommf.insert( omfRecent );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::MbxAttributesToStr( OemoMailBoxAttributes omba )
{
	ostringstream ostr;
	string str;


	for( OemoMailBoxAttributes::iterator iter = omba.begin();
			iter != omba.end();
				iter++ )
				{
					if ( (*iter) == omaNoinferiors ) ostr << "\\Noinferiors ";
					else if( (*iter) == omaNoselect ) ostr << "\\Noselect ";
					else if( (*iter) == omaMarked ) ostr << "\\Marked ";
					else if( (*iter) == omaUnmarked ) ostr << "\\Unmarked ";
				}

	str = ostr.str();
	string::size_type idx = str.find_last_of( " " );
	if( idx != string::npos ) str.erase( idx );

	return ( str );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoImapMgr::MbxFlagsToStr( OemoMailMessageFlags  ommf )
{
	ostringstream ostr;
	string str;


	for( OemoMailMessageFlags::iterator iter = ommf.begin();
			iter != ommf.end();
				iter++ )
				{
					if ( (*iter) == omfAnswered  ) ostr << "\\Answered ";
					else if( (*iter) == omfFlagged ) ostr << "\\Flagged ";
					else if( (*iter) == omfDeleted ) ostr << "\\Deleted ";
					else if( (*iter) == omfSeen ) ostr << "\\Seen ";
					else if( (*iter) == omfDraft ) ostr << "\\Draft ";
					else if( (*iter) == omfRecent ) ostr << "\\Recent ";
				}

	str = ostr.str();
	string::size_type idx = str.find_last_of( " " );
	if( idx != string::npos ) str.erase( idx );

	return ( str );

}

//////////////////////////////////////////////////
//pairfromtuple
pair<string,string> OemoImapMgr::PairFromTuple( const std::string& tuple )
{
	pair<string,string> pr;
	vtorString vStr;

	safe_tokenize_t( vStr , (char*) tuple.c_str() , " " );
	if( vStr.size() == 2 )
	{
		pr.first = vStr[0];
		pr.second = vStr[1];
	}

	return ( pr );
}


bool OemoImapMgr::ReadParams()
{
	   ostringstream ostr;	   

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
		  
		}  

		return ( true );
	   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT OemoImapMgr::GetDomainName( string& strDomainName ) 
{ 
   HRESULT hr = S_OK; 
   IADsADSystemInfo *pADsys; 
   OLECHAR* bstrDomainName = SysAllocStringLen( NULL , 256 );

   CoInitialize( NULL );
   
   try
   {
	   hr = CoCreateInstance(   CLSID_ADSystemInfo, 
						 NULL, 
						 CLSCTX_INPROC_SERVER, 
						 IID_IADsADSystemInfo, 
						 (void**)&pADsys); 

	   hr = pADsys->get_DomainDNSName( &bstrDomainName ); 

	   if( SUCCEEDED( hr ) )
	   {
		  strDomainName = (char*) (_bstr_t) bstrDomainName;
	   }

	   if (pADsys) 
		  pADsys->Release(); 
   }
   catch( ... )
   {
	   if( bstrDomainName != NULL )
	   {
			SysFreeString( bstrDomainName );
	   }
	   return ( -1 );
   }
   
   if( bstrDomainName != NULL )
   {
		SysFreeString( bstrDomainName );
   }

   CoUninitialize();

   return hr; 
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::SelectResponse( OemoMailBoxInfo info , std::ostream& ostr , bool bReadOnly )
{
	ostringstream dostr;

	dostr << "* " << info.Exists() << " EXISTS" << crlf
	      << "* " << info.Recent() << " RECENT" << crlf
	      << "* FLAGS (" << MbxFlagsToStr( info.ChangeableFlags() ) << ")" << crlf
	      << "* OK [UNSEEN " << info.Unseen() << "]" << crlf //info.FirstUnseen() << " is first unseen"  << crlf		  
	      << "* OK [UIDVALIDITY " << info.UidValidity() << "] UIDs valid" << crlf
		  << "* OK [PERMANENTFLAGS (" << MbxFlagsToStr( info.Flags() ) << ")]" << crlf;


	ostr << dostr.str();


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoImapMgr::StatusResponse( OemoMailBoxInfo info , std::ostream& ostr ,  ImapStrings vStr )
{

    ostringstream payload;
	string strPayload;

	for( ImapStrings::iterator iter = vStr.begin();
			iter != vStr.end();
			  iter++ )
			  {
					string str( lcase( *iter ) );

					if( str.compare( "messages" ) == 0 )
					{
						 payload << "messages " << info.Exists() << " ";
					}
					else if( str.compare( "uidnext" ) == 0 )
					{
						 payload << "uidnext " << info.UidNext() << " ";
					}
					else if( str.compare( "recent" ) == 0 )
					{
						 payload << "recent " << info.Recent() << " ";
					}
					else if( str.compare( "recent" ) == 0 )
					{
						 payload << "recent " << info.Recent() << " ";
					}
					else if( str.compare( "uidvalidity" ) == 0 )
					{
						 payload << "uidvalidity " << info.UidValidity() << " ";
					}
					else if( str.compare( "unseen" ) == 0 )
					{
						 payload << "unseen "  << info.Unseen() << " ";
					}			

			  }
			  strPayload = ucase( payload.str() );
			  strPayload = rtrim( strPayload  , " " );

			  ostr << strPayload;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FetchResponse( const string& msg_id , const string& payload , ostream& ostr )
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
string OemoImapMgr::SearchResponse( const OemoImapProtocolImpl::vInt& vints )
{
	ostringstream dostr;
	dostr << OemoImapMgr::asterisk << " SEARCH ";

	for( OemoImapMessageManip::vInt::const_iterator iter = vints.begin();
												iter != vints.end();
													iter++ )
	{
		dostr << *iter;		
		dostr << OemoImapMgr::space;
	}
	string str( rtrim( dostr.str() , OemoImapMgr::space ) );
	str += crlf;	

	return ( str );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::HttpToFileUrl( const string& str )
{

	if( str.empty() )
	{
		return( str );
	}

	string strPayload;
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
	ostr << strBackOffice << "/" << strDomain << "/" << strMbx << "/" << strPayload;

	return ( ostr.str() );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::MessageTag( const std::string& fileurlstr  )
{
	 CoInitialize( NULL );	
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

	 string str( FileToHttpUrl( fileurlstr , lcase( strServer ) ) );	
	 
	 string strPerm = str;
	 
	 return ( strPerm );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoImapMgr::FileToHttpUrl( const std::string& fileurl , const std::string& server )
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
		idx = idx = fileurl.find( folderpublic );
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
string OemoImapMgr::rtrim( const string &szToTrim, const string& szTrimChars )
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
string OemoImapMgr::ltrim( const string &szToTrim, const string& szTrimChars )
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
string OemoImapMgr::base64_encode( unsigned char const* bytes_to_encode , unsigned int in_len )
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


string OemoImapMgr::base64_decode( string const& encoded_string )
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

