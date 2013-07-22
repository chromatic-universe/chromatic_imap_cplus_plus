#ifndef CHROMATIC_STRINGS_H_
#define CHROMATIC_STRINGS_H_

#include <map>
#include <utility>
#include <string>
#include <memory>
#include <locale>
#include <algorithm>
#include <sstream>

namespace chromatic_imap_protocol_impl
{
	//forward
	class chromatic_fetch_strings;
	
	//enumerations
	enum class chromatic_imap_fetch_const : unsigned long
	{
			chromatic_imap_fetch_flags = 0L ,
			chromatic_imap_fetch_internaldate ,
			chromatic_imap_fetch_rfc822 ,
			chromatic_imap_fetch_rfc822size ,
			chromatic_imap_fetch_rfc822text ,
			chromatic_imap_fetch_all ,	
			chromatic_imap_fetch_envelope ,
			chromatic_imap_fetch_full ,
			chromatic_imap_fetch_fast ,
			chromatic_imap_fetch_body ,
			chromatic_imap_fetch_bodystructure ,
			chromatic_imap_fetch_uid ,
			chromatic_imap_fetch_bodypeek ,
			chromatic_imap_fetch_header ,
			chromatic_imap_fetch_headerfields ,
			chromatic_imap_fetch_unknown
	};
	
	enum class dds_retcode : unsigned long
	{
			dds_retcode_ok = 0 ,
			dds_retcode_error ,
			dds_retcode_unsupported ,
			dds_retcode_bad_parameter ,
			dds_retcode_precondition_not_met ,
			dds_retcode_out_of_resources ,
			dds_retcode_not_enabled ,
			dds_retcode_immutable_policy ,
			dds_retcode_inconsistent_policy ,
			dds_retcode_already_deleted ,
			dds_retcode_timeout ,
			dds_retcode_no_data ,
			dds_retcode_illegal_operation
	};

	enum class lex_case : unsigned long
	{
		lc_upper ,
		lc_lower
	};
	
	//types
	typedef std::map<dds_retcode , std::string> status_map;
	typedef std::map<chromatic_imap_fetch_const,std::string> chromatic_fetch_dictionary;
	typedef std::unique_ptr<chromatic_fetch_strings> cfs_ptr;	
	typedef chromatic_imap_fetch_const cifc;
	

	//generic
	template <typename T1 , typename T2>
	inline std::string resource_from_moniker( T1 const& moniker , T2 const& dictionary  )
	{
			std::ostringstream ostr;
			std::string resource{};

			auto pos = dictionary.find( moniker );
			if ( pos != dictionary.end() ) { ostr << pos->second; resource = ostr.str(); }

			return resource;
	}

	class chromatic_fetch_strings
	{
		public:
			
			//ctor
			chromatic_fetch_strings() : m_locale{ "" } ,
									    m_case{ lex_case::lc_lower }
			{}
			
			//dtor
			virtual ~chromatic_fetch_strings() 
			{}
		
		protected:
			
			
			//attributes			
			std::locale m_locale;
			lex_case m_case;
			
		public:
			
			//accesors-inspectors
			inline std::locale locale() noexcept { return m_locale; }
			inline lex_case lex() noexcept { return m_case; }

			//mutators
			inline void locale( const std::locale& lc ) { m_locale = lc; }
			inline void lex( const lex_case lc ) { m_case = lc; }			
			
			//helpers
			inline virtual void lcase( std::string& source ) { std::transform( source.begin() ,
																			   source.end() ,
																			   source.begin(), ::tolower ); }
			inline virtual void ucase( std::string& source ) { std::transform( source.begin() ,
																			   source.end() ,
																			   source.begin(), ::toupper ); }
			
			//services
			virtual std::string _R( const cifc& moniker );


			//immutable
			static const std::string str_fetch_flags;
			static const std::string str_fetch_internaldate;
			static const std::string str_fetch_rfc822size;
			static const std::string str_fetch_rfc822text;
			static const std::string str_fetch_rfc822;
			static const std::string str_fetch_all;
			static const std::string str_fetch_full;
			static const std::string str_fetch_body;
			static const std::string str_fetch_fast;
			static const std::string str_fetch_envelope;
			static const std::string str_fetch_bodystructure;
			static const std::string str_fetch_bodypeek;
			static const std::string str_fetch_uid;
			static const std::string str_fetch_header;
			static const std::string str_fetch_headerfields;
			static const std::string str_fetch_unknown;
			static const chromatic_fetch_dictionary imap_fetch_dictionary;

			static const std::string str_dds_retcode_ok;
			static const std::string str_dds_retcode_error;
			static const std::string str_dds_retcode_unsupported;
			static const std::string str_dds_retcode_bad_parameter;
			static const std::string str_dds_retcode_precondition_not_met;
			static const std::string str_dds_retcode_out_of_resources;
			static const std::string str_dds_retcode_not_enabled;
			static const std::string str_dds_retcode_immutable_policy;
			static const std::string str_dds_retcode_inconsistent_policy;
			static const std::string str_dds_retcode_already_deleted;
			static const std::string str_dds_retcode_timeout;
			static const std::string str_dds_retcode_no_data;
			static const std::string str_dds_retcode_illegal_operation;
			static const status_map sm;
			
	};
	
	
}
#endif //CHROMATIC_STRINGS_H_
