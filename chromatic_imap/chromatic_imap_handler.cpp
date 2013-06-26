

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
using std::chrono::milliseconds;
using std::unique_ptr;

const unsigned long chromatic_handler::constCommandBufferLength = 1024;
const unsigned short chromatic_handler::constMaxTokenLength = 10;
const string chromatic_handler::constObjectAttributes = "\\STATS\\LOGS\\QUERY\\MANAGE\\SERVICES";
const string chromatic_handler::constServerName = "ChromaticMessageServer";

#define REGISTER_MASK ACE_Event_Handler::READ_MASK
#define REMOVE_MASK (ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL)

chromatic_handler::chromatic_handler ( const ACE_Time_Value& max_inactivity ) : m_currentCommand{ chromatic_command::oecNoCommand } ,
																				m_chromatic_packet_error{ chromatic_packet_error::peNoPacketError } ,
																				m_bSilent{ false } ,
																				m_chromatic_acceptor{ NULL } ,
																				creator_ { ACE_Thread::self() } ,
																				m_maximumTimeToWait{ max_inactivity } ,
																				m_dwTimerToken{ 0 } ,
																				m_ptrImap{ NULL } ,
																				m_bServerInfo{ false } ,
																				m_bReflex{ true } ,
																				m_bPrompt{ true } ,
																				m_bUid{ false } ,
																				ptr{ nullptr }
																	
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::chromatic_handler" ) , __LINE__ );

  try
  {
	  char hostname[1024];
	  gethostname( hostname , 1023 );
	  m_strComputerName = hostname;
  }
  catch( ... )

  {
	 //
  }

  m_setMap.insert( make_pair( chromatic_set_param::ospPrompt , true ) );
}


chromatic_handler::~chromatic_handler ()
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::~chromatic_handler" ) , __LINE__ );

	peer().close();
}

concurrency_t chromatic_handler::concurrency()
{ 
	return ( acceptor ()->concurrency () );
}

chromatic_thread_pool_ptr chromatic_handler::thread_pool ()
{
  return ( acceptor()->thread_pool () );
}

void chromatic_handler::destroy ()
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::destroy" ) , __LINE__ );
  reactor()->remove_handler (this, REMOVE_MASK);

 
  delete this;
}


int chromatic_handler::open ( void* accept )
{
  
	  ACE_Trace _( ACE_TEXT( "chromatic_handler::open" ) , __LINE__ );
	  output_boiler_plate();
	  
	  map_commands();
	  silent( false );

	  acceptor ( (chromatic_imap_acceptor_ptr) accept);

	  m_ptrImap = new chromatic_imap_mgr(  acceptor()->Data()->default_host() ,
										   acceptor()->Data()->dn()	,
										   this );
      
	
	  string strConcurrency;

	  //check for timeout every 30 seconds
	  ACE_Time_Value reschedule( m_maximumTimeToWait.sec() / 60 );

	  m_dwTimerToken = reactor()->schedule_timer( this , (void*) timer_id , m_maximumTimeToWait , reschedule );

	  if ( concurrency () == concurrency_t::thread_per_connection_ )
	  {
		return activate ( THR_DETACHED );
	  }

	  reactor ( acceptor()->reactor () );

	  ACE_INET_Addr addr;
  
	  if ( peer ().get_remote_addr (addr) == -1 )
	  {
		return ( -1 );
	  }  

	  if ( reactor ()->register_handler ( this,
										  REGISTER_MASK ) == -1 )
	  {
		ACE_ERROR_RETURN ((LM_ERROR,
						   "(%P|%t) can't register with reactor\n"),
						  -1);
	  }  
  
  
	  ACE_DEBUG ((LM_DEBUG,
				  "(%P|%t) connected with %s\n",
				  addr.get_host_name ()));  

	  

	  return ( 0L );  
	  
}


int chromatic_handler::close(u_long flags)
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::closed" ) , __LINE__ );
  ACE_UNUSED_ARG( flags ); 

  if( m_ptrImap != NULL )
  {
	  delete m_ptrImap;
	  m_ptrImap = NULL;
  }

  destroy ();

  return ( 0L );
}


