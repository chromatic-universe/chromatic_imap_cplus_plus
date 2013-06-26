#pragma warning( disable: 4786 4800 4365 )


#include <set>
#include <string>
#include <map>


#ifndef OEMO_MESSAGE_SERVER_PROTOCOL_H
#define OEMO_MESSAGE_SERVER_PROTOCOL_H


#define OEMOPROTOCOL_API __declspec(dllexport)

namespace OemoImapProtocolImpl
{
	enum OemoProtocol
	{
		oprOperationNotSupported = 0 ,
		oprRequiresSpecification 		
	};

	enum OemoImapDispatch
	{
		oedLogin = 0 ,
		oedLogout ,
		oedAuthenticate ,		
		oedCreate ,
		oedSelect ,
		oedList ,
		oedDelete,
		oedRename ,
		oedLsub ,
		oedSubscribe ,
		oedUnsubscribe ,
		oedExamine ,
		oedNoop ,
		oedCheck ,
		oedCapability ,
		oedSearch ,
		oedCopy ,
		oedFetch ,
		oedStore ,
		oedUid ,
		oedClose ,
		oedExpunge ,
		oedAppend ,
		oedStarttls

	};

	//types
	typedef std::set<OemoProtocol> protocolSet;
	typedef std::map<OemoImapDispatch,protocolSet> ProtocolMap;

	//abstract base
	class OEMOPROTOCOL_API OemoImapMessageServerProtocol
	{
		public:

			//ctor
			OemoImapMessageServerProtocol()
			{
				//
			}

			virtual ~OemoImapMessageServerProtocol()
			{
				//
			}

			//accesors			
			

			//mutators		
			

			//services		
			virtual bool OnLogin( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;			
			virtual bool OnLogout( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnAuthenticate( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnList( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnCreate( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnSelect( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnDelete( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnRename( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;		
			virtual bool OnLsub( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnStatus( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;
			virtual bool OnSubscribe( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnUnsubscribe( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnExamine( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnNoop( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;							
			virtual bool OnCheck( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnCapability( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnSearch( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnCopy( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;	
			virtual bool OnFetch( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnStore( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnUid( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnClose( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnExpunge( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnAppend( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;				
			virtual bool OnStarttls( const std::string& params , const std::string& format , std::ostream& ostr ) = 0;								
			
	};

	typedef OemoImapMessageServerProtocol* OemoImapMessageServerProtocolPtr;
}
			

#endif//OEMO_MESSAGE_SERVER_PROTOCOL_H


