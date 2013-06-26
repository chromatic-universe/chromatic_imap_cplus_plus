//OemoDbInterfaceImpl 
//initial version 11 August 2008 William K.Johnson
//Overtone Software , Inc. , All rights reserved


#ifndef OEMO_DB_INTERFACE_IMPL
#define OEMO_DB_INTERFACE_IMPL
#pragma warning( disable: 4786 4800 4365 )


//c++ std
#include <string>
#include <iostream>          
#include <sstream>
#include <fstream>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <exception>
#include <limits>
#include <bitset>
#include <queue>
#include <algorithm>
#include "ace/Trace.h"
#include "ace/Log_Msg.h"

//helpers
#include <generic.h>

//windows
#ifdef WIN32
#include <windows.h>
#include <SQLAPI.h>
#else
//linux
#include <cstdlib>
#include <cstdio>
#include <SQLAPI.h>
#endif

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

//default resource
#include "OemoDbString.h"


static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
			 "0123456789+/";



static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

namespace OemoDbInterface
{	

	const int constMaxStatementSize = 4096L;
	const std::string constDbMySql( "mysql" );
	const std::string constDbSqlServer( "sqlserver" );
	const std::string constDbOdbc( "odbc" );

	//enums
	enum OemoDbProviders
	{
		odpMySql ,
		odpSqlServer ,
		odpOracle ,
		odpOdbc ,
		odpSybase ,
		odpInformix ,
		odpSqlLite ,
		odpFirebird ,
		odpPostgres ,
		odpUndefined
	};

	enum OemoDbManipErrorLevel
	{
		odbmCritical = 0 ,
		odbmLogic ,
		odbmInformation ,
		odbmDebug ,
		odbmNoError
	};	


	//types
	typedef SACommandType_t OemoDbType;
	typedef SACommand OemoDbCommand;
	typedef SADataType_t OemoDataType;
	typedef std::vector<std::string> RawDefines;
	typedef std::map<unsigned long,std::string> ResourceMap;
	typedef std::pair<std::string,OemoDataType> Params;
	typedef std::vector<Params> ParamStack;
	typedef std::map<unsigned long,Params> BindParams;
	typedef std::pair<std::string,BindParams> BindStatements;
	typedef std::vector<BindStatements> StatementCache;
	typedef void (*pfv)();
	
	
			
	#ifndef WIN32	
	typedef unsigned long HINSTANCE;
	#endif
	
	
	//classes
	class OemoStatementDescription
	{
		public:
	
			//ctor
			OemoStatementDescription( OemoDbCommand& command ) : m_command( command )
			{
				ConstructParams();
			}
			//dtor
			virtual ~OemoStatementDescription()
			{}

		protected:

			std::string m_content;
			OemoDbType m_type;
			OemoDbCommand& m_command;
			unsigned long m_dwParamCount;
			ParamStack m_stack;
			virtual void ConstructParams()
			{
				for( int i = 0; i < m_command.ParamCount(); i++ )
				{
					std::string str;
					SAParam& param = m_command.ParamByIndex( i );
					switch 	( param.ParamType() )
					{
						case SA_dtString:
							str =  boost::lexical_cast<std::string>( param.asString().GetBuffer( 1024L ) );
							break;
						case SA_dtULong:
							str =  boost::lexical_cast<std::string>( param.asULong() );
							break;					
						case SA_dtLong:
							str =  boost::lexical_cast<std::string>( param.asLong() );
							break;
						case SA_dtDateTime:
							{
								SAString fstr =  param.asDateTime();
								str = fstr.GetBuffer( 1024L );
				
								break;
							}
						
						default:
							break;
					}
					std::pair<std::string,OemoDataType> pr ( str , param.ParamType() );
					m_stack.push_back( pr );	
				}				
			}				
			
	  	public:
			//accessors
			inline std::string Content() const throw() { return ( m_content ); }
			inline OemoDbType StatementType() const throw() { return ( m_type ); }
			OemoDbCommand& Command() { return ( m_command ); }
			inline unsigned long ParamCount() const throw() { return ( m_dwParamCount ); }
			inline ParamStack Params() const throw() { return ( m_stack ); } 			
			
			

	};
	//-------------------------------------------------------------		
	class OemoDbManipError : public std::logic_error
	{
		
