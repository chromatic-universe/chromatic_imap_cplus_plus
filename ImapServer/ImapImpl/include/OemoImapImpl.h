// 12 Aug 2007 William K. Johnson
#pragma warning( disable: 4786 4800 4365 )

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#ifndef OEMO_IMAP_IMPL_H
#define OEMO_IMAP_IMPL_H


#include "ace/Svc_Handler.h"
#include "ace/Date_Time.h"
#include "ace/Registry.h"

#include "ace/SOCK_Acceptor.h"

#include "OemoThreadPool.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <set>
#include <vector>
#include <comdef.h>
#include <cstddef>
#include <cstdlib>


#include "thread_pool.h"
#include "OemoProtocolData.h"

#include "OemoImapMessageServerProtocol.h"
#include "OemoImapMessageManip.h"
#include "OemoDbInterfaceImpl.h"

#include "generic.h"

#include <sqlapi.h>
#include "CkMime.h"
#include "CkXml.h"
#include "CkString.h"
#include "CkEmail.h"
#include "CkMailMan.h"
#include <CkCrypt2.h>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/detail/case_conv.hpp>
#include "OemoImapStrings.h"


#import "ISYSCom.dll" no_namespace named_guids



//global event identifier
extern unsigned long g_dwFinished;

#define MAXHOSTNAMELEN 512

namespace OemoImapProtocolImpl
{
				//forward
				class OemoAcceptor;	
				class OemoMailBox;
				class OemoImapMessage;	
				class OemoImapMgr;
				class OemoHandler;
				class OemoImapSearchCriteria;
				class OemoImapIsysSearchResolver;

				//enumerations
				enum concurrency_t
				{
					single_threaded_,
					thread_per_connection_,
					thread_pool_
				};
				
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

				enum ImapMgrErrorLevel
				{
					lmlCritical = 0 ,
					lmlLogic ,
					lmlInformation ,
					lmlDebug ,
					lmlNoError
				};

				enum OemoMailboxAttribute
				{
					omaNoinferiors ,
					omaNoselect ,
					omaMarked , 
					omaUnmarked 
				};

				enum OemoMailboxResult
				{
					omrNotFound ,
					omrAlreadyExists ,
					omrSuccess ,
					omrAccessDenied
				};

				enum OemoMailMessageFlag 
				{
					omfAnswered ,
					omfDeleted ,
					omfDraft ,
					omfFlagged ,
					omfRecent , 
					omfSeen
				};

				enum OemoFlagsMethod
				{
					ofmAdd ,
					ofmRemove ,
					ofmReplace
				};

				enum OemoLogicOperations
				{
					olpAnd ,
					olpNot ,
					olpOr 		
				};

				enum OemoImapSearchErrorLevel
				{
					islCritical = 0 ,
					islLogic ,
					islInformation ,
					islDebug ,
					islNoError
				};

				enum OemoSearchKeys
				{
					oskAll ,
					oskText ,
					oskUnsupportedKey
				};

			

				typedef ACE_Acceptor <OemoHandler, ACE_SOCK_ACCEPTOR> OemoAcceptor_Base;


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
				typedef std::vector<std::string> vtorString;
				typedef std::multimap<std::string , std::string> mapString;		
				typedef void* VoidPtr;
				typedef std::map<std::string,std::string> RegParams;
				typedef std::set<OemoMailboxAttribute> OemoMailBoxAttributes;
				typedef std::vector<OemoMailBox> OemoMailBoxList;
				typedef std::vector<std::string> ImapStrings;
				typedef std::set<OemoMailMessageFlag> OemoMailMessageFlags;
				typedef std::map<std::string,OemoImapMessage> ImapServerMessages;
				typedef ImapServerMessages* ImapServerMessagesPtr;
				typedef std::vector<int> vInt;
				typedef HMODULE DllHandle;
				typedef std::pair<std::string,std::string> OemoImapFetchRequest;
				typedef std::vector<OemoImapFetchRequest> OemoImapFetchRequests;
				typedef std::map<unsigned long,std::string> ImapFetchStrings;
				typedef std::map<std::string,unsigned long> ImapUrlToUid;
				typedef std::map<std::string,std::string> ImapHeaderFields;
				typedef std::set<std::string> ImapHeaderSet;				
				typedef OemoImapMgr* OemoImapMgrPtr;
				typedef OemoHandler* OemoHandlerPtr;
				typedef OemoImapSearchCriteria* OemoImapSearchCriteriaPtr;					
				typedef OemoImapIsysSearchResolver* OemoImapIsysSearchResolverPtr;
				//atomic key with operator
				//	i.e 'not seen'
				typedef std::multimap<OemoLogicOperations , std::string> OemoAtomicSearchKey;
				typedef std::vector<OemoAtomicSearchKey> OemoAtomicSearches;
				//	=>=> parenthetical search key
				//		i.e. 'not (seen from "snaps")
				typedef std::pair<OemoLogicOperations , std::string> OemoParentheticalSearchKey;	
				typedef std::vector<OemoParentheticalSearchKey> OemoSearches;
				typedef std::set<unsigned long> MessageNumbers;
				typedef std::set<OemoSearchKeys> SearchKeys;
				typedef void* IsysEnginePtr;

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
						OemoImapMessageServerProtocolPtr ptr;
						CurrentState m_currentState;
						
