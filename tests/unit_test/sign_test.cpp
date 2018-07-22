#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"


#include <boost/test/unit_test.hpp>



BOOST_AUTO_TEST_SUITE( test_suite_sign )


// how long time elapsed to generate 4 bytes decorated address
BOOST_AUTO_TEST_CASE( test_address )
{
	bryllite::timer ts;
	size_t try_count = 0;

	while( ++try_count > 0 )
	{
		CKeyPair keyPair;
		keyPair.newKeyPair();

		std::string address = keyPair.address().str();
		std::string temp = address.substr( 0, 5 );
		//		std::transform( temp.begin(), temp.end(), temp.begin(), std::tolower );

		if( strncmp( temp.c_str(), "1jade", 5 ) == 0 ) {
			bryllite::clog << Color::GREEN << "[100%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			break;
		}

		if( strncmp( temp.c_str(), "1jad", 4 ) == 0 ) {
			bryllite::clog << Color::YELLOW << "[75%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}

		if( strncmp( temp.c_str(), "1ja", 3 ) == 0 ) {
			bryllite::clog << Color::CYAN << "[50%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}

		if( strncmp( temp.c_str(), "1j", 2 ) == 0 ) {
			bryllite::clog << Color::gray << "[25%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}
	}
}


BOOST_AUTO_TEST_SUITE_END()
