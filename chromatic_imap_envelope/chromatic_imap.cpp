/*
 * chromatic_imap.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: wiljoh
 */

#include "chromatic_imap_envelope.h"


using namespace chromatic_imap_protocol_impl;

using std::cout;
using std::unique_ptr;
using std::string;
using std::cerr;
using std::ostringstream;
using std::vector;
using std::pair;

int main()
{
	unique_ptr<ostringstream> ostr( new ( ostringstream ) );

	*ostr << "ENVELOPE" << "(";
	unique_ptr<envelope_parser> ep( new  envelope_parser( ostr.get() , "Senior.eml" ) ) ;
	*ostr << ")";
	cout << ostr->str();

	return 0;
}