int chromatic_handler::handle_input (ACE_HANDLE handle)
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::handle_input" ) , __LINE__ );

  ACE_UNUSED_ARG ( handle );  

  m_lastActivityTime = reactor()->timer_queue()->gettimeofday();

  if ( concurrency () ==  concurrency_t::thread_pool_ )
  {	  
     if ( ACE_OS::thr_equal (ACE_Thread::self(),
                             creator_) )
    {
          reactor ()->remove_handler ( this, REMOVE_MASK );

          return ( thread_pool ()->enqueue ( this ) );
    }
  }
  
  char buf[cmd_buffer_len];
  
  int rval = process (buf, sizeof (buf));

  if ( concurrency () ==  concurrency_t::thread_pool_ )
  {
      if ( rval != -1 )
	  {
        reactor ()->register_handler ( this,
                                       REGISTER_MASK );
	  }
  }
 
  return rval;
}


int chromatic_handler::handle_close (ACE_HANDLE handle,
                              ACE_Reactor_Mask mask)
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::handle_close" ) , __LINE__ );

  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (mask);
  
  stream() << " * Goodbye " << client_addr() << "......Oemo Imap Message Proxy" << crlf;
  flush_stream_to_connector();

  this->destroy ();
  return 0;
}

int chromatic_handler::handle_timeout( const ACE_Time_Value& now , const void* act )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::handle_timeout" ) , __LINE__ );

	if( act == ( short unsigned int) timer_id )
	{
		if( ( now - m_lastActivityTime ) >= m_maximumTimeToWait )
		{
			cout << client_addr() << " timed out" << endl;
			
			stream() <<  " BYE Autologout; idle for too long" << crlf;

			flush_stream_to_connector();

			reactor()->remove_handler( this , ACE_Event_Handler::READ_MASK );

			this->destroy ();
		}
	}
	
	return ( 0 );
}


int chromatic_handler::svc()
{
  ACE_Trace _( ACE_TEXT( "chromatic_handler::svc" ) , __LINE__ );

  char buf[BUFSIZ];

  // Forever...
  while( 1 )
    {
     
      if (this->process(buf, sizeof (buf)) == -1)
	  {	
        return -1;

	  }
      milliseconds dura( 50 );
      std::this_thread::sleep_for( dura );
    }

  return 0;
}


