// OemoHandler.h,v 1.9 1999/02/03 01:26:56 schmidt Exp
#pragma warning( disable: 4786 4800 4365 )


#ifndef OEMO_HANDLER_H
#define OEMO_HANDLER_H

#include <windows.h>

#include "ace/Svc_Handler.h"
#include "ace/Date_Time.h"
#include "ace/Registry.h"


#include "OemoImapMgr.h"
#include "generic.h"

#include <sqlapi.h>

#include <string>
#include <sstream>
#include <sstream>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <iostream>


#include <cstddef>
#include <cstdlib>
#include "generic.h"

#include "OemoAcceptor.h"
#include "OemoThreadPool.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/SOCK_Stream.h"


//global event identifier
extern unsigned long g_dwFinished;

#define MAXHOSTNAMELEN 512

namespace OemoProtocolImpl
{
	
	class OemoAcceptor;	

	//enumerations
	enum PacketPart
	{
		ppToken = 0 ,
		ppCommand ,
		ppPayload ,
		ppUid
	};

	

	enum PacketError
	{
		peNoToken = 0 ,
		peTokenTooLong ,		
		peNoCommand ,
		peCommandTooLong ,
		peParametersExpectedGotNone ,
		peNoParametersExpectedGotSome ,
		peTooManyParameters , 
		peTooFewParameters ,
		peInvalidPayload ,
		peInvalidSyntax ,
		peUnrecognizedCommand ,
		peSelectState ,		
		peFailure , 
		peInvalidObject ,
		peInvalidState ,
		peInvalidSpecification , 
		peNoSpecification , 
		peAttributeNotSupported ,
		peNotAuthenticated , 
		peAlreadyAuthenticated ,
		peNotEnoughPrivileges ,
		peNoPacketError 			
	};

	enum OemoState
	{
		oesNonAuthenticated = 0 ,
		oesConnected ,
		oesAuthenticated ,
		oesSelected ,
		oesAdmin ,
		oesWait ,
		oesLogout
	};

	enum OemoCommand
	{
		oecLogin = 0 ,
		oecLogout ,
		oecAuthenticate ,		
		oecCreate ,
		oecSelect ,	
		oecList ,
		oecDelete,
		oecRename ,
		oecLsub ,
		oecStatus ,
		oecSubscribe ,
		oecUnsubscribe ,
		oecExamine ,
		oecNoop ,
		oecCheck ,
		oecCapability ,
		oecSearch ,
		oecCopy ,
		oecFetch ,
		oecStore ,
		oecUid ,
		oecClose ,
		oecExpunge ,
		oecAppend ,
		oecStarttls ,
		oecNoCommand ,
		oecError
	};

	enum OemoSetParam
	{
		ospPrompt = 0 		
	};	

	enum OemoSetOp
	{
		osopAdd ,
		osopDelete ,
		osopClear
	};


	//types	
	typedef unsigned char* BytePtr;	
	typedef std::multimap<PacketPart , std::string>  MapOfParts;
	typedef std::vector<std::string> StringContainer;
	typedef std::map<std::string , bool> RegistryDictionary;
	typedef std::multimap<std::string , std::string> ObjectDictionary;
	typedef std::map<std::string , std::string> StringDictionary;
	typedef std::map<OemoSetParam,bool> SetMap;	
	typedef std::set<OemoState> CurrentState;
	typedef std::vector<std::string> ImapStrings;
	

	//constant
	const char crlf[] = "\015\012";
	const char cr[] = "\015";
	const char lf[] = "\012";
	const unsigned short timer_id = 0;
	const unsigned long cmd_buffer_len = 65536L;
	const unsigned long max_client_timeout = 1800;//seconds	

	class OemoHandler : public ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
		public:

		  //types
		  typedef ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH> inherited;		  
		  typedef int (OemoHandler::*ptrToFunction)( const std::string& strParams );//function pointer prototype		      	 
		  typedef std::map<std::string , ptrToFunction> MapOfCommands;			 

