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


		//enumerations


		//class bodystructure_parser
		class bodystructure_parser : public envelope_parser
		{
			public:

				//ctor
			    bodystructure_parser( ostr_ptr ostr , const std::string& file_name ) : envelope_parser( ostr , file_name , false )
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


			public:

				//


		};




}

#endif//CHROMATIC_IMAP_BODYSTRUCTURE_H_
