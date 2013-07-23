/*
 * chromatic_imap_bodystructure.h
 *
 *  Created on: Jul 16, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_BODYSTRUCTURE_H_
#define CHROMATIC_IMAP_BODYSTRUCTURE_H_

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

#include "chromatic_imap_envelope.h"


namespace chromatic_imap_protocol_impl
{
		//forward
		class envelope_parser;

		//types
		typedef std::pair<std::string,std::string> mime_pr;

		//constant
		//tokens
		const std::string forward_slash( "/" );
		const std::string dash( "-" );
		//named tokens
		const std::string multipart( "multipart" );
		const std::string content_type( "content-type" );
		const std::string charset( "charset" );

		//enumerations


		//class bodystructure_parser
		class bodystructure_parser : public envelope_parser
		{
			public:

				//ctor
			    bodystructure_parser( ostr_ptr ostr ,
			    					  const std::string& file_name  )	: envelope_parser( ostr , file_name ) ,
			    					  	  	  	  	  	  	  	  	  	  m_ptr_mime_str { new ( std::ostringstream ) }
			    {

			    }

				//dtor
			    virtual ~bodystructure_parser()
			    {}

			private:

				 //no copy
				 bodystructure_parser( const bodystructure_parser& ep );
				 //no assign
				 const bodystructure_parser& operator= ( const bodystructure_parser& ep );
				 //no compare
				 bool operator== ( const bodystructure_parser& ep );

			protected:

				 //attributes
				 std::unique_ptr<std::ostringstream> m_ptr_mime_str;

				 //helpers
				 bool _S( const std::string& moniker , const std::string& token , std::string::size_type& sz );
				 mime_pr splice( const std::string& category , const std::string& token );

			public:

				 //services

				 //canonical message
				 inline void mime_traverse() { mime_traverse( m_mime_ptr.get() , 0 ); };
				 //mime tree
				 void mime_traverse( ck_mime_ptr ptr_mime , int level );
		};




}

#endif//CHROMATIC_IMAP_BODYSTRUCTURE_H_
