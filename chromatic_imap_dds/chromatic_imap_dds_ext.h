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


namespace chromatic_imap_dds
{
		//forward


		//types
		typedef std::map<DDS::ReturnCode_t , std::string> status_map;

		//enumerations


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

			public:


				//immutable
				static const status_map sm;

		};


}

#endif /* CHROMATIC_IMAP_DDS_EXT_H_ */
