// 27 Oct 2008 William K. Johnson 

#ifndef OEMO_IMAP_SEARCH_H
#define OEMO_IMAP_SEARCH_H

//c++ standard
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
#include <cstddef>
#include <cstdlib>

//boost
#include  <boost/algorithm/string/detail/case_conv.hpp>

//windows
#ifdef WIN32
#include <comdef.h>
#endif

//app specific
#include "OemoImapMessageManip.h"
#include "OemoDbInterfaceImpl.h"

//local search references
#include "OemoImapHandler.h"

//circular reference brain damage
#ifndef OEMO_ACCEPTOR_H
#ifdef WIN32
#import "ISYSCom.dll" no_namespace , named_guids
#include "isys732.h"
#endif
#endif

namespace OemoProtocolImpl
{

		//forward
		class OemoImapSearchCriteria;
		class OemoSearchResolver;

		//enumerations
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


		//types
		typedef OemoImapSearchCriteria* OemoImapSearchCriteraPtr;
		typedef OemoSearchResolver* OemoSearchResolverPtr;
		//atomic key with operator
		//	i.e 'not seen'
		typedef std::multimap<OemoLogicOperations , std::string> OemoAtomicSearchKey;
		//	=>=> parenthetical search key
		//		i.e. 'not (seen from "snaps")
		typedef std::multimap<OemoLogicOperations , std::string> OemoParentheticalSearchKey;	
		typedef std::vector<OemoParentheticalSearchKey> OemoSearches;
		typedef std::set<unsigned long> MessageNumbers;
		typedef std::set<OemoSearchKeys> SearchKeys;
		

		//classes	
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
										bool bDeconstructImmediate = true ,
										bool bPropagateExceptions = true );

				//dtor
				virtual ~OemoImapSearchCriteria();

			private:
					
				//attributes
				std::string m_messageSet;
				std::string m_criteria;
				OemoSearches m_searches;
				bool m_bUid;
				bool m_bDeconstructImmediate;
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
				void DeconstructCriteria();
				
				//immutable
				static const std::string constMessageSetAll;
						

		};	

		class OemoImapSearchResolverBase
		{
			public:

				//ctors
				OemoImapSearchResolverBase() {};
				OemoImapSearchResolverBase( OemoImapMgrPtr ptrImapMgr );
				//dtor
				virtual ~OemoImapSearchResolverBase() {};

				//services
				virtual void ResolveParentheticalKey( OemoParentheticalSearchKey ) = 0;
				virtual void ResolveAtomicKey( OemoAtomicSearchKey ) = 0;
				virtual bool IsMessageSet( const std::string& message_set ) = 0;

		};

		class OemoImapSearchResolver : public OemoImapSearchResolverBase
		{
			public:

				//ctors
				OemoImapSearchResolver() {};
				OemoImapSearchResolver( OemoImapMgrPtr ptrImapMgr );
				//dtor
				virtual ~OemoImapSearchResolver();

			private:

				//attributes
				MessageNumbers  m_MessageSet; 

			protected:

				//attributes
				OemoImapMgrPtr m_ptrImap;


			public:	

				//accessors-inspectors
				inline OemoImapMgrPtr ImapMgr() { return ( m_ptrImap ); };
				inline MessageNumbers& MessageSet() { return ( m_MessageSet ); }


				//mutators
				inline void ImapMgr( OemoImapMgrPtr ptrImap ) { m_ptrImap = ptrImap; }

				
				//services
				virtual void ResolveParentheticalKey( OemoParentheticalSearchKey );
				virtual void ResolveAtomicKey( OemoAtomicSearchKey );
				virtual bool IsMessageSet( const std::string& message_set );

		};
				


		//immutable
		static const std::string constIsysKey = "M5NMV 7K5FR 44ZCY 5DYU5 858ND 7SEQV";

}




#endif //OEMO_IMAP_SEARCH_H