/*
 * chromatic_strings_unit.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: wiljoh
 */


#include "chromatic_strings_unit.h"

using namespace chromatic_test_units;
using namespace chromatic_imap_protocol_impl;

CPPUNIT_TEST_SUITE_REGISTRATION( string_table_test );

using std::ostringstream;
using std::string;
using std::unique_ptr;
using std::vector;

//------------------------------------------------------------------------------------------------------------------------------------
string_table_test::string_table_test()
{
	//
}

//------------------------------------------------------------------------------------------------------------------------------------
string_table_test::~string_table_test()
{
	//
}

//------------------------------------------------------------------------------------------------------------------------------------
void string_table_test::locale_embue_test()
{
	ostringstream ostr;
	bool b_ret( false );

	try
	{
		std::locale lc( "xxxxxxxxx" );   //bogus locale
		std::cout.imbue ( lc );  // imbue bogus , should throw
		std::cout << 3.14159 << '\n';
	}
	//if does not throw , setting locale has no effect - error
	catch( std::runtime_error& e )	{ b_ret = true; }

	CPPUNIT_ASSERT( b_ret );

}

//------------------------------------------------------------------------------------------------------------------------------------
void string_table_test::string_lex_test()
{
	cfs_ptr cfsp( new ( chromatic_fetch_strings ) );

	const string str_lcase( "ccc");
	const string str_ucase( "CCC" );
	string l_case{ str_lcase };
	string u_case{ str_ucase };

	bool b_ret( true );

	cfsp->ucase( l_case );
	cfsp->lcase( u_case );

	if( ( l_case.compare( str_ucase ) != 0 ) || ( l_case.compare( str_ucase ) != 0 ) ) { b_ret = false; }

	CPPUNIT_ASSERT( b_ret );


}

//------------------------------------------------------------------------------------------------------------------------------------
void string_table_test::enum_to_string_test(){


	bool b_ret( true );

	//do not sort
	vector<string> v_str { "flags" ,
						   "internaldate" ,
						   "rfc822.size" ,
						   "all" ,
						   "envelope" ,
						   "full"
						   "fast" ,
						   "body" ,
						   "bodystructure" ,
						   "uid" ,
						   "rfc822" ,
						   "bodypeek" ,
						   "header" ,
						   "header.fields" ,
						   "unknown"
						  };

	for( unsigned long dw =  (unsigned long) cifc::chromatic_imap_fetch_flags;
							dw < (unsigned long) cifc::chromatic_imap_fetch_unknown + 1;
								++dw )
	{
			auto pos = chromatic_fetch_strings::imap_fetch_dictionary.find( (cifc) dw );
			if( pos != chromatic_fetch_strings::imap_fetch_dictionary.end() )
			{
				//wrong string
				if( pos->second.compare( v_str[dw] ) != 0 )	{ b_ret = false; break; }
			}
			//can't find enum in dictionary
			else { 	b_ret =false; }

			CPPUNIT_ASSERT( b_ret );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
void string_table_test::setUp()
{
	//
}

//------------------------------------------------------------------------------------------------------------------------------------
void string_table_test::tearDown()
{
	//
}

