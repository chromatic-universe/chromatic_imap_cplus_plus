
//suppress from adaptive communication environment
#pragma clang diagnostic ignored "-Wself-assign"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"


#include "chromatic_imap.h"

using namespace chromatic_imap_protocol_impl;

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
using std::unique_ptr;
using std::map;

//immutable
const string chromatic_imap_search_criteria::constMessageSetAll = "1:*";
const string chromatic_imap_search_resolver::const_strKey = "DTZW2 UBMFR 6B5CH 6DWLC J57P9 NRXLA";


//---------------------------------------------------------------------------------------------------------
//ctor
chromatic_imap_search_criteria::chromatic_imap_search_criteria( const std::string& criteria ,
																const std::string& message_set ,
																bool bUid ,
																bool bPropagateExceptions ) :  m_messageSet( message_set ) ,
																							   m_criteria( criteria ) ,
																							   m_bUid( bUid ) ,
																							   m_bPropagateExceptions( bPropagateExceptions )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::chromatic_imap_search_criteria" ) , __LINE__ );

}

//---------------------------------------------------------------------------------------------------------
//dtor
chromatic_imap_search_criteria::~chromatic_imap_search_criteria()
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::~chromatic_imap_search_criteria" ) , __LINE__ );

}

//----------------------------------------------------------------------------------------------------------
string chromatic_imap_search_criteria::extract_quoted_string( const string& str )
{
	   ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::extract_quoted_string" ) , __LINE__ );

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
chromatic_searches chromatic_imap_search_criteria::extract_parenthetical_keys( const std::string& str )
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::extract_parenthetical_keys" ) , __LINE__ );
	
	chromatic_searches searches;

	return ( searches );

}

//----------------------------------------------------------------------------------------------------------
string chromatic_imap_search_criteria::search_key_to_str( const chromatic_search_keys key )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::search_key_to_str" ) , __LINE__ );

	string str( "nil" );	
	
	//supported keys
	if( key ==  chromatic_search_keys::oskAll ) str = "all";
	else if( key == chromatic_search_keys::oskText ) str = "text";

	return ( str );

}

//----------------------------------------------------------------------------------------------------------
chromatic_search_keys chromatic_imap_search_criteria::str_to_search_key( const std::string& str )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::str_to_search_key" ) , __LINE__ );


	chromatic_search_keys key( chromatic_search_keys::oskUnsupportedKey );

	string strKey( str );

	std::transform( strKey.begin() , strKey.end() , strKey.begin(), ::tolower );


	//supported keys
	if( strKey.compare( "all" ) == 0 ) key = chromatic_search_keys::oskAll;
	else if ( strKey.compare( "text" ) == 0 ) key = chromatic_search_keys::oskText;

	return ( key );

}