		protected:

			//attributes
			OemoDbManipErrorLevel m_lError;			
			unsigned long m_dwNativeError;

		public:

			//accessors
			inline OemoDbManipErrorLevel ErrorLevel() const throw()	{ return ( m_lError ); }
			unsigned long NativeErrorCode() const throw() { return ( m_dwNativeError ); }

			//mutators
			inline void ErrorLevel( OemoDbManipErrorLevel level ) { m_lError = level; }
			inline void NativeErrorCode( const unsigned long dwNative ) { m_dwNativeError = dwNative; }
		
			//ctor
			OemoDbManipError( const std::string& what_arg ,
							  OemoDbManipErrorLevel error_level = odbmNoError ,
							  unsigned long dwNativeError = 0L ) :
							  logic_error( what_arg ) ,
							  m_lError( error_level ) ,
							  m_dwNativeError( dwNativeError )
			{
				//
			}		

	};
	//-------------------------------------------------------------
	class OemoDbInterfaceInfo
	{
		public:

			OemoDbInterfaceInfo() {}
			OemoDbInterfaceInfo( const std::string& target ,
								 const std::string& user ,
								 const std::string& password ,
								 const std::string& resource_path ,
								 bool bKeepAlive = false ) : username( user ) ,
															 password( password ) ,
															 target_server( target ) ,
															 resource_path( resource_path ) ,
															 keep_alive( bKeepAlive )
			{}
			std::string username;
			std::string password;
			std::string target_server;
			std::string resource_path;
			bool keep_alive;
	};
	
	//--------------------------------------------------------------
	class OemoDbProvider
	{
		public:

			OemoDbProvider() 
			{
				//
			}
			virtual ~OemoDbProvider() {}

		private:
		
		
		public:

			//accessors
			virtual OemoDbProviders Provider() = 0;			
			virtual OemoDbInterfaceInfo DbInfo() const throw() = 0;	
			virtual void Describe( std::ostream& ostr , const std::string& statement ) const throw() = 0;
			virtual SAClient_t sqlapi_client_define() const throw() = 0;
					

			
	};
	//--------------------------------------------------------------
	class OemoDbProviderBase : public OemoDbProvider
	{
		public:

			OemoDbProviderBase() {}
			OemoDbProviderBase( const OemoDbInterfaceInfo& info ) : m_info( info )  
			{
				//
			}
			virtual ~OemoDbProviderBase()  {}

		private:

		protected:
			
			OemoDbInterfaceInfo m_info;
		
		
		public:			

			//accessors
			virtual OemoDbProviders Provider() { return ( odpUndefined ); }			
			virtual OemoDbInterfaceInfo DbInfo() const throw() { return ( m_info ); }	
			virtual void Describe( std::ostream& ostr , const std::string& statement ) const throw() {}
			virtual SAClient_t sqlapi_client_define() const throw() { return( SA_Client_NotSpecified ); }
					

			
	};
	//---------------------------------------------------------------
	class OemoMySqlProvider : public OemoDbProviderBase
	{
		public:
			
			OemoMySqlProvider( const  OemoDbInterfaceInfo& info ) :  m_provider( odpMySql ) ,
																	 m_idString( "OemoMySql" ) ,	
																	 OemoDbProviderBase( info )
			{ ACE_Trace _( ACE_TEXT( "OemoDbProviderBase::OemoDbProviderBase" ) , __LINE__,__FILE__ ); }//Manoj upgrade for vc9
			OemoMySqlProvider() :  m_provider( odpMySql ) ,
							  	   m_idString( "OemoMySql" ) 	
			{ ACE_Trace _( ACE_TEXT( "OemoDbProviderBase::OemoDbProviderBase" ) , __LINE__,__FILE__ ); }//Manoj upgrade for vc9
			virtual ~OemoMySqlProvider() {}

		protected:

			//attributes
			OemoDbProviders m_provider;
			std::string m_idString;						

		public:
		
			//accessors
			virtual std::string IdString() { return ( m_idString ); }
			virtual OemoDbProviders Provider() { return ( m_provider ); }
			virtual OemoDbInterfaceInfo DbInfo() const throw() { return ( m_info ); }
			virtual void Describe( std::ostream& ostr , const std::string& statement ) const throw() {};
			virtual SAClient_t sqlapi_client_define() const throw() { return ( SA_MySQL_Client ); }