int chromatic_handler::process ( char *rdbuf ,  int rdbuf_len )
{
  

  ssize_t bytes_read;
  const  char ctrl_c = 3;
  const  char ctrl_z = 26;
  unsigned long dw{};


  //memory stream - the stream buffer could be very large , put it on the heap
  unique_ptr<ostringstream> ostr( new ostringstream ) ;

  //we'll only wait 2 seconds
  ACE_Time_Value timeout( 50 ); 

  memset( (void*) rdbuf , '\0' , rdbuf_len );

  error_packet( chromatic_packet_error::peNoPacketError );
  

  while ( ( bytes_read = peer().recv ( rdbuf, rdbuf_len , &timeout) ) > 0 )
  {
	    *ostr << rdbuf;

		memset( (void*) rdbuf , '\0' , sizeof( rdbuf_len ) );
	
	  
	   string::size_type st = ostr->str().find( ctrl_c );
	   string::size_type dt = ostr->str().find( ctrl_z );	   

	   //ctrl-c,ctrl-z
	   if( ( st != string::npos ) || ( dt != string::npos) )
	   {
		   return ( -1 );
	   }
		
	   //request terminator
	   st = ostr->str().find( "\n" );	 	   
	   if( st == string::npos )
	   {
		   if( ostr->str().size() <= constCommandBufferLength )
		   {
				continue;
		   }
	   }
	   

	   if( bytes_read > 0 )	 
	   {		    		
		    //append
			if( command() == chromatic_command::oecAppend )
			{

				context_ptr()->append_rec().stream() << ostr->str();
				if( context_ptr()->append_rec().stream().str().size() >= context_ptr()->append_rec().size() )
				{
					dw = bytes_read;	
					
				}

				dw = context_ptr()->append_rec().stream().str().size();
				cout << context_ptr()->append_rec().stream().str().size() << "\n";
				cout << command_str() << "\n";
				
				
				break;
			}

		    perform( ostr->str() );
			
			ACE_DEBUG ((LM_DEBUG, "\n\t\t------------------------------\n",  (char*) ostr->str().c_str() ) );  
		
			
			//just a line feed or waiting - ignore
			if( ( ostr->str().find ( "\n" ) != string::npos ) && ( ostr->str().size() == 2L ) )
			{
				stream() << output_prompt( false );
				flush_stream_to_connector();

				break;
			}
			
			

			if( command() != chromatic_command::oecLogout )
			{
			
				if( error_packet() != chromatic_packet_error::peNoPacketError )
				{
					format_packet_error( error_packet() );

					break;

				}
				
			}
			else
			{
				flush_stream_to_connector();

				return ( -1 );
			}									
			
			flush_stream_to_connector();

			ostr->str( "" );

			break;
	   }
		 
   }

  //set buffer to empty 
  ostr->str( "" );  
  ostr.release();

  return ( 0L );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::client_addr()
{
	ACE_INET_Addr addr;
	char addrstr[MAXHOSTNAMELEN + 1];

	peer().get_remote_addr( addr );
	addr.addr_to_string( addrstr , sizeof( addrstr ) );	

	string strTemp( addrstr );

	return ( strTemp );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::stream_out( const string& str )
{
	stream() << str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::stream_out( const byte_ptr& ptrBytes ,  unsigned long len )
{

	//

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::stream_out( const string& str , unsigned long len )
{	
	stream() << str.substr( 0 , len );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::flush_immediate( const string& atom )
{
	stream() << atom;

	flush_stream_to_connector();

}


///////////////////////////////////////////////////////////////////////////////////////////////////
void  chromatic_handler::flush_stream_to_connector()
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::flush_stream_to_connector" ) , __LINE__ );

	if( silent() == true )
	{
		stream().str( "" );

		return;
	}

	ACE_Time_Value timeout( 2 );
	size_t bytes_sent;

	//stream() << ends;

	string strOutput ( stream().str() );

	peer().send_n( (char*) strOutput.c_str() , strOutput.size() , &timeout , &bytes_sent );	
	


	stream().str( "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void  chromatic_handler::flush_binary_stream_to_connector( unsigned char* u_buffer , unsigned long dwLen )
{
	if( silent() == true )
	{
		stream().str( "" );

		return;
	}

	ACE_Time_Value timeout( 5 );

	peer().send( (unsigned char*) u_buffer , dwLen , &timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void  chromatic_handler::output_boiler_plate( string strConcurrency )
{  

   
  stream() << "* OK IMAP4rev1 Ready Oemo Imap Message Server " << computer_name() << " Overtone Software Inc. All Rights Reserved  Ready";
  stream() << crlf;
  stream() << output_prompt( false );
  
  flush_stream_to_connector();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::output_prompt( bool btoken )
{

    ostringstream ostr;

	if( btoken == true )
	{
		ostr << token() ;
		ostr << ends;
	}
	

	return ( ostr.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::parse_command( const  string& packet ,
									   map_of_parts& parts ,
									   unsigned short& uscommandParamCount )
{
	if( packet.empty() )
	{
		error_packet( chromatic_packet_error::peInvalidSyntax );

		return;
	}
	
	string space( " " );
	string str( packet );
	
	ACE_DEBUG ((LM_DEBUG,
				  "\n\t------------------------Client Packet------------------------\n" ) ); 
	ACE_DEBUG ((LM_DEBUG,
				  "(%P|%t) %s %s\n" , (char*) token().c_str() , (char*) str.c_str() ) );
	
	//get the token 
	string::size_type idx = str.find_first_of( space );
	if( idx != string::npos )
	{
		string strtoken;
		strtoken = str.substr( 0 , idx );
		if( strtoken.size() > constMaxTokenLength )
		{
			error_packet( chromatic_packet_error::peTokenTooLong );

			return;
		}

		parts.insert( make_pair( chromatic_packet_part::ppToken , strtoken ) );
		token( strtoken );

		//truncate
		idx++;
		str = str.substr( idx );
		if( str.size() == 0 )
		{
			error_packet( chromatic_packet_error::peInvalidSyntax );

			return;
		}
		
		//get the command
		string strcommand;
		idx = str.find_first_of( space );
		if( idx == string::npos )
		{
			//no params
			uscommandParamCount = 0;
			strcommand = str;
			rtrimlast( strcommand , "\n" );
			command_str( strcommand );

			parts.insert( make_pair( chromatic_packet_part::ppCommand , strcommand ) );
		}
		else
		{			
			strcommand = str.substr( 0 , idx );

			
			command_str( strcommand );

			//truncate
			idx++;
			str = str.substr( idx );		

			parts.insert( make_pair( chromatic_packet_part::ppCommand , strcommand ) );
			chomp( str , "\n" );						
			parts.insert( make_pair( chromatic_packet_part::ppPayload , str ) );
		}
	}
	else
	{
		error_packet( chromatic_packet_error::peNoCommand );

		return;
	}

	error_packet( chromatic_packet_error::peNoPacketError );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::perform( const string& str )
{
	if( str.empty() )
	{
		error_packet( chromatic_packet_error::peInvalidPayload );

		return;
	}

	unsigned short uscommands = 0;
	map_of_parts PartsMap;

	//parse the packet
	parse_command( str , PartsMap , uscommands );
	if( error_packet() != chromatic_packet_error::peNoPacketError )
	{
		return;
	}

	//call the required function
	map_of_parts::iterator pIter = PartsMap.find( chromatic_packet_part::ppCommand );
	if( pIter != PartsMap.end() )
	{
		string strcommand( pIter->second );
		strcommand = lcase( strcommand );

		map_of_commands::iterator c_iter = command_map().find( strcommand );
		if( c_iter  == command_map().end() )
		{			
			error_packet( chromatic_packet_error::peUnrecognizedCommand );

			return;
		}

		chromatic_handler::ptrToFunction ptrFunc = c_iter->second;

		if( ptrFunc == NULL )
		{
			stream() << output_prompt( false );
			stream() << "NO no default action for this keyword" << crlf;
			flush_stream_to_connector();

			return;
		}	

		

		uid( false );
		context_ptr()->uids( false );

		//params
		pIter = PartsMap.find( chromatic_packet_part::ppPayload );
		string strPayload;

		pIter != PartsMap.end() ? strPayload = pIter->second : strPayload = "";		
		int	lRetVal = ( this->*ptrFunc ) ( strPayload );	
		
		if( ( lRetVal != 0 ) || ( command() == chromatic_command::oecError ) )
		{
			return;
		}

		

	}
	else
	{
		return;
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////	
void chromatic_handler::chomp( string& str , const string& token )
{
	string strTemp( str );	
	
	if( str.empty() )
	{
		return;
	}

	string::size_type idx = str.find( token );
	if( idx != string::npos )
	{
		str = str.substr( 0 , --idx );
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::gnaw( string& str , const string& token )
{
		string::size_type st;
	
		st = str.find( token , 0 );

		if( st != string::npos )
		{
			str.resize( st );
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::extract_quoted_string( const string& str )
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::rtrimlast( string& str , const string& token )
{
	if ( str.empty() )
	{
		return;
	}
	
	string::size_type idx = str.find_last_of( (char*) token.c_str() );

	//if not the last ignore
	if( idx != str.size() - 1 )
	{
		return;
	}

	if( idx != string::npos )
	{
		str = str.substr( 0 , --idx );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::ltrim( const string &szToTrim, const string& szTrimChars )
{
    string szToReturn = szToTrim;

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
string chromatic_handler::rtrim( const string &szToTrim, const string& szTrimChars )
{
    string szToReturn = szToTrim;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::map_commands()
{
	
	command_map().insert( std::pair<string , ptrToFunction> ( "login", &chromatic_handler::on_login ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "logout", &chromatic_handler::on_logout ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "create", &chromatic_handler::on_create ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "select", &chromatic_handler::on_select ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "examine", &chromatic_handler::on_examine ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "list", &chromatic_handler::on_list ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "delete", &chromatic_handler::on_delete ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "rename", &chromatic_handler::on_rename ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "lsub", &chromatic_handler::on_lsub ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "subscribe", &chromatic_handler::on_subscribe ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "unsubscribe", &chromatic_handler::on_unsubscribe ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "noop", &chromatic_handler::on_noop ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "check", &chromatic_handler::on_check ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "capability", &chromatic_handler::on_capability ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "search", &chromatic_handler::on_search ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "copy", &chromatic_handler::on_copy ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "fetch", &chromatic_handler::on_fetch ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "store", &chromatic_handler::on_store ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "uid", &chromatic_handler::on_uid ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "close", &chromatic_handler::on_close ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "expunge", &chromatic_handler::on_expunge ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "append", &chromatic_handler::on_append ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "starttls", &chromatic_handler::on_starttls ) );
	command_map().insert( std::pair<string , ptrToFunction> ( "status", &chromatic_handler::on_status ) );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::lcase( const string& str )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string chromatic_handler::ucase( const string& str )
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
void chromatic_handler::output_ok_response( const string& ack )
{
	stream() << token() << " " << "OK " << ack << crlf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::output_no_response( const string& nack )
{
	stream() << token() << " " << "NO " << nack << crlf;
	command_str( "" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::format_packet_error( chromatic_packet_error pe )
{	

	command( chromatic_command::oecError );
	stream() << output_prompt();

	stream() << " NO ";

	if( pe == chromatic_packet_error::peNoToken ) stream() << "no token supplied - expected";
	else if( pe == chromatic_packet_error::peTokenTooLong) stream() << "token too long";
	else if( pe == chromatic_packet_error::peNoCommand ) stream() << "no command supplied - expected";
	else if( pe == chromatic_packet_error::peCommandTooLong ) stream() << "buffer to large";
	else if( pe == chromatic_packet_error::peTooManyParameters ) stream() << "too many parameters supplied";
	else if( pe == chromatic_packet_error::peInvalidPayload ) stream() << "invalid buffer";
	else if( pe == chromatic_packet_error::peInvalidSyntax ) stream() << "invalid syntax";
	else if( pe == chromatic_packet_error::peUnrecognizedCommand ) stream() << "command unrecognized";
	else if( pe == chromatic_packet_error::peSelectState ) stream() << " a target must be selected before a query";
	else if( pe == chromatic_packet_error::peTooFewParameters ) stream() << " too few parameters - more";
	else if( pe == chromatic_packet_error::peNoParametersExpectedGotSome ) stream() << " no parameters expected - got some";
	else if( pe == chromatic_packet_error::peParametersExpectedGotNone ) stream() << "parameters expected - got none";
	else if( pe == chromatic_packet_error::peFailure ) stream() << "a server operation within this context failed";
	else if( pe == chromatic_packet_error::peInvalidObject ) stream() << "object does not exist";
	else if( pe == chromatic_packet_error::peInvalidSpecification ) stream() << "invalid specification";
	else if( pe == chromatic_packet_error::peNoSpecification ) stream() << "no specification";
	else if( pe == chromatic_packet_error::peInvalidState ) stream() << "invalid state";
	else if( pe == chromatic_packet_error::peAttributeNotSupported ) stream() << "attribute not supported";
	else if( pe == chromatic_packet_error::peNotAuthenticated ) stream() << "please login";
	else if( pe == chromatic_packet_error::peAlreadyAuthenticated ) stream() << "already logged in";
	else if( pe == chromatic_packet_error::peNotEnoughPrivileges ) stream() << "not enough privileges";
	

	stream() << crlf;
	stream() << output_prompt( false );

	flush_stream_to_connector();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool chromatic_handler::check_connection_state( chromatic_state state )
{
	bool bRet = false;

	current_state::iterator iter = m_currentState.find( state );
	iter == m_currentState.end() ? bRet = false  : bRet = true;

	return ( bRet );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::update_connection_state( chromatic_set_op op , chromatic_state state )
{
	switch( op )
	{
		case chromatic_set_op::osopAdd:
			m_currentState.insert( state );
			break;
		case chromatic_set_op::osopDelete:
			m_currentState.erase( state );
			break;
		case chromatic_set_op::osopClear:
			m_currentState.clear();
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//handlers
int chromatic_handler::on_append( const string& params )
{	
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_append" ) , __LINE__ );

	command( chromatic_command::oecAppend );

	ostringstream ostr;
	ostr << "* "  << command_str() << "%s" << crlf ;
			
	try
	{
		bool bRet = context_ptr()->on_append( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			stream() << "+ waiting" << crlf;
			flush_stream_to_connector();

			//output_ok_response( "append completed" );
		}
		else
		{
			output_no_response( "append failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}

	
	
	return ( 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
int chromatic_handler::on_authenticate( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_authenticate" ) , __LINE__ );
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_capability( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::OnCapability" ) , __LINE__ );

	command( chromatic_command::oecCapability );

	ostringstream ostr;
	ostr << "* "  << command_str() << " " << "%s" << crlf ;
			
	bool bRet = context_ptr()->on_capability( params , ostr.str() , stream() );
    if( bRet == true ) 
	{
		output_ok_response( "capability completed" );
	}	
	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_check( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_check" ) , __LINE__ );

	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_close( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_close" ) , __LINE__ );

	command( chromatic_command::oecClose );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}
	
	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = context_ptr()->on_close( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "close completed" );
		}
		else
		{
			output_no_response( "close failed" );
		}
		m_currentState.erase( chromatic_state::oesSelected );
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	
	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_copy( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_copy" ) , __LINE__ );

	command( chromatic_command::oecCopy );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}
	
	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = context_ptr()->on_copy( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "copy completed" );
		}
		else
		{
			output_no_response( "copy failed" );
		}		
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_status( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_status" ) , __LINE__ );

	command( chromatic_command::oecStatus );

	ostringstream ostr;
	ostr << "* "  << command_str() << " " << "%s (%s)" << crlf ;
			
	try
	{
		bool bRet = context_ptr()->on_status( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "status completed" );
		}
		else
		{
			output_no_response( "status failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}
	
	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_create( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_create" ) , __LINE__ );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peNotAuthenticated  );

		return ( 0 );
	}

	command( chromatic_command::oecCreate );
	
	ostringstream ostr;

	try
	{
		bool bRet = context_ptr()->on_create( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "create completed" );
		}
		else
		{
			output_no_response( "create failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	


	return ( 0 );

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_delete( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_delete" ) , __LINE__ );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peNotAuthenticated  );

		return ( 0 );
	}

	command( chromatic_command::oecDelete );
	
	ostringstream ostr;

	try
	{
		bool bRet = context_ptr()->on_delete( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "delete completed" );
		}
		else
		{
			output_no_response( "delete failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	return ( 0 );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_examine( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_examine" ) , __LINE__ );

	command( chromatic_command::oecExamine );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = context_ptr()->on_examine( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "[read-only] examine completed" );
			update_connection_state( chromatic_set_op::osopAdd , chromatic_state::oesSelected );
		}
		else
		{
			output_no_response( "examine failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	return ( 0 );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_expunge( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_expunge" ) , __LINE__ );

	command( chromatic_command::oecExpunge );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = context_ptr()->on_expunge( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "expunge completed" );
		}
		else
		{
			output_no_response( "expunge failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	
	

	return ( 0 );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_login( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_login" ) , __LINE__ );

	command( chromatic_command::oecLogin );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == true )
	{
		error_packet( chromatic_packet_error::peAlreadyAuthenticated );

		return ( 0 );
	}
	
	ostringstream ostr;

	bool bRet = context_ptr()->on_login( params , ostr.str() , stream() );
    if( bRet == true ) 
	{
		update_connection_state( chromatic_set_op::osopAdd , chromatic_state::oesAuthenticated );

		output_ok_response( "login completed" );
	}
	else
	{
		output_no_response( "login failed" );
	}

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_logout( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_logout" ) , __LINE__ );

	command( chromatic_command::oecLogout );
	
	ostringstream ostr;

	bool bRet = context_ptr()->on_logout( params , ostr.str() , stream() );
    if( bRet == true ) 
	{
		output_ok_response( "logout completed" );
	}
	else
	{
		output_no_response( "logout failed" );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_fetch( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_fetch" ) , __LINE__ );

	command( chromatic_command::oecFetch );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		
		bool bRet = context_ptr()->on_fetch( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			if( uid() == false )
			{				
				output_ok_response( "fetch completed" );
			}
		}
		else
		{
			output_no_response( "fetch failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_list( const string& params )
{	
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_list" ) , __LINE__ );

	command( chromatic_command::oecList );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "* "  << command_str() << " (%s) \"%s\" \"%s\"" << crlf ;

	try
	{
		bool bRet = context_ptr()->on_list( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "list completed" );
		}
		else
		{
			output_no_response( "list failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_lsub( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_lsub" ) , __LINE__ );

	command( chromatic_command::oecLsub );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "* "  << command_str() << " (%s) \"%s\" \"%s\"" << crlf ;

	try
	{
		bool bRet = context_ptr()->on_lsub( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "lsub completed" );
		}
		else
		{
			output_no_response( "lsub failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_noop( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_noop" ) , __LINE__ );

	command( chromatic_command::oecNoop );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{		
		return ( 0 );
	}	

	ostringstream ostr;

	try
	{
		context_ptr()->on_noop( params , ostr.str() , stream() );
	    output_ok_response( "noop completed" );
	
	}
	catch( chromatic_imap_mgr_error& )
	{		
		//
	}	

	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::wait_()
{
	stream() << output_prompt();
	stream() << " "  << "WAIT" << crlf;

	flush_stream_to_connector();

    update_connection_state( chromatic_set_op::osopAdd , chromatic_state::oesWait );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_rename( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_rename" ) , __LINE__ );

	command( chromatic_command::oecRename );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = context_ptr()->on_rename( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "rename completed" );
		}
		else
		{
			output_no_response( "rename failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_search( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::OnSearch" ) , __LINE__ );

	command( chromatic_command::oecSearch );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		
		bool bRet = context_ptr()->on_search( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			if( uid() == false )
			{	
				output_ok_response( "search completed" );
			}
		}
		else
		{
			output_no_response( "search failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_select( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_select" ) , __LINE__ );

	command( chromatic_command::oecSelect );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = context_ptr()->on_select( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "[read-write] select completed" );
			update_connection_state( chromatic_set_op::osopAdd , chromatic_state::oesSelected );
		}
		else
		{
			output_no_response( "select failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	

	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_starttls( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_starttls" ) , __LINE__ );

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_store( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::OnStore" ) , __LINE__ );

	command( chromatic_command::oecStore );

	if( ( check_connection_state( chromatic_state::oesAuthenticated ) == false )
			|| ( check_connection_state( chromatic_state::oesSelected ) == false ) )
	{
		error_packet( chromatic_packet_error::peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;	

	try
	{
		bool bRet = context_ptr()->on_store( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "store completed" );
		}
		else
		{
			output_no_response( "store failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_subscribe( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_subscribe" ) , __LINE__ );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peNotAuthenticated  );

		return ( 0 );
	}

	command( chromatic_command::oecSubscribe );
	
	ostringstream ostr;

	try
	{
		bool bRet = context_ptr()->on_subscribe( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "subscribe completed" );
		}
		else
		{
			output_no_response( "subscribe failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_uid( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_uid" ) , __LINE__ );

	uid( true );
	context_ptr()->uids( true );

	string space( " " );

	string str( lcase( params ) );
	
	string::size_type idx = str.find_first_of( space );
	if( idx != string::npos )
	{
		
		string strParam = str.substr( ++idx );
		str = str.substr( 0 , idx );
		
		str.resize( str.size() - 1 );
		if( str.compare( "fetch" ) == 0L )
		{
			on_fetch( strParam );
		}
		else if( str.compare( "search" ) == 0L )
		{
			on_search( strParam );
		}		
	}
	

	output_ok_response( "uid completed" );

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int chromatic_handler::on_unsubscribe( const string& params )
{
	ACE_Trace _( ACE_TEXT( "chromatic_handler::on_unsubscribe" ) , __LINE__ );

	if( check_connection_state( chromatic_state::oesAuthenticated ) == false )
	{
		error_packet( chromatic_packet_error::peNotAuthenticated  );

		return ( 0 );
	}

	command( chromatic_command::oecUnsubscribe );
	
	ostringstream ostr;

	try
	{
		bool bRet = context_ptr()->on_unsubscribe( params , ostr.str() , stream() );
		if( bRet == true ) 
		{
			output_ok_response( "unsubscribe completed" );
		}
		else
		{
			output_no_response( "unsubscribe failed" );
		}
	}
	catch( chromatic_imap_mgr_error& oe )
	{		
		output_no_response( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chromatic_handler::parse_params( const string& params , string_container& strings )
{

		string str( params );
		string space( " " );
		string left_paren( "(" );
		string right_paren( ")" );

		//get one parameter - no atoms
		string::size_type idx = str.find( left_paren );
		if( idx == string::npos )
		{	
			//truncate
			string strTemp;
			chomp( str , "\n" );
			idx++;
			strTemp = strTemp.substr( idx );

			if( ! strTemp.empty() )
			{
				error_packet( chromatic_packet_error::peInvalidSyntax );

				return;
			}			

			strings.push_back( str );
		}	
		else
		{
			str = str.substr( 1 );

			//parameter with atoms
			idx = str.find_first_of( space );
			if( idx == string::npos )
			{					
				error_packet( chromatic_packet_error::peInvalidSyntax );

				return;
			}			
			
			string strParam = str.substr( 0 , idx );
			idx++;
			strings.push_back( strParam );
			
			//find the closing paren
			string::size_type jdx = str.find( right_paren );
			if( jdx != string::npos )
			{
			  //get the atoms
			  str = str.substr( idx , jdx - idx );
			  chomp( str , "\n" );	
			  
			  strings.push_back( str );
			}
			else
			{
				error_packet( chromatic_packet_error::peInvalidSyntax );

				return;
			}		
			
		}
}


chromatic_imap_mgr_ptr chromatic_handler::context_ptr()
{
	
	return ( m_ptrImap );
	
}
