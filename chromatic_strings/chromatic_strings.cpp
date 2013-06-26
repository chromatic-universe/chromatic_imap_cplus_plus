/*
 * chromatic_strings.cpp

 *
 *  Created on: Jun 20, 2013
 *      Author: wiljoh
 */

#include <chromatic_strings.h>

using namespace chromatic_imap_protocol_impl;

using std::string;
using std::ostringstream;


//static initialization - default english
const string chromatic_fetch_strings::str_fetch_flags { "flags" };
const string chromatic_fetch_strings::str_fetch_internaldate  { "internaldate" };
const string chromatic_fetch_strings::str_fetch_rfc822size { "rfc822.size" };
const string chromatic_fetch_strings::str_fetch_rfc822text { "rfc822.text" };
const string chromatic_fetch_strings::str_fetch_rfc822 { "rfc822" };
const string chromatic_fetch_strings::str_fetch_all { "all" };
const string chromatic_fetch_strings::str_fetch_full { "full" };
const string chromatic_fetch_strings::str_fetch_body { "body" };
const string chromatic_fetch_strings::str_fetch_fast { "fast" };
const string chromatic_fetch_strings::str_fetch_envelope { "envelope" };
const string chromatic_fetch_strings::str_fetch_bodystructure { "bodystructure" };
const string chromatic_fetch_strings::str_fetch_bodypeek { "bodypeek" };
const string chromatic_fetch_strings::str_fetch_uid { "uid" };
const string chromatic_fetch_strings::str_fetch_header { "header" };
const string chromatic_fetch_strings::str_fetch_headerfields { "header.fields" };
const string chromatic_fetch_strings::str_fetch_unknown { "unknown" };

const chromatic_fetch_dictionary chromatic_fetch_strings::imap_fetch_dictionary { 	 { cifc::chromatic_imap_fetch_flags ,  str_fetch_flags } ,
																					 { cifc::chromatic_imap_fetch_internaldate , str_fetch_internaldate } ,
																					 { cifc::chromatic_imap_fetch_rfc822size , str_fetch_rfc822size } ,
																					 { cifc::chromatic_imap_fetch_rfc822text , str_fetch_rfc822text } ,
																					 { cifc::chromatic_imap_fetch_rfc822 , str_fetch_rfc822 } ,
																					 { cifc::chromatic_imap_fetch_all , str_fetch_all } ,
																					 { cifc::chromatic_imap_fetch_envelope , str_fetch_envelope } ,
																					 { cifc::chromatic_imap_fetch_full ,str_fetch_full } ,
																					 { cifc::chromatic_imap_fetch_body , str_fetch_body } ,
																					 { cifc::chromatic_imap_fetch_fast , str_fetch_fast } ,
																					 { cifc::chromatic_imap_fetch_bodystructure , str_fetch_bodystructure } ,
																					 { cifc::chromatic_imap_fetch_bodypeek , str_fetch_bodypeek } ,
																					 { cifc::chromatic_imap_fetch_uid , str_fetch_uid } ,
																					 { cifc::chromatic_imap_fetch_header , str_fetch_header } ,
																					 { cifc::chromatic_imap_fetch_headerfields , str_fetch_headerfields } ,
																					 { cifc::chromatic_imap_fetch_unknown , str_fetch_unknown } };


//------------------------------------------------------------------------------------------------------------------------------------
std::string chromatic_fetch_strings::_R( const cifc moniker )
{
	ostringstream ostr;

	string str{};
	ostr.imbue( locale() );

	auto pos = imap_fetch_dictionary.find( moniker );
	if ( pos != imap_fetch_dictionary.end() )
	{
		ostr << pos->second;
		str = ostr.str();
		lex() == lex_case::lc_upper ? ucase( str ) : lcase( str );
	}

	return str;
}



