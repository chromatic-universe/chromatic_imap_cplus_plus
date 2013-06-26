//============================================================================
// Name        : the_linux_programming_interface.cpp
// Author      : Wlliam K. Johnson
// Version     :
// Copyright   : copyright Willliam K. Johnson 2013
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "tlpi.h"

using namespace tlpi;
//using namespace tlpi::tlpi_authentication;

using std::cout;
using std::cerr;
using std::endl;
using std::unique_ptr;
using std::tuple;
using std::system_error;

int main( int argc , char* argv[] )
{
	try
	{
		unique_ptr<tlpi_authentication_assist> tas( new tlpi_authentication_assist() );
		tas->populate_info_dictionary();
	}
	catch( user_passwd_info_error& error )
	{
		cerr << error.what() << "\n";
	}
	catch( system_error& error )
	{
		cerr << "error: " << error.code() << " - " << error.what() << '\n';
	}

	return 0;
}