						//imap manager
						OemoImapMgrPtr m_ptrImap;
						
						
						
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
					  
					  
					  OemoImapMgrPtr ContextPtr(); 
					  
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

				//exception
				class OemoImapMgrError : public std::logic_error
				{
					protected:
						//attributes
						ImapMgrErrorLevel m_lError;			

					public:
						//accessors
						inline ImapMgrErrorLevel ErrorLevel() const throw()	{ return ( m_lError ); }

						//mutators
						inline void ErrorLevel( ImapMgrErrorLevel level ) { m_lError = level; }
					
						//ctor
						OemoImapMgrError( const std::string& what_arg ,
											  ImapMgrErrorLevel error_level = lmlNoError ) :
											  logic_error( what_arg ) ,
											  m_lError( error_level )
						{
							//
						}
				};




				//types
				typedef OemoImapMgrError* OemoLdapMgrErrorPtr;

				class OemoMailBox
				{
					public:

						OemoMailBox() 
						{
							//
						}

						~OemoMailBox()
						{
							//
						}
					
					private:

						//attributes
						std::string m_strName;
						OemoMailBoxAttributes m_attributes;
						bool m_bSubscribed;

					public:

						//accessors
						std::string Name() const throw() { return ( m_strName ); }
						OemoMailBoxAttributes& Attributes() { return ( m_attributes ); }
						bool Subscribed() const throw() { return ( m_bSubscribed ); }

						//mutators
						void Name( const std::string& name ) { m_strName = name; }
						void Attributes( OemoMailBoxAttributes attributes ) { m_attributes = attributes; }
						void Subscribed( bool subscribed ) { m_bSubscribed = subscribed; }
						
				}; 

				//classes
				class OemoImapMessage
				{
					public:

					  OemoImapMessage()  : m_dwUid( 0L ) , m_dwSize( 0L )
					  {
						//
					  }

					  virtual ~OemoImapMessage()
					  {
						//
					  }

					private:

					  long m_dwUid;
					  long m_dwSize;
					  std::string m_strUrl;
					  std::string m_strFileUrl;
					  std::string m_strOffset;
					  std::string m_strServer;
					  OemoMailMessageFlags m_flags;

					public:

					   //accessors
					   long Uid() const throw() { return ( m_dwUid ); }
					   long Size() const throw() { return ( m_dwSize ); }
					   std::string Url() const throw() { return ( m_strUrl ); }
					   std::string FileUrl() const throw() { return ( m_strFileUrl ); }
					   std::string Offset() const throw() { return ( m_strOffset ); }
					   std::string Server() const throw() { return ( m_strServer ); }
					   OemoMailMessageFlags Flags() const throw() { return ( m_flags ); }

					   //mutators
					   inline void Uid( const long uid ) { m_dwUid = uid; }
					   inline void Size( const long size ) { m_dwSize = size; }
					   inline void Url( const std::string& url ) { m_strUrl = url; }
					   inline void FileUrl( const std::string& url ) { m_strFileUrl = url; }
					   inline void Offset( const std::string& offset ) { m_strOffset = offset; }
					   inline void Server( const std::string& server ) { m_strServer = server; }
					   inline void Flags( const OemoMailMessageFlags flags ) { m_flags = flags; }					   

				};