		  //ctor
		  OemoHandler ( const ACE_Time_Value& max_inactivity = ACE_Time_Value ( max_client_timeout ) );
 
		  //actual destructor
		  void destroy ();
 
		  //open handler
		  int open (void *acceptor);

		  //close handler
		  int close (u_long flags = 0);

  
		  virtual int handle_close (ACE_HANDLE handle = ACE_INVALID_HANDLE,
									ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK);

		  int handle_timeout( const ACE_Time_Value& now , const void* act );



		private:

			//attributes
			std::ostringstream m_ostr;
			std::string m_strComputerName;
			std::string m_strToken;
			std::string m_strCommand;			
			OemoCommand m_currentCommand;
			PacketError m_packetError;
			ObjectDictionary m_objectDictionary;
			StringDictionary m_validObjects;
			std::string m_currentContext;
			bool m_bSilent;
			OemoAcceptor* m_oemoAcceptor;
			const ACE_Time_Value m_maximumTimeToWait;
			ACE_Time_Value m_lastActivityTime;
			long m_dwTimerToken;
			std::string m_strSpecify;			
			bool m_bServerInfo;
			bool m_bReflex;
			bool m_bPrompt;
			bool m_bUid;
			MapOfCommands m_commandMap;	  
			ObjectDictionary m_attributeMap;	
			SetMap m_setMap;
			std::string m_strCurrentModule;
			OvertoneOemoUtils::OemoImapMessageServerProtocolPtr ptr;
			CurrentState m_currentState;
			
			//imap manager
			OvertoneOemoUtils::OemoImapMgrPtr m_ptrImap;
			
			
			
		protected:
  
		  OemoThreadPool* thread_pool ();
		  ACE_thread_t creator_;

		  //adaptive communication environment overrides
		  int svc ();  
		  int handle_input (ACE_HANDLE handle);  

		  //thread queue callback
		  int process ( char *rdbuf, int rdbuf_len );

		  //accessors
		  inline std::ostringstream& Stream() { return ( m_ostr ); }
		  inline MapOfCommands& CommandMap() { return ( m_commandMap ); }

		  //helpers
		  void StreamOut( const std::string& str );
		  void StreamOut( const std::string& str , unsigned long len );
		  void StreamOut( const BytePtr& ptrBytes , unsigned long len );
		  void FlushStreamToConnector();
		  void FlushBinaryStreamToConnector( unsigned char* u_buffer , unsigned long dwLen );
		  int concurrency (); 
		  bool CheckConnectionState( OemoState state );
		  void UpdateConnectionState( OemoSetOp op , OemoState state );

		  std::string ClientAddr();
		  void OutPutBoilerPlate( std::string strConcurrency = "" );
		  std::string OutPutPrompt( bool bToken = true );
		  void OutputOkResponse( const std::string& ack );
		  void OutputNoResponse( const std::string& nack );
		  void ParseCommand( const std::string& packet , 
							 MapOfParts& parts ,
							 unsigned short& usCommandParamCount );
		  void ParseParams( const std::string& params , StringContainer& strings );
		  void Perform( const std::string& str );
		  void MapCommands();
		  
		  
		  OvertoneOemoUtils::OemoImapMgrPtr ContextPtr(); 
		  
		  void Wait();

		  //string helpers

		  //find the first instance of token and truncate string from there
		  void gnaw( std::string& str , const std::string& token = " ");

		  //truncate string& at token and return truncation
		  void chomp( std::string& str , const std::string& token = cr );		  

		  //trim 
		  void rtrimlast( std::string& str , const std::string& token = " " );
		  std::string rtrim( const std::string &szToTrim, const std::string& szTrimChars );
		  std::string ltrim( const std::string &szToTrim, const std::string& szTrimChars );
		  std::string ExtractQuotedString( const std::string& str );

