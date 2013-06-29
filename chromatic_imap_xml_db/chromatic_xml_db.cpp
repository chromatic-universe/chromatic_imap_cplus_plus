//============================================================================
// Name        : chromatic_xml_db.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "chromatic_imap_xml_db.h"

using namespace chromatic_imap_xml_db;
namespace utils = chromatic_imap_protocol_impl::chromatic_universe_utils;
using namespace utils;

using std::cout;
using std::unique_ptr;

int main()
{
	trace_into_first( "chromatic_imap , copyright 2012 , William K. Johnson , this is the xml db module" );

	try
	{
		unique_ptr<sedna_connection> sc( new sedna_connection() );
	}
	catch( ... )
	{
		//
	}

	trace_into_last( "chromatic_imap...fini for the xml db module");

	return 0;
}