			//mutators
			void virtual DbInfo( OemoDbInterfaceInfo info ) { m_info = info; }
			

	};
	//-----------------------------------------------------------------
	class OemoSqlServerProvider : public OemoDbProviderBase
	{
		public:

			OemoSqlServerProvider( const OemoDbInterfaceInfo& info ) :  m_provider( odpMySql ) ,
																		m_idString( "OemoSqlServer" ) ,
																		OemoDbProviderBase( info )
			{ ACE_Trace _( ACE_TEXT( "OemoSqlServerProvider::OemoSqlServerProvider" ) , __LINE__,__FILE__ ); }//Manoj upgrade for vc9
			OemoSqlServerProvider() : m_provider( odpMySql ) ,
								      m_idString( "OemoSqlServer" )
			{ ACE_Trace _( ACE_TEXT( "OemoSqlServerProvider::OemoSqlServerProvider" ) , __LINE__,__FILE__ ); }//Manoj upgrade for vc9
			virtual ~OemoSqlServerProvider() {}

		protected:

			//attributes
			OemoDbProviders m_provider;
			std::string m_idString;					
			
			

		public:

			//accessors
			virtual std::string IdString() { return ( m_idString ); }
			virtual OemoDbProviders Provider() { return ( m_provider ); }			
			virtual OemoDbInterfaceInfo DbInfo() const throw() { return ( m_info ); }
			virtual void Describe( std::ostream& ostr , const std::string& statement ) const throw() {};	
			virtual SAClient_t sqlapi_client_define() const throw() { return ( SA_SQLServer_Client ); }

			//mutators
			virtual void DbInfo( OemoDbInterfaceInfo info ) { m_info = info; }

	};
	//-----------------------------------------------------------------
	class OemoOdbcProvider : public OemoDbProviderBase
	{
		public:

			OemoOdbcProvider( const OemoDbInterfaceInfo& info ) :  m_provider( odpMySql ) ,
																   m_idString( "OemoOdbc" ) ,
																   OemoDbProviderBase( info )
			{ ACE_Trace _( ACE_TEXT( "OemoOdbcProvider::OemoOdbcProvider" ) , __LINE__,__FILE__ ); }
			OemoOdbcProvider() : m_provider( odpOdbc ) ,
								      m_idString( "OemoOdbcProvider" )
			{ ACE_Trace _( ACE_TEXT( "OemoOdbcProvider::OemoOdbcProvider" ) , __LINE__,__FILE__ ); }
			virtual ~OemoOdbcProvider() {}

		protected:

			//attributes
			OemoDbProviders m_provider;
			std::string m_idString;	
			std::string m_connectString;		

			//helpers
			std::string ConnectStringFromInfo()
			{
				std::string strConnect( "" );

				return ( strConnect );
			}
			

		public:

			//accessors
			virtual std::string IdString() { return ( m_idString ); }
			virtual std::string ConnectString() { return ( m_connectString ); }
			virtual OemoDbProviders Provider() { return ( m_provider ); }			
			virtual OemoDbInterfaceInfo DbInfo() const throw() { return ( m_info ); }
			virtual void Describe( std::ostream& ostr , const std::string& statement ) const throw() {};	
			virtual SAClient_t sqlapi_client_define() const throw() { return ( SA_ODBC_Client ); }

			//mutators
			virtual void DbInfo( OemoDbInterfaceInfo info ) { m_info = info; }

	};

	typedef OemoDbProvider odp;	
	

    //template classes
	template <typename T> 
	class OemoDbManip
	{
		public:
            
			//ctors
			OemoDbManip( T *provider , SAConnection* con = NULL , bool bInitializeImmediate = true  ) : m_provider( provider ) , 	
																										m_ptrConnection( con ) ,			
																										m_bUseLocal( false ) ,																		 
																										m_bInitialized( false ) ,
																										m_strLast( "nil" )
			{ if( bInitializeImmediate == true ) Initialize(); }

			
			//dtor
			~OemoDbManip()
			{
				//
			}

		protected:
			
