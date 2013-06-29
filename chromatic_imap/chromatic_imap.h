//chromatic_imap.h
//created on: May 28, 2013
//author: William K. Johnson
//c++11

//suppress from adaptive communication environment
//#pragma clang diagnostic ignored "-Wself-assign"
//#pragma clang diagnostic ignored "-Wconstant-logical-operand"


#ifndef CHROMATIC_IMAP_H_
#define CHROMATIC_IMAP_H_

//c++ standard
#include <tuple>
#include <stdexcept>
#include <exception>
#include <system_error>
#include <new>
#include <ios>
#include <future>
#include <typeinfo>
#include <iostream>
#include <utility>
#include <string>
#include <memory>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <thread>

//c runtime std namepsace
#include <cstdio>
#include <cerrno>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <climits>
#include <cassert>
//c runtime
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//adaptive communication environment
#include <ace/Task.h>
#include <ace/Atomic_Op.h>
#include <ace/Acceptor.h>
#include <ace/Svc_Handler.h>
#include <ace/Date_Time.h>
#include <ace/Mutex.h>
#include <ace/OS.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Trace.h>
#include <ace/Timer_Queue_T.h>

//db
#include <SQLAPI.h>

//chilka
#include "CkMime.h"
#include "CkXml.h"
#include "CkString.h"
#include "CkEmail.h"
#include "CkMailMan.h"
#include <CkCrypt2.h>

//boost
#include <boost/format.hpp>
#include <boost/format/format_class.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/format/internals_fwd.hpp>
#include <boost/format/internals.hpp>
#include <boost/format/alt_sstream.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/value_type.hpp>
#include <boost/algorithm/string/detail/case_conv.hpp>

//hand rolled
//generic
#include <generic.h>
//types
#include "chromatic_types.h"
//strings
#include <chromatic_strings.h>


namespace chromatic_imap_protocol_impl
{

				//forward
				class chromatic_thread_pool;
				class chromatic_counter_guard;
				class chromatic_protocol_data;
				class chromatic_handler;
				class chromatic_acceptor;
				class chromatic_mailbox;
				class chromatic_imap_message;
				class chromatic_imap_mgr;

				//types
				typedef ACE_Atomic_Op<ACE_Mutex, int> counter_t;


				//functors
				class PopulateRange
				{
					   private:

						  int m_value;

					   public:

						  //ctor
						  PopulateRange( int initValue )
								  : m_value{ initValue } {}

						  //functor call
						  int operator() ()
						  {
							 return ( m_value++ );
						  }

				 };

				//predicates

				//generic

				//abstract
				class chromatic_imap_message_server_protocol
				{
					public:

						//ctor
						chromatic_imap_message_server_protocol() {}

						//dtor
						virtual ~chromatic_imap_message_server_protocol() {}

