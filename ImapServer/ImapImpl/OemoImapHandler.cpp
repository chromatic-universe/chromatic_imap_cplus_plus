//OemoProtocol Overtone Software , Inc. 2004
//Intitial version William K. Johnson 14 July 2004


#include "ace/Trace.h"
#ifndef OEMO_IMAP_IMPL_H
#include "OemoImapImpl.h"
#endif


using namespace oipi;
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


typedef WCHAR wchar;


const unsigned long OemoHandler::constCommandBufferLength = 1024;
const unsigned short OemoHandler::constMaxTokenLength = 10;
const string OemoHandler::constObjectAttributes = "\\STATS\\LOGS\\QUERY\\MANAGE\\SERVICES";
const string OemoHandler::constServerName = "OemoMessageServer";

#define REGISTER_MASK ACE_Event_Handler::READ_MASK
#define REMOVE_MASK (ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL)

OemoHandler::OemoHandler ( const ACE_Time_Value& max_inactivity ) : m_currentCommand( oecNoCommand ) ,
																	m_packetError( peNoPacketError ) ,
																	m_bSilent( false ) ,
																	m_oemoAcceptor( NULL ) ,
																	creator_ ( ACE_Thread::self () ) ,
																	m_maximumTimeToWait( max_inactivity ) ,
																	m_dwTimerToken( 0 ) ,
																	m_ptrImap( NULL ) ,																	
																	m_bServerInfo( false ) ,
																	m_bReflex( true ) ,
																	m_bPrompt( true ) ,
																	m_bUid( false )
																	
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::OemoHandler" ) , __LINE__ );	

  char computer[256];
  unsigned long dw = 256;

  ::GetComputerName(  computer ,  &dw );
  m_strComputerName = computer;  
  
  try
  {
	 //
  }
  catch( ... )

  {
	 //
  }

  m_setMap.insert( make_pair( ospPrompt , true ) );
}


OemoHandler::~OemoHandler ()
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::~OemoHandler" ) , __LINE__ );	

	peer().close();
}

int OemoHandler::concurrency()
{ 
	return ( Acceptor ()->concurrency () ); 
}

OemoThreadPool* OemoHandler::thread_pool ()
{
  return ( Acceptor()->thread_pool () );
}

void OemoHandler::destroy ()
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::destroy" ) , __LINE__ );	
  reactor()->remove_handler (this, REMOVE_MASK);

 
  delete this;
}


int OemoHandler::open ( void* acceptor )
{
  
	  ACE_Trace _( ACE_TEXT( "OemoHandler::open" ) , __LINE__ );	
	  OutPutBoilerPlate(); 
	  
	  MapCommands();
	  Silent( false );	 	  


	  Acceptor ((OemoAcceptor *) acceptor);

	  m_ptrImap = new OemoImapMgr( Acceptor()->Data()->DefaultHost() ,
								   Acceptor()->Data()->Dn()	,
								   this );
      
	
	  string strConcurrency;

	  //check for timeout every 30 seconds
	  ACE_Time_Value reschedule( m_maximumTimeToWait.sec() / 60 );

	  m_dwTimerToken = reactor()->schedule_timer( this , (void*) timer_id , m_maximumTimeToWait , reschedule );

	  if ( concurrency () == thread_per_connection_ )
	  {
		return activate ( THR_DETACHED );
	  }

	  reactor ( Acceptor()->reactor () );

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


int OemoHandler::close(u_long flags)
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::closed" ) , __LINE__ );	
  ACE_UNUSED_ARG( flags ); 

  if( m_ptrImap != NULL )
  {
	  delete m_ptrImap;
	  m_ptrImap = NULL;
  }

  destroy ();

  return ( 0L );
}