//----------------------------------------------------------------------------------------------------------
void chromatic_imap_search_criteria::deconstruct_criteria( chromatic_imap_search_resolver_ptr resolver )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_criteria::deconstruct_criteria" ) , __LINE__ );

	string source(  raw_criteria() );

	std::transform( source.begin() , source.end() , source.begin(), ::tolower );
	search_keys keys;
	search_keys found_keys;
	
	//supported keys
	for( int i = (int) chromatic_search_keys::oskAll; i < (int) chromatic_search_keys::oskUnsupportedKey; i++ )
	{
		keys.insert( (chromatic_search_keys) i );
	}

	try
	{
		//no nested parenthetical keys , straightforward atomic logic search
		if( source.find( chromatic_imap_mgr::right_paren ) == string::npos )
		{
			string::size_type idx = source.find_first_of( chromatic_imap_mgr::space );
			if( idx != string::npos )
			{
				string str( source.substr( 0 , idx ) );
				//key?
				for( search_keys::iterator iter = keys.begin();
							iter != keys.end();
								iter++ )
				{
					if( search_key_to_str( *iter ).compare( str ) == 0 )
					{						
						found_keys.insert( *iter );						
					}
				}
			
				//not a key , message set?
				unsigned long dwMax( 0L );
				unique_ptr<chromatic_message_manip> cma ( new chromatic_message_manip() );
				uid() == false ? dwMax = resolver->imap_mgr()->messages()->size() : dwMax = resolver->imap_mgr()->max_uid();
				if( found_keys.empty() == true )
				{					

					bool bRet = cma->is_message_set( str , dwMax );
					//not a key  , not a message set - throw
					if( bRet == false )
					{
						chromatic_imap_search_error e( "search key not recognized"  , chromatic_imap_search_error_level::islCritical );

						throw( e );
					}	
					else
					{						
						//top level
						message_set( str );
					}
				}
				else
				{
					if( found_keys.find( chromatic_search_keys::oskAll ) != found_keys.end() )
					{
						message_set( constMessageSetAll );
						//remove all key
						string strAll( "all" );
						string::size_type idx = source.find( strAll );
						if( idx != string::npos )
						{
							source.erase( idx , strAll.size() );
						}
					}
					else if( message_set().compare( "nil" ) == 0 )
					{
						message_set( constMessageSetAll );
					}
					else if( ( found_keys.find( chromatic_search_keys::oskAll ) != found_keys.end() ) && (  message_set().compare( "nil" ) == 0 ) )
					{
						//syntax error
						chromatic_imap_search_error e( "syntax error in search"  , chromatic_imap_search_error_level::islCritical );

						throw( e );
					}

				}

				//expand message sequence
				v_int ints( cma->expand_message_sequence( message_set() , dwMax ) );
				//parenthetical key
				chromatic_parenthetical_search_key opsk( pair<chromatic_logic_operations , string>( chromatic_logic_operations::olpAnd , source ) );

				//resolve
				resolver->resolve_parenthetical_key( opsk , ints );

			}
		}
		else
		{
			//nested parenthetical keys
		}
		
	}
	catch( chromatic_imap_search_error& osie )
	{
		if( m_bPropagateExceptions == true )
		{
			chromatic_imap_search_error e( osie.what() , osie.error_level() );

			throw( e );
		}
	}	
}

//chromatic_imap_search_resolver
//---------------------------------------------------------------------------------------------------------
chromatic_imap_search_resolver::chromatic_imap_search_resolver( chromatic_imap_mgr_ptr ptrimap_mgr ,
																chromatic_imap_search_criteria_ptr criteria ,
																const std::string& document_path ,
																const std::string& search_domain ) :	m_ptrImap( ptrimap_mgr ) ,
																										m_ptrCriteria( criteria ) ,
																										m_strDocumentPath( document_path ) ,
																										m_strSearchDomain( search_domain ) ,
																										m_bInitialized( false )


																							
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::chromatic_imap_search_resolver" ) , __LINE__ );


}

//---------------------------------------------------------------------------------------------------------
chromatic_imap_search_resolver::~chromatic_imap_search_resolver()
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::~chromatic_imap_search_resolver" ) , __LINE__ );
}

//---------------------------------------------------------------------------------------------------------
v_int chromatic_imap_search_resolver::uids_to_message_numbers( const v_int v_ints )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::uids_to_message_numbers" ) , __LINE__ );

	v_int ret_v_ints;
	typedef map<unsigned long , unsigned long> uid_to_message;
	uid_to_message UtoM;

	for( imap_server_messages::iterator dter = imap_mgr()->messages()->begin();
												dter != imap_mgr()->messages()->end();
													dter++ )
	{
		
		pair<unsigned long , unsigned long> pr( dter->second.uid() ,
											    boost::lexical_cast<unsigned long>( dter->first ) );
	    UtoM.insert( pr );
	}
	
	for( v_int::const_iterator iter = v_ints.begin();
				iter != v_ints.end();
					iter++ )
	{
		if( UtoM.find( *iter ) != UtoM.end() )
		{
			ret_v_ints.push_back( UtoM[*iter] );
		}
	}

	return( ret_v_ints );
}