		  void FormatPacketError( PacketError pe );		  

		  //case
		  std::string lcase( const std::string& str );
		  std::string ucase( const std::string& str );
 
		  //hidden destructor
		  ~OemoHandler (void);

		public:

			//immutable
			static const unsigned long constCommandBufferLength;
			static const unsigned short constMaxTokenLength;
			static const unsigned short constMaxLength;
			static const std::string constObjectAttributes;
			static const std::string constServerName;

			//accessors
			bool Uid() const throw() { return ( m_bUid ); }
			inline std::string ComputerName() const throw() { return ( m_strComputerName ); }
			inline std::string Token() const throw()  { return ( m_strToken ); }			
			inline std::string CommandStr() { return ( ucase( m_strCommand ) ); }
			inline OemoCommand Command() const throw()  { return ( m_currentCommand ); }
			inline PacketError ErrorPacket() const throw() { return ( m_packetError ); }
			inline ObjectDictionary& RegObjects() { return ( m_objectDictionary ); }
			inline StringDictionary& ValidObjects() { return ( m_validObjects ); }
			inline std::string CurrentContext()  const throw() { return ( m_currentContext ); }
			inline bool Silent() { return ( m_bSilent ); }
			inline OemoAcceptor* Acceptor() { return ( m_oemoAcceptor ); }	
			inline std::string Specify()  { return ( m_strSpecify ); }							
			inline std::string ModuleString()  { return ( m_strCurrentModule ); }
			inline bool ServerInfo() { return ( m_bServerInfo ); }
			inline bool Reflex() { return ( m_bReflex ); }
			inline bool Prompt() { return ( m_bPrompt ); }				


			//mutators
			inline void Token( const std::string& str_token ) { m_strToken = str_token; }
			inline void CommandStr( const std::string& str_command ) { m_strCommand = str_command; }
			inline void Command( OemoCommand command ) { m_currentCommand = command; }
			inline void ErrorPacket( PacketError error ) { m_packetError = error; }
			inline void CurrentContext( const std::string& context ) { m_currentContext = context; }
			inline void Silent( bool bSilent ) { m_bSilent = bSilent; }
			inline void Acceptor ( OemoAcceptor *acceptor ) { m_oemoAcceptor = acceptor; }		
			inline void Specify( const std::string& spec ) { m_strSpecify = spec; }	
			inline void ModuleString( const std::string& module ) { m_strCurrentModule = module; }			
			inline void Reflex( bool bReflex ) { m_bReflex = bReflex; }	
			inline void Prompt( bool bPrompt ) { m_bPrompt = bPrompt; }	
			inline void Uid( bool uid ) { m_bUid = uid; }
		
			//handlers
			int OnLogin( const std::string& params );
			int OnLogout( const std::string& params );
			int OnAuthenticate( const std::string& params );
			int OnList( const std::string& params );
			int OnCreate( const std::string& params );
			int OnSelect( const std::string& params );
			int OnDelete( const std::string& params );
			int OnRename( const std::string& params );		
			int OnLsub( const std::string& params );
			int OnSubscribe( const std::string& params );				
			int OnUnsubscribe( const std::string& params );				
			int OnExamine( const std::string& params );				
			int OnNoop( const std::string& params );							
			int OnCheck( const std::string& params );				
			int OnCapability( const std::string& params );				
			int OnSearch( const std::string& params );				
			int OnCopy( const std::string& params );	
			int OnFetch( const std::string& params );				
			int OnStore( const std::string& params );				
			int OnUid( const std::string& params );				
			int OnClose( const std::string& params );				
			int OnExpunge( const std::string& params );				
			int OnAppend( const std::string& params );	
			int OnStatus( const std::string& params );
			int OnStarttls( const std::string& params );	

			void FlushImmediate( const std::string& atom );

			
		
	};
}

namespace opi = OemoProtocolImpl;

#endif /* OEMO_HANDLER_H */