				class OemoMailBoxInfo
				{
					public:
						 
						OemoMailBoxInfo() : m_dwRecentMessages( 0L ) ,
											m_dwUnseenMessages( 0L ) ,
											m_dwFirstUnseen( 0L ) ,
											m_dwExistsMessages( 0L ) ,
											m_bReadOnly( false ) 
						{
							//
						}

						virtual ~OemoMailBoxInfo() {}

					private:
						
						//attributes
						std::string m_strName;
						long m_dwRecentMessages;
						long m_dwFirstUnseen;
						long m_dwUnseenMessages;
						long m_dwExistsMessages;
						bool m_bReadOnly;		
						OemoMailMessageFlags m_flags;
						OemoMailMessageFlags m_changeable_flags;
						std::string m_strUidValidity;
						std::string m_strUidNext;
						

					protected:

						//

					public:
						
						//accessors
						std::string Name() const throw() { return ( m_strName ); }
						long Recent() const throw() { return ( m_dwRecentMessages ); }
						long Unseen() const throw() { return ( m_dwUnseenMessages ); }
						long FirstUnseen() const throw() { return ( m_dwFirstUnseen ); }
						long Exists() const throw() { return ( m_dwExistsMessages ); }
						bool ReadOnly() const throw() { return ( m_bReadOnly ); }			
						OemoMailMessageFlags Flags() const throw() { return ( m_flags ); }
						OemoMailMessageFlags  ChangeableFlags() const throw() { return ( m_changeable_flags ); }
						std::string UidValidity() const throw() { return ( m_strUidValidity ); }
						std::string UidNext() const throw() { return ( m_strUidNext ); }

						//mutators
						inline void Name( const std::string& name ) { m_strName = name; }
						inline void Recent( const long recent ) { m_dwRecentMessages = recent; }
						inline void Unseen( const long unseen ) { m_dwUnseenMessages = unseen; } 
						inline void FirstUnseen( const long first ) { m_dwFirstUnseen = first; }
						inline void Exists( const long exists ) { m_dwExistsMessages = exists; }
						inline void Flags( const OemoMailMessageFlags flags ) { m_flags = flags; }
						inline void ChangeableFlags( const OemoMailMessageFlags changeable_flags ) { m_changeable_flags = changeable_flags; }
						inline void UidValidity( const std::string& uidvalidity ) { m_strUidValidity = uidvalidity; }
						inline void UidNext( const std::string& uidNext ) { m_strUidNext = uidNext; }
						inline void ReadOnly( bool bReadOnly ) { m_bReadOnly = bReadOnly; }			

						//services
						void clear()
						{
								m_strName = "";
								m_dwRecentMessages = 0L;
								m_dwFirstUnseen = 0L;
								m_dwUnseenMessages = 0L;
								m_dwExistsMessages = 0L;
								m_bReadOnly = false;
								m_flags.clear();
								m_changeable_flags.clear();
								m_strUidValidity = "";
								m_strUidNext = "";
						}

				};


				class OemoAppendRecord
				{
					public:

						OemoAppendRecord() : m_dwSize( 0L ) 							     
						{
							//
						}

						~OemoAppendRecord() {}

					private:

						unsigned long m_dwSize;
						std::string m_strMbx;
						OemoMailMessageFlags m_flags;
						std::ostringstream m_ostr;

					public:

						//accessors
						unsigned long Size() const throw() { return ( m_dwSize ); }
						std::string Mbx() const throw() { return ( m_strMbx ); }
						OemoMailMessageFlags Flags() { return ( m_flags ); }
						std::ostringstream& Stream() { return ( m_ostr ); }

						//mutators
						inline void Size( const unsigned long sz ) { m_dwSize = sz; }
						inline void Mbx( const std::string& mbx ) { m_strMbx = mbx; }
						inline void Flags( const OemoMailMessageFlags flags ) { m_flags = flags; }
						

						//services
						void clear()
						{
							m_dwSize = 0L;
							m_strMbx = "";
							m_flags.clear();
						}
				};    

				class OemoImapMgr : public OemoImapMessageServerProtocol
				{		
					friend OemoImapIsysSearchResolver;