//---------------------------------------------------------------------------------------------------------
v_int chromatic_imap_search_resolver::message_numbers_to_uids( const v_int v_ints )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::message_numbers_to_uids" ) , __LINE__ );

	v_int ret_v_ints;
	
	for( v_int::const_iterator iter = v_ints.begin();
												iter != v_ints.end();
													iter++ )
	{
		string str( boost::lexical_cast<string>( *iter ) );
		if( imap_mgr()->messages()->find( str ) != imap_mgr()->messages()->end() )
		{
			ret_v_ints.push_back( (*imap_mgr()->messages() )[str].uid() );
		}
	}

	return( ret_v_ints );
}

//---------------------------------------------------------------------------------------------------------
void chromatic_imap_search_resolver::resolve_parenthetical_key( chromatic_parenthetical_search_key key , v_int v_ints )
{

	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::resolve_parenthetical_key" ) , __LINE__ );

	if( m_bInitialized == false )
	{
		chromatic_imap_search_error e( "search engine not initialized"  , chromatic_imap_search_error_level::islCritical );

		throw( e );
	}


	v_int search_result_set;
	v_int message_set( v_ints );

	try
	{

		/*ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): resolving parenthetical search key....\n" ) ) );
		
		IISYSSearchPtr isys_search;
		IISYSResultListPtr isys_results;
		IISYSDocumentPtr isys_document;
		IISYSFilterPtr isys_filter;
		const string strFlat( "-FlatUrlSpace-" ); 

		//url to uid map
		UrlToUid().clear();
		
		//fragment of url for file filter workaround
		string strFlatEncode( imap_mgr()->base64_encode( (unsigned char*) strFlat.c_str() , strFlat.size() ) );

		//populate map
		for( imap_server_messages::iterator iter = imap_mgr()->messages()->begin();
										   iter != imap_mgr()->messages()->end();
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
			
			imap_mgr()->GetDomainName( str );
			to_lower( str );
			ostr << str;
		}
		else
		{
			str = SearchDomain();
			to_lower( str );
			ostr << str << ".com";
		}
		ostr << "/exchange/" << imap_mgr()->User() << "/" << strFlat;
		
		string strFilter( imap_mgr()->base64_encode( (unsigned  char*) ostr.str().c_str() , ostr.str().size() ) );
		strFilter = strFilter.substr( 0 , strFilter.size() - strFlatEncode.size() );

		ACE_DEBUG ( ( LM_INFO, ACE_TEXT ( "%D %M (thread_id->%t): parsing search keys....\n" ) ) );
		//bind search
		//initial implementation we assume a full text search on all messages
		string::size_type iter = key.second.find( "text" );
		if( iter != string::npos )
		{
			//find criteria
			iter = key.second.find( chromatic_imap_mgr::space );
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
									message_set = message_numbers_to_uids( message_set );
								}//end if uid

								//intersection of specified message set and search result set
								m_v_ints = AtomsIntersection( message_set , search_result_set );
							
							}// end if results > 0
						}//end execute
					}//end if get filter
				}//end if put query text
			}//end if key found
		}//end if criteria found
		else
		{
			
			chromatic_imap_search_error e( "parse error in search"  , chromatic_imap_search_error_level::islCritical );

			throw( e );

		}	*/
		
	}
	catch( chromatic_imap_search_error& e )
	{
		chromatic_imap_search_error me(  e.what()  , chromatic_imap_search_error_level::islCritical );

		throw( me );
	}

	catch( ... )
	{	
	    ACE_ERROR ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Unknown exception.....\n" )));					
	}
	

}

//---------------------------------------------------------------------------------------------------------
void chromatic_imap_search_resolver::resolve_parenthetical_key( chromatic_parenthetical_search_key key , const std::string& top_level_message_set )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::resolve_parenthetical_key" ) , __LINE__ );
}

//---------------------------------------------------------------------------------------------------------
void chromatic_imap_search_resolver::resolve_atomic_key( chromatic_atomic_search_key key )
{
	ACE_Trace _( ACE_TEXT( "chromatic_imap_search_resolver::resolve_atomic_key" ) , __LINE__ );
}





