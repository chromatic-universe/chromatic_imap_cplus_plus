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
				 const bodystructure_parser& operator= ( const bodystructure_parser ep );
				 //no compare
				 bool operator== ( const bodystructure_parser& ep );

			protected:

				//attributes
				 std::unique_ptr<std::ostringstream> m_ptr_mime_str;

				 //helpers

				 inline bool _S( const std::string& moniker , const std::string& token , std::string::size_type& sz )
				 {
					 bool b_ret( false );
					 sz = moniker.find( token );
					 sz != std::string::npos  ? b_ret = true : b_ret = false;

					 return b_ret;
				 }

				 inline mime_pr splice( const std::string& category , const std::string& token )
				 {
					 mime_pr pr{};
					 std::string::size_type sz;

					 if( _S( category , token , sz ) == true ){ pr.first = category.substr( 0 , sz );
						 	 	 	 	 	 	 	 	 	    pr.second = category.substr( ++sz ); }
					 std::ostringstream ostr;
					 ostr << quote << pr.first << quote; pr.first = ostr.str(); ostr.str( "" );
					 ostr << quote << pr.second << quote; pr.second = ostr.str();

					 return ( pr );
				 }

			public:

				 //services
				 inline void mime_traverse() { mime_traverse( m_mime_ptr.get() , 0 ); };
				 inline void mime_traverse( ck_mime_ptr ptr_mime , int level )
				 {
					  CkString strContentType;
					  ptr_mime->get_ContentType( strContentType );

					  mime_pr pr( splice( strContentType.getString() , forward_slash ) );
					  mime_pr cr( splice( ptr_mime->charset() , dash ) );
					  *m_ptr_ostr << content_type
							      << colon
							      << pr.first << " " << pr.second
							      << crlf
							      << charset
							      << colon
							      << cr.first << " " << cr.second
							      << crlf;

					  int num_parts = ptr_mime->get_NumParts();
					  for ( int i = 0; i < num_parts; i++ )
					  {
						std::cout << i << "\n";
						std::unique_ptr<CkMime> mime_part( ptr_mime->GetPart( i ) );
						mime_traverse( mime_part.get() , level + 1 );
					  }

				 }
		};




}

#endif//CHROMATIC_IMAP_BODYSTRUCTURE_H_
