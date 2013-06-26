/*
 * chromatic_strings_unit.h
 *
 *  Created on: Jun 21, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_STRINGS_UNIT_H_
#define CHROMATIC_STRINGS_UNIT_H_

#include <stdexcept>
#include <cppunit/extensions/HelperMacros.h>
#include "chromatic_strings.h"

namespace chromatic_test_units
{
		class string_table_test : public CPPUNIT_NS::TestFixture
		{

				CPPUNIT_TEST_SUITE( string_table_test );

				CPPUNIT_TEST( locale_embue_test );
				CPPUNIT_TEST( string_lex_test );
				CPPUNIT_TEST( enum_to_string_test );

				CPPUNIT_TEST_SUITE_END();

				public:

					//ctor
					string_table_test();

					//dtor
					virtual ~string_table_test();


				protected:

					//attributes


					//services
					virtual void locale_embue_test();
					virtual void string_lex_test();
					virtual void enum_to_string_test();

				private:

					//

				public:

					//overrides
					void setUp() ;
					void tearDown();
		};
}


#endif /* CHROMATIC_STRINGS_UNIT_H_ */
