//Fcn , Inc. William K. Johnson 1 Nov 2008

#pragma warning( disable: 4786 4800 4365 )

#include "ace/Trace.h"

#ifndef OEMO_IMAP_HANDLER_H
#include "OemoImapImpl.h"
#endif




using namespace OemoImapProtocolImpl;
using namespace OemoDbInterface;
using namespace OemoImapMessageManip;


using std::string;
using std::ends;
using std::ostringstream;
using std::wostringstream;
using std::wstring;
using std::make_pair;
using std::cout;
using std::endl;
using std::pair;
using std::set;
using std::ofstream;
using std::auto_ptr;
using boost::algorithm::to_lower;
using std::map;

//immutable
const string OemoImapSearchCriteria::constMessageSetAll = "1:*";
const string OemoImapIsysSearchResolver::const_strKey = "DTZW2 UBMFR 6B5CH 6DWLC J57P9 NRXLA";


//---------------------------------------------------------------------------------------------------------
//ctor
OemoImapSearchCriteria::OemoImapSearchCriteria( const std::string& criteria ,											    
											    const std::string& message_set ,
												bool bUid ,												
												bool bPropagateExceptions ) :  m_messageSet( message_set ) ,																			   
																			   m_criteria( criteria ) ,
																			   m_bUid( bUid ) ,																			   
																			   m_bPropagateExceptions( bPropagateExceptions )
{
	ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::OemoImapSearchCriteria" ) , __LINE__ );	

}
//---------------------------------------------------------------------------------------------------------
//dtor
OemoImapSearchCriteria::~OemoImapSearchCriteria()
{

	ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::~OemoImapSearchCriteria" ) , __LINE__ );	

}
//----------------------------------------------------------------------------------------------------------
string OemoImapSearchCriteria::ExtractQuotedString( const string& str )
{
	   ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::ExtractQuotedString" ) , __LINE__ );	

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
//----------------------------------------------------------------------------------------------------------
OemoSearches OemoImapSearchCriteria::ExtractParentheticalKeys( const std::string& str )
{

	ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::ExtractParentheticalKeys" ) , __LINE__ );	


	
	OemoSearches searches;

	return ( searches );

}

//----------------------------------------------------------------------------------------------------------
string OemoImapSearchCriteria::SearchKeyToStr( const OemoSearchKeys key )
{
	ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::SearchKeyToStr" ) , __LINE__ );	

	string str( "nil" );	
	
	//supported keys
	if( key ==  oskAll ) str = "all";
	else if( key == oskText ) str = "text";

	return ( str );

}

//----------------------------------------------------------------------------------------------------------
OemoSearchKeys OemoImapSearchCriteria::StrToSearchKey( const std::string& str )
{
	ACE_Trace _( ACE_TEXT( "OemoImapSearchCriteria::StrToSearchKey" ) , __LINE__ );	


	OemoSearchKeys key( oskUnsupportedKey );

	string strKey( str );

	to_lower( strKey );


	//supported keys
	if( strKey.compare( "all" ) == 0 ) key = oskAll;
	else if ( strKey.compare( "text" ) == 0 ) key = oskText;	

	return ( key );

}
//----------------------------------------------------------------------------------------------------------
void OemoImapSearchCriteria::DeconstructCriteria( OemoImapIsysSearchResolverPtr resolver )
{
	string source(  RawCriteria() );
	to_lower( source );	
	SearchKeys keys;
	SearchKeys found_keys;	
	
	//supported keys
	for( int i = (int) oskAll; i < (int) oskUnsupportedKey; i++ )
	{
		keys.insert( (OemoSearchKeys) i );
	}

	try
	{
		//no nested parenthetical keys , straightforward atomic logic search
		if( source.find( OemoImapMgr::right_paren ) == string::npos )
		{
			string::size_type idx = source.find_first_of( OemoImapMgr::space );
			if( idx != string::npos )
			{
				string str( source.substr( 0 , idx ) );
				//key?
				for( SearchKeys::iterator iter = keys.begin();
							iter != keys.end();
								iter++ )
				{
					if( SearchKeyToStr( *iter ).compare( str ) == 0 )
					{						
						found_keys.insert( *iter );						
					}
				}
			
				//not a key , message set?
				unsigned long dwMax( 0L );
				Uid() == false ? dwMax = resolver->ImapMgr()->Messages()->size() : dwMax = resolver->ImapMgr()->MaxUid();
				if( found_keys.empty() == true )
				{					

					bool bRet = IsMessageSet( str , dwMax );
					//not a key  , not a message set - throw
					if( bRet == false )
					{
						OemoImapSearchError e( "search key not recognized"  , islCritical );

						throw( e );
					}	
					else
					{						
						//top level
						MessageSet( str );
					}
				}
				else
				{
					if( found_keys.find( oskAll ) != found_keys.end() )
					{
						MessageSet( constMessageSetAll );
						//remove all key
						string strAll( "all" );
						string::size_type idx = source.find( strAll );
						if( idx != string::npos )
						{
							source.erase( idx , strAll.size() );
						}
					}
					else if( MessageSet().compare( "nil" ) == 0 )
					{
						MessageSet( constMessageSetAll );
					}
					else if( ( found_keys.find( oskAll ) != found_keys.end() ) && (  MessageSet().compare( "nil" ) == 0 ) )
					{
						//syntax error
						OemoImapSearchError e( "syntax error in search"  , islCritical );

						throw( e );
					}

				}

				//expand message sequence
				OemoImapMessageManip::vInt ints( ExpandMessageSequence( MessageSet() , dwMax ) );
				//parenthetical key
				OemoParentheticalSearchKey opsk( pair<OemoLogicOperations , string>( olpAnd , source ) );

				//resolve
				resolver->ResolveParentheticalKey( opsk , ints );			

			}
		}
		else
		{
			//nested parenthetical keys
		}
		
	}
	catch( OemoImapSearchError osie )
	{
		if( m_bPropagateExceptions == true )
		{
			OemoImapSearchError e( osie.what() , osie.ErrorLevel() );

			throw( e );
		}
	}	
}

//OemoSearchResolver
//---------------------------------------------------------------------------------------------------------
OemoImapIsysSearchResolver::OemoImapIsysSearchResolver( OemoImapMgrPtr ptrImapMgr ,
														const std::string& isys_path ,
														OemoImapSearchCriteriaPtr criteria ,
														const std::string& document_path ,														
														const std::string& search_domain ) :	m_ptrImap( ptrImapMgr ) ,
																								m_strIsysPath( isys_path ) ,
																								m_strDocumentPath( document_path ) ,
																								m_ptrEngine( NULL ) ,
																								m_bInitialized( false ) ,
																								m_strSearchDomain( search_domain ) ,
																								m_ptrCriteria( criteria )
																							
{

	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::OemoImapIsysSearchResolver" ) , __LINE__ );	

#ifdef WIN32

	CoInitialize( NULL );	


	try
	{
		HRESULT hr = CoCreateInstance( CLSID_ISYSEngine , 
									   NULL , 
									   CLSCTX_INPROC_SERVER, 
									   IID_IISYSEngine , 
									   (void**)&m_ptrEngine ); 

		if( hr == S_OK )
		{
			hr = m_ptrEngine->InitializeEngine( (char*) IsysPath().c_str() , (char*) const_strKey.c_str() , _bstr_t() );
			if( FAILED( hr ) )
			{
				ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Could not initialize Isys Engine...\n")));

				OemoImapSearchError e( "could not initialize search"  , islCritical );

				throw( e );
				
			}
			m_bInitialized = true;
		}
		
	}
	catch( _com_error& e )
	{
		dump_com_error( e );	
		
	}
	catch( IISYSError& e )
	{
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Isys error->%s...\n") , (char*) e.Text ));		
	}
	catch( ... )
	{	
	    ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Unknown exception.....\n" )));					
	}
	
	CoUninitialize();	

#endif

}
//---------------------------------------------------------------------------------------------------------
OemoImapIsysSearchResolver::~OemoImapIsysSearchResolver()
{

#ifdef WIN32

	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::~OemoImapIsysSearchResolver" ) , __LINE__ );		

#endif	

}
//---------------------------------------------------------------------------------------------------------
OemoImapMessageManip::vInt OemoImapIsysSearchResolver::UidsToMessageNumbers( const OemoImapMessageManip::vInt vints )
{
	OemoImapMessageManip::vInt ret_vints;
	typedef map<unsigned long , unsigned long> uid_to_message;
	uid_to_message UtoM;

	for( ImapServerMessages::iterator dter = ImapMgr()->Messages()->begin();
											dter != ImapMgr()->Messages()->end();
												dter++ )
	{
		
		pair<unsigned long , unsigned long> pr( dter->second.Uid() , 
											    boost::lexical_cast<unsigned long>( dter->first ) );
	    UtoM.insert( pr );
	}
	
	for( OemoImapMessageManip::vInt::const_iterator iter = vints.begin();
												iter != vints.end();
													iter++ )
	{
		if( UtoM.find( *iter ) != UtoM.end() )
		{
			ret_vints.push_back( UtoM[*iter] );			
		}
	}

	return( ret_vints );
}
//---------------------------------------------------------------------------------------------------------
OemoImapMessageManip::vInt OemoImapIsysSearchResolver::MessageNumbersToUids( const OemoImapMessageManip::vInt vints )																			 
{
	OemoImapMessageManip::vInt ret_vints;
	
	for( OemoImapMessageManip::vInt::const_iterator iter = vints.begin();
												iter != vints.end();
													iter++ )
	{
		string str( boost::lexical_cast<string>( *iter ) );
		if( ImapMgr()->Messages()->find( str ) != ImapMgr()->Messages()->end() )
		{
			ret_vints.push_back( (*ImapMgr()->Messages() )[str].Uid() );
		}
	}

	return( ret_vints );
}
//---------------------------------------------------------------------------------------------------------
void OemoImapIsysSearchResolver::ResolveParentheticalKey( OemoParentheticalSearchKey key , OemoImapMessageManip::vInt vints )
{

	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::ResolveParentheticalKey" ) , __LINE__ );	

	if( m_bInitialized == false )
	{
		OemoImapSearchError e( "search engine not initialized"  , islCritical );

		throw( e );
	}

	CoInitialize( NULL );

	OemoImapMessageManip::vInt search_result_set;
	OemoImapMessageManip::vInt message_set( vints );

	try
	{

		ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): resolving parenthetical search key....\n" ) ) );
		
		IISYSSearchPtr isys_search;
		IISYSResultListPtr isys_results;
		IISYSDocumentPtr isys_document;
		IISYSFilterPtr isys_filter;
		const string strFlat( "-FlatUrlSpace-" ); 

		//url to uid map
		UrlToUid().clear();
		
		//fragment of url for file filter workaround
		string strFlatEncode( ImapMgr()->base64_encode( (unsigned char*) strFlat.c_str() , strFlat.size() ) );	

		//populate map
		for( ImapServerMessages::iterator iter = ImapMgr()->Messages()->begin();
										   iter != ImapMgr()->Messages()->end();
												iter++ )
		{
			string str( iter->second.Url() );
			str += ".xml";

			pair<string,unsigned long> pr( str , iter->second.Uid() );
			UrlToUid().insert( pr );
		}

		//isys has a bug that cannot resolve max path
		char buffer[MAX_PATH];
		::GetShortPathName( (char*) DocumentPath().c_str() , buffer , MAX_PATH );

		//search object
		isys_search = Isys()->NewSearch();
		isys_search->put_Engine( Isys() );

		//cache file filter
		ostringstream ostr;
		string str;
		ostr << "http://"  ;
		if( SearchDomain().compare( "nil" ) == 0 )
		{
			
			ImapMgr()->GetDomainName( str );
			to_lower( str );
			ostr << str;
		}
		else
		{
			str = SearchDomain();
			to_lower( str );
			ostr << str << ".com";
		}
		ostr << "/exchange/" << ImapMgr()->User() << "/" << strFlat;		
		
		string strFilter( ImapMgr()->base64_encode( (unsigned  char*) ostr.str().c_str() , ostr.str().size() ) );
		strFilter = strFilter.substr( 0 , strFilter.size() - strFlatEncode.size() );

		ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): parsing search keys....\n" ) ) );
		//bind search
		//initial implementation we assume a full text search on all messages
		string::size_type iter = key.second.find( "text" );
		if( iter != string::npos )
		{
			//find criteria
			iter = key.second.find( OemoImapMgr::space );
			if( iter != string::npos )
			{
				++iter;
				string strCriteria( key.second.substr( iter ) );

				_variant_t v( (char*) strCriteria.c_str() );
				//set query criteria
				isys_search = Isys()->NewSearch();
				HRESULT hr = isys_search->put_QueryText( v );
				if( SUCCEEDED( hr ) )
				{
					//set document cache
					_variant_t v2 ( (char*) DocumentPath().c_str() );
					isys_search->put_QuerySource( v2 );
					
					ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): applying search filter...\n" ) ) );
					hr = isys_search->get_Filter( &isys_filter );
					if( SUCCEEDED( hr ) )
					{
						//filter the encoded path so the result set only contains the current user
						isys_filter->put_PathContains( (_bstr_t) (char*) strFilter.c_str() );
						ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): executing search....\n" ) ) );
						//execute search
						isys_results = isys_search->Execute();
						if( isys_results != NULL )
						{						
							ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): processing result set....\n" ) ) );
							if( isys_results->GetLength() > 0 )
							{
								//isys eccentrically does not use zero based arrays
								for( long i = 1; i < isys_results->GetLength() + 1; i++ )
								{

									isys_document = isys_results->Item( i );
									if( ( isys_document == NULL ) || ( isys_results->GetError()->GetNumber() != 0 ) )
									{
										break;
									}

									string strFileName( (char*) isys_document->GetFilename() );									
									string::size_type idx = strFileName.find_last_of( "\\" );
									if( idx != string::npos )
									{
										idx++;
										strFileName = strFileName.substr( idx );
										ImapUrlToUid::iterator iter = UrlToUid().find( strFileName );
										//the url is in our message cache
										if( iter != UrlToUid().end() )
										{
											search_result_set.push_back( iter->second );
											
										}
									}
								}//end for

								//always use uids locally
								if( Criteria()->Uid() == false )
								{
									message_set = MessageNumbersToUids( message_set );
								}//end if uid

								//intersection of specified message set and search result set
								m_vints = AtomsIntersection( message_set , search_result_set );
							
							}// end if results > 0
						}//end execute
					}//end if get filter
				}//end if put query text
			}//end if key found
		}//end if criteria found
		else
		{
			
			OemoImapSearchError e( "parse error in search"  , islCritical );

			throw( e );

		}		
		
	}
	catch( OemoImapSearchError e )
	{
		OemoImapSearchError me(  e.what()  , islCritical );

		throw( me );
	}
	catch( _com_error& e )
	{
		dump_com_error( e );	
		
	}
	catch( IISYSError& e )
	{
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Isys error->%s...\n") , (char*) e.Text ));
		OemoImapSearchError me( (char*) e.GetText()  , islCritical );
	}
	catch( ... )
	{	
	    ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Unknown exception.....\n" )));					
	}
	
	CoUninitialize();
}
//---------------------------------------------------------------------------------------------------------
void OemoImapIsysSearchResolver::ResolveParentheticalKey( OemoParentheticalSearchKey key , const std::string& top_level_message_set )
{
	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::ResolveParentheticalKey" ) , __LINE__ );

	

}
//---------------------------------------------------------------------------------------------------------
void OemoImapIsysSearchResolver::ResolveAtomicKey( OemoAtomicSearchKey key )
{

	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::ResolveAtomicKey" ) , __LINE__ );	

}
//---------------------------------------------------------------------------------------------------------
bool OemoImapIsysSearchResolver::InitializeIsysEngine()
{

#ifdef WIN32

	ACE_Trace _( ACE_TEXT( "OemoImapIsysSearchResolver::InitializeIsysEngine" ) , __LINE__ );
	ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): Initializing Isys engine....\n" ) ) );

	if( m_bInitialized == false ) return ( false );

	CoInitialize( NULL );

	try
	{
			HRESULT hr = Isys()->InitializeEngine( (char*) IsysPath().c_str() , (char*) constIsysKey.c_str() , _bstr_t() );
			if( FAILED( hr ) )
			{
				ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Could not initialize Isys Engine...\n")));
				m_bInitialized = false;					
			}			
		
	}
	catch( _com_error& e )
	{
		dump_com_error( e );	
		
	}
	catch( IISYSError& e )
	{
		ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Isys error->%s...\n") , (char*) e.Text ));		
	}
	catch( ... )
	{	
	    ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Unknown exception.....\n" )));					
	}
	
	CoUninitialize();

	return ( m_bInitialized );

#else

	return ( true );

#endif

}
//---------------------------------------------------------------------------------------------------------
#ifdef WIN32
void OemoImapIsysSearchResolver::dump_com_error( _com_error &e )
{
 
	ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Com error->%08lx %s...\n") , e.Error() ,
																					     (char*) e.ErrorMessage() ) );		   
   	
}
#endif