			//attributes
			T* m_provider;
			std::string   m_idString;
			SAConnection  m_con;
			SAConnection* m_ptrConnection;
					    

			//helpers
			bool LoadLibraryResource( const std::string& resource_path , HINSTANCE hinstance );
			ResourceMap ParseDefineHeaders( RawDefines& rds );
			RawDefines GobbleResourceHeader( const std::string& resource_path );
			std::string base64_encode( unsigned char const* bytes_to_encode, unsigned int in_len );
			std::string base64_decode( std::string const& encoded_string ); 

			//services			
			void DeInitialize();
			void ReInitialize();
			#ifdef WIN32
			//microsoft vc++ non-compliant with standard exception spec
			void virtual DeconstructParams( BindParams& bp , OemoDbCommand* command ) throw ( ... );
			#else
			void virtual DeconstructParams( BindParams& bp , OemoDbCommand* command ) throw ( OemoDbManipError );
			#endif

		private:			

			//attributes
			bool m_bUseLocal;
			bool m_bInitialized;
			BindParams m_params;
			HINSTANCE m_hInstance;
			std::string m_strLast;

			//copy constructor
			OemoDbManip( OemoDbManip<T> const& ); 
			//assignment operator
			OemoDbManip<T>& operator= ( OemoDbManip const& ); 
			
		public:

			//accessors
			OemoDbProviders Provider() { return ( m_provider.Provider() ); }
			T* ProviderRef() { return ( m_provider ); }
			std::string IdString() { return ( m_provider.IdString() ); }
			inline bool UseLocal() const throw() { return ( m_bUseLocal ); }
			inline SAConnection* Connection() { if( UseLocal() == true ) return ( &m_con ); else return(  m_ptrConnection ); }			
			inline bool Initialized() const throw() { return ( m_bInitialized ); }
			inline BindParams Params() const throw() { return ( m_params ); }
			inline HINSTANCE& ResInstance() { return ( m_hInstance ); }	
			inline std::string LastStatementStr() const throw() { return ( m_strLast ); }
					

			//mutators
			inline void UseLocal( bool bLocal ) { m_bUseLocal = bLocal; }
			inline void Initialized( bool bInitialized ) { m_bInitialized = bInitialized; }
			inline void Params( BindParams params ) { m_params = params; } 
			inline void ResInstance( HINSTANCE instance ) { m_hInstance = instance; }
			inline void LastStatementStr( const std::string& statement ) { m_strLast = statement; }

			//helpers
			std::string _R( const unsigned long resource_id , HINSTANCE hinstance );

			//services
			#ifdef WIN32
			//microsoft vc++ non-compliant with standard exception spec
			void Execute( unsigned long resource_id , OemoDbCommand* command  ) throw ( ... );
			void Execute( const std::string& statement , OemoDbCommand* command  ) throw ( ... );
			void Execute( StatementCache statements , OemoDbCommand* command  ) throw ( ... );
			void Execute( OemoDbCommand* command ) throw ( ... );
			#else
			void Execute( unsigned long resource_id , OemoDbCommand* command  ) throw ( OemoDbManipError);
			void Execute( const std::string& statement , OemoDbCommand* command ) throw ( OemoDbManipError );
			void Execute( StatementCache statements , OemoDbCommand* command ) throw ( OemoDbManipError );
			void Execute( OemoDbCommand* command ) throw ( OemoDbManipError );
			#endif

			bool Initialize();

			

			
	};

	//helpers
    void OemoDbManipUnexpected();

	class GenDbObj
	{
		public:

			GenDbObj() {}
			virtual ~GenDbObj() {}

		protected:

			//

		public:
			