						//services
						virtual bool on_login( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_logout( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_authenticate( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_list( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_create( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_select( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_delete( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_rename( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_lsub( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_status( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_subscribe( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_unsubscribe( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_examine( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_noop( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_check( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_capability( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_search( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_copy( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_fetch( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_store( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_uid( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_close( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_expunge( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_append( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
						virtual bool on_starttls( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;

				};

				typedef chromatic_imap_message_server_protocol* chromatic_imap_message_server_protocol_ptr;


				//class chromatic_search_resolver_base
				class  chromatic_search_resolver_base
				{
					public:

						//ctors
						 chromatic_search_resolver_base() {};
						//dtor
						virtual ~ chromatic_search_resolver_base() {};

						//services
						virtual void resolve_parenthetical_key( chromatic_parenthetical_search_key key ,
															    v_int v_ints ) = 0;
						virtual void resolve_atomic_key( chromatic_atomic_search_key key ) = 0;

				};

				//exception
				class chromatic_imap_mgr_error : public std::logic_error
				{
					public:


						//ctor
						chromatic_imap_mgr_error( const std::string& what_arg ,
												  chromatic_imap_mgr_error_level error_level
													  = chromatic_imap_mgr_error_level::lmlNoError ) :
												  logic_error( what_arg ) ,
												  m_lError( error_level ) {}

					protected:

						//attributes
						chromatic_imap_mgr_error_level m_lError;

					public:

						//accessors
						inline chromatic_imap_mgr_error_level ErrorLevel() noexcept	{ return ( m_lError ); }

						//mutators
						inline void ErrorLevel( chromatic_imap_mgr_error_level level ) { m_lError = level; }

				};

				class chromatic_imap_search_error : public std::logic_error
				{
					public:

					    //ctor
						chromatic_imap_search_error(  const std::string& what_arg ,
													  chromatic_imap_search_error_level error_level
													    = chromatic_imap_search_error_level::islNoError ) :
													  logic_error( what_arg ) ,
													  m_lError( error_level ) {}

					protected:

						//attributes
						chromatic_imap_search_error_level m_lError;

					public:

						//accessors
						inline chromatic_imap_search_error_level error_level() noexcept	{ return ( m_lError ); }

						//mutators
						inline void error_level( chromatic_imap_search_error_level level ) { m_lError = level; }

				};


				//helpers
				//instance data
				class chromatic_protocol_data
				{
					public:

						//ctor
						chromatic_protocol_data( const std::string& nametag ) :  m_strNameTag{ nametag } ,
																				 m_dwPort{ 8000L } ,
																				 m_dwThreads{ 125 }
																				 {}

						//dtor
						virtual ~chromatic_protocol_data() {}

					protected:

						std::string m_strNameTag;
						std::string m_strDefaultHost;
						std::string m_strDocumentPath;
						unsigned long m_dwPort;
						unsigned long m_dwThreads;
						std::string m_strDn;

					public:

						//accessors
						inline std::string tag() { return ( m_strNameTag ); }
						inline std::string default_host() { return ( m_strDefaultHost ); }
						inline std::string dn() { return ( m_strDn ); }
						inline std::string document_path() { return ( m_strDocumentPath ); }
						inline unsigned long port() { return ( m_dwPort ); }
						inline unsigned long thread_pool_size() { return ( m_dwThreads ); }

						//mutators
						inline void port( unsigned long port ) { m_dwPort = port; }
						inline void default_host( std::string host ) { m_strDefaultHost = host; }
						inline void dn( std::string dn ) { m_strDn = dn; }
						inline void document_path( std::string document_path ) { m_strDocumentPath = document_path; }
						inline void thread_pool_size( unsigned long pool_size ) { m_dwThreads = pool_size; }

				};


				//services
				//class thread_pool
				class chromatic_thread_pool : public ACE_Task<ACE_MT_SYNCH>
				{
					public:

					  typedef ACE_Task<ACE_MT_SYNCH> inherited;

					  //ctor
					  explicit chromatic_thread_pool( void );

					  enum size_t
					  {
						default_pool_size_ = 25
					  };

					protected:

					  //upcall
					  int svc ( void );

					  //attributes
					  counter_t active_threads_;

					public:

					  //services
					  int start ( int pool_size = default_pool_size_ );
					  virtual int stop ( void );
					  int enqueue ( ACE_Event_Handler *handler ) ;

				};

				class chromatic_mailbox
				{
					public:

						//ctor
						explicit chromatic_mailbox()  : m_bSubscribed( false ) {}

						//dtor
						~chromatic_mailbox() {}

					private:

						//attributes
						std::string m_strName;
						chromatic_mailbox_attributes m_attributes;
						bool m_bSubscribed;

					public:

						//accessors
						inline std::string name() noexcept { return ( m_strName ); }
						inline chromatic_mailbox_attributes& attributes() { return ( m_attributes ); }
						inline bool subscribed() noexcept { return ( m_bSubscribed ); }

						//mutators
						inline void name( const std::string& name ) { m_strName = name; }
						inline void attributes( chromatic_mailbox_attributes attributes ) { m_attributes = attributes; }
						inline void subscribed( bool subscribed ) { m_bSubscribed = subscribed; }

				};

				//class chromatic_counter_guard
				class chromatic_counter_guard
				{
					public:

					  //ctor
					  chromatic_counter_guard ( counter_t &counter )
						: counter_ ( counter ) { ++counter_; }

					  //dtor
					  ~chromatic_counter_guard ( void ) { --counter_; }

					protected:

					  //attributes
					  counter_t &counter_;
				};

				//class chromatic_message_block_guard
				class chromatic_message_block_guard
				{
					public:

					  //ctor
					  chromatic_message_block_guard ( ACE_Message_Block *&mb ): mb_ { mb } {}

					  //dtor
					  ~chromatic_message_block_guard (void)	  {	mb_->release (); }

					protected:

					  //attributes
					  ACE_Message_Block *&mb_;

				};

				class chromatic_handler : public ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH>
				{
					public:

					  //types
					  typedef ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH> inherited;
					  typedef int (chromatic_handler::*ptrToFunction)( const std::string& strParams );//function pointer prototype
					  typedef std::map<std::string , ptrToFunction> map_of_commands;

					  //ctor
					  chromatic_handler ( const ACE_Time_Value& max_inactivity = ACE_Time_Value ( max_client_timeout ) );

					  //actual destructor
					  void destroy ();

					  //open handler
					  int open ( void *accept );

					  //close handler
					  int close ( u_long flags = 0 );


					  virtual int handle_close (ACE_HANDLE handle = ACE_INVALID_HANDLE,
												ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK);

					  int handle_timeout( const ACE_Time_Value& now , const void* act );



					private:

						//attributes
						std::ostringstream m_ostr;
						std::string m_strComputerName;
						std::string m_strToken;
						std::string m_strCommand;
						chromatic_command m_currentCommand;
						chromatic_packet_error m_chromatic_packet_error;
						object_dictionary m_objectDictionary;
						string_dictionary m_validObjects;
						std::string m_currentContext;
						bool m_bSilent;
						chromatic_imap_acceptor_ptr m_chromatic_acceptor;
						const ACE_Time_Value m_maximumTimeToWait;
						ACE_Time_Value m_lastActivityTime;
						long m_dwTimerToken;
						std::string m_strSpecify;
						bool m_bServerInfo;
						bool m_bReflex;
						bool m_bPrompt;
						bool m_bUid;
						map_of_commands m_commandMap;
						object_dictionary m_attributeMap;
						set_map m_setMap;
						std::string m_strCurrentModule;
						chromatic_imap_message_server_protocol_ptr ptr;
						current_state m_currentState;

						//imap manager
						chromatic_imap_mgr_ptr m_ptrImap;



					protected:

					  chromatic_thread_pool* thread_pool ();
					  ACE_thread_t creator_;

					  //adaptive communication environment overrides
					  int svc ();
					  int handle_input (ACE_HANDLE handle);

					  //thread queue callback
					  int process ( char *rdbuf, int rdbuf_len );

					  //accessors
					  inline std::ostringstream& stream() { return ( m_ostr ); }
					  inline map_of_commands& command_map() { return ( m_commandMap ); }

					  //helpers
					  void stream_out( const std::string& str );
					  void stream_out( const std::string& str , unsigned long len );
					  void stream_out( const byte_ptr& ptrBytes , unsigned long len );
					  void flush_stream_to_connector();
					  void flush_binary_stream_to_connector( unsigned char* u_buffer , unsigned long dwLen );
					  concurrency_t concurrency ();
					  bool check_connection_state( chromatic_state  state );
					  void update_connection_state( chromatic_set_op , chromatic_state state );

					  std::string client_addr();
					  void output_boiler_plate( std::string strConcurrency = "" );
					  std::string output_prompt( bool bToken = true );
					  void output_ok_response( const std::string& ack );
					  void output_no_response( const std::string& nack );
					  void parse_command( const std::string& packet ,
										 map_of_parts& parts ,
										 unsigned short& usCommandParamCount );
					  void parse_params( const std::string& params , string_container& strings );
					  void perform( const std::string& str );
					  void map_commands();


					  chromatic_imap_mgr_ptr context_ptr();

					  void wait_();

					  //string helpers

					  //find the first instance of token and truncate string from there
					  void gnaw( std::string& str , const std::string& token = " ");

					  //truncate string& at token and return truncation
					  void chomp( std::string& str , const std::string& token = cr );

					  //trim
					  void rtrimlast( std::string& str , const std::string& token = " " );
					  std::string rtrim( const std::string &szToTrim, const std::string& szTrimChars );
					  std::string ltrim( const std::string &szToTrim, const std::string& szTrimChars );
					  std::string extract_quoted_string( const std::string& str );

					  void format_packet_error( chromatic_packet_error pe );

					  //case
					  std::string lcase( const std::string& str );
					  std::string ucase( const std::string& str );

					  //hidden destructor
					  ~chromatic_handler (void);

					public:

						//immutable
						static const unsigned long constCommandBufferLength;
						static const unsigned short constMaxTokenLength;
						static const unsigned short constMaxLength;
						static const std::string constObjectAttributes;
						static const std::string constServerName;

						//accessors
						bool uid() const throw() { return ( m_bUid ); }
						inline std::string computer_name() const throw() { return ( m_strComputerName ); }
						inline std::string token() const throw()  { return ( m_strToken ); }
						inline std::string command_str() { return ( ucase( m_strCommand ) ); }
						inline chromatic_command command() const throw()  { return ( m_currentCommand ); }
						inline chromatic_packet_error error_packet() const throw() { return ( m_chromatic_packet_error ); }
						inline std::string current_context()  const throw() { return ( m_currentContext ); }
						inline bool silent() { return ( m_bSilent ); }
						inline std::string specify()  { return ( m_strSpecify ); }
						inline std::string module_string()  { return ( m_strCurrentModule ); }
						inline bool server_info() { return ( m_bServerInfo ); }
						inline bool feflex() { return ( m_bReflex ); }
						inline bool prompt() { return ( m_bPrompt ); }


						//mutators
						inline void token( const std::string& str_token ) { m_strToken = str_token; }
						inline void command_str( const std::string& str_command ) { m_strCommand = str_command; }
						inline void command( chromatic_command command ) { m_currentCommand = command; }
						inline void error_packet( chromatic_packet_error error ) { m_chromatic_packet_error = error; }
						inline void current_context( const std::string& context ) { m_currentContext = context; }
						inline void silent( bool bSilent ) { m_bSilent = bSilent; }
						inline void acceptor ( chromatic_acceptor *acceptor ) { m_chromatic_acceptor = acceptor; }
						inline void specify( const std::string& spec ) { m_strSpecify = spec; }
						inline void module_string( const std::string& module ) { m_strCurrentModule = module; }
						inline void reflex( bool bReflex ) { m_bReflex = bReflex; }
						inline void prompt( bool bPrompt ) { m_bPrompt = bPrompt; }
						inline void uid( bool uid ) { m_bUid = uid; }
						inline object_dictionary& reg_objects() { return ( m_objectDictionary ); }
						inline string_dictionary& valid_objects() { return ( m_validObjects ); }
						inline chromatic_acceptor* acceptor() { return ( m_chromatic_acceptor ); }

						//handlers
						int on_login( const std::string& params );
						int on_logout( const std::string& params );
						int on_authenticate( const std::string& params );
						int on_list( const std::string& params );
						int on_create( const std::string& params );
						int on_select( const std::string& params );
						int on_delete( const std::string& params );
						int on_rename( const std::string& params );
						int on_lsub( const std::string& params );
						int on_subscribe( const std::string& params );
						int on_unsubscribe( const std::string& params );
						int on_examine( const std::string& params );
						int on_noop( const std::string& params );
						int on_check( const std::string& params );
						int on_capability( const std::string& params );
						int on_search( const std::string& params );
						int on_copy( const std::string& params );
						int on_fetch( const std::string& params );
						int on_store( const std::string& params );
						int on_uid( const std::string& params );
						int on_close( const std::string& params );
						int on_expunge( const std::string& params );
						int on_append( const std::string& params );
						int on_status( const std::string& params );
						int on_starttls( const std::string& params );

						void flush_immediate( const std::string& atom );



				};

				typedef ACE_Acceptor <chromatic_handler, ACE_SOCK_ACCEPTOR> chromatic_acceptor_base;

				//class chromatic_acceptor
				class chromatic_acceptor : public chromatic_acceptor_base
				{
					public:

					  typedef chromatic_acceptor_base inherited;

					  chromatic_acceptor ( concurrency_t concurrency = concurrency_t::thread_pool_);
					  chromatic_acceptor ( chromatic_thread_pool &thread_pool);
					  ~chromatic_acceptor (void);

					  int open (const ACE_INET_Addr &addr,
								ACE_Reactor *reactor,
								chromatic_protocol_data_ptr instance_data ,
								int pool_size = chromatic_thread_pool::default_pool_size_ );

					  int close (void);

					protected:

					  //attributes
					  concurrency_t concurrency_;
					  chromatic_thread_pool private_thread_pool_;
					  chromatic_thread_pool &the_thread_pool_;
					  chromatic_protocol_data_ptr m_instanceData;

					public:

					  //accessors
					  inline concurrency_t concurrency (void) noexcept { return this->concurrency_; }
					  inline  chromatic_thread_pool_ptr thread_pool (void) { return &this->the_thread_pool_; }
					  int thread_pool_is_private (void) {return &the_thread_pool_ == &private_thread_pool_; }
					  chromatic_protocol_data_ptr  Data() { return ( m_instanceData ); }

					  //mutators
					  inline void thread_pool( ACE_Thread_Manager* thrd_mgr ) { ACE_Task<ACE_MT_SYNCH> ( thr_mgr ); }
					  inline void Data( chromatic_protocol_data_ptr data ) { m_instanceData = data; }

				};

				//class chromatic_imap_message
				class chromatic_imap_message
				{
					public:

					  //ctor
					  chromatic_imap_message()  : m_dwUid{ 0L }
												, m_dwSize{ 0L }
												{}

					  //dtor
					  virtual ~chromatic_imap_message()  {}

					private:

					  long m_dwUid;
					  long m_dwSize;
					  std::string m_strUrl;
					  std::string m_strFileUrl;
					  std::string m_strOffset;
					  std::string m_strServer;
					  chromatic_mail_message_flags m_flags;

					public:

					   //accessors
					   inline long uid() noexcept { return ( m_dwUid ); }
					   inline long size() noexcept { return ( m_dwSize ); }
					   inline std::string url() noexcept { return ( m_strUrl ); }
					   inline std::string file_url() noexcept { return ( m_strFileUrl ); }
					   inline std::string offset() noexcept { return ( m_strOffset ); }
					   inline std::string server() noexcept { return ( m_strServer ); }
					   inline chromatic_mail_message_flags flags() noexcept { return ( m_flags ); }

					   //mutators
					   inline void uid( const long uid ) { m_dwUid = uid; }
					   inline void size( const long size ) { m_dwSize = size; }
					   inline void url( const std::string& url ) { m_strUrl = url; }
					   inline void file_url( const std::string& url ) { m_strFileUrl = url; }
					   inline void offset( const std::string& offset ) { m_strOffset = offset; }
					   inline void server( const std::string& server ) { m_strServer = server; }
					   inline void flags( const chromatic_mail_message_flags flags ) { m_flags = flags; }

				};

				//class chromatic_mailbox_info
				class chromatic_mailbox_info
				{
					public:

						//ctor
						explicit chromatic_mailbox_info() : m_dwRecentMessages{ 0L } ,
															m_dwUnseenMessages{ 0L } ,
															m_dwFirstUnseen{ 0L } ,
															m_dwExistsMessages{ 0L } ,
															m_bReadOnly{ false }
															{}

						//dtor
						virtual ~chromatic_mailbox_info() {}

					private:

						//attributes
						std::string m_strName;
						long m_dwRecentMessages;
						long m_dwUnseenMessages;
						long m_dwFirstUnseen;
						long m_dwExistsMessages;
						bool m_bReadOnly;
						chromatic_mail_message_flags m_flags;
						chromatic_mail_message_flags m_changeable_flags;
						std::string m_strUidValidity;
						std::string m_strUidNext;


					protected:

						//

					public:

						//accessors
						std::string name() noexcept { return ( m_strName ); }
						long recent() noexcept { return ( m_dwRecentMessages ); }
						long unseen() noexcept { return ( m_dwUnseenMessages ); }
						long first_unseen() noexcept { return ( m_dwFirstUnseen ); }
						long exists() noexcept { return ( m_dwExistsMessages ); }
						bool read_only() noexcept { return ( m_bReadOnly ); }
						chromatic_mail_message_flags flags() noexcept { return ( m_flags ); }
						chromatic_mail_message_flags  changeable_flags() noexcept { return ( m_changeable_flags ); }
						std::string uid_validity() noexcept { return ( m_strUidValidity ); }
						std::string uid_next() noexcept { return ( m_strUidNext ); }

						//mutators
						inline void name( const std::string& name ) { m_strName = name; }
						inline void recent( const long recent ) { m_dwRecentMessages = recent; }
						inline void unseen( const long unseen ) { m_dwUnseenMessages = unseen; }
						inline void first_unseen( const long first ) { m_dwFirstUnseen = first; }
						inline void exists( const long exists ) { m_dwExistsMessages = exists; }
						inline void flags( const chromatic_mail_message_flags flags ) { m_flags = flags; }
						inline void changeable_flags( const chromatic_mail_message_flags changeable_flags ) { m_changeable_flags = changeable_flags; }
						inline void uid_validity( const std::string& uidvalidity ) { m_strUidValidity = uidvalidity; }
						inline void uid_next( const std::string& uidNext ) { m_strUidNext = uidNext; }
						inline void read_only( bool bReadOnly ) { m_bReadOnly = bReadOnly; }

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

				//class chromatic_append_record
				class chromatic_append_record
				{
					public:

						//ctor
						chromatic_append_record() : m_dwSize( 0L ) {}

						//dtor
						~chromatic_append_record() {}

					private:

						unsigned long m_dwSize;
						std::string m_strMbx;
						chromatic_mail_message_flags m_flags;

					protected:

						std::ostringstream m_ostr;

					public:

						//accessors
						inline unsigned long size() noexcept { return ( m_dwSize ); }
						inline std::string mbx() noexcept { return ( m_strMbx ); }
						inline chromatic_mail_message_flags flags() { return ( m_flags ); }
						inline std::ostringstream& stream() { return ( m_ostr ); }

						//mutators
						inline void size( const unsigned long sz ) { m_dwSize = sz; }
						inline void mbx( const std::string& mbx ) { m_strMbx = mbx; }
						inline void flags( const chromatic_mail_message_flags flags ) { m_flags = flags; }


						//services
						void clear()
						{
							m_dwSize = 0L;
							m_strMbx = "";
							m_flags.clear();
						}
				};

				//class chromatic_imap_search_criteria
				class chromatic_imap_search_criteria
				{
					public:

						//ctor
						chromatic_imap_search_criteria( const std::string& criteria ,
														const std::string& message_set = "nil" ,
														bool bUid = false ,
														bool bPropagateExceptions = true );

						//dtor
						virtual ~chromatic_imap_search_criteria();

					private:

						//attributes
						std::string m_messageSet;
						std::string m_criteria;
						chromatic_searches m_searches;
						bool m_bUid;
						bool m_bPropagateExceptions;

					protected:

						//helpers
						std::string extract_quoted_string( const std::string& str );
						chromatic_searches extract_parenthetical_keys( const std::string& str );
						std::string search_key_to_str( const chromatic_search_keys  key );
						chromatic_search_keys str_to_search_key( const std::string& str );
						inline void clear()
						{
							m_messageSet = "nil";
							m_criteria = "";
							m_searches.clear();
						}


					public:

						//accessors-inspectors
						inline std::string message_set() noexcept { return ( m_messageSet ); }
						inline std::string raw_criteria() noexcept { return ( m_criteria ); }
						inline chromatic_searches& searches() { return ( m_searches ); }
						inline bool uid() noexcept { return ( m_bUid ); }



						//mutators
						inline void message_set( const std::string& message_set )  { m_messageSet = message_set; }
						inline void raw_criteria( const std::string& raw_criteria )  { m_criteria = raw_criteria; }
						inline void searches( const chromatic_searches& searches ) { m_searches = searches; }
						inline void uid( bool uid ) { m_bUid = uid; }


						//services
						void deconstruct_criteria( chromatic_imap_search_resolver_ptr resolver );

						//immutable
						static const std::string constMessageSetAll;

				};

				//class chromatic_message_manip
				class chromatic_message_manip
				{
					public:

						//ctors
						explicit chromatic_message_manip() {}

						//dtor
						virtual ~chromatic_message_manip()
						{}

					private:

						//

					protected:

						//

					public:

					    //services
						virtual v_int expand_message_sequence( const std::string& sequence_in ,
													  	  	  int   dwLimit );
						virtual int  get_message_count( const std::string& sequence_in , int dwLimit = 0 );
						virtual void process_atoms( const std::string& atom  , v_int& subatoms , int dwLimit = 0 );
						virtual int  max_atom( v_int subatoms );
						virtual int  min_atom( v_int subatoms );
						virtual v_int atoms_intersection( v_int& first_atoms , v_int& second_atoms );
						virtual std::string extract_parenthetical_atoms( const std::string& payload );
						virtual bool is_message_set( const std::string& atom , int dwLimit = 0 );

				};

				//class chromatic_imap_search_resolver
				class chromatic_imap_search_resolver
				{
					public:

						//ctors
						chromatic_imap_search_resolver() :  m_ptrImap{ nullptr } ,
															m_ptrCriteria{ nullptr } ,
														    m_bInitialized{ false } {}
						chromatic_imap_search_resolver( chromatic_imap_mgr_ptr ptr_imap_mgr ,
														chromatic_imap_search_criteria_ptr criteria ,
														const std::string& document_path ,
														const std::string& search_domain = "nil" );

						//dtor
						virtual ~chromatic_imap_search_resolver();

					private:

						//attributes
						chromatic_message_numbers  m_MessageSet;
						imap_url_to_uid m_dictionary;

					protected:

						//attributes
						chromatic_imap_mgr_ptr m_ptrImap;
						chromatic_imap_search_criteria_ptr m_ptrCriteria;
						std::string m_strDocumentPath;
						std::string m_strSearchDomain;
						bool m_bInitialized;
						v_int m_v_ints;

					public:

						//accessors-inspectors
						inline chromatic_imap_mgr_ptr imap_mgr() { return ( m_ptrImap ); };
						inline chromatic_message_numbers& message_set() { return ( m_MessageSet ); }
						inline chromatic_imap_search_criteria_ptr criteria() { return ( m_ptrCriteria ); }
						inline std::string document_path() noexcept { return ( m_strDocumentPath ); }
						inline std::string search_domain() noexcept { return ( m_strSearchDomain ); }
						inline bool initialized() { return ( m_bInitialized ); }
						inline imap_url_to_uid& url_to_uid() { return ( m_dictionary ); }
						inline v_int search_results() { return ( m_v_ints ); }


						//mutators
						inline void imap_mgr( chromatic_imap_mgr_ptr ptrImap ) { m_ptrImap = ptrImap; }
						inline void criteria( chromatic_imap_search_criteria_ptr ptrCriteria ) { m_ptrCriteria = ptrCriteria; }
						inline void document_path( const std::string& document_path ) { m_strDocumentPath = document_path; }
						inline void search_domain( const std::string& search_domain ) { m_strSearchDomain = search_domain; }


						//services
						virtual void resolve_parenthetical_key( chromatic_parenthetical_search_key key , v_int v_ints );
						virtual void resolve_parenthetical_key( chromatic_parenthetical_search_key key , const std::string& top_level_message_set );
						virtual void resolve_atomic_key( chromatic_atomic_search_key key );
						v_int message_numbers_to_uids( const v_int v_ints );
						v_int uids_to_message_numbers( const v_int v_ints );

						//immutable
						static const std::string const_strKey;


				};

				//class chromatic_imap_mgr
				class chromatic_imap_mgr : public chromatic_imap_message_server_protocol
				{
					friend chromatic_imap_search_resolver;

					public:

						chromatic_imap_mgr( const std::string& dbserver ,
									 const std::string& proxystore ,
									 chromatic_handler_ptr imap_handler = NULL );
						virtual ~chromatic_imap_mgr();

					private:

						//attributes
						bool m_bConnected;
						bool m_bCloseExpunge;
						std::ostream* m_ptrStream;
						std::string m_strDbServer;
						std::unique_ptr<imap_server_messages> m_ptrMessages;
						std::string m_strProxyStore;
						bool m_bUseUid;
						std::string m_strServer;
						std::string m_strDsn;
						std::string m_strUser;
						std::string m_strUserDomain;
						chromatic_mailbox_info m_mbxInfo;
						chromatic_append_record m_appendRecord;
						imap_fetch_strings m_fetchMap;
						unsigned long m_dwMinUid;
						unsigned long m_dwMaxUid;

						//no copy
						chromatic_imap_mgr( const chromatic_imap_mgr& cim );
						//no assign
						const chromatic_imap_mgr& operator= ( const chromatic_imap_mgr& cim );

					protected:

						//helpers
						bool make_directory( const std::string& newdir );
						std::pair<std::string,std::string> pair_from_tuple( const std::string& tuple );
						bool read_params();

						//string manip
						std::string lcase( const std::string& str );
						std::string ucase( const std::string& str );
						std::string rtrim( const std::string &szToTrim, const std::string& szTrimChars );
						std::string ltrim( const std::string &szToTrim, const std::string& szTrimChars );

						//imap helpers
						std::string check_folder_name( const std::string& folder );
						std::string file_to_http_url( const std::string& fileurl , const std::string& server );
						std::string http_to_file_url( const std::string& str );
						void status_response( chromatic_mailbox_info info , std::ostream& ostr , imap_strings vStr );
						void select_response( chromatic_mailbox_info info , std::ostream& ostr , bool bReadOnly );
						std::string fetch_response( const std::string& msg_id , const std::string& payload , std::ostream& ostr );
						std::string search_response( const v_int& v_intsr );
						void store_response( const std::string& msg_id , const std::string& payload , std::ostream& ostr );
						std::string extract_quoted_string( const std::string& str );
						std::string mbx_attributes_to_str( chromatic_mailbox_attributes omba );
						std::string mbx_flags_to_str(chromatic_mail_message_flags  ommf );
						void str_to_mbx_flags( const std::string& flag_string ,chromatic_mail_message_flags&  ommf );
						std::string time_zone_info();
						std::string internal_date( const std::string& filename );
						std::string base64_encode( unsigned char const* bytes_to_encode , unsigned int in_len );
						std::string base64_decode(std::string const& encoded_string);
						std::string fetch_envelope( const std::string& message_path );
						std::string fetch_body_structure( const std::string& message_path );
						std::string fetch_rfc822( const std::string& message_path , const std::string& command );
						std::string fetch_mime( const std::string& message_path , const std::string& mime_part );
						imap_header_fields fetch_header_fields_mime( const std::string& message_path , const std::string& params );
						std::string fetch_header_fields(  const std::string& message_path , const std::string& params );
						void parse_header_field_params( const std::string& source , imap_header_set& fields_set );
						std::string fetch_header( const std::string& message_path , const std::string& params , bool bIsPart = false );
						void store_flags( unsigned long msg_id ,chromatic_mail_message_flags flags ,
										 chromatic_flags_method  method , std::ostream& ostr ,
										 bool bSilent = false );
						std::string message_tag( const std::string& fileurlstr  );


						//internal services
						void enum_mailbox_list(   chromatic_mailbox_list& mbxs ,
												  const std::string& username ,
												  const std::string& reference ,
												  const std::string& criteria ,
												  bool subscribed = false );
						void collect_mailboxes_by_db(  const std::string& reg ,
												   	   const std::string& username ,
													   chromatic_mailbox_list& obl ,
													   bool subscribed = false );
						bool is_subscribed_by_db( const std::string& folder , const std::string& nickname );
						chromatic_mailbox_result folder_exists( const std::string& folder );
						void fill_message_list_by_db( const std::string& folder );
						void update_mailbox_info_by_db( const std::string& folder , bool bReadOnly = false );
						void update_mailbox_info_by_db( const std::string& folder , chromatic_mailbox_info& info );
						std::string fetch_message(  unsigned long msg_id , std::ostream& ostr , chromatic_imap_fetch_requests& requests );
						void parse_fetch_request_list( const std::string& request , chromatic_imap_fetch_requests& requests );
						void set_message_flags_by_db( const std::string& url ,chromatic_mail_message_flags flags );

						//attributes
						reg_params m_regParams;
						SAConnection m_db;
						chromatic_handler_ptr m_ptrHandler;

					public:

						//accessors-inspectors
						inline std::ostream* output_stream() { if( m_ptrStream != NULL ) return ( m_ptrStream ); else return ( NULL ); };
						inline std::string server_string() noexcept { return  (m_strServer);}
						inline std::string db_server_string() noexcept { return  ( m_strDbServer );}
						inline std::string dsn() noexcept { return  ( m_strDsn );}
						inline std::string user() noexcept { return  ( m_strUser );}
						inline std::string proxy_store() noexcept { return  ( m_strProxyStore );}
						inline bool is_connected() noexcept { return m_bConnected;};
						inline bool close_expunge() noexcept { return m_bCloseExpunge;};
						inline bool uids() noexcept { return ( m_bUseUid ); }
						inline unsigned long max_uid() noexcept { return ( m_dwMaxUid ); }
						inline unsigned long min_uid() noexcept { return ( m_dwMinUid ); }
						inline chromatic_handler_ptr  imap_handler() { return ( m_ptrHandler ); }
						inline std::string user_domain() noexcept { return  ( m_strUserDomain );}

						//mutators
						inline void output_stream( std::ostream* ptrStream ) { m_ptrStream  = ptrStream; };
						virtual void db_server_string( const std::string& db ) { m_strDbServer = db; }
						virtual void proxy_store( const std::string& store ) { m_strProxyStore = store; }
						virtual void dsn( const std::string& dsn ) { m_strDsn = dsn; }
						virtual void user( const std::string& user ) { m_strUser = user; }
						virtual void user_domain( const std::string& user_domain ) { m_strUserDomain = user_domain; }
						inline void uids( bool bUid ) { m_bUseUid = bUid; }
						inline void close_expunge( bool bClose ) { m_bCloseExpunge = bClose; }
						inline void max_uid( unsigned long dw ) { m_dwMaxUid = dw; }
						inline void min_uid( unsigned long dw ) { m_dwMinUid = dw; }
						inline chromatic_mailbox_info& mbx_info() { return ( m_mbxInfo ); }
						inline chromatic_append_record& append_rec() { return ( m_appendRecord ); }
						inline reg_params& params() { return ( m_regParams ); }
						inline imap_server_messages_ptr messages() { return ( m_ptrMessages.get() ); }

						//services
						bool open_server();
						void deinit();


						//overrides
						virtual bool on_login( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_logout( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_authenticate( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_list( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_create( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_select( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_delete( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_rename( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_lsub( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_status( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_subscribe( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_unsubscribe( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_examine( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_noop( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_check( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_capability( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_search( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_copy( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_fetch( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_store( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_uid( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_close( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_expunge( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_append( const std::string& params , const std::string& format , std::ostream& ostr );
						virtual bool on_starttls( const std::string& params , const std::string& format , std::ostream& ostr );


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



				//immutable



}
#endif //CHROMATIC_IMAP_H_
