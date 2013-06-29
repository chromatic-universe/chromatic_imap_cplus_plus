//chromatic_universe_utils.h
//  Created on: Aug 1, 2010
//  Author: William K. Johnson


#ifndef CHROMATIC_UNIVERSE_UTILS_H_
#define CHROMATIC_UNIVERSE_UTILS_H_

//c++ standard
#include <iostream>
#include <sstream>
#include <functional>
#include <string>
#include <vector>
#include <stdexcept>
#include <set>
#include <memory>
#include <algorithm>
#include <fstream>
#include <map>


//c runtime
#include <cstdlib>
#include <cstdio>
#include <cstddef>


//adaptive communication environment
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/OS_NS_pwd.h"
#include "ace/os_include/os_pwd.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_pwd.h"
#include "ace/Process.h"
#include "ace/Log_Msg.h"

//boost
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

namespace chromatic_imap_protocol_impl
{
		namespace chromatic_universe_utils
		{

					  //scope
					  using boost::lexical_cast;
					  using boost::bad_lexical_cast;

					  //forward
					  class trace_into_first;
					  class trace_into_last;

					  //types
					  typedef std::vector<std::string> v_str;
					  typedef v_str::iterator v_str_iter;
					  typedef std::vector<int> v_int;
					  typedef std::vector<unsigned long> v_dword;


					  //enumerations


					  //predicates


					  //exception classes


					  //service classes


					  //helper classes
					  class trace_into_first
					  {

						public:

						  //ctor
						  trace_into_first( const std::string& boiler_plate )
						  {
							ACE_TRACE( ACE_TEXT ( "trace_into_first" ) );
							ACE_DEBUG ((LM_DEBUG,
										"\033[1;31m(%P|%t) connected with %s\033[0m\n",
										 boiler_plate.c_str() ));
						  }

						  //stream
						  friend ostream& operator<< ( ostream& o , const trace_into_first& s ) throw();

					  };

					  inline ostream& operator<< ( ostream& o , const trace_into_first& s ) throw()
					  {
						return o << " trace_into_first";
					  }

					  class trace_into_last
						{
						  public:

							//ctor
							trace_into_last( const std::string& boiler_plate )
							{
							  ACE_TRACE( ACE_TEXT ( "trace_into_last" ) );
							  ACE_DEBUG ((LM_DEBUG,
										 "\033[1;31m(%P|%t) %s\033[0m\n",
										 boiler_plate.c_str() ));
							}

							//stream
							friend ostream& operator<< ( ostream& o , const trace_into_last& s ) throw();

						};

					  inline ostream& operator<< ( ostream& o , const trace_into_last& s ) throw()
					   {
						 return o << " trace_into_last";
					   }

					  //generic


					  //immutable

		}
}



#endif //CHROMATIC_UNIVERSE_UTILS_H_