					public:

						OemoImapMgr( const std::string& dbserver ,
									 const std::string& proxystore ,
									 OemoHandlerPtr imap_handler = NULL );		
						virtual ~OemoImapMgr();

					private:

						//attributes
						std::ostream* m_ptrStream;				
						bool m_bConnected;
						bool m_bUseUid;
						bool m_bCloseExpunge;
						std::string m_strServer;
						std::string m_strDbServer;
						std::string m_strDsn;
						std::string m_strUser;
						std::string m_strProxyStore;
						std::string m_strUserDomain;
						std::auto_ptr<ImapServerMessages> m_ptrMessages;
						OemoMailBoxInfo m_mbxInfo;
						OemoAppendRecord m_appendRecord;
						DllHandle m_dynHandle;
						DllHandle m_dynUtilsHandle;
						ImapFetchStrings m_fetchMap;
						unsigned long m_dwMinUid;
						unsigned long m_dwMaxUid;

						//no copy
						OemoImapMgr( const OemoImapMgr& theOemoImappMgr );
						
					protected:

						//helpers
						BOOL MakeDirectory(const std::string& newdir );								
						std::string FormatComError( _com_error* ptrErr );											
						std::pair<std::string,std::string> PairFromTuple( const std::string& tuple );			
						bool ReadParams();		
					
						//string manip
						std::string lcase( const std::string& str );
						std::string ucase( const std::string& str );
						std::string rtrim( const std::string &szToTrim, const std::string& szTrimChars );
						std::string ltrim( const std::string &szToTrim, const std::string& szTrimChars );

						//imap helpers
						std::string CheckFolderName( const std::string& folder );
						std::string FileToHttpUrl( const std::string& fileurl , const std::string& server );
						std::string HttpToFileUrl( const std::string& str );
						HRESULT GetDomainName( std::string& strDomainName );
						void StatusResponse( OemoMailBoxInfo info , std::ostream& ostr , ImapStrings vStr );
						void SelectResponse( OemoMailBoxInfo info , std::ostream& ostr , bool bReadOnly );
						std::string FetchResponse( const std::string& msg_id , const std::string& payload , std::ostream& ostr );
						std::string SearchResponse( const vInt& vintsr );
						void StoreResponse( const std::string& msg_id , const std::string& payload , std::ostream& ostr );
						std::string ExtractQuotedString( const std::string& str );
						std::string MbxAttributesToStr( OemoMailBoxAttributes omba );
						std::string MbxFlagsToStr( OemoMailMessageFlags  ommf );
						void StrToMbxFlags( const std::string& flag_string , OemoMailMessageFlags&  ommf );	
						std::string TimeZoneInfo();
						std::string InternalDate( const std::string& filename );
						std::string base64_encode( unsigned char const* bytes_to_encode , unsigned int in_len );
						std::string base64_decode(std::string const& encoded_string);
						std::string FetchEnvelope( const std::string& message_path );
						std::string FetchBodyStructure( const std::string& message_path );
						std::string FetchRfc822( const std::string& message_path , const std::string& command );
						std::string FetchMime( const std::string& message_path , const std::string& mime_part );
						ImapHeaderFields FetchHeaderFieldsMime( const std::string& message_path , const std::string& params );
						std::string FetchHeaderFields(  const std::string& message_path , const std::string& params );
						void ParseHeaderFieldParams( const std::string& source , ImapHeaderSet& fields_set );
						std::string FetchHeader( const std::string& message_path , const std::string& params , bool bIsPart = false );
						void StoreFlags( unsigned long msg_id , OemoMailMessageFlags flags ,
										 OemoFlagsMethod  method , std::ostream& ostr ,
										 bool bSilent = false );
						std::string MessageTag( const std::string& fileurlstr  );


