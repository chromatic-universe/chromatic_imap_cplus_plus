/*
 * chromatic_imap_dds_ext.h
 *
 *  Created on: Jul 21, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_DDS_EXT_H_
#define CHROMATIC_IMAP_DDS_EXT_H_


//c++ standard
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>

//open splice
#include "chromatic_imap_dds.h"
#include "ccpp_dds_dcps.h"

//chromatic
#include "chromatic_strings.h"

namespace cipi = chromatic_imap_protocol_impl;

namespace chromatic_imap_dds
{
		//helpers
		class check_status
		{
			public:

				//ctor
				explicit check_status()
				{}

				//dtor
				~check_status()
				{}

			protected:

				//attributes
				std::string m_last_ret_error_str;


			public:

				//helpers
				inline std::string error_string( chromatic_imap_protocol_impl::dds_retcode error_code )
				{
					std::string str { "" };
					auto pos = cipi::chromatic_fetch_strings::sm.find( error_code );
					pos != cipi::chromatic_fetch_strings::sm.end() ? str = pos->second : str = "none";

					return str;
				}

				//acceptors-inspectors
				inline std::string last_error_string() noexcept { return m_last_ret_error_str; }

		};


}

#endif//CHROMATIC_IMAP_DDS_EXT_H_
