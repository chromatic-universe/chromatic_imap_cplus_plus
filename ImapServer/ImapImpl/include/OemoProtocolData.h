#ifndef OEMO_PROTOCOL_DATA
#define OEMO_PROTOCOL_DATA
#include <string>

namespace OemoImapProtocolImpl
{
	typedef void* VoidPtr;

	//instance data
	class OemoProtocolData
	{
		public:
			
			//ctor
			OemoProtocolData( const std::string& nametag ) : m_strNameTag( nametag ) ,
															 m_dwPort( 8000L ) ,
															 m_dwThreads( 125 )
															 
			{
				//
			}

			//dtor
			virtual ~OemoProtocolData()
			{
				//
			}
		

		protected:

			std::string m_strNameTag;
			std::string m_strSearchPath;
			std::string m_strDefaultHost;
			std::string m_strDocumentPath;
			unsigned long m_dwPort;
			unsigned long m_dwThreads;
			std::string m_strDn;

		public:

			//accessors
			inline std::string Tag() { return ( m_strNameTag ); }
			inline std::string DefaultHost() { return ( m_strDefaultHost ); }
			inline std::string Dn() { return ( m_strDn ); }
			inline std::string IsysPath() { return ( m_strSearchPath ); }
			inline std::string DocumentPath() { return ( m_strDocumentPath ); }
			inline unsigned long Port() { return ( m_dwPort ); }			
			inline unsigned long ThreadPoolSize() { return ( m_dwThreads ); }

			//mutators
			inline void Port( unsigned long port ) { m_dwPort = port; }
			inline void DefaultHost( std::string host ) { m_strDefaultHost = host; }
			inline void Dn( std::string dn ) { m_strDn = dn; }
			inline void IsysPath( std::string search_path ) { m_strSearchPath = search_path; }
			inline void DocumentPath( std::string document_path ) { m_strDocumentPath = document_path; }
			inline void ThreadPoolSize( unsigned long pool_size ) { m_dwThreads = pool_size; }
			
	};

	typedef OemoProtocolData* OemoProtocolDataPtr;

}



#endif//#define OEMO_PROTOCOL_DATA