						//internal services
						void EnumMailboxList( OemoMailBoxList& mbxs ,
											  const std::string& username ,
											  const std::string& reference ,
											  const std::string& criteria ,
											  bool subscribed = false );
						void CollectMailboxesByDb( const std::string& reg ,
												   const std::string& username ,
												   OemoMailBoxList& obl ,
												   bool subscribed = false );
						bool IsSubscribedByDb( const std::string& folder , const std::string& nickname );
						OemoMailboxResult FolderExists( const std::string& folder );   
						void FillMessageListByDb( const std::string& folder );
						void UpdateMailBoxInfoByDb( const std::string& folder , bool bReadOnly = false );
						void UpdateMailBoxInfoByDb( const std::string& folder , OemoMailBoxInfo& info );
						std::string FetchMessage(  unsigned long msg_id , std::ostream& ostr , OemoImapFetchRequests& requests );
						void ParseFetchRequestList( const std::string& request , OemoImapFetchRequests& requests );
						void SetMessageFlagsByDb( const std::string& url , OemoMailMessageFlags flags );


						//attributes
						RegParams m_regParams;
						SAConnection m_db;
						OemoHandlerPtr m_ptrHandler;

					public:

						//accessors-inspectors												
						inline std::ostream* OutputStream() { if( m_ptrStream != NULL ) return ( m_ptrStream ); else return ( NULL ); };					
						std::string ServerString() const throw() { return  (m_strServer);}					
						std::string DbServerString() const throw() { return  ( m_strDbServer );}
						std::string Dsn() const throw() { return  ( m_strDsn );}
						std::string User() const throw() { return  ( m_strUser );}
						std::string ProxyStore() const throw() { return  ( m_strProxyStore );}
						bool IsConnected()const throw() { return m_bConnected;};			
						bool CloseExpunge()const throw() { return m_bCloseExpunge;};			
						ImapServerMessagesPtr Messages() { return ( m_ptrMessages.get() ); }
						inline RegParams& Params() { return ( m_regParams ); }
						OemoMailBoxInfo& MbxInfo() { return ( m_mbxInfo ); }
						OemoAppendRecord& AppendRec() { return ( m_appendRecord ); }
						bool Uids() const throw() { return ( m_bUseUid ); }
						DllHandle StrTblHandle() { return ( m_dynHandle ); }			
						DllHandle UtilsHandle() { return ( m_dynUtilsHandle ); }
						unsigned long MaxUid() const throw() { return ( m_dwMaxUid ); }
						unsigned long MinUid() const throw() { return ( m_dwMinUid ); }
						OemoHandlerPtr  ImapHandler() { return ( m_ptrHandler ); }
						std::string UserDomain() const throw() { return  ( m_strUserDomain );}
						
						//mutators								
						inline void OutputStream( std::ostream* ptrStream ) { m_ptrStream  = ptrStream; };		
						virtual void DbServerString( const std::string& db ) { m_strDbServer = db; }	
						virtual void ProxyStore( const std::string& store ) { m_strProxyStore = store; }	
						virtual void Dsn( const std::string& dsn ) { m_strDsn = dsn; }			
						virtual void User( const std::string& user ) { m_strUser = user; }	
						virtual void UserDomain( const std::string& user_domain ) { m_strUserDomain = user_domain; }	
						inline void Uids( bool bUid ) { m_bUseUid = bUid; }
						inline void CloseExpunge( bool bClose ) { m_bCloseExpunge = bClose; }
						inline void MaxUid( unsigned long dw ) { m_dwMaxUid = dw; }
						inline void MinUid( unsigned long dw ) { m_dwMinUid = dw; }			