			std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > GenManip( const std::string& db_string ,
																										std::auto_ptr<OemoDbInterface::OemoDbProviderBase> & odbpb ,
																										OemoDbInterfaceInfo info )//Manoj Kumar - Upgrade for Email Archive
			{
				std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > omdp;

				std::string str( db_string );
				boost::to_lower( str );
				if( str.compare( constDbMySql ) == 0 )
				{
					odbpb = std::auto_ptr<OemoDbInterface::OemoMySqlProvider> ( new OemoMySqlProvider( info ) );
				}
				else if( str.compare( constDbSqlServer ) == 0 )
				{
					odbpb = std::auto_ptr<OemoDbInterface::OemoSqlServerProvider> ( new OemoSqlServerProvider( info ) );
				}
				else if( str.compare( constDbSqlServer ) == 0 )
				{
					odbpb = std::auto_ptr<OemoDbInterface::OemoOdbcProvider> ( new OemoOdbcProvider( info ) );
				}
				else
				{
					OemoDbManipError e( "unsupported db string" );			
					e.ErrorLevel( odbmCritical );

					throw( e );
				}

				omdp = std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > 
					( new OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> ( odbpb.get() ) );


				return ( omdp );
			}	
			std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > GenManipByEnum( OemoDbProviders odp ,
																											  std::auto_ptr<OemoDbInterface::OemoDbProviderBase> & odbpb ,
																											  OemoDbInterfaceInfo info )//Manoj Kumar - Upgrade for Email Archive
			{
				std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > omdp;

				switch( odp )
				{
					case odpMySql:

						odbpb = std::auto_ptr<OemoDbInterface::OemoMySqlProvider> ( new OemoMySqlProvider( info ) );


						break;

					case odpSqlServer:

						odbpb = std::auto_ptr<OemoDbInterface::OemoSqlServerProvider> ( new OemoSqlServerProvider( info ) );

						break;

					
					case odpOdbc:

						odbpb = std::auto_ptr<OemoDbInterface::OemoOdbcProvider> ( new OemoOdbcProvider( info ) );

						break;

					default:
						{
							OemoDbManipError e( "unsupported db enumeration" );			
							e.ErrorLevel( odbmCritical );

							throw( e );
						}
				}

				
				omdp = std::auto_ptr<OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> > 
					( new OemoDbInterface::OemoDbManip<OemoDbInterface::OemoDbProviderBase> ( odbpb.get() ) );				

				return ( omdp );
			}
	};

};

//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
bool OemoDbInterface::OemoDbManip<T>::Initialize()
{
	if( Initialized() == true ) return ( true );

	//we won't throw exception on initialization

	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::Initialize()" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

	try
	{
		bool bRet = LoadLibraryResource( ProviderRef()->DbInfo().resource_path , ResInstance() );
		if( bRet == false )
		{
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): dll resource could not be found....\n") ));

			return ( bRet );
		}		

		if( m_ptrConnection == NULL )
		{
				
			UseLocal( true );

			try
			{
				  m_con.Connect(  (char*)ProviderRef()->DbInfo().target_server.c_str() ,     
								  (char*)ProviderRef()->DbInfo().username.c_str() ,  
								  (char*)ProviderRef()->DbInfo().password.c_str() ,//Manoj upgrade for vc9
	           					  (SAClient_t) ProviderRef()->sqlapi_client_define() );

				  m_con.setAutoCommit( SA_AutoCommitOff );
			}
			catch( SAException &x )
			{
			
				try
				{					
					// on error rollback changes
					m_con.Rollback();
				}
				catch( SAException & )
				{
				}
				
				ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	


				return ( false );

			}
		}
		else
		{
			if( m_ptrConnection->isConnected() == false )
			{
				try
				{
					 m_ptrConnection->Connect(   (char*)ProviderRef()->DbInfo().target_server.c_str() ,     
												 (char*)ProviderRef()->DbInfo().username.c_str() ,  
												 (char*)ProviderRef()->DbInfo().password.c_str() ,  
	           									 (SAClient_t) ProviderRef()->sqlapi_client_define() );

					 m_ptrConnection->setAutoCommit( SA_AutoCommitOff );
				}
				catch( SAException &x )
				{
				
					try
					{					
						// on error rollback changes
						m_ptrConnection->Rollback();
					}
					catch( SAException & )
					{
					}
					
					ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));					

					return ( false );

				}
			}
		}
	}
	catch( ... )
	{
		ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Untyped exception on Initialize()...\n") ));

		return ( false );
	}	

	Initialized( true );
	ACE_DEBUG ((LM_INFO, ACE_TEXT ("%D %M (thread_id->%t): Connected to database=%s...\n") ,
						(char*)ProviderRef()->DbInfo().target_server.c_str() ));		

	return ( true );
}

