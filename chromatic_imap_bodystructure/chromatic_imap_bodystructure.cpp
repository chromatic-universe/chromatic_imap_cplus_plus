//============================================================================
// Name        : chromatic_imap_bodystructure.cpp
// Author      : 
// Version     :
// Copyright   :
// Description :
//============================================================================

#include "chromatic_imap_bodystructure.h"

using namespace chromatic_imap_protocol_impl;
namespace cipi = chromatic_imap_protocol_impl;

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

	//*ostr << "ENVELOPE" << "(";
	unique_ptr<bodystructure_parser> bsp( new  bodystructure_parser( ostr.get() , "Senior.eml" ) ) ;
	bsp->mime_traverse();
	//*ostr << ")";

	cout << ostr->str();

	return 0;

}