						//services		
						bool OpenServer();
						void DeInit();
						std::string _R( unsigned long dwIdx );
					
						
						//overrides	
						virtual bool OnLogin( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnLogout( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnCapability( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnAuthenticate( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnList( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnCreate( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnSelect( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnDelete( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnRename( const std::string& params , const std::string& fmt , std::ostream& ostr );		
						virtual bool OnLsub( const std::string& params , const std::string& fmt , std::ostream& ostr );
						virtual bool OnStatus( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool OnSubscribe( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnUnsubscribe( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnExamine( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnNoop( const std::string& params , const std::string& fmt , std::ostream& ostr );							
						virtual bool OnCheck( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnSearch( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnCopy( const std::string& params , const std::string& fmt , std::ostream& ostr );	
						virtual bool OnFetch( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnStore( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnUid( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnClose( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnExpunge( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnAppend( const std::string& params , const std::string& fmt , std::ostream& ostr );				
						virtual bool OnStarttls( const std::string& params , const std::string& fmt , std::ostream& ostr );						
						
						//immutable
						static const std::string constCryptUnlockCode;
						static const std::string constMimeUnlockCode;
						static const std::string constMailUnlockCode;
						static const std::string crlf;	
						static const std::string space;	
						static const std::string asterisk;
						static const std::string right_paren;	
						static const std::string left_paren;
						static const std::string left_bracket;
						static const std::string right_bracket;
						static const std::string left_brace;
						static const std::string right_brace;
						static const std::string quote;
						static const unsigned long one;
						static const unsigned long two;
						static const unsigned long short_call;	

						//function prototypes
						typedef unsigned long (*proc_no_args_return_dword) ();
						typedef void (*proc_char_ptr_args_return_void) ( char* );
						typedef unsigned long (*proc_char_ptr_in_char_ptr_out_args_return_dword ) ( const char* , char* , unsigned long );

						
				};

				class OemoImapSearchError : public std::logic_error
				{
					protected:

						//attributes
						OemoImapSearchErrorLevel m_lError;			

					public:

						//accessors
						inline OemoImapSearchErrorLevel ErrorLevel() const throw()	{ return ( m_lError ); }

						//mutators
						inline void ErrorLevel( OemoImapSearchErrorLevel level ) { m_lError = level; }
					
						//ctor
						OemoImapSearchError(  const std::string& what_arg ,
											  OemoImapSearchErrorLevel error_level = islNoError ) :
											  logic_error( what_arg ) ,
											  m_lError( error_level )
						{
							//
						}
				};

				class OemoImapSearchCriteria
				{
					public:

						//ctor
						OemoImapSearchCriteria( const std::string& criteria ,											
												const std::string& message_set = "nil" ,
												bool bUid = false ,										
												bool bPropagateExceptions = true );

						//dtor
						virtual ~OemoImapSearchCriteria();

					private:
							
						//attributes
						std::string m_messageSet;
						std::string m_criteria;
						OemoSearches m_searches;				
						bool m_bUid;				
						bool m_bPropagateExceptions;			

					protected:

						//helpers
						std::string ExtractQuotedString( const std::string& str );
						OemoSearches ExtractParentheticalKeys( const std::string& str );
						std::string SearchKeyToStr( const OemoSearchKeys key );
						OemoSearchKeys StrToSearchKey( const std::string& str );
						inline void Clear()
						{
							m_messageSet = "nil";
							m_criteria = "";
							m_searches.clear();				
						}			


					public:

						//accessors-inspectors
						inline std::string MessageSet() const throw() { return ( m_messageSet ); }
						inline std::string RawCriteria() const throw() { return ( m_criteria ); }
						inline OemoSearches& Searches() { return ( m_searches ); }
						inline bool Uid() const throw() { return ( m_bUid ); } 
						


						//mutators
						inline void MessageSet( const std::string& message_set )  { m_messageSet = message_set; }
						inline void RawCriteria( const std::string& raw_criteria )  { m_criteria = raw_criteria; }
						inline void Searches( const OemoSearches& searches ) { m_searches = searches; }
						inline void Uid( bool uid ) { m_bUid = uid; } 
						

						//services
						void DeconstructCriteria( OemoImapIsysSearchResolverPtr resolver );
						
						//immutable
						static const std::string constMessageSetAll;
								

				};	

				class OemoImapSearchResolverBase
				{
					public:

						//ctors
						OemoImapSearchResolverBase() {};						
						//dtor
						virtual ~OemoImapSearchResolverBase() {};

						//services
						virtual void ResolveParentheticalKey( OemoParentheticalSearchKey key , vInt vints ) = 0;
						virtual void ResolveAtomicKey( OemoAtomicSearchKey key ) = 0;						

				};

				class OemoImapIsysSearchResolver : public OemoImapSearchResolverBase
				{
					public:

						//ctors
						OemoImapIsysSearchResolver() {};						
						OemoImapIsysSearchResolver( OemoImapMgrPtr ptrImapMgr ,
													const std::string& isys_path ,
													OemoImapSearchCriteriaPtr criteria ,
													const std::string& document_path ,													
													const std::string& search_domain = "nil" );
						
						//dtor
						virtual ~OemoImapIsysSearchResolver();

					private:

						//attributes
						MessageNumbers  m_MessageSet; 
						ImapUrlToUid m_dictionary;						

					protected:

						//attributes
						OemoImapSearchCriteriaPtr m_ptrCriteria;
						OemoImapMgrPtr m_ptrImap;
						std::string m_strIsysPath;
						std::string m_strDocumentPath;
						std::string m_strSearchDomain;
						vInt m_vints;

						IISYSEnginePtr m_ptrEngine;
						bool m_bInitialized;

						//helpers
						bool InitializeIsysEngine();
						#ifdef WIN32
						void dump_com_error( _com_error &e );
						#endif
						
					public:	

						//accessors-inspectors
						inline OemoImapMgrPtr ImapMgr() { return ( m_ptrImap ); };
						inline MessageNumbers& MessageSet() { return ( m_MessageSet ); }
						inline OemoImapSearchCriteriaPtr Criteria() { return ( m_ptrCriteria ); }
						inline std::string IsysPath() const throw() { return ( m_strIsysPath ); }
						inline std::string DocumentPath() const throw() { return ( m_strDocumentPath ); }
						inline std::string SearchDomain() const throw() { return ( m_strSearchDomain ); }
						inline IISYSEnginePtr Isys() { return ( m_ptrEngine ); }
						inline bool Initialized() { return ( m_bInitialized ); }
						inline ImapUrlToUid& UrlToUid() { return ( m_dictionary ); }
						inline vInt SearchResults() { return ( m_vints ); }


						//mutators
						inline void ImapMgr( OemoImapMgrPtr ptrImap ) { m_ptrImap = ptrImap; }
						inline void Criteria( OemoImapSearchCriteriaPtr ptrCriteria ) { m_ptrCriteria = ptrCriteria; }
						inline void IsysPath( const std::string& isys_path ) { m_strIsysPath = isys_path; }
						inline void DocumentPath( const std::string& document_path ) { m_strDocumentPath = document_path; }
						inline void SearchDomain( const std::string& search_domain ) { m_strSearchDomain = search_domain; }

						
						//services
						virtual void ResolveParentheticalKey( OemoParentheticalSearchKey key , OemoImapMessageManip::vInt vints );
						virtual void ResolveParentheticalKey( OemoParentheticalSearchKey key , const std::string& top_level_message_set );
						virtual void ResolveAtomicKey( OemoAtomicSearchKey key );
						vInt MessageNumbersToUids( const vInt vints );
						vInt UidsToMessageNumbers( const vInt vints );


						//immutable
						static const std::string const_strKey;
						

				};				
			

				//class OemoAcceptor
				class OemoAcceptor : public OemoAcceptor_Base
				{
					public:

					  typedef OemoAcceptor_Base inherited;
			  
					  OemoAcceptor (int concurrency = thread_pool_);

					  OemoAcceptor (OemoThreadPool &thread_pool);

					  ~OemoAcceptor (void);

					  int open (const ACE_INET_Addr &addr,
								ACE_Reactor *reactor,
								OemoProtocolDataPtr instance_data ,
								int pool_size = OemoThreadPool::default_pool_size_ );

					  int close (void);		 

					protected:

					  int concurrency_;

					  OemoThreadPool private_thread_pool_;

					  OemoThreadPool &the_thread_pool_;

					  OemoProtocolDataPtr m_instanceData;

					public:	

					  //accessors
					  int concurrency (void)
					  {
						return this->concurrency_;
					  }

					  OemoThreadPool *thread_pool (void)
					  {
						return &this->the_thread_pool_;
					  }

					  int thread_pool_is_private (void)
					  {
						return &the_thread_pool_ == &private_thread_pool_;
					  }

					  OemoProtocolDataPtr Data() 
					  {
						  return ( m_instanceData );
					  }

					  //mutators
					  void thread_pool( ACE_Thread_Manager* thrd_mgr )
					  {
						  ACE_Task<ACE_MT_SYNCH> ( thr_mgr );
					  }

					  inline void Data( OemoProtocolDataPtr data )
					  {
						  m_instanceData = data;
					  }

				}; 

				//immutable
				static const std::string constIsysKey = "DTZW2 UBMFR 6B5CH 6DWLC J57P9 NRXLA";
				
}

namespace oipi = OemoImapProtocolImpl;

#endif /* OEMO_IMAP_IMPL_H */