//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
void OemoDbInterface::OemoDbManip<T>::DeInitialize()
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::DeInitialize()" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

	if( ( UseLocal() == true ) && ( ProviderRef().DbInfo().keep_alive == false ) )
	{
		m_con.Disconnect();				
		Initialized( false );
		m_strLast( "nil" );
	}	
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
void OemoDbInterface::OemoDbManip<T>::ReInitialize()
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::ReInitialize()" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

	if( ( UseLocal() == true ) && ( Initialized() == false ) )
		{
		m_con.Disconnect();		
		
		UseLocal( true );

		try
		{
			  m_con.Connect( (char*)ProviderRef()->DbInfo().target_server.c_str() ,     
							 (char*)ProviderRef()->DbInfo().username.c_str() ,  
							 (char*)ProviderRef()->DbInfo().password.c_str() ,  
           					 (SAClient_t) ProviderRef()->sqlapi_client_define() );
		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				m_con.Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;						

			throw( e );

		}

		Initialized( true );
		
	}	
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
#ifdef WIN32
void OemoDbInterface::OemoDbManip<T>::DeconstructParams( BindParams& bp , OemoDbCommand* command ) throw( ... )
#else
void OemoDbInterface::OemoDbManip<T>::DeconstructParams( BindParams& bp , OemoDbCommand* command ) throw( OemoDbManipError )
#endif
{
		//localization of type casts to this virtual function, 
		if( bp.empty() ) return;

		try
		{
			if( command->isOpened() ) command->Close();
			for( BindParams::iterator iter = bp.begin();
				iter != bp.end();
			  		iter++ )
					{
						
  						//first    second<first  second>
						//<unsigned long <string,OemoDataType> >
						std::string bind( iter->second.first ); 
						//Manoj upgrade for vc9 - start
						switch ( iter->second.second )
						{
							case SA_dtString:
								if(bind.empty())
									*command << "NIL";
								else
							 		*command << (char*) bind.c_str();

								break;	

							case SA_dtULong:

								if(bind.empty())
									*command << (unsigned long)0;
								else
									*command << boost::lexical_cast<unsigned long>( bind );

								break;
					
							case SA_dtLong:

								if(bind.empty())
									*command << (long)0;
								else
									*command << boost::lexical_cast<long>( bind );
								//Manoj upgrade for vc9 - end
								break;

							case SA_dtDateTime:

								{
								  //expecting a base64 string here of a tm structure
								  std::auto_ptr<tm>  val ( reinterpret_cast<tm*>( (unsigned char*) OemoDbInterface::OemoDbManip<T>::base64_decode( bind ).c_str() ) );						  								  
								  
								  SADateTime sdt( (const struct tm&) *val.get() );
														
								  *command <<  sdt ;
								}
								break;

							default:

							     *command << boost::lexical_cast<char*>( bind );

							     break;						

						}
						
					}	
			

		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				m_ptrConnection->Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;						

			throw( e );

		}
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
#ifdef WIN32
void OemoDbInterface::OemoDbManip<T>::Execute( unsigned long resource_id , OemoDbCommand* command ) throw ( ... )
#else
void OemoDbInterface::OemoDbManip<T>::Execute( unsigned long resource_id , OemoDbCommand* command ) throw ( OemoDbManipError )
#endif
{
	    ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::Execute( unsigned long resource_id )" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

		if( command->isOpened() ) command->Close();

		command->setConnection( Connection() );

	    ReInitialize();
		if( Initialized() == false )
		{
			OemoDbManipError e( "initialization failure" );			
			e.ErrorLevel( odbmCritical );

			throw( e );
		}

		try
		{
			#ifdef WIN32 
			//get resource string
			std::string str = _R( resource_id , ResInstance() );
			//set statement
			command->setCommandText( (char*) str.c_str() );
			LastStatementStr( str );
			//bind parameters
			OemoDbInterface::OemoDbManip<T>::DeconstructParams( OemoDbInterface::OemoDbManip<T>::Params() , command );
			//execute
			command->Execute();
			//Manoj Kumar - Upgrade for Email Archive
			int i = 0;
			if(str.size() > 6)
			{
				for(i = 0; i < 6; i++)
					str[i] = tolower( str[i] );
			}
			if(str.compare(0,6,"select") != 0)
				Connection()->Commit();
			#endif
			
		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				if( UseLocal() == true ) m_con.Rollback(); else m_ptrConnection->Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;									

			throw( e );

		}
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
#ifdef WIN32
void OemoDbInterface::OemoDbManip<T>::Execute( const std::string& statement , OemoDbCommand* command  ) throw ( ... )
#else
void OemoDbInterface::OemoDbManip<T>::Execute( const std::string& statement , OemoDbCommand* command  ) throw ( OemoDbManipError )
#endif
{
        if( command->isOpened() ) command->Close();

		command->setConnection( Connection() );

		ReInitialize();
		if( Initialized() == false )
		{
			OemoDbManipError e( "Failure in execute immediate.." , odbmCritical );					

			throw( e );
		}

		try
		{
			#ifdef WIN32 			
			//set statement
			command->setCommandText( (char*) statement.c_str() );
			LastStatementStr( statement );
			//bind parameters
			OemoDbInterface::OemoDbManip<T>::DeconstructParams( OemoDbInterface::OemoDbManip<T>::Params() , command );
			//execute
			command->Execute();
			//Manoj Kumar - Upgrade for Email Archive
			int i = 0;
			std::string Str_Command = statement;
			if(Str_Command.size() > 6)
			{
				for(i = 0; i < 6; i++)
					Str_Command[i] = tolower( Str_Command[i] );
			}

			if(Str_Command.compare(0,6,"select") != 0)
				Connection()->Commit();
			#endif  
		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				if ( UseLocal() == true ) m_con.Rollback(); else m_ptrConnection->Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;						

			throw( e );

		}
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
#ifdef WIN32
void OemoDbInterface::OemoDbManip<T>::Execute( StatementCache statements , OemoDbCommand* command ) throw ( ... )
#else
void OemoDbInterface::OemoDbManip<T>::Execute( StatementCache statements , OemoDbCommand* command ) throw ( OemoDbManipError )
#endif
{
	    if( command->isOpened() ) command->Close();

		command->setConnection( Connection() );

		ReInitialize();
		if( Initialized() == false )
		{
			OemoDbManipError e;			
			e.ErrorLevel( odbmCritical );

			throw( e );
		}

		try
		{
			for( StatementCache::iterator iter = statements.begin();
					iter != statements.end();
						iter++ )
					{						  
					}

			Connection()->Commit();
		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				if UseLocal() == true m_con.Rollback() else em_ptrConnection->Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;						

			throw( e );

		}
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
#ifdef WIN32
void OemoDbInterface::OemoDbManip<T>::Execute( OemoDbCommand* command ) throw ( ... )
#else
void OemoDbInterface::OemoDbManip<T>::Execute( OemoDbCommand* command ) throw ( OemoDbManipError )
#endif
{
        if( command->isOpened() ) command->Close();

		command->setConnection( Connection() );

		ReInitialize();
		if( ( Initialized() == false ) || ( LastStatementStr().compare( "nil" ) == 0 ) )
		{
			OemoDbManipError e;			
			e.ErrorLevel( odbmCritical );

			throw( e );
		}

		try
		{
			#ifdef WIN32 			
			//execute
			command->setCommandText( (char*) LastStatementStr().c_str() );
			command->Execute();
			Connection()->Commit();
			#endif
		}
		catch( SAException &x )
		{
		
			try
			{					
				// on error rollback changes
				if UseLocal() == true m_con.Rollback() else em_ptrConnection->Rollback();
			}
			catch( SAException & )
			{
			}
			
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): %s...\n") ,  (const char*) x.ErrText() ));	

			OemoDbManipError e( (const char*) x.ErrText() , odbmCritical , x.ErrNativeCode() ) ;						

			throw( e );

		}
}
//--------------------------------------------------------------------------------------------------------------------------
template <typename T>
bool OemoDbInterface::OemoDbManip<T>::LoadLibraryResource( const std::string& resource_path , HINSTANCE hinstance )
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::LoadLibraryResource" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

	if( resource_path.empty() == true ) return ( false );
        
        try
        {
		   #ifdef WIN32 
		   hinstance = LoadLibraryA( (char*) resource_path.c_str() );
		   if( hinstance != NULL ) ResInstance( hinstance );
		   #else
		   return ( true );//noop
		   #endif
        }
        catch( ... )
        {
           return ( false );
        }


        if ( hinstance == NULL ) return ( false ); else return ( true );
}
//---------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::string OemoDbInterface::OemoDbManip<T>::_R( const unsigned long resource_id , HINSTANCE hinstance )
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::_R()" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9
	std::string str( "NIL" );    
	char* buffer = new char[ constMaxStatementSize ];
		
	#ifdef WIN32
        try
        {
          unsigned long dwSize = LoadStringA( hinstance ,
                                              resource_id ,
                                              buffer ,
											  constMaxStatementSize );
          if( dwSize != 0L )  str = buffer;
        }
        catch( ... )
        {
           ACE_DEBUG ((LM_INFO, ACE_TEXT ("%D %M (thread_id->%t): Could not retrieve resource string...\n") ));
        }
	 #endif

        if( buffer != NULL ) delete[] buffer;	    

        return ( str );
}

//-----------------------------------------------------------------------------------------------------------------------------
template <typename T>
OemoDbInterface::ResourceMap OemoDbInterface::OemoDbManip<T>::ParseDefineHeaders( OemoDbInterface::RawDefines& rds )
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::ParseDefineHeaders" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

        OemoDbInterface::ResourceMap rm;
        std::string str;
        std::string strDefine;
        std::string strValue;

        for( RawDefines::iterator iter = rds.begin();
               iter != rds.end();
                        iter++ )
                        {
                           str = *iter;
                           std::string::size_type idx = str.find_first_of( "#" );
                           if( idx != std::string::npos )
                           {
                                idx++;
                                std::string::size_type jdx = str.find( " " , idx );
                                if( jdx != std::string::npos )
                                {
                                        jdx++;
                                        str = str.substr( jdx );
                                        idx = str.find( " " );
                                        if( idx != std::string::npos )
                                        {
                                            strDefine = str.substr( 0 , idx );
                                            idx++;
                                            str = str.substr( idx );
                                            idx = str.find_first_not_of( " " );
                                            if( idx != std::string::npos )
                                            {
                                                str = str.substr( idx );
                                            }

                                            std::ostringstream ostr;
                                            unsigned long dw = atoi( (char*) str.c_str() );
                                            std::pair<std::string,std::string> pr ( dw ,  _R( dw ) );
                                            rm.insert( pr );
                                        }
                                }
                           }


                        }
		if( rm.empty() == true )
			ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%D %M (thread_id->%t): Empty header container...\n") ));

        return ( rm );

}
//-----------------------------------------------------------------------------------------------------------------------------
template <typename T>
OemoDbInterface::RawDefines OemoDbInterface::OemoDbManip<T>::GobbleResourceHeader( const std::string& resource_path )
{
	ACE_Trace _( ACE_TEXT( "OemoDbInterface::OemoDbManip<T>::GobbleResourceHeader" ) , __LINE__,__FILE__ );//Manoj upgrade for vc9

	OemoDbInterface::RawDefines rds;
	std::ifstream ifstr( resource_path );
	std::string strDefine( "#define" );

    	if( ifstr.is_open() )
    	{
            
            char read_buffer[1024];
            std::string str;
            

            while ( ! ifstr.eof() )
            {
                ifstr.getline( read_buffer , 1024 );
                str = read_buffer;
                str = boost::to_upper( str );
                if( str.find( strDefine ) != std::string::npos )
                {
                    rds.push_back( str );
                }

            }
            ifstr.close();

            OemoDbInterface::ResourceMap rm = ParseDefineHeaders( rds );            

   	 }
	else
	{
		ACE_DEBUG ((LM_ERROR,
			    ACE_TEXT ("%D %M (thread_id->%t): Could not open resource file=%s...\n") ,
			   (char*)  resource_path.c_str()  ));
	}


	return ( rds );
				

}

//-----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::string OemoDbInterface::OemoDbManip<T>::base64_encode( unsigned char const* bytes_to_encode, unsigned int in_len )
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
//-----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::string OemoDbInterface::OemoDbManip<T>::base64_decode( std::string const& encoded_string )
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






#endif //OEMO_DB_INTERFACE_IMPL

