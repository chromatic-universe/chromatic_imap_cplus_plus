/*
 * chromatic_imap_envelope.h
 *
 *  Created on: Jul 7, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_ENVELOPE_H_
#define CHROMATIC_IMAP_ENVELOPE_H_


#pragma GCC diagnostic ignored "-Wswitch"

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cassert>
#include <utility>

#include <boost/algorithm/string.hpp>

#include <stdio.h>
#include <CkByteData.h>
#include <CkString.h>
#include <CkMime.h>
#include <CkEmail.h>

#include <unistd.h>


namespace chromatic_imap_protocol_impl
{

			//types
			typedef CkMime ck_mime;
			typedef CkEmail ck_email;
			typedef ck_email* ck_email_ptr;
			typedef ck_mime* ck_mime_ptr;
			typedef CkString ck_string;
			typedef std::ostringstream* ostr_ptr;
			typedef std::pair<std::string,std::string> addr_pr;
			typedef std::vector<addr_pr> addr_container;

			//constant
			const std::string constMimeUnlockCode = "SFCNICOMSMIME_sK7NCgi98C5p";
			const std::string constMailUnlockCode = "SFCNICOMMAILQ_5NklwCi28R6H";
			//tokens
			const std::string space(" ");
			const std::string right_paren( ")" );
			const std::string left_paren( "(" );
			const std::string nil( "NIL" );
			const std::string quote( "\"" );
			const std::string empty( "" );
			const std::string colon( ":" );
			const std::string ampersand( "@" );
			const std::string left_angle( "<" );
			const std::string right_angle( ">" );
			const std::string crlf( "\n" );
			//named tokens
			const std::string date( "Date" );
			const std::string subject( "Subject" );
			const std::string in_reply_to( "In-Reply-To" );
			const std::string message_id( "Message-ID" );
			const std::string sender( "Sender" );
			const std::string reply_to( "Reply-To" );

			//enumerations
			enum class addr_type : unsigned long
			{
				atFrom ,
				atTo ,
				atReplyTo ,
				atSender ,
				atCC ,
				atBCC ,
				atGroup ,
				atNoMoreAddrs
			};

			//class envelope parser
			class envelope_parser
			{
				public:


					 //ctor
					 envelope_parser( ostr_ptr ostr ,  const std::string& file_name );

					 //dtor
					 virtual ~envelope_parser();

				private:

					 //no default
					 explicit envelope_parser();
					 //no copy
					 envelope_parser( const envelope_parser& ep );
					 //no assign
					 const envelope_parser& operator= ( const envelope_parser ep );
					 //no compare
					 bool operator== ( const envelope_parser& ep );

				protected:

					//attributes
					ostr_ptr m_ptr_ostr;
					std::unique_ptr<ck_mime> m_mime_ptr;
					std::unique_ptr<ck_email> m_email_ptr;
					std::string m_filename;
					addr_container m_ac;

					//helpers
					bool check_for_group_addr( const std::string& addr , std::string& group_moniker );
					void process_group_addrs( const std::string& addrs );
					std::string addr_atom( const std::string& line );
					void single_sender_addr_line( const std::string& sender );
					void single_addr_line( const std::string& moniker );
					void multiple_addr_lines( const std::string& moniker );
					std::string friendly_name( const std::string& moniker );
  				    inline void nil_addr_line()	{ *m_ptr_ostr << nil << crlf; }

					//services
					void process_multiple_addrs( addr_container& adrc );
					void fetch_addrs( addr_type at );
					void str_env_field_to_stream( const std::string& field_moniker );

				public:

					//services
					virtual void parse();

					//accessors-inspectors
					inline std::string filename() noexcept { return m_filename; }

					//mutators
					inline void filename( const std::string& name ) { m_filename = name; }

			};
}


#endif//CHROMATIC_IMAP_ENVELOPE_H_