int OemoHandler::handle_input (ACE_HANDLE handle)
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::handle_input" ) , __LINE__ );	

  ACE_UNUSED_ARG ( handle );  

  m_lastActivityTime = reactor()->timer_queue()->gettimeofday();

  if ( concurrency () == thread_pool_ )
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

  if ( concurrency () == thread_pool_ )
  {
      if ( rval != -1 )
	  {
        reactor ()->register_handler ( this,
                                       REGISTER_MASK );
	  }
  }
 
  return rval;
}


int OemoHandler::handle_close (ACE_HANDLE handle,
                              ACE_Reactor_Mask mask)
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::handle_close" ) , __LINE__ );	

  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (mask);
  
  Stream() << " * Goodbye " << ClientAddr() << "......Oemo Imap Message Proxy" << crlf;
  FlushStreamToConnector();

  this->destroy ();
  return 0;
}

int OemoHandler::handle_timeout( const ACE_Time_Value& now , const void* act )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::handle_timeout" ) , __LINE__ );	

    unsigned short token = (unsigned short) act;

	if( act == timer_id )
	{
		if( ( now - m_lastActivityTime ) >= m_maximumTimeToWait )
		{
			cout << ClientAddr() << " timed out" << endl;			
			
			Stream() <<  " BYE Autologout; idle for too long" << crlf;			

			FlushStreamToConnector();

			reactor()->remove_handler( this , ACE_Event_Handler::READ_MASK );

			this->destroy ();
		}
	}
	
	return ( 0 );
}


int OemoHandler::svc()
{
  ACE_Trace _( ACE_TEXT( "OemoHandler::svc" ) , __LINE__ );	

  char buf[BUFSIZ];

  // Forever...
  while( 1 )
    {
     
      if (this->process(buf, sizeof (buf)) == -1)
	  {	
        return -1;

	  }
	  SleepEx( 50 , FALSE );
    }

  return 0;
}


int OemoHandler::process ( char *rdbuf ,  int rdbuf_len )
{
  

  ssize_t bytes_read;
  const  char ctrl_c = 3;
  const  char ctrl_z = 26;
  unsigned long dw = 0L;


  //memory stream - the stream buffer could be very large , put it on the heap
  auto_ptr<ostringstream> ostr( new ostringstream ) ;

  //we'll only wait 2 seconds
  ACE_Time_Value timeout( 50 ); 

  memset( (void*) rdbuf , '\0' , rdbuf_len );

  ErrorPacket( peNoPacketError ); 
  

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
			if( Command() == oecAppend )
			{

				ContextPtr()->AppendRec().Stream() << ostr->str();
				if( ContextPtr()->AppendRec().Stream().str().size() >= ContextPtr()->AppendRec().Size() )
				{
					dw = bytes_read;	
					
				}

				dw = ContextPtr()->AppendRec().Stream().str().size();
				cout << ContextPtr()->AppendRec().Stream().str().size() << "\n";				
				cout << CommandStr() << "\n";
				
				
				break;
			}

		    Perform( ostr->str() );	
			
			ACE_DEBUG ((LM_DEBUG, "\n\t\t------------------------------\n",  (char*) ostr->str().c_str() ) );  
		
			
			//just a line feed or waiting - ignore
			if( ( ostr->str().find ( "\n" ) != string::npos ) && ( ostr->str().size() == 2L ) )
			{
				Stream() << OutPutPrompt( false );	
				FlushStreamToConnector();

				break;
			}
			
			

			if( Command() != oecLogout ) 
			{
			
				if( ErrorPacket() != peNoPacketError )
				{
					FormatPacketError( ErrorPacket() );						 

					break;

				}
				
			}
			else
			{
				FlushStreamToConnector();

				return ( -1 );
			}									
			
			FlushStreamToConnector();

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
string OemoHandler::ClientAddr()
{
	ACE_INET_Addr addr;
	char addrstr[MAXHOSTNAMELEN + 1];

	peer().get_remote_addr( addr );
	addr.addr_to_string( addrstr , sizeof( addrstr ) );	

	string strTemp( addrstr );

	return ( strTemp );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::StreamOut( const std::string& str )
{
	Stream() << str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::StreamOut( const BytePtr& ptrBytes ,  unsigned long len )
{

	//

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::StreamOut( const std::string& str , unsigned long len )
{	
	Stream() << str.substr( 0 , len );	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::FlushImmediate( const std::string& atom )
{
	Stream() << atom;

	FlushStreamToConnector();

}


///////////////////////////////////////////////////////////////////////////////////////////////////
void  OemoHandler::FlushStreamToConnector()
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::FlushStreamToConnector" ) , __LINE__ );	

	if( Silent() == true )
	{
		Stream().str( "" );

		return;
	}

	ACE_Time_Value timeout( 2 );
	size_t bytes_sent;

	//Stream() << ends;

	string strOutput ( Stream().str() );

	peer().send_n( (char*) strOutput.c_str() , strOutput.size() , &timeout , &bytes_sent );	
	


	Stream().str( "" );	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void  OemoHandler::FlushBinaryStreamToConnector( unsigned char* u_buffer , unsigned long dwLen )
{
	if( Silent() == true )
	{
		Stream().str( "" );

		return;
	}

	ACE_Time_Value timeout( 5 );

	peer().send( (unsigned char*) u_buffer , dwLen , &timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void  OemoHandler::OutPutBoilerPlate( string strConcurrency )
{  

   
  Stream() << "* OK IMAP4rev1 Ready Oemo Imap Message Server " << ComputerName() << " Overtone Software Inc. All Rights Reserved  Ready";
  Stream() << crlf;
  Stream() << OutPutPrompt( false );
  
  FlushStreamToConnector(); 

}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoHandler::OutPutPrompt( bool bToken )
{

    ostringstream ostr;

	if( bToken == true )
	{
		ostr << Token() ;
		ostr << ends;
	}
	

	return ( ostr.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::ParseCommand( const  std::string& packet , 
									   MapOfParts& parts ,
									   unsigned short& usCommandParamCount )
{
	if( packet.empty() )
	{
		ErrorPacket( peInvalidSyntax );

		return;
	}
	
	string space( " " );
	string str( packet );
	
	ACE_DEBUG ((LM_DEBUG,
				  "\n\t------------------------Client Packet------------------------\n" ) ); 
	ACE_DEBUG ((LM_DEBUG,
				  "(%P|%t) %s %s\n" , (char*) Token().c_str() , (char*) str.c_str() ) );
	
	//get the token 
	string::size_type idx = str.find_first_of( space );
	if( idx != string::npos )
	{
		string strToken;
		strToken = str.substr( 0 , idx );
		if( strToken.size() > constMaxTokenLength )
		{
			ErrorPacket( peTokenTooLong );

			return;
		}

		parts.insert( make_pair( ppToken , strToken ) );
		Token( strToken );

		//truncate
		idx++;
		str = str.substr( idx );
		if( str.size() == 0 )
		{
			ErrorPacket( peInvalidSyntax );

			return;
		}
		
		//get the command
		string strCommand;
		idx = str.find_first_of( space );
		if( idx == string::npos )
		{
			//no params
			usCommandParamCount = 0;
			strCommand = str;
			rtrimlast( strCommand , "\n" );
			CommandStr( strCommand );			

			parts.insert( make_pair( ppCommand , strCommand ) );
		}
		else
		{			
			strCommand = str.substr( 0 , idx );

			
			CommandStr( strCommand );

			//truncate
			idx++;
			str = str.substr( idx );		

			parts.insert( make_pair( ppCommand , strCommand ) );
			chomp( str , "\n" );						
			parts.insert( make_pair( ppPayload , str ) );		
		}
	}
	else
	{
		ErrorPacket( peNoCommand );

		return;
	}

	ErrorPacket( peNoPacketError );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::Perform( const std::string& str )
{
	if( str.empty() )
	{
		ErrorPacket( peInvalidPayload );

		return;
	}

	unsigned short usCommands = 0;
	MapOfParts PartsMap;

	//parse the packet
	ParseCommand( str , PartsMap , usCommands );
	if( ErrorPacket() != peNoPacketError )
	{
		return;
	}

	//call the required function
	MapOfParts::iterator pIter = PartsMap.find( ppCommand );
	if( pIter != PartsMap.end() )
	{
		string strCommand( pIter->second );
		strCommand = lcase( strCommand );

		MapOfCommands::iterator c_iter = CommandMap().find( strCommand );
		if( c_iter  == CommandMap().end() )
		{			
			ErrorPacket( peUnrecognizedCommand );		

			return;
		}

		OemoHandler::ptrToFunction ptrFunc = c_iter->second;

		if( ptrFunc == NULL )
		{
			Stream() << OutPutPrompt( false );	
			Stream() << "NO no default action for this keyword" << crlf;
			FlushStreamToConnector();

			return;
		}	

		

		Uid( false );
		ContextPtr()->Uids( false );

		//params
		pIter = PartsMap.find( ppPayload );
		string strPayload;

		pIter != PartsMap.end() ? strPayload = pIter->second : strPayload = "";		
		int	lRetVal = ( this->*ptrFunc ) ( strPayload );	
		
		if( ( lRetVal != 0 ) || ( Command() == oecError ) )
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
void OemoHandler::chomp( std::string& str , const std::string& token )
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
void OemoHandler::gnaw( string& str , const string& token )
{
		string::size_type st;
	
		st = str.find( token , 0 );

		if( st != string::npos )
		{
			str.resize( st );
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string OemoHandler::ExtractQuotedString( const std::string& str )
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
void OemoHandler::rtrimlast( string& str , const string& token )
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
string OemoHandler::ltrim( const string &szToTrim, const string& szTrimChars )
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
string OemoHandler::rtrim( const string &szToTrim, const string& szTrimChars )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::MapCommands()
{
	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "login", &OemoHandler::OnLogin ) );	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "logout", &OemoHandler::OnLogout ) );	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "create", &OemoHandler::OnCreate ) );	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "select", &OemoHandler::OnSelect ) ); 
	CommandMap().insert( std::pair<string , ptrToFunction> ( "examine", &OemoHandler::OnExamine ) ); 
	CommandMap().insert( std::pair<string , ptrToFunction> ( "list", &OemoHandler::OnList ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "delete", &OemoHandler::OnDelete ) );	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "rename", &OemoHandler::OnRename ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "lsub", &OemoHandler::OnLsub ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "subscribe", &OemoHandler::OnSubscribe ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "unsubscribe", &OemoHandler::OnUnsubscribe ) );	
	CommandMap().insert( std::pair<string , ptrToFunction> ( "noop", &OemoHandler::OnNoop ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "check", &OemoHandler::OnCheck ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "capability", &OemoHandler::OnCapability ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "search", &OemoHandler::OnSearch ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "copy", &OemoHandler::OnCopy ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "fetch", &OemoHandler::OnFetch ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "store", &OemoHandler::OnStore ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "uid", &OemoHandler::OnUid ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "close", &OemoHandler::OnClose ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "expunge", &OemoHandler::OnExpunge ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "append", &OemoHandler::OnAppend ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "starttls", &OemoHandler::OnStarttls ) );
	CommandMap().insert( std::pair<string , ptrToFunction> ( "status", &OemoHandler::OnStatus ) );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string OemoHandler::lcase( const string& str )
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
string OemoHandler::ucase( const string& str )
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
void OemoHandler::OutputOkResponse( const string& ack )
{
	Stream() << Token() << " " << "OK " << ack << crlf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::OutputNoResponse( const std::string& nack )
{
	Stream() << Token() << " " << "NO " << nack << crlf;
	CommandStr( "" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::FormatPacketError( OemoImapProtocolImpl::PacketError pe )
{	

	Command( oecError );
	Stream() << OutPutPrompt();	

	Stream() << " NO ";

	if( pe == peNoToken ) Stream() << "no token supplied - expected";
	else if( pe == peTokenTooLong) Stream() << "token too long";
	else if( pe == peNoCommand ) Stream() << "no command supplied - expected";
	else if( pe == peCommandTooLong ) Stream() << "buffer to large";
	else if( pe == peTooManyParameters ) Stream() << "too many parameters supplied";
	else if( pe == peInvalidPayload ) Stream() << "invalid buffer";
	else if( pe == peInvalidSyntax ) Stream() << "invalid syntax";
	else if( pe == peUnrecognizedCommand ) Stream() << "command unrecognized";
	else if( pe == peSelectState ) Stream() << " a target must be selected before a query";
	else if( pe == peTooFewParameters ) Stream() << " too few parameters - more";
	else if( pe == peNoParametersExpectedGotSome ) Stream() << " no parameters expected - got some";
	else if( pe == peParametersExpectedGotNone ) Stream() << "parameters expected - got none";
	else if( pe == peFailure ) Stream() << "a server operation within this context failed";
	else if( pe == peInvalidObject ) Stream() << "object does not exist";
	else if( pe == peInvalidSpecification ) Stream() << "invalid specification";
	else if( pe == peNoSpecification ) Stream() << "no specification";
	else if( pe == peInvalidState ) Stream() << "invalid state";
	else if( pe == peAttributeNotSupported ) Stream() << "attribute not supported";	
	else if( pe == peNotAuthenticated ) Stream() << "please login";
	else if( pe == peAlreadyAuthenticated ) Stream() << "already logged in";
	else if( pe == peNotEnoughPrivileges ) Stream() << "not enough privileges";
	

	Stream() << crlf;
	Stream() << OutPutPrompt( false );		

	FlushStreamToConnector();	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OemoHandler::CheckConnectionState( OemoState state )
{
	bool bRet = false;

	CurrentState::iterator iter = m_currentState.find( state );
	iter == m_currentState.end() ? bRet = false  : bRet = true;	

	return ( bRet );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::UpdateConnectionState( OemoSetOp op , OemoState state )
{
	switch( op )
	{
		case osopAdd:
			m_currentState.insert( state );
			break;
		case osopDelete:
			m_currentState.erase( state );
			break;
		case osopClear:
			m_currentState.clear();
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//handlers
int OemoHandler::OnAppend( const std::string& params )
{	
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnAppend" ) , __LINE__ );	

	Command( oecAppend );

	ostringstream ostr;
	ostr << "* "  << CommandStr() << "%s" << crlf ;		
			
	try
	{
		bool bRet = ContextPtr()->OnAppend( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			Stream() << "+ waiting" << crlf;
			FlushStreamToConnector();

			//OutputOkResponse( "append completed" );
		}
		else
		{
			OutputNoResponse( "append failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}

	
	
	return ( 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
int OemoHandler::OnAuthenticate( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnAuthenticate" ) , __LINE__ );	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnCapability( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnCapability" ) , __LINE__ );	

	Command( oecCapability );

	ostringstream ostr;
	ostr << "* "  << CommandStr() << " " << "%s" << crlf ;		
			
	bool bRet = ContextPtr()->OnCapability( params , ostr.str() , Stream() );
    if( bRet == true ) 
	{
		OutputOkResponse( "capability completed" );
	}	
	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnCheck( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnCheck" ) , __LINE__ );	

	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnClose( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnClose" ) , __LINE__ );	

	Command( oecClose );

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}
	
	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = ContextPtr()->OnClose( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "close completed" );
		}
		else
		{
			OutputNoResponse( "close failed" );
		}
		m_currentState.erase( oesSelected );
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	
	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnCopy( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnCopy" ) , __LINE__ );	

	Command( oecCopy );

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}
	
	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = ContextPtr()->OnCopy( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "copy completed" );
		}
		else
		{
			OutputNoResponse( "copy failed" );
		}		
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnStatus( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnStatus" ) , __LINE__ );	

	Command( oecStatus );

	ostringstream ostr;
	ostr << "* "  << CommandStr() << " " << "%s (%s)" << crlf ;		
			
	try
	{
		bool bRet = ContextPtr()->OnStatus( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "status completed" );
		}
		else
		{
			OutputNoResponse( "status failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}
	
	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnCreate( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnCreate" ) , __LINE__ );	

	if( CheckConnectionState( oesAuthenticated ) == false )
	{
		ErrorPacket( peNotAuthenticated  );

		return ( 0 );
	}

	Command( oecCreate );
	
	ostringstream ostr;

	try
	{
		bool bRet = ContextPtr()->OnCreate( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "create completed" );
		}
		else
		{
			OutputNoResponse( "create failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	


	return ( 0 );

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnDelete( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnDelete" ) , __LINE__ );	

	if( CheckConnectionState( oesAuthenticated ) == false )
	{
		ErrorPacket( peNotAuthenticated  );

		return ( 0 );
	}

	Command( oecDelete );
	
	ostringstream ostr;

	try
	{
		bool bRet = ContextPtr()->OnDelete( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "delete completed" );
		}
		else
		{
			OutputNoResponse( "delete failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	return ( 0 );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnExamine( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnExamine" ) , __LINE__ );	

	Command( oecExamine );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = ContextPtr()->OnExamine( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "[read-only] examine completed" );
			UpdateConnectionState( osopAdd , oesSelected );
		}
		else
		{
			OutputNoResponse( "examine failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	return ( 0 );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnExpunge( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnExpunge" ) , __LINE__ );	

	Command( oecExpunge );

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		bool bRet = ContextPtr()->OnExpunge( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "expunge completed" );
		}
		else
		{
			OutputNoResponse( "expunge failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	
	

	return ( 0 );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnLogin( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnLogin" ) , __LINE__ );	

	Command( oecLogin );

	if( CheckConnectionState( oesAuthenticated ) == true )	
	{
		ErrorPacket( peAlreadyAuthenticated );

		return ( 0 );
	}
	
	ostringstream ostr;

	bool bRet = ContextPtr()->OnLogin( params , ostr.str() , Stream() );
    if( bRet == true ) 
	{
		UpdateConnectionState( osopAdd , oesAuthenticated );

		OutputOkResponse( "login completed" );
	}
	else
	{
		OutputNoResponse( "login failed" );
	}

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnLogout( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnLogout" ) , __LINE__ );	

	Command( oecLogout );
	
	ostringstream ostr;

	bool bRet = ContextPtr()->OnLogout( params , ostr.str() , Stream() );
    if( bRet == true ) 
	{
		OutputOkResponse( "logout completed" );
	}
	else
	{
		OutputNoResponse( "logout failed" );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnFetch( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnFetch" ) , __LINE__ );	

	Command( oecFetch );	

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	//internal format
	ostr << "nil";	

	try
	{
		
		bool bRet = ContextPtr()->OnFetch( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			if( Uid() == false )
			{				
				OutputOkResponse( "fetch completed" );
			}
		}
		else
		{
			OutputNoResponse( "fetch failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnList( const std::string& params )
{	
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnList" ) , __LINE__ );	

	Command( oecList );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "* "  << CommandStr() << " (%s) \"%s\" \"%s\"" << crlf ;		

	try
	{
		bool bRet = ContextPtr()->OnList( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "list completed" );
		}
		else
		{
			OutputNoResponse( "list failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnLsub( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnLsub" ) , __LINE__ );	

	Command( oecLsub );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "* "  << CommandStr() << " (%s) \"%s\" \"%s\"" << crlf ;		

	try
	{
		bool bRet = ContextPtr()->OnLsub( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "lsub completed" );
		}
		else
		{
			OutputNoResponse( "lsub failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnNoop( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnNoop" ) , __LINE__ );	

	Command( oecNoop );	

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{		
		return ( 0 );
	}	

	ostringstream ostr;

	try
	{
		ContextPtr()->OnNoop( params , ostr.str() , Stream() );		
	    OutputOkResponse( "noop completed" );
	
	}
	catch( OemoImapMgrError& )
	{		
		//
	}	

	return ( 0 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::Wait()
{
	Stream() << OutPutPrompt();
	Stream() << " "  << "WAIT" << crlf;

	FlushStreamToConnector();

    UpdateConnectionState( osopAdd , oesWait );
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnRename( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnRename" ) , __LINE__ );	

	Command( oecRename );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = ContextPtr()->OnRename( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "rename completed" );			
		}
		else
		{
			OutputNoResponse( "rename failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnSearch( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnSearch" ) , __LINE__ );	

	Command( oecSearch );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		
		bool bRet = ContextPtr()->OnSearch( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			if( Uid() == false )
			{	
				OutputOkResponse( "search completed" );			
			}
		}
		else
		{
			OutputNoResponse( "search failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnSelect( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnSelect" ) , __LINE__ );	

	Command( oecSelect );	

	if( CheckConnectionState( oesAuthenticated ) == false )	
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;

	ostr << "%s";		

	try
	{
		bool bRet = ContextPtr()->OnSelect( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "[read-write] select completed" );
			UpdateConnectionState( osopAdd , oesSelected );
		}
		else
		{
			OutputNoResponse( "select failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	

	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnStarttls( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnStarttls" ) , __LINE__ );	

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnStore( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnStore" ) , __LINE__ );	

	Command( oecStore );	

	if( ( CheckConnectionState( oesAuthenticated ) == false )
			|| ( CheckConnectionState( oesSelected ) == false ) )
	{
		ErrorPacket( peInvalidState );

		return ( 0 );
	}	

	ostringstream ostr;	

	try
	{
		bool bRet = ContextPtr()->OnStore( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "store completed" );
		}
		else
		{
			OutputNoResponse( "store failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnSubscribe( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnSubscribe" ) , __LINE__ );	

	if( CheckConnectionState( oesAuthenticated ) == false )
	{
		ErrorPacket( peNotAuthenticated  );

		return ( 0 );
	}

	Command( oecSubscribe );
	
	ostringstream ostr;

	try
	{
		bool bRet = ContextPtr()->OnSubscribe( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "subscribe completed" );
		}
		else
		{
			OutputNoResponse( "subscribe failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnUid( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnUid" ) , __LINE__ );	

	Uid( true );
	ContextPtr()->Uids( true );

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
			OnFetch( strParam );
		}
		else if( str.compare( "search" ) == 0L )
		{
			OnSearch( strParam );
		}		
	}
	

	OutputOkResponse( "UID completed" );

	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OemoHandler::OnUnsubscribe( const std::string& params )
{
	ACE_Trace _( ACE_TEXT( "OemoHandler::OnUnsubscribe" ) , __LINE__ );	

	if( CheckConnectionState( oesAuthenticated ) == false )
	{
		ErrorPacket( peNotAuthenticated  );

		return ( 0 );
	}

	Command( oecUnsubscribe );
	
	ostringstream ostr;

	try
	{
		bool bRet = ContextPtr()->OnUnsubscribe( params , ostr.str() , Stream() );
		if( bRet == true ) 
		{
			OutputOkResponse( "unsubscribe completed" );
		}
		else
		{
			OutputNoResponse( "unsubscribe failed" );
		}
	}
	catch( OemoImapMgrError& oe )
	{		
		OutputNoResponse( oe.what() );
	}	
	
	return ( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OemoHandler::ParseParams( const std::string& params , OemoImapProtocolImpl::StringContainer& strings )
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
				ErrorPacket( peInvalidSyntax );

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
				ErrorPacket( peInvalidSyntax );

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
				ErrorPacket( peInvalidSyntax );

				return;
			}		
			
		}
}


OemoImapMgrPtr OemoHandler::ContextPtr()
{
	
	return ( m_ptrImap );
	
}
